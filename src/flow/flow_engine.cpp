#include "flow_engine.h"

FlowEngine::FlowEngine(QObject *parent) : QObject(parent) {}

void FlowEngine::addRootBlock(FlowBlock *block) {
    rootBlocks_.append(block);
    // Rekurencyjnie podłącz sygnały outputReady dla SendFrameBlock
    // (na razie uproszczone – w pełnej wersji będzie graf)
}

void FlowEngine::clear() {
    rootBlocks_.clear();
}

void FlowEngine::processFrame(const CanFrame &frame) {
    for (auto *root : rootBlocks_) {
        QVector<CanFrame> results = root->process(frame);
        for (const auto &out : results) {
            // Przekaż do następnych bloków (ręcznie, bo nie mamy pełnego grafu)
            // Na razie emitujemy sygnał dla SendFrameBlock
            emit outputReady(out);
        }
    }
}
