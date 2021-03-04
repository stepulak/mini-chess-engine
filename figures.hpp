#pragma once

#include <cstdint>
#include <string>

using Square = uint8_t;

enum class Figure : Square {
    PAWN = 0,
    PAWN_NOT_MOVED,
    PAWN_EN_PASSANT,
    KNIGHT,
    BISHOP,
    ROOK,
    ROOK_NOT_MOVED,
    QUEEN,
    KING,
    KING_NOT_MOVED,
    NONE,
};

enum class Color : Square {
    WHITE = 0,
    BLACK,
};

static constexpr Figure figure(Square s)
{
    return static_cast<Figure>(s & 0xF);
}

static constexpr Color color(Square s)
{
    return static_cast<Color>(s >> 4);
}

static constexpr Square square(Figure f, Color c)
{
    return (static_cast<Square>(c) << 4) | static_cast<Square>(f);
}

static constexpr bool enemy(Square s1, Square s2)
{
    return figure(s1) != Figure::NONE && figure(s2) != Figure::NONE && color(s1) != color(s2);
}

static constexpr bool own(Square s1, Square s2)
{
    return figure(s1) != Figure::NONE && figure(s2) != Figure::NONE && color(s1) == color(s2);
}

static constexpr Color enemyColor(Color c)
{
    return c == Color::WHITE ? Color::BLACK : Color::WHITE;
}

int figureScore(Figure f, Color c, int pos);
std::string figureSymbol(Figure f, Color c);
