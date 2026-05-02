#include "socketcan_j1939_adapter.h"
#include <QDebug>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <cstring>
#include <sys/time.h>

// Stałe nie zawsze dostępne w nagłówkach
#ifndef SOL_CAN_J1939
#define SOL_CAN_J1939 278
#endif
#ifndef SO_J1939_PROMISC
#define SO_J1939_PROMISC 1
#endif

struct j1939_hdr_ {
    uint32_t pgn;
    uint8_t src;
    uint8_t dst;
};

SocketCanJ1939Adapter::SocketCanJ1939Adapter() {}

SocketCanJ1939Adapter::~SocketCanJ1939Adapter() { close(); }

bool SocketCanJ1939Adapter::open(const QString &ifname, uint8_t srcAddr, bool promisc) {
    if (sock_ >= 0) { qWarning("Socket J1939 już otwarty"); return false; }
    sock_ = socket(PF_CAN, SOCK_DGRAM, CAN_J1939);
    if (sock_ < 0) { qCritical("socket(CAN_J1939) failed"); return false; }

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_addr.j1939.addr = srcAddr;

    struct ifreq ifr;
    strncpy(ifr.ifr_name, ifname.toStdString().c_str(), IFNAMSIZ - 1);
    if (ioctl(sock_, SIOCGIFINDEX, &ifr) < 0) {
        qCritical("ioctl SIOCGIFINDEX failed"); ::close(sock_); sock_ = -1; return false;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        qCritical("bind() J1939 failed"); ::close(sock_); sock_ = -1; return false;
    }

    if (promisc) {
        int flag = 1;
        setsockopt(sock_, SOL_CAN_J1939, SO_J1939_PROMISC, &flag, sizeof(flag));
    }

    ifname_ = ifname;
    qDebug("J1939 otwarty na %s", qPrintable(ifname));
    return true;
}

void SocketCanJ1939Adapter::close() {
    if (sock_ >= 0) { ::close(sock_); sock_ = -1; ifname_.clear(); }
}

bool SocketCanJ1939Adapter::readFrame(J1939Frame &frame) {
    if (sock_ < 0) return false;
    struct {
        j1939_hdr_ hdr;
        uint8_t data[8];
    } pkt;

    ssize_t n = read(sock_, &pkt, sizeof(pkt));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return false;
        return false;
    }
    if (n < (ssize_t)sizeof(j1939_hdr_)) return false;

    frame.priority = (pkt.hdr.pgn >> 26) & 0x7;
    frame.pgn = pkt.hdr.pgn & 0x3FFFF;
    frame.srcAddr = pkt.hdr.src;
    frame.dstAddr = pkt.hdr.dst;
    frame.dlc = n - sizeof(j1939_hdr_);
    if (frame.dlc > 8) frame.dlc = 8;
    memcpy(frame.data, pkt.data, frame.dlc);
    if (ioctl(sock_, SIOCGSTAMP, &frame.timestamp) < 0)
        gettimeofday(&frame.timestamp, nullptr);
    return true;
}

bool SocketCanJ1939Adapter::writeFrame(const J1939Frame &frame) { /* uproszczone */ return false; }
QString SocketCanJ1939Adapter::interfaceName() const { return ifname_; }
int SocketCanJ1939Adapter::socketHandle() const { return sock_; }
