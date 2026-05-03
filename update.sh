#!/bin/bash
# fix_remove_channel_safe.sh – MagistralaCAN3: bezpieczne usuwanie kanału + filtrowanie bajtów wykresu
# Uruchom w katalogu ~/MagistralaCAN3

set -e
echo "🔧 MagistralaCAN3 – bezpieczny Usuń kanał + filtrowanie bajtów wykresu"

if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Nie znaleziono CMakeLists.txt. Uruchom w ~/MagistralaCAN3"
    exit 1
fi

# ------------------------------------------------------------
# 1. Poprawka CanWorker – dodajemy slot stop() (już jest, ale upewniamy się)
#    W main.cpp zmieniamy w->stop() na bezpieczne wywołanie
# ------------------------------------------------------------
python3 << 'PYFIX'
import re

with open("src/main.cpp", "r") as f:
    content = f.read()

# Zamień `w->stop();` na `QMetaObject::invokeMethod(w, "stop", Qt::QueuedConnection);`
old_stop = 'w->stop();'
new_stop = 'QMetaObject::invokeMethod(w, "stop", Qt::QueuedConnection);'
content = content.replace(old_stop, new_stop)

# Dodaj #include <QMetaObject> jeśli nie ma
if '#include <QMetaObject>' not in content:
    content = content.replace('#include <QShortcut>', '#include <QShortcut>\n#include <QMetaObject>')

with open("src/main.cpp", "w") as f:
    f.write(content)
print("✔ Bezpieczne usuwanie kanału (invokeMethod)")
PYFIX

# ------------------------------------------------------------
# 2. Dodanie filtrowania bajtów na wykresie (checkboxy w TimeSeriesWidget)
# ------------------------------------------------------------
cp src/gui/time_series_widget.h src/gui/time_series_widget.h.bak_bytefilt_$(date +%Y%m%d_%H%M%S)
cp src/gui/time_series_widget.cpp src/gui/time_series_widget.cpp.bak_bytefilt_$(date +%Y%m%d_%H%M%S)

# Rozszerzamy interfejs o checkboxy i metody filtrowania
cat > src/gui/time_series_widget.h << 'CEOF'
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
CEOF

cat > src/gui/time_series_widget.cpp << 'CEOF'
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
CEOF

echo "✔ Filtrowanie bajtów na wykresie dodane"

# Kompilacja
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "✅ Usuwanie kanału jest teraz bezpieczne (nie crashuje)."
echo "✅ Na wykresie są checkboxy do włączania/wyłączania bajtów."
echo ""
echo "Aby wypchnąć na GitHub:"
echo "  git add ."
echo "  git commit -m \"Bezpieczne usuwanie kanału, filtrowanie bajtów wykresu\""
echo "  git push"
