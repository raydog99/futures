#include <iostream>
#include <seda/nbio/NonblockingSocket.h>
#include <seda/nbio/SelectItem.h>
#include <seda/nbio/Selectable.h>

#include "async_io/QueueElementIF.h"
#include "async_io/SinkException.h"
#include "async_io/ATcpConnection.h"
#include "async_io/ConnectSockState.h"
#include "async_io/ATcpConnectRequest.h"
#include "async_io/ATcpConnectFailedEvent.h"
#include "async_io/SockState.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class SelectSourceIF;

class SelectSource : public SelectSourceIF {
public:
    void registerItem(Selectable* sel) override {
        // Implementation for registering Selectable items
    }

    void deregisterItem(Selectable* sel) override {
        // Implementation for deregistering Selectable items
    }
};

class ConnectSockStateNbio : public ConnectSockState {
private:
    NonblockingSocket nbsock;
    SelectItem si;
    SelectSource write_selsource;

public:
    ConnectSockStateNbio(ATcpConnectRequest* req, SelectSourceIF* write_selsource) : ConnectSockState(req) {
        this->write_selsource = *dynamic_cast<SelectSource*>(write_selsource);
        si = SelectItem(&nbsock, this, Selectable::CONNECT_READY);
        write_selsource.registerItem(&si);
    }

protected:
    void complete() override {
        if (completed) return;

        try {
            ATcpConnection conn(clisock, nbsock.getInetAddress(), nbsock.getPort());
            QueueElementIF tmparr[] = {&conn};

            // Split-phase enqueue
            compQ.enqueue_prepare(tmparr);

            if (!nbsock.connectDone()) {
                compQ.enqueue_abort(key);
                std::cerr << "aSocket.CSS.complete: Warning: connectDone returned false!\n";
                return;
            }

            Socket sock = nbsock;
            SockState ss(conn, sock, writeClogThreshold);
            conn.sockState = &ss;

            // Finally enqueue
            compQ.enqueue_commit(key);
            completed = true;
        } catch (const std::exception& e) {
            error(ATcpConnectFailedEvent(clisock, "Got error trying to connect: " + std::string(e.what())));
            if (key != nullptr) compQ.enqueue_abort(key);
            return;
        }

        // Deregister
        si.revents = 0;
        write_selsource.deregisterItem(&si);
    }

    void error(const aSocketErrorEvent& error) override {
        si.revents = 0;
        compQ.enqueue_lossy(error);
        write_selsource.deregisterItem(&si);
    }
};