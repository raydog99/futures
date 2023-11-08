#include "async_io/AUdpFlushRequest.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

AUdpFlushRequest::AUdpFlushRequest(AUdpSocket* sock, SinkIF* compQ) {
    this->sock = sock;
    this->compQ = compQ;
}