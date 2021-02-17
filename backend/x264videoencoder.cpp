#include <stdio.h>
#include <string.h>
#include "x264videoencoder.h"
#include "x264/x264_adapter.h"
#include "demo_constants.h"

X264VideoEncoder::X264VideoEncoder()
    : _sessionId(0){
}

X264VideoEncoder::~X264VideoEncoder() {
    x264::Shutdown();
}


bool X264VideoEncoder::Initialize() {
    _sessionId = x264::CreateEncoder();
    return x264::InitializeEncoder(_sessionId,
                                   demo::IMAGE_WIDTH,
                                   demo::IMAGE_HEIGHT,
                                   demo::BIT_RATE,
                                   demo::FRAME_RATE);
}

void X264VideoEncoder::Shutdown() {
    if(_sessionId) {
        x264::DestroyEncoder(_sessionId);
        _sessionId = 0;
    }
}

bool X264VideoEncoder::Encode(uint8_t* input, size_t,
                              uint8_t** output, size_t* outLen, int* type) {

    unsigned char out[500000];
    *outLen = x264::EncoderProcessFrame(_sessionId, input, out, type);

    *output = new unsigned char[*outLen];
    memcpy(*output, out, *outLen);

    return true;
}
