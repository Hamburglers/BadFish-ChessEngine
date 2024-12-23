#ifndef KING_H
#define KING_H

#include "Piece.h"
#include <vector>
#include <string>

class King : public Piece {
public:
    King(char color);

    std::string getType() const override;

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const std::vector<std::vector<Piece*>>& board) const override;
};

#endif
