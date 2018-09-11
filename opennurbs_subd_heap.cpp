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


static void* ON_SubD__Allocate(size_t sz)
{
  if (0 == sz)
    return nullptr;

  // double array allocation is used to insure the memory
  // returned by new is properly aligned for any type.
  double* a;
  size_t sz1 = sz % sizeof(a[0]);
  if (sz1 > 0)
    sz += (sizeof(a[0]) - sz1);
  a = new(std::nothrow) double[sz];

  if (nullptr == a)
    return ON_SUBD_RETURN_ERROR(nullptr);

  return a;
}

static void ON_SubD__Free(void* p)
{
  if (nullptr != p)
  {
    double* a = (double*)p;
    delete[] a;
  }
}



//////////////////////////////////////////////////////////////////////////
//
// ON_SubD_FixedSizeHeap
//

unsigned int ON_SubD_FixedSizeHeap::m__sn_factory = 0;

ON_SubD_FixedSizeHeap::~ON_SubD_FixedSizeHeap()
{
  Destroy();
}


void ON_SubD_FixedSizeHeap::Destroy()
{
  Reset();
  m_v_capacity = 0;
  m_e_capacity = 0;
  m_f_capacity = 0;
  m_p_capacity = 0;
  void* p[4] = { m_v, m_e, m_f, m_p };
  m_v = nullptr;
  m_e = nullptr;
  m_f = nullptr;
  m_p = nullptr;
  ON_SubD__Free(p[0]);
  ON_SubD__Free(p[1]);
  ON_SubD__Free(p[2]);
  ON_SubD__Free(p[3]);
}

void ON_SubD_FixedSizeHeap::Reset()
{
  m_v_index = 0;
  m_e_index = 0;
  m_f_index = 0;
  m_p_index = 0;
}

bool ON_SubD_FixedSizeHeap::InUse() const
{
  return (m_v_index > 0 || m_e_index > 0 || m_f_index>0 || m_p_index>0);
}


bool ON_SubD_FixedSizeHeap::ReserveSubDWorkspace(
  size_t vertex_capacity,
  size_t edge_capacity,
  size_t face_capacity,
  size_t array_capacity
  )
{
  if (0 == vertex_capacity || (0 == edge_capacity && 0 == face_capacity && 0 == array_capacity))
  {
    Destroy();
    return true;
  }

  if (m_v_capacity >= vertex_capacity && m_e_capacity >= edge_capacity && m_f_capacity >= face_capacity && m_p_capacity >= array_capacity)
  {
    Reset();
    return true;
  }

  Destroy();

  size_t max_capacity = 0xFFFFFFF0U;
  if (vertex_capacity > max_capacity || edge_capacity > max_capacity || face_capacity > max_capacity || array_capacity > max_capacity)
    return ON_SUBD_RETURN_ERROR(false);

  for (;;)
  {
    m_v = (ON_SubDVertex*)ON_SubD__Allocate(vertex_capacity*sizeof(m_v[0]));
    if (nullptr == m_v && vertex_capacity > 0)
      break;
    m_e = (ON_SubDEdge*)ON_SubD__Allocate(edge_capacity*sizeof(m_e[0]));
    if (nullptr == m_e && edge_capacity > 0)
      break;
    m_f = (ON_SubDFace*)ON_SubD__Allocate(face_capacity*sizeof(m_f[0]));
    if (nullptr == m_f && face_capacity > 0)
      break;
    m_p = (ON__UINT_PTR*)ON_SubD__Allocate(array_capacity*sizeof(m_p[0]));
    if (nullptr == m_p && array_capacity > 0)
      break;

    m_v_capacity = (unsigned int)vertex_capacity;
    m_e_capacity = (unsigned int)edge_capacity;
    m_f_capacity = (unsigned int)face_capacity;
    m_p_capacity = (unsigned int)array_capacity;
    return true;
  }

  Destroy();

  return ON_SUBD_RETURN_ERROR(false);
}


bool ON_SubD_FixedSizeHeap::ReserveSubDWorkspace(
  ON_SubD::SubDType subdivision_type,
  unsigned int extraordinary_valence
  )
{
  if (0 == extraordinary_valence)
  {
    Destroy();
    return true;
  }

  // capacity depends on extraordinary_valence and subdivision_type

  bool bTri = (ON_SubD::SubDType::TriLoopWarren == subdivision_type);

  unsigned int ordinary_valence = bTri ? 6 : 4;

  if (extraordinary_valence < ordinary_valence)
    extraordinary_valence = ordinary_valence;

  // When ON_SubD::FacetType::Unset == facet_type,
  // the maximum of quad or tri capcacity is used.
  // For all but the face array, tri capacity < quad capacity.

  const unsigned int v_capacity
    = bTri
    ? (extraordinary_valence + 6)
    : (2 * extraordinary_valence + 8); // quads

  const unsigned int e_capacity
    = bTri
    ? (extraordinary_valence + 14)
    : (3 * extraordinary_valence + 12); // quads or unset
  
  // const unsigned int f_capacity = extraordinary_valence + 7 for tris
  // const unsigned int f_capacity = extraordinary_valence + 5 for quads
  // 7 is alwasy used to accomodate unset as well
  const unsigned int f_capacity = extraordinary_valence + 7;

  const unsigned int p_capacity = 2*(ordinary_valence*v_capacity + (extraordinary_valence - ordinary_valence));

  return ReserveSubDWorkspace(v_capacity, e_capacity, f_capacity, p_capacity);
}


