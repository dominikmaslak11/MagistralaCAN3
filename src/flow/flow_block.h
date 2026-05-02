#ifndef FLOW_BLOCK_H
#define FLOW_BLOCK_H

#include <QObject>
#include <QVector>
#include <QString>
#include "can/can_interface.h"

class FlowBlock : public QObject {
    Q_OBJECT
public:
    explicit FlowBlock(const QString &name, QObject *parent = nullptr);
    virtual ~FlowBlock();

    // Główna metoda przetwarzania ramki
    virtual QVector<CanFrame> process(const CanFrame &frame) = 0;

    // Dodaj blok, który ma otrzymać wynik tego bloku
    void addNextBlock(FlowBlock *block);

    QString name() const { return name_; }

signals:
    void outputReady(const CanFrame &frame);

protected:
    QString name_;
    QVector<FlowBlock*> nextBlocks_;
};

#endif
