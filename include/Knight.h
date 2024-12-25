#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.h"

class Knight : public Piece {
public:
    Knight(char color);

    string getType() const override;

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const override;

    Piece* clone() const override {
        return new Knight(*this);
    }
};

#endif
