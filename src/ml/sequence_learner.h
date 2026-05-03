#ifndef SEQUENCE_LEARNER_H
#define SEQUENCE_LEARNER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QPair>
#include "assoc_learner.h"

class SequenceLearner : public AssocLearner {
    Q_OBJECT
public:
    enum SeqMode { SingleID, Bigram, Trigram };

    explicit SequenceLearner(double windowSec = 2.0, QObject *parent = nullptr);

    void setSeqMode(SeqMode mode);
    SeqMode seqMode() const;

    QVector<Candidate> getCandidates(int topN = 20) const override; // nadpisujemy
    QSet<uint32_t> candidateIds() const override;

    void reset() override;

private:
    void flushWindow() override; // nadpisujemy

    SeqMode seqMode_ = SingleID;
    // Dla bigramów/trigramów – śledzimy ostatnie ID
    uint32_t lastId_ = 0xFFFFFFFF;
    uint32_t secondLastId_ = 0xFFFFFFFF;
    bool hasLast_ = false;
    bool hasSecond_ = false;

    // Dodatkowe zliczenia dla par
    QMap<QPair<uint32_t,uint32_t>, int> pairEventCounts_;
    QMap<QPair<uint32_t,uint32_t>, int> pairTotalCounts_;
    QMap<QPair<uint32_t,uint32_t>, int> tripleEventCounts_;
    QMap<QPair<uint32_t,uint32_t>, int> tripleTotalCounts_;
};

#endif
