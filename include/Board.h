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
    bool isBlackInCheck = false;
    bool isWhiteInCheck = false;
    bool isWithinBoard(int startX, int startY, int endX, int endY);
    char currentPlayer;
    pair<int, int> enPassantTarget;
    castle whiteCastlingRights;
    castle blackCastlingRights;
public:
    Board();
    ~Board();
    Board& operator=(const Board& other);
    Board(const Board& other);
    vector<vector<Piece*>> board;
    std::tuple<int, int> whiteKing = {7, 4};
    std::tuple<int, int> blackKing = {0, 4};
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
    tuple<int, int, int, int> previousMove = {-1, -1, -1, -1};
    long long perft(int depth, char currentPlayer, long long& captureCount);
};
#endif
