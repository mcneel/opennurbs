/* $Header: /src4/opennurbs/Tests/TestCurveSurfaceIntersection.cpp 21    6/16/05 3:40p Dalelear $ */
/* $NoKeywords: $ */

#include "Tests.h"

static bool bPurify = false;

static double TestOverlapDistance( 
                  ON_Interval overlap_domain,
                  const ON_Curve* curve, 
                  const ON_Surface* surface,
                  int sample_count
                  )
{
  int n, j;
  double w, s, t, d;
  ON_2dPoint uv;
  ON_3dPoint C, S;
  w = 1.0;
  if (sample_count < 1 )
    sample_count = 1;

  double maxd = 0.0;

  for ( n = 2; sample_count > 0; n *= 2 )
  {
    w *= 0.5;
    for ( j = 1; j < n; j += 2 )
    {
      s = j;
      s *= w;
      t = overlap_domain.ParameterAt(s);
      C = curve->PointAt(t);
      uv.Set(ON_UNSET_VALUE,ON_UNSET_VALUE);
      surface->GetClosestPoint(C,&uv.x,&uv.y);
      S = surface->PointAt(uv.x,uv.y);
      d = C.DistanceTo(S);
      if ( d > maxd )
      {
        maxd = d;
      }
      sample_count--;
    }
  }

  return maxd;
}

