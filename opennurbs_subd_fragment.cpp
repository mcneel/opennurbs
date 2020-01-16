#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_subd_data.h"

/*
//
// Copyright (c) 1993-2019 Robert McNeel & Associates. All rights reserved.
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

/////////////////////////////////////////////////////////////////////////////////////////
//
// ON_SubDMeshFragment
//

void ON_SubDMeshFragment::Internal_Set3dPointArrayToNan(double* a, size_t a_count, size_t a_stride)
{
  if (nullptr == a || a_count <= 0 || a_stride < 3)
    return;
  double *a2 = a + a_count * a_stride;
  if (3 == a_stride)
  {
    while (a < a2)
    {
      *a++ = ON_DBL_QNAN;
      *a++ = ON_DBL_QNAN;
      *a++ = ON_DBL_QNAN;
    }
  }
  else
  {
    a_stride -= 3;
    while (a < a2)
    {
      *a++ = ON_DBL_QNAN;
      *a++ = ON_DBL_QNAN;
      *a++ = ON_DBL_QNAN;
      a += a_stride;
    }
  }
}


unsigned ON_SubDMeshFragment::VertexCapacity() const
{
  return (m_vertex_capacity_etc & ON_SubDMeshFragment::ValueMask);
}

bool ON_SubDMeshFragment::ManagedArrays() const
{
  const size_t capacity = (ON_SubDMeshFragment::ValueMask & m_vertex_capacity_etc);
  return (
    0 != (ON_SubDMeshFragment::EtcManagedArraysBit & m_vertex_capacity_etc)
    && capacity > 0
    && 3 == m_P_stride && 3 == m_N_stride && 3 == m_T_stride
    && nullptr != m_P
    && m_N == m_P + (capacity * 3)
    && m_T == m_N + (capacity * 3)
    ) ? true : false;
}

bool ON_SubDMeshFragment::DeleteManagedArrays()
{
  if (ManagedArrays())
  {
    void* ptr = m_P;
    m_vertex_count_etc &= ON_SubDMeshFragment::EtcMask;
    m_vertex_capacity_etc &= ON_SubDMeshFragment::EtcMask;
    m_vertex_capacity_etc &= ~ON_SubDMeshFragment::EtcManagedArraysBit;
    m_P = nullptr;
    m_N = nullptr;
    m_T = nullptr;
    m_P_stride = 0;
    m_N_stride = 0;
    m_T_stride = 0;
    onfree(ptr);
    return true;
  }
  return false;
}

bool ON_SubDMeshFragment::UnmanagedArrays() const
{
  return
    (0 == (ON_SubDMeshFragment::EtcManagedArraysBit & m_vertex_capacity_etc) && 0 != (m_vertex_capacity_etc & ON_SubDMeshFragment::ValueMask) )
    ? true
    : false;
}

bool ON_SubDMeshFragment::SetUnmanagedVertexCapacity(size_t vertex_capacity)
{
  if (vertex_capacity < 0 || vertex_capacity > (size_t)ON_SubDMeshFragment::MaximumVertexCount)
    return ON_SUBD_RETURN_ERROR(false);
  if (ManagedArrays())
  {
    // attempting to internally managed memory to externally managed memory
    return ON_SUBD_RETURN_ERROR(false);
  }
  unsigned short etc = m_vertex_capacity_etc &= ON_SubDMeshFragment::EtcMask;
  etc &= ~ON_SubDMeshFragment::EtcManagedArraysBit;
  m_vertex_capacity_etc = ((unsigned short)vertex_capacity) | etc;
  return true;
}

bool ON_SubDMeshFragment::ReserveManagedVertexCapacity(size_t vertex_capacity)
{
  if (vertex_capacity < 0 || vertex_capacity >(size_t)ON_SubDMeshFragment::MaximumVertexCount)
    return ON_SUBD_RETURN_ERROR(false);
  if (vertex_capacity > (size_t)(ON_SubDMeshFragment::ValueMask))
    return ON_SUBD_RETURN_ERROR(false); // too big
  if (UnmanagedArrays())
  {
    // attempting to convert externally managed memory to internally managed memory.
    return ON_SUBD_RETURN_ERROR(false);
  }
  const size_t current_capacity = (size_t)(ON_SubDMeshFragment::ValueMask & m_vertex_capacity_etc);
  if (ManagedArrays())
  {
    if (current_capacity >= vertex_capacity)
      return true;
    DeleteManagedArrays();
  }
  unsigned short etc = m_vertex_capacity_etc &= ON_SubDMeshFragment::EtcMask;
  etc |= ON_SubDMeshFragment::EtcManagedArraysBit;
  double* p = (double*)onmalloc(9 * vertex_capacity);
  m_P = p;
  m_P_stride = 3;
  m_N = m_P + (vertex_capacity*m_P_stride);
  m_N_stride = 3;
  m_T = m_N + (vertex_capacity*m_N_stride);
  m_T_stride = 3;
  return true;
}

unsigned ON_SubDMeshFragment::VertexCount() const
{
  return (m_vertex_count_etc & ON_SubDMeshFragment::ValueMask);
}

bool ON_SubDMeshFragment::SetVertexCount(size_t vertex_count)
{
  if (vertex_count < 0 || vertex_count > VertexCapacity())
    return ON_SUBD_RETURN_ERROR(false);
  unsigned short etc = m_vertex_count_etc & ON_SubDMeshFragment::EtcMask;
  m_vertex_count_etc = ((unsigned short)vertex_count) | etc;
  return true;
}

unsigned int ON_SubDMeshFragment::PointCount() const
{
  return (nullptr != m_P && m_P_stride >= 3) ? VertexCount() : 0U;
}

unsigned int ON_SubDMeshFragment::NormalCount() const
{
  return (nullptr != m_N && m_N_stride >= 3) ? VertexCount() : 0U;
}

unsigned int ON_SubDMeshFragment::PointCapacity() const
{
  return (nullptr != m_P && m_P_stride >= 3) ? VertexCapacity() : 0U;
}

unsigned int ON_SubDMeshFragment::NormalCapacity() const
{
  return (nullptr != m_N && m_N_stride >= 3) ? VertexCapacity() : 0U;
}

const double* ON_SubDMeshFragment::PointArray(ON_SubDComponentLocation subd_appearance)const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? &m_ctrlnetP[0][0] : m_P;
}

size_t ON_SubDMeshFragment::PointArrayStride(ON_SubDComponentLocation subd_appearance)const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? 3 : m_P_stride;
}

unsigned ON_SubDMeshFragment::PointArrayCount(ON_SubDComponentLocation subd_appearance) const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? 4U : PointCount();
}

const double* ON_SubDMeshFragment::NormalArray(ON_SubDComponentLocation subd_appearance)const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? m_ctrlnetN : m_N;
}

size_t ON_SubDMeshFragment::NormalArrayStride(ON_SubDComponentLocation subd_appearance)const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? 0 : m_N_stride;
}

unsigned ON_SubDMeshFragment::NormalArrayCount(ON_SubDComponentLocation subd_appearance) const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? 4U : NormalCount();
}

const ON_3dPoint ON_SubDMeshFragment::ControlNetQuadPoint(
  bool bGridOrder,
  unsigned point_index
) const
{
  if (point_index >= 4 || 0 == (ON_SubDMeshFragment::EtcControlNetQuadBit & m_vertex_count_etc))
    return ON_3dPoint::NanPoint;
  if (false == bGridOrder)
  {
    if (2 == point_index)
      point_index = 3;
    else if (3 == point_index)
      point_index = 2;
  }
  return ON_3dPoint(m_ctrlnetP[point_index]);
}

const ON_SubDMeshFragment ON_SubDMeshFragment::ControlNetQuadFragmentForExperts() const
{
  ON_SubDMeshFragment q(*this);
  for (;;)
  {
    if ((m_vertex_count_etc & ON_SubDMeshFragment::ValueMask) < 4)
      break;
    if ( 0 == (m_vertex_count_etc & ON_SubDMeshFragment::EtcControlNetQuadBit) )
      break;

    q.m_vertex_count_etc = 4;
    q.m_vertex_count_etc |= ON_SubDMeshFragment::EtcControlNetQuadBit;
    q.m_vertex_count_etc |= (ON_SubDMeshFragment::EtcTextureCoordinatesExistBit&m_vertex_count_etc);

    q.m_vertex_capacity_etc = 4;

    // 4 quad corner points in grid order with stride = 3
    q.m_P = const_cast<double*>(&m_ctrlnetP[0][0]);
    q.m_P_stride = 3;

    // 4 identical normals with stride = 0
    q.m_N = const_cast<double*>(&m_ctrlnetN[0]);
    q.m_N_stride = 0;

    // 4 quad corner texture coordinates in grid order with stride = 3
    q.m_T = const_cast<double*>(&m_ctrlnetT[0][0]);
    q.m_T_stride = 3;

    // The grid is a single quad
    q.m_grid = ON_SubDMeshFragmentGrid::OneQuadGrid;

    // both bounding boxes are equal.
    q.m_surface_bbox = ControlNetQuadBoundingBox();
    return q;
  }

  memset(&q, 0, sizeof(q));
  return ON_SUBD_RETURN_ERROR(q);
}

bool ON_SubDMeshFragment::GetControlNetQuad(
  bool bGridOrder,
  ON_3dPoint quad_points[4],
  ON_3dVector& quad_normal
) const
{
  const bool rc = 0 != (ON_SubDMeshFragment::EtcControlNetQuadBit & m_vertex_count_etc) ? true : false;
  if (nullptr != quad_points)
  {
    if (rc)
    {
      int i;
      quad_points[0] = ON_3dPoint(m_ctrlnetP[0]);
      quad_points[1] = ON_3dPoint(m_ctrlnetP[1]);
      i = bGridOrder ? 2 : 3;
      quad_points[i] = ON_3dPoint(m_ctrlnetP[2]);
      i = bGridOrder ? 3 : 2;
      quad_points[i] = ON_3dPoint(m_ctrlnetP[3]);
      quad_normal = ON_3dVector(m_ctrlnetN);
    }
    else
    {
      for (int i = 0; i < 4; i++)
        quad_points[i] = ON_3dPoint::NanPoint;
      quad_normal = ON_3dVector::ZeroVector;
    }
  }
  return rc;
}

const ON_SubDMeshFragmentGrid& ON_SubDMeshFragment::Grid(ON_SubDComponentLocation subd_appearance) const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? ON_SubDMeshFragmentGrid::OneQuadGrid : m_grid;
}

void ON_SubDMeshFragment::SetControlNetQuad(bool bGridOrder, const ON_3dPoint quad_points[4], ON_3dVector quad_normal )
{
  if (nullptr != quad_points && quad_points[0].IsValid() && quad_points[1].IsValid() && quad_points[2].IsValid() && quad_points[3].IsValid() && quad_normal.IsNotZero())
  {
    int i;

    m_ctrlnetP[0][0] = quad_points[0].x;
    m_ctrlnetP[0][1] = quad_points[0].y;
    m_ctrlnetP[0][2] = quad_points[0].z;

    m_ctrlnetP[1][0] = quad_points[1].x;
    m_ctrlnetP[1][1] = quad_points[1].y;
    m_ctrlnetP[1][2] = quad_points[1].z;

    i = bGridOrder ? 2 : 3;
    m_ctrlnetP[i][0] = quad_points[2].x;
    m_ctrlnetP[i][1] = quad_points[2].y;
    m_ctrlnetP[i][2] = quad_points[2].z;

    i = bGridOrder ? 3 : 2;
    m_ctrlnetP[i][0] = quad_points[3].x;
    m_ctrlnetP[i][1] = quad_points[3].y;
    m_ctrlnetP[i][2] = quad_points[3].z;

    m_ctrlnetN[0] = quad_normal.x;
    m_ctrlnetN[1] = quad_normal.y;
    m_ctrlnetN[2] = quad_normal.z;
    m_vertex_count_etc |= ON_SubDMeshFragment::EtcControlNetQuadBit;
  }
  else
  {
    UnsetControlNetQuad();
  }
}

void ON_SubDMeshFragment::UnsetControlNetQuad()
{
  for (int i = 0; i < 4; ++i)
  {
    m_ctrlnetP[i][0] = m_ctrlnetP[i][1] = m_ctrlnetP[i][2] = ON_DBL_QNAN;
  }
  m_ctrlnetN[0] = m_ctrlnetN[1] = m_ctrlnetN[2] = ON_DBL_QNAN;
  m_vertex_count_etc &= ~ON_SubDMeshFragment::EtcControlNetQuadBit;
}

const ON_BoundingBox ON_SubDMeshFragment::SurfaceBoundingBox() const
{
  return m_surface_bbox;
}

const ON_BoundingBox ON_SubDMeshFragment::ControlNetQuadBoundingBox() const
{
  ON_3dPoint P[4];
  ON_3dVector N;
  if (this->GetControlNetQuad(false, P, N))
  {
    ON_BoundingBox bbox;
    bbox.Set(3, 0, 4, 3, &P[0].x, false);
    return bbox;
  }
  return ON_BoundingBox::NanBoundingBox;
}

const ON_BoundingBox ON_SubDMeshFragment::BoundingBox(ON_SubDComponentLocation subd_appearance) const
{
  return
    (ON_SubDComponentLocation::ControlNet == subd_appearance)
    ? ControlNetQuadBoundingBox()
    : SurfaceBoundingBox();
}


unsigned int ON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(
  unsigned int display_density
  )
{
  if (display_density <= ON_SubDDisplayParameters::MaximumDensity)
    return (1 << display_density);

  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDMeshFragment::DisplayDensityFromSideSegmentCount(
  unsigned int side_segment_count
  )
{
  unsigned int display_density;
  for (display_density = 0; display_density < ON_SubDDisplayParameters::MaximumDensity; display_density++)
  {
    if ( 1U << display_density >= side_segment_count )
      break;
  }

  if ( 1U << display_density == side_segment_count )
    return display_density;

  return ON_SUBD_RETURN_ERROR(display_density);
}

unsigned int ON_SubDMeshFragment::PointCountFromDisplayDensity(
  unsigned int mesh_density
  )
{
  unsigned int count = ON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  if ( 0 == count)
    return 0;
  return (count + 1)*(count+1);
}

unsigned int ON_SubDMeshFragment::FaceCountFromDisplayDensity(
  unsigned int mesh_density
  )
{
  unsigned int count = ON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  return count*count; // same for quads or tris
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// ON_SubDManagedMeshFragment
//

ON_SubDManagedMeshFragment::ON_SubDManagedMeshFragment() ON_NOEXCEPT
{
  memset(this, 0, sizeof(*this));
}

ON_SubDManagedMeshFragment::~ON_SubDManagedMeshFragment() ON_NOEXCEPT
{
  Destroy();
}

ON_SubDManagedMeshFragment::ON_SubDManagedMeshFragment(const ON_SubDManagedMeshFragment& src) ON_NOEXCEPT
{
  Clear();
  CopyHelper(src);
}

ON_SubDManagedMeshFragment& ON_SubDManagedMeshFragment::operator=(const ON_SubDManagedMeshFragment& src) ON_NOEXCEPT
{
  if (this != &src)
  {
    Clear();
    CopyHelper(src);
  }
  return *this;
}

#if defined(ON_HAS_RVALUEREF)
// rvalue copy constructor
ON_SubDManagedMeshFragment::ON_SubDManagedMeshFragment( ON_SubDManagedMeshFragment&& src ) ON_NOEXCEPT
{
  memcpy(this, &src, sizeof(*this));
  src.m_storage = nullptr;
  src.m_storage_capacity = 0;
}

// rvalue assignment operator
ON_SubDManagedMeshFragment& ON_SubDManagedMeshFragment::operator=( ON_SubDManagedMeshFragment&& src ) ON_NOEXCEPT
{
  if (this != &src)
  {
    Destroy();
    memcpy(this, &src, sizeof(*this));
    src.m_storage = nullptr;
    src.m_storage_capacity = 0;
  }
  return *this;
}

#endif

bool ON_SubDManagedMeshFragment::ReserveCapacity(
  unsigned int display_density
  ) ON_NOEXCEPT
{
  Clear();

  if ( display_density > 8)
    return ON_SUBD_RETURN_ERROR(false);

  unsigned int fragment_side_count = ON_SubDManagedMeshFragment::SideSegmentCountFromDisplayDensity(display_density);
  if ( 0 == fragment_side_count )
    return true;

  // Sanity check
  if ( fragment_side_count > 0xFFU )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int short_capacity_sanity_check = 0xFFFFU;

  const unsigned int V_capacity = ON_SubDManagedMeshFragment::PointCountFromDisplayDensity(display_density);
  if (V_capacity >= short_capacity_sanity_check )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int F_capacity = ON_SubDManagedMeshFragment::FaceCountFromDisplayDensity(display_density);
  if ( F_capacity >= short_capacity_sanity_check)
    return ON_SUBD_RETURN_ERROR(false);

  const size_t P_stride = 3;
  const size_t N_stride = 3;
  const size_t T_stride = 3;

  size_t storage_capacity = (P_stride + N_stride + T_stride)*V_capacity;
  if (storage_capacity > m_storage_capacity || nullptr == m_storage)
  {
    if (m_storage_capacity > 0 && nullptr != m_storage)
    {
      delete[] m_storage;
      m_storage = nullptr;
    }

    m_storage = new(std::nothrow) double[storage_capacity];
    if (nullptr == m_storage)
      return ON_SUBD_RETURN_ERROR(false);
    m_storage_capacity = storage_capacity;
  }

  UnsetControlNetQuad();
  SetUnmanagedVertexCapacity(V_capacity);
  SetVertexCount(V_capacity);

  m_P_stride = P_stride;
  m_N_stride = N_stride;
  m_T_stride = T_stride;

  m_P = m_storage;
  m_N = m_storage + (3*V_capacity);
  m_T = m_storage + (6*V_capacity);

  m_surface_bbox = ON_BoundingBox::NanBoundingBox;

  m_grid = ON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(display_density,0U);
  if ( nullptr == m_grid.m_F)
    return ON_SUBD_RETURN_ERROR(false);

  return true;
}

void ON_SubDManagedMeshFragment::Clear() ON_NOEXCEPT
{
  memset(this, 0, sizeof(ON_SubDMeshFragment));
}

void ON_SubDManagedMeshFragment::Destroy() ON_NOEXCEPT
{
  double* p = const_cast<double*>(m_P);
  memset(this, 0, sizeof(*this));
  if ( nullptr != p)
    delete[] p;
}

void ON_SubDManagedMeshFragment::CopyHelper(const ON_SubDMeshFragment& src)
{
  if (this == &src)
    return;

  m_vertex_capacity_etc = 0;
  m_vertex_count_etc = 0;
  m_P = nullptr;
  m_P_stride = 0;
  m_N = nullptr;
  m_N_stride = 0;
  m_T = nullptr;
  m_T_stride = 0;
  m_surface_bbox = ON_BoundingBox::NanBoundingBox;
  const size_t V_count = src.VertexCount();
  if (0 == V_count)
    return;

  const unsigned P_count = src.PointCount();
  if (0 != P_count && V_count != P_count)
  {
    ON_SUBD_ERROR("invalid counts");
    return;
  }

  const unsigned N_count = src.NormalCount();
  if (0 != N_count && V_count != N_count)
  {
    ON_SUBD_ERROR("invalid counts");
    return;
  }

  const unsigned T_count = src.TextureCoordinateCount();
  if (0 != T_count && V_count != T_count)
  {
    ON_SUBD_ERROR("invalid counts");
    return;
  }

  m_ctrlnetT[0][0] = src.m_ctrlnetT[0][0];
  m_ctrlnetT[0][1] = src.m_ctrlnetT[0][1];
  m_ctrlnetT[1][0] = src.m_ctrlnetT[1][0];
  m_ctrlnetT[1][1] = src.m_ctrlnetT[1][1];
  m_ctrlnetT[2][0] = src.m_ctrlnetT[2][0];
  m_ctrlnetT[2][1] = src.m_ctrlnetT[2][1];
  m_ctrlnetT[3][0] = src.m_ctrlnetT[3][0];
  m_ctrlnetT[3][1] = src.m_ctrlnetT[3][1];
  m_surface_bbox = src.m_surface_bbox;
  m_grid = src.m_grid;

  const size_t storage_capacity = 9U * V_count;
  m_storage = new(std::nothrow) double[storage_capacity];
  if (nullptr == m_storage)
  {
    ON_SubDIncrementErrorCount();
    return;
  }
  m_storage_capacity = storage_capacity;
  SetUnmanagedVertexCapacity(V_count);
  SetVertexCount(V_count);

  double buffer3[3];
  const double* src_p;
  size_t src_p_stride;
  double* p;
  double* p1;

  m_P = m_storage;
  m_P_stride = 3;
  p = m_P;
  p1 = p + (m_P_stride*V_count);
  if ( P_count > 0 )
  {
    src_p = src.m_P;
    src_p_stride = src.m_P_stride;
  }
  else
  {
    buffer3[0] = buffer3[1] = buffer3[2] = ON_DBL_QNAN;
    src_p = buffer3;
    src_p_stride = 0;
  }
  while (p < p1)
  {
    p[0] = src_p[0];
    p[1] = src_p[1];
    p[2] = src_p[2];
    p += m_P_stride;
    src_p += src_p_stride;
  }

  if (N_count > 0)
  {
    src_p = src.m_N;
    src_p_stride = src.m_N_stride;
  }
  else
  {
    buffer3[0] = buffer3[1] = buffer3[2] = 0.0;
    src_p = buffer3;
    src_p_stride = 0;
  }
  m_N = m_storage + (3* V_count);
  m_N_stride = 3;
  p = m_N;
  p1 = p + (m_N_stride*V_count);
  while (p < p1)
  {
    p[0] = src_p[0];
    p[1] = src_p[1];
    p[2] = src_p[2];
    p += m_N_stride;
    src_p += src_p_stride;
  }

  if (T_count > 0)
  {
    src_p = src.m_T;
    src_p_stride = src.m_T_stride;
  }
  else
  {
    buffer3[0] = buffer3[1] = buffer3[2] = ON_DBL_QNAN;
    src_p = buffer3;
    src_p_stride = 0;
  }
  m_T = m_storage + (6*V_count);
  m_T_stride = 3;
  p = m_T;
  p1 = p + (m_T_stride*V_count);
  while (p < p1)
  {
    p[0] = src_p[0];
    p[1] = src_p[1];
    p[2] = src_p[2];
    p += m_T_stride;
    src_p += src_p_stride;
  }

  ON_3dPoint quad_points[4];
  ON_3dVector quad_normal;
  src.GetControlNetQuad(false, quad_points, quad_normal);
  SetControlNetQuad(false, quad_points, quad_normal);
}

ON_SubDManagedMeshFragment ON_SubDManagedMeshFragment::Create(const ON_SubDMeshFragment& src) ON_NOEXCEPT
{
  ON_SubDManagedMeshFragment mf;
  mf.CopyHelper(src);
  return mf;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// ON_SubDMeshFragmentGrid
//

unsigned int ON_SubDMeshFragmentGrid::SetQuads(
  unsigned int side_segment_count,
  unsigned int level_of_detail,
  unsigned int* quads,
  size_t quad_capacity,
  size_t quad_stride,
  unsigned int* sides,
  size_t side_capacity,
  size_t side_stride
)
{
  if ( false == ON_SubDMeshFragment::SideSegmentCountIsValid(side_segment_count) )
    return ON_SUBD_RETURN_ERROR(0);

  if ( side_segment_count <= 1 )
    level_of_detail = 0;
  else if (level_of_detail > 0)
  {
    if (level_of_detail >= 32 || 1U << level_of_detail > side_segment_count)
    {
      level_of_detail = 1;
      while ( 2*level_of_detail < side_segment_count )
        level_of_detail*= 2;
    }
  }


  const unsigned int side_point_count = (side_segment_count+1);
  const unsigned int P_di = (1 << level_of_detail);
  const unsigned int P_dj = P_di*side_point_count;

  const unsigned int side_quad_count = side_segment_count / P_di;

  if (quad_capacity > 0 || nullptr != quads)
  {
    if ( quad_stride < 4 )
      return ON_SUBD_RETURN_ERROR(0);
    if (side_quad_count*side_quad_count > quad_capacity)
      return ON_SUBD_RETURN_ERROR(0);

    unsigned int* fvi = quads;
    for (unsigned int n = 0; n < side_quad_count; n++)
    {
      unsigned int vi0 = n*P_dj;
      unsigned int vi1 = vi0 + P_dj;
      for (const unsigned int* fvi1 = fvi + quad_stride*side_quad_count; fvi < fvi1; fvi += quad_stride)
      {
        fvi[0] = vi0;
        fvi[3] = vi1;
        vi0 += P_di;
        vi1 += P_di;
        fvi[1] = vi0;
        fvi[2] = vi1;
      }
    }
  }

  if (side_capacity > 0 || nullptr != sides)
  {
    if ( side_stride < 1 )
      return ON_SUBD_RETURN_ERROR(0);
    if (side_capacity < 4*side_quad_count +1 )
      return ON_SUBD_RETURN_ERROR(0);

    unsigned int vi = 0;

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi += P_di;
    }

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi += P_dj;
    }

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi -= P_di;
    }

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi -= P_dj;
    }

    *sides = 0;
  }

  return side_quad_count*side_quad_count;
}

bool ON_SubDMeshFragment::SideSegmentCountIsValid(
  unsigned int side_segment_count
  )
{
  if (side_segment_count > 0 && side_segment_count <= ON_SubDMeshFragment::MaximumSideSegmentCount)
  {
    for (unsigned int n = 1; n <= side_segment_count; n *= 2)
    {
      if (n == side_segment_count)
        return true;
    }
  }

  return ON_SUBD_RETURN_ERROR(false);
}

unsigned int ON_SubDMeshFragment::SidePointCountFromSideCount(
  unsigned int side_segment_count
  )
{
  return ON_SubDMeshFragment::SideSegmentCountIsValid(side_segment_count) ? (side_segment_count+1) : 0U;
}

unsigned int ON_SubDMeshFragment::QuadGridPointCountFromSideCount(
  unsigned int side_segment_count
  )
{
  unsigned int side_point_count = ON_SubDMeshFragment::SidePointCountFromSideCount(side_segment_count);
  return side_point_count*side_point_count;
}

unsigned int ON_SubDMeshFragment::QuadGridQuadCountFromSideCount(
  unsigned int side_segment_count
  )
{
  return ON_SubDMeshFragment::SideSegmentCountIsValid(side_segment_count) ? (side_segment_count*side_segment_count) : 0U;
}

const ON_SubDFace* ON_SubDMeshFragment::SubDFace() const
{
  return m_face;
}

bool ON_SubDMeshFragment::IsFullFaceFragment() const
{
  return
    nullptr != m_face
    && 4 == m_face->m_edge_count
    && 1 == m_face_fragment_count
    && 0 == m_face_fragment_index
    && 0 == m_face_vertex_index[0]
    && 1 == m_face_vertex_index[1]
    && 2 == m_face_vertex_index[2]
    && 3 == m_face_vertex_index[3]
    && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S
    ;
}

bool ON_SubDMeshFragment::IsFaceCornerFragment() const
{
  return ON_UNSET_UINT_INDEX != FaceCornerIndex();
}

unsigned int ON_SubDMeshFragment::FaceCornerIndex() const
{
  return (
    nullptr != m_face
    && (3 == m_face->m_edge_count || m_face->m_edge_count >= 5)
    && m_face->m_edge_count == m_face_fragment_count
    && m_face_fragment_index < m_face_fragment_count
    && m_face_vertex_index[2] < m_face->m_edge_count
    && m_face_vertex_index[0] > ON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[1] > ON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[3] > ON_SubDFace::MaximumEdgeCount
    && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S
    )
    ? ((unsigned int)m_face_vertex_index[2])
    : ON_UNSET_UINT_INDEX
    ;
}

const ON_SubDMeshFragment* ON_SubDMeshFragment::FirstFaceFragment() const
{
  if (IsFullFaceFragment())
    return this;
  if (false == IsFaceCornerFragment())
    return nullptr;

  const ON_SubDMeshFragment* first = this;
  while (nullptr != first && first->m_face_fragment_index > 0)
    first = first->PreviousFaceFragment(false);
  if (nullptr == first)
    return nullptr;
  const ON_SubDMeshFragment* last = first->NextFaceFragment(false);
  while (nullptr != last && last->m_face_fragment_index + 1 < last->m_face_fragment_count)
    last = last->NextFaceFragment(false);
  if (nullptr == last)
    return nullptr;
  return first;
}

const ON_SubDMeshFragment* ON_SubDMeshFragment::LastFaceFragment() const
{
  if (IsFullFaceFragment())
    return this;
  if (false == IsFaceCornerFragment())
    return nullptr;

  const ON_SubDMeshFragment* last = this;
  while (nullptr != last && last->m_face_fragment_index + 1 < m_face_fragment_count)
    last = last->NextFaceFragment(false);
  if (nullptr == last)
    return nullptr;
  const ON_SubDMeshFragment* first = last->PreviousFaceFragment(false);
  while (nullptr != first && first->m_face_fragment_index > 0)
    first = first->PreviousFaceFragment(false);
  if (nullptr == first)
    return nullptr;
  return last;
}

const ON_SubDMeshFragment* ON_SubDMeshFragment::PreviousFaceFragment(
  bool bLastFromFirst
) const
{
  if ( nullptr != m_face && m_face_fragment_index < m_face_fragment_count )
  {
    if (bLastFromFirst && 0 == m_face_fragment_index)
    {
      return LastFaceFragment();
    }
    else if (
      m_face_fragment_index > 0
      && nullptr != m_prev_fragment
      && m_face == m_prev_fragment->m_face
      && m_face_fragment_count == m_prev_fragment->m_face_fragment_count
      && m_face_fragment_index - 1 == m_prev_fragment->m_face_fragment_index
      )
    {
      return m_prev_fragment;
    }
  }
  return nullptr;
}

const ON_SubDMeshFragment* ON_SubDMeshFragment::NextFaceFragment(
  bool bFirstFromLast
) const
{
  if ( nullptr != m_face && m_face_fragment_index < m_face_fragment_count )
  {
    if (bFirstFromLast && m_face_fragment_index+1 == m_face_fragment_count)
    {
      return FirstFaceFragment();
    }
    else if (
      m_face_fragment_index + 1 < m_face_fragment_count
      && nullptr != m_next_fragment
      && m_face == m_next_fragment->m_face
      && m_face_fragment_count == m_next_fragment->m_face_fragment_count
      && m_face_fragment_index + 1 == m_next_fragment->m_face_fragment_index
      )
    {
      return m_next_fragment;
    }
  }
  return nullptr;
}

unsigned int ON_SubDMeshFragment::FaceFragmentCount() const
{
  return m_face_fragment_count;
}

unsigned int ON_SubDMeshFragment::GetFaceFragments(
  const ON_SubDMeshFragment** fragments,
  size_t fragments_capacity
) const
{
  const unsigned int fragment_count = FaceFragmentCount();
  if (fragments_capacity < (size_t)(fragment_count > 0 ? fragment_count : 1U))
    return 0;
  const ON_SubDMeshFragment* fragment = FirstFaceFragment();
  for (unsigned i = 0; i < fragment_count; ++i, fragment = fragment->m_next_fragment)
  {
    if (nullptr == fragment)
      return 0;
    fragments[i] = fragment;
  }
  return fragment_count;
}

unsigned int ON_SubDMeshFragment::GetFaceFragments(
  ON_SimpleArray<const ON_SubDMeshFragment*>& fragments
) const
{
  unsigned int fragment_count = FaceFragmentCount();
  fragments.SetCount(0);
  fragments.Reserve(fragment_count);
  fragment_count = GetFaceFragments(fragments.Array(), fragment_count);
  fragments.SetCapacity(fragment_count);
  return fragment_count;
}

const ON_3dPoint ON_SubDMeshFragment::VertexPoint(
  ON_2udex grid2dex
) const
{
  return VertexPoint(m_grid.PointIndexFromGrid2dex(grid2dex.i, grid2dex.j));
}

const ON_3dPoint ON_SubDMeshFragment::VertexPoint(
  unsigned grid2dex_i,
  unsigned grid2dex_j
) const
{
  return VertexPoint(m_grid.PointIndexFromGrid2dex(grid2dex_i, grid2dex_j));
}

const ON_3dPoint ON_SubDMeshFragment::VertexPoint(
  unsigned grid_point_index
) const
{
  if (grid_point_index >= (unsigned)PointCount())
    return ON_3dPoint::NanPoint;
  return ON_3dPoint(m_P + grid_point_index * m_P_stride);
}

const ON_3dVector ON_SubDMeshFragment::VertexNormal(
  ON_2udex grid2dex
) const
{
  return VertexNormal(m_grid.PointIndexFromGrid2dex(grid2dex.i, grid2dex.j));
}

const ON_3dVector ON_SubDMeshFragment::VertexNormal(
  unsigned grid2dex_i,
  unsigned grid2dex_j
) const
{
  return VertexNormal(m_grid.PointIndexFromGrid2dex(grid2dex_i, grid2dex_j));
}

const ON_3dVector ON_SubDMeshFragment::VertexNormal(
  unsigned grid_point_index
) const
{
  return
    (grid_point_index >= NormalCount())
    ? ON_3dVector::ZeroVector
    : ON_3dVector(m_N + grid_point_index * m_N_stride)
    ;
}

const class ON_SubDEdge* ON_SubDMeshFragment::SubDEdge(
  unsigned int grid_side_index
  ) const
{
  return SubDEdgePtr(grid_side_index).Edge();
}

const class ON_SubDEdgePtr ON_SubDMeshFragment::SubDEdgePtr(
  unsigned int grid_side_index
  ) const
{
  const unsigned int grid_side_count = 4; // will be 3 for a tri sub-D
  if (nullptr != m_face &&  m_face->m_edge_count >= 3 && grid_side_index < grid_side_count)
  {
    unsigned short fei = m_face_vertex_index[grid_side_index];
    if (fei < m_face->m_edge_count)
      return m_face->EdgePtr(fei);
    fei = m_face_vertex_index[(grid_side_index+1) % grid_side_count];
    if (fei < m_face->m_edge_count)
      return m_face->EdgePtr((fei+m_face->m_edge_count-1) % m_face->m_edge_count);
  }
  return ON_SubDEdgePtr::Null;
}

const class ON_SubDVertex* ON_SubDMeshFragment::SubDVertex(
  unsigned int grid_corner_index
  ) const
{
  if (nullptr != m_face && m_face->m_edge_count >= 3 && grid_corner_index < 4)
  {
    const unsigned short fei = m_face_vertex_index[grid_corner_index];
    if (fei < m_face->m_edge_count)
      return m_face->Vertex(fei);
  }
  return nullptr;
}

const ON_3dPoint ON_SubDMeshFragment::CornerPoint(
  unsigned int grid_corner_index
  ) const
{
  if ( grid_corner_index >= 4 || nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return ON_3dPoint(m_P + (i*m_P_stride));
}

const ON_3dVector ON_SubDMeshFragment::CornerNormal(unsigned int grid_corner_index) const
{
  if ( grid_corner_index >= 4 || nullptr == m_N || m_N_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return ON_3dVector(m_N + (i*m_N_stride));
}

const ON_3dPoint ON_SubDMeshFragment::SidePoint(unsigned int grid_side_index) const
{
  if ( grid_side_index >= 4 || nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = grid_side_index*m_grid.m_side_segment_count + m_grid.m_side_segment_count/2;

  return ON_3dPoint(m_P + (i*m_P_stride));
}

const ON_3dVector ON_SubDMeshFragment::SideNormal(unsigned int grid_side_index) const
{
  if ( grid_side_index >= 4 || nullptr == m_N || m_N_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = grid_side_index*m_grid.m_side_segment_count + m_grid.m_side_segment_count/2;

  return ON_3dVector(m_N + (i*m_N_stride));
}

const ON_3dPoint ON_SubDMeshFragment::CenterPoint() const
{
  if ( nullptr == m_P || m_P_stride < 3 || m_grid.m_side_segment_count <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  if (1 == m_grid.m_side_segment_count)
  {
    return ON_3dPoint(
      0.25*(m_P[0] + m_P[m_P_stride] + m_P[2 * m_P_stride] + m_P[3 * m_P_stride]),
      0.25*(m_P[1] + m_P[1+m_P_stride] + m_P[1 + 2 * m_P_stride] + m_P[1 + 3 * m_P_stride]),
      0.25*(m_P[2] + m_P[2+m_P_stride] + m_P[2 + 2 * m_P_stride] + m_P[2 + 3 * m_P_stride])
    );
  }

  // otherwise there is an actual point at the center of the grid.
  const unsigned int i = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;
  return ON_3dPoint(m_P + (i*m_P_stride));
}

const ON_3dVector ON_SubDMeshFragment::CenterNormal() const
{
  if (nullptr == m_N || (m_N_stride != 0 && m_N_stride < 3) || m_grid.m_side_segment_count <= 0 || nullptr == m_grid.m_S)
    return ON_3dVector::NanVector;

  if (1 == m_grid.m_side_segment_count)
  {
    const ON_3dVector N(
      (m_N[0] + m_N[m_N_stride] + m_N[2 * m_N_stride] + m_N[3 * m_N_stride]),
      (m_N[1] + m_N[1 + m_N_stride] + m_N[1 + 2 * m_N_stride] + m_N[1 + 3 * m_N_stride]),
      (m_N[2] + m_N[2 + m_N_stride] + m_N[2 + 2 * m_N_stride] + m_N[2 + 3 * m_N_stride])
    );
    return N.UnitVector();
  }

  const unsigned int i = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;

  return ON_3dVector(m_N + (i*m_N_stride));
}

bool ON_SubDMeshFragment::Internal_GetFrameHelper(
  unsigned int P_dex,
  unsigned int Q_dex,
  ON_Plane& frame
) const
{
  const unsigned int P_count = PointCount();
  if ( P_dex < P_count
    && Q_dex < P_count
    && NormalCount() == PointCount()
    )
  {
    const ON_3dPoint P(VertexPoint(P_dex));
    const ON_3dVector Z(VertexNormal(P_dex));
    if (P.IsValid() && Z.IsNotZero())
    {
      const ON_3dPoint Q(VertexPoint(Q_dex));
      const ON_3dVector V = (Q - P).UnitVector();
      const ON_3dVector X = (V - (Z*V)*Z).UnitVector();
      const ON_3dVector Y = ON_CrossProduct(Z, X).UnitVector();
      if ( X.IsUnitVector() && Y.IsUnitVector() 
        && fabs(X*Z) <= ON_SQRT_EPSILON
        && fabs(Y*Z) <= ON_SQRT_EPSILON
        )
      {
        frame.origin = P;
        frame.xaxis = X;
        frame.yaxis = Y;
        frame.zaxis = Z;
        frame.UpdateEquation();
      }
      else
      {
        frame = ON_Plane(P, Z);
      }
      return true;
    }
  }
  return false;
}


const ON_Plane ON_SubDMeshFragment::CornerFrame(
  unsigned int grid_corner_index
) const
{
  if (grid_corner_index < 4 && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S)
  {
    unsigned int S_dex = grid_corner_index * m_grid.m_side_segment_count;
    ON_Plane corner_frame;
    if (Internal_GetFrameHelper(m_grid.m_S[S_dex], m_grid.m_S[S_dex + 1], corner_frame))
      return corner_frame;
  }

  return ON_Plane::NanPlane;
}

const ON_Plane ON_SubDMeshFragment::SideFrame(unsigned int grid_side_index) const
{
  const unsigned int count = m_grid.m_side_segment_count;
  if (grid_side_index < 4U && count > 0 && nullptr != m_grid.m_S)
  {
    const unsigned int S_dex = grid_side_index * count + count / 2U;
    const unsigned int S_dex1 = (S_dex < (4U * count)) ? (S_dex + 1U) : (S_dex - 1U);
    ON_Plane side_frame;
    if (Internal_GetFrameHelper(m_grid.m_S[S_dex], m_grid.m_S[S_dex1], side_frame))
      return side_frame;
  }

  return ON_Plane::NanPlane;
}


const bool ON_SubDMeshFragment::HasValidPointAndNormalGrid() const
{
  return
    nullptr != m_P
    && m_P_stride >= 3
    && nullptr != m_N
    && ( 0 == m_N_stride || m_N_stride >= 3)
    && m_grid.m_side_segment_count > 0
    && nullptr != m_grid.m_S
    ;
}

const ON_Plane ON_SubDMeshFragment::CenterFrame() const
{
  if (false == HasValidPointAndNormalGrid() )
    return ON_Plane::NanPlane;

  if (1 == m_grid.m_side_segment_count)
  {
    const ON_3dPoint P(
      0.25*(m_P[0] + m_P[m_P_stride] + m_P[2 * m_P_stride] + m_P[3 * m_P_stride]),
      0.25*(m_P[1] + m_P[1 + m_P_stride] + m_P[1 + 2 * m_P_stride] + m_P[1 + 3 * m_P_stride]),
      0.25*(m_P[2] + m_P[2 + m_P_stride] + m_P[2 + 2 * m_P_stride] + m_P[2 + 3 * m_P_stride])
    );
    if( false == (P.x == P.x))
      return ON_Plane::NanPlane;
    ON_3dVector N = ON_3dVector(
      (m_N[0] + m_N[m_N_stride] + m_N[2 * m_N_stride] + m_N[3 * m_N_stride]),
      (m_N[1] + m_N[1 + m_N_stride] + m_N[1 + 2 * m_N_stride] + m_N[1 + 3 * m_N_stride]),
      (m_N[2] + m_N[2 + m_N_stride] + m_N[2 + 2 * m_N_stride] + m_N[2 + 3 * m_N_stride])
    ).UnitVector();
    if (false == N.IsUnitVector())
      N = ON_3dVector(m_N).UnitVector();
    if ( N.IsUnitVector() )
    {
      ON_Plane center_frame(P, N);
      const ON_3dPoint Q(
        0.5*(m_P[m_P_stride] + m_P[3 * m_P_stride]),
        0.5*(m_P[1 + m_P_stride] + m_P[1 + 3 * m_P_stride]),
        0.5*(m_P[2 + m_P_stride] + m_P[2 + 3 * m_P_stride])
      );
      const ON_3dVector V = (Q - P).UnitVector();
      const ON_3dVector X = (V - (N*V)*V).UnitVector();
      if (X.IsUnitVector())
      {
        center_frame.xaxis = X;
        center_frame.yaxis = ON_CrossProduct(N, X);
      }
      return center_frame;
    }
  }
  else
  {
    const unsigned int P_dex = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;
    ON_Plane center_frame;
    if (Internal_GetFrameHelper(P_dex, P_dex + 1, center_frame))
      return center_frame;
  }

  return ON_Plane::NanPlane;
}

const class ON_SubDVertexPtr ON_SubDMeshFragment::SubDVertexPtr(
  unsigned int grid_corner_index
  ) const
{
  return ON_SubDVertexPtr::Create(SubDVertex(grid_corner_index));
}

ON_ComponentStatus ON_SubDMeshFragment::Status() const
{
  return (nullptr == m_face) ? ON_ComponentStatus::NoneSet : m_face->m_status;
}


unsigned int ON_SubDMeshFragmentGrid::LevelOfDetail() const
{
  return m_F_level_of_detail;
}

unsigned int ON_SubDMeshFragmentGrid::DisplayDensity() const
{
  unsigned int side_segment_count = SideSegmentCount();
  if (0 == side_segment_count)
    return ON_UNSET_UINT_INDEX;
  unsigned int display_density = 0;
  while ((side_segment_count /= 2) > 0)
    display_density++;
  return display_density;
}

unsigned int ON_SubDMeshFragmentGrid::SideSegmentCount() const
{
  return m_side_segment_count;
  //unsigned int side_segment_count = 1;
  //while( side_segment_count*side_segment_count < m_F_count)
  //  side_segment_count *= 2;
  //return (side_segment_count*side_segment_count == m_F_count) ? side_segment_count : 0;
}

unsigned int ON_SubDMeshFragmentGrid::SidePointCount() const
{
  const unsigned int i = SideSegmentCount();
  return (i > 0U) ? (i + 1U) : 0U;
}

unsigned int ON_SubDMeshFragmentGrid::GridFaceCount() const
{
  // TODO: Suport tri fragments
  unsigned int side_segment_count = SideSegmentCount();
  return side_segment_count * side_segment_count;
}


unsigned int ON_SubDMeshFragmentGrid::GridPointCount() const
{
  // TODO: Suport tri fragments
  unsigned int side_segment_count = SideSegmentCount();
  return (side_segment_count > 0U) ? ((side_segment_count + 1U)*(side_segment_count + 1U)) : 0U;
}

unsigned int ON_SubDMeshFragmentGrid::GridId() const
{
  for (;;)
  {
    if (nullptr != m_F)
      break;

    // m_F_count = 2^(2n)
    for (unsigned int id = 0; id <= 16; id += 2)
    {
      if ((1U << id) == m_F_count)
      {
        id /= 2; // id = "n"
        unsigned int lod = (m_F_level_of_detail > id) ? id : (unsigned int)m_F_level_of_detail;
        id = 32*id + 2*lod;
        return id;
      }
    }
  }

  return 0;
}

const ON_2udex ON_SubDMeshFragmentGrid::Grid2dexFromPointIndex(
  unsigned int grid_point_index
)  const
{
  for (;;)
  {
    const unsigned int side_segment_count = SideSegmentCount();
    if (0 == side_segment_count)
      break;
    const unsigned int grid_side_point_count = side_segment_count + 1;
    if (grid_point_index >= grid_side_point_count * grid_side_point_count)
      break;
    const ON_2udex griddex(
      grid_point_index % grid_side_point_count,
      grid_point_index / grid_side_point_count
    );
    return griddex;
  }
  return ON_SUBD_RETURN_ERROR(ON_2udex::Unset);
}

unsigned int ON_SubDMeshFragmentGrid::PointIndexFromGrid2dex(
  unsigned int i,
  unsigned int j
)  const
{
  for (;;)
  {
    const unsigned int side_segment_count = SideSegmentCount();
    if (0 == side_segment_count)
      break;
    const unsigned int grid_side_point_count = side_segment_count + 1;
    if (i >= grid_side_point_count && j >= grid_side_point_count)
      break;
    return grid_side_point_count * i + j;
  }
  return ON_UNSET_UINT_INDEX;
}


bool ON_SubDMeshFragmentGrid::GetGridParameters(
  unsigned int grid_point_index,
  double grid_parameters[2]
  ) const
{
  for (;;)
  {
    const unsigned int side_segment_count = SideSegmentCount();
    if ( 0 == side_segment_count )
      break;
    const unsigned int grid_side_point_count = side_segment_count + 1;
    if (grid_point_index >= grid_side_point_count*grid_side_point_count)
      break;
    const unsigned int i = grid_point_index % grid_side_point_count;
    grid_parameters[0]
      = (i < grid_side_point_count)
      ? (((double)i) / ((double)grid_side_point_count))
      : 1.0;
    const unsigned int j = grid_point_index / grid_side_point_count;
    grid_parameters[1]
      = (j < grid_side_point_count)
      ? (((double)j) / ((double)grid_side_point_count))
      : 1.0;
    return true;
  }
  grid_parameters[0] = ON_UNSET_VALUE;
  grid_parameters[1] = ON_UNSET_VALUE;
  return false;
}


ON_SubDMeshFragmentGrid ON_SubDMeshFragmentGrid::QuadGridFromSideSegmentCount(
  unsigned int side_segment_count,
  unsigned int level_of_detail
)
{
  const unsigned int display_density = ON_SubDMeshFragment::DisplayDensityFromSideSegmentCount(side_segment_count);
  if ( side_segment_count != ON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(display_density) )
    return ON_SUBD_RETURN_ERROR(ON_SubDMeshFragmentGrid::Empty);
  return ON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(display_density,level_of_detail);
}


ON_SubDMeshFragmentGrid ON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(
  unsigned int display_density,
  unsigned int level_of_detail
  )
{
  static const ON_SubDMeshFragmentGrid* grid_cache[9] = { 0 }; // side_segment_count = 1,2,4,8,16,32,64,128,256

  if ( ((size_t)display_density) >= sizeof(grid_cache)/sizeof(grid_cache[0]))
    return ON_SUBD_RETURN_ERROR(ON_SubDMeshFragmentGrid::Empty);

  const ON_SubDMeshFragmentGrid* fragment_grid = grid_cache[display_density];
  if (nullptr != fragment_grid)
  {
    while ( fragment_grid->m_F_level_of_detail < level_of_detail && nullptr != fragment_grid->m_next_level_of_detail)
      fragment_grid = fragment_grid->m_next_level_of_detail;
    return *fragment_grid;
  }

  // The code below is thread safe and constructs the ON_SubDLimitMeshFragmentGrids
  // that are resources shared by all ON_SubDLimitMeshFragments.

  static ON_SleepLock lock;
  const bool bReturnLock = lock.GetLock(50,ON_SleepLock::OneMinute,true);

  // try again in case cache was made while waiting
  fragment_grid = grid_cache[display_density];
  if (nullptr != fragment_grid)
  {
    lock.ReturnLock();
    while ( fragment_grid->m_F_level_of_detail < level_of_detail && nullptr != fragment_grid->m_next_level_of_detail)
      fragment_grid = fragment_grid->m_next_level_of_detail;
    return *fragment_grid;
  }

  // The ON_SubDMeshFragmentGrid classes created below are created one time as needed
  // and used millions of times after that. These are app workspace allocations
  // and not memory leaks. Once a grid exists, it is saved in grid_cache[] and returned
  // above the next zillion times it is required.
  ON_MemoryAllocationTracking disable_tracking(false);

  // make the requested grid
  unsigned int quad_capacity = 0;
  unsigned int side_segment_capacity = 0;
  unsigned int grid_count = 0;
  unsigned int grid_cache_index0 = display_density;
  unsigned int grid_cache_index1 = display_density;
  if (display_density <= ON_SubDDisplayParameters::DefaultDensity)
  {
    // make all the common small grids
    grid_cache_index0 = 0;
    grid_cache_index1 = ON_SubDDisplayParameters::DefaultDensity;
  }
  for (unsigned int i = grid_cache_index0; i <= grid_cache_index1; i++)
  {
    // allocate all levels of detail for each segment side count
    unsigned int s1 = (1U << i);
    for (unsigned int s2 = s1; s2 > 0; s2 /= 2)
    {
      quad_capacity += ON_SubDMeshFragment::QuadGridQuadCountFromSideCount(s2);
      side_segment_capacity += 4*s2 + 1;
      grid_count++;
    }
  }


  const unsigned int vdex_capacity = (quad_capacity*4 + side_segment_capacity);
  size_t sz1 = grid_count*sizeof(ON_SubDMeshFragmentGrid);
  size_t sz2 = vdex_capacity*sizeof(unsigned int);
  if (0 != sz2 % sizeof(ON_SubDMeshFragmentGrid))
    sz2 = (1 + sz2/sizeof(ON_SubDMeshFragmentGrid))*sizeof(ON_SubDMeshFragmentGrid);
  ON_SubDMeshFragmentGrid* grids = new (std::nothrow) ON_SubDMeshFragmentGrid[(sz1 + sz2)/sizeof(ON_SubDMeshFragmentGrid)];

  ON_SubDMeshFragmentGrid grid = ON_SubDMeshFragmentGrid::Empty;
  grid.m_F_stride = 4;
  unsigned int* vdex0 = (unsigned int*)(grids + grid_count);
  unsigned int* vdex1 = vdex0 + vdex_capacity;
  unsigned int* vdex = vdex0;

  for (unsigned int i = grid_cache_index0; i <= grid_cache_index1; i++)
  {
    const unsigned int s1 = (1U << i);
    ON_SubDMeshFragmentGrid* first_lod = grids;
    ON_SubDMeshFragmentGrid* prev_lod = nullptr;
    grid.m_F_level_of_detail = 0;
    for (unsigned int s2 = s1; s2 > 0; s2 /= 2, grids++)
    {
      const unsigned int grid_F_capacity = ON_SubDMeshFragment::QuadGridQuadCountFromSideCount(s2);
      const unsigned int grid_S_capacity = 4*s2 + 1;

      grid.m_side_segment_count = (unsigned char)s2;
      grid.m_F_count = (unsigned short)grid_F_capacity;
      grid.m_F = vdex;
      vdex += 4*grid_F_capacity;
      grid.m_S = vdex;
      vdex += grid_S_capacity;

      if (vdex > vdex1)
      {
        ON_SubDIncrementErrorCount();
        break;
      }

      ON_SubDMeshFragmentGrid::SetQuads(
        s1, // top level side_segment_count
        grid.m_F_level_of_detail,
        const_cast<unsigned int*>(grid.m_F),
        grid_F_capacity,
        grid.m_F_stride,
        const_cast<unsigned int*>(grid.m_S),
        grid_S_capacity,
        1U
        );

      *grids = grid;
      if ( nullptr != prev_lod )
      {
        grids->m_prev_level_of_detail = prev_lod;
        prev_lod->m_next_level_of_detail = grids;
      }
      prev_lod = grids;

      grid.m_F += (grid.m_F_count*grid.m_F_stride);
      grid.m_F_level_of_detail++;
    }

    // Do not initialize grid_cache[i] until entire linked list is ready to be used.
    // This way if the lock is stolen for some unforseen reason, we risk leaking memory
    // but we will not crash.
    grid_cache[i] = first_lod;
  }

  if ( bReturnLock )
    lock.ReturnLock();

  if (vdex != vdex1)
  {
    ON_SubDIncrementErrorCount();
  }

  fragment_grid = grid_cache[display_density];
  if (nullptr != fragment_grid)
  {
    while ( fragment_grid->m_F_level_of_detail < level_of_detail && nullptr != fragment_grid->m_next_level_of_detail)
      fragment_grid = fragment_grid->m_next_level_of_detail;
    return *fragment_grid;
  }

  return ON_SUBD_RETURN_ERROR(ON_SubDMeshFragmentGrid::Empty);
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDMesh
//

void ON_SubDMeshImpl::ClearFragmentFacePointers(
  bool bResetSubDWeakPtr
)
{
  // When the ON_SubDimple that manages the faces referenced by
  // fragment->m_face is deleted, fragment->m_face must be set to zero.
  if (bResetSubDWeakPtr)
    m_subdimple_wp.reset();
  if (nullptr != m_first_fragment && nullptr != m_first_fragment->m_face)
  {
    for (auto fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
      fragment->m_face = nullptr;
  }
}
//
//bool ON_SubDMeshImpl::ReserveCapacityx(
//  const class ON_SubD& subd,
//  ON_SubDDisplayParameters limit_mesh_parameters
//  )
//{
//  const unsigned int level_index = limit_mesh_parameters.m_level_index;
//
//  unsigned int subd_fragment_count = subd.MeshFragmentCount(level_index);
//  if ( subd_fragment_count < 1 )
//    return ON_SUBD_RETURN_ERROR(false);
//
//  ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subd.LevelSubDType(level_index));
//
//  return ReserveCapacity(
//    subd_fragment_count,
//    facet_type,
//    limit_mesh_parameters.m_display_density,
//    level_index);
//}

bool ON_SubDMeshImpl::ReserveCapacity(
  unsigned int subd_fragment_count,
  unsigned int display_density
  )
{
  ClearTree();

  m_display_density = 0;
  m_fragment_count = 0;
  m_fragment_point_count = 0;
  m_first_fragment = nullptr;

  if ( display_density > ON_SubDDisplayParameters::MaximumDensity)
    return ON_SUBD_RETURN_ERROR(false);

  unsigned int fragment_point_count = ON_SubDMeshFragment::PointCountFromDisplayDensity(display_density);
  if ( subd_fragment_count < 1 )
    return ON_SUBD_RETURN_ERROR(false);

  size_t sizeof_PNT = 9*fragment_point_count*sizeof(double);
  size_t sizeof_fragment = sizeof(ON_SubDMeshFragment);
  if (0 != sizeof_fragment % sizeof(double))
  {
    sizeof_fragment = (1 + sizeof_fragment / sizeof(double))*sizeof(double);
  }

  if( false == m_fsp.Create(sizeof_fragment + sizeof_PNT,subd_fragment_count,0))
    return ON_SUBD_RETURN_ERROR(false);

  m_display_density = display_density;
  m_fragment_point_count = fragment_point_count;

  return true;
}

size_t ON_SubDMeshFragment::SizeofFragment(unsigned int display_density)
{
  if (display_density > ON_SubDDisplayParameters::MaximumDensity)
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned side_seg_count = ON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(display_density);
  if (side_seg_count < 1)
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned vertex_capacity = (side_seg_count + 1)*(side_seg_count + 1);
  size_t sz = sizeof(ON_SubDMeshFragment);
  while (0 != (sz % sizeof(double)))
    ++sz;
  sz += vertex_capacity * 9 * sizeof(double); // 8=3+3+3 (3d points in m_P[], 3d vectors in m_N[], and 3d points in m_T[]).
  return sz;
}

bool ON_SubDMeshFragment::CopyFrom(
  const ON_SubDMeshFragment& src_fragment
)
{
  unsigned int display_density = ON_UNSET_UINT_INDEX;
  return ON_SubDMeshFragment::CopyFrom(src_fragment, display_density);
}

bool ON_SubDMeshFragment::CopyFrom(
  const ON_SubDMeshFragment& src_fragment,
  unsigned int display_density
  )
{
  m_face = src_fragment.m_face;
  m_face_vertex_index[0] = src_fragment.m_face_vertex_index[0];
  m_face_vertex_index[1] = src_fragment.m_face_vertex_index[1];
  m_face_vertex_index[2] = src_fragment.m_face_vertex_index[2];
  m_face_vertex_index[3] = src_fragment.m_face_vertex_index[3];
  m_face_fragment_count = src_fragment.m_face_fragment_count;
  m_face_fragment_index = src_fragment.m_face_fragment_index;
  m_vertex_count_etc = 0;
  m_grid = ON_SubDMeshFragmentGrid::Empty;
  m_surface_bbox = ON_BoundingBox::NanBoundingBox;
  // Note well: Do not change capacity on this.
  SetVertexCount(0);

  ON_3dPoint quad_points[4];
  ON_3dVector quad_normal;
  src_fragment.GetControlNetQuad(false, quad_points, quad_normal);
  SetControlNetQuad(false, quad_points, quad_normal);

  m_ctrlnetT[0][0] = src_fragment.m_ctrlnetT[0][0];
  m_ctrlnetT[0][1] = src_fragment.m_ctrlnetT[0][1];
  m_ctrlnetT[0][2] = src_fragment.m_ctrlnetT[0][2];

  m_ctrlnetT[1][0] = src_fragment.m_ctrlnetT[1][0];
  m_ctrlnetT[1][1] = src_fragment.m_ctrlnetT[1][1];
  m_ctrlnetT[1][2] = src_fragment.m_ctrlnetT[1][2];

  m_ctrlnetT[2][0] = src_fragment.m_ctrlnetT[2][0];
  m_ctrlnetT[2][1] = src_fragment.m_ctrlnetT[2][1];
  m_ctrlnetT[2][2] = src_fragment.m_ctrlnetT[2][2];

  m_ctrlnetT[3][0] = src_fragment.m_ctrlnetT[3][0];
  m_ctrlnetT[3][1] = src_fragment.m_ctrlnetT[3][1];
  m_ctrlnetT[3][2] = src_fragment.m_ctrlnetT[3][2];

  if ( display_density > 8 && ON_UNSET_UINT_INDEX != display_density)
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned src_V_count = src_fragment.VertexCount();
  if ( 0 == src_V_count || src_V_count != src_fragment.m_grid.GridPointCount() )
    return ON_SUBD_RETURN_ERROR(false);

  if (src_V_count != src_fragment.PointCount())
    return ON_SUBD_RETURN_ERROR(false);

  const ON_SubDMeshFragmentGrid grid
    = (ON_UNSET_UINT_INDEX == display_density)
    ? src_fragment.m_grid
    : ON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(display_density,0);
  const unsigned V_count = grid.GridPointCount();
  if (VertexCapacity() < V_count)
  {
    if (false == this->ReserveManagedVertexCapacity(V_count))
      return ON_SUBD_RETURN_ERROR(false);
  }
  if ( V_count > VertexCapacity() || V_count > src_fragment.VertexCount())
    return ON_SUBD_RETURN_ERROR(false);

  m_grid = grid;

  double* p = m_P;
  size_t p_stride = m_P_stride;
  const double* src_p = src_fragment.m_P;
  size_t src_p_stride = src_fragment.m_P_stride;
  const double* src_p1 = src_p + src_p_stride * src_V_count;
  if (V_count == src_V_count)
  {
    // Most common case where srf_fragment and target have the same density - faster way to copy.

    // copy m_P[]
    while (src_p < src_p1)
    {
      p[0] = src_p[0];
      p[1] = src_p[1];
      p[2] = src_p[2];
      p += p_stride;
      src_p += src_p_stride;
    }

    if (V_count <= NormalCapacity() )
    {
      if (V_count == src_fragment.NormalCount())
      {
        // copy m_N[]
        p = m_N;
        p_stride = m_N_stride;
        src_p = src_fragment.m_N;
        src_p_stride = src_fragment.m_N_stride;
        src_p1 = src_p + src_p_stride * src_V_count;
        while (src_p < src_p1)
        {
          p[0] = src_p[0];
          p[1] = src_p[1];
          p[2] = src_p[2];
          p += p_stride;
          src_p += src_p_stride;
        }
      }
      else
      {
        ON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_N, V_count, m_N_stride);
      }
    }

    if (V_count <= TextureCoordinateCapacity())
    {
      if (V_count == src_fragment.TextureCoordinateCount())
      {
        // copy m_T[]
        SetTextureCoordinatesExist(true);
        p = m_T;
        p_stride = m_T_stride;
        src_p = src_fragment.m_T;
        src_p_stride = src_fragment.m_T_stride;
        src_p1 = src_p + src_p_stride * src_V_count;
        while (src_p < src_p1)
        {
          p[0] = src_p[0];
          p[1] = src_p[1];
          p[2] = src_p[2];
          p += p_stride;
          src_p += src_p_stride;
        }
      }
      else
      {
        ON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_T, V_count, m_T_stride);
      }
    }
  }
  else
  {
    // srf_fragment is more dense than target. Copy a subset of the points.
    const unsigned int s0 = grid.SideSegmentCount();
    const unsigned int s1 = src_fragment.m_grid.SideSegmentCount();
    unsigned int x = 1;
    while (x*s0 < s1)
      x *= 2;
    const unsigned int m = src_fragment.m_grid.SidePointCount();
    const unsigned int di = src_fragment.m_grid.PointIndexFromGrid2dex(x, 0);
    const unsigned int dj = src_fragment.m_grid.PointIndexFromGrid2dex(0, x);

    // copy m_P[]
    size_t i_stride = di * src_fragment.m_P_stride;
    size_t j_stride = m * dj * src_fragment.m_P_stride;
    for (unsigned int j = 0; j < m; j += dj)
    {
      src_p = src_fragment.m_P + j * j_stride;
      for (unsigned int i = 0; i < m; i += di)
      {
        p[0] = src_p[0];
        p[1] = src_p[1];
        p[2] = src_p[2];
        p += p_stride;
        src_p += i_stride;
      }
    }

    if (V_count <= NormalCapacity())
    {
      // copy m_N[]
      if (V_count <= src_fragment.NormalCount())
      {
        p = m_N;
        p_stride = m_N_stride;
        i_stride = di * src_fragment.m_N_stride;
        j_stride = m * dj * src_fragment.m_N_stride;
        for (unsigned int j = 0; j < m; j += dj)
        {
          src_p = src_fragment.m_N + j * j_stride;
          for (unsigned int i = 0; i < m; i += di)
          {
            p[0] = src_p[0];
            p[1] = src_p[1];
            p[2] = src_p[2];
            p += p_stride;
            src_p += i_stride;
          }
        }
      }
      else
      {
        ON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_N, V_count, m_N_stride);
      }
    }

    if (V_count <= TextureCoordinateCapacity())
    {
      // copy m_T[]
      if (V_count <= src_fragment.TextureCoordinateCount())
      {
        SetTextureCoordinatesExist(true);
        p = m_T;
        p_stride = m_T_stride;
        i_stride = di * src_fragment.m_T_stride;
        j_stride = m * dj * src_fragment.m_T_stride;
        for (unsigned int j = 0; j < m; j += dj)
        {
          src_p = src_fragment.m_T + j * j_stride;
          for (unsigned int i = 0; i < m; i += di)
          {
            p[0] = src_p[0];
            p[1] = src_p[1];
            p[2] = src_p[2];
            p += p_stride;
            src_p += i_stride;
          }
        }
      }
      else
      {
        ON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_T, V_count, m_T_stride);
      }
    }
  }

  SetVertexCount(V_count);
  m_surface_bbox = src_fragment.m_surface_bbox;

  return true;
}

ON_SubDMeshFragment* ON_SubDMeshImpl::CopyCallbackFragment(
    const ON_SubDMeshFragment* callback_fragment
    )
{
  if ( nullptr == callback_fragment)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( 0 == callback_fragment->VertexCount() )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( callback_fragment->VertexCount() > m_fragment_point_count )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( 0 == callback_fragment->PointCount() || 0 == callback_fragment->NormalCount() )
    return ON_SUBD_RETURN_ERROR(nullptr);

  // m_fsp.AllocateElement() allocates room for the fragment and the m_P[], m_N[], and m_T[] arrays.
  ON_SubDMeshFragment* fragment = (ON_SubDMeshFragment*)m_fsp.AllocateElement();
  if ( nullptr == fragment)
    return ON_SUBD_RETURN_ERROR(nullptr);

  double* P = (double*)(fragment + 1);
  double* N = P + 3 * m_fragment_point_count;
  double* T = N + 3 * m_fragment_point_count;

  fragment->SetUnmanagedVertexCapacity(m_fragment_point_count); // m_fsp manages array memory
  fragment->SetVertexCount(m_fragment_point_count);
  fragment->m_P = P;
  fragment->m_P_stride = 3;
  fragment->m_N = N;
  fragment->m_N_stride = 3;
  fragment->m_T = T;
  fragment->m_T_stride = 3;

  fragment->CopyFrom(*callback_fragment);

  ChangeContentSerialNumber();

  return fragment;
}

bool ON_SubDMeshImpl::AddFinishedFragment(
  ON_SubDMeshFragment* finished_fragment
  )
{
  if ( nullptr == finished_fragment)
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 == finished_fragment->PointCount() || 0 == finished_fragment->NormalCount() )
    return ON_SUBD_RETURN_ERROR(false);
  if ( finished_fragment->PointCount() > m_fragment_point_count )
    return ON_SUBD_RETURN_ERROR(false);

  m_fragment_count++;

  if (nullptr == m_first_fragment)
  {
    m_first_fragment = finished_fragment;
    m_last_fragment = finished_fragment;
    m_bbox = finished_fragment->m_surface_bbox;
  }
  else
  {
    m_last_fragment->m_next_fragment = finished_fragment;
    finished_fragment->m_prev_fragment = m_last_fragment;
    m_last_fragment = finished_fragment;
    m_bbox.Union(finished_fragment->m_surface_bbox);
  }

  ChangeContentSerialNumber();

  return true;
}

class ON_SubDLimitMeshSealEdgeInfo
{
public:
  ON_SubDLimitMeshSealEdgeInfo() = default;
  ~ON_SubDLimitMeshSealEdgeInfo() = default;
  ON_SubDLimitMeshSealEdgeInfo(const ON_SubDLimitMeshSealEdgeInfo&) = default;
  ON_SubDLimitMeshSealEdgeInfo& operator=(const ON_SubDLimitMeshSealEdgeInfo&) = default;

  static const ON_SubDLimitMeshSealEdgeInfo Unset;

  enum Bits : unsigned char
  {
    // Set if edge orientation is opposite face boundary ccw orientation.
    EdgeDir = 0x01,

    // Set if the edge is smooth and normals should be sealed along with location.
    Smooth = 0x02,

    // The high bits are used for half edges so they will sort after full edges
    FirstHalf = 0x40,
    SecondHalf = 0x80,
    HalfMask = 0xC0,

    // Used to ignore edge dir
    IgnoreEdgeDirMask = 0xFE
  };

  bool SetEdge(
    unsigned int grid_side_dex
  )
  {
    for (;;)
    {
      if (nullptr == m_fragment || nullptr == m_fragment->m_face)
        break;

      m_face_edge_count = m_fragment->m_face->m_edge_count;
      if (m_face_edge_count < 3 || m_face_edge_count > ON_SubDFace::MaximumEdgeCount)
        break; // bogus face

      const ON_SubDEdgePtr eptr = m_fragment->SubDEdgePtr(grid_side_dex);
      const ON_SubDEdge* edge = eptr.Edge();
      m_edge_id = (nullptr != edge && edge->m_face_count >= 2) ? edge->m_id : 0;
      if (0 == m_edge_id)
        break; // nothing to seal

      const bool bCompleteFragment = m_fragment->IsFullFaceFragment();
      const bool bPartialFragment
        = (false == bCompleteFragment)
        && m_fragment->m_face_vertex_index[0] > ON_SubDFace::MaximumEdgeCount
        && m_fragment->m_face_vertex_index[1] > ON_SubDFace::MaximumEdgeCount
        && m_fragment->m_face_vertex_index[2] < ON_SubDFace::MaximumEdgeCount
        && m_fragment->m_face_vertex_index[3] > ON_SubDFace::MaximumEdgeCount
        ;

      if (false == bCompleteFragment && false == bPartialFragment)
      {
        ON_SUBD_ERROR("Unexpected m_face_vertex[] falues in partial fragment.");
        break;
      }

      m_bits = 0;
      const ON__UINT_PTR edge_dir = eptr.EdgeDirection();
      if (bPartialFragment)
      {
        // The high bit is used for partial fragments so they will sort after full fragments.
        if ( 1 == grid_side_dex )
          m_bits |= (0==edge_dir) ? Bits::SecondHalf : Bits::FirstHalf;
        else if ( 2 == grid_side_dex )
          m_bits |= (0==edge_dir) ? Bits::FirstHalf : Bits::SecondHalf;
        else
        {
          // this is an inteior edge of a partial fragment and it
          // is alwasy sealed with its neighbor when it is created.
          break;
        }
      }
      if (0 != edge_dir)
        m_bits |= Bits::EdgeDir;
      if (edge->IsSmooth())
        m_bits |= Bits::Smooth;

      m_grid_side_dex = (unsigned char)grid_side_dex; // 0,1,2, or 3

      return true;
    }


    m_edge_id = 0;
    m_bits = 0;
    m_grid_side_dex = 0;
    m_face_edge_count = 0;
    return false;
  }

  static bool Seal(
    const ON_SubDLimitMeshSealEdgeInfo& src,
    const ON_SubDLimitMeshSealEdgeInfo& dst
  );


  static int CompareEdgeIdBitsFaceId(
    const ON_SubDLimitMeshSealEdgeInfo* lhs,
    const ON_SubDLimitMeshSealEdgeInfo* rhs
  )
  {
    // sort by edge id
    if (lhs->m_edge_id < rhs->m_edge_id)
      return -1;
    if (lhs->m_edge_id > rhs->m_edge_id)
      return 1;

    // then sort by bits with full edges before half edges
    // Critical to ignore the EdgeDir bit because we need
    // the identical sections of edges to be sorted together.
    // When an edge has 2 N-gons (N != 4) attached, we need
    // the pairs for the first half and 2nd half sorted toether
    const unsigned char lhs_bits = (lhs->m_bits & ON_SubDLimitMeshSealEdgeInfo::Bits::IgnoreEdgeDirMask);
    const unsigned char rhs_bits = (rhs->m_bits & ON_SubDLimitMeshSealEdgeInfo::Bits::IgnoreEdgeDirMask);
    if (lhs_bits < rhs_bits)
      return -1;
    if (lhs_bits > rhs_bits)
      return 1;

    // then sort by face id
    unsigned int lhs_face_id = (nullptr != lhs->m_fragment->m_face) ? lhs->m_fragment->m_face->m_id : 0xFFFFFFFF;
    unsigned int rhs_face_id = (nullptr != rhs->m_fragment->m_face) ? rhs->m_fragment->m_face->m_id : 0xFFFFFFFF;
    if (0 == lhs_face_id)
      lhs_face_id = 0xFFFFFFFE;
    if (0 == rhs_face_id)
      rhs_face_id = 0xFFFFFFFE;
    if (lhs_face_id < rhs_face_id)
      return -1;
    if (lhs_face_id > rhs_face_id)
      return 1;

    return 0;
  }

public:
  unsigned int m_edge_id = 0;

  // m_bits is bitwise or of ON_SubDLimitMeshSealEdgeInfo::Bits enum values
  unsigned char m_bits = 0;
  unsigned char m_grid_side_dex = 0; // 0,1,2,or 3
  unsigned short m_face_edge_count = 0;

  ON_SubDMeshFragment* m_fragment = nullptr;
};

const ON_SubDLimitMeshSealEdgeInfo ON_SubDLimitMeshSealEdgeInfo::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDLimitMeshSealEdgeInfo);

bool ON_SubDLimitMeshSealEdgeInfo::Seal(
  const ON_SubDLimitMeshSealEdgeInfo& src,
  const ON_SubDLimitMeshSealEdgeInfo& dst
)
{
  if (src.m_edge_id != dst.m_edge_id || 0 == src.m_edge_id)
    return false;
  if (nullptr == src.m_fragment || nullptr == dst.m_fragment)
    return false;
  const bool bSealNormals = (0 != (src.m_bits & ON_SubDLimitMeshSealEdgeInfo::Bits::Smooth));
  const unsigned char src_half = (src.m_bits & ON_SubDLimitMeshSealEdgeInfo::Bits::HalfMask);
  const unsigned char dst_half = (dst.m_bits & ON_SubDLimitMeshSealEdgeInfo::Bits::HalfMask);
  unsigned int src_side_segment_count = src.m_fragment->m_grid.m_side_segment_count;
  unsigned int dst_side_segment_count = dst.m_fragment->m_grid.m_side_segment_count;
  unsigned int i0 = src.m_grid_side_dex*src_side_segment_count;
  unsigned int i1 = i0 + src_side_segment_count;

  ////unsigned int vid[2] = {};
  ////const ON_SubDEdge* e = src.m_fragment->Edge(src.m_grid_side_dex);
  ////if (nullptr != e )
  ////{
  ////  const ON_SubDVertex* v = src.m_fragment->Vertex(src.m_grid_side_dex);
  ////  if ( nullptr != v )
  ////    vid[0] = v->m_id;
  ////  v = src.m_fragment->Vertex((src.m_grid_side_dex+1)%4);
  ////  if ( nullptr !=v )
  ////    vid[1] = v->m_id;
  ////}

  // src_dir = 0 if SubD edge and fragment side have compatible natural orientations
  const unsigned char src_dir = (src.m_bits&ON_SubDLimitMeshSealEdgeInfo::Bits::EdgeDir);

  if (src_half != dst_half || src_side_segment_count != dst_side_segment_count)
  {
    if (
      0 == src_half
      && 4 == src.m_face_edge_count
      && 4 != dst.m_face_edge_count
      && 2 * dst_side_segment_count == src_side_segment_count)
    {
      // The face for src_half is a quad and the face for dest_half is an N-gon with N != 3,
      // and src_fragment is a full sized fragment and dst_fragment is a half sized fragment.
      if (ON_SubDLimitMeshSealEdgeInfo::Bits::FirstHalf == dst_half)
      {
        // only copy half of src_fragment side
        if (0 == src_dir)
        {
          i1 -= dst_side_segment_count; // copy first half of src_framgent side
          ////vid[1] = 0;
        }
        else
        {
          i0 += dst_side_segment_count; // copy 2nd half of src_framgent side
          ////vid[0] = 0;
        }
      }
      else if (ON_SubDLimitMeshSealEdgeInfo::Bits::SecondHalf == dst_half)
      {
        // only copy half of src_fragment side
        if (0 == src_dir)
        {
          i0 += dst_side_segment_count; // copy 2nd half of src_framgent side
          ////vid[0] = 0;
        }
        else
        {
          i1 -= dst_side_segment_count; // copy first half of src_framgent side
          ////vid[1] = 0;
        }
      }
      else
      {
        // bug in this code or the code that sets the m_bits information.
        ON_SUBD_ERROR("unexpected dst_half");
        return false;
      }
    }
    else
    {
      // Either the parent subd is invalid or information
      // set in the fragments, or the sorting in ON_SubDMeshImpl::SealEdges()
      // is not valid (or some other bug).
      ON_SUBD_ERROR("unexpected sealing fragment portions");
      return false;
    }
  }

  // seal this edge
  const bool bSameDirection = (src_dir == (dst.m_bits&ON_SubDLimitMeshSealEdgeInfo::Bits::EdgeDir));
  const unsigned int j0 = dst.m_grid_side_dex*dst_side_segment_count + (bSameDirection?0:dst_side_segment_count);
  const unsigned int j1 = bSameDirection ? (j0+dst_side_segment_count) : (j0-dst_side_segment_count);
  ON_SubDMeshFragment::SealAdjacentSides(
    true, // bTestNearEqual,
    bSealNormals,
    *src.m_fragment,
    i0,
    i1,
    *dst.m_fragment,
    j0,
    j1
  );
  return true;
}


void ON_SubDMeshImpl::SealEdges()
{
  ON_SimpleArray<ON_SubDLimitMeshSealEdgeInfo> fe_list(m_fragment_count);
  ON_SubDLimitMeshSealEdgeInfo fe;
  for (fe.m_fragment = m_first_fragment; nullptr != fe.m_fragment; fe.m_fragment = fe.m_fragment->m_next_fragment)
  {
    if (nullptr == fe.m_fragment->m_face)
      continue;
    for (unsigned int grid_side_dex = 0; grid_side_dex < 4; grid_side_dex++)
    {
      if ( fe.SetEdge(grid_side_dex) )
        fe_list.Append(fe);
    }
  }

  fe_list.QuickSort(ON_SubDLimitMeshSealEdgeInfo::CompareEdgeIdBitsFaceId);
  const unsigned int fe_list_count = fe_list.UnsignedCount();
  unsigned int i0 = 0;
  while ( i0 < fe_list_count )
  {
    fe = fe_list[i0];
    const unsigned char src_half_mask = (fe.m_bits & ON_SubDLimitMeshSealEdgeInfo::Bits::HalfMask);
    for ( i0++; i0 < fe_list_count && fe.m_edge_id == fe_list[i0].m_edge_id; i0++ )
    {
      if (0 != src_half_mask && 0 == (src_half_mask & fe_list[i0].m_bits))
        break; // necessary when all faces attached to an edge are not quads.
      ON_SubDLimitMeshSealEdgeInfo::Seal(fe, fe_list[i0]);
    }
  }
}

ON__UINT64 ON_SubDMesh::ContentSerialNumber() const
{
  ON_SubDMeshImpl* imple = SubLimple();
  return (nullptr != imple) ? imple->ContentSerialNumber() : 0;
}

ON__UINT64 ON_SubDMeshImpl::ContentSerialNumber() const
{
  return m_mesh_content_serial_number;
}

ON__UINT64 ON_SubDMeshImpl::ChangeContentSerialNumber()
{
  return (m_mesh_content_serial_number = ON_NextContentSerialNumber());
}

ON_SubDMeshImpl::ON_SubDMeshImpl()
{
  ChangeContentSerialNumber();
}

ON_SubDMeshImpl::ON_SubDMeshImpl( const class ON_SubDMeshImpl& src )
{
  ChangeContentSerialNumber();
  if ( nullptr != src.m_first_fragment && ReserveCapacity((unsigned int)src.m_fsp.ActiveElementCount(), src.m_display_density ) )
  {
    for (const ON_SubDMeshFragment* src_fragment = src.m_first_fragment; nullptr != src_fragment; src_fragment = src_fragment->m_next_fragment)
    {
      ON_SubDMeshFragment* fragment = CopyCallbackFragment(src_fragment);
      AddFinishedFragment(fragment);
    }
  }
}

void ON_SubDMeshImpl::ClearTree()
{
  if (nullptr != m_fragment_tree)
  {
    delete m_fragment_tree;
    m_fragment_tree = nullptr;
  }
}

const ON_RTree& ON_SubDMeshImpl::FragmentTree() const
{
  if (nullptr != m_fragment_tree && nullptr != m_first_fragment)
  {
    ON_RTree* fragment_tree = new ON_RTree();
    for (const ON_SubDMeshFragment* fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
    {
      if (fragment->PointCount() > 0 )
        fragment_tree->Insert(&(fragment->m_surface_bbox.m_min.x), &(fragment->m_surface_bbox.m_max.x), (void*)fragment);
    }
    const_cast< ON_SubDMeshImpl* >(this)->m_fragment_tree = fragment_tree;
  }
  return (nullptr == m_fragment_tree ) ? ON_RTree::Empty : *m_fragment_tree;
}

bool ON_SubDMeshImpl::GetTightBoundingBox(
  ON_BoundingBox& bbox,
  bool bGrowBox,
  const ON_Xform* xform
  ) const
{
  ON_BoundingBox local_bbox = ON_BoundingBox::EmptyBoundingBox;
  if (nullptr != xform && xform->IsIdentity())
    xform = nullptr;

  if (nullptr == xform)
  {
    local_bbox = m_bbox;
  }
  else
  {
    for (const ON_SubDMeshFragment* fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
      ON_GetPointListBoundingBox(3, 0, fragment->PointCount(), (int)fragment->m_P_stride, fragment->m_P, local_bbox, fragment != m_first_fragment, xform);
  }

  if (bGrowBox && bbox.IsValid())
    bbox.Union(local_bbox);
  else
    bbox = local_bbox;

  return true;
}

#if defined(ON_HAS_RVALUEREF)
ON_SubDMesh::ON_SubDMesh( ON_SubDMesh&& src) ON_NOEXCEPT
  : m_impl_sp(std::move(src.m_impl_sp))
{}

ON_SubDMesh& ON_SubDMesh::operator=(ON_SubDMesh&& src)
{
  if (this != &src)
  {
    m_impl_sp.reset();
    m_impl_sp = std::move(src.m_impl_sp);
  }
  return *this;
}
#endif

ON_SubDDisplayParameters ON_SubDMesh::DisplayParameters() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
  {
    ON_SubDDisplayParameters dp;
    dp.SetDisplayDensity(impl->m_display_density);
    return dp;
  }
  return ON_SubDDisplayParameters::Empty;
}

unsigned int ON_SubDMesh::DisplayDensity() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_display_density
    : 0;
}

unsigned int ON_SubDMesh::FragmentCount() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_fragment_count
    : 0;
}

const ON_SubDMeshFragment* ON_SubDMesh::FirstFragment() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_first_fragment
    : nullptr;
}

const ON_SubDMeshFragment* ON_SubDMesh::FaceFragment(
  const class ON_SubDFace* face
) const
{
  if (nullptr == face)
    return nullptr;
  for (const ON_SubDMeshFragment* fragment = FirstFragment(); nullptr != fragment; fragment = fragment->m_next_fragment)
  {
    if (face == fragment->m_face)
      return fragment;
  }
  return nullptr;
}

bool ON_SubDMesh::GetFaceCenterPointAndNormal(
  const class ON_SubDFace* face,
  double* P,
  double* N
) const
{
  if (nullptr != P)
  {
    P[0] = ON_DBL_QNAN;
    P[1] = ON_DBL_QNAN;
    P[2] = ON_DBL_QNAN;
  }
  if (nullptr != N)
  {
    N[0] = ON_DBL_QNAN;
    N[1] = ON_DBL_QNAN;
    N[2] = ON_DBL_QNAN;
  }
  const ON_SubDMeshFragment* fragment = FaceFragment(face);
  if (nullptr == fragment)
    return false;
  if (nullptr == fragment->m_P || nullptr == fragment->m_N)
    return false;
  const unsigned int n = fragment->m_grid.m_side_segment_count;
  const unsigned int P_dex = fragment->IsFullFaceFragment() ? (n*(n + 2) / 2) : 0;
  if (P_dex >= fragment->PointCount())
    return false;
  const double* fragment_P = fragment->m_P + (P_dex * fragment->m_P_stride);
  const double* fragment_N = fragment->m_N + (P_dex * fragment->m_N_stride);
  if (nullptr != P)
  {
    P[0] = fragment_P[0];
    P[1] = fragment_P[1];
    P[2] = fragment_P[2];
  }
  if (nullptr != N)
  {
    N[0] = fragment_N[0];
    N[1] = fragment_N[1];
    N[2] = fragment_N[2];
  }
  return true;
}


bool ON_SubDMesh::GetEdgeCenterPointAndNormal(
  const class ON_SubDEdge* edge,
  unsigned int edge_face_index,
  double* P,
  double* N
) const
{
  if (nullptr != P)
  {
    P[0] = ON_DBL_QNAN;
    P[1] = ON_DBL_QNAN;
    P[2] = ON_DBL_QNAN;
  }
  if (nullptr != N)
  {
    N[0] = ON_DBL_QNAN;
    N[1] = ON_DBL_QNAN;
    N[2] = ON_DBL_QNAN;
  }
  if (nullptr == edge)
    return false;
  const ON_SubDFace* face = edge->Face(edge_face_index);
  if (nullptr == face)
    return false;
  const unsigned int fei = face->EdgeArrayIndex(edge);
  if (fei >= face->EdgeCount())
    return false;

  unsigned int P_dex = ON_UNSET_UINT_INDEX;

  bool bIsPartialFragment = false;
  const ON_SubDMeshFragment* fragment = nullptr;
  for (
    fragment =  FaceFragment(face);
    nullptr != fragment && fragment->m_face == face;
    fragment = bIsPartialFragment ? fragment->m_next_fragment : nullptr
    )
  {
    bIsPartialFragment = fragment->IsFaceCornerFragment();
    for (unsigned int grid_side_index = 0; grid_side_index < 4; grid_side_index++)
    {
      const ON_SubDEdge* grid_side_edge = fragment->SubDEdge(grid_side_index);
      if (edge != grid_side_edge)
        continue;
      const unsigned int n = fragment->m_grid.m_side_segment_count;
      if (n <= 0 || nullptr == fragment->m_grid.m_S)
        break;
      if (bIsPartialFragment)
      {
        const ON_SubDVertex* v = fragment->SubDVertex(grid_side_index);
        if (nullptr != v)
        {
          const unsigned int evi = (0 == face->EdgeDirection(fei)) ? 0U : 1U;
          if (v == edge->Vertex(evi))
            grid_side_index++;
          P_dex = fragment->m_grid.m_S[grid_side_index*n];
        }
      }
      else if (fragment->IsFullFaceFragment())
      {
        P_dex = fragment->m_grid.m_S[grid_side_index*n + n / 2];
      }
      break;
    }
    if (false == bIsPartialFragment || ON_UNSET_UINT_INDEX != P_dex)
      break;
  }
  if (nullptr == fragment)
    return false;
  if (P_dex >= fragment->PointCount())
    return false;
  const double* fragment_P = fragment->m_P + (P_dex * fragment->m_P_stride);
  const double* fragment_N = fragment->m_N + (P_dex * fragment->m_N_stride);
  if (nullptr != P)
  {
    P[0] = fragment_P[0];
    P[1] = fragment_P[1];
    P[2] = fragment_P[2];
  }
  if (nullptr != N)
  {
    N[0] = fragment_N[0];
    N[1] = fragment_N[1];
    N[2] = fragment_N[2];
  }
  return true;
}

bool ON_SubDMesh::Update(
  bool bShareUpdate
  )
{
  return false;
}

bool ON_SubDMesh::IsEmpty() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr == impl || 0 == impl->m_fragment_count );
}

const ON_RTree& ON_SubDMesh::FragmentTree() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->FragmentTree();
  return ON_RTree::Empty;
}

ON_BoundingBox ON_SubDMesh::BoundingBox() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->m_bbox;
  return ON_BoundingBox::EmptyBoundingBox;
}

ON_SubD ON_SubDMesh::SubD() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  if ( nullptr == impl )
    return ON_SubD::Empty;
  ON_SubD subd;
  subd.ShareDimple(*impl);
  return subd;
}

ON_SubDRef ON_SubDMesh::SubDRef() const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  if ( nullptr == impl )
    return ON_SubDRef::Empty;
  ON_SubDRef subd_ref;
  ON_SubD& subd = subd_ref.NewSubD();
  subd.ShareDimple(*impl);
  return subd_ref;
}

bool ON_SubDMesh::GetTightBoundingBox(
  ON_BoundingBox& bbox,
  bool bGrowBox,
  const ON_Xform* xform
  ) const
{
  const ON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->GetTightBoundingBox(bbox,bGrowBox,xform);
  return (bGrowBox && bbox.IsValid());
}

void ON_SubDMesh::Clear()
{
  m_impl_sp.reset();
}

void ON_SubDMesh::ClearTree()
{
  ON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    impl->ClearTree();
}

ON_SubDMesh& ON_SubDMesh::CopyFrom(
  const ON_SubDMesh& src
  )
{
  if (this != &src)
  {
    m_impl_sp.reset();
    const ON_SubDMeshImpl* src_impl = src.m_impl_sp.get();
    if (nullptr != src_impl)
    {
      ON_SubDMeshImpl* impl = new ON_SubDMeshImpl(*src_impl);
      std::shared_ptr< ON_SubDMeshImpl > new_impl_sp(impl);
      m_impl_sp.swap(new_impl_sp);
    }
  }
  return *this;
}

void ON_SubDMesh::Swap(
  ON_SubDMesh& a,
  ON_SubDMesh& b
  )
{
  if (&a == &ON_SubDMesh::Empty || &b == &ON_SubDMesh::Empty)
  {
    ON_SubDIncrementErrorCount();
  }
  else
  {
    std::swap(a.m_impl_sp, b.m_impl_sp);
  }
}

const ON_SubDDisplayParameters ON_SubDDisplayParameters::CreateFromDisplayDensity(
  unsigned int display_density
  )
{
  if ( display_density > ON_SubDDisplayParameters::MaximumDensity )
    return ON_SUBD_RETURN_ERROR(ON_SubDDisplayParameters::Empty);

  ON_SubDDisplayParameters limit_mesh_parameters;
  limit_mesh_parameters.m_display_density = display_density;
  return limit_mesh_parameters;
}

unsigned int ON_SubDDisplayParameters::DisplayDensity() const
{
  return m_display_density;
}

void ON_SubDDisplayParameters::SetDisplayDensity(unsigned int display_density)
{
  if (display_density <= ON_SubDDisplayParameters::MaximumDensity)
    m_display_density = display_density;
}

ON_SubDComponentLocation ON_SubDDisplayParameters::MeshLocation() const
{
  return m_bControlNetMesh ? ON_SubDComponentLocation::ControlNet : ON_SubDComponentLocation::Surface;
}

void ON_SubDDisplayParameters::SetMeshLocation(ON_SubDComponentLocation mesh_location)
{
  m_bControlNetMesh = (ON_SubDComponentLocation::ControlNet == mesh_location) ? true : false;
}

bool ON_SubDDisplayParameters::AddNgons() const
{
  return m_bSkipMeshNgons ? false : true;
}

void ON_SubDDisplayParameters::SetAddNgons(bool bAddNgons)
{
  m_bSkipMeshNgons = bAddNgons ? false : true;
}

bool ON_SubDDisplayParameters::AddFakeEvaluationParameters() const
{
  return m_bSkipFakeEvaluationParameters ? false : true;
}

void ON_SubDDisplayParameters::SetAddFakeEvaluationParameters(bool bAddFakeEvaluationParameters)
{
  m_bSkipFakeEvaluationParameters = bAddFakeEvaluationParameters ? false : true;
}

unsigned char ON_SubDDisplayParameters::EncodeAsUnsignedChar() const
{
  unsigned char encoded_parameters;
  if (
    ON_SubDDisplayParameters::Default.m_display_density == m_display_density
    && ON_SubDDisplayParameters::Default.MeshLocation() == MeshLocation()
    && ON_SubDDisplayParameters::Default.m_bSkipMeshNgons == m_bSkipMeshNgons
    && ON_SubDDisplayParameters::Default.m_bSkipFakeEvaluationParameters == m_bSkipFakeEvaluationParameters
    )
  {
    // use defaults
    encoded_parameters = 0;
  }
  else
  {
    const unsigned int density
      = (m_display_density <= ON_SubDDisplayParameters::MaximumDensity)
      ? m_display_density
      : ON_SubDDisplayParameters::DefaultDensity
      ;

    const unsigned char density_as_char
      = ((unsigned char)(density & ((unsigned int)ON_SubDDisplayParameters::subd_mesh_density_mask)));

    const unsigned char location
      = (ON_SubDComponentLocation::ControlNet == MeshLocation())
      ? ON_SubDDisplayParameters::subd_mesh_location_bit
      : 0;

    const unsigned char skip_ngons
      = (m_bSkipMeshNgons)
      ? ON_SubDDisplayParameters::subd_mesh_skip_ngons_bit
      : 0;

    const unsigned char skip_fakeevalparams
      = (m_bSkipFakeEvaluationParameters)
      ? ON_SubDDisplayParameters::subd_mesh_skip_fakeevalparams_bit
      : 0;

    const unsigned char nondefault_settings = ON_SubDDisplayParameters::subd_mesh_nondefault_bit;

    encoded_parameters
      = nondefault_settings
      | density_as_char
      | location
      | skip_ngons
      | skip_fakeevalparams
      ;
  }
  return encoded_parameters;
}

const ON_SubDDisplayParameters ON_SubDDisplayParameters::DecodeFromUnsignedChar(
  unsigned char encoded_parameters
)
{
  ON_SubDDisplayParameters p(ON_SubDDisplayParameters::Default);

  if (0 != (ON_SubDDisplayParameters::subd_mesh_nondefault_bit & encoded_parameters))
  {
    const unsigned char density = (ON_SubDDisplayParameters::subd_mesh_density_mask & encoded_parameters);
    const unsigned char location = (ON_SubDDisplayParameters::subd_mesh_location_bit & encoded_parameters);
    const unsigned char skip_ngons = (ON_SubDDisplayParameters::subd_mesh_skip_ngons_bit & encoded_parameters);
    const unsigned char skip_fakeevalparams = (ON_SubDDisplayParameters::subd_mesh_skip_fakeevalparams_bit & encoded_parameters);
    p.m_display_density = (unsigned int)density;
    if (0 != location)
      p.SetMeshLocation( ON_SubDComponentLocation::ControlNet );
    if (0 != skip_ngons)
      p.m_bSkipMeshNgons = true;
    if (0 != skip_fakeevalparams)
      p.m_bSkipFakeEvaluationParameters = true;
  }

  return p;
}

bool ON_SubDDisplayParameters::UseMultipleThreads() const
{
  return m_bUseMultipleThreads;
}

void ON_SubDDisplayParameters::SetUseMultipleThreads(bool bUseMultipleThreads)
{
  m_bUseMultipleThreads = bUseMultipleThreads ? true : false;
}

ON_Terminator * ON_SubDDisplayParameters::Terminator() const
{
  return m_terminator;
}

void ON_SubDDisplayParameters::SetTerminator(ON_Terminator * terminator)
{
  m_terminator = terminator;
}

ON_ProgressReporter * ON_SubDDisplayParameters::ProgressReporter() const
{
  return m_progress_reporter;
}

const ON_Interval ON_SubDDisplayParameters::ProgressReporterInterval() const
{
  return m_progress_reporter_interval;
}

void ON_SubDDisplayParameters::SetProgressReporter(ON_ProgressReporter * progress_reporter, ON_Interval progress_reporter_interval)
{
  m_progress_reporter = progress_reporter;
  m_progress_reporter_interval = progress_reporter_interval;
}

ON_SubDMeshFragmentIterator::ON_SubDMeshFragmentIterator(const class ON_SubDMesh limit_mesh)
{
  m_limit_mesh = limit_mesh;
  m_subd.ShareDimple(m_limit_mesh.SubD());
}

ON_SubDMeshFragmentIterator::ON_SubDMeshFragmentIterator(ON_SubDRef& subd_ref)
{
  m_subd.ShareDimple(subd_ref.SubD());
  m_fit = m_subd.FaceIterator();
  m_bFromFaceFragments = true;
}

ON_SubDMeshFragmentIterator::ON_SubDMeshFragmentIterator(const ON_SubD& subd)
{
  m_subd.ShareDimple(subd);
  m_fit = m_subd.FaceIterator();
  m_bFromFaceFragments = true;
}

ON_SubDMeshFragmentIterator::ON_SubDMeshFragmentIterator(const ON_SubDFaceIterator& fit)
{
  m_subd.ShareDimple(fit.SubD());
  m_fit = fit;
  m_bFromFaceFragments = true;
}

void ON_SubDMeshFragmentIterator::Internal_CopyFrom(const ON_SubDMeshFragmentIterator& src)
{
  m_limit_mesh = src.m_limit_mesh;
  m_subd.ShareDimple(src.m_subd); // <- reason default copy ctor and op= can't be used.
  m_fit = src.m_fit;
  m_current_fragment = src.m_current_fragment;
  m_bFromFaceFragments = src.m_bFromFaceFragments;
  m_bHaveCounts = src.m_bHaveCounts;
  m_maximum_mesh_density = src.m_maximum_mesh_density;
  m_full_size_fragment_count = src.m_full_size_fragment_count;
  m_half_size_fragment_count = src.m_half_size_fragment_count;
}

ON_SubDMeshFragmentIterator::ON_SubDMeshFragmentIterator(const ON_SubDMeshFragmentIterator& src)
{
  Internal_CopyFrom(src);
}

ON_SubDMeshFragmentIterator& ON_SubDMeshFragmentIterator::operator=(const ON_SubDMeshFragmentIterator& src)
{
  if (this != &src)
    Internal_CopyFrom(src);
  return *this;
}


const ON_BoundingBox ON_SubDMeshFragmentIterator::BoundingBox() const
{
  return
    (ON_SubDComponentLocation::ControlNet == SubDAppearance())
    ? ControlNetQuadBoundingBox()
    : SurfaceBoundingBox();
}


const ON_BoundingBox ON_SubDMeshFragmentIterator::SurfaceBoundingBox() const
{
  ON_BoundingBox bbox;
  if (m_bFromFaceFragments)
    m_subd.GetTightBoundingBox(bbox);
  else
    bbox = m_limit_mesh.BoundingBox();
  return bbox;
}

const ON_BoundingBox ON_SubDMeshFragmentIterator::ControlNetQuadBoundingBox() const
{
  ON_BoundingBox bbox;
  if (m_bFromFaceFragments)
    bbox = m_subd.BoundingBox();
  else
  {
    ON_SubDMeshFragmentIterator local_frit(*this);
    for (const ON_SubDMeshFragment* fragment = local_frit.FirstFragment(); nullptr != fragment; fragment = local_frit.NextFragment())
    {
      bbox.Union(fragment->ControlNetQuadBoundingBox());
    }
  }
  return bbox;
}

bool ON_SubDMeshFragmentIterator::IsEmpty() const
{
  const ON_SubDMeshFragment* f0 = nullptr;
  if (m_bFromFaceFragments)
  {
    ON_SubDFaceIterator local_fit(m_fit); // cannot modify fit
    for (const ON_SubDFace* f = local_fit.FirstFace(); nullptr != f; f = local_fit.NextFace())
    {
      f0 = f->MeshFragments();
      if (nullptr != f0)
        break;
    }
  }
  else
  {
    f0 = m_limit_mesh.FirstFragment();
  }
  return (nullptr == f0);
}

const ON_SubDMeshFragment* ON_SubDMeshFragmentIterator::FirstFragment()
{
  if (m_bFromFaceFragments)
  {
    m_current_fragment = nullptr;
    for (const ON_SubDFace* f = m_fit.FirstFace(); nullptr != f; f = m_fit.NextFace())
    {
      m_current_fragment = f->MeshFragments();
      if (nullptr != m_current_fragment)
        break;
    }
  }
  else
  {
    m_current_fragment = m_limit_mesh.FirstFragment();
  }
  return m_current_fragment;
}

const ON_SubDMeshFragment* ON_SubDMeshFragmentIterator::NextFragment()
{
  if (nullptr != m_current_fragment)
  {
    m_current_fragment = m_current_fragment->m_next_fragment;
    if (nullptr == m_current_fragment && m_bFromFaceFragments)
    {
      for (const ON_SubDFace* f = m_fit.NextFace(); nullptr != f; f = m_fit.NextFace())
      {
        m_current_fragment = f->MeshFragments();
        if (nullptr != m_current_fragment)
          break;
      }
    }
  }
  return m_current_fragment;
}

const ON_SubDMeshFragment* ON_SubDMeshFragmentIterator::CurrentFragment()
{
  return m_current_fragment;
}

const ON_SubD& ON_SubDMeshFragmentIterator::SubD() const
{
  return m_subd;
}

unsigned int ON_SubDMeshFragmentIterator::FragmentCount() const
{
  if ( false  == m_bHaveCounts )
  {
    // lazy evaluation of this value because it's frequently never used.
    ON_SubDMeshFragmentIterator frit(*this);
    unsigned int side_count = 0;
    unsigned int full_size_fragment_count = 0;
    unsigned int half_size_fragment_count = 0;
    for (const ON_SubDMeshFragment* fragment = frit.FirstFragment(); nullptr != fragment; fragment = frit.NextFragment())
    {
      if ( 1 == fragment->m_face_fragment_count )
      {
        ++full_size_fragment_count;
        if (0 == side_count)
          side_count = fragment->m_grid.SideSegmentCount();
      }
      else if (fragment->m_face_fragment_count > 1)
      {
        ++half_size_fragment_count;
        if (0 == side_count)
          side_count = 2*fragment->m_grid.SideSegmentCount();
      }
    }
    unsigned int mesh_density = 0;
    for (unsigned int i = 1; i < side_count; i *= 2)
      ++mesh_density;

    m_maximum_mesh_density = mesh_density;
    m_full_size_fragment_count = full_size_fragment_count;
    m_half_size_fragment_count = half_size_fragment_count;

    m_bHaveCounts = true;
  }
  return m_full_size_fragment_count + m_half_size_fragment_count;
}

unsigned int ON_SubDMeshFragmentIterator::MaximumMeshDensity() const
{
  return FragmentCount() > 0 ? m_maximum_mesh_density : 0;
}

unsigned int ON_SubDMeshFragmentIterator::GetFragmentCounts(
    unsigned int& full_size_fragment_count,
    unsigned int& half_size_fragment_count
  ) const
{
  unsigned int fragment_count = FragmentCount();

  if (fragment_count > 0)
  {
    full_size_fragment_count = m_full_size_fragment_count;
    half_size_fragment_count = m_half_size_fragment_count;
  }
  else
  {
    full_size_fragment_count = 0;
    half_size_fragment_count = 0;
  }
  return fragment_count;
}

unsigned int ON_SubDMeshFragmentIterator::FullSizeFragmentCount() const
{
  return FragmentCount() > 0 ? m_full_size_fragment_count : 0;
}

unsigned int ON_SubDMeshFragmentIterator::HalfSizeFragmentCount() const
{
  return FragmentCount() > 0 ? m_half_size_fragment_count : 0;
}


unsigned int ON_SubDMeshFragmentIterator::MinimumMeshDensity() const
{
  return (MaximumMeshDensity() > 0 && HalfSizeFragmentCount() > 0) ? 1 : 0;
}

unsigned int ON_SubDMeshFragmentIterator::ClampMeshDensity(unsigned int candidate_mesh_density) const
{
  if (candidate_mesh_density > MaximumMeshDensity())
    return MaximumMeshDensity();
  if (candidate_mesh_density < MinimumMeshDensity())
    return MinimumMeshDensity();
  return candidate_mesh_density;
}

unsigned int ON_SubDMeshFragment::FullFragmentMeshQuadCountFromDensity(
  unsigned int mesh_density
)
{
  if (mesh_density > ON_SubDDisplayParameters::MaximumDensity)
    return 0;
  const unsigned int grid_side_count = 1 << mesh_density;
  return grid_side_count * grid_side_count;
}

unsigned int ON_SubDMeshFragment::HalfFragmentMeshQuadCountFromDensity(
  unsigned int mesh_density
)
{
  return
    (mesh_density >= 1 && mesh_density <= ON_SubDDisplayParameters::MaximumDensity)
    ? ON_SubDMeshFragment::FullFragmentMeshQuadCountFromDensity(mesh_density - 1)
    : 0;
}

unsigned int ON_SubDMeshFragment::FullFragmentMeshPointCountFromDensity(
  unsigned int mesh_density
)
{
  if (mesh_density > ON_SubDDisplayParameters::MaximumDensity)
    return 0;
  const unsigned int grid_side_count = 1 << mesh_density;
  return (grid_side_count+1) * (grid_side_count+1);
}

unsigned int ON_SubDMeshFragment::HalfFragmentMeshPointCountFromDensity(
  unsigned int mesh_density
)
{
  return
    (mesh_density >= 1 && mesh_density <= ON_SubDDisplayParameters::MaximumDensity)
    ? ON_SubDMeshFragment::FullFragmentMeshPointCountFromDensity(mesh_density - 1)
    : 0;
}

unsigned int ON_SubDMeshFragmentIterator::TotalQuadCount(
  unsigned int mesh_density
  ) const
{
  mesh_density = ClampMeshDensity(mesh_density);
  unsigned int full_size_fragment_count = 0;
  unsigned int half_size_fragment_count = 0;
  ON_SubDMeshFragmentIterator::GetFragmentCounts(full_size_fragment_count, half_size_fragment_count);
  const unsigned int full_size_quad_count = ON_SubDMeshFragment::FullFragmentMeshQuadCountFromDensity(mesh_density);
  const unsigned int half_size_quad_count = ON_SubDMeshFragment::HalfFragmentMeshQuadCountFromDensity(mesh_density);
  return full_size_fragment_count * full_size_quad_count + half_size_fragment_count * half_size_quad_count;
}

unsigned int ON_SubDMeshFragmentIterator::TotalPointCount(
  unsigned int mesh_density
  ) const
{
  mesh_density = ClampMeshDensity(mesh_density);
  unsigned int full_size_fragment_count = 0;
  unsigned int half_size_fragment_count = 0;
  ON_SubDMeshFragmentIterator::GetFragmentCounts(full_size_fragment_count, half_size_fragment_count);
  // const unsigned int grid_side_count = 1 << mesh_density;
  const unsigned int full_size_point_count = ON_SubDMeshFragment::FullFragmentMeshPointCountFromDensity(mesh_density);
  const unsigned int half_size_point_count = ON_SubDMeshFragment::HalfFragmentMeshPointCountFromDensity(mesh_density);
  return full_size_fragment_count * full_size_point_count + half_size_fragment_count * half_size_point_count;
}

unsigned int ON_SubDMeshFragmentIterator::MaximumDensityQuadCount() const
{
  return TotalQuadCount(MaximumMeshDensity());
}

unsigned int ON_SubDMeshFragmentIterator::MaximumDensityPointCount() const
{
  return TotalPointCount(MaximumMeshDensity());
}
