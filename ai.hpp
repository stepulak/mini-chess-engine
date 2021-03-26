#pragma once

#include "board.hpp"
#include "board_stats.hpp"

#include <optional>

class AI {
public:
    AI(Board& b, Color c, const BoardStats& stats);

    void run();
    void stop();
    void join();

    std::optional<Move> bestMove() const;

private:
    using MoveAndScore = std::pair<Move, int>;

    Board& _board;
    const Color _color;
    const BoardStats& stats;

    bool _stop = false;

    std::optional<MoveAndScore> coutBestMove(Board& b, Color c, size_t depth) const;
    static int negascout(Board& b, Color c, int alpha, int beta, size_t depth, const bool& stop);
};
