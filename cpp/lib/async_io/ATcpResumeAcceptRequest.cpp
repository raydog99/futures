#include "async_io/QueueElementIF.h"
#include "async_io/aSocketRequest.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpResumeAcceptRequest : public aSocketRequest, public QueueElementIF {
public:
    ATcpServerSocket* servsock;

    ATcpResumeAcceptRequest(ATcpServerSocket* servsock) : servsock(servsock) {}
};