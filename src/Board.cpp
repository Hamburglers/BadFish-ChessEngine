#include <iostream>
#include <iomanip>
#include "Bishop.h"
#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Piece.h"
#include "Queen.h"
#include "Rook.h"
#include <typeinfo>

std::tuple<int, int, int, int> Board::previousMove = {-1, -1, -1, -1};

Board::Board() {
    board = vector<vector<Piece*>>(8, vector<Piece*>(8, nullptr));
}

Board::~Board() {
    // free dynamically allocated pieces
    for (auto& row : board) {
        for (auto& piece : row) {
            delete piece;
        }
    }
}

void Board::initialise() {
    // pawns
    for (int i = 0; i < 8; ++i) {
        board[1][i] = new Pawn('B');
        board[6][i] = new Pawn('W');
    }

    // rooks
    board[0][0] = new Rook('B');
    board[0][7] = new Rook('B');
    board[7][0] = new Rook('W');
    board[7][7] = new Rook('W');

    // knights
    board[0][1] = new Knight('B');
    board[0][6] = new Knight('B');
    board[7][1] = new Knight('W');
    board[7][6] = new Knight('W');

    // bishops
    board[0][2] = new Bishop('B');
    board[0][5] = new Bishop('B');
    board[7][2] = new Bishop('W');
    board[7][5] = new Bishop('W');

    // queens
    board[0][3] = new Queen('B');
    board[7][3] = new Queen('W');

    // kings
    board[0][4] = new King('B');
    board[7][4] = new King('W');
}

void Board::display() const {
    // clear terminal so looks nice
    std::cout << "\033[2J\033[H";
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == nullptr) {
                std::cout << ". ";
            } else {
                string type = board[i][j]->getType();
                char pieceChar = type[0]; // First letter of the type
                std::cout << (board[i][j]->getColor() == 'W' ? pieceChar : static_cast<char>(tolower(pieceChar))) << " ";
            }
        }
        std::cout << std::endl;
    }
}

bool Board::isWithinBoard(int startX, int startY, int endX, int endY) {
    return (startX != endX || startY != endY) 
        && startX >= 0 && startX < 8 && startY >= 0 && startY < 8
        && endX >= 0 && endX < 8 && endY >= 0 && endY < 8;
}

tuple<int, int> Board::getWhiteKing() {
    return whiteKing;
}

tuple<int, int> Board::getBlackKing() {
    return blackKing;
}

bool Board::isLegalMove(int startX, int startY, int endX, int endY, bool flag) {
    // Backup the current state
    Piece* movingPiece = board[startX][startY];
    Piece* capturedPiece = board[endX][endY];

    // castling logic, recursively calls thrice
    if (!flag && movingPiece->getType() == "King") {
        King* kingPiece = static_cast<King*>(movingPiece);
        // if both king and rook have not moved
        if (kingPiece->checkPseudoCastle(endX, endY, board)) {
            int dx = endX - startX;
            int dy = endY - startY;
            // castling
            if (dx == 0 && abs(dy) == 2) {
                if (dy > 0) {
                    // first time to check if king is in check
                    // check if square and 2nd square is empty
                    // second time check if next square is in check
                    // third time check if 2nd square is in check
                    return isLegalMove(startX, startY, startX, startY, true) &&
                        board[startX][startY+1] == nullptr &&
                        board[startX][startY+2] == nullptr &&
                        isLegalMove(startX, startY, endX, startY + 1, true) &&
                        isLegalMove(startX, startY, endX, endY, true);
                } else {
                    return isLegalMove(startX, startY, startX, startY, true) &&
                        board[startX][startY-1] == nullptr &&
                        board[startX][startY-2] == nullptr &&
                        isLegalMove(startX, startY, endX, endY - 1, true) &&
                        isLegalMove(startX, startY, endX, endY, true);
                }
            }
        }
    }

    // Make the move temporarily
    char currentPlayer = board[startX][startY]->getColor();
    board[endX][endY] = movingPiece;
    board[startX][startY] = nullptr;

    // Update king position if the moved piece is a king
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            whiteKing = {endX, endY};
        } else {
            blackKing = {endX, endY};
        }
    }

    // Check if the current player's king is in check
    bool isInCheck = false;
    auto [kingX, kingY] = (currentPlayer == 'W') ? whiteKing : blackKing;
    for (int i = 0; i < 8 && !isInCheck; i++) {
        for (int j = 0; j < 8 && !isInCheck; j++) {
            if (board[i][j] && board[i][j]->getColor() != currentPlayer) {
                // Check if the opponent piece can attack the king
                if (board[i][j]->isValidPieceMove(i, j, kingX, kingY, board)) {
                    isInCheck = true;
                }
            }
        }
    }
    // Undo the move to restore the original board state
    board[startX][startY] = movingPiece;
    board[endX][endY] = capturedPiece;

    // Restore king's position if it was moved
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            whiteKing = {startX, startY};
        } else {
            blackKing = {startX, startY};
        }
    }
    return !isInCheck;
}


