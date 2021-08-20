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

    void exec(const QString &cmd_, bool send = true);

private:
    int side = UNKNOWN;
    int prev_side = UNKNOWN;
    QTimer timer;
    Ui::Board *ui;
    QGridLayout *upper;
    QGridLayout *lower;
    std::vector<ChessLabel *> chess;
    ChessLabel *selected = nullptr;

    ChessLabel *getChess(int x, int y);

    void drawBoard(const std::string& type_ = "all", ChessLabel *chess_ = nullptr);

    void chessClicked(ChessLabel *chess_);

    void chessClicked(int x, int y);

    void chessRevealed(int side_);

    static int distance(ChessLabel *a, ChessLabel *b, const std::string &rule);

    static bool movable(ChessLabel *current, ChessLabel *target);

    void connectChess(ChessLabel *c) const;

    void setSelected(ChessLabel *s = nullptr);

signals:

    void stepProceeded(const QString &cmd_);

    void genMap(const QString &cmd_);

    void sideChanged(int side_);
};


#endif //ARMYCHESS_BOARD_H
