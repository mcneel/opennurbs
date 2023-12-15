/* $Header: /src4/opennurbs/Tests/TestClosestPoint.cpp 36    10/07/05 4:27p Dalelear $ */
/* $NoKeywords: $ */

#include "Tests.h"

static const bool bPurify     = false;
static const bool bDoPoints   = true;
static const bool bSmallTest  = false;
static const bool bDoGooseTests = false; // SLOW

static void CCXHelper3( const ON_CurveTreeNode* nodeA, const ON_CurveTreeNode* nodeB,
         double tol3d,
         ON_SimpleArray<TL_CX_EVENT>& ccx
         )
{
  // third level checker for CCX - both nodes are bottom level
  // leaves with m_bez != NULL
  const ON_CurveTreeBezier& bezA = *nodeA->m_bez;
  const ON_CurveTreeBezier& bezB = *nodeB->m_bez;
  double a, b;
  ON_3dPoint A, B;
  bezA.m_leafbox.GetClosestPointSeed( bezB.m_leafbox,&a,&b );

  // TODO - call 2d solver here

  // make sure results are ok.
  A = bezA.PointAt(a);
  B = bezB.PointAt(b);
  if ( A.DistanceTo(B) <= tol3d )
  {
    TL_CX_EVENT& x = ccx.AppendNew();
    x.A[0] = A;
    x.A[1] = A;
    x.B[0] = B;
    x.B[1] = B;
    x.a[0] = x.a[1] = nodeA->m_domain.ParameterAt(a);
    x.b[0] = x.b[1] = x.b[2] = x.b[3] = nodeB->m_domain.ParameterAt(b);
    x.type = TL_CCX_POINT;
    x.cnodeA[0] = nodeA;
    x.cnodeB[0] = nodeB;
    x.x[0] = A.DistanceTo(B);
  }
}

static void CCXHelper1( const ON_CurveTreeNode* nodeA, const ON_CurveTreeNode* nodeB, 
         double tol3d,
         ON_SimpleArray<TL_CX_EVENT>& ccx
         );

static void CCXHelper2( const ON_CurveTreeNode* nodeA, const ON_CurveTreeNode* nodeB,
         double tol3d,
         ON_SimpleArray<TL_CX_EVENT>& ccx
         )
{
  // second level checker for CCX - both nodes have m_bez != NULL and valid leaf boxes
  // but they may have down nodes
  if ( nodeA->m_down[0] || nodeA->m_down[1] )
  {
    if (    (nodeB->m_down[0] || nodeB->m_down[1])
         && (nodeA->m_bez->m_leafbox.m_L.Length() <= nodeB->m_bez->m_leafbox.m_L.Length()) )
    {
      // nodeB is bigger, split it
      CCXHelper1( nodeA, nodeB->m_down[0], tol3d, ccx );
      CCXHelper1( nodeA, nodeB->m_down[1], tol3d, ccx );
    }
    else
    {
      // nodeA is bigger, split t
      CCXHelper1( nodeA->m_down[0], nodeB, tol3d, ccx );
      CCXHelper1( nodeA->m_down[1], nodeB, tol3d, ccx );
    }
  }
  else if ( nodeB->m_down[0] || nodeB->m_down[1] )
  {
    CCXHelper1( nodeA, nodeB->m_down[0], tol3d, ccx );
    CCXHelper1( nodeA, nodeB->m_down[1], tol3d, ccx );
  }
  else
  {
    // time to look fot intersections on leaves
    CCXHelper3( nodeA, nodeB, tol3d, ccx );
  }
}

static void CCXHelper1( const ON_CurveTreeNode* nodeA, const ON_CurveTreeNode* nodeB, 
         double tol3d,
         ON_SimpleArray<TL_CX_EVENT>& ccx
         )
{
  // first level checker for CCX
  double a,b;
  if ( !nodeA )
    return;
  if ( !nodeB )
    return;
  if ( nodeA->IsFartherThan(tol3d,nodeB) )
  {
    return;
  }
  if ( nodeA->m_bez )
  {
    if ( nodeB->m_bez )
    {
      CCXHelper2( nodeA, nodeB, tol3d, ccx );
    }
    else
    {
      CCXHelper1(nodeA,nodeB->m_down[0],tol3d,ccx);
      CCXHelper1(nodeA,nodeB->m_down[1],tol3d,ccx);
    }
  }
  else if ( nodeB->m_bez )
  {
    if ( nodeA->m_bez )
    {
      CCXHelper2( nodeA, nodeB, tol3d, ccx );
    }
    else
    {
      CCXHelper1(nodeA->m_down[0],nodeB,tol3d,ccx);
      CCXHelper1(nodeA->m_down[1],nodeB,tol3d,ccx);
    }
  }
  else
  {
    a = nodeA->m_bbox.Diagonal().LengthSquared();
    b = nodeB->m_bbox.Diagonal().LengthSquared();
    if ( a <= b )
    {
      CCXHelper1(nodeA,nodeB->m_down[0],tol3d,ccx);
      CCXHelper1(nodeA,nodeB->m_down[1],tol3d,ccx);
    }
    else
    {
      CCXHelper1(nodeA->m_down[0],nodeB,tol3d,ccx);
      CCXHelper1(nodeA->m_down[1],nodeB,tol3d,ccx);
    }
  }
}


void DaleCCX( const ON_Curve& curveA,
         const ON_Curve& curveB,
         double tol3d,
         ON_SimpleArray<TL_CX_EVENT>& ccx
         )
{
  ON_SimpleArray<ON_X_EVENT> xxx(16);
  curveA.IntersectCurve(&curveB,xxx,tol3d);
	for(int i=0; i<xxx.Count(); i++){
		ON_X_EVENT& x = xxx[i];
		if( x.m_type==ON_X_EVENT::ccx_point || x.m_type==ON_X_EVENT::ccx_overlap){
			TL_CX_EVENT tlX;
			if( x.m_type==ON_X_EVENT::ccx_point)
				tlX.type = TL_CCX_POINT;
			else 
				tlX.type = TL_CCX_OVERLAP;

			tlX.A[0]=x.m_A[0];
			tlX.A[1]=x.m_A[1];
			tlX.B[0]=x.m_B[0];
			tlX.B[1]=x.m_B[1];
			tlX.a[0]=x.m_a[0];
			tlX.a[1]=x.m_a[1];
			tlX.b[0]=x.m_b[0];
			tlX.b[1]=x.m_b[1];
			tlX.b[2]=x.m_b[2];
			tlX.b[3]=x.m_b[3];
			ccx.Append(tlX);
		}
	}

  return;

  //ON_3dPoint A, B;
  //double d;
  const ON_CurveTree* treeA = curveA.CurveTree();
  const ON_CurveTree* treeB = curveB.CurveTree();

  //bool bRedo = true;
  //int i = ccx.Count();
  CCXHelper1( treeA->m_root, treeB->m_root, tol3d, ccx );

  /*
  while ( i < ccx.Count() )
  {
    bRedo = false;
    TL_CX_EVENT& e = ccx[i++];
    A = curveA.PointAt(e.a[0]);
    B = curveB.PointAt(e.b[0]);
    d = A.DistanceTo(B);
    if ( d > tol3d )
      bRedo = true;
  }

  if ( bRedo )
  {
    CCXHelper1( treeA->m_root, treeB->m_root, tol3d, ccx );
  }
  */
}




class EF
{
public:
  const ON_CurveTree* ct;
  const ON_CurveTreeNode* node;
  ON_3dPoint P;
  ON_BezierCurve b;
  double d;
  double s; // bezier parameter
  double t; // curve parameter
  int local_solves;
};


class Greg_farg
{
public:
  const ON_BezierCurve* bez;
  ON_3dPoint P;
  double GregLocalSolveAbsThresh2;
};

static 
int GregDistance2CrvPtHess(void* fdata, int n, const double* x, double& fx, double* df, double** Hess){
	Greg_farg* farg = (Greg_farg*) fdata;
	ON_3dVector CEval[3]; 
	ON_3dVector& C=CEval[0];
	ON_3dVector& Ct=CEval[1];
	ON_3dVector& Ctt=CEval[2];
	ON_BOOL32 rc = farg->bez->Evaluate(*x,(Hess)?2:1,3,CEval[0]);
  for(int i=farg->bez->Dimension(); i<3; i++)
		CEval[0][i]=CEval[1][i]=CEval[2][i] = 0.0;
	C -= farg->P;
	fx = C*C;
	if(df)
		*df = 2 * C* Ct;
	if(Hess && *Hess)
		Hess[0][0] = 2.0 * ( Ct*Ct + Ctt*C);
	if( fx <= farg->GregLocalSolveAbsThresh2 )
		return 2;
	return rc?1:-1;
}

int ClosestForkHelper_dbrent( EF& ef, const ON_CurveTreeNode* node )
{
  const ON_CurveTreeBezier* bez = node->m_bez;

  ON_3dPoint Q;
  double x, w, t;
  int i;

  ef.local_solves++;

  // set "t" to best guess based on the line.
  bez->m_leafbox.GetClosestPointSeed(ef.P,&t);

  if ( 2 == bez->m_order && !bez->m_is_rat )
  {
    Q = bez->PointAt(t);
    w = Q.DistanceTo(ef.P);
    if ( ef.d < 0.0 || w < ef.d )
    {
      ef.d = w;
      ef.s = t;
      ef.t = node->m_domain.ParameterAt(ef.s);
      ef.node = node;
      return 1;
    }
    return 0;
  }

  if ( ef.b.m_dim != bez->m_dim || ef.b.m_is_rat != bez->m_is_rat || ef.b.m_order != bez->m_order )
  {
    ef.b.m_dim = bez->m_dim;
    ef.b.m_is_rat = bez->m_is_rat;
    ef.b.m_cv_stride = bez->m_cv_stride;
    ef.b.m_order = bez->m_order;
    ef.b.ReserveCVCapacity(ef.b.m_cv_stride*ef.b.m_order);
  }
  double *cv = ef.b.m_cv;
  memcpy( cv, bez->m_cv, ef.b.m_cv_stride*ef.b.m_order*sizeof(*cv));
  i = ef.b.m_order;
  if ( ef.b.m_is_rat )
  {
    while(i--)
    {
      w = cv[3];
      x = 1.0/w;
      cv[0] = w*(x*cv[0] - ef.P.x);
      cv[1] = w*(x*cv[1] - ef.P.y);
      cv[2] = w*(x*cv[2] - ef.P.z);
      cv += 4;
    }
  }
  else
  {
    while(i--)
    {
      *cv++ -= ef.P.x;
      *cv++ -= ef.P.y;
      *cv++ -= ef.P.z;
    }
  }

  i = TL_EvBezierLocalExtremum(
    0, // means look for a local min
    ef.b.m_dim, ef.b.m_is_rat, ef.b.m_order,
    ef.b.m_cv,
    0.0,1.0,  // search entire bezier from 0.0 <= s <= 1.0
    0.0,      // stop search if a distance of 0.0 is found
    0.5*ON_SQRT_EPSILON, // step tol
    &t);

  Q = bez->PointAt(t);
  x = Q.DistanceTo(ef.P);

  if ( x < ef.d || ef.d < 0.0 )
  {
    ef.s = t;
    ef.t = node->m_domain.ParameterAt(ef.s);
    ef.d = x;
    ef.node = node;
    i = 1; 
  }
  else
  {
    i = 0;
  }
  return i;
}

class CGooseEv : public IwEvalFunctionObject
{
public:
  const ON_CurveTreeBezier& m_bez;
  const IwPoint3d m_crTestPoint;
  double m_dSquaredTolerance;  
  IwSolverOperationType m_eSolverOperation;
  IwBoolean m_bHaveScale;
  ULONG m_lNumScale;

  CGooseEv(const EF& ef,  const ON_CurveTreeNode* node,
        double dSquaredTolerance, IwSolverOperationType eSolverOperation) 
        : m_bez(*node->m_bez),
          m_crTestPoint(ef.P.x,ef.P.y,ef.P.z), 
        m_dSquaredTolerance(dSquaredTolerance), 
        m_eSolverOperation(eSolverOperation),
        m_bHaveScale(false), m_lNumScale(0) {}

  IwStatus Evaluate(double dT, 
                    double & rdFOfT, 
                    double & rdFPrimeOfT,
                    IwBoolean & rbFoundAnswer);
};

IwStatus CGooseEv::Evaluate(double dT, 
                            double & rdFOfT, 
                            double & rdFPrimeOfT,
                            IwBoolean & rbFoundAnswer) 
{
    rbFoundAnswer = false;
    IwPoint3d sPVV[3];

    // put pt, 1rst, 2nd der in sPVV
    m_bez.Evaluate(dT,2,3,&sPVV[0].x);
    //SER(m_crCurve.Evaluate(dT,2,true,sPVV));

    IwVector3d sVec = sPVV[0] - m_crTestPoint;

    // Scale the problem down to a reasonable size (0-100)
    if (!m_bHaveScale) {
        while (sPVV[1].LengthSquared() > 1.0e4) {
            m_lNumScale ++;
            sVec = sVec / 10.0;
            sPVV[0] = sPVV[0] / 10.0;
            sPVV[1] = sPVV[1] / 10.0;
            sPVV[2] = sPVV[2] / 10.0;
        }
        m_bHaveScale = true;
    }
    else {
        for (ULONG kk=0; kk<m_lNumScale; kk++) {
            sVec = sVec / 10.0;
            sPVV[0] = sPVV[0] / 10.0;
            sPVV[1] = sPVV[1] / 10.0;
            sPVV[2] = sPVV[2] / 10.0;
        }
    }

    double sTanLenSq = sPVV[1].LengthSquared();
    // if we have a zero length tangent then we must fail
    if (sTanLenSq < IW_EFF_ZERO_SQ) {
        return IW_ERR;
    }

    rdFOfT = sVec.Dot(sPVV[1]);
    rdFPrimeOfT = sPVV[2].Dot(sVec) + sPVV[1].Dot(sPVV[1]);
    if (m_eSolverOperation == IW_SO_MINIMIZE) {
        // Let sign of FOfT rule
        rdFPrimeOfT = iwos_Fabs(rdFPrimeOfT);
    }
    else if (m_eSolverOperation == IW_SO_MAXIMIZE) {
        // Let - sign of FofT rule
        rdFPrimeOfT = - iwos_Fabs(rdFPrimeOfT);
    }
    
    // Now test to see if geometric tolerance satisfied
    double sVecLenSq = sVec.LengthSquared();
    if (sVecLenSq < m_dSquaredTolerance) {
        if (m_eSolverOperation == IW_SO_MAXIMIZE) {
            return IW_ERR;
        }
        rbFoundAnswer = true;
        return IW_SUCCESS;
    }

    // Find square of projection of Vec onto the Tangent of curve.
    // Test this to see if we satisfy our tolerance
    double dProjectionDistanceSquared = (rdFOfT * rdFOfT) / (sTanLenSq * sVecLenSq);
    if (dProjectionDistanceSquared < m_dSquaredTolerance) {
        rbFoundAnswer = true;
        return IW_SUCCESS;
    }

    return IW_SUCCESS;
}

