cmake_minimum_required(VERSION 3.15)

project(opennurbs CXX C)

if(NOT "${CMAKE_CXX_STANDARD}")
  # setting the C++ standard if not specified
  if(DEFINED CMAKE_CXX20_STANDARD_COMPILE_OPTION
     OR DEFINED CMAKE_CXX20_EXTENSION_COMPILE_OPTION)
    set(CXX_LATEST_STANDARD 20)
  elseif(DEFINED CMAKE_CXX17_STANDARD_COMPILE_OPTION
         OR DEFINED CMAKE_CXX17_EXTENSION_COMPILE_OPTION)
    set(CXX_LATEST_STANDARD 17)
  elseif(DEFINED CMAKE_CXX14_STANDARD_COMPILE_OPTION
         OR DEFINED CMAKE_CXX14_EXTENSION_COMPILE_OPTION)
    set(CXX_LATEST_STANDARD 14)
  else()
    set(CXX_LATEST_STANDARD 11)
  endif()
  set(CMAKE_CXX_STANDARD ${CXX_LATEST_STANDARD})
endif()
message(STATUS "CMAKE_CXX_STANDARD: ${CXX_LATEST_STANDARD}")

# opennurbs source
file(GLOB opennurbs_SOURCE "${CMAKE_SOURCE_DIR}/*.h"
     "${CMAKE_SOURCE_DIR}/*.cpp")
# exclude the examples
list(FILTER opennurbs_SOURCE EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/example*")

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(REMOVE_ITEM opennurbs_SOURCE
       "${CMAKE_SOURCE_DIR}/opennurbs_unicode_cp932.cpp")
  list(REMOVE_ITEM opennurbs_SOURCE
       "${CMAKE_SOURCE_DIR}/opennurbs_unicode_cp949.cpp")
endif()

# remove opennurbs_gl if no opengl
find_package(OpenGL)
if(NOT ${OPENGL_FOUND})
  message(WARNING "OpenGL not found. Excluding opennurbs_gl")
  list(REMOVE_ITEM opennurbs_SOURCE "${CMAKE_SOURCE_DIR}/opennurbs_gl.cpp")
endif()

# Build the opennurbs library
option(OPENNURBS_SHARED "Build shared libraries" OFF)
if(${OPENNURBS_SHARED})
  # if dynamic

  # opennurbs shared library
  add_library(opennurbs SHARED ${opennurbs_SOURCE})

  # define opennurbs_EXPORTS to compile as a shared library
  target_compile_definitions(opennurbs PRIVATE OPENNURBS_EXPORTS
                                               ON_COMPILING_OPENNURBS)

  # define OPENNURBS_IMORTS for the usage of the library
  message(
    WARNING
      "Define `OPENNURBS_IMORTS` when using the opennurbs.dll before including opennurbs_public.h\n  target_compile_definitions(main PUBLIC OPENNURBS_IMPORTS)"
  )
else()
  # if static

  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_library(opennurbs STATIC ${opennurbs_SOURCE})
  else()
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL
                                             "Android")
      # Include UUID source (bundled with opennurbs)
      file(GLOB UUID_SRC "${CMAKE_SOURCE_DIR}/android_uuid/*.h"
           "${CMAKE_SOURCE_DIR}/android_uuid/*.c")
      list(REMOVE_ITEM UUID_SRC
           "${CMAKE_SOURCE_DIR}/android_uuid/gen_uuid_nt.c")
    endif()

    # Need to combine all source files for static linking on non-windows
    add_library(opennurbs STATIC ${UUID_SRC} ${opennurbs_SOURCE})
  endif()

  # define ON_COMPILING_OPENNURBS to compile as a static library
  target_compile_definitions(opennurbs PRIVATE ON_COMPILING_OPENNURBS)
endif()

