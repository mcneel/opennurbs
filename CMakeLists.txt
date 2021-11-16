cmake_minimum_required(VERSION 3.15)

project(OpenNURBS CXX C)

# OpenNURBS source
file(GLOB OpenNURBS_SOURCE "${CMAKE_SOURCE_DIR}/*.h"
     "${CMAKE_SOURCE_DIR}/*.cpp")

# Build the opennurbs library
option(opennurbs_SHARED "Build shared libraries" OFF)
if(${opennurbs_SHARED})
  # if dynamic

  # Cannot build as shared library on Linux
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(
      FATAL_ERROR
        "Building OpenNURBS as a shared library is not supported on Linux")
  endif()

  # Generate a shared library
  add_library(opennurbs SHARED ${OpenNURBS_SOURCE})
  set_target_properties(opennurbs PROPERTIES DEBUG_POSTFIX "d")

  # define opennurbs_EXPORTS
  target_compile_definitions(opennurbs PRIVATE opennurbs_EXPORTS)
else()
  # if static

  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_library(opennurbs STATIC ${OpenNURBS_SOURCE})
  else()
    # Include UUID source (bundled with opennurbs)
    file(GLOB UUID_SRC "${CMAKE_SOURCE_DIR}/android_uuid/*.h"
         "${CMAKE_SOURCE_DIR}/android_uuid/*.c")
    list(REMOVE_ITEM UUID_SRC "${CMAKE_SOURCE_DIR}/android_uuid/gen_uuid_nt.c")

    # Need to combine all source files for static linking on non-windows
    add_library(opennurbs STATIC ${UUID_SRC} ${OpenNURBS_SOURCE})
  endif()
endif()

# compile definitions
target_compile_definitions(
  opennurbs
  PRIVATE
    ON_COMPILING_OPENNURBS
    OPENNURBS_INPUT_LIBS_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
    UNICODE)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows specific

  # Fix "WIN32" preprocessor definitions on x64
  if(${CMAKE_SIZEOF_VOID_P} EQUAL "8")
    string(REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    target_compile_definitions(opennurbs PRIVATE WIN64)
  endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux specific

  # Linux compiler definitions
  target_compile_definitions(opennurbs PRIVATE ON_RUNTIME_LINUX)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  target_compile_definitions(opennurbs PRIVATE ON_COMPILER_CLANG)
endif()

# Dependencies

# zlib
if(NOT ${opennurbs_ZLIB_LIB_DIR})
  # build zlib as static library (bundled with OpenNURBS)
  file(GLOB ZLIB_SOURCE "${CMAKE_SOURCE_DIR}/zlib/*.h"
       "${CMAKE_SOURCE_DIR}/zlib/*.c")
  add_library(zlib STATIC ${ZLIB_SOURCE})
  target_compile_definitions(zlib PRIVATE MY_ZCALLOC Z_PREFIX)
  target_compile_definitions(
    opennurbs
    PRIVATE
      opennurbs_ZLIB_LIB_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>")
else()
  target_compile_definitions(
    opennurbs PRIVATE opennurbs_ZLIB_LIB_DIR=${opennurbs_ZLIB_LIB_DIR})
endif()
# zlib-specific flags for opennurbs
target_compile_definitions(opennurbs PRIVATE MY_ZCALLOC Z_PREFIX)
target_link_libraries(opennurbs PRIVATE zlib)

# shlwapi on windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(opennurbs PRIVATE shlwapi)
endif()

# Set the outputs of OpenNURBS CMake
set(OpenNURBS_LIBRARY ${opennurbs})
set(OpenNURBS_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/opennurbs")