int ClosestForkHelper_hybrid( EF& ef, const ON_CurveTreeNode* node )
{
  double t;

  node->m_bez->m_leafbox.GetClosestPointSeed(ef.P,&t);

  CGooseEv sEFO(ef,node,IW_EFF_ZERO_SQ,IW_SO_MINIMIZE);
  IwLocalSolve1d sLS(sEFO, IwExtent1d(0.0,1.0), false);
  IwBoolean bFoundSolution;
  double dFoundT;
  if (sLS.SolveIt(t,1.0e-8,bFoundSolution,dFoundT) != IW_SUCCESS) 
  {
    // Goose failed - use foolproof method.
    ClosestForkHelper_dbrent(ef,node);
  }
  else
  {
    ON_3dPoint Q = node->m_bez->PointAt(dFoundT);
    double d = Q.DistanceTo(ef.P);
    if ( ef.d < 0.0 || d < ef.d )
    {
      ef.s = dFoundT;
      ef.t = node->m_domain.ParameterAt(ef.s);
      ef.d = d;
      ef.node = node;
    }
  }

  return 1;
}

static bool ClosestFork( EF& ef, double d, ON_CurveTreeNode* node )
{
  double downd[2];
  int i;
  bool rc = false;

  if ( ef.d > 0.0 )
  {
    if ( d < 0.0 )
    {
      d = node->MinimumDistanceLowerBound(ef.P);
    }
    if ( d > ef.d )
      return false;
  }

  if ( node->m_down[0] )
  {
    if ( node->m_down[1] )
    {
      downd[0] = node->m_down[0]->MinimumDistanceLowerBound(ef.P);
      downd[1] = node->m_down[1]->MinimumDistanceLowerBound(ef.P);
      i = ( downd[0] <= downd[1] ) ? 0 : 1;
      if ( ef.d < 0.0 || downd[i] < ef.d )
      {
        rc = ClosestFork( ef, downd[i], node->m_down[i] );
        if ( ef.d < 0.0 || downd[1-i] < ef.d )
        {
          if ( ClosestFork( ef, downd[1-i], node->m_down[1-i] ) )
          {
            rc = true;
          }
        }
      }
    }
    else
    {
      rc = ClosestFork( ef, ON_UNSET_VALUE, node->m_down[0] );
    }
  }
  else if ( node->m_down[1] )
  {
    rc = ClosestFork( ef, ON_UNSET_VALUE, node->m_down[1] );
  }
  else if ( node->m_bez )
  {
    // it's a leaf
    if ( ClosestForkHelper_hybrid(ef,node) )
    {
      rc = true;
    }
  }

  return rc;
}

class CPolishUp : public ON_NurbsCurve
{
public:
  ON_3dPoint m_P;
};

static void fff(void* farg,double t,double* x,double* dx)
{
  ON_3dVector V[3];
  double d;
  CPolishUp* p = (CPolishUp*)farg;

  ON_EvaluateNurbsSpan( 
     p->m_dim, p->m_is_rat, p->m_order, 
     p->m_knot,
     p->m_cv_stride, p->m_cv,
     dx?2:1, //der_count, 
     t,
     3, //v_stride, 
     &V[0].x 
     );

  V[0].x -= p->m_P.x;
  V[0].y -= p->m_P.y;
  V[0].z -= p->m_P.z;

  d = V[0].x*V[1].x + V[0].y*V[1].y + V[0].z*V[1].z;
  *x = d;
  if ( dx )
  {
    *dx = V[0].x*V[2].x + V[0].y*V[2].y + V[0].z*V[2].z -(V[1].x*V[1].x + V[1].y*V[1].y + V[1].z*V[1].z);
  }
}

class CCurveTestPoint
{
public:
  int test_point_index;

  ON_3dPoint P;  // test point = C + d*(variable unit vector)
  double t;
  double d;

  // The information below is filled in during accuracy testing phase

  enum FUNC_CRV_CLSPT
  {
    FUNC_CRV_CLSPT_NONE        = 0,
    FUNC_CRV_CLSPT_BASELINE    = 1,  // Dale's new tree with hybrid newton/debrent local solve
    FUNC_CRV_CLSPT_ON_TREETEST = 2,  // Used to determine where code is spending time
    FUNC_CRV_CLSPT_ON_CRV      = 3,  // Generic ON_Curve::GetClosestPoint()
    FUNC_CRV_CLSPT_ON_NURBS    = 4,  // OpenNurbs ON_NurbsCurve::GetClosestPoint()
    FUNC_CRV_CLSPT_GOOSE       = 5,  // old gslib 
    FUNC_CRV_CLSPT_COUNT       = 6
  };

  // paramter of closest point
  double crv_t[FUNC_CRV_CLSPT_COUNT];

  // distance from P to 
  double crv_d[FUNC_CRV_CLSPT_COUNT];

  // best result from a test
  double best_t;
  double best_d;
};


void GetCurvePointCloud( const ON_Curve& curve, 
                         double sample_start_distance,
                         double sample_stop_distance,
                         ON_SimpleArray<CCurveTestPoint>& CPT )
{
  // appends a cloud of points to pts.
  int hint = 0;
  int i,j,k,n,m;

  ON_Plane plane;
  ON_3dPoint P;
  ON_3dVector T;
  ON_Interval span_domain;
  double r, a, t;

  if ( sample_start_distance >= sample_stop_distance )
    sample_start_distance = sample_stop_distance;

  int span_count = curve.SpanCount();
  if (span_count < 1 )
  {
    return;
  }
  double* span_vector = (double*)onmalloc((span_count+1)*sizeof(*span_vector));
  if ( !curve.GetSpanVector(span_vector))
    return;

  int degree = curve.Degree();

  int span_sample_count = 4*3*5*7;
  if ( sample_stop_distance <= 0.0 )
  {
    k = 1;
  }
  else
  {
    k = 0;
    for ( r = sample_start_distance; r <= sample_stop_distance*(1.0+ON_SQRT_EPSILON); r *= 2.0 )
    {
      k++;
    }
    if ( k < 1 )
      k = 1;
    k *= 4;
  }


  while ( span_count*span_sample_count*k < 30000 )
  {
    span_sample_count *= 2;
  }

  if ( degree <= 2 && span_count <= 4 )
  {
    while ( span_count*span_sample_count*k < 60000 )
      span_sample_count *= 2;
  }

  if ( bPurify )
  {
    // reduce sample counts so purify will finish somtime today
    span_sample_count = 3;
    sample_stop_distance = sample_start_distance;
  }

  CPT.Reserve(span_count*span_sample_count*17);

  for (i = 0; i < span_count; i++ )
  {
    span_domain.Set(span_vector[i],span_vector[i+1]);
    for ( j = i?0:1; j <= span_sample_count; j++ )
    {
      t = span_domain.ParameterAt(((double)j)/((double)span_sample_count));
      curve.EvTangent(t,P,T,1,&hint);
      plane.CreateFromNormal(P,T);

      if ( sample_stop_distance <= 0.0 )
      {
        CCurveTestPoint& CP = CPT.AppendNew();
        CP.test_point_index = CPT.Count()-1;
        CP.P = P;
        CP.t = t;
        CP.d = 0.0;
        CP.best_t = ON_UNSET_VALUE;
        CP.best_d = ON_UNSET_VALUE;
        for( m = 0; m < CCurveTestPoint::FUNC_CRV_CLSPT_COUNT; m++ )
        {
          CP.crv_d[m] = ON_UNSET_VALUE;
          CP.crv_t[m] = ON_UNSET_VALUE;
        }
      }
      else
      {
        k = 0;
        for ( r = sample_start_distance; r <= sample_stop_distance*(1.0+ON_SQRT_EPSILON); r *= 2.0 )
        {
          for ( n = 0; n <= 4; n++ )
          {
            CCurveTestPoint& CP = CPT.AppendNew();
            CP.test_point_index = CPT.Count()-1;
            a = (k/6.0 + n*0.5)*ON_PI;
            CP.P = plane.origin + r*(cos(a)*plane.xaxis + sin(a)*plane.yaxis);
            CP.t = t;
            CP.d = P.DistanceTo(CP.P);
            CP.best_t = ON_UNSET_VALUE;
            CP.best_d = ON_UNSET_VALUE;
            for( m = 0; m < CCurveTestPoint::FUNC_CRV_CLSPT_COUNT; m++ )
            {
              CP.crv_d[m] = ON_UNSET_VALUE;
              CP.crv_t[m] = ON_UNSET_VALUE;
            }
            if ( 0.0 == r )
              break;
          }
          k++;
        }
      }
    }
  }  
  onfree(span_vector);

  // FOR DEBUGGING A CLOSEST POINT TO CURVE BAD POINT
  int singleton_index = -1;
  if ( singleton_index >= 0 && singleton_index < CPT.Count() )
  {
    CPT[0] = CPT[singleton_index];
    CPT.SetCount(1);
  }
}

class CClosestPointToCurveTestResults
{
public:
  bool m_bSpeedTest;
  bool m_bAccuracyTest;
  double m_elapsed_time;
  double m_speed_factor; // =m_elapsed_time/best_time. 1 = best, > 1 means slower than the best

  double m_on_curve_3d_error;  // average absolute 3d distance error for points on the curve
  double m_on_curve_t_error;   // relative t error for points on the curve

  double m_off_curve_3d_error; // average relative error for points away from the curve
  double m_off_curve_t_error;   // relative t error for points on the curve

  int m_test_count;     // total number of tests
  int m_failure_count;  // number of failures to return any answer

  int m_error_count; // number of calls to ON_Error
  int m_warning_count; // number of calls to ON_Warning

  int m_on_test_count;
  int m_on_best_count;
  int m_on_sloppy_count;   // number of sloppy answers for points on the curve
  int m_on_wrong_count;    // number of wrong answers for points on the curve

  int m_off_test_count; 
  int m_off_best_count;     // number of times this got the best answer
  int m_off_sloppy_count;   // number of sloppy answers for points off of the curve
  int m_off_wrong_count;    // number of wrong answers for points off of the curve

};

class CClosestPointToSurfaceTestResults
{
public:
  bool m_bSpeedTest;
  bool m_bAccuracyTest;
  double m_elapsed_time;
  double m_speed_factor; // =m_elapsed_time/best_time. 1 = best, > 1 means slower than the best

  double m_on_surface_3d_error;  // average absolute 3d distance error for points on the surface
  ON_2dVector m_on_surface_uv_error;   // relative t error for points on the surface

  double m_off_surface_3d_error; // average relative error for points away from the surface
  ON_2dVector m_off_surface_uv_error;   // relative t error for points on the surface

  int m_test_count;     // total number of tests
  int m_failure_count;  // number of failures to return any answer

  int m_error_count; // number of calls to ON_Error
  int m_warning_count; // number of calls to ON_Warning

  int m_on_test_count;
  int m_on_best_count;
  int m_on_sloppy_count;   // number of sloppy answers for points on the surface
  int m_on_wrong_count;    // number of wrong answers for points on the surface

  int m_off_test_count; 
  int m_off_best_count;     // number of times this got the best answer
  int m_off_sloppy_count;   // number of sloppy answers for points off of the surface
  int m_off_wrong_count;    // number of wrong answers for points off of the surface

};

class CClosestPointToMeshTestResults
{
public:
  bool m_bSpeedTest;
  bool m_bAccuracyTest;
  double m_mesh_elapsed_time;
  double m_surface_elapsed_time;
  double m_speed_factor; // =m_mesh_elapsed_time/m_surface_elapsed_time; (bigger = slower)

  double m_on_mesh_3d_error;  // average absolute 3d distance error for points on the mesh

  double m_off_mesh_3d_error; // average relative error for points away from the mesh

  int m_test_count;     // total number of tests
  int m_failure_count;  // number of failures to return any answer

  int m_error_count; // number of calls to ON_Error
  int m_warning_count; // number of calls to ON_Warning

  int m_on_test_count;
  int m_on_best_count;
  int m_on_sloppy_count;   // number of sloppy answers for points on the mesh
  int m_on_wrong_count;    // number of wrong answers for points on the mesh

  int m_off_test_count; 
  int m_off_best_count;     // number of times this got the best answer
  int m_off_sloppy_count;   // number of sloppy answers for points off of the mesh
  int m_off_wrong_count;    // number of wrong answers for points off of the mesh
};


class CClosestPointToCurveTest
{
public:
  virtual void Setup( const ON_Curve* curve ) = 0;

  virtual int GetClosestPoint( ON_3dPoint P, double* t ) = 0;

  // returns time in seconds
  double SpeedTest( int test_point_count, const CCurveTestPoint* tp);

  // returns error count
  int AccuracyTest( int test_point_count, CCurveTestPoint* tp );

  void CalculateResults( const ON_Curve& curve, 
                         int test_point_count, 
                         const CCurveTestPoint* tp, 
                         double best_time 
                         );

  void Print( ON_TextLog& text_log, int test_count );

  CCurveTestPoint::FUNC_CRV_CLSPT m_func;

  //const ON_NurbsCurve* m_nurbs_curve;
  const ON_Curve* m_curve;

  CClosestPointToCurveTestResults m_results;

  void SetupHelper( CCurveTestPoint::FUNC_CRV_CLSPT func, const ON_Curve* curve );
};

