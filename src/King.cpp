#include "King.h"

King::King(char color) : Piece(color) {}

string King::getType() const { 
    return "King";
}

bool King::isPositionCheck(int x, int y, const vector<vector<Piece*>>& board) const {
    for (size_t i = 0; i < board.size(); i++) {
        for (size_t j = 0; j < board.size(); j++) {
            Piece* piece = board[i][j];
            if (piece == nullptr) {
                continue;
            }
            // skip pieces of the same colour
            if (piece->getColor() == this->getColor()) {
                continue;
            }
            // if any opponent piece can move to (x, y), it is under attack
            if (piece->isValidPieceMove(i, j, x, y, board)) {
                return true;
            }
        }
    }
    return false;
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

    // check new position for checks
    return !isPositionCheck(endX, endY, board);
}