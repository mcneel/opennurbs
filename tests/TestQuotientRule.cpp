#include "../opennurbs.h"
#include "Tests.h"

class CF
{
public:
  CF() {r = ON_UNSET_VALUE; s = ON_UNSET_VALUE; t = ON_UNSET_VALUE;}
  double r, s, t;
  double f[6]; // = x/wr
  double g[6]; // = y/ws
  double h[6]; // = z/wt
  double x[6];
  double y[6];
  double z[6];
  double wr[6];
  double ws[6];
  double wt[6];
};

class CV
{
public:
  double F, X, W;
};

static class CV func(double r, double s, double t,
                     ON_3dex ijk)
{
  static CF ev;
  if ( r != ev.r )
  {
    ev.x[0] = tan(r);
    ev.x[1] = 1.0+ev.x[0]*ev.x[0];
    ev.x[2] = 2.0*ev.x[0]*ev.x[1];
    ev.x[3] = 2.0*(ev.x[1]*ev.x[1] + ev.x[0]*ev.x[2]);
    ev.x[4] = 2.0*(3.0*ev.x[1]*ev.x[2] + ev.x[0]*ev.x[3]);
    ev.x[5] = 2.0*(3.0*ev.x[2]*ev.x[2] + 4.0*ev.x[1]*ev.x[3] + ev.x[0]*ev.x[4]);
    ev.r = r;

    ev.wr[0] = 1.0/cos(r);
    ev.wr[1] = ev.wr[0]*ev.x[0];
    ev.wr[2] = ev.wr[1]*ev.x[0] + ev.wr[0]*ev.x[1];
    ev.wr[3] = ev.wr[2]*ev.x[0] + 2.0*ev.wr[1]*ev.x[1] + ev.wr[0]*ev.x[2];
    ev.wr[4] = ev.wr[3]*ev.x[0] + 3.0*ev.wr[2]*ev.x[1] + 3.0*ev.wr[1]*ev.x[2] + ev.wr[0]*ev.x[3];
    ev.wr[5] = ev.wr[4]*ev.x[0] + 4.0*ev.wr[3]*ev.x[1] + 6.0*ev.wr[2]*ev.x[2] + 4.0*ev.wr[1]*ev.x[3] + ev.wr[0]*ev.x[4];

    ev.f[0] = sin(r);
    ev.f[1] = cos(r);
    ev.f[2] = -ev.f[0];
    ev.f[3] = -ev.f[1];
    ev.f[4] = -ev.f[2];
    ev.f[5] = -ev.f[3];
  }

  if ( s != ev.s )
  {
    ev.y[0] = exp(3.0*s);
    ev.y[1] = 3.0*ev.y[0];
    ev.y[2] = 9.0*ev.y[0];
    ev.y[3] = 27.0*ev.y[0];
    ev.y[4] = 81.0*ev.y[0];
    ev.y[5] = 243.0*ev.y[0];

    ev.ws[0] = exp(2.0*s);
    ev.ws[1] = 2.0*ev.ws[0];
    ev.ws[2] = 4.0*ev.ws[0];
    ev.ws[3] = 8.0*ev.ws[0];
    ev.ws[4] = 16.0*ev.ws[0];
    ev.ws[5] = 32.0*ev.ws[0];

    ev.g[0] = exp(s);
    ev.g[1] = ev.g[0];
    ev.g[2] = ev.g[0];
    ev.g[3] = ev.g[0];
    ev.g[4] = ev.g[0];
    ev.g[5] = ev.g[0];
    ev.s = s;
  }

  if ( t != ev.t )
  {
    double t2 = t*t;
    double t3 = t*t2;
    double t4 = t*t3;
    double t5 = t*t4;
    double t6 = t*t5;
    double t7 = t*t6;

    ev.z[0] = t7;
    ev.z[1] = 7.0*t6;
    ev.z[2] = 42.0*t5;
    ev.z[3] = 210.0*t4;
    ev.z[4] = 840.0*t3;
    ev.z[5] = 2520.0*t2;

    ev.wt[0] = t5;
    ev.wt[1] = 5.0*t4;
    ev.wt[2] = 20.0*t3;
    ev.wt[3] = 60.0*t2;
    ev.wt[4] = 120.0*t;
    ev.wt[5] = 120.0;

    ev.h[0] = t2;
    ev.h[1] = 2.0*t;
    ev.h[2] = 2.0;
    ev.h[3] = 0.0;
    ev.h[4] = 0.0;
    ev.h[5] = 0.0;
    ev.t = t;
  }


  CV V;
  V.X = ev.x[ijk.i]*ev.y[ijk.j]*ev.z[ijk.k];
  V.W = ev.wr[ijk.i]*ev.ws[ijk.j]*ev.wt[ijk.k];
  V.F = ev.f[ijk.i]*ev.g[ijk.j]*ev.h[ijk.k];
  
  return V;
}

