#include "async_io/NonblockingInputStream.h"
#include "async_io/NonblockingOutputStream.h"
#include "async_io/SelectItem.h"
#include "async_io/SelectSource.h"
#include <iostream>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class SelectSourceIF {
public:
    virtual void register(Object selobj) = 0;
    virtual void deregister(Object selobj) = 0;
    virtual void update() = 0;
    virtual void update(Object selobj) = 0;
    virtual int numRegistered() = 0;
    virtual int numActive() = 0;
    virtual int size() = 0;
    virtual QueueElementIF* dequeue() = 0;
    virtual QueueElementIF** dequeue_all() = 0;
    virtual QueueElementIF** dequeue(int num) = 0;
    virtual QueueElementIF* blocking_dequeue(int timeout_millis) = 0;
    virtual QueueElementIF** blocking_dequeue_all(int timeout_millis) = 0;
    virtual QueueElementIF** blocking_dequeue(int timeout_millis, int num) = 0;
};

class SelectSource : public SelectSourceIF {
private:
    static const bool DEBUG = false;
    static const bool PROFILE = false;

    SelectSet selset;
    SelectItem* ready;
    int ready_offset, ready_size;

    bool do_balance;
    const int BALANCER_SEQUENCE_SIZE = 10000;
    int* balancer_seq;
    int balancer_seq_off;
    Object blocker;
    Tracer tracer;

public:
    SelectSource() : SelectSource(true) {}

    SelectSource(bool do_balance) : ready(nullptr), ready_offset(0), ready_size(0), do_balance(do_balance) {
        blocker = Object();
        if (DEBUG) std::cerr << "SelectSource created, do_balance = " << do_balance << std::endl;
        if (PROFILE) tracer = Tracer("NBIO SelectSource <" + std::to_string(reinterpret_cast<std::intptr_t>(&selset)) + ">");
        if (do_balance) initBalancer();
    }

    void register(Object selobj) override {
        if (DEBUG) std::cerr << "SelectSource: register " << selobj << std::endl;
        if (!(selobj instanceof SelectItem)) {
            std::cerr << "register() called with non-SelectItem argument. Should not happen!!" << std::endl;
            return;
        }
        selset.interruptSelect(); // blow it out of any select, unlock blocker.
        SelectItem sel = static_cast<SelectItem*>(selobj);
        selset.add(sel);
        synchronized (blocker) {
            blocker.notify();
        }
    }

    void deregister(Object selobj) override {
        if (DEBUG) std::cerr << "SelectSource: deregister " << selobj << std::endl;
        if (!(selobj instanceof SelectItem)) {
            std::cerr << "deregister() called with non-SelectItem argument. Should not happen!!" << std::endl;
            return;
        }
        selset.interruptSelect(); // blow it out of any selects, unlock blocker
        SelectItem sel = static_cast<SelectItem*>(selobj);
        selset.remove(sel);
        synchronized (blocker) {
            blocker.notify();
        }
    };
};