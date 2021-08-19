// ChessLabel.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include "ChessLabel.h"

ChessLabel::ChessLabel(QWidget *parent, int side, int type, int x_ind, int y_ind) :
        QLabel(parent), side(side), type(type), x_ind(x_ind), y_ind(y_ind) {
    this->setFixedSize(90 / scaleRatio, 45 / scaleRatio);
    this->setFocusPolicy(Qt::StrongFocus);
    QPixmap image(this->qrcFilename());
    this->setPixmap(image.scaled(90 / scaleRatio, 45 / scaleRatio, Qt::KeepAspectRatio));
}

void ChessLabel::setInd(int x, int y) {
    if (x >= 0) x_ind = x;
    if (y >= 0) y_ind = y;
}

void ChessLabel::reveal(bool reveal /* = true */) {
    revealed = reveal;
    this->updateImage();
}

void ChessLabel::kill() {
    type = EMPTY;
    x_ind = -1;
    y_ind = -1;
    this->updateImage();
}

QString ChessLabel::qrcFilename() const {
    if (type == EMPTY) return {":/chess/empty.png"};
    return revealed ? QString(":/chess/%1_%2").arg(type).arg(side)
                    : QString(":/chess/unknown.png");
}

bool ChessLabel::onUpper() const { return y_ind < 6; }

int ChessLabel::getXInd() const { return x_ind; }

int ChessLabel::getYInd() const { return y_ind; }

bool ChessLabel::isRevealed() const { return revealed; }

void ChessLabel::updateImage() {
    QPixmap image(this->qrcFilename());
    this->setPixmap(image.scaled(90 / scaleRatio, 45 / scaleRatio, Qt::KeepAspectRatio));
}

void ChessLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (type == EMPTY) return;
        if (!revealed) {
            this->reveal();
            emit operate(QString("reveal %1 %2").arg(x_ind).arg(y_ind));
            return;
        } else {
            emit chessSelected(x_ind, y_ind);
        }
    }
}
