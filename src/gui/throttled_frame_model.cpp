#include "throttled_frame_model.h"

ThrottledFrameModel::ThrottledFrameModel(CanFrameModel *target, QObject *parent)
    : QObject(parent), model_(target)
{
    flushTimer_ = new QTimer(this);
    flushTimer_->setInterval(33);
    flushTimer_->setSingleShot(true);
    connect(flushTimer_, &QTimer::timeout, this, [this]() {
        if (!pending_.isEmpty()) {
            for (const auto &f : pending_) {
            emit model_->newFramesReady(pending_);
            if (true) {
                model_->addFrame(f);
            }
            }
            pending_.clear();
        }
    });
}

void ThrottledFrameModel::appendFrames(const QVector<CanFrame> &frames) {
    pending_.append(frames);
    if (!flushTimer_->isActive()) {
        flushTimer_->start();
    }
}
