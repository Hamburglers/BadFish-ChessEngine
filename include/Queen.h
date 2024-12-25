#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

class Queen : public Piece {
public:
    Queen(char color);

    string getType() const override;

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const override;
    
    Piece* clone() const override {
        return new Queen(*this);
    }
};

#endif
