// mainwindow.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef ARMYCHESS_MAINWINDOW_H
#define ARMYCHESS_MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QTcpServer>
#include <QTcpSocket>

#define DISCONNECTED 0
#define LISTENING 1
#define CONNECTED_SERVER 2
#define CONNECTED_CLIENT 3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void changeSide(int side);

    void timeOut(int count);

private:
    int state = DISCONNECTED;
    bool opponent_started = false;
    Ui::MainWindow *ui;
    QTcpServer *server = nullptr;
    QTcpSocket *socket = nullptr;

    void createServer();

    void connectServer();

    bool tryConnect(const QString &ip);

    void stopServer();

    void stopClient();

    void connectionEstablished();

    void connectionInterrupted();

    void send(const QString &cmd);

    void receive();

    void setStatus(int new_state);

    void setTurn(const QString& text);

    void setTime(int time);

    void connectBoard();

    void start();

    void enableAdmitDefeat();

    void admitDefeat();

    void win(bool w = true);

#ifdef CHEAT
    QGridLayout *cheatLayout;
    QLineEdit *cheatCmd;

    void cheat();

#endif

signals:

    void statusChanged(int new_status);
};


#endif //ARMYCHESS_MAINWINDOW_H
