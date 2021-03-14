#pragma once

#include "board.hpp"

#include <unordered_map>

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
            { Figure::BISHOP, 16 },
            { Figure::ROOK, 16 },
            { Figure::ROOK_IDLE, 16 },
            { Figure::QUEEN, 32 },
            { Figure::KING_IDLE, 10 },
            { Figure::KING, 10 },
        };
        const int maxscore = 2*(4*8+8*2+16*2+16*2+32+10);
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
        //std::cout << "XXXSCORE: " << score << " " << ( mindepth * maxscore / score) << std::endl;
        size_t depth = mindepth * maxscore / score;
        if (depth % 2 == 1) {
            depth--;
        }
        return depth;
    }
};