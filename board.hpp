#pragma once

#include "figures.hpp"
#include "movesgenerator.hpp"

#include <array>

class Board {
public:

    Board();

    moves(Color c) const;

private:

    std::array<Square, 64u> _board;
};