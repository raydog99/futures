#include "async_io/aSocketRequest.h"
#include "async_io/QueueElementIF.h"
#include "async_io/SinkIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpConnectRequest {
public:
    ATcpConnectRequest(ATcpClientSocket* clisock, InetAddress* addr, int port, SinkIF* compQ, int writeClogThreshold, int connectClogTries)
        : clisock(clisock), compQ(compQ), addr(addr), port(port), writeClogThreshold(writeClogThreshold), connectClogTries(connectClogTries) {
    }

    ATcpClientSocket* clisock;
    SinkIF* compQ;
    InetAddress* addr;
    int port;
    int writeClogThreshold;
    int connectClogTries;
};