#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>
#include "Board.h"

int main() {
    Board board;
    // board.initialise();
    board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // 1st run
    // Perft Test - Depth: 0 - Nodes: 1 - Captures: 0 - Time: 8.4e-08 seconds
    // Perft Test - Depth: 1 - Nodes: 20 - Captures: 0 - Time: 8.175e-05 seconds
    // Perft Test - Depth: 2 - Nodes: 400 - Captures: 0 - Time: 0.00123687 seconds
    // Perft Test - Depth: 3 - Nodes: 9194 - Captures: 34 - Time: 0.0231149 seconds (should be 8902)
    // Perft Test - Depth: 4 - Nodes: 209691 - Captures: 1932 - Time: 0.385174 seconds (should be 197,281, 1576)
    for (int depth = 0; depth < 5; depth++) {
        char startingPlayer = 'W';
        long long captureCount = 0;
        auto start = std::chrono::high_resolution_clock::now();
        long long nodes = board.perft(depth, startingPlayer, captureCount);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Perft Test - Depth: " << depth 
            << " - Nodes: " << nodes 
            << " - Captures: " << captureCount
            << " - Time: " << elapsed.count() << " seconds" 
            << std::endl;
    }

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Chess");
    sf::Texture piecesTexture;
    if (!piecesTexture.loadFromFile("Chess_Pieces_Sprite.svg.png")) {
        std::cerr << "Error loading Chess_Pieces_Sprite.svg.png" << std::endl;
        return 1;
    }

    sf::Sprite pieceSprite(piecesTexture);
    const int squareSize = 100;
    pieceSprite.setScale(squareSize / 427.0f, squareSize / 427.0f);

    std::unordered_map<std::string, sf::IntRect> pieceMap = {
        {"KingW", sf::IntRect(0 * 427, 0, 427, 427)},
        {"QueenW", sf::IntRect(1 * 427, 0, 427, 427)},
        {"BishopW", sf::IntRect(2 * 427, 0, 427, 427)},
        {"KnightW", sf::IntRect(3 * 427, 0, 427, 427)},
        {"RookW", sf::IntRect(4 * 427, 0, 427, 427)},
        {"PawnW", sf::IntRect(5 * 427, 0, 427, 427)},
        {"KingB", sf::IntRect(0 * 427, 427, 427, 427)},
        {"QueenB", sf::IntRect(1 * 427, 427, 427, 427)},
        {"BishopB", sf::IntRect(2 * 427, 427, 427, 427)},
        {"KnightB", sf::IntRect(3 * 427, 427, 427, 427)},
        {"RookB", sf::IntRect(4 * 427, 427, 427, 427)},
        {"PawnB", sf::IntRect(5 * 427, 427, 427, 427)},
    };

    char currentPlayer = 'W';
    int selectedX = -1, selectedY = -1;
    std::vector<std::pair<int, int>> legalMoves;
    Piece* selectedPiece = nullptr;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x / squareSize;
                    int y = event.mouseButton.y / squareSize;

                    if (selectedX == -1 && selectedY == -1) {
                        // Selecting a piece
                        selectedPiece = board.getPieceAt(y, x);
                        if (selectedPiece && board.getPieceColor(y, x) == currentPlayer) {
                            selectedX = x;
                            selectedY = y;
                            legalMoves = board.getLegalMoves(y, x, currentPlayer); // Calculate legal moves
                        }
                    } else {
                        // Moving a piece
                        auto moveIt = std::find(legalMoves.begin(), legalMoves.end(), std::make_pair(y, x));
                        if (moveIt != legalMoves.end()) {
                            // Move the piece only if the clicked square is a legal move
                            if (board.movePiece(selectedY, selectedX, y, x, currentPlayer)) {
                                currentPlayer = (currentPlayer == 'W' ? 'B' : 'W');
                            }
                        }
                        // Clear selection after moving (or failing to move)
                        selectedX = selectedY = -1;
                        legalMoves.clear();
                    }
                }
            }
        }

        window.clear();
        sf::Color cream(240, 217, 181);
        sf::Color brown(181, 136, 99);
        sf::Color highlight(50, 205, 50, 128); // Semi-transparent green

        // Render board
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
                square.setPosition(j * squareSize, i * squareSize);
                square.setFillColor((i + j) % 2 == 0 ? cream : brown);
                window.draw(square);
                Piece* piece = board.getPieceAt(i, j);
                if (piece) {
                    std::string key = piece->getType() + std::string(1, piece->getColor());
                    if (pieceMap.find(key) != pieceMap.end()) {
                        pieceSprite.setTextureRect(pieceMap[key]);
                        pieceSprite.setPosition(j * squareSize, i * squareSize);
                        window.draw(pieceSprite);
                    }
                }
            }
        }
        // Highlight legal moves
        for (auto& move : legalMoves) {
            sf::RectangleShape highlightSquare(sf::Vector2f(squareSize, squareSize));
            highlightSquare.setPosition(move.second * squareSize, move.first * squareSize);
            highlightSquare.setFillColor(highlight);
            window.draw(highlightSquare);
        }
        window.display();
        // incorrect, should loop through entire grid, checking for legal modes and checkmate if all is empty
        // if (currentPlayer == 'B') {
        //     auto [bx, by] = board.getBlackKing();
        //     if (!board.isLegalMove(bx, by, bx, by) && board.getLegalMoves(bx, by, 'B').empty()) {
        //         std::cout << "White wins by Checkmate!\nPress <Enter> to quit" << std::endl;
        //         std::string dummy;
        //         getline(std::cin, dummy);
        //         return 0; 
        //     }
        // } else if (currentPlayer == 'W') {
        //     auto [wx, wy] = board.getWhiteKing();
        //     std::cout << wx << " " << wy << std::endl;
        //     if (!board.isLegalMove(wx, wy, wx, wy) && board.getLegalMoves(wx, wy, 'W').empty()) {
        //         std::cout << "Black wins by Checkmate!\nPress <Enter> to quit" << std::endl;
        //         std::string dummy;
        //         getline(std::cin, dummy);
        //         return 0;
        //     }
        // }
    }

    return 0;
}
