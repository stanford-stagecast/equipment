cmake_minimum_required(VERSION 3.10)
project(CueLab VERSION 1.0)

# Compiler Settings (mostly taken from CS144)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -pedantic -pedantic-errors -Werror -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wformat=2 -Weffc++ -Wold-style-cast")

# Web Backend
add_subdirectory(web)

# Tests
add_subdirectory(tests)
include(tests.cmake)
