#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_subd_data.h"

/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2014 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////
*/

/*
class ON_CLASS ON_SubDSectorType
{
public:
  ON_SubDSectorType() = default;
  ON_SubDSectorType(const ON_SubDSectorType&) = default;
  ON_SubDSectorType& operator=(const ON_SubDSectorType&) = default;

  static const ON_SubDSectorType Unset;

  bool IsValid() const;

  static ON_SubDSectorType Create(
    ON_SubD::SubDType subd_type,
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_edge_count,
    double sector_angle_radians
    );

  static ON_SubDSectorType CreateSmooth(
    ON_SubD::SubDType subd_type,
    unsigned int sector_edge_count
    );

  static ON_SubDSectorType CreateCrease(
    ON_SubD::SubDType subd_type,
    unsigned int sector_edge_count
    );

  static ON_SubDSectorType CreateDart(
    ON_SubD::SubDType subd_type,
    unsigned int sector_edge_count
    );

  static ON_SubDSectorType CreateCorner(
    ON_SubD::SubDType subd_type,
    unsigned int sector_edge_count,
    double sector_corner_angle_radians
    );

  static int Compare(
    const ON_SubDSectorType& a,
    const ON_SubDSectorType& b
    );

  ON_SubD::SubDType m_subd_type = ON_SubD::SubDType::Unset;
  ON_SubD::VertexTag m_vertex_tag = ON_SubD::VertexTag::Unset;

private:
  unsigned char m_reserved1 = 0;
  unsigned char m_reserved2 = 0;
  unsigned int m_reserved3 = 0;

public:
  unsigned int m_sector_edge_count = 0;
  unsigned int m_sector_face_count = 0;
  double m_sector_angle_radians = 0.0;
  double m_sector_weight = 0.0;
};
*/





double ON_SubDSectorType::SectorWeightCalculationError()
{
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorWeight);
}

bool ON_SubDSectorType::IsValid() const
{
  if ( 0 == m_hash)
    return ON_SUBD_RETURN_ERROR(false);

  switch (m_subd_type)
  {
  case ON_SubD::SubDType::TriLoopWarren:
  case ON_SubD::SubDType::QuadCatmullClark:
  case ON_SubD::SubDType::CustomTri:
  case ON_SubD::SubDType::CustomQuad:
    break;
  default:
    return ON_SUBD_RETURN_ERROR(false);
    break;
  }

  if ( m_sector_face_count < ON_SubDSectorType::MinimumSectorFaceCount(m_vertex_tag))
    return ON_SUBD_RETURN_ERROR(false);

  if ( m_sector_face_count > ON_SubDVertex::MaximumFaceCount)
    return ON_SUBD_RETURN_ERROR(false);

  switch (m_vertex_tag)
  {
  case ON_SubD::VertexTag::Smooth:
    if (!(m_corner_sector_angle_radians == ON_SubDSectorType::IgnoredCornerSectorAngle))
      return ON_SUBD_RETURN_ERROR(false);
    if (!(m_sector_weight == ON_SubDSectorType::IgnoredSectorWeight))
      return ON_SUBD_RETURN_ERROR(false);
    break;

  case ON_SubD::VertexTag::Crease:
    if (!(m_corner_sector_angle_radians == ON_SubDSectorType::IgnoredCornerSectorAngle))
      return ON_SUBD_RETURN_ERROR(false);
    if (!(m_sector_weight == ON_SubDSectorType::CreaseSectorWeight(m_subd_type,m_sector_face_count)))
      return ON_SUBD_RETURN_ERROR(false);
    break;

  case ON_SubD::VertexTag::Corner:
    if (!(m_corner_sector_angle_radians > 0.0 && m_corner_sector_angle_radians < 2.0*ON_PI))
      return ON_SUBD_RETURN_ERROR(false);
    if (!(m_sector_weight == ON_SubDSectorType::CornerSectorWeight(m_subd_type,m_sector_face_count,m_corner_sector_angle_radians)))
      return ON_SUBD_RETURN_ERROR(false);
    break;

  case ON_SubD::VertexTag::Dart:
    if (!(m_corner_sector_angle_radians == ON_SubDSectorType::IgnoredCornerSectorAngle))
      return ON_SUBD_RETURN_ERROR(false);
    if (!(m_sector_weight == ON_SubDSectorType::DartSectorWeight(m_subd_type,m_sector_face_count)))
      return ON_SUBD_RETURN_ERROR(false);
    break;

  default:
    return ON_SUBD_RETURN_ERROR(false);
    break;
  }

  return true;
}

