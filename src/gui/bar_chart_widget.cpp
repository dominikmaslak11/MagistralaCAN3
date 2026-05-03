#include "bar_chart_widget.h"
#include <QVBoxLayout>

BarChartWidget::BarChartWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    plot_ = new QCustomPlot;
    layout->addWidget(plot_);

    plot_->setInteraction(QCP::iRangeDrag, false);
    plot_->setInteraction(QCP::iRangeZoom, false);
    plot_->xAxis->setLabel("ID");
    plot_->yAxis->setLabel("Score");
    plot_->yAxis->setRange(0, 1);
}

void BarChartWidget::setCandidates(const QVector<Candidate> &candidates) {
    plot_->clearPlottables();
    if (candidates.isEmpty()) {
        plot_->replot();
        return;
    }

    QVector<double> ticks, values;
    QVector<QString> labels;
    for (int i = 0; i < qMin(10, candidates.size()); ++i) {
        ticks.append(i + 0.5);
        values.append(candidates[i].score);
        labels.append(QString("0x%1").arg(candidates[i].id, 3, 16, QChar('0')));
    }

    QCPBars *bars = new QCPBars(plot_->xAxis, plot_->yAxis);
    bars->setData(ticks, values);
    bars->setWidth(0.6);

    // Poprawne ustawienie etykiet osi X za pomocą QCPAxisTickerText
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    for (int i = 0; i < labels.size(); ++i) {
        ticker->addTick(ticks[i], labels[i]);
    }
    plot_->xAxis->setTicker(ticker);
    plot_->xAxis->setRange(0, ticks.size());

    plot_->rescaleAxes();
    plot_->replot();
}
