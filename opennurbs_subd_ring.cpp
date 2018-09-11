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

////unsigned int ON_SubD::SectorEdgeCount(
////  const class ON_SubDSectorIterator& sit
////  )
////{
////  ON_SubDSectorIterator sit_local(sit);
////  const ON_SubDVertex* center_vertex = sit_local.CenterVertex();
////  if ( nullptr == center_vertex )
////    return ON_SUBD_RETURN_ERROR(0);
////  if ( 0 == center_vertex->m_edge_count && 0 == center_vertex->m_face_count )
////    return 0; // not an error
////  if ( center_vertex->m_edge_count < 2 || center_vertex->m_face_count < 1 || nullptr == center_vertex->m_edges || nullptr == center_vertex->m_faces )
////    return ON_SUBD_RETURN_ERROR(0);
////  const ON_SubDFace* face0 = sit_local.CurrentFace();
////  const ON_SubDEdge* edge0 = sit_local.CurrentEdge(0);
////  const ON_SubDEdge* e1 = sit_local.CurrentEdge(1);
////  if ( nullptr == face0 || nullptr == edge0 || nullptr == e1 || edge0 == e1 )
////    return ON_SUBD_RETURN_ERROR(0);
////
////  const ON_SubDFace* f0 = face0;
////  const unsigned int N = center_vertex->m_edge_count; 
////  unsigned int edge_count = 0; 
////  while (edge_count <= N) // edge_count <= N used to prevent infinite recursion on damaged topology
////  {
////    const ON_SubDFace* f1 = sit_local.NextFace(true);
////    if (f0 == f1 || f1 == face0)
////      return ON_SUBD_RETURN_ERROR(0);
////    const ON_SubDEdge* e0 = sit_local.CurrentEdge(0);
////    if (e0 != e1)
////      return ON_SUBD_RETURN_ERROR(0);
////    if (f0 == face0 && e0 == edge0)
////      return edge_count; // back to where we started
////
////    edge_count++;
////    e1 = sit_local.CurrentEdge(1);
////    if (e0 == e1)
////      return ON_SUBD_RETURN_ERROR(0);
////
////    if (e1 == edge0)
////    {
////      // edge0 should be a crease and center_vertex should be a dart
////      if (ON_SubD::EdgeTag::Crease == edge0->m_edge_tag)
////      {
////        f1 = sit_local.NextFace(true);
////        if (nullptr == f1)
////          return edge_count; 
////      }
////      return ON_SUBD_RETURN_ERROR(0);
////    }
////
////    if (nullptr == e1)
////    {
////      // e0 should be a crease and f1 should be null
////      if ( ON_SubD::EdgeTag::Crease != e0->m_edge_tag && 2 == e0->m_face_count)
////        return ON_SUBD_RETURN_ERROR(0);
////
////      if (nullptr != f1)
////        return ON_SUBD_RETURN_ERROR(0);
////       
////      sit_local = sit;
////      f0 = face0;
////      e1 = edge0;
////      while (edge_count <= N)
////      {
////        f1 = sit_local.PrevFace(true);
////        e0 = sit_local.CurrentEdge(1);
////        if (e0 != e1)
////          return ON_SUBD_RETURN_ERROR(0);
////        e1 = sit_local.CurrentEdge(0);
////        if (nullptr == e1)
////        {
////          // e0 should be a crease, e1 and f1 should be null
////          if ( nullptr == f1)
////            return edge_count; // hit matching crease
////          return ON_SUBD_RETURN_ERROR(0);
////        }
////        if ( e1 == edge0)
////          return ON_SUBD_RETURN_ERROR(0);
////        edge_count++;
////      }
////      return ON_SUBD_RETURN_ERROR(0); // damaged topology
////    }
////  }
////
////  return ON_SUBD_RETURN_ERROR(0); // damaged topology
////}

