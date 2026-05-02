#ifndef CAN_FD_INTERFACE_H
#define CAN_FD_INTERFACE_H

#include <QString>
#include <linux/can.h>
#include <sys/time.h>

struct CanFdFrame {
    canid_t id;
    uint8_t data[64];
    uint8_t dlc;
    uint8_t flags;
    timeval timestamp;
};

class CanFdInterface {
public:
    virtual ~CanFdInterface() = default;
    virtual bool open(const QString &ifname) = 0;
    virtual void close() = 0;
    virtual bool readFrame(CanFdFrame &frame) = 0;
    virtual bool writeFrame(const CanFdFrame &frame) = 0;
    virtual QString interfaceName() const = 0;
    virtual int socketHandle() const = 0;
};

#endif // CAN_FD_INTERFACE_H
