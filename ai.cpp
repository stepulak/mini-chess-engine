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
        return {};
    }
    return _bestMove->first;
}

std::optional<AI::MoveAndScore> AI::countBestMove(Board& b, Color c, size_t depth) const
{
    auto generator = b.moveGenerator(c);
    if (!generator.hasMoves()) {
        return {};
    }
    int bestScore = MIN;
    Move bestMove;

    while (generator.hasMoves()) {
        for (const auto& m : generator.movesChunk()) {
            int undos = b.applyMove(m);
            if (_boardStats.threeFoldRepetition(b)) {
                // skip this move
                b.undoMove(undos);
                continue;
            }
            const auto score = -negascout(b, enemyColor(c), MIN, MAX, depth - 1u, c == Color::WHITE, _stop);
            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
            b.undoMove(undos);
        }
    }

    return std::make_optional(std::make_pair(bestMove, bestScore));
}

int AI::negascout(Board& b, Color c, int alpha, int beta, size_t depth, bool maxing, const std::atomic_bool& stop)
{
    if (depth == 0 || b.kingCaptured() || (stop && depth % 2u == 0u)) {
        // Bottom of search tree
        // King is dead
        // Negascout is stopped and ply finished
        return b.score() * (maxing ? 1 : -1);
    }
    int score = MIN;
    bool first = true;
    auto generator = b.moveGenerator(c);

    while (generator.hasMoves()) {
        for (const auto& m : generator.movesChunk()) {
            int undos = b.applyMove(m);
            if (first) {
                score = -negascout(b, enemyColor(c), -beta, -alpha, depth - 1, maxing, stop);
                first = false;
            } else {
                score = -negascout(b, enemyColor(c), -alpha - 1, -alpha, depth - 1, maxing, stop);
                if (alpha < score && score < beta) {
                    score = -negascout(b, enemyColor(c), -beta, -score, depth - 1, maxing, stop);
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