#include "async_io/QueueElementIF.h"
#include "async_io/AUdpSocket.h"
#include <netinet/in.h>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class AUdpInPacket : public QueueElementIF {
public:
    AUdpInPacket(AUdpSocket* sock, DatagramPacket* packet) : sock(sock), packet(packet), seqNum(0) {}

    AUdpInPacket(AUdpSocket* sock, DatagramPacket* packet, long seqNum) : sock(sock), packet(packet), seqNum(seqNum) {}

    AUdpSocket* getSocket() {
        return sock;
    }

    DatagramPacket* getPacket() {
        return packet;
    }

    byte* getBytes() {
        return packet->getData();
    }

    int size() {
        return packet->getLength();
    }

    long getSequenceNumber() {
        return seqNum;
    }

    std::string toString() {
        return "AUdpInPacket [sock=" + sock->toString() + ", size=" + std::to_string(packet->getLength()) + "]";
    }

private:
    AUdpSocket* sock;
    DatagramPacket* packet;
    long seqNum;
};