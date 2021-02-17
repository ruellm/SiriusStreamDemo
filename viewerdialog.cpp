#include "viewerdialog.h"
#include "ui_viewerdialog.h"

ViewerDialog::ViewerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewerDialog)
{
    ui->setupUi(this);
}

ViewerDialog::~ViewerDialog()
{
    delete ui;
}
