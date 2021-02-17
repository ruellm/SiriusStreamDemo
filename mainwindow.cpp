#include <QMessageBox>
#include <QTimer>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "backend/backend.h"
#include "dialog/inputdialog.h"

void dispatchToMainThread(std::function<void()> callback)
{
    // any thread
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);

    QObject::connect(timer, &QTimer::timeout, [=]()
    {
        // main thread
        callback();
        timer->deleteLater();
    });

    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _logged(false)
{
    ui->setupUi(this);

    backend::SetOnChannelInvite(std::bind(&MainWindow::OnChannelInvite, this, std::placeholders::_1));

    backend::SetOnChannelCreated(std::bind(&MainWindow::OnChannelCreated, this, std::placeholders::_1));

    backend::SetOnChannelCreationError(std::bind(&MainWindow::OnChannelCreationError, this, std::placeholders::_1));

    backend::Initialize([=](){
        ui->listWidget->addItem("Application Ready");
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddToLog(const std::string& msg) {
    ui->listWidget->addItem(QString::fromStdString(msg));
}

void MainWindow::on_registerBtn_clicked()
{
    AddToLog("Registration ...");

    backend::Register(
                [=](const std::string& identity){
        AddToLog("Registration succesful :");
        AddToLog(identity);
    },
    [=](std::string msg){
        AddToLog("ERROR : " + msg );
    });
}

void MainWindow::on_loginBtn_clicked()
{
    InputDialog input;
    input.SetWindowLabel("Enter Identity Id: \n\rEmpty = use last registered user");
    input.setModal(true);
    input.exec();

    QString identity = input.GetInput();

    ui->listWidget->addItem("Announcing Presence of " + identity + " ...");

    backend::Login(
                identity.toStdString(),

                [=](const std::string& key){
        AddToLog("Login succesful with Presence Key : ");
        AddToLog(key);

        _logged = true;
    },

    [=](std::string msg){
        AddToLog("ERROR : " + msg );
    });
}

void MainWindow::on_createChannelBtn_clicked()
{
    InputDialog input;
    input.SetWindowLabel("Enter User Presence Id: ");
    input.setModal(true);
    input.exec();

    QString identity = input.GetInput();
    if(identity.size() == 0) {
         QMessageBox::information(this, "Error", "User Presence ID must not be empty!");
        return;
    }

    backend::CreateChannel(identity.toStdString());
}


void MainWindow::OnChannelInvite(const std::string& identity) {

    dispatchToMainThread([=](){
        QString message = "User with Presence Key " + QString::fromStdString(identity) +
                " is inviting you to create channel, Approve?";

        QMessageBox::StandardButton reply
                = QMessageBox::question(this, "Confirm Channel",
                                        message,
                                        QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes) {
           backend::ConfirmChannel(identity);
        } else {
            backend::DenyChannel(identity);
        }
    });
}


void MainWindow::OnChannelCreated(const std::string& identity) {

    dispatchToMainThread([=](){
        AddToLog("Channel created with " + identity);

        auto chat = std::make_shared<ChatDialog>();
        chat->SetIdentity(QString::fromStdString(identity));
        chat->setModal(false);
        chat->show();

        _chatDialog.push_back(std::move(chat));
    });
}

void MainWindow::OnChannelCreationError(const std::string& msg) {
    dispatchToMainThread([=](){
        AddToLog("Channel Creation Error :" + msg);
    });
}

void MainWindow::on_createVideoStreamBtn_clicked()
{
    AddToLog("Creating broadcast stream... ");

    backend::CreateBroadcastStream([=](backend::VideoStreamId id){
        dispatchToMainThread([=](){
            _streamerDialog.reset(new StreamerDialog());
            _streamerDialog->setModal(false);
            _streamerDialog->show();

            AddToLog("... Succesful! ");
            AddToLog(id);
        });
    },
    [=](std::string message){
        dispatchToMainThread([=](){
            AddToLog(message);
        });
    });
}

void MainWindow::on_viewVideoStreamBtn_clicked()
{
    auto view = std::make_shared<ViewerDialog>();
    view->setModal(false);
    view->show();

    _viewerDialog.push_back(std::move(view));
}
