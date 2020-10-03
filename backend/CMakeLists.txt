cmake_minimum_required(VERSION 3.10)
project(CueLab VERSION 1.0)

# Web Backend
add_subdirectory(web)

# Tests
enable_testing()
add_test(NAME web_sample COMMAND web_sample)
