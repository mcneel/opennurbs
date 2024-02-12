#pragma once
#if !defined(OPENNURBS_TESTS_INC_)

#if !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
// Visual Studio 2005 issues a C4996 warning for lots of
// standard C runtime functions that take string pointers.
// The _CRT_SECURE_NO_DEPRECATE suppresses these warnings.
#endif

#pragma warning( push )
#include "../opennurbs.h"
#pragma warning( pop )

#pragma warning( push )
//#include <signal.h> 
//#include <time.h>
#pragma warning( pop )

#pragma warning( push )
#include "../../tl/tl.h"
#include "../../tl/tl_array.h"
#include "../../tl/tl_gslib.h"
#include "../../tl/tl_init.h"
#include "../../tl/tl_brep.h"
#include "../../tl/tl_nurbscurve.h"
#include "../../tl/tl_nurbssurface.h"
#include "../../tl/tl_tree.h"
#include "../../tl/tl_optimize.h"

#pragma warning( pop )


#define TL_GSLIB_INTERFACE_CODE
#pragma warning( push )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4313 )
#include "../../tl/tl_gslib_inc.h"
#pragma warning( pop )
#undef TL_GSLIB_INTERFACE_CODE


#if defined(_DEBUG)

#pragma comment(lib, "../../tl/DebugStaticLib/tl_staticlibd.lib")
#pragma comment(lib, "../../gslib/DebugStaticLib/gslib_staticlibd.lib")
#pragma comment(lib, "../DebugStaticLib/opennurbs_staticd.lib")
#pragma comment(lib, "../zlib/Debug/zlib_d.lib")
#pragma comment(lib, "msvcrtd.lib")

#pragma comment(linker, "/disallowlib:msvcrt.lib")

#else

#pragma comment(lib, "../../tl/ReleaseStaticLib/tl_staticlib.lib")
#pragma comment(lib, "../../gslib/ReleaseStaticLib/gslib_staticlib.lib")
#pragma comment(lib, "../ReleaseStaticLib/opennurbs_static.lib")
#pragma comment(lib, "../zlib/Release/zlib.lib")
#pragma comment(lib, "msvcrt.lib")

#pragma comment(linker, "/disallowlib:msvcrtd.lib")

#endif

#pragma comment(linker, "/disallowlib:libcd.lib")
#pragma comment(linker, "/disallowlib:libcmtd.lib")
#pragma comment(linker, "/disallowlib:libc.lib")
#pragma comment(linker, "/disallowlib:libcmt.lib")

#pragma comment(linker, "/disallowlib:mfco42ud.lib")
#pragma comment(linker, "/disallowlib:mfcd42ud.lib")
#pragma comment(linker, "/disallowlib:mfcn42ud.lib")
#pragma comment(linker, "/disallowlib:mfc42.lib")
#pragma comment(linker, "/disallowlib:mfcs42.lib")
#pragma comment(linker, "/disallowlib:mfc42ud.lib")
#pragma comment(linker, "/disallowlib:mfcs42ud.lib")
#pragma comment(linker, "/disallowlib:mfc42u.lib")
#pragma comment(linker, "/disallowlib:mfcs42u.lib")
#pragma comment(linker, "/disallowlib:mfco42d.lib")
#pragma comment(linker, "/disallowlib:mfcd42d.lib")
#pragma comment(linker, "/disallowlib:mfcn42d.lib")
#pragma comment(linker, "/disallowlib:mfc42d.lib")
#pragma comment(linker, "/disallowlib:mfcs42d.lib")
#pragma comment(linker, "/disallowlib:mfc42ud.lib")
#pragma comment(linker, "/disallowlib:mfcs42ud.lib")
#pragma comment(linker, "/disallowlib:mfc42u.lib")
#pragma comment(linker, "/disallowlib:mfcs42u.lib")
#pragma comment(linker, "/disallowlib:mfco42d.lib")
#pragma comment(linker, "/disallowlib:mfcd42d.lib")
#pragma comment(linker, "/disallowlib:mfcn42d.lib")
#pragma comment(linker, "/disallowlib:mfc42d.lib")
#pragma comment(linker, "/disallowlib:mfcs42d.lib")
#pragma comment(linker, "/disallowlib:mfc42.lib")
#pragma comment(linker, "/disallowlib:mfcs42.lib")
#pragma comment(linker, "/disallowlib:mfc42u.lib")
#pragma comment(linker, "/disallowlib:mfcs42u.lib")
#pragma comment(linker, "/disallowlib:mfco42ud.lib")
#pragma comment(linker, "/disallowlib:mfcd42ud.lib")
#pragma comment(linker, "/disallowlib:mfcn42ud.lib")
#pragma comment(linker, "/disallowlib:mfc42d.lib")
#pragma comment(linker, "/disallowlib:mfcs42d.lib")
#pragma comment(linker, "/disallowlib:mfc42.lib")
#pragma comment(linker, "/disallowlib:mfcs42.lib")
#pragma comment(linker, "/disallowlib:mfc42ud.lib")
#pragma comment(linker, "/disallowlib:mfcs42ud.lib")

