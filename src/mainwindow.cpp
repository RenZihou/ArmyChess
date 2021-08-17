// mainwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH


#include "mainwindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("Army Chess");
    this->setFixedSize(static_cast<int>(1000 / scaleRatio), static_cast<int>(950 / scaleRatio));
    this->setFocusPolicy(Qt::NoFocus);

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
}

#ifdef CHEAT
void MainWindow::cheat() {
    QString cmd = cheatCmd->text();
    cheatCmd->clear();
    ui->board->exec(cmd);
}
#endif // CHEAT

