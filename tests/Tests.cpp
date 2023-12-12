/* $Header: /src4/opennurbs/Tests/Tests.cpp 26    2/24/06 9:46p Dalelear $ */
/* $NoKeywords: $ */

#include "Tests.h"


#pragma warning( push )
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
#pragma warning( pop )

#define ON_CRASH_TEST_STATICS
#include "../opennurbs_crashtest.h"
#undef ON_CRASH_TEST_STATICS

static int starthere()
{
  ON::Begin();
  return 0;
}

// 13 Nov 2007 Dale Lear - added this comment.
// the init_seg pragma and initializing this global 
// force ON::Begin() to be called before main().
// I can't remember why this was important, but I
// think it had to do with purify.
int starthere__starthere = starthere();

class TEST_RESULTS
{
public:
  clock_t clock0;
  int error_count0;
  int warning_count0;
  int matherr_count0;
  int fperr_count0;

  ON_TextLog* m_text_log;

  int passed_count;
  int failed_count;

  bool ok;
};


CTestObject::CTestObject()
{
  m_model_object_index = 0;
  memset(&m_uuid,0,sizeof(m_uuid));
  memset(&m_ci,0,sizeof(m_ci));
}

CTestObject::~CTestObject()
{
  Destroy();
}

void CTestObject::Destroy()
{
  m_model_object_index = 0;
  memset(&m_uuid,0,sizeof(m_uuid));
  m_name.Destroy();
  memset(&m_ci,0,sizeof(m_ci));
}


CTestCurve::CTestCurve()
{
  m_curve = 0;
  memset(&m_nurbcrv,0,sizeof(m_nurbcrv));
  m_goose_curve = 0;
}

CTestCurve::~CTestCurve()
{
  Destroy();
}

void CTestCurve::Destroy()
{
  if ( m_goose_curve )
  {
    delete m_goose_curve;
    m_goose_curve = 0;
  }

  if ( m_nurbcrv.cv == m_nurbs_curve.m_cv )
  {
    m_nurbcrv.cv = 0;
  }
  if ( m_nurbcrv.knot == m_nurbs_curve.m_knot )
  {
    m_nurbcrv.knot = 0;
  }
  TL_DestroyNurb(&m_nurbcrv);

  m_nurbs_curve.Destroy();

  m_curve = 0;

  CTestObject::Destroy();
}


const TL_NurbsCurve& CTestCurve::NurbsCurve()
{
  if ( !m_nurbs_curve.m_cv && m_curve )
  {
    m_bAdjustNurbsParameter = ( m_curve->GetNurbForm(m_nurbs_curve) > 1 );
    m_nurbcrv.dim      = m_nurbs_curve.m_dim;
    m_nurbcrv.is_rat   = m_nurbs_curve.m_is_rat;
    m_nurbcrv.order    = m_nurbs_curve.m_order;
    m_nurbcrv.cv_count = m_nurbs_curve.m_cv_count;
    m_nurbcrv.cv       = m_nurbs_curve.m_cv;
    m_nurbcrv.knot     = m_nurbs_curve.m_knot;
  }
  return m_nurbs_curve;
}

const IwBSplineCurve* CTestCurve::GooseCurve()
{
  if ( !m_goose_curve && m_curve )
  {
    NurbsCurve();
    if ( m_nurbcrv.cv && m_nurbcrv.knot )
    {
      TL_Convert( m_nurbcrv, m_goose_curve );
    }
  }
  return m_goose_curve;
}



CTestSurface::CTestSurface()
{
  memset(&m_uuid,0,sizeof(m_uuid));
  m_surface = 0;
  memset(&m_nurbsrf,0,sizeof(m_nurbsrf));
  m_goose_surface = 0;
}

CTestSurface::~CTestSurface()
{
  Destroy();
}

