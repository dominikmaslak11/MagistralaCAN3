#ifndef CAN_FRAME_MODEL_H
#define CAN_FRAME_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>
#include <linux/can.h>
#include "can/can_interface.h"

class CanFrameModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CanFrameModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setOverwriteMode(bool enable);
    void setHighlightEnabled(bool enable);
    bool highlightEnabled() const;
    bool overwriteMode() const;

    CanFrame frameAt(int row) const;

public slots:
    void addFrame(const CanFrame &frame);
    void clear();
    void setHighlightedCandidates(const QSet<uint32_t> &ids);

signals:
    void frameUpdated(int row, const CanFrame &frame, QVector<int> changedBytes);

private:
    int findRowById(canid_t id) const;
    QList<CanFrame> frames_;
    QMap<canid_t, int> idToRow_;
    QMap<int, CanFrame> previousFrames_;
    bool overwriteMode_ = false;
    bool highlightEnabled_ = true;
    QSet<uint32_t> highlightedIds_;
    static const int maxFrames = 100000;
};

#endif