struct TEST_INPUT
{
  int err_count;
  int pass_count;
  double der_tol[6];
  int der_count; // 0 <= der_count <= 5
  int dim;       // dim > 0
  int n;         // 0 <= n < dim
  int v_stride;  // dim < v_stride <= 7
  double v[6*7*8*7];  // >= (dc+1)*(dc+2)*(dc+3)&v_stride dc = max der_count, vs = max v_stride
  double a[6*7*8];    // >= (dc+1)*(dc+2)*(dc+3)
  ON_3dex ijk[6*7*8]; // >= (dc+1)*(dc+2)*(dc+3)

  double maxe1;
  double maxe2;
  double maxe3;
};

static void TestQR1(double r, double s, double t,
                   int coord,
                   TEST_INPUT& TI
                   )
{
  CV V;
  double e;

  int err_count = 0;

  int ii = (coord==0)?1:0;
  int jj = (coord==1)?1:0;
  int kk = (coord==2)?1:0;

  int dc;

  for (dc = 0; dc <= TI.der_count; dc++ )
  {
    TI.ijk[dc].i = ii*dc;
    TI.ijk[dc].j = jj*dc;
    TI.ijk[dc].k = kk*dc;
    V = func(r,s,t, TI.ijk[dc]);
    TI.v[dc*TI.v_stride+TI.n] = V.X;
    TI.v[dc*TI.v_stride+TI.dim] = V.W;
    TI.a[dc] = V.F;
  }

  if ( !ON_EvaluateQuotientRule(TI.dim,TI.der_count,TI.v_stride,TI.v) )
  {
    ON_ERROR("ON_EvaluateQuotientRule - error");
    TI.err_count += (TI.der_count+1);
  }
  else
  {
    for ( dc = 0; dc <= TI.der_count; dc++ )
    {
      e = fabs(TI.a[dc] - TI.v[dc*TI.v_stride+TI.n]);
      if ( e > TI.der_tol[dc] && e > ON_SQRT_EPSILON*fabs(TI.a[dc]) )
      {
        ON_ERROR("ON_EvaluateQuotientRule - error");
        TI.err_count++;
      }
      else
      {
        if ( e > TI.maxe1 )
          TI.maxe1 = e;
        TI.pass_count++;
      }
    }
  }
}


static void TestQR2(double r, double s, double t,
                   int coord,
                   TEST_INPUT& TI
                   )
{
  CV V;
  double e;

  int imax = (coord==0)?0:1;
  int jmax = (coord==1)?0:1;
  int kmax = (coord==2)?0:1;

  int i=0;
  int j=0;
  int k=0;
  int mmax=0;
  int m,der_count;

  for ( der_count = 0; der_count <= TI.der_count; der_count++ )
  {
    for (m = 0; m <= der_count; m++ )
    {
      if ( 0 == coord )
      {
        TI.ijk[mmax].i = 0;
        TI.ijk[mmax].j = der_count-m;
        TI.ijk[mmax].k = m;
      }
      else if ( 1 == coord )
      {
        TI.ijk[mmax].i = der_count-m;
        TI.ijk[mmax].j = 0;
        TI.ijk[mmax].k = m;
      }
      else 
      {
        TI.ijk[mmax].i = der_count-m;
        TI.ijk[mmax].j = m;
        TI.ijk[mmax].k = 0;
      }
      V = func(r,s,t, TI.ijk[mmax]);
      TI.v[mmax*TI.v_stride+TI.n] = V.X;
      TI.v[mmax*TI.v_stride+TI.dim] = V.W;
      TI.a[mmax] = V.F;
      mmax++;
    }
  }

  if ( !ON_EvaluateQuotientRule2(TI.dim,TI.der_count,TI.v_stride,TI.v) )
  {
    ON_ERROR("ON_EvaluateQuotientRule2 - error");
    TI.err_count += mmax;
  }
  else
  {
    for ( m = 0; m < mmax; m++ )
    {
      e = fabs(TI.a[m] - TI.v[m*TI.v_stride+TI.n]);
      der_count = TI.ijk[m].i+TI.ijk[m].j+TI.ijk[m].k;
      if ( e > TI.der_tol[der_count] && e > ON_SQRT_EPSILON*fabs(TI.a[m]) )
      {
        ON_ERROR("ON_EvaluateQuotientRule2 - error");
        TI.err_count++;
      }
      else
      {
        if ( e > TI.maxe2 )
          TI.maxe2 = e;
        TI.pass_count++;
      }
    }
  }
}


