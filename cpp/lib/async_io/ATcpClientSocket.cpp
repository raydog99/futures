#include "async_io/aSocketMgr.h"
#include "async_io/ATcpConnection.h"
#include "async_io/SinkIF.h"
#include "async_io/aSocketMgr.h"
#include "async_io/ATcpConnectRequest.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ATcpClientSocket {
private:
    InetAddress address;
    int port;

public:
    ATcpClientSocket(InetAddress addr, int port, SinkIF* compQ) {
        this->address = addr;
        this->port = port;
        aSocketMgr::enqueueRequest(ATcpConnectRequest(this, addr, port, compQ, -1, -1));
    }

    ATcpClientSocket(std::string host, int port, SinkIF* compQ) {
        try {
            InetAddress addr = InetAddress::getByName(host);
            this->address = addr;
            this->port = port;
            aSocketMgr::enqueueRequest(ATcpConnectRequest(this, addr, port, compQ, -1, -1));
        } catch (std::exception& e) {
            // Handle the exception (e.g., UnknownHostException) as needed.
            Tracer::trace(Tracer::ERROR, "Error: " + std::string(e.what()));
        }
    }

    InetAddress getAddress() {
        return address;
    }

    int getPort() {
        return port;
    }
};