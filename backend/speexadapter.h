#ifndef SPEEXADAPTER_H
#define SPEEXADAPTER_H

#include <stdint.h>

class SpeexAdapter
{
public:
    SpeexAdapter();
    ~SpeexAdapter();
    int Decompress(size_t len, uint8_t* buffer);
    int Compress(size_t len, uint8_t* buffer);

private:
    int _sessionId;
};

#endif // SPEEXADAPTER_H
