#include "figure_moves.hpp"

#include <array>
#include <functional>

namespace {

#define MOVES_GENERATOR_ARGS \
    Moves &moves, const Board &b, int x, int y

void moveInDirection(Moves& moves, const Board& b, Square toSq, int x, int y, int dx, int dy, size_t steps = Board::SIZE)
{
    const auto fromSq = b.get(x, y);
    int mx = x + dx;
    int my = y + dy;

    for (size_t i = 0; i < steps && Board::validIndex(mx, my); i++) {
        const auto sq = b.get(mx, my);
        if (own(sq, fromSq)) {
            break;
        }
        moves.emplace_back(Move { x, y, mx, my, toSq });
        if (enemy(sq, fromSq)) {
            break;
        }
        mx += dx;
        my += dy;
    }
}

template <typename Func>
void diagonalMoves(Func&& f)
{
    for (int d = 0; d < 4; d++) {
        const auto dx = (d / 2) ? -1 : 1;
        const auto dy = (d % 2) ? -1 : 1;
        f(dx, dy);
    }
}

template <typename Func>
void straightMoves(Func&& f)
{
    for (int d = 0; d < 4; d++) {
        const auto minus = (d % 2) ? -1 : 1;
        const auto dx = d / 2;
        const auto dy = 1 - dx;
        f(dx * minus, dy * minus);
    }
}

Square makeFigureColorFromPosition(Figure f, const Board& b, int x, int y)
{
    const auto col = color(b.get(x, y));
    const auto toSq = square(f, col);
}

void bishopMoves(MOVES_GENERATOR_ARGS)
{
    const auto sq = makeFigureColorFromPosition(Figure::BISHOP, b, x, y);
    diagonalMoves([&](int dx, int dy) {
        moveInDirection(moves, b, sq, x, y, dx, dy);
    });
}

void rookMoves(MOVES_GENERATOR_ARGS)
{
    const auto sq = makeFigureColorFromPosition(Figure::ROOK, b, x, y);
    straightMoves([&](int dx, int dy) {
        moveInDirection(moves, b, sq, x, y, dx, dy);
    });
}

template <typename Func>
void forEachDirection(Func&& f)
{
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx != 0 || dy != 0) {
                f(dx, dy);
            }
        }
    }
}

void queenMoves(MOVES_GENERATOR_ARGS)
{
    const auto sq = makeFigureColorFromPosition(Figure::QUEEN, b, x, y);
    forEachDirection([&](int dx, int dy) {
        moveInDirection(moves, b, sq, x, y, dx, dy);
    });
}

void kingMoves(MOVES_GENERATOR_ARGS)
{
    const auto sq = makeFigureColorFromPosition(Figure::KING, b, x, y);
    forEachDirection([&](int dx, int dy) {
        moveInDirection(moves, b, sq, x, y, dx, dy, 1);
    });
}

void kingMovesIdle(MOVES_GENERATOR_ARGS)
{
    const auto sq = makeFigureColorFromPosition(Figure::KING, b, x, y);

    kingMoves(moves, b, x, y);

    const auto pathClear = [&b, y](int start, int end) {
        for (int i = start; i < end; i++) {
            if (figure(b.get(i, y)) != Figure::NONE) {
                return false;
            }
        }
        return true;
    };

    // Left castling
    if (figure(b.get(0, y)) == Figure::ROOK_IDLE && pathClear(1, x)) {
        moves.emplace_back(Move { x, y, 2, y, sq, true });
    }
    // Right castling
    if (figure(b.get(Board::SIZE - 1u, y)) == Figure::ROOK_IDLE && pathClear(x + 1, Board::SIZE)) {
        moves.emplace_back(Move { x, y, Board::SIZE - 2u, y, sq, true });
    }
}

void knightMoves(MOVES_GENERATOR_ARGS)
{
    straightMoves([&](int dx, int dy) {
        for (int i = -1; i <= 1; i += 2) {
            const auto mx = x + dx * 2 + dy * i;
            const auto my = y + dy * 2 + dx * i;
            if (Board::validIndex(mx, my) && !own(sq, b.get(mx, my))) {
                moves.emplace_back(Move { x, y, mx, my, Figure::KNIGHT });
            }
        }
    });
}

int pawnDirection(Color pawnCol)
{
    return pawnCol == Color::WHITE ? 1 : -1;
}

bool promotePawn(Color pawnCol, int y)
{
    return pawnCol == Color::WHITE ? (y == Board::SIZE - 1) : (y == 0);
}

void pawnMoves(MOVES_GENERATOR_ARGS)
{
    const auto fromSq = b.get(x, y);
    const auto col = color(fromSq);
    const auto dir = pawnDirection(col);

    if (figure(b.get(x, y + dir)) == Figure::NONE) {
        const auto toSq = promotePawn(col, y + dir) ? square(Figure::QUEEN, col) : square(Figure::PAWN, col);
        moves.emplace_back(Move { x, y, x, y + dir, toSq });
    }

    for (int i = -1; i <= 1; i += 2) {
        if (Board::validIndex(x + i, y + dir) && enemy(fromSq, b.get(x + i, y + dir))) {
            const auto toSq = promotePawn(col, y + dir) ? square(Figure::QUEEN, col) : square(Figure::PAWN, col);
            moves.emplace_back(Move { x, y, x + i, y + dir, toSq });
        }
    }
}

void pawnMovesEnPassant(MOVES_GENERATOR_ARGS)
{
    pawnMoves(moves, b, x, y);

    const auto col = color(b.get(x, y));
    const auto dir = pawnDirection(col);

    if (figure(b.get(x, y + dir)) == Figure::NONE && figure(b.get(x, y + dir * 2)) == Figure::NONE) {
        moves.emplace_back(Move { x, y, x, y + dir * 2, square(Figure::PAWN, col) });
    }
}

using MoveFunction = std::function<void(MOVES_GENERATOR_ARGS)>;

const std::array<MoveFunction, NUM_FIGURES> FIGURE_MOVES = {
    pawnMoves,
    pawnMoves, // pawn idle
    pawnMovesEnPassant, // pawn idle
    knightMoves,
    bishopMoves,
    rookMoves,
    rookMoves, // rook idle
    queenMoves,
    kingMoves,
    kingMovesIdle,
};

} // namespace

Moves figureMoves(Figure f, const Board& b, Square sq, int x, int y)
{
}
