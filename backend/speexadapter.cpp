#include "speexadapter.h"
#include "speex/adapter.h"

SpeexAdapter::SpeexAdapter()
{
    _sessionId = libspeex_adapter::create(2);
}

SpeexAdapter::~SpeexAdapter() {
    libspeex_adapter::destroy(_sessionId);
}

int SpeexAdapter::Decompress(size_t len, uint8_t* buffer) {
    return libspeex_adapter::decompress(_sessionId, len, (char*)buffer);
}

int SpeexAdapter::Compress(size_t len, uint8_t* buffer) {
    return libspeex_adapter::compress(_sessionId, len,(char*) buffer);
}
