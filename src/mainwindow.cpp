// mainwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <QHostInfo>

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "createserverwindow.h"
#include "connectserverwindow.h"

#define PORT 4738


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("Army Chess");
    this->setFixedSize(static_cast<int>(1000 / scaleRatio), static_cast<int>(940 / scaleRatio));
    this->setFocusPolicy(Qt::NoFocus);
    ui->connectLabel->setGeometry(static_cast<int>(670 / scaleRatio), static_cast<int>(10 / scaleRatio),
                                  static_cast<int>(300 / scaleRatio), static_cast<int>(60 / scaleRatio));
    QObject::connect(ui->actionCreate_Connection, &QAction::triggered, this, &MainWindow::createServer);
    QObject::connect(ui->actionConnect_to_Server, &QAction::triggered, this, &MainWindow::connectServer);

#ifdef CHEAT
    ui->cheatBar->setGeometry(static_cast<int>(640 / scaleRatio), static_cast<int>(840 / scaleRatio),
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
    delete socket;
}

void MainWindow::createServer() {
    // TODO: server already created?
    // get local ip
    QString hostname = QHostInfo::localHostName();
    QHostInfo host_info = QHostInfo::fromName(hostname);
    QString ip;
    QHostAddress host;
    QList<QHostAddress> add_list = host_info.addresses();
    for (const auto &a : add_list) {
        if (a.protocol() == QAbstractSocket::IPv4Protocol) {
            ip = a.toString();
            host = a;
            break;
        }
    }
    if (state == DISCONNECTED) {
        // create server
        server = new QTcpServer(this);
        QObject::connect(server, &QTcpServer::newConnection, this, &MainWindow::connectionEstablished);

        //    server->listen(host, PORT);  // TODO: set port & disable localhost
        server->listen(QHostAddress::LocalHost, PORT);
    }
    this->setStatus(LISTENING);
    auto win = new createServerWindow(this, ip, state);
    win->show();
    QObject::connect(win, &createServerWindow::rejected, this, &MainWindow::stopServer);
    QObject::connect(this, &MainWindow::statusChanged, win, &createServerWindow::setInfo);

    qDebug() << "server created";
}

void MainWindow::connectServer() {
    auto win = new connectServerWindow(this);
    QObject::connect(win, &connectServerWindow::tryConnect, this, &MainWindow::tryConnect);
    QObject::connect(win, &connectServerWindow::rejected, this, &MainWindow::stopClient);
    QObject::connect(this, &MainWindow::statusChanged, win, &connectServerWindow::setInfo);
    win->show();
}

bool MainWindow::tryConnect(const QString &ip) {
    socket = new QTcpSocket(this);
//    client->connectToHost(ip, PORT);  // TODO: disable localhost
    socket->connectToHost(QHostAddress::LocalHost, PORT);
    QObject::connect(socket, &QTcpSocket::connected, this, &MainWindow::connectionEstablished);
    return true;
}

void MainWindow::stopServer() {
    socket->disconnectFromHost();
    server->close();
    socket = nullptr;
    server = nullptr;
    this->setStatus(DISCONNECTED);
    qDebug() << "server stopped";
}

void MainWindow::stopClient() {
    if (socket != nullptr) socket->disconnectFromHost();
    socket = nullptr;
    this->setStatus(DISCONNECTED);
    qDebug() << "client stopped";
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
        this->setStatus(CONNECTED_SERVER);
    } else {
        this->setStatus(CONNECTED_CLIENT);
    }
    QObject::connect(socket, &QTcpSocket::readyRead, this, &MainWindow::receive);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &MainWindow::connectionInterrupted);
    qDebug() << "connection established";
}

void MainWindow::connectionInterrupted() {
//    if (server != nullptr) {
//        this->setStatus(LISTENING);
//    } else {
//        this->setStatus(DISCONNECTED);
//    }
    this->setStatus(server == nullptr ? DISCONNECTED : LISTENING);
    socket = nullptr;
    qDebug() << "connection interrupted";
}

void MainWindow::send(const QString &cmd) {
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

void MainWindow::setStatus(int new_state) {
    state = new_state;
    QString text;
    switch (new_state) {
        case DISCONNECTED:
            text = "disconnected";
            break;
        case LISTENING:
            text = "listening";
            break;
        case CONNECTED_SERVER:
            text = "connected (server)";
            break;
        case CONNECTED_CLIENT:
            text = "connected (client)";
        default:
            break;
    }
    ui->connectLabel->setText(QString("[%1]").arg(text));
    ui->actionCreate_Connection->setEnabled(new_state != CONNECTED_CLIENT);
    ui->actionConnect_to_Server->setEnabled(new_state == DISCONNECTED || new_state == CONNECTED_CLIENT);
    emit statusChanged(new_state);
}

#ifdef CHEAT

void MainWindow::cheat() {
    QString cmd = cheatCmd->text();
    cheatCmd->clear();
    ui->board->exec(cmd);
}

#endif // CHEAT

