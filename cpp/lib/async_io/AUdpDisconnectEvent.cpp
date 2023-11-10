#include "async_io/AUdpDisconnectEvent.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

AUdpDisconnectEvent::AUdpDisconnectEvent(AUdpSocket* sock) {
    this->theSocket = sock;
}

AUdpSocket* AUdpDisconnectEvent::getSocket() {
    return theSocket;
}

std::string AUdpDisconnectEvent::toString() {
    return "AUdpDisconnectEvent [" + theSocket + "]";
}
