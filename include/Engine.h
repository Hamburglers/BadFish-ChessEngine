#ifndef ENGINE_H
#define ENGINE_H

#include <Board.h>
#include <unordered_map>

class Engine : private Board {
private:
    unordered_map<std::string, int> pieceValues = {{"Pawn", 100}, 
                                                    {"Knight", 300}, 
                                                    {"Bishop", 300},
                                                    {"Rook", 500},
                                                    {"Queen", 900}};
    char color;
public:
    Engine(char color);
    ~Engine();
    std::pair<std::pair<int, int>, std::pair<int, int>> getBestMove(char currentPlayer);
    int evaluate() const;
    bool moveAndUnmove(int startX, int startY, int endX, int endY, bool flag=false, int &eval, int depth, char currentPlayer);
    int evaluatePosition(int depth, char currentPlayer) const;
};

#endif
