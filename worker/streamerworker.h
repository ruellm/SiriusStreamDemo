#ifndef STREAMERWORKER_H
#define STREAMERWORKER_H

#include <QThread>
#include "videocaptureworker.h"
#include "backend/ivideoencoder.h"

class StreamerWorker : public QThread
{    
    Q_OBJECT

public:
    StreamerWorker(QObject *parent = nullptr);
    void SetVideoCapture(VideoCaptureWorker* worker);

protected:
   void run() override;

private:
    VideoCaptureWorker* _capture;
    std::unique_ptr<IVideoEncoder> _encoder;
};

#endif // STREAMERWORKER_H
