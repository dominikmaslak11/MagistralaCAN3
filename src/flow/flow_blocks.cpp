#include "flow_blocks.h"
#include <algorithm>

// FilterIdBlock
FilterIdBlock::FilterIdBlock(uint32_t id, QObject *parent)
    : FlowBlock("FilterId", parent), id_(id) {}

QVector<CanFrame> FilterIdBlock::process(const CanFrame &frame) {
    if (frame.id == id_)
        return {frame};
    return {};
}

// ModifyByteBlock
ModifyByteBlock::ModifyByteBlock(int byteIndex, uint8_t value, QObject *parent)
    : FlowBlock("ModifyByte", parent), byteIndex_(byteIndex), value_(value) {}

QVector<CanFrame> ModifyByteBlock::process(const CanFrame &frame) {
    CanFrame out = frame;
    if (byteIndex_ < 8 && byteIndex_ < out.dlc) {
        out.data[byteIndex_] = value_;
    }
    return {out};
}

// SwapBytesBlock
SwapBytesBlock::SwapBytesBlock(int idxA, int idxB, QObject *parent)
    : FlowBlock("SwapBytes", parent), idxA_(idxA), idxB_(idxB) {}

QVector<CanFrame> SwapBytesBlock::process(const CanFrame &frame) {
    CanFrame out = frame;
    if (idxA_ < out.dlc && idxB_ < out.dlc) {
        std::swap(out.data[idxA_], out.data[idxB_]);
    }
    return {out};
}

// SendFrameBlock
SendFrameBlock::SendFrameBlock(QObject *parent)
    : FlowBlock("SendFrame", parent) {}

QVector<CanFrame> SendFrameBlock::process(const CanFrame &frame) {
    emit sendRequested(frame);
    return {}; // końcowy blok
}
