#include "async_io/Tracer.h"
#include "async_io/ManagerIF.h"
#include "async_io/SystemManagerIF.h"
#include "async_io/ThreadManagerIF.h"
#include "async_io/ConfigData.h"
#include "async_io/Sandstorm.h"
#include "async_io/SandstormConfig.h"
#include "async_io/aSocketImplFactory.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class ListenEventHandler : public aSocketEventHandler, public EventHandlerIF {
private:
    static const bool DEBUG = false;

public:
    ListenEventHandler() {
    }

    void init(ConfigDataIF config) {
    }

    void destroy() {
    }

private:
    void processAccept(ListenSockState* lss) {
        if (DEBUG) std::cerr << "ListenEventHandler: processAccept called" << std::endl;

        Socket* sock;
        int numAccepted = 0;

        // Try to do as many accepts as we can in one go
        while (numAccepted++ < aSocketConst::MAX_ACCEPTS_AT_ONCE) {
            sock = lss->accept();

            if (sock == nullptr) break;

            if (DEBUG) std::cerr << "ListenThread: did accept on " << sock->getInetAddress().getHostAddress() << ":" << sock->getPort() << std::endl;
            ATcpConnection* conn = new ATcpConnection(lss->servsock, sock->getInetAddress(), sock->getPort());
            if (DEBUG) std::cerr << "ListenThread: Created new conn " << conn << std::endl;
            SockState* ss = aSocketMgr.getFactory().newSockState(conn, sock, lss->writeClogThreshold);
            if (DEBUG) std::cerr << "ListenThread: Created new sockstate " << ss << std::endl;
            conn->sockState = ss;
            if (DEBUG) std::cerr << "ListenThread: Calling lss complete" << std::endl;
            lss->complete(conn);
        }

        if (DEBUG) std::cerr << "ListenEventHandler: processAccept finished" << std::endl;
    }

    void processListenRequest(aSocketRequest* req) {
        if (req instanceof ATcpListenRequest) {
            // This registers itself
            ListenSockState* lss;
            lss = aSocketMgr.getFactory().newListenSockState((ATcpListenRequest*)req, selsource);
        } else if (req instanceof ATcpSuspendAcceptRequest) {
            ATcpSuspendAcceptRequest* susreq = (ATcpSuspendAcceptRequest*)req;
            ListenSockState* lss = susreq->servsock->lss;
            if (lss == nullptr) {
                throw std::runtime_error("ListenEventHandler: Got ATcpSuspendAcceptRequest for server socket " + susreq->servsock + " with null lss!");
            }
            lss->suspend();
        } else if (req instanceof ATcpResumeAcceptRequest) {
            ATcpResumeAcceptRequest* resreq = (ATcpResumeAcceptRequest*)req;
            ListenSockState* lss = resreq->servsock->lss;
            if (lss == nullptr) {
                throw std::runtime_error("ListenEventHandler: Got ATcpResumeAcceptRequest for server socket " + resreq->servsock + " with null lss!");
            }
            lss->resume();
        } else if (req instanceof ATcpCloseServerRequest) {
            ATcpCloseServerRequest* creq = (ATcpCloseServerRequest*)req;
            ListenSockState* lss = creq->servsock->lss;
            // OK for lss to be null if closed down already
            if (lss != nullptr) lss->close();
        } else {
            throw std::invalid_argument("Bad request type to enqueueListen");
        }
    }

public:
    void handleEvent(QueueElementIF* qel) {
        if (DEBUG) std::cerr << "ListenEventHandler: Got QEL: " << qel << std::endl;

        try {
            if (dynamic_cast<SelectQueueElement*>(qel) != nullptr) {
                ListenSockState* lss = (ListenSockState*)((SelectQueueElement*)qel)->getAttachment();
                ((SelectQueueElement*)qel)->clearEvents();
                processAccept(lss);
            } else if (dynamic_cast<aSocketRequest*>(qel) != nullptr) {
                processListenRequest((aSocketRequest*)qel);
            } else {
                throw std::invalid_argument("ReadEventHandler: Got an unknown event type");
            }
        } catch (const std::exception& e) {
            std::cerr << "ListenEventHandler: Got exception: " << e.what() << std::endl;
            e.printStackTrace();
        }
    }

    void handleEvents(QueueElementIF* qelarr[], int length) {
        for (int i = 0; i < length; i++) {
            handleEvent(qelarr[i]);
        }
    }
};
