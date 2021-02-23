#include "streamerworker.h"
#include "backend/x264videoencoder.h"
#include "backend/backend.h"

StreamerWorker::StreamerWorker(QObject *parent)
    : QThread { parent }
    , capture_(nullptr)
{
    encoder_.reset(new X264VideoEncoder());
    encoder_->Initialize();
}

void StreamerWorker::run()
{
    while(true) {
        size_t len = 0;
        auto buffer = capture_->GetCurrentFrameBuffer(&len);
        if(buffer == nullptr)
            continue;

        uint8_t* output = nullptr;
        size_t outLen = 0;
        int type = 0;

        encoder_->Encode(buffer, len, &output, &outLen, &type);

        backend::SendVideoFrame(output, outLen, type);

        delete[] output;
    }
}

void StreamerWorker::SetVideoCapture(VideoCaptureWorker* worker) {
    capture_ = worker;
}
