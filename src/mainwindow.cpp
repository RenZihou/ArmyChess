// mainwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <random>

#include <QHostInfo>
#include <QMessageBox>

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
    ui->sideLabel->setGeometry(static_cast<int>(670 / scaleRatio), static_cast<int>(70 / scaleRatio),
                               static_cast<int>(300 / scaleRatio), static_cast<int>(60 / scaleRatio));
    ui->turnLabel->setGeometry(static_cast<int>(670 / scaleRatio), static_cast<int>(130 / scaleRatio),
                               static_cast<int>(300 / scaleRatio), static_cast<int>(60 / scaleRatio));
    ui->timeLabel->setGeometry(static_cast<int>(670 / scaleRatio), static_cast<int>(190 / scaleRatio),
                               static_cast<int>(300 / scaleRatio), static_cast<int>(60 / scaleRatio));
    ui->actionStart->setEnabled(false);
    ui->actionAdmit_Defeat->setEnabled(false);
    QObject::connect(ui->actionCreate_Connection, &QAction::triggered, this, &MainWindow::createServer);
    QObject::connect(ui->actionConnect_to_Server, &QAction::triggered, this, &MainWindow::connectServer);
    QObject::connect(ui->actionStart, &QAction::triggered, this, &MainWindow::start);
    QObject::connect(ui->actionAdmit_Defeat, &QAction::triggered, this, &MainWindow::admitDefeat);

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

void MainWindow::changeSide(int side) {
    ui->sideLabel->setText(QString("Your Side: [%1]").arg(side == RED ? "red" : "blue"));
    // TODO: support UNKNOWN
}

void MainWindow::createServer() {
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
    if (state == DISCONNECTED && server == nullptr) {
        // create server
        server = new QTcpServer(this);
        QObject::connect(server, &QTcpServer::newConnection, this, &MainWindow::connectionEstablished);

        //    server->listen(host, PORT);  // TODO: set port & disable localhost
        server->listen(QHostAddress::LocalHost, PORT);
        this->setStatus(LISTENING);
    }
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
        this->connectBoard();
        this->send(QString("seed %1").arg(seed));
        this->setStatus(CONNECTED_SERVER);
    } else {
        this->setStatus(CONNECTED_CLIENT);
    }
    ui->actionStart->setEnabled(true);
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
    socket->write(cmd.toUtf8().append("\n"));
    if (cmd == "finish") this->setTurn("Current Player: [opponent]");
    else if (cmd == "win") this->win();
    else if (cmd == "lose") this->win(false);
    qDebug() << "[sent]" << cmd;
}

void MainWindow::receive() {
    while (socket->canReadLine()) {
        QString cmd = socket->readLine();
        qDebug() << "[received]" << cmd;
        if (cmd.startsWith("seed")) {
            long long seed = cmd.remove("seed ").toLongLong();
            delete ui->board;
            ui->board = new Board(ui->centralwidget, seed);
            ui->board->show();
            this->connectBoard();
        } else if (cmd.startsWith("start")) {
            opponent_started = true;
            int turn = cmd.remove("start ").toInt();
            switch (turn) {
                case 0:
                    ui->board->flipTurn();
                    ui->board->resetTimer();
                    this->setTurn("Current Player: [you]");
                    break;
                case 1:
                    ui->board->resetTimer();
                    this->setTurn("Current Player: [opponent]");
                    break;
                default:
                    break;
            }
        } else {
            if (cmd.startsWith("finish")) this->setTurn("Current Player: [you]");
            else if (cmd.startsWith("win")) this->win(false);
            else if (cmd.startsWith("lose")) this->win();
            ui->board->exec(cmd, false);
        }
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

void MainWindow::setTurn(const QString &text) {
    ui->turnLabel->setText(text);
}

void MainWindow::setTime(int time) {
    ui->timeLabel->setText(QString("Time Left: [%1]").arg(time));
}

void MainWindow::connectBoard() {
    QObject::connect(ui->board, &Board::stepProceeded, this, &MainWindow::send);
    QObject::connect(ui->board, &Board::sideChanged, this, &MainWindow::changeSide);
    QObject::connect(ui->board, &Board::timeChanged, this, &MainWindow::setTime);
    QObject::connect(ui->board, &Board::canAdmitDefeat, this, &MainWindow::enableAdmitDefeat);
}

void MainWindow::start() {
    int turn = -1;
    if (opponent_started) {
        std::mt19937 mt(std::time(nullptr));
        std::uniform_real_distribution<double> dist(0, 1);
        turn = (dist(mt) > 0.5);
        if (turn) {
            ui->board->flipTurn();
            this->setTurn("Current Player: [you]");
        } else this->setTurn("Current Player: [opponent]");
        ui->board->resetTimer();
    } else this->setTurn("[waiting opponent]");
    this->send(QString("start %1").arg(turn));
    ui->actionStart->setEnabled(false);
}

void MainWindow::enableAdmitDefeat() { ui->actionAdmit_Defeat->setEnabled(true); }

void MainWindow::admitDefeat() {
    this->win(false);
    ui->board->exec("lose");
}

void MainWindow::win(bool w /* = true */) {
    auto msg = new QMessageBox(this);
    msg->setWindowTitle("Game Over");
    msg->setText(w ? "You Win :)" : "You Lose :(");
    msg->setStandardButtons(QMessageBox::Ok);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->show();
}

#ifdef CHEAT

void MainWindow::cheat() {
    QString cmd = cheatCmd->text();
    cheatCmd->clear();
    ui->board->exec(cmd);
}

#endif // CHEAT