static
int GetGooseCurveSurfaceIntersection(
     const ON_Curve* curve,
     const ON_Surface* surface,
     const IwBSplineCurve* goose_curve,
     const IwBSplineSurface* goose_surface,
     double intersection_tolerance,
     double overlap_tolerance,
     double test_tolerance,
     ON_SimpleArray<ON_X_EVENT>& goose_x
     )
{
  if ( !goose_curve || !goose_surface )
    return 1;

  ON_3dPoint Cpt[2], Spt[2];
  int bogus_events_from_old_csx = 0;
  IwSolutionArray G( 100 );
  goose_surface->GlobalCurveIntersect(
                    goose_surface->GetNaturalUVDomain(),
                    *goose_curve,
                    goose_curve->GetNaturalInterval(),
                    intersection_tolerance,
                    G 
                    );

  unsigned int gi;
  int i;
  double d;
  for ( gi = 0; gi < G.GetSize(); gi++ )
  {
    int closest_i = -1;
    double closest_d = 1.0e300;

    IwSolution& g = G[gi];

    ON_X_EVENT goose_event;
    IwSolutionType iw_xtype = g.m_eSolutionType;
    IwPoint3d iwpt;
    if ( iw_xtype == IW_ST_RANGE_OF_VALUES && g.m_vStart[0] >= g.m_vEnd[0] ) 
    {
      iw_xtype = IW_ST_SINGLE_VALUE;
      g.m_vEnd[0] = g.m_vStart[0];
      g.m_vEnd[1] = g.m_vStart[1];
      g.m_vEnd[2] = g.m_vStart[2];
    }

    switch ( iw_xtype ) 
    {
    case IW_ST_SINGLE_VALUE:
      // isolated intersection point
      goose_event.m_type = ON_X_EVENT::csx_point;

      goose_event.m_a[0] = goose_event.m_a[1] = g.m_vStart[0];
      goose_event.m_b[0] = goose_event.m_b[2] = g.m_vStart[1];
      goose_event.m_b[1] = goose_event.m_b[3] = g.m_vStart[2];
      
      goose_curve->EvaluatePoint(goose_event.m_a[0],iwpt);
      goose_event.m_A[0].Set(iwpt.x,iwpt.y,iwpt.z);
      goose_event.m_A[1] = goose_event.m_A[0];
      
      goose_surface->EvaluatePoint(IwPoint2d(goose_event.m_b[0],goose_event.m_b[1]),iwpt);
      goose_event.m_B[0].Set(iwpt.x,iwpt.y,iwpt.z);
      goose_event.m_B[1] = goose_event.m_B[0];
      break;

    case IW_ST_RANGE_OF_VALUES:
      // overlap region
      goose_event.m_type = ON_X_EVENT::csx_overlap;

      goose_event.m_a[0] = g.m_vStart[0];
      goose_event.m_a[1] = g.m_vEnd[0];
      goose_event.m_b[0] = g.m_vStart[1];
      goose_event.m_b[1] = g.m_vStart[2];
      goose_event.m_b[2] = g.m_vEnd[1];
      goose_event.m_b[3] = g.m_vEnd[2];
      
      goose_curve->EvaluatePoint(goose_event.m_a[0],iwpt);
      goose_event.m_A[0].Set(iwpt.x,iwpt.y,iwpt.z);
      goose_curve->EvaluatePoint(goose_event.m_a[1],iwpt);
      goose_event.m_A[1].Set(iwpt.x,iwpt.y,iwpt.z);
      
      goose_surface->EvaluatePoint(IwPoint2d(goose_event.m_b[0],goose_event.m_b[1]),iwpt);
      goose_event.m_B[0].Set(iwpt.x,iwpt.y,iwpt.z);
      goose_surface->EvaluatePoint(IwPoint2d(goose_event.m_b[2],goose_event.m_b[3]),iwpt);
      goose_event.m_B[1].Set(iwpt.x,iwpt.y,iwpt.z);
      break;
    }
    for ( i = 0; i < 2; i++ )
    {
      if ( i && ON_X_EVENT::csx_overlap != goose_event.m_type )
      {
        goose_event.m_a[1] = goose_event.m_a[0];
        goose_event.m_b[2] = goose_event.m_b[0];
        goose_event.m_b[3] = goose_event.m_b[1];
        goose_event.m_nodeA_t[1] = goose_event.m_nodeA_t[0];
        goose_event.m_nodeB_t[2] = goose_event.m_nodeB_t[0];
        goose_event.m_nodeB_t[3] = goose_event.m_nodeB_t[1];
      }
      else
      {
        goose_event.m_nodeA_t[i] = goose_event.m_a[i];
        curve->GetCurveParameterFromNurbFormParameter(goose_event.m_a[i],&goose_event.m_a[i]);
        goose_event.m_nodeB_t[2*i] = goose_event.m_b[2*i];
        goose_event.m_nodeB_t[2*i+1] = goose_event.m_b[2*i+1];
        surface->GetSurfaceParameterFromNurbFormParameter(goose_event.m_b[2*i],goose_event.m_b[2*i+1],&goose_event.m_b[2*i],&goose_event.m_b[2*i+1]);
        if ( i && goose_event.m_a[1] <= goose_event.m_a[0] )
        {
          // micro overlap??
          bogus_events_from_old_csx++;
          ON_X_EVENT::CopyEventPart(goose_event,0,goose_event,1);
          goose_event.m_type = ON_X_EVENT::csx_point;
        }
      }
    }

    d = goose_event.m_A[0].DistanceTo(goose_event.m_B[0]);
    if ( d > test_tolerance )
    {
      bogus_events_from_old_csx++;
      ON_X_EVENT::CopyEventPart(goose_event,1,goose_event,0);
      goose_event.m_type = ON_X_EVENT::csx_point;
    }

    d = goose_event.m_A[1].DistanceTo(goose_event.m_B[1]);
    if ( d > test_tolerance )
    {
      bogus_events_from_old_csx++;
      if ( goose_event.m_a[0] == goose_event.m_a[1] )
        continue;
      ON_X_EVENT::CopyEventPart(goose_event,0,goose_event,1);
      goose_event.m_type = ON_X_EVENT::csx_point;
    }

    // validate goose_event
    Cpt[0] = curve->PointAt(goose_event.m_a[0]);
    Spt[0] = surface->PointAt(goose_event.m_b[0],goose_event.m_b[1]);
    Cpt[1] = curve->PointAt(goose_event.m_a[1]);
    Spt[1] = surface->PointAt(goose_event.m_b[2],goose_event.m_b[3]);
    d = Cpt[0].DistanceTo(Spt[0]);
    if ( d > test_tolerance )
    {
      bogus_events_from_old_csx++;
      ON_X_EVENT::CopyEventPart(goose_event,1,goose_event,0);
      goose_event.m_type = ON_X_EVENT::csx_point;
      Cpt[0] = Cpt[1];
      Spt[0] = Spt[1];
    }

    d = Cpt[1].DistanceTo(Spt[1]);
    if ( d > test_tolerance )
    {
      bogus_events_from_old_csx++;
      if ( goose_event.m_a[0] == goose_event.m_a[1] )
        continue;
      ON_X_EVENT::CopyEventPart(goose_event,0,goose_event,1);
      goose_event.m_type = ON_X_EVENT::csx_point;
    }

    if ( ON_X_EVENT::csx_overlap == goose_event.m_type )
    {
      d = TestOverlapDistance(ON_Interval(goose_event.m_a[0],goose_event.m_a[1]),
                              curve,surface,120);
      if ( d > test_tolerance )
      {
        bogus_events_from_old_csx++;
        goose_event.m_type = ON_X_EVENT::csx_point;
        ON_X_EVENT& e0 = goose_x.AppendNew();
        e0 = goose_event;
        ON_X_EVENT::CopyEventPart( e0, 0, e0, 1 );
        e0.m_type = ON_X_EVENT::csx_point;
        ON_X_EVENT::CopyEventPart( goose_event, 1, goose_event, 0 );
        goose_event.m_type = ON_X_EVENT::csx_point;
      }
    }

    goose_x.Append(goose_event);
  }

  return bogus_events_from_old_csx;
}
                         
