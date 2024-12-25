#include "King.h"
#include "Rook.h"

King::King(char color) : Piece(color) {}

string King::getType() const { 
    return "King";
}

void King::makeMove() {
    hasMoved = true;
}

// should only be called after both checkPseudoCastle && isLegalMove is called and both are true
pair<int, int> King::getRookPosition(int endX, int endY, const vector<vector<Piece*>>& board) const {
    pair<int, int> key = {endX, endY};
    const std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> &haystack 
        = color == 'W' ? whiteCastleLocationToRook : blackCastleLocationToRook;
    return haystack.find(key)->second;
}

bool King::checkPseudoCastle(int endX, int endY, const vector<vector<Piece*>>& board) const {
    // kingside castle
    // if white, then rook must be on 7,7
    // if black, then rook must be on 0,7
    // queenside castle
    // if white, then rook must be on 7,0
    // if black, then rook must be on 0,0
    if (!hasMoved) {
        pair<int, int> key = {endX, endY};
        const std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> &haystack 
            = color == 'W' ? whiteCastleLocationToRook : blackCastleLocationToRook;
        if (haystack.find(key) != haystack.end()) {
            auto [rookX, rookY] = haystack.find(key)->second;
            if (board[rookX][rookY] != nullptr && board[rookX][rookY]->getType() == "Rook") {
                Rook* rook = static_cast<Rook*>(board[rookX][rookY]);
                return rook->canCastle();
            } 
        }   
    }
    return false;
}

bool King::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const {

    if (!hasMoved) {
        if (checkPseudoCastle(endX, endY, board)) {
            return true;
        }
    }

    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    // only one square in any direction
    if (dx > 1 || dy > 1) {
        return false;
    }

    // check if the destination is valid
    Piece* destination = board[endX][endY];
    if (destination != nullptr && destination->getColor() == color) {
        return false;
    }

    return true;
}