ON_SubDVertex* ON_SubD_FixedSizeHeap::AllocateVertex(
  const double vertexP[3],
  unsigned int edge_capacity,
  unsigned int face_capacity
  )
{
  if (nullptr == m_v || m_v_index >= m_v_capacity)
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (edge_capacity + face_capacity + m_p_index >= m_p_capacity )
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON__UINT_PTR* a = nullptr;
  if (0 != edge_capacity || 0 != face_capacity)
  {
    if ( edge_capacity > 0xFFFFu)
      return ON_SUBD_RETURN_ERROR(nullptr);
    if ( face_capacity > 0xFFFFu)
      return ON_SUBD_RETURN_ERROR(nullptr);
    a = AllocatePtrArray(edge_capacity + face_capacity, true);
    if (nullptr == a)
      return ON_SUBD_RETURN_ERROR(nullptr);
  }

  ON_SubDVertex* v = m_v + m_v_index;
  memset(v, 0, sizeof(*v));
  if (m_v_index > 0)
  {
    // code in ON_SubDFaceNeighborhood.Subdivide() relies on 
    // m_next_vertex being set this way.
    m_v[m_v_index - 1].m_next_vertex = v;
    v->m_prev_vertex = &m_v[m_v_index - 1];
  }
  v->m_id = ++m_v_index;

  if (nullptr != vertexP)
  {
    v->m_P[0] = vertexP[0];
    v->m_P[1] = vertexP[1];
    v->m_P[2] = vertexP[2];
  }

  if (edge_capacity > 0)
  {
    v->m_edge_capacity = (unsigned short)edge_capacity;
    v->m_edges = (ON_SubDEdgePtr*)a;
    a += edge_capacity;
  }
  if (face_capacity > 0)
  {
    v->m_face_capacity = (unsigned short)face_capacity;
    v->m_faces = (const ON_SubDFace**)a;
  }
  a = 0;

  return v;
}