ON_SubD::VertexTag ON_SubDSectorType::VertexTag() const
{
  return m_vertex_tag;
}

ON_SubD::SubDType ON_SubDSectorType::SubDType() const
{
  return m_subd_type;
}

ON_SubD::FacetType ON_SubDSectorType::FacetType() const
{
  return ON_SubD::FacetTypeFromSubDType(m_subd_type);
}

unsigned int ON_SubDSectorType::FacetEdgeCount() const
{
  switch (FacetType())
  {
  case ON_SubD::FacetType::Tri:
    return 3;
    break;
  case ON_SubD::FacetType::Quad:
    return 4;
    break;
  default:
    break;
  }
  return 0;
}

double ON_SubDSectorType::CornerSectorAngleRadians() const
{
  return 
    (ON_SubD::VertexTag::Corner == m_vertex_tag) 
    ? m_corner_sector_angle_radians
    : ON_SubDSectorType::ErrorCornerSectorAngle;
}

unsigned int ON_SubDSectorType::CornerSectorAngleIndex() const
{
  return (m_vertex_tag == ON_SubD::VertexTag::Corner) ? m_corner_sector_angle_index : ON_UNSET_UINT_INDEX;
}

bool ON_SubDSectorType::IsSmoothSector() const
{
  return (m_vertex_tag == ON_SubD::VertexTag::Smooth);
}


bool ON_SubDSectorType::IsDartSector() const
{
  return (m_vertex_tag == ON_SubD::VertexTag::Dart);
}


bool ON_SubDSectorType::IsCreaseSector() const
{
  return (m_vertex_tag == ON_SubD::VertexTag::Crease);
}

bool ON_SubDSectorType::IsCornerSector() const
{
  return (m_vertex_tag == ON_SubD::VertexTag::Corner && m_corner_sector_angle_index <=  ON_SubDSectorType::MaximumAngleIndex);
}

bool ON_SubDSectorType::IsConvexCornerSector() const
{
  return (m_vertex_tag == ON_SubD::VertexTag::Corner && 2*m_corner_sector_angle_index >=  ON_SubDSectorType::MaximumAngleIndex);
}

bool ON_SubDSectorType::IsConcaveCornerSector() const
{
  return (m_vertex_tag == ON_SubD::VertexTag::Corner && 2*m_corner_sector_angle_index <=  ON_SubDSectorType::MaximumAngleIndex);
}


unsigned int ON_SubDSectorType::EdgeCount() const
{
  if (m_sector_face_count >= ON_SubDSectorType::MinimumSectorFaceCount(m_vertex_tag))
  {
    switch (m_vertex_tag)
    {
    case ON_SubD::VertexTag::Smooth:
      return m_sector_face_count;
      break;

    case ON_SubD::VertexTag::Crease:
      return m_sector_face_count + 1;
      break;

    case ON_SubD::VertexTag::Corner:
      return m_sector_face_count + 1;
      break;

    case ON_SubD::VertexTag::Dart:
      return m_sector_face_count;
      break;

    default:
      break;
    }
  }
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDSectorType::FaceCount() const
{
  if ( m_sector_face_count >= ON_SubDSectorType::MinimumSectorFaceCount(m_vertex_tag))
    return m_sector_face_count;

  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDSectorType::ComponentRingCount() const
{
  return 1 + m_sector_face_count + EdgeCount();
}

unsigned int ON_SubDSectorType::PointRingCount() const
{
  return ON_SubDSectorType::SectorPointRingCountFromFaceCount(
    m_subd_type,
    m_vertex_tag,
    m_sector_face_count
    );
}

double ON_SubDSectorType::CreaseSectorTheta(
  unsigned int sector_face_count
  )
{
  if (sector_face_count >= 1) 
    return (ON_PI / ((double)sector_face_count));
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorTheta);
}

double ON_SubDSectorType::DartSectorTheta(
  unsigned int sector_face_count
  )
{
  if (sector_face_count >= 2)
    return (2.0*ON_PI / ((double)sector_face_count));
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorTheta);
}

