#include "async_io/ATcpConnection.h"
#include "async_io/aSocketMgr.h"
#include "async_io/ATcpInPacket.h"
#include "async_io/BufferElement.h"
#include "async_io/SinkClosedException.h"
#include "async_io/BadQueueElementException.h"
#include "async_io/ATcpStartReadRequest.h"
#include "async_io/ATcpWriteRequest.h"
#include "async_io/ATcpFlushRequest.h"
#include "async_io/ATcpCloseRequest.h"
#include "async_io/SelectSourceIF.h"
#include <InetAddress>
#include <Socket>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

ATcpConnection::ATcpConnection(InetAddress address, int port)
    : address(address), port(port), closed(false), readerstarted(false), sockState(nullptr), userTag(nullptr) {
}

ATcpConnection::ATcpConnection(ATcpClientSocket cliSock, InetAddress address, int port)
    : ATcpConnection(address, port), clientSocket(cliSock) {
}

ATcpConnection::ATcpConnection(ATcpServerSocket servSock, InetAddress address, int port)
    : ATcpConnection(address, port), serverSocket(servSock) {
}

InetAddress ATcpConnection::getAddress() {
    return address;
}

int ATcpConnection::getPort() {
    return port;
}

bool ATcpConnection::isClosed() {
    return closed || (closed = sockState->isClosed());
}

ATcpServerSocket ATcpConnection::getServerSocket() {
    return serverSocket;
}

ATcpClientSocket ATcpConnection::getClientSocket() {
    return clientSocket;
}

Socket ATcpConnection::getSocket() {
    return sockState->nbsock;
}

void ATcpConnection::startReader(SinkIF receiveQ) {
    if (readerstarted) {
        throw std::invalid_argument("startReader already called on this connection");
    }
    aSocketMgr::enqueueRequest(new ATcpStartReadRequest(this, receiveQ, -1));
    readerstarted = true;
}

void ATcpConnection::startReader(SinkIF receiveQ, int readClogTries) {
    if (readerstarted) {
        throw std::invalid_argument("startReader already called on this connection");
    }
    aSocketMgr::enqueueRequest(new ATcpStartReadRequest(this, receiveQ, readClogTries));
    readerstarted = true;
}

void ATcpConnection::enqueue(QueueElementIF buf) {
    if (isClosed()) {
        throw SinkClosedException("ATcpConnection closed");
    }
    if (buf == nullptr) {
        throw BadQueueElementException("ATcpConnection.enqueue got null element", buf);
    }
    aSocketMgr::enqueueRequest(new ATcpWriteRequest(this, (BufferElement*)buf));
}

bool ATcpConnection::enqueue_lossy(QueueElementIF buf) {
    if (isClosed()) {
        return false;
    }
    if (buf == nullptr) {
        return false;
    }
    aSocketMgr::enqueueRequest(new ATcpWriteRequest(this, (BufferElement*)buf));
    return true;
}

void ATcpConnection::enqueue_many(QueueElementIF bufarr[]) {
    if (isClosed()) {
        throw SinkClosedException("ATcpConnection closed");
    }
    for (int i = 0; i < bufarr.length; i++) {
        if (bufarr[i] == nullptr) {
            throw BadQueueElementException("ATcpConnection.enqueue_many got null element", bufarr[i]);
        }
        aSocketMgr::enqueueRequest(new ATcpWriteRequest(this, (BufferElement*)bufarr[i]));
    }
}

void ATcpConnection::close(SinkIF compQ) {
    if (isClosed()) {
        throw SinkClosedException("ATcpConnection closed");
    }
    closed = true;
    aSocketMgr::enqueueRequest(new ATcpCloseRequest(this, compQ));
}

void ATcpConnection::flush(SinkIF compQ) {
    if (isClosed()) {
        throw SinkClosedException("ATcpConnection closed");
    }
    aSocketMgr::enqueueRequest(new ATcpFlushRequest(this, compQ));
}

int ATcpConnection::size() {
    if (sockState == nullptr) {
        return 0;
    }
    if (sockState->writeReqList == nullptr) {
        return 0; // If closed
    } else {
        return sockState->writeReqList->size();
    }
}

long ATcpConnection::getSequenceNumber() {
    if (sockState == nullptr) {
        return 0;
    }
    return sockState->seqNum;
}

int ATcpConnection::profileSize() {
    return size();
}

std::string ATcpConnection::toString() {
    return "ATcpConnection [" + address.getHostAddress() + ":" + std::to_string(port) + "/" + sockState + "/" + clientSocket + "]";
}
