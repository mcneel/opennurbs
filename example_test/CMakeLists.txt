cmake_minimum_required(VERSION 3.25)
project(ON_Test)

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

target_compile_features(example_test PRIVATE cxx_std_20)