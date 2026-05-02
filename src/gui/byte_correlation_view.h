#ifndef BYTE_CORRELATION_VIEW_H
#define BYTE_CORRELATION_VIEW_H

#include <QWidget>
#include <QTableWidget>
#include "ml/value_assoc_learner.h"

class ByteCorrelationView : public QWidget {
    Q_OBJECT
public:
    explicit ByteCorrelationView(QWidget *parent = nullptr);
    void setCorrelations(const QVector<ByteCorrelation> &correlations);
private:
    QTableWidget *table_;
};

#endif
