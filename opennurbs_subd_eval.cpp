#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_subd_data.h"

#if defined(OPENNURBS_SUBD_WIP)

ON_SubD* ON_SubDSectorType::SectorRingSubD(
  double radius,
  double sector_angle_radians,
  ON_SubD* subd
  ) const
{
  if (subd)
    *subd = ON_SubD::Empty;

  if (!IsValid())
    return ON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int R = PointRingCount();
  if (R < 3)
    return ON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int F = FaceCount();
  if ( F < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int N = EdgeCount();
  if (N < 2)
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (F != N && F + 1 != N)
    return ON_SUBD_RETURN_ERROR(nullptr);

  const ON_SubD::SubDType subdivision_type = SubDType();
  const ON_SubD::VertexTag vertex_tag = VertexTag();

  const unsigned f_edge_count = ON_SubD::FacetEdgeCount(subdivision_type);
  if (3 != f_edge_count && 4 != f_edge_count)
    return ON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int ring_ei_delta = f_edge_count - 2;

  if (nullptr == subd)
    subd = new ON_SubD;

  ON_SubD::VertexTag vertex_tag0;
  ON_SubD::VertexTag vertex_tag1;
  ON_SubD::EdgeTag edge_tag0;
  ON_SubD::EdgeTag edge_tag1;

  switch (vertex_tag)
  {
  case ON_SubD::VertexTag::Smooth:
    sector_angle_radians = 2.0*ON_PI;
    vertex_tag0 = ON_SubD::VertexTag::Smooth;
    vertex_tag1 = ON_SubD::VertexTag::Smooth;
    edge_tag0 = ON_SubD::EdgeTag::Smooth;
    edge_tag1 = ON_SubD::EdgeTag::Smooth;
    break;

  case ON_SubD::VertexTag::Crease:
    if ( !(sector_angle_radians > 0.0 && sector_angle_radians < 2.0*ON_PI) )
      sector_angle_radians = 0.5*ON_PI;
    vertex_tag0 = ON_SubD::VertexTag::Crease;
    vertex_tag1 = ON_SubD::VertexTag::Crease;
    edge_tag0 = ON_SubD::EdgeTag::Crease;
    edge_tag1 = ON_SubD::EdgeTag::Crease;
    break;

  case ON_SubD::VertexTag::Corner:
    sector_angle_radians = CornerSectorAngleRadians();
    vertex_tag0 = ON_SubD::VertexTag::Crease;
    vertex_tag1 = ON_SubD::VertexTag::Crease;
    edge_tag0 = ON_SubD::EdgeTag::Crease;
    edge_tag1 = ON_SubD::EdgeTag::Crease;
    break;

  case ON_SubD::VertexTag::Dart:
    sector_angle_radians = 2.0*ON_PI;
    vertex_tag0 = ON_SubD::VertexTag::Crease;
    vertex_tag1 = ON_SubD::VertexTag::Smooth;
    edge_tag0 = ON_SubD::EdgeTag::Crease;
    edge_tag1 = ON_SubD::EdgeTag::Smooth;
    break;

  default:
    return ON_SUBD_RETURN_ERROR(nullptr);
    break;
  }

  unsigned int sector_angle_index = ON_SubDSectorType::AngleIndexFromAngleRadians(sector_angle_radians);
  if (sector_angle_index <= ON_SubDSectorType::MaximumAngleIndex
    && fabs(ON_SubDSectorType::AngleRadiansFromAngleIndex(sector_angle_index) - sector_angle_radians) <= 1.0e-6
    )
  {
    sector_angle_radians = ON_SubDSectorType::AngleRadiansFromAngleIndex(sector_angle_index);
  }
  else
  {
    sector_angle_radians = ON_UNSET_UINT_INDEX;
  }
  
  const double smooth_edge_w0 = this->SectorWeight();

  ON_SimpleArray< ON_SubDVertex* > V(R);
  ON_SimpleArray< ON_SubDEdge* > E(N);

  ON_3dPoint vertexP = ON_3dPoint::Origin;

  for (unsigned int vi = 0; vi < R; vi++)
  {
    ON_SubD::VertexTag vertex_tag_vi;
    if ( 0 == vi )
      vertex_tag_vi = vertex_tag; // center vertex
    else if ( 1 == vi )
      vertex_tag_vi = vertex_tag0; // first edge
    else if ( R == vi+1 && N > F )
      vertex_tag_vi = vertex_tag1; // last edge
    else
      vertex_tag_vi = ON_SubD::VertexTag::Smooth; // interior edge or an outer face vertex

    if (radius > 0.0)
    {
      double cos_a, sin_a;
      if (sector_angle_index == ON_UNSET_UINT_INDEX)
      {
        double a = (vi / ((double)(R-1)))*sector_angle_radians;
        cos_a = cos(a);
        sin_a = sin(a);
      }
      else
      {
        ON_SubDMatrix::EvaluateCosAndSin(2*sector_angle_index*vi, (R-1)*ON_SubDSectorType::MaximumAngleIndex,&cos_a,&sin_a);
      }
      const double r = (3 == f_edge_count) || (1 == (vi%2)) ? radius : (2.0*radius);
      vertexP.x = r*cos_a;
      vertexP.y = r*sin_a;
    }
    ON_SubDVertex* vertex = subd->AddVertex( vertex_tag_vi, vertexP);
    V.Append(vertex);
  }
  //V[0]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::radial;
  
  for (unsigned int vei = 0; vei < N; vei++)
  {
    ON_SubD::EdgeTag edge_tag_vei;
    if ( 0 == vei )
      edge_tag_vei = edge_tag0; // first edge
    else if ( vei+1 == N )
      edge_tag_vei = edge_tag1; // last edge
    else
      edge_tag_vei = ON_SubD::EdgeTag::Smooth; // interior edge

    double w0 = (ON_SubD::EdgeTag::Smooth == edge_tag_vei) ? smooth_edge_w0 : ON_SubDSectorType::IgnoredSectorWeight;
    unsigned int ev1i = 1 + vei*ring_ei_delta;
    E.Append(
      subd->AddEdgeWithSectorCoefficients(
        edge_tag_vei,
        V[0], w0,
        V[ev1i], ON_SubDSectorType::IgnoredSectorWeight)
        );
  }

  ON_SubDVertex* f_vertex[4] = {};
  ON_SubDEdge* f_edge[4] = {};
  ON_SubDEdgePtr f_edgeptr[4] = {};

  f_vertex[0] = V[0];
  f_vertex[f_edge_count - 1] = const_cast<ON_SubDVertex*>(E[0]->m_vertex[1]);
  f_edge[f_edge_count - 1] = E[0];
  for (unsigned int vfi = 0; vfi < F; vfi++)
  {
    f_edge[0] = f_edge[f_edge_count - 1];
    f_edge[f_edge_count-1] = E[(vfi + 1) % N];
    f_vertex[1] = const_cast<ON_SubDVertex*>(f_edge[0]->m_vertex[1]);
    f_vertex[f_edge_count - 1] = const_cast<ON_SubDVertex*>(f_edge[f_edge_count - 1]->m_vertex[1]);

    f_edgeptr[0] = ON_SubDEdgePtr::Create(f_edge[0], 0);
    f_edgeptr[f_edge_count - 1] = ON_SubDEdgePtr::Create(f_edge[f_edge_count - 1], 1);
    if (4 == f_edge_count)
    {
      f_vertex[2] = V[2 + 2 * vfi];
      f_edge[1] = subd->AddEdgeWithSectorCoefficients(ON_SubD::EdgeTag::Smooth, f_vertex[1], ON_SubDSectorType::IgnoredSectorWeight, f_vertex[2], ON_SubDSectorType::IgnoredSectorWeight);
      f_edge[2] = subd->AddEdgeWithSectorCoefficients(ON_SubD::EdgeTag::Smooth, f_vertex[2], ON_SubDSectorType::IgnoredSectorWeight, f_vertex[3], ON_SubDSectorType::IgnoredSectorWeight);
      f_edgeptr[1] = ON_SubDEdgePtr::Create(f_edge[1], 0);
      f_edgeptr[2] = ON_SubDEdgePtr::Create(f_edge[2], 0);
    }
    else
    {
      f_edge[1] = subd->AddEdgeWithSectorCoefficients(ON_SubD::EdgeTag::Smooth, f_vertex[1], ON_SubDSectorType::IgnoredSectorWeight, f_vertex[2], ON_SubDSectorType::IgnoredSectorWeight);
      f_edgeptr[1] = ON_SubDEdgePtr::Create(f_edge[1], 0);
    }
    subd->AddFace(f_edge_count, f_edgeptr);
  }

  return subd;
}

static bool TestPoint(
  const ON_3dPoint* SP, 
  unsigned int SPi,
  ON_3dPoint Q,
  unsigned int Pi,
  double* e,
  unsigned int* ei
  )
{
  ON_3dPoint P = SP[SPi];
  double z = fabs(P[(Pi + 1) % 3]) + fabs(P[(Pi + 1) % 3])+ fabs(Q[(Pi + 1) % 3]) + fabs(Q[(Pi + 1) % 3]);
  if (!(0.0 == z))
  {
    // ON_ERROR("point coordinate is not zero.");
    return ON_SUBD_RETURN_ERROR(false);
  }
  if (fabs(P[Pi]) > 1.0)
  {
    // ON_ERROR("point coordinate P[Pi] > 1.");
    return ON_SUBD_RETURN_ERROR(false);
  }
  if (fabs(Q[Pi]) > 1.0)
  {
    // ON_ERROR("point coordinate Q[Pi] > 1.");
    return ON_SUBD_RETURN_ERROR(false);
  }

  double d = fabs(P[Pi] - Q[Pi]);
  if (d > e[Pi])
  {
    e[Pi] = d;
    *ei = SPi;
#if defined(ON_DEBUG)
    if (d > 0.0001)
    {
      // almost certainly a bug
      ON_SubDIncrementErrorCount();
    }
#endif
  }

  return true;
}

//double ON_QNaN()
//{
//  const char* sIgnored = "";
//  return nan(sIgnored);
//}
//
//bool ON_IsNaN(double x)
//{
//  return (0 != isnan(x));
//}
//
//void ON_SetToQNaN(double* x)
//{
//  if (nullptr != x)
//    *x = ON_QNaN();
//}
//
//bool ON_IsNaN(ON_3dPoint& point)
//{
//  return (ON_IsNaN(point.x) || ON_IsNaN(point.y) || ON_IsNaN(point.z));
//}
//
//void ON_SetToQNaN(ON_3dPoint* point)
//{
//  if (nullptr != point)
//  {
//    point->x = ON_QNaN();
//    point->y = ON_QNaN();
//    point->z = ON_QNaN();
//  }
//}

static bool ClearCachedPoints(
  unsigned int component_ring_count,
  const ON_SubDComponentPtr* component_ring
  )
{
  if ( component_ring_count < 4 || nullptr == component_ring)
    return ON_SUBD_RETURN_ERROR(false);
  ON_SubDVertex* vertex = component_ring[0].Vertex();
  if ( nullptr == vertex)
    return ON_SUBD_RETURN_ERROR(false);
  vertex->ClearSavedSubdivisionPoint();
  vertex->ClearSavedLimitPoints();
  for (unsigned int i = 1; i < component_ring_count; i++)
  {
    ON_SubDEdge* edge = component_ring[i].Edge();
    if ( nullptr == edge)
      return ON_SUBD_RETURN_ERROR(false);
    edge->ClearSavedSubdivisionPoint();
    i++;
    if (i >= component_ring_count)
      break;
    ON_SubDFace* face = component_ring[i].Face();
    if ( nullptr == face)
      return ON_SUBD_RETURN_ERROR(false);
    face->ClearSavedSubdivisionPoint();
  }
  return true;
}

double ON_SubDMatrix::TestEvaluation() const
{
  if ( nullptr == m_S || m_R < 3 )
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

  if (!m_sector_type.IsValid())
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

  const ON_SubD::SubDType subd_type = m_sector_type.SubDType();
  //ON_SubD::VertexTag center_vertex_tag = m_sector_type.VertexTag();

  const unsigned int F = m_sector_type.FaceCount();
  if (0 == F)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

  const unsigned int N = m_sector_type.EdgeCount();
  if (0 == N)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

  const unsigned int R = m_sector_type.PointRingCount();
  if (0 == R)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  if (R != m_R)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  
  const unsigned f_edge_count = m_sector_type.FacetEdgeCount();
  if (0 == f_edge_count)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

  double rc = TestMatrix();
  const double*const* S = m_S;
  
  unsigned int SP_low_precision_index = ON_UNSET_UINT_INDEX;
  
  ON_SimpleArray< ON_3dPoint > _P(R);
  ON_3dPoint* SP = _P.Array();

  ON_SimpleArray< double > _Scol(R);
  double* Scol = _Scol.Array();

  ON_SubD subd;
  if (&subd != m_sector_type.SectorRingSubD(0.0,0.0,&subd))
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  const ON_SubDVertex* vertex0 = subd.FirstVertex();
  if (nullptr == vertex0)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  if (N != vertex0->m_edge_count)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  if (F != vertex0->m_face_count)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  
  ON_SubDSectorIterator sit;
  if ( nullptr == sit.Initialize(vertex0) )
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

  ON_SimpleArray<const ON_SubDVertex*> vertex_ring_array(subd.VertexCount());
  for (const ON_SubDVertex* vertex = vertex0; nullptr != vertex; vertex = vertex->m_next_vertex)
  {
    vertex_ring_array.Append(vertex);
  }
  if ( R != vertex_ring_array.UnsignedCount())
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  const ON_SubDVertex*const* vertex_ring = vertex_ring_array.Array();

  ON_SimpleArray<ON_SubDComponentPtr> component_ring_array;
  const unsigned int component_ring_count = ON_SubD::GetSectorComponentRing(sit,component_ring_array);
  if ( component_ring_count < 4 || component_ring_count != m_sector_type.ComponentRingCount())
    return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
  const ON_SubDComponentPtr* component_ring = component_ring_array.Array();

  ON_SimpleArray< ON_3dPoint > _ringP0;

  ON_SimpleArray< ON_3dPoint > _ringP1;

  for (unsigned int vi = 0; vi < R; vi++)
    Scol[vi] = ON_DBL_QNAN;

  for (unsigned int vi = 0; vi < R; vi++)
  {
    double N_vertex_point_precision[3] = { 0 };
    double N_outer_point_precision[3] = { 0 };
    double N_Scol_precision[3] = { 0 };

    for (unsigned int Pi = 0; Pi < 3; Pi++)
    {
      if (false == ClearCachedPoints(component_ring_count,component_ring))
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

      const_cast<ON_SubDVertex*>(vertex_ring[vi])->m_P[Pi] = 1.0;

      if ( R != ON_SubD::GetSectorPointRing(subd_type,false,component_ring_count,component_ring,_ringP0))
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      const ON_3dPoint* ringP0 = _ringP0.Array();

      // vertex_ring[]->m_P and ringP0[] should be same point lists
      for (unsigned int i = 0; i < R; i++)
      {
        if (0.0 == ringP0[i][(Pi+1)%3] && 0.0 == ringP0[i][(Pi+2)%3])
        {
          if ( ringP0[i][Pi] == ((i == vi) ? 1.0 : 0.0) )
            continue;
        }
        // vertex_ring[] is not in the expected order or
        // there is a bug in ON_SubD::GetSectorPointRing
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      }

      if ( R != ON_SubD::GetSectorSubdivisionPointRing(subd_type,component_ring_count,component_ring,_ringP1))
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      const ON_3dPoint* ringP1 = _ringP1.Array();

      for (unsigned int i = 0; i < R; i++)
      {
        SP[i] = ON_3dPoint::Origin;
        for (unsigned int j = 0; j < R; j++)
        {
          SP[i] += S[i][j] * ringP0[j];
        }
      }

      if (!(SP[vi][Pi] > 0.0))
      {
        // ON_ERROR("SP[vi][Pi] is not positive.");
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      }

      if (false == TestPoint(SP, 0, ringP1[0], Pi, N_vertex_point_precision, &SP_low_precision_index))
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);

      for (unsigned int j = 1; j < R; j++)
      {
        if (false == TestPoint(SP, j, ringP1[j], Pi, N_outer_point_precision, &SP_low_precision_index))
          return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      }

      for (unsigned int i = 0; i < R; i++)
      {
        double d = fabs(S[i][vi] - ringP1[i][Pi]);
        if (d > N_Scol_precision[Pi])
          N_Scol_precision[Pi] = d;
      }

      if (!(N_vertex_point_precision[0] == N_vertex_point_precision[Pi]))
      {
        ON_ERROR("x,y,z vertex point precisions are not identical.");
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      }
      if (!(N_outer_point_precision[0] == N_outer_point_precision[Pi]))
      {
        ON_ERROR("x,y,z outer point precisions are not identical.");
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      }
      if (!(N_Scol_precision[0] == N_Scol_precision[Pi]))
      {
        ON_ERROR("x,y,z S column precisions are not identical.");
        return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
      }

      if (rc < N_vertex_point_precision[0])
        rc = N_vertex_point_precision[0];
      if (rc < N_outer_point_precision[0])
        rc = N_outer_point_precision[0];
      if (rc < N_Scol_precision[0])
        rc = N_Scol_precision[0];

      const_cast<ON_SubDVertex*>(vertex_ring[vi])->m_P[Pi] = 0.0;
    }
  }

  return rc; // basic tests passed.
}

