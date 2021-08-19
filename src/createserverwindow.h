// createserverwindow.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef ARMYCHESS_CREATESERVERWINDOW_H
#define ARMYCHESS_CREATESERVERWINDOW_H

#include <QDialog>

#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class createServerWindow; }
QT_END_NAMESPACE

class createServerWindow : public QDialog {
Q_OBJECT

public:
    explicit createServerWindow(QWidget *parent = nullptr, const QString& ip = "",
                                int state = LISTENING);

    ~createServerWindow() override;

    void setInfo(int status);

private:
    Ui::createServerWindow *ui;
    QString ip_;
};


#endif //ARMYCHESS_CREATESERVERWINDOW_H
