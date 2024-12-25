#ifndef ENGINE_H
#define ENGINE_H

#include <Board.h>

class Engine {
public:
    std::pair<std::pair<int, int>, std::pair<int, int>> getBestMove(Board& board, char currentPlayer);
    int evaluate();
};

#endif