unsigned int ON_SubD::GetQuadSectorPointRing(
  ON_SubD::SubDType subd_type,
  bool bFirstPass,
  bool bSecondPass,
  const ON_SubDVertex* center_vertex,
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring,
  size_t point_ring_stride,
  double* point_ring
  )
{
  //// NO VALIDATION CHECKS
  ////  CALLER INSURES INPUT HAS NON-nullptr POINTERS AND CORRECT COUNTS

  double subP[3];
  const double* Q = nullptr;

  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = ON_SubD::ComponentRingFaceCount(component_ring_count);
  const unsigned int point_ring_count = 1 + N + F;

  const double* point_ring1 = point_ring + (point_ring_count*point_ring_stride);

  const bool bUseSavedSubdivisionPoint = bSecondPass;

  for (unsigned int pass = (bFirstPass ? 0U : 1U); pass < (bSecondPass ? 2U : 1U); pass++)
  {
    double* P = point_ring;

    const ON_SubDEdgePtr* edges;
    const ON_SubDFacePtr* faces;
    size_t element_stride;

    const ON_SubDVertex* vertex0 = center_vertex;
    if (nullptr != vertex0)
    {
      edges = vertex0->m_edges;
      faces = (const ON_SubDFacePtr*)(vertex0->m_faces);
      element_stride = 1;
    }
    else
    {
      vertex0 = component_ring[0].Vertex();
      edges = (const ON_SubDEdgePtr*)(component_ring+1);
      faces = (const ON_SubDFacePtr*)(component_ring+2);
      element_stride = 2;
    }

    if (0 == pass)
      Q = vertex0->m_P;
    else
    {
      if ( false == bSecondPass)
        return ON_SUBD_RETURN_ERROR(0); // subdivision not permitted

      if (false == vertex0->GetSubdivisionPoint(subd_type, bUseSavedSubdivisionPoint, subP))
        return ON_SUBD_RETURN_ERROR(0);

      Q = subP;
    }
    P[0] = Q[0];
    P[1] = Q[1];
    P[2] = Q[2];
    P += point_ring_stride;

    for ( unsigned int i = 0; i < N; i++, edges += element_stride, faces += element_stride )
    {
      // Get edge point
      ON__UINT_PTR eptr = edges->m_ptr;
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr);
      if (nullptr == edge)
        return ON_SUBD_RETURN_ERROR(0);
      eptr = 1 - ON_SUBD_EDGE_DIRECTION(eptr);
      const ON_SubDVertex* vertex = edge->m_vertex[eptr];
      if ( nullptr == vertex)
        return ON_SUBD_RETURN_ERROR(0);

      if (0 == pass)
      {
        if (ON_SubD::EdgeTag::X == edge->m_edge_tag)
          break; // need to use subdivision point in 2nd pass
        if (ON_SubD::VertexTag::Smooth == vertex->m_vertex_tag 
          || ON_SubD::EdgeTag::Crease == edge->m_edge_tag
          || 0.5 == edge->m_sector_coefficient[eptr]
          )
        {
          Q = vertex->m_P;
        }
        else
          break; // need to use subdivision point in 2nd pass
      }
      else
      {
        if (false == edge->GetSubdivisionPoint(subd_type, bUseSavedSubdivisionPoint, subP))
          return ON_SUBD_RETURN_ERROR(0);
        Q = subP;
      }
      P[0] = Q[0];
      P[1] = Q[1];
      P[2] = Q[2];
      P += point_ring_stride;
      
      if (point_ring1 == P)
      {
        // success on a sector with crease boundary
        return point_ring_count;
      }

      const ON_SubDFace* face = ON_SUBD_FACE_POINTER(faces->m_ptr);
      if (0 == pass)
      {
        if (4 != face->m_edge_count)
          break; // need 2nd pass

        // find the vertex opposite vertex0
        eptr = face->m_edge4[0].m_ptr;
        edge = ON_SUBD_EDGE_POINTER(eptr);
        if ( nullptr == edge)
          return ON_SUBD_RETURN_ERROR(0);
        eptr = ON_SUBD_EDGE_DIRECTION(eptr);
        if (vertex0 == edge->m_vertex[eptr])
          eptr = 2; // vertex0 = face->Vertex(0), set vertex = face->vertex(2)
        else if (vertex0 == edge->m_vertex[1-eptr])
          eptr = 3; // vertex0 = face->Vertex(1), set vertex = face->vertex(3)
        else
        {
          eptr = face->m_edge4[2].m_ptr;
          edge = ON_SUBD_EDGE_POINTER(eptr);
          if ( nullptr == edge)
            return ON_SUBD_RETURN_ERROR(0);
          eptr = ON_SUBD_EDGE_DIRECTION(eptr);
          if (vertex0 == edge->m_vertex[eptr])
            eptr = 0; // vertex0 = face->Vertex(2), set vertex = face->vertex(0)
          else if (vertex0 == edge->m_vertex[1-eptr])
            eptr = 1; // vertex0 = face->Vertex(3), set vertex = face->vertex(1)
          else
            return ON_SUBD_RETURN_ERROR(0);
        }
        eptr = face->m_edge4[eptr].m_ptr;
        edge = ON_SUBD_EDGE_POINTER(eptr);
        if ( nullptr == edge)
          return ON_SUBD_RETURN_ERROR(0);
        eptr = ON_SUBD_EDGE_DIRECTION(eptr);
        vertex = edge->m_vertex[eptr];
        if ( nullptr == vertex)
          return ON_SUBD_RETURN_ERROR(0);
        Q = vertex->m_P;
      }
      else
      {
        if (false == face->GetSubdivisionPoint(subd_type, bUseSavedSubdivisionPoint, subP))
          return ON_SUBD_RETURN_ERROR(0);
      }
      P[0] = Q[0];
      P[1] = Q[1];
      P[2] = Q[2];
      P += point_ring_stride;
    }
    
    if (point_ring1 == P)
    {
      // success on a smooth sector
      return point_ring_count;
    }
  }

  return ON_SUBD_RETURN_ERROR(0);
}


