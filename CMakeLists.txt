cmake_minimum_required(VERSION 3.9)

project(Dijkstra)

set( CMAKE_CXX_FLAGS "-std=c++11" )
set( CMAKE_BUILD_TYPE DEBUG)

add_executable(Dijkstra src/Model.hpp src/main.cpp src/Graph.hpp src/Cheapest.hpp src/Fastest.hpp src/Fastest_with_waiting.hpp)