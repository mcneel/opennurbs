#include "../opennurbs.h"
#include "../opennurbs_extensions.h"

#include "Tests.h"

static
bool TestNurbsCageEvaluateHelper( ON_TextLog& text_log, const ON_NurbsCage& cage )
{
  ON_Workspace ws;

  int err_count = 0;
  int pass_count = 0;
  bool ok = true;
  const int der_count = 3; // <= 3
  const int v_stride = 3;
  ON_NurbsSurface srfst, srfrs, srfrt;
  int cage_hint[3] = {0,0,0};
  int srfst_hint[2] = {0,0};
  int srfrs_hint[2] = {0,0};
  int srfrt_hint[2] = {0,0};
  ON_3dVector cageV,stV,rsV,rtV;

  ON_3dVector *cagev, *stv, *rsv, *rtv;

  cagev = (ON_3dVector*)onmalloc(sizeof(*cagev)*(der_count+1)*(der_count+2)*(der_count+3)/6);
  stv = (ON_3dVector*)onmalloc(sizeof(*stv)*(der_count+1)*(der_count+2)/2);
  rsv = (ON_3dVector*)onmalloc(sizeof(*rsv)*(der_count+1)*(der_count+2)/2);
  rtv = (ON_3dVector*)onmalloc(sizeof(*rtv)*(der_count+1)*(der_count+2)/2);

  int i,j,k,ii,jj,kk,di,dj,dk,dc;
  double r,s,t,errst,errrt,errrs,tol;
  for ( i = cage.m_order[0]-2; ok&&i < cage.m_cv_count[0]-1; i++ )
  {
    ON_Interval rspan(cage.m_knot[0][i],cage.m_knot[0][i+1]);
    if ( rspan.Length() <= 0.0 )
      continue;
    for ( ii = 0; ok && ii <= 4; ii++ )
    {
      r = rspan.ParameterAt(0.25*ii);
      if (!cage.IsoSurface(0,r,&srfst))
      {
        err_count++;
        ON_ERROR("ON_NurbsCage::IsoSurface(dir=1,...) failed");
        ok = false;
      }

      for ( j = cage.m_order[1]-2; ok && j < cage.m_cv_count[1]-1; j++ )
      {
        ON_Interval sspan(cage.m_knot[1][j],cage.m_knot[1][j+1]);
        if ( sspan.Length() <= 0.0 )
          continue;
        for ( jj = 0; ok && jj <= 4; jj++ )
        {
          s = sspan.ParameterAt(0.25*jj);
          if ( !cage.IsoSurface(1,s,&srfrt) )
          {
            err_count++;
            ON_ERROR("ON_NurbsCage::IsoSurface(dir=1,...) failed");
            ok = false;
          }

          for ( k = cage.m_order[2]-2; ok && k < cage.m_cv_count[2]-1; k++ )
          {
            ON_Interval tspan(cage.m_knot[2][k],cage.m_knot[2][k+1]);
            if ( tspan.Length() <= 0.0 )
              continue;        
            for ( kk = 0; ok && kk <= 4; kk++ )
            {
              t = tspan.ParameterAt(0.25*kk);
              if( !cage.IsoSurface(2,t,&srfrs) )
              {
                err_count++;
                ON_ERROR("ON_NurbsCage::IsoSurface(dir=2,...) failed");
                ok = false;
              }

              if ( !cage.Evaluate(r,s,t,der_count,v_stride,&cagev[0].x,0,cage_hint))
              {
                err_count++;
                ON_ERROR("ON_NurbsCage::Evaluate - invalid cage");
                ok = false;
              }

              if ( !srfst.Evaluate(s,t,der_count,v_stride,&stv[0].x,0,srfst_hint))
              {
                err_count++;
                ON_ERROR("ON_NurbsCage::IsoSurface(dir=0,...) returned invalid surface");
                ok = false;
              }

              if ( !srfrt.Evaluate(s,t,der_count,v_stride,&rtv[0].x,0,srfrt_hint))
              {
                err_count++;
                ON_ERROR("ON_NurbsCage::IsoSurface(dir=1,...) returned invalid surface");
                ok = false;
              }

              if ( !srfrs.Evaluate(s,t,der_count,v_stride,&rsv[0].x,0,srfrs_hint))
              {
                err_count++;
                ON_ERROR("ON_NurbsCage::IsoSurface(dir=2,...) returned invalid surface");
                ok = false;
              }


              for ( dc = 0; dc <= der_count; dc++ )
              {
                tol = pow(10.0,dc-11);
                for ( di = dc; di >= 0; di--)
                {
                  for (dj = dc-di; dj >= 0; dj--)
                  {
                    dk = dc-di-dj;
                    if ( di && dj && dk )
                      continue;
                    cageV = cagev[dc*(dc+1)*(dc+2)/6 + (dj+dk)*(dj+dk+1)/2 + dk];
                    stV = (di) ? cageV : stv[(dj+dk)*(dj+dk+1)/2 + dk];
                    rtV = (dj) ? cageV : stv[(di+dk)*(di+dk+1)/2 + dk];
                    rsV = (dk) ? cageV : stv[(di+dj)*(di+dj+1)/2 + dj];
                    errst = (cageV-stV).MaximumCoordinate();
                    errrt = (cageV-rtV).MaximumCoordinate();
                    errrs = (cageV-rsV).MaximumCoordinate();
                    if ( errst > tol && errrt > tol && errrs > tol )
                    {
                      err_count++;
                      ON_ERROR("ON_NurbsCage::Evaluate - bad value");
                      if ( err_count > 20 )
                        ok = false;
                    }
                    else
                    {
                      pass_count++;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  onfree(rtv);
  onfree(rsv);
  onfree(stv);
  onfree(cagev);

  return ok;
}

bool TestNurbsCageEvaluate( ON_TextLog& text_log )
{
  bool ok = false;

  ONX_Model model;
  const char* sFileName = "..\\TEstNurbsCageEvaluate.3dm";
  FILE* fp = ON::OpenFile( sFileName, "rb");

  bool bModelRead = false;
  bool bModelIsValid = false;

  if ( 0 != fp )
  {
    ON_BinaryFile archive( ON::read3dm, fp );
    bModelRead = model.Read( archive, &text_log );
    ON::CloseFile( fp );
  }

  if ( !bModelRead )
  {
    text_log.Print("TestNurbsCageEvaluate cannot read %s\n",sFileName);
  }
  else
  {
    int i;
    for ( i = 0; i < model.m_object_table.Count(); i++)
    {
      const ONX_Model_Object& mo = model.m_object_table[i];
      const ON_NurbsCage* model_cage = ON_NurbsCage::Cast(mo.m_object);
      if ( !model_cage )
        continue;
      ok = TestNurbsCageEvaluateHelper( text_log, *model_cage );
      if ( !ok )
        break;
    }
  }

  return ok;
}

