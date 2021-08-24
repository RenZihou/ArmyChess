// connectserverwindow.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef ARMYCHESS_CONNECTSERVERWINDOW_H
#define ARMYCHESS_CONNECTSERVERWINDOW_H

#include <QDialog>

#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class connectServerWindow; }
QT_END_NAMESPACE

class connectServerWindow : public QDialog {
Q_OBJECT

public:
    explicit connectServerWindow(QWidget *parent = nullptr);

    ~connectServerWindow() override;

    /**
     * set display information
     * @param state connection state
     */
    void setInfo(int state);

private:
    Ui::connectServerWindow *ui;  // ui generated by .ui file

    /**
     * called when connect button is clicked
     */
    void connectClicked();

signals:

    /**
     * signal of 'connect button is clicked'
     * @param ip host address try to connect to
     */
    void tryConnect(QString ip);
};


#endif //ARMYCHESS_CONNECTSERVERWINDOW_H
