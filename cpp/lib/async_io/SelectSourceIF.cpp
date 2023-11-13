#ifndef SELECT_SOURCE_IF_H
#define SELECT_SOURCE_IF_H

#include "async_io/SelectSourceIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class SelectSourceIF : public SourceIF {
public:
    virtual void register(Object* sel) = 0;
    virtual Object* register(Object* sc, int ops) = 0;
    virtual void deregister(Object* sel) = 0;
    virtual void update() = 0;
    virtual void update(Object* sel) = 0;
    virtual int numRegistered() = 0;
    virtual int numActive() = 0;
    virtual int size() = 0;
    virtual void interruptSelect() = 0;
};

#endif