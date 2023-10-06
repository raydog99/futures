#include "async_io/ATcpConnection.h"
#include "async_io/ATcpServerSocket.h"
#include "async_io/SinkIF.h"

#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ListenSockState {
private:
    static const bool DEBUG = false;
    static int num_connections;

protected:
    ATcpServerSocket *servsock;
    int port;
    SinkIF *compQ;
    int writeClogThreshold;

    virtual int getLocalPort() = 0;
    virtual int accept() = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void close() = 0;
    virtual void complete(ATcpConnection *conn) = 0;
};