void CTestSurface::Destroy()
{
  if ( m_goose_surface )
  {
    delete m_goose_surface;
    m_goose_surface = 0;
  }

  if ( m_nurbsrf.cv == m_nurbs_surface.m_cv )
  {
    m_nurbsrf.cv = 0;
  }
  if ( m_nurbsrf.knot[0] == m_nurbs_surface.m_knot[0] )
  {
    m_nurbsrf.knot[0] = 0;
  }
  if ( m_nurbsrf.knot[1] == m_nurbs_surface.m_knot[1] )
  {
    m_nurbsrf.knot[1] = 0;
  }
  TL_DestroyNurbSrf(&m_nurbsrf);

  m_nurbs_surface.Destroy();

  m_surface = 0;

  CTestObject::Destroy();
}

const TL_NurbsSurface& CTestSurface::NurbsSurface()
{
  if ( !m_nurbs_surface.m_cv && m_surface )
  {
    m_bAdjustNurbsParameter = ( m_surface->GetNurbForm(m_nurbs_surface) > 1 );
    m_nurbsrf.dim         = m_nurbs_surface.m_dim;
    m_nurbsrf.is_rat      = m_nurbs_surface.m_is_rat;
    m_nurbsrf.order[0]    = m_nurbs_surface.m_order[0];
    m_nurbsrf.order[1]    = m_nurbs_surface.m_order[1];
    m_nurbsrf.cv_count[0] = m_nurbs_surface.m_cv_count[0];
    m_nurbsrf.cv_count[1] = m_nurbs_surface.m_cv_count[1];
    m_nurbsrf.cv          = m_nurbs_surface.m_cv;
    m_nurbsrf.knot[0]     = m_nurbs_surface.m_knot[0];
    m_nurbsrf.knot[1]     = m_nurbs_surface.m_knot[1];
  }
  return m_nurbs_surface;
}

const IwBSplineSurface* CTestSurface::GooseSurface()
{
  if ( !m_goose_surface && m_surface )
  {
    NurbsSurface();
    if ( m_nurbsrf.cv && m_nurbsrf.knot[0] && m_nurbsrf.knot[1] )
    {
      TL_Convert( m_nurbsrf, m_goose_surface );
    }
  }
  return m_goose_surface;
}


CTestBrep::CTestBrep()
{
  m_brep = 0;
}

CTestBrep::~CTestBrep()
{
  m_brep = 0;
}

static int sort_model_helper(const ONX_Model_Object* a, const ONX_Model_Object* b )
{
  // sort by uuid, then object name, then order in the file
  int i = ON_UuidCompare(a->m_attributes.m_uuid,b->m_attributes.m_uuid);
  if ( 0 == i )
  {
    i = a->m_attributes.m_name.Compare( b->m_attributes.m_name );
    if ( 0 == i )
    {
      // compare order in the model
      i = ((a<b) ? -1 : ((a>b) ? 1 : 0));
    }
  }
  return i;
}

CTestModel::CTestModel()
{
  m_bPurify = false;
}

