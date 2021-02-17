#pragma once

#include <optional>
#include <string>
#include <vector>

#include "IChannel.hpp"

namespace proximax::client
{

enum class VideoFrameOrientation
{
    rotate0 = 0,
    rotate90 = 90,
    rotate180 = 180,
    rotate270 = 270,
};

struct StreamingOptions
{
    bool audioCaptureEnabled{false};
    int32_t initialSpeakerVolume{100};
    int32_t initialMicVolume{100};

    bool videoCaptureEnabled{false};
    int32_t streamingWidth{640};
    int32_t streamingHeight{480};
    VideoFrameOrientation orientation{VideoFrameOrientation::rotate0};
};

class StreamingOptionsBuilder
{
public:
    StreamingOptionsBuilder& audioCaptureEnabled(bool rhs)
    {
        value_.audioCaptureEnabled = std::move(rhs);
        return *this;
    }

    StreamingOptionsBuilder& initialSpeakerVolume(int32_t rhs)
    {
        value_.initialSpeakerVolume = std::move(rhs);
        return *this;
    }

    StreamingOptionsBuilder& initialMicVolume(int32_t rhs)
    {
        value_.initialMicVolume = std::move(rhs);
        return *this;
    }

    StreamingOptionsBuilder& videoCaptureEnabled(bool rhs)
    {
        value_.videoCaptureEnabled = std::move(rhs);
        return *this;
    }

    StreamingOptionsBuilder& streamingWidth(int32_t rhs)
    {
        value_.streamingWidth = std::move(rhs);
        return *this;
    }

    StreamingOptionsBuilder& streamingHeight(int32_t rhs)
    {
        value_.streamingHeight = std::move(rhs);
        return *this;
    }

    StreamingOptionsBuilder& orientation(VideoFrameOrientation rhs)
    {
        value_.orientation = std::move(rhs);
        return *this;
    }

    StreamingOptions build()
    {
        return std::move(value_);
    }

private:
    StreamingOptions value_;
};

struct VideoCapability
{
    int32_t width{0};
    int32_t height{0};
    int32_t maxFPS{0};
    // webrtc::VideoType videoType{webrtc::VideoType::kUnknown};
    bool isInterlaced{false};
};

class VideoCapabilityBuilder
{
public:
    VideoCapabilityBuilder& width(int32_t rhs)
    {
        value_.width = std::move(rhs);
        return *this;
    }

    VideoCapabilityBuilder& height(int32_t rhs)
    {
        value_.height = std::move(rhs);
        return *this;
    }

    VideoCapabilityBuilder& maxFPS(int32_t rhs)
    {
        value_.maxFPS = std::move(rhs);
        return *this;
    }

    VideoCapabilityBuilder& isInterlaced(bool rhs)
    {
        value_.isInterlaced = std::move(rhs);
        return *this;
    }

    VideoCapability build()
    {
        return std::move(value_);
    }

private:
    VideoCapability value_;
};

struct VideoDevice
{
    std::string name;
    std::string id;
    std::vector<VideoCapability> videoCapabilities;
};

class VideoDeviceBuilder
{
public:
    VideoDeviceBuilder& name(std::string rhs)
    {
        value_.name = std::move(rhs);
        return *this;
    }

    VideoDeviceBuilder& id(std::string rhs)
    {
        value_.id = std::move(rhs);
        return *this;
    }

    VideoDeviceBuilder& videoCapabilities(std::vector<VideoCapability> rhs)
    {
        value_.videoCapabilities = std::move(rhs);
        return *this;
    }

    VideoDevice build()
    {
        return std::move(value_);
    }

private:
    VideoDevice value_;
};

struct VideoStreamParameters
{
    uint32_t version{1};
    bool isCamEnabled{false};
    bool isMicEnabled{false};
    uint32_t width{0};
    uint32_t height{0};
    std::vector<std::string> codecs;
};

class VideoStreamParametersBuilder
{
public:
    VideoStreamParametersBuilder& version(uint32_t rhs)
    {
        value_.version = std::move(rhs);
        return *this;
    }

    VideoStreamParametersBuilder& isCamEnabled(bool rhs)
    {
        value_.isCamEnabled = std::move(rhs);
        return *this;
    }

    VideoStreamParametersBuilder& isMicEnabled(bool rhs)
    {
        value_.isMicEnabled = std::move(rhs);
        return *this;
    }

    VideoStreamParametersBuilder& width(uint32_t rhs)
    {
        value_.width = std::move(rhs);
        return *this;
    }

