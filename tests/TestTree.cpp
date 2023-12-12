#include "Tests.h"


static const bool bPurify     = false;
static const bool bDoCurves   = true;
static const bool bDoSurfaces = true;

bool TestCurveTree(ON_TextLog& text_log, const ON_Curve* curve )
{
  if ( !curve )
  {
    text_log.Print("Curve pointer is NULL.\n");
    return false;
  }

  const ON_CurveTree* tree = curve->CurveTree();
  if ( !tree )
  {
    text_log.Print("CurveTree() returned  NULL.\n");
    return false;
  }

  bool rc = tree->IsValid( &text_log, curve );
  if ( !rc )
  {
    text_log.Print("Curve tree is not valid\n");
  }

  if ( tree != tree->m_root )
  {
    // As of 29 March, I want tree to be = tree->m_root.
    //  and I'd like to get rid of the m_root field, but
    //  I don't know if that will work for making
    //  auxillary tree's over a set of trees.
    text_log.Print("Curve tree != tree->m_root - please tell Dale Lear.\n");
  }

  const int span_count = curve->SpanCount();
  ON_SimpleArray<double> t(10*(span_count+1));
  t.SetCount(span_count+1);
  curve->GetSpanVector(t.Array());
  int i;
  for ( i = 0; i < span_count; i++ )
  {
    t.Append(0.5*(t[i]+t[i+1]));
  }


  ON_CurveTreeNode* leaf = tree->FirstLeaf();
  int leaf_count=0;
  double maxar = 0.0;
  double maxr = 0.0;
  double maxlen = 0.0;
  double longest = 0.0;
  double thickest = 0.0;
  double shortest = fabs(ON_UNSET_VALUE);
  ON_Line axis;
  int notmonocount = 0;
  while (leaf)
  {
    if ( t.Search( leaf->m_domain[0] ) < 0 )
      t.Append(leaf->m_domain[0]);
    t.Append( leaf->m_domain.ParameterAt(0.2) );
    t.Append( leaf->m_domain.ParameterAt(0.7) );
    if( !leaf->m_bez->m_leafbox.m_bMono )
      notmonocount++;
    axis.from = leaf->m_bez->PointAt(0.0);
    axis.to = leaf->m_bez->PointAt(1.0);
    double len = axis.Length();
    if (len > longest )
      longest = len;
    if ( len < shortest )
      shortest = len;
    if ( leaf->m_bez->m_leafbox.m_r > thickest )
      thickest = leaf->m_bez->m_leafbox.m_r;
    if ( leaf->m_bez->m_leafbox.m_r > maxar*len )
    {
      maxar = leaf->m_bez->m_leafbox.m_r/len;
      maxlen = len;
      maxr = leaf->m_bez->m_leafbox.m_r;
    }
    leaf_count++;
    leaf = leaf->NextLeaf();
  }
  if ( notmonocount > 0 )
  {
    text_log.Print("ON_CurveTree: %d spans, %d leaves (%d are not monotone).\n",span_count,leaf_count,notmonocount);
  }
  else
  {
    text_log.Print("ON_CurveTree: %d spans, %d leaves (all are monotone).\n",span_count,leaf_count);
  }

  text_log.PushIndent();
  text_log.Print("Shortest: %g\n",shortest);
  text_log.Print("Longest: %g\n",longest);
  text_log.Print("Thickest: radius = %g\n",thickest);
  text_log.Print("Fattest: rad/len = %g (rad = %g, len = %g)\n",maxar,maxr,maxlen);
  text_log.PopIndent();

  // test evaluation
  const bool bAdjustParameter = tree->AdjustParameter();
  double x, y, d0, d1, tol0, tol1;
  ON_3dVector X[2], Y[2], Xerr[2], Yerr[2];
  double xerr, yerr, d0err=0.0, d1err=0.0;
  int error_count = 0;
  int test_count = 0;
  int hint = 0;
  int side = 0;
  const ON_CurveTreeNode* evaluation_node;
  if ( 2 == curve->Dimension() )
  {
    X[0].z = X[1].z = Y[0].z = Y[1].z = 0.0;
  }
  for ( i = 0; i < t.Count(); i++ )
  {
    x = y = t[i];
    if ( i > span_count && bAdjustParameter)
    {
      // NOTE - the curve paramter = nurbs form paramter
      //        at all span vector values.
      curve->GetCurveParameterFromNurbFormParameter(x,&y);
    }
    for ( side = -1; side < 1; side++ )
    {
      test_count++;
      X[0].x = -99.0;
      Y[0].x = 99.0;
      curve->Evaluate(y,1,3,&Y[0].x,side,&hint);
      evaluation_node = tree->Evaluate(x,1,3,&X[0].x,side);
      if ( !evaluation_node )
      {
        error_count++;
        ON_ERROR("TestCurveTree() - tree failed to evaluate.");
        d0err = 1.0e100;
        d1err = 1.0e100;
        xerr = x;
        yerr = y;
        Xerr[0] = ON_UNSET_POINT;
        Xerr[1] = ON_UNSET_POINT;
        Yerr[0] = Y[0];
        Yerr[1] = Y[1];
      }
      else
      {
        if ( side < 0 && x == evaluation_node->m_domain[0] )
        {
          // x should be at the start of the curve's domain
          // and there shouldn't be any leaves before this one.
          if ( evaluation_node->PrevLeaf() || x != t[0] )
          {
            ON_ERROR("TestCurveTree() - evaluation used wrong node");
          }
        }
        else if ( side > 0 && x == evaluation_node->m_domain[1] )
        {
          // x should be at the end of the curve's domain
          // and there shouldn't be any leaves after this one.
          if ( evaluation_node->NextLeaf() || x != t[span_count] )
          {
            ON_ERROR("TestCurveTree() - evaluation used wrong node");
          }
        }

        if ( bAdjustParameter )
        {
          // Different parameterizations result in derivatives
          // with different lengths.
          X[1].Unitize();
          Y[1].Unitize();
        }
        tol0 = ON_ZERO_TOLERANCE + ON_SQRT_EPSILON*Y[0].MaximumCoordinate();
        tol1 = ON_ZERO_TOLERANCE + ON_SQRT_EPSILON*Y[1].MaximumCoordinate();
        d0 = (X[0]-Y[0]).Length();
        d1 = (X[1]-Y[1]).Length();
        if ( d0 > tol0 || d1 > tol1 )
        {
          rc = false;
          error_count++;
          if ( d0 > d0err || (d1 > d1err && d0err < 0.01) )
          {
            if ( d0 > d0err) d0err = d0;
            if ( d1 > d1err) d1err = d1;
            xerr = x;
            yerr = y;
            Xerr[0] = X[0];
            Xerr[1] = X[1];
            Yerr[0] = Y[0];
            Yerr[1] = Y[1];
          }
        }
      }
    }
  }
  text_log.PushIndent();
  if ( error_count > 0 )
  {
    ON_ERROR("TestCurveTree() - tree failed evaluation test.");
    text_log.Print("Evaluation test at %d points had %d errors.\n",test_count,error_count);
    text_log.PushIndent();
    text_log.Print("curve(%g) = (%g,%g,%g)\n",yerr,Y[0].x,Y[0].y,Y[0].z);
    text_log.Print("tree(%g)  = (%g,%g,%g)\n",xerr,X[0].x,X[0].y,X[0].z);
    text_log.Print("curve D = (%g,%g,%g)\n",Y[1].x,Y[1].y,Y[1].z);
    text_log.Print("tree D  = (%g,%g,%g)\n",X[1].x,X[1].y,X[1].z);
    text_log.PopIndent();
  }
  else
  {
    text_log.Print("Evaluation test at %d points was successful.\n",test_count);
  }
  text_log.PopIndent();

  return rc;
}


