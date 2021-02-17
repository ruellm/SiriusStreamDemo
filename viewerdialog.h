#ifndef VIEWERDIALOG_H
#define VIEWERDIALOG_H

#include <QDialog>

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
};

#endif // VIEWERDIALOG_H