unsigned int ON_SubD::GetTriSectorPointRing(
  ON_SubD::SubDType subd_type,
  bool bFirstPass,
  bool bSecondPass,
  const ON_SubDVertex* vertex0,
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring,
  size_t point_ring_stride,
  double* point_ring
  )
{
  //// NO VALIDATION CHECKS
  ////  CALLER INSURES INPUT HAS NON-nullptr POINTERS AND CORRECT COUNTS

  double subP[3];
  const double* Q = nullptr;

  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int point_ring_count = 1 + N;
  const double* point_ring1 = point_ring + (point_ring_count*point_ring_stride);

  const bool bUseSavedSubdivisionPoint = bSecondPass;

  for (unsigned int pass = (bFirstPass ? 0U : 1U); pass < (bSecondPass ? 2U : 1U); pass++)
  {
    double* P = point_ring;

    const ON_SubDEdgePtr* edges;
    const ON_SubDFacePtr* faces;
    size_t element_stride;

    if (nullptr != vertex0)
    {
      edges = vertex0->m_edges;
      faces = (const ON_SubDFacePtr*)(vertex0->m_faces);
      element_stride = 1;
    }
    else
    {
      vertex0 = component_ring[0].Vertex();
      edges = (const ON_SubDEdgePtr*)(component_ring+1);
      faces = (const ON_SubDFacePtr*)(component_ring+2);
      element_stride = 2;
    }

    if (0 == pass)
      Q = vertex0->m_P;
    else
    {
      if ( false == bSecondPass)
        return ON_SUBD_RETURN_ERROR(0); // subdivision not permitted

      if (false == vertex0->GetSubdivisionPoint(subd_type, bUseSavedSubdivisionPoint, subP))
        return ON_SUBD_RETURN_ERROR(0);

      Q = subP;
    }
    P[0] = Q[0];
    P[1] = Q[1];
    P[2] = Q[2];
    P += point_ring_stride;

    for (unsigned int i = 0; i < N; i++, edges += element_stride, faces += element_stride)
    {
      // Get edge point
      ON__UINT_PTR eptr = edges->m_ptr;
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr);
      if (nullptr == edge)
        return ON_SUBD_RETURN_ERROR(0);
      eptr = 1 - ON_SUBD_EDGE_DIRECTION(eptr);
      const ON_SubDVertex* vertex = edge->m_vertex[eptr];
      if (nullptr == vertex)
        return ON_SUBD_RETURN_ERROR(0);

      if (0 == pass)
      {
        if (ON_SubD::EdgeTag::X == edge->m_edge_tag)
          break; // need 2nd pass 
        if (ON_SubD::VertexTag::Smooth == vertex->m_vertex_tag
          || ON_SubD::EdgeTag::Crease == edge->m_edge_tag
          || 0.5 == edge->m_sector_coefficient[eptr]
          )
        {
          Q = vertex->m_P;
        }
        else
        {
          break; // need 2nd pass 
        }
      }
      else
      {
        if (false == edge->GetSubdivisionPoint(subd_type, bUseSavedSubdivisionPoint, subP))
          return ON_SUBD_RETURN_ERROR(0);
        Q = subP;
      }
      P[0] = Q[0];
      P[1] = Q[1];
      P[2] = Q[2];
      P += point_ring_stride;

      if (point_ring1 == P)
      {
        // success on a sector with crease boundary
        return point_ring_count;
      }

      const ON_SubDFace* face = ON_SUBD_FACE_POINTER(faces->m_ptr);
      if (3 != face->m_edge_count)
        return ON_SUBD_RETURN_ERROR(0);
    }

    if (point_ring1 == P)
    {
      // success on a smooth sector
      return point_ring_count;
    }
  }

  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubD::ComponentRingEdgeCount(size_t component_ring_count)
{
  const unsigned int N 
    = (component_ring_count < 4)
    ? 0
    : (unsigned int)(component_ring_count/2);
  return N;
}

unsigned int ON_SubD::ComponentRingFaceCount(size_t component_ring_count)
{
  const unsigned int N = ComponentRingEdgeCount(component_ring_count);
  const unsigned int F
    = (N < 2)
    ? 0 
    : (unsigned int)(component_ring_count - N - 1);
  return F;
}

