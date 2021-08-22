// connectserverwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <QMessageBox>

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
    auto server_add = QHostAddress(ui->IpInput->text());
    if (server_add.protocol() == QAbstractSocket::IPv4Protocol)
        emit this->tryConnect(ui->IpInput->text());
    else {
        QMessageBox::warning(this, "Invalid ip",
                             "This is an invalid ipv4 address. Check and input again.");
        ui->IpInput->clear();
    }
}

void connectServerWindow::setInfo(int state) {
    if (state == CONNECTED_CLIENT) this->accept();
}
