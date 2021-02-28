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

Figure figure(Square s)
{
    return static_cast<Figure>(s & 0xF);
}

Color color(Square s)
{
    return static_cast<Color>(s >> 4);
}

Square square(Figure f, Color c)
{
    return (static_cast<Square>(c) << 4) | static_cast<Square>(f);
}

int figureScore(Figure f, Color c, int pos);
std::string figureSymbol(Figure f, Color c);