ON_SubDVertex* ON_SubD_FixedSizeHeap::AllocateVertex(
  const ON_SubDVertex* vertex0,
  ON_SubD::SubDType subdivision_type,
  bool bUseSavedSubdivisionPoint,
  unsigned int edge_capacity,
  unsigned int face_capacity
  )
{
  if ( nullptr == vertex0)
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  double subdP[3];
  if (false == vertex0->GetSubdivisionPoint(subdivision_type,bUseSavedSubdivisionPoint,subdP))
    return ON_SUBD_RETURN_ERROR(nullptr);
  ON_SubDVertex* v1 = AllocateVertex(subdP,edge_capacity,face_capacity);
  
  if ( nullptr == v1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  v1->m_level = vertex0->m_level+1;
  
  v1->m_vertex_tag = vertex0->m_vertex_tag;

  //if ( ON_SubD::SubDType::QuadCatmullClark == subdivision_type)
  //  v1->m_vertex_facet_type = ON_SubD::VertexFacetType::Quad;
  //else if ( ON_SubD::SubDType::TriLoopWarren == subdivision_type)
  //  v1->m_vertex_facet_type = ON_SubD::VertexFacetType::Tri;

  if (bUseSavedSubdivisionPoint && subdivision_type == vertex0->SavedLimitPointType())
  {
    ON_SubDSectorLimitPoint limit_point;
    if (vertex0->GetLimitPoint(subdivision_type, vertex0->m_faces[0], bUseSavedSubdivisionPoint, limit_point))
    {
      if (nullptr == limit_point.m_sector_face)
      {
        limit_point.m_next_sector_limit_point = (const ON_SubDSectorLimitPoint*)1;
        v1->SetSavedLimitPoint(subdivision_type, limit_point);
      }
    }
  }
  
  return v1;
}

ON_SubDVertex* ON_SubD_FixedSizeHeap::AllocateVertex(
  const ON_SubDEdge* edge0,
  ON_SubD::SubDType subdivision_type,
  bool bUseSavedSubdivisionPoint,
  unsigned int edge_capacity,
  unsigned int face_capacity
  )
{
  if ( nullptr == edge0)
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  double subdP[3];
  if (false == edge0->GetSubdivisionPoint(subdivision_type,bUseSavedSubdivisionPoint,subdP))
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  ON_SubDVertex* v1 = AllocateVertex(subdP,edge_capacity,face_capacity);
  if ( nullptr == v1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  v1->m_level = edge0->m_level+1;
  
  if (ON_SubD::EdgeTag::Smooth == edge0->m_edge_tag || ON_SubD::EdgeTag::X == edge0->m_edge_tag)
    v1->m_vertex_tag = ON_SubD::VertexTag::Smooth;
  else if (ON_SubD::EdgeTag::Crease == edge0->m_edge_tag)
    v1->m_vertex_tag = ON_SubD::VertexTag::Crease;

  //if ( ON_SubD::SubDType::QuadCatmullClark == subdivision_type)
  //  v1->m_vertex_facet_type = ON_SubD::VertexFacetType::Quad;
  //else if ( ON_SubD::SubDType::TriLoopWarren == subdivision_type)
  //  v1->m_vertex_facet_type = ON_SubD::VertexFacetType::Tri;

  return v1;
}

ON_SubDVertex* ON_SubD_FixedSizeHeap::AllocateVertex(
  const ON_SubDFace* face0,
  ON_SubD::SubDType subdivision_type,
  bool bUseSavedSubdivisionPoint,
  unsigned int edge_capacity,
  unsigned int face_capacity
  )
{
  if ( nullptr == face0)
    return ON_SUBD_RETURN_ERROR(nullptr);

  double subdP[3];
  if (false == face0->GetSubdivisionPoint(subdivision_type,bUseSavedSubdivisionPoint,subdP))
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDVertex* v1 = AllocateVertex(subdP,edge_capacity,face_capacity);
  if ( nullptr == v1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  v1->m_level = face0->m_level+1;

  v1->m_vertex_tag = ON_SubD::VertexTag::Smooth;

  //if ( ON_SubD::SubDType::QuadCatmullClark == subdivision_type)
  //  v1->m_vertex_facet_type = ON_SubD::VertexFacetType::Quad;
  //else if ( ON_SubD::SubDType::TriLoopWarren == subdivision_type)
  //  v1->m_vertex_facet_type = ON_SubD::VertexFacetType::Tri;

  return v1;
}

ON_SubDEdge* ON_SubD_FixedSizeHeap::AllocateEdge(
  ON_SubDVertex* v0,
  double v0_sector_weight,
  ON_SubDVertex* v1,
  double v1_sector_weight
  )
{
  if ( nullptr != v0 && nullptr == v0->m_edges)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( nullptr != v1 && nullptr == v1->m_edges)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == m_e || m_e_index >= m_e_capacity)
    return ON_SUBD_RETURN_ERROR(nullptr);

  bool bTaggedVertex[2];
  if (nullptr != v0)
  {
    if (nullptr == v0->m_edges || v0->m_edge_count >= v0->m_edge_capacity)
      return ON_SUBD_RETURN_ERROR(nullptr);
    if (ON_SubD::VertexTag::Smooth == v0->m_vertex_tag)
    {
      bTaggedVertex[0] = false;
      v0_sector_weight = ON_SubDSectorType::IgnoredSectorWeight;
    }
    else
    {
      bTaggedVertex[0] = (ON_SubD::VertexTag::Unset != v0->m_vertex_tag);
    }
  }
  else
    bTaggedVertex[0] = false;

  if (nullptr != v1)
  {
    if (nullptr == v1->m_edges || v1->m_edge_count >= v1->m_edge_capacity)
      return ON_SUBD_RETURN_ERROR(nullptr);
    if (ON_SubD::VertexTag::Smooth == v1->m_vertex_tag)
    {
      bTaggedVertex[1] = false;
      v1_sector_weight = ON_SubDSectorType::IgnoredSectorWeight;
    }
    else
    {
      bTaggedVertex[1] = (ON_SubD::VertexTag::Unset != v0->m_vertex_tag);
      if (bTaggedVertex[0] && bTaggedVertex[1])
      {
        // crease edge - no weights
        v0_sector_weight = ON_SubDSectorType::IgnoredSectorWeight;
        v1_sector_weight = ON_SubDSectorType::IgnoredSectorWeight;
      }
    }
  }
  else
    bTaggedVertex[1] = false;

  if ( false == ON_SubDSectorType::IsValidSectorWeightValue(v0_sector_weight, true))
    return ON_SUBD_RETURN_ERROR(nullptr);

  if ( false == ON_SubDSectorType::IsValidSectorWeightValue(v1_sector_weight, true))
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDEdge* e = m_e + m_e_index;
  memset(e, 0, sizeof(*e));
  if (m_e_index > 0)
  {
    // code in ON_SubDFaceNeighborhood.Subdivide() relies on m_next_edge being set this way.
    m_e[m_e_index - 1].m_next_edge = e;
    e->m_prev_edge = &m_e[m_e_index - 1];
  }

  e->m_id = ++m_e_index;
  
  if (nullptr != v0)
  {
    e->m_vertex[0] = v0;
    v0->m_edges[v0->m_edge_count++] = ON_SubDEdgePtr::Create(e,0);
    //v0->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
    e->m_level = v0->m_level;
  }

  if (nullptr != v1)
  {
    e->m_vertex[1] = v1;
    v1->m_edges[v1->m_edge_count++] = ON_SubDEdgePtr::Create(e,1);
    //v1->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
    if ( e->m_level < v1->m_level)
      e->m_level = v1->m_level;
  }

  e->m_sector_coefficient[0] = v0_sector_weight;
  e->m_sector_coefficient[1] = v1_sector_weight;
  e->m_edge_tag = (bTaggedVertex[0] && bTaggedVertex[1]) ? ON_SubD::EdgeTag::Crease : ON_SubD::EdgeTag::Smooth;

  return e;
}

ON_SubDFace* ON_SubD_FixedSizeHeap::AllocateFace(
  unsigned int zero_face_id,
  unsigned int parent_face_id
  )
{
  if (nullptr == m_f || m_f_index >= m_f_capacity)
    return ON_SUBD_RETURN_ERROR(nullptr);
  ON_SubDFace* f = m_f + m_f_index;
  memset(f, 0, sizeof(*f));
  if (m_f_index > 0)
  {
    // code in ON_SubDFaceNeighborhood.Subdivide() relies on 
    // m_next_face being set this way.
    m_f[m_f_index-1].m_next_face = f;
    f->m_prev_face = &m_f[m_f_index-1];
  }

  f->m_id = ++m_f_index;
  f->m_zero_face_id = (0 == zero_face_id) ? parent_face_id : zero_face_id;
  f->m_parent_face_id = parent_face_id;

  return f;
}

ON_SubDFace* ON_SubD_FixedSizeHeap::AllocateQuad(
  unsigned int zero_face_id,
  unsigned int parent_face_id,
  const ON_SubDEdgePtr eptrs[4]
  )
{
  if (nullptr == eptrs)
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDEdge* edges[4] = {
    ON_SUBD_EDGE_POINTER(eptrs[0].m_ptr),
    ON_SUBD_EDGE_POINTER(eptrs[1].m_ptr),
    ON_SUBD_EDGE_POINTER(eptrs[2].m_ptr),
    ON_SUBD_EDGE_POINTER(eptrs[3].m_ptr)};

  if (nullptr == edges[0] || edges[0]->m_face_count > 1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[1] || edges[1]->m_face_count > 1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[2] || edges[2]->m_face_count > 1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[3] || edges[3]->m_face_count > 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON__UINT_PTR edgedirs[4] = {
    ON_SUBD_EDGE_DIRECTION(eptrs[0].m_ptr),
    ON_SUBD_EDGE_DIRECTION(eptrs[1].m_ptr),
    ON_SUBD_EDGE_DIRECTION(eptrs[2].m_ptr),
    ON_SUBD_EDGE_DIRECTION(eptrs[3].m_ptr)};

  ON_SubDVertex* vertices[4] = {
    const_cast<ON_SubDVertex*>(edges[0]->m_vertex[edgedirs[0]]),
    const_cast<ON_SubDVertex*>(edges[1]->m_vertex[edgedirs[1]]),
    const_cast<ON_SubDVertex*>(edges[2]->m_vertex[edgedirs[2]]),
    const_cast<ON_SubDVertex*>(edges[3]->m_vertex[edgedirs[3]]) };

  if (nullptr == vertices[0] || nullptr == vertices[0]->m_faces || vertices[0]->m_face_count >= vertices[0]->m_face_capacity || vertices[0] != edges[3]->m_vertex[1-edgedirs[3]])
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[1] || nullptr == vertices[1]->m_faces || vertices[1]->m_face_count >= vertices[1]->m_face_capacity || vertices[1] != edges[0]->m_vertex[1-edgedirs[0]])
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[2] || nullptr == vertices[2]->m_faces || vertices[2]->m_face_count >= vertices[2]->m_face_capacity || vertices[2] != edges[1]->m_vertex[1-edgedirs[1]])
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[3] || nullptr == vertices[3]->m_faces || vertices[3]->m_face_count >= vertices[3]->m_face_capacity || vertices[3] != edges[2]->m_vertex[1-edgedirs[2]])
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  ON_SubDFace* f = AllocateFace(zero_face_id,parent_face_id);
  if (nullptr == f)
    return ON_SUBD_RETURN_ERROR(nullptr);

  f->m_edge_count = 4;
  f->m_edge4[0] = eptrs[0];
  f->m_edge4[1] = eptrs[1];
  f->m_edge4[2] = eptrs[2];
  f->m_edge4[3] = eptrs[3];

  edges[0]->m_face2[edges[0]->m_face_count++] = ON_SubDFacePtr::Create(f,edgedirs[0]);
  edges[1]->m_face2[edges[1]->m_face_count++] = ON_SubDFacePtr::Create(f,edgedirs[1]);
  edges[2]->m_face2[edges[2]->m_face_count++] = ON_SubDFacePtr::Create(f,edgedirs[2]);
  edges[3]->m_face2[edges[3]->m_face_count++] = ON_SubDFacePtr::Create(f,edgedirs[3]);

  vertices[0]->m_faces[vertices[0]->m_face_count++] = f;
  //vertices[0]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
  vertices[1]->m_faces[vertices[1]->m_face_count++] = f;
  //vertices[1]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
  vertices[2]->m_faces[vertices[2]->m_face_count++] = f;
  //vertices[2]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
  vertices[3]->m_faces[vertices[3]->m_face_count++] = f;
  //vertices[3]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;

  f->m_level = edges[0]->m_level;

  return f;
}