unsigned int ON_SubDSectorType::AngleIndexFromAngleRadians(
  double corner_sector_angle_radians
  )
{
  corner_sector_angle_radians = ON_SubDSectorType::ClampCornerSectorAngleRadians(corner_sector_angle_radians);
  if (ON_SubDSectorType::IsValidCornerSectorAngleRadians(corner_sector_angle_radians))
  {
    const double max_index = ON_SubDSectorType::MaximumAngleIndex;
    unsigned int i = (unsigned int)floor(max_index*(corner_sector_angle_radians / (2.0*ON_PI)));
    if (i < ON_SubDSectorType::MaximumAngleIndex)
    {
      double a0 = ON_SubDSectorType::AngleRadiansFromAngleIndex(i);
      double a1 = ON_SubDSectorType::AngleRadiansFromAngleIndex(i+1);
      double d0 = fabs(a0 - corner_sector_angle_radians);
      double d1 = fabs(a1 - corner_sector_angle_radians);
      if (d1 < d0)
        i++;
    }
    return i;
  }
  return ON_SUBD_RETURN_ERROR(ON_UNSET_UINT_INDEX);
}

double ON_SubDSectorType::AngleRadiansFromAngleIndex(
  unsigned int corner_sector_angle_index
  )
{
  if (corner_sector_angle_index <= ON_SubDSectorType::MaximumAngleIndex)
  {
    const double max_index = ON_SubDSectorType::MaximumAngleIndex;
    return (corner_sector_angle_index / max_index)*2.0*ON_PI;
  }
  return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
}


double ON_SubDSectorType::CornerSectorThetaFromCornerAngle(
    unsigned int sector_face_count,
    double corner_sector_angle_radians
    )
{
  if (sector_face_count >= ON_SubDSectorType::MinimumSectorFaceCount(ON_SubD::VertexTag::Corner)
    && sector_face_count <= ON_SubDVertex::MaximumFaceCount
    )
  {
    unsigned int corner_index = ON_SubDSectorType::AngleIndexFromAngleRadians(corner_sector_angle_radians);
    if (corner_index <= ON_SubDSectorType::MaximumAngleIndex)
    {
      if (2*corner_index > ON_SubDSectorType::MaximumAngleIndex)
      {
        // concave corner - theta = (2 pi - corner_sector_angle_radians)/sector_face_count
        corner_index = ON_SubDSectorType::MaximumAngleIndex - corner_index;
      }
      double a = ((corner_index/((double)ON_SubDSectorType::MaximumAngleIndex))*ON_PI)/((double)sector_face_count);
      return a;
    }
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorTheta);
}

double ON_SubDSectorType::SectorTheta() const
{
  return m_sector_theta;
}

double ON_SubDSectorType::CornerSectorAngleRadiansFromEdges(
  ON_SubDEdgePtr sector_boundary_edge0_ptr,
  ON_SubDEdgePtr sector_boundary_edge1_ptr
  )
{
  const ON_SubDEdge* edges[2] = { ON_SUBD_EDGE_POINTER(sector_boundary_edge0_ptr.m_ptr), ON_SUBD_EDGE_POINTER(sector_boundary_edge1_ptr.m_ptr) };
  if (nullptr == edges[0] || nullptr == edges[1])
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorCornerSectorAngle);

  if (edges[0] == edges[1])
    return ON_SUBD_RETURN_ERROR(0.0);

  ON__UINT_PTR edge_ends[2] = { ON_SUBD_EDGE_DIRECTION(sector_boundary_edge0_ptr.m_ptr), ON_SUBD_EDGE_DIRECTION(sector_boundary_edge1_ptr.m_ptr) };

  const ON_SubDVertex* V[2] = { edges[0]->m_vertex[1 - edge_ends[0]], edges[1]->m_vertex[1 - edge_ends[1]] };
  if (nullptr == V[0] || nullptr == V[1])
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorCornerSectorAngle);

  const ON_SubDVertex* corner_vertex = edges[0]->m_vertex[edge_ends[0]];
  if (nullptr == corner_vertex || corner_vertex != edges[1]->m_vertex[edge_ends[1]])
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorCornerSectorAngle);

  const double* cornerP = corner_vertex->m_P;

  const double* endP[2] = { V[0]->m_P, V[1]->m_P };

  // A = vector from cornerP to endP[0]
  ON_3dVector A(endP[0][0] - cornerP[0], endP[0][1] - cornerP[1], endP[0][2] - cornerP[2]);
  // B = vector from cornerP to endP[1]
  ON_3dVector B(endP[1][0] - cornerP[0], endP[1][1] - cornerP[1], endP[1][2] - cornerP[2]);

  // Unitize A and B
  A.Unitize();
  B.Unitize();

  // calculate angle between A and B
  // In reality, we will be lucky if we get 3 digits of precision in the trig functions
  // using the dot and cross of unitized differences.
  double cos_alpha = A*B;
  double sin_alpha = ON_CrossProduct(A, B).Length();
  
  const double trig_zero_tol = 0.002;
  if (fabs(cos_alpha) <= trig_zero_tol)
    cos_alpha = 0.0;
  if (fabs(sin_alpha) <= trig_zero_tol)
    sin_alpha = 0.0;

  if ( fabs(cos_alpha*cos_alpha + sin_alpha*sin_alpha - 1.0) <= 0.125 )
  {
    // valid sin and cos and no NaNs
    const double trig_one_tol = 0.999;
    if ( 0.0 == cos_alpha || fabs(sin_alpha) >= trig_one_tol)
      return (sin_alpha < 0.0) ? 1.5*ON_PI : 0.5*ON_PI;

    if ( 0.0 == sin_alpha || fabs(cos_alpha) >= trig_one_tol)
      return (cos_alpha < 0.0) ? ON_PI : 0.0;

    double alpha = atan2(sin_alpha, cos_alpha);
    if (!ON_IsValid(alpha))
      return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorCornerSectorAngle);

    if (alpha < 0.0)
    {
      alpha += 2.0*ON_PI;
      if ( alpha >= 2.0*ON_PI )
        alpha = 0.0;
    }

    if (alpha >= 0.0 && alpha <= (1.0 + ON_EPSILON)*2.0*ON_PI)
    {
      if (alpha >= 2.0*ON_PI)
      {
        alpha = 0.0;
      }
      else if ( fabs(alpha - ON_PI) <= 0.002 )
      {
        // straight "corner"
        alpha = ON_PI;
      }
      return alpha;
    }
  }

  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorCornerSectorAngle);
}





