#include "async_io/BufferElement.h"
#include "async_io/ATcpConnection.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpWriteRequest : public aSocketRequest, public QueueElementIF {
public:
    ATcpConnection* conn;
    BufferElement* buf;

    ATcpWriteRequest(ATcpConnection* conn, BufferElement* buf) : conn(conn), buf(buf) {
    }
};