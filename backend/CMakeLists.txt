cmake_minimum_required(VERSION 3.10)
project(CueLab VERSION 1.0)

# Compiler Settings (mostly taken from CS144)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -pedantic -pedantic-errors -Werror -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wformat=2 -Weffc++ -Wold-style-cast")

# Packages
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.71 REQUIRED COMPONENTS thread system serialization)
message(STATUS "Boost Version: ${Boost_VERSION}")

# Web Backend
add_subdirectory(web)

# Mixer
add_subdirectory(mixer)

# CLI MIDI client
add_subdirectory(cli)

# DMX transmitter
add_subdirectory(dmx)

# Tests
add_subdirectory(tests)
include(tests.cmake)
