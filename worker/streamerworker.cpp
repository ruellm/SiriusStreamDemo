#include "streamerworker.h"
#include "backend/x264videoencoder.h"
#include "backend/backend.h"

StreamerWorker::StreamerWorker(QObject *parent)
    : QThread { parent }
    , _capture(nullptr)
{
    _encoder.reset(new X264VideoEncoder());
    _encoder->Initialize();
}

void StreamerWorker::run()
{
    while(true) {
        size_t len = 0;
        auto buffer = _capture->GetCurrentFrameBuffer(&len);
        if(buffer == nullptr)
            continue;

        uint8_t* output = nullptr;
        size_t outLen = 0;
        int type = 0;

        _encoder->Encode(buffer, len, &output, &outLen, &type);

        backend::SendVideoFrame(output, outLen, type);

        delete[] output;
    }
}

void StreamerWorker::SetVideoCapture(VideoCaptureWorker* worker) {
    _capture = worker;
}
