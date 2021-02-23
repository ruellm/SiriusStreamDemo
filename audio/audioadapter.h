#ifndef AUDIOADAPTER_H
#define AUDIOADAPTER_H
#include "portaudio/portaudio.h"

#define InitializeAudioSystem   Pa_Initialize();
#define ShutdownAudioSystem     Pa_Terminate();

#define SAMPLE_RATE  16000
#define FRAMES_PER_BUFFER 320

/* Select sample format. */
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)

// simple audio clipping
// we are using sample frame size of 320,
// our sampling rate is 16000 samples per second.
// We need to cache enough audio samples to avoid audio clicking
// and annoying sound before playing. For this demo, we will cache for 1
// second worth of audio:
// therefore 16000/320 = 50 frames of 320 samples
// Drop the audio before that to avoid audio delay
// this value can be reduced further but it is advisable that
// enough audio samples should be available before playing to avoid
// clicking and audio gap sound.
#define CLIP_BOUND  (SAMPLE_RATE/FRAMES_PER_BUFFER)

#endif // AUDIOADAPTER_H