static int TestCSXCompareWithGoose( 
                     ON_TextLog& text_log, 
                     CTestCurve& test_curve, 
                     CTestSurface& test_surface,
                     const ON_SimpleArray<ON_X_EVENT>& x,
                     int overlap_sample_count,
                     double intersection_tolerance,
                     double overlap_tolerance
                     )
{
  int gi;
  int j, xi;
  double d;

  int missed_event_count = 0;

  intersection_tolerance = ON_X_EVENT::IntersectionTolerance(intersection_tolerance);
  overlap_tolerance = ON_X_EVENT::OverlapTolerance(intersection_tolerance,overlap_tolerance);
  // testing accuracy
  const double test_tolerance = 1.01*intersection_tolerance;

  const ON_Curve* curve = test_curve.m_curve;
  const ON_Surface* surface = test_surface.m_surface;
  const IwBSplineCurve* goose_curve = test_curve.GooseCurve();
  const IwBSplineSurface* goose_surface = test_surface.GooseSurface();


  ON_SimpleArray<ON_X_EVENT> goose_x(16 + x.Count());
  int bogus_events_from_old_csx = GetGooseCurveSurfaceIntersection(
     curve,surface,goose_curve,goose_surface,
     intersection_tolerance,overlap_tolerance,test_tolerance,
     goose_x
     );

  for ( gi = 0; gi < goose_x.Count(); gi++ )
  {
    ON_X_EVENT& goose_event = goose_x[gi];
    for ( j = 0; j < 2; j++ )
    {
      if ( j && ON_X_EVENT::csx_overlap != goose_event.m_type)
        break;

      double closest_d = 1.0e300;
      int closest_i = -1;

      // find the csx event that is closest to goose_event.m_A[j]
      for ( xi = 0; xi < x.Count() && closest_d > test_tolerance; xi++ )
      {
        const ON_X_EVENT& e = x[xi];
        d = goose_event.m_A[j].DistanceTo(e.m_A[0]);
        if ( d < closest_d )
        {
          closest_d = d;
          closest_i = xi;
        }

        if ( ON_X_EVENT::csx_overlap == e.m_type )
        {
          if ( d <= test_tolerance && 0 == j && goose_event.m_a[0] < e.m_a[0] )
          {
            goose_event.m_a[0] = e.m_a[0];
          }

          d = goose_event.m_A[j].DistanceTo(e.m_A[1]);
          if ( d < closest_d )
          {
            closest_d = d;
            closest_i = xi;
          }
          if ( d <= test_tolerance && 1 == j && goose_event.m_a[1] > e.m_a[1] )
          {
            goose_event.m_a[1] = e.m_a[1];
          }

          if ( e.m_a[0] <= goose_event.m_a[j] && goose_event.m_a[j] <= e.m_a[1] )
          {
            // goose event is inside an overlap region
            d = test_tolerance;
            if ( d < closest_d )
            {
              closest_d = d;
              closest_i = xi;
            }
          }
        }
      }

      if ( closest_d > test_tolerance )
      {
        ON_ERROR("ON_Curve::IntersectSurface() missed a point");
        text_log.Print("ERROR: Missed an intersection event at curve(%g) = ",goose_event.m_a[j]);
        text_log.Print(goose_event.m_A[j]);
        text_log.Print("\n");
        missed_event_count++;
      }
    }

    if ( ON_X_EVENT::csx_overlap == goose_event.m_type && goose_event.m_a[0] < goose_event.m_a[1] )
    {
      // make sure we found this overlap
      for ( xi = 0; xi < x.Count(); xi++ )
      {
        const ON_X_EVENT& e = x[xi];
        if ( e.m_a[0] <= goose_event.m_a[0] && goose_event.m_a[1] <= e.m_a[1] )
        {
          break;
        }

        if (  goose_event.m_a[1] - goose_event.m_a[0] <= 1.0e-3
             && e.m_A[0].DistanceTo( goose_event.m_A[0] ) <= test_tolerance 
             && e.m_A[0].DistanceTo( goose_event.m_A[1] ) <= test_tolerance 
             && e.m_A[0].DistanceTo( curve->PointAt(0.5*(goose_event.m_a[0]+goose_event.m_a[1])) ) <= test_tolerance 
             )
        {
          // bogus goose micro overlap
          break;
        }
      }

      if ( xi >= x.Count() )
      {
        ON_ERROR("ON_Curve::IntersectSurface() missed an overlap.");
        text_log.Print("ERROR: Missed an itersection event on curve(%g to %g) = \n",goose_event.m_a[0],goose_event.m_a[1]);
        text_log.PushIndent();
        text_log.Print(goose_event.m_A[0]); text_log.Print(" to ");
        text_log.Print(goose_event.m_A[1]);
        text_log.PopIndent();
        text_log.Print("\n");
        missed_event_count++;
      }
    }
  }

  if ( bogus_events_from_old_csx > 0 )
  {
    ON_WARNING("Old TL_CSX had bogus events - make sure TestCSXCompareWithGoose() is working right.");
    text_log.Print("ALERT Old TL_CSX had bogus events - make sure TestCSXCompareWithGoose() is working right.\n\n");
  }

  return missed_event_count;
}

