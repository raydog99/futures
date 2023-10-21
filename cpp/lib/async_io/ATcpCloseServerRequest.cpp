#include "async_io/aSocketRequest.h"
#include "async_io/QueueElementIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;


class ATcpCloseServerRequest : public aSocketRequest, public QueueElementIF {
public:
    ATcpServerSocket* servsock;

    ATcpCloseServerRequest(ATcpServerSocket* servsock) : servsock(servsock) {}
};