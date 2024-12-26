#include <utility>
#include <iostream>
#include <mutex>
#include <thread>
#include "Engine.h"
#include "Piece.h"
#include "King.h"
#include "PieceValue.h"

#define DEPTH 4
#define ENDGAME_THRESHOLD 14

Engine::Engine(Board& board) : board(board) {}

// finds the best move
std::pair<std::pair<int, int>, std::pair<int, int>> Engine::getBestMove(char currentPlayer) {
    // initial best value
    int bestValue = (currentPlayer == 'W') ? -1000000 : 1000000;
    // move to return
    std::pair<std::pair<int, int>, std::pair<int, int>> bestMove;
    std::vector<std::tuple<int, int, std::pair<int, int>>> moves;

    std::mutex bestMoveMutex;
    // iterate through all possible moves
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board.board[i][j];
            if (piece && piece->getColor() == currentPlayer) {
                // Generate legal moves for the piece
                auto legalMoves = board.getLegalMoves(i, j, currentPlayer);
                for (const auto& move : legalMoves) {
                    moves.emplace_back(i, j, move);
                }
            }
        }
    }

    // evaluate a subset of moves
    auto evaluateMoves = [&](const std::vector<std::tuple<int, int, std::pair<int, int>>>& movesSubset, Board threadLocalBoard) {
        int localBestValue = bestValue;
        std::pair<std::pair<int, int>, std::pair<int, int>> localBestMove;

        for (const auto& [startX, startY, end] : movesSubset) {
            int eval;
            if (moveAndUnmove(startX, startY, end.first, end.second, eval, DEPTH, currentPlayer, threadLocalBoard)) {
                if ((currentPlayer == 'W' && eval > localBestValue) ||
                    (currentPlayer == 'B' && eval < localBestValue)) {
                    localBestValue = eval;
                    localBestMove = {{startX, startY}, end};
                }
            }
        }

        std::lock_guard<std::mutex> lock(bestMoveMutex);
        if ((currentPlayer == 'W' && localBestValue > bestValue) ||
            (currentPlayer == 'B' && localBestValue < bestValue)) {
            bestValue = localBestValue;
            bestMove = localBestMove;
        }
    };
    // divide the moves into chunks for each thread

    // Goes from: (1 thread only)
    // Spent 3.53503s
    // Spent 6.25526s
    // Spent 10.4587s

    // to: (12 threads on my cpu)
    // Spent 0.922531s
    // Spent 1.29316s
    // Spent 1.52756s
    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    size_t chunkSize = std::max((moves.size() + numThreads - 1) / numThreads, size_t(1));

    for (size_t i = 0; i < moves.size(); i += chunkSize) {
        std::vector<std::tuple<int, int, std::pair<int, int>>> movesSubset(
            moves.begin() + i,
            moves.begin() + std::min(i + chunkSize, moves.size())
        );
        threads.emplace_back([&, movesSubset, threadLocalBoard = board]() {
            evaluateMoves(movesSubset, threadLocalBoard);
        });
    }
    // join all threads
    for (auto& thread : threads) {
        thread.join();
    }
    return bestMove;
}

