#include "async_io/aSocketStageWrapper.h"
#include "async_io/StageWrapperIF.h"
#include "async_io/ThreadPool.h"
#include "async_io/StageIF.h"
#include "async_io/EventHandlerIF.h"
#include "async_io/ConfigDataIF.h"
#include "async_io/ThreadManagerIF.h"
#include "async_io/FiniteQueue.h"
#include "async_io/QueueThresholdPredicate.h"
#include "async_io/Stage.h"
#include "async_io/aSocketThread.h"

#include <iostream>
#include <vector>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class SockState {
protected:
    static const bool DEBUG = false;

    Socket* nbsock;
    ATcpConnection* conn;
    SinkIF* readCompQ;
    QueueElementIF* clogged_qel;
    int clogged_numtries;
    int readClogTries, writeClogThreshold;
    char* readBuf;
    bool closed = false;
    long seqNum = 1;

    int outstanding_writes, numEmptyWrites;
    ssLinkedList* writeReqList;
    ATcpWriteRequest* cur_write_req;
    int cur_offset, cur_length_target;
    char* writeBuf;
    ATcpInPacket* pkt;

    static int numActiveWriteSockets;

public:
    SockState() {
        nbsock = nullptr;
        conn = nullptr;
        readCompQ = nullptr;
        clogged_qel = nullptr;
        clogged_numtries = 0;
        readClogTries = 0;
        writeClogThreshold = 0;
        readBuf = nullptr;
        closed = false;
        seqNum = 1;
        outstanding_writes = 0;
        numEmptyWrites = 0;
        writeReqList = nullptr;
        cur_write_req = nullptr;
        cur_offset = 0;
        cur_length_target = 0;
        writeBuf = nullptr;
        pkt = nullptr;
    }

    ~SockState() { }

    virtual void readInit(SelectSourceIF* read_selsource, SinkIF* compQ, int readClogTries) = 0;
    virtual void doRead() = 0;
    virtual bool addWriteRequest(aSocketRequest* req, SelectSourceIF* write_selsource) = 0;
    virtual void initWrite(ATcpWriteRequest* req) = 0;
    virtual bool tryWrite() throw(SinkClosedException) = 0;
    virtual void writeReset() = 0;
    virtual void writeMaskEnable() = 0;
    virtual void writeMaskDisable() = 0;

    static int numActiveWriters() {
        return numActiveWriteSockets;
    }

    bool isClosed() {
        return closed;
    }

    virtual void close(SinkIF* closeEventQueue) = 0;
};