cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_FLAGS_DEBUG "-g -ldl -export-dynamic")
set(CMAKE_CXX_FLAGS_RELEASE "-g -ldl -export-dynamic -O3") # TODO (Carter) Remove -g in release

file(GLOB expr_headers
    "${PROJECT_SOURCE_DIR}/src/include/*.hpp"
)

file(GLOB expr_src
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(expr_lib
    ${expr_headers}
    ${expr_src}
)
target_include_directories(expr_lib INTERFACE "${PROJECT_SOURCE_DIR}/src/include")
target_link_libraries(expr_lib exl ${llvm_libs})

add_executable(${PROJECT_NAME} entry.cpp)
target_link_libraries(${PROJECT_NAME} expr_lib)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/src/include"
    PREFIX "Header Files"
    FILES ${expr_headers}""
)