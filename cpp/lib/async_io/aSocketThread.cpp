#include "async_io/EventHandlerIF.h"
#include "async_io/ManagerIF.h"
#include "async_io/QueueElementIF.h"
#include "async_io/SourceIF.h"
#include "async_io/BatchDescrIF.h"
#include "async_io/BatchSorterIF.h"
#include "async_io/StageWrapperIF.h"
#include "async_io/ThreadManagerIF.h"
#include "async_io/ThreadPool.h"

#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class aSocketThreadManager;

class aSocketThread {
protected:s
    ThreadPool* tp;
    StageWrapperIF* wrapper;
    SelectSourceIF* selsource;
    SourceIF* eventQ;
    std::string name;
    EventHandlerIF* handler;
    BatchSorterIF* sorter;
    Tracer* tracer;

public:
    aSocketThread(aSocketStageWrapper* wrapper) {
        if (DEBUG) std::cerr << "!!!!!aSocketThread init" << std::endl;
        this->wrapper = wrapper;
        this->name = "aSocketThread <" + wrapper->getStage()->getName() + ">";
        this->selsource = wrapper->getSelectSource();
        this->eventQ = wrapper->getEventQueue();
        this->handler = wrapper->getEventHandler();
        this->sorter = wrapper->getBatchSorter();
        sorter->init(wrapper, mgr);

        if (PROFILE) {
            if (name.find("WriteStage") != std::string::npos) {
                this->tracer = WriteEventHandler::tracer;
            } else {
                this->tracer = new Tracer(name);
            }
        }
    }

    void registerTP(ThreadPool* tp) {
        this->tp = tp;
    }

    void run() {
        if (DEBUG) std::cerr << name << ": starting, selsource=" << selsource << ", eventQ=" << eventQ
            << ", handler=" << handler << std::endl;

        while (true) {
            std::this_thread::yield();

            if (DEBUG) std::cerr << name << ": Looping in run()" << std::endl;
            try {
                while (selsource != nullptr && selsource->numActive() == 0) {
                    if (DEBUG) std::cerr << name << ": numActive is zero, waiting on event queue" << std::endl;

                    if (PROFILE) tracer->trace("sorter.nextBatch");
                    BatchDescrIF* batch = sorter->nextBatch(EVENT_QUEUE_TIMEOUT);
                    if (batch != nullptr) {
                        std::vector<QueueElementIF*> qelarr = batch->getBatch();
                        if (DEBUG) std::cerr << name << ": got " << qelarr.size() << " new requests" << std::endl;
                        if (PROFILE) tracer->trace("sorter.nextBatch return non-null");
                        handler->handleEvents(qelarr);
                        if (PROFILE) tracer->trace("handle batch return");
                    } else {
                        if (PROFILE) tracer->trace("sorter.nextBatch return null");
                    }
                }

                for (int s = 0; s < SELECT_SPIN; s++) {
                    if (DEBUG) std::cerr << name << ": doing select, numActive " << selsource->numActive() << std::endl;
                    SelectQueueElement* ret;
                    if (PROFILE) tracer->trace("selsource.blocking_dequeue_all");
                    ret = selsource->blocking_dequeue_all(SELECT_TIMEOUT);
                    if (ret != nullptr) {
                        if (DEBUG) std::cerr << name << ": select got " << ret->size() << " elements" << std::endl;
                        if (PROFILE) tracer->trace("selsource return non-null");

                        long tstart = std::time(nullptr);
                        handler->handleEvents(ret);
                        long tend = std::time(nullptr);
                        wrapper->getStats()->recordServiceRate(ret->size(), tend - tstart);

                    } else {
                        if (DEBUG) std::cerr << name << ": select got null" << std::endl;
                        if (PROFILE) tracer->trace("selsource return null");
                    }
                }

                if (DEBUG) std::cerr << name << ": Checking request queue" << std::endl;
                for (int s = 0; s < EVENT_QUEUE_SPIN; s++) {
                    if (PROFILE) tracer->trace("eventq nextBatch");
                    BatchDescrIF* batch = sorter->nextBatch(0);
                    if (batch != nullptr) {
                        if (PROFILE) tracer->trace("eventq nextBatch ret non-null");
                        std::vector<QueueElementIF*> qelarr = batch->getBatch();
                        if (DEBUG) std::cerr << name << ": got " << qelarr.size() << " new requests" << std::endl;
                        handler->handleEvents(qelarr);
                        if (PROFILE) tracer->trace("eventq nextBatch handler done");
                        break;
                    } else {
                        if (PROFILE) tracer->trace("eventq nextBatch ret null");
                        // std::this_thread::yield();
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << name << ": got exception " << e.what() << std::endl;
                e.printStackTrace();
            }
        }
    }

};