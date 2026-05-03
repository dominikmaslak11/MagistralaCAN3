#include "playback_engine.h"
#include "can/can_worker.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

PlaybackEngine::PlaybackEngine(QObject *parent) : QObject(parent) {
    timer_ = new QTimer(this);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, this, &PlaybackEngine::sendNextFrame);
}

bool PlaybackEngine::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        emit logMessage("Nie można otworzyć pliku: " + fileName);
        return false;
    }

    QDataStream in(&file);
    quint32 frameCount;
    in >> frameCount;
    if (frameCount == 0 || frameCount > 1000000) { // sanity check
        emit logMessage("Nieprawidłowy format pliku lub brak ramek.");
        return false;
    }

    frames_.resize(frameCount);
    for (quint32 i = 0; i < frameCount; ++i) {
        in.readRawData(reinterpret_cast<char*>(&frames_[i]), sizeof(CanFrame));
    }

    // Spróbuj odczytać timestampy (jeśli są)
    if (!in.atEnd()) {
        quint32 tsCount;
        in >> tsCount;
        timestamps_.resize(tsCount);
        for (quint32 i = 0; i < tsCount; ++i) {
            in >> timestamps_[i];
        }
    }

    file.close();

    emit logMessage(QString("Wczytano %1 ramek z pliku %2").arg(frameCount).arg(fileName));
    return true;
}

void PlaybackEngine::start(CanWorker *worker) {
    if (playing_ || !worker) return;
    if (frames_.isEmpty()) {
        emit logMessage("Brak ramek do odtworzenia.");
        return;
    }
    worker_ = worker;
    playIndex_ = 0;
    playing_ = true;
    paused_ = false;
    emit logMessage("Odtwarzanie offline rozpoczęte");
    sendNextFrame();
}

void PlaybackEngine::stop() {
    playing_ = false;
    paused_ = false;
    timer_->stop();
    worker_ = nullptr;
    emit playbackFinished();
    emit logMessage("Odtwarzanie zatrzymane");
}

void PlaybackEngine::pause() {
    if (!playing_ || paused_) return;
    paused_ = true;
    timer_->stop();
    emit logMessage("Odtwarzanie spauzowane");
}

void PlaybackEngine::resume() {
    if (!playing_ || !paused_) return;
    paused_ = false;
    sendNextFrame();
    emit logMessage("Odtwarzanie wznowione");
}

bool PlaybackEngine::isPlaying() const { return playing_; }
bool PlaybackEngine::isPaused() const { return paused_; }
int PlaybackEngine::totalFrames() const { return frames_.size(); }
int PlaybackEngine::currentIndex() const { return playIndex_; }

void PlaybackEngine::sendNextFrame() {
    if (!playing_ || paused_ || playIndex_ >= frames_.size()) {
        if (playing_) {
            playing_ = false;
            emit playbackFinished();
            emit logMessage("Odtwarzanie zakończone (koniec pliku)");
        }
        return;
    }

    if (worker_) {
        worker_->sendFrame(frames_.at(playIndex_));
    }

    emit progressChanged(playIndex_ + 1, frames_.size());

    // Oblicz opóźnienie do następnej ramki
    int delayMs = 1;
    if (playIndex_ > 0 && playIndex_ < timestamps_.size()) {
        double dt = timestamps_.at(playIndex_) - timestamps_.at(playIndex_ - 1);
        delayMs = qBound(1, (int)(dt * 1000), 1000);
    }
    playIndex_++;

    timer_->start(delayMs);
}
