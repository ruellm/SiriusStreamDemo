#ifndef PORTAUDIOPLAYER_H
#define PORTAUDIOPLAYER_H

#include <stdint.h>
#include <queue>
#include "portaudio/portaudio.h"
#include "backend/speexadapter.h"
#include "audio/audioadapter.h"
#include "backend/backend.h"
#include "audio/ivideoviewer.h"

namespace audio {
class PortAudioPlayer
{
public:
    struct AudioData {
        unsigned char buffer[FRAMES_PER_BUFFER * 2];
        backend::TimePoint timestamp;
    };

public:
    PortAudioPlayer();
    bool Initialize();
    bool Start();
    bool Stop();

    void SetViewer(IVideoVidewer* viewer);
    IVideoVidewer* GetViewer();

    void AddData(uint8_t* buffer, size_t len, backend::TimePoint timestamp);

    std::queue<AudioData> audioData_;

private:
    PaStreamParameters outputParameters_;
    PaStream* stream_;
    SpeexAdapter speex_;
    IVideoVidewer* viewer_;
};

}

#endif // PORTAUDIOPLAYER_H
