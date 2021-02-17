#include "viewerworker.h"
#include "backend/colorconverter.h"

ViewerWorker::ViewerWorker(QObject *parent)
    : QThread { parent }
{
}

void ViewerWorker::run()
{
    _decoder.Initialize();

    while(true) {
        _waitMutex.lock();
        if(_frameList.size() == 0)
            _waitCond.wait(&_waitMutex);
        _waitMutex.unlock();

        _dataMutex.lock();
        auto frame = _frameList.front();
        _frameList.pop();
        _dataMutex.unlock();

        uint8_t* output = nullptr;
        size_t outSize = 0;
        int width = 0;
        int height = 0;
        bool ret = _decoder.MakeFrame(frame.bytes, frame.bufferSize,
                        &output, &outSize, &width, &height, (int)frame.type);

        if(ret) {

            uint8_t* rgb = new uint8_t[width * height * 3];
            colorconverter::Yuv420ProgPlanarToRgb(output, rgb, width, height);

            QImage image(rgb, width, height, QImage::Format_RGB888);
            _currentPixmap = QPixmap::fromImage(image);
            emit frameReady();

            delete[] rgb;
        }

        delete[] frame.bytes;
        if(output)
            delete[] output;

    }
}

QPixmap ViewerWorker::GetLatestPixmap() const {
    return _currentPixmap;
}

void ViewerWorker::AddToList(backend::MediaFrame frame) {
    _dataMutex.lock();
    _frameList.push(frame);
    _dataMutex.unlock();

    _waitCond.wakeOne();
}

void ViewerWorker::CleanUp() {   
    _decoder.Shutdown();
}
