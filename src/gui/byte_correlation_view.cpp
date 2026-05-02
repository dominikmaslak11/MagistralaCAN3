#include "byte_correlation_view.h"
#include <QVBoxLayout>
#include <QHeaderView>

ByteCorrelationView::ByteCorrelationView(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    table_ = new QTableWidget(0, 4);
    table_->setHorizontalHeaderLabels({"ID", "Bajt", "Korelacja", "Próbki"});
    table_->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(table_);
}

void ByteCorrelationView::setCorrelations(const QVector<ByteCorrelation> &correlations) {
    table_->setRowCount(0);
    for (const auto &c : correlations) {
        int row = table_->rowCount();
        table_->insertRow(row);
        table_->setItem(row, 0, new QTableWidgetItem(QString("0x%1").arg(c.id, 3, 16, QChar('0'))));
        table_->setItem(row, 1, new QTableWidgetItem(QString::number(c.byteIndex)));
        table_->setItem(row, 2, new QTableWidgetItem(QString::number(c.correlation, 'f', 3)));
        table_->setItem(row, 3, new QTableWidgetItem(QString::number(c.samples)));
    }
}