# compile definitions
target_compile_definitions(
  opennurbs
  PRIVATE
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
option({OPENNURBS_EXTERNAL_ZLIB, "use external zlib package" OFF)
if(${OPENNURBS_EXTERNAL_ZLIB} OR ${OPENNURBS_ZLIB_LIB_DIR})
  message(STATUS "Using external ZLIB")
  find_package(ZLIB REQUIRED)
  target_link_libraries(opennurbs PRIVATE ZLIB::ZLIB)
  target_compile_definitions(ZLIB::ZLIB PRIVATE MY_ZCALLOC Z_PREFIX)
  get_target_property(OPENNURBS_ZLIB_LIB_DIR ZLIB::ZLIB
                      LIBRARY_OUTPUT_DIRECTORY)
  target_compile_definitions(
    opennurbs PRIVATE OPENNURBS_ZLIB_LIB_DIR="${OPENNURBS_ZLIB_LIB_DIR}")
else()
  # build zlib (bundled with opennurbs)
  file(GLOB ZLIB_SOURCE "${CMAKE_SOURCE_DIR}/zlib/*.h"
       "${CMAKE_SOURCE_DIR}/zlib/*.c")
  add_library(zlib ${ZLIB_SOURCE})
  target_compile_definitions(zlib PRIVATE MY_ZCALLOC Z_PREFIX)
  set_target_properties(
    zlib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
                    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>")
  set(opennurbs_ZLIB_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>")
  target_compile_definitions(
    opennurbs PRIVATE opennurbs_ZLIB_LIB_DIR=${opennurbs_ZLIB_LIB_DIR})
  target_link_libraries(opennurbs PRIVATE zlib)
endif()
# zlib-specific flags for opennurbs
target_compile_definitions(opennurbs PRIVATE MY_ZCALLOC Z_PREFIX)

# shlwapi on windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(opennurbs PRIVATE shlwapi)
endif()

# cross_dirent
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  find_path(DIRENT_INCLUDE_DIR dirent.h)
  if(${DIRENT_INCLUDE_DIR_FOUND})
    target_include_directories(opennurbs PRIVATE ${DIRENT_INCLUDE_DIR_FOUND})
  else()
    file(DOWNLOAD https://github.com/tronkko/dirent/raw/1.23.2/include/dirent.h
         ${CMAKE_CURRENT_BINARY_DIR}/_deps/dirent/include/dirent.h)
    target_include_directories(
      opennurbs PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/_deps/dirent/include/")
  endif()
endif()

# uuid with clang on non-linux, android, windows
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux"
   AND NOT CMAKE_SYSTEM_NAME STREQUAL "Windows"
   AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  # uuid/uuid.h is required by opennurbs
  message(STATUS "Finding uuid library")
  find_package(UUID QUIET)
  if(${UUID_FOUND})
    target_include_directories(opennurbs PRIVATE ${UUID_INCLUDE_DIRS})
  else()
    # find uuid manually
    if(NOT UUID_INCLUDE_DIR)
      find_path(UUID_INCLUDE_DIR uuid/uuid.h)
    endif()
    if(EXISTS "${UUID_INCLUDE_DIR}")
      include(CheckCXXSymbolExists)

      set(UUID_INCLUDE_DIRS ${UUID_INCLUDE_DIR})
      set(CMAKE_REQUIRED_INCLUDES ${UUID_INCLUDE_DIRS})
      check_cxx_symbol_exists("uuid_generate_random" "uuid/uuid.h"
                              _uuid_header_only)
      if(NOT _uuid_header_only AND NOT UUID_LIBRARY)
        include(CheckLibraryExists)
        check_library_exists("uuid" "uuid_generate_random" "" _have_libuuid)
        if(_have_libuuid)
          set(UUID_LIBRARY "uuid")
        endif()
      endif()
    endif()

    if(UUID_LIBRARY)
      set(UUID_LIBRARIES ${UUID_LIBRARY})
    endif()

    unset(CMAKE_REQUIRED_INCLUDES)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(uuid DEFAULT_MSG UUID_INCLUDE_DIR)

    # finally include it
    target_include_directories(opennurbs PRIVATE ${UUID_INCLUDE_DIRS})
  endif()
endif()

# OpenGL
if(${OPENGL_FOUND})
  target_link_libraries(opennurbs PRIVATE OpenGL::GL OpenGL::GLU)
endif()

# Suppress -Wdefaulted-function-deleted on Clang
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  # TODO fix this
  message(WARNING "Suppressing -Wdefaulted-function-deleted for opennurbs")
  target_compile_options(opennurbs PRIVATE -Wno-defaulted-function-deleted)
endif()

# Set the outputs of opennurbs CMake
set(opennurbs_LIBRARY ${opennurbs})
set(opennurbs_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}")