bool TestSurfaceTree(ON_TextLog& text_log, const ON_Surface* surface )
{
  if ( !surface )
  {
    text_log.Print("Surface pointer is NULL.\n");
    return false;
  }

  const ON_SurfaceTree* tree = surface->SurfaceTree();
  if ( !tree )
  {
    text_log.Print("SurfaceTree() returned NULL.\n");
    return false;
  }

  bool rc = tree->IsValid( &text_log, surface );
  if ( !rc )
  {
    text_log.Print("Surface tree is not valid.\n");
  }

  int i;
  const int span_count0 = surface->SpanCount(0);
  ON_SimpleArray<double> s(10*(span_count0 + 1));
  s.SetCount(span_count0+1);
  surface->GetSpanVector( 0, s.Array() );
  for ( i = 0; i < span_count0; i++ )
  {
    s.Append(0.5*(s[i]+s[i+1]));
  }

  int j;
  const int span_count1 = surface->SpanCount(1);
  ON_SimpleArray<double> t(10*(span_count1 + 1));
  t.SetCount(span_count1+1);
  surface->GetSpanVector( 1, t.Array() );
  for ( j = 0; j < span_count1; j++ )
  {
    t.Append(0.5*(t[j]+t[j+1]));
  }

  const int span_count = span_count0*span_count1;

  ON_SurfaceTreeNode* leaf = tree->FirstLeaf();
  int leaf_count=0;
  double maxar = 0.0;
  double maxr = 0.0;
  double maxlen = 0.0;
  double longest = 0.0;
  double thickest = 0.0;
  double shortest = fabs(ON_UNSET_VALUE);
  ON_3dPoint C[4];
  int notmonocount = 0;
  while (leaf)
  {
    if ( s.Search(leaf->m_domain[0][0]) < 0 )
      s.Append(leaf->m_domain[0][0]);
    if ( s.Search(leaf->m_domain[0].ParameterAt(0.3)) < 0 )
      s.Append(leaf->m_domain[0].ParameterAt(0.3));
    if ( s.Search(leaf->m_domain[0].ParameterAt(0.8)) < 0 )
      s.Append(leaf->m_domain[0].ParameterAt(0.8));

    if ( t.Search(leaf->m_domain[1][0]) < 0 )
      t.Append(leaf->m_domain[1][0]);
    if ( t.Search(leaf->m_domain[1].ParameterAt(0.2)) < 0 )
      t.Append(leaf->m_domain[1].ParameterAt(0.2));
    if ( t.Search(leaf->m_domain[1].ParameterAt(0.7)) < 0 )
      t.Append(leaf->m_domain[1].ParameterAt(0.7));


    if( !leaf->m_bez->m_leafbox.m_bMono )
      notmonocount++;
    C[0] = leaf->m_bez->PointAt(0.0,0.0);
    C[1] = leaf->m_bez->PointAt(1.0,0.0);
    C[2] = leaf->m_bez->PointAt(1.0,1.0);
    C[3] = leaf->m_bez->PointAt(0.0,1.0);

    double len = C[3].DistanceTo(C[0]);

    for ( i = 0; i < 3; i++ )
    {
      double x = C[i].DistanceTo(C[i+1]);
      if ( x > len )
        len = x;
    }

    if (len > longest )
      longest = len;

    if ( len < shortest )
      shortest = len;

    if ( leaf->m_bez->m_leafbox.Height() > thickest )
      thickest = leaf->m_bez->m_leafbox.Height();

    if ( leaf->m_bez->m_leafbox.Height() > maxar*len )
    {
      maxlen = len;
      maxr = leaf->m_bez->m_leafbox.Height();
      maxar = maxr/len;
    }
    leaf_count++;
    leaf = leaf->NextLeaf();
  }

  if ( notmonocount > 0 )
  {
    text_log.Print("ON_SurfaceTree: %d spans, %d leaves (%d are not monotone).\n",span_count,leaf_count,notmonocount);
  }
  else
  {
    text_log.Print("ON_SurfaceTree: %d spans, %d leaves (all are monotone).\n",span_count,leaf_count);
  }

  text_log.PushIndent();
  text_log.Print("Shortest: %g\n",shortest);
  text_log.Print("Longest: %g\n",longest);
  text_log.Print("Thickest: height = %g\n",thickest);
  text_log.Print("Fattest: ht/len = %g (ht = %g, len = %g)\n",maxar,maxr,maxlen);
  text_log.PopIndent();

  // test evaluation
  const bool bAdjustParameter = tree->AdjustParameter();
  ON_2dPoint x,y;
  double d0, ds, dt, tol0, tol1, tol2;
  ON_3dVector X[3], Y[3], Xerr[3], Yerr[3];
  ON_2dVector xerr, yerr;
  double d0err=0.0, d1err=0.0;
  int error_count = 0;
  int test_count = 0;
  int hint[2] ={0,0};
  int quadrant = 0;
  const ON_SurfaceTreeNode* evaluation_node;
  for ( i = 0; i < s.Count(); i++ )
  {
    x.x = y.x = s[i];
    for ( j = 0; j < t.Count(); j++ )
    {
      x.y = y.y = t[j];
      if (( i > span_count0 || j > span_count1) && bAdjustParameter)
      {
        // NOTE - the surface paramter = nurbs form paramter
        //        at all span vector values.
        surface->GetSurfaceParameterFromNurbFormParameter(s[i],t[j],&y.x,&y.y);
      }

      for (quadrant = 0; quadrant <= 4; quadrant++ )
      {
        test_count++;
        X[0].x = -99.0;
        Y[0].x = 99.0;
        surface->Evaluate(y.x,y.y,1,3,&Y[0].x,quadrant,hint);
        evaluation_node = tree->Evaluate(x.x,x.y,1,3,&X[0].x,quadrant);
        if ( !evaluation_node )
        {
          error_count++;
          ON_ERROR("TestSuraceTree() - tree failed to evaluate.");
          d0err = 1.0e100;
          d1err = 1.0e100;
          xerr = x;
          yerr = y;
          Xerr[0] = ON_UNSET_POINT;
          Xerr[1] = ON_UNSET_POINT;
          Xerr[2] = ON_UNSET_POINT;
          Yerr[0] = Y[0];
          Yerr[1] = Y[1];
          Yerr[2] = Y[2];
        }
        else
        {
          if ( x.x == evaluation_node->m_domain[0][0] && x.x > s[0] )
          {
            if ( 2 == quadrant || 3 == quadrant )
            {
              ON_ERROR("TestSuraceTree() ON_SurfaceTreeNode::Evaluate() used the wrong node.");
            }
          }
          else if ( x.x == evaluation_node->m_domain[0][1] && x.x < s[span_count0] )
          {
            if ( 1 == quadrant || 4 == quadrant )
            {
              ON_ERROR("TestSuraceTree() ON_SurfaceTreeNode::Evaluate() used the wrong node.");
            }
          }

          if ( x.y == evaluation_node->m_domain[1][0] && x.y > t[0] )
          {
            if ( 3 == quadrant || 4 == quadrant )
            {
              ON_ERROR("TestSuraceTree() ON_SurfaceTreeNode::Evaluate() used the wrong node.");
            }
          }
          else if ( x.y == evaluation_node->m_domain[1][1] && x.y < t[span_count1] )
          {
            if ( 1 == quadrant || 2 == quadrant )
            {
              ON_ERROR("TestSuraceTree() ON_SurfaceTreeNode::Evaluate() used the wrong node.");
            }
          }


          if ( bAdjustParameter )
          {
            // Different parameterizations result in derivatives
            // with different lengths.
            if ( X[1].Length() <= ON_ZERO_TOLERANCE && Y[1].Length() <= ON_ZERO_TOLERANCE )
            {
              // singular point 
              //  Probably a place where the revolute hits the axis in a surface of revolution 
              X[1].Zero();
              Y[1].Zero();
            }
            else
            {
              X[1].Unitize();
              Y[1].Unitize();
            }
            if ( X[2].Length() <= ON_ZERO_TOLERANCE && Y[2].Length() <= ON_ZERO_TOLERANCE )
            {
              // singular point
              //  Probably a place where the revolute hits the axis in a surface of revolution.
              X[2].Zero();
              Y[2].Zero();
            }
            else
            {
              X[2].Unitize();
              Y[2].Unitize();
            }
          }
          tol0 = ON_ZERO_TOLERANCE + ON_SQRT_EPSILON*Y[0].MaximumCoordinate();
          tol1 = ON_ZERO_TOLERANCE + ON_SQRT_EPSILON*Y[1].MaximumCoordinate();
          tol2 = ON_ZERO_TOLERANCE + ON_SQRT_EPSILON*Y[2].MaximumCoordinate();
          d0 = (X[0]-Y[0]).Length();
          ds = (X[1]-Y[1]).Length();
          dt = (X[2]-Y[2]).Length();
          if ( d0 > tol0 || ds > tol1 || dt > tol2 )
          {
            rc = false;
            error_count++;
            if ( d0 > d0err || (ds > d1err && d0err < 0.01) || (dt > d1err && d0err < 0.01) )
            {
              if ( d0 > d0err) d0err = d0;
              if ( ds > d1err) d1err = ds;
              if ( dt > d1err) d1err = dt;
              xerr = x;
              yerr = y;
              Xerr[0] = X[0];
              Xerr[1] = X[1];
              Xerr[2] = X[2];
              Yerr[0] = Y[0];
              Yerr[1] = Y[1];
              Yerr[2] = Y[2];
            }
          }
        }
      }
    }
  }

  text_log.PushIndent();
  if ( error_count > 0 )
  {
    ON_ERROR("TestSurfaceTree() - tree failed evaluation test.");
    text_log.Print("Evaluation test at %d points had %d errors.\n",test_count,error_count);
    text_log.PushIndent();
    text_log.Print("surface(%g,%g) = (%g,%g,%g)\n",yerr.x,yerr.y,Y[0].x,Y[0].y,Y[0].z);
    text_log.Print("tree(%g,%g)  = (%g,%g,%g)\n",xerr.x,xerr.y,X[0].x,X[0].y,X[0].z);
    text_log.Print("curve Ds = (%g,%g,%g)\n",Y[1].x,Y[1].y,Y[1].z);
    text_log.Print("tree Ds  = (%g,%g,%g)\n",X[1].x,X[1].y,X[1].z);
    text_log.Print("curve Dt = (%g,%g,%g)\n",Y[2].x,Y[2].y,Y[2].z);
    text_log.Print("tree Dt  = (%g,%g,%g)\n",X[2].x,X[2].y,X[2].z);
    text_log.PopIndent();
  }
  else
  {
    text_log.Print("Evaluation test at %d points was successful.\n",test_count);
  }
  text_log.PopIndent();

  return rc;
}

