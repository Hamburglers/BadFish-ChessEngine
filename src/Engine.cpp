#include <utility>
#include "Engine.h"
#include "Piece.h"
#include "King.h"

#define DEPTH 5

Engine::Engine(char color) : color(color) {}

// finds the best move
std::pair<std::pair<int, int>, std::pair<int, int>> Engine::getBestMove(char currentPlayer) {
    // initial best value
    int bestValue = (currentPlayer == 'W') ? -1000000 : 1000000;
    // move to return
    std::pair<std::pair<int, int>, std::pair<int, int>> bestMove;

    // iterate through all possible moves
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board[i][j];
            if (piece && piece->getColor() == currentPlayer) {
                // Generate legal moves for the piece
                std::vector<std::pair<int, int>> legalMoves = getLegalMoves(i, j, currentPlayer);
                for (const auto& move : legalMoves) {
                    int eval;
                    moveAndUnmove(i, j, move.first, move.second, false, eval, DEPTH, currentPlayer);
                    if ((currentPlayer == 'W' && eval > bestValue) ||
                        (currentPlayer == 'B' && eval < bestValue)) {
                        bestValue = eval;
                        bestMove = {{i, j}, move};
                    }
                }
            }
        }
    }

    return bestMove;
}

int Engine::evaluate() const {
    int whiteEval{};
    int blackEval{};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]) {
                Piece* piece = board[i][j];
                if (piece->getColor() == 'W') {
                    whiteEval += pieceValues.find(piece->getType())->second;
                } else {
                    blackEval += pieceValues.find(piece->getType())->second;
                }
            }
        }
    }
    return whiteEval - blackEval;
}

int Engine::evaluatePosition(int depth, char currentPlayer) const {
    if (depth > 0) {
        return minimax(depth - 1, (currentPlayer == 'W') ? 'B' : 'W', -1000000, 1000000);
    }
    return evaluate();
}

bool Engine::moveAndUnmove(int startX, int startY, int endX, int endY, bool flag, int &eval, int depth, char currentPlayer) {
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
                bool result;
                if (dy > 0) {
                    // king side castle, need to check 2 squares
                    result = isLegalMove(startX, startY, startX, startY, true) &&
                        board[startX][startY+1] == nullptr &&
                        board[startX][startY+2] == nullptr &&
                        isLegalMove(startX, startY, endX, startY + 1, true) &&
                        isLegalMove(startX, startY, endX, endY, true);
                } else {
                    // queen side castle, need to check 3 squares
                    result = isLegalMove(startX, startY, startX, startY, true) &&
                        board[startX][startY-1] == nullptr &&
                        board[startX][startY-2] == nullptr &&
                        board[startX][startY-3] == nullptr &&
                        isLegalMove(startX, startY, endX, endY - 2, true) &&
                        isLegalMove(startX, startY, endX, endY - 1, true) &&
                        isLegalMove(startX, startY, endX, endY, true);
                }
                if (result) {
                    auto [rookX, rookY] = kingPiece->getRookPosition(endX, endY, board);
                    Piece* rook = board[rookX][rookY];

                    // Move rook temporarily
                    if (dy > 0) { // Kingside
                        board[rookX][rookY - 2] = rook;
                        board[rookX][rookY] = nullptr;
                    } else { // Queenside
                        board[rookX][rookY + 3] = rook;
                        board[rookX][rookY] = nullptr;
                    }

                    // Move king temporarily
                    board[endX][endY] = movingPiece;
                    board[startX][startY] = nullptr;

                    // Update king's position
                    if (currentPlayer == 'W') {
                        whiteKing = {endX, endY};
                    } else {
                        blackKing = {endX, endY};
                    }

                    // special move and unmove for castling
                    // Call minimax or evaluate the position
                    eval = evaluatePosition(depth, currentPlayer);

                    // Undo king move
                    board[startX][startY] = movingPiece;
                    board[endX][endY] = nullptr;

                    // Undo rook move
                    if (dy > 0) { // Kingside
                        board[rookX][rookY] = rook;
                        board[rookX][rookY - 2] = nullptr;
                    } else { // Queenside
                        board[rookX][rookY] = rook;
                        board[rookX][rookY + 3] = nullptr;
                    }

                    // Restore king's position
                    if (currentPlayer == 'W') {
                        whiteKing = {startX, startY};
                    } else {
                        blackKing = {startX, startY};
                    }

                    return result;
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
                    if (board[i][j]->isValidPieceMove(i, j, kingX, kingY, board)) {
                        isInCheck = true;
                    }
                }
            }
        }

        // special move and unmove for enpassant
        // If valid, call minimax for the next depth
        eval = evaluatePosition(depth, currentPlayer);

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
                if (board[i][j]->isValidPieceMove(i, j, kingX, kingY, board)) {
                    isInCheck = true;
                }
            }
        }
    }
    
    // normal move and unmove
    // If valid, call minimax for the next depth
    eval = evaluatePosition(depth, currentPlayer);
    
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