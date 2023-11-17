#include "async_io/SimpleSink.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class AUdpSocket : public SimpleSink {
public:
    static const int DEFAULT_MAX_PACKETSIZE = 16384;

    AUdpSocket(SinkIF* compQ) : compQ(compQ), maxPacketSize(DEFAULT_MAX_PACKETSIZE), writeClogThreshold(-1), readerstarted(false), closed(false) {
        aSocketMgr.init();
        sockState = aSocketMgr.getFactory().newDatagramSockState(this, nullptr, 0);
    }

    AUdpSocket(int localport, SinkIF* compQ) : compQ(compQ), maxPacketSize(DEFAULT_MAX_PACKETSIZE), writeClogThreshold(-1), readerstarted(false), closed(false) {
        aSocketMgr.init();
        sockState = aSocketMgr.getFactory().newDatagramSockState(this, nullptr, localport);
    }

    AUdpSocket(InetAddress* localaddr, int localport, SinkIF* compQ, int maxPacketSize, int writeClogThreshold)
        : localaddress(nullptr), remaddress(nullptr), localport(localport), remport(-1), compQ(compQ), maxPacketSize(maxPacketSize), writeClogThreshold(writeClogThreshold), readerstarted(false), closed(false) {
        aSocketMgr.init();
        sockState = aSocketMgr.getFactory().newDatagramSockState(this, localaddr, localport);
    }

    void startReader(SinkIF* receiveQ) {
        startReader(receiveQ, -1);
    }

    void startReader(SinkIF* receiveQ, int readClogTries) {
        if (readerstarted) {
            throw std::invalid_argument("startReader already called on this socket");
        }
        aSocketMgr.enqueueRequest(new AUdpStartReadRequest(this, receiveQ, readClogTries));
        readerstarted = true;
    }

    void enqueue(QueueElementIF* packet) override {
        if (closed) {
            throw SinkClosedException("AUdpSocket closed");
        }
        if (!packet) {
            throw BadQueueElementException("AUdpSocket.enqueue got null element", packet);
        }
        aSocketMgr.enqueueRequest(new AUdpWriteRequest(this, dynamic_cast<BufferElement*>(packet)));
    }

    bool enqueue_lossy(QueueElementIF* packet) {
        if (closed) {
            return false;
        }
        if (!packet) {
            return false;
        }
        aSocketMgr.enqueueRequest(new AUdpWriteRequest(this, dynamic_cast<BufferElement*>(packet)));
        return true;
    }

    void enqueue_many(QueueElementIF* packets[], int size) override {
        if (closed) {
            throw SinkClosedException("AUdpSocket closed");
        }
        for (int i = 0; i < size; i++) {
            if (!packets[i]) {
                throw BadQueueElementException("AUdpSocket.enqueue_many got null element", packets[i]);
            }
            aSocketMgr.enqueueRequest(new AUdpWriteRequest(this, dynamic_cast<BufferElement*>(packets[i])));
        }
    }

    void close(SinkIF* compQ) override {
        if (closed) {
            throw SinkClosedException("AUdpSocket closed");
        }
        closed = true;
        aSocketMgr.enqueueRequest(new AUdpCloseRequest(this, compQ));
    }

    void flush(SinkIF* compQ) override {
        if (closed) {
            throw SinkClosedException("AUdpSocket closed");
        }
        aSocketMgr.enqueueRequest(new AUdpFlushRequest(this, compQ));
    }

    int size() {
        if (!sockState || !sockState->writeReqList) {
            return 0;
        }
        return sockState->writeReqList->size();
    }

    long getSequenceNumber() {
        if (!sockState) {
            return 0;
        }
        return sockState->seqNum;
    }

    int profileSize() override {
        return size();
    }

    void connect(InetAddress* addr, int port) {
        aSocketMgr.enqueueRequest(new AUdpConnectRequest(this, addr, port));
    }

    void disconnect() {
        aSocketMgr.enqueueRequest(new AUdpDisconnectRequest(this));
    }

    InetAddress* getAddress() {
        return sockState->getSocket()->getInetAddress();
    }

    int getPort() {
        return sockState->getSocket()->getPort();
    }

    InetAddress* getLocalAddress() {
        return sockState->getSocket()->getLocalAddress();
    }

    int getLocalPort() {
        return sockState->getSocket()->getLocalPort();
    }

    DatagramSocket* getSocket() {
        return sockState->getSocket();
    }

private:
    InetAddress* localaddress;
    InetAddress* remaddress;
    int localport;
    int remport;
    int maxPacketSize;
    int writeClogThreshold;
    SinkIF* compQ;
    bool readerstarted;
    bool closed;
    DatagramSockState* sockState;
};