static bool GetSectorLimitPointHelper(
  ON_SubD::SubDType subdivision_type,
  const ON_SubDSectorIterator& sit,
  ON_SubDSectorLimitPoint& limit_point
  )
{
  const ON_SubDSectorType sector_type = ON_SubDSectorType::Create(subdivision_type,sit);
  if (false == sector_type.IsValid())
    return  ON_SUBD_RETURN_ERROR(false);

  const unsigned int R = sector_type.PointRingCount();
  if (R < 3)
    return  ON_SUBD_RETURN_ERROR(false);

  double stack_point_ring[41*3];
  double* point_ring = stack_point_ring;
  const unsigned int point_ring_stride = 3;
  unsigned int point_ring_capacity = (unsigned int)(sizeof(stack_point_ring)/(point_ring_stride*sizeof(stack_point_ring[0])));

  if (point_ring_capacity < R )
  {
    point_ring = new(std::nothrow) double[point_ring_stride*R];
    if ( nullptr == point_ring)
      return  ON_SUBD_RETURN_ERROR(false);
    point_ring_capacity = R;
  }

  if ( R != ON_SubD::GetSectorPointRing(subdivision_type,true,sit,point_ring_capacity,point_ring_stride,point_ring) )
     return  ON_SUBD_RETURN_ERROR(false);

  bool rc = false;
  for (;;)
  {
    const ON_SubDMatrix& SM = ON_SubDMatrix::FromCache(sector_type);
    if (R != SM.m_R || nullptr == SM.m_LP)
      break;

    if (false == SM.EvaluateLimitPoint(R, point_ring_stride, point_ring, limit_point))
      break;

    rc = true;
    break;
  }

  if ( point_ring != stack_point_ring)
    delete[] point_ring;

  return rc
    ? true
    : ON_SUBD_RETURN_ERROR(false);
}

