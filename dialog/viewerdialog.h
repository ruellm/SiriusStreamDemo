#ifndef VIEWERDIALOG_H
#define VIEWERDIALOG_H

#include <QDialog>
#include "backend/backend.h"
#include "worker/viewerworker.h"

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
    backend::VideoStreamId _videoStreamId;
    ViewerWorker* _worker;

protected:
    void showEvent(QShowEvent *ev);
    void reject() override;
};

#endif // VIEWERDIALOG_H
