#include "async_io/AUdpConnectEvent.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

AUdpConnectEvent::AUdpConnectEvent(AUdpSocket* sock) {
    theSocket = sock;
}

AUdpSocket* AUdpConnectEvent::getSocket() {
    return theSocket;
}

std::string AUdpConnectEvent::toString() {
    return "AUdpConnectEvent [" + theSocket->toString() + "]";
}