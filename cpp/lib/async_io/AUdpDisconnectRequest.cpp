#include "async_io/AUdpDisconnectRequest.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

AUdpDisconnectRequest::AUdpDisconnectRequest(AUdpSocket* sock) {
    this->sock = sock;
}
