#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"

class Pawn : public Piece {
private:
    bool hasMoved;
public:
    Pawn(char color) : Piece(color), hasMoved(false) {}

    string getType() const override { return "Pawn"; }

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) override {
        int direction = (color == 'W') ? -1 : 1;

        // first move can be 1 or 2
        if (!hasMoved && startY == endY && 
            (endX == startX + direction || endX == startX + 2 * direction) &&
            board[endX][endY] == nullptr) {
            hasMoved = true;
            return true;
        }

        // standard forward move
        if (startY == endY && endX == startX + direction && board[endX][endY] == nullptr) {
            hasMoved = true;
            return true;
        }

        // capture diagonally
        if (endX == startX + direction && (endY == startY - 1 || endY == startY + 1) &&
            board[endX][endY] != nullptr && board[endX][endY]->getColor() != color) {
            return true;
        }

        // TODO: en passant

        return false;
    }
};

#endif
