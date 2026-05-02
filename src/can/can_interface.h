#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include <QString>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/time.h>

struct CanFrame {
    canid_t id;
    uint8_t data[CAN_MAX_DLEN];
    uint8_t dlc;
    timeval timestamp;
};

class CanInterface {
public:
    virtual ~CanInterface() = default;
    virtual bool open(const QString &ifname) = 0;
    virtual void close() = 0;
    virtual bool readFrame(CanFrame &frame) = 0;
    virtual bool writeFrame(const CanFrame &frame) = 0;
    virtual QString interfaceName() const = 0;
    virtual int socketHandle() const = 0;
};

#endif // CAN_INTERFACE_H
