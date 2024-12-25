#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include <vector>
#include <tuple>
using namespace std;

struct castle {
    bool kingSide;
    bool queenSide;
};

class Board {
protected:
    std::tuple<int, int> blackKing = {0, 4};
    bool isBlackInCheck = false;
    bool isWhiteInCheck = false;
    std::tuple<int, int> whiteKing = {7, 4};
    bool isWithinBoard(int startX, int startY, int endX, int endY);
    char currentPlayer;
    pair<int, int> enPassantTarget;
    castle whiteCastlingRights;
    castle blackCastlingRights;
public:
    vector<vector<Piece*>> board;
    Board();
    ~Board();
    Piece* getPieceAt(int row, int col) const;
    char getPieceColor(int row, int col) const;  
    void initialise();
    void loadFromFEN(string fen);
    void display() const;
    bool movePiece(int startX, int startY, int endX, int endY, char currentPlayer);
    std::vector<std::pair<int, int>> getLegalMoves(int startX, int startY, char currentPlayer);
    bool isLegalMove(int startX, int startY, int endX, int endY, bool flag=false);
    std::tuple<int, int> getBlackKing();
    std::tuple<int, int> getWhiteKing();
    static tuple<int, int, int, int> previousMove;
    long long perft(int depth, char currentPlayer, long long& captureCount);
};
#endif
