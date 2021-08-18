// createserverwindow.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef ARMYCHESS_CREATESERVERWINDOW_H
#define ARMYCHESS_CREATESERVERWINDOW_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class createServerWindow; }
QT_END_NAMESPACE

class createServerWindow : public QDialog {
Q_OBJECT

public:
    explicit createServerWindow(QWidget *parent = nullptr, const QString& ip = "");

    ~createServerWindow() override;

private:
    Ui::createServerWindow *ui;
};


#endif //ARMYCHESS_CREATESERVERWINDOW_H
