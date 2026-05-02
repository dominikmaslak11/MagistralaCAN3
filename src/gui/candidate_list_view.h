#ifndef CANDIDATE_LIST_VIEW_H
#define CANDIDATE_LIST_VIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QVector>
#include "ml/assoc_learner.h"

class CandidateListView : public QWidget {
    Q_OBJECT
public:
    explicit CandidateListView(QWidget *parent = nullptr);

public slots:
    void setCandidates(const QVector<Candidate> &candidates);

private:
    QTableWidget *table_;
};

#endif
