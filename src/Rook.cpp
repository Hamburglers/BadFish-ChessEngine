#include "Rook.h"

Rook::Rook(char color) : Piece(color), hasMoved(false) {}

string Rook::getType() const { 
    return "Rook";
}

void Rook::makeMove() {
    hasMoved = true;
}

bool Rook::canCastle() const {
    return !hasMoved;
}

bool Rook::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const {
    // straight line
    if (!(startX == endX || startY == endY)) {
        return false;
    }

    // check if the path is clear
    // vertical move
    if (startX == endX) {
        int step = (endY > startY) ? 1 : -1;
        for (int y = startY + step; y != endY; y += step) {
            if (board[startX][y] != nullptr) {
                return false;
            }
        }
    // horizontal move
    } else if (startY == endY) {
        int step = (endX > startX) ? 1 : -1;
        for (int x = startX + step; x != endX; x += step) {
            if (board[x][startY] != nullptr) {
                return false;
            }
        }
    }

    // check if the destination is valid
    Piece* destination = board[endX][endY];
    if (destination != nullptr && destination->getColor() == this->getColor()) {
        return false;
    }

    return true;
}