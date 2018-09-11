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
  const int grid_side_count = 4; // will be 3 for tri grid
  if (nullptr != m_face && grid_side_index < grid_side_count)
  {
    unsigned short fei = m_face_vertex_index[grid_side_index];
    if (fei < m_face->m_edge_count)
      return m_face->EdgePtr(fei);
    grid_side_index = (grid_side_index+grid_side_count-1) % grid_side_count;
    fei = m_face_vertex_index[grid_side_index];
    if (fei < m_face->m_edge_count)
      return m_face->EdgePtr(fei);
  }
  return ON_SubDEdgePtr::Null;
}

const class ON_SubDVertex* ON_SubDLimitMeshFragment::Vertex(
  unsigned int grid_corner_index
  ) const
{
  ON_SubDEdgePtr eptr = EdgePtr(grid_corner_index);
  const ON_SubDEdge* edge = eptr.Edge();
  if ( nullptr != edge )
    return edge->m_vertex[eptr.EdgeDirection()];
  return nullptr;
}

ON_3dPoint ON_SubDLimitMeshFragment::CornerPoint(
  unsigned int grid_corner_index
  ) const
{
  if ( grid_corner_index >= 4 || nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return ON_3dPoint::UnsetPoint;
  
  //unsigned int i = grid_corner_index*m_grid.m_side_segment_count + 1;
  unsigned int i = grid_corner_index*m_grid.m_side_segment_count;

  return ON_3dPoint(m_P + (i*m_P_stride));
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


bool ON_SubDLimitMeshFragment::IsSubFragment() const
{
  return ( nullptr != m_face && m_face_vertex_index[0] < ON_SubDFace::MaximumEdgeCount );
}


bool ON_SubDLimitMeshFragment::IsCompleteFragment() const
{
  return ( 
    nullptr != m_face 
    && m_face_vertex_index[0] < ON_SubDFace::MaximumEdgeCount 
    && m_face_vertex_index[1] < ON_SubDFace::MaximumEdgeCount 
    && m_face_vertex_index[2] < ON_SubDFace::MaximumEdgeCount 
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

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  // The code below is thread safe and constructs the ON_SubDLimitMeshFragmentGrids
  // that are resources shared by all ON_SubDLimitMeshFragments.

  static ON_SleepLock lock;
  const bool bReturnLock = lock.GetLock(50,ON_SleepLock::OneMinute,true);
#endif


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

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  if ( bReturnLock )
    lock.ReturnLock();
#endif

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

void ON_SubDLimitMeshImpl::ClearFragmentFacePointers()
{
  // When the ON_SubDimple that manages the faces referenced by
  // fragment->m_face is deleted, fragment->m_face must be set to zero.
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

class /*DO NOT EXPORT*/ON_SubDLimitMeshImpl_CallbackContext
{
public:
#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  bool m_bUseMultipleThreads = false; // If true, callback uses the lock
  ON_SleepLock m_lock;
#endif
  ON_SubDLimitMeshImpl* m_impl = nullptr;
  ON_SimpleArray< ON_SubDLimitMeshFragment* > m_face_mesh_fragments;

  static bool FragmentCallbackFunction(
    ON__UINT_PTR impl_context_as_void,
    const class ON_SubDLimitMeshFragment* fragment
    );

  /*
  Description:
    compares the face id and then the fragment index.
  Parameters:
    a - [in]
    b - [in]
      The caller insures that a, b, a->m_face and b->m_face are not nullptr.
  */
  static int CompareFragmentIndex(
    ON_SubDLimitMeshFragment*const* a,
    ON_SubDLimitMeshFragment*const* b
    );
  
  /*
  Description:
    Once all the mesh fragments for a face are delivered and sorted,
    this function is called to make coincident vertices have identical
    locations and normals.

  Parameters:
     count - [in]
       Number of elements in mesh_fragments[].  This should be the same
       number as sub_fragments[i].m_face_fragment_count.
     mesh_fragments - [in]
       Every element in the array should have the same ON_SubDLimitMeshFragment.m_face->m_id
  */
  static bool ProcessSortedFragments(
    unsigned int count,
    ON_SubDLimitMeshFragment** mesh_fragments
    );

  static bool CoincidentSideCopy(
    ON_SubDLimitMeshFragment* prev_fragment,
    ON_SubDLimitMeshFragment* fragment
    );
};

bool ON_SubDLimitMeshImpl_CallbackContext::FragmentCallbackFunction(
  ON__UINT_PTR callback_context_as_void,
  const class ON_SubDLimitMeshFragment* callback_fragment
  )
{
  bool bContinueMeshCalculation = true;

  ON_SubDLimitMeshImpl_CallbackContext* context = (ON_SubDLimitMeshImpl_CallbackContext*)callback_context_as_void;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  bool bReleaseLock = false;
  if (context->m_bUseMultipleThreads)
  {
    if (false == context->m_lock.GetLock(0, ON_SleepLock::OneSecond))
    {
      // return true to keep going, but something is really hogging the lock
      return ON_SUBD_RETURN_ERROR(bContinueMeshCalculation);
    }
  }
#endif

  for (;;)
  {
    ON_SubDLimitMeshFragment* impl_managed_fragment = context->m_impl->CopyCallbackFragment(callback_fragment);
    if (nullptr == impl_managed_fragment)
    {
      ON_SubDIncrementErrorCount();
      bContinueMeshCalculation = false; // terminate meshing
      break;
    }

    const unsigned int face_fragment_count = impl_managed_fragment->m_face_fragment_count;

    const unsigned int face_id 
      = (nullptr == impl_managed_fragment->m_face)
      ? 0
      : impl_managed_fragment->m_face->m_id;


    if ( 0 == face_id || ON_UNSET_UINT_INDEX == face_id
      || face_fragment_count <= 1
      || impl_managed_fragment->m_face_fragment_index >= face_fragment_count
      )
    {
      // simple case where no additional processing is reqired
      context->m_impl->AddFinishedFragment(impl_managed_fragment);
      break;
    }


    unsigned int count = context->m_face_mesh_fragments.UnsignedCount();

    if (count + 1 < face_fragment_count)
    {
      // waiting on more fragments
      context->m_face_mesh_fragments.Append(impl_managed_fragment);
      break;
    }

    ON_SubDLimitMeshFragment** mesh_fragments = context->m_face_mesh_fragments.Array();
    unsigned int delivered_mesh_fragment_count = 0;
    unsigned int i0 = ON_UNSET_UINT_INDEX;
    for (unsigned int i = 0; i < count; i++)
    {
      if ( face_id != mesh_fragments[i]->m_face->m_id)
        continue;
      if ( 0 == delivered_mesh_fragment_count++)
        i0 = i;
      if ( delivered_mesh_fragment_count + 1 == face_fragment_count )
        break;
    }

    if (delivered_mesh_fragment_count + 1 < face_fragment_count)
    {
      // waiting on more fragments
      context->m_face_mesh_fragments.Append(impl_managed_fragment);
      break;
    }

    // copy the fragments we need to process to local storage I can release the lock.
    ON_SimpleArray< ON_SubDLimitMeshFragment* > local_mesh_fragments(face_fragment_count);
    if (delivered_mesh_fragment_count == count)
    {
      local_mesh_fragments.Append(count,mesh_fragments);
      context->m_face_mesh_fragments.SetCount(0);
    }
    else
    {
      local_mesh_fragments.Append(mesh_fragments[i0]);
      unsigned int count1 = 0;
      for (unsigned int i = i0+1; i < count; i++)
      {
        if (face_id != mesh_fragments[i]->m_face->m_id)
        {
          mesh_fragments[count1++] = mesh_fragments[i];
          continue;
        }
        local_mesh_fragments.Append(mesh_fragments[i]);
      }
      context->m_face_mesh_fragments.SetCount(count1);
    }

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
    if (bReleaseLock)
    {
      // don't keep lock during processing
      context->m_lock.ReturnLock();
      bReleaseLock = false;
    }
#endif

    local_mesh_fragments.Append(impl_managed_fragment);
    local_mesh_fragments.QuickSort(ON_SubDLimitMeshImpl_CallbackContext::CompareFragmentIndex);
    count = local_mesh_fragments.UnsignedCount();
    mesh_fragments = local_mesh_fragments.Array();

    if ( count == face_fragment_count)
      ON_SubDLimitMeshImpl_CallbackContext::ProcessSortedFragments(count,mesh_fragments);
    else
    {
      // there is a bug in the code above
      ON_SubDIncrementErrorCount();
    }
    
#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
    // If needed, get a lock before adding the processed fragments to
    // the linked list.
    if (context->m_bUseMultipleThreads)
    {
      if (false == context->m_lock.GetLock(0, ON_SleepLock::OneSecond))
      {
        // return true to keep going, but something is really hogging the lock
        return ON_SUBD_RETURN_ERROR(bContinueMeshCalculation);
      }
    }
#endif

    for ( unsigned int i = 0; i < count; i++ )
      context->m_impl->AddFinishedFragment(local_mesh_fragments[i]);

    break;
  }

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  if (bReleaseLock)
    context->m_lock.ReturnLock();
#endif

  return bContinueMeshCalculation;
}


int ON_SubDLimitMeshImpl_CallbackContext::CompareFragmentIndex(
    ON_SubDLimitMeshFragment*const* a,
    ON_SubDLimitMeshFragment*const* b
    )
{
  if ( a == b )
    return 0;

  // caller insures no nulls
  //if ( nullptr == a )
  //  return 1;
  //if ( nullptr == b )
  //  return -1;

  //unsigned int a_id = ( nullptr == (*a)->m_face ) ? ON_UNSET_UINT_INDEX : (*a)->m_face->m_id;
  //unsigned int b_id = ( nullptr == (*b)->m_face ) ? ON_UNSET_UINT_INDEX : (*b)->m_face->m_id;
  //if ( a_id < b_id )
  //  return -1;
  //if ( a_id > b_id )
  //  return 1;

  unsigned int a_index = (*a)->m_face_fragment_index;
  unsigned int b_index = (*b)->m_face_fragment_index;
  if ( a_index < b_index )
    return -1;
  if ( a_index > b_index )
    return 1;

  // identical values should never appear in separate entries.
  return ON_SUBD_RETURN_ERROR(0);
}


bool ON_SubDLimitMeshImpl_CallbackContext::CoincidentSideCopy(
  ON_SubDLimitMeshFragment* prev_fragment,
  ON_SubDLimitMeshFragment* fragment
  )
{
  if ( prev_fragment->m_grid.m_side_segment_count != fragment->m_grid.m_side_segment_count)
    return ON_SUBD_RETURN_ERROR(false);

  //const unsigned int side_segment_count = fragment->m_grid.m_side_segment_count;
  //const unsigned int* S0 = prev_fragment->m_grid.m_S + 4*side_segment_count;
  //const unsigned int* S1 = fragment->m_grid.m_S;

  //const double* P0 = prev_fragment->m_P;
  //double* P1 = fragment->m_P;
  //const double* N0 = prev_fragment->m_N;
  //double* N1 = fragment->m_N;

  // const double* src;
  // double* dst;


  //for (const unsigned int* S1max = S1 + side_segment_count; S1 < S1max; S1++, S0--)
  //{
  //  src = P0 + P0_stride * *S0;
  //  dst = P1 + P1_stride * *S1;
  //  
  //  d = fabs(src[0] - dst[0]) + fabs(src[1] - dst[1]) + fabs(src[2] - dst[2]);
  //  if (!(d <= 1e-8))
  //    return ON_SUBD_RETURN_ERROR(false);

  //  *dst++ = *src++;
  //  *dst++ = *src++;
  //  *dst = *src;

  //  src = N0 + N0_stride * *S0;
  //  dst = N1 + N1_stride * *S1;

  //  d = fabs(src[0] - dst[0]) + fabs(src[1] - dst[1]) + fabs(src[2] - dst[2]);
  //  if (!(d <= 0.01))
  //    return ON_SUBD_RETURN_ERROR(false);

  //  *dst++ = *src++;
  //  *dst++ = *src++;
  //  *dst = *src;
  //}

  const size_t side_point_count = 1U + (unsigned int)(fragment->m_grid.m_side_segment_count);

  const double* P0 = prev_fragment->m_P;
  const double* N0 = prev_fragment->m_N;
  const size_t P0_stride = side_point_count*prev_fragment->m_P_stride;
  const size_t N0_stride = side_point_count*prev_fragment->m_N_stride;

  double* P1 = fragment->m_P;
  double* N1 = fragment->m_N;
  const size_t P1_stride = fragment->m_P_stride;
  const size_t N1_stride = fragment->m_N_stride;

  double* P1max = P1 + side_point_count*P1_stride;
  while ( P1 < P1max )
  {
    double d = fabs(P0[0] - P1[0]) + fabs(P0[1] - P1[1]) + fabs(P0[2] - P1[2]);
    if (!(d <= 1e-8))
      return ON_SUBD_RETURN_ERROR(false);

    P1[0] = P0[0];
    P1[1] = P0[1];
    P1[2] = P0[2];
    P0 += P0_stride;
    P1 += P1_stride;

    d = fabs(N0[0] - N1[0]) + fabs(N0[1] - N1[1]) + fabs(N0[2] - N1[2]);
    if (!(d <= 0.01))
      return ON_SUBD_RETURN_ERROR(false);

    N1[0] = N0[0];
    N1[1] = N0[1];
    N1[2] = N0[2];
    N0 += N0_stride;
    N1 += N1_stride;
  }

  return true;
}


bool ON_SubDLimitMeshImpl_CallbackContext::ProcessSortedFragments(
  unsigned int count,
  ON_SubDLimitMeshFragment** mesh_fragments
  )
{
  if ( count < 2 || nullptr == mesh_fragments)
    return ON_SUBD_RETURN_ERROR(false);
  if ( nullptr == mesh_fragments[0] 
    || nullptr == mesh_fragments[0]->m_face
    || nullptr == mesh_fragments[0]->m_P
    || nullptr == mesh_fragments[0]->m_N
    || 0 != mesh_fragments[0]->m_face_fragment_index
    || count != mesh_fragments[0]->m_face_fragment_count
    )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int face_id = mesh_fragments[0]->m_face->m_id;
  const unsigned int grid_F_count = mesh_fragments[0]->m_grid.m_F_count;
  const unsigned int grid_side_count = mesh_fragments[0]->m_grid.m_side_segment_count;
  if ( 0 == face_id || ON_UNSET_UINT_INDEX == face_id )
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 == grid_F_count || grid_side_count*grid_side_count != grid_F_count )
    return ON_SUBD_RETURN_ERROR(false);

  if ( nullptr == mesh_fragments[count-1] 
    || nullptr == mesh_fragments[count-1]->m_face
    || nullptr == mesh_fragments[count-1]->m_P
    || nullptr == mesh_fragments[count-1]->m_N
    || (count-1) != mesh_fragments[count-1]->m_face_fragment_index
    || count != mesh_fragments[count-1]->m_face_fragment_count
    || face_id != mesh_fragments[count-1]->m_face->m_id
    || grid_F_count != mesh_fragments[count-1]->m_grid.m_F_count
    || grid_side_count != mesh_fragments[count-1]->m_grid.m_side_segment_count
    )
    return ON_SUBD_RETURN_ERROR(false);


  ON_SubDLimitMeshFragment* fragment = mesh_fragments[count-1];

  for (unsigned int i = 0; i < count; i++)
  {
    ON_SubDLimitMeshFragment* prev_fragment = fragment;
    fragment = mesh_fragments[i];
    if (i > 0)
    {
      if (nullptr == fragment
        || nullptr == fragment->m_face
        || face_id != fragment->m_face->m_id
        || grid_F_count != fragment->m_grid.m_F_count
        || grid_side_count != fragment->m_grid.m_side_segment_count
        || i != fragment->m_face_fragment_index
        || count != fragment->m_face_fragment_count
        || nullptr == fragment->m_P
        || nullptr == fragment->m_N
        )
        return ON_SUBD_RETURN_ERROR(false);
    }

    if ( false == ON_SubDLimitMeshImpl_CallbackContext::CoincidentSideCopy(prev_fragment,fragment) )
      return ON_SUBD_RETURN_ERROR(false);
  }
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

ON_SubDLimitMesh* ON_SubDLimitMesh::Create(
  const ON_SubD& subd,
  const class ON_SubDDisplayParameters& limit_mesh_parameters,
  ON_SubDLimitMesh* destination_mesh
  )
{
  ON_SubDFaceIterator fit = subd.FaceIterator();
  return ON_SubDLimitMesh::Create(fit,limit_mesh_parameters,destination_mesh);
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

ON_SubDLimitMesh* ON_SubDLimitMesh::Create(
    ON_SubDFaceIterator fit,
    const class ON_SubDDisplayParameters& limit_mesh_parameters,
    ON_SubDLimitMesh* destination_mesh
    )
{
  ON_SubDLimitMeshImpl* impl = 0;
  std::shared_ptr< ON_SubDLimitMeshImpl > impl_sp;

  if (nullptr != destination_mesh)
  {
    destination_mesh->Clear();
    std::shared_ptr< ON_SubDLimitMeshImpl > dest_sp = destination_mesh->m_impl_sp;
    if (1 == dest_sp.use_count())
    {
      impl_sp.swap(dest_sp);
      impl = impl_sp.get();
    }
  }

  if ( limit_mesh_parameters.m_display_density > ON_SubDLimitMesh::MaximumDisplayDensity )
    return ON_SUBD_RETURN_ERROR(nullptr);

  const ON_SubD& subd = fit.SubD();

  ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subd.ActiveLevelSubDType());
  
  const unsigned int subd_fragment_count = fit.LimitSurfaceMeshFragmentCount(facet_type);
  if ( 0 == subd_fragment_count )
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (nullptr == impl)
  {
    impl = new(std::nothrow) ON_SubDLimitMeshImpl();
    if ( nullptr == impl)
      return ON_SUBD_RETURN_ERROR(nullptr);
    std::shared_ptr< ON_SubDLimitMeshImpl > new_impl_sp(impl);
    impl_sp.swap(new_impl_sp);
  }

  if (false == impl->ReserveCapacity(subd_fragment_count,facet_type,limit_mesh_parameters.m_display_density))
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDLimitMeshImpl_CallbackContext callback_context;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  callback_context.m_bUseMultipleThreads = limit_mesh_parameters.m_bUseMultipleThreads;
#endif

  callback_context.m_impl = impl_sp.get();
  subd.GetLimitSurfaceMeshInFragments(
    limit_mesh_parameters,
    (ON__UINT_PTR)&callback_context,
    ON_SubDLimitMeshImpl_CallbackContext::FragmentCallbackFunction
    );

  if (ON_Terminator::TerminationRequested(limit_mesh_parameters.m_terminator))
  {
    return nullptr; // not an error - outside intervention canceled meshing
  }

  if (impl->m_fragment_count < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  ON_SubDLimitMesh* limit_mesh 
    = (nullptr != destination_mesh)
    ? destination_mesh
    : new ON_SubDLimitMesh();


  // The next three lines are conceptually the same as
  //   impl_sp->m_subdimple_sp = fit.SubD().m_subdimple_sp;
  // The three line approach is required because ON_SubD::m_subdimple_sp is  private.
  {
    ON_SubD tmp_sub;
    tmp_sub.ShareDimple(fit.SubD()); // tmp_sub.m_subdimple_sp = fit.SubD().m_subdimple_sp (increments fit.SubD().m_subdimple_sp ref count)
    // NOTE: 
    //  There are at least std::shared_ptr references to the ON_SubDimple (fit and tmp_subd),
    //  This insures the std::weak_ptr on impl_sp will be valid.
    tmp_sub.SwapDimple(*impl_sp);  // swap impl_sp->m_subdimple_wp and tmp_sub.m_subdimple_sp
  }

  // Let limit_mesh manage the contents
  limit_mesh->m_impl_sp.swap(impl_sp);

  // return the new mesh
  return limit_mesh;
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