double ON_SubDSectorType::SectorWeightFromTheta(
  ON_SubD::SubDType subdivision_type,
  double sector_theta
  )
{
  if (!(sector_theta > 0.0 && sector_theta <= ON_PI))
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorWeight);

  const ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subdivision_type);

  switch (facet_type)
  {
  case ON_SubD::FacetType::Tri:
  case ON_SubD::FacetType::Quad:
    break;

  default:
    // bogus facet type
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorWeight);
    break;
  }

  double cos_theta = cos(sector_theta);

  // If cos_theta is near 0, +1/2, -1/2, +1 or -1, then use those values
  // so the weights have the easily identified values associated with the
  // most common cases.
  const double cos_tol = 1e-6;
  const double abs_cos_theta = fabs(cos_theta);
  if (fabs(abs_cos_theta) <= cos_tol)
    cos_theta = 0.0;
  else if (fabs(abs_cos_theta - 0.5) <= cos_tol)
    cos_theta = (cos_theta < 0.0) ? -0.5 : 0.5;
  else if (abs_cos_theta + cos_tol >= 1.0)
    cos_theta = (cos_theta < 0.0) ? -1.0 : 1.0;

  double a, wrange[2];

  if (ON_SubD::FacetType::Tri == facet_type)
  {
    // Triangle case: w = 1/3 + 1/3*cos(theta);
    a = 1.0 / 3.0;
    wrange[0] = 0.0;
    wrange[1] = 2.0 / 3.0;
  }
  else
  {
    // Quadrangle case: w = 1/2 + 1/3*cos(theta);
    a = 0.5;
    wrange[0] = 1.0 / 6.0;
    wrange[1] = 5.0 / 6.0;
  }

  if (cos_theta > -1.0 && cos_theta < 1.0)
  {
    const double w = a + cos_theta / 3.0;
    if (w > wrange[0] && w < wrange[1])
      return w;
    if (w <= wrange[0])
      return wrange[0];
    if (w >= wrange[1])
      return wrange[1];
  }

  if (cos_theta >= 1.0)
    return wrange[1];

  if (cos_theta <= -1.0)
    return wrange[0];

  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorWeight); // error
}

double ON_SubDSectorType::SectorWeight() const
{
  return m_sector_weight;
}

bool ON_SubDSectorType::IsValidSectorWeightValue(
  double weight_value,
  bool bAllowUnsetTaggedEndWeight
  )
{
  return ((weight_value >= 0.0 && weight_value < 1.0) || (bAllowUnsetTaggedEndWeight && ON_SubDSectorType::UnsetSectorWeight == weight_value));
}

bool ON_SubDSectorType::IsValidCornerSectorAngleRadians(
  double corner_sector_angle_radians
  )
{
  return (corner_sector_angle_radians >= 0.0 && corner_sector_angle_radians <= 2.0*ON_PI);
}

