#pragma once
#include <array>
#include <chrono>
#include <vector>

#include <iostream>

#include "Orientation.hpp"
#include <boost/noncopyable.hpp>
namespace proximax::media::sirius_stream
{

enum class FrameType : uint8_t
{
    AUDIO = 0,
    VIDEO_IDR,
    VIDEO_I,
    VIDEO_P,
    VIDEO_B,
    AUDIO_C,
    VIDEO_C
};

class Frame : private boost::noncopyable
{
public:
    using Orientation = palx_headless::Orientation;
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    static constexpr const size_t layerCount{3};

    Frame();
    Frame(FrameType type,
          TimePoint ts,
          uint32_t seq,
          Orientation newOrientation,
          int len[3],
          uint8_t* frame_bytes[3]);

    Frame(FrameType type,
          TimePoint ts,
          uint32_t seq,
          Orientation newOrientation,
          std::array<std::vector<uint8_t>, layerCount> frameBytes);
    ~Frame();

    FrameType frameType{FrameType::AUDIO};
    TimePoint timestamp{};
    uint32_t sequence{0};

    /// This field is used for segregating audio packets
    uint32_t uid{0};

    std::array<std::vector<uint8_t>, layerCount> bytes{};

    Orientation orientation{Orientation::rotate0};

};

}
