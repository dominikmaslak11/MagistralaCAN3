#include "candump_exporter.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

CandumpExporter::CandumpExporter(QObject *parent) : QObject(parent) {}

bool CandumpExporter::exportToFile(const QList<CanFrame> &frames, const QString &path, const QString &interface) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Nie można zapisać do" << path;
        return false;
    }

    QTextStream out(&file);
    for (const auto &f : frames) {
        // Format: (timestamp) interface id#data
        double ts = f.timestamp.tv_sec + f.timestamp.tv_usec / 1e6;
        out << "(" << QString::number(ts, 'f', 6) << ") " << interface << " ";

        // ID w hex bez rozszerzenia EFF (jeśli 29-bit, candump zwykle używa formatu ## dla 29-bit)
        if (f.id & CAN_EFF_FLAG) {
            out << QString("%1#").arg(f.id & CAN_EFF_MASK, 8, 16, QChar('0'));
        } else {
            out << QString("%1#").arg(f.id & CAN_SFF_MASK, 3, 16, QChar('0'));
        }

        for (int i = 0; i < f.dlc; ++i) {
            out << QString("%1").arg(f.data[i], 2, 16, QChar('0')).toUpper();
        }
        out << "\n";
    }
    file.close();
    qDebug() << "Wyeksportowano" << frames.size() << "ramek do" << path;
    return true;
}
