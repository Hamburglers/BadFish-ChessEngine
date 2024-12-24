#include "King.h"
#include "Rook.h"

King::King(char color) : Piece(color) {}

string King::getType() const { 
    return "King";
}

void King::makeMove() {
    hasMoved = true;
}

bool King::isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) const {
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
                Rook* kingsideRook = static_cast<Rook*>(board[rookX][rookY]);
                if (kingsideRook->canCastle()) {
                    return true;
                }
            } 
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