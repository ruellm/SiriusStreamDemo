#ifndef VIDEOVIEWERMANAGER_H
#define VIDEOVIEWERMANAGER_H

#include <QPixmap>
#include <QObject>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>
#include "backend/backend.h"
#include "audio/ivideoviewer.h"

class VideoViewerManager : public QObject, public IVideoVidewer
{
    Q_OBJECT

public:
    struct VideoFrame {
      backend::TimePoint timestamp;
      QPixmap image;
      short width;
      short height;
    };

public:

    explicit VideoViewerManager(QObject *parent = nullptr);

    void Start();
    void Stop();

    void AddToList(backend::MediaFrame frame);

    void SetTimestamps(backend::TimePoint ts_from_audio);

    virtual void ResetTime(backend::TimePoint ts);

signals:
   void frameReady(QPixmap pixmap); // new pixmap image catured

private:
    using Mutex = std::recursive_mutex;
    using Lock = std::unique_lock<Mutex>;
    using TimePoint = backend::TimePoint;

    void DoDecode();
    void DoRender();

    std::queue<backend::MediaFrame> inputFrames_;
    std::queue<VideoFrame> videoFrames_;

    std::thread decoderThread_;
    std::condition_variable_any decoderCond_;

    std::thread renderThread_;
    std::condition_variable_any renderCond_;

    Mutex decoderMutex_;
    Mutex renderMutex_;
    Mutex videoFramesMutex_;
    std::atomic<bool> alive_;

    TimePoint incomingTimeStamp{std::numeric_limits<TimePoint>::min()};
    TimePoint streamTime_;
    TimePoint referenceTime_;

};

#endif // VIDEOVIEWERMANAGER_H
