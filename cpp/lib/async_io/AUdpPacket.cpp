#include "async_io/BufferElement.h"
#include "async_io/SinkIF.h"
#include <netinet/in.h>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class AUdpPacket : public BufferElement {
public:
    AUdpPacket(byte* data, int offset, int size) : BufferElement(data, offset, size, nullptr) {}
    AUdpPacket(byte* data, int offset, int size, SinkIF* compQ) : BufferElement(data, offset, size, compQ) {}
    AUdpPacket(byte* data, int offset, int size, SinkIF* compQ, InetAddress address, int port)
        : BufferElement(data, offset, size, compQ), address(address), port(port) {}

    AUdpPacket(int size) : BufferElement(new byte[size], 0, size, nullptr) {}

    AUdpPacket(DatagramPacket dp) : BufferElement(dp.getData(), dp.getOffset(), dp.getLength(), nullptr),
                                     address(dp.getAddress()), port(dp.getPort()) {}

    AUdpPacket(DatagramPacket dp, SinkIF* compQ) : BufferElement(dp.getData(), dp.getOffset(), dp.getLength(), compQ),
                                                   address(dp.getAddress()), port(dp.getPort()) {}

    InetAddress getAddress() {
        return address;
    }

    int getPort() {
        return port;
    }

private:
    InetAddress address;
    int port;
};