bool Board::movePiece(int startX, int startY, int endX, int endY, char currentPlayer) {
    // if (board[startX][startY]) { // Check if the pointer is not null
    //     std::cout << "Start position: " << typeid(*board[startX][startY]).name() << std::endl;
    //     std::cout << board[startX][startY]->getType() << std::endl;
    // }
    // if (board[endX][endY]) { // Check if the pointer is not null
    //     std::cout << "End position: " << typeid(*board[endX][endY]).name() << std::endl;
    //     std::cout << board[endX][endY]->getType() << std::endl;
    // }

    // check if even within bounds
    if (!isWithinBoard(startX, startY, endX, endY)) {
        std::cout << "Outside of grid bounds!" << std::endl; 
        return false;
    }
    // check that piece moved is not empty or other players
    if (board[startX][startY] == nullptr || board[startX][startY]->getColor() != currentPlayer) {
        std::cout << "Invalid piece selection!" << std::endl;
        return false;
    }
    // check if captured piece is either nothing, or the other player
    if (board[endX][endY] != nullptr && board[endX][endY]->getColor() == currentPlayer) {
        std::cout << "Cannot capture own piece!" << std::endl;
        return false;
    }
    // check if piece selected can move there (using its own overriden method)
    if (!board[startX][startY]->isValidPieceMove(startX, startY, endX, endY, board)) {
        std::cout << "Invalid move!" << std::endl;
        return false;
    }
    // now that the move is pseudolegal, check if it is actually legal
    // i.e. that it doesnt put king in check
    if (!isLegalMove(startX, startY, endX, endY)) {
        std::cout << "Not a legal move!" << std::endl;
        return false;
    }
    // check if it was castle, then update rook as well
    if (board[startX][startY]->getType() == "King") {
        int dx = endX - startX;
        int dy = endY - startY;
        // confirmed if king moved two spots (and was a legal move)
        if (dx == 0 && abs(dy) == 2) {
            King* king = static_cast<King*>(board[startX][startY]);
            auto [rookX, rookY] = king->getRookPosition(endX, endY, board);
            // kingside castle
            if (dy > 0) {
                board[rookX][rookY-2] = board[rookX][rookY];
                board[rookX][rookY] = nullptr;
            // queenside castle
            } else {
                board[rookX][rookY+3] = board[rookX][rookY];
                board[rookX][rookY] = nullptr;
            }
        }
    }

    // update position of king
    if (board[startX][startY]->getType() == "King") {
        if (board[startX][startY]->getColor() == 'W') {
            whiteKing = std::make_tuple(endX, endY);
        } else {
            blackKing = std::make_tuple(endX, endY);
        }
    }
    delete board[endX][endY];
    auto type = board[startX][startY]->getType();
    // promotion for pawn on 0th, 7th rank
    if (type == "Pawn" && endX == 0 && currentPlayer == 'W') {
        delete board[startX][startY];
        board[startX][startY] = nullptr;
        board[endX][endY] = new Queen('W');
    } else if (type == "Pawn" && endX == 7 && currentPlayer == 'B') {
        delete board[startX][startY];
        board[startX][startY] = nullptr;
        board[endX][endY] = new Queen('B');
    // enpassant, need to remove the pawn being enpassanted
    } else if (type == "Pawn" && get<1>(previousMove) == get<3>(previousMove) &&
           abs(get<2>(previousMove) - get<0>(previousMove)) == 2 &&
           abs(startY - get<1>(previousMove)) == 1 &&
           startX == get<2>(previousMove)) {
        int capturedPawnX = get<2>(previousMove); // Previous pawn's end rank
        int capturedPawnY = get<3>(previousMove); // Previous pawn's file/column

        // delete the en passant captured pawn
        delete board[capturedPawnX][capturedPawnY];
        board[capturedPawnX][capturedPawnY] = nullptr;

        // move the current pawn to its destination
        board[endX][endY] = board[startX][startY];
        board[endX][endY]->makeMove();
        board[startX][startY] = nullptr;
    } else {
        board[endX][endY] = board[startX][startY];
        board[endX][endY]->makeMove();
        board[startX][startY] = nullptr;
    }
    // if (board[startX][startY]) { // Check if the pointer is not null
    //     std::cout << "Start position: " << typeid(*board[startX][startY]).name() << std::endl;
    //     std::cout << board[startX][startY]->getType() << std::endl;
    // }
    // if (board[endX][endY]) { // Check if the pointer is not null
    //     std::cout << "End position: " << typeid(*board[endX][endY]).name() << std::endl;
    //     std::cout << board[endX][endY]->getType() << std::endl;
    // }
    previousMove = {startX, startY, endX, endY};
    display();
    return true;
}

Piece* Board::getPieceAt(int row, int col) const {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        return board[row][col];
    }
    return nullptr;
}

char Board::getPieceColor(int row, int col) const {
    Piece* piece = getPieceAt(row, col);
    if (piece) {
        return piece->getColor();
    }
    return ' ';
}

std::vector<std::pair<int, int>> Board::getLegalMoves(int startX, int startY, char currentPlayer) {
    std::vector<std::pair<int, int>> legalMoves;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (i == startX and j == startY) {
                continue;
            }
            if (board[startX][startY]->isValidPieceMove(startX, startY, i, j, board) && isLegalMove(startX, startY, i, j)) {
                // Check legality and revert the move
                // std::cout << startX << " " << startY << "->" << i << " " << j << std::endl;
                legalMoves.emplace_back(i, j);
            }
        }
    }
    return legalMoves;
}