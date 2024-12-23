#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"

class Rook : public Piece {
private:
    bool hasMoved = false;
public:
    Rook(char color);

    string getType() const override;

    bool canCastle() const;

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) const override;

    void makeMove() override;
};

#endif
