#include <async_io.h>
#include "async_io/aSocketImplFactory.h"
#include <async_io/ConnectSockState.h>
#include <async_io/DatagramSockState.h>
#include <async_io/ListenSockState.h>
#include <async_io/SelectQueueElement.h>
#include <async_io/SelectSource.h>
#include <async_io/SockState.h>
#include <iostream>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;


class ASIOFactory : public aSocketImplFactory {
private:
  static const bool DEBUG = false;

protected:
  SelectSourceIF* newSelectSource() override {
    return new SelectSource();
  }

  SelectQueueElement* newSelectQueueElement(Object* item) override {
    return new SelectQueueElement(static_cast<SelectItem*>(item));
  }

  SockState* newSockState(ATcpConnection* conn, Socket* nbsock, int writeClogThreshold) override {
    return new SockState(conn, nbsock, writeClogThreshold);
  }

  ConnectSockState* newConnectSockState(ATcpConnectRequest* req, SelectSourceIF* selsource) override {
    return new ConnectSockState(req, selsource);
  }

  ListenSockState* newListenSockState(ATcpListenRequest* req, SelectSourceIF* selsource) override {
    return new ListenSockState(req, selsource);
  }

  DatagramSockState* newDatagramSockState(AUdpSocket* sock, InetAddress* addr, int port) override {
    return new DatagramSockState(sock, addr, port);
  }
};