#include "async_io/SinkIF.h"

class ConnectSockState {
protected:
    ATcpClientSocket* clisock;
    SinkIF* compQ;
    int connectClogTries, connectNumTries;
    int writeClogThreshold;
    bool completed = false;

protected:
    virtual void complete() = 0;
    virtual void error(aSocketErrorEvent* error) = 0;
};