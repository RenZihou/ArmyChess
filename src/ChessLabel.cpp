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

void ChessLabel::moveToEmpty(ChessLabel *target) {
    if (target->type != EMPTY) return;
    target->type = this->type;
    target->side = this->side;
    target->revealed = this->revealed;
    target->updateImage();
    this->type = EMPTY;
    this->side = UNKNOWN;
    this->revealed = true;
    this->updateImage();
}

void ChessLabel::kill() {
    type = EMPTY;
    side = UNKNOWN;
    this->revealed = true;
    this->updateImage();
}

void ChessLabel::highlight(bool h /* = true */) {
    h ? this->setStyleSheet("border: 2px solid red;")
      : this->setStyleSheet("");  // TODO: change color
}

QString ChessLabel::qrcFilename() const {
    if (type == EMPTY) return {":/chess/empty.png"};
    return revealed ? QString(":/chess/%1_%2").arg(type).arg(side)
                    : QString(":/chess/unknown.png");
}

bool ChessLabel::onUpper() const { return y_ind < 6; }

int ChessLabel::getXInd() const { return x_ind; }

int ChessLabel::getYInd() const { return y_ind; }

bool ChessLabel::inBunker() const {
    switch (x_ind) {
        case 1:
        case 3:
            return y_ind == 2 || y_ind == 4 || y_ind == 7 || y_ind == 9;
        case 2:
            return y_ind == 3 || y_ind == 8;
        default:
            return false;
    }
}

bool ChessLabel::isRevealed() const { return revealed; }

int ChessLabel::getSide() const { return side; }

int ChessLabel::getType() const { return type; }

void ChessLabel::updateImage() {
    QPixmap image(this->qrcFilename());
    this->setPixmap(image.scaled(90 / scaleRatio, 45 / scaleRatio, Qt::KeepAspectRatio));
}

void ChessLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit chessClicked(this);
//        if (!revealed) {
//            this->reveal();
//            emit revealSide(side);
//            emit operate(QString("reveal %1 %2").arg(x_ind).arg(y_ind));
//            return;
//        } else {
//            emit chessClicked(this);
//        }
    }
}