int Engine::evaluate(Board& threadLocalBoard) const {
    int whiteEval{};
    int blackEval{};
    int nonPawnMaterial{};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (threadLocalBoard.board[i][j]) {
                Piece* piece = threadLocalBoard.board[i][j];

                // check if king under attack
                // if (piece->getType() == "King") {
                //     if (piece->getColor() == 'W') {
                //         auto [kingX, kingY] = threadLocalBoard.whiteKing;
                //         auto legalMoves = threadLocalBoard.getLegalMoves(i, j, 'W');
                //         if (!legalMoves.empty()) {
                //             if (piece->isValidPieceMove(i, j, kingX, kingY, threadLocalBoard.board, threadLocalBoard.previousMove)) {
                //                 return std::numeric_limits<int>::max(); 
                //             }
                //         }
                //     } else {
                //         auto [kingX, kingY] = threadLocalBoard.blackKing;
                //         auto legalMoves = threadLocalBoard.getLegalMoves(i, j, 'B');
                //         if (!legalMoves.empty()) {
                //            if (piece->isValidPieceMove(i, j, kingX, kingY, threadLocalBoard.board, threadLocalBoard.previousMove)) {
                //                 return std::numeric_limits<int>::min(); 
                //             } 
                //         }
                //     }
                // }

                int row = (piece->getColor() == 'W') ? i : 7 - i;
                int col = j;

                int pieceValue = pieceValues.find(piece->getType())->second;

                // add positional value from piece-square table
                if (piece->getType() == "Pawn") {
                    pieceValue += pawnTable[row][col];

                    // White pawn on 7th rank
                    if (piece->getColor() == 'W' && row == 6) {
                        // add queen bonus
                        pieceValue += 900; 
                    // Black pawn on 2nd rank
                    } else if (piece->getColor() == 'B' && row == 1) {
                        // add queen bonus
                        pieceValue += 900; 
                    }
                } else if (piece->getType() == "Knight") {
                    pieceValue += knightTable[row][col];
                } else if (piece->getType() == "Bishop") {
                    pieceValue += bishopTable[row][col];
                } else if (piece->getType() == "Rook") {
                    pieceValue += rookTable[row][col];
                } else if (piece->getType() == "Queen") {
                    pieceValue += queenTable[row][col];
                } else if (piece->getType() == "King") {
                    if (nonPawnMaterial <= ENDGAME_THRESHOLD) {
                        pieceValue += kingEndGameTable[row][col];
                    } else {
                        pieceValue += kingMiddleGameTable[row][col];
                    }
                }

                if (piece->getColor() == 'W') {
                    whiteEval += pieceValue;
                } else {
                    blackEval += pieceValue;
                }

                // track non-pawn material for endgame determination
                if (piece->getType() != "Pawn") {
                    nonPawnMaterial += pieceValues.find(piece->getType())->second;
                }

            }
        }
    }
    return whiteEval - blackEval;
}

int Engine::evaluatePosition(int depth, char currentPlayer, Board& threadLocalBoard) {
    if (depth > 0) {
        return minimax(depth - 1, (currentPlayer == 'W') ? 'B' : 'W', -1000000, 1000000, threadLocalBoard);
    }
    return evaluate(threadLocalBoard);
}

