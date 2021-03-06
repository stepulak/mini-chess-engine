#pragma once

#include <cstdint>
#include <string>

using Square = uint8_t;

enum class Figure : Square {
    PAWN = 0,
    PAWN_IDLE,
    PAWN_EN_PASSANT,
    KNIGHT,
    BISHOP,
    ROOK,
    ROOK_IDLE,
    QUEEN,
    KING,
    KING_IDLE,
    NONE,
};

enum class Color : Square {
    WHITE = 0,
    BLACK,
};

namespace {

constexpr size_t figureIndex(Figure f)
{
    return static_cast<size_t>(f);
}

constexpr Figure figure(Square s)
{
    return static_cast<Figure>(s & 0xF);
}

constexpr Color color(Square s)
{
    return static_cast<Color>(s >> 4);
}

constexpr Square square(Figure f, Color c)
{
    return (static_cast<Square>(c) << 4) | static_cast<Square>(f);
}

constexpr bool enemy(Square s1, Square s2)
{
    return figure(s1) != Figure::NONE && figure(s2) != Figure::NONE && color(s1) != color(s2);
}

constexpr bool own(Square s1, Square s2)
{
    return figure(s1) != Figure::NONE && figure(s2) != Figure::NONE && color(s1) == color(s2);
}

constexpr Color enemyColor(Color c)
{
    return c == Color::WHITE ? Color::BLACK : Color::WHITE;
}

constexpr auto NUM_FIGURES = figureIndex(Figure::NONE) - figureIndex(Figure::PAWN);
constexpr auto EMPTY_SQUARE = square(Figure::NONE, Color::WHITE);

} // namespace

int figureScore(Figure f, Color c, int pos);
std::string figureSymbol(Figure f, Color c);
