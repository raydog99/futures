#include "async_io/AUdpStartReadRequest.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

AUdpStartReadRequest::AUdpStartReadRequest(AUdpSocket* sock, SinkIF* compQ, int readClogTries) {
    this->sock = sock;
    this->compQ = compQ;
    this->readClogTries = readClogTries;
}