int Engine::minimax(int depth, char currentPlayer, int alpha, int beta, Board& threadLocalBoard) {
    // base case: if depth is 0
    if (depth == 0) {
        return evaluate(threadLocalBoard);
    }
    // maximizing player
    if (currentPlayer == 'W') {
        int maxEval = -1000000;
        for (int startX = 0; startX < 8; startX++) {
            for (int startY = 0; startY < 8; startY++) {
                Piece* piece = threadLocalBoard.board[startX][startY];
                if (piece && piece->getColor() == 'W') {
                    // get legal moves for the piece
                    std::vector<std::pair<int, int>> legalMoves = threadLocalBoard.getLegalMoves(startX, startY, 'W');
                    for (const auto& move : legalMoves) {
                        int eval;
                        if (moveAndUnmove(startX, startY, move.first, move.second, eval, depth, currentPlayer, threadLocalBoard)) {
                            maxEval = std::max(maxEval, eval);
                            alpha = std::max(alpha, maxEval);
                            // Alpha-beta pruning
                            if (beta <= alpha) {
                                return maxEval;
                            }
                        }
                    }
                }
            }
        }
        return maxEval;
    } else {
        // minimizing player
        int minEval = 1000000;
        for (int startX = 0; startX < 8; startX++) {
            for (int startY = 0; startY < 8; startY++) {
                Piece* piece = threadLocalBoard.board[startX][startY];
                if (piece && piece->getColor() == 'B') {
                    // Get legal moves for the piece
                    std::vector<std::pair<int, int>> legalMoves = threadLocalBoard.getLegalMoves(startX, startY, 'B');
                    for (const auto& move : legalMoves) {
                        int eval;
                        if (moveAndUnmove(startX, startY, move.first, move.second, eval, depth, currentPlayer, threadLocalBoard)) {
                            minEval = std::min(minEval, eval);
                            beta = std::min(beta, minEval);
                            // Alpha-beta pruning
                            if (beta <= alpha) {
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

bool Engine::moveAndUnmove(int startX, int startY, int endX, int endY, int &eval, int depth, char currentPlayer, Board& threadLocalBoard, bool flag) {
    // backup the current state
    Piece* movingPiece = threadLocalBoard.board[startX][startY];
    Piece* capturedPiece = threadLocalBoard.board[endX][endY];
    Piece* enPassantCapturedPawn = nullptr;

    // castling logic, recursively calls thrice
    if (!flag && movingPiece->getType() == "King") {
        King* kingPiece = static_cast<King*>(movingPiece);
        // if both king and rook have not moved
        if (kingPiece->checkPseudoCastle(endX, endY, threadLocalBoard.board)) {
            int dx = endX - startX;
            int dy = endY - startY;
            // castling
            if (dx == 0 && abs(dy) == 2) {
                bool result;
                if (dy > 0) {
                    // king side castle, need to check 2 squares
                    result = threadLocalBoard.isLegalMove(startX, startY, startX, startY, true) &&
                        threadLocalBoard.board[startX][startY+1] == nullptr &&
                        threadLocalBoard.board[startX][startY+2] == nullptr &&
                        threadLocalBoard.isLegalMove(startX, startY, endX, startY + 1, true) &&
                        threadLocalBoard.isLegalMove(startX, startY, endX, endY, true);
                } else {
                    // queen side castle, need to check 3 squares
                    result = threadLocalBoard.isLegalMove(startX, startY, startX, startY, true) &&
                        threadLocalBoard.board[startX][startY-1] == nullptr &&
                        threadLocalBoard.board[startX][startY-2] == nullptr &&
                        threadLocalBoard.board[startX][startY-3] == nullptr &&
                        threadLocalBoard.isLegalMove(startX, startY, endX, startY - 2, true) &&
                        threadLocalBoard.isLegalMove(startX, startY, endX, startY - 1, true) &&
                        threadLocalBoard.isLegalMove(startX, startY, endX, endY, true);
                }
                if (result) {
                    auto [rookX, rookY] = kingPiece->getRookPosition(endX, endY, threadLocalBoard.board);
                    Piece* rook = threadLocalBoard.board[rookX][rookY];

                    // move rook temporarily
                    if (dy > 0) { // kingside
                        threadLocalBoard.board[rookX][rookY - 2] = rook;
                        threadLocalBoard.board[rookX][rookY] = nullptr;
                    } else { // queenside
                        threadLocalBoard.board[rookX][rookY + 3] = rook;
                        threadLocalBoard.board[rookX][rookY] = nullptr;
                    }

                    // move king temporarily
                    threadLocalBoard.board[endX][endY] = movingPiece;
                    threadLocalBoard.board[startX][startY] = nullptr;

                    // update king's position
                    if (currentPlayer == 'W') {
                        threadLocalBoard.whiteKing = {endX, endY};
                    } else {
                        threadLocalBoard.blackKing = {endX, endY};
                    }

                    // special move and unmove for castling
                    // call minimax or evaluate the position
                    eval = evaluatePosition(depth, currentPlayer, threadLocalBoard);

                    // undo king move
                    threadLocalBoard.board[startX][startY] = movingPiece;
                    threadLocalBoard.board[endX][endY] = nullptr;

                    // undo rook move
                    if (dy > 0) { // kingside
                        threadLocalBoard.board[rookX][rookY] = rook;
                        threadLocalBoard.board[rookX][rookY - 2] = nullptr;
                    } else { // queenside
                        threadLocalBoard.board[rookX][rookY] = rook;
                        threadLocalBoard.board[rookX][rookY + 3] = nullptr;
                    }

                    // restore king's position
                    if (currentPlayer == 'W') {
                        threadLocalBoard.whiteKing = {startX, startY};
                    } else {
                        threadLocalBoard.blackKing = {startX, startY};
                    }

                    return result;
                }
            }
        }
    }
    // if enpassant
    if (movingPiece->getType() == "Pawn" &&
        get<1>(threadLocalBoard.previousMove) == get<3>(threadLocalBoard.previousMove) &&
        abs(get<2>(threadLocalBoard.previousMove) - get<0>(threadLocalBoard.previousMove)) == 2 &&
        abs(startY - get<1>(threadLocalBoard.previousMove)) == 1 &&
        startX == get<2>(threadLocalBoard.previousMove) &&
        endX == get<2>(threadLocalBoard.previousMove) + 
            ((threadLocalBoard.board[get<2>(threadLocalBoard.previousMove)][get<3>(threadLocalBoard.previousMove)]->getColor() == 'B') ? -1 : 1) &&
        endY == get<3>(threadLocalBoard.previousMove)) {
        // previous pawn's end rank
        int capturedPawnX = get<2>(threadLocalBoard.previousMove); 
        // previous pawn's file/column
        int capturedPawnY = get<3>(threadLocalBoard.previousMove);

        // temporarily remove the en passant captured pawn
        enPassantCapturedPawn = threadLocalBoard.board[capturedPawnX][capturedPawnY];
        threadLocalBoard.board[capturedPawnX][capturedPawnY] = nullptr;

        // move the current pawn to its destination
        threadLocalBoard.board[endX][endY] = movingPiece;
        threadLocalBoard.board[startX][startY] = nullptr;

        // check if the king is in check after this move
        auto [kingX, kingY] = (currentPlayer == 'W') ? threadLocalBoard.whiteKing : threadLocalBoard.blackKing;
        bool isInCheck = false;

        // check if the current player's king is in check
        for (int i = 0; i < 8 && !isInCheck; i++) {
            for (int j = 0; j < 8 && !isInCheck; j++) {
                if (threadLocalBoard.board[i][j] && threadLocalBoard.board[i][j]->getColor() != currentPlayer) {
                    // Check if the opponent piece can attack the king
                    if (threadLocalBoard.board[i][j]->isValidPieceMove(i, j, kingX, kingY, threadLocalBoard.board, threadLocalBoard.previousMove)) {
                        isInCheck = true;
                    }
                }
            }
        }

        // special move and unmove for enpassant
        // if valid, call minimax for the next depth
        eval = evaluatePosition(depth, currentPlayer, threadLocalBoard);

        // undo the en passant move and restore the state
        threadLocalBoard.board[startX][startY] = movingPiece;
        threadLocalBoard.board[endX][endY] = nullptr;
        threadLocalBoard.board[capturedPawnX][capturedPawnY] = enPassantCapturedPawn;
        return !isInCheck;
    }
    // make the move temporarily
    threadLocalBoard.board[endX][endY] = movingPiece;
    threadLocalBoard.board[startX][startY] = nullptr;

    // update king position if the moved piece is a king
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            threadLocalBoard.whiteKing = {endX, endY};
        } else {
            threadLocalBoard.blackKing = {endX, endY};
        }
    }
    // check if the current player's king is in check
    bool isInCheck = false;
    auto [kingX, kingY] = (currentPlayer == 'W') ? threadLocalBoard.whiteKing : threadLocalBoard.blackKing;
    for (int i = 0; i < 8 && !isInCheck; i++) {
        for (int j = 0; j < 8 && !isInCheck; j++) {
            if (threadLocalBoard.board[i][j] && threadLocalBoard.board[i][j]->getColor() != currentPlayer) {
                // check if the opponent piece can attack the king
                if (threadLocalBoard.board[i][j]->isValidPieceMove(i, j, kingX, kingY, threadLocalBoard.board, threadLocalBoard.previousMove)) {
                    isInCheck = true;
                }
            }
        }
    }
    // normal move and unmove
    // if valid, call minimax for the next depth
    eval = evaluatePosition(depth, currentPlayer, threadLocalBoard);
    
    // undo the move to restore the original board state
    threadLocalBoard.board[startX][startY] = movingPiece;
    threadLocalBoard.board[endX][endY] = capturedPiece;
    // restore king's position if it was moved
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            threadLocalBoard.whiteKing = {startX, startY};
        } else {
            threadLocalBoard.blackKing = {startX, startY};
        }
    }
    return !isInCheck;
}