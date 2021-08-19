// createserverwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include "createserverwindow.h"
#include "ui_createServerWindow.h"

createServerWindow::createServerWindow(QWidget *parent, const QString &ip, int state) :
        QDialog(parent), ui(new Ui::createServerWindow) {
    ui->setupUi(this);
    ip_ = ip;
    this->setWindowTitle("Create Server");
    this->setInfo(state);
    QObject::connect(ui->confirmButton, &QPushButton::clicked, this, &QDialog::accept);
    QObject::connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

createServerWindow::~createServerWindow() {
    delete ui;
}

void createServerWindow::setInfo(int status) {
    if (status == LISTENING) ui->ipLabel->setText("Listening at " + ip_);
    else if (status == CONNECTED_SERVER)
        ui->ipLabel->setText(QString("Connected (%1)").arg(ip_));
}