void CTestModel::GetTestObjects( bool bEdgesAreCurves, bool bTrimsAreCurves )
{
  m_curves.Destroy();
  m_surfaces.Destroy();
  m_breps.Destroy();
  m_curves.Reserve(32);
  m_surfaces.Reserve(64);
  m_curves.Reserve(64);

  const ON_Curve* curve;
  const ON_Surface* surface;
  const TL_Brep* brep;

  int i, j;

  // Sorting the model object table keeps 
  // the report's object order consistent
  m_object_table.HeapSort(sort_model_helper);


  for (i = 0; i < m_object_table.Count(); i++ )
  {
    ONX_Model_Object& model_object = m_object_table[i];

    curve = ON_Curve::Cast(model_object.m_object);
    if ( curve )
    {
      CTestCurve& C = m_curves.AppendNew();

      C.m_uuid = model_object.m_attributes.m_uuid;
      C.m_name = model_object.m_attributes.m_name;
      if (C.m_name.IsEmpty())
        C.m_name = L"anonymous curve";
      C.m_model_object_index = i;

      C.m_curve = curve;

      continue;
    }

    surface = ON_Surface::Cast(model_object.m_object);
    if ( surface )
    {
      CTestSurface& S = m_surfaces.AppendNew();

      S.m_uuid = model_object.m_attributes.m_uuid;
      S.m_name = model_object.m_attributes.m_name;
      if (S.m_name.IsEmpty())
        S.m_name = L"anonymous surface";
      S.m_model_object_index = i;

      S.m_surface = surface;

      continue;
    }

    brep = TL_Brep::Promote(ON_Brep::Cast(model_object.m_object));
    if ( brep )
    {
      CTestBrep& B = m_breps.AppendNew();

      B.m_uuid = model_object.m_attributes.m_uuid;
      B.m_name = model_object.m_attributes.m_name;
      if (B.m_name.IsEmpty())
        B.m_name = L"anonymous brep";
      B.m_model_object_index = i;

      B.m_brep = brep;

      if ( bTrimsAreCurves )
      {
        for ( j = 0; j < brep->m_C2.Count(); j++ )
        {
          curve = brep->m_C2[j];
          if ( !curve )
            continue;

          CTestCurve& C = m_curves.AppendNew();

          C.m_uuid = model_object.m_attributes.m_uuid;
          C.m_name.Format(L"brep[%d]->m_C2[%d] in %s",m_breps.Count()-1,j,B.m_name.Array());
          C.m_model_object_index = i;
          C.m_ci.m_type = ON_COMPONENT_INDEX::brep_trim;
          C.m_ci.m_index = j;

          C.m_curve = curve;
        }
      }

      if ( bEdgesAreCurves )
      {
        for ( j = 0; j < brep->m_C3.Count(); j++ )
        {
          curve = brep->m_C3[j];
          if ( !curve )
            continue;

          CTestCurve& C = m_curves.AppendNew();

          C.m_uuid = model_object.m_attributes.m_uuid;
          C.m_name.Format(L"brep[%d]->m_C3[%d] in %s",m_breps.Count()-1,j,B.m_name.Array());
          C.m_model_object_index = i;
          C.m_ci.m_type = ON_COMPONENT_INDEX::brep_edge;
          C.m_ci.m_index = j;

          C.m_curve = curve;
        }
      }

      for ( j = 0; j < brep->m_S.Count(); j++ )
      {
        surface = brep->m_S[j];
        if ( !surface )
          continue;

        CTestSurface& S = m_surfaces.AppendNew();

        S.m_uuid = model_object.m_attributes.m_uuid;
        S.m_name.Format(L"brep[%d]->m_S[%d] in %s\n",m_breps.Count()-1,j,B.m_name.Array());
        S.m_model_object_index = i;
        S.m_ci.m_index = j;

        S.m_surface = surface;
      }

      continue;
    }
  }
};

//static void ON_TEST_BeginTest( struct tagON_TEST_RESULTS& test_results, const char* func_name );

//static void ON_TEST_EndTest( struct tagON_TEST_RESULTS& test_results );

//static bool ON_TEST_InitFPErrorHandler(void);

class CTestList
{
public:

  CTestList()
  {
    m_bTestTree = true;
    m_bTestClsPtCrv = true;
    m_bTestClsPtSrf = true;
    m_bTestClsPtMesh = true;
    m_bTestCCX = true;
    m_bTestCSX = true;
    m_bTestSSX = true;
    m_bTestZLibCompression = false;
    m_bAudit = true;
    m_bDump = true;
    m_bEdgesAreCurves = true;
    m_bTrimsAreCurves = true;
    m_bPurify = false;
    m_bCrashTestDblZeroDiv = false;
    m_bCrashTestDblOverflow = false;
    m_bCrashTestDblInvalidOp = false;

    m__bClearTests = true;

    m_default_3dm_filename = L"\\src4\\opennurbs\\tests\\input\\MyTest.3dm";
    m_default_log_filename = L"\\src4\\opennurbs\\tests\\results\\MyResults.txt";
    m_3dm_filename = m_default_3dm_filename;
  }

