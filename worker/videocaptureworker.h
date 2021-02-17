#ifndef VIDEOCAPTUREWORKER_H
#define VIDEOCAPTUREWORKER_H

#include <stdint.h>
#include <QPixmap>
#include <QImage>
#include <QThread>
#include <opencv2/opencv.hpp>

#define ID_CAMERA 0

class VideoCaptureWorker : public QThread
{
    Q_OBJECT

public:
    VideoCaptureWorker(QObject *parent = nullptr);

    QPixmap pixmap() const
    {
       return mPixmap;
    }

   uint8_t* GetCurrentFrameBuffer(size_t* len);

   void CloseCamera();

signals:
   void newPixmapCaptured(); // new pixmap image catured

protected:
   void run() override;

private:
   QPixmap mPixmap;
   cv::Mat mFrame;
   cv::Mat mYuv420;
   cv::VideoCapture mVideoCap;

   QImage cvMatToQImage(const cv::Mat &inMat);
   QPixmap cvMatToQPixmap(const cv::Mat &inMat);
};

#endif // VIDEOCAPTUREWORKER_H
