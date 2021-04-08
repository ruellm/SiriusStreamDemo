#include <memory>
#include <atomic>
#include "portaudioplayer.h"
#include "audioadapter.h"

namespace audio {

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int PlayCallback(const void*, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void* pData)
{
    PortAudioPlayer* player = (PortAudioPlayer*)pData;

    SAMPLE* wptr = (SAMPLE*)outputBuffer;
    if(player->audioData_.size() == 0) {
        memset(wptr, 0, sizeof(SAMPLE) * framesPerBuffer);
        return paContinue;
    }

#if 1
    PortAudioPlayer::AudioData top = player->audioData_.front();
    player->audioData_.pop();

    SAMPLE* inptr = (SAMPLE*)top.buffer;
    memcpy(wptr, inptr, sizeof(SAMPLE) * framesPerBuffer);
#endif

    if(player->GetViewer())
    {
        player->GetViewer()->ResetTime(top.timestamp);
    }

    return paContinue;
}

PortAudioPlayer::PortAudioPlayer() :
    viewer_(nullptr)
{

}

bool PortAudioPlayer::Initialize() {
    PaError err = paNoError;

    outputParameters_.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters_.device == paNoDevice)
       return false;

    outputParameters_.channelCount = 1;                     /* stereo output */
    outputParameters_.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters_.suggestedLatency = Pa_GetDeviceInfo(outputParameters_.device)->defaultLowOutputLatency;
    outputParameters_.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
       &stream_,
       nullptr, /* no input */
       &outputParameters_,
       SAMPLE_RATE,
       FRAMES_PER_BUFFER,
       paClipOff,      /* we won't output out of range samples so don't bother clipping them */
       PlayCallback,
       this);

    if (err != paNoError || stream_ == nullptr)
        return false;

    return true;
}

bool PortAudioPlayer::Start() {

    PaError err = paNoError;
    err = Pa_StartStream(stream_);
    if (err != paNoError)
        return false;

    return true;
}

bool PortAudioPlayer::Stop() {
    if(stream_ == nullptr)
        return false;

    PaError err = Pa_CloseStream(stream_);
    if (err != paNoError)
        return false;

    stream_ = nullptr;

    return true;
}

void PortAudioPlayer::AddData(uint8_t* buffer, size_t len, backend::TimePoint ts) {

    if(!stream_)
        return;

    uint8_t decoded[FRAMES_PER_BUFFER * 2];
    memcpy(decoded, buffer, len);
    auto size = speex_.Decompress(len, decoded);

    AudioData info;
    memcpy(info.buffer, decoded, size * 2);
    info.timestamp = ts;

    audioData_.push(info);

    if(audioData_.size() >= CLIP_BOUND) {
        size_t clip = audioData_.size() - CLIP_BOUND;
        for(size_t i = 0; i < clip; i++) {
            audioData_.pop();
        }
    }
}

void PortAudioPlayer::SetViewer(IVideoVidewer* viewer) {
    viewer_ = viewer;
}

IVideoVidewer* PortAudioPlayer::GetViewer() {
    return viewer_;
}
}