  bool ParseCommandLine( int argc, const char* argv[] ); 

  bool m_bTestTree;
  bool m_bTestClsPtCrv;
  bool m_bTestClsPtSrf;
  bool m_bTestClsPtMesh;
  bool m_bTestCCX;
  bool m_bTestCSX;
  bool m_bTestSSX;
  bool m_bTestZLibCompression;
  bool m_bAudit;
  bool m_bDump;
  bool m_bPurify;
  bool m_bCrashTestDblZeroDiv;
  bool m_bCrashTestDblOverflow;
  bool m_bCrashTestDblInvalidOp;

  bool m_bEdgesAreCurves;
  bool m_bTrimsAreCurves;

  ON_wString m_3dm_filename;
  ON_wString m_log_filename;
  ON_wString m_zlib_filespec;

private:
  ON_String m_default_3dm_filename;
  ON_String m_default_log_filename;
  bool m__bClearTests;
  bool GetOption( const char* arg, 
                  const char* opt, 
                  ON_wString& value );
  void Clear();
};

void CTestList::Clear()
{
  if ( m__bClearTests )
  {
    // do not clear m_bEdgesAreCurves or m_bTrimsAreCurves
    m_bTestTree = false;
    m_bTestClsPtCrv = false;
    m_bTestClsPtSrf = false;
    m_bTestClsPtMesh = false;
    m_bTestCCX = false;
    m_bTestCSX = false;
    m_bTestSSX = false;
    m_bTestZLibCompression = false;
    m_bAudit = false;
    m_bDump = false;
    m_bCrashTestDblZeroDiv = false;
    m_bCrashTestDblOverflow = false;
    m_bCrashTestDblInvalidOp = false;

    m__bClearTests = false;
  }
}

bool CTestList::GetOption( const char* arg, 
                       const char* opt, 
                       ON_wString& value )
{
  int i;
  bool rc = false;

  value.Destroy();

  if (    arg 
       && ('-' == arg[0] || '/' == arg[0])
       && arg[1]
       && opt 
       && opt[0]
       && '-' != opt[0]
       && '/' != opt[0]
       && ':' != opt[0]
       )
  {
    arg++;
    for ( i = 0; arg[i]; i++ )
    {
      if ( ':' == arg[i] )
        break;
    }

    if ( strlen(opt) == i && 0 == _strnicmp(arg,opt,i) )
    {
      rc = true;
      if ( ':' == arg[i] )
      {
        value = &arg[i+1];
      }
    }      
  }
  
  return rc;
}

