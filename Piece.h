#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
using namespace std;

class Piece {
protected:
    char color; // 'W' for white, 'B' for black

public:
    Piece(char color) : color(color) {}
    virtual ~Piece() {}

    char getColor() const { 
        return color;
    }

    virtual bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) = 0;
    virtual string getType() const = 0;
};

#endif
