#include "Board.h"
#include <iostream>

int main() {
    Board board;
    board.initialise();
    board.display();

    char currentPlayer = 'W';
    while (true) {
        int startX, startY, endX, endY;
        std::cout << (currentPlayer == 'W' ? "White's turn" : "Black's turn") << std::endl;
        std::cout << "Enter move (startX startY endX endY): ";
        std::cin >> startX >> startY >> endX >> endY;

        if (board.movePiece(startX, startY, endX, endY, currentPlayer)) {
            board.display();
            currentPlayer = (currentPlayer == 'W' ? 'B' : 'W');
        } else {
            std::cout << "Invalid move! Try again." << std::endl;
        }
    }

    return 0;
}
