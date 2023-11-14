#ifndef SELECT_QUEUE_ELEMENT_H
#define SELECT_QUEUE_ELEMENT_H

#include "async_io/QueueElementIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class SelectQueueElement : public QueueElementIF {
public:
    virtual Object* getItem() = 0;
    virtual Object* getAttachment() = 0;
    virtual void clearEvents() = 0;
};

#endif