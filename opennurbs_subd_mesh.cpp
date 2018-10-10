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

bool ON_SubDFaceRegionBreakpoint(
  unsigned int level0_face_id,
  const class ON_SubDComponentRegionIndex& region_index
)
{
#if defined(ON_DEBUG)
  if (
    11 != level0_face_id
    )
  {
    return false;
  }

  const unsigned short region_pattern[] = { 3, 3 };
  const unsigned short region_pattern_count = (unsigned short)(sizeof(region_pattern) / sizeof(region_pattern[0]));

  if (region_index.m_subdivision_count < region_pattern_count)
    return false;

  for (unsigned short i = 0; i < region_pattern_count; i++)
  {
    if (region_index.m_index[i] != region_pattern[i])
      return false;
  }

  return true;// <- breakpoint here (or above)
#else
  return false;
#endif
}

bool ON_SubDComponentRegionBreakpoint(const ON_SubDComponentRegion* component_region)
{
#if defined(ON_DEBUG)
  if (nullptr != component_region)
  {
    switch (component_region->m_level0_component.ComponentType())
    {
    case ON_SubDComponentPtr::Type::Face:
      return ON_SubDFaceRegionBreakpoint(component_region->m_level0_component_id, component_region->m_region_index);
      break;
    case ON_SubDComponentPtr::Type::Edge:
      break;
    case ON_SubDComponentPtr::Type::Vertex:
      break;
    default:
      break;
    }
  }
#endif
  return false;
}


bool ON_SubDLimitNurbsFragment::IsEmpty() const
{
  return 0 == SetBispanCount();
}

unsigned int ON_SubDLimitNurbsFragment::MaximumBispanCount() const
{
  if (ON_SubDLimitNurbsFragment::Type::BicubicSingle == m_type)
    return 1;
  if (ON_SubDLimitNurbsFragment::Type::BicubicQuadrant == m_type)
    return 4;
  return 0;
}

unsigned int ON_SubDLimitNurbsFragment::SetBispanCount() const
{
  unsigned int set_bispan_count = 0;
  const unsigned int imax = MaximumBispanCount();
  for (unsigned  int i = 0; i < imax; i++)
  {
    if (
      ON_SubDLimitNurbsFragment::BispanType::Exact == m_bispan_type[i]
      || ON_SubDLimitNurbsFragment::BispanType::Approximate == m_bispan_type[i]
      )
    {
      set_bispan_count++;
    }
  }

  return set_bispan_count;
}


unsigned int ON_SubDLimitNurbsFragment::UnsetBispanCount() const
{
  return MaximumBispanCount() - SetBispanCount();
}

static bool Internal_CheckNurbsSurfaceCVs(
  const ON_NurbsSurface& s
  )
{
  for (int i = 0; i < s.m_cv_count[0]; i++)
  {
    for (int j = 0; j < s.m_cv_count[1]; j++)
    {
      double * cv = s.CV(i, j);
      for (unsigned k = 0; k < 3; k++)
      {
        if (!ON_IsValid(cv[k]))
        {
          return ON_SUBD_RETURN_ERROR(false);
        }
      }
    }
  }
  return true;
}

bool ON_SubDLimitNurbsFragment::IsApproximate() const
{
  const unsigned int imax = MaximumBispanCount();
  for (unsigned  int i = 0; i < imax; i++)
  {
    if (ON_SubDLimitNurbsFragment::BispanType::Approximate == m_bispan_type[i])
      return true;
  }
  return false;
}

ON_NurbsSurface* ON_SubDLimitNurbsFragment::GetSurface(
  ON_NurbsSurface* destination_surface
) const
{
  const unsigned int bispan_count = SetBispanCount();
  if (bispan_count != MaximumBispanCount())
    return nullptr;

  const double knots[7] = { -2,-1,0,1,2,3,4 };
  ON_NurbsSurface patch_srf;
  patch_srf.m_dim = 3;
  patch_srf.m_is_rat = 0;
  patch_srf.m_order[0] = 4;
  patch_srf.m_order[1] = 4;
  patch_srf.m_knot[0] = (double*)knots;
  patch_srf.m_knot[1] = (double*)knots;
  patch_srf.m_cv_stride[0] = 5 * 3;
  patch_srf.m_cv_stride[1] = 3;

  patch_srf.m_cv_count[0] = (1 == bispan_count) ? 4 : 5;
  patch_srf.m_cv_count[1] = patch_srf.m_cv_count[0];
  patch_srf.m_cv = (double*)m_patch_cv[0][0];
  ON_NurbsSurface* surface = nullptr;
  if (destination_surface)
  {
    surface = destination_surface;
    *surface = patch_srf;
  }
  else
  {
    surface = new ON_NurbsSurface(patch_srf);
  }

  Internal_CheckNurbsSurfaceCVs(*surface);

  return surface;
}

ON_NurbsSurface* ON_SubDLimitNurbsFragment::GetQuadrantSurface(
  unsigned int quadrant_index,
  ON_NurbsSurface* destination_surface
) const
{
  if (quadrant_index >= 4)
    return nullptr;

  if (
    ON_SubDLimitNurbsFragment::BispanType::Exact != m_bispan_type[quadrant_index]
    && ON_SubDLimitNurbsFragment::BispanType::Approximate != m_bispan_type[quadrant_index]
    )
    return nullptr;

  //const ON_2dex cvdex[4] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
  const ON_2dex cvdex(
    (1 == quadrant_index || 2 == quadrant_index) ? 1 : 0, 
    (2 == quadrant_index || 3 == quadrant_index) ? 1 : 0
  );

  const double knots[7] = {-2,-1,0,1,2,3,4};
  ON_NurbsSurface patch_srf;
  patch_srf.m_dim = 3;
  patch_srf.m_is_rat = 0;
  patch_srf.m_order[0] = 4;
  patch_srf.m_order[1] = 4;
  patch_srf.m_knot[0] = (double*)(knots+cvdex.i);
  patch_srf.m_knot[1] = (double*)(knots+cvdex.j);
  patch_srf.m_cv_stride[0] = 5*3;
  patch_srf.m_cv_stride[1] = 3;

  patch_srf.m_cv_count[0] = 4;
  patch_srf.m_cv_count[1] = 4;
  patch_srf.m_cv = (double*)m_patch_cv[cvdex.i][cvdex.j];

  ON_NurbsSurface* surface = nullptr;
  if (destination_surface)
  {
    surface = destination_surface;
    *surface = patch_srf;
  }
  else
  {
    surface = new ON_NurbsSurface(patch_srf);
  }

  Internal_CheckNurbsSurfaceCVs(*surface);

  return surface;
}

const ON_SubDComponentRegion Internal_CreateSubdivisionEdgeRegion(unsigned short subdivision_count, bool bReversedEdge )
{
  return ON_SubDComponentRegion::CreateSubdivisionRegion(ON_SubDComponentPtr::Type::Edge, bReversedEdge, subdivision_count, true);
}

class Internal_SubQuadTransientComponents
{
public:
  Internal_SubQuadTransientComponents(ON_SubDFaceIterator& fit)
  {
    // The edge_transient_vertex_id[] array stores ON_2udex
    // that are used to record the transient vertex id
    // for a level 0 edge subdivision point.
    // ON_2udex.i = level 0 edge id
    // ON_2udex.j = transient vertex id to use for the edge subdivision vertex
    //
    // These are required when an edge is shared by 2 N-gons with N != 4 because
    // each N-gon must use the same transient vertex id in order for the 
    // final NURBS surface to be easily and quickly joined into an ON_Brep.

    m_edge_transient_vertex_id_map.Reserve(64);
    ON_2udex u;
    u.i = 0;
    u.j = 0;
    for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
    {
      const unsigned int edge_count = face->EdgeCount();
      if (edge_count <= 0 || 4 == edge_count)
        continue;
      for (unsigned int fei = 0; fei < edge_count; fei++)
      {
        const ON_SubDEdge* e = face->Edge(fei);
        if (nullptr == e)
          continue;
        u.i = e->m_id;
        m_edge_transient_vertex_id_map.Append(u);
      }
    }

    const unsigned int count0 = m_edge_transient_vertex_id_map.UnsignedCount();
    if (count0 <= 0)
      return;

    m_edge_transient_vertex_id_map.QuickSort(ON_2udex::DictionaryCompare);
    unsigned int count1 = 0;
    unsigned int prev_edge_id = 0;
    ON_2udex* a = m_edge_transient_vertex_id_map.Array();
    for (unsigned int i = 0; i < count0; i++)
    {
      u = a[i];
      if (u.i <= prev_edge_id)
        continue; // paired edge
      prev_edge_id = u.i;
      u.j = ON_SubDComponentRegion::NewTransientId();
      a[count1++] = u;
    }
    m_edge_transient_vertex_id_map.SetCount(count1);
  }

  ~Internal_SubQuadTransientComponents() = default;

public:
  static const Internal_SubQuadTransientComponents Empty;

public:
  void Initialize(
    const ON_SubDFace* face
  )
  {
    m_face = nullptr;
    m_edge_count = 0;

    if (m_edge_transient_vertex_id_map.UnsignedCount() <= 0)
      return;
    if (nullptr == face)
      return;
    const unsigned short edge_count = face->m_edge_count;
    if (edge_count < 3 || 4 == edge_count)
      return;

    // Used to create edge regions for subdivision edges because their is no "real" 
    // edge to reference.
    // We need a unique id and region so merging and brep joining work correctly.
    m_edge_count = edge_count;
    m_fei = edge_count-1;
    m_face = face;
    m_radial_edge_region[2] = ON_SubDComponentRegion::CreateSubdivisionRegion(ON_SubDComponentPtr::Type::Edge, false, 1, true);
    m_radial_edge_region[1] = m_radial_edge_region[2];
    m_face_center_vertex_id = ON_SubDComponentRegion::NewTransientId();
    m_radial_vertex_id[2] = Internal_EdgeTransientVertexId();
    m_radial_vertex_id[1] = m_radial_vertex_id[2];

  }

  void NextSubQuad()
  {
    if (0 == m_edge_count || nullptr == m_face || 0 == m_edge_transient_vertex_id_map.UnsignedCount() )
      return;
    m_fei = (m_fei + 1) % m_edge_count;
    const bool bCreateSubdivisionEdgeRegion = (m_fei + 1 < m_edge_count);
    m_radial_edge_region[0] = m_radial_edge_region[1];
    m_radial_edge_region[1] 
      = bCreateSubdivisionEdgeRegion 
      ? ON_SubDComponentRegion::CreateSubdivisionRegion(ON_SubDComponentPtr::Type::Edge, false, 1, true)
      : m_radial_edge_region[2];
    m_radial_vertex_id[0] = m_radial_vertex_id[1];
    m_radial_vertex_id[1] 
      = bCreateSubdivisionEdgeRegion 
      ? Internal_EdgeTransientVertexId()
      : m_radial_vertex_id[2];
  }

public:
  const ON_SubDFace* m_face = nullptr;
  ON_SubDComponentRegion m_radial_edge_region[3] = {};
  unsigned int m_face_center_vertex_id = 0;
  unsigned int m_radial_vertex_id[3] = {};
  unsigned short m_edge_count = 0;
  unsigned short m_fei = 0;

  ON_SimpleArray< ON_2udex > m_edge_transient_vertex_id_map;

private:
  Internal_SubQuadTransientComponents() = delete;
  Internal_SubQuadTransientComponents(const Internal_SubQuadTransientComponents&) = delete;
  Internal_SubQuadTransientComponents& operator= (const Internal_SubQuadTransientComponents&) = delete;

private:
 unsigned int Internal_EdgeTransientVertexId() const
  {
    const ON_SubDEdge* e = (m_edge_count>0) ? m_face->Edge((m_fei+1)%m_edge_count) : nullptr;
    if (nullptr == e)
      return 0;
    const ON_2udex key(e->m_id, 0);
    int i = m_edge_transient_vertex_id_map.BinarySearch(&key, ON_2udex::CompareFirstIndex);
    return (i >= 0) ?m_edge_transient_vertex_id_map[i].j : 0;
  }
};


static void Internal_SetLevel0FaceAndEdgeRegion(
  const ON_SubDFace* face,
  unsigned int qi,
  const Internal_SubQuadTransientComponents& face_transient_components,
  ON_SubDFaceRegion& face_region
)
{
  face_region = ON_SubDFaceRegion::Empty;
  const unsigned int N = face->EdgeCount();
  face_region.m_face_region.SetLevel0Face(face);
  if ( 4 == N )
  {
    for (unsigned int fei = 0; fei < 4; fei++)
      face_region.m_edge_region[fei].SetLevel0EdgePtr(face->EdgePtr(fei));
    for (unsigned int fvi = 0; fvi < 4; fvi++)
    {
      const ON_SubDVertex* v = face->Vertex(fvi);
      if (nullptr != v)
        face_region.m_vertex_id[fvi] = v->m_id;
    }
  }
  else if (N >= 3 && qi < N)
  {
    face_region.Push(qi); // original N-gon (N != 4) was subdivided into N quads.

    const ON_SubDVertex* v = face->Vertex((qi+1)%N);
    if (nullptr != v)
      face_region.m_vertex_id[2] = v->m_id;
    face_region.m_vertex_id[0] = face_transient_components.m_face_center_vertex_id;
    face_region.m_vertex_id[1] = face_transient_components.m_radial_vertex_id[0];
    face_region.m_vertex_id[3] = face_transient_components.m_radial_vertex_id[1];

    face_region.m_edge_region[0] = face_transient_components.m_radial_edge_region[0];
    face_region.m_edge_region[1].SetLevel0EdgePtr(face->EdgePtr(qi));
    face_region.m_edge_region[1].PushAdjusted(1);
    face_region.m_edge_region[2].SetLevel0EdgePtr(face->EdgePtr((qi+1)%N));
    face_region.m_edge_region[2].PushAdjusted(0);
    face_region.m_edge_region[3] = face_transient_components.m_radial_edge_region[1];
    face_region.m_edge_region[3].m_level0_component = face_region.m_edge_region[3].m_level0_component.SetMark();
  }
  else
  {
    ON_SUBD_ERROR("Unexpected parameters.");
  }
}


const ON_SubDComponentRegion ON_SubDComponentRegion::Create(
    const class ON_SubDFace* level0_face
)
{
  ON_SubDComponentRegion r;
  r.m_level0_component = ON_SubDComponentPtr::Create(level0_face);
  r.m_level0_component_id = (nullptr != level0_face ? level0_face->m_id : 0);
  return r;
}

const ON_SubDComponentRegion ON_SubDComponentRegion::Create(
  unsigned int component_id,
  ON_SubDComponentPtr::Type component_type,
  bool bComponentMark
)
{
  ON_SubDComponentRegion r;
  r.m_level0_component = ON_SubDComponentPtr::CreateNull(component_type, bComponentMark);
  r.m_level0_component_id = component_id;
  return r;
}

const ON_SubDComponentRegion ON_SubDComponentRegion::CreateSubdivisionRegion(
  ON_SubDComponentPtr::Type component_type,
  bool bComponentMark,
  unsigned short subdivision_count,
  bool bAssignTransientId
)
{
  ON_SubDComponentRegion r;
  r.m_region_index = ON_SubDComponentRegionIndex::Unset;
  r.m_region_index.m_subdivision_count = subdivision_count;
  r.m_level0_component = ON_SubDComponentPtr::CreateNull(component_type, bComponentMark);

  if (bAssignTransientId)
  {
    r.m_level0_component_id = ON_SubDComponentRegion::NewTransientId();
  }
  return r;
}

////ON_SubDComponentRegion ON_SubDComponentRegion::Reverse() const
////{
////  ON_SubDComponentRegion r(*this);
////   r.m_level0_component = r.m_level0_component.ToggleMark();
////  if (r.m_subdivision_count > 0)
////  {
////    const int c = (int)(sizeof(m_region_index) / sizeof(m_region_index[0]));
////    int i = (int)(r.m_subdivision_count - 1);
////    int j = 0;
////    while (j < c && 0xFFFF == r.m_region_index[j])
////      j++;
////    for ( /*empty init*/; j < i && j < c; ++j,--i)
////    {
////      if (i < c)
////      {
////        unsigned short x = r.m_region_index[i];
////        r.m_region_index[i] = r.m_region_index[j];
////        r.m_region_index[j] = x;
////      }
////      else
////      {
////        r.m_region_index[j] = 0;
////      }
////    }
////  }  
////  return r;
////}
////
////ON_SubDComponentRegion ON_SubDComponentRegion::ReverseIfMarked() const
////{
////  return
////    0 != m_level0_component.ComponentMark()
////    ? Reverse()
////    : *this;
////}

int ON_SubDComponentRegion::CompareTypeIdMark(
  const ON_SubDComponentRegion* lhs,
  const ON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == rhs)
    return 1;
  if (nullptr == lhs)
    return -1;

  int rc = ON_SubDComponentPtr::CompareType(&lhs->m_level0_component, &rhs->m_level0_component);
  if (0 != rc)
    return rc;

  if (lhs->m_level0_component_id < rhs->m_level0_component_id)
    return -1;
  if (lhs->m_level0_component_id > rhs->m_level0_component_id)
    return 1;

  rc = (0 != lhs->m_level0_component.ComponentMark() ? (int)1 : (int)0) - (0 != lhs->m_level0_component.ComponentMark() ? (int)1 : (int)0);
  if (0 != rc)
    return rc;

  return 0;
}

int ON_SubDComponentRegionIndex::Compare(
  const ON_SubDComponentRegionIndex* lhs,
  const ON_SubDComponentRegionIndex* rhs
  )
{
  if (lhs == rhs)
    return 0;
  if (nullptr == rhs)
    return 1;
  if (nullptr == lhs)
    return -1;
  if (lhs->m_subdivision_count < rhs->m_subdivision_count)
    return -1;
  if (lhs->m_subdivision_count > rhs->m_subdivision_count)
    return 1;
  return ON_SubDComponentRegionIndex::CompareMinimumSubregion(lhs, rhs);
}

int ON_SubDComponentRegionIndex::CompareMinimumSubregion(
  const ON_SubDComponentRegionIndex* lhs,
  const ON_SubDComponentRegionIndex* rhs
  )
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  unsigned short subdivision_count0 = (lhs->m_subdivision_count < rhs->m_subdivision_count) ? lhs->m_subdivision_count : rhs->m_subdivision_count;
  if (subdivision_count0 > ON_SubDComponentRegionIndex::IndexCapacity)
    subdivision_count0 = ON_SubDComponentRegionIndex::IndexCapacity;
  for (unsigned short i = 0; i < subdivision_count0; i++)
  {
    if (lhs->m_index[i] < rhs->m_index[i])
      return -1;
    if (lhs->m_index[i] > rhs->m_index[i])
      return 1;
  }
  return 0;
}

int ON_SubDComponentRegion::CompareTypeIdMarkMinimumSubregion(
  const ON_SubDComponentRegion* lhs,
  const ON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;

  const int rc = ON_SubDComponentRegion::CompareTypeIdMark(lhs, rhs);
  if (0 != rc)
    return rc;

  return ON_SubDComponentRegionIndex::CompareMinimumSubregion( &lhs->m_region_index, &rhs->m_region_index);
}

int ON_SubDComponentRegion::CompareTypeIdMarkSubregion(
  const ON_SubDComponentRegion* lhs,
  const ON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;

  int rc = ON_SubDComponentRegion::CompareTypeIdMark(lhs, rhs);
  if (0 == rc)
  {
    rc = ON_SubDComponentRegionIndex::CompareMinimumSubregion(&lhs->m_region_index, &rhs->m_region_index);
    if (0 == rc)
    {
      if (lhs->m_region_index.m_subdivision_count < rhs->m_region_index.m_subdivision_count)
        rc = -1;
      else if (lhs->m_region_index.m_subdivision_count > rhs->m_region_index.m_subdivision_count)
        rc = 1;
    }
  }

  return rc;
}


int ON_SubDComponentRegion::Compare(
  const ON_SubDComponentRegion* lhs,
  const ON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;

  const int rc = ON_SubDComponentRegion::CompareTypeIdMarkSubregion(lhs, rhs);
  if (0 != rc)
    return rc;

  if (lhs->m_level0_component.m_ptr < rhs->m_level0_component.m_ptr)
    return -1;
  if (lhs->m_level0_component.m_ptr > rhs->m_level0_component.m_ptr)
    return 1;

  return 0;
}


void ON_SubDComponentRegion::SetLevel0Component(
  ON_SubDComponentPtr component_ptr
)
{
  const class ON_SubDComponentBase* component_base = component_ptr.ComponentBase();
  if (nullptr != component_base)
  {
    m_level0_component = component_ptr;
    m_level0_component_id = component_base->m_id;
  }
  else
  {
    m_level0_component = ON_SubDComponentPtr::Null;
    m_level0_component_id = 0;
  }
  m_region_index = ON_SubDComponentRegionIndex::Zero;
}

void ON_SubDComponentRegion::SetLevel0Face(
  const ON_SubDFace* face
  )
{
  SetLevel0Component(ON_SubDComponentPtr::Create(face));
}

void ON_SubDComponentRegion::SetLevel0EdgePtr(
  const ON_SubDEdgePtr edge_ptr
)
{
  SetLevel0Component(ON_SubDComponentPtr::Create(edge_ptr));
}

void ON_SubDComponentRegion::SetLevel0Vertex(
  const ON_SubDVertex* vertex
)
{
  SetLevel0Component(ON_SubDComponentPtr::Create(vertex));
}

void ON_SubDComponentRegionIndex::Push(
  unsigned int region_index
  )
{
  if ( region_index > 0xFFFFU )
    region_index = 0xFFFFU;
  if ( m_subdivision_count < ON_SubDComponentRegionIndex::IndexCapacity )
    m_index[m_subdivision_count] = (unsigned short)region_index;
  ++m_subdivision_count;
}

void ON_SubDComponentRegionIndex::Pop()
{
  if (m_subdivision_count > 0)
  {
    m_subdivision_count--;
    if ( m_subdivision_count < ON_SubDComponentRegionIndex::IndexCapacity)
      m_index[m_subdivision_count] = 0;
  }
}


void ON_SubDComponentRegion::PushAdjusted(
  unsigned int region_index
  )
{
  if (
    ON_SubDComponentPtr::Type::Edge == m_level0_component.ComponentType()
    && 0 != m_level0_component.ComponentMark()
    && region_index <= 1
    )
  {
    region_index = 1 - region_index;
  }
  PushAbsolute(region_index);
}

void ON_SubDComponentRegion::PushAbsolute(
  unsigned int region_index
  )
{
  m_region_index.Push(region_index);
  ON_SubDComponentRegionBreakpoint(this);
}

void ON_SubDComponentRegion::Pop()
{
  m_region_index.Pop();
}

unsigned short ON_SubDComponentRegion::SubdivisionCount() const
{
  return m_region_index.m_subdivision_count;
}


unsigned short ON_SubDComponentRegionIndex::Index(
  unsigned short i
) const
{
  return
    (i < m_subdivision_count && i < ON_SubDComponentRegionIndex::IndexCapacity)
    ? m_index[i]
    : 0xFFFF;
}

void ON_SubDFaceRegion::Push(unsigned int quadrant_index)
{
  m_face_region.PushAbsolute(quadrant_index);

  if (quadrant_index >= 0 && quadrant_index < 4)
  {
    m_edge_region[quadrant_index].PushAdjusted(0); // 1st half of this edge relative to face's orientation (adjusted to edge's orientation)
    m_edge_region[(quadrant_index + 1) % 4] = ON_SubDComponentRegion::CreateSubdivisionRegion(ON_SubDComponentPtr::Type::Edge, true, m_edge_region[quadrant_index].SubdivisionCount(), false);
    m_edge_region[(quadrant_index + 2) % 4] = ON_SubDComponentRegion::CreateSubdivisionRegion(ON_SubDComponentPtr::Type::Edge, false, m_edge_region[quadrant_index].SubdivisionCount(), false);
    m_edge_region[(quadrant_index + 3) % 4].PushAdjusted(1); // 2nd half of this edge relative to face's orientation (adjusted to edge's orientation)
  }

  const int surviving_vi
    = ((4 != m_level0_edge_count) && (1 == m_face_region.SubdivisionCount()))
    ? 2
    : quadrant_index;
  m_vertex_id[(surviving_vi+1)%4] = 0;
  m_vertex_id[(surviving_vi+2)%4] = 0;
  m_vertex_id[(surviving_vi+3)%4] = 0;
}

