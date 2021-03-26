#pragma once

#include "board.hpp"

#include <unordered_map>

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

    void visit(const Board& b)
    {
        _visits[b]++;
    }

private:
    struct BoardHash {
        size_t operator()(const Board& b) const
        {
            return b.hash();
        }
    };
    std::unordered_map<Board, size_t, BoardHash> _visits;
};