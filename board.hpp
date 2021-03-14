#pragma once

#include "figures.hpp"
#include "move.hpp"

#include <array>

class Board {
public:
    using BoardType = std::array<Square, 64u>;

    // Intentionaly not using size_t
    static constexpr int WIDTH = 8;
    static constexpr int HEIGHT = 8;
    static constexpr int SIZE = WIDTH * HEIGHT;

    class MoveGenerator {
    public:
        MoveGenerator() = delete;
        MoveGenerator(const Board& b, Color c);

        Moves movesChunk();

        bool hasMoves() const
        {
            return _y < Board::HEIGHT;
        }

    private:
        const Board& _board;
        const Color _color;
        int _x = 0;
        int _y = 0;

        void nextSquare();
    };

    Board();
    
    // TODO DELETE THIS:
    //Board(const Board&) = delete;
    //Board& operator=(const Board&) = delete;

    constexpr Square get(int pos) const
    {
        return _board[pos];
    }

    constexpr Square get(int x, int y) const
    {
        return get(position(x, y));
    }

    void set(int x, int y, Square sq)
    {
        set(position(x, y), sq);
    }

    void set(int pos, Square sq);

    int score() const
    {
        return _score;
    }

    MoveGenerator moveGenerator(Color c) const
    {
        return MoveGenerator(*this, c);
    }

    bool kingCaptured() const
    {
        return std::abs(_score) > KING_CAPTURED_MIN_SCORE;
    }

    size_t hash() const
    {
        return _hash;
    }

    bool operator==(const Board& b) const
    {
        return b._hash == _hash && b._board == _board;
    }

    static constexpr bool validIndex(int x, int y)
    {
        return x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT;
    }

    size_t applyMove(const Move& m);
    void undoMove(size_t numUndoMoves);

private:
    static constexpr auto KING_CAPTURED_MIN_SCORE = 70000;

    BoardType _board;
    UndoMoves _undoMoves;
    size_t _hash = 0u;
    int _score = 0;

    // Using int instead of size_t everywhere due to negative integers
    static constexpr int position(int x, int y)
    {
        return y * 8 + x;
    }
};

std::ostream& operator<<(std::ostream& os, const Board& b);