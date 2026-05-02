#include "flow_block.h"

FlowBlock::FlowBlock(const QString &name, QObject *parent)
    : QObject(parent), name_(name) {}

FlowBlock::~FlowBlock() {}

void FlowBlock::addNextBlock(FlowBlock *block) {
    nextBlocks_.append(block);
}
