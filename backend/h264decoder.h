#ifndef H264DECODER_H
#define H264DECODER_H
#include <stdint.h>

class H264Decoder
{
public:
    H264Decoder();
    ~H264Decoder();

    void Initialize();
    void Shutdown();

    bool MakeFrame(uint8_t* input, size_t,
                   uint8_t** output, size_t* outLen,
                   int* width, int* height, int type);
private:
    int _sessionId;
    bool _firstH264FrameSent;
    uint8_t* _workArea;
    int _intendedWidth;
    int _intendedHeight;
};

#endif // H264DECODER_H
