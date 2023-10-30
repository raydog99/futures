namespace aSocketConst {
    // The size of the internal read buffer in bytes
    const int READ_BUFFER_SIZE = 16384;
    // Indicates whether the reader should copy data into a new buffer
    const bool READ_BUFFER_COPY = true;
    // Number of times to try to finish a socket write
    const int TRYWRITE_SPIN = 10;
    // Maximum number of bytes to try writing at once; -1 if no limit
    const int MAX_WRITE_LEN = -1;
    // Maximum number of write reqs on a socket to process at once
    const int MAX_WRITE_REQS_PER_SOCKET = 1000;
    // Maximum number of writes to process at once
    const int MAX_WRITES_AT_ONCE = -1;
    // Maximum number of accepts to process at once
    const int MAX_ACCEPTS_AT_ONCE = 1000;
    // Number of empty writes after which write-ready mask is disabled. If set to -1, no disable will occur.
    const int WRITE_MASK_DISABLE_THRESHOLD = 10;

    // Time in ms to sleep waiting for select
    const int SELECT_TIMEOUT = 1000;
    // Number of times to spin on select
    const int SELECT_SPIN = 1;
    // Time in ms to sleep waiting on event queue
    const int EVENT_QUEUE_TIMEOUT = 1000;
    // Number of times to spin on event queue
    const int EVENT_QUEUE_SPIN = 10;

    // Maximum aggregation constant for aSocketRCTM.
    const int LARGE_AGGREGATION = 4096;
    // Number of measurements to use when adjusting rate in aSocketRCTM.
    const int MEASUREMENT_SIZE = 200;

    const std::string READSTAGE_NAME = "aSocket ReadStage";
    const std::string WRITESTAGE_NAME = "aSocket WriteStage";
    const std::string LISTENSTAGE_NAME = "aSocket ListenStage";
}