void CClosestPointToCurveTest::SetupHelper( CCurveTestPoint::FUNC_CRV_CLSPT func, const ON_Curve* curve )
{
  m_func = func;
  m_curve = curve;
  memset(&m_results,0,sizeof(m_results));
  m_results.m_elapsed_time = ON_UNSET_VALUE;
}

double CClosestPointToCurveTest::SpeedTest(int point_count, const CCurveTestPoint* tp)
{
  double t;
  int i;
  clock_t time0, time1;

  int error_count0 = ON_GetErrorCount();
  int warning_count0 = ON_GetWarningCount();

  i = point_count;
  time0 = clock();
  while(i--)
  {
    t = ON_UNSET_VALUE;
    GetClosestPoint(tp->P,&t);
    tp++;
  }
  time1 = clock();

  m_results.m_elapsed_time = ((double)(time1 - time0))/((double)CLOCKS_PER_SEC);

  m_results.m_bSpeedTest = true;

  m_results.m_error_count += (ON_GetErrorCount()-error_count0);
  m_results.m_warning_count += (ON_GetWarningCount()-warning_count0);

  return m_results.m_elapsed_time;
}

int CClosestPointToCurveTest::AccuracyTest(int point_count, CCurveTestPoint* tp)
{
  ON_3dPoint Q;
  double d;
  int rc = 0;
  int i;
  double t;

  int error_count0 = ON_GetErrorCount();
  int warning_count0 = ON_GetWarningCount();

  m_results.m_bAccuracyTest = true;
  m_results.m_test_count = point_count;

  for ( i = 0; i < point_count; i++ )
  {
    t = ON_UNSET_VALUE;
    if( !GetClosestPoint(tp[i].P,&t) )
    {
      rc++;
      tp[i].crv_t[m_func] = ON_UNSET_VALUE;
      tp[i].crv_d[m_func] = ON_UNSET_VALUE;
      m_results.m_failure_count++;
    }
    else
    {
      Q = m_curve->PointAt(t);
      d = Q.DistanceTo(tp[i].P);
      tp[i].crv_t[m_func] = t;
      tp[i].crv_d[m_func] = d;
      if ( ON_UNSET_VALUE == tp[i].best_d || d < tp[i].best_d )
      {
        tp[i].best_d = d;
        tp[i].best_t = t;
      }
    }
  }

  m_results.m_error_count += (ON_GetErrorCount()-error_count0);
  m_results.m_warning_count += (ON_GetWarningCount()-warning_count0);

  return rc;
}


void CClosestPointToCurveTest::CalculateResults( const ON_Curve& curve,
                                          int test_point_count,
                                          const CCurveTestPoint* tp,
                                          double best_time
                                          )
{
  int badi = -1;
  double badd = 0.0;
  m_results.m_speed_factor = (m_results.m_bSpeedTest && best_time>0.0) ? m_results.m_elapsed_time/best_time : 0.0;
  if ( m_results.m_bAccuracyTest )
  {
    ON_Sum on_3d_error;
    ON_Sum off_3d_error;
    ON_Sum on_t_error;
    ON_Sum off_t_error;
    double d_err, t_err;
    int i;
    int hint = 0;
    ON_3dPoint P;
    ON_3dVector D;
    for ( i = 0; i < test_point_count; i++ )
    {
      const CCurveTestPoint& T = tp[i];

      if ( !ON_IsValid(T.crv_t[m_func]) )
        continue;

      curve.Ev1Der( T.best_t, P, D, 1, &hint );

      t_err = D.Length();
      t_err = ( t_err > ON_ZERO_TOLERANCE ) ? 1.0/t_err : 0.0;
      t_err *= fabs(T.crv_t[m_func] - T.best_t);

      d_err = (T.best_d < T.d) ? T.best_d : T.d;

      d_err = T.crv_d[m_func] - d_err;
      if ( 0.0 == T.d || 0.0 == T.best_d )
      {
        m_results.m_on_test_count++;

        if (  T.crv_d[m_func] <= T.best_d )
        {
          m_results.m_on_best_count++;
          t_err = 0.0;
        }
        if ( d_err > 1.0e-4 )
        {
          m_results.m_on_wrong_count++;
          if ( CCurveTestPoint::FUNC_CRV_CLSPT_GOOSE != m_func && d_err > badd )
          {
            badd = d_err;
            badi = T.test_point_index;
          }
        }
        else if ( d_err > 1.0e-6 )
        {
          m_results.m_on_sloppy_count++;
        }
        else
        {
          on_3d_error.Plus(d_err);
          on_t_error.Plus(t_err);
        }
      }
      else if ( T.best_d > 0.0 )
      {
        m_results.m_off_test_count++;

        if( T.crv_d[m_func] <= T.best_d )
        {
          m_results.m_off_best_count++;
          t_err = 0.0;
        }
        d_err /= T.best_d;
        if ( d_err > 0.15 )
        {
          m_results.m_off_wrong_count++;
        }
        else if ( d_err > 0.05 )
        {
          m_results.m_off_sloppy_count++;
        }
        else
        {
          off_3d_error.Plus(d_err);
          off_t_error.Plus(t_err);
        }
      }
    }

    i = on_3d_error.SummandCount();
    if ( i > 0 )
    {
      d_err = on_3d_error.Total()/((double)i);
      t_err = on_t_error.Total()/((double)i);
      m_results.m_on_curve_3d_error = d_err;
      m_results.m_on_curve_t_error = t_err;
    }

    i = off_3d_error.SummandCount();
    if ( i > 0 )
    {
      d_err = off_3d_error.Total()/((double)i);
      t_err = off_t_error.Total()/((double)i);
      m_results.m_off_curve_3d_error = d_err;
      m_results.m_off_curve_t_error = t_err;
    }
  }

  if ( badi>=0) 
  {
    printf("TEST POINT %d had error of = %g.  This is a ClosestPointToCurve BUG.\n",badi,badd);
  }
}

void CClosestPointToCurveTest::Print( ON_TextLog& text_log, int test_count )
{
  const char* format = "%s %s %5d  %4.1fX (%6.3f secs) %3d%%  %.1g  %.1g";

  const char* name = 0;
  if ( test_count > 0 )
  {
    text_log.Print("Name       Test  Point      Speed           Accuracy\n");
    text_log.Print("                 count  sloth   time        best  3d err  t err\n");
    name = "Perfect    ";
  }
  else
  {
    switch(m_func)
    {
    case CCurveTestPoint::FUNC_CRV_CLSPT_BASELINE:
      name = "Baseline   ";
      break;
    case CCurveTestPoint::FUNC_CRV_CLSPT_ON_TREETEST:
      name = "Tree Test  ";
      break;
    case CCurveTestPoint::FUNC_CRV_CLSPT_ON_CRV:
      name = "ON_Curve   ";
      break;
    case CCurveTestPoint::FUNC_CRV_CLSPT_ON_NURBS:
      name = "NURBS form ";
      break;
    case CCurveTestPoint::FUNC_CRV_CLSPT_GOOSE:
      name = "Goose      ";
      break;
    default:
      name = "Anonymous  ";
      break;
    }
  }


  if ( test_count > 0 )
  {
    text_log.Print(format,
      name,
      "    ",
      test_count,    // m_results.m_test_count,
      1.0,  //m_results.m_speed_factor,
      0.0,  //m_results.m_elapsed_time,
      100,//test_count,    //m_results.m_best_count,
      1e-18,  //m_results.m_on_curve_3d_error,
      1e-18,  //m_results.m_on_curve_t_error,
      0,    //m_results.m_on_wrong_count,
      0     //m_results.m_on_sloppy_count
      );
    text_log.Print("\n");
  }
  else
  {
    if ( m_results.m_on_test_count > 0 )
    {
      int best = (int)floor(100.0*((double)m_results.m_on_best_count)/((double)m_results.m_test_count));
      text_log.Print(format,
        name,
        "on  ",
        m_results.m_on_test_count,
        m_results.m_speed_factor,
        m_results.m_elapsed_time,
        best,//m_results.m_on_best_count,
        m_results.m_on_curve_3d_error,
        m_results.m_on_curve_t_error
        );
      if( m_results.m_failure_count > 0 )
        text_log.Print(" %d FAILURES",m_results.m_failure_count);

      if( m_results.m_error_count > 0 )
        text_log.Print(" %d ON_ERRORs",m_results.m_error_count);
      if( m_results.m_warning_count > 0 )
        text_log.Print(" %d ON_WARNINGSs",m_results.m_warning_count);

      if( m_results.m_on_wrong_count > 0 )
        text_log.Print(" %d WRONG ANSWERS",m_results.m_on_wrong_count);
      if( m_results.m_on_sloppy_count > 0 )
        text_log.Print(" %d sloppys answers",m_results.m_on_sloppy_count);
      text_log.Print("\n");
    }

    if ( m_results.m_off_test_count > 0 )
    {
      int best = (int)floor(100.0*((double)m_results.m_off_best_count)/((double)m_results.m_test_count));
      text_log.Print(format,
        name,
        "off ",
        m_results.m_off_test_count,
        m_results.m_speed_factor,
        m_results.m_elapsed_time,
        best, //m_results.m_off_best_count,
        m_results.m_off_curve_3d_error,
        m_results.m_off_curve_t_error
        );
      if( m_results.m_failure_count > 0 )
        text_log.Print(" %d FAILURES",m_results.m_failure_count);

      if( m_results.m_error_count > 0 )
        text_log.Print(" %d ON_ERRORs",m_results.m_error_count);
      if( m_results.m_warning_count > 0 )
        text_log.Print(" %d ON_WARNINGSs",m_results.m_warning_count);

      if( m_results.m_off_wrong_count > 0 )
        text_log.Print(" %d WRONG ANSWERS",m_results.m_off_wrong_count);
      if( m_results.m_off_sloppy_count > 0 )
        text_log.Print(" %d sloppys answers",m_results.m_off_sloppy_count);
      text_log.Print("\n");
    }
  }
}



class CSurfaceTestPoint
{
public:
  int test_point_index;

  ON_3dPoint P;  // test point = C + d*(variable unit vector)
  ON_2dPoint uv;
  double d;

  // The information below is filled in during accuracy testing phase

  enum FUNC_SRF_CLSPT
  {
    FUNC_SRF_CLSPT_NONE  = 0,
    FUNC_SRF_CLSPT_NEWON = 1,  // 
    FUNC_SRF_CLSPT_GOOSE = 2,  // old gslib 
    FUNC_SRF_CLSPT_COUNT = 3
  };

  // paramter of closest point
  ON_2dPoint srf_uv[FUNC_SRF_CLSPT_COUNT];

  // distance from P to 
  double srf_d[FUNC_SRF_CLSPT_COUNT];

  // best result from a test
  ON_2dPoint best_uv;
  double best_d;
};




class CMeshTestPoint
{
public:
  int test_point_index;

  ON_MESH_POINT S; // seed point on mesh
  ON_3dVector N;   // normal for offsets
  ON_3dPoint P;    // test point

  ON_MESH_POINT M; // best answer we found

  double S_d; // distance from S to P.
  double M_d; // distance from M to P.
};

ON_3dPoint FacePoint( const ON_MESH_POINT& S )
{
  ON_3dPoint P(ON_UNSET_VALUE,ON_UNSET_VALUE,ON_UNSET_VALUE);
  if ( S.m_mesh )
  {
    if ( S.m_face_index >= 0 && S.m_face_index < S.m_mesh->m_F.Count() )
    {
      ON_3dPoint V[4];
      const int* fvi = S.m_mesh->m_F[S.m_face_index].vi;
      const ON_3fPoint* mesh_V = S.m_mesh->m_V.Array();
      V[0] = mesh_V[fvi[0]];
      V[1] = mesh_V[fvi[1]];
      V[2] = mesh_V[fvi[2]];
      V[3] = mesh_V[fvi[3]];
      const double* t = S.m_t;
      P.x = t[0]*V[0].x + t[1]*V[1].x + t[2]*V[2].x + t[3]*V[3].x;
      P.y = t[0]*V[0].y + t[1]*V[1].y + t[2]*V[2].y + t[3]*V[3].y;
      P.z = t[0]*V[0].z + t[1]*V[1].z + t[2]*V[2].z + t[3]*V[3].z;
    }
  }
  return P;
}

static
bool GetMeshPointCloudVertexHelper( 
             const ON_Mesh& mesh,
             const ON_MeshTopology& top,
             int topvi,
             ON_SimpleArray<CMeshTestPoint>& CPT
             )
{
  const ON_MeshTopologyVertex& V = top.m_topv[topvi];
  if ( V.m_tope_count < 1 || V.m_v_count < 1 )
    return false;

  const ON_MeshTopologyEdge& E = top.m_tope[V.m_topei[0]];
  if ( E.m_topf_count < 1 )
    return false;

  int fi = E.m_topfi[0];
  const int* fvi = mesh.m_F[fi].vi;
  int j;
  for ( j = 0; j < 4; j++ )
  {
    int vi = fvi[j];
    if ( top.m_topv_map[vi] == topvi )
    {
      CMeshTestPoint& tp = CPT.AppendNew();
      tp.test_point_index = CPT.Count()-1;
      tp.N = mesh.m_N[vi];
      tp.S.m_P = mesh.m_V[vi];
      tp.S.m_face_index = fi;
      tp.S.m_t[j] = 1.0;
      tp.S.m_mesh = &mesh;
      return true;
    }
  }
  return false;
}


static
bool GetMeshPointCloudEdgeHelper( 
             const ON_Mesh& mesh,
             const ON_MeshTopology& top,
             int topei,
             int edge_sample_count,
             ON_SimpleArray<CMeshTestPoint>& CPT
             )
{
  const ON_MeshTopologyEdge& E = top.m_tope[topei];
  if ( E.m_topf_count < 1 )
    return false;
  const int fi = E.m_topfi[0];
  const ON_MeshTopologyFace& F0 = top.m_topf[fi];
  int fei;
  for ( fei = 0; fei < 4; fei++ )
  {
    if ( F0.m_topei[fei] == topei )
    {
      int fvi0 = ( F0.IsTriangle() ) 
               ? ((fei+2)%3) 
               : ((fei+3)%4);
      int fvi1 = fei;
      const int* fvi = mesh.m_F[fi].vi;
      ON_Line L;
      L.from = mesh.m_V[fvi[fvi0]];
      L.to   = mesh.m_V[fvi[fvi1]];

      ON_3dVector edgeN = mesh.m_FN[fi];
      int j;
      for ( j = 1; j < E.m_topf_count; j++ )
      {
        edgeN = edgeN + ON_3dVector(mesh.m_FN[E.m_topfi[j]]);
      }
      edgeN.Unitize();

      for ( j = 1; j <= edge_sample_count; j++ )
      {
        double t = ((double)j)/((double)(edge_sample_count+1));
        CMeshTestPoint& CP = CPT.AppendNew();
        CP.test_point_index = CPT.Count()-1;
        CP.N = edgeN;
        CP.S.m_P = L.PointAt(t);
        CP.S.m_face_index = fi;
        CP.S.m_t[fvi0] = 1.0-t;
        CP.S.m_t[fvi1] = t;
        CP.S.m_mesh = &mesh;
      }
      return true;
    }
  }
  return false;
}

