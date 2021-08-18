// connectserverwindow.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef ARMYCHESS_CONNECTSERVERWINDOW_H
#define ARMYCHESS_CONNECTSERVERWINDOW_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class connectServerWindow; }
QT_END_NAMESPACE

class connectServerWindow : public QDialog {
Q_OBJECT

public:
    explicit connectServerWindow(QWidget *parent = nullptr);

    ~connectServerWindow() override;

private:
    Ui::connectServerWindow *ui;

    void connectClicked();

signals:

    void tryConnect(QString ip);
};


#endif //ARMYCHESS_CONNECTSERVERWINDOW_H