static int TestCSX( ON_TextLog& text_log, 
                     CTestCurve& test_curve, 
                     CTestSurface& test_surface,
                     int* missed_event_count
                     )
{
  int i;

  const ON_Curve* curve = test_curve.m_curve;

  const ON_Surface* surface = test_surface.m_surface;

  *missed_event_count = 0;

  const int overlap_sample_count = bPurify ? 3 : 255;

  const double intersection_tolerance = 0.0;
  const double overlap_tolerance = 0.0;

  const ON_CurveTree* ctree = curve->CurveTree();
  if ( !ctree )
  {
    text_log.Print("ERROR - curve->CurveTree() = NULL\n");
    return 0;
  }

  const ON_SurfaceTree* stree = surface->SurfaceTree();
  if ( !stree )
  {
    text_log.Print("ERROR - surface->SurfaceTree() = NULL\n");
    return 0;
  }

  ON_SimpleArray<ON_X_EVENT> x(16);
  curve->IntersectSurface(surface,x,intersection_tolerance,overlap_tolerance);

  text_log.Print("%d curve-surface intersection events.\n",x.Count());
  text_log.PushIndent();

  // dump intersection information
  ON_String saved_double_format;
  text_log.GetDoubleFormat(saved_double_format);
  text_log.SetDoubleFormat("%g"); // so diffs don't flag minute changes
  for ( i = 0; i < x.Count(); i++ )
  {
    text_log.Print("xevent[%d]:\n",i);
    text_log.PushIndent();
    x[i].Dump(text_log);
    text_log.PopIndent();
  }
  text_log.SetDoubleFormat(saved_double_format);

  // data structure validation
  if ( x.Count() > 0 )
  {
    text_log.Print("Validating intersection events ...\n");
    text_log.PushIndent();
    bool bIsValid = ON_X_EVENT::IsValidList(
                      x.Count(),
                      x.Array(),
                      &text_log,
                      intersection_tolerance,
                      overlap_tolerance,
                      curve,0,
                      0,0,
                      surface,0,0
                      );
    text_log.PopIndent();
    if ( bIsValid )
    {
      text_log.Print("... Event is list valid.\n");
    }
  }

  if ( !bPurify )
  {
    // Compare results with goose to see if there are any missed events.
    // (This really slow's down the purify tests - so its skipped in purify builds.)
    i =  TestCSXCompareWithGoose( 
                             text_log, 
                             test_curve, 
                             test_surface, 
                             x, 
                             overlap_sample_count, 
                             intersection_tolerance, 
                             overlap_tolerance
                             );

    *missed_event_count = *missed_event_count + i;
  }

  text_log.PopIndent();

  return x.Count();
}

