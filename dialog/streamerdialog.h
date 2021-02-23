#ifndef STREAMERDIALOG_H
#define STREAMERDIALOG_H

#include <QDialog>

#include "worker/videocaptureworker.h"
#include "worker/streamerworker.h"
#include "audio/portaudiocapture.h"
#include "worker/audiocaptureworker.h"

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

    void CleanupVideo();
    void CleanupAudio();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::StreamerDialog *ui;
    VideoCaptureWorker* openCV_;
    StreamerWorker* streamer_;

    audio::PortAudioCapture audioCapture_;
    AudioCaptureWorker worker_;
};

#endif // STREAMERDIALOG_H