bool CTestList::ParseCommandLine( int argc, const char* argv[] )
{
  int argi;
  for ( argi = 1; argi < argc; argi++ )
  {
    ON_wString value;

    if ( GetOption(argv[argi],"log",value) )
    {
      if ( value.IsEmpty() )
      {
        m_log_filename = m_default_log_filename;
      }
      else
      {
        m_log_filename = value;
      }
      continue;
    }

    if ( GetOption(argv[argi],"3dm",value) )
    {
      if ( value.IsEmpty() )
      {
        m_3dm_filename = m_default_3dm_filename;
      }
      else
      {    
        m_3dm_filename = value;
      }
      continue;
    }

    if ( GetOption(argv[argi],"audit",value) )
    {
      Clear();
      m_bAudit = true;
      continue;
    }

    if ( GetOption(argv[argi],"dump",value) )
    {
      Clear();
      m_bDump = true;
      continue;
    }

    if ( GetOption(argv[argi],"tree",value) )
    {
      Clear();
      m_bTestTree = true;
      continue;
    }

    if ( GetOption(argv[argi],"crashtestzerodiv",value) )
    {
      Clear();
      m_bCrashTestDblZeroDiv = true;
      continue;
    }

    if ( GetOption(argv[argi],"crashtestoverflow",value) )
    {
      Clear();
      m_bCrashTestDblOverflow = true;
      continue;
    }

    if ( GetOption(argv[argi],"crashtestinvalidop",value) )
    {
      Clear();
      m_bCrashTestDblInvalidOp = true;
      continue;
    }

    if ( GetOption(argv[argi],"purify",value) )
    {
      Clear();
      m_bPurify = true;
      continue;
    }

    if ( GetOption(argv[argi],"clspt",value) )
    {
      Clear();
      m_bTestClsPtCrv = true;
      m_bTestClsPtSrf = true;
      m_bTestClsPtMesh = true;
      continue;
    }

    if ( GetOption(argv[argi],"clsptcrv",value) )
    {
      Clear();
      m_bTestClsPtCrv = true;
      continue;
    }

    if ( GetOption(argv[argi],"clsptsrf",value) )
    {
      Clear();
      m_bTestClsPtSrf = true;
      continue;
    }

    if ( GetOption(argv[argi],"clsptmesh",value) )
    {
      Clear();
      m_bTestClsPtMesh = true;
      continue;
    }

    if ( GetOption(argv[argi],"ccx",value) )
    {
      Clear();
      m_bTestCCX = true;
      continue;
    }


    if ( GetOption(argv[argi],"csx",value) )
    {
      Clear();
      m_bTestCSX = true;
      continue;
    }

    if ( GetOption(argv[argi],"ssx",value) )
    {
      Clear();
      m_bTestSSX = true;
      continue;
    }

    if ( GetOption(argv[argi],"zlib",value) )
    {
      Clear();
      m_bTestZLibCompression = true;
      m_zlib_filespec = value;
      continue;
    }

    if ( GetOption(argv[argi],"notrims",value) )
    {
      m_bTrimsAreCurves = false;
      continue;
    }

    if ( GetOption(argv[argi],"noedges",value) )
    {
      m_bEdgesAreCurves = false;
      continue;
    }

    {
      printf("%s command line options\n",argv[0]);
      printf("  -?     print this message\n");
      printf("  -help  print this message\n");
      printf("  -3dm:filename.3dm input 3dm model to test\n");
      printf("  -log:filename.txt output text file for results\n");
      printf("  -purify    limited tests for dog slow purify\n");
      printf("  -audit     audit 3dm model\n");
      printf("  -dump:     dump 3dm model\n");
      printf("  -notrims   do not include brep trims with curves\n");
      printf("  -noedge    do not include brep edges with curves\n");
      printf("  -tree      test trees\n");
      printf("  -clspt     test closest point to anything\n");
      printf("  -clsptcrv  test closest point to curve\n");
      printf("  -clsptsrf  test closest point to surface\n");
      printf("  -clsptmesh test closest point to mesh\n");
      printf("  -ccx       test curve-curve intersection\n");
      printf("  -csx       test curve and edge-surface intersection\n");
      printf("  -ssx       test surface-surface intersection\n");
      printf("  -crashtestzerodiv test 1.0/0.0 exception handling\n");
      printf("  -crashtestoverflow test 1.0e200*2.0e222 exception handling\n");
      printf("  -crashtestinvalidop test log(negative number) exception handling\n");
      printf("\n");
      printf("  If no 3dm filename is specified, then\n");
      printf("  %s is used.\n",m_default_3dm_filename.Array());
      printf("  If no filename is specified with the -log option, then\n");
      printf("  %s is used.\n",m_default_log_filename.Array());
      printf("  If no options are specified, then all tests are performed.\n");

      return false;
    }
  }

  return true;
}

class CAlphaOmega
{
public:
  CAlphaOmega();
  ~CAlphaOmega();
};

CAlphaOmega::CAlphaOmega()
{
  TL_Begin();

  // Break when error occurs
  ON_EnableDebugBreak(true);

  ON_FPU_Init();

  // Mask minor exceptions (underflow, partial loss of precision, denormal )
  // but crash on the bad exceptions (zero divide, overflow, invalid op)
  ON__UINT32 bad_ex = ON_FPU_EX_ZERODIVIDE 
                    | ON_FPU_EX_OVERFLOW 
                    | ON_FPU_EX_INVALID;
  ON_FPU_UnmaskExceptions(bad_ex);
}

