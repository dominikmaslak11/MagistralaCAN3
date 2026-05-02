#ifndef CAN_FRAME_DETAIL_VIEW_H
#define CAN_FRAME_DETAIL_VIEW_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QVector>
#include "can/can_interface.h"

class CanFrameDetailView : public QWidget {
    Q_OBJECT
public:
    explicit CanFrameDetailView(QWidget *parent = nullptr);

public slots:
    void showFrame(const CanFrame &frame);
    void highlightChanges(const QVector<int> &changedBytes);
    void clearFrame();

private:
    QLabel *idLabel_;
    QLabel *dlcLabel_;
    QLabel *timeLabel_;
    QLabel *hexLabels_[8];
    QLabel *binLabels_[8];
    QCheckBox *showBitsCheck_;
    CanFrame currentFrame_;
    QSet<int> changedBytes_;

    void updateDisplay();
    QString byteToBin(uint8_t byte) const;
};

#endif
