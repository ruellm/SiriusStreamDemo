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
    VideoCaptureWorker* capture_;
    std::unique_ptr<IVideoEncoder> encoder_;
};

#endif // STREAMERWORKER_H
