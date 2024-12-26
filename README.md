# BadFish ChessEngine
Todo:
3. fix up perft test discrepancies 
    -> (will do after making the engine might be easier to find out whats the invalid mode)
4. threefold, 50 move draw
6. actual chess engine stuff
7. create function isSquareUnderAttack rather than keep calling isLegalMove
8. use iterative deepening, transposition table, piece square tables, opening tablebase, endgame

To use Player vs Player
cd build
cmake -DCOMPUTER_MODE=OFF ..
make
./ChessGame

To use Player vs Computer
cd build
cmake -DCOMPUTER_MODE=ON ..
make
./ChessGame