bool ON_SubDComponentRegion::IsEmptyRegion() const
{
  return
    ON_SubDComponentPtr::Type::Unset == m_level0_component.ComponentType()
    && m_level0_component.IsNull()
    && 0 == m_level0_component_id
    && 0 == SubdivisionCount();
}

bool ON_SubDFaceRegion::IsValid(
  bool bSilentError
) const
{
  if (m_face_region.IsEmptyRegion())
  {
    for (int ei = 0; ei < 4; ei++)
    {
      if (false == m_edge_region[ei].IsEmptyRegion())
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("m_face_region is empty and m_edge_region[] is not empty.");
        return false;
      }
    }
    for (int vi = 0; vi < 4; vi++)
    {
      if (0 != m_vertex_id[vi])
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("m_face_region is empty and m_vertex_id[] is not zero.");
        return false;
      }
    }
    return true;
  }
  


  const ON_SubDComponentPtr::Type face_type = m_face_region.m_level0_component.ComponentType();
  if (ON_SubDComponentPtr::Type::Face != face_type)
  {
    if (false == bSilentError)
      ON_SUBD_ERROR("Invalid m_face_region.");
    return false;
  }

  if (false == m_face_region.IsPersistentId())
  {
    if (false == bSilentError)
      ON_SUBD_ERROR("m_face_region.IsPersistentId() is false");
    return false;
  }

  const ON_SubDFace* face = m_face_region.m_level0_component.Face();
  if (nullptr == face )
  {
    if (false == bSilentError)
      ON_SUBD_ERROR("m_face_region.m_level0_component.Face() is nullptr.");
    return false;
  }

  if (face->m_id != m_face_region.m_level0_component_id)
  {
    if (false == bSilentError)
      ON_SUBD_ERROR("Unexpected value for m_face_region.m_level0_component_id");
    return false;
  }

  const unsigned int edge_count = (nullptr != face) ? face->EdgeCount() : 0;
  const bool bIsQuad = (4 == edge_count);
  if (false == bIsQuad)
  {
    if (0 == m_face_region.SubdivisionCount())
    {
      if (false == bSilentError)
        ON_SUBD_ERROR("m_face_region is not a quad and 0 = m_subdivision_count.");
      return false;
    }
    if (((unsigned int)m_face_region.m_region_index.m_index[0]) >= edge_count)
    {
      if (false == bSilentError)
        ON_SUBD_ERROR("Unexpected value in face_region.m_region_index[0].");
      return false;
    }
  }

  bool bPeristentVertex[4] = { bIsQuad, bIsQuad, true, bIsQuad };
  bool bPeristentEdge[4] = { bIsQuad, true, true, bIsQuad };
  for (unsigned short i = bIsQuad?0:1; i < m_face_region.SubdivisionCount() && i < ON_SubDComponentRegionIndex::IndexCapacity; i++)
  {
    const unsigned short r = m_face_region.m_region_index.m_index[i];
    if (r >= 4)
    {
      if (false == bSilentError)
        ON_SUBD_ERROR("Unexpected value in face_region.m_region_index[].");
      return false;
    }
    bPeristentVertex[(r+1)%4] = false;
    bPeristentVertex[(r+2)%4] = false;
    bPeristentVertex[(r+3)%4] = false;
    bPeristentEdge[(r+1)%4] = false;
    bPeristentEdge[(r+2)%4] = false;
    if (false == bPeristentVertex[r] && false == bPeristentEdge[r] && false == bPeristentEdge[(r + 3) % 4] )
      break;
  }
   
  for (int ei = 0; ei < 4; ei++)
  {
    const bool bEmptyEdge = m_edge_region[ei].IsEmptyRegion();
    if (bEmptyEdge)
    {
      if ( bPeristentEdge[ei])
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("Unexpected empty edge in m_edge_region[].");
        return false;
      }
      continue;
    }

    const ON_SubDComponentPtr::Type edge_type = m_edge_region[ei].m_level0_component.ComponentType();
    if (ON_SubDComponentPtr::Type::Edge != edge_type)
    {
      if (false == bSilentError)
        ON_SUBD_ERROR("Invalid m_face_region.");
      return false;
    }
    if ( m_edge_region[ei].SubdivisionCount() != m_face_region.SubdivisionCount() )
    {
      if (false == bSilentError)
        ON_SUBD_ERROR("m_edge_region[].m_subdivision_count != m_face_region.m_subdivision_count.");
      return false;
    }

    if (bPeristentEdge[ei])
    {
      if (false == m_edge_region[ei].IsPersistentId())
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("m_edge_region[] missing a persistent edge id.");
        return false;
      }
      const ON_SubDEdge* edge = m_edge_region[ei].m_level0_component.Edge();
      if (nullptr == edge)
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("Unexpected value for m_edge_region[].m_level0_component.Edge()");
        return false;
      }
      if (edge->m_id != m_edge_region[ei].m_level0_component_id)
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("Unexpected value for m_edge_region[].m_level0_component_id");
        return false;
      }
    }
    else
    {
      if ( false == m_edge_region[ei].IsTransientId() )
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("m_edge_region[] missing a transient edge id.");
        return false;
      }
    }
  }

   
  for (int vi = 0; vi < 4; vi++)
  {
    if (bPeristentVertex[vi])
    {
      if (false == ON_SubDComponentRegion::IsPersistentId(m_vertex_id[vi]))
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("m_vertex_id[] missing a persistent vertex id.");
        return false;
      }
    }
    else if ( 0 != m_vertex_id[vi] )
    {
      if (false == ON_SubDComponentRegion::IsTransientId(m_vertex_id[vi]))
      {
        if (false == bSilentError)
          ON_SUBD_ERROR("m_vertex_id[] missing a transient vertex id.");
        return false;
      }
    }
  }

  return true;
}

bool ON_SubDComponentRegion::IsTransientId() const
{
  return ON_SubDComponentRegion::IsTransientId(m_level0_component_id);
}

bool ON_SubDComponentRegion::IsPersistentId() const
{
  return ON_SubDComponentRegion::IsPersistentId(m_level0_component_id);
}

static unsigned int Internal_TransientIdHelper(
  bool bReset
)
{
  static unsigned int src = 0; // shoule be a "atomic" unsigned - but not critical

  if (bReset)
  {
    src = 0;
    return 0;
  }

  unsigned int transient_id = ++src;
  if (0 != (ON_SubDComponentRegion::TransientIdBit & transient_id))
  {
    src = 1;
    transient_id = 1; // because other threads may have modified src.
  }
  transient_id |= ON_SubDComponentRegion::TransientIdBit;
  return transient_id;
}

void ON_SubDComponentRegion::ResetTransientId()
{
  Internal_TransientIdHelper(true);
}

const unsigned int ON_SubDComponentRegion::NewTransientId()
{
  return Internal_TransientIdHelper(false);
}

bool ON_SubDComponentRegion::IsPersistentId(unsigned int id)
{
  return (0 != id) && (0 == (ON_SubDComponentRegion::TransientIdBit & id));
}

unsigned int ON_SubDComponentRegion::TransientId(unsigned int id)
{
  return (0 != (ON_SubDComponentRegion::TransientIdBit & id)) ? ((~ON_SubDComponentRegion::TransientIdBit) & id) : 0;
}


bool ON_SubDComponentRegion::IsTransientId(unsigned int id)
{
  return 0 != (ON_SubDComponentRegion::TransientIdBit & id) && 0 != ((~ON_SubDComponentRegion::TransientIdBit) & id);
}

static wchar_t* Internal_AppendUnsigned(
  //wchar_t prefix1,
  //wchar_t prefix2,
  unsigned int i,
  wchar_t* s,
  wchar_t* s1
  )
{
  //if (0 != prefix1 && s < s1)
  //  *s++ = prefix1;
  //if (0 != prefix2 && s < s1)
  //  *s++ = prefix2;
  const bool bTransientId = (0 != (ON_SubDComponentRegion::TransientIdBit & i) );
  if (bTransientId)
  {
    if (s < s1)
      *s++ = '<';
    i &= ~ON_SubDComponentRegion::TransientIdBit;
  }
  wchar_t buffer[64];
  wchar_t* sdigit = buffer;
  wchar_t* sdigit1 = sdigit + (sizeof(buffer)/sizeof(buffer[0]));
  for ( *sdigit++ = 0; sdigit < sdigit1; sdigit++ )
  {
    *sdigit = (wchar_t)('0' + (i%10));
    i /= 10;
    if (0 == i)
    {
      while ( s < s1 && 0 != (*s = *sdigit--) )
        s++;
      break;
    }
  }
  if (bTransientId)
  {
    if (s < s1)
      *s++ = '>';
  }
  if (s <= s1)
    *s = 0;
  return s;
}

wchar_t* ON_SubDComponentPtr::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  *s = 0;
  wchar_t* s1 = s + (s_capacity - 1);
  *s1 = 0;
  if (s < s1)
  {
    if (0 == m_ptr)
    {
      if (s + 7 < s1)
      {
        *s++ = 'N';
        *s++ = 'u';
        *s++ = 'l';
        *s++ = 'l';
        *s++ = 'P';
        *s++ = 't';
        *s++ = 'r';
      }
    }
    else
    {
      wchar_t c;
      switch (ComponentType())
      {
      case ON_SubDComponentPtr::Type::Vertex:
        c = 'v';
        break;
      case ON_SubDComponentPtr::Type::Edge:
        if ( s+2 < s1 )
          *s++ = (ComponentMark()) ? '-' : '+';
        c = 'e';
        break;
      case ON_SubDComponentPtr::Type::Face:
        c = 'f';
        break;
      case ON_SubDComponentPtr::Type::Unset:
        c = 0;
        break;
      default:
        c = 0;
        break;
      }

      if (0 == c)
      {
        *s++ = '?';
      }
      else
      {
        *s++ = c;
        if (IsNull() && s + 6 < s1)
        {
          *s++ = '[';
          *s++ = 'n';
          *s++ = 'u';
          *s++ = 'l';
          *s++ = 'l';
          *s++ = ']';
        }
      }
    }
  }
  
  if (nullptr != s && s <= s1)
    *s = 0;
  return s;
};


const ON_wString ON_SubDComponentRegionIndex::ToString() const
{
  wchar_t buffer[32];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return ON_wString(buffer);
  return ON_wString::EmptyString;
}

wchar_t* ON_SubDComponentRegionIndex::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  *s = 0;
  wchar_t* s1 = s + (s_capacity - 1);
  *s1 = 0;

  if (s < s1)
  {
    for (unsigned short i = 0; i < m_subdivision_count && nullptr != s && s < s1; i++)
    {
      if (s < s1)
        *s++ = '.';
      if (i >= ON_SubDComponentRegionIndex::IndexCapacity)
      {
        // more subdivision levels that m_region_index[] can record.
        if (s < s1)
          *s++ = '_';
        break;
      }
      if (0xFFFF == m_index[i])
      {
        // This is component was added during a subdivision
        // and did not exist at level i.
        if (s < s1)
          *s++ = 'x';
      }
      else
      {
        // portion of component subdivided at level i
        s = Internal_AppendUnsigned(m_index[i], s, s1);
      }
    }
  }

  if (nullptr != s && s <= s1)
    *s = 0;

  return s;
}


const ON_wString ON_SubDComponentPtr::ToString() const
{
  wchar_t buffer[32];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return ON_wString(buffer);
  return ON_wString::EmptyString;
}


wchar_t* ON_SubDComponentRegion::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  *s = 0;
  wchar_t* s1 = s + (s_capacity - 1);
  *s1 = 0;
  if (s < s1)
  {
    s = m_level0_component.ToString(s, s_capacity);
    if (nullptr != s && s < s1)
      s = Internal_AppendUnsigned(m_level0_component_id, s, s1);
  }

  if (nullptr != s && s < s1)
    s = m_region_index.ToString(s, 1 + (s1 - s));
  
  if (nullptr != s && s <= s1)
    *s = 0;

  return s;
}


const ON_wString ON_SubDComponentRegion::ToString() const
{
  wchar_t buffer[128];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return ON_wString(buffer);
  return ON_wString::EmptyString;
}


ON__UINT32 ON_SubDComponentRegionIndex::ToCompressedRegionIndex() const
{
  return ON_SubDComponentRegionIndex::ToCompressedRegionIndex( m_subdivision_count, m_index);
}

const ON_SubDComponentRegionIndex ON_SubDComponentRegionIndex::FromCompressedRegionIndex(
  ON__UINT32 compressed_region_index
)
{
  ON_SubDComponentRegionIndex ri;
  ON_SubDComponentRegionIndex::FromCompressedRegionIndex(compressed_region_index, &ri.m_subdivision_count, ri.m_index);
  return ri;
}

ON__UINT32 ON_SubDComponentRegionIndex::ToCompressedRegionIndex(
    unsigned short subdivision_count,
    const unsigned short* region_index
  )
{
  ON__UINT32 rc 
    = (subdivision_count >= 255)
    ? 255
    : (ON__UINT32)subdivision_count;
  rc <<= 24;
  if (nullptr != region_index && subdivision_count > 0)
  {
    ON__UINT32 idx
      = (region_index[0] >= 255)
      ? 255
      : (ON__UINT32)region_index[0];
    idx <<= 16;
    ON__UINT32 shift = 14;
    for (unsigned short i = 1; i < subdivision_count && i < ON_SubDComponentRegionIndex::IndexCapacity && shift <= 14; i++)
    {
      ON__UINT32 bits = (ON__UINT32)region_index[i];
      if (bits > 3)
        bits = 3;
      idx |= (bits << shift);
      shift -= 2;
    }
    rc |= idx;
  }
  return rc;
  //ON__UINT32 shift = 27;
  //ON__UINT32 rc = (ON__UINT32)subdivision_count;
  //rc <<= shift;
  //
  //const unsigned short count
  //  = (subdivision_count <= ON_SubDComponentRegion::region_index_capacity)
  //  ? subdivision_count
  //  : ON_SubDComponentRegion::region_index_capacity;
  //for (unsigned short i = 0; i < count; i++)
  //{
  //  shift -= 3;
  //  ON__UINT32 three_bits = (ON__UINT32)(region_index[i] % 0x07);
  //  if (0 != three_bits)
  //    rc |= (three_bits << shift);
  //}
  //return rc;
}

void ON_SubDComponentRegionIndex::FromCompressedRegionIndex(
  ON__UINT32 compressed_region_index,
  unsigned short* subdivision_count,
  unsigned short region_index[]
)
{
  const ON__UINT32 count = (compressed_region_index >> 24);
  if (nullptr != subdivision_count)
    *subdivision_count = (unsigned short)count;
  if (nullptr != region_index)
  {
    region_index[0] = (unsigned short)((compressed_region_index & 0x00FF0000) >> 16);
    for (unsigned short i = 1; i < ON_SubDComponentRegionIndex::IndexCapacity; i++)
    {
      region_index[i] = (unsigned short)((compressed_region_index & 0x0000C000) >> 14);
      compressed_region_index <<= 2;
    }
  }

  //ON__UINT32 shift = 27;
  //ON__UINT32 count = (region32 >> shift);
  //if (nullptr != subdivision_count)
  //  *subdivision_count = (unsigned short)count;
  //if (nullptr != region_index)
  //{
  //  for (unsigned short i = 0; i < ON_SubDComponentRegion::region_index_capacity; i++)
  //    region_index[i] = 0;
  //  unsigned int idx = region32 << (32-shift);
  //  for (unsigned short i = 0; 0 != idx && i < ON_SubDComponentRegion::region_index_capacity; i++)
  //  {
  //    ON__UINT32 three_bits = (idx & 0xE0000000U);
  //    three_bits >>= 29;
  //    region_index[i] = (unsigned short)three_bits;
  //    idx <<= 3;      
  //  }
  //}

}

static ON_ProgressStepCounter CreateFragmentProgressStepCounter(
  ON_SubDFaceIterator& fit,
  const ON_SubDDisplayParameters& limit_mesh_parameters
  )
{
  unsigned int progress_step_count = 0;

  if (nullptr != limit_mesh_parameters.m_progress_reporter)
  {
    for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
    {
      if ( 4 == face->m_edge_count)
        progress_step_count++;
      else
        progress_step_count += face->m_edge_count;
    }
  }

  ON_ProgressStepCounter counter = ON_ProgressStepCounter::Create(
    limit_mesh_parameters.m_progress_reporter,
    progress_step_count,
    limit_mesh_parameters.m_progress_reporter_interval[0],
    limit_mesh_parameters.m_progress_reporter_interval[1],
    100
    );

  return counter;

}

wchar_t* ON_SubDFaceRegion::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  wchar_t* s1 = s + s_capacity-1;
  *s1 = 0;

  s = m_face_region.ToString(s, s_capacity);
  if (nullptr != s && s+4 < s1)
  {
    for (unsigned int i = 0; i < 4 && nullptr != s && s + 4 < s1; i++)
    {
      *s++ = ON_wString::Space;
      *s++ = (0 == i) ? '(' : ',';
      if ( ON_SubDComponentPtr::Type::Edge == m_edge_region[i].m_level0_component.ComponentType() )
      {
        s = m_edge_region[i].ToString(s, s1 - s);
      }
      else
      {
        *s++ = 'e';
        *s++ = '?';
      }
    }
    if (nullptr != s && s < s1)
      *s++ = ')';
  }

  if (nullptr != s && s+4 < s1)
  {
    for (unsigned int i = 0; i < 4 && nullptr != s && s + 4 < s1; i++)
    {
      *s++ = ON_wString::Space;
      *s++ = (0 == i) ? '(' : ',';
      if (0 != m_vertex_id[i])
      {
        *s++ = 'v';
        s = Internal_AppendUnsigned(m_vertex_id[i], s, s1);
      }
      else
      {
        *s++ = '0';
      }
    }
    if (nullptr != s && s < s1)
      *s++ = ')';
  }

  if (nullptr != s && s <= s1)
    *s = 0;
  return s;
}

const ON_wString ON_SubDFaceRegion::ToString() const
{
  wchar_t buffer[256];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return ON_wString(buffer);
  return ON_wString::EmptyString;
}

static unsigned int GetQuadLimitSurfaceMeshFragmentsHelper(
  ON_SubDFaceIterator& fit,
  const ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*mesh_fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitMeshFragment*)
  )
{
  ON_ProgressStepCounter counter = CreateFragmentProgressStepCounter(fit,limit_mesh_parameters);
  
  if (nullptr == fit.FirstFace() )
    return ON_SUBD_RETURN_ERROR(0);

  if ( nullptr == mesh_fragment_callback_function )
    return ON_SUBD_RETURN_ERROR(0);

  unsigned int display_density = limit_mesh_parameters.m_display_density;
  //const bool bUseMultipleThreads = limit_mesh_parameters.m_bUseMultipleThreads;


  ON_SubDManagedLimitMeshFragment fragment;
  ON_SubDManagedLimitMeshFragment sub_fragment;
  ON_SubDQuadFaceMesher qfm;
  ON_SubDQuadFaceMesher sub_qfm;
  ON_SubDFaceNeighborhood fnbd;

  qfm.m_output = ON_SubDQuadFaceMesher::Output::mesh;

  ON_SubDLimitMeshFragment* callback_fragment = nullptr;
  const ON_SubDFace** quad_faces = nullptr;
  unsigned int quad_face_count = 0;

  unsigned int fragment_count = 0;

  if (0 == display_density)
  {
    // make sure all faces are quads.  If not, increase density to 1
    for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
    {
      if (4 == face->m_edge_count)
        continue;
      display_density = 1;
      break;
    }
  }

  Internal_SubQuadTransientComponents face_transient_components(fit);

  // TODO 
  //
  //   Support multiple threads by adding more fragment, sub_fragment qfm, sub_qfm and fnbd
  //   resources and managing them.  
  //
  const unsigned int subquad_display_density = (display_density > 1) ? (display_density - 1) : 0;
  const unsigned short unset_face_edge_index = 0xFFFFU;
  for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
  {
    if (face->m_edge_count < 3)
      continue;

    face_transient_components.Initialize(face);

    if (4 == face->m_edge_count)
    {
      // face is a quad
      quad_faces = &face;
      quad_face_count = 1;
      if (callback_fragment != &fragment)
      {
        if (0 == fragment.m_P_capacity)
        {
          fragment.ReserveCapacity(ON_SubD::FacetType::Quad, display_density);
          fragment.m_P_count = (unsigned short)ON_SubDLimitMeshFragment::PointCountFromDisplayDensity(ON_SubD::FacetType::Quad,display_density);
          if ( fragment.m_P_count > fragment.m_P_capacity)
            return ON_SUBD_RETURN_ERROR(0);
          fragment.m_face_vertex_index[0] = 0;
          fragment.m_face_vertex_index[1] = 1;
          fragment.m_face_vertex_index[2] = 2;
          fragment.m_face_vertex_index[3] = 3;
        }
        callback_fragment = &fragment;
        qfm.SetDestinationToMeshFragment(display_density, fragment);
      }
    }
    else
    {
      // face is not a quad.  It will be subdivided into quads and each
      // of those quads is meshed a a level of display_density-1.
      if (false == fnbd.Subdivide(ON_SubD::SubDType::QuadCatmullClark, face))
        continue;
      quad_faces = fnbd.m_center_vertex1->m_faces;
      quad_face_count = fnbd.m_center_vertex1->m_face_count;
      if (quad_face_count < 2)
        continue;
      if (callback_fragment != &sub_fragment)
      {
        if (0 == sub_fragment.m_P_capacity)
        {
          sub_fragment.ReserveCapacity(ON_SubD::FacetType::Quad, subquad_display_density);
          sub_fragment.m_P_count = (unsigned short)ON_SubDLimitMeshFragment::PointCountFromDisplayDensity(ON_SubD::FacetType::Quad,subquad_display_density);
          if ( sub_fragment.m_P_count > sub_fragment.m_P_capacity)
            return ON_SUBD_RETURN_ERROR(0);
          sub_fragment.m_face_vertex_index[0] = unset_face_edge_index;
          sub_fragment.m_face_vertex_index[1] = unset_face_edge_index;
          sub_fragment.m_face_vertex_index[2] = unset_face_edge_index;
          sub_fragment.m_face_vertex_index[3] = unset_face_edge_index;
        }
        callback_fragment = &sub_fragment;
        qfm.SetDestinationToMeshFragment(subquad_display_density, sub_fragment);
      }
    }

    callback_fragment->m_face = face;
    callback_fragment->m_face_fragment_count = (unsigned short)quad_face_count;

    for (unsigned int qi = 0; qi < quad_face_count; qi++)
    {
      face_transient_components.NextSubQuad();
      ON_SubDFaceRegion face_region;
      Internal_SetLevel0FaceAndEdgeRegion(
        face,
        qi, face_transient_components, // qi and subdivsion_edge_region[] are used only when "f" is a level 1 sud quad of "face"
        face_region
      );
      
      const ON_SubDFace* f = quad_faces[qi];
      if (unset_face_edge_index == callback_fragment->m_face_vertex_index[0])
        callback_fragment->m_face_vertex_index[2] = (unsigned short)((qi+1)%quad_face_count);

      callback_fragment->m_face_fragment_index = (unsigned short)qi;

      if (false == qfm.GetLimitMesh(face_region, f))
        continue;
      
      fragment_count++;
      callback_fragment->m_bbox = ON_PointListBoundingBox(3,0,callback_fragment->m_P_count,(int)callback_fragment->m_P_stride,callback_fragment->m_P);
      if (false == mesh_fragment_callback_function(fragment_callback_context, callback_fragment))
        return true;

      if (0 == (fragment_count % 16))
      {
        if (ON_Terminator::TerminationRequested(limit_mesh_parameters.m_terminator))
          return 0; // not an error
      }
      counter.IncrementStep();
    }
  }

  counter.Finished();

  return fragment_count;
}