double ON_SubDSectorType::ClampCornerSectorAngleRadians(
  double corner_sector_angle_radians
  )
{
  if ( corner_sector_angle_radians < -ON_PI )
    corner_sector_angle_radians += 2.0*ON_PI;
  else if ( corner_sector_angle_radians > 3.0*ON_PI )
    corner_sector_angle_radians -= 2.0*ON_PI;
  const double angle_tol = 0.25*(ON_PI/180.0); // 1/4 degree.
  if (fabs(corner_sector_angle_radians - ON_PI) <= angle_tol)
    return ON_PI;
  if (fabs(corner_sector_angle_radians - 2.0*ON_PI) <= angle_tol)
    return 2.0*ON_PI;
  return corner_sector_angle_radians;
}


double ON_SubDSectorType::SmoothSectorWeight()
{
  return ON_SubDSectorType::IgnoredSectorWeight;
}

double ON_SubDSectorType::CreaseSectorWeight(
  ON_SubD::SubDType subdivision_type,
  unsigned int sector_face_count
  )
{
  if (sector_face_count < 1)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorWeight);
  if (false == ON_SubD::IsQuadOrTriSubDType(subdivision_type))
    return ON_SubDSectorType::UnsetSectorWeight;
  double sector_theta = ON_SubDSectorType::CreaseSectorTheta(sector_face_count);
  return ON_SubDSectorType::SectorWeightFromTheta(subdivision_type, sector_theta);
}

double ON_SubDSectorType::DartSectorWeight(
  ON_SubD::SubDType subdivision_type,
  unsigned int sector_face_count
  )
{
  if (sector_face_count < 2)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorWeight);
  if (false == ON_SubD::IsQuadOrTriSubDType(subdivision_type))
    return ON_SubDSectorType::UnsetSectorWeight;
  double sector_theta = ON_SubDSectorType::DartSectorTheta(sector_face_count);
  return ON_SubDSectorType::SectorWeightFromTheta(subdivision_type, sector_theta);
}

double ON_SubDSectorType::CornerSectorWeight(
  ON_SubD::SubDType subdivision_type,
  unsigned int sector_face_count,
  double corner_sector_angle_radians
  )
{
  if ( ON_SubD::SubDType::Unset == subdivision_type)
    return ON_SubDSectorType::UnsetSectorWeight;

  corner_sector_angle_radians = ON_SubDSectorType::ClampCornerSectorAngleRadians(corner_sector_angle_radians);
  if (ON_SubDSectorType::IsValidCornerSectorAngleRadians(corner_sector_angle_radians) 
    && sector_face_count >= ON_SubDSectorType::MinimumSectorFaceCount(ON_SubD::VertexTag::Corner)
    && sector_face_count <= ON_SubDVertex::MaximumFaceCount
    && ON_SubD::IsQuadOrTriSubDType(subdivision_type)
    )
  {
    const double sector_theta = ON_SubDSectorType::CornerSectorThetaFromCornerAngle(sector_face_count,corner_sector_angle_radians);
    if (sector_theta >= 0.0)
      return ON_SubDSectorType::SectorWeightFromTheta(subdivision_type, sector_theta);
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::ErrorSectorWeight);
}

static int CompareUnsinged(unsigned int a, unsigned int b)
{
  if (a < b)
    return -1;
  if (a > b)
    return 1;
  return 0;
}

int ON_SubDSectorType::Compare(const ON_SubDSectorType* a, const ON_SubDSectorType* b)
{
  if ( a == b )
    return 0;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;
  
  int rc = CompareUnsinged((unsigned int)a->m_subd_type,(unsigned int)b->m_subd_type);
  for (;;)
  {
    if (0 != rc)
    {
      // bias towards ccquad, then lwtri, then exotics
      if ( ON_SubD::SubDType::QuadCatmullClark == a->m_subd_type)
        rc = -1;
      else if ( ON_SubD::SubDType::QuadCatmullClark == b->m_subd_type)
        rc = 1;
      else if ( ON_SubD::SubDType::TriLoopWarren == a->m_subd_type)
        rc = -1;
      else if ( ON_SubD::SubDType::TriLoopWarren == b->m_subd_type)
        rc = 1;
      break;
    }
    rc = CompareUnsinged((unsigned int)a->m_vertex_tag,(unsigned int)b->m_vertex_tag);
    if (0 != rc)
    {
      // bias towards valid tags
      if ( ON_SubD::VertexTag::Unset == b->m_vertex_tag)
        rc = -1;
      else if ( ON_SubD::VertexTag::Unset == a->m_vertex_tag)
        rc = 1;
      break;
    }
    rc = CompareUnsinged(a->m_sector_face_count,b->m_sector_face_count);
    if (0 != rc)
    {
      // bias towards valid m_sector_face_count bug small
      if ( 0 == b->m_sector_face_count)
        rc = -1;
      else if ( 0 == a->m_sector_face_count)
        rc = 1;
      break;
    }
    if (ON_SubD::VertexTag::Corner == a->m_vertex_tag)
    {
      rc = CompareUnsinged(a->m_corner_sector_angle_index, b->m_corner_sector_angle_index);
      if (0 != rc)
        break;
    }

    return 0; // equal
  }

  return rc; // not equal
}


