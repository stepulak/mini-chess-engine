#pragma once

#include "board.hpp"
#include "board_stats.hpp"

#include <atomic>
#include <optional>

class AI {
public:
    AI(Board& b, Color c, const BoardStats& stats);

    void run();
    void stop();

    std::optional<Move> bestMove() const;

private:
    using MoveAndScore = std::pair<Move, int>;

    // Negascout min, max depth
    static constexpr size_t MIN_DEPTH = 4u;
    static constexpr size_t MAX_DEPTH = 10u;

    Board& _board;
    const Color _color;
    const BoardStats& _boardStats;
    std::optional<MoveAndScore> _bestMove;

    // No need of mutexes if AI is stopped from another thread
    // Negascout needs to finish ply and remaining depth nor time of stop does not matter
    std::atomic_bool _stop = false;

    std::optional<MoveAndScore> countBestMove(Board& b, Color c, size_t depth) const;
    static int negascout(Board& b, Color c, int alpha, int beta, size_t depth, bool maxing, const std::atomic_bool& stop);
};
