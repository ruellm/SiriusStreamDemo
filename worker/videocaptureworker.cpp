#include <QDebug>
#include "videocaptureworker.h"
#include "demo_constants.h"

using namespace cv;

VideoCaptureWorker::VideoCaptureWorker(QObject *parent)
    : QThread { parent }
    , mVideoCap { ID_CAMERA }
{
    // set resolution
   mVideoCap.set(CAP_PROP_FRAME_WIDTH, demo::IMAGE_WIDTH);
   mVideoCap.set(CAP_PROP_FRAME_HEIGHT, demo::IMAGE_HEIGHT);
}

void VideoCaptureWorker::run()
{
    if (mVideoCap.isOpened())
    {
        while (true)
        {
            mVideoCap >> mFrame;
            if (!mFrame.empty())
            {
                mPixmap = cvMatToQPixmap(mFrame);
                emit newPixmapCaptured();
            }
        }
    }
}

void VideoCaptureWorker::CloseCamera() {
    if (mVideoCap.isOpened())
    {
        mVideoCap.release();
    }
}

QImage VideoCaptureWorker::cvMatToQImage(const cv::Mat &inMat)
{
    switch (inMat.type())
    {
        // 8-bit, 4 channel
        case CV_8UC4:
        {
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_ARGB32);
            return image;
        }
        // 8-bit, 3 channel
        case CV_8UC3:
        {
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        // 8-bit, 1 channel
        case CV_8UC1:
        {
    #if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_Grayscale8);
    #else
            static QVector<QRgb>  sColorTable;

            // only create our color table the first time
            if (sColorTable.isEmpty())
            {
                sColorTable.resize(256 );

                for (int i = 0; i < 256; ++i )
                {
                    sColorTable[i] = qRgb(i, i, i );
                }
            }

            QImage image(inMat.data,
                         inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step),
                         QImage::Format_Indexed8 );

            image.setColorTable(sColorTable );
    #endif
            return image;
        }
        default:
        {
            qWarning()<< "ASM::cvMatToQImage()- cv::Mat image type not handled in switch:" << inMat.type();
            break;
        }
    }
    return QImage();
}

QPixmap VideoCaptureWorker::cvMatToQPixmap(const cv::Mat &inMat)
{
    return QPixmap::fromImage(cvMatToQImage(inMat));
}

uint8_t* VideoCaptureWorker::GetCurrentFrameBuffer(size_t* len) {

    // convert frame to YUV420
    if(mFrame.empty())
        return nullptr;

    // create a clone incase we need to resize
    cv::Mat dst = mFrame.clone();

    // perform resize first
    if( mFrame.size().width != demo::IMAGE_WIDTH &&  mFrame.size().height != demo::IMAGE_HEIGHT) {
        Size size(demo::IMAGE_WIDTH, demo::IMAGE_HEIGHT);
        resize(mFrame, dst,size);//resize image
    }

    // convert RGB image to gray
    cv::cvtColor(dst, mYuv420, cv::COLOR_BGRA2YUV_I420);

    *len = mYuv420.rows * mYuv420.cols * mYuv420.elemSize1();
    return mYuv420.data;
}
