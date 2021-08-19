// connectserverwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

// You may need to build the project (run Qt uic code generator) to get "ui_connectServerWindow.h" resolved

#include "connectserverwindow.h"
#include "ui_connectServerWindow.h"


connectServerWindow::connectServerWindow(QWidget *parent) :
        QDialog(parent), ui(new Ui::connectServerWindow) {
    ui->setupUi(this);
    this->setWindowTitle("Connect to Server");

    QObject::connect(ui->connectButton, &QPushButton::clicked, this, &connectServerWindow::connectClicked);
    QObject::connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

connectServerWindow::~connectServerWindow() {
    delete ui;
}

void connectServerWindow::connectClicked() {
    emit this->tryConnect(ui->IpInput->text());
}

void connectServerWindow::setInfo(int state) {
    if (state == CONNECTED_CLIENT) this->accept();
}
