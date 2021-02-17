#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

    void SetIdentity(const QString& identity);

private:
    Ui::ChatDialog *ui;

    QString _identity;

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void AddToLog(const std::string& msg);
    void on_pushButton_clicked();
};

#endif // CHATDIALOG_H
