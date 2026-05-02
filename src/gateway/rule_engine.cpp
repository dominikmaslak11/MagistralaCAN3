#include "rule_engine.h"
#include <QDebug>
#include <QRegularExpression>

RuleEngine::RuleEngine(QObject *parent) : QObject(parent) {}

void RuleEngine::loadRules(const QStringList &rules) {
    rules_ = rules;
}

QStringList RuleEngine::currentRules() const {
    return rules_;
}

bool RuleEngine::processFrame(CanFrame &frame) {
    for (const QString &rule : rules_) {
        if (rule.trimmed().isEmpty() || rule.trimmed().startsWith('#')) continue;
        if (!execRule(rule, frame)) {
            emit logMessage(QString("Reguła odrzuciła ramkę 0x%1").arg(frame.id, 3, 16, QChar('0')));
            return false; // drop frame
        }
    }
    return true;
}

int RuleEngine::evalExpr(const QString &expr, const CanFrame &frame) {
    QString e = expr.trimmed();
    // Proste parsowanie: id, dlc, data[N]
    if (e == "id") return frame.id;
    if (e == "dlc") return frame.dlc;
    QRegularExpression dataRe("data\\[(\\d+)\\]");
    auto match = dataRe.match(e);
    if (match.hasMatch()) {
        int idx = match.captured(1).toInt();
        if (idx >= 0 && idx < 8) return frame.data[idx];
    }
    // Jeśli to liczba
    bool ok;
    int val = e.toInt(&ok, 0); // 0x... też
    if (ok) return val;
    return 0;
}

bool RuleEngine::execRule(const QString &rule, CanFrame &frame) {
    // Składnia: if <expr> <op> <expr> -> <command>
    // Komendy: drop(), setByte(N,value), sendTo(channel)
    QRegularExpression re(R"(if\s+(.+?)\s+([<>=!]+)\s+(.+?)\s*->\s*(.+))");
    auto match = re.match(rule);
    if (!match.hasMatch()) {
        emit logMessage("Nieprawidłowa składnia: " + rule);
        return true; // ignoruj
    }

    int left = evalExpr(match.captured(1), frame);
    QString op = match.captured(2);
    int right = evalExpr(match.captured(3), frame);
    QString cmd = match.captured(4).trimmed();

    bool condition = false;
    if (op == "==") condition = (left == right);
    else if (op == "!=") condition = (left != right);
    else if (op == "<") condition = (left < right);
    else if (op == ">") condition = (left > right);
    else if (op == "<=") condition = (left <= right);
    else if (op == ">=") condition = (left >= right);

    if (!condition) return true; // reguła nie dotyczy, przepuszczamy

    // Wykonaj komendę
    if (cmd == "drop()") {
        return false;
    } else if (cmd.startsWith("setByte(")) {
        QRegularExpression sbre("setByte\\((\\d+)\\s*,\\s*(.+)\\)");
        auto sm = sbre.match(cmd);
        if (sm.hasMatch()) {
            int idx = sm.captured(1).toInt();
            int val = evalExpr(sm.captured(2), frame);
            if (idx >= 0 && idx < 8) {
                frame.data[idx] = val;
                emit logMessage(QString("setByte %1 = 0x%2").arg(idx).arg(val, 2, 16, QChar('0')));
            }
        }
    }
    return true;
}
