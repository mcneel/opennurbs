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

/////////////////////////////////////////////////////////////////////////////////////////
//
// ON_SubDLimitMeshFragment
//

unsigned int ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(
  unsigned int display_density
  )
{
  if (display_density <= ON_SubDLimitMesh::MaximumDisplayDensity)
    return (1 << display_density);

  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDLimitMeshFragment::DisplayDensityFromSideSegmentCount(
  unsigned int side_segment_count
  )
{
  unsigned int display_density;
  for (display_density = 0; display_density < ON_SubDLimitMesh::MaximumDisplayDensity; display_density++)
  {
    if ( 1U << display_density >= side_segment_count )
      break;
  }

  if ( 1U << display_density == side_segment_count )
    return display_density;

  return ON_SUBD_RETURN_ERROR(display_density);
}

unsigned int ON_SubDLimitMeshFragment::PointCountFromDisplayDensity(
  ON_SubD::FacetType facet_type,
  unsigned int mesh_density
  )
{
  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  if ( 0 == count)
    return 0;
  if ( ON_SubD::FacetType::Quad == facet_type )
    return (count + 1)*(count+1);

  if ( ON_SubD::FacetType::Tri == facet_type )
    return ((count+1)*(count+2))/2;

  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDLimitMeshFragment::FaceCountFromDisplayDensity(
  unsigned int mesh_density
  )
{
  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  return count*count; // same for quads or tris
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// ON_SubDManagedLimitMeshFragment
//

ON_SubDManagedLimitMeshFragment::ON_SubDManagedLimitMeshFragment() ON_NOEXCEPT
{
  memset(this, 0, sizeof(*this));
}

ON_SubDManagedLimitMeshFragment::~ON_SubDManagedLimitMeshFragment() ON_NOEXCEPT
{
  Destroy();
}

ON_SubDManagedLimitMeshFragment::ON_SubDManagedLimitMeshFragment(const ON_SubDManagedLimitMeshFragment& src) ON_NOEXCEPT
{
  Clear();
  CopyHelper(src);
}

ON_SubDManagedLimitMeshFragment& ON_SubDManagedLimitMeshFragment::operator=(const ON_SubDManagedLimitMeshFragment& src) ON_NOEXCEPT
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
ON_SubDManagedLimitMeshFragment::ON_SubDManagedLimitMeshFragment( ON_SubDManagedLimitMeshFragment&& src ) ON_NOEXCEPT
{
  memcpy(this, &src, sizeof(*this));
  src.m_storage = nullptr;
  src.m_storage_capacity = 0;
}

// rvalue assignment operator
ON_SubDManagedLimitMeshFragment& ON_SubDManagedLimitMeshFragment::operator=( ON_SubDManagedLimitMeshFragment&& src ) ON_NOEXCEPT
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

bool ON_SubDManagedLimitMeshFragment::ReserveCapacity(
  ON_SubD::FacetType facet_type,
  unsigned int mesh_density
  ) ON_NOEXCEPT
{
  Clear();

  unsigned int fragment_side_count = ON_SubDManagedLimitMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  if ( 0 == fragment_side_count )
    return true;

  // Sanity check
  if ( fragment_side_count > 0xFFU )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int short_capacity_sanity_check = 0xFFFFU;

  const unsigned int P_capacity = ON_SubDManagedLimitMeshFragment::PointCountFromDisplayDensity(facet_type,mesh_density);
  if ( P_capacity >= short_capacity_sanity_check )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int F_capacity = ON_SubDManagedLimitMeshFragment::FaceCountFromDisplayDensity(mesh_density);
  if ( F_capacity >= short_capacity_sanity_check)
    return ON_SUBD_RETURN_ERROR(false);

  const size_t P_stride = 3;
  const size_t N_stride = 3;

  size_t storage_capacity = (P_stride + N_stride)*P_capacity;
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

  m_P_capacity = (unsigned short)P_capacity;

  m_P_stride = P_stride;
  m_N_stride = N_stride;

  m_P = m_storage;
  m_N = m_P + (3*P_capacity);

  m_grid = ON_SubDLimitMeshFragmentGrid::Facets(facet_type,fragment_side_count,0U);
  if ( nullptr == m_grid.m_F)
    return ON_SUBD_RETURN_ERROR(false);

  return true;
}

void ON_SubDManagedLimitMeshFragment::Clear() ON_NOEXCEPT
{
  memset(this, 0, sizeof(ON_SubDLimitMeshFragment));
}

void ON_SubDManagedLimitMeshFragment::Destroy() ON_NOEXCEPT
{
  double* p = const_cast<double*>(m_P);
  memset(this, 0, sizeof(*this));
  if ( nullptr != p)
    delete[] p;
}

void ON_SubDManagedLimitMeshFragment::CopyHelper(const ON_SubDLimitMeshFragment& src)
{
  unsigned short P_count = (nullptr != src.m_P && src.m_P_stride >= 3) ? src.m_P_count : 0U;
  unsigned short N_count = (nullptr != src.m_N && src.m_N_stride >= 3) ? src.m_P_count : 0U;

  const size_t P_stride = 3;
  const size_t N_stride = 3;

  size_t storage_capacity = P_stride*P_count + N_stride*N_count;
  if (storage_capacity > 0)
  {
    double* p = new(std::nothrow) double[storage_capacity];
    if (nullptr == p)
    {
      ON_SubDIncrementErrorCount();
      return;
    }
    m_storage = p;
    m_storage_capacity = storage_capacity;
    if (P_count > 0)
    {
      m_P = p;
      m_P_count = P_count;
      m_P_stride = P_stride;
      double* p1 = p + (m_P_stride*P_count);
      const double* srcP = src.m_P;
      while (p < p1)
      {
        p[0] = srcP[0];
        p[1] = srcP[1];
        p[2] = srcP[2];
        p += P_stride;
        srcP += src.m_P_stride;
      }
    }
    if (N_count > 0)
    {
      m_N = p;
      m_P_count = N_count; // correct m_P_count counts both m_P and m_N.
      m_N_stride = N_stride;
      double* p1 = p + (m_N_stride*N_count);
      const double* srcN = src.m_N;
      while (p < p1)
      {
        p[0] = srcN[0];
        p[1] = srcN[1];
        p[2] = srcN[2];
        p += N_stride;
        srcN += src.m_N_stride;
      }
    }

    m_grid = src.m_grid;
  }
}

ON_SubDManagedLimitMeshFragment ON_SubDManagedLimitMeshFragment::Create(const ON_SubDLimitMeshFragment& src) ON_NOEXCEPT
{
  ON_SubDManagedLimitMeshFragment mf;
  mf.CopyHelper(src);
  return mf;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// ON_SubDLimitMeshFragmentGrid
//

unsigned int ON_SubDLimitMeshFragmentGrid::SetQuads(
    unsigned int side_segment_count,
    unsigned int level_of_detail,
    size_t quad_capacity,
    size_t quad_stride,
    unsigned int* quads,
    size_t side_capacity,
    size_t side_stride,
    unsigned int* sides
  )
{
  if ( false == ON_SubDLimitMeshFragment::SideSegmentCountIsValid(side_segment_count) )
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

bool ON_SubDLimitMeshFragment::SideSegmentCountIsValid(
  unsigned int side_segment_count
  )
{
  if (side_segment_count > 0 && side_segment_count <= ON_SubDLimitMeshFragment::MaximumSideSegmentCount)
  {
    for (unsigned int n = 1; n <= side_segment_count; n *= 2)
    {
      if (n == side_segment_count)
        return true;
    }
  }

  return ON_SUBD_RETURN_ERROR(false);
}

unsigned int ON_SubDLimitMeshFragment::SidePointCountFromSideCount(
  unsigned int side_segment_count
  )
{
  return ON_SubDLimitMeshFragment::SideSegmentCountIsValid(side_segment_count) ? (side_segment_count+1) : 0U;
}

unsigned int ON_SubDLimitMeshFragment::QuadGridPointCountFromSideCount(
  unsigned int side_segment_count
  )
{
  unsigned int side_point_count = ON_SubDLimitMeshFragment::SidePointCountFromSideCount(side_segment_count);
  return side_point_count*side_point_count;
}

unsigned int ON_SubDLimitMeshFragment::QuadGridQuadCountFromSideCount(
  unsigned int side_segment_count
  )
{
  return ON_SubDLimitMeshFragment::SideSegmentCountIsValid(side_segment_count) ? (side_segment_count*side_segment_count) : 0U;
}


const class ON_SubDEdge* ON_SubDLimitMeshFragment::Edge(
  unsigned int grid_side_index
  ) const
{
  return EdgePtr(grid_side_index).Edge();
}

const class ON_SubDEdgePtr ON_SubDLimitMeshFragment::EdgePtr(
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

const class ON_SubDVertex* ON_SubDLimitMeshFragment::Vertex(
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

const ON_3dPoint ON_SubDLimitMeshFragment::CornerPoint(
  unsigned int grid_corner_index
  ) const
{
  if ( grid_corner_index >= 4 || nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return ON_3dPoint(m_P + (i*m_P_stride));
}

const ON_3dVector ON_SubDLimitMeshFragment::CornerNormal(unsigned int grid_corner_index) const
{
  if ( grid_corner_index >= 4 || nullptr == m_N || m_N_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return ON_3dVector(m_N + (i*m_N_stride));
}

const ON_3dPoint ON_SubDLimitMeshFragment::SidePoint(unsigned int grid_side_index) const
{
  if ( grid_side_index >= 4 || nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = grid_side_index*m_grid.m_side_segment_count + m_grid.m_side_segment_count/2;

  return ON_3dPoint(m_P + (i*m_P_stride));
}

const ON_3dVector ON_SubDLimitMeshFragment::SideNormal(unsigned int grid_side_index) const
{
  if ( grid_side_index >= 4 || nullptr == m_N || m_N_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = grid_side_index*m_grid.m_side_segment_count + m_grid.m_side_segment_count/2;

  return ON_3dVector(m_N + (i*m_N_stride));
}

const ON_3dPoint ON_SubDLimitMeshFragment::CenterPoint() const
{
  if ( nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;

  return ON_3dPoint(m_P + (i*m_P_stride));
}

const ON_3dVector ON_SubDLimitMeshFragment::CenterNormal() const
{
  if ( nullptr == m_N || m_N_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::NanPoint;

  const unsigned int i = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;

  return ON_3dVector(m_N + (i*m_N_stride));
}

bool ON_SubDLimitMeshFragment::Internal_GetFrameHelper(
  unsigned int P_dex,
  unsigned int Q_dex,
  ON_Plane& frame
) const
{
  const unsigned int P_count = m_P_count;
  if ( P_dex < P_count
    && Q_dex < P_count
    && nullptr != m_P
    && m_P_stride > 0
    && nullptr != m_N
    && m_N_stride > 0
    )
  {
    const ON_3dVector Z(m_N + (P_dex*m_N_stride));
    const ON_3dPoint P(m_P + (P_dex*m_P_stride));
    const ON_3dPoint Q (m_P + (Q_dex*m_P_stride));
    ON_3dVector V = (Q - P).UnitVector();
    ON_3dVector X = (V - (frame.zaxis*V)*V).UnitVector();
    if (X.IsUnitVector())
    {
      frame.origin = P;
      frame.xaxis = X;
      frame.yaxis = ON_CrossProduct(Z, X);
      frame.zaxis = Z;
      frame.UpdateEquation();
    }
    else
    {
      frame = ON_Plane(P, Z);
    }
    return true;
  }
  return false;
}


bool ON_SubDLimitMeshFragment::GetCornerFrame(
  unsigned int grid_corner_index,
  ON_Plane& frame
) const
{
  if (grid_corner_index < 4 && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S)
  {
    unsigned int S_dex = grid_corner_index * m_grid.m_side_segment_count;
    if (Internal_GetFrameHelper(m_grid.m_S[S_dex], m_grid.m_S[S_dex + 1], frame))
      return true;
  }

  frame = ON_Plane::NanPlane;
  return false;
}

bool ON_SubDLimitMeshFragment::GetSideFrame(unsigned int grid_side_index, ON_Plane & frame) const
{
  const unsigned int count = m_grid.m_side_segment_count;
  if (grid_side_index < 4U && count > 0 && nullptr != m_grid.m_S)
  {
    const unsigned int S_dex = grid_side_index * count + count / 2U;
    const unsigned int S_dex1 = (S_dex < (4U * count)) ? (S_dex + 1U) : (S_dex - 1U);
    if (Internal_GetFrameHelper(m_grid.m_S[S_dex], m_grid.m_S[S_dex1], frame))
      return true;
  }

  frame = ON_Plane::NanPlane;
  return false;
}

bool ON_SubDLimitMeshFragment::GetCenterFrame(ON_Plane & frame) const
{
  if (m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S)
  {
    const unsigned int n = m_grid.m_side_segment_count;
    const unsigned int P_dex = (n * (n + 2))/2; // central point index
    if (Internal_GetFrameHelper(P_dex, P_dex+1, frame))
      return true;
  }

  frame = ON_Plane::NanPlane;
  return false;
}

const class ON_SubDVertexPtr ON_SubDLimitMeshFragment::VertexPtr(
  unsigned int grid_corner_index
  ) const
{
  return ON_SubDVertexPtr::Create(Vertex(grid_corner_index));
}

ON_ComponentStatus ON_SubDLimitMeshFragment::Status() const
{
  return (nullptr == m_face) ? ON_ComponentStatus::NoneSet : m_face->m_status;
}


bool ON_SubDLimitMeshFragment::IsPartialFragment() const
{
  return (
    nullptr != m_face
    && m_face->m_edge_count < ON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[0] >= ON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[1] >= ON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[2] < m_face->m_edge_count
    && m_face_vertex_index[3] >= ON_SubDFace::MaximumEdgeCount
    && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S
    );
}

bool ON_SubDLimitMeshFragment::IsCompleteFragment() const
{
  // For a complete fragment,
  // m_face_vertex_index[i] = {0,1,2,3} for quads
  // m_face_vertex_index[i] = {0,1,2,?} for tris
  return (
    nullptr != m_face
    && m_face->m_edge_count < ON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[0] < m_face->m_edge_count
    && m_face_vertex_index[1] < m_face->m_edge_count
    && m_face_vertex_index[2] < m_face->m_edge_count
    && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S
    );
}

unsigned int ON_SubDLimitMeshFragmentGrid::SideSegmentCount() const
{
  unsigned int side_segment_count = 1;
  while( side_segment_count*side_segment_count < m_F_count)
    side_segment_count *= 2;
  return (side_segment_count*side_segment_count == m_F_count) ? side_segment_count : 0;
}

unsigned int ON_SubDLimitMeshFragmentGrid::GridId() const
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
        if ( ON_SubD::FacetType::Tri == m_F_type)
          id += 1;
        else if ( ON_SubD::FacetType::Quad != m_F_type)
          break;
        return id;
      }
    }
  }

  return 0;
}

unsigned int ON_SubDLimitMeshFragmentGrid::GridFacetSideCount() const
{
  switch (m_F_type)
  {
  case ON_SubD::FacetType::Tri:
    return 3;
  case ON_SubD::FacetType::Quad:
    return 4;
  }
  return 0;
}

bool ON_SubDLimitMeshFragmentGrid::GetGridParameters(
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
    unsigned int i = grid_point_index % grid_side_point_count;
    unsigned int j = grid_point_index / grid_side_point_count;
    if ( 0 == i )
      grid_parameters[0] = 0.0;
    else if ( i == grid_side_point_count)
      grid_parameters[0] = 1.0;
    else
      grid_parameters[0] = i*1.0 / ((double)grid_side_point_count);
    if ( 0 == j )
      grid_parameters[1] = 0.0;
    else if ( j == grid_side_point_count)
      grid_parameters[1] = 1.0;
    else
      grid_parameters[1] = j*1.0 / ((double)grid_side_point_count);
    return true;
  }
  grid_parameters[0] = ON_UNSET_VALUE;
  grid_parameters[1] = ON_UNSET_VALUE;
  return false;
}

ON_SubDLimitMeshFragmentGrid ON_SubDLimitMeshFragmentGrid::Facets(
  ON_SubD::FacetType facet_type,
  unsigned int side_segment_count,
  unsigned int level_of_detail
  )
{
  if (ON_SubD::FacetType::Quad == facet_type)
    return ON_SubDLimitMeshFragmentGrid::Quads(side_segment_count, level_of_detail);

  if (ON_SubD::FacetType::Tri == facet_type)
    return ON_SubDLimitMeshFragmentGrid::Quads(side_segment_count, level_of_detail);

  return ON_SubDLimitMeshFragmentGrid::Empty;
}

ON_SubDLimitMeshFragmentGrid ON_SubDLimitMeshFragmentGrid::Tris(
  unsigned int side_segment_count,
  unsigned int level_of_detail
  )
{
  return ON_SUBD_RETURN_ERROR(ON_SubDLimitMeshFragmentGrid::Empty);
}

ON_SubDLimitMeshFragmentGrid ON_SubDLimitMeshFragmentGrid::Quads(
  unsigned int side_segment_count,
  unsigned int level_of_detail
  )
{
  static const ON_SubDLimitMeshFragmentGrid* grid_cache[9] = { 0 }; // side_segment_count = 1,2,4,8,16,32,64,128,256

  const unsigned int grid_cache_index = ON_SubDLimitMeshFragment::DisplayDensityFromSideSegmentCount(side_segment_count);
  if ( side_segment_count != ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(grid_cache_index) )
    return ON_SUBD_RETURN_ERROR(ON_SubDLimitMeshFragmentGrid::Empty);

  const ON_SubDLimitMeshFragmentGrid* fragment_grid = grid_cache[grid_cache_index];
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


  // The ON_SubDLimitMeshFragmentGrid classes created below are created one time as needed
  // and used millions of times after that. These are app workspace allocations
  // and not memory leaks. Once a grid exists, it is saved in grid_cache[] and returned
  // above the next zillion times it is required.
  ON_MemoryAllocationTracking disable_tracking(false);

  // make the requested grid
  unsigned int quad_capacity = 0;
  unsigned int side_segment_capacity = 0;
  unsigned int grid_count = 0;
  unsigned int grid_cache_index0 = grid_cache_index;
  unsigned int grid_cache_index1 = grid_cache_index;
  if (grid_cache_index <= 4)
  {
    // make all the common small grids
    grid_cache_index0 = 0;
    grid_cache_index1 = 4;
  }
  for (unsigned int i = grid_cache_index0; i <= grid_cache_index1; i++)
  {
    // allocate all levels of detail for each segment side count
    unsigned int s1 = (1U << i);
    for (unsigned int s2 = s1; s2 > 0; s2 /= 2)
    {
      quad_capacity += ON_SubDLimitMeshFragment::QuadGridQuadCountFromSideCount(s2);
      side_segment_capacity += 4*s2 + 1;
      grid_count++;
    }
  }


  const unsigned int vdex_capacity = (quad_capacity*4 + side_segment_capacity);
  size_t sz1 = grid_count*sizeof(ON_SubDLimitMeshFragmentGrid);
  size_t sz2 = vdex_capacity*sizeof(unsigned int);
  if (0 != sz2 % sizeof(ON_SubDLimitMeshFragmentGrid))
    sz2 = (1 + sz2/sizeof(ON_SubDLimitMeshFragmentGrid))*sizeof(ON_SubDLimitMeshFragmentGrid);
  ON_SubDLimitMeshFragmentGrid* grids = new (std::nothrow) ON_SubDLimitMeshFragmentGrid[(sz1 + sz2)/sizeof(ON_SubDLimitMeshFragmentGrid)];

  ON_SubDLimitMeshFragmentGrid grid = ON_SubDLimitMeshFragmentGrid::Empty;
  grid.m_F_type = ON_SubD::FacetType::Quad;
  grid.m_F_stride = 4;
  unsigned int* vdex0 = (unsigned int*)(grids + grid_count);
  unsigned int* vdex1 = vdex0 + vdex_capacity;
  unsigned int* vdex = vdex0;

  for (unsigned int i = grid_cache_index0; i <= grid_cache_index1; i++)
  {
    const unsigned int s1 = (1U << i);
    ON_SubDLimitMeshFragmentGrid* first_lod = grids;
    ON_SubDLimitMeshFragmentGrid* prev_lod = nullptr;
    grid.m_F_level_of_detail = 0;
    for (unsigned int s2 = s1; s2 > 0; s2 /= 2, grids++)
    {
      const unsigned int grid_F_capacity = ON_SubDLimitMeshFragment::QuadGridQuadCountFromSideCount(s2);
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

      ON_SubDLimitMeshFragmentGrid::SetQuads(
        s1, // top level side_segment_count
        grid.m_F_level_of_detail,
        grid_F_capacity,
        grid.m_F_stride,
        const_cast<unsigned int*>(grid.m_F),
        grid_S_capacity,
        1U,
        const_cast<unsigned int*>(grid.m_S)
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

  fragment_grid = grid_cache[grid_cache_index];
  if (nullptr != fragment_grid)
  {
    while ( fragment_grid->m_F_level_of_detail < level_of_detail && nullptr != fragment_grid->m_next_level_of_detail)
      fragment_grid = fragment_grid->m_next_level_of_detail;
    return *fragment_grid;
  }

  return ON_SUBD_RETURN_ERROR(ON_SubDLimitMeshFragmentGrid::Empty);
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDLimitMesh
//

void ON_SubDLimitMeshImpl::ClearFragmentFacePointers(
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
//bool ON_SubDLimitMeshImpl::ReserveCapacityx(
//  const class ON_SubD& subd,
//  ON_SubDDisplayParameters limit_mesh_parameters
//  )
//{
//  const unsigned int level_index = limit_mesh_parameters.m_level_index;
//
//  unsigned int subd_fragment_count = subd.LimitSurfaceMeshFragmentCount(level_index);
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

bool ON_SubDLimitMeshImpl::ReserveCapacity(
  unsigned int subd_fragment_count,
  ON_SubD::FacetType facet_type,
  unsigned int display_density
  )
{
  ClearTree();

  m_display_density = 0;
  m_facet_type = ON_SubD::FacetType::Unset;
  m_fragment_count = 0;
  m_fragment_point_count = 0;
  m_first_fragment = nullptr;

  if ( display_density > ON_SubDLimitMesh::MaximumDisplayDensity)
    return ON_SUBD_RETURN_ERROR(false);

  unsigned int fragment_point_count = ON_SubDLimitMeshFragment::PointCountFromDisplayDensity(facet_type, display_density);
  if ( subd_fragment_count < 1 )
    return ON_SUBD_RETURN_ERROR(false);

  size_t sizeof_point_and_normals = 6*fragment_point_count*sizeof(double);
  size_t sizeof_fragment = sizeof(ON_SubDLimitMeshFragment);
  if (0 != sizeof_fragment % sizeof(double))
  {
    sizeof_fragment = (1 + sizeof_fragment / sizeof(double))*sizeof(double);
  }

  if( false == m_fsp.Create(sizeof_fragment + sizeof_point_and_normals,subd_fragment_count,0))
    return ON_SUBD_RETURN_ERROR(false);

  m_display_density = display_density;
  m_facet_type = facet_type;
  m_fragment_point_count = fragment_point_count;

  return true;
}

ON_SubDLimitMeshFragment* ON_SubDLimitMeshImpl::CopyCallbackFragment(
    const ON_SubDLimitMeshFragment* callback_fragment
    )
{
  if ( nullptr == callback_fragment)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( 0 == callback_fragment->m_P_count )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( callback_fragment->m_P_count > m_fragment_point_count )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( nullptr == callback_fragment->m_P || nullptr == callback_fragment->m_N )
    return ON_SUBD_RETURN_ERROR(nullptr);

  double* P = (double*)m_fsp.AllocateDirtyElement();
  if ( nullptr == P)
    return ON_SUBD_RETURN_ERROR(nullptr);

  double* N = P + 3*m_fragment_point_count;
  ON_SubDLimitMeshFragment* fragment = (ON_SubDLimitMeshFragment*)(N + 3*m_fragment_point_count);

  *fragment = *callback_fragment;

  const size_t fragment_stride = 3;
  fragment->m_P_capacity = (unsigned short)m_fragment_point_count;
  fragment->m_P_stride = fragment_stride;
  fragment->m_P = P;
  fragment->m_N_stride = fragment_stride;
  fragment->m_N = N;


  size_t srcP_stride = callback_fragment->m_P_stride;
  const double* srcP = callback_fragment->m_P;
  const double* srcP1 = srcP + srcP_stride*callback_fragment->m_P_count;
  while ( srcP < srcP1 )
  {
    P[0] = srcP[0];
    P[1] = srcP[1];
    P[2] = srcP[2];
    P += fragment_stride;
    srcP += srcP_stride;
  }

  srcP_stride = callback_fragment->m_N_stride;
  srcP = callback_fragment->m_N;
  srcP1 = srcP + srcP_stride*callback_fragment->m_P_count;
  while ( srcP < srcP1 )
  {
    N[0] = srcP[0];
    N[1] = srcP[1];
    N[2] = srcP[2];
    N += fragment_stride;
    srcP += srcP_stride;
  }

  m_limit_mesh_content_serial_number = ON_SubDLimitMeshImpl::Internal_NextContentSerialNumber();

  return fragment;
}

bool ON_SubDLimitMeshImpl::AddFinishedFragment(
  ON_SubDLimitMeshFragment* finished_fragment
  )
{
  if ( nullptr == finished_fragment)
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 == finished_fragment->m_P_count )
    return ON_SUBD_RETURN_ERROR(false);
  if ( finished_fragment->m_P_count > m_fragment_point_count )
    return ON_SUBD_RETURN_ERROR(false);
  if ( nullptr == finished_fragment->m_P || nullptr == finished_fragment->m_N )
    return ON_SUBD_RETURN_ERROR(false);

  m_fragment_count++;

  if (nullptr == m_first_fragment)
  {
    m_first_fragment = finished_fragment;
    m_last_fragment = finished_fragment;
    m_bbox = finished_fragment->m_bbox;
  }
  else
  {
    m_last_fragment->m_next_fragment = finished_fragment;
    finished_fragment->m_prev_fragment = m_last_fragment;
    m_last_fragment = finished_fragment;
    m_bbox.Union(finished_fragment->m_bbox);
  }

  m_limit_mesh_content_serial_number = ON_SubDLimitMeshImpl::Internal_NextContentSerialNumber();

  return true;
}


////class ON_SubDLimitMeshSealVertexInfo
////{
////public:
////  ON_SubDLimitMeshSealVertexInfo() = default;
////  ~ON_SubDLimitMeshSealVertexInfo() = default;
////  ON_SubDLimitMeshSealVertexInfo(const ON_SubDLimitMeshSealVertexInfo&) = default;
////  ON_SubDLimitMeshSealVertexInfo& operator=(const ON_SubDLimitMeshSealVertexInfo&) = default;
////
////  static const ON_SubDLimitMeshSealVertexInfo Unset;
////
////  enum Bits : unsigned char
////  {
////    // Set if the vertex is smooth and normals should be sealed along with location.
////    SmoothVertex = 0x01,
////
////    // Set if the edge is smooth and normals should be sealed along with location.
////    SmoothEdge = 0x02,
////  };
////
////public:
////  unsigned int m_vertex_id = 0;
////  unsigned int m_edge_id = 0;
////  unsigned char m_bits = 0;
////  double* m_P = nullptr;
////  double* m_N = nullptr;
////};
////const ON_SubDLimitMeshSealVertexInfo ON_SubDLimitMeshSealVertexInfo::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDLimitMeshSealVertexInfo);

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

      const ON_SubDEdgePtr eptr = m_fragment->EdgePtr(grid_side_dex);
      const ON_SubDEdge* edge = eptr.Edge();
      m_edge_id = (nullptr != edge && edge->m_face_count >= 2) ? edge->m_id : 0;
      if (0 == m_edge_id)
        break; // nothing to seal

      const bool bCompleteFragment = m_fragment->IsCompleteFragment();
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

  ON_SubDLimitMeshFragment* m_fragment = nullptr;
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
      // set in the fragments, or the sorting in ON_SubDLimitMeshImpl::SealEdges()
      // is not valid (or some other bug).
      ON_SUBD_ERROR("unexpected sealing fragment portions");
      return false;
    }
  }

  // seal this edge
  const bool bSameDirection = (src_dir == (dst.m_bits&ON_SubDLimitMeshSealEdgeInfo::Bits::EdgeDir));
  const unsigned int j0 = dst.m_grid_side_dex*dst_side_segment_count + (bSameDirection?0:dst_side_segment_count);
  const unsigned int j1 = bSameDirection ? (j0+dst_side_segment_count) : (j0-dst_side_segment_count);
  ON_SubDLimitMeshFragment::SealAdjacentSides(
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


void ON_SubDLimitMeshImpl::SealEdges()
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


unsigned int ON_SubDLimitMeshImpl::Internal_NextContentSerialNumber()
{
  static unsigned int serial_number = 0;
  serial_number++;
  return serial_number;
}

ON_SubDLimitMeshImpl::ON_SubDLimitMeshImpl()
  : m_limit_mesh_content_serial_number(ON_SubDLimitMeshImpl::Internal_NextContentSerialNumber())
{}

ON_SubDLimitMeshImpl::ON_SubDLimitMeshImpl(
  const class ON_SubDLimitMeshImpl& src
  )
  : m_limit_mesh_content_serial_number(ON_SubDLimitMeshImpl::Internal_NextContentSerialNumber())
{
  if ( nullptr != src.m_first_fragment && ReserveCapacity((unsigned int)src.m_fsp.ActiveElementCount(), src.m_facet_type, src.m_display_density ) )
  {
    for (const ON_SubDLimitMeshFragment* src_fragment = src.m_first_fragment; nullptr != src_fragment; src_fragment = src_fragment->m_next_fragment)
    {
      ON_SubDLimitMeshFragment* fragment = CopyCallbackFragment(src_fragment);
      AddFinishedFragment(fragment);
    }
  }
}

void ON_SubDLimitMeshImpl::ClearTree()
{
  if (nullptr != m_fragment_tree)
  {
    delete m_fragment_tree;
    m_fragment_tree = nullptr;
  }
}

const ON_RTree& ON_SubDLimitMeshImpl::FragmentTree() const
{
  if (nullptr != m_fragment_tree && nullptr != m_first_fragment)
  {
    double Pbox[2][3];
    ON_RTree* fragment_tree = new ON_RTree();
    for (const ON_SubDLimitMeshFragment* fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
    {
      const double* P = fragment->m_P;
      const size_t P_stride = fragment->m_P_stride;
      if (nullptr == P || P_stride < 3 || fragment->m_P_count <= 0)
        continue;
      const double* P1 = P + P_stride*fragment->m_P_count;
      Pbox[0][0] = Pbox[1][0] = P[0];
      Pbox[0][1] = Pbox[1][1] = P[1];
      Pbox[0][2] = Pbox[1][2] = P[2];
      for (P += P_stride; P < P1; P += P_stride)
      {
        if (P[0] < Pbox[0][0])
          Pbox[0][0] = P[0];
        else if (P[0] > Pbox[1][0])
          Pbox[1][0] = P[0];
        if (P[1] < Pbox[0][1])
          Pbox[0][1] = P[1];
        else if (P[1] > Pbox[1][1])
          Pbox[1][1] = P[1];
        if (P[2] < Pbox[0][2])
          Pbox[0][2] = P[2];
        else if (P[2] > Pbox[1][2])
          Pbox[1][2] = P[2];
        fragment_tree->Insert(Pbox[0], Pbox[1], (void*)fragment);
      }
    }
    const_cast< ON_SubDLimitMeshImpl* >(this)->m_fragment_tree = fragment_tree;
  }
  return (nullptr == m_fragment_tree ) ? ON_RTree::Empty : *m_fragment_tree;
}

bool ON_SubDLimitMeshImpl::GetTightBoundingBox(
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
    for (const ON_SubDLimitMeshFragment* fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
    {
      ON_GetPointListBoundingBox(3, 0, fragment->m_P_count, (int)fragment->m_P_stride, fragment->m_P, local_bbox, fragment != m_first_fragment, xform);
    }
  }

  if (bGrowBox && bbox.IsValid())
    bbox.Union(local_bbox);
  else
    bbox = local_bbox;

  return true;
}

#if defined(ON_HAS_RVALUEREF)
ON_SubDLimitMesh::ON_SubDLimitMesh( ON_SubDLimitMesh&& src) ON_NOEXCEPT
  : m_impl_sp(std::move(src.m_impl_sp))
{}

ON_SubDLimitMesh& ON_SubDLimitMesh::operator=(ON_SubDLimitMesh&& src)
{
  if (this != &src)
  {
    m_impl_sp.reset();
    m_impl_sp = std::move(src.m_impl_sp);
  }
  return *this;
}
#endif

ON_SubDDisplayParameters ON_SubDLimitMesh::DisplayParameters() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
  {
    ON_SubDDisplayParameters dp;
    dp.m_display_density = impl->m_display_density;
    return dp;
  }
  return ON_SubDDisplayParameters::Empty;
}

unsigned int ON_SubDLimitMesh::DisplayDensity() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_display_density
    : 0;
}

unsigned int ON_SubDLimitMesh::FragmentCount() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_fragment_count
    : 0;
}

const ON_SubDLimitMeshFragment* ON_SubDLimitMesh::FirstFragment() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_first_fragment
    : nullptr;
}

const ON_SubDLimitMeshFragment* ON_SubDLimitMesh::FaceFragment(
  const class ON_SubDFace* face
) const
{
  if (nullptr == face)
    return nullptr;
  for (const ON_SubDLimitMeshFragment* fragment = FirstFragment(); nullptr != fragment; fragment = fragment->m_next_fragment)
  {
    if (face == fragment->m_face)
      return fragment;
  }
  return nullptr;
}

bool ON_SubDLimitMesh::GetFaceCenterPointAndNormal(
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
  const ON_SubDLimitMeshFragment* fragment = FaceFragment(face);
  if (nullptr == fragment)
    return false;
  if (nullptr == fragment->m_P || nullptr == fragment->m_N)
    return false;
  const unsigned int n = fragment->m_grid.m_side_segment_count;
  const unsigned int P_dex = fragment->IsCompleteFragment() ? (n*(n + 2) / 2) : 0;
  if (P_dex >= (unsigned int)fragment->m_P_count)
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


bool ON_SubDLimitMesh::GetEdgeCenterPointAndNormal(
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
  const ON_SubDLimitMeshFragment* fragment = nullptr;
  for (
    fragment =  FaceFragment(face);
    nullptr != fragment && fragment->m_face == face;
    fragment = bIsPartialFragment ? fragment->m_next_fragment : nullptr
    )
  {
    bIsPartialFragment = fragment->IsPartialFragment();
    for (unsigned int grid_side_index = 0; grid_side_index < 4; grid_side_index++)
    {
      const ON_SubDEdge* grid_side_edge = fragment->Edge(grid_side_index);
      if (edge != grid_side_edge)
        continue;
      const unsigned int n = fragment->m_grid.m_side_segment_count;
      if (n <= 0 || nullptr == fragment->m_grid.m_S)
        break;
      if (bIsPartialFragment)
      {
        const ON_SubDVertex* v = fragment->Vertex(grid_side_index);
        if (nullptr != v)
        {
          const unsigned int evi = (0 == face->EdgeDirection(fei)) ? 0U : 1U;
          if (v == edge->Vertex(evi))
            grid_side_index++;
          P_dex = fragment->m_grid.m_S[grid_side_index*n];
        }
      }
      else if (fragment->IsCompleteFragment())
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
  if (P_dex >= (unsigned int)fragment->m_P_count)
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

bool ON_SubDLimitMesh::Update(
  bool bShareUpdate
  )
{
  return false;
}

bool ON_SubDLimitMesh::IsEmpty() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  return (nullptr == impl || 0 == impl->m_fragment_count );
}

const ON_RTree& ON_SubDLimitMesh::FragmentTree() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->FragmentTree();
  return ON_RTree::Empty;
}

ON_BoundingBox ON_SubDLimitMesh::BoundingBox() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->m_bbox;
  return ON_BoundingBox::EmptyBoundingBox;
}

ON_SubD ON_SubDLimitMesh::SubD() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if ( nullptr == impl )
    return ON_SubD::Empty;
  ON_SubD subd;
  subd.ShareDimple(*impl);
  return subd;
}

ON_SubDRef ON_SubDLimitMesh::SubDRef() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if ( nullptr == impl )
    return ON_SubDRef::Empty;
  ON_SubDRef subd_ref;
  ON_SubD& subd = subd_ref.NewSubD();
  subd.ShareDimple(*impl);
  return subd_ref;
}

bool ON_SubDLimitMesh::GetTightBoundingBox(
  ON_BoundingBox& bbox,
  bool bGrowBox,
  const ON_Xform* xform
  ) const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->GetTightBoundingBox(bbox,bGrowBox,xform);
  return (bGrowBox && bbox.IsValid());
}

ON_SubD::FacetType ON_SubDLimitMesh::GridType() const
{
  const ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->m_facet_type;
  return ON_SubD::FacetType::Unset;
}

void ON_SubDLimitMesh::Clear()
{
  m_impl_sp.reset();
}

void ON_SubDLimitMesh::ClearTree()
{
  ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    impl->ClearTree();
}

unsigned int ON_SubDLimitMesh::ContentSerialNumber() const
{
  ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl) ? impl->m_limit_mesh_content_serial_number : 0;
}

ON_SubDLimitMesh& ON_SubDLimitMesh::CopyFrom(
  const ON_SubDLimitMesh& src
  )
{
  if (this != &src)
  {
    m_impl_sp.reset();
    const ON_SubDLimitMeshImpl* src_impl = src.m_impl_sp.get();
    if (nullptr != src_impl)
    {
      ON_SubDLimitMeshImpl* impl = new ON_SubDLimitMeshImpl(*src_impl);
      std::shared_ptr< ON_SubDLimitMeshImpl > new_impl_sp(impl);
      m_impl_sp.swap(new_impl_sp);
    }
  }
  return *this;
}

void ON_SubDLimitMesh::Swap(
  ON_SubDLimitMesh& a,
  ON_SubDLimitMesh& b
  )
{
  if (&a == &ON_SubDLimitMesh::Empty || &b == &ON_SubDLimitMesh::Empty)
  {
    ON_SubDIncrementErrorCount();
  }
  else
  {
    std::swap(a.m_impl_sp, b.m_impl_sp);
  }
}

ON_SubDDisplayParameters ON_SubDDisplayParameters::CreateFromDisplayDensity(
  unsigned int display_density
  )
{
  if ( display_density > ON_SubDLimitMesh::MaximumDisplayDensity )
    return ON_SUBD_RETURN_ERROR(ON_SubDDisplayParameters::Empty);

  ON_SubDDisplayParameters limit_mesh_parameters;
  limit_mesh_parameters.m_display_density = display_density;
  return limit_mesh_parameters;
}

#endif
