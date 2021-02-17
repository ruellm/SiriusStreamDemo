#pragma once

#include "ivideoencoder.h"

class X264VideoEncoder : public IVideoEncoder
{
public:
    X264VideoEncoder();
    virtual ~X264VideoEncoder();

    bool Initialize() override;
    void Shutdown() override;

    bool Encode(uint8_t* input, size_t len,
                uint8_t** output, size_t* outLen,
                int* type) override;
private:
    int _sessionId;
};

