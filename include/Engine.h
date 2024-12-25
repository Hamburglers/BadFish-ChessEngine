#ifndef ENGINE_H
#define ENGINE_H

#include <Board.h>
#include <unordered_map>

class Engine {
private:
    unordered_map<std::string, int> pieceValues = {{"Pawn", 100}, 
                                                    {"Knight", 300}, 
                                                    {"Bishop", 300},
                                                    {"Rook", 500},
                                                    {"Queen", 900}};
    char color;
    Board& board;
public:
    Engine(Board& board);
    std::pair<std::pair<int, int>, std::pair<int, int>> getBestMove(char currentPlayer);
    int evaluate() const;
    bool moveAndUnmove(int startX, int startY, int endX, int endY, int &eval, int depth, char currentPlayer, bool flag=false);
    int evaluatePosition(int depth, char currentPlayer);
    int minimax(int depth, char currentPlayer, int alpha, int beta);
};

#endif
