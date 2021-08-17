// board.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <random>
#include <algorithm>
#include <string>
#include <sstream>

#include "board.h"
#include "ui_Board.h"


Board::Board(QWidget *parent) :
        QWidget(parent), ui(new Ui::Board) {
    int w = static_cast<int>(640 / scaleRatio);
    int h = static_cast<int>(920 / scaleRatio);
    ui->setupUi(this);
    this->setFixedSize(w, h);
    this->setFocusPolicy(Qt::NoFocus);
    ui->background->setFixedSize(w, h);
    ui->chessLower->setGeometry(0, h / 2, w, h / 2);
    ui->chessUpper->setGeometry(0, 0, w, h / 2);
    upper = new QGridLayout(ui->chessUpper);
    upper->setVerticalSpacing(static_cast<int>(20 / scaleRatio));
    upper->setHorizontalSpacing(static_cast<int>(41 / scaleRatio));
    upper->setContentsMargins(static_cast<int>(14 / scaleRatio), static_cast<int>(14 / scaleRatio),
                              static_cast<int>(12 / scaleRatio), static_cast<int>(76 / scaleRatio));
    lower = new QGridLayout(ui->chessLower);
    lower->setVerticalSpacing(static_cast<int>(20 / scaleRatio));
    lower->setHorizontalSpacing(static_cast<int>(41 / scaleRatio));
    lower->setContentsMargins(static_cast<int>(14 / scaleRatio), static_cast<int>(74 / scaleRatio),
                              static_cast<int>(12 / scaleRatio), static_cast<int>(16 / scaleRatio));

    QPixmap bg(":/background/board.png");
    ui->background->setPixmap(bg.scaled(w, h, Qt::KeepAspectRatio));

    for (int type = LANDMINE; type <= MARSHAL; ++type) {
        switch (type) {
            case LANDMINE:
            case SAPPER:
            case LIEUTENANT:
            case CAPTAIN:
                chess.push_back(new ChessLabel(this, RED, type));
                chess.push_back(new ChessLabel(this, BLUE, type));
            case BOMB:
            case MAJOR:
            case COLONEL:
            case BRIGADIER:
            case MAJGENERAL:
                chess.push_back(new ChessLabel(this, RED, type));
                chess.push_back(new ChessLabel(this, BLUE, type));
            case GENERAL:
            case MARSHAL:
            case ENSIGN:
                chess.push_back(new ChessLabel(this, RED, type));
                chess.push_back(new ChessLabel(this, BLUE, type));
            default:
                break;
        }
    }
    // ==== NOTICE begin ====
    // recommended way here: std::mt19937(std:random_device()())
    // MinGW-w64 has a fatal bug before 10.0
    // the random device doesn't work (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85494)
    // however, qt6 requires mingw 8.1.0,
    // so I have to choose a not recommended way (use time as seed)
    // ==== NOTICE end ====
    std::shuffle(chess.begin(), chess.end(), std::mt19937(std::time(nullptr)));

    int x = 0, y = 0;
    for (auto &c : chess) {
        if ((x == 1 || x == 3) && (y == 2 || y == 4 || y == 7 || y == 9)) ++x;
        if (x == 2 && (y == 3 || y == 8)) ++x;
        c->setInd(x++, y);
        if (x == 5) {
            x = 0;
            y++;
        }

        QObject::connect(c, &ChessLabel::chessFlipped, this, &Board::drawBoard);
    }

    drawBoard();
}

Board::~Board() {
    delete ui;
}

ChessLabel *Board::getChess(int x, int y) {
    for (auto c : chess) {
        if (c->getXInd() == x && c->getYInd() == y) return c;
    }
    return nullptr;
}

void Board::drawBoard() {
    QLayoutItem *child;
    while ((child = upper->takeAt(0)) != nullptr) {
        delete child;
    }
    while ((child = lower->takeAt(0)) != nullptr) {
        delete child;
    }
    for (auto &c : chess) {
        if (c->onUpper()) {
            upper->addWidget(dynamic_cast<QWidget *>(c), c->getYInd(), c->getXInd());
        } else {
            lower->addWidget(dynamic_cast<QWidget *>(c), c->getYInd() - 6, c->getXInd());
        }
    }
}

void Board::exec(const QString &cmd_) {
    std::string cmd = cmd_.toStdString();
    std::string part;
    std::vector<std::string> parts;
    std::stringstream cmd_ss(cmd);
    while (cmd_ss >> part) parts.push_back(part);

    auto it = parts.begin();
    if (*it == "reveal") {
        int x = stoi(*(++it));
        int y = stoi(*(++it));
        ChessLabel *c = this->getChess(x, y);
        if (c != nullptr) c->reveal();
    } else if (*it == "kill") {
        int x = stoi(*(++it));
        int y = stoi(*(++it));
        ChessLabel *c = this->getChess(x, y);
        if (c != nullptr) c->kill();
    }
}