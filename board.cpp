#include "board.hpp"

Board::MoveGenerator::MoveGenerator(const Board& b, Color c)
    : _board(b)
    , _color(c)
{
}

bool Board::MoveGenerator::ended() const {
    return y >= Board::HEIGHT;
}

Moves Board::MoveGenerator::nextMoves() {
    while (!ended()) {
        const auto sq = _board.get(x, y);
        if (color(sq) != _color) {
            nextSquare();
            continue;
        }
        const auto f = figure(sq);
        if (f == Figure::NONE) {
            nextSquare();
            continue;
        }
        figureMoves(f, _board, sq, x, y);
        
    }
    return {};
}

void Board::MoveGenerator::nextSquare() {
    x++;
    if (x >= Board::WIDTH) {
        x = 0;
        y++;
    }
}