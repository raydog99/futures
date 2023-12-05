#include "async_io/NonblockingInputStream.h"
#include <async_io/NonblockingOutputStream.h>
#include <async_io/SelectItem.h>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class SockState : public SockState {
private:
  static const bool DEBUG = false;

  NonblockingInputStream nbis;
  NonblockingOutputStream nbos;
  SelectItem readsi, writesi;

  SelectSource read_selsource;
  SelectSource write_selsource;

public:
  SockState(ATcpConnection* conn, Socket* nbsock, int writeClogThreshold) : SockState(conn, nbsock, Integer(writeClogThreshold)) {}

  SockState(ATcpConnection* conn, Socket* nbsock, Integer writeClogThreshold) : 
    conn(conn),
    nbsock(nbsock),
    writeClogThreshold(writeClogThreshold.intValue()),
    write_selsource(nullptr) {

    if (DEBUG) std::cerr << "SockState: Constructor called with " << conn << ", " << nbsock << ", " << writeClogThreshold << std::endl;
    
    if (DEBUG) std::cerr << "Sockstate " << nbsock << ": Const creating nbis" << std::endl;
    nbis = dynamic_cast<NonblockingInputStream*>(nbsock->getInputStream());

    if (DEBUG) std::cerr << "Sockstate " << nbsock << ": Const creating nbos" << std::endl;
    nbos = dynamic_cast<NonblockingOutputStream*>(nbsock->getOutputStream());

    if (DEBUG) std::cerr << "SockState " << nbsock << ": Const creating readBuf of size " << aSocketConst.READ_BUFFER_SIZE << std::endl;
    readBuf = new byte[aSocketConst.READ_BUFFER_SIZE];

    if (DEBUG) std::cerr << "SockState " << nbsock << ": Setting flags" << std::endl;
    outstanding_writes = 0;
    numEmptyWrites = 0;
    writeReqList = new ssLinkedList();

    clogged_qel = nullptr;
    clogged_numtries = 0;
    if (DEBUG) std::cerr << "SockState " << nbsock << ": Const done" << std::endl;
  }

  void readInit(SelectSource* read_selsource, SinkIF* compQ, int readClogTries) {
    if (DEBUG) std::cerr << "readInit called on " << this << std::endl;
    if (closed) return; // May have been closed already
    this->read_selsource = dynamic_cast<SelectSource*>(read_selsource);
    this->readCompQ = compQ;
    this->readClogTries = readClogTries;
    readsi = SelectItem(dynamic_cast<NonblockingSocket*>(nbsock), this, Selectable::READ_READY);
    this->read_selsource->register(&readsi); 
  }

  void synchronizedReadInit(SelectSource* read_selsource, SinkIF* compQ, int readClogTries) {
    std::lock_guard<std::mutex> lock(mutex_);
    readInit(read_selsource, compQ, readClogTries);
  }

  void doRead() {
    std::lock_guard<std::mutex> lock(mutex_);
    doReadInternal();
  }

  bool addWriteRequest(aSocketRequest* req, SelectSource* write_selsource) {
    std::lock_guard<std::mutex> lock(mutex_);
    return addWriteRequestInternal(req, write_selsource);
  }

  void initWrite(ATcpWriteRequest* req) {
    std::lock_guard<std::mutex> lock(mutex_);
    initWriteInternal(req);
  }

  bool tryWrite() {
    std::lock_guard<std::mutex> lock(mutex_);
    return tryWriteInternal();
  }

  void writeMaskEnable() {
    std::lock_guard<std::mutex> lock(mutex_);
    writeMaskEnableInternal();
  }

  void writeMaskDisable() {
    std::lock_guard<std::mutex> lock(mutex_);
    writeMaskDisableInternal();
  }

  void close(SinkIF* closeEventQueue) {
    std::lock_guard<std::mutex> lock(mutex_);
    closeInternal(closeEventQueue);
  }

  void doReadInternal() {
  if (DEBUG) std::cerr << "SockState: doRead called" << std::endl;
  if (closed) return;

  if (clogged_qel != nullptr) {
    if (DEBUG) std::cerr << "SockState: doRead draining clogged element " << clogged_qel << std::endl;
    try {
      readCompQ->enqueue(clogged_qel);
      clogged_qel = nullptr;
      clogged_numtries = 0;
    } catch (SinkFullException& qfe) {
      if ((readClogTries != -1) && (++clogged_numtries >= readClogTries)) {
        if (DEBUG) std::cerr << "SockState: Warning: readClogTries exceeded, dropping " << clogged_qel << std::endl;
        clogged_qel = nullptr;
        clogged_numtries = 0;
      } else {
        return;
      }
    } catch (SinkException& sce) {
      this->close(nullptr);
    }
  }

  int len;

  try {
    if (DEBUG) std::cerr << "SockState: doRead trying read" << std::endl;
    len = nbis->read(readBuf, 0, aSocketConst::READ_BUFFER_SIZE);
    if (DEBUG) std::cerr << "SockState: read returned " << len << std::endl;

    if (len == 0) {
      // std::cerr << "ss.doRead: Warning: Got empty read on socket" << std::endl;
      readsi.revents = 0;
      return;
    } else if (len < 0) {
      if (DEBUG) std::cerr << "ss.doRead: read failed, sock closed" << std::endl;
      this->close(readCompQ);
      readsi.revents = 0;
      return;
    }
  }
};