void ON_SubDSectorType::SetHash()
{
  unsigned int hash = 0;
  hash = ON_CRC32(hash,sizeof(m_subd_type),&m_subd_type);
  hash = ON_CRC32(hash,sizeof(m_vertex_tag),&m_vertex_tag);
  hash = ON_CRC32(hash,sizeof(m_sector_face_count),&m_sector_face_count);
  if ( ON_SubD::VertexTag::Corner == m_vertex_tag)
    hash = ON_CRC32(hash,sizeof(m_corner_sector_angle_index),&m_corner_sector_angle_index);
  if ( 0 == hash )
    hash = 1;
  m_hash = hash;
}

unsigned int ON_SubDSectorType::SectorTypeHash() const
{
  return m_hash;
}

static bool ON_SubDSectorType_IsValidFaceCount(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  return (sector_face_count >= ON_SubDSectorType::MinimumSectorFaceCount(vertex_tag) && sector_face_count <= ON_SubDVertex::MaximumFaceCount);
}

static bool ON_SubDSectorType_IsValidFaceCountForCreate(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  return ( 0 == sector_face_count || ON_UNSET_UINT_INDEX == sector_face_count || ON_SubDSectorType_IsValidFaceCount(vertex_tag,sector_face_count));
}

static bool ON_SubDSectorType_IsValidTypes(
  ON_SubD::SubDType subd_type,
  ON_SubD::VertexTag vertex_tag,
  ON_SubD::FacetType facet_type
  )
{
  if (ON_SubD::SubDType::Unset == subd_type && ON_SubD::FacetType::Unset == facet_type )
    return true;
  if (ON_SubD::FacetType::Unset != facet_type)
    return true;
  return false;
}

ON_SubDSectorType ON_SubDSectorType::CreateSmoothSectorType(
  ON_SubD::SubDType subd_type,
  unsigned int sector_face_count
  )
{
  const ON_SubD::VertexTag vertex_tag = ON_SubD::VertexTag::Smooth;
  if (ON_SubDSectorType_IsValidFaceCountForCreate(vertex_tag,sector_face_count))
  {
    const ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subd_type);
    if (ON_SubDSectorType_IsValidTypes(subd_type,vertex_tag,facet_type))
    {
      ON_SubDSectorType st;
      st.m_subd_type = subd_type;
      st.m_facet_type = facet_type;
      st.m_vertex_tag = vertex_tag;
      st.m_sector_face_count 
        = ON_SubDSectorType_IsValidFaceCount(vertex_tag,sector_face_count)
        ? sector_face_count
        : 0;
      st.m_sector_weight 
        = (st.m_sector_face_count>0)
        ? ON_SubDSectorType::IgnoredSectorWeight
        :  ON_SubDSectorType::UnsetSectorWeight;
      st.m_sector_theta 
        = (st.m_sector_face_count>0)
        ? ON_SubDSectorType::SmoothSectorTheta
        : ON_SubDSectorType::UnsetSectorTheta;
      st.SetHash();
      return st;
    }
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
}

ON_SubDSectorType ON_SubDSectorType::CreateCreaseSectorType(
  ON_SubD::SubDType subd_type,
  unsigned int sector_face_count
  )
{
  const ON_SubD::VertexTag vertex_tag = ON_SubD::VertexTag::Crease;
  if (ON_SubDSectorType_IsValidFaceCountForCreate(vertex_tag,sector_face_count))
  {
    const ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subd_type);
    if (ON_SubDSectorType_IsValidTypes(subd_type,vertex_tag,facet_type))
    {
      ON_SubDSectorType st;
      st.m_subd_type = subd_type;
      st.m_facet_type = facet_type;
      st.m_vertex_tag = vertex_tag;
      st.m_sector_face_count 
        = ON_SubDSectorType_IsValidFaceCount(vertex_tag,sector_face_count)
        ? sector_face_count
        : 0;
      st.m_sector_weight 
        = (st.m_sector_face_count>0)
        ? ON_SubDSectorType::CreaseSectorWeight(subd_type,sector_face_count)
        : ON_SubDSectorType::UnsetSectorWeight;
      st.m_sector_theta 
        = (st.m_sector_face_count>0)
        ? ON_SubDSectorType::CreaseSectorTheta(sector_face_count)
        : ON_SubDSectorType::UnsetSectorTheta;
      st.SetHash();
      return st;
    }
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
}


