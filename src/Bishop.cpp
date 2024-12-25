#include "Bishop.h"


Bishop::Bishop(char color) : Piece(color) {}



string Bishop::getType() const { return "Bishop"; }

bool Bishop::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    // check diagonal move
    if (dx != dy) {
        return false;
    }

    // check path is clear
    int xStep = (endX - startX) > 0 ? 1 : -1;
    int yStep = (endY - startY) > 0 ? 1 : -1;

    int x = startX + xStep;
    int y = startY + yStep;
    while (x != endX && y != endY) {
        if (board[x][y] != nullptr) {
            // path is blocked
            return false;
        }
        x += xStep;
        y += yStep;
    }
    Piece* destination = board[endX][endY];
    if (destination != nullptr && destination->getColor() == this->getColor()) {
        // can't capture your own piece
        return false;
    }
    return true;
}