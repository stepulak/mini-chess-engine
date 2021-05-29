cmake_minimum_required(VERSION 3.5)

project(mini_chess_engine)

set(CMAKE_CXX_FLAGS "-std=c++17 -pthread -O3")

add_executable(mce ai.cpp board.cpp figure_moves.cpp figures.cpp main.cpp)