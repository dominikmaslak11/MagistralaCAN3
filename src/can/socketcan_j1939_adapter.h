#ifndef SOCKETCAN_J1939_ADAPTER_H
#define SOCKETCAN_J1939_ADAPTER_H

#include "j1939_interface.h"

class SocketCanJ1939Adapter : public J1939Interface {
public:
    SocketCanJ1939Adapter();
    ~SocketCanJ1939Adapter() override;

    bool open(const QString &ifname, uint8_t srcAddr, bool promisc = true) override;
    void close() override;
    bool readFrame(J1939Frame &frame) override;
    bool writeFrame(const J1939Frame &frame) override;
    QString interfaceName() const override;
    int socketHandle() const override;

private:
    int sock_ = -1;
    QString ifname_;
};

#endif
