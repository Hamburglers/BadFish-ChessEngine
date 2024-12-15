#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"

class Rook : public Piece {
private:
    bool hasMoved;
public:
    Rook(char color) : Piece(color), hasMoved(false) {}

    string getType() const override { 
        return "Rook";
    }

    bool canCastle() const {
        return !hasMoved;
    }

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) override {
        // TODO: castle logic
        // straight line
        if (!(startX == endX || startY == endY)) {
            return false;
        }

        // check if the path is clear
        // vertical move
        if (startX == endX) {
            int step = (endY > startY) ? 1 : -1;
            for (int y = startY + step; y != endY; y += step) {
                if (board[startX][y] != nullptr) {
                    return false;
                }
            }
        // horizontal move
        } else if (startY == endY) {
            int step = (endX > startX) ? 1 : -1;
            for (int x = startX + step; x != endX; x += step) {
                if (board[x][startY] != nullptr) {
                    return false;
                }
            }
        }

        // check if the destination is valid
        Piece* destination = board[endX][endY];
        if (destination != nullptr && destination->getColor() == this->getColor()) {
            return false;
        }

        return true;
    }
};

#endif