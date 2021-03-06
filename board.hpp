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

    constexpr Square get(int pos) const {
        return _board[pos];
    }

    constexpr Square get(int x, int y) const {
        return get(position(x, y));
    }

    constexpr void set(int pos, Square s) {
        _board[pos] = s;
    }

    constexpr void set(int x, int y, Square s) {
        set(position(x, y), s);
    }

    int score() const {
        return _score;
    }

    bool kingCaptured() const;

    size_t apply(const Move& m);
    void undo(size_t numUndoMoves);

    static constexpr bool validIndex(int x, int y) {
        return x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT;
    }

private:
    std::array<Square, 64u> _board;
    UndoMoves _undoMoves;
    int _score = 0;

    // Using int instead of size_t everywhere due to negative integers
    static constexpr int position(int x, int y) {
        return y * 8 + x;
    }
};

std::ostream& operator<<(std::ostream& os, const Board& b);