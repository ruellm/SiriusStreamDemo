#pragma once
#include <stdint.h>

class IVideoEncoder {
public:
    virtual ~IVideoEncoder(){}
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    virtual bool Encode(uint8_t* input, size_t len,
                        uint8_t** output, size_t* outLen,
                        int* type) = 0;
};
