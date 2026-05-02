#include "candidate_list_view.h"
#include <QVBoxLayout>
#include <QHeaderView>

CandidateListView::CandidateListView(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    table_ = new QTableWidget(0, 4);
    table_->setHorizontalHeaderLabels({"ID", "Score", "Event", "Total"});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(table_);
}

void CandidateListView::setCandidates(const QVector<Candidate> &candidates) {
    table_->setRowCount(0);
    for (const auto &c : candidates) {
        int row = table_->rowCount();
        table_->insertRow(row);
        table_->setItem(row, 0, new QTableWidgetItem(QString("0x%1").arg(c.id, 3, 16, QChar('0'))));
        table_->setItem(row, 1, new QTableWidgetItem(QString::number(c.score, 'f', 3)));
        table_->setItem(row, 2, new QTableWidgetItem(QString::number(c.eventCount)));
        table_->setItem(row, 3, new QTableWidgetItem(QString::number(c.totalCount)));
    }
}
