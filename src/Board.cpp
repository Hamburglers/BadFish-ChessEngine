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
#include <sstream>

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

// Copy constructor
Board::Board(const Board& other) {
    // Deep copy of board
    board.resize(8, vector<Piece*>(8, nullptr));
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (other.board[i][j]) {
                board[i][j] = other.board[i][j]->clone();
            }
        }
    }

    // Copy other members
    isBlackInCheck = other.isBlackInCheck;
    isWhiteInCheck = other.isWhiteInCheck;
    currentPlayer = other.currentPlayer;
    enPassantTarget = other.enPassantTarget;
    whiteCastlingRights = other.whiteCastlingRights;
    blackCastlingRights = other.blackCastlingRights;
    whiteKing = other.whiteKing;
    blackKing = other.blackKing;
}

// Copy assignment operator
Board& Board::operator=(const Board& other) {
    if (this != &other) {
        // Free existing resources
        for (auto& row : board) {
            for (auto& piece : row) {
                delete piece; // Clean up existing pointers
            }
        }

        // Resize and deep copy board
        board.resize(8, vector<Piece*>(8, nullptr));
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (other.board[i][j]) {
                    board[i][j] = other.board[i][j]->clone();
                }
            }
        }

        // Copy other members
        isBlackInCheck = other.isBlackInCheck;
        isWhiteInCheck = other.isWhiteInCheck;
        currentPlayer = other.currentPlayer;
        enPassantTarget = other.enPassantTarget;
        whiteCastlingRights = other.whiteCastlingRights;
        blackCastlingRights = other.blackCastlingRights;
        whiteKing = other.whiteKing;
        blackKing = other.blackKing;
    }
    return *this;
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

