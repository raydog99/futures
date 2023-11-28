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

};