static
bool GetMeshPointCloudFaceHelper( 
             const ON_Mesh& mesh,
             const ON_MeshTopology& top,
             int fi,
             int face_sample_count,
             int edge_sample_count,
             ON_SimpleArray<CMeshTestPoint>& CPT
             )
{
  ON_MeshFace f = mesh.m_F[fi];
  ON_3dVector N = mesh.m_FN[fi];
  ON_3dPoint V[4];
  int tri[2][3];
  int edge[2];
  V[0] = mesh.m_V[f.vi[0]];
  V[1] = mesh.m_V[f.vi[1]];
  V[2] = mesh.m_V[f.vi[2]];
  V[3] = mesh.m_V[f.vi[3]];

  int tricount;
  if ( f.IsTriangle() )
  {
    tri[0][0] = tri[1][0] = 0;
    tri[0][1] = tri[1][1] = 1;
    tri[0][2] = tri[1][2] = 2;
    tricount = 1;
    edge[0] = edge[1] = -1;
  }
  else
  {
    tricount = 2;
    double d0 = V[0].DistanceTo(V[2]);
    double d1 = V[1].DistanceTo(V[3]);
    if ( d0 <= d1 )
    {
      tri[0][0] = 0;
      tri[0][1] = 1;
      tri[0][2] = 2;
      tri[1][0] = 0;
      tri[1][1] = 2;
      tri[1][2] = 3;
      edge[0] = 0;
      edge[1] = 2;
    }
    else
    {
      tri[0][0] = 0;
      tri[0][1] = 1;
      tri[0][2] = 3;
      tri[1][0] = 1;
      tri[1][1] = 2;
      tri[1][2] = 3;
      edge[0] = 1;
      edge[1] = 3;
    }
  }

  double r, s, t;

  int i, a, b, n;

  for( n = 3; (n-2)*(n-1) < face_sample_count*2; n++ )
  {
    // empty
  }


  for ( i = 0; i < tricount; i++ )
  {
    for ( a = 1; a < n-1; a++ )
    {
      for ( b = 1; a+b < n; b++ )
      {
        r = ((double)a)/((double)n);
        s = ((double)b)/((double)n);
        t = 1.0-r-s;
        CMeshTestPoint& tp = CPT.AppendNew();
        tp.test_point_index = CPT.Count()-1;
        tp.N = N;
        tp.S.m_t[tri[i][0]] = r;
        tp.S.m_t[tri[i][1]] = s;
        tp.S.m_t[tri[i][2]] = t;
        tp.S.m_face_index = fi;
        tp.S.m_mesh = &mesh;
        tp.S.m_P = r*V[tri[i][0]] + s*V[tri[i][1]] + t*V[tri[i][2]];
      }
    }
  }

  if ( edge[0] != edge[1] )
  {
    for ( i = 1; i <= edge_sample_count; i++ )
    {
      t = ((double)i)/((double)(edge_sample_count+1));
      s = 1.0-t;
      CMeshTestPoint& tp = CPT.AppendNew();
      tp.test_point_index = CPT.Count()-1;
      tp.N = N;
      tp.S.m_t[edge[0]] = s;
      tp.S.m_t[edge[1]] = t;
      tp.S.m_face_index = fi;
      tp.S.m_mesh = &mesh;
      tp.S.m_P = s*V[edge[0]] + t*V[edge[1]];
    }
  }
  return true;
}

void GetMeshPointCloud( const ON_Mesh& mesh, 
                         double sample_start_distance,
                         double sample_stop_distance,
                         ON_SimpleArray<CMeshTestPoint>& CPT )
{
  // appends a cloud of points to pts.
  const int i0 = CPT.Count();

  int i,n;
  double r;

  if ( sample_stop_distance < 0.0 )
    sample_stop_distance = 0.0;
  if ( sample_start_distance >= sample_stop_distance )
    sample_start_distance = sample_stop_distance;

  const ON_MeshTopology& top = mesh.Topology();
  const int face_count   = mesh.m_F.Count();
  const int vertex_count = top.m_topv.Count();
  const int edge_count   = top.m_tope.Count();
  const int quad_count   = mesh.QuadCount();
  const int triangle_count = face_count + quad_count;

  int face_n = 4;
  int triangle_sample_count = ((face_n-2)*(face_n-1))/2;
  int edge_sample_count = 1;
  int vertical_count = 0;
  if ( sample_start_distance > 0.0 )
  {
    for ( r = sample_start_distance; r <= sample_stop_distance*(1.0+ON_SQRT_EPSILON); r *= 2.0 )
    {
      if ( 0.0 != r )
        vertical_count += 2;
    }
  }
  if ( vertical_count < 1 )
    vertical_count = 1;

  int test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;

  while ( test_point_count < 1000 )
  {
    face_n++;
    triangle_sample_count = ((face_n-2)*(face_n-1))/2;
    if ( (face_n % 2) )
      edge_sample_count++;

    test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;

    if ( face_n >= 7 )
      break;
  }

  const int max_test_point_count = 750000;

  if ( test_point_count > max_test_point_count )
  {
    while ( edge_sample_count > 1 && test_point_count > max_test_point_count )
    {
      edge_sample_count--;
      test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;
    }
    while ( triangle_sample_count > 3 && test_point_count > max_test_point_count )
    {
      face_n--;
      triangle_sample_count = ((face_n-2)*(face_n-1))/2;
      test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;
    }
    while ( vertical_count > 2 )
    {
      sample_start_distance *= 2.0;
      vertical_count -= 2;
      test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;
    }
    while ( edge_sample_count > 0 && test_point_count > max_test_point_count )
    {
      edge_sample_count--;
      test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;
    }
    while ( triangle_sample_count > 1 && test_point_count > max_test_point_count )
    {
      face_n--;
      triangle_sample_count = ((face_n-2)*(face_n-1))/2;
      test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;
    }
  }

  if ( bPurify )
  {
    // reduce sample counts so purify will finish somtime today
    triangle_sample_count = 1;
    edge_sample_count = 0;
    sample_stop_distance = sample_start_distance;
    vertical_count = (sample_stop_distance>0.0) ? 2 : 1;
    test_point_count = (triangle_sample_count*triangle_count + edge_sample_count*(edge_count+quad_count) + vertex_count)*vertical_count;
  }

  CPT.Reserve( test_point_count );

  if ( !mesh.HasFaceNormals() )
  {
    const_cast<ON_Mesh*>(&mesh)->ComputeFaceNormals();
  }
  if ( !mesh.HasVertexNormals() )
  {
    const_cast<ON_Mesh*>(&mesh)->ComputeVertexNormals();
  }

  // get a test point at each vertex
  for ( i = 0; i < vertex_count; i++ )
  {
    GetMeshPointCloudVertexHelper( mesh, top, i, CPT );
  }  

  // get test points along each edge
  if ( edge_sample_count > 0 )
  {
    for ( i = 0; i < edge_count; i++ )
    {
      GetMeshPointCloudEdgeHelper( mesh, top, i, edge_sample_count, CPT );
    }
  }

  // get test points along each face
  if ( triangle_sample_count > 0 )
  {
    for (i = 0; i < face_count; i++)
    {
      GetMeshPointCloudFaceHelper( mesh, top, i, triangle_sample_count, edge_sample_count, CPT );
    }
  }

  CMeshTestPoint TP;
  const int i1 = CPT.Count();
  for ( i = i0; i < i1; i++ )
  {
    CMeshTestPoint& CP0 = CPT[i];
    CP0.P = FacePoint( CP0.S );
    CP0.S_d = CP0.P.DistanceTo(CP0.S.m_P);
    CP0.M_d = ON_UNSET_VALUE;
    ON_3dVector D = CP0.P - CP0.S.m_P;
    if (    fabs(D.x) >= (1.0+fabs(CP0.P.x))*ON_EPSILON
          && fabs(D.y) >= (1.0+fabs(CP0.P.y))*ON_EPSILON
          && fabs(D.z) >= (1.0+fabs(CP0.P.z))*ON_EPSILON )
    {
      ON_ERROR("GetMeshPointCloud - created bogus input values");
    }
    TP = CP0;

    bool bAppend = false;
    if ( 0.0 < sample_start_distance  )
    {
      int level_count;
      r = sample_start_distance;
      for ( level_count = 0; level_count < vertical_count; level_count += 2 )
      {
        for ( n = 0; n <= 1; n++ )
        {
          CMeshTestPoint& CP = bAppend ? CPT.AppendNew() : CP0;
          CP = TP;
          if ( bAppend )
            CP.test_point_index = CPT.Count()-1;
          CP.P = TP.P + (n?-r:r)*TP.N;
          CP.S_d = CP.S.m_P.DistanceTo(CP.P);
          bAppend = true;
        }
        r *= 2.0;
      }
    }
  }

  return;

}


void GetSurfacePointCloud( const ON_Surface& surface, 
                         double sample_start_distance,
                         double sample_stop_distance,
                         ON_SimpleArray<CSurfaceTestPoint>& CPT )
{
  // appends a cloud of points to pts.
  int hint[2] = {0,0};
  int j,jj,n,m,spani,spanj;

  ON_3dPoint P;
  ON_3dVector N;
  ON_Interval span_domain[2];
  double r, s, t;

  if ( sample_start_distance >= sample_stop_distance )
    sample_start_distance = sample_stop_distance;

  int span_count0 = surface.SpanCount(0);
  if (span_count0 < 1 )
  {
    return;
  }

  int span_count1 = surface.SpanCount(1);
  if (span_count1 < 1 )
  {
    return;
  }

  double* span_vector0 = (double*)onmalloc((span_count0+span_count1+2)*sizeof(*span_vector0));
  double* span_vector1 = span_vector0 + (span_count0+1);
  
  if ( !surface.GetSpanVector(0,span_vector0))
    return;

  if ( !surface.GetSpanVector(1,span_vector1))
    return;

  int degree0 = surface.Degree(0);
  int degree1 = surface.Degree(1);

  int span_sample_count = 4*3*5;
  int vertical_count = 1;
  if ( sample_stop_distance > 0.0 )
  {
    for ( r = sample_start_distance; r <= sample_stop_distance*(1.0+ON_SQRT_EPSILON); r *= 2.0 )
    {
      if ( 0.0 != r )
        vertical_count += 2;
    }
  }

  while ( span_count0*span_count1*span_sample_count*span_sample_count < 1000 )
    span_sample_count *= 2;

  if ( 1 == degree0 && 1 == degree1 && span_count0*span_count1*span_sample_count*span_sample_count < 10000 )
  {
    while ( span_count0*span_count1*span_sample_count*span_sample_count < 10000 )
      span_sample_count *= 2;
  }
  else if ( span_sample_count > 5 )
  {
    while ( span_sample_count > 5 && span_count0*span_count1*span_sample_count*span_sample_count > 1000000 )
    {
      span_sample_count /= 2;
    }
    while ( span_sample_count > 5 && span_count0*span_count1*span_sample_count*span_sample_count > 200000 )
    {
      span_sample_count *= 2;
      span_sample_count /= 3;
    }
  }

  if ( bPurify )
  {
    // reduce sample counts so purify will finish somtime today
    span_sample_count = 3;
    sample_stop_distance = sample_start_distance;
  }

  CPT.Reserve(span_count0*span_count1*span_sample_count*span_sample_count*vertical_count);

  for (spani = 0; spani < span_count0; spani++ )
  {
    span_domain[0].Set(span_vector0[spani],span_vector0[spani+1]);
    for ( j = spani?1:0; j <= span_sample_count; j++ )
    {
      s = span_domain[0].ParameterAt(((double)j)/((double)span_sample_count));
      for ( spanj = 0; spanj < span_count1; spanj++ )
      {
        span_domain[1].Set(span_vector1[spanj],span_vector1[spanj+1]);
        for ( jj = spanj?1:0; jj <= span_sample_count; jj++ )
        {
          t = span_domain[1].ParameterAt(((double)jj)/((double)span_sample_count));
          surface.EvNormal(s,t,P,N,1,hint);

          if ( sample_stop_distance <= 0.0 )
          {
            CSurfaceTestPoint& CP = CPT.AppendNew();
            CP.test_point_index = CPT.Count()-1;
            CP.P = P;
            CP.uv.Set(s,t);
            CP.d = 0.0;
            CP.best_uv.Set(ON_UNSET_VALUE,ON_UNSET_VALUE);
            CP.best_d = ON_UNSET_VALUE;
            for( m = 0; m < CSurfaceTestPoint::FUNC_SRF_CLSPT_COUNT; m++ )
            {
              CP.srf_d[m] = ON_UNSET_VALUE;
              CP.srf_uv[m].Set(ON_UNSET_VALUE,ON_UNSET_VALUE);
            }
          }
          else
          {
            for ( r = sample_start_distance; r <= sample_stop_distance*(1.0+ON_SQRT_EPSILON); r *= 2.0 )
            {
              for ( n = 0; n <= 1; n++ )
              {
                CSurfaceTestPoint& CP = CPT.AppendNew();
                CP.test_point_index = CPT.Count()-1;
                CP.P = P + (n?-r:r)*N;
                CP.uv.Set(s,t);
                CP.d = P.DistanceTo(CP.P);
                CP.best_uv.Set(ON_UNSET_VALUE,ON_UNSET_VALUE);
                CP.best_d = ON_UNSET_VALUE;
                for( m = 0; m < CSurfaceTestPoint::FUNC_SRF_CLSPT_COUNT; m++ )
                {
                  CP.srf_d[m] = ON_UNSET_VALUE;
                  CP.srf_uv[m].Set(ON_UNSET_VALUE,ON_UNSET_VALUE);
                }
                if ( 0.0 == r )
                  break;
              }
            }
          }
        }
      }
    }
  }  
  onfree(span_vector0);

  // FOR DEBUGGING A CLOSEST POINT TO SURFACE BAD POINT
  int singleton_index = -1;
  if ( singleton_index >= 0 && singleton_index < CPT.Count() )
  {
    CPT[0] = CPT[singleton_index];
    CPT.SetCount(1);
  }
}