static unsigned int GetQuadLimitSurfacePatchFragmentsHelper(
  ON_SubDFaceIterator& fit,
  const ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*begin_face_callback_function)(ON__UINT_PTR ,const ON_SubDFaceRegion&),//, const class ON_SubDFace*, const class ON_SubDFace*, unsigned int),
  bool(*patch_fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitNurbsFragment*)
  )
{
  ON_ProgressStepCounter counter = CreateFragmentProgressStepCounter(fit,limit_mesh_parameters);
  
  if (nullptr == fit.FirstFace() )
    return ON_SUBD_RETURN_ERROR(0);

  if ( nullptr == patch_fragment_callback_function )
    return ON_SUBD_RETURN_ERROR(0);

  unsigned int display_density = limit_mesh_parameters.m_display_density;
  //const bool bUseMultipleThreads = limit_mesh_parameters.m_bUseMultipleThreads;
  
  ON_SubDQuadFacePatcher patcher;
  ON_SubDQuadFaceMesher qfm;
  ON_SubDQuadFaceMesher sub_qfm;
  ON_SubDFaceNeighborhood fnbd;

  Internal_SubQuadTransientComponents face_transient_components(fit);

  const ON_SubDFace** quad_faces = nullptr;
  unsigned int quad_face_count = 0;

  unsigned int fragment_count = 0;

  if (0 == display_density)
  {
    // make sure all faces are quads.  If not, increase density to 1
    for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
    {
      if (4 == face->m_edge_count)
      {
        display_density = 1;
        break;
      }
    }
  }

  patcher.m_fragment_callback_context = fragment_callback_context;
  patcher.m_fragment_callback_function = patch_fragment_callback_function;
  const unsigned int subquad_display_density = (display_density > 1) ? (display_density - 1) : 0;

  for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
  {
    if (face->m_edge_count < 3)
      continue;

    face_transient_components.Initialize(face);

    if (4 == face->m_edge_count)
    {
      // face is a quad
      quad_faces = &face;
      quad_face_count = 1;
      patcher.m_display_density = display_density;
    }
    else
    {
      // face is not a quad.  It will be subdivided into quads and each
      // of those quads is meshed a a level of display_density-1.
      if (false == fnbd.Subdivide(ON_SubD::SubDType::QuadCatmullClark, face))
        continue;
      quad_faces = fnbd.m_center_vertex1->m_faces;
      quad_face_count = fnbd.m_center_vertex1->m_face_count;
      patcher.m_display_density = subquad_display_density;
      if (quad_face_count < 2)
        continue;
    }

    patcher.m_patch_fragment = ON_SubDLimitNurbsFragment::Unset;
    qfm.SetDestinationToPatchFragment(patcher);

    for (unsigned int qi = 0; qi < quad_face_count; qi++)
    {
      face_transient_components.NextSubQuad();

      // Internal_SetLevel0FaceAndEdgeRegion() sets face_region and patcher.m_patch_fragment.m_edge_region[]
      // In the case when face is not a quad and "f" is the level 1 subd quad, 
      // the regions (face_region and patcher.m_patch_fragment.m_edge_region[])
      // are "Pushed()" so they indicate what portion of "face" and it's original edges are being used.
      ON_SubDFaceRegion face_region;
      Internal_SetLevel0FaceAndEdgeRegion(
        face, 
        qi, face_transient_components, // qi and subdivsion_edge_region[] are used only when "f" is a level 1 sud quad of "face"
        face_region
      );

      const ON_SubDFace* f = quad_faces[qi];
      if (nullptr != begin_face_callback_function)
      {
        // SubD to NURBS case:
        //   fragment_callback_context = pointer to Internal_SubDNurbsPatchGetter class
        //   begin_face_callback_function = Internal_SubDNurbsPatchGetter::BeginFaceCallback()
        //   Internal_SubDNurbsPatchGetter::BeginFaceCallback() flushes accumulated surface content from previous SUbD face.
        //   Then it initializes fragment_callback_context for current "face" and, it it exits, sub-quad "f"
        begin_face_callback_function(fragment_callback_context, face_region);//, face, (f != face) ? f : nullptr, qi);
      }
      
      patcher.m_patch_fragment.m_face_region = face_region;

      if (false == qfm.GetLimitPatches(face_region, f))
        continue;
      
      fragment_count++;

      if (0 == (fragment_count % 16))
      {
        if (ON_Terminator::TerminationRequested(limit_mesh_parameters.m_terminator))
          return 0; // not an error
      }
      counter.IncrementStep();
    }
  }

  counter.Finished();

  return fragment_count;
}

class GetLimitSurfaceMesh_context_FragmentMark
{
public:
  class GetLimitSurfaceMesh_context* m_context = nullptr;

  size_t m_point_count0 = 0;
  size_t m_quad_count0 = 0;
  size_t m_fragment_count0 = 0;

  size_t m_point_count1 = 0;
  size_t m_quad_count1 = 0;
  size_t m_fragment_count1 = 0;

  unsigned int m_mark_index = 0;
  unsigned int m_face_id = 0;
  unsigned int m_zero_face_id = 0;
  unsigned int m_parent_face_id = 0;
  unsigned int m_face_fragment_count = 0;
  unsigned int m_face_fragment_index = 0;

  ON__UINT_PTR m_fragment_group_id = 0;

  // points to memory in the m_context that is under construction.
  ON_SubDLimitMeshFragment m_fragment = ON_SubDLimitMeshFragment::Empty;

  static int CompareFaceIdAndFragmentIndex(
    const GetLimitSurfaceMesh_context_FragmentMark* a,
    const GetLimitSurfaceMesh_context_FragmentMark* b
    );
};

int GetLimitSurfaceMesh_context_FragmentMark::CompareFaceIdAndFragmentIndex(
    const GetLimitSurfaceMesh_context_FragmentMark* a,
    const GetLimitSurfaceMesh_context_FragmentMark* b
    )
{
  if ( a == b )
    return 0;
  if ( nullptr == a )
    return -1;
  if ( nullptr == b )
    return 1;
  if ( a->m_face_id < b->m_face_id )
    return -1;
  if ( a->m_face_id > b->m_face_id )
    return 1;
  if ( a->m_face_fragment_index < b->m_face_fragment_index )
    return -1;
  if ( a->m_face_fragment_index > b->m_face_fragment_index )
    return 1;
  return 0;
}

class GetLimitSurfaceMesh_context
{
public:
  GetLimitSurfaceMesh_context() = default;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  bool m_bUseMultipleThreads = false; // If true, callback uses the lock
  ON_SleepLock m_lock;
#endif

  size_t m_point_capacity = 0;
  double* m_points = nullptr;
  size_t m_point_stride = 0;

  size_t m_normal_capacity = 0;
  double* m_normals = nullptr;
  size_t m_normal_stride = 0;

  size_t m_quad_capacity = 0;
  
  unsigned int* m_quads = nullptr;
  size_t m_quad_stride = 0; // >= 4

  ON__UINT_PTR* m_quad_group_ids = nullptr;
  size_t m_quad_group_id_stride = 0;

  // counts are updated as meshing progresses
  size_t m_point_count = 0;
  size_t m_quad_count = 0;
  size_t m_fragment_count = 0;

  ON_SimpleArray< GetLimitSurfaceMesh_context_FragmentMark > m_marks;
};

static bool CoincidentPointTest(
  unsigned int i,
  unsigned int j,
  const double* points,
  const size_t point_stride,
  const double* normals,
  const size_t normal_stride
  )
{
  if ( i == j)
    return true;
  
  const double* a = points + i*point_stride;
  const double* b = points + j*point_stride;
  double d = fabs(a[0]-b[0]) + fabs(a[1]-b[1]) + fabs(a[2]-b[2]);
  if (!(d <= 1e-8))
    return ON_SUBD_RETURN_ERROR(false);
  
  a = normals + i*normal_stride;
  b = normals + j*normal_stride;
  d = fabs(a[0]-b[0]) + fabs(a[1]-b[1]) + fabs(a[2]-b[2]);
  if (!(d <= 0.01))
    return ON_SUBD_RETURN_ERROR(false);
  
  return true;
}

static bool GetLimitSurfaceMesh_callback(ON__UINT_PTR void_context, const class ON_SubDLimitMeshFragment* fragment)
{
  GetLimitSurfaceMesh_context* context = (GetLimitSurfaceMesh_context*)void_context;

  unsigned int mark_count = 0;
  ON_SimpleArray< GetLimitSurfaceMesh_context_FragmentMark > face_fragment_marks;

  // When using multiple threads, 
  // get the lock,
  // quickly reserve the section of the desitinataion arrays that will be used by this fragment, 
  // and then return the lock.
  // SInce the space is now reserved, the actual copying can be done after returning the lock.
  GetLimitSurfaceMesh_context_FragmentMark mark;
  mark.m_context = context;
  mark.m_fragment = *fragment;
  if (nullptr != fragment->m_face)
  {
    mark.m_face_id = fragment->m_face->m_id;
    mark.m_zero_face_id = fragment->m_face->m_zero_face_id;
    mark.m_parent_face_id = fragment->m_face->m_parent_face_id;
  }
  mark.m_face_fragment_count = fragment->m_face_fragment_count;
  mark.m_face_fragment_index = fragment->m_face_fragment_index;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  bool bReleaseLock = false;
  if (context->m_bUseMultipleThreads)
  {
    if (false == context->m_lock.GetLock(0, ON_SleepLock::OneSecond))
    {
      // return tru to keep going, but something is really hogging the lock
      return ON_SUBD_RETURN_ERROR(true);
    }
  }
#endif

  mark.m_point_count0 = context->m_point_count;
  mark.m_quad_count0 = context->m_quad_count;
  mark.m_point_count1 = mark.m_point_count0 + fragment->m_P_count;
  mark.m_quad_count1 = mark.m_quad_count0 + fragment->m_grid.m_F_count;
  if (mark.m_point_count1 > context->m_point_capacity || mark.m_quad_count1 > context->m_quad_capacity)
  {
#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
    if (bReleaseLock)
      context->m_lock.ReturnLock();
#endif
    return ON_SUBD_RETURN_ERROR(false);
  }
  mark.m_mark_index = context->m_marks.UnsignedCount();

  mark.m_fragment_group_id = (ON__UINT_PTR)(context->m_fragment_count+1);

  if (mark.m_face_fragment_count <= 1 || 0 == mark.m_face_id || ON_UNSET_UINT_INDEX == mark.m_face_id)
  {
    mark_count = 1;
  }
  else if ( context->m_marks.UnsignedCount() + 1 >= mark.m_face_fragment_count )
  { 
    GetLimitSurfaceMesh_context_FragmentMark* context_fragment_marks = context->m_marks.Array();
    const unsigned int context_mark_count0 = context->m_marks.UnsignedCount();

    // See if we have all the subfragments for this face
    for (unsigned int i = 0; i < context_mark_count0; i++)
    {
      if ( context_fragment_marks[i].m_face_id == mark.m_face_id )
        mark_count++;
    }

    if (mark_count + 1 >= mark.m_face_fragment_count)
    {
      // move all the marks for this face from context->m_marks[] (possibly used by multiple threads)
      // to the local context_fragment_marks[] array for processing below.
      unsigned int context_mark_count1 = 0;
      face_fragment_marks.Reserve(mark.m_face_fragment_count);
      if (context_mark_count0 + 1 >= mark.m_face_fragment_count)
      {
        face_fragment_marks.Append((int)context_mark_count0, context_fragment_marks);
      }
      else
      {
        for (unsigned int i = 0; i < context_mark_count0; i++)
        {
          if (context_fragment_marks[i].m_face_id == mark.m_face_id)
            face_fragment_marks.Append(context_fragment_marks[i]);
          else
          {
            if (i < context_mark_count1)
              context_fragment_marks[context_mark_count1] = context_fragment_marks[i];
            context_mark_count1++;
          }
        }
      }
      context->m_marks.SetCount(context_mark_count1);
      face_fragment_marks.Append(mark);
      mark_count++;
    }
    else
    {
      // This face will have more subfragments delivered in the future.
      mark_count = 0;
      context->m_marks.AppendNew() = mark;
    }
  }
  else
  {
    // This face will have more subfragments delivered in the future.
    context->m_marks.AppendNew() = mark;
  }

  const size_t P_stride = context->m_point_stride;
  const size_t N_stride = context->m_normal_stride;
  const size_t F_stride = context->m_quad_stride;
  const size_t GID_stride = context->m_quad_group_id_stride;

  double* P 
    = (nullptr != context->m_points) 
    ? (context->m_points + mark.m_point_count0*P_stride)
    : nullptr;
  double* N 
    = (nullptr != context->m_normals) 
    ? (context->m_normals + mark.m_point_count0*N_stride)
    : nullptr;
  unsigned int* F 
    = (nullptr != context->m_quads)
    ? (context->m_quads + mark.m_quad_count0*F_stride)
    : nullptr;
  ON__UINT_PTR* GID 
    = (nullptr != context->m_quad_group_ids)
    ? (context->m_quad_group_ids + mark.m_quad_count0*GID_stride)
    : nullptr;

  context->m_point_count = mark.m_point_count1;
  context->m_quad_count = mark.m_quad_count1;
  context->m_fragment_count++;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  if (bReleaseLock)
    context->m_lock.ReturnLock();
#endif

  // Copy the mesh from the fragment to the destination arrays
  //
  // All the code below must be thread safe.  
  //
  // Basically, space context->m_point_stride[] and the other arrays
  // is reserved above and time consuming calculations take place below so
  // any other threads working to create this mesh can continue.
  if ( nullptr != P )
  {
    const double* srcP1 = fragment->m_P + fragment->m_P_count*fragment->m_P_stride;
    for (const double* srcP = fragment->m_P; srcP < srcP1; srcP += fragment->m_P_stride)
    {
      P[0] = srcP[0];
      P[1] = srcP[1];
      P[2] = srcP[2];
      P += P_stride;
    }  
  }

  if ( nullptr != N )
  {
    const double* srcN1 = fragment->m_N + fragment->m_P_count*fragment->m_N_stride;
    for (const double* srcN = fragment->m_N; srcN < srcN1; srcN += fragment->m_N_stride)
    {
      N[0] = srcN[0];
      N[1] = srcN[1];
      N[2] = srcN[2];
      N += N_stride;
    }  
  }

  if ( nullptr != F )
  {
    const ON_SubDLimitMeshFragmentGrid& quads = fragment->m_grid;
    const unsigned int fvi0 = (unsigned int)mark.m_point_count0;
    const unsigned int* srcF1 = quads.m_F + quads.m_F_count*quads.m_F_stride;
    for (const unsigned int* srcF = quads.m_F; srcF < srcF1; srcF += quads.m_F_stride)
    {
      F[0] = srcF[0] + fvi0;
      F[1] = srcF[1] + fvi0;
      F[2] = srcF[2] + fvi0;
      F[3] = srcF[3] + fvi0;
      F += F_stride;
    }  
  }

  if ( nullptr != GID )
  {
    const ON__UINT_PTR* group_id1 = GID + fragment->m_grid.m_F_count*GID_stride;
    while(GID < group_id1)
    {
      *GID = mark.m_fragment_group_id;
      GID += GID_stride;
    }  
  }

  while (mark_count >= 2 && face_fragment_marks.UnsignedCount() == mark_count )
  {
    // combine subfragments into a single fragment.
    face_fragment_marks.QuickSort( GetLimitSurfaceMesh_context_FragmentMark::CompareFaceIdAndFragmentIndex );
    if (face_fragment_marks[0].m_face_id != face_fragment_marks[mark_count - 1].m_face_id )
      break;
    if (0 != face_fragment_marks[0].m_face_fragment_index)
      break;
    if (mark_count - 1 != face_fragment_marks[mark_count - 1].m_face_fragment_index)
      break;

    mark = face_fragment_marks[0];
    unsigned int* quads = mark.m_context->m_quads;
    const size_t quad_stride = mark.m_context->m_quad_stride;
    const double* points = mark.m_context->m_points;
    const size_t point_stride = mark.m_context->m_point_stride;
    const double* normals = mark.m_context->m_normals;
    const size_t normal_stride = mark.m_context->m_normal_stride;
    ON__UINT_PTR* group_ids = mark.m_context->m_quad_group_ids;
    const size_t group_id_stride = mark.m_context->m_quad_group_id_stride;

    if ( nullptr == quads || nullptr == points || nullptr == normals)
      break;
    if ( quad_stride < 4 || point_stride < 3 || normal_stride < 3 )
      break;

    const unsigned int grid_F_count = mark.m_fragment.m_grid.m_F_count;
    const unsigned int grid_side_count = mark.m_fragment.m_grid.SideSegmentCount();
    const size_t magic_stride = grid_side_count*quad_stride;

    const ON__UINT_PTR group_id =
      (nullptr != group_ids && group_id_stride > 0)
      ? group_ids[mark.m_quad_count0*group_id_stride]
      : 0;

    const unsigned int apex_point_index = (unsigned int)mark.m_point_count0;
    const ON_3dPoint apex_point(points+apex_point_index*point_stride);
    const ON_3dVector apex_normal(normals+apex_point_index*normal_stride);
          
    unsigned int* subfragment_quads = quads + face_fragment_marks[mark_count-1].m_quad_count0*quad_stride;
    unsigned int i;
    for ( i = 0; i < mark_count; i++ )
    {
      mark = face_fragment_marks[i];
      if ( grid_F_count != mark.m_fragment.m_grid.m_F_count)
        break;
      unsigned int* prev_subfragment_quads = subfragment_quads;
      subfragment_quads = quads + mark.m_quad_count0*quad_stride;

      if (false == CoincidentPointTest(apex_point_index,subfragment_quads[0],points,point_stride,normals,normal_stride))
        break;
      subfragment_quads[0] = apex_point_index;

      unsigned int n;
      for (n = 0; n < grid_side_count; n++)
      {
        if (false == CoincidentPointTest(subfragment_quads[0],prev_subfragment_quads[0],points,point_stride,normals,normal_stride))
          break;
        prev_subfragment_quads[0] = subfragment_quads[0];
        
        if (false == CoincidentPointTest(subfragment_quads[1],prev_subfragment_quads[3],points,point_stride,normals,normal_stride))
          break;
        prev_subfragment_quads[3] = subfragment_quads[1];

        subfragment_quads += quad_stride;
        prev_subfragment_quads += magic_stride;
      }

      if (n < grid_side_count)
        break;

      subfragment_quads -= magic_stride;

      if (0 != group_id)
      {
        ON__UINT_PTR* p0 = group_ids + mark.m_quad_count0*group_id_stride;
        if (group_id != p0[0])
        {
          for ( ON__UINT_PTR* p1 = p0 + grid_F_count; p0 < p1; p0 += group_id_stride)
            *p0 = group_id;
        }
      }

    }
    if ( mark_count != i )
      break;

    break;
  }

  return true;
}

static int CompareQuadGroupId(const void* a, const void* b)
{
  ON__UINT_PTR ai = *(const ON__UINT_PTR*)a;
  ON__UINT_PTR bi = *(const ON__UINT_PTR*)b;
  if (ai < bi)
    return -1;
  if (ai > bi)
    return 1;
  return 0;
}

