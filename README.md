# openNURBS

[![Discourse users](https://img.shields.io/discourse/https/discourse.mcneel.com/users.svg)](https://discourse.mcneel.com/c/opennurbs)

The openNURBS Initiative provides CAD, CAM, CAE, and computer graphics software developers the tools to accurately transfer 3-D geometry between applications.

The openNURBS Toolkit consists of C++ source code for a library that will read and write openNURBS 3D model files (_.3dm_). More than 400 software development teams and applications, including _Rhinoceros®_, exchange 3D models using the openNURBS (_.3dm_) file format.

The openNURBS Toolkit reads and writes all Rhino 3DM files. Additionally, the openNURBS Toolkit provides NURBS evaluation tools and elementary geometric and 3D view manipulation tools.

Unlike other open development initiatives, alliances, or consortia:

* Commercial use is encouraged.
* The tools, support, and membership are free.
* There are no restrictions. Neither copyright nor copyleft restrictions apply.
* No contribution of effort or technology is required from the members, although it is encouraged.

The openNURBS Toolkit is intended for C++ and .NET programmers. The toolkit includes complete source code to create a library that will read and write 3DM files. The toolkit also includes source code for several example programs.

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

  For example, if opennurbs_public is installed in `C:\dev\faraway\opennurbs_public` and you want to statically link, then the following two lines would be in your stdafx.h file.

  ```cpp
  #define OPENNURBS_PUBLIC_INSTALL_DIR "C:/dev/faraway/opennurbs_public"
  #include "C:/dev/faraway/opennurbs_public/opennurbs_public.h"
```

Please see ["Getting started"](http://developer.rhino3d.com/guides/opennurbs/getting-started/) for more information about openNURBS including supported compilers, build instructions, and a description of the examples.

## Questions?

For technical support, please head over to [Discourse](http://discourse.mcneel.com/category/opennurbs).
