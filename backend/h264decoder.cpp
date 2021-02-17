#include "h264decoder.h"
#include "h264/h264Adapter.h"
#include "backend/backend.h"

H264Decoder::H264Decoder()
    : _sessionId(0)
    , _firstH264FrameSent(false)
    , _workArea(0)
    , _intendedWidth(0)
    , _intendedHeight(0)
{

}

H264Decoder::~H264Decoder() {
    Shutdown();
}

void H264Decoder::Initialize() {
    _sessionId = h264libAdapter::CreateViewer();

    _workArea = new uint8_t[1920 * 1080 * 3];
}

void H264Decoder::Shutdown() {
    if(_sessionId) {
        h264libAdapter::DestroyViewerInstance(_sessionId);
        _sessionId = 0;
    }

    if(_workArea) {
        delete[] _workArea;
        _workArea = 0;
    }
}


bool H264Decoder::MakeFrame(uint8_t* input, size_t len,
               uint8_t** output, size_t* outLen,
               int* width, int* height, int type) {

    int processingFrameType = 0;
    bool ret = false;

    switch(type) {
       case 2 :    //FrameType.VIDEO_I:
           break;
       case 1:     //FrameType.VIDEO_IDR:
           processingFrameType = 0; // IDR Frame
           break;
       case 4:     //FrameType.VIDEO_B:
       case 3:     //FrameType.VIDEO_P:
           processingFrameType = 2; // intermediate frame
           break;
       default:
           processingFrameType = 3; // unknown Frame
           return false;
    }

    if(!_firstH264FrameSent) {

        ret = h264libAdapter::ProcessFirstH264Frame(_sessionId,
            input, len, NULL, 0, &_intendedWidth, &_intendedHeight, 1);

        // Could not read the SPS so could not get the dimensions of the IDR Frame.
        // Severe problem and requires a bugout.
        if(!ret) {
            return false;
        }

        ret = h264libAdapter::ProcessFirstH264Frame(_sessionId, input, len,
                _workArea, 0, &_intendedWidth, &_intendedHeight, 1);

        if(!ret) {
            return false;
        }

        _firstH264FrameSent = true;

    } else {
       ret = h264libAdapter::ProcessH264Frame(_sessionId, input, len, _workArea, 1);

       if(ret && processingFrameType == 0 ) { // *IDR Frame
           int sw = h264libAdapter::GetH264Width(_sessionId);
           int sh = h264libAdapter::GetH264Height(_sessionId);

           // resolution change detection
           if (sw != _intendedWidth || sh != _intendedHeight)
           {
               _intendedWidth = sw;
               _intendedHeight = sh;

               // set this.firstH264FrameSent to false here and do recursive call
               // to reinit decoder with decoderMain->processFirstH264Frame
               _firstH264FrameSent = false;

               return MakeFrame(input, len, output, outLen, width, height, type);
           }
       }
    }

    if(ret) {
        int imageSize = _intendedWidth * _intendedHeight * 3 / 2;

        *output = new unsigned char[imageSize];
        memcpy(*output, _workArea, imageSize);

        *width = _intendedWidth;
        *height = _intendedHeight;
        *outLen = imageSize;
    }

    return true;
}

