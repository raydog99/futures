#include "async_io/SinkIF.h"
#include "async_io/ATcpConnection.h"

class ATcpStartReadRequest : public aSocketRequest, public QueueElementIF {
public:
    ATcpConnection* conn;
    SinkIF* compQ;
    int readClogTries;

    ATcpStartReadRequest(ATcpConnection* conn, SinkIF* compQ, int readClogTries)
        : conn(conn), compQ(compQ), readClogTries(readClogTries) {
    }
};