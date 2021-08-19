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
//    delete client;
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
//    server->listen(host, 4738);  // TODO: set port & disable localhost
    server->listen(QHostAddress::LocalHost, 4738);

    auto win = new createServerWindow(this, ip);
    win->show();
    QObject::connect(win, &connectServerWindow::rejected, this, &MainWindow::stopListen);

    qDebug() << "server created";
}

void MainWindow::connectServer() {
    auto win = new connectServerWindow(this);
    QObject::connect(win, &connectServerWindow::tryConnect, this, &MainWindow::tryConnect);
    win->show();
}

bool MainWindow::tryConnect(const QString &ip) {
    socket = new QTcpSocket(this);
//    client->connectToHost(ip, 4738);  // TODO: disable localhost
    socket->connectToHost(QHostAddress::LocalHost, 4738);
    QObject::connect(socket, &QTcpSocket::connected, this, &MainWindow::connectionEstablished);
    return true;
}

void MainWindow::stopListen() {
    server->close();
    qDebug() << "listening stopped";
}

void MainWindow::connectionEstablished() {
    if (server != nullptr) {
        socket = server->nextPendingConnection();
        delete ui->board;
        long long seed = std::time(nullptr);
        ui->board = new Board(ui->centralwidget, seed);
        ui->board->show();
        QObject::connect(ui->board, &Board::stepProceeded, this, &MainWindow::send);
        this->send(QString("seed %1").arg(seed));
    }
    QObject::connect(socket, &QTcpSocket::readyRead, this, &MainWindow::receive);
    qDebug() << "connection established";
}

void MainWindow::send(const QString& cmd) {
    if (socket == nullptr) return;
    socket->write(cmd.toUtf8());
    qDebug() << "[sent]" << cmd;
}

void MainWindow::receive() {
    QString cmd = socket->readLine();
    qDebug() << "[received]" << cmd;
    if (cmd.startsWith("seed")) {
        long long seed = cmd.remove("seed ").toLongLong();
        delete ui->board;
        ui->board = new Board(ui->centralwidget, seed);
        ui->board->show();
        QObject::connect(ui->board, &Board::stepProceeded, this, &MainWindow::send);
    } else {
        ui->board->exec(cmd, false);
    }
}

#ifdef CHEAT
void MainWindow::cheat() {
    QString cmd = cheatCmd->text();
    cheatCmd->clear();
    ui->board->exec(cmd);
}
#endif // CHEAT

