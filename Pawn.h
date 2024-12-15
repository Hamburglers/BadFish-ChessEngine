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

        // standard forward move
        if (startY == endY && endX == startX + direction && board[endX][endY] == nullptr) {
            hasMoved = true;
            return true;
        }

        // initial double move
        if (!hasMoved && startY == endY && 
            endX == startX + 2 * direction && 
            board[startX + direction][endY] == nullptr && 
            board[endX][endY] == nullptr) {
            hasMoved = true;
            return true;
        }

        // diagonal capture
        if (endX == startX + direction && (endY == startY - 1 || endY == startY + 1) &&
            board[endX][endY] != nullptr && board[endX][endY]->getColor() != color) {
            return true;
        }

        // TODO: En passant and promotion logic
        return false;
    }
};

#endif
