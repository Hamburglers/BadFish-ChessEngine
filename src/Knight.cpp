#include "Knight.h"

Knight::Knight(char color) : Piece(color) {}

string Knight::getType() const { return "Knight"; }

bool Knight::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    // check for the "L" shape movement
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        return false;
    }

    // check if the destination is valid
    Piece* destination = board[endX][endY];
    if (destination != nullptr && destination->getColor() == this->getColor()) {
        // can't capture your own piece
        return false;
    }
    return true;
}