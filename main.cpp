
#include "ai.hpp"
#include "board.hpp"
#include "board_stats.hpp"
#include "figure_moves.hpp"
#include "timer.hpp"

#include <iostream>

namespace {

constexpr auto COMPUTER_PLAY_TIME = 2000u;

inline bool pointValid(const Point& p)
{
    return Board::validIndex(p.x, p.y);
}

bool detectCastling(const Point& from, const Point& to, const Board& board)
{
    if (figure(board.get(from.x, from.y)) != Figure::KING_IDLE) {
        return false;
    }
    if (to.x == 6 && figure(board.get(7, to.y)) == Figure::ROOK_IDLE) {
        return true;
    }
    if (to.x == 2 && figure(board.get(0, to.y)) == Figure::ROOK_IDLE) {
        return true;
    }
    return false;
}

std::optional<Move> parseMove(const std::string& str, const Board& board)
{
    const Point from = { str[0] - 'a', str[1] - '1' };
    const Point to = { str[2] - 'a', str[3] - '1' };

    if (!pointValid(from) || !pointValid(to)) {
        return {};
    }
    const auto castling = detectCastling(from, to, board);
    const auto sq = board.get(from.x, from.y);
    const auto fig = figure(sq);
    const auto col = color(sq);

    Figure resultFigure;

    if (fig == Figure::KING_IDLE) {
        resultFigure = Figure::KING;
    } else if (fig == Figure::ROOK_IDLE) {
        resultFigure = Figure::ROOK;
    } else if (fig == Figure::PAWN_IDLE) {
        resultFigure = std::abs(from.y - to.y) == 2 ? Figure::PAWN_EN_PASSANT : Figure::PAWN;
    } else if (fig == Figure::PAWN_EN_PASSANT) {
        resultFigure = Figure::PAWN;
    } else {
        resultFigure = fig;
    }

    return Move { from, to, square(resultFigure, col), castling };
}

void playerPlays(Board& board, BoardStats& boardStats, Color col)
{
    while (true) {
        std::cout << "Enter valid move: ";
        std::string input;
        std::cin >> input;

        const auto m = parseMove(input, board);
        if (!m || !figureMoveValid(*m, board, col)) {
            continue;
        }

        board.applyMove(*m);
        boardStats.visit(board);
        break;
    }
}

Move computerPlays(Board& board, BoardStats& boardStats, Color col)
{
    AI ai(board, col, boardStats);
    Timer timer(COMPUTER_PLAY_TIME, [&] {
        ai.stop();
    });

    ai.run();
    timer.stop();
    // Wait for AI to finish
    timer.join();

    const auto m = ai.bestMove();
    if (!m) {
        throw std::runtime_error("Computer - no move available");
    }
    board.applyMove(*m);
    board.clearUndoMoves();
    boardStats.visit(board);

    /*TODO REMOVE*/std::cout << "SCORE: " << board.score() << std::endl; 

    std::cout << "My move: " << *m << std::endl << std::endl;
}

enum class GameStatus {
    CONTINUE,
    DRAW,
    WIN_LOSS
};

template <typename Fn>
void forEachMove(Board& b, Color c, Fn&& f)
{
    for (auto generator = b.moveGenerator(c); generator.hasMoves();) {
        for (const auto& m : generator.movesChunk()) {
            const auto undos = b.applyMove(m);
            f(m);
            b.undoMove(undos);
        }
    }
}

GameStatus gameStatus(Board& board, Color col)
{
    if (board.kingCaptured()) {
        return GameStatus::WIN_LOSS;
    }
    const auto ecol = enemyColor(col);
    bool draw = true;

    forEachMove(board, col, [&](const auto&) {
        if (draw) {
            bool kingAlive = true;
            forEachMove(board, ecol, [&](const auto&) {
                kingAlive = kingAlive && !board.kingCaptured();
            });
            if (kingAlive) {
                draw = false;
            }
        }
    });

    if (draw) {
        return GameStatus::DRAW;
    }

    return GameStatus::CONTINUE;
}

bool resolveGameStatus(Board& board, Color col, bool computerTurn, GameStatus status)
{
    if (status == GameStatus::WIN_LOSS) {
        if (computerTurn) {
            std::cout << "You have lost." << std::endl;
        } else {
            std::cout << "You have won!" << std::endl;
        }
        return true;
    }
    if (status == GameStatus::DRAW) {
        std::cout << "Draw!" << std::endl;
        return true;
    }
    return false;
}

} // namespace

int main()
{
    Board board;
    BoardStats boardStats;

    auto color = Color::WHITE;
    bool computerTurn = true;

    while (true) {
        std::cout << board;

        const auto status = gameStatus(board, color);
        if (resolveGameStatus(board, color, computerTurn, status)) {
            break;
        }

        if (computerTurn) {
            computerPlays(board, boardStats, color);
        } else {
            playerPlays(board, boardStats, color);
        }
        computerTurn = !computerTurn;
        color = enemyColor(color);
    }

    return 0;
}
