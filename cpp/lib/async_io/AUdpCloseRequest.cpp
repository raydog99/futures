#include "async_io/AUdpCloseRequest.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

AUdpCloseRequest::AUdpCloseRequest(AUdpSocket* sock, SinkIF* compQ) {
    this->sock = sock;
    this->compQ = compQ;
}