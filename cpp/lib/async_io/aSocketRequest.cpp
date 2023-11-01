#include "async_io/QueueElementIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

// This is the superclass for the various requests that can be issued to the aSocketMgr.
class aSocketRequest : public QueueElementIF {
    // Empty class
};