#include "async_io/aSocketRequest.h"
#include "async_io/QueueElementIF.h"
#include "async_io/SinkIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpListenRequest : public aSocketRequest, public QueueElementIF {
public:
    ATcpServerSocket* servsock;
    SinkIF* compQ;
    int port;
    int writeClogThreshold;

    ATcpListenRequest(ATcpServerSocket* servsock, int port, SinkIF* compQ, int writeClogThreshold)
        : servsock(servsock), compQ(compQ), port(port), writeClogThreshold(writeClogThreshold) {}
};