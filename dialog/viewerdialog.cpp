#include <QMessageBox>
#include "viewerdialog.h"
#include "ui_viewerdialog.h"
#include "dialog/inputdialog.h"
#include "demo_constants.h"

extern void dispatchToMainThread(std::function<void()> callback);

ViewerDialog::ViewerDialog(QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::ViewerDialog)
    , _worker(nullptr)
{
    ui->setupUi(this);
    _worker = new ViewerWorker();

    connect(_worker, &ViewerWorker::frameReady, this, [&]()
    {
        auto pixmap = _worker->GetLatestPixmap();
        if(pixmap.isNull())
            return;

        ui->label->setPixmap(_worker->GetLatestPixmap().scaled(demo::IMAGE_WIDTH,
                                                               demo::IMAGE_HEIGHT));
    });
}

ViewerDialog::~ViewerDialog()
{
    delete ui;
    delete _worker;
}

void ViewerDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);

    InputDialog input;
    input.SetWindowLabel("Enter Viewer Stream ID : ");
    input.setModal(true);
    input.exec();

    QString identity = input.GetInput();
    if(identity.size() == 0) {
       QMessageBox::information(this, "Error", "Viewer Stream ID must not be empty!");
       return;
    }

    backend::CreateViewerStream(identity.toStdString(), [=](backend::VideoStreamId id){
        _videoStreamId = id;
        _worker->start(QThread::HighestPriority);
    },
    [=](backend::MediaFrame frame) {
        // pass to viewer thread for decoding
        _worker->AddToList(frame);

        // viewer thread decodes and emits signal back to this dialog for display
    },
    [=](std::string error){
        QMessageBox::critical(this, "Error", QString::fromStdString(error));
    });
}


void ViewerDialog::reject()
{
    _worker->CleanUp();

    QDialog::reject();
}

