cmake_minimum_required(VERSION 3.16)
project(ON_Test)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(../../opennurbs build_opennurbs)

add_executable(
  example_test
  example_test.cpp
)

target_link_libraries(
  example_test
  OpenNURBS
)