# Google Test and OpenNURBS

This is a basic project to get OpenNURBS working with Google Test.

Based on this Google Test Quickstart: https://google.github.io/googletest/quickstart-cmake.html

## Associated files

- rhino/src4/opennurbs/tests/test_ClassSize.cpp
- rhino/src4/opennurbs/tests/CMakeLists.txt

## building & running

- `cmake -S . -B build`
- `cmake --build build`
- `cd build && ctest`

## Notes

- Since `ON_WindowsDWriteFontInformation` isn't available for macos or linux, we have to comment out that test before building.
- 2023.12.28 On macos and linux the following tests fail:
  - `ON_3dmRevisionHistory` 96 expected / 136 calculated
  - `ON_3dmProperties` 512 expected / 552 calculated
  - `ON_SleepLock` 4 expected / 1 calculated