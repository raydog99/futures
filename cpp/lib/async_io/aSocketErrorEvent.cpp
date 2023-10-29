#include "async_io/QueueElementIF.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class aSocketErrorEvent : public QueueElementIF {
private:
    std::string message;

protected:
    aSocketErrorEvent(const std::string& message) : message(message) {}

public:
    std::string getMessage() {
        return message;
    }
};