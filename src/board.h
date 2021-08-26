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
    /**
     * constructor
     * @param parent parent widget
     * @param seed map seed, if = 0, then use time as seed
     */
    explicit Board(QWidget *parent = nullptr, long long seed = 0);

    ~Board() override;

    /**
     * get player's side
     * @return player's side
     */
    [[nodiscard]] int getSide() const;

    /**
     * flip turn or set turn
     * @param turn_ -1 (default) to flip turn, 0 or 1 to set turn
     */
    void flipTurn(int turn_ = -1);

    /**
     * reset timer and begin a new round
     */
    void resetTimer();

    /**
     * execute instruction
     * @param cmd_ the instruction to execute
     * @param send whether emit signal to tell program to send
     * instruction to peer
     */
    void exec(const QString &cmd_, bool send = true);

private:
    int side = UNKNOWN;  // player's side
    int prev_side = UNKNOWN;  // last revealed chess's side
    int landmine_left = 3;  // number of landmine to dig out
    int soldier_left = 21;  // number of movable soldier alive
    int timeout = 0;  // timeout number
    int time = 20;  // time left for current round
    int total_turn = 0;  // total turns of the game
    bool turn = false;  // is player's turn
    bool frozen = false;  // only for debug: freeze game and no count-down
    QTimer *timer;  // countdown timer
    Ui::Board *ui;  // ui generated from .ui file
    QGridLayout *upper;  // layout of upper chess
    QGridLayout *lower;  // layout of lower chess
    std::vector<ChessLabel *> chess;  // all chess
    ChessLabel *selected = nullptr;  // selected chess
    ChessLabel *opponent_selected = nullptr;  // opponent selected chess (to highlight)

    /**
     * get the chess with given coordinate
     * @param x the x coordinate of the chess, 0 <= x < 5
     * @param y the y coordinate of the chess, 0 <= y < 12
     * @return the pointer to the chess,
     * if invalid coordinate is given, return nullptr
     */
    [[nodiscard]] ChessLabel *getChess(int x, int y) const;

    /**
     * draw chess on board. only call this to initialize the board
     */
    void drawBoard();

    /**
     * called when a chess is clicked, used to do steps
     * @param chess_ the chess that is clicked
     */
    void chessClicked(ChessLabel *chess_);

    /**
     * called when a chess is revealed, used to determine player's side
     * @param side_ the side that the revealed chess belong to
     */
    void chessRevealed(int side_);

    /**
     * evaluate specified distance between two chess on the board
     * @param a one chess
     * @param b another chess
     * @param rule "hamilton" distance or "chebyshev" distance
     * @return the distance between `a` and `b`
     */
    static int distance(ChessLabel *a, ChessLabel *b, const std::string &rule);

    /**
     * whether a chess can move to the target
     * @param current the chess that attempt to move
     * @param target where the chess `current` attempt to move to
     * @return whether the movement is valid
     */
    bool movable(ChessLabel *current, ChessLabel *target);

    /**
     * whether a chess can kill the target
     * @param current the chess that attempt to attack
     * @param target the chess that `current` attempt to attack
     * @return {-2: illegal move}, {-1: die self}, {0: die together},
     * {1: can kill}, {2: kill and win the game}
     */
    int killable(ChessLabel *current, ChessLabel *target);

    /**
     * whether a chess can reach the target through highway
     * (ignored several 1-step situation)
     * @param current the chess that attempt to move
     * @param target where the chess `current` attempt to move to
     * @param can_turn whether the chess `current` can change direction
     * on the highway (is a sapper)
     * @return whether the movement is valid
     */
    bool reachable(ChessLabel *current, ChessLabel *target, bool can_turn) const;

    /**
     * build connections from chess signal to board slot
     * @param c the chess to build connection
     */
    void connectChess(ChessLabel *c) const;

    /**
     * set a chess as selected and highlight it
     * @param s the selected chess
     */
    void setSelected(ChessLabel *s = nullptr);

    /**
     * receive timeout signal from timer and count down to 0
     */
    void countDown();

    /**
     * count up dead soldier number and judge lose when all movable
     * soldiers are killed
     */
    void soldierKilled();

signals:

    /**
     * signal of 'an instruction is executed'
     * @param cmd_ executed instruction
     */
    void stepProceeded(const QString &cmd_);

    /**
     * signal of 'player side is determined'
     * @param side_ determined side
     */
    void sideChanged(int side_);

    /**
     * signal of 'time counted down'
     * @param time_ count-down time
     */
    void timeChanged(int time_);

    /**
     * signal of 'player run out of time'
     * @param count count of timeout times
     */
    void timeOut(int count);

    /**
     * signal of 'player can admit defeat from now on'
     */
    void canAdmitDefeat();
};


#endif //ARMYCHESS_BOARD_H
