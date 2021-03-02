#include "move_generator.hpp"


MoveGenerator::MoveGenerator(const Board& b, Color c)
    : _board(b)
    , _color(c)
{
}

std::optional<Moves> MoveGenerator::nextMoves()
{

}