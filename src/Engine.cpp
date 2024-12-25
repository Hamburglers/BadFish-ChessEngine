#include <utility>
#include "Engine.h"
#include "Piece.h"
#include "King.h"
#include <iostream>

#define DEPTH 3

Engine::Engine(Board& board) : board(board) {}

// finds the best move
std::pair<std::pair<int, int>, std::pair<int, int>> Engine::getBestMove(char currentPlayer) {
    // initial best value
    int bestValue = (currentPlayer == 'W') ? -1000000 : 1000000;
    // move to return
    std::pair<std::pair<int, int>, std::pair<int, int>> bestMove;

    // iterate through all possible moves
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board.board[i][j];
            if (piece && piece->getColor() == currentPlayer) {
                // Generate legal moves for the piece
                std::vector<std::pair<int, int>> legalMoves = board.getLegalMoves(i, j, currentPlayer);
                for (const auto& move : legalMoves) {
                    int eval;
                    moveAndUnmove(i, j, move.first, move.second, eval, DEPTH, currentPlayer);
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
            if (board.board[i][j]) {
                Piece* piece = board.board[i][j];
                if (piece->getType() == "King") {
                    continue;
                }
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

int Engine::evaluatePosition(int depth, char currentPlayer) {
    if (depth > 0) {
        return minimax(depth - 1, (currentPlayer == 'W') ? 'B' : 'W', -1000000, 1000000);
    }
    return evaluate();
}

int Engine::minimax(int depth, char currentPlayer, int alpha, int beta) {
    // Base case: if depth is 0
    if (depth == 0) {
        return evaluate(); // Evaluate the board at the current state
    }
    if (currentPlayer == 'W') { // Maximizing player
        int maxEval = -1000000;
        for (int startX = 0; startX < 8; startX++) {
            for (int startY = 0; startY < 8; startY++) {
                Piece* piece = board.board[startX][startY];
                if (piece && piece->getColor() == 'W') {
                    // Get legal moves for the piece
                    std::vector<std::pair<int, int>> legalMoves = board.getLegalMoves(startX, startY, 'W');
                    for (const auto& move : legalMoves) {
                        int eval;
                        if (moveAndUnmove(startX, startY, move.first, move.second, eval, depth, currentPlayer)) {
                            maxEval = std::max(maxEval, eval);
                            alpha = std::max(alpha, maxEval);
                            if (beta <= alpha) { // Alpha-beta pruning
                                return maxEval;
                            }
                        }
                    }
                }
            }
        }
        return maxEval;
    } else { // Minimizing player
        int minEval = 1000000;
        for (int startX = 0; startX < 8; startX++) {
            for (int startY = 0; startY < 8; startY++) {
                Piece* piece = board.board[startX][startY];
                if (piece && piece->getColor() == 'B') {
                    // Get legal moves for the piece
                    std::vector<std::pair<int, int>> legalMoves = board.getLegalMoves(startX, startY, 'B');
                    for (const auto& move : legalMoves) {
                        int eval;
                        if (moveAndUnmove(startX, startY, move.first, move.second, eval, depth, currentPlayer)) {
                            minEval = std::min(minEval, eval);
                            beta = std::min(beta, minEval);
                            if (beta <= alpha) { // Alpha-beta pruning
                                return minEval;
                            }
                        }
                    }
                }
            }
        }
        return minEval;
    }
}

bool Engine::moveAndUnmove(int startX, int startY, int endX, int endY, int &eval, int depth, char currentPlayer, bool flag) {
    // Backup the current state
    Piece* movingPiece = board.board[startX][startY];
    Piece* capturedPiece = board.board[endX][endY];
    Piece* enPassantCapturedPawn = nullptr;

    // castling logic, recursively calls thrice
    if (!flag && movingPiece->getType() == "King") {
        King* kingPiece = static_cast<King*>(movingPiece);
        // if both king and rook have not moved
        if (kingPiece->checkPseudoCastle(endX, endY, board.board)) {
            int dx = endX - startX;
            int dy = endY - startY;
            // castling
            if (dx == 0 && abs(dy) == 2) {
                bool result;
                if (dy > 0) {
                    // king side castle, need to check 2 squares
                    result = board.isLegalMove(startX, startY, startX, startY, true) &&
                        board.board[startX][startY+1] == nullptr &&
                        board.board[startX][startY+2] == nullptr &&
                        board.isLegalMove(startX, startY, endX, startY + 1, true) &&
                        board.isLegalMove(startX, startY, endX, endY, true);
                } else {
                    // queen side castle, need to check 3 squares
                    result = board.isLegalMove(startX, startY, startX, startY, true) &&
                        board.board[startX][startY-1] == nullptr &&
                        board.board[startX][startY-2] == nullptr &&
                        board.board[startX][startY-3] == nullptr &&
                        board.isLegalMove(startX, startY, endX, endY - 2, true) &&
                        board.isLegalMove(startX, startY, endX, endY - 1, true) &&
                        board.isLegalMove(startX, startY, endX, endY, true);
                }
                if (result) {
                    auto [rookX, rookY] = kingPiece->getRookPosition(endX, endY, board.board);
                    Piece* rook = board.board[rookX][rookY];

                    // Move rook temporarily
                    if (dy > 0) { // Kingside
                        board.board[rookX][rookY - 2] = rook;
                        board.board[rookX][rookY] = nullptr;
                    } else { // Queenside
                        board.board[rookX][rookY + 3] = rook;
                        board.board[rookX][rookY] = nullptr;
                    }

                    // Move king temporarily
                    board.board[endX][endY] = movingPiece;
                    board.board[startX][startY] = nullptr;

                    // Update king's position
                    if (currentPlayer == 'W') {
                        board.whiteKing = {endX, endY};
                    } else {
                        board.blackKing = {endX, endY};
                    }

                    // special move and unmove for castling
                    // Call minimax or evaluate the position
                    eval = evaluatePosition(depth, currentPlayer);

                    // Undo king move
                    board.board[startX][startY] = movingPiece;
                    board.board[endX][endY] = nullptr;

                    // Undo rook move
                    if (dy > 0) { // Kingside
                        board.board[rookX][rookY] = rook;
                        board.board[rookX][rookY - 2] = nullptr;
                    } else { // Queenside
                        board.board[rookX][rookY] = rook;
                        board.board[rookX][rookY + 3] = nullptr;
                    }

                    // Restore king's position
                    if (currentPlayer == 'W') {
                        board.whiteKing = {startX, startY};
                    } else {
                        board.blackKing = {startX, startY};
                    }

                    return result;
                }
            }
        }
    }

    // if enpassant
    if (movingPiece->getType() == "Pawn" &&
        get<1>(board.previousMove) == get<3>(board.previousMove) &&
        abs(get<2>(board.previousMove) - get<0>(board.previousMove)) == 2 &&
        abs(startY - get<1>(board.previousMove)) == 1 &&
        startX == get<2>(board.previousMove) &&
        endX == get<2>(board.previousMove) + 
            ((board.board[get<2>(board.previousMove)][get<3>(board.previousMove)]->getColor() == 'B') ? -1 : 1) &&
        endY == get<3>(board.previousMove)) {
        // previous pawn's end rank
        int capturedPawnX = get<2>(board.previousMove); 
        // previous pawn's file/column
        int capturedPawnY = get<3>(board.previousMove);

        // temporarily remove the en passant captured pawn
        enPassantCapturedPawn = board.board[capturedPawnX][capturedPawnY];
        board.board[capturedPawnX][capturedPawnY] = nullptr;

        // move the current pawn to its destination
        board.board[endX][endY] = movingPiece;
        board.board[startX][startY] = nullptr;

        // check if the king is in check after this move
        char currentPlayer = movingPiece->getColor();
        auto [kingX, kingY] = (currentPlayer == 'W') ? board.whiteKing : board.blackKing;
        bool isInCheck = false;

        // Check if the current player's king is in check
        for (int i = 0; i < 8 && !isInCheck; i++) {
            for (int j = 0; j < 8 && !isInCheck; j++) {
                if (board.board[i][j] && board.board[i][j]->getColor() != currentPlayer) {
                    // Check if the opponent piece can attack the king
                    if (board.board[i][j]->isValidPieceMove(i, j, kingX, kingY, board.board)) {
                        isInCheck = true;
                    }
                }
            }
        }

        // special move and unmove for enpassant
        // If valid, call minimax for the next depth
        eval = evaluatePosition(depth, currentPlayer);

        // undo the en passant move and restore the state
        board.board[startX][startY] = movingPiece;
        board.board[endX][endY] = nullptr;
        board.board[capturedPawnX][capturedPawnY] = enPassantCapturedPawn;
        return !isInCheck;
    }

    // Make the move temporarily
    board.board[endX][endY] = movingPiece;
    board.board[startX][startY] = nullptr;

    // Update king position if the moved piece is a king
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            board.whiteKing = {endX, endY};
        } else {
            board.blackKing = {endX, endY};
        }
    }

    // Check if the current player's king is in check
    bool isInCheck = false;
    auto [kingX, kingY] = (currentPlayer == 'W') ? board.whiteKing : board.blackKing;
    for (int i = 0; i < 8 && !isInCheck; i++) {
        for (int j = 0; j < 8 && !isInCheck; j++) {
            if (board.board[i][j] && board.board[i][j]->getColor() != currentPlayer) {
                // Check if the opponent piece can attack the king
                if (board.board[i][j]->isValidPieceMove(i, j, kingX, kingY, board.board)) {
                    isInCheck = true;
                }
            }
        }
    }
    
    // normal move and unmove
    // If valid, call minimax for the next depth
    eval = evaluatePosition(depth, currentPlayer);
    
    // Undo the move to restore the original board state
    board.board[startX][startY] = movingPiece;
    board.board[endX][endY] = capturedPiece;

    // Restore king's position if it was moved
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            board.whiteKing = {startX, startY};
        } else {
            board.blackKing = {startX, startY};
        }
    }
    return !isInCheck;
}