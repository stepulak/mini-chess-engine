#pragma once

#include "board.hpp"

Moves figureMoves(Figure f, const Board& b, int x, int y);
bool figureMoveValid(const Move& m, const Board& b, Color c);