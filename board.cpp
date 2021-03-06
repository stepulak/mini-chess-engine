#include "board.hpp"
#include "figure_moves.hpp"

Board::MoveGenerator::MoveGenerator(const Board& b, Color c)
    : _board(b)
    , _color(c)
{
}

bool Board::MoveGenerator::ended() const {
    return y >= Board::HEIGHT;
}

Moves Board::MoveGenerator::nextMoves() {
    while (!ended()) {
        const auto sq = _board.get(x, y);
        if (color(sq) != _color) {
            nextSquare();
            continue;
        }
        const auto f = figure(sq);
        if (f == Figure::NONE) {
            nextSquare();
            continue;
        }
        return figureMoves(f, _board, x, y);
    }
    return {};
}

void Board::MoveGenerator::nextSquare() {
    x++;
    if (x >= Board::WIDTH) {
        x = 0;
        y++;
    }
}

bool Board::kingCaptured() const {
    return std::abs(_score) > 80000;
}

size_t Board::apply(const Move& m) {
    const auto fromSq = get(m.from.x, m.from.y);
    const auto toSq = get(m.to.x, m.to.y);
    const auto fromSqCol = color(fromSq);
    const auto fromSqFig = figure(fromSq);
    const auto toSqCol = color(fromSq);
    const auto toSqFig = figure(fromSq);

    _undoMoves.emplace_back(UndoMove { m.from, m.to, fromSq, toSq, _score });
    _score -= figureScore(fromSqFig, fromSqCol, position(m.from.x, m.from.y));
    _score -= figureScore(toSqFig, toSqCol, position(m.to.x, m.to.y));

    set(m.to.x, m.to.y, m.toSq);
    set(m.from.x, m.from.y, EMPTY_SQUARE);

    _score += figureScore(toSqFig, toSqCol, position(m.to.x, m.to.y));

    if (m.castling) {
        int fx = (m.from.x < m.to.x) ? 7 : 0;
        int tx = m.to.x - (m.from.x < m.to.x ? 1 : -1);
        int y = m.to.y;
        return apply(Move { fx, y, tx, m.to.y, square(Figure::ROOK, fromSqCol) }) + 1u;
    }

    return 1u;
}

void Board::undo(size_t numUndoMoves) {
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

std::ostream& operator<<(std::ostream& os, const Board& b) {

}