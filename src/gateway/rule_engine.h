#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

#include <QObject>
#include <QVector>
#include <QStringList>
#include "can/can_interface.h"

class RuleEngine : public QObject {
    Q_OBJECT
public:
    explicit RuleEngine(QObject *parent = nullptr);

    void loadRules(const QStringList &rules);
    QStringList currentRules() const;

    // Przetwarza ramkę. Zwraca true, jeśli ramka ma być przepuszczona (być może zmodyfikowana).
    // frame jest modyfikowane w miejscu.
    bool processFrame(CanFrame &frame);

signals:
    void logMessage(const QString &msg);

private:
    bool execRule(const QString &rule, CanFrame &frame);
    int evalExpr(const QString &expr, const CanFrame &frame);
    QStringList rules_;
};

#endif