ON_SubDFace* ON_SubD_FixedSizeHeap::AllocateTri(
  unsigned int zero_face_id,
  unsigned int parent_face_id,
  const ON_SubDEdgePtr eptrs[3]
  )
{
  if (nullptr == eptrs)
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDEdge* edges[3] = {
    ON_SUBD_EDGE_POINTER(eptrs[0].m_ptr),
    ON_SUBD_EDGE_POINTER(eptrs[1].m_ptr),
    ON_SUBD_EDGE_POINTER(eptrs[2].m_ptr)};

  if (nullptr == edges[0] || edges[0]->m_face_count > 1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[1] || edges[1]->m_face_count > 1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[2] || edges[2]->m_face_count > 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON__UINT_PTR edgedirs[3] = {
    ON_SUBD_EDGE_DIRECTION(eptrs[0].m_ptr),
    ON_SUBD_EDGE_DIRECTION(eptrs[1].m_ptr),
    ON_SUBD_EDGE_DIRECTION(eptrs[2].m_ptr)};

  ON_SubDVertex* vertices[3] = {
    const_cast<ON_SubDVertex*>(edges[0]->m_vertex[edgedirs[0]]),
    const_cast<ON_SubDVertex*>(edges[1]->m_vertex[edgedirs[1]]),
    const_cast<ON_SubDVertex*>(edges[2]->m_vertex[edgedirs[2]])};

  if (nullptr == vertices[0] || nullptr == vertices[0]->m_faces || vertices[0]->m_face_count >= vertices[0]->m_face_capacity || vertices[0] != edges[2]->m_vertex[1-edgedirs[2]])
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[1] || nullptr == vertices[1]->m_faces || vertices[1]->m_face_count >= vertices[1]->m_face_capacity || vertices[1] != edges[0]->m_vertex[1-edgedirs[0]])
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[2] || nullptr == vertices[2]->m_faces || vertices[2]->m_face_count >= vertices[2]->m_face_capacity || vertices[2] != edges[1]->m_vertex[1-edgedirs[1]])
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  ON_SubDFace* f = AllocateFace(zero_face_id,parent_face_id);
  if (nullptr == f)
    return ON_SUBD_RETURN_ERROR(nullptr);

  f->m_edge_count = 3;
  f->m_edge4[0] = eptrs[0];
  f->m_edge4[1] = eptrs[1];
  f->m_edge4[2] = eptrs[2];

  edges[0]->m_face2[edges[0]->m_face_count++] = ON_SubDFacePtr::Create(f,edgedirs[0]);
  edges[1]->m_face2[edges[1]->m_face_count++] = ON_SubDFacePtr::Create(f,edgedirs[1]);
  edges[2]->m_face2[edges[2]->m_face_count++] = ON_SubDFacePtr::Create(f,edgedirs[2]);

  vertices[0]->m_faces[vertices[0]->m_face_count++] = f;
  //vertices[0]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
  vertices[1]->m_faces[vertices[1]->m_face_count++] = f;
  //vertices[1]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
  vertices[2]->m_faces[vertices[2]->m_face_count++] = f;
  //vertices[2]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;

  f->m_level = edges[0]->m_level;

  return f;
}
ON__UINT_PTR* ON_SubD_FixedSizeHeap::AllocatePtrArray(
  unsigned int capacity,
  bool bZeroMemory
  )
{
  if (0 == capacity)
    return nullptr;

  if (nullptr == m_p || capacity + m_p_index >= m_p_capacity)
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON__UINT_PTR* p = m_p + m_p_index;
  m_p_index += capacity;

  if (bZeroMemory)
  {
    ON__UINT_PTR* p1 = p + capacity;
    while (p1 > p)
    {
      *(--p1) = 0;
    }
  }

  return p;
}