class CClosestPointToMeshTest
{
public:
  bool GetClosestPoint( ON_3dPoint P, ON_MESH_POINT* mp );

  // returns time in seconds
  double SpeedTest( int test_point_count, const CMeshTestPoint* tp);

  // returns error count
  int AccuracyTest( int test_point_count, CMeshTestPoint* tp );

  void CalculateResults( int test_point_count, 
                         const CMeshTestPoint* tp
                         );

  void Print( ON_TextLog& text_log, int test_count );

  void SetupHelper( const ON_Mesh* mesh, const ON_Surface* surface );

  const ON_Mesh* m_mesh;
  const ON_MeshTree* m_mesh_tree;

  const ON_Surface* m_surface;
  const ON_SurfaceTree* m_surface_tree;

  CClosestPointToMeshTestResults m_results;
};

bool CClosestPointToMeshTest::GetClosestPoint( ON_3dPoint P, ON_MESH_POINT* mp )
{
  return m_mesh_tree->GetClosestPoint( P, mp );
}

void CClosestPointToMeshTest::SetupHelper( const ON_Mesh* mesh, const ON_Surface* surface )
{
  m_mesh = mesh;
  m_mesh_tree = m_mesh ? m_mesh->MeshTree() : 0;

  m_surface = surface;
  m_surface_tree = m_surface ? m_surface->SurfaceTree() : 0;

  memset(&m_results,0,sizeof(m_results));
}



double CClosestPointToMeshTest::SpeedTest(int point_count, const CMeshTestPoint* tp0)
{
  ON_MESH_POINT mp;
  const CMeshTestPoint* tp;;

  double s, t;
  int i;
  clock_t time0, time1;

  int error_count0 = ON_GetErrorCount();
  int warning_count0 = ON_GetWarningCount();

  m_results.m_mesh_elapsed_time = 0.0;
  m_results.m_surface_elapsed_time = 0.0;

  if ( m_mesh_tree )
  {
    tp = tp0;
    i = point_count;
    
    time0 = clock();
    while(i--)
    {
      s = t = ON_UNSET_VALUE;
      m_mesh_tree->GetClosestPoint(tp->P,&mp);
      tp++;
    }
    time1 = clock();

    m_results.m_mesh_elapsed_time = ((double)(time1 - time0))/((double)CLOCKS_PER_SEC);
  }

  if ( m_surface_tree )
  {
    tp = tp0;
    i = point_count;
    
    time0 = clock();
    while(i--)
    {
      s = t = ON_UNSET_VALUE;
      m_surface_tree->GetClosestPoint(tp->P,&s,&t);
      tp++;
    }
    time1 = clock();

    m_results.m_surface_elapsed_time = ((double)(time1 - time0))/((double)CLOCKS_PER_SEC);
  }

  m_results.m_bSpeedTest = true;

  m_results.m_error_count += (ON_GetErrorCount()-error_count0);
  m_results.m_warning_count += (ON_GetWarningCount()-warning_count0);

  return m_results.m_mesh_elapsed_time;
}

int CClosestPointToMeshTest::AccuracyTest(int point_count, CMeshTestPoint* tp)
{
  ON_3dPoint Q;
  double d;
  int rc = 0;
  int i;


  int error_count0 = ON_GetErrorCount();
  int warning_count0 = ON_GetWarningCount();

  m_results.m_bAccuracyTest = true;
  m_results.m_test_count = 0;

  if ( m_mesh_tree )
  {
    m_results.m_test_count = point_count;

    

    for ( i = 0; i < point_count; i++ )
    {
      if( !m_mesh_tree->GetClosestPoint(tp[i].P,&tp[i].M) )
      {
        rc++;
        memset(&tp[i].M,0,sizeof(tp[i].M));
        tp[i].M_d = ON_UNSET_VALUE;
        m_results.m_failure_count++;
      }
      else
      {
        Q = FacePoint(tp[i].M);
        d = Q.DistanceTo(tp[i].P);
        tp[i].M_d = d;
        ON_3dVector D = Q - tp[i].M.m_P;
        if (    fabs(D.x) >= (1.0+fabs(Q.x))*ON_EPSILON
             && fabs(D.y) >= (1.0+fabs(Q.y))*ON_EPSILON
             && fabs(D.z) >= (1.0+fabs(Q.z))*ON_EPSILON )
        {
          ON_String msg;
          msg.Format(
            "Bogus ON_MESH_POINT tp[%d].M.m_P value returned from ON_MeshTree::GetClosestPoint()\n",
            i);
          ON_ERROR(msg);
        }
      }
    }
  }

  m_results.m_error_count += (ON_GetErrorCount()-error_count0);
  m_results.m_warning_count += (ON_GetWarningCount()-warning_count0);

  return rc;
}


void CClosestPointToMeshTest::CalculateResults( 
                                          int test_point_count,
                                          const CMeshTestPoint* tp
                                          )
{
  int badi = -1;
  double badd = 0.0;

  m_results.m_speed_factor = (m_results.m_bSpeedTest && m_results.m_surface_elapsed_time > 0.0) 
                           ? m_results.m_mesh_elapsed_time/m_results.m_surface_elapsed_time 
                           : 0.0;

  if ( m_results.m_bAccuracyTest )
  {
    ON_Sum on_3d_error;
    ON_Sum off_3d_error;
    double best_d, d_err;
    int i;
    ON_3dPoint P;
    for ( i = 0; i < test_point_count; i++ )
    {
      const CMeshTestPoint& T = tp[i];

      if ( !T.M.m_mesh )
        continue;

      best_d = (T.M_d < T.S_d) ? T.M_d : T.S_d;

      d_err = T.M_d - best_d;
      if ( 0.0 == best_d )
      {
        m_results.m_on_test_count++;

        if (  T.M_d <= best_d )
        {
          m_results.m_on_best_count++;
        }
        if ( d_err > 1.0e-4 )
        {
          m_results.m_on_wrong_count++;
          if ( d_err > badd )
          {
            badd = d_err;
            badi = T.test_point_index;
          }
        }
        else if ( d_err > 1.0e-6 )
        {
          m_results.m_on_sloppy_count++;
        }
        else
        {
          on_3d_error.Plus(d_err);
        }
      }
      else
      {
        m_results.m_off_test_count++;

        if( T.M_d <= best_d )
        {
          m_results.m_off_best_count++;
        }
        d_err /= best_d;
        if ( d_err > 0.15 )
        {
          m_results.m_off_wrong_count++;
        }
        else if ( d_err > 0.05 )
        {
          m_results.m_off_sloppy_count++;
        }
        else
        {
          off_3d_error.Plus(d_err);
        }
      }
    }

    i = on_3d_error.SummandCount();
    if ( i > 0 )
    {
      d_err = on_3d_error.Total()/((double)i);
      m_results.m_on_mesh_3d_error = d_err;
    }

    i = off_3d_error.SummandCount();
    if ( i > 0 )
    {
      d_err = off_3d_error.Total()/((double)i);
      m_results.m_off_mesh_3d_error = d_err;
    }
  }

  if ( badi>=0) 
  {
    printf("TEST POINT %d had error of = %g.  This is a ClosestPointToSurface BUG.\n",badi,badd);
  }
}



void CClosestPointToMeshTest::Print( ON_TextLog& text_log, int test_count )
{
  const char* format = "%s %s %5d  %4.1fX (%6.3f secs) %3d%%  %.1g";

  text_log.Print("Name    Test  Point  mesh/   Mesh        Accuracy\n");
  text_log.Print("              count   srf    time        best  3d err\n");
  text_log.Print(format,
    "Perfect ",
    "    ",
    test_count,    // m_results.m_test_count,
    1.0,  //m_results.m_speed_factor,
    0.0,  //m_results.m_elapsed_time,
    100,//test_count,    //m_results.m_best_count,
    1e-18  //m_results.m_on_surface_3d_error,
    );
  text_log.Print("\n");

  if ( m_results.m_on_test_count > 0 )
  {
    int best = (int)floor(100.0*((double)m_results.m_on_best_count)/((double)m_results.m_test_count));
    text_log.Print(format,
      "mesh    ",
      "on  ",
      m_results.m_on_test_count,
      m_results.m_speed_factor,
      m_results.m_mesh_elapsed_time,
      best,
      m_results.m_on_mesh_3d_error
      );
    if( m_results.m_failure_count > 0 )
      text_log.Print(" %d FAILURES",m_results.m_failure_count);
    if( m_results.m_error_count > 0 )
      text_log.Print(" %d ON_ERRORs",m_results.m_error_count);
    if( m_results.m_warning_count > 0 )
      text_log.Print(" %d ON_WARNINGSs",m_results.m_warning_count);

    if( m_results.m_on_wrong_count > 0 )
      text_log.Print(" %d WRONG ANSWERS",m_results.m_on_wrong_count);
    if( m_results.m_on_sloppy_count > 0 )
      text_log.Print(" %d sloppys answers",m_results.m_on_sloppy_count);
    text_log.Print("\n");
  }

  if ( m_results.m_off_test_count > 0 )
  {
    int best = (int)floor(100.0*((double)m_results.m_off_best_count)/((double)m_results.m_test_count));
    text_log.Print(format,
      "mesh    ",
      "off ",
      m_results.m_off_test_count,
      m_results.m_speed_factor,
      m_results.m_mesh_elapsed_time,
      best,
      m_results.m_off_mesh_3d_error
      );
    if( m_results.m_failure_count > 0 )
      text_log.Print(" %d FAILURES",m_results.m_failure_count);
    if( m_results.m_error_count > 0 )
      text_log.Print(" %d ON_ERRORs",m_results.m_error_count);
    if( m_results.m_warning_count > 0 )
      text_log.Print(" %d ON_WARNINGSs",m_results.m_warning_count);
    if( m_results.m_off_wrong_count > 0 )
      text_log.Print(" %d WRONG ANSWERS",m_results.m_off_wrong_count);
    if( m_results.m_off_sloppy_count > 0 )
      text_log.Print(" %d sloppys answers",m_results.m_off_sloppy_count);
    text_log.Print("\n");
  }
}



class CClosestPointToSurfaceTest
{
public:
  virtual void Setup( ON_NurbsSurface* nurbs_surface ) = 0;

  virtual int GetClosestPoint( ON_3dPoint P, double* s, double* t ) = 0;

  // returns time in seconds
  double SpeedTest( int test_point_count, const CSurfaceTestPoint* tp);

  // returns error count
  int AccuracyTest( int test_point_count, CSurfaceTestPoint* tp );

  void CalculateResults( const ON_NurbsSurface& nurbs_surface, 
                         int test_point_count, 
                         const CSurfaceTestPoint* tp, 
                         double best_time 
                         );

  void Print( ON_TextLog& text_log, int test_count );

  CSurfaceTestPoint::FUNC_SRF_CLSPT m_func;

  const ON_NurbsSurface* m_nurbs_surface;

  CClosestPointToSurfaceTestResults m_results;

  void SetupHelper( CSurfaceTestPoint::FUNC_SRF_CLSPT func, ON_NurbsSurface* nurbs_surface );
};

void CClosestPointToSurfaceTest::SetupHelper( CSurfaceTestPoint::FUNC_SRF_CLSPT func, ON_NurbsSurface* nurbs_surface )
{
  m_func = func;
  m_nurbs_surface = nurbs_surface;
  memset(&m_results,0,sizeof(m_results));
  m_results.m_elapsed_time = ON_UNSET_VALUE;
}

double CClosestPointToSurfaceTest::SpeedTest(int point_count, const CSurfaceTestPoint* tp)
{
  double s, t;
  int i;
  clock_t time0, time1;

  int error_count0 = ON_GetErrorCount();
  int warning_count0 = ON_GetWarningCount();

  i = point_count;
  
  time0 = clock();
  while(i--)
  {
    s = t = ON_UNSET_VALUE;
    GetClosestPoint(tp->P,&s,&t);
    tp++;
  }
  time1 = clock();

  m_results.m_elapsed_time = ((double)(time1 - time0))/((double)CLOCKS_PER_SEC);

  m_results.m_bSpeedTest = true;

  m_results.m_error_count += (ON_GetErrorCount()-error_count0);
  m_results.m_warning_count += (ON_GetWarningCount()-warning_count0);

  return m_results.m_elapsed_time;
}

int CClosestPointToSurfaceTest::AccuracyTest(int point_count, CSurfaceTestPoint* tp)
{
  ON_3dPoint Q;
  double d;
  int rc = 0;
  int i;
  double s,t;


  int error_count0 = ON_GetErrorCount();
  int warning_count0 = ON_GetWarningCount();

  m_results.m_bAccuracyTest = true;
  m_results.m_test_count = point_count;

  for ( i = 0; i < point_count; i++ )
  {
    s = t = ON_UNSET_VALUE;
    if( !GetClosestPoint(tp[i].P,&s,&t) )
    {
      rc++;
      tp[i].srf_uv[m_func].Set(ON_UNSET_VALUE,ON_UNSET_VALUE);
      tp[i].srf_d[m_func] = ON_UNSET_VALUE;
      m_results.m_failure_count++;
    }
    else
    {
      Q = m_nurbs_surface->PointAt(s,t);
      d = Q.DistanceTo(tp[i].P);
      tp[i].srf_uv[m_func].Set(s,t);
      tp[i].srf_d[m_func] = d;
      if ( ON_UNSET_VALUE == tp[i].best_d || d < tp[i].best_d )
      {
        tp[i].best_d = d;
        tp[i].best_uv.Set(s,t);
      }
    }
  }

  m_results.m_error_count += (ON_GetErrorCount()-error_count0);
  m_results.m_warning_count += (ON_GetWarningCount()-warning_count0);

  return rc;
}