ON_Mesh* ON_SubD::GetLimitSurfaceMesh(
  const class ON_SubDDisplayParameters& limit_mesh_parameters,
  ON_Mesh* destination_mesh
  ) const
{
  ON_SubDDisplayParameters local_limit_mesh_parameters = limit_mesh_parameters;
  ON_ProgressReporter::ReportProgress( local_limit_mesh_parameters.m_progress_reporter, 0.0 );

  if (destination_mesh)
    destination_mesh->Destroy();

  const ON_SubDLevel& active_level = ActiveLevel();
  if (active_level.IsEmpty())
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (ON_SubD::SubDType::Unset == active_level.m_subdivision_type )
    const_cast<ON_SubD*>(this)->SetSubDType(ON_SubD::SubDType::QuadCatmullClark);

  const unsigned int fragment_count = LimitSurfaceMeshFragmentCount();
  if ( fragment_count <= 0 )
    return ON_SUBD_RETURN_ERROR(nullptr);

  const ON_SubD::SubDType subd_type = active_level.m_subdivision_type;

  if (ON_SubD::SubDType::QuadCatmullClark != subd_type)
  {
    // TODO - support tri subd after quad stuff is finished.
    return ON_SUBD_RETURN_ERROR(nullptr);
  }

  if (0 == local_limit_mesh_parameters.m_display_density && fragment_count > FaceCount())
    local_limit_mesh_parameters.m_display_density = 1;

  //////////////////////////////////////////////////////////////////
  //
  // Set:
  //  vertex_count = number of points needed to calculate the mesh
  //  face_count = number of quad faces needed to calculate the mesh
  //

  unsigned int vertex_count = 0;
  unsigned int face_count = 0;

  unsigned int tri_count = 0; // tri count
  unsigned int quad_count = 0; // quad count
  unsigned int ngon_edge_sum = 0; // sum of edge counts for faces with m_edge_count >= 5.
  ON_SubDFaceIterator fit(*this);
  for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
  {
    if (3 == f->m_edge_count)
      tri_count++;
    if (4 == f->m_edge_count)
      quad_count++;
    else
      ngon_edge_sum += f->m_edge_count;
  }

  if (ON_SubD::SubDType::QuadCatmullClark == subd_type)
  {
    const unsigned int m0 = 1 << local_limit_mesh_parameters.m_display_density;
    const unsigned int m1 = (m0 > 1) ? m0 / 2 : 1;
    if (ngon_edge_sum > 0 || tri_count > 0)
    {
      ngon_edge_sum += 4 * quad_count + 3 * tri_count;
      quad_count = 0;
      tri_count = 0;
    }

    vertex_count = quad_count*(m0 + 1)*(m0 + 1) + ngon_edge_sum*(m1 + 1)*(m1 + 1);
    face_count = quad_count*m0*m0 + ngon_edge_sum*m1*m1;
  }
  else if (ON_SubD::SubDType::TriLoopWarren == subd_type)
  {
    // TODO ADD TRI SUPPORT
    return ON_SUBD_RETURN_ERROR(nullptr);
  }
  
  if (vertex_count < 4 || face_count < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  std::unique_ptr< ON_Mesh > up;  
  ON_Mesh* mesh = nullptr;
  if (nullptr != destination_mesh)
    mesh = destination_mesh;
  else
  {
    up = std::make_unique< ON_Mesh >();
    mesh = up.get();
  }

  if ( nullptr == mesh)
    return ON_SUBD_RETURN_ERROR(nullptr);

  // mesh vertices

  // mesh face group ids
 
  GetLimitSurfaceMesh_context context;

  ON_3dPointArray& D = mesh->DoublePrecisionVertices();
  context.m_point_capacity = vertex_count;
  context.m_points = (double*)D.Reserve(vertex_count);
  context.m_point_stride = 3;

  ON_SimpleArray< ON_3dVector > mesh_N;
  context.m_normal_capacity = vertex_count;
  context.m_normals = (double*)mesh_N.Reserve(vertex_count);
  context.m_normal_stride = 3;

  context.m_quad_capacity = face_count;
  context.m_quads = (unsigned int*)mesh->m_F.Reserve(face_count);
  context.m_quad_stride = 4;

  ON_SimpleArray< ON__UINT_PTR > quad_group_ids_buffer;
  ON__UINT_PTR* quad_group_ids = quad_group_ids_buffer.Reserve(face_count);
  context.m_quad_group_ids = quad_group_ids;
  context.m_quad_group_id_stride = 1;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  context.m_bUseMultipleThreads = local_limit_mesh_parameters.m_bUseMultipleThreads;
#endif

  if (ON_SubD::SubDType::QuadCatmullClark == subd_type)
  {
    //ON_Interval progress_limits(0.1,0.9);
    const ON_Interval progress_limits = local_limit_mesh_parameters.m_progress_reporter_interval;
    if (progress_limits.IsIncreasing())
    {
      local_limit_mesh_parameters.m_progress_reporter_interval.Set(
        progress_limits.ParameterAt(0.0),
        progress_limits.ParameterAt(0.5)
        );
    }
    unsigned int quad_fragment_count = GetQuadLimitSurfaceMeshFragmentsHelper(
      fit,
      local_limit_mesh_parameters,
      (ON__UINT_PTR)&context,
      GetLimitSurfaceMesh_callback
      );
    local_limit_mesh_parameters.m_progress_reporter_interval = progress_limits;
    if ( 0 == quad_fragment_count )
      return ON_SUBD_RETURN_ERROR(nullptr);
  }
  else if (ON_SubD::SubDType::TriLoopWarren == subd_type)
  {
    // todo - add tri support
    return ON_SUBD_RETURN_ERROR(nullptr);
  }
  else
  {
    return ON_SUBD_RETURN_ERROR(nullptr);
  }
  
  const unsigned int mesh_point_count = (unsigned int)context.m_point_count;
  const unsigned int mesh_face_count = (unsigned int)context.m_quad_count;
  
  if (mesh_point_count < 3 || mesh_face_count < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  // Set face count
  mesh->m_F.SetCount(mesh_face_count);

  // Set vertex counts
  D.SetCount(mesh_point_count);
  quad_group_ids_buffer.SetCount(mesh_face_count);
  mesh->UpdateSinglePrecisionVertices();

  // copy ON_3dVector vertex normals in mesh_N[] to ON_3fVector normals in mesh->m_N[] 
  mesh_N.SetCount(mesh_point_count);
  const ON_3dVector* dN = mesh_N.Array();
  const ON_3dVector* dN1  = dN + mesh_point_count;
  ON_3fVector* fN = mesh->m_N.Reserve(mesh_point_count);
  mesh->m_N.SetCount(mesh_point_count);
  while (dN < dN1)
    *fN++ = *dN++;

  // set bounding boxes
  mesh->BoundingBox();

  // group all mesh faces that came from a subd control net face into an ngon.
  for (;;)
  {
    const ON_3dPointListRef mesh_vertex_list(mesh);
    const ON_MeshFaceList mesh_face_list(mesh);
    ON_MeshVertexFaceMap vf_map;
    if (!vf_map.SetFromFaceList(mesh_vertex_list.PointCount(), mesh_face_list, false))
      break;
    const unsigned int *const* vertex_face_map = vf_map.VertexFaceMap();
    if (nullptr == vertex_face_map)
      break;

    ON_SimpleArray< unsigned int > mesh_face_index_buffer;
    mesh_face_index_buffer.Reserve(mesh_face_count);
    mesh_face_index_buffer.SetCount(mesh_face_count);
    unsigned int* mesh_face_index = mesh_face_index_buffer.Array();

    // mesh_face_index[] = permutation of (0,1,...,quad_count-1)
    //   such that the mesh faces that came from the same level zero subd face
    //   are grouped together.
    ON_Sort(ON::sort_algorithm::quick_sort, mesh_face_index, quad_group_ids, mesh_face_count, sizeof(quad_group_ids[0]), CompareQuadGroupId);

    ON_SimpleArray< unsigned int> ngon_fi(256);
    ON_SimpleArray< unsigned int> ngon_vi;
    ON_ProgressStepCounter counter = ON_ProgressStepCounter::Create(
      local_limit_mesh_parameters.m_progress_reporter,
      mesh_face_count,
      0.5, 1.0,
      50
      );
    for (unsigned int i = 0; i < mesh_face_count; /*empty increment*/)
    {
      // get list of faces in the mesh ngon
      ngon_fi.SetCount(0);
      ngon_fi.Append(mesh_face_index[i]);
      const ON__UINT_PTR ngon_group_id = quad_group_ids[mesh_face_index[i]];
      for (i++; i < mesh_face_count && ngon_group_id == quad_group_ids[mesh_face_index[i]]; i++)
      {
        ngon_fi.Append(mesh_face_index[i]);
      }

      counter.IncrementStep();
      if (ngon_fi.Count() < 2)
        continue;

      // create ngon
      ngon_vi.SetCount(0);
      if (ON_MeshNgon::FindNgonOuterBoundary(mesh_vertex_list, mesh_face_list, vertex_face_map, ngon_fi.UnsignedCount(), ngon_fi.Array(), ngon_vi) >= 3)
      {
        mesh->AddNgon(ngon_vi.UnsignedCount(), ngon_vi.Array(), ngon_fi.UnsignedCount(), ngon_fi.Array());
      }
    }
    break;
  }

  // success
  ON_ProgressReporter::ReportProgress(local_limit_mesh_parameters.m_progress_reporter,1.0);
  up.release();
  return mesh;
}

class VertexToDuplicate
{
public:
  const ON_SubDVertex* m_vertex = nullptr;
  const ON_SubDFace* m_face = nullptr;
  unsigned int m_mesh_V_index = 0;
  unsigned int m_mesh_F_index = 0;

  static int CompareVertexId(const class VertexToDuplicate* a, const class VertexToDuplicate*);
  static int CompareVertexAndFaceIds(const class VertexToDuplicate* a, const class VertexToDuplicate*);

  static bool NeedsDuplicated(
    const ON_SubDVertex* vertex
    );
};

int VertexToDuplicate::CompareVertexId(const class VertexToDuplicate* a, const class VertexToDuplicate* b)
{
  if ( a == b )
    return 0;
  if ( nullptr == a )
    return -1;
  if ( nullptr == b )
    return 1;

  unsigned int a_id = a->m_vertex ? a->m_vertex->m_id : 0;
  unsigned int b_id = b->m_vertex ? b->m_vertex->m_id : 0;
  if ( a_id < b_id )
    return -1;
  if ( a_id > b_id )
    return 1;

  return 0;
}
int VertexToDuplicate::CompareVertexAndFaceIds(const class VertexToDuplicate* a, const class VertexToDuplicate* b)
{
  if ( a == b )
    return 0;
  int rc = VertexToDuplicate::CompareVertexId(a,b);
  if (0 != rc)
    return rc;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;
  unsigned int a_id = a->m_face ? a->m_face->m_id : 0;
  unsigned int b_id = b->m_face ? b->m_face->m_id : 0;
  if (a_id < b_id)
    return -1;
  if (a_id > b_id)
    return 1;
  return 0;
}

bool VertexToDuplicate::NeedsDuplicated(
  const ON_SubDVertex* vertex
  )
{
  if ( nullptr == vertex || vertex->m_face_count <= 0 || vertex->m_edge_count < 2 || nullptr == vertex->m_edges )
    return false;
  if (vertex->IsSmooth())
    return false;
  const unsigned int edge_count = vertex->m_edge_count;
  for (unsigned int vei = 0; vei < edge_count; vei++)
  {
    const ON_SubDEdge* edge = vertex->Edge(vei);
    if ( nullptr != edge && false == edge->IsSmooth(true) && edge->m_face_count > 1 )
      return true;
  }
  return false;
}

static bool ChangeMeshFaceIndex(
  unsigned int mesh_V_index0,
  unsigned int mesh_F_count,
  ON_Mesh* mesh,
  VertexToDuplicate& dup,
  ON_SimpleArray<VertexToDuplicate>& dups_sub_array
  )
{
  int k = dups_sub_array.BinarySearch(&dup,VertexToDuplicate::CompareVertexAndFaceIds);
  if (k < 0)
  {
    // error.  terminate creation of dups.
    ON_SubDIncrementErrorCount();
    return false;
  }

  VertexToDuplicate* dupk = dups_sub_array.Array() + k;

  if (mesh_V_index0 != dup.m_mesh_V_index)
  {
    if (mesh_V_index0 == dupk->m_mesh_V_index && dupk->m_mesh_F_index < mesh_F_count)
    {
      unsigned int* fvi = (unsigned int*)(mesh->m_F[dupk->m_mesh_F_index].vi);
      if (fvi[0] == mesh_V_index0)
        fvi[0] = dup.m_mesh_V_index;
      if (fvi[1] == mesh_V_index0)
        fvi[1] = dup.m_mesh_V_index;
      if (fvi[2] == mesh_V_index0)
        fvi[2] = dup.m_mesh_V_index;
      if (fvi[3] == mesh_V_index0)
        fvi[3] = dup.m_mesh_V_index;
    }
  }
  dupk->m_mesh_V_index = ON_UNSET_UINT_INDEX;
  dupk->m_mesh_F_index = ON_UNSET_UINT_INDEX;
  return true;
}

static bool DuplicateVerticesAtCreases(
  ON_Mesh* mesh,
  ON_3dPointArray& D,
  ON_SimpleArray<VertexToDuplicate>& dups_array
  )
{
  const unsigned int mesh_F_count = mesh->m_F.UnsignedCount();
  const unsigned int mesh_D_count0 = D.UnsignedCount();

  const unsigned int dups_count = dups_array.UnsignedCount();
  if (dups_count <= 1)
    return true;

  dups_array.QuickSort(VertexToDuplicate::CompareVertexAndFaceIds);
  ON_SimpleArray<VertexToDuplicate> dups_sub_array; // for searching
  VertexToDuplicate* dups = dups_array;
  VertexToDuplicate dup;
  unsigned int i1 = 0;
  for (unsigned int i0 = i1; i0 < dups_count; i0 = i1)
  {
    dup = dups[i0];
    if (nullptr == dup.m_vertex)
    {
      ON_SubDIncrementErrorCount();
      return false;
    }
    for (i1 = i0 + 1; i1 < dups_count; i1++)
    {
      int rc = VertexToDuplicate::CompareVertexId(&dup,dups+i1);
      if (rc < 0)
        break;
      if ( 0 != rc
          || dup.m_vertex != dups[i1].m_vertex 
          || dup.m_mesh_V_index != dups[i1].m_mesh_V_index 
          || dup.m_mesh_V_index >= mesh_D_count0
          )
      {
        ON_SubDIncrementErrorCount();
        return false;
      }
    }

    if ( i1 == i0+1)
      continue;

    const unsigned int mesh_V_index0 = dup.m_mesh_V_index;
    const ON_3dPoint P = D[mesh_V_index0];
    dups_sub_array.SetArray(dups+i0,i1-i0,0);
    ON_SubDSectorIterator sit;
    unsigned int sector_count = 0;
    bool bDupError = false;
    for (unsigned int i = i0; i < i1 && false == bDupError; i++)
    {
      if (dups[i].m_mesh_V_index >= mesh_D_count0 || dups[i].m_mesh_F_index >= mesh_F_count)
      {
        if (sector_count > 0
          && ON_UNSET_UINT_INDEX == dups[i].m_mesh_V_index
          && ON_UNSET_UINT_INDEX == dups[i].m_mesh_F_index
          )
        {
          // this dup[i] was part of a previously processed sector.
          continue;
        }
        // error.  terminate creation of dups.
        ON_SubDIncrementErrorCount();
        bDupError = true;
        break;
      }

      if (nullptr == sit.Initialize(dups[i].m_face, 0, dup.m_vertex))
      {
        // error.  terminate creation of dups.
        ON_SubDIncrementErrorCount();
        bDupError = true;
        break;
      }
      if ( nullptr == sit.IncrementToCrease(-1) )
      {
        // error.  terminate creation of dups.
        ON_SubDIncrementErrorCount();
        bDupError = true;
        break;
      }

      if (dup.m_vertex->IsDart())
      {
        const ON_SubDEdge* edge = sit.CurrentEdge(0);
        if (nullptr == edge || false == edge->IsCrease(false) || 2 != edge->m_face_count)
        {
          ON_SubDIncrementErrorCount();
          bDupError = true;
          break;
        }
        for (unsigned int efi = 0; efi < 2; efi++)
        {
          dup.m_face = edge->Face(efi);
          dup.m_mesh_V_index = D.UnsignedCount();
          D.Append(P);
          if (false == ChangeMeshFaceIndex(mesh_V_index0, mesh_F_count, mesh, dup, dups_sub_array))
          {
            ON_SubDIncrementErrorCount();
            bDupError = true;
            break;
          }
        }

        sit.NextFace(true);
      }

      sector_count++;
      if (sector_count > 1)
      {
        dup.m_mesh_V_index = D.UnsignedCount();
        D.Append(P);
      }
      else
      {
        dup.m_mesh_V_index = mesh_V_index0;
      }

      for (dup.m_face = sit.CurrentFace(); nullptr != dup.m_face && false == bDupError; dup.m_face = sit.NextFace(true))
      {
        if (false == ChangeMeshFaceIndex(mesh_V_index0, mesh_F_count, mesh, dup, dups_sub_array))
        {
          ON_SubDIncrementErrorCount();
          bDupError = true;
          break;
        }
      }
      if (bDupError)
        break;

    }
    dups_sub_array.SetCapacity(0);
    if (bDupError)
      return false;
  }
  return true;
}



ON_Mesh* ON_SubD::GetControlNetMesh(
  ON_Mesh* destination_mesh
  ) const
{
  if (destination_mesh)
    destination_mesh->Destroy();

  const ON_SubDLevel& level = ActiveLevel();
  if (level.IsEmpty())
    return ON_SUBD_RETURN_ERROR(nullptr);

  VertexToDuplicate dup;
  ON_SimpleArray<VertexToDuplicate> dups_array;

  const ON_SubDimple* subdimple = SubDimple();
  if ( nullptr == subdimple)
    return nullptr;

  const unsigned int subd_vertex_count = level.m_vertex_count;

  unsigned int mesh_ngon_count = 0;
  unsigned int mesh_face_count = 0;
  unsigned int max_ngon_Vcount = 0;
  for (const ON_SubDFace* face = level.m_face[0]; nullptr != face; face = face->m_next_face)
  {
    if ( face->m_edge_count < 2 )
      continue;
    if (face->m_edge_count <= 4)
    {
      mesh_face_count++;
      continue;
    }
    mesh_ngon_count++;
    mesh_face_count += face->m_edge_count;
    if ( max_ngon_Vcount < face->m_edge_count )
      max_ngon_Vcount = face->m_edge_count;
  }

  if (subd_vertex_count < 4 || mesh_face_count < 1 )
    return ON_SUBD_RETURN_ERROR(nullptr);

  std::unique_ptr< ON_Mesh > up;  
  ON_Mesh* mesh = nullptr;
  if (nullptr != destination_mesh)
    mesh = destination_mesh;
  else
  {
    up = std::make_unique< ON_Mesh >();
    mesh = up.get();
  }

  ON_3dPointArray& D = mesh->DoublePrecisionVertices();
  D.Reserve(subd_vertex_count+mesh_ngon_count);
  D.SetCount(0);

  mesh->m_F.Reserve(mesh_face_count);
  mesh->m_F.SetCount(0);

  ON_SimpleArray< ON_2udex > ngon_spans(mesh_ngon_count);

  bool rc = false;
  for (;;)
  {

    unsigned int archive_id_partition[4] = {};
    level.SetArchiveId(archive_id_partition);
    if (archive_id_partition[1] - archive_id_partition[0] != subd_vertex_count)
      break;

    for (const ON_SubDVertex* vertex = level.m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      unsigned int vi = vertex->ArchiveId();
      if (vi < 1 || vi > subd_vertex_count)
        break;
      if (D.UnsignedCount()+1 != vi)
        break;
      D.AppendNew() = vertex->m_P;
    }

    if (D.UnsignedCount() != subd_vertex_count)
      break;

    ngon_spans.Reserve(mesh_ngon_count);
    unsigned int max_ngon_face_count = 0;
    mesh_face_count = 0;
    for (const ON_SubDFace* face = level.m_face[0]; nullptr != face; face = face->m_next_face)
    {
      ON_MeshFace meshf = {};

      if (face->m_edge_count <= 4)
      {
        if (face->m_edge_count < 3)
          continue;

        for (unsigned short fvi = 0; fvi < face->m_edge_count; fvi++)
        {
          const ON_SubDVertex* vertex = face->Vertex(fvi);
          meshf.vi[fvi] = (int)((nullptr != vertex) ? vertex->ArchiveId() : 0U);
          if (meshf.vi[fvi] < 1 || meshf.vi[fvi] > (int)subd_vertex_count)
          {
            meshf.vi[0] = -1;
            break;
          }
          meshf.vi[fvi]--;
          if (VertexToDuplicate::NeedsDuplicated(vertex))
          {
            dup.m_vertex = vertex;
            dup.m_face = face;
            dup.m_mesh_F_index = mesh->m_F.UnsignedCount();
            dup.m_mesh_V_index = meshf.vi[fvi];
            dups_array.Append(dup);
          }
        }
        if (-1 == meshf.vi[0] )
          continue;
        if ( 3 == face->m_edge_count)
          meshf.vi[3] = meshf.vi[2];
        mesh->m_F.Append(meshf);
        continue;
      }
      else
      {
        ON_3dPoint center_point;
        if (false == face->GetSubdivisionPoint(ON_SubD::SubDType::QuadCatmullClark, true, center_point))
          continue;

        ON_2udex ngon_span = { mesh->m_F.UnsignedCount(), 0 };

        const unsigned int dup_count0 = dups_array.UnsignedCount();

        const unsigned int Dcount0 = D.UnsignedCount();
        const unsigned int Fcount0 = mesh->m_F.UnsignedCount();
        meshf.vi[2] = (int)Dcount0;
        meshf.vi[3] = meshf.vi[2];

        const ON_SubDVertex* vertex = face->Vertex(0);
        meshf.vi[1] = (nullptr != vertex) ? vertex->ArchiveId() : 0;
        if (meshf.vi[1] < 1 || meshf.vi[1] >= (int)subd_vertex_count)
          continue;
        meshf.vi[1]--;

        if (VertexToDuplicate::NeedsDuplicated(vertex))
        {
          dup.m_vertex = vertex;
          dup.m_face = face;
          dup.m_mesh_F_index = mesh->m_F.UnsignedCount();
          dup.m_mesh_V_index = meshf.vi[1];
          dups_array.Append(dup);
        }

        D.Append(center_point);

        for (unsigned short fvi = 1; fvi <= face->m_edge_count; fvi++)
        {
          meshf.vi[0] = meshf.vi[1];
          vertex = face->Vertex(fvi % face->m_edge_count);
          meshf.vi[1] = (int)((nullptr != vertex) ? vertex->ArchiveId() : 0U);
          if (meshf.vi[1] < 1 || meshf.vi[1] > (int)subd_vertex_count)
          {
            meshf.vi[0] = -1;
            break;
          }
          meshf.vi[1]--;

          if (VertexToDuplicate::NeedsDuplicated(vertex))
          {
            dup.m_vertex = vertex;
            dup.m_face = face;
            dup.m_mesh_F_index = mesh->m_F.UnsignedCount();
            dup.m_mesh_V_index = meshf.vi[1];
            dups_array.Append(dup);
          }

          mesh->m_F.Append(meshf);
        }
        ngon_span.j = mesh->m_F.UnsignedCount();

        unsigned int ngon_face_count = ngon_span.j - ngon_span.i;
        if (-1 == meshf.vi[0] || ngon_face_count < 3)
        {
          D.SetCount(Dcount0);
          mesh->m_F.SetCount(Fcount0);
          dups_array.SetCount(dup_count0);
          continue;
        }
        ngon_span.j = mesh->m_F.UnsignedCount();
        if (ngon_face_count >= 3)
        {
          ngon_spans.Append(ngon_span);
          if ( ngon_face_count > max_ngon_face_count)
            max_ngon_face_count = ngon_face_count;
        }
      }
    }

    if (mesh->m_F.UnsignedCount() <= 0)
      break;

    rc = true;
    break;
  }

  level.ClearArchiveId();
  if (false == rc )
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (D.UnsignedCount() < 3 || mesh->m_F.UnsignedCount() < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  DuplicateVerticesAtCreases(mesh,D,dups_array);
  mesh->UpdateSinglePrecisionVertices();
  mesh->ComputeFaceNormals();
  mesh->ComputeVertexNormals();
  mesh->BoundingBox();

  // group all mesh faces that came from the same level zero subd face into an ngon.
  if (ngon_spans.UnsignedCount() > 0 && max_ngon_Vcount >= 3)
  {
    ON_SimpleArray< unsigned int> ngon_buffer;
    unsigned int* ngon_fi = ngon_buffer.Reserve(2*max_ngon_Vcount);
    unsigned int* ngon_vi = ngon_fi + max_ngon_Vcount;
    for (unsigned int ngon_dex = 0; ngon_dex < ngon_spans.UnsignedCount(); ngon_dex++ )
    {
      ON_2udex ngon_span = ngon_spans[ngon_dex];
      unsigned int Fcount = ngon_span.j-ngon_span.i;
      if ( Fcount < 3)
        continue;

      ngon_fi[0] = ngon_span.i;
      ngon_fi[0] = (unsigned int)mesh->m_F[ngon_fi[0]].vi[0];

      unsigned int ngon_Vcount = 0;
      for (unsigned int i = ngon_span.i; i < ngon_span.j; i++)
      {
        ngon_fi[ngon_Vcount] = i;
        ngon_vi[ngon_Vcount] = (unsigned int)(mesh->m_F[i].vi[0]);
        ngon_Vcount++;
      }
      mesh->AddNgon(ngon_Vcount, ngon_vi, ngon_Vcount, ngon_fi );
    }
  }

  up.release();
  return mesh;
}

double* ON_SubDQuadFaceMesher::Internal_Buffer(size_t buffer_capacity)
{
  if (buffer_capacity < m__buffer_capacity)
    return m__buffer;
  if (nullptr != m__buffer)
  {
    delete[] m__buffer;
    m__buffer = nullptr;
  }
  m__buffer_capacity = 0;
  m__buffer = new(std::nothrow) double[buffer_capacity];
  if (nullptr != m__buffer)
    m__buffer_capacity = buffer_capacity;
  return m__buffer;
}

void ON_SubDQuadFaceMesher::SetDestinationInitialize(
  ON_SubDQuadFaceMesher::Output output
  )
{
  // Reuse workspaces
  ReturnAllFixedSizeHeaps();

  m_output = output;
  m_display_density = 0;
  m_count = 0;
  m_capacity = 0;
  m_point_stride0 = 0;
  m_point_stride1 = 0;
  m_points = nullptr;
  m_normal_stride0 = 0;
  m_normal_stride1 = 0;
  m_normals = nullptr;
  m_patcher = nullptr;
}

bool ON_SubDQuadFaceMesher::SetDestinationToLocalMeshBuffer(
  unsigned int mesh_density
  )
{
  const unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  
  const size_t point_count = (count > 0) ? ((count + 1)*(count + 1)) : 0;

  // initialize this to make another mesh.
  SetDestinationInitialize(ON_SubDQuadFaceMesher::Output::mesh);

  double* buffer = Internal_Buffer(6 * point_count);
  if (point_count > 0 && nullptr == buffer )
  {
    return ON_SUBD_RETURN_ERROR(false);
  }
  
  if (0 == count && mesh_density > 0)
    return ON_SUBD_RETURN_ERROR(false);

  m_points = buffer;
  m_normals = buffer + 3*point_count;
  m_point_stride0 = 3;
  m_point_stride1 = (count+1)*m_point_stride0;
  m_normal_stride0 = m_point_stride0;
  m_normal_stride1 = m_point_stride1;

  m_display_density = mesh_density;
  m_count = 0;
  m_capacity = count;

  return (count == m_count);
}

bool ON_SubDQuadFaceMesher::SetDestinationToMeshFragment(
  unsigned int mesh_density,
  class ON_SubDLimitMeshFragment& fragment
  )
{
  // initialize this
  SetDestinationInitialize(ON_SubDQuadFaceMesher::Output::mesh);
  
  if ( nullptr == fragment.m_P || nullptr == fragment.m_N)
    return ON_SUBD_RETURN_ERROR(false);
  
  const unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  const unsigned int point_count = (count + 1)*(count + 1);
  if ( point_count > fragment.m_P_capacity )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int quad_count = count*count;
  if (fragment.m_grid.m_F_count != quad_count || 0 != fragment.m_grid.m_F_level_of_detail 
    || nullptr == fragment.m_grid.m_F || fragment.m_grid.m_F_stride != 4)
  {
    fragment.m_grid = ON_SubDLimitMeshFragmentGrid::Quads(count,0);
    if ( nullptr == fragment.m_grid.m_F)
      return ON_SUBD_RETURN_ERROR(false);
  }

  m_point_stride0 = fragment.m_P_stride;
  m_point_stride1 = (count+1)*m_point_stride0;
  m_points = fragment.m_P;

  m_normal_stride0 = fragment.m_N_stride;
  m_normal_stride1 = (count+1)*m_normal_stride0;
  m_normals = fragment.m_N;

  m_display_density = mesh_density;
  m_count = 0;
  m_capacity = count;

  return true;
}

bool ON_SubDQuadFaceMesher::SetDestinationToPatchFragment(
  class ON_SubDQuadFacePatcher& patcher
  )
{
  // initialize this
  SetDestinationInitialize(ON_SubDQuadFaceMesher::Output::patches);
  m_patcher = &patcher;
  m_display_density = patcher.m_display_density;
  
  return true;
}


bool ON_SubDQuadFaceMesher::UnsetMeshPoints()
{
  if (ON_SubDQuadFaceMesher::Output::mesh != m_output)
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == m_points || 0 == m_count)
    return ON_SUBD_RETURN_ERROR(false);

  double* p1end = m_points + (m_count + 1)*m_point_stride1;
  for (double* p1 = m_points; p1 < p1end; p1 += m_point_stride1)
  {
    double* pend = p1 + (m_count + 1)*m_point_stride0;
    for (double* p = p1; p < pend; p += m_point_stride0)
      *p = ON_UNSET_VALUE;
  }

  return true;
}

void ON_SubDQuadFaceMesher::Get6xCubicBasis(
  double t,
  double b6[4]
  )
{
  const double knot[6] = {-2.0,-1.0,0.0,1.0,2.0,3.0};
  double N[16];
  ON_EvaluateNurbsBasis( 4, knot, t, N );
  b6[0] = 6.0*N[0];
  b6[1] = 6.0*N[1];
  b6[2] = 6.0*N[2];
  b6[3] = 6.0*N[3];

  // TODO - hard code polynomial formula
  //const double tt = t*t;
  //const double s = 1.0 - t;
  //const double ss = s*s;
 
  //b6[0] = (t*tt);
  //b6[1] = (1.0 + 3.0*(s*tt));
  //b6[2] = (1.0 + 3.0*(t*ss));
  //b6[3] = (s*ss);
}


void ON_SubDQuadFaceMesher::GetCubicBasisDerivative(
  double t,
  double d[4]
  )
{
  const double knot[6] = {-2.0,-1.0,0.0,1.0,2.0,3.0};
  double N[16];
  ON_EvaluateNurbsBasis( 4, knot, t, N );
  ON_EvaluateNurbsBasisDerivatives( 4, knot, 1, N );
  d[0] = N[4];
  d[1] = N[5];
  d[2] = N[6];
  d[3] = N[7];

  // TODO - hard code polynomial formula
  //const double tt = t*t;
  //const double s = 1.0 - t;
  //const double ss = s*s;

  //d[0] = tt;
  //d[1] =  (0.5 + t) - 1.5*tt;
  //d[2] = -(0.5 + s) + 1.5*ss;
  //d[3] = -ss;
}

bool ON_SubDQuadFaceMesher::Internal_EvaluateSurfaceNormalBackup1(
  double s,
  double t,
  unsigned int count,
  unsigned int i,
  unsigned int j,
  double* N
) const
{
  const double knot[6] = {-2.0,-1.0,0.0,1.0,2.0,3.0};
  ON_3dVector v[6]; // P, Du, Dv, Duu, Duv, Dvv
  bool bHaveNormal = ON_EvaluateNurbsSurfaceSpan(
        3,    // dim
        0,    // is_rat
        4, 4, // order, order
        knot, // knot0[] array of (2*order0-2) doubles
        knot, // knot1[] array of (2*order1-2) doubles
        12,3, // cv_stride0, cv_stride1
        &m_srf_cv[0][0][0],// cv at "lower left" of bispan
        2,    // der_count,                      // number of derivatives to compute (>=0)
        s,t,  // evaluation parameters
        3,    // v_stride
        &v[0][0] // P, Du, Dv, Duu, Duv, Dvv returned here
        );

  if (bHaveNormal)
  {
    // P[], Du, Dv and V[0], V[1], V[2] should be nearly identical
    int limit_dir = 0;
    if (0 == i)
    {
      limit_dir = (j < count) ? 1 : 2;
    }
    else if (count == i)
    {
      limit_dir = (j < count) ? 4 : 3;
    }
    if (0 == j)
    {
      limit_dir = 1;
    }
    else if (count == j)
    {
      limit_dir = 4;
    }
    bHaveNormal = ON_EvNormal(limit_dir, v[1], v[2], v[3], v[4], v[5], *((ON_3dVector*)N));
  }

  return bHaveNormal;
}

bool ON_SubDQuadFaceMesher::Internal_EvaluateSurfaceNormalBackup2(
  const double* P00,
  unsigned int count,
  unsigned int i,
  unsigned int j,
  double* N
) const
{
  if  (false == ((const ON_3dPoint*)(P00 + ((i * m_point_stride0) + (j * m_point_stride1))))->IsValid() )
    return false;

  ON_2dex corners_dex[4];
  ON_3dPoint corners[4];
  if ( i <= 0 || i >= count || j <= 0 || j >= count )
  {
    corners_dex[0].i = (i > 0) ? (i - 1) : 0;
    corners_dex[0].j = (j > 0) ? (j - 1) : 0;
    corners_dex[2].i = (i < count) ? (i + 1) : count;
    corners_dex[2].j = (j < count) ? (j + 1) : count;
    corners_dex[1].i = corners_dex[2].i;
    corners_dex[1].j = corners_dex[0].j;
    corners_dex[3].i = corners_dex[0].i;
    corners_dex[3].j = corners_dex[2].j;
  }
  else
  {
    corners_dex[0].i = (i - 1);
    corners_dex[0].j = j;
    corners_dex[1].i = i;
    corners_dex[1].j = (j - 1);
    corners_dex[2].i = (i + 1);
    corners_dex[2].j = j;
    corners_dex[3].i = i;
    corners_dex[3].j = (j + 1);
  }

  for (int k = 0; k < 4; k++)
  {
    corners[k] = P00 + ((corners_dex[k].i * m_point_stride0) + (corners_dex[k].j * m_point_stride1));
    if (false == corners[k].IsValid())
      return false;
  }
  const ON_3dVector A (corners[2] - corners[0]);
  const ON_3dVector B (corners[3] - corners[1]);
  *((ON_3dVector*)N) = ON_CrossProduct(A, B);
  bool bHaveNormal = ((ON_3dVector*)N)->Unitize();
  return bHaveNormal;
}

bool ON_SubDQuadFaceMesher::EvaluateSurface(
  unsigned int count,
  unsigned int point_i0,
  unsigned int point_j0
  ) const
{
  double iso_cv[4][3];
  double b[4], Du[3], Dv[3], s, t;
  unsigned int i, j;

  if ( nullptr == m_points || m_point_stride0 < 3 || m_point_stride1 < 3)
    return ON_SUBD_RETURN_ERROR(false);

  // verify that count is a power of 2
  for ( i = count; i > 1; i /= 2)
  {
    if (0 != (i%2))
      return ON_SUBD_RETURN_ERROR(false); // count != 2^n
  }
  if (1 != i)
    return ON_SUBD_RETURN_ERROR(false); // count != 2^n
  
  double* P00 = m_points + point_i0*m_point_stride0 + point_j0*m_point_stride1;

  const size_t normal_stride[2] = {(nullptr != m_normals)?m_normal_stride0:0,(nullptr != m_normals)?m_normal_stride1:0};
  double* N00 = m_normals + point_i0*normal_stride[0] + point_j0*normal_stride[1];

  const double delta_t = 1.0 / ((double)count);

  if (nullptr != N00)
  {
    for (i = 0; i <= count; i++)
    {

      // Set  iso_cv[][] = cvs for isocurve(s) = srf(i*delta_t,s)
      s = (i < count) ? (i*delta_t) : 1.0;
      ON_SubDQuadFaceMesher::Get6xCubicBasis(s, b);
      const double* srf = &m_srf_cv[0][0][0];
      iso_cv[0][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[0][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[0][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[1][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[1][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[1][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[2][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[2][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[2][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[3][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[3][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[3][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;

      double* P = P00 + (i * m_point_stride0);
      double* N = N00 + (i * normal_stride[0]);
      for (j = 0; j <= count; j++)
      {
        if (ON_UNSET_VALUE == P[0])
        {
          // evaluate Dv(i*delta_t,j*delta_t) and save it in N[0,1,2]
          t = (j < count) ? (j*delta_t) : 1.0;
          ON_SubDQuadFaceMesher::GetCubicBasisDerivative(t, b);
          N[0] = ((b[0] * iso_cv[0][0] + b[3] * iso_cv[3][0]) + (b[1] * iso_cv[1][0] + b[2] * iso_cv[2][0]));
          N[1] = ((b[0] * iso_cv[0][1] + b[3] * iso_cv[3][1]) + (b[1] * iso_cv[1][1] + b[2] * iso_cv[2][1]));
          N[2] = ((b[0] * iso_cv[0][2] + b[3] * iso_cv[3][2]) + (b[1] * iso_cv[1][2] + b[2] * iso_cv[2][2]));
        }
        P += m_point_stride1;
        N += normal_stride[1];
      }
    }
  }

  bool bUseSurfaceNormalBackup2 = false;

  for (j = 0; j <= count; j++)
  {
    // Set  iso_cv[][] = cvs for isocurve(s) = srf(s,j*delta_t)
    t = (j < count) ? (j*delta_t) : 1.0;
    ON_SubDQuadFaceMesher::Get6xCubicBasis(t, b);
    const double* srf = &m_srf_cv[0][0][0];
    // The ((first + fourth) + (second + third)) insures the values of the 
    // mesh points and normals will be exactly symmetric when the control points are exactly
    // symmetric.
    iso_cv[0][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[0][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[0][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf = &m_srf_cv[1][0][0];
    iso_cv[1][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[1][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[1][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf = &m_srf_cv[2][0][0];
    iso_cv[2][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[2][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[2][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf = &m_srf_cv[3][0][0];
    iso_cv[3][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[3][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[3][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;

    double* P = P00 + (j * m_point_stride1);
    double* N = N00 + (j * normal_stride[1]);
    for (i = 0; i <= count; i++)
    {
      if (ON_UNSET_VALUE == P[0])
      {
        s = (i < count) ? (i*delta_t) : 1.0;
        ON_SubDQuadFaceMesher::Get6xCubicBasis(s, b);
        // P = srf(i*delta_t,j*delta_t)
        P[0] = ((b[0] * iso_cv[0][0] + b[3] * iso_cv[3][0]) + (b[1] * iso_cv[1][0] + b[2] * iso_cv[2][0])) / 6.0;
        P[1] = ((b[0] * iso_cv[0][1] + b[3] * iso_cv[3][1]) + (b[1] * iso_cv[1][1] + b[2] * iso_cv[2][1])) / 6.0;
        P[2] = ((b[0] * iso_cv[0][2] + b[3] * iso_cv[3][2]) + (b[1] * iso_cv[1][2] + b[2] * iso_cv[2][2])) / 6.0;

        if (nullptr != N)
        {
          // Above, I saved Dv in N[];
          Dv[0] = N[0];
          Dv[1] = N[1];
          Dv[2] = N[2];

          ON_SubDQuadFaceMesher::GetCubicBasisDerivative(s, b);
          // evaluate = Du(i*delta_t,j*delta_t)
          Du[0] = ((b[0] * iso_cv[0][0] + b[3] * iso_cv[3][0]) + (b[1] * iso_cv[1][0] + b[2] * iso_cv[2][0]));
          Du[1] = ((b[0] * iso_cv[0][1] + b[3] * iso_cv[3][1]) + (b[1] * iso_cv[1][1] + b[2] * iso_cv[2][1]));
          Du[2] = ((b[0] * iso_cv[0][2] + b[3] * iso_cv[3][2]) + (b[1] * iso_cv[1][2] + b[2] * iso_cv[2][2]));

          // surface normal = Du X Dv
          *((ON_3dVector*)N) = ON_CrossProduct(Du, Dv);
          bool bHaveNormal = ((ON_3dVector*)N)->Unitize();
          if (false == bHaveNormal)
          {
            bHaveNormal = Internal_EvaluateSurfaceNormalBackup1(s,t,count,i,j,N);
            if (false == bHaveNormal)
            {
              bUseSurfaceNormalBackup2 = true;
              N[0] = 0.0;
              N[1] = 0.0;
              N[2] = 0.0;
            }
          }
        }
      }
      P += m_point_stride0;
      N += normal_stride[0];
    }
  }

  if (bUseSurfaceNormalBackup2)
  {
    for (j = 0; j <= count; j++)
    {
      double* N = N00 + (j * normal_stride[1]);
      for (i = 0; i <= count; i++)
      {
        if (0.0 == N[0] && 0.0 == N[1] && 0.0 == N[2] )
        {
          if ( false == Internal_EvaluateSurfaceNormalBackup2(P00,count,i,j,N) )
          {
            bUseSurfaceNormalBackup2 = true;
            ON_ERROR("Unable to get surface normal.");
            N[0] = 0.0;
            N[1] = 0.0;
            N[2] = 0.0;
          }
        }
        N += normal_stride[0];
      }
    }
  }
 
  return true;
}

bool ON_SubDQuadFacePatcher::Send4x4Patch(
  unsigned int display_density,
  const class ON_SubDFaceRegion& face_region,
  ON_SubDLimitNurbsFragment::BispanType bispan_type
  )
{
  m_patch_fragment.m_type = ON_SubDLimitNurbsFragment::Type::BicubicSingle;
  m_patch_fragment.m_bispan_type[0] = bispan_type;
  m_patch_fragment.m_bispan_type[1] = ON_SubDLimitNurbsFragment::BispanType::None;
  m_patch_fragment.m_bispan_type[2] = ON_SubDLimitNurbsFragment::BispanType::None;
  m_patch_fragment.m_bispan_type[3] = ON_SubDLimitNurbsFragment::BispanType::None;

  m_patch_fragment.m_face_region = face_region;

  const bool rc = m_fragment_callback_function(m_fragment_callback_context,&m_patch_fragment);

  // erase any region modifications made by the callback
  m_patch_fragment.m_face_region = face_region; 

  return rc;
}
  
bool ON_SubDQuadFacePatcher::Send5x5Patch(
  unsigned int display_density,
  const class ON_SubDFaceRegion& face_region,
  const ON_SubDLimitNurbsFragment::BispanType bispan_type[4]
  )
{
  m_patch_fragment.m_type = ON_SubDLimitNurbsFragment::Type::BicubicQuadrant;
  m_patch_fragment.m_bispan_type[0] = bispan_type[0];
  m_patch_fragment.m_bispan_type[1] = bispan_type[1];
  m_patch_fragment.m_bispan_type[2] = bispan_type[2];
  m_patch_fragment.m_bispan_type[3] = bispan_type[3];

  m_patch_fragment.m_face_region = face_region;

  const bool rc = m_fragment_callback_function(m_fragment_callback_context,&m_patch_fragment);

  // erase any region modifications made by the callback
  m_patch_fragment.m_face_region = face_region; 

  return rc;
}

bool ON_SubDQuadFaceMesher::GetLimitSubMeshOrPatch(
  const ON_SubDFaceRegion& face_region,
  ON_SubDQuadNeighborhood* qft,
  unsigned int display_density,
  unsigned int point_i0,
  unsigned int point_j0
  )
{
  // count = power of 2 = 2^display_density
  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(display_density);
  if ( 0 == count )
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == qft || false == qft->IsSet())
    return ON_SUBD_RETURN_ERROR(false);

  if ( ON_SubDQuadFaceMesher::Output::mesh == m_output && count > m_capacity)
    return ON_SUBD_RETURN_ERROR(false);

  if (qft->m_bIsCubicPatch)
  {
    switch (m_output)
    {
    case ON_SubDQuadFaceMesher::Output::mesh:
      if (false == qft->GetLimitSurfaceCV(&m_srf_cv[0][0][0], 4U))
        return ON_SUBD_RETURN_ERROR(false);
      return EvaluateSurface(count, point_i0, point_j0);
      break;

    case ON_SubDQuadFaceMesher::Output::patches:
      if (false == qft->GetLimitSurfaceCV(&m_patcher->m_patch_fragment.m_patch_cv[0][0][0], 5U))
        return ON_SUBD_RETURN_ERROR(false);
      //Internal_UpdateFaceRegionCorners(qft, face_region);
      m_patcher->Send4x4Patch(display_density,face_region,ON_SubDLimitNurbsFragment::BispanType::Exact);
      return true; // even if callback returns false
      break;
    }
    return ON_SUBD_RETURN_ERROR(false);
  }

  if (count > 1 && display_density > 0)
  {
    count /= 2;

    bool bSubDivide[4] = {};
    unsigned int subdivide_count = 0;
    for (unsigned int q0fvi = 0; q0fvi < 4; q0fvi++)
    {
      if ( qft->m_bExactQuadrantPatch[q0fvi] )
      {
        if (ON_SubDQuadFaceMesher::Output::mesh == m_output)
        {
          // Use the exact patch to calcuate exact mesh vertex and normal values
          if (qft->GetLimitSubSurfaceSinglePatchCV(q0fvi, m_srf_cv))
          {
            unsigned int submesh_point_i0 = point_i0 + ((1 == q0fvi || 2 == q0fvi) ? count : 0);
            unsigned int submesh_point_j0 = point_j0 + ((2 == q0fvi || 3 == q0fvi) ? count : 0);
            if (false == EvaluateSurface(count, submesh_point_i0, submesh_point_j0))
              return ON_SUBD_RETURN_ERROR(false);
            continue;
          }
        }
      }
      else
      {
        // local subdivision is required
        bSubDivide[q0fvi] = true;
        subdivide_count++;
      }
    }

    if (ON_SubDQuadFaceMesher::Output::patches == m_output && subdivide_count < 4 )
    {
      // Get the bicubic NURBS control points for the patches that are exact.
      // (subdivide_count < 4) means there is at least one.
      // These are delivered as a collection to enable merging them into as
      // large a face/patch/... as possible.
      ON_SubDLimitNurbsFragment::BispanType pt[4] =
      {
        ON_SubDLimitNurbsFragment::BispanType::None,
        ON_SubDLimitNurbsFragment::BispanType::None,
        ON_SubDLimitNurbsFragment::BispanType::None,
        ON_SubDLimitNurbsFragment::BispanType::None
      };
      // Harvest any exact patches that are available at this subdivision level
      const bool bEnableApproximatePatch = false;
      unsigned int quadrant_count = qft->GetLimitSubSurfaceMultiPatchCV(
        bEnableApproximatePatch,
        m_patcher->m_patch_fragment.m_patch_cv,
        pt
        );
      if ( 4 != subdivide_count + quadrant_count )
        return ON_SUBD_RETURN_ERROR(false);
      m_patcher->m_patch_fragment.m_type = ON_SubDLimitNurbsFragment::Type::BicubicQuadrant;
      //Internal_UpdateFaceRegionCorners(qft, face_region);
      bool bCallbackResult = m_patcher->Send5x5Patch( display_density, face_region, pt );
      if ( false == bCallbackResult)
        return true;
    }

    for (unsigned int q0fvi = 0; q0fvi < 4; q0fvi++)
    {
      if ( false == bSubDivide[q0fvi])
        continue;
      
      // local subdivision is required
      ON_SubD_FixedSizeHeap* fsh = CheckOutLocalFixedSizeHeap();
      if ( nullptr == fsh )
        return ON_SUBD_RETURN_ERROR(false);
      
      ON_SubDQuadNeighborhood qft1;      
      if (false == qft->Subdivide( q0fvi, *fsh, &qft1 ))
      {
        ReturnLocalFixedSizeHeap(fsh);
        return ON_SUBD_RETURN_ERROR(false);
      }
      //if (ON_SubDQuadFaceMesher::Output::patches == m_output)
      //  Internal_UpdateFaceRegionCorners(qft, face_region);

      subdivide_count--;
      if (0 == subdivide_count)
      {
        // If there is an exraordiary vertex and qft is using one of the
        // m_fsh[] on this class, then the recursion will need the
        // m_fsh the no longer needed qft is using.
        const bool bRetainFixedSizeHeap 
          = nullptr == qft->m_fsh
          || false == ReturnLocalFixedSizeHeap(qft->m_fsh);
        ON_SubDQuadNeighborhood::Clear(qft, bRetainFixedSizeHeap);
      }

      unsigned int submesh_point_i0 = point_i0 + ((1==q0fvi || 2==q0fvi) ? count : 0);
      unsigned int submesh_point_j0 = point_j0 + ((2==q0fvi || 3==q0fvi) ? count : 0);

      ON_SubDFaceRegion face_corner_region(face_region);
      face_corner_region.Push(q0fvi);
      const bool rc = GetLimitSubMeshOrPatch(face_corner_region, &qft1, display_density-1, submesh_point_i0, submesh_point_j0 );      

      ReturnLocalFixedSizeHeap(fsh);
      if ( false == rc )
        return ON_SUBD_RETURN_ERROR(false);
    }

    return true;
  }
  
  if (1 == count && 0 == display_density)
  {
    // No more subdivison steps are permitted
    if (ON_SubDQuadFaceMesher::Output::patches == m_output)
    {
      ON_SubDLimitNurbsFragment::BispanType pt[4] =
      {
        ON_SubDLimitNurbsFragment::BispanType::None,
        ON_SubDLimitNurbsFragment::BispanType::None,
        ON_SubDLimitNurbsFragment::BispanType::None,
        ON_SubDLimitNurbsFragment::BispanType::None
      };

      ON_SubDComponentRegionBreakpoint(&face_region.m_face_region);

      // Harvest whatever patches are available and allow approximate patches to be returned
      // if an appropriate number of subdivisions have been performed
      const bool bEnableApproximatePatch 
        = qft->m_extraordinary_corner_vertex_count <= 1 
        && face_region.m_face_region.SubdivisionCount() >= 2;

      while(
        bEnableApproximatePatch
        && 1 == qft->m_extraordinary_corner_vertex_count
        && 1 == qft->m_exact_quadrant_patch_count
        && nullptr != qft->m_center_edges[0] 
        && nullptr != qft->m_center_edges[1] 
        && nullptr != qft->m_center_edges[2] 
        && nullptr != qft->m_center_edges[3]
        )
      {
        bool bIsDart = false;
        unsigned int extraordinary_vertex_index = qft->ExtraordinaryCenterVertexIndex(ON_SubD::VertexTag::Crease, 4);
        if (extraordinary_vertex_index > 3 && ON_SubDQuadFaceMesher::Output::patches == m_output)
        {
          // Test for Dart fixes RH-31322
          extraordinary_vertex_index = qft->ExtraordinaryCenterVertexIndex(ON_SubD::VertexTag::Dart, 4);
          if ( extraordinary_vertex_index > 3)
            break;
          bIsDart = true;
        }
        const ON_SubDVertex* extraordinary_vertex = qft->CenterVertex(extraordinary_vertex_index);
        if (nullptr == extraordinary_vertex)
          break;


        ON_SubD_FixedSizeHeap* fsh = CheckOutLocalFixedSizeHeap();
        if (nullptr == fsh)
          break;
        ON_SubDQuadNeighborhood qft1;

        // claculate limit points on edges needed to get approximate NURBS patches near the singular point.
        if (qft->Subdivide(extraordinary_vertex_index, *fsh, &qft1))
        {
          ON_2dex qft1_vdex[2] = {ON_2dex::Unset,ON_2dex::Unset};
          unsigned int center_edge_index[2] = { ON_UNSET_UINT_INDEX,ON_UNSET_UINT_INDEX };
          switch (extraordinary_vertex_index)
          {
          case 0:
            center_edge_index[0] = 3;
            qft1_vdex[0] = ON_2dex(1, 2);
            center_edge_index[1] = 0;
            qft1_vdex[1] = ON_2dex(2, 1);
            break;
          case 1:
            center_edge_index[0] = 0;
            qft1_vdex[0] = ON_2dex(1, 1);
            center_edge_index[1] = 1;
            qft1_vdex[1] = ON_2dex(2, 2);
            break;
          case 2:
            center_edge_index[0] = 1;
            qft1_vdex[0] = ON_2dex(2, 1);
            center_edge_index[1] = 2;
            qft1_vdex[1] = ON_2dex(1, 2);
            break;
          case 3:
            center_edge_index[0] = 2;
            qft1_vdex[0] = ON_2dex(2, 2);
            center_edge_index[1] = 3;
            qft1_vdex[1] = ON_2dex(1, 1);
            break;
          default:
            center_edge_index[0] = ON_UNSET_UINT_INDEX;
            qft1_vdex[0] = ON_2dex::Unset;
            center_edge_index[1] = ON_UNSET_UINT_INDEX;
            qft1_vdex[1] = ON_2dex::Unset;
            break;
          }
          for (int n = 0; n < 2; n++)
          {
            if (center_edge_index[n] >= 4)
              continue;
            if (qft->m_bCenterEdgeLimitPoint[center_edge_index[n]])
              continue;
            const ON_SubDVertex* v = qft1.m_vertex_grid[qft1_vdex[n].i][qft1_vdex[n].j];
            if (nullptr == v)
              continue;
            qft->m_bCenterEdgeLimitPoint[center_edge_index[n]] = v->GetLimitPoint(ON_SubD::SubDType::QuadCatmullClark, qft1.m_face_grid[1][1], true, qft->m_center_edge_limit_point[center_edge_index[n]]);
          }
        }

        const ON_2dex srf_cv1_side_midpoint_dex[4] = { ON_2dex(2,0), ON_2dex(4,2), ON_2dex(2,4), ON_2dex(0,2) };
        const ON_2dex srf_cv1_ccw_dex[4] = { ON_2dex(1,0), ON_2dex(0,1), ON_2dex(-1,0), ON_2dex(0,-1) };
        if (ON_SubD::EdgeTag::Crease == qft->m_center_edges[extraordinary_vertex_index]->m_edge_tag)
        {
          const ON_2dex delta(srf_cv1_ccw_dex[extraordinary_vertex_index]);
          const ON_2dex dex0 = srf_cv1_side_midpoint_dex[extraordinary_vertex_index];
          const ON_2dex dex1(dex0.i + delta.i, dex0.j + delta.j);
          const ON_2dex dex2(dex1.i + delta.i, dex1.j + delta.j);
          double *P[3] = { &qft->m_srf_cv1[dex0.i][dex0.j][0], &qft->m_srf_cv1[dex1.i][dex1.j][0], &qft->m_srf_cv1[dex2.i][dex2.j][0] };
          while (ON_UNSET_VALUE == P[0][0] && ON_UNSET_VALUE == P[1][0] && ON_UNSET_VALUE == P[2][0])
          {
            // calculate 3 additional subd points needed to get patch 3
            ON_SubDQuadNeighborhood::Clear(&qft1, false);
            if (false == qft->Subdivide((extraordinary_vertex_index + 1) % 4, *fsh, &qft1))
              break;
            if (1 != qft1.m_boundary_crease_count)
              break;
            for (int n = 0; n < 4; n++)
            {
              if (
                qft1.m_bBoundaryCrease[n]
                && nullptr != qft1.m_center_edges[n]
                && ON_SubD::EdgeTag::Crease == qft1.m_center_edges[n]->m_edge_tag
                )
              {
                ON_2dex crease_dex[3];
                crease_dex[0] = ON_SubDQuadNeighborhood::CenterVertexDex(n);
                crease_dex[1] = ON_SubDQuadNeighborhood::CenterVertexDex((n+1)%4);
                ON_2dex d(crease_dex[1].i - crease_dex[0].i, crease_dex[1].j - crease_dex[0].j);
                crease_dex[2] = ON_2dex(crease_dex[1].i + d.i, crease_dex[1].j + d.j);
                int tmp = d.j; d.j = d.i; d.i = -tmp;
                const ON_2dex smooth_dex[3] = {
                  ON_2dex(crease_dex[0].i + d.i, crease_dex[0].j + d.j),
                  ON_2dex(crease_dex[1].i + d.i, crease_dex[1].j + d.j),
                  ON_2dex(crease_dex[2].i + d.i, crease_dex[2].j + d.j)
                };
                for (int k = 0; k < 3; k++)
                {
                  const ON_SubDVertex* crease_vertex = qft1.m_vertex_grid[crease_dex[k].i][crease_dex[k].j];
                  if (nullptr == crease_vertex)
                    continue;
                  if ( ON_SubD::VertexTag::Crease != crease_vertex->m_vertex_tag)
                    continue;
                  const ON_SubDVertex* smooth_vertex = qft1.m_vertex_grid[smooth_dex[k].i][smooth_dex[k].j];
                  if (nullptr == smooth_vertex)
                    continue;
                  if ( ON_SubD::VertexTag::Smooth != smooth_vertex->m_vertex_tag)
                    continue;
                  P[k][0] = 2.0*crease_vertex->m_P[0] - smooth_vertex->m_P[0];
                  P[k][1] = 2.0*crease_vertex->m_P[1] - smooth_vertex->m_P[1];
                  P[k][2] = 2.0*crease_vertex->m_P[2] - smooth_vertex->m_P[2];
                }
                break;
              }
            }
            break;
          }
        }

        if (ON_SubD::EdgeTag::Crease == qft->m_center_edges[(extraordinary_vertex_index+3)%4]->m_edge_tag)
        {
          const ON_2dex delta(srf_cv1_ccw_dex[(extraordinary_vertex_index+1)%4]); // +1 to go reverse direction
          const ON_2dex dex0(srf_cv1_side_midpoint_dex[(extraordinary_vertex_index + 3)%4]);
          const ON_2dex dex1(dex0.i + delta.i, dex0.j + delta.j);
          const ON_2dex dex2(dex1.i + delta.i, dex1.j + delta.j);
          double *P[3] = { &qft->m_srf_cv1[dex0.i][dex0.j][0], &qft->m_srf_cv1[dex1.i][dex1.j][0], &qft->m_srf_cv1[dex2.i][dex2.j][0] };
          if (ON_UNSET_VALUE == P[0][0] && ON_UNSET_VALUE == P[1][0] && ON_UNSET_VALUE == P[2][0])
          {
            // calculate 3 additional subd points needed to get patch 1

            ON_SubDQuadNeighborhood::Clear(&qft1, false);
            if (false == qft->Subdivide((extraordinary_vertex_index + 3) % 4, *fsh, &qft1))
              break;
            if (1 != qft1.m_boundary_crease_count)
              break;
            for (int n = 0; n < 4; n++)
            {
              if (
                qft1.m_bBoundaryCrease[n]
                && nullptr != qft1.m_center_edges[n]
                && ON_SubD::EdgeTag::Crease == qft1.m_center_edges[n]->m_edge_tag
                )
              {
                ON_2dex crease_dex[3];
                crease_dex[1] = ON_SubDQuadNeighborhood::CenterVertexDex(n);
                crease_dex[0] = ON_SubDQuadNeighborhood::CenterVertexDex((n+1)%4);
                ON_2dex d(crease_dex[1].i - crease_dex[0].i, crease_dex[1].j - crease_dex[0].j);
                crease_dex[2] = ON_2dex(crease_dex[1].i + d.i, crease_dex[1].j + d.j);
                int tmp = d.i; d.i = d.j; d.j = -tmp;
                const ON_2dex smooth_dex[3] = {
                  ON_2dex(crease_dex[0].i + d.i, crease_dex[0].j + d.j),
                  ON_2dex(crease_dex[1].i + d.i, crease_dex[1].j + d.j),
                  ON_2dex(crease_dex[2].i + d.i, crease_dex[2].j + d.j)
                };
                for (int k = 0; k < 3; k++)
                {
                  const ON_SubDVertex* crease_vertex = qft1.m_vertex_grid[crease_dex[k].i][crease_dex[k].j];
                  if (nullptr == crease_vertex)
                    continue;
                  if ( ON_SubD::VertexTag::Crease != crease_vertex->m_vertex_tag)
                    continue;
                  const ON_SubDVertex* smooth_vertex = qft1.m_vertex_grid[smooth_dex[k].i][smooth_dex[k].j];
                  if (nullptr == smooth_vertex)
                    continue;
                  if ( ON_SubD::VertexTag::Smooth != smooth_vertex->m_vertex_tag)
                    continue;
                  P[k][0] = 2.0*crease_vertex->m_P[0] - smooth_vertex->m_P[0];
                  P[k][1] = 2.0*crease_vertex->m_P[1] - smooth_vertex->m_P[1];
                  P[k][2] = 2.0*crease_vertex->m_P[2] - smooth_vertex->m_P[2];
                }
                break;
              }
            }
          }
        }

        if (nullptr != fsh)
          ReturnLocalFixedSizeHeap(fsh);
        break;
      }


      unsigned int quadrant_count = qft->GetLimitSubSurfaceMultiPatchCV(
        bEnableApproximatePatch,
        m_patcher->m_patch_fragment.m_patch_cv,
        pt
        );

      if (quadrant_count > 0)
      {
        m_patcher->m_patch_fragment.m_type = ON_SubDLimitNurbsFragment::Type::BicubicQuadrant;
        bool bCallbackResult = m_patcher->Send5x5Patch(display_density, face_region, pt);

        if (false == bCallbackResult)
          return true;
      }

      return true;
    }

    // Use limit point evalators to get exact locations and normals
    // for the unset corners of this mesh quad.
    // In a qft, all the vertices have single sectors
    double* P[2][2];
    double* N[2][2];
    ON_SubDSectorLimitPoint limit_point;
    for (unsigned int i = 0; i < 2; i++)
    {
      for (unsigned int j = 0; j < 2; j++)
      {
        P[i][j] = m_points + (point_i0 + i*count)*m_point_stride0 + (point_j0 + j*count)*m_point_stride1;
        if (ON_UNSET_VALUE == P[i][j][0])
        {
          N[i][j] = m_normals + (point_i0 + i*count)*m_normal_stride0 + (point_j0 + j*count)*m_normal_stride1;

          if (false == qft->m_vertex_grid[1 + i][1 + j]->GetLimitPoint(ON_SubD::SubDType::QuadCatmullClark, qft->m_face_grid[1][1], true, limit_point ))
            return ON_SUBD_RETURN_ERROR(false);

          P[i][j][0] = limit_point.m_limitP[0];
          P[i][j][1] = limit_point.m_limitP[1];
          P[i][j][2] = limit_point.m_limitP[2];

          N[i][j][0] = limit_point.m_limitN[0];
          N[i][j][1] = limit_point.m_limitN[1];
          N[i][j][2] = limit_point.m_limitN[2];
        }
      }
    }
    return true;
  }
    
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDQuadFaceMesher::GetLimitMesh(
  const ON_SubDFaceRegion& face_region,
  const ON_SubDFace* face
  )
{

  ReturnAllFixedSizeHeaps();

  m_count = 0;

  if (ON_SubDQuadFaceMesher::Output::mesh != m_output)
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == face || 4 != face->m_edge_count)
    return ON_SUBD_RETURN_ERROR(false);

  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(m_display_density);

  if (0 == count)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }


  if (count > m_capacity)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  // Get neighborhood topology information
  ON_SubDQuadNeighborhood qft;
  if (false == qft.Set(face))
    return ON_SUBD_RETURN_ERROR(false);

  // GetLimitSubMesh is recursive.
  m_count = count;
  UnsetMeshPoints();

  return GetLimitSubMeshOrPatch(face_region,&qft,m_display_density,0,0);
}


bool ON_SubDQuadFaceMesher::GetLimitPatches(
  const ON_SubDFaceRegion& face_region,  
  const ON_SubDFace* face
  )
{
  ReturnAllFixedSizeHeaps();

  m_count = 0;

  if (ON_SubDQuadFaceMesher::Output::patches != m_output)
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == face || 4 != face->m_edge_count)
    return ON_SUBD_RETURN_ERROR(false);

  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(m_display_density);

  if (0 == count)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  // Get neighborhood topology information
  ON_SubDQuadNeighborhood qft;
  if (false == qft.Set(face))
    return ON_SUBD_RETURN_ERROR(false);

  // GetLimitSubMesh is recursive.
  return GetLimitSubMeshOrPatch(face_region,&qft,m_display_density,0,0);
}


static bool GetLimitSurfaceInStepsSetup(
  const ON_SubD& subd,
  ON_SubDDisplayParameters& limit_mesh_parameters
  )
{
  const unsigned int level_count = subd.LevelCount();
  if (0 == level_count)
    return ON_SUBD_RETURN_ERROR(false);


  if (1 == level_count && ON_SubD::SubDType::Unset == subd.ActiveLevelSubDType())
    const_cast< ON_SubD& >(subd).SetSubDType(ON_SubD::SubDType::QuadCatmullClark);

  const ON_SubD::SubDType subd_type = subd.ActiveLevelSubDType();
  if (ON_SubD::SubDType::QuadCatmullClark != subd_type)
  {
    // TODO - support tri subd after quad stuff is finished.
    return ON_SUBD_RETURN_ERROR(false);
  }

  limit_mesh_parameters.m_progress_reporter_interval.Intersection(ON_Interval::ZeroToOne);
  if ( false == limit_mesh_parameters.m_progress_reporter_interval.IsIncreasing())
    limit_mesh_parameters.m_progress_reporter_interval = ON_Interval::ZeroToOne;

  return true;
}

unsigned int ON_SubD::GetLimitSurfaceMeshInFragments(
  const class ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitMeshFragment*)
  ) const
{
  ON_SubDDisplayParameters local_limit_mesh_parameters = limit_mesh_parameters;

  if ( false == GetLimitSurfaceInStepsSetup(*this,local_limit_mesh_parameters) )
    return ON_SUBD_RETURN_ERROR(0);


  ON_SubDFaceIterator fit(*this);
  unsigned int fragment_count = GetQuadLimitSurfaceMeshFragmentsHelper(
    fit,
    local_limit_mesh_parameters,
    fragment_callback_context,
    fragment_callback_function
    );

  if ( fragment_count > 0 )
    return fragment_count;

  return ON_SUBD_RETURN_ERROR(0);
}  

unsigned int ON_SubD::GetLimitSurfaceNurbsFragments(
  const class ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*begin_face_callback_function)(ON__UINT_PTR ,const ON_SubDFaceRegion&),//, const class ON_SubDFace*, const class ON_SubDFace*, unsigned int),
  bool(*fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitNurbsFragment*)
  ) const
{
  ON_SubDDisplayParameters local_limit_mesh_parameters = limit_mesh_parameters;

  if ( false == GetLimitSurfaceInStepsSetup(*this,local_limit_mesh_parameters) )
    return ON_SUBD_RETURN_ERROR(0);

  ON_SubDFaceIterator fit(*this);
  unsigned int fragment_count = GetQuadLimitSurfacePatchFragmentsHelper(
    fit,
    local_limit_mesh_parameters,
    fragment_callback_context,
    begin_face_callback_function,
    fragment_callback_function
    );

  if ( fragment_count > 0 )
    return fragment_count;

  return ON_SUBD_RETURN_ERROR(0);
}



unsigned int ON_SubDFaceIterator::LimitSurfaceMeshFragmentCount(
  ON_SubD::FacetType facet_type
  ) const
{
  unsigned int fragment_count = 0;
  unsigned short ordinary_edge_count 
    = (ON_SubD::FacetType::Tri == facet_type)
    ? 3U
    : 4U; // default is quads

  for (const ON_SubDFace* face = m_face_first; nullptr != face; face = face->m_next_face)
  {
    if ( ordinary_edge_count == face->m_edge_count )
      fragment_count++;
    else
      fragment_count += face->m_edge_count;
  }
  return fragment_count;
}


unsigned int ON_SubD::LimitSurfaceMeshFragmentCount() const
{
  return ActiveLevel().LimitSurfaceMeshFragmentCount();
}

unsigned int ON_SubDLevel::LimitSurfaceMeshFragmentCount() const
{
  unsigned int fragment_count = 0;
  ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(m_subdivision_type);
  unsigned short ordinary_edge_count 
    = (ON_SubD::FacetType::Tri == facet_type)
    ? 3U
    : 4U; // default is quads

  for (const ON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
  {
    if ( ordinary_edge_count == face->m_edge_count )
      fragment_count++;
    else
      fragment_count += face->m_edge_count;
  }
  return fragment_count;
}

ON_SubDLimitMesh ON_SubD::UpdateLimitSurfaceMesh(
  unsigned int minimum_display_density
  ) const
{

  ON_SubDDisplayParameters display_parameters;
  display_parameters.m_display_density = minimum_display_density;
  return ActiveLevel().UpdateLimitSurfaceMesh(*this,display_parameters);
}

ON_SubDLimitMesh ON_SubDLevel::UpdateLimitSurfaceMesh(
    const ON_SubD& subd,
    const class ON_SubDDisplayParameters& display_parameters
    ) const
{
  if ( IsEmpty() )
    return ON_SubDLimitMesh::Empty;

  if (m_limit_mesh.IsEmpty() || display_parameters.m_display_density > m_limit_mesh.DisplayParameters().m_display_density)
  {
    ON_SubDLimitMesh local_limit_mesh;
    if (nullptr != ON_SubDLimitMesh::Create(subd, display_parameters, &local_limit_mesh))
    {
      ON_SubDLimitMesh::Swap(m_limit_mesh,local_limit_mesh);
      local_limit_mesh.Clear();
    }
  }

  return m_limit_mesh;
}

class ON_SubDLimitMesh ON_SubD::LimitSurfaceMesh() const
{
  return ActiveLevel().m_limit_mesh;
}

void ON_SubD::ClearLimitSurfaceMesh() const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();

  if ( nullptr != level )
    level->m_limit_mesh = ON_SubDLimitMesh::Empty;
}

void ON_SubD::ClearEvaluationCache() const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();

  if (nullptr != level)
  {
    level->ClearEdgeFlags();
    level->ClearBoundingBox();
    level->ClearSubdivisonAndLimitPoints();
    level->m_limit_mesh = ON_SubDLimitMesh::Empty;
  }
}



////////////////////////////////////////////////////////////////////////////

class ON_SUBD_CLASS ON_SubDLimitSurfaceFragment
{
public:
  ON_SubDLimitSurfaceFragment() = default;
  ~ON_SubDLimitSurfaceFragment() = default;
  ON_SubDLimitSurfaceFragment(const ON_SubDLimitSurfaceFragment&) = default;
  ON_SubDLimitSurfaceFragment& operator=(const ON_SubDLimitSurfaceFragment&) = default;

public:
  static const ON_SubDLimitSurfaceFragment Empty;

public:
  // m_face_region identifies what part of the SubD level0 face is or will be modeled by m_surface.
  ON_SubDFaceRegion m_face_region;

  // knot vector is uniform and not clamped. 
  ON_NurbsSurface* m_surface = nullptr;

  ON_SubDLimitSurfaceFragment* Quadrant(unsigned int quadrant_index, bool bAllocateIfMissing);
  ON_SubDLimitSurfaceFragment* Parent();

  static ON_SubDLimitSurfaceFragment* AllocateSurfaceFragment();
  static void ReturnSurfaceFragment(ON_SubDLimitSurfaceFragment*);

  bool SetSurface(ON_NurbsSurface* surface);

  bool SetSurfaceFromQuadrants(
    ON_SubD::NurbsSurfaceType nurbs_surface_type
    );

  bool SetQuadrantSurface(ON_NurbsSurface* quadrant_surface,unsigned int quadrant_index);

private:
  // Parent fragment for this 
  ON_SubDLimitSurfaceFragment* m_parent = nullptr;

  // The 4 quadrants of this region
  ON_SubDLimitSurfaceFragment* m_quadrants[4] = {};

  static ON_FixedSizePool m_fsp;
};

ON_FixedSizePool ON_SubDLimitSurfaceFragment::m_fsp;

ON_SubDLimitSurfaceFragment* ON_SubDLimitSurfaceFragment::AllocateSurfaceFragment()
{
  ON_MemoryAllocationTracking disable_tracking(false); 
  if (0 == ON_SubDLimitSurfaceFragment::m_fsp.SizeofElement())
  {
    ON_SubDLimitSurfaceFragment::m_fsp.Create(sizeof(ON_SubDLimitSurfaceFragment), 64, 64);
  }
  ON_SubDLimitSurfaceFragment* f = (ON_SubDLimitSurfaceFragment*)ON_SubDLimitSurfaceFragment::m_fsp.AllocateElement();
  if (nullptr == f)
  {
    ON_SUBD_ERROR("Allocation failed");
  }
  return f;
}

void ON_SubDLimitSurfaceFragment::ReturnSurfaceFragment(ON_SubDLimitSurfaceFragment* f )
{
  if (nullptr != f)
    ON_SubDLimitSurfaceFragment::m_fsp.ReturnElement(f);
}

bool ON_SubDLimitSurfaceFragment::SetSurface(ON_NurbsSurface* surface)
{
  if (nullptr == surface)
    return false;
  if (nullptr != m_surface)
  {
    ON_SUBD_ERROR("Surface exists.");
    return false;
  }
  if (
    nullptr != m_quadrants[0]
    || nullptr != m_quadrants[1]
    || nullptr != m_quadrants[2]
    || nullptr != m_quadrants[3]
    )
  {
    ON_SUBD_ERROR("Setting surface when quadrants exist.");
  }
  m_surface = surface;
  return true;
}

bool ON_SubDLimitSurfaceFragment::SetQuadrantSurface(ON_NurbsSurface* quadrant_surface, unsigned int quadrant_index)
{
  if (nullptr == quadrant_surface)
    return false;
  ON_SubDLimitSurfaceFragment* q = Quadrant(quadrant_index, true);
  if (nullptr == q)
    return false;
  return q->SetSurface(quadrant_surface);
}

static bool Internal_EqualKnots(
  double knot_tol,
  int dir,
  const ON_NurbsSurface* lhs,
  const ON_NurbsSurface* rhs
)
{
  // all orders are 4
  const int knot_count = lhs->KnotCount(dir);
  if (knot_count != rhs->KnotCount(dir))
    return false;
  const double* lhs_knot = lhs->m_knot[dir];
  const double* rhs_knot = rhs->m_knot[dir];
  for (int i = 0; i < knot_count; i++)
  {
    if ( !(fabs(lhs_knot[i] - rhs_knot[i]) <= knot_tol) )
      return false;
  }
  return true;
}


static bool Internal_OverlapingKnots(
  double knot_tol,
  int dir,
  const ON_NurbsSurface* lhs,
  const ON_NurbsSurface* rhs
)
{
  // all orders are 4
  const double* lhs_knot = lhs->m_knot[dir];
  const double* rhs_knot = rhs->m_knot[dir];
  if (!(rhs_knot[0] < rhs_knot[1] && rhs_knot[1] < rhs_knot[2] && rhs_knot[2] < rhs_knot[3]))
    return false;
  const unsigned int lhs_knot_count = lhs->KnotCount(dir);
  lhs_knot += (lhs_knot_count - 5);
  for (unsigned int i = 0; i < 5; i++)
  {
    if (!(fabs(lhs_knot[i] - rhs_knot[i]) <= knot_tol))
      return false;
  }
  return true;
}

static ON_NurbsSurface* Internal_MergeC2Neighbors(
  ON_SubD::NurbsSurfaceType nurbs_surface_type,
  int dir,
  ON_NurbsSurface* lhs,
  ON_NurbsSurface* rhs
)
{
  // Context:
  //   lhs and rhs are cubic non-rational NURBS and are known to meed C2 at the shared edge.
  //
  // dir = 0: join East side of lhs to West side of rhs
  // dir = 1: join North side of lhs to South side of rhs
  // Remaining comments are for dir = 0:

  if (
    dir < 0 || dir > 1
    || nullptr == lhs || nullptr == rhs
    || 4 != lhs->m_order[0] || 4 != lhs->m_order[1]
    || 4 != rhs->m_order[0] || 4 != rhs->m_order[1]
    || 0 != lhs->m_is_rat || 0 != rhs->m_is_rat
    || 3 != lhs->m_dim || 3 != rhs->m_dim
    )
  {
    ON_SUBD_ERROR("Invalid input.");
    return nullptr;
  }

  const int dir1 = 1 - dir;

  if (!(lhs->Domain(dir).m_t[1] == rhs->Domain(dir)[0]))
  {
    ON_SUBD_ERROR("Invalid dir or input domains.");
    return nullptr;
  }

  if (!(lhs->Domain(dir1) == rhs->Domain(dir1)))
  {
    ON_SUBD_ERROR("Invalid dir or input domains.");
    return nullptr;
  }

  const double knot_tol = 1e-8;

  // merged->m_knots[dir][...] begins with lhs->m_knot[dir][...] and ends with rhs->m_knot[dir][...]
  const bool bOverlapMerge = Internal_OverlapingKnots(knot_tol, dir, lhs, rhs);
  if (false == bOverlapMerge)
  {
    if (ON_SubD::NurbsSurfaceType::Large != nurbs_surface_type)
      return nullptr; // not permitted to modify knots.
    lhs->ClampEnd(dir, 2);
    rhs->ClampEnd(dir, 2);
  }

  // We need lhs->m_knot[dir1][...] = rhs->m_knot[dir1][...]
  // and merged->m_knots[dir1][...] = lhs->m_knot[dir1][...] 
  if (false == Internal_EqualKnots(knot_tol, dir1, lhs, rhs))
  {
    if (ON_SubD::NurbsSurfaceType::Large != nurbs_surface_type)
      return nullptr; // not permitted to modify knots.

    lhs->ClampEnd(dir1, 2);
    rhs->ClampEnd(dir1, 2);
    if (false == Internal_EqualKnots(knot_tol, dir1, lhs, rhs))
    {
      // Insert knots to make lhs->m_knot[dir1][...] = rhs->m_knot[dir1][...] equal
      double lhs_k = lhs->m_knot[dir1][2];
      double rhs_k = rhs->m_knot[dir1][2];
      int lhs_i = 3;
      int rhs_i= 3;
      while (lhs_i < lhs->m_cv_count[dir1] && rhs_i < rhs->m_cv_count[dir1])
      {
        double lhs_k0 = lhs_k;
        double rhs_k0 = rhs_k;
        lhs_k = lhs->m_knot[dir1][lhs_i];
        rhs_k = rhs->m_knot[dir1][rhs_i];
        if (!(lhs_k0+knot_tol < lhs_k))
        {
          ON_SUBD_ERROR("Invalid lhs knots or a bug.");
          return nullptr;
        }
        if (!(rhs_k0+knot_tol < rhs_k))
        {
          ON_SUBD_ERROR("Invalid rhs knots or a bug.");
          return nullptr;
        }

        if (lhs_k + knot_tol < rhs_k)
        {
          // insert knot in rhs at lhs_k
          if (false == rhs->InsertKnot(dir1, lhs_k, 1))
          {
            ON_SUBD_ERROR("rhs knot insertion failed.");
            return nullptr;
          }
          rhs_k = rhs->m_knot[dir1][rhs_i];
        }

        if (rhs_k + knot_tol < lhs_k)
        {
          // insert knot in lhs at rhs_k
          if ( false == lhs->InsertKnot(dir1, rhs_k, 1) )
          {
            ON_SUBD_ERROR("lhs knot insertion failed.");
            return nullptr;
          }
          lhs_k = lhs->m_knot[dir1][lhs_i];
        }
        if (!(fabs(lhs_k - rhs_k) <= knot_tol))
        {
          ON_SUBD_ERROR("Unexpected knot insertion failure.");
          return nullptr;
        }
        lhs_i++;
        rhs_i++;
      }
      if (false == Internal_EqualKnots(knot_tol, dir1, lhs, rhs))
      {
        ON_SUBD_ERROR("Unexpected different knot vectors.8");
        return nullptr;
      }
    }
  }

  //// DEBUGGING
  //if (false == lhs->IsValid())
  //{
  //  ON_SUBD_ERROR("lhs is not valid.");
  //  return nullptr;
  //}

  //  // DEBUGGING
  //if (false == rhs->IsValid())
  //{
  //  ON_SUBD_ERROR("rhs is not valid.");
  //  return nullptr;
  //}

  // Fill in merged surface
  const int lhs_cv_count0 = lhs->m_cv_count[0];
  const int lhs_cv_count1 = lhs->m_cv_count[1];
  const int rhs_cv_count0 = rhs->m_cv_count[0];
  const int rhs_cv_count1 = rhs->m_cv_count[1];
  int cv_count0 = lhs_cv_count0;
  int cv_count1 = lhs_cv_count1;
  int rhs_cv_dex0 = 0;
  int rhs_cv_dex1 = 0;
  if (0 == dir)
  {
    if (lhs_cv_count1 != rhs_cv_count1)
    {
      ON_SUBD_ERROR("Bug in dir=0 merging.");
      return nullptr;
    }
    rhs_cv_dex0 = (bOverlapMerge) ? 3 : 1;
    cv_count0 += (rhs_cv_count0 - rhs_cv_dex0);
  }
  else
  {
    if (lhs_cv_count0 != rhs_cv_count0)
    {
      ON_SUBD_ERROR("Bug in dir=1 merging.");
      return nullptr;
    }
    rhs_cv_dex1 = (bOverlapMerge) ? 3 : 1;
    cv_count1 += (rhs_cv_count1 - rhs_cv_dex1);
  }
  ON_NurbsSurface* merged_srf = new ON_NurbsSurface(3, 0, 4, 4, cv_count0, cv_count1);

  const double* src;
  double* dst;
  for (int i = 0; i < lhs_cv_count0; i++)
  {
    for (int j = 0; j < lhs_cv_count1; j++)
    {
      src = lhs->CV(i, j);
      dst = merged_srf->CV(i, j);
      dst[0] = src[0];
      dst[1] = src[1];
      dst[2] = src[2];
    }
  }

  if (0 == dir)
  {
    int dst_i = lhs_cv_count0;
    for (int i = rhs_cv_dex0; i < rhs_cv_count0; i++, dst_i++)
    {
      for (int j = 0; j < rhs_cv_count1; j++)
      {
        src = rhs->CV(i, j);
        dst = merged_srf->CV(dst_i, j);
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
      }
    }
  }
  else
  {
    int dst_j = lhs_cv_count1;
    for (int j = rhs_cv_dex1; j < rhs_cv_count1; j++, dst_j++)
    {
      for (int i = 0; i < rhs_cv_count0; i++)
      {
        src = rhs->CV(i, j);
        dst = merged_srf->CV(i, dst_j);
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
      }
    }
  }

  const int lhs_knot_count0 = lhs->KnotCount(0);
  const int lhs_knot_count1 = lhs->KnotCount(1);
  for (int i = 0; i < lhs_knot_count0; i++)
    merged_srf->m_knot[0][i] = lhs->m_knot[0][i];
  for (int i = 0; i < lhs_knot_count1; i++)
    merged_srf->m_knot[1][i] = lhs->m_knot[1][i];

  if (0 == dir)
  {
    const int rhs_knot_count0 = rhs->KnotCount(0);
    dst = &merged_srf->m_knot[0][lhs_knot_count0];
    for (int i = 2 + rhs_cv_dex0; i < rhs_knot_count0; i++)
      *dst++ = rhs->m_knot[0][i];
  }
  else
  {
    const int rhs_knot_count1 = rhs->KnotCount(1);
    dst = &merged_srf->m_knot[1][lhs_knot_count1];
    for (int i = 2 + rhs_cv_dex1; i < rhs_knot_count1; i++)
      *dst++ = rhs->m_knot[1][i];
  }


  //// DEBUGGING
  //if (false == merged_srf->IsValid())
  //{
  //  ON_SUBD_ERROR("merged_srf is not valid.");
  //  delete merged_srf;
  //  merged_srf = nullptr;
  //}

  return merged_srf;
}

bool ON_SubDLimitSurfaceFragment::SetSurfaceFromQuadrants(
  ON_SubD::NurbsSurfaceType nurbs_surface_type
  )
{
  if (nullptr != m_surface)
    return true;

  ON_NurbsSurface* s[4] = { 0 };
  for (unsigned int quadrant_index = 0; quadrant_index < 4; quadrant_index++)
  {
    if (nullptr == m_quadrants[quadrant_index])
      return false;

    if (nullptr == m_quadrants[quadrant_index]->m_surface)
      return false;
    s[quadrant_index] = m_quadrants[quadrant_index]->m_surface;
    if (ON_SubD::NurbsSurfaceType::Large != nurbs_surface_type)
    {
      // not permitted to add knots
      if (
        s[0]->m_cv_count[0] != s[quadrant_index]->m_cv_count[0]
        || s[0]->m_cv_count[1] != s[quadrant_index]->m_cv_count[1]
        )
      {
        return false;
      }
    }
  }

  s[0]->SetDomain(0, 0.0, 0.5);
  s[1]->SetDomain(0, 0.5, 1.0);
  s[2]->SetDomain(0, 0.5, 1.0);
  s[3]->SetDomain(0, 0.0, 0.5);

  s[0]->SetDomain(1, 0.0, 0.5);
  s[1]->SetDomain(1, 0.0, 0.5);
  s[2]->SetDomain(1, 0.5, 1.0);
  s[3]->SetDomain(1, 0.5, 1.0);

  ON_NurbsSurface* bottom = Internal_MergeC2Neighbors(nurbs_surface_type, 0, s[0], s[1]);
  if (nullptr == bottom)
    return false;
  ON_NurbsSurface* top = Internal_MergeC2Neighbors(nurbs_surface_type, 0, s[3], s[2]);
  if (nullptr == top)
  {
    delete bottom;
    return false;
  }
   
  m_surface = Internal_MergeC2Neighbors(nurbs_surface_type, 1, bottom, top);
  delete bottom;
  delete top;
  if (nullptr == m_surface)
    return false;

  for (unsigned int quadrant_index = 0; quadrant_index < 4; quadrant_index++)
  {
    delete m_quadrants[quadrant_index]->m_surface;
    m_quadrants[quadrant_index]->m_surface = nullptr;
    m_quadrants[quadrant_index]->m_parent = nullptr;
    ON_SubDLimitSurfaceFragment::ReturnSurfaceFragment(m_quadrants[quadrant_index]);
    m_quadrants[quadrant_index] = nullptr;
  }

  return true;
}

const ON_SubDLimitSurfaceFragment ON_SubDLimitSurfaceFragment::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDLimitSurfaceFragment);

class Internal_SubDNurbsPatchGetter
{
public:
  Internal_SubDNurbsPatchGetter(
    const ON_SubD& subd,
    unsigned int patch_density,
    ON_SubD::NurbsSurfaceType nurbs_surface_type,
    ON__UINT_PTR nurbs_callback_context,
    bool(*nurbs_callback_function)(ON__UINT_PTR, const ON_SubDFaceRegion&, class ON_NurbsSurface*)
    )
    : m_subd(subd)
    , m_patch_density(patch_density)
    , m_nurbs_surface_type(ON_SubD::NurbsSurfaceType::Unset == nurbs_surface_type ? ON_SubD::NurbsSurfaceType::Medium : nurbs_surface_type)
    , m_nurbs_callback_context(nurbs_callback_context)
    , m_nurbs_callback_function(nurbs_callback_function)
    {}

  const ON_SubD& m_subd;

  const unsigned int m_patch_density = 2;
  const ON_SubD::NurbsSurfaceType m_nurbs_surface_type = ON_SubD::NurbsSurfaceType::Unset;

  // m_current_face_region identifies the current region being accumulated in m_fragment_tree
  // and is set in Internal_SubDNurbsPatchGetter::BeginFaceCallback().
  // If the level 0 face is a quad, then m_fragments_face_region.m_subdivision_count = 0;
  // If the level 0 face is an N-gon (N != 4), then m_fragments_face_region.m_subdivision_count = 1.
  ON_SubDFaceRegion m_current_face_region;

  enum : unsigned int
  {
    fragments_acculator_capacity = 5
  };
  ON_SubDLimitSurfaceFragment* m_fragment_tree = nullptr;
  ON_SubDLimitSurfaceFragment* FragmentLeaf(
    const ON_SubDFaceRegion& face_region
  );


  void AddOutputSurface(
    const ON_SubDFaceRegion& face_region,
    ON_NurbsSurface* output_surface
  );

  ON__UINT_PTR m_nurbs_callback_context = 0;
  bool(*m_nurbs_callback_function)(ON__UINT_PTR, const ON_SubDFaceRegion&, class ON_NurbsSurface*) = nullptr;

  unsigned int m_bicubic_span_count = 0;

  void AddPatch(
    const ON_SubDLimitNurbsFragment* patch_fragment
    );

  void ConvertFragmentsToSurfaces();

  void ConvertPatchToSurfaces(
    const ON_SubDLimitNurbsFragment& patch_fragment
    );

  static bool BeginFaceCallback(
    ON__UINT_PTR  context, // contest = Internal_SubDNurbsPatchGetter*
    const ON_SubDFaceRegion& face_region
    //,
    //const class ON_SubDFace* level0_face_OBSOLETE, 
    //const class ON_SubDFace* level1_face_OBSOLETE, 
    //unsigned int level1_face_region_index_OBSOLETE
  );


  static bool GetLimitSurfaceInPatchesCallback(
    ON__UINT_PTR  context, // contest = Internal_SubDNurbsPatchGetter*
    const ON_SubDLimitNurbsFragment* patch_fragment
    );

  static bool AddToSurfaceArrayCallback1(
    ON__UINT_PTR context, // ON_SimpleArray<ON_NurbsSurface*>*
    const ON_SubDFaceRegion& face_region,
    class ON_NurbsSurface* nurbs_surface
  );

  static bool AddToSurfaceArrayCallback2(
    ON__UINT_PTR context, // ON_SimpleArray<ON_SubDFaceRegionAndNurbs>*
    const ON_SubDFaceRegion& face_region,
    class ON_NurbsSurface* nurbs_surface
  );

private:
  unsigned int m_new_subdivision_edge_id = 0;

  wchar_t* AppendUnsigned(
    wchar_t prefix,
    unsigned int i,
    wchar_t* s,
    wchar_t* send
    );
private:
  Internal_SubDNurbsPatchGetter() = delete;
  Internal_SubDNurbsPatchGetter(const Internal_SubDNurbsPatchGetter&) = delete;
  Internal_SubDNurbsPatchGetter& operator=(const Internal_SubDNurbsPatchGetter&) = delete;
};

bool Internal_SubDNurbsPatchGetter::BeginFaceCallback(
  ON__UINT_PTR  context, // contest = Internal_SubDNurbsPatchGetter*
  const ON_SubDFaceRegion& face_region
)
{
  Internal_SubDNurbsPatchGetter* p = (Internal_SubDNurbsPatchGetter*)context;
  if (nullptr == p)
    return true;

  // Flush accumulated fragments from previous face
  p->ConvertFragmentsToSurfaces(); 

  p->m_current_face_region = face_region;
  return true;
}

bool Internal_SubDNurbsPatchGetter::GetLimitSurfaceInPatchesCallback(
  ON__UINT_PTR context,
  const ON_SubDLimitNurbsFragment* patch_fragment
  )
{
  ((Internal_SubDNurbsPatchGetter*)context)->AddPatch(patch_fragment);
  return true;
}

wchar_t* Internal_SubDNurbsPatchGetter::AppendUnsigned(
  wchar_t prefix,
  unsigned int i,
  wchar_t* s,
  wchar_t* send
  )
{
  if ( 0 != prefix && s < send)
    *s++ = prefix;
  wchar_t buffer[64];
  wchar_t* sdigit = buffer;
  wchar_t* sdigit1 = sdigit + (sizeof(buffer)/sizeof(buffer[0]));
  for ( *sdigit++ = 0; sdigit < sdigit1; sdigit++ )
  {
    *sdigit = (wchar_t)('0' + (i%10));
    i /= 10;
    if (0 == i)
    {
      while ( s < send && 0 != (*s = *sdigit--) )
        s++;
      return s;
    }
  }
  return s;
}

ON_SubDLimitSurfaceFragment* ON_SubDLimitSurfaceFragment::Parent()
{
  return m_parent;
}

ON_SubDLimitSurfaceFragment* ON_SubDLimitSurfaceFragment::Quadrant(unsigned int quadrant_index, bool bAllocateIfMissing)
{
  if (quadrant_index >= 4)
  {
    ON_SUBD_ERROR("Invalid quadrant_index value.");
    return nullptr;
  }
  if (nullptr == m_quadrants[quadrant_index] && bAllocateIfMissing)
  {
    m_quadrants[quadrant_index] = ON_SubDLimitSurfaceFragment::AllocateSurfaceFragment();
    if (nullptr != m_quadrants[quadrant_index])
    {
      m_quadrants[quadrant_index]->m_parent = this;
      m_quadrants[quadrant_index]->m_face_region = m_face_region;
      m_quadrants[quadrant_index]->m_face_region.Push(quadrant_index);
    }
  }
  return m_quadrants[quadrant_index];
}


ON_SubDLimitSurfaceFragment* Internal_SubDNurbsPatchGetter::FragmentLeaf(
  const ON_SubDFaceRegion& patch_face_region
) 
{
  const ON_SubDComponentRegion& fragment_fr = m_current_face_region.m_face_region;
  if (fragment_fr.m_level0_component_id == 0)
  {
    ON_SUBD_ERROR("m_fragments_face_region.m_level0_component_id not set.");
    return nullptr;
  }

  const ON_SubDComponentRegion& patch_fr = patch_face_region.m_face_region;
  if (fragment_fr.m_level0_component_id != patch_fr.m_level0_component_id)
  {
    ON_SUBD_ERROR("m_fragments_face_region.m_level0_component_id != patch_face_region.m_level0_component_id");
    return nullptr;
  }
  if (patch_fr.SubdivisionCount() < fragment_fr.SubdivisionCount())
  {
    ON_SUBD_ERROR("patch_face_region.SubdivisionCount() < m_fragments_face_region.SubdivisionCount()");
    return nullptr;
  }

  if (patch_fr.SubdivisionCount() > ON_SubDComponentRegionIndex::IndexCapacity)
  {
    // unreasonable number of subdivisions
    ON_SUBD_ERROR("patch_face_region.SubdivisionCount() > ON_SubDComponentRegionIndex::IndexCapacity");
    return nullptr;
  }

  for ( unsigned short i = 0; i < fragment_fr.SubdivisionCount(); i++ )
  {
    if (fragment_fr.m_region_index.m_index[i] != patch_fr.m_region_index.m_index[i])
    {
      ON_SUBD_ERROR("m_fragments_face_region.m_region_index[] differs from patch_face_region");
      return nullptr;
    }
  }

  if (nullptr == m_fragment_tree)
  {
    m_fragment_tree = ON_SubDLimitSurfaceFragment::AllocateSurfaceFragment();
    m_fragment_tree->m_face_region = m_current_face_region;
  }

  ON_SubDLimitSurfaceFragment* fragment_leaf = m_fragment_tree;
  ON_SubDComponentRegion r = fragment_fr;
  while (r.SubdivisionCount() < patch_fr.SubdivisionCount())
  {
    unsigned short quadrant_dex = patch_fr.m_region_index.Index(r.SubdivisionCount());
    if (quadrant_dex >= 4)
    {
      ON_SUBD_ERROR("patch_face_region.m_region_index[] value >= 4.");
      return nullptr;
    }
    r.PushAbsolute(quadrant_dex); // increments r.m_subdivision_count
    fragment_leaf = fragment_leaf->Quadrant(quadrant_dex, true);
    if (nullptr == fragment_leaf)
    {
      ON_SUBD_ERROR("fragment tree allocation error.");
      return nullptr;
    }
    if (0 != ON_SubDComponentRegion::CompareTypeIdMarkSubregion(&r, &fragment_leaf->m_face_region.m_face_region))
    {
      ON_SUBD_ERROR("corrupt fragment tree.");
      return nullptr;
    }
  }

  return fragment_leaf;
}


void Internal_SubDNurbsPatchGetter::AddPatch(
  const ON_SubDLimitNurbsFragment* patch_fragment
)
{
  if (nullptr == patch_fragment)
    return;

  // face_region identifies what part of the SubD region is represented by patch_fragment
  const ON_SubDFaceRegion face_region = patch_fragment->m_face_region;

  ON_SubDLimitNurbsFragment local_patch_fragment(*patch_fragment);
  patch_fragment = &local_patch_fragment;


  bool rc = false;
  for (;;)
  {
    if (m_current_face_region.m_face_region.m_level0_component_id == 0)
    {
      ON_SUBD_ERROR("m_fragments_face_region.m_level0_component_id == 0");
      break;
    }

    if (patch_fragment->m_face_region.m_face_region.m_level0_component_id == 0)
    {
      ON_SUBD_ERROR("patch_fragment->m_face_region.m_level0_component_id == 0");
      break;
    }

    const unsigned int max_bispan_count = patch_fragment->MaximumBispanCount();
    const unsigned int bispan_count = patch_fragment->SetBispanCount();
    if (0 == bispan_count || 0 == max_bispan_count)
    {
      ON_SUBD_ERROR("No bispans in patch_fragment.");
      break;
    }

    if ( 
      ON_SubD::NurbsSurfaceType::Small == m_nurbs_surface_type 
      || ON_SubD::NurbsSurfaceType::Unprocessed == m_nurbs_surface_type 
      )
    {
      // happens when debugging
      rc = true;
      break;
    }

    ON_SubDLimitSurfaceFragment* fragment_leaf = FragmentLeaf(patch_fragment->m_face_region);
    if (nullptr == fragment_leaf)
    {
      ON_SUBD_ERROR("Unable to get surface holder for patch_fragment->m_face_region.");
break;
    }

    if (nullptr != fragment_leaf->m_surface)
    {
      ON_SUBD_ERROR("fragment_leaf->m_surface is already set.");
      break;
    }

    // patch_fragment is part of the face we are currently patching
    if (bispan_count == max_bispan_count)
    {
      if (false == fragment_leaf->SetSurface(patch_fragment->GetSurface(nullptr)))
      {
        ON_SUBD_ERROR("Failed to set surface.");
        break;
      }
      local_patch_fragment = ON_SubDLimitNurbsFragment::Empty;
    }
    else
    {
      for (unsigned int quadrant_index = 0; quadrant_index < max_bispan_count; quadrant_index++)
      {
        if (ON_SubDLimitNurbsFragment::BispanType::None == patch_fragment->m_bispan_type[quadrant_index])
          continue;
        if (false == fragment_leaf->SetQuadrantSurface(patch_fragment->GetQuadrantSurface(quadrant_index, nullptr), quadrant_index))
        {
          ON_SUBD_ERROR("Failed to set quadrant surface.");
          continue;
        }
        local_patch_fragment.m_bispan_type[quadrant_index] = ON_SubDLimitNurbsFragment::BispanType::None;
      }
    }

    if (0 == local_patch_fragment.SetBispanCount())
      rc = true;

    while (nullptr != fragment_leaf && fragment_leaf->SetSurfaceFromQuadrants(m_nurbs_surface_type))
    {
      fragment_leaf = fragment_leaf->Parent();
    }

    break;
  }

  if (false == rc)
    ConvertFragmentsToSurfaces();

  // Convert this patch. Patches should all set to none if we are merging patches.)
  // Restore face and edge region information
  local_patch_fragment.m_face_region = face_region;

  ConvertPatchToSurfaces(*patch_fragment);

  return;
}

void Internal_SubDNurbsPatchGetter::ConvertFragmentsToSurfaces()
{
  if (nullptr == m_fragment_tree)
    return;
  ON_SimpleArray<ON_SubDLimitSurfaceFragment*> a(64);
  a.Append(m_fragment_tree);
  m_fragment_tree = nullptr;
  unsigned int a_count = a.UnsignedCount();
  while ( a_count > 0 )
  {
    for (unsigned int i = 0; i < a_count; i++)
    {
      ON_SubDLimitSurfaceFragment* f = a[i];
      if (nullptr == f)
        continue;
      a[i] = 0;
      if (f->m_surface)
      {
        AddOutputSurface(f->m_face_region, f->m_surface);
        f->m_surface = nullptr; // receiver must manage f->m_surface.
      }      
      for (unsigned int j = 0; j < 4; j++)
      {
        ON_SubDLimitSurfaceFragment* q = f->Quadrant(j, false);
        if (q)
          a.Append(q);
      }
      ON_SubDLimitSurfaceFragment::ReturnSurfaceFragment(f);
    }
    unsigned int k = a_count;
    const unsigned int kmax = a.UnsignedCount();
    a_count = 0;
    while (k < kmax)
      a[a_count++] = a[k++];
    a.SetCount(a_count);
  }
  m_current_face_region = ON_SubDFaceRegion::Empty;
}


void Internal_SubDNurbsPatchGetter::AddOutputSurface(
  const ON_SubDFaceRegion& face_region,
  ON_NurbsSurface* output_surface
)
{
  if (nullptr == output_surface)
    return;

  if ( ON_SubD::NurbsSurfaceType::Unprocessed != m_nurbs_surface_type )
  {
    output_surface->ClampEnd(0, 2);
    output_surface->ClampEnd(1, 2);
    Internal_CheckNurbsSurfaceCVs(*output_surface);
  }

  m_nurbs_callback_function(m_nurbs_callback_context, face_region, output_surface);
}

void Internal_SubDNurbsPatchGetter::ConvertPatchToSurfaces(
  const ON_SubDLimitNurbsFragment& patch_fragment
  )
{
  // Exports patches as is with no merging.
  unsigned int bispan_count = patch_fragment.SetBispanCount();
  if (bispan_count <= 0)
    return;

  unsigned int maximum_bispan_count = patch_fragment.MaximumBispanCount();
  ON_SubDComponentRegion central_edge_regions[4] = {};
  ON_SubDFaceRegion local_face_region = patch_fragment.m_face_region;
  for (unsigned int quadrant_index = 0; quadrant_index < maximum_bispan_count; quadrant_index++)
  {
    if (ON_SubDLimitNurbsFragment::BispanType::None == patch_fragment.m_bispan_type[quadrant_index])
      continue;
    ON_NurbsSurface* output_surface = patch_fragment.GetQuadrantSurface(quadrant_index, nullptr);
    if (nullptr == output_surface)
      continue;
    if (maximum_bispan_count > 1)
    {
      // region information must be subdivided
      local_face_region.Push(quadrant_index);      
      const unsigned short central_edge_region_subdivision_count = local_face_region.m_face_region.SubdivisionCount();

      if (0 == central_edge_regions[quadrant_index].m_level0_component_id)
        central_edge_regions[quadrant_index] = Internal_CreateSubdivisionEdgeRegion(central_edge_region_subdivision_count,false);
      local_face_region.m_edge_region[(quadrant_index + 1) % 4] = central_edge_regions[quadrant_index];
      local_face_region.m_edge_region[(quadrant_index + 1) % 4].m_level0_component = central_edge_regions[quadrant_index].m_level0_component.ToggleMark();

      const unsigned int cdex3 = (quadrant_index + 3) % 4;
      if (0 == central_edge_regions[cdex3].m_level0_component_id)
        central_edge_regions[cdex3] = Internal_CreateSubdivisionEdgeRegion(central_edge_region_subdivision_count,false);
      local_face_region.m_edge_region[(quadrant_index+2)%4] = central_edge_regions[cdex3];
    }

    AddOutputSurface(local_face_region, output_surface);

    if (maximum_bispan_count > 1)
      local_face_region = patch_fragment.m_face_region;
  }
}


unsigned int ON_SubD::GetLimitSurfaceNurbs(
  const class ON_SubDDisplayParameters& display_parameters,
  ON_SubD::NurbsSurfaceType nurbs_surface_type,
  ON__UINT_PTR callback_context,
  bool(*nurbs_callback_function)(ON__UINT_PTR, const ON_SubDFaceRegion&, class ON_NurbsSurface*)
) const
{
  // TODO: Restructure the code to support this callback function.
  Internal_SubDNurbsPatchGetter patch_getter(
    *this,
    display_parameters.m_display_density,
    nurbs_surface_type,
    callback_context,
    nurbs_callback_function
    );

  GetLimitSurfaceNurbsFragments(
    display_parameters,
    (ON__UINT_PTR)&patch_getter,
    Internal_SubDNurbsPatchGetter::BeginFaceCallback,
    Internal_SubDNurbsPatchGetter::GetLimitSurfaceInPatchesCallback
    );

  // Flush the final batch of patches.
  patch_getter.ConvertFragmentsToSurfaces();

  return patch_getter.m_bicubic_span_count;
}

bool Internal_SubDNurbsPatchGetter::AddToSurfaceArrayCallback1(
  ON__UINT_PTR context, // ON_SimpleArray<ON_NurbsSurface*>*
  const ON_SubDFaceRegion& face_region,
  class ON_NurbsSurface* nurbs_surface
)
{
  if (nullptr != nurbs_surface && 0 != context)
  {
    ON_wString region_id = face_region.ToString();
    nurbs_surface->SetUserString(L"SubDRegionId", region_id);
    ((ON_SimpleArray< ON_NurbsSurface* >*)context)->Append(nurbs_surface);
  }
  return true;
}

bool Internal_SubDNurbsPatchGetter::AddToSurfaceArrayCallback2(
  ON__UINT_PTR context, // ON_SimpleArray<ON_SubDFaceRegionAndNurbs>*
  const ON_SubDFaceRegion& face_region,
  class ON_NurbsSurface* nurbs_surface
)
{
  if (nullptr != nurbs_surface && 0 != context)
  {
    ON_SubDFaceRegionAndNurbs& e = ((ON_SimpleArray< ON_SubDFaceRegionAndNurbs >*)context)->AppendNew();
    e.m_face_region = face_region;
    e.m_nurbs_surface = nurbs_surface;
  }
  return true;
}

unsigned int ON_SubD::GetLimitSurfaceNurbs(
  const class ON_SubDDisplayParameters& display_parameters,
  ON_SubD::NurbsSurfaceType nurbs_surface_type,
  ON_SimpleArray< ON_NurbsSurface* >& patches
  ) const
{
  return GetLimitSurfaceNurbs(display_parameters, nurbs_surface_type, (ON__UINT_PTR)&patches, Internal_SubDNurbsPatchGetter::AddToSurfaceArrayCallback1);
}

unsigned int ON_SubD::GetLimitSurfaceNurbs(
  const class ON_SubDDisplayParameters& display_parameters,
  ON_SubD::NurbsSurfaceType nurbs_surface_type,
  ON_SimpleArray< ON_SubDFaceRegionAndNurbs >& patches
  ) const
{
  return GetLimitSurfaceNurbs(display_parameters, nurbs_surface_type, (ON__UINT_PTR)&patches, Internal_SubDNurbsPatchGetter::AddToSurfaceArrayCallback2);
}


static bool Internal_ToBrep(
  ON_SimpleArray< ON_SubDFaceRegionAndNurbs >& patches,
  ON_Brep& brep
);

ON_Brep* ON_SubD::GetLimitSurfaceNurbs(
  const class ON_SubDDisplayParameters& display_parameters,
  ON_Brep* destination_brep  
  ) const
{
  if (nullptr != destination_brep)
    destination_brep->Destroy();
  ON_SimpleArray< ON_SubDFaceRegionAndNurbs > patches;
  GetLimitSurfaceNurbs(display_parameters, ON_SubD::NurbsSurfaceType::Large, (ON__UINT_PTR)&patches, Internal_SubDNurbsPatchGetter::AddToSurfaceArrayCallback2);
  const unsigned int face_count = patches.UnsignedCount();
  if (face_count <= 0)
    return nullptr;

  ON_Brep* brep
    = (nullptr != destination_brep)
    ? destination_brep
    : ON_Brep::New();
  if (false == Internal_ToBrep(patches, *brep))
  {
    if (brep != destination_brep)
      delete brep;
    brep = nullptr;
  }
  return brep;
}

static bool Internal_ToBrep(
  ON_SimpleArray< ON_SubDFaceRegionAndNurbs >& patches,
  ON_Brep& brep
)
{
  const unsigned int face_count = patches.UnsignedCount();
  if (face_count <= 0)
    return false;

  const unsigned int split_edge_mark = 0xFFFFFFFF;
  const unsigned int bad_edge_mark = 0xFFFFFFFE;
  const unsigned int bad_vertex_mark = 0xFFFFFFFD;


  ON_SimpleArray<ON_2udex> vertex_map(3 * face_count + 32);
  ON_SimpleArray<ON_3udex> edge_map(4 * face_count + 32);
  ON_2udex vm(0, 0);
  ON_3udex em(0, 0, 0);

  // Begin working on building vertex_map[] and edge_map[]
  for (unsigned int fi = 0; fi < face_count; fi++)
  {
    const ON_SubDFaceRegionAndNurbs& r = patches[fi];
    ON_NurbsSurface* nurbs_surface = r.m_nurbs_surface;
    if (nullptr == nurbs_surface)
      continue;

    for (int i = 0; i < 4; i++)
    {
      vm.i = r.m_face_region.m_vertex_id[i];
      vm.j = fi;
      if ( 0 != vm.i)
        vertex_map.Append(vm);
      
      em.i = r.m_face_region.m_edge_region[i].m_level0_component_id;
      if (0 != em.i)
      {
        em.j = r.m_face_region.m_edge_region[i].m_region_index.ToCompressedRegionIndex();
        em.k = fi;
        edge_map.Append(em);
      }
    }
  }

  // Remove duplicates from vertex_map[]
  vertex_map.QuickSort(ON_2udex::DictionaryCompare);
  unsigned int count0 = vertex_map.UnsignedCount();
  unsigned int prev_id = 0;
  unsigned int brep_vertex_count = 0;
  for (unsigned int i = 0; i < count0; i++)
  {
    vm = vertex_map[i];
    if (vm.i <= prev_id)
      continue;
    prev_id = vm.i;
    vertex_map[brep_vertex_count++] = vm;
  }
  vertex_map.SetCount(brep_vertex_count);

  // Create brep vertices and set vertex_map[].j  to brep index values so we
  // can use vertex_map[] to go from a subd vertex id to a brep vertex index.
  brep.m_V.Reserve(brep_vertex_count);
  for (unsigned int i = 0; i < brep_vertex_count; i++)
  {
    ON_3dPoint P(ON_3dPoint::NanPoint);
    vm = vertex_map[i];
    const unsigned fi = vm.j;
    vertex_map[i].j = bad_vertex_mark; // will be updated below if everything works right
    vm.j = bad_vertex_mark;
    const ON_SubDFaceRegionAndNurbs& r = patches[fi];
    if (nullptr != r.m_nurbs_surface)
    {
      for (int j = 0; j < 4; j++)
      {
        if (vm.i == r.m_face_region.m_vertex_id[j])
        {
          const double s = r.m_nurbs_surface->Domain(0)[(1==j||2==j) ? 1 : 0];
          const double t = r.m_nurbs_surface->Domain(1)[(j > 1) ? 1 : 0];
          P = r.m_nurbs_surface->PointAt(s, t);
          break;
        }
      }
    }
    if (false == P.IsValid())
    {
      ON_SUBD_ERROR("Unable to calculate vertex location.");
    }
    vm.j = brep.m_V.UnsignedCount();
    vertex_map[i].j = vm.j;
    brep.NewVertex(P, 0.0);
  }

  // Remove duplicates from edge_map[]

  edge_map.QuickSort(ON_3udex::DictionaryCompare);
  count0 = edge_map.UnsignedCount();
  unsigned int edge_map_count = 0;
  ON_3udex prev_em(0, 0, 0);
  for (unsigned int i = 0; i < count0; i++)
  {
    em = edge_map[i];
    if (prev_em.i == em.i && prev_em.j == em.j)
      continue;
    prev_em = em;
    edge_map[edge_map_count++] = em;
  }
  // NOTE: final brep edge count <= edge_map_count.
  edge_map.SetCount(edge_map_count);

  // create the edges
  brep.m_C3.Reserve(edge_map_count);
  brep.m_E.Reserve(edge_map_count);
  unsigned int brep_edge_count = 0;
  for (unsigned int i = 0; i < edge_map_count; i++)
  {
    em = edge_map[i];
    const unsigned int fi = em.k;
    em.k = bad_edge_mark;
    edge_map[i].k = bad_edge_mark; // will be update below if we find all the parts.

    if (0 == em.j && i + 2 < edge_map_count && em.i == edge_map[i + 1].i && em.i == edge_map[i + 2].i)
    {
      // This edge is shared between a subd quad face and N-gon with N != 4.
      // The N-gon had to be subdivided one time to create quads and the
      // edge will be split on the quad face side.
      edge_map[i].k = split_edge_mark; // marker on the quad face edge for this situation
      continue;
    }

    ON_Curve* edge_curve = nullptr;
    const ON_SubDFaceRegionAndNurbs& r = patches[fi];
    const ON_NurbsSurface* nurbs_surface = r.m_nurbs_surface;
    unsigned vertex_id[2] = {};
    int brep_vi[2] = { -1,-1 };
    if (nullptr != nurbs_surface)
    {
      for (int j = 0; j < 4; j++)
      {
        if (em.i == r.m_face_region.m_edge_region[j].m_level0_component_id)
        {
          const bool bRevEdge = (0 != r.m_face_region.m_edge_region[j].m_level0_component.ComponentMark());
          vertex_id[bRevEdge?1:0] = r.m_face_region.m_vertex_id[j];
          vertex_id[bRevEdge?0:1] = r.m_face_region.m_vertex_id[(j+1)%4];

          for (int k = 0; k < 2; k++)
          {
            vm.i = vertex_id[k];
            vm.j = 0;
            int mapdex = vertex_map.BinarySearch(&vm, ON_2udex::CompareFirstIndex);
            if (mapdex >= 0)
            {
              vm = vertex_map[mapdex];
              brep_vi[k] = (int)vm.j;
            }
          }
          const int iso_dir = j % 2;
          const double c = nurbs_surface->Domain(1 - iso_dir)[(1==j || 2==j) ? 1 : 0];
          edge_curve = nurbs_surface->IsoCurve(iso_dir, c);
          bool bReverseCurve = (j >= 2);
          if (bRevEdge)
            bReverseCurve = !bReverseCurve;
          if (bReverseCurve && nullptr != edge_curve)
            edge_curve->Reverse();
          break;
        }
      }
    }

    if (nullptr != edge_curve)
    {
      for (int k = 0; k < 2; k++)
      {
        if (brep_vi[k] >= 0)
          continue;
        ON_SUBD_ERROR("brep edge vertex creation error.");
        ON_3dPoint P(ON_3dPoint::NanPoint);
        if (nullptr != edge_curve)
          P = (0 == k) ? edge_curve->PointAtStart() : edge_curve->PointAtEnd();
        brep_vi[k] = brep.m_V.Count();
        brep.NewVertex(P, 0.0);
      }
      int c3i = -1;
      if (nullptr != edge_curve)
      {
        c3i = brep.m_C3.Count();
        brep.m_C3.Append(edge_curve);
      }
      em.k = brep.m_E.UnsignedCount();
      edge_map[i].k = em.k;
      brep.NewEdge(brep.m_V[brep_vi[0]], brep.m_V[brep_vi[1]], c3i, nullptr, 0.0);
      brep_edge_count++;
    }
    else
    {
      ON_SUBD_ERROR("brep edge creation error.");
    }
  }
   
  // Now build faces
  ON_NurbsCurve trim_curve;
  ON_2dPoint trim_curve_cvs[2];
  double trim_curve_knots[2] = { 0.0,1.0 };
  trim_curve.m_dim = 2;
  trim_curve.m_is_rat = 0;
  trim_curve.m_order = 2;
  trim_curve.m_cv_count = 2;
  trim_curve.m_cv_stride = 2;
  trim_curve.m_cv = &trim_curve_cvs[0].x;
  trim_curve.m_knot = trim_curve_knots;
  
  for (unsigned int fi = 0; fi < face_count; fi++)
  {
    const ON_SubDFaceRegionAndNurbs& r = patches[fi];
    if (nullptr == r.m_nurbs_surface)
      continue;

    const ON_Interval srf_domain[2] = { r.m_nurbs_surface->Domain(0),r.m_nurbs_surface->Domain(1) };
    const ON_2dPoint srf_uv[4] =
    {
      ON_2dPoint(srf_domain[0][0], srf_domain[1][0]),
      ON_2dPoint(srf_domain[0][1], srf_domain[1][0]),
      ON_2dPoint(srf_domain[0][1], srf_domain[1][1]),
      ON_2dPoint(srf_domain[0][0], srf_domain[1][1])
    };

    ON_3udex brep_em[8] = {};
    bool trimRev3d[8] = {};
    ON_2udex brep_vm[8] = {};

    // Fallback if we fail to get complete information
    bool bFail = false;
    int fail_vid[4] = { -1,-1,-1,-1 };
    int fail_eid[4] = { -1,-1,-1,-1 };
    bool fail_bRev3d[4] = { false,false,false,false };

    for (int i = 0; i < 4; i++)
    {
      brep_vm[2 * i] = ON_2udex::Zero;
      brep_vm[2 * i + 1] = ON_2udex::Zero;
      brep_em[2 * i] = ON_3udex::Zero;
      brep_em[2 * i + 1] = ON_3udex::Zero;

      vm.i = r.m_face_region.m_vertex_id[i];
      vm.j = 0;
      int k = vertex_map.BinarySearch(&vm, ON_2udex::CompareFirstIndex);
      if (k < 0)
      {
        bFail = true;
      }
      else
      {
        vm = vertex_map[k];
        if (vm.j < bad_vertex_mark)
        {
          brep_vm[2 * i] = vm;
          fail_vid[i] = vm.j;
        }
        else
          bFail = true;
      }

      const bool bRev3d = (0 != r.m_face_region.m_edge_region[i].m_level0_component.ComponentMark()) ? true : false;
      trimRev3d[2 * i] = bRev3d;
      em.i = r.m_face_region.m_edge_region[i].m_level0_component_id;
      em.j = r.m_face_region.m_edge_region[i].m_region_index.ToCompressedRegionIndex();
      em.k = 0;
      k = edge_map.BinarySearch(&em, ON_3udex::CompareFirstAndSecondIndex);
      if (k < 0)
      {
        bFail = true;
      }
      else
      {
        em = edge_map[k];
        if (split_edge_mark == em.k)
        {
          if (0 != em.j || (unsigned)(k + 2) >= edge_map_count || em.i != edge_map[k+1].i || em.i != edge_map[k+2].i )
            bFail = true;
          else
          {
            trimRev3d[2 * i+1] = bRev3d;
            if (bRev3d)
            {
              brep_em[2 * i] = edge_map[k + 2];
              brep_em[2 * i + 1] = edge_map[k + 1];
            }
            else
            {
              brep_em[2 * i] = edge_map[k + 1];
              brep_em[2 * i + 1] = edge_map[k + 2];
            }
          }
        }
        else if ( em.k < bad_edge_mark )
        {
          brep_em[2 * i] = em;
          fail_eid[i] = em.k;
          fail_bRev3d[i] = bRev3d;
        }
        else
        {
          bFail = true;
        }
      }
    }

    if (bFail)
    {
      // Use simple face maker and end up with interior naked edges
      brep.NewFace(
       r.m_nurbs_surface,
       fail_vid,
       fail_eid,
       fail_bRev3d
       );
    }
    else
    {
      const int si = brep.m_S.Count();
      brep.m_S.Append(r.m_nurbs_surface);
      ON_BrepFace& brep_face = brep.NewFace(si);
      ON_BrepLoop& brep_loop = brep.NewLoop(ON_BrepLoop::TYPE::outer, brep_face);
      for (int i = 0; i < 4; i++)
      {
        trim_curve_cvs[0] = srf_uv[i];
        trim_curve_cvs[1] = srf_uv[(i+1)%4];

        if (
          0 == brep_em[2 * i].i
          || brep_em[2 * i].k >= brep_edge_count
          )
        {
          ON_SUBD_ERROR("Bog in SubD to joined brep code.");
          continue;
        }
          
        ON_NurbsCurve* curve2d[2] = {};
        if (
          0 != brep_em[2 * i].j
          && brep_em[2 * i].i == brep_em[2 * i + 1].i 
          && 0 != brep_em[2 * i + 1].j
          && brep_em[2 * i + 1].k < brep_edge_count
          )
        {
          // This face/edge corresponds to a subd quad face and edge that
          // is shared by subd N-gon with N != 4.
          // This side of the brep face needs 2 brep edges so 
          // and will join to 2 brep faces coming from level 1 subdivision
          // quads in the original subd.
          const ON_2dPoint Q(
            (trim_curve_cvs[0].x == trim_curve_cvs[1].x) ? trim_curve_cvs[0].x : 0.5*(trim_curve_cvs[0].x + trim_curve_cvs[1].x),
            (trim_curve_cvs[0].y == trim_curve_cvs[1].y) ? trim_curve_cvs[0].y : 0.5*(trim_curve_cvs[0].y + trim_curve_cvs[1].y)
          );
          trim_curve_cvs[1] = Q;
          curve2d[0] = trim_curve.Duplicate();
          trim_curve_cvs[0] = Q;
          trim_curve_cvs[1] = srf_uv[(i+1)%4];
          curve2d[1] = trim_curve.Duplicate();
        }
        else
        {
          curve2d[0] = trim_curve.Duplicate();
          curve2d[1] = nullptr;
        }
        for ( int k = 0; k < 2; k++)
        {
          ON_NurbsCurve* c2 = curve2d[k];
          if (nullptr == c2)
            continue;
          em = brep_em[2 * i + k];
          const bool bRev3d = trimRev3d[2 * i + k];
          ON_BrepEdge& brep_edge = brep.m_E[em.k];
          ON_Interval trim_domain = brep_edge.Domain();
          if (bRev3d)
            trim_domain.Reverse();
          c2->SetDomain(trim_domain[0],trim_domain[1]);
          int c2i = brep.m_C2.Count();
          brep.m_C2.Append(c2);
          ON_BrepTrim& brep_trim = brep.NewTrim(brep_edge, bRev3d, brep_loop, c2i);
          brep_trim.m_tolerance[0] = 0.0;
          brep_trim.m_tolerance[1] = 0.0;
        }
      }

      //Internal_DebugLocations(brep_face);
    }
  }

  trim_curve.m_cv = nullptr;
  trim_curve.m_knot = nullptr;


  return true;
}
