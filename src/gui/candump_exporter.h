#ifndef CANDUMP_EXPORTER_H
#define CANDUMP_EXPORTER_H

#include <QObject>
#include "can/can_interface.h"

class CandumpExporter : public QObject {
    Q_OBJECT
public:
    explicit CandumpExporter(QObject *parent = nullptr);

    bool exportToFile(const QList<CanFrame> &frames, const QString &path, const QString &interface = "vcan0");
};

#endif // CANDUMP_EXPORTER_H
