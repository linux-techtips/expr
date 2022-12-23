
cmake_minimum_required(VERSION 3.24)
project(
    expr
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_COLOR_NINJA ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(exprlib)
add_subdirectory(src)