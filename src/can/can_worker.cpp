#include "can_worker.h"
#include "socketcan_adapter.h"
#include "socketcan_fd_adapter.h"
#include "socketcan_j1939_adapter.h"
#include <QDebug>
#include <QThread>

CanWorker::CanWorker(QObject *parent) : QObject(parent) {}

CanWorker::~CanWorker() { stop(); }

void CanWorker::setInterface(const QString &name) { ifname_ = name; }
void CanWorker::setMode(Mode mode) { mode_ = mode; }

void CanWorker::stop() {
    running_ = false;
    if (adapter_) { adapter_->close(); delete adapter_; adapter_ = nullptr; }
    if (fdAdapter_) { fdAdapter_->close(); delete fdAdapter_; fdAdapter_ = nullptr; }
    if (j1939Adapter_) { j1939Adapter_->close(); delete j1939Adapter_; j1939Adapter_ = nullptr; }
}

void CanWorker::process() {
    if (mode_ == CANFD) {
        fdAdapter_ = new SocketCanFdAdapter();
        if (!fdAdapter_->open(ifname_)) {
            emit errorOccurred(QString("Nie można otworzyć CAN FD %1").arg(ifname_));
            return;
        }
        running_ = true;
        QVector<CanFdFrame> batch;
        batch.reserve(1000);
        while (running_) {
            CanFdFrame frame;
            if (fdAdapter_->readFrame(frame)) {
                batch.append(frame);
                if (batch.size() >= 1000) {
                    emit fdFramesReady(batch);
                    batch.clear();
                }
            } else {
                QThread::usleep(100);
                if (!batch.isEmpty()) { emit fdFramesReady(batch); batch.clear(); }
            }
        }
        fdAdapter_->close();
        return;
    }

    if (mode_ == J1939) {
        j1939Adapter_ = new SocketCanJ1939Adapter();
        if (!j1939Adapter_->open(ifname_, 0x80, true)) {
            emit errorOccurred(QString("Nie można otworzyć J1939 %1").arg(ifname_));
            return;
        }
        running_ = true;
        QVector<J1939Frame> batch;
        batch.reserve(1000);
        while (running_) {
            J1939Frame frame;
            if (j1939Adapter_->readFrame(frame)) {
                batch.append(frame);
                if (batch.size() >= 1000) {
                    emit j1939FramesReady(batch);
                    batch.clear();
                }
            } else {
                QThread::usleep(100);
                if (!batch.isEmpty()) { emit j1939FramesReady(batch); batch.clear(); }
            }
        }
        j1939Adapter_->close();
        return;
    }

    // ========== CAN 2.0 ==========
    adapter_ = new SocketCanAdapter();
    if (!adapter_->open(ifname_)) {
        emit errorOccurred(QString("Nie można otworzyć CAN 2.0 %1").arg(ifname_));
        return;
    }
    running_ = true;
    QVector<CanFrame> batch;
    batch.reserve(1000);
    while (running_) {
        CanFrame frame;
        if (adapter_->readFrame(frame)) {
            batch.append(frame);
            if (batch.size() >= 1000) {
                emit framesReady(batch);
                batch.clear();
            }
        } else {
            QThread::usleep(100);
            if (!batch.isEmpty()) {
                emit framesReady(batch);
                batch.clear();
            }
        }
    }
    adapter_->close();
}
