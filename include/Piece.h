#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
using namespace std;

class Piece {
protected:
    // 'W' for white, 'B' for black
    char color;

public:
    Piece(char color) : color(color) {};
    // deep copying
    virtual Piece* clone() const = 0;
    virtual ~Piece() = default; 

    char getColor() const { 
        return color;
    }

    virtual bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board, tuple<int, int, int, int> previousMove) const = 0;
    virtual string getType() const = 0;
    virtual void makeMove() {};
};

#endif