ON_SubDSectorType ON_SubDSectorType::CreateDartSectorType(
  ON_SubD::SubDType subd_type,
  unsigned int sector_face_count
  )
{
  const ON_SubD::VertexTag vertex_tag = ON_SubD::VertexTag::Dart;
  if ( ON_SubDSectorType_IsValidFaceCountForCreate(vertex_tag,sector_face_count) )
  {
    const ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subd_type);
    if (ON_SubDSectorType_IsValidTypes(subd_type,vertex_tag,facet_type))
    {
      ON_SubDSectorType st;
      st.m_subd_type = subd_type;
      st.m_facet_type = facet_type;
      st.m_vertex_tag = vertex_tag;
      st.m_sector_face_count 
        = ON_SubDSectorType_IsValidFaceCount(vertex_tag,sector_face_count)
        ? sector_face_count
        : 0;
      st.m_sector_weight 
        = (st.m_sector_face_count>0)
        ? ON_SubDSectorType::DartSectorWeight(subd_type,sector_face_count)
        : ON_SubDSectorType::UnsetSectorWeight;
      st.m_sector_theta 
        = (st.m_sector_face_count>0)
        ? ON_SubDSectorType::DartSectorTheta(sector_face_count)
        : ON_SubDSectorType::UnsetSectorTheta;
      st.SetHash();
      return st;
    }
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
}



ON_SubDSectorType ON_SubDSectorType::CreateCornerSectorType(
  ON_SubD::SubDType subd_type,
  unsigned int sector_face_count,
  double corner_sector_angle_radians
  )
{
  if (ON_SubDSectorType::UnsetCornerSectorAngle != corner_sector_angle_radians)
  {
    if (ON_UNSET_VALUE == corner_sector_angle_radians)
    {
      corner_sector_angle_radians = ON_SubDSectorType::UnsetCornerSectorAngle;
    }
    else
    {
      corner_sector_angle_radians = ON_SubDSectorType::ClampCornerSectorAngleRadians(corner_sector_angle_radians);
    }
  }

  if (ON_SubDSectorType::UnsetCornerSectorAngle == corner_sector_angle_radians
    || ON_SubDSectorType::IsValidCornerSectorAngleRadians(corner_sector_angle_radians)
    )
  {
    const ON_SubD::VertexTag vertex_tag = ON_SubD::VertexTag::Corner;
    if (ON_SubDSectorType_IsValidFaceCountForCreate(vertex_tag,sector_face_count))
    {
      const ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subd_type);
      if (ON_SubDSectorType_IsValidTypes(subd_type,vertex_tag,facet_type))
      {
        unsigned int corner_sector_angle_index 
          = (ON_SubDSectorType::UnsetCornerSectorAngle == corner_sector_angle_radians)
          ? 0
          : ON_SubDSectorType::AngleIndexFromAngleRadians(corner_sector_angle_radians);
        if (corner_sector_angle_index <= ON_SubDSectorType::MaximumAngleIndex)
        {
          ON_SubDSectorType st;
          st.m_subd_type = subd_type;
          st.m_facet_type = facet_type;
          st.m_vertex_tag = vertex_tag;
          st.m_sector_face_count 
            = ON_SubDSectorType_IsValidFaceCount(vertex_tag,sector_face_count)
            ? sector_face_count
            : 0;
          st.m_sector_weight 
            = (st.m_sector_face_count > 0 && ON_SubDSectorType::UnsetCornerSectorAngle != corner_sector_angle_radians)
            ? ON_SubDSectorType::CornerSectorWeight(subd_type, sector_face_count, corner_sector_angle_radians)
            : ON_SubDSectorType::UnsetSectorWeight;
          st.m_sector_theta 
            = (st.m_sector_face_count > 0 && ON_SubDSectorType::UnsetCornerSectorAngle != corner_sector_angle_radians)
            ? ON_SubDSectorType::CornerSectorThetaFromCornerAngle(sector_face_count, corner_sector_angle_radians)
            : ON_SubDSectorType::UnsetSectorTheta;
          st.m_corner_sector_angle_index = (unsigned char)corner_sector_angle_index;
          st.m_corner_sector_angle_radians = corner_sector_angle_radians;
          st.SetHash();
          return st;
        }
      }
    }
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
}


