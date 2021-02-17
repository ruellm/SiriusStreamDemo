
#include <QMessageBox>
#include "streamerdialog.h"
#include "ui_streamerdialog.h"
#include "demo_constants.h"

StreamerDialog::StreamerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StreamerDialog)
{
    ui->setupUi(this);

    _openCV =  new VideoCaptureWorker(this);

    connect(_openCV, &VideoCaptureWorker::newPixmapCaptured, this, [&]()
    {
        ui->label->setPixmap(_openCV->pixmap().scaled(demo::IMAGE_WIDTH, demo::IMAGE_HEIGHT));
    });

    _streamer = new StreamerWorker(this);
    _streamer->SetVideoCapture(_openCV);
}

StreamerDialog::~StreamerDialog()
{
    delete ui;

    _streamer->terminate();
    delete _streamer;

    _openCV->terminate();
    delete _openCV;

}

void StreamerDialog::on_pushButton_clicked()
{
    _streamer->start(QThread::HighestPriority);
    _openCV->start(QThread::HighestPriority);

}

void StreamerDialog::on_pushButton_2_clicked()
{
    _openCV->terminate();
    _streamer->terminate();
}

void StreamerDialog::reject()
{
    _openCV->terminate();
    _openCV->CloseCamera();

    _streamer->terminate();

     QDialog::reject();
}