// tool for saving problem cases in file that has a single curve and surface.
bool SaveCSXTest( const char* filename,
                  const ON_Curve* curve,
                  const ON_Surface* surface
                  )
{
  bool rc = false;

  if ( curve && surface && filename && filename[0] )
  {
    ONX_Model model;

    ONX_Model_Object& curve_object = model.m_object_table.AppendNew();
    curve_object.m_object = curve;
    curve_object.m_bDeleteObject = false;

    ONX_Model_Object& surface_object = model.m_object_table.AppendNew();
    surface_object.m_object = surface;
    surface_object.m_bDeleteObject = false;

    model.Polish();
    rc = model.Write(filename);
  }

  return rc;
}


static void TestCurveSurfaceIntersectionListObjects( CTestModel& model, ON_TextLog& text_log )
{
  const int curve_count = model.m_curves.Count();
  const int surface_count = model.m_surfaces.Count();

  int i;

  const wchar_t* name;
  ON_UUID curve_uuid, surface_uuid;

  text_log.Print("%d curves:\n",curve_count);
  text_log.PushIndent();
  for ( i = 0; i < curve_count; i++ )
  {
    CTestCurve& C = model.m_curves[i];
    curve_uuid = C.m_uuid;
    name = C.m_name;
    if ( !name || !name[0] )
      name = L"anonymous curve";
    const ON_NurbsCurve& nurbs_curve = C.NurbsCurve();
    text_log.Print("curve[%d] %s\n",i,C.m_curve->ClassId()->ClassName());
    text_log.PushIndent();
    text_log.Print("id: "); text_log.Print(curve_uuid); text_log.Print("\n");
    text_log.Print(L"name: %s\n",name);
    text_log.Print("degree = %d, %s,  CV count=%d\n",
                nurbs_curve.m_order-1,
                (nurbs_curve.m_is_rat ? L"rational" : L"non-rational"),
                nurbs_curve.m_cv_count
                );
    text_log.PopIndent();
  }
  text_log.PopIndent();


  text_log.Print("\n");
  text_log.Print("%d surfaces:\n",surface_count);
  text_log.PushIndent();
  for ( i = 0; i < surface_count; i++ )
  {
    CTestSurface& S = model.m_surfaces[i];
    surface_uuid = S.m_uuid;
    name = S.m_name;
    if ( !name || !name[0] )
      name = L"anonymous surface";
    const ON_NurbsSurface& nurbs_surface = S.NurbsSurface();
    text_log.Print("surface[%d] %s\n",i,S.m_surface->ClassId()->ClassName());
    text_log.PushIndent();
    text_log.Print("id: "); text_log.Print(surface_uuid); text_log.Print("\n");    
    text_log.Print(L"name: %s\n",name);
    text_log.Print(L"degree = (%d,%d), %s,  CV count=(%d,%d)\n",
                nurbs_surface.m_order[0]-1,nurbs_surface.m_order[1]-1,
                (nurbs_surface.m_is_rat ? L"rational" : L"non-rational"),
                nurbs_surface.m_cv_count[0],nurbs_surface.m_cv_count[1]
                );
    text_log.PopIndent();

  }
  text_log.PopIndent();
}