static
void TestCurveTreeHelper( ON_TextLog& text_log, const ON_Curve* curve, const wchar_t* name )
{
  TL_NurbsCurve nurbs_curve;

  if ( curve )
  {
    if ( 0 == name || 0 == *name)
    {
      name = L"anonymous";
    }

    curve->GetNurbForm(nurbs_curve);

    text_log.Print(L"Curve class = %S, name = %s\n",
                  curve->ClassId()->ClassName(),
                  name
                  );
    text_log.PushIndent();
    text_log.Print(L"degree = %d, %s,  CV count=%d\n",
                  nurbs_curve.m_order-1,
                  (nurbs_curve.m_is_rat ? L"rational" : L"non-rational"),
                  nurbs_curve.m_cv_count
                  );
    TestCurveTree( text_log, curve );
    text_log.PopIndent();
  }
}

static
void TestSurfaceTreeHelper( ON_TextLog& text_log, const ON_Surface* surface, const wchar_t* name )
{
  TL_NurbsSurface nurbs_surface;

  if ( surface )
  {
    if ( 0 == name || 0 == *name)
    {
      name = L"anonymous";
    }
    surface->GetNurbForm(nurbs_surface);

    text_log.Print(L"Surface class = %S, name = %s\n",
                  surface->ClassId()->ClassName(),
                  name
                  );
    text_log.PushIndent();
    text_log.Print(L"degree = (%d,%d), %s,  CV count= (%d,%d)\n",
                  nurbs_surface.m_order[0]-1,
                  nurbs_surface.m_order[1]-1,
                  (nurbs_surface.m_is_rat ? L"rational" : L"non-rational"),
                  nurbs_surface.m_cv_count[0],
                  nurbs_surface.m_cv_count[1]
                  );
    TestSurfaceTree( text_log, surface );
    text_log.PopIndent();
  }
}

