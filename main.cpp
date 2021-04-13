
#include "ai.hpp"
#include "board.hpp"
#include "board_stats.hpp"
#include "figure_moves.hpp"
#include "timer.hpp"

#include <iostream>

/*
TODO:
player move validation
endgame:
    draw
    win
    loss
*/

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

void computerPlays(Board& board, BoardStats& boardStats, Color col)
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
        throw std::runtime_error("");
    }

    board.applyMove(*m);
    boardStats.visit(board);
}

enum class GameStatus {
    CONTINUE,
    DRAW,
    WIN_OR_LOSS
};

template<typename Fn>
void forEachMove(Board& b, Color c, Fn&& f){
    for(auto generator = board.moveGenerator(c); generator.hasMoves(); ) {
        for (const auto& m : generator.movesChunk()) {
            const auto undos = board.applyMove(m);
            f(m);
            board.undoMove(undos);
        }
    }
}

GameStatus gameStatus(Board& board, Color col)
{
    if (board.kingCaptured()) {
        return GameStatus::WIN_OR_LOSS;
    }
    const auto ecol = enemyColor(col);
    bool draw = true;

    forEachMove(board, ecol, [&](const auto& m1) {
        forEachMove(board, col, [&](const auto& m2) {
            if (draw && !board.kingCaptured()) {
                draw = false;
                return;
            }
        });
    });

    if (draw) {
        return GameStatus::DRAW;
    }

    return GameStatus::CONTINUE;
}

} // namespace

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
            m.castling = false;
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
            bstats.visit(b);
            computerPlays = true;
        } else {
            computerPlays = false;
            const auto now = std::chrono::system_clock::now();

            AI ai(b, c, bstats);
            Timer t(2000u, [&] {
                std::cout << "TIMER FINISHED!" << std::endl;
                ai.stop();
            });
            ai.run();
            t.stop();
            t.join();
            const auto m = ai.bestMove();

            const auto dur = std::chrono::duration<double>(std::chrono::system_clock::now() - now);
            std::cout << "DURATION: " << dur.count() << std::endl;

            if (!m) {
                std::cout << "NO MOVE AVAILABLE" << std::endl;
                return 1;
            }
            std::cout << "MOVE: " << char(m->from.x + 'a') << (m->from.y + 1) << char(m->to.x + 'a') << (m->to.y + 1) << std::endl;

            b.applyMove(*m);
            bstats.visit(b);
        }
        c = enemyColor(c);
        std::cout << b << std::endl;
        std::cout << std::endl
                  << std::endl;
    }

    return 0;
}