static int TestCurveSurfaceIntersectionAccuracy( CTestModel& model, ON_TextLog& text_log )
{
  const int curve_count = model.m_curves.Count();
  const int surface_count = model.m_surfaces.Count();

  ON_UUID curve_uuid, surface_uuid;

  int total_missed_event_count = 0;
  int missed_event_count;

  int i, j;

  bool bSaveExample = false;

  int event_count = 0;


  int bad_curve_index   = -1;
  int bad_surface_index = -1;

  // intersect every curve with every surface
  for ( i = ( bad_curve_index>=0 ? bad_curve_index : 0); i < curve_count; i++ )
  {
    CTestCurve& test_curve = model.m_curves[i];
    curve_uuid = test_curve.m_uuid;

    for ( j = (bad_surface_index>=0 ? bad_surface_index : 0); j < surface_count; j++ )
    {
      CTestSurface& test_surface = model.m_surfaces[j];
      surface_uuid = test_surface.m_uuid;

      if ( bad_curve_index != -1 && bad_surface_index != -1 )
      {
        // use debugger to set bSaveExample = true
        ON_String filename;
        filename.Format("C:\\csx_bug_%03d_%03d.3dm",bad_curve_index,bad_surface_index);
        SaveCSXTest( filename.Array(), test_curve.m_curve, test_surface.m_surface );
        printf("Saved curve[%d] and surface[%d] in %s\n",i,j,filename.Array());
      }

      text_log.Print("curve[%d] ",i);    text_log.Print(curve_uuid);   text_log.Print("\n");
      text_log.Print("surface[%d] ",j); text_log.Print(surface_uuid); text_log.Print("\n");
      text_log.PushIndent();
      missed_event_count = 0;
      event_count += TestCSX( text_log, test_curve, test_surface, &missed_event_count );
      total_missed_event_count += missed_event_count;
      text_log.PopIndent();
      text_log.Print("\n");

      if ( bad_surface_index == j )
        break;
    }

    if ( bad_curve_index == i )
      break;

  }

  return total_missed_event_count;
}

