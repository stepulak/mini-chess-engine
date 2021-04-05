
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

Move parseMove(const std::string& str, const Board& board)
{
    if (str.length() != 4u) {
        throw std::runtime_error("invalid move");
    }
    Move m;
    m.castling = false;
    m.from.x = str[0] - 'a';
    m.from.y = str[1] - '1';
    m.to.x = str[2] - 'a';
    m.to.y = str[3] - '1';
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

    return m;
}

void playerPlays(Board& board, BoardStats& boardStats, Color col)
{
    Move m;

    while(true) {
        std::cout << "Your move: ";
        std::string input;
        std::cin >> input;

        try {
            m = parseMove(input, board);
        } catch(const std::exception& ex) {
            std::cout << "Parse error: " << ex.what() << std::endl;
        }
        if (!figureMoveValid(m, board, col)) {
            // Parse OK, move valid
            break;
        }
        std::cout << "Invalid move" << std::endl;
    }

    board.applyMove(m);
    boardStats.visit(board);
}

void computerPlays(Board& board, BoardStats& boardStats, Color col) {
    AI ai(board, col, boardStats);
    Timer timer(2000u, [&] {
        ai.stop();
    });
    ai.run();
    timer.stop();
    timer.join();

    const auto m = ai.bestMove();

    const auto dur = std::chrono::duration<double>(std::chrono::system_clock::now() - now);
    if (!m) {
        throw std::runtime_error("")
    }
    board.applyMove(*m);
    boardStats.visit(board);
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