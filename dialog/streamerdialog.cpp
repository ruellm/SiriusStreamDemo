
#include <QMessageBox>
#include "streamerdialog.h"
#include "ui_streamerdialog.h"
#include "demo_constants.h"
#include "worker/audiocaptureworker.h"

StreamerDialog::StreamerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StreamerDialog)
{
    ui->setupUi(this);

    openCV_ =  new VideoCaptureWorker(this);

    connect(openCV_, &VideoCaptureWorker::newPixmapCaptured, this, [&]()
    {
        ui->label->setPixmap(openCV_->pixmap().scaled(demo::IMAGE_WIDTH, demo::IMAGE_HEIGHT,
                                                      Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    streamer_ = new StreamerWorker(this);
    streamer_->SetVideoCapture(openCV_);

    audioCapture_.Initialize();
    audioCapture_.SetProcessor(&worker_);
}

StreamerDialog::~StreamerDialog()
{
    delete ui;

    CleanupVideo();
    CleanupAudio();
}

void StreamerDialog::on_pushButton_clicked()
{
    streamer_->start(QThread::HighestPriority);
    openCV_->start(QThread::HighestPriority);
}

void StreamerDialog::on_pushButton_2_clicked()
{
    openCV_->terminate();
    streamer_->terminate();
}

void StreamerDialog::reject()
{
    CleanupVideo();
    CleanupAudio();

    QDialog::reject();
}

void StreamerDialog::on_pushButton_3_clicked()
{
    if(ui->pushButton_3->text() == "Start Audio") {

        audioCapture_.Start();
        worker_.start(QThread::HighestPriority);

        ui->pushButton_3->setText("Stop Audio");

    } else {

        CleanupAudio();

        ui->pushButton_3->setText("Start Audio");
    }
}

void StreamerDialog::CleanupVideo() {

    if(streamer_) {
        streamer_->terminate();
        delete streamer_;
        streamer_ = nullptr;
    }

    if(openCV_) {
        openCV_->terminate();
        openCV_->CloseCamera();
        delete openCV_;
        openCV_ = nullptr;
    }
}

void StreamerDialog::CleanupAudio() {

    audioCapture_.Stop();
    worker_.Stop();

}
