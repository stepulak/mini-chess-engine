
#include "board.hpp"
#include "figure_moves.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <optional>
#include <string>
#include <unordered_set>

/*
class TranspositionTable {
public:
    struct Value {
        size_t depth;
        int score;
        int alpha;
        int beta;
    };

    TranspositionTable() = default;

    std::optional<Value> get(const Board::BoardType& b) const
    {
        const auto it = _table.find(b);
        if (it == _table.end()) {
            return {};
        }
        return std::make_optional(it->second);
    }

    void set(const Board::BoardType& b, Value v)
    {
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
    std::unordered_map<Board::BoardType, Value> _table;
};

class HistoryStats {
public:
    HistoryStats() = default;

    size_t get(const Board::BoardType& b) const
    {
        const auto it = _stats.find(b);
        if (it == _stats.end()) {
            return 0;
        }
        return it->second;
    }

    void inc(const Board::BoardType& b)
    {
        _stats[b]++;
    }

private:
    std::unordered_map<Board::BoardType, size_t> _stats;
};

static TranspositionTable tt;
static HistoryStats hstats;
*/

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

int tthit = 0;
int bhit = 0;
int aihit = 0;
int bihit = 0;
int mhit = 0;
int ttmiss = 0;
int nodes = 0;

const int MOVE_LIM_BASE = 150;

int negamax(Board& b, Color c, int alpha, int beta, bool maxing, size_t depth) {
    if (b.kingCaptured() || depth <= 0) {
        return b.score() * (maxing ? 1 : -1);
    }
    /*
    if (depth <= 3) {
        int lim = MOVE_LIM_BASE * int(4u - depth);
        if (maxing && b.score() > lim) {
            goto cont;
        } else if (!maxing && b.score() < -lim) {
            goto cont;
        } else {
            return b.score() * (maxing ? 1 : -1);
        }
    }

cont:*/

    nodes++;

    const auto alphaOrig = alpha;
    const auto betaOrig = beta;
    int score = MIN;

    /*
    const auto it = tt.get(b.bitboard());
    if (it && it->depth >= depth) {
        bhit++;
        const bool ai = inInterval(alpha, int(it->alpha*1), int(it->beta*1));
        const auto bi = inInterval(beta, int(it->alpha*1), int(it->beta*1));

        if (ai && bi) {
            tthit++;
            return it->score;
        }
        if (ai) {
            aihit++;
            score = it->score;
            alpha = it->alpha;
        } else if (bi) {
            bihit++;
            score = it->score;
            beta = it->beta;
        }
    }

    ttmiss++;*/

    for (const auto& m : generateMoves(b, c)) {
        int undos = b.applyMove(m);
        score = max(score, -negamax(b, enemyColor(c), -beta, -alpha, maxing, depth-1));
        b.undo();
        if (undos == 2) {
            b.undo();
        }
        alpha = max(alpha, score);
        if (alpha >= beta) {
            break;
        }
    }

    if (it && it->depth > depth) {
        return score;
    }

    TranspositionTable::Value tv;
    tv.depth = depth;
    tv.score = score;
    tv.alpha = alphaOrig;
    tv.beta = betaOrig;
    tt.set(b.bitboard(), tv);

    return score;
}

std::optional<Move> bestMove_(Board& b, Color c, size_t depth)
{
    const auto moves = generateMoves(b, c);
    if (moves.empty()) {
        return {};
    }

    int bestScore = MIN;
    Move bestMove;

    for (const auto& m : moves) {
        int undos = b.applyMove(m);
        if (hstats.get(b.bitboard()) == 2u) {
            // 3-fold repetition
            // skip this move
            b.undo();
            if (undos == 2) {
                b.undo();
            }
            continue;
        }
        const auto score = -negamax(b, enemyColor(c), MIN, MAX, depth % 2 == 0, depth - 1u);
        //std::cout << score << std::endl;
        /*if ((c == WHITE && score > bestScore) || (c == BLACK && score < bestScore)) {
            bestScore = score;
            bestMove = m;
        }*/
        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
        b.undo();
        if (undos == 2) {
            b.undo();
        }
    }

    if (bestScore == MIN) {
        return {};
    }

    return std::make_optional(bestMove);
}

std::optional<Move> bestMove(Board& b, Color c, size_t depth = 8u)
{
    std::optional<Move> bm;
    for (size_t d = depth; d <= depth; d++) {
        bm = bestMove_(b, c, d);
    }
    return bm;
}

int main()
{
    std::cout << "♛" << std::endl;

    Board b;
    b.print();
    std::cout << std::endl
              << std::endl;
    auto c = WHITE;
    bool computerPlays = true;

    while (true) {
        std::cout << "NOW PLAYS: " << (c == WHITE ? "white" : "black") << std::endl;
        if (!computerPlays) {
            std::cout << "MOVE: ";
            std::string move;
            std::cin >> move;
            Move m;
            m.fX = move[0] - 'a';
            m.fY = move[1] - '1';
            m.tX = move[2] - 'a';
            m.tY = move[3] - '1';
            const auto from = b.get(m.fX, m.fY);
            m.figure = figure(from);

            if (m.figure == KING_NOT_MOVED) {
                if (color(from) == WHITE) {
                    if (m.tY == 0 && m.tX == 6 && figure(b.get(7, 0)) == ROOK_NOT_MOVED) {
                        b.applyMove(Move { 7, 0, 5, 0, ROOK_NOT_MOVED });
                    } else if (m.tY == 0 && m.tX == 2 && figure(b.get(0, 0)) == ROOK_NOT_MOVED) {
                        b.applyMove(Move { 0, 0, 3, 0, ROOK_NOT_MOVED });
                    }
                } else {
                    if (m.tY == 7 && m.tX == 6 && figure(b.get(7, 7)) == ROOK_NOT_MOVED) {
                        b.applyMove(Move { 7, 7, 5, 7, ROOK_NOT_MOVED });
                    } else if (m.tY == 7 && m.tX == 2 && figure(b.get(0, 7)) == ROOK_NOT_MOVED) {
                        b.applyMove(Move { 0, 7, 3, 7, ROOK_NOT_MOVED });
                    }
                }
            }
            b.applyMove(m);
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
            std::cout << "MOVE: " << char(m->fX + 'a') << (m->fY + 1) << char(m->tX + 'a') << (m->tY + 1) << std::endl;

            b.applyMove(*m);
            hstats.inc(b.bitboard());
        }
        c = enemyColor(c);
        b.print();
            std::cout << "TTHIT: " << tthit << std::endl;
            std::cout << "BHIT: " << bhit << std::endl;
            std::cout << "AIHIT: " << aihit << std::endl;
            std::cout << "BIHIT: " << bihit << std::endl;
            std::cout << "MHIT: " << mhit << std::endl;
            std::cout << "TTMISS: " << ttmiss << std::endl;
            std::cout << "NODES: " << nodes << std::endl;
        std::cout << std::endl
                  << std::endl;
    }

    return 0;
}