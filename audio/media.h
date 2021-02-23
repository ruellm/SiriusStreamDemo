// ported from PSP middleware code
// for compatibility reasons with other demo applications
#ifndef __media_h
#define __media_h

#include <stdint.h>

namespace audio::psp
{

enum _AUDIO_CODEC_
{
    AC_GSM_, //! default
    AC_PCMU_,
    AC_PCMA_,
};

#define fProtocol 0x40000000

#define fCompGSM 32
#define fCompSPEEX 64
#define fCompSiren 128

#define BUFL 10200

#define nWaveHeaders 4
typedef enum States
{
    IDM_VOX_NONE,
    IDM_VOX_SLOW,
    IDM_VOX_MEDIUM,
    IDM_VOX_FAST,
} enum_voxmode;

typedef enum Protocols
{
    PROTOCOL_RTP
} enum_protocolsent;

struct soundbuf
{
    uint32_t compression;
    char sendinghost[16];
    struct
    {
        uint32_t buffer_len;
        char buffer_val[BUFL];
    } buffer;
};
typedef struct soundbuf soundbuf;

struct bufLenPair
{
    short len;
    soundbuf buf;
};
typedef struct bufLenPair bufLenPair;

#endif
}
