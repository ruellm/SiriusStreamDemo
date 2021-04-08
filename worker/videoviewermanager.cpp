#include <QPixmap>
#include <chrono>

#include "videoviewermanager.h"
#include "backend/colorconverter.h"
#include "backend/h264decoder.h"

VideoViewerManager::VideoViewerManager(QObject *parent)
    : QObject(parent)
    , alive_(false)
{

}

void VideoViewerManager::Start()
{
    alive_ = true;
    decoderThread_ = std::thread(std::bind(&VideoViewerManager::DoDecode, this));
    renderThread_ = std::thread(std::bind(&VideoViewerManager::DoRender, this));
}

void VideoViewerManager::Stop()
{
    alive_ = false;

    decoderCond_.notify_one();
    decoderThread_.join();

    renderCond_.notify_one();
    renderThread_.join();
}

void VideoViewerManager::DoDecode()
{
    H264Decoder decoder;
    decoder.Initialize();

    std::queue<backend::MediaFrame> lastFrames;

    while(alive_) {
        {
            Lock lock(decoderMutex_);
            decoderCond_.wait(lock);

            if(!alive_)
            {
                return;
            }

            std::swap(lastFrames, inputFrames_);
        }

        while(!lastFrames.empty())
        {
            auto frame = lastFrames.front();
            lastFrames.pop();

            uint8_t* output = nullptr;
            size_t outSize = 0;
            int width = 0;
            int height = 0;
            bool ret = decoder.MakeFrame(frame.bytes, frame.bufferSize,
                            &output, &outSize, &width, &height, (int)frame.type);

            if(ret)
            {
                uint8_t* rgb = new uint8_t[width * height * 3];
                colorconverter::Yuv420ProgPlanarToRgb(output, rgb, width, height);

                VideoFrame vFrame;
                QImage image(rgb, width, height, QImage::Format_RGB888);
                QPixmap pixmap = QPixmap::fromImage(image);

                vFrame.image = std::move(pixmap);
                vFrame.width = width;
                vFrame.height = height;
                vFrame.timestamp = frame.timestamp;

                Lock lock(videoFramesMutex_);
                videoFrames_.push(vFrame);
                renderCond_.notify_one();

                delete[] output;
                delete[] rgb;
            }

            delete[] frame.bytes;
         }
    }
}

void VideoViewerManager::DoRender()
{
    using namespace std;
    using namespace backend;

    static const auto streamRate = 1000ms / 60;

    while(alive_) {
        {
            Lock lock(renderMutex_);
            renderCond_.wait_for(lock, streamRate);
        }
        if (!alive_)
        {
          return;
        }

        if (incomingTimeStamp == std::numeric_limits<TimePoint>::min() ||
            videoFrames_.size() == 0)
        {
            continue;
        }

        // Skip all frames which occur significantly before the stream time...
        TimePoint frameTime{numeric_limits<TimePoint>::min()};

        {
            Lock lock2(videoFramesMutex_);

             if (!videoFrames_.empty())
             {
                 frameTime = videoFrames_.front().timestamp;
             }
        }

        auto diffTime = streamTime_ - frameTime;
        while (!videoFrames_.empty() && diffTime > streamRate)
        {

            VideoFrame vf = videoFrames_.front();
            videoFrames_.pop();

            frameTime =  numeric_limits<TimePoint>::min();

            {
                Lock lock3(videoFramesMutex_);

                if (!videoFrames_.empty())
                {
                    frameTime = videoFrames_.front().timestamp;
                }
                else
                {
                    frameTime = numeric_limits<TimePoint>::min();
                }
            }

            diffTime = streamTime_ - frameTime;
        }

        // Present topmost video frame if it's within 25 ms of the stream time....
        auto diff = frameTime - streamTime_;
        bool validDiff = (diff < streamRate && diff > -streamRate);

        if (!videoFrames_.empty() && validDiff)
        {
            VideoFrame vf = videoFrames_.front();
            emit frameReady(vf.image);
            videoFrames_.pop();

        }

        auto now = Clock::now();
        streamTime_ += now - referenceTime_;
        referenceTime_ = now;
    }
}


void VideoViewerManager::SetTimestamps(TimePoint ts_from_audio)
{
    Lock lock(renderMutex_);
    streamTime_ = ts_from_audio;
    referenceTime_ = backend::Clock::now();
}

void VideoViewerManager::AddToList(backend::MediaFrame frame) {
    using namespace std;

    // Fill in incomingTimeStamp and buf from the Frame packet...
    if (incomingTimeStamp == std::numeric_limits<TimePoint>::min())
    {
      incomingTimeStamp = frame.timestamp;
      SetTimestamps(incomingTimeStamp - 250ms);
    }
    else
      incomingTimeStamp = frame.timestamp;

    Lock lock(decoderMutex_);
    inputFrames_.push(frame);

    decoderCond_.notify_one();
}

void VideoViewerManager::ResetTime(TimePoint ts) {
    SetTimestamps(ts);
}
