#pragma once

#include "figures.hpp"

struct Move {
    // from
    int fX;
    int fY;
    // to
    int tX;
    int tY;

    Square result;
    bool castling = false;

    Move() = default;

    Move(int fx, int fy, int tx, int ty, Square res, bool castl = false)
        : fX(fx)
        , fY(fy)
        , tX(tx)
        , tY(ty)
        , result(res)
        , castling(castling)
    {
    }
};

struct UndoMove : public Move {

    
};