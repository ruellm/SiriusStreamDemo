#include "audiocaptureworker.h"
#include "backend/speexadapter.h"
#include "backend/backend.h"

AudioCaptureWorker::AudioCaptureWorker(QObject *parent)
    : QThread { parent }
    , alive_(false)
{
}

AudioCaptureWorker::~AudioCaptureWorker() {
    Stop();
}

void AudioCaptureWorker::run()
{
    SpeexAdapter speex;
    alive_ = true;

    while(alive_) {
        waitMutex_.lock();
        if(data_.size() == 0)
            waitCond_.wait(&waitMutex_);
        waitMutex_.unlock();

        dataMutex_.lock();
        auto data = data_.front();
        data_.pop();
        dataMutex_.unlock();

        //library expect length as based on sample length and not buffer
        int size = speex.Compress(data.len,(uint8_t*)data.buffer);

        backend::SendAudioFrame((uint8_t*)data.buffer, size);

        free(data.buffer);
    }
}

bool AudioCaptureWorker::ProcessAudio(const void* buffer, int len) {
    AudioData data;

    auto buffSize = len * sizeof(short); // samples are 16bit
    data.buffer = (void*)malloc(buffSize);
    memcpy(data.buffer, buffer, buffSize);
    data.len = len;

    dataMutex_.lock();
    data_.push(data);
    dataMutex_.unlock();

    waitCond_.wakeOne();

    return true;
}

void AudioCaptureWorker::Stop() {
    while(!data_.empty()) {
        auto data = data_.front();
        free(data.buffer);
        data_.pop();
    }

    alive_ = false;

    terminate();
}
