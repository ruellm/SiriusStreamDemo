#ifndef AUDIOCAPTUREWORKER_H
#define AUDIOCAPTUREWORKER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <stdint.h>
#include <queue>
#include "audio/iaudioprocessor.h"

class AudioCaptureWorker : public QThread, public IAudioProcessor
{
    Q_OBJECT

public:
    AudioCaptureWorker(QObject *parent = nullptr);
    ~AudioCaptureWorker();

    void Stop();
    virtual bool ProcessAudio(const void* buffer, int len);

protected:
   void run() override;

private:
    struct AudioData {
        void* buffer;
        size_t len;
    };

    std::queue<AudioData> data_;
    QMutex waitMutex_;
    QMutex dataMutex_;
    QWaitCondition waitCond_;
    std::atomic<bool> alive_;
};

#endif // AUDIOCAPTUREWORKER_H
