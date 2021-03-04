#pragma once

#include "figures.hpp"
#include "move.hpp"

#include <array>

class Board {
public:
    static constexpr size_t WIDTH = 8u;
    static constexpr size_t HEIGHT = 8u;
    static constexpr size_t SIZE = WIDTH * HEIGHT;
    
    class MoveGenerator {
    public:

        MoveGenerator(const Board& b, Color c);

        bool ended() const;
        Moves nextMoves();

    private:
        const Board& _board;
        const Color _color;
        int x = 0;
        int y = 0;

        void nextSquare();
    };

    using Type = std::array<Square, 64u>;

    Board();

    Square get(int x, int y) const;
    Square get(int pos) const;
    Square set(int x, int y, Square s);
    Square set(int pos, Square s);

    /*template<typename Fn>
    void foreach(Fn&& f) {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                f(get(x, y), x, y);
            }
        }
    }*/

    int score() const;

    int apply(const Move& m);
    void undo(int numUndoMoves);

private:
    std::array<Square, 64u> _board;
    UndoMove _undoMoves;
    int _score = 0;

    // Using int instead of size_t everywhere due to negative integers
    static constexpr int position(int x, int y) {
        return y * 8 + x;
    }
};

std::ostream& operator<<(std::ostream& os, const Board& b);