#include <QMessageBox>
#include "viewerdialog.h"
#include "ui_viewerdialog.h"
#include "dialog/inputdialog.h"
#include "demo_constants.h"

extern void dispatchToMainThread(std::function<void()> callback);

ViewerDialog::ViewerDialog(QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::ViewerDialog)
{
    ui->setupUi(this);

    connect(&viewerManager_, &VideoViewerManager::frameReady, this, [&](QPixmap pixmap)
    {
        if(pixmap.isNull())
            return;

        ui->label->setPixmap(pixmap.scaled(demo::IMAGE_WIDTH, demo::IMAGE_HEIGHT,
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    player_.Initialize();
    player_.SetViewer(&viewerManager_);
}

ViewerDialog::~ViewerDialog()
{
    delete ui;

    viewerManager_.Stop();

    player_.Stop();
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

    backend::CreateViewerStream(identity.toStdString(), [=](backend::VideoStreamId){
       viewerManager_.Start();
    },
    [=](backend::MediaFrame frame) {
        if(frame.type == backend::MediaFrame::Type::AUDIO) {
            player_.AddData(frame.bytes, frame.bufferSize, frame.timestamp);
        } else {
            viewerManager_.AddToList(frame);
        }
    },
    [=](std::string error){
        QMessageBox::critical(this, "Error", QString::fromStdString(error));
    });
}


void ViewerDialog::reject()
{
    viewerManager_.Stop();
    player_.Stop();
    QDialog::reject();
}


void ViewerDialog::on_pushButton_clicked()
{
    if(ui->pushButton->text()=="Start Audio") {
        ui->pushButton->setText("Stop Audio");

        player_.Start();
    }
    else{
        ui->pushButton->setText("Start Audio");
       player_.Stop();
    }
}
