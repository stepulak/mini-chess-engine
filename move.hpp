#pragma once

#include "figures.hpp"

#include <iostream>
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

    bool operator==(const Point& p) const
    {
        return x == p.x && y == p.y;
    }
};

static std::ostream& operator<<(std::ostream& os, const Point& p)
{
    os << static_cast<char>(p.x + 'a') << (p.y + 1);
    return os;
}

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

    bool operator==(const MoveBase& mb) const
    {
        return from == mb.from && to == mb.to && toSq == mb.toSq;
    }
};

enum class MoveType {
    STANDARD,
    CASTLING,
    EN_PASSANT
};

struct Move : MoveBase {
    MoveType type;

    Move() = default;

    Move(Point from_, Point to_, Square toSq_, MoveType type_ = MoveType::STANDARD)
        : MoveBase(std::move(from_), std::move(to_), toSq_)
        , type(type_)
    {
    }

    Move(int fx, int fy, int tx, int ty, Square toSq_, MoveType type_ = MoveType::STANDARD)
        : Move(Point { fx, fy }, Point { tx, ty }, toSq_, type_)
    {
    }

    bool operator==(const Move& m) const
    {
        return MoveBase::operator==(m) && type == m.type;
    }
};

static std::ostream& operator<<(std::ostream& os, const Move& m)
{
    return os << m.from << m.to;
}

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