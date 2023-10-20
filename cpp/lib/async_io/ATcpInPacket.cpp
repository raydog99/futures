#include "async_io/ATcpServerSocket.h"
#include "async_io/QueueElementIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpListenSuccessEvent : public QueueElementIF {
public:
    ATcpServerSocket* theSocket;

    ATcpListenSuccessEvent(ATcpServerSocket* sock) : theSocket(sock) {}

    ATcpServerSocket* getSocket() {
        return theSocket;
    }

    std::string toString() {
        return "ATcpListenSuccessEvent [" + theSocket + "]";
    }
};