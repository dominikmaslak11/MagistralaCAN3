#ifndef THROTTLED_FRAME_MODEL_H
#define THROTTLED_FRAME_MODEL_H

#include <QObject>
#include <QTimer>
#include "can_frame_model.h"

class ThrottledFrameModel : public QObject {
    Q_OBJECT
public:
    explicit ThrottledFrameModel(CanFrameModel *target, QObject *parent = nullptr);

public slots:
    void appendFrames(const QVector<CanFrame> &frames);

private:
    CanFrameModel *model_;
    QTimer *flushTimer_;
    QVector<CanFrame> pending_;
};

#endif
