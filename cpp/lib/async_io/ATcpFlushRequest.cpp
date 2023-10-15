#include "async_io/aSocketRequest.h"
#include "async_io/QueueElementIF.h"
#include "async_io/SinkIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpFlushRequest : public aSocketRequest, public QueueElementIF {
public:
    ATcpConnection* conn;
    SinkIF* compQ;

    ATcpFlushRequest(ATcpConnection* conn, SinkIF* compQ) : conn(conn), compQ(compQ) {}
};