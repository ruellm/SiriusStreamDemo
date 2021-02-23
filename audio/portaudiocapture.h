#ifndef PORTAUDIOCAPTURE_H
#define PORTAUDIOCAPTURE_H

#include "portaudio/portaudio.h"
#include "audio/iaudioprocessor.h"

namespace audio {
class PortAudioCapture
{
public:
    struct RecordData{
        IAudioProcessor* audioProcessor_;

        RecordData() : audioProcessor_(nullptr) {
        }
    };

public:
    PortAudioCapture();

    bool Initialize();
    void SetProcessor(IAudioProcessor* processor);
    bool Start();
    bool Stop();

private:
    PaStreamParameters inputParameters_;
    RecordData userData_;
    PaStream* stream_;

};

}

#endif // PORTAUDIOCAPTURE_H