void Board::loadFromFEN(string fen) {
    std::istringstream fenStream(fen);
    std::string piecePlacement, activeColor, castlingRights, enPassantSquare, halfmoveClock, fullmoveNumber;

    // split the FEN string into its components
    fenStream >> piecePlacement >> activeColor >> castlingRights >> enPassantSquare >> halfmoveClock >> fullmoveNumber;

    // reset the board
    board = vector<vector<Piece*>>(8, vector<Piece*>(8, nullptr));

    // parse piece placement
    int row = 0, col = 0;
    for (char ch : piecePlacement) {
        if (ch == '/') {
            // move to the next row
            row++;
            col = 0;
        } else if (isdigit(ch)) {
            // empty squares
            col += ch - '0';
        } else {
            // place a piece
            char color = isupper(ch) ? 'W' : 'B';
            ch = tolower(ch); // Normalize to lowercase for type checking
            switch (ch) {
                case 'p': board[row][col] = new Pawn(color); break;
                case 'r': board[row][col] = new Rook(color); break;
                case 'n': board[row][col] = new Knight(color); break;
                case 'b': board[row][col] = new Bishop(color); break;
                case 'q': board[row][col] = new Queen(color); break;
                case 'k': 
                    board[row][col] = new King(color); 
                    if (color == 'W') whiteKing = {row, col};
                    else blackKing = {row, col};
                    break;
            }
            col++;
        }
    }

    // parse active color
    if (activeColor == "w") {
        currentPlayer = 'W';
    } else {
        currentPlayer = 'B';
    }

    // parse castling rights
    for (char ch : castlingRights) {
        if (ch == 'K') whiteCastlingRights.kingSide = true;
        if (ch == 'Q') whiteCastlingRights.queenSide = true;
        if (ch == 'k') blackCastlingRights.kingSide = true;
        if (ch == 'q') blackCastlingRights.queenSide = true;
    }

    // parse en passant target square
    if (enPassantSquare != "-") {
        int file = enPassantSquare[0] - 'a';
        int rank = 8 - (enPassantSquare[1] - '0');
        enPassantTarget = {rank, file};
    } else {
        enPassantTarget = {-1, -1};
    }
    // not needed for now
    halfmoveClock = stoi(halfmoveClock);
    fullmoveNumber = stoi(fullmoveNumber);
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
    Piece* enPassantCapturedPawn = nullptr;

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
                    // king side castle, need to check 2 squares
                    return isLegalMove(startX, startY, startX, startY, true) &&
                        board[startX][startY+1] == nullptr &&
                        board[startX][startY+2] == nullptr &&
                        isLegalMove(startX, startY, endX, startY + 1, true) &&
                        isLegalMove(startX, startY, endX, endY, true);
                } else {
                    // queen side castle, need to check 3 squares
                    return isLegalMove(startX, startY, startX, startY, true) &&
                        board[startX][startY-1] == nullptr &&
                        board[startX][startY-2] == nullptr &&
                        board[startX][startY-3] == nullptr &&
                        isLegalMove(startX, startY, endX, startY - 2, true) &&
                        isLegalMove(startX, startY, endX, startY - 1, true) &&
                        isLegalMove(startX, startY, endX, endY, true);
                }
            }
        }
    }

    // if enpassant
    if (movingPiece->getType() == "Pawn" &&
        get<1>(previousMove) == get<3>(previousMove) &&
        abs(get<2>(previousMove) - get<0>(previousMove)) == 2 &&
        abs(startY - get<1>(previousMove)) == 1 &&
        startX == get<2>(previousMove) &&
        endX == get<2>(previousMove) + 
            ((board[get<2>(previousMove)][get<3>(previousMove)]->getColor() == 'B') ? -1 : 1) &&
        endY == get<3>(previousMove)) {
        // previous pawn's end rank
        int capturedPawnX = get<2>(previousMove); 
        // previous pawn's file/column
        int capturedPawnY = get<3>(previousMove);

        // temporarily remove the en passant captured pawn
        enPassantCapturedPawn = board[capturedPawnX][capturedPawnY];
        board[capturedPawnX][capturedPawnY] = nullptr;

        // move the current pawn to its destination
        board[endX][endY] = movingPiece;
        board[startX][startY] = nullptr;

        // check if the king is in check after this move
        char currentPlayer = movingPiece->getColor();
        auto [kingX, kingY] = (currentPlayer == 'W') ? whiteKing : blackKing;
        bool isInCheck = false;

        // Check if the current player's king is in check
        for (int i = 0; i < 8 && !isInCheck; i++) {
            for (int j = 0; j < 8 && !isInCheck; j++) {
                if (board[i][j] && board[i][j]->getColor() != currentPlayer) {
                    // Check if the opponent piece can attack the king
                    if (board[i][j]->isValidPieceMove(i, j, kingX, kingY, board, previousMove)) {
                        isInCheck = true;
                    }
                }
            }
        }

        // undo the en passant move and restore the state
        board[startX][startY] = movingPiece;
        board[endX][endY] = nullptr;
        board[capturedPawnX][capturedPawnY] = enPassantCapturedPawn;
        return !isInCheck;
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
                if (board[i][j]->isValidPieceMove(i, j, kingX, kingY, board, previousMove)) {
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
        // std::cout << "Outside of grid bounds!" << std::endl; 
        return false;
    }
    // check that piece moved is not empty or other players
    if (board[startX][startY] == nullptr || board[startX][startY]->getColor() != currentPlayer) {
        // std::cout << "Invalid piece selection!" << std::endl;
        return false;
    }
    // check if captured piece is either nothing, or the other player
    if (board[endX][endY] != nullptr && board[endX][endY]->getColor() == currentPlayer) {
        // std::cout << "Cannot capture own piece!" << std::endl;
        return false;
    }
    // check if piece selected can move there (using its own overriden method)
    if (!board[startX][startY]->isValidPieceMove(startX, startY, endX, endY, board, previousMove)) {
        // std::cout << "Invalid move!" << std::endl;
        return false;
    }
    // now that the move is pseudolegal, check if it is actually legal
    // i.e. that it doesnt put king in check
    if (!isLegalMove(startX, startY, endX, endY)) {
        // std::cout << "Not a legal move!" << std::endl;
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
    } else if (type == "Pawn" &&
        get<1>(previousMove) == get<3>(previousMove) &&
        abs(get<2>(previousMove) - get<0>(previousMove)) == 2 &&
        abs(startY - get<1>(previousMove)) == 1 &&
        startX == get<2>(previousMove) &&
        endX == get<2>(previousMove) + 
            ((board[get<2>(previousMove)][get<3>(previousMove)]->getColor() == 'B') ? -1 : 1) &&
        endY == get<3>(previousMove)) {
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
            if (board[startX][startY]->isValidPieceMove(startX, startY, i, j, board, previousMove) && isLegalMove(startX, startY, i, j)) {
                // Check legality and revert the move
                // std::cout << startX << " " << startY << "->" << i << " " << j << std::endl;
                legalMoves.emplace_back(i, j);
            }
        }
    }
    return legalMoves;
}

// backtracking performance testing for move path enumeratin
long long Board::perft(int depth, char currentPlayer, long long& captureCount) {
    // base case: one position at depth 0
    if (depth == 0) {
        return 1;
    }

    long long nodes = 0;

    // generate all legal moves for the current player
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = getPieceAt(i, j);
            if (piece && piece->getColor() == currentPlayer) {
                std::vector<std::pair<int, int>> legalMoves = getLegalMoves(i, j, currentPlayer);
                for (const auto& move : legalMoves) {
                    // make the move
                    Piece* capturedPiece = board[move.first][move.second];
                    bool isCapture = (capturedPiece != nullptr);
                    board[move.first][move.second] = board[i][j];
                    board[i][j] = nullptr;

                    // update the position if king moves
                    if (board[move.first][move.second]->getType() == "King") {
                        if (currentPlayer == 'W') whiteKing = {move.first, move.second};
                        else blackKing = {move.first, move.second};
                    }

                    // increment capture count if this move is a capture
                    if (isCapture) {
                        captureCount++;
                    }

                    // recurse to the next depth
                    nodes += perft(depth - 1, currentPlayer == 'W' ? 'B' : 'W', captureCount);

                    // undo the move (backtrack)
                    board[i][j] = board[move.first][move.second];
                    board[move.first][move.second] = capturedPiece;
                    if (board[i][j]->getType() == "King") {
                        if (currentPlayer == 'W') whiteKing = {i, j};
                        else blackKing = {i, j};
                    }
                }
            }
        }
    }

    return nodes;
}