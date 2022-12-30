
cmake_minimum_required(VERSION 3.24)
project(
    expr
    VERSION 0.0.1
    LANGUAGES C CXX #thanks llvm
)

set(CMAKE_COLOR_NINJA ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(BUILD_ARCH "-mnative")
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs
    Analysis
    Core
    ExecutionEngine
    InstCombine
    Object
    RuntimeDyld
    ScalarOpts
    Support
)

add_subdirectory(exprlib)
add_subdirectory(src)
