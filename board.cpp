#include "board.hpp"
#include "figure_moves.hpp"

#include <iostream>

Board::MoveGenerator::MoveGenerator(const Board& b, Color c)
    : _board(b)
    , _color(c)
{
}

Moves Board::MoveGenerator::movesChunk()
{
    while (hasMoves()) {
        const auto sq = _board.get(_x, _y);
        if (color(sq) != _color) {
            nextSquare();
            continue;
        }
        const auto f = figure(sq);
        if (f == Figure::NONE) {
            nextSquare();
            continue;
        }
        const auto moves = figureMoves(f, _board, _x, _y);
        nextSquare();
        return moves;
    }
    return {};
}

void Board::MoveGenerator::nextSquare()
{
    _x++;
    if (_x >= Board::WIDTH) {
        _x = 0;
        _y++;
    }
}

Board::Board()
{
    _board.fill(EMPTY_SQUARE);

    for (int x = 0; x < WIDTH; x++) {
        set(x, 1, square(Figure::PAWN_IDLE, Color::WHITE));
        set(x, 6, square(Figure::PAWN_IDLE, Color::BLACK));
    }

    set(0, 0, square(Figure::ROOK_IDLE, Color::WHITE));
    set(0, 7, square(Figure::ROOK_IDLE, Color::BLACK));
    set(7, 0, square(Figure::ROOK_IDLE, Color::WHITE));
    set(7, 7, square(Figure::ROOK_IDLE, Color::BLACK));

    set(1, 0, square(Figure::KNIGHT, Color::WHITE));
    set(1, 7, square(Figure::KNIGHT, Color::BLACK));
    set(6, 0, square(Figure::KNIGHT, Color::WHITE));
    set(6, 7, square(Figure::KNIGHT, Color::BLACK));

    set(2, 0, square(Figure::BISHOP, Color::WHITE));
    set(2, 7, square(Figure::BISHOP, Color::BLACK));
    set(5, 0, square(Figure::BISHOP, Color::WHITE));
    set(5, 7, square(Figure::BISHOP, Color::BLACK));

    set(3, 0, square(Figure::QUEEN, Color::WHITE));
    set(3, 7, square(Figure::QUEEN, Color::BLACK));
    set(4, 0, square(Figure::KING_IDLE, Color::WHITE));
    set(4, 7, square(Figure::KING_IDLE, Color::BLACK));
}

void Board::set(int pos, Square sq)
{
    _board[pos] = sq;

    if (sq == EMPTY_SQUARE) {
        _hash &= ~(1 << pos);
    } else {
        _hash |= (1 << pos);
    }
}

Point Board::kingPosition(Color c) const
{
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            const auto sq = get(x, y);
            const auto fig = figure(sq);
            if (color(sq) == c && (fig == Figure::KING || fig == Figure::KING_IDLE)) {
                return Point { x, y };
            }
        }
    }
    throw std::runtime_error("King not found!");
}

size_t Board::applyMove(const Move& m)
{
    const auto fromSq = get(m.from.x, m.from.y);
    const auto toSq = get(m.to.x, m.to.y);
    const auto fromSqCol = color(fromSq);
    const auto fromSqFig = figure(fromSq);
    const auto toSqCol = color(toSq);
    const auto toSqFig = figure(toSq);

    _undoMoves.emplace_back(UndoMove { m.from, m.to, fromSq, toSq, _score });
    _score -= figureScore(fromSqFig, fromSqCol, position(m.from.x, m.from.y));
    _score -= figureScore(toSqFig, toSqCol, position(m.to.x, m.to.y));

    set(m.to.x, m.to.y, m.toSq);
    set(m.from.x, m.from.y, EMPTY_SQUARE);

    _score += figureScore(figure(m.toSq), color(m.toSq), position(m.to.x, m.to.y));

    if (m.type == MoveType::CASTLING) {
        const int fx = (m.from.x < m.to.x) ? 7 : 0;
        const int tx = m.to.x - (m.from.x < m.to.x ? 1 : -1);
        const int y = m.to.y;
        return applyMove(Move { fx, y, tx, m.to.y, square(Figure::ROOK, fromSqCol) }) + 1u;
    }
    if (m.type == MoveType::EN_PASSANT) {
        const int x = m.to.x;
        const int y = m.from.y;
        const auto sq = get(x, y);
        _undoMoves.emplace_back(UndoMove { x, y, x, y, sq, sq, _score });
        _score -= figureScore(figure(sq), color(sq), position(x, y));
        set(x, y, EMPTY_SQUARE);
        return 2u;
    }

    return 1u;
}

void Board::undoMove(size_t numUndoMoves)
{
    for (size_t i = 0u; i < numUndoMoves; i++) {
        if (_undoMoves.empty()) {
            throw std::runtime_error("unable to undo");
        }
        const auto um = _undoMoves.back();
        _undoMoves.pop_back();

        set(um.to.x, um.to.y, um.toSq);
        set(um.from.x, um.from.y, um.fromSq);

        _score = um.score;
    }
}

std::ostream& operator<<(std::ostream& os, const Board& b)
{
    for (int y = b.HEIGHT - 1; y >= 0; y--) {
        os << (y + 1) << '.' << "  ";
        for (int x = 0; x < b.WIDTH; x++) {
            const auto sq = b.get(x, y);
            const auto sym = figureSymbol(figure(sq), color(sq));
            os << sym << ' ';
        }
        std::cout << std::endl;
    }
    os << std::endl;
    os << "    ";
    for (int x = 0; x < b.WIDTH; x++) {
        std::cout << static_cast<char>(x + 'a') << '.';
    }
    std::cout << std::endl
              << std::endl;
    return os;
}