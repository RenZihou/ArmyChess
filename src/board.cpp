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
        QWidget(parent), timer(new QTimer(this)), ui(new Ui::Board) {
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

    QObject::connect(this->timer, &QTimer::timeout, this, &Board::countDown);

    this->drawBoard();
}

Board::~Board() {
    delete upper;
    delete lower;
    delete timer;
    delete ui;
    delete selected;
}

int Board::getSide() const { return side; }

void Board::flipTurn(int turn_ /* = -1 */) {
    if (turn_ == -1) this->turn = !turn;
    else turn = turn_;
}

ChessLabel *Board::getChess(int x, int y) const {
    for (const auto &c : chess) {
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
        if (c->onUpper())
            upper->addWidget(c, c->getYInd(), c->getXInd());
        else
            lower->addWidget(c, c->getYInd() - 6, c->getXInd());
    }
}

void Board::chessClicked(ChessLabel *chess_) {
    qDebug() << "chess clicked" << chess_->getXInd() << chess_->getYInd();
    if (!turn) return;  // not player's turn
    if (opponent_selected != nullptr) {  // cancel highlight display of opponent's move
        opponent_selected->highlight(false);
        opponent_selected = nullptr;
    }
    if (!chess_->isRevealed()) {
        chess_->reveal();
        this->setSelected(nullptr);
        emit this->stepProceeded(
                QString("reveal %1 %2").arg(chess_->getXInd()).arg(chess_->getYInd()));
        emit this->stepProceeded(
                QString("highlight %1 %2").arg(chess_->getXInd()).arg(chess_->getYInd()));
        this->chessRevealed(chess_->getSide());
        if (!frozen) this->exec("finish");
        return;
    }
    if (selected == nullptr) {
        if (side == chess_->getSide()) this->setSelected(chess_);
        else return;
    } else {
        if (chess_->getSide() == side) this->setSelected(chess_);  // select another
        else if (chess_->getSide() == UNKNOWN) {  // move
            if (Board::movable(selected, chess_)) {
                selected->moveToEmpty(chess_);
                emit this->stepProceeded(QString("move %1 %2 %3 %4")
                                                 .arg(selected->getXInd())
                                                 .arg(selected->getYInd())
                                                 .arg(chess_->getXInd())
                                                 .arg(chess_->getYInd()));
                emit this->stepProceeded(QString("highlight %1 %2")
                                                 .arg(chess_->getXInd())
                                                 .arg(chess_->getYInd()));
                if (!frozen) this->exec("finish");
            }
            this->setSelected(nullptr);
        } else {  // move and try to kill opponent
            if (Board::movable(selected, chess_)) {
                switch (Board::killable(selected, chess_)) {
                    case -2:  // illegal movement
                        break;
                    case -1:  // killed by opponent
                        selected->kill();
                        emit this->stepProceeded(QString("kill %1 %2")
                                                         .arg(selected->getXInd())
                                                         .arg(selected->getYInd()));
                        emit this->stepProceeded(QString("highlight %1 %2")
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        if (!frozen) this->exec("finish");
                        break;
                    case 0:  // die together
                        chess_->kill();
                        emit this->stepProceeded(QString("kill %1 %2")
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        selected->kill();
                        emit this->stepProceeded(QString("kill %1 %2")
                                                         .arg(selected->getXInd())
                                                         .arg(selected->getYInd()));
                        emit this->stepProceeded(QString("highlight %1 %2")
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        if (!frozen) this->exec("finish");
                        break;
                    case 1:  // kill opponent
                        chess_->kill();
                        emit this->stepProceeded(QString("kill %1 %2")
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        selected->moveToEmpty(chess_);
                        emit this->stepProceeded(QString("move %1 %2 %3 %4")
                                                         .arg(selected->getXInd())
                                                         .arg(selected->getYInd())
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        emit this->stepProceeded(QString("highlight %1 %2")
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        if (!frozen) this->exec("finish");
                        break;
                    case 2:  // kill and win
                        chess_->kill();
                        emit this->stepProceeded(QString("kill %1 %2")
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        selected->moveToEmpty(chess_);
                        timer->stop();
                        emit this->stepProceeded(QString("move %1 %2 %3 %4")
                                                         .arg(selected->getXInd())
                                                         .arg(selected->getYInd())
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        emit this->stepProceeded(QString("highlight %1 %2")
                                                         .arg(chess_->getXInd())
                                                         .arg(chess_->getYInd()));
                        emit this->stepProceeded("win");
                        qDebug() << "win by killing ensign";
                }
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
    } else return Board::reachable(current, target, t == SAPPER); // move along highway
}

int Board::killable(ChessLabel *current, ChessLabel *target) {
    if (target->inBunker()) return -2;
    switch (target->getType()) {
        case ENSIGN:
            return landmine_left ? -2 : 2;
        case BOMB:
            return 0;
        case LANDMINE:
            if (current->getType() == SAPPER) {
                --landmine_left;
                return 1;
            } else if (current->getType() == BOMB) {
                --landmine_left;
                return 0;
            }
            return -2;
        default:
            if (current->getType() == BOMB) return 0;
            if (current->getType() == target->getType()) return 0;
            if (current->getType() > target->getType()) return 1;
            return -1;
    }
}

bool Board::reachable(ChessLabel *current, ChessLabel *target, bool can_turn) const {
    if (!(current->onHighway() && target->onHighway())) return false;
    if (can_turn) {
        std::vector<ChessLabel *> highway = {current};
        int prev_size = 0;
        while (static_cast<int>(highway.size()) != prev_size) {
            int inc_size = 0;
            for (int h = static_cast<int>(highway.size()) - 1; h >= prev_size; --h) {
                for (int direction = 0; direction < 4; ++direction) {
                    auto c = this->getChess(highway[h]->getXInd() + !(direction & 1) * (1 - direction),
                                            highway[h]->getYInd() + (direction & 1) * (direction - 2));
                    if (c == nullptr) continue;
                    qDebug() << "check" << c->getXInd() << c->getYInd();
                    if (c->onHighway() &&
                        std::find(highway.begin(), highway.end(), c) == highway.end()) {
                        qDebug() << "add" << c->getXInd() << c->getYInd();
                        if (c == target) return true;
                        if (c->getType() == EMPTY) {
                            highway.push_back(c);
                            ++inc_size;
                        }
                    }
                }
            }
            prev_size = static_cast<int>(highway.size()) - inc_size;
        }
        return false;
    } else {  // can only move straightly
        if (current->getXInd() == target->getXInd()) {  // vertically
            int smaller, greater;
            switch (current->getXInd()) {
                case 0:
                case 4:
                    smaller = std::min(current->getYInd(), target->getYInd());
                    greater = std::max(current->getYInd(), target->getYInd());
                    for (int y = smaller + 1; y < greater; ++y) {
                        if (this->getChess(current->getXInd(), y)->getType() != EMPTY)
                            return false;
                    }
                    return true;
//                case 2:  // this case can be substituted by moving along road
                default:
                    return false;
            }
        } else if (current->getYInd() == target->getYInd()) {  // horizontally
            int smaller, greater;
            switch (current->getYInd()) {
                case 1:
                case 5:
                case 6:
                case 10:
                    smaller = std::min(current->getXInd(), target->getXInd());
                    greater = std::max(current->getXInd(), target->getXInd());
                    for (int x = smaller + 1; x < greater; ++x) {
                        if (this->getChess(x, current->getYInd())->getType() != EMPTY)
                            return false;
                    }
                    return true;
                default:
                    return false;
            }
        } else return false;
    }
}

void Board::connectChess(ChessLabel *c) const {
    QObject::connect(c, &ChessLabel::chessClicked, this, qOverload<ChessLabel *>(&Board::chessClicked));
    QObject::connect(c, &ChessLabel::soldierKilled, this, &Board::soldierKilled);
}

void Board::setSelected(ChessLabel *s) {
    if (selected != nullptr) {
        selected->highlight(false);
    }
    if (s != nullptr) {
        s->highlight();
        emit this->stepProceeded(
                QString("highlight %1 %2").arg(s->getXInd()).arg(s->getYInd()));
    }
    selected = s;
}

void Board::countDown() {
    --time;
    emit this->timeChanged(time);
    if (turn && time == 0) {
        ++timeout;
        qDebug() << "timeout" << timeout << "times";
        if (timeout == 3) {  // judge lose
            this->exec("lose");
            return;
        }
        this->setSelected(nullptr);
        this->flipTurn();
        this->resetTimer();
        emit this->timeOut(timeout);
        emit this->stepProceeded("finish");
    }
}

void Board::resetTimer() {
    timer->start(1000);
    time = 20;
    emit this->timeChanged(time);
}

void Board::soldierKilled() { --soldier_left; }

void Board::exec(const QString &cmd_, bool send /* = true */) {
    if (stop) return;
    std::string cmd = cmd_.toStdString();
    std::string part;
    std::vector<std::string> parts;
    std::stringstream cmd_ss(cmd);
    while (cmd_ss >> part) parts.push_back(part);

    auto it = parts.begin();
    if (*it == "reveal") {
        int x = stoi(*(++it));
        int y = stoi(*(++it));
        auto c = this->getChess(x, y);
        if (c != nullptr) c->reveal();
    } else if (*it == "kill") {
        int x = stoi(*(++it));
        int y = stoi(*(++it));
        auto c = this->getChess(x, y);
        if (c != nullptr) c->kill();
    } else if (*it == "move") {
        int x0 = stoi(*(++it));
        int y0 = stoi(*(++it));
        int x1 = stoi(*(++it));
        int y1 = stoi(*(++it));
        auto c0 = this->getChess(x0, y0);
        auto c1 = this->getChess(x1, y1);
        if (c0 != nullptr && c1 != nullptr) c0->moveToEmpty(c1);
    } else if (*it == "highlight") {
        int x = stoi(*(++it));
        int y = stoi(*(++it));
        auto c = this->getChess(x, y);
        if (opponent_selected != nullptr) opponent_selected->highlight(false);
        opponent_selected = c;
        opponent_selected->highlight(true, "orange");
    } else if (*it == "freeze") {
        if (frozen) {
            timer->start(1000);
        } else {
            timer->stop();
        }
        frozen = !frozen;
    } else if (*it == "side") {
        int side_ = stoi(*(++it));
        side = 1 - side_;
        emit this->sideChanged(side);
    } else if (*it == "finish") {
        qDebug() << "tmp finish exec";
        ++total_turn;
        if (total_turn == 20) emit this->canAdmitDefeat();
        this->flipTurn();
        this->resetTimer();

        if (soldier_left == 0) {
            this->exec("lose");
            qDebug() << "lose because all soldiers are killed";
        }
    } else if (*it == "win" || *it == "lose") {
        qDebug() << "tmp win/lose exec";
        stop = true;
        timer->stop();
        this->flipTurn(0);
    }

    if (send) emit this->stepProceeded(cmd_);
}