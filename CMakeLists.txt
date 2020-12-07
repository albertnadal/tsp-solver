cmake_minimum_required(VERSION 3.17)
project(tsp C)

include_directories(/usr/local/Cellar/raylib/3.0.0/include)
link_directories(/usr/local/Cellar/raylib/3.0.0/lib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c99 -lraylib")

add_executable(tsp tsp.c)
add_executable(tsp_visual tsp_visual.c)