bool ON_SubD_FixedSizeHeap::ReturnPtrArray(
  unsigned int capacity,
  void* p
  )
{
  if (nullptr != m_p && capacity <= m_p_index && p == m_p + (m_p_index - capacity))
  {
    m_p_index -= capacity;
    return true;
  }
  return ON_SUBD_RETURN_ERROR(nullptr);
}


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDHeap
//

size_t ON_SubDHeap::m_offset_vertex_id = 0;
size_t ON_SubDHeap::m_offset_edge_id = 0;
size_t ON_SubDHeap::m_offset_face_id = 0;

ON_SubDHeap::ON_SubDHeap()
{
  m_fspv.Create(sizeof(class ON_SubDVertex), 0, 0);
  m_fspe.Create(sizeof(class ON_SubDEdge), 0, 0);
  m_fspf.Create(sizeof(class ON_SubDFace), 0, 0);
  m_fsp5.Create(5 * sizeof(ON__UINT_PTR), 0, 0);
  m_fsp9.Create(9 * sizeof(ON__UINT_PTR), 0, 0);
  m_fsp17.Create(17 * sizeof(ON__UINT_PTR), 0, 0);

  if (0 == ON_SubDHeap::m_offset_vertex_id)
  {
    ON_SubDVertex v;
    ON_SubDHeap::m_offset_vertex_id = ((const char*)(&v.m_id)) - ((const char*)&v);
    ON_SubDEdge e;
    ON_SubDHeap::m_offset_edge_id = ((const char*)(&e.m_id)) - ((const char*)&e);
    ON_SubDFace f;
    ON_SubDHeap::m_offset_face_id = ((const char*)(&f.m_id)) - ((const char*)&f);
  }
}

ON_SubDHeap::~ON_SubDHeap()
{
  Destroy();
}

class ON_SubDVertex* ON_SubDHeap::AllocateVertexAndSetId(unsigned int& max_vertex_id)
{
  // In order for m_fspv.ElementFromId() to work, it is critical that
  // once a vertex is allocated from m_fspv, the value of m_id never
  // changes.  This is imporant because the value of m_id must persist
  // in binary archives in order for ON_COMPONENT_INDEX values to 
  // persist in binary archives.
  ON_SubDVertex* v;
  if (m_unused_vertex)
  {
    v = m_unused_vertex;
    m_unused_vertex = (ON_SubDVertex*)(m_unused_vertex->m_next_vertex);
    const unsigned int id = v->m_id;
    if (ON_UNSET_UINT_INDEX == (&v->m_id)[1])
    {
      memset(v, 0, sizeof(*v));
      v->m_id = id;
    }
    else
    {
      // something is modifying ids of returned elements
      ON_SubDIncrementErrorCount();
      v->m_id = ++max_vertex_id;
    }
  }
  else
  {
    v = (class ON_SubDVertex*)m_fspv.AllocateElement();
    v->m_id = ++max_vertex_id;
  }
  return v;
}

void ON_SubDHeap::ReturnVertex(class ON_SubDVertex* v)
{
  if (nullptr != v)
  {
    ReturnVertexEdgeAndFaceArrays(v);
    (&v->m_id)[1] = ON_UNSET_UINT_INDEX; // m_archive_id == ON_UNSET_UINT_INDEX marks the fixed size pool element as unused
    v->m_next_vertex = m_unused_vertex;
    m_unused_vertex = v;
    // NO! // m_fspv.ReturnElement(v);
    // See comments in AllocateVertexAndSetId();
  }
}

class ON_SubDEdge* ON_SubDHeap::AllocateEdgeAndSetId(unsigned int& max_edge_id)
{
  // In order for m_fspe.ElementFromId() to work, it is critical that
  // once a edge is allocated from m_fspe, the value of m_id never
  // changes.  This is imporant because the value of m_id must persist
  // in binary archives in order for ON_COMPONENT_INDEX values to 
  // persist in binary archives.
  ON_SubDEdge* e;
  if (m_unused_edge)
  {
    e = m_unused_edge;
    m_unused_edge = (ON_SubDEdge*)(m_unused_edge->m_next_edge);
    const unsigned int id = e->m_id;
    if (ON_UNSET_UINT_INDEX == (&e->m_id)[1])
    {
      memset(e, 0, sizeof(*e));
      e->m_id = id;
    }
    else
    {
      // something is modifying ids of returned elements
      ON_SubDIncrementErrorCount();
      e->m_id = ++max_edge_id;
    }
  }
  else
  {
    e = (class ON_SubDEdge*)m_fspe.AllocateElement();
    e->m_id = ++max_edge_id;
  }
  return e;
}

void ON_SubDHeap::ReturnEdge(class ON_SubDEdge* e)
{
  if (nullptr != e)
  {
    if (nullptr != e->m_facex)
      ReturnArray(e->m_facex_capacity,(ON__UINT_PTR*)e->m_facex);
    (&e->m_id)[1] = ON_UNSET_UINT_INDEX; // m_archive_id == ON_UNSET_UINT_INDEX marks the fixed size pool element as unused
    e->m_next_edge = m_unused_edge;
    m_unused_edge = e;
    // NO! // m_fspe.ReturnElement(e);
    // See comments in AllocateVertexAndSetId();
  }
}

