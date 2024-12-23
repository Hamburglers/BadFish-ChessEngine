#include "King.h"

King::King(char color) : Piece(color) {}

string King::getType() const { 
    return "King";
}

bool King::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) const {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    // only one square in any direction
    if (dx > 1 || dy > 1) {
        return false;
    }

    // check if the destination is valid
    Piece* destination = board[endX][endY];
    if (destination != nullptr && destination->getColor() == this->getColor()) {
        return false;
    }

    return true;
}