void TestTree( const ONX_Model& model, ON_TextLog& text_log )
{
  TEST_HEADER(text_log,"TestClosestPoint");
  int i, k;

  ON_wString name;
  const wchar_t* attributes_name;

  // first do curves
  if (bDoCurves) 
  {
    for ( i = 0; i < model.m_object_table.Count(); i++ )
    {
      const ON_Curve* curve = ON_Curve::Cast(model.m_object_table[i].m_object);
      if ( curve )
      {
        attributes_name = model.m_object_table[i].m_attributes.m_name;
        TestCurveTreeHelper(text_log,curve,attributes_name);
        continue;
      }


      const ON_Brep* brep = ON_Brep::Cast(model.m_object_table[i].m_object);
      if ( brep )
      {
        for ( k = 0; k < brep->m_C3.Count(); k++ )
        {
          curve = brep->m_C3[k];
          if ( curve )
          {
            attributes_name = model.m_object_table[i].m_attributes.m_name;
            if ( !attributes_name )
              attributes_name = L"anonymous";
            name.Format(L"%s - brep.m_C3[%d]",attributes_name,k);
            TestCurveTreeHelper(text_log,curve,name);
          }
        }

        for ( k = 0; k < brep->m_C2.Count(); k++ )
        {
          curve = brep->m_C2[k];
          if ( curve )
          {
            attributes_name = model.m_object_table[i].m_attributes.m_name;
            if ( !attributes_name )
              attributes_name = L"anonymous";
            name.Format(L"%s - brep.m_C2[%d]",attributes_name,k);
            TestCurveTreeHelper(text_log,curve,name);
          }
        }
        continue;
      }
    }
  }

  // then do surfaces
  if ( bDoSurfaces )
  {
    for ( i = 0; i < model.m_object_table.Count(); i++ )
    {
      const ON_Surface* surface = ON_Surface::Cast(model.m_object_table[i].m_object);
      if ( surface )
      {
        attributes_name = model.m_object_table[i].m_attributes.m_name;
        TestSurfaceTreeHelper(text_log,surface,attributes_name);
        continue;
      }

      const ON_Brep* brep = ON_Brep::Cast(model.m_object_table[i].m_object);
      if ( brep )
      {
        for ( k = 0; k < brep->m_S.Count(); k++ )
        {
          surface = brep->m_S[k];
          if ( surface )
          {
            attributes_name = model.m_object_table[i].m_attributes.m_name;
            if ( !attributes_name )
              attributes_name = L"anonymous";
            name.Format(L"%s - brep.m_S[%d]",attributes_name,k);

            TestSurfaceTreeHelper(text_log,surface,model.m_object_table[i].m_attributes.m_name);
          }
        }
        continue;
      }
    }
  }
}