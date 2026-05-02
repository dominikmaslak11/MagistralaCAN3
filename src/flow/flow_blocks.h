#ifndef FLOW_BLOCKS_H
#define FLOW_BLOCKS_H

#include "flow_block.h"

// Blok filtrujący po ID
class FilterIdBlock : public FlowBlock {
    Q_OBJECT
public:
    explicit FilterIdBlock(uint32_t id = 0, QObject *parent = nullptr);
    QVector<CanFrame> process(const CanFrame &frame) override;
    void setId(uint32_t id) { id_ = id; }
private:
    uint32_t id_ = 0;
};

// Blok modyfikujący wybrany bajt
class ModifyByteBlock : public FlowBlock {
    Q_OBJECT
public:
    explicit ModifyByteBlock(int byteIndex = 0, uint8_t value = 0, QObject *parent = nullptr);
    QVector<CanFrame> process(const CanFrame &frame) override;
    void setByteIndex(int idx) { byteIndex_ = idx; }
    void setValue(uint8_t val) { value_ = val; }
private:
    int byteIndex_ = 0;
    uint8_t value_ = 0;
};

// Blok zamieniający dwa bajty miejscami
class SwapBytesBlock : public FlowBlock {
    Q_OBJECT
public:
    explicit SwapBytesBlock(int idxA = 0, int idxB = 1, QObject *parent = nullptr);
    QVector<CanFrame> process(const CanFrame &frame) override;
    void setIndices(int a, int b) { idxA_ = a; idxB_ = b; }
private:
    int idxA_ = 0;
    int idxB_ = 1;
};

// Blok wysyłający ramkę na CAN (końcowy)
class SendFrameBlock : public FlowBlock {
    Q_OBJECT
public:
    explicit SendFrameBlock(QObject *parent = nullptr);
    QVector<CanFrame> process(const CanFrame &frame) override;
signals:
    void sendRequested(const CanFrame &frame);
};

#endif
