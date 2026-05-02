#ifndef J1939_INTERFACE_H
#define J1939_INTERFACE_H

#include <QString>
#include <linux/can.h>
#include <sys/time.h>

struct J1939Frame {
    uint8_t priority;
    uint32_t pgn;
    uint8_t srcAddr;
    uint8_t dstAddr;
    uint8_t data[8];
    uint8_t dlc;
    timeval timestamp;
};

class J1939Interface {
public:
    virtual ~J1939Interface() = default;
    virtual bool open(const QString &ifname, uint8_t srcAddr, bool promisc = true) = 0;
    virtual void close() = 0;
    virtual bool readFrame(J1939Frame &frame) = 0;
    virtual bool writeFrame(const J1939Frame &frame) = 0;
    virtual QString interfaceName() const = 0;
    virtual int socketHandle() const = 0;
};

#endif
