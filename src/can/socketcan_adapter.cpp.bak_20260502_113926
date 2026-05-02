#include "socketcan_adapter.h"
#include <QDebug>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <cstring>
#include <sys/time.h>
#include "util/logger.h"

SocketCanAdapter::SocketCanAdapter() : sock_(-1) {}

SocketCanAdapter::~SocketCanAdapter() {
    close();
}

bool SocketCanAdapter::open(const QString &ifname) {
    if (sock_ >= 0) {
        qWarning() << "SocketCAN już otwarty";
        return false;
    }

    sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock_ < 0) {
        if (Logger::isDebugMode()) Logger::instance()->log(QString("socket() błąd: %1").arg(strerror(errno)), "ERROR");
        qCritical() << "Nie można utworzyć gniazda CAN:" << strerror(errno);
        return false;
    }
    if (Logger::isDebugMode()) Logger::instance()->log(QString("socket() = %1").arg(sock_));

    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, ifname.toStdString().c_str(), IFNAMSIZ - 1);
    if (Logger::isDebugMode()) Logger::instance()->log(QString("ioctl(SIOCGIFINDEX) dla %1").arg(ifname));
    if (ioctl(sock_, SIOCGIFINDEX, &ifr) < 0) {
        if (Logger::isDebugMode()) Logger::instance()->log(QString("ioctl błąd: %1").arg(strerror(errno)), "ERROR");
        qCritical() << "Nie znaleziono interfejsu" << ifname << ":" << strerror(errno);
        ::close(sock_);
        sock_ = -1;
        return false;
    }

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (Logger::isDebugMode()) Logger::instance()->log(QString("bind() błąd: %1").arg(strerror(errno)), "ERROR");
        qCritical() << "Błąd bind():" << strerror(errno);
        ::close(sock_);
        sock_ = -1;
        return false;
    }
    if (Logger::isDebugMode()) Logger::instance()->log("bind() OK");

    ifname_ = ifname;
    qDebug() << "SocketCAN otwarty na" << ifname;
    return true;
}

void SocketCanAdapter::close() {
    if (sock_ >= 0) {
        ::close(sock_);
        sock_ = -1;
        ifname_.clear();
        qDebug() << "SocketCAN zamknięty";
    }
}

static int debugFrameCounter = 0;
bool SocketCanAdapter::readFrame(CanFrame &frame) {
    if (sock_ < 0) return false;

    struct can_frame rawFrame;
    ssize_t n = read(sock_, &rawFrame, sizeof(rawFrame));
    if (Logger::isDebugMode() && n > 0 && (++debugFrameCounter % 100 == 0)) Logger::instance()->log(QString("read() %1 bajtów, ID=0x%2").arg(n).arg(rawFrame.can_id, 3, 16, QChar('0')));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return false;
        if (Logger::isDebugMode()) Logger::instance()->log(QString("read() błąd: %1").arg(strerror(errno)), "ERROR");
        qWarning() << "Błąd odczytu CAN:" << strerror(errno);
        return false;
    }
    if (n != sizeof(rawFrame)) return false;

    frame.id = rawFrame.can_id;
    frame.dlc = rawFrame.len;
    std::memcpy(frame.data, rawFrame.data, rawFrame.len);
    if (ioctl(sock_, SIOCGSTAMP, &frame.timestamp) < 0) {
        gettimeofday(&frame.timestamp, nullptr);
    }
    return true;
}

bool SocketCanAdapter::writeFrame(const CanFrame &frame) {
    if (sock_ < 0) return false;

    struct can_frame rawFrame;
    rawFrame.can_id = frame.id;
    rawFrame.len = frame.dlc;
    std::memcpy(rawFrame.data, frame.data, frame.dlc);

    ssize_t n = write(sock_, &rawFrame, sizeof(rawFrame));
    if (Logger::isDebugMode()) Logger::instance()->log(QString("write() %1 bajtów").arg(n));
    if (n != sizeof(rawFrame)) {
        qWarning() << "Błąd zapisu CAN:" << strerror(errno);
        return false;
    }
    return true;
}

QString SocketCanAdapter::interfaceName() const {
    return ifname_;
}

int SocketCanAdapter::socketHandle() const {
    return sock_;
}
