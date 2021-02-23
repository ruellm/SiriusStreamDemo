#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "backend/backend.h"

extern void dispatchToMainThread(std::function<void()> callback);

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::AddToLog(const std::string& msg) {
    ui->listWidget->addItem(QString::fromStdString(msg));
}

void ChatDialog::SetIdentity(const QString& identity) {
    identity_ = identity;
}

void ChatDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);

    ui->listWidget->addItem("Youre now connected with " +
        identity_ + ", you can now start chatting");

    backend::SetMessageHandler(identity_.toStdString(), [=](const std::string& msg){
        dispatchToMainThread([=](){
            AddToLog(identity_.toStdString() + " : " + msg);
          });
    });
}


void ChatDialog::on_pushButton_clicked()
{
    auto message = ui->lineEdit->text();
    if(message.size() == 0)
        return;

    ui->listWidget->addItem("me : " + message);

    backend::SendMessage(identity_.toStdString(),
          message.toStdString());

    ui->lineEdit->setText("");
}
