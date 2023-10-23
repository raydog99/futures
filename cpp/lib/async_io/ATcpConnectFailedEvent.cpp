#include "async_io/aSocketErrorEvent.h"
#include "async_io/ATcpClientSocket.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpConnectFailedEvent : public aSocketErrorEvent {
public:
    ATcpClientSocket* theSocket;

    ATcpConnectFailedEvent(ATcpClientSocket* sock, const std::string& message) : aSocketErrorEvent(message), theSocket(sock) {}

    ATcpClientSocket* getSocket() {
        return theSocket;
    }

    std::string toString() {
        return "ATcpConnectFailedEvent: " + getMessage();
    }
};