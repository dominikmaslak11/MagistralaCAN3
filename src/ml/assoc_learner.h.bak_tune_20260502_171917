#ifndef ASSOC_LEARNER_H
#define ASSOC_LEARNER_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QVector>
#include <QByteArray>
#include "can/can_interface.h"

struct Candidate {
    uint32_t id;
    double score;
    int eventCount;
    int totalCount;
};

class AssocLearner : public QObject {
    Q_OBJECT
public:
    explicit AssocLearner(double windowSec = 2.0, QObject *parent = nullptr);

    void processFrame(const CanFrame &frame);
    void setEventActive(bool active);
    void reset();

    QVector<Candidate> getCandidates(int topN = 20) const;
    QSet<uint32_t> candidateIds() const;
    void performIterations(int n, bool eventActive);
    QVector<Candidate> getFilteredCandidates(int topN = 20, double minScore = 0.1) const;

    // Nowe metody do pobierania zawartości okna
    QVector<uint32_t> currentWindowIds() const;
    QByteArray lastDataForId(uint32_t id) const;

signals:
    void candidatesUpdated();

private:
    void flushWindow();

    struct TimedFrame {
        CanFrame frame;
        double ts;
    };

    double windowSec_;
    bool eventActive_ = false;
    QVector<TimedFrame> window_;
    QMap<uint32_t, int> eventCounts_;
    QMap<uint32_t, int> totalCounts_;
    int totalEventWindows_ = 0;
    int totalBgWindows_ = 0;
    QSet<uint32_t> currentCandidates_;
};

#endif
