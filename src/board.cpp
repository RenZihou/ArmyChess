// board.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <random>
#include <algorithm>
#include <string>
#include <sstream>

#include "board.h"
#include "ui_Board.h"


Board::Board(QWidget *parent, long long seed) :
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
        int num;
        switch (type) {
            case LANDMINE:
            case SAPPER:
            case LIEUTENANT:
            case CAPTAIN:
                num = 3;
                break;
            case BOMB:
            case MAJOR:
            case COLONEL:
            case BRIGADIER:
            case MAJGENERAL:
                num = 2;
                break;
            case GENERAL:
            case MARSHAL:
            case ENSIGN:
                num = 1;
                break;
            default:
                break;
        }
        while (num--) {
            auto r = new ChessLabel(this, RED, type);
            auto b = new ChessLabel(this, BLUE, type);
            this->connectChess(r);
            this->connectChess(b);
            chess.push_back(r);
            chess.push_back(b);
        }
    }
    // ==== NOTICE begin ====
    // recommended way here: std::mt19937(std:random_device()())
    // MinGW-w64 has a fatal bug before 10.0
    // the random device doesn't work (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85494)
    // however, qt6 requires mingw 8.1.0,
    // so I have to choose a not recommended way (use time as seed)
    // ==== NOTICE end ====
//    std::shuffle(chess.begin(), chess.end(), std::mt19937(std::time(nullptr)));
    if (!seed) seed = std::time(nullptr);
    std::shuffle(chess.begin(), chess.end(), std::mt19937(seed));

    std::vector<ChessLabel *> empties;
    int x = 0, y = 0;
    for (auto &c : chess) {
        if ((x == 1 || x == 3) && (y == 2 || y == 4 || y == 7 || y == 9)) {
            auto e = new ChessLabel(this, UNKNOWN, EMPTY, x++, y);
            e->reveal();
            this->connectChess(e);
            empties.push_back(e);
        }
        if (x == 2 && (y == 3 || y == 8)) {
            auto e = new ChessLabel(this, UNKNOWN, EMPTY, x++, y);
            e->reveal();
            this->connectChess(e);
            empties.push_back(e);
        }
        c->setInd(x++, y);
        if (x == 5) {
            x = 0;
            y++;
        }
    }
    chess.insert(chess.end(), empties.begin(), empties.end());

    this->drawBoard("all");
}

Board::~Board() {
    delete upper;
    delete lower;
    delete ui;
    delete selected;
}

ChessLabel *Board::getChess(int x, int y) {
    for (auto c : chess) {
        if (c->getXInd() == x && c->getYInd() == y) return c;
    }
    return nullptr;
}

void Board::drawBoard(const std::string &type_, ChessLabel *chess_) {
    if (type_ == "all") {
        QLayoutItem *child;
        while ((child = upper->takeAt(0)) != nullptr) {
            delete child;
        }
        while ((child = lower->takeAt(0)) != nullptr) {
            delete child;
        }
        for (auto &c : chess) {
            if (c->onUpper())
                upper->addWidget(c, c->getYInd(), c->getXInd());
            else
                lower->addWidget(c, c->getYInd() - 6, c->getXInd());
        }
    } else if (type_ == "single") {
//        delete upper->takeAt(upper->indexOf(chess_));
//        delete lower->takeAt(lower->indexOf(chess_));
//        if (chess_->onUpper())
//            upper->addWidget(chess_, chess_->getYInd(), chess_->getXInd());
//        else
//            lower->addWidget(chess_, chess_->getYInd() - 6, chess_->getXInd());
        qDebug() << "deprecated single draw method called";
    }

}

void Board::chessClicked(int x, int y) {
    auto c = this->getChess(x, y);
    this->chessClicked(c);
}

