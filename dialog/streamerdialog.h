#ifndef STREAMERDIALOG_H
#define STREAMERDIALOG_H

#include <QDialog>

#include "worker/videocaptureworker.h"
#include "worker/streamerworker.h"

namespace Ui {
class StreamerDialog;
}

class StreamerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StreamerDialog(QWidget *parent = nullptr);
    ~StreamerDialog();

    void reject() override;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::StreamerDialog *ui;
    VideoCaptureWorker* _openCV;
    StreamerWorker* _streamer;
};

#endif // STREAMERDIALOG_H
