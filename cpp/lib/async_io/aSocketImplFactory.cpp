#include "async_io/SelectSourceIF.h"
#include "async_io/SelectQueueElement.h"
#include "async_io/SockState.h"
#include "async_io/ConnectSockState.h"
#include "async_io/ListenSockState.h"
#include "async_io/DatagramSockState.h"

#include <iostream>
#include <memory>
#include <stdexcept>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

// aSocketImplFactory is an internal abstract class used to represent
// the interface between the aSocket library and a provider implementation.
class aSocketImplFactory {
private:
    static const bool DEBUG = false;

public:
    static std::unique_ptr<aSocketImplFactory> getFactory() {
        std::unique_ptr<aSocketImplFactory> factory;

        if (aSocketMgr::USE_ASIO) {
            factory = std::make_unique<ASIOFactory>();
        } else {
            throw std::runtime_error("Unsupported aSocketMgr configuration.");
        }
        return factory;
    }

    virtual std::unique_ptr<SelectSourceIF> newSelectSource() = 0;
    virtual std::unique_ptr<SelectQueueElement> newSelectQueueElement(void* item) = 0;
    virtual std::unique_ptr<SockState> newSockState(ATcpConnection* conn, Socket* nbsock, int writeClogThreshold) = 0;
    virtual std::unique_ptr<ConnectSockState> newConnectSockState(ATcpConnectRequest* req, SelectSourceIF* selsource) = 0;
    virtual std::unique_ptr<ListenSockState> newListenSockState(ATcpListenRequest* req, SelectSourceIF* selsource) = 0;
    virtual std::unique_ptr<DatagramSockState> newDatagramSockState(AUdpSocket* sock, InetAddress* addr, int port) = 0;
};