static void TestCurveSurfaceIntersectionSpeed( CTestModel& model, ON_TextLog& text_log )
{
  const int curve_count = model.m_curves.Count();
  const int surface_count = model.m_surfaces.Count();

  int i, j;

  // pre-build GSLib trees
  int c_count = 0;
  for( i = 0; i < curve_count; i++ )
  {
    CTestCurve& C = model.m_curves[i];
    const IwBSplineCurve* goose_curve = C.GooseCurve();
    if ( goose_curve )
    {
      IwCacheMgr::GetOrCreateObjectCache(IW_OC_CURVE,goose_curve);
      c_count++;
    }
  }

  int s_count = 0;
  for ( j = 0; j < surface_count; j++ )
  {
    CTestSurface& S = model.m_surfaces[j];
    const IwBSplineSurface* goose_surface = S.GooseSurface();
    if (goose_surface )
    {
      s_count++;
      IwCacheMgr::GetOrCreateObjectCache(IW_OC_SURFACE,goose_surface);
    }
  }

  double intersection_tolerance = 0.001;
  double dale_time = 0.0;
  double goose_time = 0.0;
  double min_time = 0.0;
  double delta_time;

  int n_count = 0;
  int dale_event_count = 0;
  int goose_event_count = 0;

  if ( c_count > 0 && s_count > 0 )
  {
    n_count = 1;
#if !defined(ON_DEBUG)
    while ( n_count*c_count*s_count < 20000 )
    {
      n_count++;
    }
#endif
    int n, test;

    // test 0 = dale, 1 = goose
    for ( test = 0; test < 2; test++ )
    {
      if ( 1==test && model.m_bPurify )
      {
        n_count = 1;
        goose_time = dale_time = 0.0;
        break;
      }

      TEST_ElapsedTime();
      for ( n = 0; n < n_count; n++ )
      {
        for( i = 0; i < curve_count; i++ )
        {
          CTestCurve& C = model.m_curves[i];
          const ON_Curve* curve = C.m_curve;
          const IwBSplineCurve* goose_curve = C.m_goose_curve;
          if ( 0 == goose_curve )
            continue;

          for ( j = 0; j < surface_count; j++ )
          {
            CTestSurface& S = model.m_surfaces[j];
            const ON_Surface* surface = S.m_surface;
            const IwBSplineSurface* goose_surface = S.m_goose_surface;
            if ( 0 == goose_surface )
              continue;

            if (test)
            {
              // goose
              IwSolutionArray x(16);
              goose_surface->GlobalCurveIntersect(
                                goose_surface->GetNaturalUVDomain(),
                                *goose_curve,
                                goose_curve->GetNaturalInterval(),
                                intersection_tolerance,
                                x 
                                );
              if ( !n )
                goose_event_count += x.GetSize();
            }
            else
            {
              // dale
              ON_SimpleArray<ON_X_EVENT> x(16);
              curve->IntersectSurface( surface, x, intersection_tolerance );
              if ( !n )
                dale_event_count += x.Count();
            }
          }
        }
      }
      delta_time = TEST_ElapsedTime();

      switch(test)
      {
      case 0:
        dale_time = delta_time;
        break;
      case 1:
        goose_time = delta_time;
        break;
      }
    }
  }

  min_time = (dale_time < goose_time) ? dale_time : goose_time;
  if ( min_time <= 0.0 )
    min_time = 1.0;

  text_log.Print("%d repetitions of %d intersection tests.\n",n_count,c_count*s_count);
  text_log.Print("Test       Dale     Goose\n");
  if ( model.m_bPurify )
  {
  }
  else
  {
    text_log.Print("Seconds    %6.2f   %6.2f\n",dale_time,goose_time);
    text_log.Print("Relative   %5.2fX    %5.2fX\n",dale_time/min_time,goose_time/min_time);
  }
  text_log.Print("Events     %5d    %5d\n",dale_event_count,goose_event_count);
}

void TestCurveSurfaceIntersection( CTestModel& model, ON_TextLog& text_log )
{
  TEST_HEADER(text_log,"TestCurveSurfaceIntersection");

  bool bListObjects   = true;
  bool bAccuracyTests = true;
  bool bSpeedTests    = true;

  bPurify = model.m_bPurify;

#if defined(_DEBUG)
  bSpeedTests = false;
#endif;

  int total_missed_event_count = 0;

  if ( bListObjects && !bPurify )
  {
    text_log.Print("\n");
    text_log.Print("\n");
    text_log.Print("Curve-Surfaces Intersection Objects:\n");
    text_log.Print("\n");
    text_log.PushIndent();
    TestCurveSurfaceIntersectionListObjects( model, text_log );
    text_log.PopIndent();
  }

  if ( bAccuracyTests )
  {
    text_log.Print("\n");
    text_log.Print("\n");
    text_log.Print("Curve-Surface Intersection Accuracy Tests:\n");
    text_log.Print("\n");
    text_log.PushIndent();
    total_missed_event_count = TestCurveSurfaceIntersectionAccuracy( model, text_log );
    text_log.PopIndent();
  }

  if ( bSpeedTests && !bPurify )
  {
    text_log.Print("\n");
    text_log.Print("\n");
    text_log.Print("Curve-Surface Intersection Speed Tests:\n");
    text_log.Print("\n");
    text_log.PushIndent();
    TestCurveSurfaceIntersectionSpeed(model,text_log );
    text_log.PopIndent();
  }

  if ( total_missed_event_count > 0 )
  {
    text_log.Print(
              "Opennurbs may have missed up to %d intersection events \n"
              "and these potential misses are counted as errors below.\n",
              total_missed_event_count
              );
  }
}


