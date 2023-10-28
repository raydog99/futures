#include "async_io/aSocketErrorEvent.h"
#include "async_io/ATcpServerSocket.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpServerSocketClosedEvent : public aSocketErrorEvent {
public:
    ATcpServerSocket* server_socket;

    ATcpServerSocketClosedEvent() : aSocketErrorEvent("ATcpServerSocket is dead"), server_socket(nullptr) {}

    ATcpServerSocketClosedEvent(ATcpServerSocket* sock) : aSocketErrorEvent("ATcpServerSocket is dead"), server_socket(sock) {}
};