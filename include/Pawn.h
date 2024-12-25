#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"

// forward declare to use static previousmove attribute
class Board;

class Pawn : public Piece {
private:
    bool hasMoved = false;
public:
    Pawn(char color);
    
    string getType() const override;

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const override;

    void makeMove() override;

    Piece* clone() const override {
        return new Pawn(*this);
    }
};

#endif
