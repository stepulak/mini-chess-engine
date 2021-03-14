
#include "board.hpp"
#include "board_stats.hpp"
#include "figure_moves.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>

static BoardStats bstats;

const int MIN = std::numeric_limits<int>::min() + 1;
const int MAX = std::numeric_limits<int>::max() - 1;

int negamax(Board& b, Color c, int alpha, int beta, bool maxing, size_t depth)
{
    if (b.kingCaptured() || depth == 0) {
        return b.score() * (maxing ? 1 : -1);
    }
    int score = MIN;
    auto generator = b.moveGenerator(c);

    while (generator.hasMoves()) {
        for (const auto& m : generator.movesChunk()) {
            int undos = b.applyMove(m);
            score = std::max(score, -negamax(b, enemyColor(c), -beta, -alpha, maxing, depth - 1));
            b.undoMove(undos);
            alpha = std::max(alpha, score);
            if (alpha >= beta) {
                return score;
            }
        }
    }

    return score;
}

std::optional<Move> bestMove(Board& b, Color c, size_t depth = 6u)
{
    auto generator = b.moveGenerator(c);
    if (!generator.hasMoves()) {
        return {};
    }

    int bestScore = MIN;
    std::optional<Move> bestMove;

    while (generator.hasMoves()) {
        for (const auto& m : generator.movesChunk()) {
            int undos = b.applyMove(m);
            if (bstats.threeFoldRepetition(b)) {
                // skip this move
                b.undoMove(undos);
                continue;
            }
            const auto score = -negamax(b, enemyColor(c), MIN, MAX, depth % 2 == 0, depth - 1u);
            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
            b.undoMove(undos);
        }
    }

    return bestMove;
}

int main()
{
    Board b;
    std::cout << b << std::endl
              << std::endl;

    auto c = Color::WHITE;
    bool computerPlays = true;

    while (true) {
        std::cout << "BOARD SCORE: " << b.score() << std::endl;
        std::cout << "NOW PLAYS: " << (c == Color::WHITE ? "white" : "black") << std::endl;

        if (!computerPlays) {
            std::cout << "MOVE: ";
            std::string move;
            std::cin >> move;
            Move m;
            m.from.x = move[0] - 'a';
            m.from.y = move[1] - '1';
            m.to.x = move[2] - 'a';
            m.to.y = move[3] - '1';
            m.toSq = b.get(m.from.x, m.from.y);

            if (figure(m.toSq) == Figure::KING_IDLE) {
                const auto sq = square(Figure::ROOK_IDLE, color(m.toSq));
                if (color(m.toSq) == Color::WHITE) {
                    if (m.to.y == 0 && m.to.x == 6 && figure(b.get(7, 0)) == Figure::ROOK_IDLE) {
                        b.applyMove(Move { 7, 0, 5, 0, sq });
                    } else if (m.to.y == 0 && m.to.x == 2 && figure(b.get(0, 0)) == Figure::ROOK_IDLE) {
                        b.applyMove(Move { 0, 0, 3, 0, sq });
                    }
                } else {
                    if (m.to.y == 7 && m.to.x == 6 && figure(b.get(7, 7)) == Figure::ROOK_IDLE) {
                        b.applyMove(Move { 7, 7, 5, 7, sq });
                    } else if (m.to.y == 7 && m.to.x == 2 && figure(b.get(0, 7)) == Figure::ROOK_IDLE) {
                        b.applyMove(Move { 0, 7, 3, 7, sq });
                    }
                }
            }
            b.applyMove(m);
            bstats.lastVisit(b);
            computerPlays = true;
        } else {
            computerPlays = false;
            const auto now = std::chrono::system_clock::now();
            std::cout << "ESTIMATED DEPTH: " << bstats.minimaxDepth() << std::endl;
            const auto m = bestMove(b, c);
            const auto dur = std::chrono::duration<double>(std::chrono::system_clock::now() - now);
            std::cout << "DURATION: " << dur.count() << std::endl;

            if (!m) {
                std::cout << "NO MOVE AVAILABLE" << std::endl;
                return 1;
            }
            std::cout << "MOVE: " << char(m->from.x + 'a') << (m->from.y + 1) << char(m->to.x + 'a') << (m->to.y + 1) << std::endl;

            b.applyMove(*m);
            bstats.lastVisit(b);
        }
        c = enemyColor(c);
        std::cout << b << std::endl;
        std::cout << std::endl
                  << std::endl;
    }

    return 0;
}