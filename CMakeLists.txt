cmake_minimum_required(VERSION 3.25)
project(SDL-bench)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS source/*.cpp source/*.h)

add_executable(SDL-bench WIN32 ${SRCS})
target_link_libraries(SDL-bench PRIVATE SDL3-shared)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRCS})