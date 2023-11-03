#include "async_io/aSocketInputStream.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

aSocketInputStream::aSocketInputStream() {
    nextSeqNum = 1;
}

aSocketInputStream::aSocketInputStream(long initialSeqNum) {
    nextSeqNum = initialSeqNum;
}

void aSocketInputStream::addPacket(ATcpInPacket pkt) {
    long sn = pkt.getSequenceNumber();
    if (sn == 0) {
        // No sequence number - assume it's in order, but don't increment the nextSeqNum
        addArray(pkt.getBytes());
        return;
    } else if (sn == nextSeqNum) {
        addArray(pkt.getBytes());
        nextSeqNum++;
        // seqNum of 0 is special
        if (nextSeqNum == 0) {
            nextSeqNum = 1;
        }
    } else {
        // Assume out of order. Don't treat (sn < nextSeqNum) differently than (sn > nextSeqNum), since we have wraparound.
        outoforder.insert(pkt);
    }

    while (!outoforder.empty()) {
        ATcpInPacket first = *outoforder.begin();
        if (first.seqNum == nextSeqNum) {
            outoforder.erase(outoforder.begin());
            addArray(first.getBytes());
            nextSeqNum++;
            // seqNum of 0 is special
            if (nextSeqNum == 0) {
                nextSeqNum = 1;
            }
        } else {
            break;
        }
    }
}

void aSocketInputStream::clear() {
    MultiByteArrayInputStream::clear();
    outoforder.clear();
}

long aSocketInputStream::getNextSequenceNumber() {
    return nextSeqNum;
}