static ON_SubDSectorLimitPoint* LimitPointPool(
  const ON_SubDSectorLimitPoint* pReturnToPool // If nullptr, then one is allocated
  )
{
  static ON_FixedSizePool limit_point_fsp;
  if (0 == limit_point_fsp.SizeofElement())
  {
    if (nullptr != pReturnToPool)
      return ON_SUBD_RETURN_ERROR(nullptr);
    limit_point_fsp.Create(sizeof(ON_SubDSectorLimitPoint), 0, 0);
  }

  if (nullptr != pReturnToPool)
  {
    limit_point_fsp.ReturnElement((void*)pReturnToPool);
    return nullptr;
  }

  ON_SubDSectorLimitPoint* lp = (ON_SubDSectorLimitPoint*)limit_point_fsp.AllocateDirtyElement();  
  if (nullptr == lp)
    return ON_SUBD_RETURN_ERROR(nullptr);
  return lp;
}

bool ON_SubDVertex::GetLimitPoint(
  ON_SubD::SubDType subd_type,
  const ON_SubDFace* sector_face,
  bool bUseSavedLimitPoint,
  ON_SubDSectorLimitPoint& limit_point
  ) const
{
  bool rc = false;
  ON_SubDSectorIterator sit;
  const ON_SubDFace* limit_point_sector_face = nullptr;
  
  if (nullptr != sector_face)
  {
    for (unsigned int vfi = 0; vfi < m_face_count; vfi++)
    {
      if (sector_face == m_faces[vfi])
      {
        rc = true;
        break;
      }
    }
    if (false == rc)
    {
      // sector_face is not referenced by this vertex
      limit_point = ON_SubDSectorLimitPoint::Unset;
      return ON_SUBD_RETURN_ERROR(false);
    }
  }
  
  if (bUseSavedLimitPoint && subd_type == SavedLimitPointType() )
  {
    if (nullptr == m_limit_point.m_sector_face)
    {
      // single sector
      limit_point = m_limit_point;
      limit_point.m_next_sector_limit_point = nullptr;
      return true;
    }

    if (nullptr == sector_face)
    {
      // this vertex has multiple sectors
      limit_point = ON_SubDSectorLimitPoint::Unset;
      return ON_SUBD_RETURN_ERROR(false);
    }

    if (nullptr == sit.Initialize(sector_face, 0, this))
    {
      limit_point = ON_SubDSectorLimitPoint::Unset;
      return ON_SUBD_RETURN_ERROR(false);
    }

    limit_point_sector_face = sit.IncrementToCrease(-1);

    if (nullptr == limit_point_sector_face)
    {
      // no creases 
      limit_point = ON_SubDSectorLimitPoint::Unset;
      return ON_SUBD_RETURN_ERROR(false);
    }
    
    for (const ON_SubDSectorLimitPoint* lp = &m_limit_point; nullptr != lp; lp = lp->m_next_sector_limit_point)
    {
      if (limit_point_sector_face == lp->m_sector_face)
      {
        limit_point = *lp;
        limit_point.m_next_sector_limit_point = nullptr;
        return true;
      }
    }

    // cache does not contain this limit point.
  }


  if (nullptr == (nullptr == sector_face ? sit.Initialize(this) : sit.Initialize(sector_face, 0, this)))
  {
    limit_point = ON_SubDSectorLimitPoint::Unset;
    return ON_SUBD_RETURN_ERROR(false);
  }

  if (nullptr == sit.Initialize(sector_face,0,this))
  {
    limit_point = ON_SubDSectorLimitPoint::Unset;
    return ON_SUBD_RETURN_ERROR(false);
  }

  limit_point_sector_face = sit.IncrementToCrease(-1);

  rc = GetSectorLimitPointHelper(subd_type, sit, limit_point);

  if (false == rc)
  {
    limit_point = ON_SubDSectorLimitPoint::Unset;
    return ON_SUBD_RETURN_ERROR(false);
  }

  limit_point.m_sector_face = limit_point_sector_face;
  
  if (bUseSavedLimitPoint)
  {
    ON_SubDSectorLimitPoint saved_limit_point = limit_point;
    saved_limit_point.m_next_sector_limit_point = (ON_SubDSectorLimitPoint*)1; // causes unnecessary test to be skipped
    SetSavedLimitPoint(subd_type, saved_limit_point);
  }

  return rc;
}

