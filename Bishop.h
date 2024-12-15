#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

class Bishop : public Piece {
public:
    Bishop(char color) : Piece(color) {}

    string getType() const override { return "Bishop"; }

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) override {
        int dx = abs(endX - startX);
        int dy = abs(endY - startY);

        // check diagonal move
        if (dx != dy) {
            return false;
        }

        // check path is clear
        int xStep = (endX - startX) > 0 ? 1 : -1;
        int yStep = (endY - startY) > 0 ? 1 : -1;

        int x = startX + xStep;
        int y = startY + yStep;
        while (x != endX && y != endY) {
            if (board[x][y] != nullptr) {
                // path is blocked
                return false;
            }
            x += xStep;
            y += yStep;
        }
        Piece* destination = board[endX][endY];
        if (destination != nullptr && destination->getColor() == this->getColor()) {
            // can't capture your own piece
            return false;
        }
        return true;
    }
};

#endif
