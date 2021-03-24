#pragma once

#include "board.hpp"

#include <unordered_map>
#include <cmath>

#include <iostream> // todo remove

class BoardStats {
public:
    BoardStats() = default;

    bool threeFoldRepetition(const Board& b) const
    {
        const auto it = _visits.find(b);
        if (it == _visits.end()) {
            return false;
        }
        return it->second >= 2u;
    }

    size_t minimaxDepth() const
    {
        return _minimaxDepth;
    }

    void lastVisit(const Board& b)
    {
        _visits[b]++;
        _minimaxDepth = countMinimaxDepth(b);
    }

private:
    struct BoardHash {
        size_t operator()(const Board& b) const
        {
            return b.hash();
        }
    };
    std::unordered_map<Board, size_t, BoardHash> _visits;
    size_t _minimaxDepth = 6u;

    static size_t countMinimaxDepth(const Board& b) {
        static std::unordered_map<Figure, int> metrics = {
            { Figure::PAWN, 3 },
            { Figure::PAWN_EN_PASSANT, 3 },
            { Figure::PAWN_IDLE, 4 },
            { Figure::KNIGHT, 8 },
            { Figure::BISHOP, 14 },
            { Figure::ROOK, 14 },
            { Figure::ROOK_IDLE, 14 },
            { Figure::QUEEN, 31 },
            { Figure::KING_IDLE, 10 },
            { Figure::KING, 8 },
        };
        const int maxscore = 2*(4*8+8*2+14*2+14*2+31+10);
        const int mindepth = 6u;
        int score = 0;
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                const auto f = figure(b.get(x, y));
                if (f == Figure::NONE) {
                    continue;
                }
                score += metrics.at(f);
            }
        }
        size_t depth = 6u + 6u - (std::log(score) / std::log(2.57) - 0.01);
        std::cout << (6u - (std::log(score) / std::log(2.57) - 0.01)) << std::endl;
        /*if (depth % 2 == 1) {
            depth--;
        }*/
        return depth;
    }
};