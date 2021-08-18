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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QTcpServer *server = nullptr;
    QTcpSocket *socket = nullptr;

    void createServer();

    void connectionEstablished();

#ifdef CHEAT
    QGridLayout *cheatLayout;
    QLineEdit *cheatCmd;

    void cheat();
#endif
};


#endif //ARMYCHESS_MAINWINDOW_H
