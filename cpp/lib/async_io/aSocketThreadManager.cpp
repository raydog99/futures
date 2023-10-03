#include "async_io/aSocketStageWrapper.h"
#include "async_io/StageWrapperIF.h"
#include "async_io/ThreadPool.h"
#include "async_io/StageIF.h"
#include "async_io/EventHandlerIF.h"
#include "async_io/ConfigDataIF.h"
#include "async_io/ThreadManagerIF.h"
#include "async_io/FiniteQueue.h"
#include "async_io/QueueThresholdPredicate.h"
#include "async_io/Stage.h"
#include "async_io/aSocketThread.h"

#include <iostream>
#include <vector>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class aSocketThreadManager {
public:
    aSocketThreadManager(ManagerIF* mgr) : mgr(mgr) {}

    void register(StageWrapperIF* thestage) {
        aSocketStageWrapper* stage = static_cast<aSocketStageWrapper*>(thestage);
        aSocketThread* at = makeThread(stage);
        ThreadPool* tp = new ThreadPool(stage, mgr, at, 1);
        at->registerTP(tp);
        tp->start();
    }

    void deregister(StageWrapperIF* stage) {
        throw std::invalid_argument("aSocketThreadManager: deregister not supported");
    }

    void deregisterAll() {
        throw std::invalid_argument("aSocketThreadManager: deregisterAll not supported");
    }

     /**
       * Wake any thread waiting for work.  This is called by
       * an enqueue* method of FiniteQueue.
       **/
    void wake() { /* ignore */ }

private:
    ManagerIF* mgr;

    aSocketThread* makeThread(aSocketStageWrapper* wrapper) {
        return new aSocketThread(wrapper);
    }
};