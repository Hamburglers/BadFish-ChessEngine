#ifndef KING_H
#define KING_H

#include "Piece.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

// Custom hash for std::pair<int, int>
struct PairHash {
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& pair) const {
        return std::hash<T>()(pair.first) ^ std::hash<U>()(pair.second);
    }
};

class King : public Piece {
private:
    bool hasMoved = false;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> whiteCastleLocationToRook = {
        {{7, 2}, {7, 0}}, 
        {{7, 6}, {7, 7}}
    };

    std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> blackCastleLocationToRook = {
        {{0, 2}, {0, 0}}, 
        {{0, 6}, {0, 7}}
    };
public:
    King(char color);

    std::string getType() const override;

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const std::vector<std::vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const override;

    void makeMove() override;

    bool checkPseudoCastle(int endX, int endY, const std::vector<std::vector<Piece*>>& board) const;

    pair<int, int> getRookPosition(int endX, int endY, const vector<vector<Piece*>>& board) const;

    Piece* clone() const override {
        return new King(*this);
    }
};

#endif
