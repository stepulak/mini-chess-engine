#include "ai.hpp"

namespace {

const int MIN = std::numeric_limits<int>::min() + 1;
const int MAX = std::numeric_limits<int>::max() - 1;

} // namespace

std::optional<Move> AI::coutBestMove(Board& b, Color c, size_t depth)
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
            if (bstats.threeFoldRepetition(b)) {
                // skip this move
                b.undoMove(undos);
                continue;
            }
            const auto score = -negascout(b, enemyColor(c), MIN, MAX, depth - 1u);
            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
            b.undoMove(undos);
        }
    }

    return bestMove;
}

int AI::negascout(Board& b, Color c, int alpha, int beta, size_t depth, const bool& stop)
{
    if (depth == 0 || b.kingCaptured() || stop) {
        return b.score();
    }
    int score = MIN;
    bool first = true;
    auto generator = b.moveGenerator(c);

    while (generator.hasMoves()) {
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