static bool SetLimitPointSectorCheck(
  const ON_SubDVertex* vertex,
  ON_SubDSectorLimitPoint& limit_point
  )
{
  const unsigned int vertex_face_count = vertex->m_face_count;
  if ( vertex_face_count < 1 || nullptr == vertex->m_faces)
      return ON_SUBD_RETURN_ERROR(false);

  ON_SubDSectorIterator sit;

  const ON_SubDFace* limit_point_sector_face = limit_point.m_sector_face;

  if (nullptr != limit_point_sector_face)
  {
    bool rc = false;
    for (unsigned int vfi = 0; vfi < vertex_face_count; vfi++)
    {
      if (limit_point_sector_face == vertex->m_faces[vfi])
      {
        rc = true;
        break;
      }
    }
    if (false == rc)      
      return ON_SUBD_RETURN_ERROR(false); // sector_face is not referenced by this vertex
    if (nullptr == sit.Initialize(limit_point_sector_face, 0, vertex))
      return ON_SUBD_RETURN_ERROR(false);
  }
  else if (nullptr == sit.Initialize(vertex))
    return ON_SUBD_RETURN_ERROR(false);

  limit_point_sector_face = sit.IncrementToCrease(-1);
  unsigned int sector_face_count = 0;
  const ON_SubDFace* sector_face0 = sit.CurrentFace();
  for (const ON_SubDFace* sector_face = sector_face0; nullptr != sector_face && sector_face_count <= vertex_face_count; sector_face = sit.NextFace(true))
  {
    if (sector_face == sector_face0 && sector_face_count == vertex_face_count && vertex->IsSmoothOrDart())
    {
      // interior vertex
      limit_point_sector_face = nullptr;
      break;
    }
    sector_face_count++;
  }

  if (sector_face_count > vertex_face_count)
  {
    // error in topology information
    return ON_SUBD_RETURN_ERROR(false);
  }

  if (sector_face_count == vertex_face_count)
  {
    // vertex has 1 sector (bounded or interior)
    limit_point_sector_face = nullptr;
  }
  else if (nullptr == limit_point_sector_face )
  {
    // vertex has multiple sectors and
    // limit_point.m_sector_face must be the "first" face in the sector
    return ON_SUBD_RETURN_ERROR(false);
  }

  limit_point.m_sector_face = limit_point_sector_face;
  return true;
}

