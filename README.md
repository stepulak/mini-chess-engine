# Mini-chess-engine

- Homemade mini chess engine and game written in C++ (about 1300 C++ LOC) during couple of evenings
- AI uses Negascout algorithm with variable search depth
- Game is just CLI with "UCI-like" interface
- Using Piece square tables (PST) for board evaluation
- Support of en passant move/capture, castling, three fold repetition, check detection, draw detection, etc. - yet still not a full chess game engine like Stockfish
- C++17, CMake, GPL3
- FYI: I am not chess expert, this engine is made just for fun ad curiosity
- Feel free to report a bug