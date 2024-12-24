#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
#include "Board.h"

int main() {
    Board board;
    board.initialise();

    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess");
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
                        if (board.getPieceAt(y, x) && board.getPieceColor(y, x) == currentPlayer) {
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
    }

    return 0;
}
