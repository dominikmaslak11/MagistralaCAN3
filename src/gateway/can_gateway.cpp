#include "can_gateway.h"
#include "rule_engine.h"
#include "gui/can_frame_model.h"
#include "can/can_worker.h"
#include <QDebug>

CanGateway::CanGateway(QObject *parent) : QObject(parent) {}

void CanGateway::setModelA(CanFrameModel *model) { modelA_ = model; }
void CanGateway::setModelB(CanFrameModel *model) { modelB_ = model; }
void CanGateway::setWorkerA(CanWorker *worker) { workerA_ = worker; }
void CanGateway::setWorkerB(CanWorker *worker) { workerB_ = worker; }
void CanGateway::setBidirectional(bool enabled) { bidirectional_ = enabled; }
void CanGateway::setLuaModifyEnabled(bool enabled) { luaModify_ = enabled; }
void CanGateway::setRuleEngine(RuleEngine *engine) { ruleEngine_ = engine; }

void CanGateway::start() {
    if (running_) return;
    if (!modelA_ || !modelB_) {
        emit logMessage("Gateway: brak modeli A/B");
        return;
    }
    connect(modelA_, &CanFrameModel::frameUpdated, this, &CanGateway::onFrameReceivedA);
    connect(modelB_, &CanFrameModel::frameUpdated, this, &CanGateway::onFrameReceivedB);
    running_ = true;
    emit logMessage("Gateway uruchomiony");
}

void CanGateway::stop() {
    if (!running_) return;
    disconnect(modelA_, nullptr, this, nullptr);
    disconnect(modelB_, nullptr, this, nullptr);
    running_ = false;
    emit logMessage("Gateway zatrzymany");
}

bool CanGateway::isRunning() const { return running_; }

void CanGateway::onFrameReceivedA(int row, const CanFrame &frame, const QVector<int> &changed) {
    Q_UNUSED(row); Q_UNUSED(changed);
    relayFrame(frame, workerB_);
}

void CanGateway::onFrameReceivedB(int row, const CanFrame &frame, const QVector<int> &changed) {
    Q_UNUSED(row); Q_UNUSED(changed);
    if (bidirectional_) {
        relayFrame(frame, workerA_);
    }
}

void CanGateway::relayFrame(const CanFrame &frame, CanWorker *destWorker) {
    if (!destWorker) return;

    CanFrame modFrame = frame;

    // Przepuść przez RuleEngine (jeśli podłączony)
    if (ruleEngine_ && !ruleEngine_->processFrame(modFrame)) {
        emit logMessage(QString("Ramka 0x%1 odrzucona przez regułę").arg(frame.id, 3, 16, QChar('0')));
        return;
    }

    // Wyślij fizycznie
    destWorker->sendFrame(modFrame);
}
