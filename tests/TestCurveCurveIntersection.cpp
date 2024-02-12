

#include "Tests.h"

class CCCXTest
{
public:
	CCCXTest() : TLX0(), TLX1() {	TLX0.SetCapacity(5);	TLX1.SetCapacity(5);
																TLX2.SetCapacity(5);  TLX3.SetCapacity(5);};
	ON_3dPoint P;  // curve(t)

	ON_SimpleArray<TL_CX_EVENT> TLX0;
	ON_SimpleArray<TL_CX_EVENT> TLX1;

	ON_SimpleArray<TL_CX_EVENT> TLX2;
	ON_SimpleArray<TL_CX_EVENT> TLX3;

	IwSolutionArray iwX0;
	IwSolutionArray iwX1;
	
	int i0;				// Indicies of crv on the array of tranlated curves
	int i1;

  double t0;		// these are the parameters of an intersection point
  double t1;
  double d; // == 0 if P = crv(t) and > 0 otherwise

  double gregt;
  double tlt;
  double gooset;

  double gregd;
  double tld;
  double goosed;
};

class TestCurve{
public:
	TestCurve( const ON_Curve& orig);
	TestCurve( );
	~TestCurve();

	void SetCurve( const ON_Curve& orig);
	ON_Curve* crv;

	void MakeOthers();		// Makes the Iwbs and TL_NURB and gets trees for all three.
	IwBSplineCurve* Iwbs;
	
	int gnfrc;	//getnurbform return code

	TL_NURB Nurb;

	double t0;		// base parameter value
};

TestCurve::TestCurve( const ON_Curve& orig){
	SetCurve( orig);
	gnfrc = 0;
	Iwbs = NULL;
}

TestCurve::TestCurve(){
	crv = NULL;
	Iwbs = NULL;
}

TestCurve::~TestCurve(){
	delete crv;
	delete Iwbs;
}

void TestCurve::SetCurve( const ON_Curve& orig){
	if(crv){
		delete crv;
		delete Iwbs;
		Iwbs = NULL;
	}
	crv = orig.DuplicateCurve();
}


void TestCurve::MakeOthers(){
	memset(&Nurb,0,sizeof(Nurb));

	ON_NurbsCurve nc;
	gnfrc = crv->GetNurbForm(nc);

	if(gnfrc<1)
		return;

	Nurb.dim = nc.m_dim;
  Nurb.is_rat = nc.m_is_rat;
  Nurb.order = nc.m_order;
  Nurb.cv_count = nc.m_cv_count;
  Nurb.cv = nc.m_cv;
  Nurb.knot = nc.m_knot;


  TL_Convert( Nurb, Iwbs );

	  // build GSLib curve tree
  IwCacheMgr::GetOrCreateObjectCache(IW_OC_CURVE,Iwbs);

	  // build Greg's curve tree
  crv->CurveTree();

}

// Get a family of translates of the curve that all go though a supplied Base Point
// On input the array contains a single curve.
// On output the aray contains a list of translates of the original that pass through the base point. 
void GetCurveTranslates( const ON_Curve& C0, ON_ClassArray< TestCurve>& C, ON_3dPoint BasePoint )
{
	int Samples_per_span=7;   //7  //4;
	const int MaxSamples=150;		//150??
	const int MinSamples=35;

  const int span_count = C0.SpanCount();
  if (span_count < 1 )
  {
    return;
  }
  double* span_vector = (double*)onmalloc((span_count+1)*sizeof(*span_vector));
  if ( !C0.GetSpanVector(span_vector))
    return;
	
	int SpanGrouping = 1;
	int SpanRemainder = 0;
	if( span_count * Samples_per_span< MinSamples){
		Samples_per_span = MinSamples/span_count + 1;
	}

	if( span_count * Samples_per_span> MaxSamples){
		int MaxSpans = MaxSamples/ Samples_per_span +1;
		SpanGrouping = span_count/MaxSpans;
		SpanRemainder = span_count - SpanGrouping*MaxSpans ;
	}

  for (int i = 0; i < span_count; )
  {
	  int j=(i==0)?0:1;
		ON_Interval span_domain(span_vector[i],span_vector[i+SpanGrouping]);
		i+=SpanGrouping;
		if((SpanRemainder--)>0)
			span_domain[1]= span_vector[++i];
		
		for ( /* j already init'd*/ ; j <= Samples_per_span; j++ )
					
// TODO:  use this to avoid knots
//		for ( int j = 1; j < Samples_per_span; j++ )
    {
      double t = span_domain.ParameterAt(((double)j)/((double)Samples_per_span));	
			
			ON_3dPoint P0 = C0.PointAt(t);
			ON_3dVector Del = BasePoint - P0;

			TestCurve& Ctrans = C.AppendNew();
			Ctrans.SetCurve( C0);
			Ctrans.crv->Translate(Del);
			Ctrans.t0 = t;
			Ctrans.MakeOthers();
		}
	}

	onfree(span_vector);
}

