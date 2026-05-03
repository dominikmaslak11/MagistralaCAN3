#include "sequence_learner.h"
#include <algorithm>
#include <cmath>

SequenceLearner::SequenceLearner(double windowSec, QObject *parent)
    : AssocLearner(windowSec, parent) {}

void SequenceLearner::setSeqMode(SeqMode mode) {
    seqMode_ = mode;
    reset();
}

SequenceLearner::SeqMode SequenceLearner::seqMode() const { return seqMode_; }

QVector<Candidate> SequenceLearner::getCandidates(int topN) const {
    if (seqMode_ == SingleID) {
        return AssocLearner::getCandidates(topN);
    }

    // Dla bigramów/trigramów – budujemy z pairEventCounts_ itp.
    QVector<Candidate> out;
    const auto &eventMap = (seqMode_ == Bigram) ? pairEventCounts_ : tripleEventCounts_;
    const auto &totalMap = (seqMode_ == Bigram) ? pairTotalCounts_ : tripleTotalCounts_;

    for (auto it = eventMap.begin(); it != eventMap.end(); ++it) {
        auto key = it.key();
        int ev = it.value();
        int tot = totalMap.value(key, 0);
        if (tot == 0) continue;

        double pe = double(ev) / qMax(1, totalEventWindows_);
        double pb = double(tot - ev) / qMax(1, totalBgWindows_);
        double score = std::log(pe + 0.01) - std::log(pb + 0.01);

        Candidate c;
        // Użyjemy sztuczki: id przechowa pierwszą część pary, a drugą?
        // Ograniczenie: Candidate ma pole uint32_t id. Dla bigramów musimy
        // jakoś reprezentować parę. Dla uproszczenia: id = (first << 16) | (second & 0xFFFF)
        c.id = (key.first << 16) | (key.second & 0xFFFF);
        c.score = score;
        c.eventCount = ev;
        c.totalCount = tot;
        out.append(c);
    }

    std::sort(out.begin(), out.end(), [](const Candidate &a, const Candidate &b) {
        return a.score > b.score;
    });
    if (topN > 0 && out.size() > topN) out.resize(topN);
    return out;
}

QSet<uint32_t> SequenceLearner::candidateIds() const {
    if (seqMode_ == SingleID) return AssocLearner::candidateIds();
    // Dla bigramów/trigramów – wszystkie ID z par
    QSet<uint32_t> ids;
    const auto &eventMap = (seqMode_ == Bigram) ? pairEventCounts_ : tripleEventCounts_;
    for (auto it = eventMap.begin(); it != eventMap.end(); ++it) {
        ids.insert(it.key().first);
        ids.insert(it.key().second);
    }
    return ids;
}

void SequenceLearner::reset() {
    AssocLearner::reset();
    pairEventCounts_.clear();
    pairTotalCounts_.clear();
    tripleEventCounts_.clear();
    tripleTotalCounts_.clear();
    lastId_ = 0xFFFFFFFF;
    secondLastId_ = 0xFFFFFFFF;
    hasLast_ = false;
    hasSecond_ = false;
}

void SequenceLearner::flushWindow() {
    if (window_.isEmpty()) return;

    // Zbierz ID w oknie
    QVector<uint32_t> ids;
    for (const auto &tf : window_) {
        ids.append(tf.frame.id);
    }

    if (seqMode_ == SingleID) {
        AssocLearner::flushWindow();
        return;
    }

    // Dla bigramów – śledzimy pary
    if (seqMode_ == Bigram) {
        QSet<QPair<uint32_t,uint32_t>> seenPairs;
        for (int i = 0; i < ids.size() - 1; ++i) {
            QPair<uint32_t,uint32_t> pair(ids[i], ids[i+1]);
            seenPairs.insert(pair);
        }
        if (eventActive_) {
            totalEventWindows_++;
            for (const auto &p : seenPairs) {
                pairEventCounts_[p]++;
                pairTotalCounts_[p]++;
            }
        } else {
            totalBgWindows_++;
            for (const auto &p : seenPairs) {
                pairTotalCounts_[p]++;
            }
        }
    } else { // Trigram
        QSet<QPair<uint32_t,uint32_t>> seenTriples; // użyjemy pair jako klucz (id1,id2) -> id3? uproszczenie: triple key = (id1, id2), wartość = id3
        // Dla uproszczenia pomijamy trigram implementację (można dodać później)
    }

    emit candidatesUpdated();
}
