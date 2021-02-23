#include "portaudiocapture.h"
#include "audioadapter.h"

namespace audio {

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int RecordCallback(const void* inputBuffer, void*,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void* userData)
{
    PortAudioCapture::RecordData* pData = (PortAudioCapture::RecordData*)userData;
    if(pData->audioProcessor_ == nullptr)
        return paAbort;

    pData->audioProcessor_->ProcessAudio(inputBuffer, framesPerBuffer);
    return paContinue;
}


PortAudioCapture::PortAudioCapture()
    : stream_(0){
}

void PortAudioCapture::SetProcessor(IAudioProcessor* processor) {
    userData_.audioProcessor_ = processor;
}

bool PortAudioCapture::Initialize() {
    PaError err = paNoError;

    inputParameters_.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters_.device == paNoDevice) {
       return false;
    }

    inputParameters_.channelCount = 1;                    /* stereo input */
    inputParameters_.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters_.suggestedLatency = Pa_GetDeviceInfo(inputParameters_.device)->defaultLowInputLatency;
    inputParameters_.hostApiSpecificStreamInfo = nullptr;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
       &stream_,
       &inputParameters_,
       nullptr,                  /* &outputParameters, */
       SAMPLE_RATE,
       FRAMES_PER_BUFFER,
       paClipOff,               /* we won't output out of range samples so don't bother clipping them */
       RecordCallback,
       &userData_);

    if (err != paNoError)
       return false;

    return true;
}

bool PortAudioCapture::Start() {
    PaError err = paNoError;

    err = Pa_StartStream(stream_);
    if (err != paNoError)
       return false;

    return true;
}

bool PortAudioCapture::Stop() {
    if(stream_ == nullptr)
        return false;

    PaError err = Pa_CloseStream(stream_);
    if (err != paNoError)
        return false;

    return true;
}
}