CAlphaOmega::~CAlphaOmega()
{
  TL_End();
 
  // The ON::End() call cleans up runtime class 
  // informtion so that Purify doesn't complain about leaks.
  // OpenNURBS is completely useless after this call.
  ON::End();
}




class CCheckForErrors
{
public:
  CCheckForErrors(ON_TextLog& text_log,const wchar_t* testname) 
    : m_text_log(text_log),
      m_test_name(testname)
  {
    m_error_count1 = m_error_count0 = ON_GetErrorCount();
    m_warning_count1 = m_warning_count0 = ON_GetWarningCount();
  }

  ~CCheckForErrors()
  {
    m_error_count1 = ON_GetErrorCount()-m_error_count0;
    m_warning_count1 = ON_GetWarningCount()-m_warning_count0;

    if ( m_error_count1 > 0 || m_warning_count1 > 0 )
    {
      m_text_log.Print("\n");
      m_text_log.Print( L"ERRORS in %s tests: %d openurbs error(s) and %d opennurbs warning(s).\n",
                        m_test_name.Array(),
                        m_error_count1,
                        m_warning_count1);
    }
  }

  ON_TextLog& m_text_log;
  ON_wString m_test_name;   
  int m_error_count0;
  int m_error_count1;
  int m_warning_count0;
  int m_warning_count1;

private:
  CCheckForErrors();
};


