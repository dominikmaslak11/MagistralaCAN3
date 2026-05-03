#include "time_series_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

TimeSeriesWidget::TimeSeriesWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    plot_ = new QCustomPlot;
    layout->addWidget(plot_);

    plot_->setInteraction(QCP::iRangeDrag, true);
    plot_->setInteraction(QCP::iRangeZoom, true);
    plot_->axisRect()->setRangeZoom(Qt::Horizontal);
    plot_->legend->setVisible(true);
    plot_->xAxis->setLabel("Czas [s]");
    plot_->yAxis->setLabel("Wartość bajtu");
    plot_->xAxis->setRange(0, 10);
    plot_->yAxis->setRange(0, 255);

    QVector<QColor> colors = {
        QColor("#FF6B6B"), QColor("#4ECDC4"), QColor("#45B7D1"),
        QColor("#96CEB4"), QColor("#FFEAA7"), QColor("#DDA0DD"),
        QColor("#98D8C8"), QColor("#F7DC6F")
    };

    // Panel z checkboxami
    auto *checkLayout = new QHBoxLayout;
    for (int i = 0; i < 8; ++i) {
        QCPGraph *graph = plot_->addGraph();
        graph->setPen(QPen(colors[i]));
        graph->setName(QString("B%1").arg(i));
        byteGraphs_[i] = graph;

        byteChecks_[i] = new QCheckBox(QString("B%1").arg(i));
        byteChecks_[i]->setChecked(true);
        connect(byteChecks_[i], &QCheckBox::toggled, this, [this, i](bool vis) {
            onByteVisibilityChanged(i, vis);
        });
        checkLayout->addWidget(byteChecks_[i]);
    }
    layout->addLayout(checkLayout);
}

void TimeSeriesWidget::setActiveId(uint32_t id) {
    activeId_ = id;
    clearGraphs();
}

void TimeSeriesWidget::addDataPoint(uint32_t id, const uint8_t *data, uint8_t dlc, double timestampSec) {
    if (id != activeId_) return;

    for (int i = 0; i < dlc && i < 8; ++i) {
        byteGraphs_[i]->addData(timestampSec, data[i]);
    }
    for (auto *graph : byteGraphs_) {
        if (graph->dataCount() > maxPoints) {
            graph->data()->removeBefore(timestampSec - 20);
        }
    }
    plot_->xAxis->setRange(timestampSec - 10, timestampSec + 1);
    plot_->replot();
}

void TimeSeriesWidget::clearGraphs() {
    for (auto *graph : byteGraphs_) {
        graph->data()->clear();
    }
    plot_->replot();
}

void TimeSeriesWidget::onByteVisibilityChanged(int byteIdx, bool visible) {
    if (byteIdx >= 0 && byteIdx < 8) {
        byteGraphs_[byteIdx]->setVisible(visible);
        byteChecks_[byteIdx]->setChecked(visible); // synchronizacja
        plot_->replot();
    }
}
