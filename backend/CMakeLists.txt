cmake_minimum_required(VERSION 3.10)
project(CueLab VERSION 1.0)

# Web Backend
add_subdirectory(web)

# Tests
add_subdirectory(tests)
include(tests.cmake)
