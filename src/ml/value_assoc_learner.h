#ifndef VALUE_ASSOC_LEARNER_H
#define VALUE_ASSOC_LEARNER_H

#include "assoc_learner.h"
#include <QMap>
#include <QPair>

struct ByteCorrelation {
    uint32_t id;
    int byteIndex;
    double correlation;
    int samples;
};

class ValueAssocLearner : public AssocLearner {
    Q_OBJECT
public:
    explicit ValueAssocLearner(double windowSec = 2.0, QObject *parent = nullptr);

    void addValueSample(double value);
    QVector<ByteCorrelation> computeCorrelations(int topN = 30) const;
    void resetAll();

private:
    struct Sample {
        double value;
        QMap<uint32_t, QByteArray> frames;
    };
    QVector<Sample> samples_;
};

#endif
