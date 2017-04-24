#include "disclaimerdialog.h"
#include "ui_disclaimerdialog.h"

DisclaimerDialog::DisclaimerDialog(QWidget *parent):QDialog(parent),ui(new Ui::DisclaimerDialog) {
    ui->setupUi(this);
    connect(ui->aboutQt,&QPushButton::clicked,QApplication::aboutQt);
}

DisclaimerDialog::~DisclaimerDialog() {
    delete ui;
}
