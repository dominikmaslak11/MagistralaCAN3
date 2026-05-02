#ifndef SOCKETCAN_ADAPTER_H
#define SOCKETCAN_ADAPTER_H

#include "can/can_interface.h"
#include <QString>

class SocketCanAdapter : public CanInterface {
public:
    SocketCanAdapter();
    ~SocketCanAdapter() override;

    bool open(const QString &ifname) override;
    void close() override;
    bool readFrame(CanFrame &frame) override;
    bool writeFrame(const CanFrame &frame) override;
    QString interfaceName() const override;
    int socketHandle() const override;

private:
    int sock_;
    QString ifname_;
};

#endif // SOCKETCAN_ADAPTER_H