void TestCCX( 	ON_TextLog& text_log, 
								ON_ClassArray< TestCurve > & Crv0 ,
								ON_ClassArray< TestCurve > & Crv1,
								ON_3dPoint BasePoint)
{
  double greg_time  = 0.0;
  double goose_time = 0.0;
  double tl_time  = 0.0;

  ON_ClassArray<CCCXTest> CCX_Result;
	int N0 = Crv0.Count();
	int N1 = Crv1.Count();
	int N =  N0 * N1;

	CCX_Result.SetCapacity(N);
	CCX_Result.SetCount(N);

	// Record expected results
	int i0, i1, i;
	for ( i0 = 0; i0 < N0 ; i0++ ){
		i = N1*i0;
		for ( i1 = 0; i1 < N1 ; i1++, i++ )
		{
		  CCX_Result[i].i0 = i0; 
		  CCX_Result[i].i1 = i1; 
			CCX_Result[i].t0 = Crv0[i0].t0; 
		  CCX_Result[i].t1 = Crv1[i1].t0; 
		}
	}
	const double intersection_tolerance = .01;

  ////////////////////////////////////////////
  //
  // Speed Test
  //

  // TL's curve/curve intersection
  {
    TEST_ElapsedTime();
		for ( i0 = 0; i0 < N0 ; i0++ ){
			i = N1*i0;
		  for ( i1 = 0; i1 < N1 ; i1++, i++ )
		 {
		    TL_CCX( *Crv0[i0].crv, *Crv1[i1].crv, intersection_tolerance, CCX_Result[i].TLX0); 
		    TL_CCX( *Crv1[i1].crv, *Crv0[i0].crv, intersection_tolerance, CCX_Result[i].TLX1);
		 }
		}
	tl_time = TEST_ElapsedTime();
	}

  // Gregs new's curve/curve intersection
#if 0 
	{
    TEST_ElapsedTime();
		for ( i0 = 0; i0 < N0 ; i0++ ){
			i = N1*i0;
		  for ( i1 = 0; i1 < N1 ; i1++, i++ )
		 {
		    DaleCCX( *Crv0[i0].crv, *Crv1[i1].crv, intersection_tolerance, CCX_Result[i].TLX2); 
		    DaleCCX( *Crv1[i1].crv, *Crv0[i0].crv, intersection_tolerance, CCX_Result[i].TLX3);
		 }
		}
		greg_time = TEST_ElapsedTime();
	}
#endif

  // Get a GSLib curve
  {
    IwStatus iw_rc;
    IwPoint3d Q;

    TEST_ElapsedTime();
 		for ( i0 = 0; i0 < N0 ; i0++ ){
			i = N1*i0;
		  for ( i1 = 0; i1 < N1 ; i1++, i++ ){   

			ON_Interval D= Crv0[i0].crv->Domain();	
			IwExtent1d Dom0(D.m_t[0],D.m_t[1]);
			D= Crv1[i1].crv->Domain();	
			IwExtent1d Dom1(D.m_t[0],D.m_t[1]);

			iw_rc = Crv0[i0].Iwbs->GlobalCurveIntersect( 
                            Dom0,
                            *Crv1[i1].Iwbs,
                            Dom1,
                            intersection_tolerance,
                            CCX_Result[i].iwX0 );

      if ( iw_rc == IW_SUCCESS &&  CCX_Result[i].iwX0.GetSize() > 0) 
      {
        //rc = (sSolutions.GetSize() > 1 ) ? TL_SUCCESS+8 : TL_SUCCESS;
        //t = X.m_vStart[0];
      }
      else {
        TL_ERROR("GSLib GlobalCurveIntersect failed");
      }

			iw_rc = Crv1[i1].Iwbs->GlobalCurveIntersect( 
                            Dom1,
                            *Crv0[i0].Iwbs,
                            Dom0,
                            intersection_tolerance,
                            CCX_Result[i].iwX1 );

      if ( iw_rc == IW_SUCCESS &&  CCX_Result[i].iwX1.GetSize() > 0) 
      {
        //rc = (sSolutions.GetSize() > 1 ) ? TL_SUCCESS+8 : TL_SUCCESS;
        //t = X.m_vStart[0];
      }
      else {
        TL_ERROR("GSLib GlobalCurveIntersect failed");
      }

			
    }
	}
  goose_time = TEST_ElapsedTime();
 }


  ////////////////////////////////////////////
  //
  // Accuracy Test
  //


	ON_Interval Dom[2];
	bool IsClosed[2];
	Dom[0]= Crv0[0].crv->Domain();
	Dom[1] = Crv1[0].crv->Domain();
	IsClosed[0]=Crv0[0].crv->IsClosed()!=0;
	IsClosed[1]=Crv1[0].crv->IsClosed()!=0;

	double min_time = tl_time;

	double tl_error3d =0;
	int tl_int_point_cnt=0;
	int tl_inoverlap_cnt=0;

	double goose_error3d =0;
	int goose_int_point_cnt=0;
	int goose_inoverlap_cnt=0;

	double greg_error3d =0;
	int greg_int_point_cnt=0;
	int greg_inoverlap_cnt=0;

	for(i=0; i<CCX_Result.Count(); i++){
		CCCXTest& Test = CCX_Result[i];

		// 3d error to BasePoint
		int j;
		int jbest0=-1;
		int jbest1=-1;
		double dist;
		double bestdist1 = ON_DBL_MAX;
		bool TLInOverlap0=false;		// set true if the ideal intersection point is
																	// contained in an overlap
		bool TLInOverlap1=false;		// set true if the ideal intersection point is
																	// contained in an overlap
		for( j=0; j<Test.TLX0.Count(); j++){
			if( Test.TLX0[j].type==TL_CCX_POINT){
				dist =	BasePoint.DistanceTo( Test.TLX0[j].A[0]) +
								BasePoint.DistanceTo( Test.TLX0[j].A[1]);
				if(dist<bestdist1){
					bestdist1 = dist;
					jbest0 = j;
				}
			}
			else if(Test.TLX0[j].type==TL_CCX_OVERLAP){
				ON_Interval D0(Test.TLX0[j].a[0], Test.TLX0[j].a[1] );
				D0.m_t[0]-=(1e-6)*fabs(D0.m_t[0]);	// fatten it up
				D0.m_t[1]+=(1e-6)*fabs(D0.m_t[1]);	// fatten it up
				ON_Interval D1(Test.TLX0[j].b[0], Test.TLX0[j].b[1] );
				D1.MakeIncreasing();
				D1.m_t[0]-=(1e-6)*fabs(D1.m_t[0]);	// fatten it up
				D1.m_t[1]+=(1e-6)*fabs(D1.m_t[1]);	// fatten it up
				bool InD0 = D0.Includes(Test.t0);
				if(!InD0 && IsClosed[0] && 
							(Test.t0 == Dom[0][0] || Test.t0 == Dom[0][1])){
					InD0= D0.Includes(Dom[0][0]);
					InD0 = InD0 || D0.Includes(Dom[0][1]);
				}
				bool InD1 = D1.Includes(Test.t1);
				if(!InD1 && IsClosed[1] && 
							(Test.t1 == Dom[0][1] || Test.t1 == Dom[1][1])){
					InD1= D1.Includes(Dom[1][0]);
					InD1 = InD1 || D1.Includes(Dom[1][1]);
				}
				if( InD0 && InD1)
					TLInOverlap0 = true;	
			}
		}
		double bestdist0 = bestdist1;

		j=0;
		jbest1=-1;
		bestdist1 = ON_DBL_MAX;
		for( j=0; j<Test.TLX1.Count(); j++){
			if( Test.TLX1[j].type==TL_CCX_POINT){
				dist =	BasePoint.DistanceTo( Test.TLX1[j].A[0]) +
								BasePoint.DistanceTo( Test.TLX1[j].A[1]);
				if(dist<bestdist1){
					bestdist1 = dist;
					jbest1 = j;
				}
			}
			else if( Test.TLX1[j].type==TL_CCX_OVERLAP ){
				ON_Interval D1(Test.TLX1[j].a[0], Test.TLX1[j].a[1] );
				D1.m_t[0]-=(1e-6)*fabs(D1.m_t[0]);	// fatten it up
				D1.m_t[1]+=(1e-6)*fabs(D1.m_t[1]);	// fatten it up
				ON_Interval D0(Test.TLX1[j].b[0], Test.TLX1[j].b[1] );
				D0.MakeIncreasing();
				D0.m_t[0]-=(1e-6)*fabs(D0.m_t[0]);	// fatten it up
				D0.m_t[1]+=(1e-6)*fabs(D0.m_t[1]);	// fatten it up
				bool InD0 = D0.Includes(Test.t0);
				if(!InD0 && IsClosed[0] && 
							(Test.t0 == Dom[0][0] || Test.t0 == Dom[0][1])){
					InD0= D0.Includes(Dom[0][0]);
					InD0 = InD0 || D0.Includes(Dom[0][1]);
				}
				bool InD1 = D1.Includes(Test.t1);
				if(!InD1 && IsClosed[1] && 
							(Test.t1 == Dom[0][1] || Test.t1 == Dom[1][1])){
					InD1= D1.Includes(Dom[1][0]);
					InD1 = InD1 || D1.Includes(Dom[1][1]);
				}
				if( InD0 && InD1)
					TLInOverlap1 = true;	
			}
		}
		bool NoGoodResult = false;
		if(TLInOverlap0 )
			tl_inoverlap_cnt++;	
		else if( jbest0>=0){
			tl_error3d +=  bestdist0;
			tl_int_point_cnt++;
		}
		else {
			NoGoodResult = true;	
		}
		if(TLInOverlap1 )
			tl_inoverlap_cnt++;	
		else if( jbest1>=0){
			tl_error3d +=  bestdist1;
			tl_int_point_cnt++;
		}
		else {
			NoGoodResult = true;	
		}
		if(NoGoodResult){
				NoGoodResult = true;	// break here
		}


		// Test accuracy of GregCCX results
		// 3d error to BasePoint
		jbest0=-1;
		jbest1=-1;
		bestdist1 = ON_DBL_MAX;
		bool GregInOverlap0=false;		// set true if the ideal intersection point is
																	// contained in an overlap
		bool GregInOverlap1=false;		// set true if the ideal intersection point is
																	// contained in an overlap
		for( j=0; j<Test.TLX2.Count(); j++){
			if( Test.TLX2[j].type==TL_CCX_POINT){
				dist =	BasePoint.DistanceTo( Test.TLX2[j].A[0]) +
								BasePoint.DistanceTo( Test.TLX2[j].A[1]);
				if(dist<bestdist1){
					bestdist1 = dist;
					jbest0 = j;
				}
			}
			else if(Test.TLX2[j].type==TL_CCX_OVERLAP){
				ON_Interval D0(Test.TLX2[j].a[0], Test.TLX2[j].a[1] );
				D0.m_t[0]-=(1e-6)*fabs(D0.m_t[0]);	// fatten it up
				D0.m_t[1]+=(1e-6)*fabs(D0.m_t[1]);	// fatten it up
				ON_Interval D1(Test.TLX2[j].b[0], Test.TLX2[j].b[1] );
				D1.MakeIncreasing();
				D1.m_t[0]-=(1e-6)*fabs(D1.m_t[0]);	// fatten it up
				D1.m_t[1]+=(1e-6)*fabs(D1.m_t[1]);	// fatten it up
				bool InD0 = D0.Includes(Test.t0);
				if(!InD0 && IsClosed[0] && 
							(Test.t0 == Dom[0][0] || Test.t0 == Dom[0][1])){
					InD0= D0.Includes(Dom[0][0]);
					InD0 = InD0 || D0.Includes(Dom[0][1]);
				}
				bool InD1 = D1.Includes(Test.t1);
				if(!InD1 && IsClosed[1] && 
							(Test.t1 == Dom[0][1] || Test.t1 == Dom[1][1])){
					InD1= D1.Includes(Dom[1][0]);
					InD1 = InD1 || D1.Includes(Dom[1][1]);
				}
				if( InD0 && InD1)
					GregInOverlap0 = true;	
			}
		}
		bestdist0 = bestdist1;

		j=0;
		jbest1=-1;
		bestdist1 = ON_DBL_MAX;
		for( j=0; j<Test.TLX3.Count(); j++){
			if( Test.TLX3[j].type==TL_CCX_POINT){
				dist =	BasePoint.DistanceTo( Test.TLX3[j].A[0]) +
								BasePoint.DistanceTo( Test.TLX3[j].A[1]);
				if(dist<bestdist1){
					bestdist1 = dist;
					jbest1 = j;
				}
			}
			else if( Test.TLX3[j].type==TL_CCX_OVERLAP ){
				ON_Interval D1(Test.TLX3[j].a[0], Test.TLX3[j].a[1] );
				D1.m_t[0]-=(1e-6)*fabs(D1.m_t[0]);	// fatten it up
				D1.m_t[1]+=(1e-6)*fabs(D1.m_t[1]);	// fatten it up
				ON_Interval D0(Test.TLX3[j].b[0], Test.TLX3[j].b[1] );
				D0.MakeIncreasing();
				D0.m_t[0]-=(1e-6)*fabs(D0.m_t[0]);	// fatten it up
				D0.m_t[1]+=(1e-6)*fabs(D0.m_t[1]);	// fatten it up
				bool InD0 = D0.Includes(Test.t0);
				if(!InD0 && IsClosed[0] && 
							(Test.t0 == Dom[0][0] || Test.t0 == Dom[0][1])){
					InD0= D0.Includes(Dom[0][0]);
					InD0 = InD0 || D0.Includes(Dom[0][1]);
				}
				bool InD1 = D1.Includes(Test.t1);
				if(!InD1 && IsClosed[1] && 
							(Test.t1 == Dom[0][1] || Test.t1 == Dom[1][1])){
					InD1= D1.Includes(Dom[1][0]);
					InD1 = InD1 || D1.Includes(Dom[1][1]);
				}
				if( InD0 && InD1)
					GregInOverlap1 = true;	
			}
		}
		NoGoodResult = false;
		if(GregInOverlap0 )
			greg_inoverlap_cnt++;	
		else if( jbest0>=0){
			greg_error3d +=  bestdist0;
			greg_int_point_cnt++;
		}
		else {
			NoGoodResult = true;	
		}
		if(GregInOverlap1 )
			greg_inoverlap_cnt++;	
		else if( jbest1>=0){
			greg_error3d +=  bestdist1;
			greg_int_point_cnt++;
		}
		else {
			NoGoodResult = true;	
		}
		if(NoGoodResult){
				NoGoodResult = true;	// break here
		}

		bool GooseInOverlap0=false;
		bool GooseInOverlap1=false;

		jbest0 = -1;
		bestdist0 = ON_DBL_MAX;
		dist = ON_DBL_MAX;
		int count = Test.iwX0.GetSize();
		for( j=0; j<count; j++){
			if( Test.iwX0[j].m_eSolutionType==IW_ST_SINGLE_VALUE){
				double t0 = Test.iwX0[j].m_vStart[0];
				double t1 = Test.iwX0[j].m_vStart[1];
				ON_3dPoint PA = Crv0[Test.i0].crv->PointAt(t0);
				ON_3dPoint PB = Crv1[Test.i1].crv->PointAt(t1);
				dist =	BasePoint.DistanceTo( PA) +
								BasePoint.DistanceTo( PB);
				if(dist<bestdist0){
					bestdist0 = dist;
					jbest0 = j;
				}
			}
			else if(Test.iwX0[j].m_eSolutionType==IW_ST_RANGE_OF_VALUES){
				ON_Interval D0(Test.iwX0[j].m_vStart[0], Test.iwX0[j].m_vEnd[0] );
				D0.m_t[0]-=(1e-6)*fabs(D0.m_t[0]);	// fatten it up
				D0.m_t[1]+=(1e-6)*fabs(D0.m_t[1]);	// fatten it up
				ON_Interval D1(Test.iwX0[j].m_vStart[1], Test.iwX0[j].m_vEnd[1] );
				D1.MakeIncreasing();
				D1.m_t[0]-=(1e-6)*fabs(D1.m_t[0]);	// fatten it up
				D1.m_t[1]+=(1e-6)*fabs(D1.m_t[1]);	// fatten it up
				if( D0.Includes(Test.t0) && D1.Includes(Test.t1))
					GooseInOverlap0 = true;	
			}
		}
		bestdist1 = ON_DBL_MAX;
		jbest1 = -1;
		dist = ON_DBL_MAX;
		count = Test.iwX1.GetSize();
		for( j=0; j<count; j++){
			if( Test.iwX1[j].m_eSolutionType==IW_ST_SINGLE_VALUE){
				double t0 = Test.iwX1[j].m_vStart[1];
				double t1 = Test.iwX1[j].m_vStart[0];
				ON_3dPoint PA = Crv0[Test.i0].crv->PointAt(t0);
				ON_3dPoint PB = Crv1[Test.i1].crv->PointAt(t1);
				dist =	BasePoint.DistanceTo( PA) +
								BasePoint.DistanceTo( PB);
				if(dist<bestdist1){
					bestdist1 = dist;
					jbest1 = j;
				}
			}
			else if(Test.iwX1[j].m_eSolutionType==IW_ST_RANGE_OF_VALUES){
				ON_Interval D0(Test.iwX1[j].m_vStart[0], Test.iwX1[j].m_vEnd[0] );
				D0.m_t[0]-=(1e-6)*fabs(D0.m_t[0]);	// fatten it up
				D0.m_t[1]+=(1e-6)*fabs(D0.m_t[1]);	// fatten it up
				ON_Interval D1(Test.iwX1[j].m_vStart[1], Test.iwX1[j].m_vEnd[1] );
				D1.MakeIncreasing();
				D1.m_t[0]-=(1e-6)*fabs(D1.m_t[0]);	// fatten it up
				D1.m_t[1]+=(1e-6)*fabs(D1.m_t[1]);	// fatten it up
				if( D0.Includes(Test.t1) && D1.Includes(Test.t0))
					GooseInOverlap1 = true;	
			}
		}
	
		NoGoodResult = false;
		if( GooseInOverlap0)
			goose_inoverlap_cnt++;
		else if(jbest0>=0){
			goose_error3d +=  bestdist0;
			goose_int_point_cnt++;
		}
		else
			NoGoodResult = true;

		if( GooseInOverlap1)
			goose_inoverlap_cnt++;
		else if(jbest1>=0){
			goose_error3d +=  bestdist1;
			goose_int_point_cnt++;
		}
		else
			NoGoodResult = true;


		if(NoGoodResult){
			int jjj=7777; // break here 
		}
	}


  if ( goose_time < min_time )
    min_time = goose_time;
  if ( 0.0 == min_time )
    min_time = 1.0/((double)CLOCKS_PER_SEC);
  text_log.Print("Number of global curve curve intersection tests %d\n",N);
  text_log.Print("Code:            Perfection   TL           Goose       Greg\n");
  text_log.Print("Relative Time:    1.00X       %5.2fX       %5.2fX      %5.2fX\n",   tl_time/min_time,  goose_time/min_time, greg_time/min_time);
  text_log.Print("Absolute Time:    0.0 secs    %6.3f secs  %6.3f secs   %6.3f secs\n", tl_time,  goose_time, greg_time);

	text_log.Print("#Points/Overlap:  %d/0        %d/%d       %d/%d        %d/%d  \n", 2*N, 
																				tl_int_point_cnt, 		tl_inoverlap_cnt,  
																				goose_int_point_cnt,  goose_inoverlap_cnt,
																				greg_int_point_cnt,   greg_inoverlap_cnt			);
	if(tl_int_point_cnt>0  &&  goose_int_point_cnt>0  && greg_int_point_cnt>0)
		text_log.Print("Avg Error:        0.00        %.2g        %.2g         %.2g  \n",  tl_error3d/ tl_int_point_cnt,   
																																												goose_error3d/goose_int_point_cnt ,
																																												greg_error3d/ greg_int_point_cnt  );
}

