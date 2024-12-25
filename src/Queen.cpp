#include "Queen.h"

Queen::Queen(char color) : Piece(color) {}

string Queen::getType() const { 
    return "Queen";
}

bool Queen::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    // straight (like Rook) or diagonal (like Bishop)
    if (!(startX == endX || startY == endY || dx == dy)) {
        return false;
    }

    // check if the path is clear
    // vertical
    if (startX == endX) {
        int step = (endY > startY) ? 1 : -1;
        for (int y = startY + step; y != endY; y += step) {
            if (board[startX][y] != nullptr) {
                return false;
            }
        }
    // horizontal
    } else if (startY == endY) {
        int step = (endX > startX) ? 1 : -1;
        for (int x = startX + step; x != endX; x += step) {
            if (board[x][startY] != nullptr) {
                return false;
            }
        }
    // diagonal
    } else if (dx == dy) {
        int xStep = (endX > startX) ? 1 : -1;
        int yStep = (endY > startY) ? 1 : -1;
        int x = startX + xStep;
        int y = startY + yStep;
        while (x != endX && y != endY) {
            if (board[x][y] != nullptr) {
                return false;
            }
            x += xStep;
            y += yStep;
        }
    }

    // check if the destination is valid
    Piece* destination = board[endX][endY];
    if (destination != nullptr && destination->getColor() == this->getColor()) {
        return false;
    }

    return true;
}