static void TestQR3(double r, double s, double t,
                   TEST_INPUT& TI
                   )
{
  CV V;
  double e;

  int i=0;
  int j=0;
  int k=0;
  int mmax=0;
  int m,der_count;

  for ( der_count = 0; der_count <= TI.der_count; der_count++ )
  {
    for ( i = der_count; i >= 0; i-- )
    {
      for ( j = der_count-i; j >= 0; j-- )
      {
        k = der_count-i-j;
        TI.ijk[mmax].i = i;
        TI.ijk[mmax].j = j;
        TI.ijk[mmax].k = k;
        V = func(r,s,t, TI.ijk[mmax]);
        TI.v[mmax*TI.v_stride+TI.n] = V.X;
        TI.v[mmax*TI.v_stride+TI.dim] = V.W;
        TI.a[mmax] = V.F;
        mmax++;
      }
    }
  }

  if ( !ON_EvaluateQuotientRule3(TI.dim,TI.der_count,TI.v_stride,TI.v) )
  {
    ON_ERROR("ON_EvaluateQuotientRule3 - error");
    TI.err_count += mmax;
  }
  else
  {
    for ( m = 0; m < mmax; m++ )
    {
      e = fabs(TI.a[m] - TI.v[m*TI.v_stride+TI.n]);
      der_count = TI.ijk[m].i+TI.ijk[m].j+TI.ijk[m].k;
      if ( e > TI.der_tol[der_count] && e > ON_SQRT_EPSILON*fabs(TI.a[m]) )
      {
        ON_ERROR("ON_EvaluateQuotientRule3 - error");
        TI.err_count++;
      }
      else
      {
        if ( e > TI.maxe3 )
          TI.maxe3 = e;
        TI.pass_count++;
      }
    }
  }
}

bool TestQuotientRule( ON_TextLog& text_log )
{

  double r,s,t;
  struct TEST_INPUT TI;

  memset(&TI,0,sizeof(TI));
  TI.der_tol[0] = 1.0e-12;
  TI.der_tol[1] = 1.0e-11;
  TI.der_tol[2] = 1.0e-10;
  TI.der_tol[3] = 1.0e-9;
  TI.der_tol[4] = 1.0e-8;
  TI.der_tol[5] = 5.0e-7;

  TI.der_count = 0;
  TI.dim = 1;
  TI.n = 0;
  TI.v_stride = TI.dim+1;
  bool ok = true;
  for (TI.der_count=0;        TI.der_count<=5 && ok;  TI.der_count++) 
  for (TI.dim = 1;            TI.dim <= 4 && ok;      TI.dim++ ) 
  for (TI.n= 0;               TI.n < TI.dim && ok;    TI.n++) 
  for (TI.v_stride=TI.dim+1;  TI.v_stride<=7 && ok;   TI.v_stride++)
  {
    memset(TI.v,0,sizeof(TI.v));

    // one paramter tests
    r = 0.25*ON_PI;
    s = 0.0;
    t = 1.0;

    for ( r = -ON_PI/3.0; r <= ON_PI/3.0 && ok; r += ON_PI/120.0)
    {
      TestQR1(r,s,t,0,TI);
      if (TI.err_count > 100 )
        ok = false;
    }

    r = 0.25*ON_PI;
    s = 0.0;
    t = 1.0;

    for ( s = -2.0; s <= 2.0 && ok; s += 0.1 )
    {
      TestQR1(r,s,t,1,TI);
      if (TI.err_count > 100 )
        ok = false;
    }
    
    r = 0.25*ON_PI;
    s = 0.0;
    t = 1.0;

    for ( t = 0.1; t < 2.5 && ok; t += 0.1 )
    {
      TestQR1(r,s,t,2,TI);
      if (TI.err_count > 100 )
        ok = false;
    }

    // two paramter tests
    r = 0.25*ON_PI;
    s = 0.0;
    t = 1.0;

    for ( r = -ON_PI/3.0; r <= ON_PI/3.0 && ok; r += ON_PI/120.0)
    {
      for ( s = -2.0; s <= 2.0 && ok; s += 0.1 )
      {
        TestQR2(r,s,t, 2, TI);
        if (TI.err_count > 100 )
          ok = false;
      }
    }

    r = 0.25*ON_PI;
    s = 0.0;
    t = 1.0;

    for ( r = -ON_PI/3.0; r <= ON_PI/3.0 && ok; r += ON_PI/120.0)
    {
      for ( t = 0.1; t < 2.5 && ok; t += 0.1 )
      {
        TestQR2(r,s,t, 1, TI);
        if (TI.err_count > 100 )
          ok = false;
      }
    }

    r = 0.25*ON_PI;
    s = 0.0;
    t = 1.0;

    for ( s = -2.0; s <= 2.0 && ok; s += 0.1 )
    {
      for ( t = 0.1; t < 2.5 && ok; t += 0.1 )
      {
        TestQR2(r,s,t, 1, TI);
        if (TI.err_count > 100 )
          ok = false;
      }
    }

    // three paramter tests
    r = 0.25*ON_PI;
    s = 0.0;
    t = 1.0;

    for ( r = -ON_PI/3.0; r <= ON_PI/3.0 && ok; r += ON_PI/120.0)
    {
      for ( s = -2.0; s <= 2.0 && ok; s += 0.1 )
      {
        for ( t = 0.1; t < 2.5 && ok; t += 0.1 )
        {
          TestQR3(r,s,t, TI);
          if (TI.err_count > 100 )
            ok = false;
        }
      }
    }
  }

  text_log.Print("pass_count = %d  err_count = %d  maxe1 = %g  maxe2 = %g  maxe3 = %g\n",
    TI.pass_count,TI.err_count,TI.maxe1,TI.maxe2,TI.maxe3);

  return (TI.err_count == 0);
}
