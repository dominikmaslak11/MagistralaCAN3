#include "can_frame_detail_view.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDateTime>

CanFrameDetailView::CanFrameDetailView(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    idLabel_ = new QLabel("ID: -");
    dlcLabel_ = new QLabel("DLC: -");
    timeLabel_ = new QLabel("Czas: -");
    layout->addWidget(idLabel_);
    layout->addWidget(dlcLabel_);
    layout->addWidget(timeLabel_);

    showBitsCheck_ = new QCheckBox("Pokaż bity");
    connect(showBitsCheck_, &QCheckBox::toggled, this, [this](bool) { updateDisplay(); });
    layout->addWidget(showBitsCheck_);

    auto *grid = new QGridLayout;
    grid->addWidget(new QLabel("<b>Hex</b>"), 0, 1);
    grid->addWidget(new QLabel("<b>Bin</b>"), 0, 2);

    for (int i = 0; i < 8; ++i) {
        auto *byteLabel = new QLabel(QString("B%1").arg(i));
        grid->addWidget(byteLabel, i+1, 0);

        hexLabels_[i] = new QLabel("-");
        hexLabels_[i]->setAlignment(Qt::AlignCenter);
        hexLabels_[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        hexLabels_[i]->setMinimumWidth(30);
        grid->addWidget(hexLabels_[i], i+1, 1);

        binLabels_[i] = new QLabel("-");
        binLabels_[i]->setAlignment(Qt::AlignCenter);
        binLabels_[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        binLabels_[i]->setMinimumWidth(90);
        grid->addWidget(binLabels_[i], i+1, 2);
    }
    layout->addLayout(grid);
}

void CanFrameDetailView::showFrame(const CanFrame &frame) {
    currentFrame_ = frame;
    changedBytes_.clear();
    updateDisplay();
}

void CanFrameDetailView::highlightChanges(const QVector<int> &changedBytes) {
    changedBytes_.clear();
    for (int b : changedBytes) changedBytes_.insert(b);
    updateDisplay();
}

void CanFrameDetailView::clearFrame() {
    currentFrame_ = CanFrame{};
    changedBytes_.clear();
    updateDisplay();
}

void CanFrameDetailView::updateDisplay() {
    if (currentFrame_.dlc == 0) {
        idLabel_->setText("ID: -");
        dlcLabel_->setText("DLC: -");
        timeLabel_->setText("Czas: -");
        for (int i = 0; i < 8; ++i) {
            hexLabels_[i]->setText("-"); hexLabels_[i]->setStyleSheet("");
            binLabels_[i]->setText("-"); binLabels_[i]->setStyleSheet("");
        }
        return;
    }

    QString idStr;
    if (currentFrame_.id & CAN_EFF_FLAG)
        idStr = QString("0x%1 (29-bit)").arg(currentFrame_.id & CAN_EFF_MASK, 8, 16, QChar('0'));
    else
        idStr = QString("0x%1 (11-bit)").arg(currentFrame_.id & CAN_SFF_MASK, 3, 16, QChar('0'));
    idLabel_->setText("ID: " + idStr);

    dlcLabel_->setText(QString("DLC: %1").arg(currentFrame_.dlc));
    qint64 ms = currentFrame_.timestamp.tv_sec * 1000LL + currentFrame_.timestamp.tv_usec / 1000;
    timeLabel_->setText(QString("Czas: %1").arg(QDateTime::fromMSecsSinceEpoch(ms).toString("hh:mm:ss.zzz")));

    for (int i = 0; i < 8; ++i) {
        bool changed = changedBytes_.contains(i);
        QString style = changed ? "background-color: #FFA500; color: black;" : "";

        if (i < currentFrame_.dlc) {
            uint8_t byte = currentFrame_.data[i];
            hexLabels_[i]->setText(QString("%1").arg(byte, 2, 16, QChar('0')).toUpper());
            hexLabels_[i]->setStyleSheet(style);

            if (showBitsCheck_->isChecked()) {
                binLabels_[i]->setText(byteToBin(byte));
                binLabels_[i]->setStyleSheet(style);
                binLabels_[i]->setVisible(true);
            } else {
                binLabels_[i]->setText("-");
                binLabels_[i]->setVisible(false);
            }
        } else {
            hexLabels_[i]->setText("-"); hexLabels_[i]->setStyleSheet("");
            binLabels_[i]->setText("-"); binLabels_[i]->setStyleSheet("");
            binLabels_[i]->setVisible(false);
        }
    }
}

QString CanFrameDetailView::byteToBin(uint8_t byte) const {
    QString bin;
    for (int i = 7; i >= 0; --i) {
        bin += ((byte >> i) & 1) ? '1' : '0';
        if (i == 4) bin += ' ';
    }
    return bin;
}
