#include "Pawn.h"
#include <iostream>
Pawn::Pawn(char color) : Piece(color), hasMoved(false) {}

string Pawn::getType() const { return "Pawn"; }

void Pawn::makeMove() {
    hasMoved = true;
}

bool Pawn::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) const {
    int direction = (color == 'W') ? -1 : 1;

    // standard forward move
    if (startY == endY && endX == startX + direction && board[endX][endY] == nullptr) {
        return true;
    }

    // initial double move
    if (!hasMoved && startY == endY && 
        endX == startX + 2 * direction && 
        board[startX + direction][endY] == nullptr && 
        board[endX][endY] == nullptr) {
        return true;
    }

    // diagonal capture
    if (endX == startX + direction && (endY == startY - 1 || endY == startY + 1) &&
        board[endX][endY] != nullptr && board[endX][endY]->getColor() != color) {
        return true;
    }

    // TODO: En passant and promotion logic
    return false;
}