bool ON_SubDVertex::SetSavedLimitPoint(
  ON_SubD::SubDType subd_type,
  ON_SubDSectorLimitPoint limit_point
  ) const
{
  const bool bSkipSectorCheck = (1U == (ON__UINT_PTR)limit_point.m_next_sector_limit_point);
  limit_point.m_next_sector_limit_point = nullptr;

  if ( ON_SubD::SubDType::Unset != subd_type 
    && limit_point.IsSet()
    && (bSkipSectorCheck || SetLimitPointSectorCheck(this,limit_point))
    )
  {
    if (nullptr == limit_point.m_sector_face 
      || ON_UNSET_VALUE == m_limit_point.m_limitP[0]
      || 0 == ON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags)
      )
    {
      // vertex has 1 sector or this is the first cached limit point
      ClearSavedLimitPoints();
      m_limit_point = limit_point;
      m_limit_point.m_next_sector_limit_point = nullptr;
    }
    else
    {
      // get a point from the pool
      ON_SubDSectorLimitPoint* lp = LimitPointPool(nullptr);
      if ( nullptr == lp)
        return ON_SUBD_RETURN_ERROR(false);

      // set the point
      *lp = limit_point;
      lp->m_next_sector_limit_point = nullptr;

      // append the point to the vertex's linked list.
      const ON_SubDSectorLimitPoint* p = &m_limit_point;
      while(nullptr != p->m_next_sector_limit_point)
      {
        p = p->m_next_sector_limit_point;
      }

      const_cast<ON_SubDSectorLimitPoint*>(p)->m_next_sector_limit_point = lp;
    }

    if ( subd_type != ON_SubD::SubDTypeFromUnsigned(m_saved_points_flags & ON_SUBD_CACHE_TYPE_MASK) )
      m_saved_points_flags = 0U;

    m_saved_points_flags |= (ON_SUBD_CACHE_LIMIT_FLAG_MASK | ((unsigned char)subd_type));

    return true;
  }

  ClearSavedLimitPoints();

  if (ON_SubD::SubDType::Unset != subd_type)
    return ON_SUBD_RETURN_ERROR(false);

  return true;
}

void ON_SubDVertex::ClearSavedLimitPoints() const
{
  ON_SUBD_CACHE_CLEAR_LIMIT_FLAG(m_saved_points_flags);
  if (ON_UNSET_VALUE != m_limit_point.m_limitP[0] && nullptr != m_limit_point.m_sector_face)
  {
    // return multiple sector limit points to pool
    const ON_SubDSectorLimitPoint* next_p = m_limit_point.m_next_sector_limit_point;
    for (const ON_SubDSectorLimitPoint* p = next_p; nullptr != p; p = next_p)
    {
      next_p = p->m_next_sector_limit_point;
      LimitPointPool(p);
    }
  }
  m_limit_point = ON_SubDSectorLimitPoint::Unset;
}

ON_SubD::SubDType ON_SubDVertex::SavedLimitPointType() const
{
  return 
    (0 != (ON_SUBD_CACHE_LIMIT_FLAG_MASK & m_saved_points_flags))
    ? ((ON_SubD::SubDType)ON_SUBD_CACHE_TYPE(m_saved_points_flags))
    : ON_SubD::SubDType::Unset;
}

#endif
