# openNURBS

[![Discourse users](https://img.shields.io/discourse/https/discourse.mcneel.com/users.svg)](https://discourse.mcneel.com/c/opennurbs)

The openNURBS Initiative provides CAD, CAM, CAE, and computer graphics software developers the tools to accurately transfer 3D geometry between applications.

The openNURBS Toolkit consists of C++ source code for a library that will read and write openNURBS 3D model files (_.3dm_). More than 400 software development teams and applications, including [_Rhinoceros®_](https://rhino3d.com), exchange 3D models using the openNURBS file format. Additionally, the Toolkit provides NURBS evaluation tools and elementary geometric and 3D view manipulation tools as well as including source code for several example programs.

For more information, see ["What is openNURBS?"](https://developer.rhino3d.com/guides/opennurbs/what-is-opennurbs).

## Getting started

1. Clone this repository.
2. Open opennurbs_public.sln, select the platform and configuration, and rebuild all.
3. Create your C++ project that will use opennurbs.
4. In your project's stdafx.h, put the following lines:

    ```cpp
    // defining OPENNURBS_PUBLIC_INSTALL_DIR enables automatic linking using pragmas
    #define OPENNURBS_PUBLIC_INSTALL_DIR "<MY_INSTALLPATH>"
    // uncomment the next line if you want to use opennurbs as a DLL
    //#define OPENNURBS_IMPORTS
    #include "<MY_INSTALLPATH>/opennurbs_public.h"
    ```
    Replace `<MY_INSTALLPATH>` with the full path where you installed opennurbs using forward slashes as directory separators.

Please see ["Getting started"](https://developer.rhino3d.com/guides/opennurbs/getting-started/) for more information about openNURBS including supported compilers, build instructions, and a description of the examples.

There's also a collection of [example 3dm files](example_files/) available for testing.

## Building Using CMake:

1. Clone the repository 
2. `cd` to the root directory of the repository.
3. Run the following to configure the CMake files.
```
cmake -S ./ -B ./build -DCMAKE_FIND_FRAMEWORK=LAST
```
   
   Note: if [ninja-build](https://ninja-build.org/) is installed, you can specify `Ninja` to speed up the build:
   ```
    cmake -S ./ -B ./build -G "Ninja Multi-Config" -DCMAKE_FIND_FRAMEWORK=LAST
   ```
   
   Note: To use Ninja with the Visual Studio Compiler, open the MSVC command prompt (or run `vcvarsall.bat`), and run:
   ```
    cmake -S ./ -B ./build -G "Ninja Multi-Config" -D CMAKE_CXX_COMPILER=cl -D CMAKE_C_COMPILER=cl
   ```

   Note: `-DCMAKE_FIND_FRAMEWORK=LAST` fixes the issue of the MacOS compilers that choose an incorrect framework to build system headers.

   Note: Additionally, pass `-A ARM64` to build for ARM64. 

4. Finally, run the following to build the library.
```
cmake --build ./build --config Release
```

### Build as a shared library with CMake 

To build as a DLL with Visual Studio, you should add `-DOPENNURBS_SHARED=ON` to the command of step `3`:
```
cmake -S ./ -B ./build -DOPENNURBS_SHARED=ON
```

or with Ninja
```
cmake -S ./ -B ./build -G "Ninja Multi-Config" -D CMAKE_CXX_COMPILER=cl -D CMAKE_C_COMPILER=cl -DOPENNURBS_SHARED=ON
```

Build as usual as we described in step 4.

When using the dll, define `OPENNURBS_IMPORTS` before including `opennurbs_public.h`
```cmake
target_compile_definitions(main PUBLIC OPENNURBS_IMPORTS)
```


## Questions?

For technical support, please head over to [Discourse](https://discourse.mcneel.com/category/opennurbs).
