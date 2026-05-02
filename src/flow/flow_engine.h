#ifndef FLOW_ENGINE_H
#define FLOW_ENGINE_H

#include <QObject>
#include <QVector>
#include "flow_block.h"

class FlowEngine : public QObject {
    Q_OBJECT
public:
    explicit FlowEngine(QObject *parent = nullptr);

    void addRootBlock(FlowBlock *block);
    void clear();

    // Przetwarza ramkę przez wszystkie zarejestrowane łańcuchy
    void processFrame(const CanFrame &frame);

signals:
    void outputReady(const CanFrame &frame);

private:
    QVector<FlowBlock*> rootBlocks_;
};

#endif
