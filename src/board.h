// board.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef ARMYCHESS_BOARD_H
#define ARMYCHESS_BOARD_H

#include <vector>

#include <QWidget>
#include <QTimer>
#include <QGridLayout>

#include "ChessLabel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Board; }
QT_END_NAMESPACE

class Board : public QWidget {
Q_OBJECT

public:
    explicit Board(QWidget *parent = nullptr, long long seed = 0);

    ~Board() override;

    void flipTurn();

    void resetTimer();

    void exec(const QString &cmd_, bool send = true);

private:
    int side = UNKNOWN;
    int prev_side = UNKNOWN;
    int landmine_left = 3;
    int timeout = 0;
    int time = 20;
    bool turn = false;
    QTimer *timer;
    Ui::Board *ui;
    QGridLayout *upper;
    QGridLayout *lower;
    std::vector<ChessLabel *> chess;
    ChessLabel *selected = nullptr;

    [[nodiscard]] ChessLabel *getChess(int x, int y) const;

    void drawBoard();

    void chessClicked(ChessLabel *chess_);

    void chessRevealed(int side_);

    static int distance(ChessLabel *a, ChessLabel *b, const std::string &rule);

    bool movable(ChessLabel *current, ChessLabel *target);

    // ret: {-2: ILLEGAL}, {-1: NO}, {0: TIE}, {1: KILLABLE}, {2: WIN}
    int killable(ChessLabel *current, ChessLabel *target);

    bool reachable(ChessLabel *current, ChessLabel *target, bool can_turn) const;

    void connectChess(ChessLabel *c) const;

    void setSelected(ChessLabel *s = nullptr);

    void countDown();

signals:

    void stepProceeded(const QString &cmd_);

    void sideChanged(int side_);

    void timeChanged(int time_);

    void win(int w);
};


#endif //ARMYCHESS_BOARD_H