class ON_SubDFace* ON_SubDHeap::AllocateFaceAndSetId(unsigned int& max_face_id)
{
  // In order for m_fspf.ElementFromId() to work, it is critical that
  // once a face is allocated from m_fspf, the value of m_id never
  // changes.  This is imporant because the value of m_id must persist
  // in binary archives in order for ON_COMPONENT_INDEX values to 
  // persist in binary archives.
  ON_SubDFace* f;
  if (m_unused_face)
  {
    f = m_unused_face;
    m_unused_face = (ON_SubDFace*)(m_unused_face->m_next_face);
    const unsigned int id = f->m_id;
    if (ON_UNSET_UINT_INDEX == (&f->m_id)[1])
    {
      memset(f, 0, sizeof(*f));
      f->m_id = id;
    }
    else
    {
      // something is modifying ids of returned elements
      ON_SubDIncrementErrorCount();
      f->m_id = ++max_face_id;
    }
  }
  else
  {
    f = (class ON_SubDFace*)m_fspf.AllocateElement();
    f->m_id = ++max_face_id;
  }
  return f;
}

void ON_SubDHeap::ReturnFace(class ON_SubDFace* f)
{
  if (nullptr != f)
  {
    ReturnArray(f->m_edgex_capacity,(ON__UINT_PTR*)f->m_edgex);
    (&f->m_id)[1] = ON_UNSET_UINT_INDEX; // m_archive_id == ON_UNSET_UINT_INDEX marks the fixed size pool element as unused
    f->m_next_face = m_unused_face;
    m_unused_face = f;
    // NO! // m_fspf.ReturnElement(f);
    // See comments in AllocateVertexAndSetId();
  }
}

void ON_SubDHeap::Clear()
{
  class tagWSItem* p = m_ws;
  m_ws = 0;
  while (p)
  {
    class tagWSItem* next = p->m_next;
    onfree(p);
    p = next;
  }
  m_fspv.ReturnAll();
  m_fspe.ReturnAll();
  m_fspf.ReturnAll();
  m_fsp5.ReturnAll();
  m_fsp9.ReturnAll();
  m_fsp17.ReturnAll();

  m_unused_vertex = nullptr;
  m_unused_edge = nullptr;
  m_unused_face = nullptr;
}

void ON_SubDHeap::Destroy()
{
  Clear();
  m_fspv.Destroy();
  m_fspe.Destroy();
  m_fspf.Destroy();
  m_fsp5.Destroy();
  m_fsp9.Destroy();
  m_fsp17.Destroy();
}

void ON_SubDHeap::ClearArchiveId()
{
  ON_FixedSizePoolIterator fit;
  fit.Create(&m_fspv);
  for (ON_SubDVertex* v = (ON_SubDVertex*)fit.FirstElement(); nullptr != v; v = (ON_SubDVertex*)fit.NextElement())
  {
    if ( ON_UNSET_UINT_INDEX != v->ArchiveId())
      v->SetArchiveId(0);
  }
  fit.Create(&m_fspe);
  for (ON_SubDEdge* e = (ON_SubDEdge*)fit.FirstElement(); nullptr != e; e = (ON_SubDEdge*)fit.NextElement())
  {
    if ( ON_UNSET_UINT_INDEX != e->ArchiveId())
      e->SetArchiveId(0);
  }
  fit.Create(&m_fspf);
  for (ON_SubDFace* f = (ON_SubDFace*)fit.FirstElement(); nullptr != f; f = (ON_SubDFace*)fit.NextElement())
  {
    if ( ON_UNSET_UINT_INDEX != f->ArchiveId())
      f->SetArchiveId(0);
  }
}

const class ON_SubDVertex* ON_SubDHeap::VertexFromId(
  unsigned int vertex_id
  ) const
{
  if ( 0 == vertex_id || ON_UNSET_UINT_INDEX == vertex_id)
    return ON_SUBD_RETURN_ERROR(nullptr);
  const class ON_SubDVertex* vertex = (const class ON_SubDVertex*)m_fspv.ElementFromId(ON_SubDHeap::m_offset_vertex_id,vertex_id);
  if ( nullptr == vertex || vertex_id != vertex->m_id)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( ON_UNSET_UINT_INDEX == vertex->ArchiveId() )
    return ON_SUBD_RETURN_ERROR(nullptr);
  return vertex;
}

const class ON_SubDEdge* ON_SubDHeap::EdgeFromId(
  unsigned int edge_id
  ) const
{
  if ( 0 == edge_id || ON_UNSET_UINT_INDEX == edge_id)
    return ON_SUBD_RETURN_ERROR(nullptr);
  const class ON_SubDEdge* edge = (const class ON_SubDEdge*)m_fspe.ElementFromId(ON_SubDHeap::m_offset_edge_id,edge_id);
  if ( nullptr == edge || edge_id != edge->m_id)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( ON_UNSET_UINT_INDEX == edge->ArchiveId() )
    return ON_SUBD_RETURN_ERROR(nullptr);
  return edge;
}

const class ON_SubDFace* ON_SubDHeap::FaceFromId(
  unsigned int face_id
  ) const
{
  if ( 0 == face_id || ON_UNSET_UINT_INDEX == face_id)
    return ON_SUBD_RETURN_ERROR(nullptr);
  const class ON_SubDFace* face = (const class ON_SubDFace*)m_fspf.ElementFromId(ON_SubDHeap::m_offset_face_id,face_id);
  if ( nullptr == face || face_id != face->m_id)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( ON_UNSET_UINT_INDEX == face->ArchiveId() )
    return ON_SUBD_RETURN_ERROR(nullptr);
  return face;
}

size_t ON_SubDHeap::OversizedElementCapacityQWERTY(size_t count)
{
  size_t capacity = 32 * (count / 32);
  if (count % 32 > 0 || 0 == count)
    capacity += 32;
  return capacity;
}

