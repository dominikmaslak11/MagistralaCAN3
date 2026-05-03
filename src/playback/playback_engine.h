#ifndef PLAYBACK_ENGINE_H
#define PLAYBACK_ENGINE_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include "can/can_interface.h"

class CanWorker;

class PlaybackEngine : public QObject {
    Q_OBJECT
public:
    explicit PlaybackEngine(QObject *parent = nullptr);

    bool loadFile(const QString &fileName);
    void start(CanWorker *worker);
    void stop();
    void pause();
    void resume();
    bool isPlaying() const;
    bool isPaused() const;
    int totalFrames() const;
    int currentIndex() const;

signals:
    void progressChanged(int current, int total);
    void playbackFinished();
    void logMessage(const QString &msg);

private slots:
    void sendNextFrame();

private:
    QVector<CanFrame> frames_;
    QVector<double> timestamps_;
    CanWorker *worker_ = nullptr;
    int playIndex_ = 0;
    bool playing_ = false;
    bool paused_ = false;
    QTimer *timer_;
};

#endif