#pragma comment(linker, "/disallowlib:nafxcwd.lib")
#pragma comment(linker, "/disallowlib:nafxcw.lib")
#pragma comment(linker, "/disallowlib:uafxcwd.lib")
#pragma comment(linker, "/disallowlib:uafxcw.lib")

// Prohibit any future inclusion of CRTDBG.H by defining _INC_CRTDBG.
#if defined(_INC_CRTDBG)
#error Never include CRTDBG.H in these tests.  It breaks purify.
#endif
#define _INC_CRTDBG


class CTestObject
{
public:
  CTestObject();
  ~CTestObject();

  void Destroy();

  int m_model_object_index;  // ONX_Model m_object_table[] index
  ON_UUID m_uuid;            // 3dm parent object uuid
  ON_wString m_name;         // 3dm parent object name
  ON_COMPONENT_INDEX m_ci;   // index of object in parent
};

class CTestCurve : public CTestObject
{
public:
  CTestCurve();
  ~CTestCurve();

  void Destroy();

  const ON_Curve* m_curve;

  // True if curve and NURBS parameterizations differ
  bool m_bAdjustNurbsParameter;

  const TL_NurbsCurve& NurbsCurve();

  const class IwBSplineCurve* GooseCurve();


  TL_NurbsCurve m_nurbs_curve;
  TL_NURB m_nurbcrv;

  class IwBSplineCurve* m_goose_curve;
};

class CTestSurface : public CTestObject
{
public:
  CTestSurface();
  ~CTestSurface();

  void Destroy();

  const ON_Surface* m_surface;

  // True if surface and NURBS parameterizations differ
  bool m_bAdjustNurbsParameter;

  const TL_NurbsSurface& NurbsSurface();

  const class IwBSplineSurface* GooseSurface();


  // call NurbsSurface() to get this surface
  TL_NurbsSurface m_nurbs_surface;
  TL_NURBSRF m_nurbsrf;

  // call GooseSurface() to get this surface
  class IwBSplineSurface* m_goose_surface;
};

class CTestBrep : public CTestObject
{
public:
  CTestBrep();
  ~CTestBrep();

  const TL_Brep* m_brep;
};

class CTestModel : public ONX_Model
{
public:
  CTestModel();

  // Digs through the model and harvests curves, surfaces, 
  // and breps for testing and puts them in the m_curves[], 
  // m_surfaces[], m_breps[] arrays.
  void GetTestObjects( bool bEdgesAreCurve, bool bTrimsAreCurves );

  ON_ClassArray<CTestCurve>   m_curves;
  ON_ClassArray<CTestSurface> m_surfaces;
  ON_ClassArray<CTestBrep>    m_breps;

  bool m_bPurify; // true if tests should be limited because purify is running.
};


double TEST_ElapsedTime();
//#define ON_TEST_DECL(func) ON_TEST_BeginTest(test_results,#func); test_results->ok = func(*test_results->m_text_log); ON_TEST_EndTest(test_results);

#define TEST_HEADER(text_log,function_name) TestPrintFunctionHeader( text_log, function_name, __FILE__, __DATE__ " " __TIME__ );

void TestPrintHeader( 
        ON_TextLog& text_log, 
        int argc, 
        const char* argv[]
        );

void TestPrintFunctionHeader( 
        ON_TextLog& text_log, 
        const char* function_name, 
        const char* file_name,
        const char* compile_time 
        );

void TestTree( const ONX_Model& model, ON_TextLog& text_log );
void TestClosestPoint( const ONX_Model& model, 
                       ON_TextLog& text_log,
                       bool bDoCurves, 
                       bool bDoSurfaces, 
                       bool bDoMeshes );
void TestCurveCurveIntersection(const ONX_Model& model, ON_TextLog& text_log );
void TestCurveSurfaceIntersection( CTestModel& model, ON_TextLog& text_log );
void TestZLibCompression( 
            const wchar_t* filespec, 
            ON_TextLog& text_log 
            );


// quick-n-dirty ccx for testing curve tree
void DaleCCX( const ON_Curve& curveA,
         const ON_Curve& curveB,
         double tol3d,
         ON_SimpleArray<TL_CX_EVENT>& ccx
         );


// new FAST! ccx 
void GregCCX( const ON_Curve& curveA,
         const ON_Curve& curveB,
         double tol3d,
         ON_SimpleArray<TL_CX_EVENT>& ccx
         );

#endif
