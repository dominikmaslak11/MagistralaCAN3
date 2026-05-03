#ifndef TIME_SERIES_WIDGET_H
#define TIME_SERIES_WIDGET_H

#include <QWidget>
#include <QMap>
#include <QCheckBox>
#include "qcustomplot.h"

class TimeSeriesWidget : public QWidget {
    Q_OBJECT
public:
    explicit TimeSeriesWidget(QWidget *parent = nullptr);

public slots:
    void setActiveId(uint32_t id);
    void addDataPoint(uint32_t id, const uint8_t *data, uint8_t dlc, double timestampSec);
    void clearGraphs();

private slots:
    void onByteVisibilityChanged(int byteIdx, bool visible);

private:
    QCustomPlot *plot_;
    uint32_t activeId_ = 0xFFFFFFFF;
    QMap<int, QCPGraph*> byteGraphs_;
    QCheckBox *byteChecks_[8];
    static const int maxPoints = 5000;
};

#endif
