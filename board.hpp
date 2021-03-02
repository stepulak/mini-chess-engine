#pragma once

#include "figures.hpp"

#include <array>

class Board {
public:
    using Type = std::array<Square, 64u>;

    Board();

    Square get(int x, int y) const;
    Square get(int pos) const;
    Square set(int x, int y, Square s);
    Square set(int pos, Square s);

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