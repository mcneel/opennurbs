# OpenNURBS FileIO

This is a basic project to test OpenNURBS FileIO.

## Associated files

- rhino/src4/opennurbs/example_test/CMakeLists.txt
- rhino/src4/opennurbs/example_test/example_test.cpp

## building & running

From the example_test directory:

- `cmake -S . -B build`
- `cmake --build build`
- `cd build && ./example_test -r ../../example_files`

## Notes

- This is a test that I will eventually convert to gtest