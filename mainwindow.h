#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QScopedPointer>
#include <QBoxLayout>

#include "dialog/chatdialog.h"
#include "dialog/streamerdialog.h"
#include "dialog/viewerdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_registerBtn_clicked();

    void on_loginBtn_clicked();

    void on_createChannelBtn_clicked();

    void on_createVideoStreamBtn_clicked();

    void on_viewVideoStreamBtn_clicked();

private:
    Ui::MainWindow *ui;

    bool logged_;

    using ChatDialogPtr = std::shared_ptr<ChatDialog>;
    std::vector<ChatDialogPtr> chatDialog_;

    std::unique_ptr<StreamerDialog> streamerDialog_;

    using ViewerDialogPtr = std::shared_ptr<ViewerDialog>;
    std::vector<ViewerDialogPtr> viewerDialog_;

private:
    void AddToLog(const std::string& msg);

    void OnChannelInvite(const std::string& identity);

    void OnChannelCreated(const std::string& identity);

    void OnChannelCreationError(const std::string& identity);
};
#endif // MAINWINDOW_H
