#include <iostream>
#include <vector>

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

class WriteEventHandler : public aSocketEventHandler {
private:
    static const bool DEBUG = false;
    static const bool PROFILE = false;
    static Tracer tracer;

public:
    WriteEventHandler() {
        if (PROFILE) tracer = Tracer("aSocket WriteEH");
    }

    void init(ConfigDataIF config) override {}

    void destroy() override {}
}