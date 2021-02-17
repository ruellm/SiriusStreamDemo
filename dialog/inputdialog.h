#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>

namespace Ui {
class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = nullptr);
    ~InputDialog();

    void SetWindowLabel(const QString& message);
    QString GetInput();

private slots:
    void on_pushButton_clicked();

private:
    Ui::InputDialog *ui;
    QString input;
};

#endif // INPUTDIALOG_H