ON__UINT_PTR* ON_SubDHeap::AllocateOversizedElementQWERTY(size_t* capacity)
{
  class tagWSItem* p;
  size_t actual_capacity = ON_SubDHeap::OversizedElementCapacityQWERTY(*capacity);
  size_t sz = (actual_capacity + 1)*sizeof(ON__UINT_PTR);
  sz += sizeof(*p);
  p = (class tagWSItem*)onmalloc(sz);
  p->m_next = m_ws;
  if (p->m_next)
    p->m_next->m_prev = p;
  p->m_prev = 0;
  m_ws = p;
  ON__UINT_PTR* a = (ON__UINT_PTR*)(p + 1);
  *a++ = actual_capacity;
  *capacity = actual_capacity;
  return a;
}

void ON_SubDHeap::ReturnOversizedElement(
  size_t capacity,
  ON__UINT_PTR* a
  )
{
  if (0 != a && capacity > 0)
  {
    class tagWSItem* p = ((class tagWSItem*)(a - 1)) - 1;
    if (p == m_ws)
    {
      m_ws = p->m_next;
      p->m_next->m_prev = 0;
    }
    else
    {
      if (p->m_next)
        p->m_next->m_prev = p->m_prev;
      p->m_prev->m_next = p->m_next;
    }
    onfree(p);
  }
}

ON__UINT_PTR* ON_SubDHeap::ResizeArray(
  size_t current_count,
  size_t current_capacity,
  ON__UINT_PTR* current_a,
  size_t* new_capacity
  )
{
  ON__UINT_PTR capacity = ON_SubDHeap::ArrayCapacity(current_capacity,current_a);
  if (capacity <= 0)
  {
    return (ON__UINT_PTR*)AllocateArray(new_capacity);
  }

  if (*new_capacity <= 0)
  {
    ReturnArray(current_capacity,current_a);
    *new_capacity = 0;
    return nullptr;
  }

  if (*new_capacity <= capacity)
  {
    return current_a;
  }

  ON__UINT_PTR* new_a = AllocateArray(new_capacity);
  ON__UINT_PTR* a1 = new_a + current_count;
  while (new_a < a1)
  {
    *new_a++ = *current_a++;
  }
  ReturnArray(current_capacity,current_a - current_count);
  return (a1 - current_count);
}

bool ON_SubDHeap::GrowVertexEdgeArray(
  ON_SubDVertex* v,
  size_t capacity
  )
{
  if ( nullptr == v)
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 == capacity )
    capacity = v->m_edge_count+1;
  if ( capacity <= v->m_edge_capacity)
    return true;
  ON__UINT_PTR* a = ResizeArray(v->m_edge_count,v->m_edge_capacity,(ON__UINT_PTR*)v->m_edges,&capacity);
  if ( nullptr == a )
  {
    v->m_edge_count = 0;
    v->m_edge_capacity = 0;
    v->m_edges = 0;
    return ON_SUBD_RETURN_ERROR(false);
  }
  v->m_edges = (ON_SubDEdgePtr*)a;
  v->m_edge_capacity = (unsigned short)capacity;
  return true;
}

bool ON_SubDHeap::GrowVertexFaceArray(
  ON_SubDVertex* v,
  size_t capacity
  )
{
  if ( nullptr == v)
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 == capacity )
    capacity = v->m_face_count+1;
  if ( capacity <= v->m_face_capacity)
    return true;
  ON__UINT_PTR* a = ResizeArray(v->m_face_count,v->m_face_capacity,(ON__UINT_PTR*)v->m_faces,&capacity);
  if (nullptr == a)
  {
    v->m_face_count = 0;
    v->m_face_capacity = 0;
    v->m_faces = nullptr;
    return ON_SUBD_RETURN_ERROR(false);
  }
  v->m_faces = (const ON_SubDFace**)a;
  v->m_face_capacity = (unsigned short)capacity;
  return true;
}

bool ON_SubDHeap::GrowEdgeFaceArray(
  ON_SubDEdge* e,
  size_t capacity
  )
{
  if ( nullptr == e)
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 == capacity )
    capacity = e->m_face_count+1;
  if ( capacity <= (size_t)(2 + e->m_facex_capacity))
    return true;
  size_t xcapacity = capacity-2;
  ON__UINT_PTR* a = ResizeArray((e->m_face_count>2) ? (e->m_face_count-2) : 0,e->m_facex_capacity,(ON__UINT_PTR*)e->m_facex,&xcapacity);
  if ( nullptr == a )
  {
    e->m_face_count = 0;
    e->m_facex_capacity = 0;
    e->m_facex = nullptr;
    return ON_SUBD_RETURN_ERROR(false);
  }
  e->m_facex = (ON_SubDFacePtr*)a;
  e->m_facex_capacity = (unsigned short)xcapacity;
  return true;
}

bool ON_SubDHeap::GrowFaceEdgeArray(
  ON_SubDFace* f,
  size_t capacity
  )
{
  if ( nullptr == f)
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 == capacity )
    capacity = f->m_edge_count+1;
  if ( capacity <= (size_t)(4 + f->m_edgex_capacity))
    return true;
  size_t xcapacity = capacity-4;
  ON__UINT_PTR* a = ResizeArray((f->m_edge_count>4) ? (f->m_edge_count-4) : 0,f->m_edgex_capacity,(ON__UINT_PTR*)f->m_edgex,&xcapacity);
  if ( nullptr == a )
  {
    f->m_edge_count = 0;
    f->m_edgex_capacity = 0;
    f->m_edgex = nullptr;
    return ON_SUBD_RETURN_ERROR(false);
  }
  f->m_edgex = (ON_SubDEdgePtr*)a;
  f->m_edgex_capacity = (unsigned short)xcapacity;
  return true;
}

bool ON_SubDHeap::GrowVertexEdgeArrayByOne(
  ON_SubDVertex* v
  )
{
  return GrowVertexEdgeArray(v,0);
}

bool ON_SubDHeap::GrowVertexFaceArrayByOne(
  ON_SubDVertex* v
  )
{
  return GrowVertexFaceArray(v,0);
}

