#ifndef VIEWERWORKER_H
#define VIEWERWORKER_H

#include <QThread>
#include<QMutex>
#include <QWaitCondition>
#include <QPixmap>
#include <QImage>
#include <queue>
#include "backend/backend.h"
#include "backend/h264decoder.h"

class ViewerWorker : public QThread
{
    Q_OBJECT

public:
    ViewerWorker(QObject *parent = nullptr);

    void AddToList(backend::MediaFrame frame);
    void CleanUp();
    QPixmap GetLatestPixmap() const;

protected:
   void run() override;

signals:
   void frameReady(); // new pixmap image catured

private:
    std::queue<backend::MediaFrame> _frameList;
    QMutex _waitMutex;
    QMutex _dataMutex;
    QWaitCondition _waitCond;
    H264Decoder _decoder;
    QPixmap _currentPixmap;
};

#endif // VIEWERWORKER_H
