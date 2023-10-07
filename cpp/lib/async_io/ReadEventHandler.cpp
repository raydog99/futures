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

class ReadEventHandler {
private:
    static const bool DEBUG = false;
    static const bool PROFILE = false;

    static Tracer tracer;

public:
    ReadEventHandler() {
        if (PROFILE) tracer = Tracer("aSocket ReadEH");
    }

    void init(ConfigDataIF config) { }

    void destroy() { }

    void handleEvent(QueueElementIF* qel) {
        if (DEBUG) std::cerr << "ReadEventHandler: Got QEL: " << qel << std::endl;
        if (PROFILE) tracer.trace("ReadEH handleEvent");

        try {
            if (dynamic_cast<SelectQueueElement*>(qel) != nullptr) {
                void* attach = dynamic_cast<SelectQueueElement*>(qel)->getAttachment();
                if (PROFILE) tracer.trace("ReadEH got attach");
                if (dynamic_cast<SockState*>(attach) != nullptr) {
                    SockState* ss = static_cast<SockState*>(attach);
                    if (DEBUG) std::cerr << "ReadEventHandler: ss is " << ss << std::endl;
                    if (PROFILE) tracer.trace("TCP ss.doRead()");
                    ss->doRead();
                    if (PROFILE) tracer.trace("TCP ss.doRead() done");
                }
                if (DEBUG) std::cerr << "ReadEventHandler: returned from doRead" << std::endl;
            } else {
                throw std::invalid_argument("ReadEventHandler: Got unknown event type " + std::to_string(reinterpret_cast<uintptr_t>(qel)));
            }
        } catch (const std::exception& e) {
            std::cerr << "ReadEventHandler: Got exception: " << e.what() << std::endl;
        }
        if (PROFILE) tracer.trace("ReadEH handleEvent done");
    }

    void handleEvents(QueueElementIF* qelarr[], int length) {
        for (int i = 0; i < length; i++) {
            handleEvent(qelarr[i]);
        }
    }
};