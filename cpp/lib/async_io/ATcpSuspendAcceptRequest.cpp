#include "async_io/aSocketRequest.h"
#include "async_io/QueueElementIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpSuspendAcceptRequest : public aSocketRequest, public QueueElementIF {
public:
    ATcpServerSocket* servsock;

    ATcpSuspendAcceptRequest(ATcpServerSocket* servsock) : servsock(servsock) {}
};