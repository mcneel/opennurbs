# OpenNURBS FileIO

This is a project to test OpenNURBS FileIO. It uses GoogleTest Value Parameterized tests: http://google.github.io/googletest/advanced.html#value-parameterized-tests and is based on some of the code from example_test.

## Associated files

- rhino/src4/opennurbs/tests/fileio/CMakeLists.txt
- rhino/src4/opennurbs/tests/fileio/test_ON_FileIO.cpp

## building & running

From the example_test directory:

- `cmake -S . -B build`
- `cmake --build build`
- `cd build && ./test_ON_FileIO` (macos)

## TODO

- Complete GetGileList() method to iterate over a root and return an array of all of the files to test.