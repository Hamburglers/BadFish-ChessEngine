#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include <vector>
#include <tuple>
using namespace std;

class Board {
private:
    vector<vector<Piece*>> board;
    std::tuple<int, int> blackKing = {0, 4};
    bool isBlackInCheck = false;
    bool isWhiteInCheck = false;
    std::tuple<int, int> whiteKing = {7, 4};
    std::tuple<int, int> getBlackKing();
    std::tuple<int, int> getWhiteKing();
    bool isWithinBoard(int startX, int startY, int endX, int endY);
    bool isLegalMove(int startX, int startY, int endX, int endY, bool flag);
public:
    Board();
    ~Board();
    Piece* getPieceAt(int row, int col) const;
    char getPieceColor(int row, int col) const;  
    void initialise();
    void display() const;
    bool movePiece(int startX, int startY, int endX, int endY, char currentPlayer);
    std::vector<std::pair<int, int>> getLegalMoves(int startX, int startY, char currentPlayer);
};

#endif