ON_SubDSectorType ON_SubDSectorType::Create(
  ON_SubD::SubDType subd_type,
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_face_count,
  double corner_sector_angle_radians
  )
{
  if ( ON_SubD::SubDType::Unset == subd_type && ON_SubD::VertexTag::Unset == vertex_tag && 0 == sector_face_count)
    return ON_SubDSectorType::Empty;

  switch (vertex_tag)
  {
  case ON_SubD::VertexTag::Smooth:
    return ON_SubDSectorType::CreateSmoothSectorType(subd_type,sector_face_count);
    break;
  case ON_SubD::VertexTag::Crease:
    return ON_SubDSectorType::CreateCreaseSectorType(subd_type,sector_face_count);
    break;
  case ON_SubD::VertexTag::Corner:
    return ON_SubDSectorType::CreateCornerSectorType(subd_type,sector_face_count,corner_sector_angle_radians);
    break;
  case ON_SubD::VertexTag::Dart:
    return ON_SubDSectorType::CreateDartSectorType(subd_type,sector_face_count);
    break;
  }

  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
}

ON_SubDSectorType ON_SubDSectorType::Create(
  ON_SubD::SubDType subd_type,
  const ON_SubDSectorIterator& sit
  )
{
  const ON_SubDVertex* center_vertex = sit.CenterVertex();

  if (nullptr == center_vertex )
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);

  ON_SubDSectorIterator local_sit(sit);

  const ON_SubD::VertexTag vertex_tag = center_vertex->m_vertex_tag;

  const ON_SubDFace* face0;
  ON_SubDEdgePtr edge0ptr = ON_SubDEdgePtr::Null;
  if (ON_SubD::VertexTag::Smooth == vertex_tag)
  {
    face0 = local_sit.CurrentFace();
    if (nullptr == face0 )
      return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  }
  else
  {
    face0 = nullptr;
    if (nullptr == local_sit.IncrementToCrease(-1))
      return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
    edge0ptr = local_sit.CurrentEdgePtr(0);
  }
  
  const unsigned int vertex_face_count = center_vertex->m_face_count;
  unsigned int sector_face_count = 0;
  while(sector_face_count < vertex_face_count)
  {
    sector_face_count++;
    if ( sector_face_count > vertex_face_count )
      return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
    if (face0 == local_sit.NextFace(true))
    {
      double corner_sector_angle_radians 
          = (ON_SubD::VertexTag::Corner == vertex_tag)
          ? ON_SubDSectorType::CornerSectorAngleRadiansFromEdges(edge0ptr,local_sit.CurrentEdgePtr(0))
          : 0.0;
      return ON_SubDSectorType::Create(subd_type,vertex_tag,sector_face_count,corner_sector_angle_radians);
    }
  }

  return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
}

ON_SubDSectorType ON_SubDSectorType::Create(
  ON_SubD::SubDType subd_type,
  const class ON_SubDFace* face,
  unsigned int face_vertex_index
  )
{
  if (nullptr == face)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  if (face_vertex_index >= face->m_edge_count)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  ON_SubDSectorIterator sit;
  sit.Initialize(face,0,face_vertex_index);
  return ON_SubDSectorType::Create(subd_type,sit);
}

ON_SubDSectorType ON_SubDSectorType::Create(
  ON_SubD::SubDType subd_type,
  const class ON_SubDFace* face,
  const class ON_SubDVertex* vertex
  )
{
  if (nullptr == face)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  if (nullptr == vertex)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  unsigned int face_vertex_index = face->VertexIndex(vertex);
  if (face_vertex_index >= face->m_edge_count)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  return ON_SubDSectorType::Create(subd_type,face,face_vertex_index);
}


ON_SubDSectorType ON_SubDSectorType::Create(
  ON_SubD::SubDType subd_type,
  const class ON_SubDEdge* edge,
  unsigned int edge_vertex_index
  )
{
  if (nullptr == edge)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  const ON_SubDVertex* vertex = edge->Vertex(edge_vertex_index);
  if (nullptr == vertex)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);
  const ON_SubDFace* face = edge->Face(0);
  if (nullptr == face)
    return ON_SUBD_RETURN_ERROR(ON_SubDSectorType::Empty);

  return ON_SubDSectorType::Create(subd_type,face,vertex);
}
