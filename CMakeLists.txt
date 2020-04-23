cmake_minimum_required(VERSION 3.16)
project(Snake_practice)

set(CMAKE_CXX_STANDARD 14)

add_executable(Snake_practice main.cpp Snake.cpp Snake.h)

target_link_libraries(Snake_practice ${CMAKE_SOURCE_DIR}/lib/libncursesw.6.dylib)