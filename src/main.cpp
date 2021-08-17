// main.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return QApplication::exec();
}