int main( int argc, const char* argv[])
{
  // The alpha-omega constructor/destructor handles TL and ON
  // library initialization and cleanup.
  CAlphaOmega alphaomega;

  // test_list controls what tests are performed.
  CTestList test_list;
  
  // If no 3dm file is specified, you get to use whatever you have in
  // your MyTest.3dm file.  Do NOT check a MyTest.3dm into SourceSafe
  // or you mess up everybody else.  If the command line has a 
  // -3dm:filename option, then m_3dm_filename will be set to the
  // filename specified on the command line.

  if ( !test_list.ParseCommandLine(argc,argv) )
  {
    return -1;
  }

  /////////////////////////////////////////////////////////////////
  //
  // Set up text_log
  //
  FILE* log_fp = 0; // = fopen("C:\\TestResults.txt","w");
  if ( 0 == log_fp )
  {
    const wchar_t* log_filename = test_list.m_log_filename;
    if ( log_filename && log_filename[0] )
    {
      log_fp = _wfopen(log_filename,L"w");
      if ( !log_fp )
      {
        printf("Unable to open log file \"%S\".\n",log_filename);
        return -1;
      }
      else
      {
        printf("Beginning tests.\n  Results will be in \"%S\".\n",log_filename);
      }
    }
  }

  ON_TextLog text_log(log_fp);
  text_log.SetIndentSize(2);

  TEST_RESULTS test_results;
  memset(&test_results,0,sizeof(test_results));
  int repair_count = 0;


  if ( test_list.m_bCrashTestDblZeroDiv )
  {
    CrashTestHelper(6,text_log);
  }
  if ( test_list.m_bCrashTestDblInvalidOp )
  {
    CrashTestHelper(9,text_log);
  }
  if ( test_list.m_bCrashTestDblOverflow )
  {
    CrashTestHelper(10,text_log);
  }


  /////////////////////////////////////////////////////////////////
  //
  // zlib test's dont use a 3dm model
  //
  if (     test_list.m_bTestZLibCompression 
       //&& !test_list.m_zlib_filespec.IsEmpty() 
       )
  {
    TestZLibCompression( test_list.m_zlib_filespec, text_log );
  }
  else if ( !test_list.m_3dm_filename.IsEmpty() )
  {
    //ON_TEST_InitFPErrorHandler();
    
    test_results.m_text_log = &text_log;

    TestPrintHeader(text_log,argc,argv);

    CTestModel model;
    model.m_bPurify = test_list.m_bPurify;
    {
      CCheckForErrors check_for_errors(text_log,L"read 3dm file");
      if ( !model.Read(test_list.m_3dm_filename,&text_log) )
      {
        if ( log_fp )
          printf("Unable to read 3dm file \"%S\".\n",test_list.m_3dm_filename.Array());
      }

      // Sort model object table by uuid and then fill in the
      // CTestModel m_curves[], m_surfaces[], and m_breps[] arrays
      model.GetTestObjects( test_list.m_bEdgesAreCurves, test_list.m_bTrimsAreCurves );
    }

    if ( test_list.m_bDump )
    {
      CCheckForErrors check_for_errors(text_log,L"dump 3dm file");
      model.Dump(text_log);
    }
    else
    {
      text_log.Print(L"3dm file: %s (%d objects)\n",test_list.m_3dm_filename.Array(),model.m_object_table.Count());
    }

    if ( test_list.m_bAudit )
    {
      CCheckForErrors check_for_errors(text_log,L"audit 3dm file");
      model.Audit(true,&repair_count,&text_log, NULL);
    }

    // Sorting the model object table keeps 
    // the report's object order consistent
    model.m_object_table.HeapSort(sort_model_helper);

    if ( test_list.m_bTestTree )
    {
      if ( log_fp )
        printf("Testing curve and surface trees.\n");
      CCheckForErrors check_for_errors(text_log,L"curve and surface Tree");
      TestTree(model,text_log);
    }

    if ( test_list.m_bTestClsPtCrv )
    {
      if ( log_fp )
        printf("Testing closest point to curve.\n");
      CCheckForErrors check_for_errors(text_log,L"closest point to curve");
      TestClosestPoint(model,text_log, true, false, false );
    }

    if ( test_list.m_bTestClsPtSrf )
    {
      if ( log_fp )
        printf("Testing closest point to surface.\n");
      CCheckForErrors check_for_errors(text_log,L"closest point to surface");
      TestClosestPoint(model,text_log, false, true, false );
    }

    if ( test_list.m_bTestClsPtMesh )
    {
      if ( log_fp )
        printf("Testing closest point to mesh.\n");
      CCheckForErrors check_for_errors(text_log,L"closest point to mesh");
      TestClosestPoint(model,text_log, false, false, true );
    }

    if ( test_list.m_bTestCCX )
    {
      if ( log_fp )
        printf("Testing curve-curve intersection.\n");
      CCheckForErrors check_for_errors(text_log,L"curve-curve intersection");
      TestCurveCurveIntersection(model, text_log);
    }

    if ( test_list.m_bTestCSX )
    {
      if ( log_fp )
        printf("Testing curve-surface intersection.\n");
      CCheckForErrors check_for_errors(text_log,L"curve-surface intersection");
      TestCurveSurfaceIntersection(model,text_log);
    }

    if ( test_list.m_bTestSSX )
    {
      if ( log_fp )
        printf("Testing surface-surface intersection.\n");
      CCheckForErrors check_for_errors(text_log,L"surface-surface intersection");
      //  TestSurfaceSurfaceIntersection(text_log);
      text_log.Print("Surface-surface intersection tests not ready.\n");
    }
  }

  if (log_fp)
  {
    printf("Finished tests.  Results are in \"%S\".\n",test_list.m_log_filename.Array());
    fclose(log_fp);
    log_fp = 0;
  }

#if (_DEBUG)
  {
    // This getchar() is here so the results window doesnt' vanish
    // when the debugger is being used.
    printf("\nPress ENTER to finish.\n");
    getchar();
  }
#endif

  return test_results.failed_count+starthere__starthere;
}

