const int pawnTable[8][8] = {
    { 0,   5,  5, -10, -10,  5,  5,  0},
    { 5,  10, 10,   0,   0, 10, 10,  5},
    {10,  10, 20,  20,  20, 20, 10, 10},
    {20,  20, 30,  30,  30, 30, 20, 20},
    {30,  30, 40,  40,  40, 40, 30, 30},
    {40,  40, 50,  50,  50, 50, 40, 40},
    {50,  50, 60,  60,  60, 60, 50, 50},
    { 0,   5,  5, -10, -10,  5,  5,  0}
};

const int knightTable[8][8] = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20,   0,   5,   5,   0, -20, -40},
    {-30,   5,  10,  15,  15,  10,   5, -30},
    {-30,   0,  15,  20,  20,  15,   0, -30},
    {-30,   5,  15,  20,  20,  15,   5, -30},
    {-30,   0,  10,  15,  15,  10,   0, -30},
    {-40, -20,   0,   0,   0,   0, -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50}
};

const int bishopTable[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10,   0,   0,   5,   5,   0,   0, -10},
    {-10,   0,  10,  10,  10,  10,   0, -10},
    {-10,   5,   5,  10,  10,   5,   5, -10},
    {-10,   0,   5,  10,  10,   5,   0, -10},
    {-10,  10,  10,   5,   5,  10,  10, -10},
    {-10,   5,   0,   0,   0,   0,   5, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}
};

const int rookTable[8][8] = {
    { 0,   0,   0,   5,   5,   0,   0,   0},
    {-5,   0,   0,   0,   0,   0,   0,  -5},
    {-5,   0,   0,   0,   0,   0,   0,  -5},
    {-5,   0,   0,   0,   0,   0,   0,  -5},
    {-5,   0,   0,   0,   0,   0,   0,  -5},
    {-5,   0,   0,   0,   0,   0,   0,  -5},
    { 5,  10,  10,  10,  10,  10,  10,   5},
    { 0,   0,   0,   5,   5,   0,   0,   0}
};

const int queenTable[8][8] = {
    {-20, -10, -10,  -5,  -5, -10, -10, -20},
    {-10,   0,   0,   0,   0,   0,   0, -10},
    {-10,   0,   5,   5,   5,   5,   0, -10},
    { -5,   0,   5,   5,   5,   5,   0,  -5},
    {  0,   0,   5,   5,   5,   5,   0,  -5},
    {-10,   5,   5,   5,   5,   5,   0, -10},
    {-10,   0,   5,   0,   0,   0,   0, -10},
    {-20, -10, -10,  -5,  -5, -10, -10, -20}
};

const int kingMiddleGameTable[8][8] = {
    { 20,  30,  10,   0,   0,  10,  30,  20},
    { 20,  20,   0,   0,   0,   0,  20,  20},
    {-10, -20, -20, -20, -20, -20, -20, -10},
    {-20, -30, -30, -40, -40, -30, -30, -20},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30}
};

const int kingEndGameTable[8][8] = {
    {-50, -40, -30, -20, -20, -30, -40, -50},
    {-30, -20, -10,   0,   0, -10, -20, -30},
    {-30, -10,  20,  30,  30,  20, -10, -30},
    {-30, -10,  30,  40,  40,  30, -10, -30},
    {-30, -10,  30,  40,  40,  30, -10, -30},
    {-30, -10,  20,  30,  30,  20, -10, -30},
    {-30, -30,   0,   0,   0,   0, -30, -30},
    {-50, -30, -30, -30, -30, -30, -30, -50}
};