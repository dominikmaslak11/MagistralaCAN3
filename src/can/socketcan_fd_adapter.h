#ifndef SOCKETCAN_FD_ADAPTER_H
#define SOCKETCAN_FD_ADAPTER_H
#include "can_fd_interface.h"
class SocketCanFdAdapter : public CanFdInterface {
public:
    bool open(const QString &ifname) override { return false; }
    void close() override {}
    bool readFrame(CanFdFrame &) override { return false; }
    bool writeFrame(const CanFdFrame &) override { return false; }
    QString interfaceName() const override { return ""; }
    int socketHandle() const override { return -1; }
};
#endif
