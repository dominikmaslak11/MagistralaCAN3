#ifndef BARCHARTWIDGET_H
#define BARCHARTWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include "ml/assoc_learner.h"  // dla Candidate

class BarChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BarChartWidget(QWidget *parent = nullptr);
    void setCandidates(const QVector<Candidate> &candidates);

private:
    QCustomPlot *plot_;
};

#endif
