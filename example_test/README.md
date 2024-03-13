# OpenNURBS FileIO

This is a basic project to test OpenNURBS FileIO.

## prerequisites

- A compatible operating system (e.g. Linux, macOS, Windows).
- A compatible C++ compiler that supports at least C++14. These tests are set to use C++17.
- CMake (3.16 and above) and a compatible build tool for building the project. These will depend on the operating system. In general:
  - macos: apple clang (usually installed via xcode)
  - windows: MSVC (usually installed via Visual Studio)
  - linux: gcc

## associated files

- rhino/src4/opennurbs/example_test/CMakeLists.txt
- rhino/src4/opennurbs/example_test/example_test.cpp

## building & running

From the `src4/opennurbs/example_test` directory:

- `cmake -S . -B build`
- `cmake --build build`
- `cd build && ./example_test -r ../../example_files`
- run
  - macos: `cd build && ./example_test -r ../../example_files`
  - windows: `cd build/Debug && ./example_test -r ../../example_files`

## notes

- Part of the functionality of this test has been converted to a GoogleTest in `src4/opennurbs/tests/fileio`