void TestPrintHeader( ON_TextLog& text_log, int argc, const char* argv[] )
{
  int year = 0;
  int month = 1;
  int date = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;
  TLI_CurrentTime( &year, &month, &date, &hour, &minute, &second );
  if ( year < 1900 )
  {
    year += 1900;
    if ( year < 1970 )
      year += 1970;
  }
  char* month_name[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  text_log.Print("Test: %s\n",argv[0]);
  text_log.PushIndent();
  for ( int argi = 1; argi < argc; argi++ )
  {
    text_log.Print("%s\n",argv[argi]);
  }
  text_log.PopIndent();
  text_log.Print("Test date:         %s %2d %d %2d:%02d:%02d UCT\n",month_name[month-1],date,year,hour,minute,second);
  const char* compile_time = __DATE__ " " __TIME__;
  text_log.Print("Compile date:      %s local time\n",compile_time);
  text_log.Print("OpenNURBS version: %d\n",ON::Version());
#if defined(_DEBUG)
    text_log.Print("Build settings:    Debug\n");
#else
    text_log.Print("Build settings:    Release\n");
#endif

  text_log.Print("\n");
}


void TestPrintFunctionHeader( ON_TextLog& text_log, const char* function_name, const char* file_name, const char* compile_time )
{
  int year = 0;
  int month = 1;
  int date = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;
  TLI_CurrentTime( &year, &month, &date, &hour, &minute, &second );
  if ( year < 1900 )
  {
    year += 1900;
    if ( year < 1970 )
      year += 1970;
  }
  char* month_name[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  text_log.Print("Test function:     %s in %s\n",function_name,file_name);
  text_log.Print("Test date:         %s %2d %d %2d:%02d:%02d UCT\n",month_name[month-1],date,year,hour,minute,second);
  text_log.Print("Compile date:      %s local time\n",compile_time);

  text_log.Print("\n");
}

double TEST_ElapsedTime()
{
  static clock_t time0=0;
  double elapsed_time;
  clock_t time;
  time = clock();
  elapsed_time = ((double)(time - time0))/((double)CLOCKS_PER_SEC);
  time0 = time;
  return elapsed_time;
}


/****************************************************************************
 **
 ** _matherr()  ( replaces standard math libraries matherr() )
 ** Calls ON_TEST_MathError() whenever a math error occurs.
 **
 ****************************************************************************/

int _matherr( struct _exception *ExceptStruct )
{
  /* NOTE: This function ALWAYS returns a "1" so that the
   *       system's math library will not attempt to print
   *       some message on stderr/stdout.
   */
  switch(ExceptStruct->type) /* <- Good location for a debugger breakpoint */
  {
  case _DOMAIN: /* domain error */
    /* CRITICAL MATH ERROR - FIX THIS BUG */
    errno = EDOM;
    ON_MathError("rhino.exe","_matherr DOMAIN exception",ExceptStruct->name);
    break;

  case _SING: /* function singularity */
    /* CRITICAL MATH ERROR - FIX THIS BUG */
    errno = EDOM;
    ON_MathError("rhino.exe","_matherr SING exception",ExceptStruct->name);
    break;

  case _OVERFLOW:
    /* CRITICAL MATH ERROR - FIX THIS BUG */
    errno = ERANGE;
    ON_MathError("rhino.exe","_matherr OVERFLOW exception",ExceptStruct->name);
    break;

  case _UNDERFLOW:
    /* ignore underflow errors - they frequently happen */
    errno = ERANGE;
    break;

  case _TLOSS: /* total loss of significance (like sin(1.0e300)) */
    errno = ERANGE;
    ON_MathError("rhino.exe","_matherr TLOSS exception",ExceptStruct->name);
    break;

  case _PLOSS: /* partial loss of significance */
    /* do not modify ExceptStruct->retval and hope for the best */
    errno = ERANGE;
    ON_MathError("rhino.exe","_matherr PLOSS exception",ExceptStruct->name);
    break;

  default: /* undocumented error */
    errno = EDOM;
    ON_MathError("rhino.exe","_matherr UNDOCUMENTED ERROR",ExceptStruct->name);
    break;
  }
  return 1; /* suppress operating system's error handling */
}
