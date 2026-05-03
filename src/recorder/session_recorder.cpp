#include "session_recorder.h"
#include "can/can_worker.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QTimer>

SessionRecorder::SessionRecorder(QObject *parent) : QObject(parent) {}

void SessionRecorder::startRecording(const QString &fileName) {
    recordFileName_ = fileName;
    recordedFrames_.clear();
    recording_ = true;
    emit logMessage("Nagrywanie rozpoczęte -> " + fileName);
}

void SessionRecorder::stopRecording() {
    recording_ = false;
    saveToFile();
    emit logMessage(QString("Nagrywanie zakończone. Zapisano %1 ramek").arg(recordedFrames_.size()));
}

bool SessionRecorder::isRecording() const { return recording_; }

void SessionRecorder::startPlayback(CanWorker *worker) {
    if (!worker || playing_) return;
    if (recordedFrames_.isEmpty() && !loadFromFile(recordFileName_)) {
        emit logMessage("Brak danych do odtworzenia");
        return;
    }
    worker_ = worker;
    playIndex_ = 0;
    playing_ = true;
    emit logMessage("Odtwarzanie rozpoczęte");
    sendNextFrame();
}

void SessionRecorder::stopPlayback() {
    playing_ = false;
    emit playbackFinished();
}

bool SessionRecorder::isPlaying() const { return playing_; }

void SessionRecorder::addFrame(const CanFrame &frame) {
    if (recording_) {
        double ts = frame.timestamp.tv_sec + frame.timestamp.tv_usec / 1e6;
        timestamps_.append(ts);
    }
    if (recording_) {
        recordedFrames_.append(frame);
    }
}

void SessionRecorder::sendNextFrame() {
    if (!playing_ || playIndex_ >= recordedFrames_.size()) {
        playing_ = false;
        emit playbackFinished();
        emit logMessage("Odtwarzanie zakończone");
        return;
    }

    if (worker_) {
        worker_->sendFrame(recordedFrames_.at(playIndex_));
    }
    playIndex_++;

    // Proste odtwarzanie co 1 ms (ulepszymy w części 2)
    if (playIndex_ < recordedFrames_.size()) {
        int delayMs = 1;
        if (playIndex_ > 0 && playIndex_ < timestamps_.size()) {
            double dt = timestamps_.at(playIndex_) - timestamps_.at(playIndex_-1);
            delayMs = qBound(1, (int)(dt * 1000), 1000);
        }
        QTimer::singleShot(delayMs, this, &SessionRecorder::sendNextFrame);
    }
}

void SessionRecorder::saveToFile() {
    QFile file(recordFileName_);
    if (!file.open(QIODevice::WriteOnly)) {
        emit logMessage("Nie można zapisać pliku " + recordFileName_);
        return;
    }
    QDataStream out(&file);
    // Zapisujemy długość i każdą ramkę binarnie
    out << (quint32)recordedFrames_.size();
    out << (quint32)timestamps_.size();
    for (double ts : timestamps_) out << ts;
    for (const auto &f : recordedFrames_) {
        out.writeRawData(reinterpret_cast<const char*>(&f), sizeof(CanFrame));
    }
    file.close();
}

bool SessionRecorder::loadFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return false;
    QDataStream in(&file);
    quint32 count;
    in >> count;
    quint32 tsCount;
    in >> tsCount;
    timestamps_.resize(tsCount);
    for (quint32 i = 0; i < tsCount; ++i) {
        double ts;
        in >> ts;
        timestamps_[i] = ts;
    }
    recordedFrames_.resize(count);
    for (quint32 i = 0; i < count; ++i) {
        in.readRawData(reinterpret_cast<char*>(&recordedFrames_[i]), sizeof(CanFrame));
    }
    file.close();
    return true;
}
