#ifndef VIEWERDIALOG_H
#define VIEWERDIALOG_H

#include <QDialog>
#include "backend/backend.h"
#include "worker/videoviewermanager.h"
#include "audio/portaudioplayer.h"

namespace Ui {
class ViewerDialog;
}

class ViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewerDialog(QWidget *parent = nullptr);
    ~ViewerDialog();

private:
    Ui::ViewerDialog *ui;
    VideoViewerManager viewerManager_;
    audio::PortAudioPlayer player_;

protected:
    void showEvent(QShowEvent *ev);
    void reject() override;
private slots:
    void on_pushButton_clicked();
};

#endif // VIEWERDIALOG_H
