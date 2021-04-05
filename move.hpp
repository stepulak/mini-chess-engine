#pragma once

#include "figures.hpp"

#include <vector>

struct Point {
    int x;
    int y;

    Point() = default;

    Point(int x_, int y_)
        : x(x_)
        , y(y_)
    {
    }

    bool operator==(const Point& p) const {
        return x == p.x && y == p.y;
    }
};

struct MoveBase {
    Point from;
    Point to;
    Square toSq;

    MoveBase() = default;

    MoveBase(Point from_, Point to_, Square toSq_)
        : from(from_)
        , to(to_)
        , toSq(toSq_)
    {
    }

    virtual ~MoveBase() = default;

    bool operator==(const MoveBase& mb) const {
        return from == mb.from && to == mb.to && toSq == mb.toSq;
    }
};

struct Move : MoveBase {
    bool castling;

    Move() = default;

    Move(Point from_, Point to_, Square toSq_, bool castling_ = false)
        : MoveBase(std::move(from_), std::move(to_), toSq_)
        , castling(castling_)
    {
    }

    Move(int fx, int fy, int tx, int ty, Square toSq_, bool castling = false)
        : Move(Point { fx, fy }, Point { tx, ty }, toSq_, castling)
    {
    }

    bool operator==(const Move& m) const {
        return MoveBase::operator==(m) && castling == m.castling;
    }
};

struct UndoMove : MoveBase {
    Square fromSq;
    int score;

    UndoMove(Point from_, Point to_, Square fromSq_, Square toSq_, int score_)
        : MoveBase(std::move(from_), std::move(to_), toSq_)
        , fromSq(fromSq_)
        , score(score_)
    {
    }

    UndoMove(int fx, int fy, int tx, int ty, Square fromSq_, Square toSq_, int score)
        : UndoMove(Point { fx, fy }, Point { tx, ty }, fromSq_, toSq_, score)
    {
    }
};

using Moves = std::vector<Move>;
using UndoMoves = std::vector<UndoMove>;