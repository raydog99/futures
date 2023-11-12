#include "async_io/AUdpFlushRequest.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

AUdpWriteRequest::AUdpWriteRequest(AUdpSocket* sock, BufferElement* buf) {
    this->sock = sock;
    this->buf = buf;
}