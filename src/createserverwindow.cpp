// createserverwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include "createserverwindow.h"
#include "ui_createServerWindow.h"


createServerWindow::createServerWindow(QWidget *parent, const QString& ip) :
        QDialog(parent), ui(new Ui::createServerWindow) {
    ui->setupUi(this);
    ui->ipLabel->setText("Listening at " + ip);
    QObject::connect(ui->confirmButton, &QPushButton::clicked, this, &QDialog::accept);
    QObject::connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

createServerWindow::~createServerWindow() {
    delete ui;
}
