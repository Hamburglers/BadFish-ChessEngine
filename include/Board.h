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
    std::tuple<int, int> whiteKing = {7, 4};
    std::tuple<int, int> getBlackKing();
    std::tuple<int, int> getWhiteKing();
    bool isValidMove(int startX, int startY, int endX, int endY);
    bool isPiecePinned(int startX, int startY, char currentPlayer);
public:
    Board();
    ~Board();
    void initialise();
    void display();
    bool movePiece(int startX, int startY, int endX, int endY, char currentPlayer);
};

#endif