void TestCurveCurveIntersection( const ONX_Model& model, ON_TextLog& text_log )
{
  TEST_HEADER(text_log,"TestCurveCurveIntersection");

  int i;

  double sample_start_distance = 0.0;
  double sample_stop_distance = 0.0;
  
	ON_ClassArray< ON_ClassArray< TestCurve> > Crv;
	ON_ClassArray<ON_wString> Name;										// Input curve names

	// Base Point for all intersections
	ON_3dPoint BasePoint(23.74,-394,15);
	int ci=0;

  if ( model.IsValid() )
  {
    for ( i = 0; i < model.m_object_table.Count(); i++ )
    {
      const ON_Curve* curve = ON_Curve::Cast(model.m_object_table[i].m_object);
      if ( curve )
      {
        const wchar_t* name = model.m_object_table[i].m_attributes.m_name;
        if ( 0 == name || 0 == *name)
        {
          name = L"anonymous";
        }
				Name.Append(name); 

				ON_ClassArray<TestCurve>& CrvFamily = Crv.AppendNew();
			
				GetCurveTranslates(*curve, CrvFamily, BasePoint); 
				ci++;

      }
    }
  }

	ON_SimpleArray<int> permute( Name.Count() );
	permute.SetCount( Name.Count());
	Name.Sort( ON::heap_sort, permute,  ON_CompareIncreasing<ON_wString > );

	for( int ci=0; ci<Name.Count(); ci++){
		ON_wString dump,string;
		ON_TextLog log(dump);
		Crv[permute[ci]][0].crv->Dump(log);

		int si = dump.Find('\n');
		if(si>0){
				string = dump.Left(si);
				dump = dump.Right( dump.Length()-si-1);
				si = dump.Find('\n');
				if( si>0)
					string += dump.Left(si);
		}
		text_log.Print(L"Curve %d:  name = %s, %s\n",ci, Name[permute[ci]],
                  string);
    text_log.Print("\n");
	}

	for(int i0=0; i0<Crv.Count()-1; i0+=2){

    text_log.Print("Intersecting Curve %d with Curve %d.\n", i0,i0+1);	

		text_log.PushIndent();

    text_log.Print("\n");

    TestCCX( text_log, Crv[permute[i0]], Crv[permute[i0+1]], BasePoint );

    text_log.PopIndent();

	}
}


