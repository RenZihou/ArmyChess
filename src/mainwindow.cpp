// mainwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <QHostInfo>

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "createserverwindow.h"
#include "connectserverwindow.h"


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("Army Chess");
    this->setFixedSize(static_cast<int>(1000 / scaleRatio), static_cast<int>(950 / scaleRatio));
    this->setFocusPolicy(Qt::NoFocus);
    QObject::connect(ui->actionCreate_Connection, &QAction::triggered, this, &MainWindow::createServer);
    QObject::connect(ui->actionConnect_to_Server, &QAction::triggered, this, &MainWindow::connectServer);

#ifdef CHEAT
    ui->cheatBar->setGeometry(static_cast<int>(640 / scaleRatio), static_cast<int>(850 / scaleRatio),
                              static_cast<int>(360 / scaleRatio), static_cast<int>(100 / scaleRatio));
    cheatLayout = new QGridLayout(ui->cheatBar);
    cheatCmd = new QLineEdit(ui->cheatBar);
    cheatLayout->addWidget(cheatCmd);
    QObject::connect(cheatCmd, &QLineEdit::returnPressed, this, &MainWindow::cheat);
#endif  // CHEAT
}

MainWindow::~MainWindow() {
    delete ui;
    delete server;
    delete client;
    delete socket;
}

void MainWindow::createServer() {
    // TODO: server already created?
    // create server
    server = new QTcpServer(this);
    QObject::connect(server, &QTcpServer::newConnection, this, &MainWindow::connectionEstablished);

    // get local ip
    QString hostname = QHostInfo::localHostName();
    QHostInfo host_info = QHostInfo::fromName(hostname);
    QString ip;
    QHostAddress host;
    QList<QHostAddress> add_list = host_info.addresses();
    for (const auto& a : add_list) {
        if (a.protocol() == QAbstractSocket::IPv4Protocol) {
            ip = a.toString();
            host = a;
            break;
        }
    }
    server->listen(host, 4738);  // TODO: set port

    auto win = new createServerWindow(this, ip);
    win->show();
}

void MainWindow::connectServer() {
    auto win = new connectServerWindow(this);
    QObject::connect(win, &connectServerWindow::tryConnect, this, &MainWindow::tryConnect);
    win->show();
}

bool MainWindow::tryConnect(const QString &ip) {
    client = new QTcpSocket(this);
    client->connectToHost(ip, 4738);
    return true;
}

void MainWindow::connectionEstablished() {
    socket = server->nextPendingConnection();
    qDebug() << "connection established";
}

#ifdef CHEAT

void MainWindow::cheat() {
    QString cmd = cheatCmd->text();
    cheatCmd->clear();
    ui->board->exec(cmd);
}

#endif // CHEAT