bool ON_SubDHeap::GrowEdgeFaceArrayByOne(
  ON_SubDEdge* e
  )
{
  return GrowEdgeFaceArray(e,0);
}

bool ON_SubDHeap::GrowFaceEdgeArrayByOne(
  ON_SubDFace* f
  )
{
  return GrowFaceEdgeArray(f,0);
}

bool ON_SubDimple::GrowVertexEdgeArray(
  ON_SubDVertex* v,
  size_t capacity
  )
{
  return m_heap.GrowVertexEdgeArray(v,capacity);
}

bool ON_SubDimple::GrowVertexFaceArray(
  ON_SubDVertex* v,
  size_t capacity
  )
{
  return m_heap.GrowVertexFaceArray(v,capacity);
}

bool ON_SubDimple::GrowEdgeFaceArray(
  ON_SubDEdge* e,
  size_t capacity
  )
{
  return m_heap.GrowEdgeFaceArray(e,capacity);
}

bool ON_SubDimple::GrowFaceEdgeArray(
  ON_SubDFace* f,
  size_t capacity
  )
{
  return m_heap.GrowFaceEdgeArray(f,capacity);
}

bool ON_SubD::GrowVertexEdgeArray(
  ON_SubDVertex* v,
  size_t capacity
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return ON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowVertexEdgeArray(v,capacity);
}

bool ON_SubD::GrowVertexFaceArray(
  ON_SubDVertex* v,
  size_t capacity
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return ON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowVertexFaceArray(v,capacity);
}

bool ON_SubD::GrowEdgeFaceArray(
  ON_SubDEdge* e,
  size_t capacity
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return ON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowEdgeFaceArray(e,capacity);
}

bool ON_SubD::GrowFaceEdgeArray(
  ON_SubDFace* f,
  size_t capacity
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return ON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowFaceEdgeArray(f,capacity);
}

ON__UINT_PTR ON_SubDHeap::ArrayCapacity(
  size_t capacity,
  ON__UINT_PTR* a
  )
{
#if defined(ON_DEBUG)
  size_t acapacity = (nullptr == a) ? 0 : a[-1];
  if (capacity != acapacity)
  {
    ON_SubDIncrementErrorCount();
  }
#endif
  return (nullptr == a) ? 0 : a[-1];
}

bool ON_SubDHeap::ReturnVertexEdgeAndFaceArrays(
  ON_SubDVertex* v
  )
{
  if ( nullptr == v )
    return ON_SUBD_RETURN_ERROR(false);
  if (nullptr != v->m_edges || v->m_edge_capacity > 0 || v->m_edge_count > 0)
  {
    ReturnArray(v->m_edge_capacity,(ON__UINT_PTR*)v->m_edges);
    v->m_edges = nullptr;
    v->m_edge_capacity = 0;
    v->m_edge_count = 0;
  }
  if (nullptr != v->m_faces || v->m_face_capacity > 0 || v->m_face_count > 0)
  {
    ReturnArray(v->m_face_capacity,(ON__UINT_PTR*)v->m_faces);
    v->m_faces = nullptr;
    v->m_face_capacity = 0;
    v->m_face_count = 0;
  }
  return true;

}

bool ON_SubDHeap::ReturnEdgeExtraArray(
  ON_SubDEdge* e
  )
{
  if ( nullptr == e )
    return ON_SUBD_RETURN_ERROR(false);
  if (nullptr != e->m_facex || e->m_facex_capacity > 0)
  {
    ReturnArray(e->m_facex_capacity,(ON__UINT_PTR*)e->m_facex);
    e->m_facex = nullptr;
    e->m_facex_capacity = 0;
  }
  if (e->m_face_count > 2)
    e->m_face_count = 2;
  return true;
}

bool ON_SubDHeap::ReturnFaceExtraArray(
  ON_SubDFace* f
  )
{
  if ( nullptr == f )
    return ON_SUBD_RETURN_ERROR(false);
  if (nullptr != f->m_edgex || f->m_edgex_capacity > 0)
  {
    ReturnArray(f->m_edgex_capacity,(ON__UINT_PTR*)f->m_edgex);
    f->m_edgex = nullptr;
    f->m_edgex_capacity = 0;
  }
  if (f->m_edge_count > 4)
    f->m_edge_count = 4;
  return true;
}

ON__UINT_PTR* ON_SubDHeap::AllocateArray(size_t* capacity)
{
  ON__UINT_PTR* a;
  size_t requested_capacity = *capacity;
  if (requested_capacity <= 0)
    return nullptr;

  if (requested_capacity <= 4)
  {
    a = (ON__UINT_PTR*)m_fsp5.AllocateElement();
    *a++ = 4;
    *capacity = 4;
    return a;
  }

  if (requested_capacity <= 8)
  {
    a = (ON__UINT_PTR*)m_fsp9.AllocateElement();
    *a++ = 8;
    *capacity = 8;
    return a;
  }

  if (requested_capacity <= 16)
  {
    a = (ON__UINT_PTR*)m_fsp17.AllocateElement();
    *a++ = 16;
    *capacity = 16;
    return a;
  }

  return  AllocateOversizedElementQWERTY(capacity);
}

void ON_SubDHeap::ReturnArray(
  size_t capacity,
  ON__UINT_PTR* a
  )
{
  if (nullptr != a && 0 == capacity)
  {
    ON_SubDIncrementErrorCount();
  }

  switch (ON_SubDHeap::ArrayCapacity(capacity,a))
  {
  case 0:
    break;
  case 4:
    m_fsp5.ReturnElement(a - 1);
    break;
  case 8:
    m_fsp9.ReturnElement(a - 1);
    break;
  case 16:
    m_fsp17.ReturnElement(a - 1);
    break;
  default:
    ReturnOversizedElement(capacity,a);
    break;
  }
  return;
}

