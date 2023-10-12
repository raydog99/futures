#include "async_io/ATcpServerSocket.h"
#include "async_io/ATcpConnection.h"
#include "async_io/aSocketMgr.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;


class ATcpClientSocket {
private:
    InetAddress address;
    int port;


public:
    ATcpServerSocket(int serverPort, SinkIF* compQ) {
        this->serverPort = serverPort;
        aSocketMgr->enqueueRequest(new ATcpListenRequest(this, serverPort, compQ, -1));
    }

    ATcpServerSocket(int serverPort, SinkIF* compQ, int writeClogThreshold) {
        this->serverPort = serverPort;
        aSocketMgr->enqueueRequest(new ATcpListenRequest(this, serverPort, compQ, writeClogThreshold));
    }

    ATcpServerSocket(int serverPort, SinkIF* compQ, int writeClogThreshold) {
        this->serverPort = serverPort;
        aSocketMgr->enqueueRequest(new ATcpListenRequest(this, serverPort, compQ, writeClogThreshold));
    }

    void suspendAccept() {
        aSocketMgr->enqueueRequest(new ATcpSuspendAcceptRequest(this));
    }

    void resumeAccept() {
        aSocketMgr->enqueueRequest(new ATcpResumeAcceptRequest(this));
    }

    int getPort() {
        return serverPort;
    }

    int getLocalPort() {
        if (lss != nullptr) {
            return lss->getLocalPort();
        }
        return -1;
    }

    void close() {
        aSocketMgr->enqueueRequest(new ATcpCloseServerRequest(this));
    }
};