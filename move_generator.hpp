#pragma once

#include "move.hpp"
#include "board.hpp"

#include <optional>

class MoveGenerator {
public:

    MoveGenerator(const Board& b, Color c);

    std::optional<Moves> nextMoves();

private:
    const Board& _board;
    const Color _color;
    int x = 0;
    int y = 0;
};