#pragma once

#include "board.hpp"

#include <unordered_map>

class BoardStats {
public:
    BoardStats() = default;

    bool threeFoldRepetition(const Board& b) const
    {
        const auto it = _positionOccurrences.find(b);
        if (it == _positionOccurrences.end()) {
            return false;
        }
        return it->second >= 2u;
    }

    void boardVisit(const Board& b)
    {
        _positionOccurrences[b]++;
    }

private:
    struct BoardHash {
        size_t operator()(const Board& b) const
        {
            return b.hash();
        }
    };

    std::unordered_map<Board, size_t, BoardHash> _positionOccurrences;
}