cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_FLAGS_DEBUG "-g -ldl -export-dynamic")

file(GLOB expr_headers
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
)

file(GLOB expr_src
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(expr_lib
    ${expr_headers}
    ${expr_src}
)
target_include_directories(expr_lib INTERFACE ../include)
target_link_libraries(expr_lib exl)

add_executable(${PROJECT_NAME} entry.cpp)
target_link_libraries(${PROJECT_NAME} expr_lib)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${expr_headers}
)