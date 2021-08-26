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
    /**
     * constructor
     * @param parent parent widget
     * @param side chess side
     * @param type chess type
     * @param x_ind chess x coordinate
     * @param y_ind chess y coordinate
     */
    explicit ChessLabel(QWidget *parent = nullptr,
                        int side = UNKNOWN, int type = EMPTY,
                        int x_ind = 0, int y_ind = 0);

    ~ChessLabel() override = default;

    /**
     * set chess coordinate
     * @param x new x coordinate
     * @param y new y coordinate
     */
    void setInd(int x = -1, int y = -1);

    /**
     * reveal chess
     * @param reveal set chess as revealed or not
     */
    void reveal(bool reveal = true);

    /**
     * move chess to an empty place, if target is not empty, chess won't
     * be moved
     * @param target target place to move to
     */
    void moveToEmpty(ChessLabel *target);

    /**
     * kill a chess (to set it as empty)
     */
    void kill();

    /**
     * highlight a chess
     * @param h set chess as highlighted or not
     * @param color highlight frame color
     */
    void highlight(bool h = true, const QString& color = "red");

    /**
     * chess is on upper board or not (on lower board)
     * @return whether chess is on upper board
     */
    [[nodiscard]] bool onUpper() const;

    /**
     * get the x coordinate of chess
     * @return x coordinate
     */
    [[nodiscard]] int getXInd() const;

    /**
     * get the y coordinate of chess
     * @return y coordinate
     */
    [[nodiscard]] int getYInd() const;

    /**
     * chess is in bunker or not
     * @return whether chess is in bunker
     */
    [[nodiscard]] bool inBunker() const;

    /**
     * chess is on highway or not
     * @return whether chess is on highway
     */
    [[nodiscard]] bool onHighway() const;

    /**
     * chess is revealed or not
     * @return whether chess is revealed
     */
    [[nodiscard]] bool isRevealed() const;

    /**
     * get the side of chess
     * @return side of chess
     */
    [[nodiscard]] int getSide() const;

    /**
     * get the type of chess
     * @return type of chess
     */
    [[nodiscard]] int getType() const;

private:
    int side;  // side of chess
    int type;  // type of chess
    int x_ind;  // x coordinate of chess
    int y_ind;  // y coordinate of chess
    bool revealed = false;  // chess is revealed

    /**
     * get the chess image in the qrc file
     * @return the path of the chess image
     */
    [[nodiscard]] QString qrcFilename() const;

    /**
     * update chess image (called after the chess's side or type is changed)
     */
    void updateImage();

    /**
     * mouse press event handler, only deal with left-click
     * @param event mouse press event
     */
    void mousePressEvent(QMouseEvent *event) override;

signals:

    /**
     * signal of 'chess is clicked'
     * @param chess clicked chess
     */
    void chessClicked(ChessLabel *chess);

    /**
     * signal of 'an alley soldier is killed'
     */
    void soldierKilled();
};

#endif //ARMYCHESS_CHESSLABEL_H
