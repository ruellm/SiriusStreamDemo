#include "inputdialog.h"
#include "ui_inputdialog.h"

InputDialog::InputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
}

InputDialog::~InputDialog()
{
    delete ui;
}

void InputDialog::SetWindowLabel(const QString& message) {
    ui->label->setText(message);
}

void InputDialog::on_pushButton_clicked()
{
    input = ui->lineEdit->text();
    close();
}

QString InputDialog::GetInput() {
    return input;
}