void CClosestPointToSurfaceTest::CalculateResults( const ON_NurbsSurface& nurbs_surface,
                                          int test_point_count,
                                          const CSurfaceTestPoint* tp,
                                          double best_time
                                          )
{
  int badi = -1;
  double badd = 0.0;

  m_results.m_speed_factor = (m_results.m_bSpeedTest && best_time>0.0) ? m_results.m_elapsed_time/best_time : 0.0;
  if ( m_results.m_bAccuracyTest )
  {
    ON_Sum on_3d_error;
    ON_Sum off_3d_error;
    ON_Sum on_uvx_error, on_uvy_error;
    ON_Sum off_uvx_error, off_uvy_error;
    double d_err;
    ON_2dVector uv_err;
    int i;
    int hint[2] = {0,0};
    ON_3dPoint P;
    ON_3dVector Du, Dv;
    for ( i = 0; i < test_point_count; i++ )
    {
      const CSurfaceTestPoint& T = tp[i];

      if ( !T.srf_uv[m_func].IsValid() )
        continue;

      nurbs_surface.Ev1Der( T.best_uv.x, T.best_uv.y, P, Du, Dv, 1, hint );

      uv_err.Set( Du.Length(), Dv.Length() );
      uv_err.x = ( uv_err.x > ON_ZERO_TOLERANCE ) ? 1.0/uv_err.x : 0.0;
      uv_err.y = ( uv_err.y > ON_ZERO_TOLERANCE ) ? 1.0/uv_err.y : 0.0;
      uv_err.x *= fabs(T.srf_uv[m_func].x - T.best_uv.x);
      uv_err.y *= fabs(T.srf_uv[m_func].y - T.best_uv.y);

      d_err = (T.best_d < T.d) ? T.best_d : T.d;

      d_err = T.srf_d[m_func] - d_err;
      if ( 0.0 == T.d || 0.0 == T.best_d )
      {
        m_results.m_on_test_count++;

        if (  T.srf_d[m_func] <= T.best_d )
        {
          m_results.m_on_best_count++;
          uv_err.Set(0.0,0.0);
        }
        if ( d_err > 1.0e-4 )
        {
          m_results.m_on_wrong_count++;
          if ( CSurfaceTestPoint::FUNC_SRF_CLSPT_GOOSE != m_func  && d_err > badd )
          {
            badd = d_err;
            badi = T.test_point_index;
          }
        }
        else if ( d_err > 1.0e-6 )
        {
          m_results.m_on_sloppy_count++;
        }
        else
        {
          on_3d_error.Plus(d_err);
          on_uvx_error.Plus(uv_err.x);
          on_uvy_error.Plus(uv_err.y);
        }
      }
      else if ( T.best_d > 0.0 )
      {
        m_results.m_off_test_count++;

        if( T.srf_d[m_func] <= T.best_d )
        {
          m_results.m_off_best_count++;
          uv_err.Set(0.0,0.0);
        }
        d_err /= T.best_d;
        if ( d_err > 0.15 )
        {
          m_results.m_off_wrong_count++;
        }
        else if ( d_err > 0.05 )
        {
          m_results.m_off_sloppy_count++;
        }
        else
        {
          off_3d_error.Plus(d_err);
          off_uvx_error.Plus(uv_err.x);
          off_uvy_error.Plus(uv_err.y);
        }
      }
    }

    i = on_3d_error.SummandCount();
    if ( i > 0 )
    {
      d_err = on_3d_error.Total()/((double)i);
      uv_err.x = on_uvx_error.Total()/((double)i);
      uv_err.y = on_uvy_error.Total()/((double)i);
      m_results.m_on_surface_3d_error = d_err;
      m_results.m_on_surface_uv_error = uv_err;
    }

    i = off_3d_error.SummandCount();
    if ( i > 0 )
    {
      d_err = off_3d_error.Total()/((double)i);
      uv_err.x = off_uvx_error.Total()/((double)i);
      uv_err.y = off_uvy_error.Total()/((double)i);
      m_results.m_off_surface_3d_error = d_err;
      m_results.m_off_surface_uv_error = uv_err;
    }
  }

  if ( badi>=0) 
  {
    printf("TEST POINT %d had error of = %g.  This is a ClosestPointToSurface BUG.\n",badi,badd);
  }
}

void CClosestPointToSurfaceTest::Print( ON_TextLog& text_log, int test_count )
{
  const char* format = "%s %s %5d  %4.1fX (%6.3f secs) %3d%%  %.1g  %.1g,%.1g";

  const char* name = 0;
  if ( test_count > 0 )
  {
    text_log.Print("Name       Test  Point      Speed           Accuracy\n");
    text_log.Print("                 count  sloth   time        best  3d err  uv err\n");
    name = "Perfect    ";
  }
  else
  {
    switch(m_func)
    {
    case CSurfaceTestPoint::FUNC_SRF_CLSPT_NEWON:
      name = "OpenNURBS  ";
      break;
    case CSurfaceTestPoint::FUNC_SRF_CLSPT_GOOSE:
      name = "Goose      ";
      break;
    default:
      name = "Anonymous  ";
      break;
    }
  }


  if ( test_count > 0 )
  {
    text_log.Print(format,
      name,
      "    ",
      test_count,    // m_results.m_test_count,
      1.0,  //m_results.m_speed_factor,
      0.0,  //m_results.m_elapsed_time,
      100,//test_count,    //m_results.m_best_count,
      1e-18,  //m_results.m_on_surface_3d_error,
      1e-18,  //m_results.m_on_surface_uv_error.x,
      1e-18   //m_results.m_on_surface_uv_error.y,
      );
    text_log.Print("\n");
  }
  else
  {
    if ( m_results.m_on_test_count > 0 )
    {
      int best = (int)floor(100.0*((double)m_results.m_on_best_count)/((double)m_results.m_test_count));
      text_log.Print(format,
        name,
        "on  ",
        m_results.m_on_test_count,
        m_results.m_speed_factor,
        m_results.m_elapsed_time,
        best,//m_results.m_on_best_count,
        m_results.m_on_surface_3d_error,
        m_results.m_on_surface_uv_error.x,
        m_results.m_on_surface_uv_error.y
        );
      if( m_results.m_failure_count > 0 )
        text_log.Print(" %d FAILURES",m_results.m_failure_count);
      if( m_results.m_error_count > 0 )
        text_log.Print(" %d ON_ERRORs",m_results.m_error_count);
      if( m_results.m_warning_count > 0 )
        text_log.Print(" %d ON_WARNINGSs",m_results.m_warning_count);

      if( m_results.m_on_wrong_count > 0 )
        text_log.Print(" %d WRONG ANSWERS",m_results.m_on_wrong_count);
      if( m_results.m_on_sloppy_count > 0 )
        text_log.Print(" %d sloppys answers",m_results.m_on_sloppy_count);
      text_log.Print("\n");
    }

    if ( m_results.m_off_test_count > 0 )
    {
      int best = (int)floor(100.0*((double)m_results.m_off_best_count)/((double)m_results.m_test_count));
      text_log.Print(format,
        name,
        "off ",
        m_results.m_off_test_count,
        m_results.m_speed_factor,
        m_results.m_elapsed_time,
        best, //m_results.m_off_best_count,
        m_results.m_off_surface_3d_error,
        m_results.m_off_surface_uv_error.x,
        m_results.m_off_surface_uv_error.y
        );
      if( m_results.m_failure_count > 0 )
        text_log.Print(" %d FAILURES",m_results.m_failure_count);
      if( m_results.m_error_count > 0 )
        text_log.Print(" %d ON_ERRORs",m_results.m_error_count);
      if( m_results.m_warning_count > 0 )
        text_log.Print(" %d ON_WARNINGSs",m_results.m_warning_count);
      if( m_results.m_off_wrong_count > 0 )
        text_log.Print(" %d WRONG ANSWERS",m_results.m_off_wrong_count);
      if( m_results.m_off_sloppy_count > 0 )
        text_log.Print(" %d sloppys answers",m_results.m_off_sloppy_count);
      text_log.Print("\n");
    }
  }
}



///////////////////////////////////////////////////////////////////////////////////////
//
// Dale's closest point
//
class C_ON_Curve_PointTest : public CClosestPointToCurveTest
{
public:
  virtual void Setup( const ON_Curve* curve  );
  virtual int GetClosestPoint(ON_3dPoint P, double* t);
  const ON_Curve* m_curve;
};

void C_ON_Curve_PointTest::Setup( const ON_Curve* curve )
{
  m_curve = curve;
  m_curve->CurveTree(); // prebuild curve tree
  SetupHelper(CCurveTestPoint::FUNC_CRV_CLSPT_ON_CRV,curve);
}

