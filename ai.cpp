#include "ai.hpp"

namespace {

constexpr int MIN = std::numeric_limits<int>::min() + 1;
constexpr int MAX = std::numeric_limits<int>::max() - 1;

} // namespace

AI::AI(Board& b, Color c, const BoardStats& stats)
    : _board(b)
    , _color(c)
    , _boardStats(stats)
{
}

void AI::run()
{
    // Depth is increased by two = one ply
    for (size_t depth = MIN_DEPTH; depth <= MAX_DEPTH; depth += 2u) {
        const auto move = countBestMove(_board, _color, depth);
        if (!move) {
            continue;
        }
        if (_stop) {
            return;
        }
        _bestMove = move;
    }
}

void AI::stop()
{
    _stop = true;
}

std::optional<Move> AI::bestMove() const
{
    if (!_bestMove) {
        return std::nullopt;
    }
    return _bestMove->first;
}

std::optional<AI::MoveAndScore> AI::countBestMove(Board& b, Color c, size_t depth) const
{
    auto generator = b.moveGenerator(c);
    if (!generator.hasMoves()) {
        return {};
    }
    const bool kingCheck = kingInCheck(b, c);
    int bestScore = MIN;
    Move bestMove;

    while (generator.hasMoves()) {
        for (const auto& m : generator.movesChunk()) {
            // You cannot do castling if king is in check
            if (m.type == MoveType::CASTLING && kingCheck) {
                continue;
            }
            int undos = b.applyMove(m);
            if (_boardStats.threeFoldRepetition(b)) {
                // skip this move
                b.undoMove(undos);
                continue;
            }
            const auto score = -negascout(b, enemyColor(c), MIN, MAX, depth - 1u, _stop);
            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
            b.undoMove(undos);
        }
    }

    return std::make_optional(std::make_pair(bestMove, bestScore));
}

bool AI::kingInCheck(const Board& b, Color c) const
{
    const auto kingPosition = b.kingPosition(c);

    for (auto generator = b.moveGenerator(enemyColor(c)); generator.hasMoves();) {
        for (const auto& m : generator.movesChunk()) {
            if (m.to == kingPosition) {
                return true;
            }
        }
    }

    return false;
}

int AI::negascout(Board& b, Color c, int alpha, int beta, size_t depth, const std::atomic_bool& stop)
{
    if (depth == 0 || b.kingCaptured() || (stop && depth % 2u == 0u)) {
        // Bottom of search tree
        // King is dead
        // Negascout is stopped and ply finished
        return b.score() * (depth % 2 ? -1 : 1);
    }
    int score = MIN;
    bool first = true;

    for (auto generator = b.moveGenerator(c); generator.hasMoves();) {
        for (const auto& m : generator.movesChunk()) {
            int undos = b.applyMove(m);
            if (first) {
                score = -negascout(b, enemyColor(c), -beta, -alpha, depth - 1, stop);
                first = false;
            } else {
                score = -negascout(b, enemyColor(c), -alpha - 1, -alpha, depth - 1, stop);
                if (alpha < score && score < beta) {
                    score = -negascout(b, enemyColor(c), -beta, -score, depth - 1, stop);
                }
            }
            b.undoMove(undos);
            alpha = std::max(alpha, score);
            if (alpha >= beta) {
                return alpha;
            }
        }
    }
    return alpha;
}