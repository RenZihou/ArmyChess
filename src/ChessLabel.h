// ChessLabel.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef ARMYCHESS_CHESSLABEL_H
#define ARMYCHESS_CHESSLABEL_H

#include <QLabel>
#include <QMouseEvent>

#define UNKNOWN (-1)
#define RED 0
#define BLUE 1

#define ENSIGN 0
#define SAPPER 1
#define LIEUTENANT 2
#define CAPTAIN 3
#define MAJOR 4
#define COLONEL 5
#define BRIGADIER 6
#define MAJGENERAL 7
#define GENERAL 8
#define MARSHAL 9
#define BOMB (-1)
#define LANDMINE (-2)
#define EMPTY (-9)

class ChessLabel : public QLabel {
Q_OBJECT

public:
    explicit ChessLabel(QWidget *parent = nullptr,
                        int side = UNKNOWN, int type = EMPTY,
                        int x_ind = 0, int y_ind = 0);

    ~ChessLabel() override = default;

    void setInd(int x = -1, int y = -1);

    void reveal(bool reveal = true);

    void moveToEmpty(ChessLabel *target);

    void kill();

    void highlight(bool h = true);

    [[nodiscard]] bool onUpper() const;

    [[nodiscard]] int getXInd() const;

    [[nodiscard]] int getYInd() const;

    [[nodiscard]] bool inBunker() const;

    [[nodiscard]] bool onHighway() const;

    [[nodiscard]] bool isRevealed() const;

    [[nodiscard]] int getSide() const;

    [[nodiscard]] int getType() const;

private:
    int side;
    int type;
    int x_ind;
    int y_ind;
    bool revealed = false;

    [[nodiscard]] QString qrcFilename() const;

    void updateImage();

    void mousePressEvent(QMouseEvent *event) override;

signals:

    void chessClicked(ChessLabel *chess);

    void soldierKilled();
};

#endif //ARMYCHESS_CHESSLABEL_H
