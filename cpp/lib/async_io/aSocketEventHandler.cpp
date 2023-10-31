#include "async_io/aSocketEventHandler.h"
#include "async_io/aSocketMgr.h"
#include "async_io/QueueElementIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

aSocketEventHandler::aSocketEventHandler() {
    this->selsource = aSocketMgr::getFactory()->newSelectSource();
}

SelectSourceIF* aSocketEventHandler::getSelectSource() {
    return selsource;
}

void aSocketEventHandler::interruptSelect() {
    if (DEBUG) {
        cerr << "aSocketEventHandler::interruptSelect() called." << endl;
    }
    selsource->interruptSelect();
}