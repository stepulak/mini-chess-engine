#include "figure_moves.hpp"

namespace {

#define MOVES_GENERATOR_ARGS \
    Moves &moves, const Board &b, Square sq, int x, int y

void moveInDirection(Moves& moves, const Board& b, Square sq, Figure fig, int x, int y, int dx, int dy, size_t steps = Board::SIZE)
{
    int mx = x + dx;
    int my = y + dy;

    for (size_t i = 0; i < steps; i++) {
        if (!Board::pointValid(mx, my)) {
            break;
        }
        const auto sq2 = b.get(mx, my);
        if (own(sq, sq2)) {
            break;
        }
        moves.emplace_back(Move { x, y, mx, my, fig });
        if (enemy(sq2, sq)) {
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

void bishopMoves(MOVES_GENERATOR_ARGS)
{
    diagonalMoves([&](int dx, int dy) {
        moveInDirection(moves, b, sq, BISHOP, x, y, dx, dy);
    });
}

void rookMoves(MOVES_GENERATOR_ARGS)
{
    straightMoves([&](int dx, int dy) {
        moveInDirection(moves, b, sq, ROOK, x, y, dx, dy);
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
    forEachDirection([&](int dx, int dy) {
        moveInDirection(moves, b, sq, QUEEN, x, y, dx, dy);
    });
}

void kingMoves(MOVES_GENERATOR_ARGS)
{
    forEachDirection([&](int dx, int dy) {
        moveInDirection(moves, b, sq, KING, x, y, dx, dy, 1);
    });
}

void kingMovesNotMoved(MOVES_GENERATOR_ARGS)
{
    kingMoves(moves, b, sq, x, y);

    const auto pathClear = [&b, y](int start, int end) {
        for (int i = start; i < end; i++) {
            if (figure(b.get(i, y)) != NONE_FIGURE) {
                return false;
            }
        }
        return true;
    };

    // Left castling
    if (figure(b.get(0, y)) == ROOK_NOT_MOVED && pathClear(1, x)) {
        moves.emplace_back(Move {x, y, 2, y, KING, true });
    }
    // Right castling
    if (figure(b.get(Board::SIZE - 1u, y)) == ROOK_NOT_MOVED && pathClear(x + 1, Board::SIZE)) {
        moves.emplace_back(Move {x, y, Board::SIZE - 2u, y, KING, true });
    }
}

void knightMoves(MOVES_GENERATOR_ARGS)
{
    straightMoves([&](int dx, int dy) {
        for (int i = -1; i <= 1; i += 2) {
            const auto mx = x + dx * 2 + dy * i;
            const auto my = y + dy * 2 + dx * i;
            if (Board::pointValid(mx, my) && !own(sq, b.get(mx, my))) {
                moves.emplace_back(Move { x, y, mx, my, KNIGHT });
            }
        }
    });
}

int pawnDirection(Color pawnCol)
{
    return pawnCol == WHITE ? 1 : -1;
}

bool promotePawn(Color pawnCol, int y)
{
    return pawnCol == WHITE ? (y == Board::SIZE - 1) : (y == 0);
}

void pawnMoves(MOVES_GENERATOR_ARGS)
{
    const auto col = color(sq);
    const auto dir = pawnDirection(col);

    if (figure(b.get(x, y + dir)) == NONE_FIGURE) {
        const auto fig = promotePawn(col, y + dir) ? QUEEN : PAWN;
        moves.emplace_back(Move { x, y, x, y + dir, fig });
    }

    for (int i = -1; i <= 1; i += 2) {
        if (Board::pointValid(x + i, y + dir) && enemy(sq, b.get(x + i, y + dir))) {
            const auto fig = promotePawn(col, y + dir) ? QUEEN : PAWN;
            moves.emplace_back(Move { x, y, x + i, y + dir, fig });
        }
    }
}

void pawnMovesEnPassant(MOVES_GENERATOR_ARGS)
{
    pawnMoves(moves, b, sq, x, y);

    const auto col = color(sq);
    const auto dir = pawnDirection(col);

    if (figure(b.get(x, y + dir)) == NONE_FIGURE && figure(b.get(x, y + dir * 2)) == NONE_FIGURE) {
        moves.emplace_back(Move { x, y, x, y + dir * 2, PAWN });
    }
}

const std::unordered_map<Figure, std::function<void(MOVES_GENERATOR_ARGS)>> FIGURE_MOVES = {
    { PAWN, pawnMoves },
    { PAWN_EN_PASSANT, pawnMoves },
    { PAWN_NOT_MOVED, pawnMovesEnPassant },
    { KNIGHT, knightMoves },
    { BISHOP, bishopMoves },
    { ROOK, rookMoves },
    { ROOK_NOT_MOVED, rookMoves },
    { QUEEN, queenMoves },
    { KING, kingMoves },
    { KING_NOT_MOVED, kingMovesNotMoved }
};

} // namespace

Moves figureMoves(Figure f, const Board& b, Square sq, int x, int y)
{

}