#ifndef BACKEND_H
#define BACKEND_H

#include <functional>
#include <stdint.h>
#include <chrono>

namespace backend {

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

struct MediaFrame {

    enum class Type : uint8_t {
        AUDIO = 0,
        VIDEO_IDR,
        VIDEO_I,
        VIDEO_P,
        VIDEO_B,
        AUDIO_C,
        VIDEO_C
    };

    uint8_t* bytes;
    size_t bufferSize;

    TimePoint timestamp;
    Type type;
};

using OnApplicationReady = void();
using OnErrorEvent = void(const std::string&);
using OnChannelEvent = void(const std::string&);
using VideoStreamId = std::string;
using OnStreamCreated = void(VideoStreamId);
using OnFrameReceived = void(MediaFrame frame);

void SetOnChannelInvite(std::function<OnChannelEvent> callback);

void SetOnChannelCreated(std::function<OnChannelEvent> callback);

void SetOnChannelCreationError(std::function<OnChannelEvent> callback);

void ConfirmChannel(const std::string& identity);

void DenyChannel(const std::string& identity);

void Initialize(std::function<OnApplicationReady> onReady);

void Register(std::function<void(const std::string&)> onRegisterSuccess,
              std::function<OnErrorEvent> onError);

void Login(const std::string& identity,
           std::function<void(const std::string&)> onSuccess,
           std::function<OnErrorEvent> onError);

void CreateChannel(const std::string& identity);

void SendMessage(const std::string& identity, const std::string& message);

void SetMessageHandler(const std::string& identity,
                       std::function<backend::OnChannelEvent> callback);

void CreateBroadcastStream(std::function<OnStreamCreated> OnCreated,
                           std::function<OnErrorEvent> onError);

void SendVideoFrame(uint8_t* buffer, size_t len, int type);

void SendAudioFrame(uint8_t* buffer, size_t len);

void CreateViewerStream(std::string streamId,
                        std::function<OnStreamCreated> OnCreated,
                        std::function<OnFrameReceived> OnFrameReceived,
                        std::function<OnErrorEvent> onError);

}

#endif // BACKEND_H
