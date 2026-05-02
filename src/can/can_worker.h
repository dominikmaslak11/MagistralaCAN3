#ifndef CANWORKER_H
#define CANWORKER_H

#include <QObject>
#include <QVector>
#include "can/can_interface.h"
#include "can/can_fd_interface.h"
#include "can/j1939_interface.h"

class CanWorker : public QObject {
    Q_OBJECT
public:
    enum Mode { CAN20, CANFD, J1939 };

    explicit CanWorker(QObject *parent = nullptr);
    ~CanWorker() override;

    void setInterface(const QString &name);
    void setMode(Mode mode);
    void stop();

public slots:
    void process();

signals:
    void framesReady(const QVector<CanFrame> &frames);
    void fdFramesReady(const QVector<CanFdFrame> &frames);
    void j1939FramesReady(const QVector<J1939Frame> &frames);
    void errorOccurred(const QString &error);

private:
    CanInterface *adapter_ = nullptr;
    CanFdInterface *fdAdapter_ = nullptr;
    J1939Interface *j1939Adapter_ = nullptr;
    QString ifname_;
    Mode mode_ = CAN20;
    volatile bool running_ = false;
};

#endif