bool ON_SubD::ComponentRingIsValid(
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring
  )
{
  if (nullptr == component_ring || component_ring_count < 4)
    return ON_SUBD_RETURN_ERROR(false);
  
  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = ON_SubD::ComponentRingFaceCount(component_ring_count);

  const ON_SubDVertex* vertex = component_ring[0].Vertex();
  if ( nullptr == vertex )
    return ON_SUBD_RETURN_ERROR(false);
  if ( vertex->m_edge_count < N || nullptr == vertex->m_edges)
    return ON_SUBD_RETURN_ERROR(false);
  if ( vertex->m_face_count < F || nullptr == vertex->m_faces)
    return ON_SUBD_RETURN_ERROR(false);

  const ON_SubD::EdgeTag edge0_tag 
    = (F+1 == N || (F == N && ON_SubD::VertexTag::Dart == vertex->m_vertex_tag))
    ? ON_SubD::EdgeTag::Crease
    : ON_SubD::EdgeTag::Smooth;
  const ON_SubD::EdgeTag edge1_tag 
    = (F+1 == N)
    ? ON_SubD::EdgeTag::Crease
    : ON_SubD::EdgeTag::Smooth;

  unsigned int component_ring_index = 1;
  for (unsigned int i = 0; i < N; i++, component_ring_index++)
  {
    const ON_SubDEdge* edge = component_ring[component_ring_index].Edge();
    if ( nullptr == edge)
      return ON_SUBD_RETURN_ERROR(false);
    if (vertex != edge->m_vertex[component_ring[component_ring_index].ComponentMark()])
      return ON_SUBD_RETURN_ERROR(false);

    if (0 == i)
    {
      if (edge0_tag != edge->m_edge_tag)
      {
        if (  ON_SubD::EdgeTag::Smooth != edge0_tag || ON_SubD::EdgeTag::X != edge->m_edge_tag )
          return ON_SUBD_RETURN_ERROR(false);
      }
    }
    else if (i+1 == N)
    {
      if (edge1_tag != edge->m_edge_tag)
      {
        if (  ON_SubD::EdgeTag::Smooth != edge1_tag || ON_SubD::EdgeTag::X != edge->m_edge_tag )
          return ON_SUBD_RETURN_ERROR(false);
      }
      if ( ON_SubD::EdgeTag::Crease == edge1_tag)
        continue;
    }
    else
    {
      if (2 != edge->m_face_count || false == edge->IsSmooth(true))
        return ON_SUBD_RETURN_ERROR(false);
    }

    component_ring_index++;
    if ( component_ring_index >= component_ring_count)
      return ON_SUBD_RETURN_ERROR(false);

    const ON_SubDFace* face = component_ring[component_ring_index].Face();
    if ( nullptr == face)
      return ON_SUBD_RETURN_ERROR(false);
  }

  if (component_ring_index == component_ring_count)
    return true;

  return ON_SUBD_RETURN_ERROR(false);
}

