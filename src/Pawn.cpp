#include "Pawn.h"
#include "Board.h"
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

    // enpassant
    const auto& [prevStartX, prevStartY, prevEndX, prevEndY] = Board::previousMove;
    // the first move has not been made yet
    if (prevStartX == -1 || prevStartY == -1 || prevEndX == -1 || prevEndY == -1) {
        return false;
    }
    // check if it was pawn move
    if (board[prevEndX][prevEndY] == nullptr || board[prevEndX][prevEndY]->getType() != "Pawn") {
        return false;
    }
    // now that it is pawn move, check if it was double move
    if (abs(prevEndX - prevStartX) != 2 || prevStartY != prevEndY) {
        return false;
    }
    // now check if the end position of previous pawn move is directly 
    // to the left/right of current and the end move of the current move
    // is 1 above/below it
    if (startX == prevEndX && abs(startY - prevEndY) == 1) {
        int dir = (board[prevEndX][prevEndY]->getColor() == 'B') ? -1 : 1;
        if (endX == prevEndX + dir && endY == prevEndY) {
            return true; // En passant is valid
        }
    }
    
    return false;
}