    VideoStreamParametersBuilder& height(uint32_t rhs)
    {
        value_.height = std::move(rhs);
        return *this;
    }

    VideoStreamParametersBuilder& codecs(std::vector<std::string> rhs)
    {
        value_.codecs = std::move(rhs);
        return *this;
    }

    VideoStreamParameters build()
    {
        return std::move(value_);
    }

private:
    VideoStreamParameters value_;
};

/// Interface for video streaming
class IVideoStreamer
{
public:
    using VideoStreamID = std::string;
    using SharedConstBuffer = std::shared_ptr<std::vector<uint8_t>>;
    using OnDisplayFrameReceived =
        void(VideoStreamID, SharedConstBuffer, int32_t, int32_t, VideoFrameOrientation);
    using OnVideoStreamDestroyed = void(VideoStreamID);
    using OnRetrieveBroadcastVideoStream = void(std::optional<VideoStreamID>);
    using OnVideoStreamParameters = void(VideoStreamID, VideoStreamParameters);

    virtual ~IVideoStreamer()
    {
    }

    /// Initialize media system with streaming options provided
    virtual void initialize(const StreamingOptions& options) = 0;

    /// Method to start streaming
    virtual void startStreaming(std::function<void(VideoStreamID)> onStarted,
                                std::function<void(VideoStreamErrorID)> onError) = 0;

    /// Method to start viewing someone who is broadcasting
    virtual void startViewing(const VideoStreamID& videoStreamID,
                              std::function<void(VideoStreamID)> onStarted,
                              std::function<void(VideoStreamErrorID)> onError) = 0;

    /// Method to stop broadcasting
    virtual void stopStreaming() = 0;

    /// Method to stop viewing a specified stream
    virtual void stopViewing(const VideoStreamID& videoStreamID) = 0;

    /// Method to enable/disable audio capture
    virtual void enableAudioCapture(bool isEnabled) = 0;

    /// Method to enable/disable video capture
    virtual void enableVideoCapture(bool isEnabled) = 0;

    /// Set the microphone volume. Valid values: 0 to 100
    virtual void setMicrophoneVolume(int32_t val) = 0;

    /// Set the speaker volume. Valid values: 0 to 100
    virtual void setSpeakerVolume(int32_t val) = 0;

    /// Updates the audio input device ID
    virtual void setAudioInputDeviceID(int32_t index) = 0;

    /// Updates the audio output device ID
    virtual void setAudioOutputDeviceID(int32_t index) = 0;

    /// Set the video capability by index
    virtual void setVideoDevice(int32_t index) = 0;

    /// Set the video capability by index
    virtual void setVideoCapability(int32_t index) = 0;

    /// Retrieves the current audio input device ID
    virtual int32_t selectedAudioInputDeviceID() = 0;

    /// Retrieves the current audio output device ID
    virtual int32_t selectedAudioOutputDeviceID() = 0;

    /// Retrieves the current video device ID
    virtual int32_t selectedVideoDevice() = 0;

    /// Retrieves the current video capability by index
    virtual int32_t selectedVideoCapability() = 0;

    /// Returns a list of audio input devices
    virtual std::vector<std::string> getAudioInputDevices() = 0;

    /// Returns a list of audio output devices
    virtual std::vector<std::string> getAudioOutputDevices() = 0;

    /// Returns a list of video capabilities
    virtual std::vector<VideoDevice> getVideoDevices() = 0;

    /// Returns a list of video capabilities
    virtual std::vector<VideoCapability> getVideoCapabilities(int32_t deviceIndex) = 0;

    /// Returns the broadcast stream ID
    virtual void retrieveBroadcastVideoStreamID(
        std::function<OnRetrieveBroadcastVideoStream> onRetrieved) = 0;

    /// Register for when a display frame is received
    virtual core::signals::ConnectionPtr registerDisplayFrameReceived
    PS_SDK_NODISCARD(std::function<OnDisplayFrameReceived> onFrameReceived) = 0;

    /// Register for when the video stream is destroyed
    virtual core::signals::ConnectionPtr registerVideoStreamDestroyed
    PS_SDK_NODISCARD(std::function<OnVideoStreamDestroyed> onVideoStreamDestroyed) = 0;

    /// Register for when video stream parameters are changed
    virtual core::signals::ConnectionPtr registerVideoStreamParameters
    PS_SDK_NODISCARD(std::function<OnVideoStreamParameters> onVideoStreamParameters) = 0;
};

}