unsigned int ON_SubD::GetSectorPointRing(
  ON_SubD::SubDType subd_type,
  bool bSubdivideIfNeeded,
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring,
  ON_SimpleArray<ON_3dPoint>& point_ring
  )
{
  point_ring.SetCount(0);
  if ( component_ring_count <= 0 || nullptr == component_ring )
    return ON_SUBD_RETURN_ERROR(0);
  const ON_SubDVertex* center_vertex = component_ring[0].Vertex();
  if ( nullptr == center_vertex )
    return ON_SUBD_RETURN_ERROR(0);
  const unsigned int point_ring_capacity = (unsigned int)component_ring_count;
  ON_3dPoint* point_ring_array = point_ring.Reserve(point_ring_capacity);
  if ( nullptr == point_ring_array)
    return ON_SUBD_RETURN_ERROR(0);
  unsigned int point_ring_count = GetSectorPointRing(subd_type, bSubdivideIfNeeded, component_ring_count, component_ring, point_ring_capacity, 3, &point_ring_array[0].x);
  if (point_ring_count > 0)
  {
    point_ring.SetCount(point_ring_count);
    return point_ring_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}


unsigned int ON_SubD::GetSectorSubdivsionPointRing(
  ON_SubD::SubDType subd_type,
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring,
  size_t subd_point_ring_capacity,
  size_t subd_point_ring_stride,
  double* subd_point_ring
  )
{
  if (false == ComponentRingIsValid(component_ring_count,component_ring))
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned int K = ON_SubD::FacetEdgeCount(subd_type);
  if ( 3 != K && 4 != K )
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = ON_SubD::ComponentRingFaceCount(component_ring_count);
  const unsigned int point_ring_count = N + (K-3)*F;
  if ( point_ring_count > subd_point_ring_capacity || nullptr == subd_point_ring)
    return ON_SUBD_RETURN_ERROR(0);

  const bool bFirstPass = false;
  const bool bSecondPass = true;
  unsigned int rc;
  if ( 3 == K )
    rc = GetTriSectorPointRing(subd_type,bFirstPass,bSecondPass,nullptr,component_ring_count,component_ring,subd_point_ring_stride,subd_point_ring);
  else if ( 4 == K )
    rc = GetQuadSectorPointRing(subd_type,bFirstPass,bSecondPass,nullptr,component_ring_count,component_ring,subd_point_ring_stride,subd_point_ring);
  else
    return ON_SUBD_RETURN_ERROR(0);

  if (0 == rc)
    return ON_SUBD_RETURN_ERROR(0);

  return rc;

}

unsigned int ON_SubD::GetSectorSubdivisionPointRing(
  ON_SubD::SubDType subd_type,
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring,
  ON_SimpleArray<ON_3dPoint>& subd_point_ring
  )
{
  subd_point_ring.SetCount(0);
  if ( component_ring_count <= 0 || nullptr == component_ring )
    return ON_SUBD_RETURN_ERROR(0);
  const ON_SubDVertex* center_vertex = component_ring[0].Vertex();
  if ( nullptr == center_vertex )
    return ON_SUBD_RETURN_ERROR(0);
  const unsigned int subd_point_ring_capacity = (unsigned int)component_ring_count;
  ON_3dPoint* subd_point_ring_array = subd_point_ring.Reserve(subd_point_ring_capacity);
  if ( nullptr == subd_point_ring_array)
    return ON_SUBD_RETURN_ERROR(0);
  unsigned int subd_point_ring_count = GetSectorSubdivsionPointRing(subd_type, component_ring_count, component_ring, subd_point_ring_capacity, 3, &subd_point_ring_array[0].x);
  if (subd_point_ring_count > 0)
  {
    subd_point_ring.SetCount(subd_point_ring_count);
    return subd_point_ring_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubD::GetSectorPointRing(
  ON_SubD::SubDType subd_type,
  bool bSubdivideIfNeeded,
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring,
  size_t point_ring_capacity,
  size_t point_ring_stride,
  double* point_ring
  )
{
  if (false == ComponentRingIsValid(component_ring_count,component_ring))
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned int K = ON_SubD::FacetEdgeCount(subd_type);
  if ( 3 != K && 4 != K )
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = ON_SubD::ComponentRingFaceCount(component_ring_count);
  const unsigned int point_ring_count = N + (K-3)*F;
  if ( point_ring_count > point_ring_capacity || nullptr == point_ring)
    return ON_SUBD_RETURN_ERROR(0);

  const bool bFirstPass = true;
  const bool bSecondPass = bSubdivideIfNeeded;
  unsigned int rc;
  if ( 3 == K )
    rc = GetTriSectorPointRing(subd_type,bFirstPass,bSecondPass,nullptr,component_ring_count,component_ring,point_ring_stride,point_ring);
  else if ( 4 == K )
    rc = GetQuadSectorPointRing(subd_type,bFirstPass,bSecondPass,nullptr,component_ring_count,component_ring,point_ring_stride,point_ring);
  else
    return ON_SUBD_RETURN_ERROR(0);

  if (0 == rc)
    return ON_SUBD_RETURN_ERROR(0);

  return rc;
}


unsigned int ON_SubD::GetSectorPointRing(
  ON_SubD::SubDType subd_type,
  bool bSubdivideIfNeeded,
  const class ON_SubDSectorIterator& sit,
  size_t point_ring_capacity,
  size_t point_ring_stride,
  double* point_ring
  )
{
  const ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(subd_type);
  if ( ON_SubD::FacetType::Quad != facet_type &&  ON_SubD::FacetType::Tri != facet_type )
    return ON_SUBD_RETURN_ERROR(0);

  const ON_SubDVertex* center_vertex = sit.CenterVertex();
  if ( nullptr == center_vertex )
    return ON_SUBD_RETURN_ERROR(0);
  const unsigned int center_vertex_element_count = center_vertex->m_edge_count + center_vertex->m_face_count + 1;

  ON_SubDComponentPtr stack_component_ring[41];
  unsigned int component_ring_capacity = sizeof(stack_component_ring) / sizeof(stack_component_ring[0]);
  ON_SubDComponentPtr* component_ring = stack_component_ring;
  if (component_ring_capacity < point_ring_capacity && component_ring_capacity < center_vertex_element_count)
  {
    component_ring_capacity = (unsigned int)((center_vertex_element_count < point_ring_capacity) ? center_vertex_element_count : point_ring_capacity);
    component_ring = new(std::nothrow) ON_SubDComponentPtr[component_ring_capacity];
    if ( nullptr == component_ring)
      return ON_SUBD_RETURN_ERROR(0);
  }

  unsigned int point_ring_count = 0;
  unsigned int component_ring_count = ON_SubD::GetSectorComponentRing(sit,component_ring_capacity,component_ring);
  if (component_ring_count > 0)
  {
    const bool bFirstPass = true;
    const bool bSecondPass = bSubdivideIfNeeded;
    point_ring_count = 
      ( ON_SubD::FacetType::Quad == facet_type )
      ? ON_SubD::GetQuadSectorPointRing(subd_type,bFirstPass,bSecondPass,nullptr,component_ring_count,component_ring,point_ring_stride,point_ring)
      : ON_SubD::GetTriSectorPointRing(subd_type,bFirstPass,bSecondPass,nullptr,component_ring_count,component_ring,point_ring_stride,point_ring);
  }
  
  if ( component_ring != stack_component_ring)
    delete[] component_ring;

  return point_ring_count;
}

unsigned int ON_SubD::GetSectorPointRing(
  ON_SubD::SubDType subd_type,
  bool bSubdivideIfNeeded,
  const class ON_SubDSectorIterator& sit,
  ON_SimpleArray<ON_3dPoint>& point_ring
  )
{
  point_ring.SetCount(0);
  const ON_SubDVertex* center_vertex = sit.CenterVertex();
  if ( nullptr == center_vertex )
    return ON_SUBD_RETURN_ERROR(0);
  const unsigned int point_ring_capacity = (center_vertex->m_edge_count + center_vertex->m_face_count);
  ON_3dPoint* point_ring_array = point_ring.Reserve(point_ring_capacity);
  if ( nullptr == point_ring_array)
    return ON_SUBD_RETURN_ERROR(0);
  unsigned int point_ring_count = GetSectorPointRing(subd_type, bSubdivideIfNeeded, sit, point_ring_capacity, 3, &point_ring_array[0].x);
  if (point_ring_count > 0)
  {
    point_ring.SetCount(point_ring_count);
    return point_ring_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}

static double Subdivide_CenterVertexSectorWeight(
  const ON_SubDEdge* edge0,
  const ON_SubDVertex* vertex0
  )
{
  if ( ON_SubD::EdgeTag::Crease == edge0->m_edge_tag)
    return ON_SubDSectorType::IgnoredSectorWeight;
  if (ON_SubD::EdgeTag::Smooth == edge0->m_edge_tag || ON_SubD::EdgeTag::X == edge0->m_edge_tag)
  {
    if (vertex0 == edge0->m_vertex[0])
      return edge0->m_sector_coefficient[0];
    if (vertex0 == edge0->m_vertex[1])
      return edge0->m_sector_coefficient[1];
  }
  return ON_SUBD_RETURN_ERROR(ON_UNSET_VALUE);
}

const ON_SubDVertex* ON_SubD::SubdivideSector(
  ON_SubD::SubDType subd_type,
  const ON_SubDVertex* center_vertex,
  size_t component_ring_count,
  const ON_SubDComponentPtr* component_ring,
  ON_SubD_FixedSizeHeap& fsh
  )
{
  const unsigned int N = (nullptr != center_vertex) ? center_vertex->m_edge_count : ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = (nullptr != center_vertex) ? center_vertex->m_face_count : ON_SubD::ComponentRingFaceCount(component_ring_count);
  
  size_t element_stride;
  const ON_SubDEdgePtr* edges;
  const ON_SubDFacePtr* faces;

  if (nullptr == center_vertex)
  {
    if (nullptr == component_ring || component_ring_count < 4)
      return ON_SUBD_RETURN_ERROR(nullptr);
    edges = (const ON_SubDEdgePtr*)(component_ring + 1);
    faces = (const ON_SubDFacePtr*)(component_ring + 2);
    element_stride = 2;
    center_vertex = component_ring[0].Vertex();
    if ( nullptr == center_vertex)
      return ON_SUBD_RETURN_ERROR(nullptr);
  }
  else
  {
    edges = center_vertex->m_edges;
    faces = (const ON_SubDFacePtr*)(center_vertex->m_faces);
    element_stride = 1;
    if ( F != N && F+1 != N )
      return ON_SUBD_RETURN_ERROR(nullptr);
  }
  
  const ON_SubD::EdgeTag edge0_tag = (F+1 == N) ? ON_SubD::EdgeTag::Crease : ON_SubD::EdgeTag::Smooth;

    const unsigned int face_edge_count = ON_SubD::FacetEdgeCount(subd_type);
  if ( 3 != face_edge_count && 4 != face_edge_count )
    return ON_SUBD_RETURN_ERROR(nullptr);
  const unsigned int K = face_edge_count-1;
  
  const ON_SubDEdge* edge0 = edges->Edge();
  if ( nullptr == edge0)
    return ON_SUBD_RETURN_ERROR(nullptr);
  edges += element_stride;

  if ( edge0_tag != edge0->m_edge_tag)
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  const ON_SubDFace* face0 = faces->Face();
  if ( nullptr == face0)
    return ON_SUBD_RETURN_ERROR(nullptr);
  edges += element_stride;

  if ( false == fsh.ReserveSubDWorkspace(N+1,K*N,N,(3*K+1)*N))
    return ON_SUBD_RETURN_ERROR(nullptr);

  const bool bUseSavedSubdivisionPoint = true;

  ON_SubDVertex* v1[4] = {};
  ON_SubDEdge* e1[4] = {};
  ON_SubDEdgePtr f1epts[4] = {};

  const ON_SubDVertex* vertex0 = center_vertex;

  v1[0] = fsh.AllocateVertex(vertex0,subd_type,bUseSavedSubdivisionPoint,N,N);
  if ( nullptr == v1[0])
    return ON_SUBD_RETURN_ERROR(nullptr);
  //v1[0]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::radial;

  ON_SubDVertex* vertex1 = fsh.AllocateVertex(edge0,subd_type,bUseSavedSubdivisionPoint,3,2);
  if ( nullptr == vertex1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  // at_crease weight is used when the cooresponding vertex is a crease.
  // Otherwise, fsh.AllocateEdge() ignores at_crease_weight.
  ON_SubD::EdgeTag edge1_tag = (ON_SubD::EdgeTag::X == edge0_tag) ? ON_SubD::EdgeTag::Smooth : edge0_tag;
  const double at_crease_weight 
    = ON_SubD::EdgeTag::Crease == edge1_tag
    ? ON_SubDSectorType::CreaseSectorWeight(subd_type,5-K)
    : ON_SubDSectorType::IgnoredSectorWeight;
  ON_SubDEdge* edge1 = fsh.AllocateEdge(v1[0], Subdivide_CenterVertexSectorWeight(edge0,vertex0), vertex1, ON_SubDSectorType::IgnoredSectorWeight );
  if ( nullptr == edge1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  edge1->m_edge_tag = edge1_tag;
  
  v1[1] = vertex1;
  e1[0] = edge1;
  f1epts[0] = ON_SubDEdgePtr::Create(e1[0],0);
  edge1_tag = ON_SubD::EdgeTag::Smooth;
    
  for (unsigned int i = 1; i < N; i++, edges += element_stride, faces += element_stride)
  {
    edge0 = edges->Edge();
    if ( nullptr == edge0)
      return ON_SUBD_RETURN_ERROR(nullptr);
    if (vertex0 != edge0->m_vertex[0] && vertex0 != edge0->m_vertex[1])
      return ON_SUBD_RETURN_ERROR(nullptr);

    if (i + 1 == N)
    {
      edge1_tag = edge0_tag;
      if ( edge1_tag != edge0->m_edge_tag)
        return ON_SUBD_RETURN_ERROR(nullptr);
      if (ON_SubD::EdgeTag::Smooth == edge1_tag)
      {
        v1[K] = vertex1;
        e1[K] = edge1;
      }
    }

    if (nullptr == v1[K])
    {
      v1[K] = fsh.AllocateVertex(edge0, subd_type, bUseSavedSubdivisionPoint, 3, 2);
      if (nullptr == v1[K])
        return ON_SUBD_RETURN_ERROR(nullptr);
      e1[K] = fsh.AllocateEdge(v1[0], Subdivide_CenterVertexSectorWeight(edge0, vertex0), v1[K], ON_SubDSectorType::IgnoredSectorWeight);
      if (nullptr == e1[K])
        return ON_SUBD_RETURN_ERROR(nullptr);
      e1[K]->m_edge_tag = edge1_tag;
    }

    f1epts[K] = ON_SubDEdgePtr::Create(e1[K],1);
    if (3 == K)
    {
      // quads
      v1[2] = fsh.AllocateVertex(face0, subd_type, bUseSavedSubdivisionPoint, 2, 1 );
      e1[1] = fsh.AllocateEdge(v1[1], at_crease_weight, v1[2], ON_SubDSectorType::IgnoredSectorWeight);
      e1[2] = fsh.AllocateEdge(v1[2], ON_SubDSectorType::IgnoredSectorWeight, v1[3], at_crease_weight);
      f1epts[1] = ON_SubDEdgePtr::Create(e1[1],0);
      f1epts[2] = ON_SubDEdgePtr::Create(e1[2],0);
      if (nullptr == fsh.AllocateQuad(face0->m_zero_face_id, face0->m_id, f1epts) )
        return ON_SUBD_RETURN_ERROR(nullptr);
    }
    else
    {
      // tris
      e1[1] = fsh.AllocateEdge(v1[1], at_crease_weight, v1[2], at_crease_weight);
      f1epts[1] = ON_SubDEdgePtr::Create(e1[1],0);
      if (nullptr == fsh.AllocateTri(face0->m_zero_face_id, face0->m_id, f1epts) )
        return ON_SUBD_RETURN_ERROR(nullptr);
    }
  
    if (i + 1 == N)
    {
      if (i + 1 == N && edge0_tag == edge1_tag)
        return v1[0];

      return ON_SUBD_RETURN_ERROR(nullptr);
    }

    face0 = faces->Face();
    if ( nullptr == face0)
      return ON_SUBD_RETURN_ERROR(nullptr);

    v1[1] = v1[K];
    e1[0] = e1[K];
    f1epts[0] = f1epts[K].Reversed();
    v1[K] = nullptr;
    e1[K] = nullptr;
  }

  return ON_SUBD_RETURN_ERROR(nullptr);
}

unsigned int ON_SubD::GetSectorComponentRing(
  const class ON_SubDSectorIterator& sit,
  size_t component_ring_capacity,
  ON_SubDComponentPtr* component_ring
  )
{
  if ( component_ring_capacity < 4 || nullptr == component_ring)
    return ON_SUBD_RETURN_ERROR(0);

  const ON_SubDVertex* vertex = sit.CenterVertex();
  if ( nullptr == vertex || vertex->m_edge_count < 2 || vertex->m_face_count < 1)
    return ON_SUBD_RETURN_ERROR(0);

  const ON_SubD::VertexTag center_vertex_tag = vertex->m_vertex_tag;

  ON_SubDSectorIterator localsit(sit);
  const bool bCreases = (nullptr != localsit.IncrementToCrease(-1));
  
  ON_SubDEdgePtr edgeptr = localsit.CurrentEdgePtr(0);
  ON_SubDFacePtr faceptr = localsit.CurrentFacePtr();

  const ON_SubDEdge* edge0 = edgeptr.Edge();
  if ( nullptr == edge0 )
    return ON_SUBD_RETURN_ERROR(0);
  const ON_SubDFace* face0 = faceptr.Face();
  if ( nullptr == face0 )
    return ON_SUBD_RETURN_ERROR(0);
  const ON_SubDVertex* ring_vertex0 = localsit.CurrentEdgeRingVertex(0);
  if ( nullptr == ring_vertex0 || vertex == ring_vertex0)
    return ON_SUBD_RETURN_ERROR(0);

  if (bCreases && ON_SubD::EdgeTag::Crease != edge0->m_edge_tag)
    return ON_SUBD_RETURN_ERROR(0);

  unsigned int component_ring_count = 0;
  component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(vertex);
  component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(edgeptr);
  component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(faceptr);
  const unsigned int N = vertex->m_edge_count; // for () used to prevent infinite recursion when vertex is not valid
  for (unsigned int i = 0; i < N; i++)
  {
    const ON_SubDFace* face = localsit.NextFace(true);

    edgeptr = localsit.CurrentEdgePtr(0);
    const ON_SubDEdge* edge = edgeptr.Edge();
    if ( nullptr == edge)
      return ON_SUBD_RETURN_ERROR(0);
    const ON_SubDVertex* ring_vertex = localsit.CurrentEdgeRingVertex(0);
    if ( nullptr == ring_vertex || vertex == ring_vertex)
      return ON_SUBD_RETURN_ERROR(0);

    if (face == face0 || edge == edge0 || ring_vertex == ring_vertex0)
    {
      // back to start?
      if (edge == edge0 && ring_vertex == ring_vertex0)
      {
        if (ON_SubD::VertexTag::Smooth == center_vertex_tag)
        {
          if (face == face0 && ON_SubD::EdgeTag::Smooth == edge0->m_edge_tag)
            return component_ring_count; // back to start smooth case.
        }
        if (ON_SubD::VertexTag::Dart == center_vertex_tag)
        {
          if (nullptr == face && ON_SubD::EdgeTag::Crease == edge0->m_edge_tag)
            return component_ring_count; // back to start dart case.
        }
      }

      return ON_SUBD_RETURN_ERROR(false); // damaged topology information
    }

    if ( component_ring_count >= component_ring_capacity)
      return ON_SUBD_RETURN_ERROR(false);

    component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(edgeptr);
    
    if (nullptr == face)
    {
      if (bCreases && ON_SubD::EdgeTag::Crease == edge->m_edge_tag)
        return component_ring_count;
      return ON_SUBD_RETURN_ERROR(0);
    }

    if ( false == edge->IsSmooth(true) || 2 != edge->m_face_count )
      return ON_SUBD_RETURN_ERROR(false);

    if ( component_ring_count >= component_ring_capacity)
      return ON_SUBD_RETURN_ERROR(false);

    faceptr = localsit.CurrentFacePtr();
    component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(faceptr);
  }

  return ON_SUBD_RETURN_ERROR(false); // damaged topology information
}


unsigned int ON_SubD::GetSectorComponentRing(
  const class ON_SubDSectorIterator& sit,
  ON_SimpleArray<ON_SubDComponentPtr>& elements
  )
{
  elements.SetCount(0);
  const ON_SubDVertex* vertex = sit.CenterVertex();
  if ( nullptr == vertex || vertex->m_edge_count < 2 || vertex->m_face_count < 1)
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned int component_ring_capacity = 1 + vertex->m_edge_count + vertex->m_face_count;
  unsigned int component_ring_count = ON_SubD::GetSectorComponentRing(sit, component_ring_capacity, elements.Reserve(component_ring_capacity));
  if (component_ring_count >= 4 && component_ring_count <= component_ring_capacity)
    elements.SetCount(component_ring_count);

  return elements.UnsignedCount();
}
