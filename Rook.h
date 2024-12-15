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
        int direction = (color == 'W') ? -1 : 1;
        // TODO: castle logic
        
    }
};

#endif
