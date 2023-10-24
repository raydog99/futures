#include "async_io/aSocketErrorEvent.h"
#include "async_io/ATcpClientSocket.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpListenFailedEvent : public aSocketErrorEvent {
public:
    ATcpServerSocket* theSocket;

    ATcpListenFailedEvent(ATcpServerSocket* sock, const std::string& message) : aSocketErrorEvent(message), theSocket(sock) {}

    ATcpServerSocket* getSocket() {
        return theSocket;
    }

    std::string toString() {
        return "ATcpListenFailedEvent: " + getMessage();
    }
};
