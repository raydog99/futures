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
    }

    void update() override {
        selset.update();
    }

    int numRegistered() override {
        return selset.size();
    }

    int numActive() override {
        return selset.numActive();
    }

    int size() override {
        return (ready_size - ready_offset);
    }

    QueueElementIF* dequeue() override {
        if (selset.size() == 0) return nullptr;

        if ((ready_size == 0) || (ready_offset == ready_size)) {
            doPoll(0);
        }
        if (ready_size == 0) return nullptr;
        return new SelectQueueElement(ready[ready_offset++]);
    }

    QueueElementIF** dequeue_all() override {
        if (selset.size() == 0) return nullptr;

        if ((ready_size == 0) || (ready_offset == ready_size)) {
            doPoll(0);
        }
        if (ready_size == 0) return nullptr;
        SelectQueueElement** ret = new SelectQueueElement*[ready_size - ready_offset];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = new SelectQueueElement(ready[ready_offset++]);
        }
        return ret;
    }

    QueueElementIF* blocking_dequeue(int timeout_millis) override {
        if (selset.size() == 0) {
            if (timeout_millis == 0) return nullptr;
            // Wait for something to be registered
            synchronized (blocker) {
                if (timeout_millis == -1) {
                    try {
                        blocker.wait();
                    } catch (InterruptedException ie) {
                    }
                } else {
                    try {
                        blocker.wait(timeout_millis);
                    } catch (InterruptedException ie) {
                    }
                }
            }
        }

        if ((ready_size == 0) || (ready_offset == ready_size)) {
            doPoll(timeout_millis);
        }
        if (ready_size == 0) return nullptr;
        return new SelectQueueElement(ready[ready_offset++]);
    }

    QueueElementIF** blocking_dequeue_all(int timeout_millis) override {
        if (PROFILE) tracer.trace("blocking_dequeue_all called");
        if (selset.size() == 0) {
            if (timeout_millis == 0) return nullptr;
            // Wait for something to be registered
            if (PROFILE) tracer.trace(" bdqa: blocking");
            synchronized (blocker) {
                if (timeout_millis == -1) {
                    try {
                        blocker.wait();
                    } catch (InterruptedException ie) {
                    }
                } else {
                    try {
                        blocker.wait(timeout_millis);
                    } catch (InterruptedException ie) {
                    }
                }
            }
            if (PROFILE) tracer.trace(" bdqa: done blocking");
        }
        if ((ready_size == 0) || (ready_offset == ready_size)) {
            doPoll(timeout_millis);
        }
        if (ready_size == 0) return nullptr;
        if (PROFILE) tracer.trace(" bdqa: fill in ret[]");
        int numtoret = std::min(ready_size - ready_offset, num);
        SelectQueueElement** ret = new SelectQueueElement*[numtoret];
        for (int i = 0; i < numtoret; i++) {
            ret[i] = new SelectQueueElement(ready[ready_offset++]);
        }
        return ret;
    }
private:
    void doPoll(int timeout) {
        if (DEBUG) std::cerr << "SelectSource: Doing poll, timeout " << timeout << std::endl;
        if (PROFILE) tracer.trace("doPoll called");
        int c = selset.select(timeout);
        if (DEBUG) std::cerr << "SelectSource: poll returned " << c << std::endl;
        if (PROFILE) tracer.trace("select returned zero");
        // Didn't get anything
        ready = nullptr; ready_offset = ready_size = 0;
        if (c > 0) {
            if (PROFILE) tracer.trace("select returned nonzero");
            SelectItem* ret = selset.getEvents();
            if (PROFILE) tracer.trace("getEvents() return");
            if (ret != nullptr) {
                // Likely necessary change: Ret !== nullptr if doPoll() is synchronized with 
                // deregister()
                ready_offset = 0; ready_size = ret.length;
                if (PROFILE) tracer.trace("calling balance");
                balance(ret);
                if (PROFILE) tracer.trace("balance return");
                return;
            }
        }
        if (PROFILE) tracer.trace("select returned zero");
        // Didn't get anything
        ready = nullptr; ready_offset = ready_size = 0;
    }

    void balance(SelectItem* selarr) {
        if (DEBUG) std::cerr << "SelectSource: balance called, selarr size=" << selarr.length << std::endl;
        if ((!do_balance) || (selarr.length < 2)) {
            ready = selarr;
        } else {
            SelectItem a;
            ready = new SelectItem[selarr.length];

            for (int i = 0; i < ready.length; i++) {
                if (balancer_seq_off == BALANCER_SEQUENCE_SIZE) {
                    balancer_seq_off = 0;
                }
                int n = balancer_seq[balancer_seq_off++] % selarr.length;
                int c = 0;
                a = selarr[n]; selarr[n] = nullptr; ready[i] = a;
            }
            while (selarr[n] == nullptr) {
                    n++; c++;
                    if (n == selarr.length) n = 0;
                    if (c == selarr.length) {
                        std::cerr << "WARNING: SelectSource.balance(): All items in selarr are null (n=" << n << ", c=" << c << ", len=" << selarr.length << std::endl;
                        for (int k = 0; k < ready.length; k++) {
                            std::cerr << "[" << k << "] ready:" << ready[k] << " selarr:" << selarr[k] << std::endl;
                        }
                        throw std::invalid_argument("balance: All items in selarr are null!");
                    }
                }
                if (DEBUG) std::cerr << "SelectSource: balance: " << n << "->" << i << std::endl;
                a = selarr[n]; selarr[n] = nullptr; ready[i] = a;
        }
    }

    void initBalancer() {
        balancer_seq = new int[BALANCER_SEQUENCE_SIZE];
        std::srand(std::time(0)); // Need better seed?
        for (int i = 0; i < BALANCER_SEQUENCE_SIZE; i++) {
            balancer_seq[i] = std::abs(std::rand());
        }
        balancer_seq_off = 0;
    }

    void interruptSelect() {
        if (DEBUG) std::cerr << "SelectSource.interruptSelect() called." << std::endl;
        selset.interruptSelect();
    }
};