void Board::chessClicked(ChessLabel *chess_) {
    qDebug() << "chess clicked" << chess_->getXInd() << chess_->getYInd();
    if (!chess_->isRevealed()) {
        chess_->reveal();
        this->setSelected(nullptr);
        this->stepProceeded(
                QString("reveal %1 %2").arg(chess_->getXInd()).arg(chess_->getYInd()));
        this->chessRevealed(chess_->getSide());
        return;
    }
    if (selected == nullptr) {
        if (side == chess_->getSide()) this->setSelected(chess_);
        else return;
    } else {
        if (chess_->getSide() == side) this->setSelected(chess_);  // select another
        else if (chess_->getSide() == UNKNOWN) {  // move
            if (Board::movable(selected, chess_)) selected->moveToEmpty(chess_);
            this->setSelected(nullptr);
        } else {  // move and try to kill opponent
            switch (Board::killable(selected, chess_)) {
                case -2:  // illegal movement
                    break;
                case -1:  // killed by opponent
                    selected->kill();
                    break;
                case 0:  // die together
                    chess_->kill();
                    selected->kill();
                    break;
                case 1:  // kill opponent
                    chess_->kill();
                    selected->moveToEmpty(chess_);
            }
            this->setSelected(nullptr);
        }
    }
}

void Board::chessRevealed(int side_) {
    if (side != UNKNOWN) return;
    if (side_ == prev_side) {
        side = side_;
        emit this->sideChanged(side);
        emit this->stepProceeded(QString("side %1").arg(side));
    } else prev_side = side_;
}

int Board::distance(ChessLabel *a, ChessLabel *b, const std::string &rule) {
    if (rule == "hamilton") {
        return std::abs(a->getXInd() - b->getXInd()) + std::abs(a->getYInd() - b->getYInd());
    } else if (rule == "chebyshev") {
        return std::max(std::abs(a->getXInd() - b->getXInd()), std::abs(a->getYInd() - b->getYInd()));
    }
    return -1;
}

bool Board::movable(ChessLabel *current, ChessLabel *target) {
    int t = current->getType();
    if (t == ENSIGN || t == LANDMINE) return false;
    if (Board::distance(current, target, "hamilton") == 1) {  // move along path
        if (current->getXInd() == 1 && current->getYInd() == 5
            && target->getXInd() == 1 && target->getYInd() == 6)
            return false;
        if (current->getXInd() == 1 && current->getYInd() == 6
            && target->getXInd() == 1 && target->getYInd() == 5)
            return false;
        if (current->getXInd() == 3 && current->getYInd() == 5
            && target->getXInd() == 3 && target->getYInd() == 6)
            return false;
        if (current->getXInd() == 3 && current->getYInd() == 6
            && target->getXInd() == 3 && target->getYInd() == 5)
            return false;
        return true;
    } else if (current->inBunker() || target->inBunker()) { // move out of / into bunker
        return Board::distance(current, target, "chebyshev") == 1;
    }  // TODO: move along highway
    return false;
}

int Board::killable(ChessLabel *current, ChessLabel *target) {
    if (target->inBunker()) return -2;
    switch (target->getType()) {
        case ENSIGN:  // TODO: win
            return (current->getType() == SAPPER && !landmine_left) ? 1 : -2;
        case BOMB:
            return 0;
        case LANDMINE:
            if (current->getType() == SAPPER) {
                --landmine_left;
                return 1;
            } else return -2;
        default:
            if (current->getType() == BOMB) return 0;
            if (current->getType() == target->getType()) return 0;
            if (current->getType() > target->getType()) return 1;
            return -1;
    }
}

void Board::connectChess(ChessLabel *c) const {
//    QObject::connect(c, &ChessLabel::operate, this, &Board::stepProceeded);
//    QObject::connect(c, &ChessLabel::revealSide, this, &Board::chessRevealed);
    QObject::connect(c, &ChessLabel::chessClicked, this, qOverload<ChessLabel *>(&Board::chessClicked));
}

void Board::setSelected(ChessLabel *s) {
    if (selected != nullptr) selected->highlight(false);
    if (s != nullptr) s->highlight();
    selected = s;
}

void Board::exec(const QString &cmd_, bool send /* = true */) {
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
    } else if (*it == "move") {
        int x0 = stoi(*(++it));
        int y0 = stoi(*(++it));
        int x1 = stoi(*(++it));
        int y1 = stoi(*(++it));
        this->chessClicked(x0, y0);
        this->chessClicked(x1, y1);
    } else if (*it == "side") {
        int side_ = stoi(*(++it));
        side = 1 - side_;
        emit this->sideChanged(side);
    }

    if (send) emit this->stepProceeded(cmd_);
}