int C_ON_Curve_PointTest::GetClosestPoint( ON_3dPoint P, double* t )
{
  return m_curve->GetClosestPoint(P,t);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Dale's closest point using ON_NurbsCurve
//
class CNurbsFormCurvePointTest : public CClosestPointToCurveTest
{
public:
  virtual void Setup( const ON_Curve* curve );
  virtual int GetClosestPoint(ON_3dPoint P, double* t);
  TL_NurbsCurve m_nurbs_curve;
};

void CNurbsFormCurvePointTest::Setup( const ON_Curve* curve )
{
  curve->GetNurbForm(m_nurbs_curve);
  m_nurbs_curve.CurveTree(); // prebuild curve tree
  SetupHelper(CCurveTestPoint::FUNC_CRV_CLSPT_ON_NURBS,&m_nurbs_curve);
}

int CNurbsFormCurvePointTest::GetClosestPoint( ON_3dPoint P, double* t )
{
  return m_curve->GetClosestPoint(P,t);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Dale's closest point using ON_CurveTree
//
class CCurveTreePointTest : public CClosestPointToCurveTest
{
public:
  virtual void Setup( const ON_Curve* curve );
  virtual int GetClosestPoint(ON_3dPoint P, double* t);
  TL_NurbsCurve m_nurbs_curve;
  const ON_CurveTree* m_tree;
};

void CCurveTreePointTest::Setup( const ON_Curve* curve )
{
  curve->GetNurbForm(m_nurbs_curve);
  m_tree = m_nurbs_curve.CurveTree(); // prebuild curve tree
  SetupHelper(CCurveTestPoint::FUNC_CRV_CLSPT_ON_TREETEST,&m_nurbs_curve);
}

int CCurveTreePointTest::GetClosestPoint( ON_3dPoint P, double* t )
{
  m_tree->m__GetClosestPointOnCurveTree(m_tree->m_root, P, t, NULL, 0.0, NULL );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////////////
//
// A "hack" closest point that provides a goal
//
class CBaselineCurvePointTest : public CClosestPointToCurveTest
{
public:
  virtual void Setup( const ON_Curve* curve );
  virtual int GetClosestPoint(ON_3dPoint P, double* t);
  TL_NurbsCurve m_nurbs_curve;
};

void CBaselineCurvePointTest::Setup( const ON_Curve* curve )
{
  curve->GetNurbForm(m_nurbs_curve);
  m_nurbs_curve.CurveTree();
  SetupHelper(CCurveTestPoint::FUNC_CRV_CLSPT_BASELINE,&m_nurbs_curve);
}

int CBaselineCurvePointTest::GetClosestPoint( ON_3dPoint P, double* t )
{
  // NOTE - This doesn't handle subdomains and far point global checking
  //        See ON_Curve::GetClosestPoint
  static 
  EF ef;
  memset(&ef,0,sizeof(ef));
  ef.d = ON_UNSET_VALUE;
  ef.t = ON_UNSET_VALUE;
  ef.P = P;
  ef.ct = m_nurbs_curve.CurveTree();

  bool rc = ClosestFork(ef,ON_UNSET_VALUE,ef.ct->m_root);

  if (rc)
  {
    *t = ef.t;
  }

  return rc;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Goose closest point
//

class CGooseCurvePointTest : public CClosestPointToCurveTest
{
public:
  CGooseCurvePointTest();
  ~CGooseCurvePointTest();

  virtual void Setup( const ON_Curve* curve );
  virtual int GetClosestPoint( ON_3dPoint P, double* t );
  
  TL_NurbsCurve m_nurbs_curve;
  ON_3dPoint P0, P1, P2;
  TL_NURB N;
  IwPoint3d Q;
  IwExtent1d Interval;
  IwBSplineCurve* pC;
};

CGooseCurvePointTest::CGooseCurvePointTest()
{
  memset(&N,0,sizeof(N));
  pC =0;
}

CGooseCurvePointTest::~CGooseCurvePointTest()
{
  N.cv = 0;
  N.knot = 0;
  TL_DestroyNurb(&N);
  delete pC;
}

void CGooseCurvePointTest::Setup( const ON_Curve* curve )
{
  curve->GetNurbForm(m_nurbs_curve);
  m_nurbs_curve.CurveTree();
  SetupHelper(CCurveTestPoint::FUNC_CRV_CLSPT_GOOSE,&m_nurbs_curve);

  P0 = m_nurbs_curve.PointAtStart();
  P1 = m_nurbs_curve.PointAt(m_nurbs_curve.Domain().ParameterAt(0.5));
  P2 = m_nurbs_curve.PointAtEnd();

  memset(&N,0,sizeof(N));
  N.dim = m_nurbs_curve.m_dim;
  N.is_rat = m_nurbs_curve.m_is_rat;
  N.order = m_nurbs_curve.m_order;
  N.cv_count = m_nurbs_curve.m_cv_count;
  N.cv = m_nurbs_curve.m_cv;
  N.knot = m_nurbs_curve.m_knot;

  TL_Convert( N, pC );
  Interval = pC->GetNaturalInterval();
  IwCacheMgr::GetOrCreateObjectCache(IW_OC_CURVE,pC);
}

int CGooseCurvePointTest::GetClosestPoint( ON_3dPoint P, double* t )
{
  int rc;
  double dist_tol, d;
  IwStatus iw_rc;

  IwSolutionArray sSolutions;

  dist_tol = P.DistanceTo(P0);
  d = P.DistanceTo(P1);
  if ( d < dist_tol )
    dist_tol = d;
  d = P.DistanceTo(P2);
  if ( d < dist_tol )
    dist_tol = d;
  dist_tol *= (1.0+ON_SQRT_EPSILON);

  Q.x = P.x;
  Q.y = P.y;
  Q.z = P.z;

  iw_rc = pC->GlobalPointSolve( 
                  Interval, 
                  IW_SO_MINIMIZE, 
                  Q,
                  dist_tol, NULL, NULL,
                  IW_SR_SINGLE, 
                  sSolutions 
                  );

  if ( iw_rc == IW_SUCCESS && sSolutions.GetSize() > 0) 
  {
    *t = sSolutions[0].m_vStart[0];
    rc = 1;
  }
  else
  {
    rc = 0;
  }

  return rc;
}



void TestClosestPointToThisCurve( ON_TextLog& text_log, 
                              const ON_Curve* curve,
                              double sample_start_distance,
                              double sample_stop_distance
                              )
{
  CClosestPointToCurveTest* tests[20];
  int test_count = 0;
  int i;
  bool bSpeedTest = true;
  bool bAccuracyTest = true;

  // The commented out ones are slower than newdale_test and newon_test
  CBaselineCurvePointTest  baseline_test;
  C_ON_Curve_PointTest     on_curve_test;
  CNurbsFormCurvePointTest on_nurbsform_test;
  CCurveTreePointTest      on_tree_test;
  CGooseCurvePointTest     goose_test;

  baseline_test.Setup(curve);
  goose_test.Setup(curve);
  on_curve_test.Setup(curve);
  on_nurbsform_test.Setup(curve);
  on_tree_test.Setup(curve);

  tests[test_count++] = &baseline_test;
  tests[test_count++] = &on_tree_test;
  tests[test_count++] = &on_curve_test;
  tests[test_count++] = &on_nurbsform_test;
  if ( bDoGooseTests )
    tests[test_count++] = &goose_test;

  // get test points
  ON_SimpleArray<CCurveTestPoint> TP;
  GetCurvePointCloud( *curve, sample_start_distance, sample_stop_distance, TP );
  CCurveTestPoint* tp = TP.Array();
  int test_point_count = TP.Count();

  if ( 0.0 == sample_stop_distance )
  {
    text_log.Print("Testing %d points exactly on the curve.\n",
      test_point_count);
  }
  else if ( sample_start_distance == sample_stop_distance )
  {
    text_log.Print("Testing %d points about %g units off of the curve.\n",
      test_point_count,
      sample_start_distance);
  }
  else
  {
    text_log.Print("Testing %d points from %g to %g units off of the curve.\n",
      test_point_count,
      sample_start_distance,sample_stop_distance);
  }

  double best_time = 0.0;

  // execution time tests
  if ( bSpeedTest )
  {

    for ( i = 0; i < test_count; i++ )
    {
      tests[i]->SpeedTest(test_point_count,tp);
    }

    for ( i = 0; i < test_count; i++ )
    {
      if ( 0.0 == best_time || (tests[i]->m_results.m_elapsed_time > 0.0 && tests[i]->m_results.m_elapsed_time < best_time ))
      {
        best_time = tests[i]->m_results.m_elapsed_time;
      }
    }
  }

  if ( bAccuracyTest )
  {
    for ( i = 0; i < test_count; i++ )
    {
      tests[i]->AccuracyTest(test_point_count,tp);
    }
  }

  for ( i = 0; i < test_count; i++ )
  {
    tests[i]->CalculateResults(*curve,test_point_count,tp,best_time);
  }  

  // print title
  tests[0]->Print(text_log,test_point_count);

  // print results for each test
  for ( i = 0; i < test_count; i++ )
  {
    tests[i]->Print(text_log,0);
  }  
}


///////////////////////////////////////////////////////////////////////////////////////
//
// Dale's new surface closest point
//
class CDaleSurfacePointTest : public CClosestPointToSurfaceTest
{
public:
  virtual void Setup( ON_NurbsSurface* nurbs_surface );
  virtual int GetClosestPoint(ON_3dPoint P, double* s, double* t);
};

void CDaleSurfacePointTest::Setup( ON_NurbsSurface* nurbs_surface )
{
  SetupHelper(CSurfaceTestPoint::FUNC_SRF_CLSPT_NEWON,nurbs_surface);
  nurbs_surface->SurfaceTree(); // prebuild surface tree
}

int CDaleSurfacePointTest::GetClosestPoint( ON_3dPoint P, double* s, double* t )
{
  return m_nurbs_surface->GetClosestPoint(P,s,t);
}


///////////////////////////////////////////////////////////////////////////////////////
//
// Goose closest point
//

class CGooseSurfacePointTest : public CClosestPointToSurfaceTest
{
public:
  CGooseSurfacePointTest();
  ~CGooseSurfacePointTest();

  virtual void Setup( ON_NurbsSurface* nurbs_surface );
  virtual int GetClosestPoint( ON_3dPoint P, double* s, double* t );
  
  ON_3dPoint srfP; // surface midpoint
  TL_NURBSRF N;
  IwPoint3d Q;
  IwExtent2d Interval;
  IwBSplineSurface* pS;
};

CGooseSurfacePointTest::CGooseSurfacePointTest()
{
  memset(&N,0,sizeof(N));
  pS =0;
}

CGooseSurfacePointTest::~CGooseSurfacePointTest()
{
  N.cv = 0;
  N.knot[0] = 0;
  N.knot[1] = 0;
  TL_DestroyNurbSrf(&N);
  delete pS;
}

void CGooseSurfacePointTest::Setup( ON_NurbsSurface* nurbs_surface )
{
  SetupHelper(CSurfaceTestPoint::FUNC_SRF_CLSPT_GOOSE,nurbs_surface);

  srfP = nurbs_surface->PointAt(nurbs_surface->Domain(0).ParameterAt(0.5),nurbs_surface->Domain(1).ParameterAt(0.5));

  memset(&N,0,sizeof(N));
  N.dim = nurbs_surface->m_dim;
  N.is_rat = nurbs_surface->m_is_rat;
  N.order[0] = nurbs_surface->m_order[0];
  N.order[1] = nurbs_surface->m_order[1];
  N.cv_count[0] = nurbs_surface->m_cv_count[0];
  N.cv_count[1] = nurbs_surface->m_cv_count[1];
  N.cv = nurbs_surface->m_cv;
  N.knot[0] = nurbs_surface->m_knot[0];
  N.knot[1] = nurbs_surface->m_knot[1];

  TL_Convert( N, pS );
  Interval = pS->GetNaturalUVDomain();
  IwCacheMgr::GetOrCreateObjectCache(IW_OC_SURFACE,pS);
}

int CGooseSurfacePointTest::GetClosestPoint( ON_3dPoint P, double* s, double* t )
{
  int rc;
  double dist_tol;
  IwStatus iw_rc;

  IwSolutionArray sSolutions;

  dist_tol = (1.0+ON_SQRT_EPSILON)*P.DistanceTo(P);

  Q.x = P.x;
  Q.y = P.y;
  Q.z = P.z;

  iw_rc = pS->GlobalPointSolve( 
                  Interval, 
                  IW_SO_MINIMIZE, 
                  Q,
                  dist_tol, 
                  NULL,// NULL,
                  IW_SR_SINGLE, 
                  sSolutions 
                  );

  if ( iw_rc == IW_SUCCESS && sSolutions.GetSize() > 0) 
  {
    *s = sSolutions[0].m_vStart[0];
    *t = sSolutions[0].m_vStart[1];
    rc = 1;
  }
  else
  {
    rc = 0;
  }

  return rc;
}


void TestClosestPointToThisSurface( ON_TextLog& text_log, 
                              ON_NurbsSurface& nurbs_surface,
                              double sample_start_distance,
                              double sample_stop_distance
                              )
{
  CClosestPointToSurfaceTest* tests[20];
  int test_count = 0;
  int i;
  bool bSpeedTest = true;
  bool bAccuracyTest = true;

  CDaleSurfacePointTest  dale_test;
  CGooseSurfacePointTest goose_test;

  dale_test.Setup(&nurbs_surface);
  goose_test.Setup(&nurbs_surface);

  tests[test_count++] = &dale_test;
  //tests[test_count++] = &goose_test;

  // get test points
  ON_SimpleArray<CSurfaceTestPoint> TP;
  GetSurfacePointCloud( nurbs_surface, sample_start_distance, sample_stop_distance, TP );
  CSurfaceTestPoint* tp = TP.Array();
  int test_point_count = TP.Count();

  if ( 0.0 == sample_stop_distance )
  {
    text_log.Print("Testing %d points exactly on the surface.\n",
      test_point_count);
  }
  else if ( sample_start_distance == sample_stop_distance )
  {
    text_log.Print("Testing %d points about %g units off of the surface.\n",
      test_point_count,
      sample_start_distance);
  }
  else
  {
    text_log.Print("Testing %d points from %g to %g units off of the surface.\n",
      test_point_count,
      sample_start_distance,sample_stop_distance);
  }

  double best_time = 0.0;

  // execution time tests
  if ( bSpeedTest )
  {

    for ( i = 0; i < test_count; i++ )
    {
      tests[i]->SpeedTest(test_point_count,tp);
    }

    for ( i = 0; i < test_count; i++ )
    {
      if ( 0.0 == best_time || (tests[i]->m_results.m_elapsed_time > 0.0 && tests[i]->m_results.m_elapsed_time < best_time ))
      {
        best_time = tests[i]->m_results.m_elapsed_time;
      }
    }
  }

  if ( bAccuracyTest )
  {
    for ( i = 0; i < test_count; i++ )
    {
      tests[i]->AccuracyTest(test_point_count,tp);
    }
  }

  for ( i = 0; i < test_count; i++ )
  {
    tests[i]->CalculateResults(nurbs_surface,test_point_count,tp,best_time);
  }  

  // print title
  tests[0]->Print(text_log,test_point_count);

  // print results for each test
  for ( i = 0; i < test_count; i++ )
  {
    tests[i]->Print(text_log,0);
  }  
}

void TestClosestPointToThisMesh( ON_TextLog& text_log, 
                              const ON_Mesh& mesh,
                              const ON_Surface* srf,
                              double sample_start_distance,
                              double sample_stop_distance
                              )
{
  CClosestPointToMeshTest* tests[20];
  int test_count = 0;
  int i;
  bool bSpeedTest = true;
  bool bAccuracyTest = true;

  // get test points
  ON_SimpleArray<CMeshTestPoint> TP;
  GetMeshPointCloud( mesh, sample_start_distance, sample_stop_distance, TP );
  CMeshTestPoint* tp = TP.Array();
  int test_point_count = TP.Count();

  CClosestPointToMeshTest first_test;

  first_test.SetupHelper( &mesh, srf );

  tests[test_count++] = &first_test;

  if ( 0.0 == sample_stop_distance )
  {
    text_log.Print("Testing %d points exactly on the mesh.\n",
      test_point_count);
  }
  else if ( sample_start_distance == sample_stop_distance )
  {
    text_log.Print("Testing %d points about %g units off of the mesh.\n",
      test_point_count,
      sample_start_distance);
  }
  else
  {
    text_log.Print("Testing %d points from %g to %g units off of the mesh.\n",
      test_point_count,
      sample_start_distance,sample_stop_distance);
  }

  double best_time = 0.0;

  // execution time tests
  if ( bSpeedTest )
  {

    for ( i = 0; i < test_count; i++ )
    {
      tests[i]->SpeedTest(test_point_count,tp);
    }

  }

  if ( bAccuracyTest )
  {
    for ( i = 0; i < test_count; i++ )
    {
      tests[i]->AccuracyTest(test_point_count,tp);
    }
  }

  for ( i = 0; i < test_count; i++ )
  {
    tests[i]->CalculateResults(test_point_count,tp);
  }  

  // print title
  tests[0]->Print(text_log,test_point_count);
}

void TestCurveTree(ON_TextLog& text_log, ON_NurbsCurve& nurbs_curve)
{
  const ON_CurveTree* tree = nurbs_curve.CurveTree();
  if ( !tree->IsValid( &text_log, &nurbs_curve ) )
  {
    text_log.Print("Curve tree is not valid\n");
  }

  if (true)
  {
    ON_CurveTreeNode* leaf = tree->FirstLeaf();
    int leaf_count=0;
    double maxar = 0.0;
    double maxr = 0.0;
    double maxlen = 0.0;
    double longest = 0.0;
    double shortest = fabs(ON_UNSET_VALUE);
    ON_Line axis;
    int notmonocount = 0;
    while (leaf)
    {
      if( !leaf->m_bez->m_leafbox.m_bMono )
        notmonocount++;
      axis.from = leaf->m_bez->PointAt(0.0);
      axis.to = leaf->m_bez->PointAt(1.0);
      double len = axis.Length();
      if (len > longest )
        longest = len;
      if ( len < shortest )
        shortest = len;
      if ( leaf->m_bez->m_leafbox.Radius() > maxar*len )
      {
        maxar = leaf->m_bez->m_leafbox.Radius()/len;
        maxlen = len;
        maxr = leaf->m_bez->m_leafbox.Radius();
      }
      leaf_count++;
      leaf = leaf->NextLeaf();
    }
    if ( notmonocount > 0 )
    {
      text_log.Print("ON_CurveTree: %d leaves (%d are not monotone).\n",leaf_count,notmonocount);
    }
    else
    {
      text_log.Print("ON_CurveTree: %d leaves (all are monotone).\n",leaf_count);
    }
    text_log.PushIndent();
    text_log.Print("Longest: %g\n",longest);
    text_log.Print("Shortest: %g\n",shortest);
    if ( maxar > 0.0 )
      text_log.Print("Thickest: rad/len = %g (rad = %g, len = %g)\n",maxar,maxr,maxlen);
    text_log.PopIndent();
  }
}



void TestSurfaceTree(ON_TextLog& text_log, ON_NurbsSurface& nurbs_surface)
{
  const ON_SurfaceTree* tree = nurbs_surface.SurfaceTree();
  if ( !tree->IsValid( &text_log, &nurbs_surface ) )
  {
    text_log.Print("Surface tree is not valid.\n");
  }

  if (true)
  {
    ON_SurfaceTreeNode* leaf = tree->FirstLeaf();
    int leaf_count=0;
    double maxar = 0.0;
    double maxr = 0.0;
    double maxlen = 0.0;
    double longest = 0.0;
    double shortest = fabs(ON_UNSET_VALUE);
    ON_3dPoint C[4];
    int notmonocount = 0;
    while (leaf)
    {
      if( !leaf->m_bez->m_leafbox.m_bMono )
        notmonocount++;
      C[0] = leaf->m_bez->PointAt(0.0,0.0);
      C[1] = leaf->m_bez->PointAt(1.0,0.0);
      C[2] = leaf->m_bez->PointAt(1.0,1.0);
      C[3] = leaf->m_bez->PointAt(0.0,1.0);

      double len = C[3].DistanceTo(C[0]);

      for ( int i = 0; i < 3; i++ )
      {
        double x = C[i].DistanceTo(C[i+1]);
        if ( x > len )
          len = x;
      }

      if (len > longest )
        longest = len;

      if ( len < shortest )
        shortest = len;

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
      text_log.Print("ON_SurfaceTree: %d leaves (%d are not monotone).\n",leaf_count,notmonocount);
    }
    else
    {
      text_log.Print("ON_SurfaceTree: %d leaves (all are monotone).\n",leaf_count);
    }

    text_log.PushIndent();
    text_log.Print("Longest: %g\n",longest);
    text_log.Print("Shortest: %g\n",shortest);
    if ( maxar > 0.0 )
      text_log.Print("Thickest: ht/len = %g (ht = %g, len = %g)\n",maxar,maxr,maxlen);
    text_log.PopIndent();
  }
}

class CTestMeshTreeHelper
{
public:
  CTestMeshTreeHelper();

  void Test( const ON_MeshTreeNode* node, int depth );

  void Report( ON_TextLog& text_log ) const;

  double m_tree_diagonal;
  int m_one_branch_node_count;
  int m_two_branch_node_count;
  int m_leaf_count;
  int m_max_depth;
  int m_max_branched_node_fcount;
  int m_min_leaf_fcount;
  int m_max_leaf_fcount;
  double m_min_leaf_diagonal;
  double m_max_leaf_diagonal;
  double m_min_diagonal_ratio;   // maximum (child node diag)/(parent node diag).
  double m_max_diagonal_ratio;   // maximum (child node diag)/(parent node diag).
};

CTestMeshTreeHelper::CTestMeshTreeHelper()
{
  memset(this,0,sizeof(*this));
  m_tree_diagonal = ON_UNSET_VALUE;
  m_min_leaf_diagonal = 1.0e150;
  m_min_leaf_fcount = -1;
  m_min_diagonal_ratio = 1.0e150;
}


void CTestMeshTreeHelper::Test( const ON_MeshTreeNode* node, int depth) 
{
  if ( !node )
    return;

  int down_count = 0;

  const double node_diagonal = node->m_bbox.Diagonal().Length();

  if ( ON_UNSET_VALUE == m_tree_diagonal )
  {
    m_tree_diagonal = node_diagonal;
  }

  if ( depth > m_max_depth )
  {
    m_max_depth = depth;
  }

  if ( node->m_down[0] )
  {
    down_count++;
    double d = node->m_down[0]->m_bbox.Diagonal().Length()/node_diagonal;
    if ( d > m_max_diagonal_ratio )
    {
      m_max_diagonal_ratio = d;
    }
    if ( d < m_min_diagonal_ratio )
    {
      m_min_diagonal_ratio = d;
    }
    Test( node->m_down[0], depth+1 );
  }

  if ( node->m_down[1] )
  {
    down_count++;
    double d = node->m_down[1]->m_bbox.Diagonal().Length()/node_diagonal;
    if ( d > m_max_diagonal_ratio )
    {
      m_max_diagonal_ratio = d;
    }
    if ( d < m_min_diagonal_ratio )
    {
      m_min_diagonal_ratio = d;
    }
    Test( node->m_down[1], depth+1 );
  }

  if ( down_count )
  {
    if ( node->m_fcount > m_max_branched_node_fcount )
    {
      m_max_branched_node_fcount = node->m_fcount;
    }
    if ( 1 == down_count )
      m_one_branch_node_count++;
    else
      m_two_branch_node_count++;
  }
  else
  {
    m_leaf_count++;
    if ( 1 == m_leaf_count )
    {
      m_max_leaf_fcount = m_min_leaf_fcount = node->m_fcount;
    }
    else if ( node->m_fcount > m_max_leaf_fcount )
    {
      m_max_leaf_fcount = node->m_fcount;
    }
    else if ( node->m_fcount < m_min_leaf_fcount )
    {
      m_min_leaf_fcount = node->m_fcount;
    }

    if ( node_diagonal > m_max_leaf_diagonal )
    {
      m_max_leaf_diagonal = node_diagonal;
    }
    if ( node_diagonal < m_min_leaf_diagonal )
    {
      m_min_leaf_diagonal = node_diagonal;
    }
  }
}


void CTestMeshTreeHelper::Report( ON_TextLog& textlog ) const
{
  textlog.Print("%d one branch nodes\n",m_one_branch_node_count);
  textlog.Print("%d two branch nodes\n",m_two_branch_node_count);
  textlog.Print("%d leaf nodes\n",m_leaf_count);
  textlog.Print("%d total nodes\n",m_leaf_count+m_two_branch_node_count+m_one_branch_node_count);
  textlog.Print("Maximum depth: %d\n",m_max_depth);
  textlog.Print("Maximum branched node face count: %d\n",m_max_branched_node_fcount);
  textlog.Print("Diagonal reduction (child/parent): %g to %g\n",
                m_min_diagonal_ratio, 
                m_max_diagonal_ratio);
  textlog.Print("Leaf node face count: %d to %d\n",m_min_leaf_fcount,m_max_leaf_fcount);
  textlog.Print("Leaf diagonal size: %g to %g\n",m_min_leaf_diagonal,m_max_leaf_diagonal);
}

void TestMeshTree(ON_TextLog& text_log, const ON_Mesh& mesh)
{
  const ON_MeshTree* tree = mesh.MeshTree();

  if ( !tree )
  {
    text_log.Print("mesh.MeshTree() returned NULL.\n");
    ON_ERROR("mesh.MeshTree() returned NULL.");
    return;
  }

  if ( !tree->IsValid( &text_log ) )
  {
    text_log.Print("Mesh tree is not valid.\n");
    ON_ERROR("mesh.MeshTree() is not valid.");
  }
  else
  {
    text_log.Print("Mesh tree is valid.\n");
  }

  if (true)
  {
    CTestMeshTreeHelper testmeshhelper;
    testmeshhelper.Test(tree,1);

    text_log.PushIndent();
    testmeshhelper.Report(text_log);
    text_log.PopIndent();
  }

}

void TestClosestPointToCurveHelper( ON_TextLog& text_log, const ON_Curve* curve, const wchar_t* name, const ON_SimpleArray<ON_3dPoint>& point_list )
{
  TL_NurbsCurve nurbs_curve;
  int pass;
  double sample_start_distance = 0.0;
  double sample_stop_distance = 0.0;

  if ( curve )
  {
    if ( 0 == name || 0 == *name)
    {
      name = L"anonymous";
    }

    curve->GetNurbForm(nurbs_curve);

    text_log.Print(L"Curve class = %S, name = %s, degree = %d, %s,  CV count=%d\n",
                  curve->ClassId()->ClassName(),
                  name,
                  nurbs_curve.m_order-1,
                  (nurbs_curve.m_is_rat ? L"rational" : L"non-rational"),
                  nurbs_curve.m_cv_count
                  );

    TestCurveTree( text_log, nurbs_curve );

    for ( pass = 0; pass < 3; pass++ )
    {
      switch(pass)
      {
      case 0:
        sample_start_distance = 0.0;
        sample_stop_distance = 0.0;
        break;

      case 1:
        sample_start_distance = 1.0/pow(2.0,8);
        sample_stop_distance = 1.0/pow(2.0,5);
        break;

      case 2:
        sample_start_distance = 0.25;
        sample_stop_distance = 1.0;
        break;
      }

      text_log.PushIndent();

      text_log.Print("\n");

      TestClosestPointToThisCurve( text_log, curve, sample_start_distance, sample_stop_distance );

      text_log.PopIndent();
    }

    text_log.Print("\n");
  }
}




void TestClosestPointToSurfaceHelper( ON_TextLog& text_log, const ON_Surface* surface, const wchar_t* name, const ON_SimpleArray<ON_3dPoint>& point_list  )
{
  TL_NurbsSurface nurbs_surface;
  int pass;
  double sample_start_distance = 0.0;
  double sample_stop_distance = 0.0;

  if ( surface )
  {
    if ( 0 == name || 0 == *name)
    {
      name = L"anonymous";
    }

    surface->GetNurbForm(nurbs_surface);

    text_log.Print(L"Surface name = %s, degree = (%d,%d) %s,  CV count=(%d,%d)\n",
                  name,
                  nurbs_surface.m_order[0]-1,nurbs_surface.m_order[1]-1,
                  (nurbs_surface.m_is_rat ? L"rational" : L"non-rational"),
                  nurbs_surface.m_cv_count[0],nurbs_surface.m_cv_count[1]
                  );

    TestSurfaceTree( text_log, nurbs_surface );

    for ( pass = 0; pass < 3; pass++ )
    {
      switch(pass)
      {
      case 0:
        sample_start_distance = 0.0;
        sample_stop_distance = 0.0;
        break;

      case 1:
        sample_start_distance = 1.0/pow(2.0,8);
        sample_stop_distance = 1.0/pow(2.0,5);
        break;

      case 2:
        sample_start_distance = 0.25;
        sample_stop_distance = 1.0;
        break;
      }

      text_log.PushIndent();

      text_log.Print("\n");

      TestClosestPointToThisSurface( text_log, nurbs_surface, sample_start_distance, sample_stop_distance );

      text_log.PopIndent();
    }

    text_log.Print("\n");
  }
}

void TestClosestPointToMeshHelper( ON_TextLog& text_log, 
                                   const ON_Mesh* mesh, 
                                   const ON_Surface* surface,
                                   const wchar_t* name, 
                                   const ON_SimpleArray<ON_3dPoint>& point_list 
                                   )
{
  int pass;
  double sample_start_distance = 0.0;
  double sample_stop_distance = 0.0;

  if ( mesh )
  {
    if ( 0 == name || 0 == *name)
    {
      name = L"anonymous";
    }

    text_log.Print(L"Mesh name = %s\n",
                  name);
    text_log.PushIndent();
    text_log.Print(L"vertex count = %d\n",
                  mesh->VertexCount()
                  );
    text_log.Print(L"face count = %d (%d tris, %d quads)\n",
                  mesh->FaceCount(),
                  mesh->TriangleCount(),
                  mesh->QuadCount()
                  );
    text_log.PopIndent();

    TestMeshTree( text_log, *mesh );

    for ( pass = 0; pass < 3; pass++ )
    {
      switch(pass)
      {
      case 0:
        sample_start_distance = 0.0;
        sample_stop_distance = 0.0;
        break;

      case 1:
        sample_start_distance = 1.0/pow(2.0,8);
        sample_stop_distance = 1.0/pow(2.0,5);
        break;

      case 2:
        sample_start_distance = 0.25;
        sample_stop_distance = 1.0;
        break;
      }

      text_log.PushIndent();

      text_log.Print("\n");

      TestClosestPointToThisMesh( text_log, *mesh, surface, sample_start_distance, sample_stop_distance );

      text_log.PopIndent();
    }

    text_log.Print("\n");
  }
}



void TestClosestPoint( const ONX_Model& model, ON_TextLog& text_log,
                       bool bDoCurves, 
                       bool bDoSurfaces, 
                       bool bDoMeshes )
{
  TEST_HEADER(text_log,"TestClosestPoint");

  int i, k;

  ON_wString name;
  const wchar_t* attributes_name;

  ON_SimpleArray< ON_3dPoint > points(1024);
  // first do curves
  if (bDoPoints) 
  {
    for ( i = 0; i < model.m_object_table.Count(); i++ )
    {
      const ON_Point* point = ON_Point::Cast(model.m_object_table[i].m_object);
      if ( point )
      {
        points.Append(point->point);
        continue;
      }


      const ON_PointCloud* pointcloud = ON_PointCloud::Cast(model.m_object_table[i].m_object);
      if ( pointcloud )
      {
        points.Append( pointcloud->m_P.Count(), pointcloud->m_P.Array() );
        continue;
      }
    }
  }


  // first do curves
  if ( bDoCurves) 
  {
    for ( i = 0; i < model.m_object_table.Count(); i++ )
    {
      const ON_Curve* curve = ON_Curve::Cast(model.m_object_table[i].m_object);
      if ( curve )
      {
        attributes_name = model.m_object_table[i].m_attributes.m_name;
        TestClosestPointToCurveHelper(text_log,curve,attributes_name,points);
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
            TestClosestPointToCurveHelper(text_log,curve,name,points);
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
            TestClosestPointToCurveHelper(text_log,curve,name,points);
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
        TestClosestPointToSurfaceHelper(text_log,surface,attributes_name,points);
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

            TestClosestPointToSurfaceHelper(text_log,surface,model.m_object_table[i].m_attributes.m_name,points);
          }
        }
        continue;
      }
    }
  }

  // then do meshes
  if ( bDoMeshes )
  {
    for ( i = 0; i < model.m_object_table.Count(); i++ )
    {
      const ON_Mesh* mesh = ON_Mesh::Cast(model.m_object_table[i].m_object);
      if ( mesh )
      {
        attributes_name = model.m_object_table[i].m_attributes.m_name;
        TestClosestPointToMeshHelper(text_log,mesh,0,attributes_name,points);
        continue;
      }

      const ON_Brep* brep = ON_Brep::Cast(model.m_object_table[i].m_object);
      if ( brep )
      {
        for ( k = 0; k < brep->m_F.Count(); k++ )
        {
          mesh = brep->m_F[k].Mesh( ON::render_mesh );
          if ( mesh )
          {
            attributes_name = model.m_object_table[i].m_attributes.m_name;
            if ( !attributes_name )
              attributes_name = L"anonymous";
            name.Format(L"%s - brep.m_F[%d] render mesh",attributes_name,k);

            TestClosestPointToMeshHelper(
                  text_log,
                  mesh,
                  brep->m_F[k].SurfaceOf(),
                  model.m_object_table[i].m_attributes.m_name,points);
          }
        }
        continue;
      }
    }
  }
}

