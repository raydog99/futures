#include <string>
#include "async_io/aSocketStageWrapper.h"
#include "async_io/aSocketEventHandler.h"
#include "async_io/SelectSourceIF.h"
#include "async_io/StageIF.h"
#include "async_io/EventHandlerIF.h"
#include "async_io/ConfigDataIF.h"
#include "async_io/ThreadManagerIF.h"
#include "async_io/FiniteQueue.h"
#include "async_io/QueueThresholdPredicate.h"
#include "async_io/Stage.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

aSocketStageWrapper::aSocketStageWrapper(ManagerIF* mgr, const string& name, EventHandlerIF* handler, ConfigDataIF* config, ThreadManagerIF* tm) {
    this->name = name;
    this->handler = handler;
    this->config = config;
    this->tm = tm;

    int queuelen;
    if ((queuelen = config->getInt("_queuelength")) <= 0) {
        queuelen = -1;
    }
    if (queuelen == -1) {
        eventQ = new FiniteQueue(tm);
    } else {
        eventQ = new FiniteQueue(tm);
        QueueThresholdPredicate* pred = new QueueThresholdPredicate(eventQ, queuelen);
        eventQ->setEnqueuePredicate(pred);
    }

    if (mgr->getConfig()->getBoolean("global.batchController.enable")) {
        this->sorter = new AggThrottleBatchSorter();
    } else {
        this->sorter = new NullBatchSorter();
    }

    this->selsource = static_cast<aSocketEventHandler*>(handler)->getSelectSource();
    this->stage = new Stage(name, this, dynamic_cast<SinkIF*>(eventQ), config);
    this->config->setStage(this->stage);
}

void aSocketStageWrapper::init() {
    handler->init(config);
    tm->registerStage(this);
}

void aSocketStageWrapper::destroy() {
    tm->deregisterStage(this);
    handler->destroy();
}

EventHandlerIF* aSocketStageWrapper::getEventHandler() {
    return handler;
}

StageIF* aSocketStageWrapper::getStage() {
    return stage;
}

SourceIF* aSocketStageWrapper::getSource() {
    return eventQ;
}

ThreadManagerIF* aSocketStageWrapper::getThreadManager() {
    return tm;
}

SelectSourceIF* aSocketStageWrapper::getSelectSource() {
    return selsource;
}

SourceIF* aSocketStageWrapper::getEventQueue() {
    return eventQ;
}

void aSocketStageWrapper::setBatchSorter(BatchSorterIF* sorter) {
}

BatchSorterIF* aSocketStageWrapper::getBatchSorter() {
    return sorter;
}

ResponseTimeControllerIF* aSocketStageWrapper::getResponseTimeController() {
    return nullptr;
}

string aSocketStageWrapper::toString() {
    return "ASOCKETSW[" + stage->getName() + "]";
}