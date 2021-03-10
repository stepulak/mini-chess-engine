
#include "board.hpp"
#include "figure_moves.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <optional>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <optional>

struct BoardHash {
    size_t operator()(const Board& b) const {
        return b.hash();
    }
};

class TranspositionTable {
public:
    struct Value {
        size_t depth;
        int score;
        int alpha;
        int beta;
    };

    TranspositionTable() = default;

    std::optional<Value> get(const Board& b) const
    {
        const auto it = _table.find(b);
        if (it == _table.end()) {
            return {};
        }
        return std::make_optional(it->second);
    }

    void set(const Board& b, Value v)
    {
        if (size() > 1e7) { clear(); }
        _table[b] = v;
    }

    size_t size() const
    {
        return _table.size();
    }

    void clear()
    {
        _table.clear();
    }

private:
    std::unordered_map<Board, Value, BoardHash> _table;
};

class HistoryStats {
public:
    HistoryStats() = default;

    size_t get(const Board& b) const
    {
        const auto it = _stats.find(b);
        if (it == _stats.end()) {
            return 0;
        }
        return it->second;
    }

    void inc(const Board& b)
    {
        _stats[b]++;
    }

private:
    std::unordered_map<Board, size_t, BoardHash> _stats;
};

static TranspositionTable tt;
static HistoryStats hstats;

int min(int s1, int s2)
{
    return std::min(s1, s2);
}

int max(int s1, int s2)
{
    return std::max(s1, s2);
}

bool inInterval(int v, int ia, int ib)
{
    return v >= ia && v < ib;
}

const int MIN = -9999999;
const int MAX = 9999999;

int negamax(Board& b, Color c, int alpha, int beta, bool maxing, size_t depth) {
    if (b.kingCaptured() || depth == 0) {
        return b.score() * (maxing ? 1 : -1);
    }
    int score = MIN;

    for (auto generator = b.moveGenerator(c); !generator.ended();) {
        for (const auto& m : generator.nextMoves()) {
            int undos = b.apply(m);
            score = max(score, -negamax(b, enemyColor(c), -beta, -alpha, maxing, depth-1));
            b.undo(undos);
            alpha = max(alpha, score);
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
    if (generator.ended()) {
        return {};
    }

    int bestScore = MIN;
    Move bestMove;

    while(!generator.ended()) {
        for (const auto& m : generator.nextMoves()) {
            int undos = b.apply(m);
            if (hstats.get(b) == 2u) {
                // 3-fold repetition
                // skip this move
                b.undo(undos);
                continue;
            }
            const auto score = -negamax(b, enemyColor(c), MIN, MAX, depth % 2 == 0, depth - 1u);
            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
            b.undo(undos);
        }
    }

    if (bestScore == MIN) {
        return {};
    }

    return std::make_optional(bestMove);
}

int main()
{
    Board b;
    std::cout << b << std::endl << std::endl;

    auto c = Color::WHITE;
    bool computerPlays = true;

    while (true) {
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
                    const auto sq =  square(Figure::ROOK_IDLE, color(m.toSq));
                if (color(m.toSq) == Color::WHITE) {
                    if (m.to.y == 0 && m.to.x == 6 && figure(b.get(7, 0)) == Figure::ROOK_IDLE) {
                        b.apply(Move { 7, 0, 5, 0, sq });
                    } else if (m.to.y == 0 && m.to.x == 2 && figure(b.get(0, 0)) == Figure::ROOK_IDLE) {
                        b.apply(Move { 0, 0, 3, 0, sq });
                    }
                } else {
                    if (m.to.y == 7 && m.to.x == 6 && figure(b.get(7, 7)) == Figure::ROOK_IDLE) {
                        b.apply(Move { 7, 7, 5, 7, sq });
                    } else if (m.to.y == 7 && m.to.x == 2 && figure(b.get(0, 7)) == Figure::ROOK_IDLE) {
                        b.apply(Move { 0, 7, 3, 7, sq });
                    }
                }
            }
            b.apply(m);
            computerPlays = true;
        } else {
            computerPlays = false;
            const auto now = std::chrono::system_clock::now();
            const auto m = bestMove(b, c);
            const auto dur = std::chrono::duration<double>(std::chrono::system_clock::now() - now);
            std::cout << "DURATION: " << dur.count() << std::endl;

            if (!m) {
                std::cout << "NO MOVE AVAILABLE" << std::endl;
                return 1;
            }
            std::cout << "MOVE: " << char(m->from.x + 'a') << (m->from.y + 1) << char(m->to.x + 'a') << (m->to.y + 1) << std::endl;

            b.apply(*m);
            hstats.inc(b);
        }
        c = enemyColor(c);
        std::cout << b << std::endl;
        std::cout << std::endl
                  << std::endl;
    }

    return 0;
}