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
// Copyright (c) 1993-2015 Robert McNeel & Associates. All rights reserved.
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

ON_SubDComponentPtr::Type ON_SubDComponentPtr::ComponentPtrTypeFromUnsigned(
  unsigned int element_pointer_type_as_unsigned
  )
{
  switch (element_pointer_type_as_unsigned)
  {
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDComponentPtr::Type::Unset);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDComponentPtr::Type::Vertex);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDComponentPtr::Type::Edge);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDComponentPtr::Type::Face);
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDComponentPtr::Type::Unset);
}

ON_SubD::VertexTag ON_SubD::VertexTagFromUnsigned(
  unsigned int vertex_tag_as_unsigned
  )
{
  switch (vertex_tag_as_unsigned)
  {
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexTag::Unset);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexTag::Smooth);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexTag::Crease);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexTag::Corner);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexTag::Dart);
  }
  return ON_SUBD_RETURN_ERROR(ON_SubD::VertexTag::Unset);
}


bool ON_SubD::VertexTagIsSet(
  ON_SubD::VertexTag vertex_tag
)
{
  return (
    ON_SubD::VertexTag::Smooth == vertex_tag
    || ON_SubD::VertexTag::Crease == vertex_tag
    || ON_SubD::VertexTag::Corner == vertex_tag
    || ON_SubD::VertexTag::Dart == vertex_tag
    );
}


ON_SubD::EdgeTag ON_SubD::EdgeTagFromUnsigned(
  unsigned int edge_tag_as_unsigned
  )
{
  switch (edge_tag_as_unsigned)
  {
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::EdgeTag::Unset);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::EdgeTag::Smooth);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::EdgeTag::Crease);
  //ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::EdgeTag::Sharp);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::EdgeTag::SmoothX);
  }
  return ON_SUBD_RETURN_ERROR(ON_SubD::EdgeTag::Unset);
}

bool ON_SubD::EdgeTagIsSet(
  ON_SubD::EdgeTag edge_tag
)
{
  return (
    ON_SubD::EdgeTag::Smooth == edge_tag
    || ON_SubD::EdgeTag::Crease == edge_tag
    //|| ON_SubD::EdgeTag::Sharp == edge_tag
    || ON_SubD::EdgeTag::SmoothX == edge_tag
    );
}

ON_SubD::VertexFacetType ON_SubD::VertexFacetTypeFromUnsigned(
  unsigned int vertex_facet_type_as_unsigned
  )
{
  switch (vertex_facet_type_as_unsigned)
  {
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexFacetType::Unset);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexFacetType::Tri);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexFacetType::Quad);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexFacetType::Ngon);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_SubD::VertexFacetType::Mixed);
  }
  return ON_SUBD_RETURN_ERROR(ON_SubD::VertexFacetType::Unset);
}

unsigned int ON_SubDSectorType::SectorPointRingCountFromEdgeCount(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_edge_count
  )
{
  if (sector_edge_count >= ON_SubDSectorType::MinimumSectorEdgeCount(vertex_tag) && sector_edge_count <= ON_SubDVertex::MaximumEdgeCount)
  {
    if (ON_SubD::VertexTag::Smooth == vertex_tag || ON_SubD::VertexTag::Dart == vertex_tag)
    {
      // interior vertex
      return (2 * sector_edge_count + 1);
    }

    if (ON_SubD::VertexTag::Crease == vertex_tag || ON_SubD::VertexTag::Corner == vertex_tag)
    {
      // boundary vertex
      return (2 * sector_edge_count);
    }
  }
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDSectorType::SectorPointRingCountFromFaceCount(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  unsigned int sector_edge_count = ON_SubDSectorType::SectorEdgeCountFromFaceCount(vertex_tag,sector_face_count);
  return (sector_edge_count > 0)
    ? ON_SubDSectorType::SectorPointRingCountFromEdgeCount(vertex_tag,sector_edge_count)
    : ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDSectorType::SectorFaceCountFromEdgeCount(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_edge_count
  )
{
  if (sector_edge_count >= 2 && sector_edge_count <= ON_SubDVertex::MaximumEdgeCount)
  {
    unsigned int sector_face_count
      = (ON_SubD::VertexTag::Crease == vertex_tag || ON_SubD::VertexTag::Corner == vertex_tag)
      ? sector_edge_count-1
      : sector_edge_count;
    return sector_face_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDSectorType::SectorEdgeCountFromFaceCount(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  if (sector_face_count > 0 && sector_face_count <= ON_SubDVertex::MaximumFaceCount)
  {
    unsigned int sector_edge_count
      = (ON_SubD::VertexTag::Crease == vertex_tag || ON_SubD::VertexTag::Corner == vertex_tag)
      ? sector_face_count+1
      : sector_face_count;
    return sector_edge_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDSectorType::MinimumSectorEdgeCount(
  ON_SubD::VertexTag vertex_tag
  )
{
  if (ON_SubD::VertexTag::Smooth == vertex_tag || ON_SubD::VertexTag::Dart == vertex_tag)
    return ON_SubDSectorType::MinimumSectorFaceCount(vertex_tag);
  if (ON_SubD::VertexTag::Corner == vertex_tag || ON_SubD::VertexTag::Crease == vertex_tag)
    return ON_SubDSectorType::MinimumSectorFaceCount(vertex_tag)+1;
  return ON_UNSET_UINT_INDEX;
}

unsigned int ON_SubDSectorType::MinimumSectorFaceCount(
  ON_SubD::VertexTag vertex_tag
  )
{
  unsigned int minimum_sector_face_count;
  switch (vertex_tag)
  {
  case ON_SubD::VertexTag::Unset:
    ON_SUBD_ERROR("Unset tag.");
    minimum_sector_face_count = ON_UNSET_UINT_INDEX;
    break;

  case ON_SubD::VertexTag::Smooth:
    // April 2019 Dale Lear - Catmull Clark Valence 2 Evaluation: Smooth
    // See comments in ON_SubDSectorType::GetSurfaceEvaluationCoefficients()
    // for more details on how this case is handled.
    minimum_sector_face_count = 2; // 3 without special case handling
    break;

  case ON_SubD::VertexTag::Crease:
    // A "wire" crease can have zero faces - this is the minimum when faces exist
    minimum_sector_face_count = 1;
    break;

  case ON_SubD::VertexTag::Corner:
    // A "wire" corner can have zero faces - this is the minimum when faces exist
    minimum_sector_face_count = 1;
    break;

  case ON_SubD::VertexTag::Dart:
    // April 2019 Dale Lear - Catmull Clark Valence 2 Evaluation: Dart
    // See comments in ON_SubDSectorType::GetSurfaceEvaluationCoefficients()
    // for more details on how this case is handled.
    minimum_sector_face_count = 2; // 3 without special case handling
    break;

  default:
    ON_SUBD_ERROR("Invalid tag.");
    minimum_sector_face_count = ON_UNSET_UINT_INDEX;
    break;
  }

  return minimum_sector_face_count;
}

bool ON_SubD::IsValidSectorEdgeCount(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_edge_count
  )
{
  return (sector_edge_count >= ON_SubDSectorType::MinimumSectorEdgeCount(vertex_tag) && sector_edge_count <= ON_SubDVertex::MaximumEdgeCount);
}

bool ON_SubD::IsValidSectorFaceCount(
  ON_SubD::VertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  return (sector_face_count >= ON_SubDSectorType::MinimumSectorFaceCount(vertex_tag) && sector_face_count <= ON_SubDVertex::MaximumFaceCount);
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertexPtr
//
//
bool ON_SubDVertexPtr::IsNull() const
{
  return (nullptr == ON_SUBD_VERTEX_POINTER(m_ptr));
}

bool ON_SubDVertexPtr::IsNotNull() const
{
  return (nullptr != ON_SUBD_VERTEX_POINTER(m_ptr));
}

class ON_SubDVertex* ON_SubDVertexPtr::Vertex() const
{
  return ON_SUBD_VERTEX_POINTER(m_ptr);
}

ON__UINT_PTR ON_SubDVertexPtr::VertexDirection() const
{
  return ON_SUBD_VERTEX_DIRECTION(m_ptr);
}

const ON_ComponentStatus ON_SubDVertexPtr::Status() const
{
  const ON_SubDVertex* vertex = ON_SUBD_VERTEX_POINTER(m_ptr);
  return (nullptr == vertex) ? ON_ComponentStatus::NoneSet : vertex->m_status;
}

const ON_SubDVertexPtr ON_SubDVertexPtr::Create(
  const class ON_SubDVertex* vertex
  )
{
  return ON_SubDVertexPtr::Create(vertex,0);
}

const ON_SubDVertexPtr ON_SubDVertexPtr::Create(
  const class ON_SubDVertex* vertex,
  ON__UINT_PTR vertex_mark
  )
{
  ON_SubDVertexPtr vptr = { (ON__UINT_PTR)vertex | (vertex_mark & ON_SUBD_COMPONENT_DIRECTION_MASK) };
  return vptr;
}

const ON_SubDVertexPtr ON_SubDVertexPtr::Create(
  const class ON_SubDComponentPtr& vertex_element
  )
{
  return ON_SubDVertexPtr::Create(vertex_element.Vertex(), vertex_element.ComponentDirection());
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdgePtr
//


bool ON_SubDEdgePtr::IsNull() const
{
  return (nullptr == ON_SUBD_EDGE_POINTER(m_ptr));
}

bool ON_SubDEdgePtr::IsNotNull() const
{
  return (nullptr != ON_SUBD_EDGE_POINTER(m_ptr));
}

bool ON_SubDEdgePtr::IsNotNullAndVerticesAreNotNull() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e && nullptr != e->m_vertex[0] && nullptr != e->m_vertex[1]);
}


class ON_SubDEdge* ON_SubDEdgePtr::Edge() const
{
  return ON_SUBD_EDGE_POINTER(m_ptr);
}

unsigned int ON_SubDEdgePtr::EdgeId() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->m_id : 0U;
}

unsigned int ON_SubDEdgePtr::EdgeFaceCount() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? ((unsigned int)e->m_face_count) : 0U;
}

bool ON_SubDEdgePtr::EdgeIsSmooth() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsSmooth() : false;
}   

bool ON_SubDEdgePtr::EdgeIsCrease() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsCrease() : false;
}   

bool ON_SubDEdgePtr::EdgeIsHardCrease() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsHardCrease() : false;
}   

bool ON_SubDEdgePtr::EdgeIsDartCrease() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsDartCrease() : false;
}

ON__UINT_PTR ON_SubDEdgePtr::EdgeDirection() const
{
  return ON_SUBD_EDGE_DIRECTION(m_ptr);
}

const class ON_SubDVertex* ON_SubDEdgePtr::RelativeVertex(
  int relative_vertex_index
  ) const
{
  for (;;)
  {
    if (relative_vertex_index < 0 || relative_vertex_index>1)
      break;
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(m_ptr);
    if (nullptr == edge)
      break;
    if (0 != ON_SUBD_EDGE_DIRECTION(m_ptr))
      relative_vertex_index = 1 - relative_vertex_index;
    return edge->m_vertex[relative_vertex_index];
  }
  return nullptr;
}

const ON_3dPoint ON_SubDEdgePtr::RelativeControlNetPoint(
  int relative_vertex_index
) const
{
  const ON_SubDVertex* v = RelativeVertex(relative_vertex_index);
  return (nullptr != v) ? v->ControlNetPoint() : ON_3dPoint::NanPoint;
}

const ON_Line ON_SubDEdgePtr::RelativeControlNetLine() const
{
  return ON_Line(RelativeControlNetPoint(0), RelativeControlNetPoint(1));
}

const ON_3dVector ON_SubDEdgePtr::RelativeControlNetDirection() const
{
  return RelativeControlNetLine().Direction();
}

double ON_SubDEdgePtr::RelativeSectorCoefficient(
  int relative_vertex_index
) const
{
  for (;;)
  {
    if (relative_vertex_index < 0 || relative_vertex_index>1)
      break;
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(m_ptr);
    if (nullptr == edge)
      break;
    if (0 != ON_SUBD_EDGE_DIRECTION(m_ptr))
      relative_vertex_index = 1 - relative_vertex_index;
    return edge->m_sector_coefficient[relative_vertex_index];
  }
  return ON_SubDSectorType::ErrorSectorCoefficient;
}

const ON_3dVector ON_SubDEdgePtr::RelativeDirection() const
{
  for (;;)
  {
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(m_ptr);
    if (nullptr == edge)
      break;
    if (nullptr == edge->m_vertex[0] || nullptr == edge->m_vertex[1])
      break;
    const int i0 = (0 != ON_SUBD_EDGE_DIRECTION(m_ptr)) ? 1 : 0;
    const ON_3dPoint P0(edge->m_vertex[i0]->m_P);
    const ON_3dPoint P1(edge->m_vertex[1-i0]->m_P);
    return (P1 - P0);
  }
  return ON_3dVector::NanVector;
}

const ON_ComponentStatus ON_SubDEdgePtr::Status() const
{
  const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr == edge) ? ON_ComponentStatus::NoneSet : edge->m_status;
}

const ON_SubDEdgePtr ON_SubDEdgePtr::Reversed() const
{
  return ON_SubDEdgePtr::Create(ON_SUBD_EDGE_POINTER(m_ptr), 1 - (m_ptr & 1));
}

const ON_SubDEdgePtr ON_SubDEdgePtr::Create(
  const class ON_SubDEdge* edge
  )
{
  ON_SubDEdgePtr eptr = { (ON__UINT_PTR)edge };
  return eptr;
}

const ON_SubDEdgePtr ON_SubDEdgePtr::Create(
  const class ON_SubDEdge* edge,
  ON__UINT_PTR direction
  )
{
  ON_SubDEdgePtr eptr = { (ON__UINT_PTR)edge | (direction & ON_SUBD_COMPONENT_DIRECTION_MASK) };
  return eptr;
}

const ON_SubDEdgePtr ON_SubDEdgePtr::Create(
  const class ON_SubDComponentPtr& edge_element
  )
{
  return ON_SubDEdgePtr::Create(edge_element.Edge(), edge_element.ComponentDirection());
}

const ON_SubDEdgePtr ON_SubDEdgePtr::CreateFromStartVertex(
  const class ON_SubDEdge* edge,
  const ON_SubDVertex* start_vertex
)
{
  for (;;)
  {
    if (nullptr == edge || nullptr == start_vertex)
      break;
    if (edge->m_vertex[0] == edge->m_vertex[1])
      break;
    
    ON__UINT_PTR dir;
    if (start_vertex == edge->m_vertex[0])
      dir = 0;
    else if (start_vertex == edge->m_vertex[1])
      dir = 1;
    else
      break;
    return ON_SubDEdgePtr::Create(edge, dir);
  }
  return ON_SubDEdgePtr::Null;
}

const ON_SubDEdgePtr ON_SubDEdgePtr::CreateFromEndVertex(
  const class ON_SubDEdge* edge,
  const ON_SubDVertex* end_vertex
)
{
  return CreateFromStartVertex(edge,end_vertex).Reversed();
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFacePtr
//

bool ON_SubDFacePtr::IsNull() const
{
  return (nullptr == ON_SUBD_FACE_POINTER(m_ptr));
}

bool ON_SubDFacePtr::IsNotNull() const
{
  return (nullptr != ON_SUBD_FACE_POINTER(m_ptr));
}

ON_SubDFace* ON_SubDFacePtr::Face() const
{
  return ON_SUBD_FACE_POINTER(m_ptr);
}

unsigned int ON_SubDFacePtr::FaceId() const
{
  const ON_SubDFace* f = ON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr != f) ? f->m_id : 0U;
}

unsigned int ON_SubDFacePtr::FaceEdgeCount() const
{
  const ON_SubDFace* f = ON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr != f) ? ((unsigned int)f->m_edge_count) : 0U;
}

ON__UINT_PTR ON_SubDFacePtr::FaceDirection() const
{
  return ON_SUBD_FACE_DIRECTION(m_ptr);
}

const ON_ComponentStatus ON_SubDFacePtr::Status() const
{
  const ON_SubDFace* face = ON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr == face) ? ON_ComponentStatus::NoneSet : face->m_status;
}

const ON_SubDFacePtr ON_SubDFacePtr::Reversed() const
{
  return ON_SubDFacePtr::Create(ON_SUBD_FACE_POINTER(m_ptr), 1 - (m_ptr & 1));
}

const ON_SubDFacePtr ON_SubDFacePtr::Create(
  const class ON_SubDFace* face,
  ON__UINT_PTR direction
  )
{
  ON_SubDFacePtr fptr = { (ON__UINT_PTR)face | (direction & ON_SUBD_COMPONENT_DIRECTION_MASK) };
  return fptr;
}

const ON_SubDFacePtr ON_SubDFacePtr::Create(
  const class ON_SubDComponentPtr& face_element
  )
{
  return ON_SubDFacePtr::Create(face_element.Face(), face_element.ComponentDirection());
}


int ON_SubDFacePtr::Compare(
  const ON_SubDFacePtr* lhs,
  const ON_SubDFacePtr* rhs
)
{
  if ( nullptr == lhs )
    return 1;
  if ( nullptr == rhs )
    return -1;

  if (lhs->m_ptr < rhs->m_ptr)
    return -1;
  if (lhs->m_ptr > rhs->m_ptr)
    return 1;

  return 0;
}

int ON_SubDFacePtr::CompareFacePointer(
  const ON_SubDFacePtr* lhs,
  const ON_SubDFacePtr* rhs
)
{
  if (lhs == rhs)
    return 0;
  if ( nullptr == lhs )
    return 1;
  if ( nullptr == rhs )
    return -1;

  const ON__UINT_PTR lhs_ptr = (lhs->m_ptr & ON_SUBD_COMPONENT_POINTER_MASK);
  const ON__UINT_PTR rhs_ptr = (rhs->m_ptr & ON_SUBD_COMPONENT_POINTER_MASK);
  if (lhs_ptr < rhs_ptr)
    return -1;
  if (lhs_ptr > rhs_ptr)
    return 1;
  return 0;
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentPtr
//

bool ON_SubDComponentPtr::IsNull() const
{
  return (0 == (ON_SUBD_COMPONENT_POINTER_MASK && m_ptr));
}

bool ON_SubDComponentPtr::IsNotNull() const
{
  if (nullptr != ON_SUBD_COMPONENT_POINTER(m_ptr))
  {
    switch (ON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
    {
    case ON_SUBD_COMPONENT_TYPE_VERTEX:
    case ON_SUBD_COMPONENT_TYPE_EDGE:
    case ON_SUBD_COMPONENT_TYPE_FACE:
      return true;
    }
  }
  return false;
}

unsigned int ON_SubDComponentPtr::ComponentId() const
{
  const ON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_id : 0U;
}

const ON_3dPoint ON_SubDComponentPtr::ControlNetCenterPoint() const
{
  switch (ON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case ON_SUBD_COMPONENT_TYPE_VERTEX:
  {
    const ON_SubDVertex* v = Vertex();
    if (nullptr != v)
      return v->ControlNetPoint();
  }
  break;
  case ON_SUBD_COMPONENT_TYPE_EDGE:
  {
    const ON_SubDEdge* e = Edge();
    if (nullptr != e)
      return e->ControlNetCenterPoint();
  }
  break;
  case ON_SUBD_COMPONENT_TYPE_FACE:
  {
    const ON_SubDFace* f = Face();
    if (nullptr != f)
      return f->ControlNetCenterPoint();
  }
  break;
  }
  return ON_3dPoint::NanPoint;
}

const ON_BoundingBox ON_SubDComponentPtr::ControlNetBoundingBox() const
{
  switch (ON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case ON_SUBD_COMPONENT_TYPE_VERTEX:
  {
    const ON_SubDVertex* v = Vertex();
    if (nullptr != v)
      return v->ControlNetBoundingBox();
  }
  break;
  case ON_SUBD_COMPONENT_TYPE_EDGE:
  {
    const ON_SubDEdge* e = Edge();
    if (nullptr != e)
      return e->ControlNetBoundingBox();
  }
  break;
  case ON_SUBD_COMPONENT_TYPE_FACE:
  {
    const ON_SubDFace* f = Face();
    if (nullptr != f)
      return f->ControlNetBoundingBox();
  }
  break;
  }
  return ON_BoundingBox::NanBoundingBox;
}



ON__UINT16 ON_SubDComponentPtr::Hash16FromTypeAndId() const
{
  const ON_SubDComponentBase* c = ComponentBase();
  return (0 != c)
    ? ON_CRC16((ON__UINT16)(ON_SUBD_COMPONENT_TYPE_MASK & m_ptr), sizeof(c->m_id), &(c->m_id))
    : ((ON__UINT16)0U)
    ;
}

ON__UINT32 ON_SubDComponentPtr::Hash32FromPointer() const
{
  const ON__UINT_PTR ptr = (ON__UINT_PTR)ComponentBase();
  return ON_CRC32((ON__UINT32)(ON_SUBD_COMPONENT_TYPE_MASK & m_ptr), sizeof(ptr), &ptr);
}


ON_SubDComponentPtr::Type ON_SubDComponentPtr::ComponentType() const
{
  switch (ON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case ON_SUBD_COMPONENT_TYPE_VERTEX:
    return ON_SubDComponentPtr::Type::Vertex;
  case ON_SUBD_COMPONENT_TYPE_EDGE:
    return ON_SubDComponentPtr::Type::Edge;
  case ON_SUBD_COMPONENT_TYPE_FACE:
    return ON_SubDComponentPtr::Type::Face;
  }
  return ON_SubDComponentPtr::Type::Unset;
}


const ON_ComponentStatus ON_SubDComponentPtr::Status() const
{
  switch (ComponentType())
  {
  case ON_SubDComponentPtr::Type::Vertex:
    {
      const ON_SubDVertex* vertex = Vertex();
      if ( nullptr != vertex )
        return vertex->m_status;
    }
    break;
  case ON_SubDComponentPtr::Type::Edge:
    {
      const ON_SubDEdge* edge = Edge();
      if ( nullptr != edge )
        return edge->m_status;
    }
    break;
  case ON_SubDComponentPtr::Type::Face:
    {
      const ON_SubDFace* face = Face();
      if ( nullptr != face )
        return face->m_status;
    }
    break;
  }
  return ON_ComponentStatus::NoneSet;
}

unsigned int ON_SubDComponentPtr::SetStatus(
  ON_ComponentStatus status
  )
{
  switch (ComponentType())
  {
  case ON_SubDComponentPtr::Type::Vertex:
    {
      const ON_SubDVertex* vertex = Vertex();
      if (nullptr != vertex)
        return vertex->m_status.SetStatus(status);
    }
    break;
  case ON_SubDComponentPtr::Type::Edge:
    {
      const ON_SubDEdge* edge = Edge();
      if (nullptr != edge)
        return edge->m_status.SetStatus(status);
    }
    break;
  case ON_SubDComponentPtr::Type::Face:
    {
      const ON_SubDFace* face = Face();
      if (nullptr != face)
        return face->m_status.SetStatus(status);
    }
    break;
  }
  return ON_SUBD_RETURN_ERROR(0);
}


unsigned int ON_SubDComponentPtr::SetStates(
  ON_ComponentStatus states_to_set
  )
{
  switch (ComponentType())
  {
  case ON_SubDComponentPtr::Type::Vertex:
    {
      const ON_SubDVertex* vertex = Vertex();
      if (nullptr != vertex)
        return vertex->m_status.SetStates(states_to_set);
    }
    break;
  case ON_SubDComponentPtr::Type::Edge:
    {
      const ON_SubDEdge* edge = Edge();
      if (nullptr != edge)
        return edge->m_status.SetStates(states_to_set);
    }
    break;
  case ON_SubDComponentPtr::Type::Face:
    {
      const ON_SubDFace* face = Face();
      if (nullptr != face)
        return face->m_status.SetStates(states_to_set);
    }
    break;
  }
  return ON_SUBD_RETURN_ERROR(0);
}


unsigned int ON_SubDComponentPtr::ClearStates(
  ON_ComponentStatus states_to_clear
  )
{
  switch (ComponentType())
  {
  case ON_SubDComponentPtr::Type::Vertex:
    {
      const ON_SubDVertex* vertex = Vertex();
      if (nullptr != vertex)
        return vertex->m_status.ClearStates(states_to_clear);
    }
    break;
  case ON_SubDComponentPtr::Type::Edge:
    {
      const ON_SubDEdge* edge = Edge();
      if (nullptr != edge)
        return edge->m_status.ClearStates(states_to_clear);
    }
    break;
  case ON_SubDComponentPtr::Type::Face:
    {
      const ON_SubDFace* face = Face();
      if (nullptr != face)
        return face->m_status.ClearStates(states_to_clear);
    }
    break;
  }
  return ON_SUBD_RETURN_ERROR(0);
}

bool ON_SubDComponentPtr::Mark() const
{
  const ON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool ON_SubDComponentPtr::ClearMark() const
{
  const ON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool ON_SubDComponentPtr::SetMark() const
{
  const ON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool ON_SubDComponentPtr::SetMark(
  bool bMark
) const
{
  const ON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}

bool ON_SubDVertexPtr::Mark() const
{
  const ON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool ON_SubDVertexPtr::ClearMark() const
{
  const ON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool ON_SubDVertexPtr::SetMark() const
{
  const ON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool ON_SubDVertexPtr::SetMark(
  bool bMark
) const
{
  const ON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}


bool ON_SubDEdgePtr::Mark() const
{
  const ON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool ON_SubDEdgePtr::ClearMark() const
{
  const ON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool ON_SubDEdgePtr::SetMark() const
{
  const ON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool ON_SubDEdgePtr::SetMark(
  bool bMark
) const
{
  const ON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}


bool ON_SubDFacePtr::Mark() const
{
  const ON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool ON_SubDFacePtr::ClearMark() const
{
  const ON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool ON_SubDFacePtr::SetMark() const
{
  const ON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool ON_SubDFacePtr::SetMark(
  bool bMark
) const
{
  const ON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}

ON__UINT_PTR ON_SubDComponentPtr::ComponentDirection() const
{
  return ON_SUBD_COMPONENT_DIRECTION(m_ptr);
}

const ON_SubDComponentPtr ON_SubDComponentPtr::ClearComponentDirection() const
{
  ON_SubDComponentPtr component_ptr = *this;
  component_ptr.m_ptr &= (ON_SUBD_COMPONENT_POINTER_MASK | ON_SUBD_COMPONENT_TYPE_MASK);
  return component_ptr;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::SetComponentDirection() const
{
  ON_SubDComponentPtr component_ptr = *this;
  component_ptr.m_ptr |= ON_SUBD_COMPONENT_DIRECTION_MASK;
  return component_ptr;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::SetComponentDirection(ON__UINT_PTR dir) const
{
  ON_SubDComponentPtr component_ptr = *this;
  if (0 == dir)
    component_ptr.m_ptr &= ~((ON__UINT_PTR)ON_SUBD_COMPONENT_DIRECTION_MASK);
  else if (1 == dir)
    component_ptr.m_ptr |= ON_SUBD_COMPONENT_DIRECTION_MASK;
  else
    ON_SUBD_ERROR("Invalid dir parameter");
  return component_ptr;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::ToggleComponentDirection() const
{
  return (0 != (m_ptr & ON_SUBD_COMPONENT_DIRECTION_MASK)) ? ClearComponentDirection() : SetComponentDirection();
}

const ON_SubDComponentPtr ON_SubDComponentPtr::CreateNull(
  ON_SubDComponentPtr::Type component_type,
  ON__UINT_PTR component_direction
)
{
  ON_SubDComponentPtr component_ptr;
  switch (component_type)
  {
  case ON_SubDComponentPtr::Type::Unset:
    component_ptr.m_ptr = 0;
    break;
  case ON_SubDComponentPtr::Type::Vertex:
    component_ptr.m_ptr = ON_SUBD_COMPONENT_TYPE_VERTEX;
    break;
  case ON_SubDComponentPtr::Type::Edge:
    component_ptr.m_ptr = ON_SUBD_COMPONENT_TYPE_EDGE;
    break;
  case ON_SubDComponentPtr::Type::Face:
    component_ptr.m_ptr = ON_SUBD_COMPONENT_TYPE_FACE;
    break;
  default:
    component_ptr.m_ptr = 0;
    break;
  }
  if (1 == component_direction)
    component_ptr.m_ptr |= ON_SUBD_COMPONENT_DIRECTION_MASK;
  return component_ptr;
}


class ON_SubDComponentBase* ON_SubDComponentPtr::ComponentBase() const
{
  switch ((ON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
  {
  case ON_SUBD_COMPONENT_TYPE_VERTEX:
  case ON_SUBD_COMPONENT_TYPE_EDGE:
  case ON_SUBD_COMPONENT_TYPE_FACE:
    return ((class ON_SubDComponentBase*)ON_SUBD_COMPONENT_POINTER(m_ptr));
  }
  return nullptr;
}


class ON_SubDVertex* ON_SubDComponentPtr::Vertex() const
{
  if (ON_SUBD_COMPONENT_TYPE_VERTEX == (ON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
    return ON_SUBD_VERTEX_POINTER(m_ptr);
  return nullptr;
}

class ON_SubDEdge* ON_SubDComponentPtr::Edge() const
{
  if (ON_SUBD_COMPONENT_TYPE_EDGE == (ON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
    return ON_SUBD_EDGE_POINTER(m_ptr);
  return nullptr;
}

class ON_SubDFace* ON_SubDComponentPtr::Face() const
{
  if (ON_SUBD_COMPONENT_TYPE_FACE == (ON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
    return ON_SUBD_FACE_POINTER(m_ptr);
  return nullptr;
}

const ON_SubDVertexPtr ON_SubDComponentPtr::VertexPtr() const
{
  ON__UINT_PTR element_type = ON_SUBD_COMPONENT_TYPE(m_ptr);
  if ( ON_SUBD_COMPONENT_TYPE_VERTEX == element_type)
    return ON_SubDVertexPtr::Create(Vertex(), ComponentDirection());

  if ( 0 == element_type )
    return ON_SubDVertexPtr::Null;

  return ON_SUBD_RETURN_ERROR(ON_SubDVertexPtr::Null);
}

const ON_SubDEdgePtr ON_SubDComponentPtr::EdgePtr() const
{
  ON__UINT_PTR element_type = ON_SUBD_COMPONENT_TYPE(m_ptr);
  if ( ON_SUBD_COMPONENT_TYPE_EDGE == element_type)
    return ON_SubDEdgePtr::Create(Edge(), ComponentDirection());

  if ( 0 == element_type )
    return ON_SubDEdgePtr::Null;

  return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);
}

const ON_SubDFacePtr ON_SubDComponentPtr::FacePtr() const
{
  ON__UINT_PTR element_type = ON_SUBD_COMPONENT_TYPE(m_ptr);
  if ( ON_SUBD_COMPONENT_TYPE_FACE == element_type)
    return ON_SubDFacePtr::Create(Face(), ComponentDirection());

  if ( 0 == element_type )
    return ON_SubDFacePtr::Null;

  return ON_SUBD_RETURN_ERROR(ON_SubDFacePtr::Null);
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  const class ON_SubDVertex* vertex
  )
{
  if (nullptr != vertex)
  {
    ON_SubDComponentPtr vptr = { (ON__UINT_PTR)vertex | ON_SUBD_COMPONENT_TYPE_VERTEX };
    return vptr;
  }
  return ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  const class ON_SubDEdge* edge
  )
{
  if (nullptr != edge)
  {
    ON_SubDComponentPtr eptr = { (ON__UINT_PTR)edge | ON_SUBD_COMPONENT_TYPE_EDGE };
    return eptr;
  }
  return ON_SubDComponentPtr::Null;
}


const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  const class ON_SubDFace* face
  )
{
  if (nullptr != face)
  {
    ON_SubDComponentPtr fptr = { (ON__UINT_PTR)face | ON_SUBD_COMPONENT_TYPE_FACE };
    return fptr;
  }
  return ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  const class ON_SubDVertex* vertex,
  ON__UINT_PTR vertex_direction
  )
{
  if (nullptr != vertex)
  {
    ON_SubDComponentPtr vptr = { (ON__UINT_PTR)vertex | ON_SUBD_COMPONENT_TYPE_VERTEX | (vertex_direction & ON_SUBD_COMPONENT_DIRECTION_MASK) };
    return vptr;
  }
  return ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  const class ON_SubDEdge* edge,
  ON__UINT_PTR edge_direction
  )
{
  if (nullptr != edge)
  {    
    ON_SubDComponentPtr eptr = { (ON__UINT_PTR)edge | (ON_SUBD_COMPONENT_TYPE_EDGE | (edge_direction & ON_SUBD_COMPONENT_DIRECTION_MASK)) };
    return eptr;
  }
  return ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  const class ON_SubDFace* face,
  ON__UINT_PTR face_direction
  )
{
  if (nullptr != face)
  {
    ON_SubDComponentPtr fptr = { (ON__UINT_PTR)face | (ON_SUBD_COMPONENT_TYPE_FACE | (face_direction & ON_SUBD_COMPONENT_DIRECTION_MASK)) };
    return fptr;
  }
  return ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  ON_SubDVertexPtr vertexptr
  )
{
  return Create(vertexptr.Vertex(), vertexptr.VertexDirection());
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  ON_SubDEdgePtr edgeptr
  )
{
  return Create(edgeptr.Edge(), edgeptr.EdgeDirection());
}

const ON_SubDComponentPtr ON_SubDComponentPtr::Create(
  ON_SubDFacePtr faceptr
  )
{
  return Create(faceptr.Face(), faceptr.FaceDirection());
}

int ON_SubDComponentPtr::CompareComponentPtrType(
  ON_SubDComponentPtr::Type a,
  ON_SubDComponentPtr::Type b
  )
{
  if ( a == b )
    return 0;
  switch (a)
  {
  case ON_SubDComponentPtr::Type::Vertex:
    return -1;
    break;
  case ON_SubDComponentPtr::Type::Edge:
    return (ON_SubDComponentPtr::Type::Vertex == b) ? 1 : -1;
    break;
  case ON_SubDComponentPtr::Type::Face:
    return (ON_SubDComponentPtr::Type::Vertex == b || ON_SubDComponentPtr::Type::Edge == b) ? 1 : -1;
    break;
  default:
    break;
  }
  return (((unsigned char)a) < ((unsigned char)b)) ? -1 : 1;
}

int ON_SubDComponentPtr::CompareType(
  const ON_SubDComponentPtr* a,
  const ON_SubDComponentPtr* b
  )
{
  if ( a == b )
    return 0;
  if ( nullptr == a )
    return 1;
  if ( nullptr == b )
    return -1;
  return ON_SubDComponentPtr::CompareComponentPtrType(a->ComponentType(), b->ComponentType());
}


int ON_SubDComponentPtr::CompareComponent(
  const ON_SubDComponentPtr* a,
  const ON_SubDComponentPtr* b
)
{
  if (a == b)
    return 0;
  const int rc = ON_SubDComponentPtr::CompareComponentPtrType(a->ComponentType(), b->ComponentType());
  if (0 == rc)
  {
    const ON__UINT_PTR x = a->m_ptr;
    const ON__UINT_PTR y = b->m_ptr;
    if (x < y)
      return -1;
    if (x > y)
      return 1;
  }
  return rc;
}

int ON_SubDComponentPtr::CompareComponentAndDirection(
  const ON_SubDComponentPtr* a,
  const ON_SubDComponentPtr* b
)
{
  if (a == b)
    return 0;
  const int rc = ON_SubDComponentPtr::CompareComponent(a, b);
  if (0 == rc)
  {
    const ON__UINT_PTR x = (a->m_ptr & ON_SUBD_COMPONENT_POINTER_MASK);
    const ON__UINT_PTR y = (b->m_ptr & ON_SUBD_COMPONENT_POINTER_MASK);
    if (x < y)
      return -1;
    if (x > y)
      return 1;
  }
  return rc;
}


int ON_SubDComponentPoint::CompareComponentAndDirection(
  const ON_SubDComponentPoint* a,
  const ON_SubDComponentPoint* b
  )
{
  if ( a == b)
    return 0;  
  if ( nullptr == a)
    return 1; // nullptr > non-null pointer.
  if ( nullptr == b)
    return -1; // nullptr > non-null pointer.

  // 1st: compare component type
  // unset < vertex < edge < face
  ON__UINT_PTR x = (ON_SUBD_COMPONENT_TYPE_MASK & a->m_component_ptr.m_ptr);
  ON__UINT_PTR y = (ON_SUBD_COMPONENT_TYPE_MASK & b->m_component_ptr.m_ptr);
  if ( x < y )
    return -1;
  if ( x > y )
    return 1;

  // 2nd: compare component pointer
  x = (a->m_component_ptr.m_ptr & ON_SUBD_COMPONENT_POINTER_MASK);
  y = (b->m_component_ptr.m_ptr & ON_SUBD_COMPONENT_POINTER_MASK);
  if (x < y)
    return -1;
  if (x > y)
    return 1;

  // 3rd: compare component direction flag
  x = (a->m_component_ptr.m_ptr & ON_SUBD_COMPONENT_DIRECTION_MASK);
  y = (b->m_component_ptr.m_ptr & ON_SUBD_COMPONENT_DIRECTION_MASK);
  if (x < y)
    return -1;
  if (x > y)
    return 1;

  return 0;
}


const ON_SubDComponentPtrPair ON_SubDComponentPtrPair::Create(ON_SubDComponentPtr first_ptr, ON_SubDComponentPtr second_ptr)
{
  ON_SubDComponentPtrPair p;
  p.m_pair[0] = first_ptr;
  p.m_pair[1] = second_ptr;
  return p;
}

int ON_SubDComponentPtrPair::CompareComponent(
  const ON_SubDComponentPtrPair* lhs,
  const ON_SubDComponentPtrPair* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls sort to end.
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  int rc = ON_SubDComponentPtr::CompareComponent(&lhs->m_pair[0], &rhs->m_pair[0]);
  if (0 == rc)
    rc = ON_SubDComponentPtr::CompareComponent(&lhs->m_pair[1], &rhs->m_pair[1]);
  return rc;
}

int ON_SubDComponentPtrPair::CompareComponentAndDirection(
  const ON_SubDComponentPtrPair* lhs,
  const ON_SubDComponentPtrPair* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls sort to end.
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  int rc = ON_SubDComponentPtr::CompareComponentAndDirection(&lhs->m_pair[0], &rhs->m_pair[0]);
  if (0 == rc)
    rc = ON_SubDComponentPtr::CompareComponentAndDirection(&lhs->m_pair[1], &rhs->m_pair[1]);
  return rc;
}


int ON_SubDComponentPtrPair::CompareFirstPointer(
  const ON_SubDComponentPtrPair* lhs,
  const ON_SubDComponentPtrPair* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls sort to end.
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  const ON__UINT_PTR lhs_ptr = (ON_SUBD_COMPONENT_POINTER_MASK & lhs->m_pair[0].m_ptr);
  const ON__UINT_PTR rhs_ptr = (ON_SUBD_COMPONENT_POINTER_MASK & rhs->m_pair[0].m_ptr);
  if (lhs_ptr < rhs_ptr)
    return -1;
  if (lhs_ptr > rhs_ptr)
    return 1;
  return 0;
}

ON_SubDComponentPtr::Type ON_SubDComponentPtrPair::ComponentType() const
{
  const ON_SubDComponentPtr::Type type = m_pair[0].ComponentType();
  return (type == m_pair[1].ComponentType()) ? type : ON_SubDComponentPtr::Type::Unset;
}

const ON_SubDComponentPtrPair ON_SubDComponentPtrPair::SwapPair() const
{
  return ON_SubDComponentPtrPair::Create(m_pair[1], m_pair[0]);
}

const ON_SubDComponentPtr ON_SubDComponentPtrPair::First() const
{
  return m_pair[0];
}

const ON_SubDComponentPtr ON_SubDComponentPtrPair::Second() const
{
  return m_pair[1];
}

bool ON_SubDComponentPtrPair::FirstIsNull() const
{
  return (0 == (ON_SUBD_COMPONENT_POINTER_MASK & m_pair[0].m_ptr));
}

bool ON_SubDComponentPtrPair::SecondIsNull() const
{
  return (0 == (ON_SUBD_COMPONENT_POINTER_MASK & m_pair[1].m_ptr));
}

bool ON_SubDComponentPtrPair::BothAreNull() const
{
  return (0 == (ON_SUBD_COMPONENT_POINTER_MASK & m_pair[0].m_ptr)) && 0 == (ON_SUBD_COMPONENT_POINTER_MASK & m_pair[1].m_ptr);
}

//////////////////////////////////////////////////////////////////////////
//
// ON_ToSubDParameters
//

ON_ToSubDParameters::ConvexCornerOption ON_ToSubDParameters::ConvexCornerOptionFromUnsigned(
  unsigned int convex_corner_option_as_unsigned
  )
{
  switch (convex_corner_option_as_unsigned)
  {
	case (unsigned int)ON_ToSubDParameters::ConvexCornerOption::Unset:
		return ON_ToSubDParameters::ConvexCornerOption::Unset;
	case (unsigned int)ON_ToSubDParameters::ConvexCornerOption::None:
		return ON_ToSubDParameters::ConvexCornerOption::None;
	case (unsigned int)ON_ToSubDParameters::ConvexCornerOption::AtMeshCorner:
		return ON_ToSubDParameters::ConvexCornerOption::AtMeshCorner;
  default:
    break;
  }
	return ON_ToSubDParameters::ConvexCornerOption::Unset;
}

void ON_ToSubDParameters::SetConvexCornerOption(
  ON_ToSubDParameters::ConvexCornerOption convex_corner_option
  )
{
  m_convex_corner_option = ON_ToSubDParameters::ConvexCornerOptionFromUnsigned((unsigned int)convex_corner_option);
}

ON_ToSubDParameters::ConvexCornerOption ON_ToSubDParameters::ConvexCornerTest() const
{
  switch (m_convex_corner_option)
  {
  case ON_ToSubDParameters::ConvexCornerOption::Unset:
  case ON_ToSubDParameters::ConvexCornerOption::None:
    return m_convex_corner_option;

  case ON_ToSubDParameters::ConvexCornerOption::AtMeshCorner:
    if ( m_maximum_convex_corner_edge_count >= 2 
         && m_maximum_convex_corner_edge_count <= ON_SubDVertex::MaximumEdgeCount
         && m_maximum_convex_corner_angle_radians >= 0.0
         && m_maximum_convex_corner_angle_radians < ON_PI
         )
         return m_convex_corner_option;
    break;
  }

  return ON_ToSubDParameters::ConvexCornerOption::Unset;
}

void ON_ToSubDParameters::SetMaximumConvexCornerEdgeCount(
  unsigned int maximum_convex_corner_edge_count
  )
{
  if ( maximum_convex_corner_edge_count >= 2 && maximum_convex_corner_edge_count <= ON_SubDVertex::MaximumEdgeCount)
    m_maximum_convex_corner_edge_count = (unsigned short)maximum_convex_corner_edge_count;
}

unsigned int ON_ToSubDParameters::MaximumConvexCornerEdgeCount() const
{
  return m_maximum_convex_corner_edge_count;
}

void ON_ToSubDParameters::SetMaximumConvexCornerAngleRadians(
  double maximum_convex_corner_angle_radians
  )
{
  if (maximum_convex_corner_angle_radians > 0.0 && maximum_convex_corner_angle_radians < ON_PI)
    m_maximum_convex_corner_angle_radians = maximum_convex_corner_angle_radians;
}

double ON_ToSubDParameters::MaximumConvexCornerAngleRadians() const
{
  return m_maximum_convex_corner_angle_radians;
}

ON_ToSubDParameters::ConvexCornerOption ON_ToSubDParameters::CopyConvexCornerTest(
  ON_ToSubDParameters source_parameters
  )
{
  SetConvexCornerOption(source_parameters.ConvexCornerTest());
  SetMaximumConvexCornerEdgeCount(source_parameters.MaximumConvexCornerEdgeCount());
  SetMaximumConvexCornerAngleRadians(source_parameters.MaximumConvexCornerAngleRadians());
  return ConvexCornerTest();
}

bool ON_ToSubDParameters::InterpolateMeshVertices() const
{
  return m_bInterpolateMeshVertices;
}

void ON_ToSubDParameters::SetInterpolateMeshVertices(
  bool bInterpolateMeshVertices
)
{
  // Not supported in free opennurbs
  m_bInterpolateMeshVertices = false;
}

bool ON_ToSubDParameters::MergeColinearBoundaryEdges() const
{
  // clear bit means true, set bit means false
  return (0 == (ON_ToSubDParameters::MergeColinearBoundaryEdgesMask & m_merge_edges_bits));
}

void ON_ToSubDParameters::SetMergeColinearBoundaryEdges(
  bool bAllowColinearBoundaryEdges
)
{
  const unsigned char mask = ON_ToSubDParameters::MergeColinearBoundaryEdgesMask;
  if (false == bAllowColinearBoundaryEdges)
    m_merge_edges_bits |= mask; // set bit
  else
    m_merge_edges_bits &= ~mask; // clear bit
}

bool ON_ToSubDParameters::MergeColinearInteriorEdges() const
{
  // clear bit means true, set bit means false
  return (0 == (ON_ToSubDParameters::MergeColinearInteriorEdgesMask & m_merge_edges_bits));
}

void ON_ToSubDParameters::SetMergeColinearInteriorEdges(
  bool bAllowColinearInteriorEdges
)
{
  const unsigned char mask = ON_ToSubDParameters::MergeColinearInteriorEdgesMask;
  if (false == bAllowColinearInteriorEdges)
    m_merge_edges_bits |= mask; // set bit
  else
    m_merge_edges_bits &= ~mask; // clear bit
}


void ON_ToSubDParameters::SetInteriorCreaseOption(
  ON_ToSubDParameters::InteriorCreaseOption interior_crease_option
  )
{
  m_interior_crease_option = ON_ToSubDParameters::InteriorCreaseOptionFromUnsigned((unsigned int)interior_crease_option);
}

ON_ToSubDParameters::InteriorCreaseOption ON_ToSubDParameters::InteriorCreaseTest() const
{
  return m_interior_crease_option;
}

void ON_ToSubDParameters::SetMinimumCreaseAngleRadians(
  double minimum_crease_angle_radians
  )
{
  if (minimum_crease_angle_radians >= 0.0 && minimum_crease_angle_radians < ON_PI)
    m_minimum_crease_angle_radians = minimum_crease_angle_radians;
}


double ON_ToSubDParameters::MinimumCreaseAngleRadians() const
{
  return m_minimum_crease_angle_radians;
}

ON_ToSubDParameters::InteriorCreaseOption ON_ToSubDParameters::CopyInteriorCreaseTest(
  ON_ToSubDParameters source_parameters
  )
{
  SetInteriorCreaseOption(source_parameters.InteriorCreaseTest());
  SetMinimumCreaseAngleRadians(source_parameters.MinimumCreaseAngleRadians());
  return InteriorCreaseTest();
}

ON_ToSubDParameters::InteriorCreaseOption ON_ToSubDParameters::InteriorCreaseOptionFromUnsigned(
  unsigned int interior_crease_option_as_unsigned
  )
{
  switch (interior_crease_option_as_unsigned)
  {
  case (unsigned int)ON_ToSubDParameters::InteriorCreaseOption::Unset:
    return ON_ToSubDParameters::InteriorCreaseOption::Unset;
    break;
  case (unsigned int)ON_ToSubDParameters::InteriorCreaseOption::None:
    return ON_ToSubDParameters::InteriorCreaseOption::None;
    break;
  case (unsigned int)ON_ToSubDParameters::InteriorCreaseOption::AtMeshCrease:
    return ON_ToSubDParameters::InteriorCreaseOption::AtMeshCrease;
    break;
  case (unsigned int)ON_ToSubDParameters::InteriorCreaseOption::AtMeshEdge:
    return ON_ToSubDParameters::InteriorCreaseOption::AtMeshEdge;
    break;
  default:
    break;
  }

  return ON_ToSubDParameters::InteriorCreaseOption::Unset;
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertex
//

const ON_SubDEdgePtr ON_SubDVertex::EdgePtr(
  unsigned int i
) const
{
  return (i < m_edge_count) ? m_edges[i] : ON_SubDEdgePtr::Null;
}


const class ON_SubDEdge* ON_SubDVertex::Edge(
  unsigned int i
  ) const
{
  return (i < m_edge_count) ? ON_SUBD_EDGE_POINTER(m_edges[i].m_ptr) : nullptr;
}

ON__UINT_PTR ON_SubDVertex::EdgeDirection(
  unsigned int i
  ) const
{
  return (i < m_edge_count) ? ON_SUBD_EDGE_DIRECTION(m_edges[i].m_ptr) : 0;
}

unsigned int ON_SubDVertex::EdgeCount() const
{
  return m_edge_count;
}

unsigned int ON_SubDVertex::EdgeCount(
  ON_SubD::EdgeTag edge_tag
  ) const
{
  if (nullptr != m_edges)
  {
    unsigned int matching_edge_count = 0;
    const unsigned int edge_count = m_edge_count;
    for (unsigned int vei = 0; vei < edge_count; vei++)
    {
      const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr != e && edge_tag == e->m_edge_tag)
        matching_edge_count++;
    }
    return matching_edge_count;
  }
  return 0;
}

unsigned int ON_SubDVertex::EdgeArrayIndex(
  const ON_SubDEdge* edge
  ) const
{
  if ( nullptr == edge )
    return ON_UNSET_UINT_INDEX;
  const unsigned int edge_count = m_edge_count;
  if ( 0 == edge_count)
    return ON_UNSET_UINT_INDEX;
  const ON_SubDEdgePtr* eptr = m_edges;
  if ( nullptr == eptr)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_UINT_INDEX);

  for ( unsigned int i = 0; i < edge_count; i++, eptr++)
  {
    if (edge == ON_SUBD_EDGE_POINTER(eptr->m_ptr))
      return i;
  }

  return ON_UNSET_UINT_INDEX;
}

unsigned int ON_SubDVertex::MarkedEdgeCount() const
{
  unsigned int mark_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr != e && e->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

unsigned int ON_SubDVertex::MarkedFaceCount() const
{
  unsigned int mark_count = 0;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const ON_SubDFace* f = m_faces[vfi];
    if (nullptr != f && f->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

unsigned int ON_SubDVertex::MinimumFaceEdgeCount() const
{
  unsigned short min_count = 0xFFFFU;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const ON_SubDFace* f = m_faces[vfi];
    if (nullptr != f && f->m_edge_count < min_count)
      min_count = f->m_edge_count;
  }
  return min_count < 0xFFFFU ? min_count : 0;
}

unsigned int ON_SubDVertex::MaximumFaceEdgeCount() const
{
  unsigned short max_count = 0;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const ON_SubDFace* f = m_faces[vfi];
    if (nullptr != f && f->m_edge_count < max_count)
      max_count = f->m_edge_count;
  }
  return max_count;
}


unsigned int ON_SubDVertex::MinimumEdgeFaceCount() const
{
  unsigned short min_count = 0xFFFFU;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr != e && e->m_face_count < min_count)
      min_count = e->m_face_count;
  }
  return min_count < 0xFFFFU ? min_count : 0;
}

unsigned int ON_SubDVertex::MaximumEdgeFaceCount() const
{
  unsigned short max_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr != e && e->m_face_count > max_count)
    max_count = e->m_face_count;
  }
  return max_count;
}

unsigned int ON_SubDEdge::MarkedVertexCount() const
{
  unsigned int mark_count = 0;
  for (unsigned evi = 0; evi < 2; ++evi)
  {
    const ON_SubDVertex* v = m_vertex[evi];
    if (nullptr != v && v->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

unsigned int ON_SubDEdge::MarkedFaceCount() const
{
  unsigned int mark_count = 0;
  const ON_SubDFacePtr* fptr = m_face2;
  for (unsigned short efi = 0; efi < m_face_count; ++efi, ++fptr)
  {
    if (2 == efi)
    {
      fptr = m_facex;
      if (nullptr == fptr)
        break;
    }
    const ON_SubDFace* f = ON_SUBD_FACE_POINTER(fptr->m_ptr);
    if (nullptr != f && f->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

unsigned int ON_SubDVertex::FaceCount() const
{
  return m_face_count;
}

const class ON_SubDFace* ON_SubDVertex::Face(
  unsigned int i
  ) const
{
  //return (i < m_face_count) ? ON_SUBD_FACE_POINTER(m_faces[i].m_ptr) : nullptr;
  return (i < m_face_count) ? m_faces[i] : nullptr;
}


unsigned int ON_SubDVertex::FaceArrayIndex(
  const ON_SubDFace* face
  ) const
{
  if ( nullptr == face )
    return ON_UNSET_UINT_INDEX;
  const unsigned int face_count = m_face_count;
  if ( 0 == face_count)
    return ON_UNSET_UINT_INDEX;
  if ( nullptr == m_faces)
    return ON_SUBD_RETURN_ERROR(ON_UNSET_UINT_INDEX);

  for ( unsigned int i = 0; i < face_count; i++)
  {
    if (face == m_faces[i] )
      return i;
  }

  return ON_UNSET_UINT_INDEX;
}

unsigned int ON_SubDVertex::ReplaceFaceInArray(const ON_SubDFace * old_face, const ON_SubDFace * new_face)
{
  unsigned vfi = (nullptr != old_face && old_face != new_face) ? FaceArrayIndex(old_face) : ON_UNSET_UINT_INDEX;
  if (ON_UNSET_UINT_INDEX == vfi)
    return ON_UNSET_UINT_INDEX;
  if (nullptr != new_face)
  {
    m_faces[vfi] = new_face;
  }
  else
  {
    const unsigned c = (unsigned)(m_face_count--);
    while (++vfi < c)
      m_faces[vfi - 1] = m_faces[vfi];
  }
  return vfi;
}

const ON_3dPoint ON_SubDVertex::ControlNetPoint() const
{
  return ON_3dPoint(m_P);
}

bool ON_SubDVertex::IsSmooth() const
{
  return (ON_SubD::VertexTag::Smooth == m_vertex_tag);
}

bool ON_SubDVertex::IsCrease() const
{
  return (ON_SubD::VertexTag::Crease == m_vertex_tag);
}

bool ON_SubDVertex::IsCorner() const
{
  return (ON_SubD::VertexTag::Corner == m_vertex_tag);
}

bool ON_SubDVertex::IsDart() const
{
  return (ON_SubD::VertexTag::Dart == m_vertex_tag);
}

bool ON_SubDVertex::IsCreaseOrCorner() const
{
  return (ON_SubD::VertexTag::Crease == m_vertex_tag || ON_SubD::VertexTag::Corner == m_vertex_tag);
}

bool ON_SubDVertex::IsDartOrCreaseOrCorner() const
{
  return (
    ON_SubD::VertexTag::Dart == m_vertex_tag
    || ON_SubD::VertexTag::Crease == m_vertex_tag
    || ON_SubD::VertexTag::Corner == m_vertex_tag
    );
}



bool ON_SubDVertex::IsSmoothOrDart() const
{
  return (ON_SubD::VertexTag::Smooth == m_vertex_tag || ON_SubD::VertexTag::Dart == m_vertex_tag);
}

bool ON_SubDVertex::IsSmoothOrCrease() const
{
  return (ON_SubD::VertexTag::Smooth == m_vertex_tag || ON_SubD::VertexTag::Crease == m_vertex_tag);
}

bool ON_SubDVertex::GetBoundaryVertexEdges(
  ON_SubDEdgePtr* eptr0,
  ON_SubDEdgePtr* eptr1
) const
{
  unsigned int vbi[2] = {};
  const bool rc = GetBoundaryVertexEdgeIndices(&vbi[0], &vbi[1]);
  if (rc)
  {
    if (nullptr != eptr0)
      *eptr0 = m_edges[vbi[0]];
    if (nullptr != eptr1)
      *eptr1 = m_edges[vbi[1]];
  }
  else
  {
    if (nullptr != eptr0)
      *eptr0 = ON_SubDEdgePtr::Null;
    if (nullptr != eptr1)
      *eptr1 = ON_SubDEdgePtr::Null;
  }
  return rc;
}

bool ON_SubDVertex::GetBoundaryVertexEdgeIndices(
  unsigned* vei0,
  unsigned* vei1
) const
{
  unsigned int vbi_count = 0;
  unsigned int vbi[2] = {};
  for (unsigned short vei = 0; vei < m_edge_count; vei++)
  {
    const ON_SubDEdge* e = m_edges[vei].Edge();
    if (1 == e->m_face_count)
    {
      if (vbi_count < 2)
        vbi[vbi_count++] = vei;
      else
      {
        vbi_count = 0;
        break;
      }
    }
  }
  if (2 != vbi_count)
    vbi[0] = vbi[1] = ON_UNSET_UINT_INDEX;
  if (nullptr != vei0)
    *vei0 = vbi[0];
  if (nullptr != vei1)
    *vei1 = vbi[1];
  return (2 == vbi_count);
}

const ON_SubDVertexEdgeProperties ON_SubDVertex::EdgeProperties() const
{
  ON_SubDVertexEdgeProperties ep;

  ep.m_edge_count = m_edge_count;
  ep.m_face_count = m_face_count;

  bool bFirstEdge = true;
  for (unsigned short vei = 0; vei < ep.m_edge_count; vei++)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr == e)
    {
      ep.m_null_edge_count++;
      continue;
    }

    if (e->IsCrease())
      ep.m_crease_edge_count++;
    else if (e->IsSmooth())
      ep.m_smooth_edge_count++;
    else
      ep.m_unset_edge_count++;

    const unsigned short edge_face_count = e->m_face_count;
    if (bFirstEdge)
    {
      bFirstEdge = false;
      ep.m_min_edge_face_count = edge_face_count;
      ep.m_max_edge_face_count = edge_face_count;
    }
    else if (edge_face_count < ep.m_min_edge_face_count)
      ep.m_min_edge_face_count = edge_face_count;
    else if (edge_face_count > ep.m_max_edge_face_count)
      ep.m_max_edge_face_count = edge_face_count;

    if (0 == edge_face_count)
      ep.m_wire_edge_count++;
    else if (1 == edge_face_count)
      ep.m_boundary_edge_count++;
    else if (2 == edge_face_count)
      ep.m_interior_edge_count++;
    else
      ep.m_nonmanifold_edge_count++;

  }
  return ep;
}


bool ON_SubDEdge::IsCrease() const
{
  return (ON_SubD::EdgeTag::Crease == m_edge_tag) ? true : false;
}


bool ON_SubDEdge::IsHardCrease() const
{
  return
    (
      ON_SubD::EdgeTag::Crease == m_edge_tag
      && nullptr != m_vertex[0]
      && nullptr != m_vertex[1]
      && m_vertex[0]->IsCreaseOrCorner()
      && m_vertex[1]->IsCreaseOrCorner()
      )
    ? true
    : false;
}

bool ON_SubDEdge::IsDartCrease() const
{
  return
    (ON_SubD::EdgeTag::Crease == m_edge_tag && DartCount() > 0 )
    ? true
    : false;
}

unsigned int ON_SubDEdge::DartCount() const
{
  unsigned int dart_count = 0;
  if (nullptr != m_vertex[0] && ON_SubD::VertexTag::Dart == m_vertex[0]->m_vertex_tag)
    dart_count++;
  if (nullptr != m_vertex[1] && ON_SubD::VertexTag::Dart == m_vertex[1]->m_vertex_tag)
    dart_count++;
  return dart_count;
}

bool ON_SubDEdge::IsSmooth() const
{
  return (ON_SubD::EdgeTag::Smooth == m_edge_tag || ON_SubD::EdgeTag::SmoothX == m_edge_tag) ? true : false;
}

bool ON_SubDEdge::IsSmoothNotX() const
{
  return (ON_SubD::EdgeTag::Smooth == m_edge_tag) ? true : false;
}

bool ON_SubDEdge::IsSmoothX() const
{
  return (ON_SubD::EdgeTag::SmoothX == m_edge_tag) ? true : false;
}

bool ON_SubDVertex::IsSingleSectorVertex() const
{
  const bool bIsCreaseOrCorner = IsCreaseOrCorner();
  if (bIsCreaseOrCorner)
  {
    if (m_face_count < 1 || m_face_count + 1 != m_edge_count)
      return false;
  }
  else if (IsSmoothOrDart())
  {
    if (m_face_count < 2 || m_edge_count!= m_face_count)
      return false;
  }
  else
    return false;


  unsigned short boundary_crease_count = 0;
  unsigned short interior_crease_count = 0;
  unsigned short interior_smooth_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(this->m_edges[vei].m_ptr);
    if (nullptr == e)
      return false;

    if (e->IsSmooth())
    {
      if (2 == e->m_face_count)
      {
        ++interior_smooth_count;
        continue;
      }
    }
    else if (ON_SubD::EdgeTag::Crease == e->m_edge_tag)
    {
      if (2 == e->m_face_count)
      {
        ++interior_crease_count;
        if (ON_SubD::VertexTag::Dart == m_vertex_tag && 1 == interior_crease_count)
          continue;
      }
      else if (1 == e->m_face_count)
      {
        ++boundary_crease_count;
        if (bIsCreaseOrCorner && boundary_crease_count <= 2)
          continue;
      }
    }

    return false;
  }

  if (bIsCreaseOrCorner)
  {
    if (2 == boundary_crease_count && 2+interior_smooth_count == m_edge_count)
      return true;
  }
  else if (ON_SubD::VertexTag::Dart == m_vertex_tag)
  {
    if (1 == interior_crease_count && 1+interior_smooth_count == m_edge_count)
      return true;
  }
  else if (ON_SubD::VertexTag::Smooth == m_vertex_tag)
  {
    if (interior_smooth_count == m_edge_count)
      return true;
  }

  return false;
}

bool ON_SubDVertex::IsManifoldBoundaryVertex() const
{
  return IsCreaseOrCorner() && IsSingleSectorVertex();
}


bool ON_SubDVertex::HasInteriorVertexTopology() const
{
  if (m_edge_count >= 2 && m_edge_count == m_face_count && nullptr != m_edges && nullptr != m_faces)
  {
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == e || 2 != e->m_face_count)
        return false;
    }
    return true;
  }    
  return false;
}

bool ON_SubDVertex::HasBoundaryVertexTopology() const
{
  if (m_edge_count >= 2 && m_edge_count == m_face_count+1 && nullptr != m_edges && nullptr != m_faces)
  {
    unsigned boundary_count = 0;
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == e || 0 == e->m_face_count || e->m_face_count > 2)
        return false;
      if (1 == e->m_face_count)
        ++boundary_count;
    }
    if (2 == boundary_count)
      return true;
  }    
  return false;
}


bool ON_SubDVertexEdgeProperties::HasInteriorVertexTopology() const
{
  return 
    m_null_edge_count == 0
    && m_edge_count == m_face_count
    && m_boundary_edge_count == 0
    && m_interior_edge_count >= 2
    && m_nonmanifold_edge_count == 0
    ;
}

bool ON_SubDVertexEdgeProperties::HasBoundaryVertexTopology() const
{
  return
    m_null_edge_count == 0
    && m_edge_count == m_face_count+1
    && m_boundary_edge_count == 2
    && m_nonmanifold_edge_count == 0
    ;
}

bool ON_SubDVertexEdgeProperties::HasManifoldVertexTopology() const
{
  return HasInteriorVertexTopology() || HasBoundaryVertexTopology();
}

bool ON_SubDVertexEdgeProperties::HasNonmanifoldVertexTopology() const
{
  return  
    (m_null_edge_count == 0)
    && (m_wire_edge_count > 0 || m_nonmanifold_edge_count > 0)
    ;
}

bool ON_SubDVertex::IsStandard() const
{
  if (nullptr == m_edges)
    return false;

  if (nullptr == m_faces)
    return false;

  const unsigned int edge_count = m_edge_count;
  if (!ON_SubD::IsValidSectorEdgeCount(m_vertex_tag,edge_count))
    return false;

  const unsigned int face_count = m_face_count;
  if (face_count != ON_SubDSectorType::SectorFaceCountFromEdgeCount(m_vertex_tag, edge_count))
    return false;

  const unsigned short f_edge_count = 4;

  unsigned int crease_edge_face_count = ON_UNSET_UINT_INDEX;
  bool bTaggedVertex = false;
  switch (m_vertex_tag)
  {
  case ON_SubD::VertexTag::Unset:
    return false;
    break;

  case ON_SubD::VertexTag::Smooth:
    if (edge_count != face_count)
      return false;
    break;

  case ON_SubD::VertexTag::Crease:
    if (edge_count != face_count+1)
      return false;
    crease_edge_face_count = 1;
    bTaggedVertex = true;
    break;

  case ON_SubD::VertexTag::Corner:
    if (edge_count != face_count+1)
      return false;
    crease_edge_face_count = 1;
    bTaggedVertex = true;
    break;

  case ON_SubD::VertexTag::Dart:
    if (edge_count != face_count)
      return false;
    crease_edge_face_count = 2;
    bTaggedVertex = true;
    break;

  default:
    return false;
    break;
  }
  

  for (unsigned int vfi = 0; vfi < face_count; vfi++)
  {
    const ON_SubDFace* f = m_faces[vfi];
    if (nullptr == f)
      return false;
    if (f_edge_count != f->m_edge_count)
      return false;
  }

  unsigned int creased_edge_count = 0;
  double sector_weight = 0.0;
  for (unsigned int vei = 0; vei < edge_count; vei++)
  {
    const ON_SubDEdge* e = m_edges[vei].Edge();
    if (nullptr == e)
      return false;

    unsigned int evi;
    if (this == e->m_vertex[0])
      evi = 0;
    else if (this == e->m_vertex[1])
      evi = 1;
    else
      return false;

    const ON_SubDVertex* other_vertex = e->m_vertex[1-evi];

    if (nullptr == other_vertex)
      return false;

    if (ON_SubD::EdgeTag::Smooth == e->m_edge_tag)
    {
      if (2 != e->m_face_count)
        return false;

      if (bTaggedVertex && 0 == vei)
      {
        sector_weight = e->m_sector_coefficient[evi];
        if (!(0.0 <= sector_weight && sector_weight <= 1.0))
          return false;
      }
      
      if (!(sector_weight == e->m_sector_coefficient[evi]))
        return false;

      if (ON_SubD::VertexTag::Smooth == other_vertex->m_vertex_tag)
      {
        if ( !(0.0 == e->m_sector_coefficient[1-evi]) )
          return false;
      }
      else
      {
        if ( bTaggedVertex )
          return false;
        if ( !(0.5 == e->m_sector_coefficient[1-evi]) )
          return false;
      }
    }
    else if (ON_SubD::EdgeTag::Crease == e->m_edge_tag)
    {
      if (crease_edge_face_count != e->m_face_count)
        return false;
      creased_edge_count++;
      if (creased_edge_count > 2)
        return false;
      if (false == other_vertex->IsDartOrCreaseOrCorner())
        return false;
    }
    else
    {
      return false;
    }
  }

  switch (creased_edge_count)
  {
  case 0:
    if (false == IsSmooth())
      return false;
    break;
  case 1:
    if (false == IsDart())
      return false;
    break;
  case 2:
    if (false == IsCreaseOrCorner())
      return false;
    break;
  default:
    return false;
  }

  // The standard subdivison matrix will correctly calculate
  // the subdivision location for this vertex.
  return true;
}


unsigned int ON_SubDEdge::EdgeAttributes() const
{
  unsigned int edge_topology_attributes = 0U;

  if (nullptr == m_vertex[0] || nullptr == m_vertex[1])
  {
    edge_topology_attributes |= ON_ComponentAttributes::Damaged;
  }
  else
  {
    const double* P[2] = { m_vertex[0]->m_P, m_vertex[1]->m_P };
    if (
      fabs(P[0][0]) < ON_UNSET_POSITIVE_VALUE
      && fabs(P[0][1]) < ON_UNSET_POSITIVE_VALUE
      && fabs(P[0][2]) < ON_UNSET_POSITIVE_VALUE
      && fabs(P[1][0]) < ON_UNSET_POSITIVE_VALUE
      && fabs(P[1][1]) < ON_UNSET_POSITIVE_VALUE
      && fabs(P[1][2]) < ON_UNSET_POSITIVE_VALUE
      )
    {
      if (P[0][0] == P[1][0] && P[0][1] == P[1][1] && P[0][2] == P[1][2])
        edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::ZeroLength;
      else
        edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::NonzeroLength;
    }
    if (m_vertex[0] != m_vertex[1])
      edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::Open;
    else
      edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::Closed;
  }
  
  const ON_SubDFace* f[2] = { ON_SUBD_FACE_POINTER(m_face2[0].m_ptr),ON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
  switch (m_face_count)
  {
  case 0:
    edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::Wire;
    break;

  case 1:
    edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::Boundary;
    if ( nullptr == f[0])
      edge_topology_attributes |= ON_ComponentAttributes::Damaged;
    break;

  case 2:
    edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::Interior;

    if (IsSmooth())
      edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::InteriorSmooth;
    else if (IsCrease())
      edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::InteriorCrease;

    if (nullptr != f[0] && nullptr != f[1])
    {
      if (ON_SUBD_FACE_DIRECTION(m_face2[0].m_ptr) == ON_SUBD_FACE_DIRECTION(m_face2[1].m_ptr))
        edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::InteriorNotOriented;
      else
        edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::InteriorOriented;
      if (f[0] != f[1])
      {
        edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::InteriorTwoFaced;
      }
      else
      {
        const unsigned int fecount = f[0]->EdgeCount();
        const unsigned int fei0 = f[0]->EdgeArrayIndex(this);
        if (fecount > 2 && fei0 < fecount)
        {
          for (unsigned int fei1 = fei0 + 1; fei1 < fecount; ++fei1)
          {
            if (this == f[0]->Edge(fei1))
            {
              if (f[0]->EdgeDirection(fei0) != f[0]->EdgeDirection(fei1))
              {
                if ( fei1+1 == fei0 || (0 == fei0 && fei1+1 == fecount))
                  edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::InteriorSlit;
                else
                  edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::InteriorSeam;
              }
              break;
            }
          }
        }
      }
    }
    else
    {
      edge_topology_attributes |= ON_ComponentAttributes::Damaged;
    }
    break;

  default:
    edge_topology_attributes |= ON_ComponentAttributes::EdgeAttributes::Nonmanifold;
    if ( nullptr == f[0] || nullptr == f[1] || nullptr == m_facex )
      edge_topology_attributes |= ON_ComponentAttributes::Damaged;
    break;
  }

  return edge_topology_attributes;
}

static int Compare_ON__UINT_PTR(const void* a, const void* b)
{
  ON__UINT_PTR ai = *((const unsigned int*)a);
  ON__UINT_PTR bi = *((const unsigned int*)b);
  if (ai < bi)
    return -1;
  if (ai > bi)
    return 1;
  return 0;
}

static int ComparePointerArrays(
  size_t count,
  const ON__UINT_PTR* a,
  const ON__UINT_PTR* b
  )
{
  if (count <= 0)
    return 0;
  if (nullptr == a)
    return ((nullptr == b) ? 0 : -1);
  if (nullptr == b)
    return 1;
  if (1 == count)
    return Compare_ON__UINT_PTR(a, b);

  unsigned int stack_buffer[128];
  unsigned int* adex
    = (2 * count <= sizeof(stack_buffer) / sizeof(stack_buffer[0]))
    ? stack_buffer
    : new (std::nothrow) unsigned int[2 * count];  
  if (nullptr == adex)
    return 0;
  
  unsigned int* bdex = adex + count;
  ON_Sort(ON::sort_algorithm::quick_sort, adex, a, count, sizeof(a[0]), Compare_ON__UINT_PTR);
  ON_Sort(ON::sort_algorithm::quick_sort, bdex, b, count, sizeof(b[0]), Compare_ON__UINT_PTR);
  
  int rc = 0;
  for (unsigned int i = 0; 0 == rc && i < count; i++)
  {
    rc = Compare_ON__UINT_PTR(a + adex[i], b + bdex[i]);
  }

  if (adex != stack_buffer)
    delete[] adex;

  return rc;
}

int ON_SubDVertex::CompareUnorderedEdges(
  const ON_SubDVertex* a,
  const ON_SubDVertex* b
  )
{
  if (nullptr == a)
    return ((nullptr == b) ? 0 : -1);
  if (nullptr == b)
    return 1;

  if (a->m_edge_count < b->m_edge_count)
    return -1;
  if (a->m_edge_count > b->m_edge_count)
    return 1;
  return ComparePointerArrays(a->m_edge_count, (const ON__UINT_PTR*)a->m_edges, (const ON__UINT_PTR*)b->m_edges);
}

int ON_SubDVertex::CompareUnorderedFaces(
  const ON_SubDVertex* a,
  const ON_SubDVertex* b
  )
{
  if (nullptr == a)
    return ((nullptr == b) ? 0 : -1);
  if (nullptr == b)
    return 1;

  if (a->m_face_count < b->m_face_count)
    return -1;
  if (a->m_face_count > b->m_face_count)
    return 1;
  return ComparePointerArrays(a->m_face_count, (const ON__UINT_PTR*)a->m_faces, (const ON__UINT_PTR*)b->m_faces);
}

int ON_SubDVertex::CompareUnorderedEdgesAndFaces(
  const ON_SubDVertex* a,
  const ON_SubDVertex* b
  )
{
  int rc = CompareUnorderedEdges(a, b);
  if (0 == rc)
    rc = CompareUnorderedFaces(a, b);
  return rc;
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdge
//

void ON_SubDComponentBase::CopyBaseFrom(
  const ON_SubDComponentBase* src
  )
{
  if ( nullptr == src )
    src = &ON_SubDComponentBase::Unset;

  *this = *src;
  m_subd_point1 = nullptr;
  Internal_ClearSurfacePointFlag();
}

void ON_SubDEdge::CopyFrom(
  const ON_SubDEdge* src,
  bool bReverseEdge,
  bool bCopyVertexArray,
  bool bCopyFaceArray
  )
{
  if (nullptr == src)
    src = &ON_SubDEdge::Empty;

  CopyBaseFrom(src);

  m_next_edge = nullptr;
  
  m_edge_tag = src->m_edge_tag;

  unsigned int end0 = bReverseEdge ? 1 : 0;

  if (bCopyVertexArray)
  {
    m_vertex[0] = src->m_vertex[end0];
    m_vertex[1] = src->m_vertex[1 - end0];
  }

  m_sector_coefficient[0] = src->m_sector_coefficient[end0];
  m_sector_coefficient[1] = src->m_sector_coefficient[1 - end0];

  if (bCopyFaceArray)
  {
    if (src->m_face_count > 0 && (src->m_face_count <= 2 || (nullptr != src->m_facex && nullptr != m_facex)))
    {
      m_face2[0] = src->m_face2[0];
      m_face2[1] = src->m_face2[1];
      unsigned int face_count = src->m_face_count;
      if (face_count > 2)
      {
        face_count -= 2;
        for (unsigned int efi = 0; efi < face_count; efi++)
          m_facex[efi] = src->m_facex[efi];
      }
      m_face_count = src->m_face_count;
    }
    else
      m_face_count = 0;
  }
}

unsigned int ON_SubDEdge::TaggedEndIndex() const
{
  unsigned int tagged_end_index = 3;
  for (unsigned int evi = 0; evi < 2; evi++)
  {
    const ON_SubDVertex* v = m_vertex[evi];
    if (nullptr == v || false == v->IsDartOrCreaseOrCorner())
      continue;
    tagged_end_index = (3 == tagged_end_index) ? evi : 2;
  }
  return tagged_end_index;
}

const ON_SubDFacePtr ON_SubDEdge::FacePtr(
  unsigned int i
  ) const
{
  return (i < 2) ? m_face2[i] : ((i < m_face_count) ? m_facex[i - 2] : ON_SubDFacePtr::Null);
  //return (i < m_face_count) ? ((i < 2) ? m_face2[i] : m_facex[i - 2]) : ON_SubDFacePtr::Null;
}

unsigned int ON_SubDEdge::FaceCount() const
{
  return m_face_count;
}

bool ON_SubDEdge::IsInteriorEdge() const
{
  for (;;)
  {
    if (2 != m_face_count)
      break;
    if (ON_SUBD_FACE_DIRECTION(m_face2[0].m_ptr) == ON_SUBD_FACE_DIRECTION(m_face2[1].m_ptr))
      break;
    if (nullptr == ON_SUBD_FACE_POINTER(m_face2[0].m_ptr))
      break;
    if (nullptr == ON_SUBD_FACE_POINTER(m_face2[1].m_ptr))
      break;
  }
  return false;
}

const class ON_SubDFace* ON_SubDEdge::Face(
  unsigned int i
  ) const
{
  return (i < 2) ? ON_SUBD_FACE_POINTER(m_face2[i].m_ptr) : ((i < m_face_count) ? ON_SUBD_FACE_POINTER(m_facex[i - 2].m_ptr) : nullptr);
}

ON__UINT_PTR ON_SubDEdge::FaceDirection(
  unsigned int i
  ) const
{
  return (i < 2) ? ON_SUBD_FACE_DIRECTION(m_face2[i].m_ptr) : ((i < m_face_count) ? ON_SUBD_FACE_DIRECTION(m_facex[i - 2].m_ptr) : 0);
}

const ON_SubDFacePtr ON_SubDEdge::FacePtrFromFace(
  const class ON_SubDFace* f
) const
{
  if (nullptr != f)
  {
    const ON_SubDFacePtr* fptr = m_face2;
    const unsigned int edge_face_count = m_face_count;
    for (unsigned int efi = 0; efi < edge_face_count; efi++, fptr++)
    {
      if (2 == efi)
      {
        fptr = m_facex;
        if (nullptr == fptr)
          break;
      }
      if (fptr->Face() == f)
        return *fptr;
    }
  }
  return ON_SubDFacePtr::Null;
}


unsigned int ON_SubDEdge::FaceArrayIndex(
  const ON_SubDFace* f
  ) const
{
  if (nullptr == f)
    return ON_UNSET_UINT_INDEX;
  const unsigned int face_count = m_face_count;
  if (face_count > 0)
  {
    if (f == ON_SUBD_FACE_POINTER(m_face2[0].m_ptr))
      return 0;
    if (face_count > 1)
    {
      if (f == ON_SUBD_FACE_POINTER(m_face2[1].m_ptr))
        return 1;
      if (face_count > 2 && nullptr != m_facex)
      {
        const ON_SubDFacePtr* fptr = m_facex - 2;
        for (unsigned int efi = 2; efi < face_count; efi++)
        {
          if (f == ON_SUBD_FACE_POINTER(fptr[efi].m_ptr))
            return efi;
        }
      }
    }
  }
  return ON_UNSET_UINT_INDEX;
}

unsigned int ON_SubDEdge::ReplaceFaceInArray(const ON_SubDFace * old_face, const ON_SubDFace * new_face)
{
  unsigned efi = (nullptr != old_face && old_face != new_face) ? FaceArrayIndex(old_face) : ON_UNSET_UINT_INDEX;
  if (ON_UNSET_UINT_INDEX == efi)
    return ON_UNSET_UINT_INDEX;
  ON_SubDFacePtr* fptr = (efi < 2) ? &m_face2[efi] : &m_facex[efi - 2];
  if (nullptr != new_face)
  {
    *fptr = ON_SubDFacePtr::Create(new_face, fptr->FaceDirection());
  }
  else
  {    
    unsigned efi1 = efi + 1;
    ON_SubDFacePtr* fptr1 = (efi1 < 2) ? &m_face2[efi1] : &m_facex[efi1 - 2];
    for (const unsigned c = (unsigned)(m_face_count--); efi1 < c; ++efi, ++efi1)
    {
      if (2 == efi)
        fptr = m_facex;
      else if (2 == efi1)
        fptr1 = m_facex;
      *fptr++ = *fptr1++;
    }
  }
  return efi;
}



unsigned int ON_SubDEdge::VertexArrayIndex(const ON_SubDVertex * v) const
{
  if (nullptr == v || m_vertex[0] == m_vertex[1])
    return ON_UNSET_UINT_INDEX;
  if (v == m_vertex[0])
    return 0;
  if (v == m_vertex[1])
    return 1;
  return ON_UNSET_UINT_INDEX;
}


const ON_SubDFace* ON_SubDEdge::NeighborFace(
  const ON_SubDFace* face,
    bool bStopAtCrease
  ) const
{
  if ( nullptr == face || 2 != m_face_count )
    return nullptr;
  // Do not stop at x tags
  if (bStopAtCrease && ON_SubD::EdgeTag::Crease == m_edge_tag)
    return nullptr;
  const ON_SubDFace* f[2] = { ON_SUBD_FACE_POINTER(m_face2[0].m_ptr), ON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
  if (nullptr == f[0] || nullptr == f[1] )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (face == f[0])
  {
    if (face == f[1] )
      return ON_SUBD_RETURN_ERROR(nullptr);
    return f[1];
  }
  if (face == f[1])
  {
    return f[0];
  }
  return ON_SUBD_RETURN_ERROR(nullptr);
}

const ON_SubDFacePtr ON_SubDEdge::NeighborFacePtr(
  const ON_SubDFace* face,
    bool bStopAtCrease
  ) const
{
  if ( nullptr == face || 2 != m_face_count )
    return ON_SubDFacePtr::Null;
  // Do not stop at x tags
  if (bStopAtCrease && ON_SubD::EdgeTag::Crease == m_edge_tag)
    return ON_SubDFacePtr::Null;
  const ON_SubDFace* f[2] = { ON_SUBD_FACE_POINTER(m_face2[0].m_ptr), ON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
  if (nullptr == f[0] || nullptr == f[1] )
    return ON_SUBD_RETURN_ERROR(ON_SubDFacePtr::Null);
  if (face == f[0])
  {
    if (face == f[1] )
      return ON_SUBD_RETURN_ERROR(ON_SubDFacePtr::Null);
    return m_face2[1];
  }
  if (face == f[1])
  {
    return m_face2[0];
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDFacePtr::Null);
}

const ON_SubDEdgePtr ON_SubDEdge::AdjacentEdgePtr(
  unsigned int edge_vertex_index,
  unsigned int i
) const
{
  if ( edge_vertex_index >= 2 )
    return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);
  const ON_SubDFacePtr fptr = FacePtr(i);
  const ON_SubDFace* f = ON_SUBD_FACE_POINTER(fptr.m_ptr);
  if (nullptr == f)
    return ON_SubDEdgePtr::Null;
  const unsigned int fe_count = f->m_edge_count;
  if ( fe_count < 3 || fe_count > ON_SubDFace::MaximumEdgeCount)
    return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);
  const unsigned int fei = f->EdgeArrayIndex(this);
  if( fei >= fe_count)
    return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);
  const ON_SubDEdgePtr eptr = f->EdgePtr(fei);
  if ( this != ON_SUBD_EDGE_POINTER(eptr.m_ptr))
    return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);
  const ON__UINT_PTR dir = ON_SUBD_FACE_DIRECTION(fptr.m_ptr);
  if (dir != ON_SUBD_EDGE_DIRECTION(eptr.m_ptr))
    return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);
  const unsigned int fei1
    = (dir == ((ON__UINT_PTR)edge_vertex_index))
    ? ((fei + fe_count - 1) % fe_count)
    : ((fei + 1) % fe_count)
    ;
  return f->EdgePtr(fei1);
}

const ON_SubDEdge* ON_SubDEdge::AdjacentEdge(
  unsigned int edge_vertex_index,
  unsigned int i
  ) const
{
  return ON_SUBD_EDGE_POINTER(AdjacentEdgePtr(edge_vertex_index, i).m_ptr);
}

const class ON_SubDVertex* ON_SubDEdge::Vertex(
  unsigned int i
  ) const
{
  return (i <= 1) ? m_vertex[i] : nullptr;
}


const ON_SubDVertex* ON_SubDEdge::OtherEndVertex(
  const ON_SubDVertex* vertex
  ) const
{
  if (nullptr != vertex)
  {
    if (m_vertex[0] == vertex)
    {
      if (m_vertex[1] != vertex )
        return m_vertex[1];
    }
    else if (m_vertex[1] == vertex )
      return m_vertex[0];
  }
  return nullptr;
}

const ON_SubDEdgePtr ON_SubDEdge::FromVertices(
  const ON_SubDVertex* vertex0,
  const ON_SubDVertex* vertex1
)
{
  if (nullptr != vertex0 && nullptr != vertex1 && vertex0 != vertex1 && nullptr != vertex0->m_edges)
  {
    for (unsigned short vei = 0; vei < vertex0->m_edge_count; vei++)
    {
      const ON_SubDEdgePtr eptr = vertex0->m_edges[vei];
      if (vertex1 == eptr.RelativeVertex(1) && vertex0 == eptr.RelativeVertex(0))
        return eptr;
    }
  }
  return ON_SubDEdgePtr::Null;
}

const ON_SubDEdge* ON_SubDEdge::FromVertices(
  const ON_SubDVertex* vertex0,
  const ON_SubDVertex* vertex1,
  bool bIgnoreOrientation
)
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(ON_SubDEdge::FromVertices(vertex0, vertex1).m_ptr);
  if ( nullptr != e && false == bIgnoreOrientation && vertex0 != e->m_vertex[0] )
    e = nullptr;
  return e;
}

const ON_3dPoint ON_SubDEdge::ControlNetPoint( unsigned int i) const
{
  if (i >= 2 || nullptr == m_vertex[i])
    return ON_3dPoint::NanPoint;
  return (ON_3dPoint(m_vertex[i]->m_P));
}

const ON_Line ON_SubDEdge::ControlNetLine() const
{
  return ON_Line(ControlNetPoint(0), ControlNetPoint(1));
}

const ON_3dVector ON_SubDEdge::ControlNetDirection() const
{
  if (nullptr == m_vertex[0] || nullptr == m_vertex[1])
    return ON_3dVector::NanVector;
  const ON_3dPoint P[2] = { ON_3dPoint(m_vertex[0]->m_P) ,ON_3dPoint(m_vertex[1]->m_P) };
  return (P[0].IsValid() && P[1].IsValid()) ? (P[1] - P[0]) : ON_3dVector::NanVector;
}

const ON_3dVector ON_SubDEdge::ControlNetDirectionFrom(
  const ON_SubDVertex* v
) const
{
  if (nullptr != v)
  {
    if (v == m_vertex[0] && nullptr != m_vertex[1])
      return ControlNetDirection();
    if (v == m_vertex[1] && nullptr != m_vertex[0])
      return -ControlNetDirection();
  }
  return ON_3dVector::NanVector;
}


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFace
//



//bool ON_SubDFace::IsOrdinary(
//  ON_SubD::SubDType subdivision_type,
//  bool bTestFaces
//  ) const
//{
//  unsigned int ordinary_face_edge_count = 0;
//  if (ON_SubD::SubDType::QuadCatmullClark == subdivision_type)
//    ordinary_face_edge_count = 4;
//  else if (ON_SubD::SubDType::TriLoopWarren == subdivision_type)
//    ordinary_face_edge_count = 3;
//  else
//    return false;
//
//  if (ordinary_face_edge_count != m_edge_count)
//    return false;
//
//  for (unsigned int fei = 0; fei < ordinary_face_edge_count; fei++)
//  {
//    ON__UINT_PTR eptr = m_edge4[fei].m_ptr;
//    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr);
//    if (nullptr == e || 2 != e->m_face_count || ON_SubD::EdgeTag::Smooth != e->m_edge_tag)
//      return false;
//    ON__UINT_PTR edir = ON_SUBD_EDGE_DIRECTION(eptr);
//    const ON_SubDVertex* v = e->m_vertex[edir];
//    if (nullptr == v || false == v->IsOrdinary(subdivision_type,ON_SubD::VertexTag::Unset,bTestFaces))
//      return false;
//  }
//  return true;
//}

void ON_SubDFace::CopyFrom(
  const ON_SubDFace* src,
  bool bCopyEdgeArray
  )
{
  if (nullptr == src)
    src = &ON_SubDFace::Empty;

  CopyBaseFrom(src);

  m_next_face = nullptr;

  m_zero_face_id = src->m_zero_face_id;
  m_parent_face_id = src->m_parent_face_id;

  if (bCopyEdgeArray)
  {
    if (src->m_edge_count > 0 && (src->m_edge_count <= 4 || (nullptr != src->m_edgex && nullptr != m_edgex)))
    {
      m_edge4[0] = src->m_edge4[0];
      m_edge4[1] = src->m_edge4[1];
      m_edge4[2] = src->m_edge4[2];
      m_edge4[3] = src->m_edge4[3];
      unsigned int edge_count = src->m_edge_count;
      if (edge_count > 4)
      {
        edge_count -= 4;
        for (unsigned int fei = 0; fei < edge_count; fei++)
          m_edgex[fei] = src->m_edgex[fei];
      }
      m_edge_count = src->m_edge_count;
    }
    else
      m_edge_count = 0;
  }

  // RH-56133 need to copy texture coordinate information that was recently added.
  m_texture_coordinate_origin[0] = src->m_texture_coordinate_origin[0];
  m_texture_coordinate_origin[1] = src->m_texture_coordinate_origin[1];
  m_texture_coordinate_delta[0] = src->m_texture_coordinate_delta[0];
  m_texture_coordinate_delta[1] = src->m_texture_coordinate_delta[1];
  m_texture_coordinate_bits = src->m_texture_coordinate_bits;
}

const ON_SubDEdgePtr ON_SubDFace::EdgePtr(
  unsigned int i
  ) const
{
  return (i < 4) ? m_edge4[i] : ((i < m_edge_count) ? m_edgex[i-4] : ON_SubDEdgePtr::Null);
}

unsigned int ON_SubDFace::EdgeCount() const
{
  return m_edge_count;
}

unsigned int ON_SubDFace::MarkedEdgeCount() const
{
  unsigned int marked_edge_count = 0;
  const ON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != e && e->m_status.RuntimeMark())
      ++marked_edge_count;
  }
  return marked_edge_count;
}


unsigned int ON_SubDFace::SetEdgeMarks(bool bMark) const
{
  bMark = bMark ? true : false; // so exact compare can be used
  unsigned int changed_mark_count = 0;
  const ON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != e && bMark != e->m_status.RuntimeMark())
    {
      e->m_status.SetRuntimeMark(bMark);
      ++changed_mark_count;
    }
  }
  return changed_mark_count;
}

unsigned int ON_SubDFace::GetEdgeArray(
  ON_SimpleArray< ON_SubDEdgePtr >& face_edge_array
) const
{
  face_edge_array.SetCount(0);
  const unsigned edge_count = m_edge_count;
  face_edge_array.Reserve(edge_count);
  face_edge_array.Append(edge_count <= 4 ? edge_count : 4U, m_edge4);
  if (edge_count > 4)
  {
    if ( nullptr != m_edgex )
     face_edge_array.Append(edge_count-4U, m_edgex);
    else
    {
      for (unsigned fei = 4; fei < edge_count; ++fei)
        face_edge_array.Append(ON_SubDEdgePtr::Null);
    }
  }
  return edge_count;
}

unsigned int ON_SubDFace::SetVertexMarks(bool bMark) const
{
  bMark = bMark ? true : false; // so exact compare can be used
  unsigned int changed_mark_count = 0;
  const ON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const ON_SubDVertex* v = eptr->RelativeVertex(0);
    if (nullptr != v && bMark != v->m_status.RuntimeMark())
    {
      v->m_status.SetRuntimeMark(bMark);
      ++changed_mark_count;
    }
  }
  return changed_mark_count;
}

unsigned int ON_SubDFace::MarkedVertexCount() const
{
  unsigned int marked_vertex_count = 0;
  const ON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const ON_SubDVertex* v = eptr->RelativeVertex(0);
    if (nullptr != v && v->m_status.RuntimeMark())
      ++marked_vertex_count;
  }
  return marked_vertex_count;
}

const class ON_SubDVertex* ON_SubDFace::Vertex(
  unsigned int i
  ) const
{
  ON_SubDEdge* e;
  const ON__UINT_PTR edge_ptr = (i < 4) ? m_edge4[i].m_ptr : ((i < m_edge_count) ? m_edgex[i - 4].m_ptr : 0);
  return (nullptr != (e = ON_SUBD_EDGE_POINTER(edge_ptr))) ? e->m_vertex[ON_SUBD_EDGE_DIRECTION(edge_ptr)] : nullptr;
}

const ON_3dPoint ON_SubDFace::ControlNetPoint(unsigned int i) const
{
  const ON_SubDEdge* e;
  const ON__UINT_PTR edge_ptr = (i < 4) ? m_edge4[i].m_ptr : ((i < m_edge_count) ? m_edgex[i - 4].m_ptr : 0);
  const ON_SubDVertex* v = (nullptr != (e = ON_SUBD_EDGE_POINTER(edge_ptr))) ? e->m_vertex[ON_SUBD_EDGE_DIRECTION(edge_ptr)] : nullptr;
  return (nullptr != v) ? ON_3dPoint(v->m_P) : ON_3dPoint::NanPoint;
}

const ON_SubDVertex* ON_SubDFace::QuadOppositeVertex(
  const ON_SubDVertex* vertex
  ) const
{
  if ( nullptr == vertex )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( 4 != m_edge_count)
    return nullptr; // not an error
  
  ON__UINT_PTR ptr0 = m_edge4[0].m_ptr;
  const ON_SubDEdge* e0 = ON_SUBD_EDGE_POINTER(ptr0);
  if ( nullptr == e0 )
    return ON_SUBD_RETURN_ERROR(nullptr);
  ptr0 = ON_SUBD_EDGE_DIRECTION(ptr0);
  
  ON__UINT_PTR ptr2 = m_edge4[2].m_ptr;
  const ON_SubDEdge* e2 = ON_SUBD_EDGE_POINTER(ptr2);
  if ( nullptr == e2 )
    return ON_SUBD_RETURN_ERROR(nullptr);
  ptr2 = ON_SUBD_EDGE_DIRECTION(ptr2);

  if (vertex == e0->m_vertex[ptr0])
    return e2->m_vertex[ptr2];

  if (vertex == e0->m_vertex[1-ptr0])
    return e2->m_vertex[1-ptr2];

  if (vertex == e2->m_vertex[ptr2])
    return e0->m_vertex[ptr0];

  if (vertex == e2->m_vertex[1-ptr2])
    return e0->m_vertex[1-ptr0];

  return ON_SUBD_RETURN_ERROR(nullptr);
}

const ON_SubDEdge* ON_SubDFace::QuadOppositeEdge(
  const ON_SubDEdge* edge
  ) const
{
  if ( nullptr == edge )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( 4 != m_edge_count)
    return nullptr; // not an error
  
  for (unsigned int fei = 0; fei < 4; fei++)
  {
    const ON_SubDEdge* e0 = ON_SUBD_EDGE_POINTER(m_edge4[fei].m_ptr);
    if (nullptr == e0)
      return ON_SUBD_RETURN_ERROR(nullptr);
    if (e0 == edge)
    {
      e0 = ON_SUBD_EDGE_POINTER(m_edge4[(fei + 2) % 4].m_ptr);
      if (nullptr == e0)
        return ON_SUBD_RETURN_ERROR(nullptr);
      return e0;
    }
  }

  return ON_SUBD_RETURN_ERROR(nullptr);
}


const class ON_SubDEdge* ON_SubDFace::Edge(
  unsigned int i
  ) const
{
  return (i < 4) ? ON_SUBD_EDGE_POINTER(m_edge4[i].m_ptr) : ((i < m_edge_count) ? ON_SUBD_EDGE_POINTER(m_edgex[i - 4].m_ptr) : nullptr);
}

ON__UINT_PTR ON_SubDFace::EdgeDirection(
  unsigned int i
  ) const
{
  return (i < 4) ? ON_SUBD_EDGE_DIRECTION(m_edge4[i].m_ptr) : ((i < m_edge_count) ? ON_SUBD_EDGE_DIRECTION(m_edgex[i - 4].m_ptr) : 0);
}


const ON_SubDEdgePtr ON_SubDFace::EdgePtrFromEdge(
  const class ON_SubDEdge* e
) const
{
  if (nullptr != e)
  {
    const ON_SubDEdgePtr* eptr = m_edge4;
    const unsigned int face_edge_count = m_edge_count;
    for (unsigned int fei = 0; fei < face_edge_count; fei++, eptr++)
    {
      if (4 == fei)
      {
        eptr = m_edgex;
        if (nullptr == eptr)
          break;
      }
      if (e == ON_SUBD_EDGE_POINTER(eptr->m_ptr))
        return *eptr;
    }
  }

  return ON_SubDEdgePtr::Null;
}


unsigned int ON_SubDFace::EdgeArrayIndex(
  const ON_SubDEdge* e
  ) const
{
  if (nullptr != e)
  {
    const ON_SubDEdgePtr* eptr = m_edge4;
    const unsigned int face_edge_count = m_edge_count;
    for (unsigned int fei = 0; fei < face_edge_count; fei++, eptr++)
    {
      if (4 == fei)
      {
        eptr = m_edgex;
        if (nullptr == eptr)
          break;
      }
      if (e == ON_SUBD_EDGE_POINTER(eptr->m_ptr))
        return fei;
    }
  }

  return ON_UNSET_UINT_INDEX;
}


const ON_SubDEdge* ON_SubDFace::PrevEdge(
  const ON_SubDEdge* edge
  ) const
{
  unsigned int edge_index = EdgeArrayIndex(edge);
  if (ON_UNSET_UINT_INDEX == edge_index)
    return nullptr;
  const unsigned int edge_count = m_edge_count;
  edge_index = (edge_index + (edge_count - 1)) % edge_count;
  return Edge(edge_index);
}

const ON_SubDEdge* ON_SubDFace::NextEdge(
  const ON_SubDEdge* edge
  ) const
{
  unsigned int edge_index = EdgeArrayIndex(edge);
  if (ON_UNSET_UINT_INDEX == edge_index)
    return nullptr;
  edge_index = (edge_index + 1) % ((unsigned int)m_edge_count);
  return Edge(edge_index);
}

unsigned int ON_SubDFace::PrevEdgeArrayIndex(
  unsigned int edge_array_index
  ) const
{
  const unsigned int edge_count = m_edge_count;
  return (edge_array_index < edge_count) ? ((edge_array_index + edge_count - 1) % edge_count) : ON_UNSET_UINT_INDEX;
}

unsigned int ON_SubDFace::NextEdgeArrayIndex(
  unsigned int edge_array_index
  ) const
{
  const unsigned int edge_count = m_edge_count;
  return (edge_array_index < edge_count) ? ((edge_array_index +  1) % edge_count) : ON_UNSET_UINT_INDEX;
}

bool ON_SubDVertex::RemoveEdgeFromArray(const ON_SubDEdge * e)
{
  if (nullptr == e || 0 == m_edge_count || nullptr == m_edges)
    return ON_SUBD_RETURN_ERROR(false);
  unsigned short new_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const ON_SubDEdgePtr eptr = m_edges[vei];
    if (e == ON_SUBD_EDGE_POINTER(eptr.m_ptr))
      continue;
    m_edges[new_count++] = eptr;
  }
  if (new_count == m_edge_count)
    return false;
  m_edge_count = new_count;
  return true;
}

bool ON_SubDVertex::RemoveFaceFromArray(const ON_SubDFace * f)
{
  if (nullptr == f || 0 == m_face_count || nullptr == m_faces)
    return ON_SUBD_RETURN_ERROR(false);
  unsigned short new_count = 0;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const ON_SubDFace* vf = m_faces[vfi];
    if (f == vf)
      continue;
    m_faces[new_count++] = vf;
  }
  if (new_count == m_face_count)
    return false;
  m_face_count = new_count;
  return true;
}

bool ON_SubDEdge::RemoveFaceFromArray(
  const ON_SubDFace* f
  )
{
  unsigned int i;
  if (nullptr == f)
    return false;

  if (m_face_count <= 2)
  {
    for (i = 0; i < m_face_count; i++)
    {
      if (f == ON_SUBD_FACE_POINTER(m_face2[i].m_ptr))
      {
        for (i++; i < m_face_count; i++)
          m_face2[i - 1] = m_face2[i];
        m_face_count--;
        return true;
      }
    }
  }
  else
  {
    for (i = 0; i < 2; i++)
    {
      if (f == ON_SUBD_FACE_POINTER(m_face2[i].m_ptr))
      {
        for (i++; i < 2; i++)
          m_face2[i - 1] = m_face2[i];
        m_face2[1] = m_facex[0];
        for (i = 3; i < m_face_count; i++)
          m_facex[i - 3] = m_facex[i - 2];
        m_face_count--;
        return true;
      }
    }
    for (i = 2; i < m_face_count; i++)
    {
      if (f == ON_SUBD_FACE_POINTER(m_facex[i - 4].m_ptr))
      {
        for (i++; i < m_face_count; i++)
          m_facex[i - 3] = m_facex[i - 2];
        m_face_count--;
        return true;
      }
    }
  }

  return false;
}

bool ON_SubDEdge::AddFaceToArray(
  ON_SubDFacePtr face_ptr
)
{
  if (m_face_count < 2)
    m_face2[m_face_count] = face_ptr;
  else if (nullptr != m_facex && m_face_count < 2 + m_facex_capacity)
    m_facex[m_face_count - 2] = face_ptr;
  else
  {
    // not enough room in m_facex.
    // If you really are trying to make a non-manifold subd, 
    // then use ON_SubD::GrowEdgeFaceArray().
    return ON_SUBD_RETURN_ERROR(false);
  }
  m_face_count++;
  return true;
}

bool ON_SubDEdge::RemoveFaceFromArray(
  unsigned int i,
  ON_SubDFacePtr& removed_face
  )
{
  removed_face = ON_SubDFacePtr::Null;
  unsigned int count = m_face_count;
  if ( i >= count )
    return ON_SUBD_RETURN_ERROR(false);
  if (i < 2)
  {
    removed_face = m_face2[i];
  }
  if (count > 2)
  {
    if ( nullptr == m_facex || m_facex_capacity + ((unsigned short)2) < m_face_count )
      return ON_SUBD_RETURN_ERROR(false);
    if ( i >= 2 )
      removed_face = m_facex[i-2];
  }

  unsigned int j = i+1;

  while (j < 2 && j < count )
    m_face2[i++] = m_face2[j++];

  if (count > 2)
  {
    m_face2[1] = m_facex[0];
    i = 0;
    j = 1;
    count -= 2;
    while (j < count )
      m_facex[i++] = m_facex[j++];
  }

  m_face_count--;

  return true;

}

bool ON_SubDFace::ReplaceEdgeInArray(
  unsigned int fei0,
  ON_SubDEdge* edge_to_remove,
  ON_SubDEdge* edge_to_insert
)
{
  const unsigned int face_edge_count = m_edge_count;
  ON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned int fei = 0; fei < face_edge_count; fei++, eptr++)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    if (fei >= fei0 && edge_to_remove == eptr->Edge() )
    {
      const ON__UINT_PTR edir = eptr->EdgeDirection();
      *eptr = ON_SubDEdgePtr::Create(edge_to_insert,edir);
      return true;
    }
  }
  return false;
}


bool ON_SubDFace::RemoveEdgeFromArray(
  unsigned int i,
  ON_SubDEdgePtr& removed_edge
  )
{
  removed_edge = ON_SubDEdgePtr::Null;
  unsigned int count = m_edge_count;
  if ( i >= count )
    return ON_SUBD_RETURN_ERROR(false);
  if (i < 4)
  {
    removed_edge = m_edge4[i];
  }
  if (count > 4)
  {
    if ( nullptr == m_edgex || m_edgex_capacity + ((unsigned short)4) < m_edge_count )
      return ON_SUBD_RETURN_ERROR(false);
    if ( i >= 4 )
      removed_edge = m_edgex[i-4];
  }

  unsigned int j = i+1;

  while (j < 4 && j < count )
    m_edge4[i++] = m_edge4[j++];

  if (count > 4)
  {
    m_edge4[3] = m_edgex[0];
    i = 0;
    j = 1;
    count -= 4;
    while (j < count )
      m_edgex[i++] = m_edgex[j++];
  }

  m_edge_count--;

  return true;
}

bool ON_SubDFace::RotateEdgeArray(
  unsigned int fei0
)
{
  if (0 == fei0)
    return true;

  const unsigned int edge_count = m_edge_count;
  if (edge_count < 2 || edge_count > ON_SubDFace::MaximumEdgeCount || fei0 >= edge_count)
    return false;

  ON_SubDEdgePtr stack_eptr[8];
  ON_SubDEdgePtr* eptr 
    = (edge_count*sizeof(stack_eptr[0]) > sizeof(stack_eptr)) 
    ? ((ON_SubDEdgePtr*)onmalloc(edge_count * sizeof(eptr[0])))
    : stack_eptr;
  if (nullptr == eptr)
    return false;

  ON_SubDEdgePtr* feptr = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++)
  {
    if (4 == fei)
    {
      feptr = m_edgex;
      if (nullptr == feptr)
      {
        if ( eptr != stack_eptr )
          onfree(eptr);  
        return false;
      }
    }
    eptr[fei] = *feptr++;
  }

  feptr = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++)
  {
    if (4 == fei)
      feptr = m_edgex;
    *feptr++ = eptr[(fei + fei0) % edge_count];
  }
  if ( eptr != stack_eptr )
    onfree(eptr);

  return true;
}




bool ON_SubDFace::RemoveEdgeFromArray(
  const ON_SubDEdge* e
  )
{
  unsigned int i;
  if (nullptr == e)
    return false;

  if (m_edge_count <= 4)
  {
    for (i = 0; i < m_edge_count; i++)
    {
      if (e == ON_SUBD_EDGE_POINTER(m_edge4[i].m_ptr))
      {
        for (i++; i < m_edge_count; i++)
          m_edge4[i - 1] = m_edge4[i];
        m_edge_count--;
        m_edge4[m_edge_count] = ON_SubDEdgePtr::Null;
        return true;
      }
    }
  }
  else
  {
    for (i = 0; i < 4; i++)
    {
      if (e == ON_SUBD_EDGE_POINTER(m_edge4[i].m_ptr))
      {
        for (i++; i < 4; i++)
          m_edge4[i - 1] = m_edge4[i];
        m_edge4[3] = m_edgex[0];
        for (i = 5; i < m_edge_count; i++)
          m_edgex[i - 5] = m_edgex[i - 4];
        m_edge_count--;
        m_edgex[m_edge_count-4] = ON_SubDEdgePtr::Null;
        return true;
      }
    }
    for (i = 4; i < m_edge_count; i++)
    {
      if (e == ON_SUBD_EDGE_POINTER(m_edgex[i - 4].m_ptr))
      {
        for (i++; i < m_edge_count; i++)
          m_edgex[i - 5] = m_edgex[i - 4];
        m_edge_count--;
        m_edgex[m_edge_count-4] = ON_SubDEdgePtr::Null;
        return true;
      }
    }
  }

  return false;
}

unsigned int ON_SubDFace::VertexIndex(
  const ON_SubDVertex* vertex
  ) const
{
  if (nullptr == vertex)
    return ON_UNSET_UINT_INDEX;

  const ON_SubDEdgePtr* face_edges = m_edge4;
  const unsigned int edge_count = m_edge_count;

  for (unsigned int i = 0; i < edge_count; i += 2)
  {
    if (4 == i)
    {
      face_edges = m_edgex;
      if (nullptr == face_edges)
        break;
      face_edges -= 4;
    }
    ON__UINT_PTR e_ptr = face_edges[i].m_ptr;
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr != edge)
    {
      if (vertex == edge->m_vertex[0])
        return (0 == ON_SUBD_EDGE_DIRECTION(e_ptr)) ? i : ((i + 1) % edge_count);
      if (vertex == edge->m_vertex[1])
        return (0 == ON_SUBD_EDGE_DIRECTION(e_ptr)) ? ((i + 1) % edge_count) : i;
    }
  }

  return ON_UNSET_UINT_INDEX;
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubD
//

ON_OBJECT_IMPLEMENT(ON_SubD,ON_Geometry,"F09BA4D9-455B-42C3-BA3B-E6CCACEF853B");

ON_SubD::ON_SubD() ON_NOEXCEPT
  : ON_Geometry()
{}

ON_SubD::~ON_SubD()
{
  this->Destroy();
}


#if defined(ON_HAS_RVALUEREF)
ON_SubD::ON_SubD( ON_SubD&& src ) ON_NOEXCEPT
  : ON_Geometry(std::move(src))
  , m_subdimple_sp(std::move(src.m_subdimple_sp))
{}

ON_SubD& ON_SubD::operator=( ON_SubD&& src )
{
  if ( this != &src )
  {
    this->Destroy();
    ON_Geometry::operator=(std::move(src));
    m_subdimple_sp = std::move(src.m_subdimple_sp);
  }
  return *this;
}
#endif


ON__UINT64 ON_SubD::RuntimeSerialNumber() const
{
  ON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->RuntimeSerialNumber : 0;
}


ON__UINT64 ON_SubD::ContentSerialNumber() const
{
  ON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ContentSerialNumber() : 0;
}

ON__UINT64 ON_SubD::ChangeContentSerialNumberForExperts()
{
  if (this == &ON_SubD::Empty)
    return 0;
  ON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ChangeContentSerialNumber() : 0;
}

ON_SubDComponentLocation ON_SubD::ToggleSubDAppearanceValue(ON_SubDComponentLocation subd_appearance)
{
  if (ON_SubDComponentLocation::Surface == subd_appearance)
    return ON_SubDComponentLocation::ControlNet;
  if (ON_SubDComponentLocation::ControlNet == subd_appearance)
    return ON_SubDComponentLocation::Surface;
  return subd_appearance;
}

ON_SubDComponentLocation ON_SubDMeshFragmentIterator::SubDAppearance() const
{
  return 
    (ON_SubDComponentLocation::Surface == m_subd_appearance_override || ON_SubDComponentLocation::ControlNet == m_subd_appearance_override)
    ? m_subd_appearance_override
    : SubD().SubDAppearance();
}

void ON_SubDMeshFragmentIterator::SetSubDAppearanceOverride(ON_SubDComponentLocation subd_appearance_override)
{
  if (m_subd_appearance_override != subd_appearance_override )
    m_subd_appearance_override = subd_appearance_override;
}

ON_SubDComponentLocation ON_SubD::SubDAppearance() const
{
  const ON_SubDimple* subdimple = this->SubDimple();
  return (nullptr != subdimple) ? subdimple->SubDAppearance() : ON_SubD::DefaultSubDAppearance;
}

ON_SubDComponentLocation ON_SubDimple::SubDAppearance() const
{
  return m_subd_appearance;
}

void ON_SubD::SetSubDAppearance(ON_SubDComponentLocation subd_appearance) const
{
  if (
    subd_appearance != SubDAppearance()
    && (ON_SubDComponentLocation::Surface == subd_appearance || ON_SubDComponentLocation::ControlNet == subd_appearance)
    )
  {
    const ON_SubDimple* subdimple = const_cast<ON_SubD*>(this)->SubDimple(true);
    if (nullptr != subdimple)
      subdimple->SetSubDAppearance(subd_appearance);
  }
}

void ON_SubDimple::SetSubDAppearance(ON_SubDComponentLocation subd_appearance) const
{
  if (
    subd_appearance != m_subd_appearance
    && (ON_SubDComponentLocation::Surface == subd_appearance || ON_SubDComponentLocation::ControlNet == subd_appearance)
    )
  {
    m_subd_appearance = subd_appearance;
  }
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubD - ON_Object overrides
//

//virtual 
void ON_SubD::MemoryRelocate()
{
}

static bool ON_SubDIsNotValid(bool bSilentError)
{
  ON_SubDIncrementErrorCount();
  return bSilentError ? false : ON_IsNotValid();
}

static bool EdgeSectorCoefficientIsSet(
  double edge_sector_coefficient
  )
{
  return (0.0 < edge_sector_coefficient && edge_sector_coefficient < 1.0);
}

static bool EdgeSectorWeightIsValid(
  double edge_vertex_weight,
  const ON_SubDEdge* edge
  )
{
  if (0.0 <= edge_vertex_weight && edge_vertex_weight < 1.0)
    return true;

  if (ON_SubDSectorType::UnsetSectorCoefficient == edge_vertex_weight && nullptr != edge && 0 == edge->SubdivisionLevel())
    return true;

  return false;
}

static bool IsValidVertexEdgeLink(
  const ON_SubDVertex* vertex,
  const ON_SubDEdge* edge,
  ON__UINT_PTR end_index,
  bool bSilentError
  )
{
  if (nullptr == vertex || nullptr == edge)
    return ON_SubDIsNotValid(bSilentError);

  if (end_index > 1)
    return ON_SubDIsNotValid(bSilentError);

  if (edge->m_vertex[end_index] != vertex)
    return ON_SubDIsNotValid(bSilentError);

  if (vertex->SubdivisionLevel() != edge->SubdivisionLevel())
    return ON_SubDIsNotValid(bSilentError);

  if (false == EdgeSectorWeightIsValid(edge->m_sector_coefficient[end_index],edge))
    return ON_SubDIsNotValid(bSilentError);

  if ( edge->IsSmooth() )
  {
    // edge->m_edge_tag is ON_SubD::EdgeTag::Smooth or ON_SubD::EdgeTag::SmoothX
    if (ON_SubD::VertexTag::Smooth == vertex->m_vertex_tag)
    {
      if (false == (0.0 == edge->m_sector_coefficient[end_index]))
        return ON_SubDIsNotValid(bSilentError);
    }
    else
    {
      const unsigned int tagged_end_index = edge->TaggedEndIndex();
      if (ON_SubD::EdgeTag::SmoothX == edge->m_edge_tag)
      {
        if (2 != tagged_end_index)
          return ON_SubDIsNotValid(bSilentError);
      }
      else
      {
        if (tagged_end_index != (unsigned int)end_index)
          return ON_SubDIsNotValid(bSilentError);
      }
       
      ON_SubDSectorType st = ON_SubDSectorType::Create(edge,(unsigned int)end_index);
      if (!st.IsValid())
        return ON_SubDIsNotValid(bSilentError);

      const double expected_sector_coefficient = st.SectorCoefficient();
      if (false == (expected_sector_coefficient == edge->m_sector_coefficient[end_index]))
        return ON_SubDIsNotValid(bSilentError);

      if (false == EdgeSectorCoefficientIsSet(expected_sector_coefficient))
        return ON_SubDIsNotValid(bSilentError);
    }
  }
  else if(ON_SubD::EdgeTag::Crease == edge->m_edge_tag)
  {
    // crease edge
    if (!(0.0 == edge->m_sector_coefficient[end_index]))
      return ON_SubDIsNotValid(bSilentError);

    if (ON_SubD::VertexTag::Smooth == vertex->m_vertex_tag)
      return ON_SubDIsNotValid(bSilentError);

    if (ON_SubD::VertexTag::Unset == vertex->m_vertex_tag)
      return ON_SubDIsNotValid(bSilentError);
  }
  else
  {
    return ON_SubDIsNotValid(bSilentError);
  }

  return true;
}

static bool IsValidVertexFaceLink(
  const ON_SubDVertex* vertex,
  const ON_SubDFace* face,
  unsigned int vertex_face_index,
  unsigned int face_vertex_index,
  bool bSilentError
  )
{
  if (nullptr == vertex || nullptr == face)
    return ON_SubDIsNotValid(bSilentError);

  if (vertex->SubdivisionLevel() != face->SubdivisionLevel())
    return ON_SubDIsNotValid(bSilentError);

  const unsigned int vertex_face_count = vertex->m_face_count;
  if (vertex_face_count <= 0)
    return ON_SubDIsNotValid(bSilentError);
  if (nullptr == vertex->m_faces)
    return ON_SubDIsNotValid(bSilentError);

  if (vertex_face_index >= vertex_face_count && ON_UNSET_UINT_INDEX != vertex_face_index)
    return ON_SubDIsNotValid(bSilentError);

  const unsigned int face_vertex_count = face->m_edge_count;
  if (face_vertex_count <= 0)
    return ON_SubDIsNotValid(bSilentError);
  if (face_vertex_count > 4 && nullptr == face->m_edgex)
    return ON_SubDIsNotValid(bSilentError);

  if (face_vertex_index >= face_vertex_count && ON_UNSET_UINT_INDEX != face_vertex_index)
    return ON_SubDIsNotValid(bSilentError);

  for (unsigned int i = 0; i < vertex_face_count; i++)
  {
    if (face == vertex->Face(i))
    {
      if (ON_UNSET_UINT_INDEX == vertex_face_index)
        vertex_face_index = i;
      else if (i != vertex_face_index)
        return ON_SubDIsNotValid(bSilentError);
    }
    else if (i == vertex_face_index)
    {
      return ON_SubDIsNotValid(bSilentError);
    }
  }

  for (unsigned int i = 0; i < face_vertex_count; i++)
  {
    if (vertex == face->Vertex(i))
    {
      if (ON_UNSET_UINT_INDEX == face_vertex_index)
        face_vertex_index = i;
      else if (i != face_vertex_index)
        return ON_SubDIsNotValid(bSilentError);
    }
    else if (i == face_vertex_index)
    {
      return ON_SubDIsNotValid(bSilentError);
    }
  }

  return true;
}


static bool IsValidEdgeFaceLink(
  const ON_SubDEdge* edge,
  const ON_SubDFace* face,
  unsigned int edge_face_index,
  unsigned int face_edge_index,
  bool bSilentError
  )
{
  if (nullptr == edge || nullptr == face)
    return ON_SubDIsNotValid(bSilentError);

  if (edge->SubdivisionLevel() != face->SubdivisionLevel())
    return ON_SubDIsNotValid(bSilentError);

  const unsigned int edge_face_count = edge->m_face_count;
  if (edge_face_count <= 0)
    return ON_SubDIsNotValid(bSilentError);
  if (edge_face_count > 2 && nullptr == edge->m_facex)
    return ON_SubDIsNotValid(bSilentError);

  if (edge_face_index >= edge_face_count && ON_UNSET_UINT_INDEX != edge_face_index)
    return ON_SubDIsNotValid(bSilentError);

  const unsigned int face_edge_count = face->m_edge_count;
  if (face_edge_count <= 0)
    return ON_SubDIsNotValid(bSilentError);
  if (face_edge_count > 4 && nullptr == face->m_edgex)
    return ON_SubDIsNotValid(bSilentError);

  if (face_edge_index >= face_edge_count && ON_UNSET_UINT_INDEX != face_edge_index)
    return ON_SubDIsNotValid(bSilentError);

  for (unsigned int i = 0; i < edge_face_count; i++)
  {
    if (face == edge->Face(i))
    {
      if (ON_UNSET_UINT_INDEX == edge_face_index)
        edge_face_index = i;
      else if (i != edge_face_index)
        return ON_SubDIsNotValid(bSilentError);
    }
    else if (i == edge_face_index)
    {
      return ON_SubDIsNotValid(bSilentError);
    }
  }

  for (unsigned int i = 0; i < face_edge_count; i++)
  {
    if (edge == face->Edge(i))
    {
      if (ON_UNSET_UINT_INDEX == face_edge_index)
        face_edge_index = i;
      else if (i != face_edge_index)
        return ON_SubDIsNotValid(bSilentError);
    }
    else if (i == face_edge_index)
    {
      return ON_SubDIsNotValid(bSilentError);
    }
  }

  return true;
}

class ON_Internal_DamagedMarker
{
public:
  ON_Internal_DamagedMarker() = default;
  ~ON_Internal_DamagedMarker()
  {
    if (nullptr != m_subd_component)
      m_subd_component->m_status.SetDamagedState(true);
  }

private:
  ON_Internal_DamagedMarker(const ON_Internal_DamagedMarker&) = delete;
  ON_Internal_DamagedMarker& operator=(const ON_Internal_DamagedMarker&) = delete;

public:
  ON_Internal_DamagedMarker(const ON_SubDComponentBase* subd_component)
    : m_subd_component(subd_component)
  {}

  void ClearComponent()
  {
    m_subd_component = nullptr;
  }

private:
  const ON_SubDComponentBase* m_subd_component = nullptr;
};

static bool IsValidSubDVertexTag(
  const ON_SubDVertex* vertex,
  bool bSilentError
)
{
  if (nullptr == vertex)
    return true; // this error detected elsewhere.

  ON_Internal_DamagedMarker dm(vertex);

  const unsigned short vertex_edge_count = vertex->m_edge_count;
  unsigned short crease_edge_count = 0;
  unsigned short smooth_edge_count = 0;
  for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
    if (nullptr == e)
      continue;
    if (e->IsSmooth())
      ++smooth_edge_count;
    else if (e->IsCrease())
      ++crease_edge_count;
  }
  const bool bValidEdgeTags = (vertex_edge_count == crease_edge_count + smooth_edge_count);

  switch (vertex->m_vertex_tag)
  {
  case ON_SubD::VertexTag::Unset:
    return ON_SubDIsNotValid(bSilentError);
    break;

  case ON_SubD::VertexTag::Smooth:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if (
      0 != crease_edge_count
      || vertex_edge_count < 2
      || vertex_edge_count != smooth_edge_count
      || vertex_edge_count != vertex->m_face_count
      )
    {
      return ON_SubDIsNotValid(bSilentError);
    }
    break;

  case ON_SubD::VertexTag::Crease:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if ( 2 != crease_edge_count )
    {
      return ON_SubDIsNotValid(bSilentError);
    }
    break;

  case ON_SubD::VertexTag::Corner:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if (0 == crease_edge_count)
    {
      // currently, isolated vertices are not permitted - may change in the future
      return ON_SubDIsNotValid(bSilentError);
    }
    if (1 == crease_edge_count)
    {
      // must be a single wire crease edge ending at this vertex 
      if ( 1 != vertex_edge_count || 0 != vertex->m_face_count)
        return ON_SubDIsNotValid(bSilentError);

    }
    break;

  case ON_SubD::VertexTag::Dart:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if (
      1 != crease_edge_count
      || vertex_edge_count < 2
      || vertex_edge_count != smooth_edge_count + crease_edge_count
      || vertex_edge_count != vertex->m_face_count
      )
    {
      return ON_SubDIsNotValid(bSilentError);
    }
    break;

  default:
    return ON_SubDIsNotValid(bSilentError);
    break;
  }

  dm.ClearComponent();
  return true;
}

static bool IsValidSubDEdgeTag(
  const ON_SubDEdge* edge,
  bool bSilentError
)
{
  if (nullptr == edge)
    return true; // this error detected elsewhere.

  //ON_SubD::VertexTag vtag[2] = { ON_SubD::VertexTag::Unset,ON_SubD::VertexTag::Unset };
  unsigned int smooth_vertex_count = 0;
  unsigned int crease_vertex_count = 0;
  unsigned int corner_vertex_count = 0;
  unsigned int dart_vertex_count = 0;

  for ( unsigned int evi = 0; evi < 2; evi++)
  {
    if (nullptr == edge->m_vertex[evi])
      return true; // topology errors detected elsewhere
    switch (edge->m_vertex[evi]->m_vertex_tag)
    {
    case ON_SubD::VertexTag::Smooth:
      ++smooth_vertex_count;
      break;
    case ON_SubD::VertexTag::Crease:
      ++crease_vertex_count;
      break;
    case ON_SubD::VertexTag::Corner:
      ++corner_vertex_count;
      break;
    case ON_SubD::VertexTag::Dart:
      ++dart_vertex_count;
      break;
    }
  };

  if (2 != smooth_vertex_count + crease_vertex_count + corner_vertex_count + dart_vertex_count)
    return true; // invalid vertex tags detected in IsValidSubDVertexTag();

  ON_Internal_DamagedMarker dm(edge);

  //const unsigned short edge_face_count = edge->m_face_count;
  switch(edge->m_edge_tag)
  {

  case ON_SubD::EdgeTag::Unset:
    return ON_SubDIsNotValid(bSilentError);
    break;

  case ON_SubD::EdgeTag::Smooth:
    if ( 2 != edge->m_face_count)
      return ON_SubDIsNotValid(bSilentError);
    if ( smooth_vertex_count < 1)
      return ON_SubDIsNotValid(bSilentError);
    break;

  case ON_SubD::EdgeTag::Crease:
    if ( 0 != smooth_vertex_count )
      return ON_SubDIsNotValid(bSilentError);
    break;

  case ON_SubD::EdgeTag::SmoothX:
    if ( 2 != edge->m_face_count)
      return ON_SubDIsNotValid(bSilentError);
    if ( 0 != smooth_vertex_count )
      return ON_SubDIsNotValid(bSilentError);
    break;

  default:
    return ON_SubDIsNotValid(bSilentError);
    break;
  }

  dm.ClearComponent();
  return true;
}


static bool IsValidSubDVertex(
  const ON_SubDVertex* vertex,
  unsigned short level,
  unsigned int* vertex_id_range,
  unsigned short ordinary_valence_count,
  bool bSilentError
  )
{
  if (nullptr == vertex)
    return ON_SubDIsNotValid(bSilentError);
  
  if (vertex->SubdivisionLevel() != level)
    return ON_SubDIsNotValid(bSilentError);
  
  if (nullptr != vertex_id_range)
  {
    if (vertex->m_id < vertex_id_range[0])
      return ON_SubDIsNotValid(bSilentError);
    if (vertex->m_id > vertex_id_range[1])
      return ON_SubDIsNotValid(bSilentError);
  }

  ON_Internal_DamagedMarker dm(vertex);

  if (vertex->m_edge_count < vertex->m_face_count)
  {
    if ( ON_SubD::VertexTag::Corner != vertex->m_vertex_tag || vertex->m_edge_count < 3 )
      return ON_SubDIsNotValid(bSilentError);
  }

  if (vertex->m_edge_count > 0 && nullptr == vertex->m_edges)
    return ON_SubDIsNotValid(bSilentError);

  if (vertex->m_face_count > 0 && nullptr == vertex->m_faces)
    return ON_SubDIsNotValid(bSilentError);

  switch (vertex->m_vertex_tag)
  {
  case ON_SubD::VertexTag::Smooth: // interior vertex
    if (vertex->m_edge_count != vertex->m_face_count)
      return ON_SubDIsNotValid(bSilentError);
    break;

  case ON_SubD::VertexTag::Crease:
    if ( vertex->m_edge_count < 2 )
      return ON_SubDIsNotValid(bSilentError);
    break;

  case ON_SubD::VertexTag::Corner:
    if ( vertex->m_edge_count < 1 )
      return ON_SubDIsNotValid(bSilentError);
    break;

  case ON_SubD::VertexTag::Dart: // interior vertex
    if (level > 0 && ordinary_valence_count != vertex->m_edge_count)
      return ON_SubDIsNotValid(bSilentError);
    if (vertex->m_edge_count != vertex->m_face_count)
      return ON_SubDIsNotValid(bSilentError);
    break;

  default:
    // invalid value for this enum
    return ON_SubDIsNotValid(bSilentError);
    break;
  }

  unsigned int count = vertex->m_edge_count;
  for (unsigned int i = 0; i < count; i++)
  {
    const ON_SubDEdge* edge = vertex->Edge(i);
    if (nullptr == edge)
      return ON_SubDIsNotValid(bSilentError);
  }

  count = vertex->m_face_count;
  for (unsigned int i = 0; i < count; i++)
  {
    const ON_SubDFace* face = vertex->Face(i);
    if (nullptr == face)
      return ON_SubDIsNotValid(bSilentError);
  }

  dm.ClearComponent();
  return true;
}


static bool IsValidSubDEdge(
  const ON_SubDEdge* edge,
  unsigned short level,
  unsigned int* edge_id_range,
  bool bSilentError
  )
{
  if (nullptr == edge)
    return ON_SubDIsNotValid(bSilentError);

  if (edge->SubdivisionLevel() != level)
    return ON_SubDIsNotValid(bSilentError);

  if (nullptr != edge_id_range)
  {
    if (edge->m_id < edge_id_range[0])
      return ON_SubDIsNotValid(bSilentError);
    if (edge->m_id > edge_id_range[1])
      return ON_SubDIsNotValid(bSilentError);
  }

  ON_Internal_DamagedMarker dm(edge);

  const ON_SubDVertex* edge_vertex[2] = { 0 };
  for (unsigned int i = 0; i < 2; i++)
  {
    const ON_SubDVertex* vertex = edge->Vertex(i);
    if (nullptr == vertex)
      return ON_SubDIsNotValid(bSilentError);
    edge_vertex[i] = vertex;
  }
  if (edge_vertex[0] == edge_vertex[1])
    return ON_SubDIsNotValid(bSilentError);

  if (edge->IsSmooth())
  {
    // m_edge_tag is ON_SubD::EdgeTag::Smooth or ON_SubD::EdgeTag::SmoothX
    if ( 2 != edge->m_face_count)
      return ON_SubDIsNotValid(bSilentError);
  }
  else if (ON_SubD::EdgeTag::Crease != edge->m_edge_tag)
  {
    return ON_SubDIsNotValid(bSilentError);
  }

  if (edge->m_face_count > 2 && nullptr == edge->m_facex)
    return ON_SubDIsNotValid(bSilentError);

  dm.ClearComponent();

  return true;
}

static bool IsValidSubDFace(
  const ON_SubDFace* face,
  unsigned short level,
  unsigned int* face_id_range,
  unsigned short ordinary_face_edge_count,
  bool bSilentError
  )
{
  if (nullptr == face)
    return ON_SubDIsNotValid(bSilentError);

  if (face->SubdivisionLevel() != level)
    return ON_SubDIsNotValid(bSilentError);

  if (nullptr != face_id_range)
  {
    if (face->m_id < face_id_range[0])
      return ON_SubDIsNotValid(bSilentError);
    if (face->m_id > face_id_range[1])
      return ON_SubDIsNotValid(bSilentError);
  }

  ON_Internal_DamagedMarker dm(face);

  if (face->m_edge_count < 3)
    return ON_SubDIsNotValid(bSilentError);

  if (face->m_edge_count > 4 && nullptr == face->m_edgex)
    return ON_SubDIsNotValid(bSilentError);

  if (level > 0 && ordinary_face_edge_count != face->m_edge_count)
    return ON_SubDIsNotValid(bSilentError);

  dm.ClearComponent();

  return true;
}

bool ON_SubDimple::IsValidLevel(
  const ON_SubD& subd,
  unsigned int level_index,
  bool bSilentError,
  ON_TextLog* text_log
  ) const
{
  const unsigned int level_count = m_levels.UnsignedCount();
  if (level_index >= level_count || level_index >= 0xFFFF)
    return ON_SubDIsNotValid(bSilentError);
  const ON_SubDLevel* level = m_levels[level_index];
  if ( nullptr == level)
    return ON_SubDIsNotValid(bSilentError);
  level->ClearComponentDamagedState();

  if ( level->m_level_index != level_index)
    return ON_SubDIsNotValid(bSilentError);

  if (level_index <= 0)
  {
    if (level->m_vertex_count < 3)
      return ON_SubDIsNotValid(bSilentError);
    if (level->m_edge_count < 3)
      return ON_SubDIsNotValid(bSilentError);
    if (level->m_face_count < 1)
      return ON_SubDIsNotValid(bSilentError);
  }
  else
  {
    const ON_SubDLevel* previous_level = m_levels[level_index - 1];
    if (nullptr == previous_level)
      return ON_SubDIsNotValid(bSilentError);
    if (level->m_vertex_count <= previous_level->m_vertex_count)
      return ON_SubDIsNotValid(bSilentError);
    if (level->m_edge_count <= previous_level->m_edge_count)
      return ON_SubDIsNotValid(bSilentError);
    if (level->m_face_count <= previous_level->m_face_count)
      return ON_SubDIsNotValid(bSilentError);
  }

  if (nullptr == level->m_vertex[0])
    return ON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_edge[0])
    return ON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_face[0])
    return ON_SubDIsNotValid(bSilentError);

  if (nullptr == level->m_vertex[1])
    return ON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_edge[1])
    return ON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_face[1])
    return ON_SubDIsNotValid(bSilentError);


  const unsigned short expected_level = (unsigned short)level_index;
  unsigned int i;
  const ON_SubDVertex* vertex;
  const ON_SubDEdge* edge;
  const ON_SubDFace* face;

  unsigned int v_id_range[2] = { ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX };
  unsigned int e_id_range[2] = { ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX };
  unsigned int f_id_range[2] = { ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX };

  unsigned int point_vertex_count = 0;
  unsigned int wire_edge_count = 0;

  // simple vertex validation
  if (level_index == subd.ActiveLevelIndex())
  {
    if (subd.FirstVertex() != level->m_vertex[0])
      return ON_SubDIsNotValid(bSilentError);
    ON_SubDVertexIterator vit = subd.VertexIterator();
    if (vit.FirstVertex() != level->m_vertex[0])
      return ON_SubDIsNotValid(bSilentError);
    ON_SubDVertexArray va = subd.VertexArray();
    if (va.VertexCount() != level->m_vertex_count)
      return ON_SubDIsNotValid(bSilentError);
    if (va[0] != level->m_vertex[0])
      return ON_SubDIsNotValid(bSilentError);
    if (va[level->m_vertex_count-1] != level->m_vertex[1])
      return ON_SubDIsNotValid(bSilentError);
  }
  const ON_SubDVertex* last_vertex = nullptr;
  for (i = 0, vertex = level->m_vertex[0]; i < level->m_vertex_count && nullptr != vertex; i++, vertex = vertex->m_next_vertex)
  {
    if (false == IsValidSubDVertex(vertex, expected_level, nullptr, level->m_ordinary_vertex_valence, bSilentError))
      return false;

    if (0 == i)
    {
      v_id_range[0] = v_id_range[1] = vertex->m_id;
    }
    else if (vertex->m_id < v_id_range[0])
      v_id_range[0] = vertex->m_id;
    else if (vertex->m_id > v_id_range[1])
      v_id_range[1] = vertex->m_id;

    if (0 == vertex->m_edge_count)
    {
      point_vertex_count++;
    }
    last_vertex = vertex;
  }
  if (level->m_vertex[1] != last_vertex)
    return ON_SubDIsNotValid(bSilentError);


  if (i != level->m_vertex_count || nullptr != vertex)
    return ON_SubDIsNotValid(bSilentError);
  if (1 + v_id_range[1] - v_id_range[0] < level->m_vertex_count)
    return ON_SubDIsNotValid(bSilentError);
  if ( v_id_range[1] > m_max_vertex_id )
    return ON_SubDIsNotValid(bSilentError);

  // currently, point vertices are not permitted
  if (point_vertex_count > 0)
    return ON_SubDIsNotValid(bSilentError);

  // simple edge validation
  if (level_index == subd.ActiveLevelIndex())
  {
    if (subd.FirstEdge() != level->m_edge[0])
      return ON_SubDIsNotValid(bSilentError);
    ON_SubDEdgeIterator eit = subd.EdgeIterator();
    if (eit.FirstEdge() != level->m_edge[0])
      return ON_SubDIsNotValid(bSilentError);
    ON_SubDEdgeArray ea = subd.EdgeArray();
    if (ea.EdgeCount() != level->m_edge_count)
      return ON_SubDIsNotValid(bSilentError);
    if (ea[0] != level->m_edge[0])
      return ON_SubDIsNotValid(bSilentError);
    if (ea[level->m_edge_count-1] != level->m_edge[1])
      return ON_SubDIsNotValid(bSilentError);
  }
  const ON_SubDEdge* last_edge = nullptr;
  for (i = 0, edge = level->m_edge[0]; i < level->m_edge_count && nullptr != edge; i++, edge = edge->m_next_edge)
  {
    if (false == IsValidSubDEdge(edge, expected_level, nullptr, bSilentError))
      return false;
    if (0 == edge->m_face_count)
    {
      wire_edge_count++;
    }
    if (0 == i)
    {
      e_id_range[0] = e_id_range[1] = edge->m_id;
    }
    else if (edge->m_id < e_id_range[0])
      e_id_range[0] = edge->m_id;
    else if (edge->m_id > e_id_range[1])
      e_id_range[1] = edge->m_id;

    last_edge = edge;
  }
  if (i != level->m_edge_count || nullptr != edge)
    return ON_SubDIsNotValid(bSilentError);
  if (1 + e_id_range[1] - e_id_range[0] < level->m_edge_count)
    return ON_SubDIsNotValid(bSilentError);
  if (level->m_edge[1] != last_edge)
    return ON_SubDIsNotValid(bSilentError);
  if ( e_id_range[1] > m_max_edge_id )
    return ON_SubDIsNotValid(bSilentError);
   
  // As of NOvember 12, 2019 
  // Wire edges are permitted. THey exist in subds being edited.
  ////// currently, wire edges are not permitted
  ////if (wire_edge_count > 0)
  ////  return ON_SubDIsNotValid(bSilentError);
   
  // simple face validation
  if (level_index == subd.ActiveLevelIndex())
  {
    if (subd.FirstFace() != level->m_face[0])
      return ON_SubDIsNotValid(bSilentError);
    ON_SubDFaceIterator fit = subd.FaceIterator();
    if (fit.FirstFace() != level->m_face[0])
      return ON_SubDIsNotValid(bSilentError);
    ON_SubDFaceArray fa = subd.FaceArray();
    if (fa.FaceCount() != level->m_face_count)
      return ON_SubDIsNotValid(bSilentError);
    if (fa[0] != level->m_face[0])
      return ON_SubDIsNotValid(bSilentError);
    if (fa[0] != level->m_face[0])
      return ON_SubDIsNotValid(bSilentError);
  }
  const ON_SubDFace* last_face = nullptr;
  for (i = 0, face = level->m_face[0]; i < level->m_face_count && nullptr != face; i++, face = face->m_next_face)
  {
    if (false == IsValidSubDFace(face, expected_level, nullptr, level->m_ordinary_face_edge_count, bSilentError))
      return false;

    if (0 == i)
    {
      f_id_range[0] = f_id_range[1] = face->m_id;
    }
    else if (face->m_id < f_id_range[0])
      f_id_range[0] = face->m_id;
    else if (face->m_id > f_id_range[1])
      f_id_range[1] = face->m_id;

    last_face = face;
  }
  if (i != level->m_face_count || nullptr != face)
    return ON_SubDIsNotValid(bSilentError);
  if (1 + f_id_range[1] - f_id_range[0] < level->m_face_count)
    return ON_SubDIsNotValid(bSilentError);  
  if (level->m_face[1] != last_face)
    return ON_SubDIsNotValid(bSilentError);
  if ( f_id_range[1] > m_max_face_id )
    return ON_SubDIsNotValid(bSilentError);


  // vertex topology validation
  for (vertex = level->m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
  {
    for (i = 0; i < vertex->m_edge_count; i++)
    {
      edge = vertex->Edge(i);
      if (false == IsValidSubDEdge(edge, expected_level, e_id_range, bSilentError))
        return false;
      if (false == IsValidVertexEdgeLink(vertex, edge, vertex->EdgeDirection(i), bSilentError))
        return false;
    }

    for (i = 0; i < vertex->m_face_count; i++)
    {
      face = vertex->Face(i);
      if (false == IsValidSubDFace(face, expected_level, f_id_range, level->m_ordinary_face_edge_count, bSilentError))
        return false;
      if (false == IsValidVertexFaceLink(vertex, face, i, ON_UNSET_UINT_INDEX, bSilentError))
        return false;
    }
  }

  // edge topology validation
  for (edge = level->m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    for (i = 0; i < 2; i++)
    {
      vertex = edge->m_vertex[i];
      if (false == IsValidSubDVertex(vertex, expected_level, v_id_range, level->m_ordinary_vertex_valence, bSilentError))
        return false;
      if (false == IsValidVertexEdgeLink(vertex, edge, i, bSilentError))
        return false;
    }

    for (i = 0; i < edge->m_face_count; i++)
    {
      face = edge->Face(i);
      if (false == IsValidSubDFace(face, expected_level, f_id_range, level->m_ordinary_face_edge_count, bSilentError))
        return false;
      if (false == IsValidEdgeFaceLink(edge, face, i, ON_UNSET_UINT_INDEX, bSilentError))
        return false;
    }
  }

  // face topology validation
  for (face = level->m_face[0]; nullptr != face; face = face->m_next_face)
  {
    for (i = 0; i < face->m_edge_count; i++)
    {
      edge = face->Edge(i);
      if (false == IsValidSubDEdge(edge, expected_level, e_id_range, bSilentError))
        return false;
      if (false == IsValidEdgeFaceLink(edge, face, ON_UNSET_UINT_INDEX, i, bSilentError))
        return false;
    }

    for (i = 0; i < face->m_edge_count; i++)
    {
      vertex = face->Vertex(i);
      if (false == IsValidSubDVertex(vertex, expected_level, v_id_range, level->m_ordinary_vertex_valence, bSilentError))
        return false;
      if (false == IsValidVertexFaceLink(vertex, face, ON_UNSET_UINT_INDEX, i, bSilentError))
        return false;
    }
  }


  // edge tag validation
  for (edge = level->m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    if (false == IsValidSubDEdgeTag(edge, bSilentError))
      return false;
  }

  // vertex tag validation
  for (vertex = level->m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
  {
    if (false == IsValidSubDVertexTag(vertex, bSilentError))
      return false;
  }

  // edge length validation
  for (edge = level->m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    const ON_3dPoint P[2] = { edge->m_vertex[0]->ControlNetPoint(), edge->m_vertex[1]->ControlNetPoint() };
    if (false == (P[0] != P[1]))
    {
      edge->m_status.SetDamagedState(true);
      return ON_SubDIsNotValid(bSilentError);
    }
  }

  return true;
}

bool ON_SubDimple::IsValid(
  const ON_SubD& subd,
  bool bSilentError,
  ON_TextLog* text_log
  ) const
{
  const unsigned int level_count = m_levels.UnsignedCount();
  if (level_count < 1)
  {
    return ON_SubDIsNotValid(bSilentError);
  }
  for (unsigned int level_index = 0; level_index < level_count; level_index++)
  {
    if (false == IsValidLevel(subd, level_index, bSilentError, text_log))
      return false;
  }
  
  if (false == m_heap.IsValid())
  {
    // id values are not increasing in the heap blocks.
    if (nullptr != text_log)
      text_log->Print("Component ids are not set correctly. m_heap.ResetId() will fix this but may break externally stored component references.\n");
    return ON_SubDIsNotValid(bSilentError);
  }

  if (MaximumVertexId() < m_heap.MaximumVertexId())
  {
    if (nullptr != text_log)
      text_log->Print("MaximumVertexId() = %u < m_heap.MaximumVertexId() = %u\n", MaximumVertexId(), m_heap.MaximumVertexId());
    return ON_SubDIsNotValid(bSilentError);
  }

  if (MaximumEdgeId() < m_heap.MaximumEdgeId())
  {
    if (nullptr != text_log)
      text_log->Print("MaximumEdgeId() = %u < m_heap.MaximumEdgeId() = %u\n", MaximumEdgeId(), m_heap.MaximumEdgeId());
    return ON_SubDIsNotValid(bSilentError);
  }

  if (MaximumFaceId() <m_heap.MaximumFaceId())
  {
    if (nullptr != text_log)
      text_log->Print("MaximumFaceId() = %u < m_heap.MaximumFaceId() = %u\n", MaximumFaceId(), m_heap.MaximumFaceId());
    return ON_SubDIsNotValid(bSilentError);
  }

  return true;
}

//virtual
bool ON_SubD::IsValid(ON_TextLog* text_logx) const
{
  // If low bit of text_log pointer is 1, then ON_Error is not called when the
  // knot vector is invalid.
  const ON__INT_PTR lowbit = 1;
  const ON__INT_PTR hightbits = ~lowbit;
  const bool bSilentError = (0 != (lowbit & ((ON__INT_PTR)text_logx)));
  ON_TextLog* text_log = (ON_TextLog*)(((ON__INT_PTR)text_logx) & hightbits);
  
  const ON_SubDimple* subdimple = SubDimple();
  if (nullptr == subdimple)
    return ON_SubDIsNotValid(bSilentError);

  return subdimple->IsValid(*this, bSilentError, text_log);
}

//virtual
void ON_SubD::Dump(ON_TextLog& text_log) const
{
  unsigned int component_sample_count = 16; // dump the first 16 vertices, edges, faces
  ON_2udex id_range;
  id_range.i = component_sample_count;
  id_range.j = 0;
  DumpTopology(id_range,id_range,id_range,text_log);
}

unsigned int ON_SubD::DumpTopology(ON_TextLog & text_log) const
{
  return DumpTopology(ON_2udex::Zero,ON_2udex::Zero,ON_2udex::Zero,text_log);
}


unsigned int ON_SubD::DumpTopology(
  ON_2udex vertex_id_range,
  ON_2udex edge_id_range,
  ON_2udex face_id_range,
  ON_TextLog& text_log
) const
{
  const class ON_SubDimple* subdimple = SubDimple();
  if (nullptr == subdimple)
  {
    text_log.Print(L"SubD: Empty\n");
    return 0;
  }

  const unsigned int level_count = LevelCount();
  const unsigned int active_level_index = ActiveLevel().m_level_index;

  const ON__UINT64 runtime_sn = (text_log.IsTextHash()) ? 0 : RuntimeSerialNumber(); // TextHash ignores settings that don't depend on 3dm file content.

  const ON_wString subd_texture_domain = ON_SubD::TextureDomainTypeToString(this->TextureDomainType());
  if (level_count > 1)
    text_log.Print(L"SubD[%" PRIu64 "]: %u levels. Level %u is active. texture domain type = %ls.\n",
      runtime_sn, 
      level_count,
      active_level_index,
      static_cast<const wchar_t*>(subd_texture_domain)
    );
  else
    text_log.Print(L"SubD[%" PRIu64 "]: texture domain type = %ls.\n", runtime_sn, static_cast<const wchar_t*>(subd_texture_domain));


  text_log.Print(L"Levels:\n");

  ON_SubDLevelIterator lit(subdimple->LevelIterator());

  const ON_2udex empty_id_range(ON_UNSET_UINT_INDEX, 0);

  unsigned int error_count = 0;
  for (const ON_SubDLevel* level = lit.First(); nullptr != level; level = lit.Next())
  {
    ON_TextLogIndent indent1(text_log);
    if (nullptr == level)
      continue;
    ON_2udex level_vertex_id_range
      = (0 != vertex_id_range.j || active_level_index == level->m_level_index)
      ? vertex_id_range
      : empty_id_range;
    ON_2udex level_edge_id_range
      = (0 != edge_id_range.j || active_level_index == level->m_level_index)
      ? edge_id_range
      : empty_id_range;
    ON_2udex level_face_id_range
      = (0 != face_id_range.j || active_level_index == level->m_level_index)
      ? face_id_range
      : empty_id_range;

    error_count += level->DumpTopology(
      subdimple->MaximumVertexId(),
      subdimple->MaximumEdgeId(),
      subdimple->MaximumFaceId(),
      level_vertex_id_range,
      level_edge_id_range, 
      level_face_id_range,
      text_log);
  }

  return error_count;
}

unsigned int ON_SubDLevel::DumpTopology(
  const unsigned int validate_max_vertex_id,
  const unsigned int validate_max_edge_id,
  const unsigned int validate_max_face_id,
  ON_2udex vertex_id_range,
  ON_2udex edge_id_range,
  ON_2udex face_id_range,
  ON_TextLog& text_log
) const
{
  // NOTE WELL:
  //  This is a debugging tool.
  //  The code in this function needs to work when the topology information is corrupt.
  const unsigned short level_index = (unsigned short)m_level_index;

  const unsigned int vertex_max_dump_count 
    = (vertex_id_range.i > 0 && vertex_id_range.i != ON_UNSET_UINT_INDEX && 0 == vertex_id_range.j)
    ? vertex_id_range.i 
    : 0;
  const unsigned int edge_max_dump_count 
    = (edge_id_range.i > 0 && edge_id_range.i != ON_UNSET_UINT_INDEX && 0 == edge_id_range.j)
    ? edge_id_range.i
    : 0;
  const unsigned int face_max_dump_count 
    = (face_id_range.i > 0 && face_id_range.i != ON_UNSET_UINT_INDEX && 0 == face_id_range.j)
    ? face_id_range.i 
    : 0;

  const bool bVertexIdTest = (vertex_max_dump_count > 0) || (vertex_id_range.i < vertex_id_range.j) || (ON_UNSET_UINT_INDEX == (vertex_id_range.i));
  const bool bEdgeIdTest =  (edge_max_dump_count > 0) || (edge_id_range.i < edge_id_range.j) || (ON_UNSET_UINT_INDEX == (edge_id_range.i));
  const bool bFaceIdTest =  (face_max_dump_count > 0) || (face_id_range.i < face_id_range.j) || (ON_UNSET_UINT_INDEX == (face_id_range.i));

  const char error_code_point = '!';
  char prefix[16] = {};

  unsigned int vertex_error_count = 0;
  unsigned int edge_error_count = 0;
  unsigned int face_error_count = 0;

  text_log.Print(L"SubD level %u topology: %u vertices, %u edges", m_level_index, m_vertex_count, m_edge_count);


  unsigned int wire_edge_count = 0U;
  unsigned int boundary_edge_count = 0U;  
  unsigned int interior_edge_count = 0U;
  unsigned int nonmanifold_edge_count = 0U;
  for (const ON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
  {
    if (0 == e->m_face_count)
      ++wire_edge_count;
    else if (1 == e->m_face_count)
      ++boundary_edge_count;
    else if (2 == e->m_face_count)
      ++interior_edge_count;
    else if (e->m_face_count >= 3)
      ++nonmanifold_edge_count;
  }

  if (wire_edge_count > 0U)
  {
    if (nonmanifold_edge_count > 0U)
    {
      if (boundary_edge_count > 0U && interior_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u interior, %u wire, %u nonmanifold)", boundary_edge_count, interior_edge_count, wire_edge_count, nonmanifold_edge_count);
      else if (boundary_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u wire, %u nonmanifold)", boundary_edge_count, wire_edge_count, nonmanifold_edge_count);
      else if (interior_edge_count > 0U)
        text_log.Print(L" (%u interior, %u wire, %u nonmanifold)", interior_edge_count, wire_edge_count, nonmanifold_edge_count);
      else
        text_log.Print(L" (%u wire, %u nonmanifold)", wire_edge_count, nonmanifold_edge_count);
    }
    else
    {
      if (boundary_edge_count > 0U && interior_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u interior, %u wire)", boundary_edge_count, interior_edge_count, wire_edge_count);
      else if (boundary_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u wire)", boundary_edge_count, wire_edge_count);
      else if (interior_edge_count > 0U)
        text_log.Print(L" (%u interior, %u wire)", interior_edge_count, wire_edge_count);
      else
        text_log.Print(L" (%u wire)", wire_edge_count);
    }
  }
  else if (nonmanifold_edge_count > 0U)
  {
    if (boundary_edge_count > 0U && interior_edge_count > 0U)
      text_log.Print(L" (%u boundary, %u interior, %u nonmanifold)", boundary_edge_count, interior_edge_count, nonmanifold_edge_count);
    else if (boundary_edge_count > 0U)
      text_log.Print(L" (%u boundary, %u nonmanifold)", boundary_edge_count, nonmanifold_edge_count);
    else if (interior_edge_count > 0U)
      text_log.Print(L" (%u interior, %u nonmanifold)", interior_edge_count, nonmanifold_edge_count);
    else
      text_log.Print(L" (%u nonmanifold)", nonmanifold_edge_count);
  }
  else if (boundary_edge_count > 0U && interior_edge_count > 0U)
  {
    text_log.Print(L" (%u boundary, %u interior)", boundary_edge_count, interior_edge_count);
  }

  text_log.Print(L", ");

  unsigned int ngon_count[65] = {};
  unsigned int maxN = (unsigned int)(sizeof(ngon_count) / sizeof(ngon_count[0])) - 1;

  unsigned int face_count = 0;
  unsigned int uniformN = 0;
  for (const ON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
  {
    if (face_count >= m_face_count && f->SubdivisionLevel() != level_index)
      break;
    face_count++;
    unsigned int N = f->EdgeCount();
    if (1 == face_count)
      uniformN = N;
    else if (N != uniformN)
      uniformN = 0;
    unsigned int j = (N < maxN) ? N : maxN;
    if (N < maxN)
      ngon_count[j]++;
  }

  if (face_count != m_face_count)
    face_error_count++;


  if (uniformN > 0 && face_count == m_face_count)
  {
    if (3 == uniformN)
      text_log.Print(L"%u triangles\n", m_face_count);
    else if (4 == uniformN)
      text_log.Print(L"%u quads\n", m_face_count);
    else
      text_log.Print(L"%u %u-gons\n", m_face_count, uniformN);
  }
  else
  {
    text_log.Print(L"%u faces\n", m_face_count );
    text_log.PushIndent();
    for (unsigned int N = 0; N <= maxN; N++)
    {
      if (0 == ngon_count[N])
        continue;
      if (3 == N)
        text_log.Print(L"%u triangles\n", ngon_count[N]);
      else if (4 == N)
        text_log.Print(L"%u quads\n", ngon_count[N]);
      else if (N < maxN)
        text_log.Print(L"%u %u-gons\n", ngon_count[N], N);
      else
        text_log.Print(L"%u N-gons\n", ngon_count[N]);
    }
    text_log.PopIndent();
  }

  if (IsEmpty())
    return 0;

  unsigned int damaged_vertex_count = 0;
  unsigned int damaged_edge_count = 0;
  unsigned int damaged_face_count = 0;
  enum : unsigned int
  {
    damaged_id_list_capacity = 8
  };
  for (;;)
  {
    unsigned int damaged_vertex_id[damaged_id_list_capacity] = {};
    unsigned int damaged_edge_id[damaged_id_list_capacity] = {};
    unsigned int damaged_face_id[damaged_id_list_capacity] = {};
    for (const ON_SubDVertex* v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
    {
      if (false == v->m_status.IsDamaged())
        continue;
      if (damaged_vertex_count < damaged_id_list_capacity)
        damaged_vertex_id[damaged_vertex_count] = v->m_id;
      ++damaged_vertex_count;
    }
    for (const ON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
    {
      if (false == e->m_status.IsDamaged())
        continue;
      if (damaged_edge_count < damaged_id_list_capacity)
        damaged_edge_id[damaged_edge_count] = e->m_id;
      ++damaged_edge_count;
    }
    for (const ON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
    {
      if (false == f->m_status.IsDamaged())
        continue;
      if (damaged_face_count < damaged_id_list_capacity)
        damaged_face_id[damaged_face_count] = f->m_id;
      ++damaged_face_count;
    }

    if (0U == damaged_vertex_count && 0U == damaged_edge_count && 0U == damaged_face_count)
      break;
    text_log.Print("DAMAGED SubD level:\n");
    ON_TextLogIndent indent1(text_log);
    if (damaged_vertex_count > 0)
    {
      text_log.Print(L"%u DAMAGED vertices: ", damaged_vertex_count);
      text_log.Print("v%u", damaged_vertex_id[0]);
      for (unsigned i = 1U; i < damaged_vertex_count; ++i)
        text_log.Print(", v%u", damaged_vertex_id[i]);
      if (damaged_vertex_count > damaged_id_list_capacity)
        text_log.Print(", ...");
      text_log.PrintNewLine();
    }
    if (damaged_edge_count > 0)
    {
      text_log.Print(L"%u DAMAGED edges: ", damaged_edge_count);
      text_log.Print("e%u", damaged_edge_id[0]);
      for (unsigned i = 1U; i < damaged_edge_count; ++i)
        text_log.Print(", e%u", damaged_edge_id[i]);
      if (damaged_edge_count > damaged_id_list_capacity)
        text_log.Print(", ...");
      text_log.PrintNewLine();
    }
    if (damaged_face_count > 0)
    {
      text_log.Print(L"%u DAMAGED faces: ", damaged_face_count);
      text_log.Print("f%u", damaged_face_id[0]);
      for (unsigned i = 1U; i < damaged_face_count; ++i)
        text_log.Print(", f%u", damaged_face_id[i]);
      if (damaged_face_count > damaged_id_list_capacity)
        text_log.Print(", ...");
      text_log.PrintNewLine();
    }
    break;
  }

  ///////////////////////////////////////////////////////////////////
  //
  // Vertex Topology
  // vN (x, y, z)
  //   vEdges[vertex_edge_count] = { +eA, -eB, ... }
  //   vFaces[vertex_edge_count] = { fP, fQ, fR, ... }
  //
  damaged_vertex_count = 0U;
  unsigned int vertex_count = 0;
  unsigned int vertex_dump_count = 0;
  ON_2udex skipped_vertex_id = ON_2udex::Zero;
  unsigned int max_vertex_id = 0;
  bool bSkippedPreviousComponent = false;
  for (const ON_SubDVertex* v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
  {
    if (vertex_count >= m_vertex_count && v->SubdivisionLevel() != level_index)
      break;
    if (v->m_id > max_vertex_id)
      max_vertex_id = v->m_id;

    vertex_count++;
    const bool bIsDamaged = v->m_status.IsDamaged();
    if (bIsDamaged)
      ++damaged_vertex_count;

    if (bVertexIdTest && (false == bIsDamaged || damaged_vertex_count > damaged_id_list_capacity))
    {
      bool bSkip = true;
      for (;;)
      {
        if (ON_UNSET_UINT_INDEX == vertex_id_range.i)
          break;
        if (vertex_max_dump_count > 0)
        {
          if (vertex_count > vertex_max_dump_count)
            break;
        }
        else
        {
          if (v->m_id < vertex_id_range.i || v->m_id >= vertex_id_range.j)
            break;
        }
        bSkip = false;
        break;
      }
      if (bSkip)
      {
        if (0 == skipped_vertex_id.j)
        {
          skipped_vertex_id.i = v->m_id;
          skipped_vertex_id.j = v->m_id;
        }
        else if (v->m_id < skipped_vertex_id.i)
          skipped_vertex_id.i = v->m_id;
        else if (v->m_id > skipped_vertex_id.j)
          skipped_vertex_id.j = v->m_id;
        bSkippedPreviousComponent = true;
        continue;
      }
    }

    if (0 == vertex_dump_count)
      text_log.Print(L"Vertices:\n");
    vertex_dump_count++;
    ON_TextLogIndent vindent(text_log);

    const ON_3dPoint P0(v->ControlNetPoint());

    ON_wString vtag;
    switch (v->m_vertex_tag)
    {
    case ON_SubD::VertexTag::Unset:
      vtag = L"Unset";
      break;
    case ON_SubD::VertexTag::Smooth:
      vtag = L"Smooth";
      break;
    case ON_SubD::VertexTag::Crease:
      vtag = L"Crease";
      break;
    case ON_SubD::VertexTag::Corner:
      vtag = L"Corner";
      break;
    case ON_SubD::VertexTag::Dart:
      vtag = L"Dart";
      break;
    default:
      vtag = L"INVALID";
      break;
    }

    if (bSkippedPreviousComponent)
    {
      text_log.Print(L"...\n");
      bSkippedPreviousComponent = false;
    }
    if (bIsDamaged)
    {
      text_log.Print(
        "v%u: (DAMAGED) %ls (%g, %g, %g)\n",
        v->m_id,
        static_cast<const wchar_t*>(vtag),
        P0.x, P0.y, P0.z
      );
    }
    else
    {
      text_log.Print(
        "v%u: %ls (%g, %g, %g)\n",
        v->m_id,
        static_cast<const wchar_t*>(vtag),
        P0.x, P0.y, P0.z
      );
    }

    text_log.PushIndent();

    ON_3dVector D(ON_3dVector::NanVector);
    if (v->GetSubdivisionDisplacement(&D.x) && D.IsValid())
      text_log.Print( "v.SubdivisionDisplacement: (%g, %g, %g)\n", D.x, D.y, D.z );

    ON_3dPoint P1(ON_3dPoint::NanPoint);
    if (v->GetSavedSubdivisionPoint(&P1.x) && P1.IsValid())
      text_log.Print( "v.SubdivisionPoint: (%g, %g, %g)\n", P1.x, P1.y, P1.z );

    ON_3dPoint S(ON_3dPoint::NanPoint);
    if (v->GetSavedSurfacePoint(&S.x) && S.IsValid())
      text_log.Print( "v.SurfacePoint: (%g, %g, %g)\n", S.x, S.y, S.z );

    const unsigned int vertex_edge_count = v->m_edge_count;
    text_log.Print("v.Edges[%u] = {", vertex_edge_count);
    prefix[0] = ON_String::Space;
    prefix[1] = error_code_point;
    prefix[2] = 'e';
    prefix[3] = 0;
    for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
    {
      prefix[1] = error_code_point;
      if (1 == vei)
      {
        prefix[0] = ',';
      }
      const ON_SubDEdge* e = v->Edge(vei);
      unsigned int eid = 0;
      if (nullptr != e)
      {
        if (v == e->m_vertex[0] && v != e->m_vertex[1])
          prefix[1] = '+';
        else if (v != e->m_vertex[0] && v == e->m_vertex[1])
          prefix[1] = '-';
        eid = e->m_id;
      }
      text_log.Print("%s%u", prefix, eid);
      if (error_code_point == prefix[1])
        vertex_error_count++;
    }
    text_log.Print(" }\n");

    const unsigned int vertex_face_count = v->m_face_count;
    text_log.Print("v.Faces[%u] = {", vertex_face_count);
    prefix[0] = ON_String::Space;
    prefix[1] = ON_String::Space;
    prefix[2] = 'f';
    prefix[3] = 0;
    for (unsigned int vfi = 0; vfi < vertex_face_count; vfi++)
    {
      prefix[1] = error_code_point;
      if (1 == vfi)
      {
        prefix[0] = ',';
      }
      const ON_SubDFace* f = v->Face(vfi);
      unsigned int fid = 0;
      if (nullptr != f)
      {
        if (f->VertexIndex(v) < ON_UNSET_UINT_INDEX)
          prefix[1] = ON_String::Space;
        fid = f->m_id;
      }
      text_log.Print("%s%u", prefix, fid);
      if (error_code_point == prefix[1])
        vertex_error_count++;
    }
    text_log.Print(" }\n");
    text_log.PopIndent();
  }

  text_log.PushIndent();
  if (vertex_dump_count > 0 && vertex_dump_count < vertex_count)
  {
    text_log.Print(
      L"... %u additional vertices (v%u to v%u).\n", 
      vertex_count-vertex_dump_count,
      skipped_vertex_id.i,
      skipped_vertex_id.j
    );
  }
  text_log.Print("Maximum vertex id = %u.  ",max_vertex_id);
  if (validate_max_vertex_id >= max_vertex_id)
    text_log.Print("Next id = %u.\n", validate_max_vertex_id + 1);
  else
    text_log.Print("ERROR Next id = %u.\n", validate_max_vertex_id + 1);
  text_log.PopIndent();

  ///////////////////////////////////////////////////////////////////
  //
  // Edge Topology
  // eN (+vA, -vB)
  //   eFaces[edge_face_count] = { fP, fQ, fR, ... }
  //
  damaged_edge_count = 0U;
  wire_edge_count = 0U;
  nonmanifold_edge_count = 0U;
  unsigned int edge_count = 0;
  unsigned int edge_dump_count = 0;
  ON_2udex skipped_edge_id = ON_2udex::Zero;
  unsigned int max_edge_id = 0;
  bSkippedPreviousComponent = false;
  for (const ON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
  {
    if (edge_count >= m_edge_count && e->SubdivisionLevel() != level_index)
      break;
    if (e->m_id > max_edge_id)
      max_edge_id = e->m_id;
    edge_count++;

    const bool bIsDamaged = e->m_status.IsDamaged();
    if (bIsDamaged)
      ++damaged_edge_count;
    const bool bIsWireEdge = (0U == e->m_face_count);
    if (bIsWireEdge)
      ++wire_edge_count;
    const bool bIsNonmanifoldEdge = (e->m_face_count >= 3U);
    if (bIsNonmanifoldEdge)
      ++nonmanifold_edge_count;

    if (
      bEdgeIdTest 
      && (false == bIsDamaged || damaged_edge_count > damaged_id_list_capacity)
      && (false == bIsWireEdge || wire_edge_count > damaged_id_list_capacity)
      && (false == bIsNonmanifoldEdge || nonmanifold_edge_count > damaged_id_list_capacity)
      )
    {
      bool bSkip = true;
      for (;;)
      {
        if (ON_UNSET_UINT_INDEX == edge_id_range.i)
          break;
        if (edge_max_dump_count > 0)
        {
          if (edge_count > edge_max_dump_count)
            break;
        }
        else
        {
          if (e->m_id < edge_id_range.i || e->m_id >= edge_id_range.j)
            break;
        }
        bSkip = false;
        break;
      }
      if (bSkip)
      {
        if (0 == skipped_edge_id.j)
        {
          skipped_edge_id.i = e->m_id;
          skipped_edge_id.j = e->m_id;
        }
        else if (e->m_id < skipped_edge_id.i)
          skipped_edge_id.i = e->m_id;
        else if (e->m_id > skipped_edge_id.j)
          skipped_edge_id.j = e->m_id;
        bSkippedPreviousComponent = true;
        continue;
      }
    }

    if (0 == edge_dump_count)
      text_log.Print(L"Edges:\n");
    edge_dump_count++;
    ON_TextLogIndent eindent(text_log);

    ON_wString etag;
    switch (e->m_edge_tag)
    {
    case ON_SubD::EdgeTag::Unset:
      etag = L"Unset";
      break;
    case ON_SubD::EdgeTag::Smooth:
      etag = L"Smooth";
      break;
    case ON_SubD::EdgeTag::Crease:
      etag = L"Crease";
      break;
    case ON_SubD::EdgeTag::SmoothX:
      etag = L"SmmothX";
      break;
    default:
      etag = L"INVALID";
      break;
    }

    if (bSkippedPreviousComponent)
    {
      text_log.Print(L"...\n");
      bSkippedPreviousComponent = false;
    }
    if (bIsDamaged)
    {
      if (bIsWireEdge)
      {
        text_log.Print(
          "e%u: (DAMAGED) %ls wire (",
          e->m_id,
          static_cast<const wchar_t*>(etag)
        );
      }
      else if (bIsNonmanifoldEdge)
      {
        text_log.Print(
          "e%u: (DAMAGED) %ls nonmanifold (",
          e->m_id,
          static_cast<const wchar_t*>(etag)
        );
      }
      else
      {
        text_log.Print(
          "e%u: (DAMAGED) %ls (",
          e->m_id,
          static_cast<const wchar_t*>(etag)
        );
      }
    }
    else
    {
      if (bIsWireEdge)
      {
        text_log.Print(
          "e%u: wire %ls (",
          e->m_id,
          static_cast<const wchar_t*>(etag)
        );
      }
      else if (bIsNonmanifoldEdge)
      {
        text_log.Print(
          "e%u: nonmanifold %ls (",
          e->m_id,
          static_cast<const wchar_t*>(etag)
        );
      }
      else
      {
        text_log.Print(
          "e%u: %ls (",
          e->m_id,
          static_cast<const wchar_t*>(etag)
        );
      }
    }

    prefix[0] = ON_String::Space;
    prefix[1] = error_code_point;
    prefix[2] = 'v';
    prefix[3] = 0;
    for (unsigned int evi = 0; evi < 2; evi++)
    {
      if (1 == evi)
        text_log.Print(" to");
      prefix[1] = error_code_point;
      const ON_SubDVertex* v = e->m_vertex[evi];
      unsigned int vid = 0;
      if (nullptr != v)
      {
        vid = v->m_id;
        if (v->EdgeArrayIndex(e) < ON_UNSET_INT_INDEX)
          prefix[1] = ON_String::Space;
      }
      if (error_code_point == prefix[1])
        edge_error_count++;
      text_log.Print("%s%u", (0==evi)?(prefix+1):(prefix), vid);
    }
    text_log.Print(")\n");

    text_log.PushIndent();

    ON_3dVector D(ON_3dVector::NanVector);
    if (e->GetSubdivisionDisplacement(&D.x) && D.IsValid())
      text_log.Print( "e.SubdivisionDisplacement: (%g, %g, %g)\n", D.x, D.y, D.z );

    ON_3dPoint P1(ON_3dPoint::NanPoint);
    if (e->GetSavedSubdivisionPoint(&P1.x) && P1.IsValid())
      text_log.Print( "e.SubdivisionPoint: (%g, %g, %g)\n", P1.x, P1.y, P1.z );

    const unsigned int edge_face_count = e->m_face_count;
    text_log.Print("e.Faces[%u] = {", edge_face_count);
    prefix[0] = ON_String::Space;
    prefix[1] = error_code_point;
    prefix[2] = 'f';
    prefix[3] = 0;
    for (unsigned int efi = 0; efi < edge_face_count; efi++)
    {
      prefix[1] = error_code_point;
      if (1 == efi)
      {
        prefix[0] = ',';
      }
      ON_SubDFacePtr fptr = e->FacePtr(efi);
      const ON_SubDFace* f = fptr.Face();
      const ON__UINT_PTR edge_fdir = fptr.FaceDirection();
      unsigned int fid = 0;
      if (nullptr != f)
      {
        fid = f->m_id;
        ON_SubDEdgePtr eptr = f->EdgePtrFromEdge(e);
        if (eptr.Edge() == e && eptr.EdgeDirection() == edge_fdir)
        {
          prefix[1] = (0 == edge_fdir) ? '+' : '-';
        }
      }
      if (error_code_point == prefix[1])
        edge_error_count++;
      text_log.Print("%s%u", prefix, fid);
    }
    text_log.Print(" }\n");
    text_log.PopIndent();
  }

  text_log.PushIndent();
  if (edge_dump_count > 0 && edge_dump_count < edge_count)
  {
    text_log.Print(L"... %u additional edges (e%u to e%u).\n",
      edge_count - edge_dump_count,
      skipped_edge_id.i,
      skipped_edge_id.j
    );
  }
  text_log.Print("Maximum edge id = %u.  ",max_edge_id);
  if (validate_max_edge_id >= max_edge_id)
    text_log.Print("Next id = %u.\n", validate_max_edge_id + 1);
  else
    text_log.Print("ERROR Next id = %u.\n", validate_max_edge_id + 1);
  text_log.PopIndent();


  ///////////////////////////////////////////////////////////////////
  //
  // Face Topology
  // fN
  //   fEdges[face_edge_count] = { +eA, -eB, +eC, ...}
  //   fVertices[face_edge_count] = { vP, vQ, vR, ... }
  //
  damaged_face_count = 0U;
  face_count = 0;
  unsigned int face_dump_count = 0;
  ON_2udex skipped_face_id = ON_2udex::Zero;
  unsigned int max_face_id = 0;
  bSkippedPreviousComponent = false;
  for (const ON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
  {
    if (face_count >= m_face_count && f->SubdivisionLevel() != level_index)
      break;
    if (f->m_id > max_face_id)
      max_face_id = f->m_id;
    face_count++;

    const bool bIsDamaged = f->m_status.IsDamaged();
    if (bIsDamaged)
      ++damaged_face_count;

    if (bFaceIdTest && (false == bIsDamaged || damaged_face_count > damaged_id_list_capacity))
    {
      bool bSkip = true;
      for (;;)
      {
        if (ON_UNSET_UINT_INDEX == face_id_range.i)
          break;
        if (face_max_dump_count > 0)
        {
          if (face_count > face_max_dump_count)
            break;
        }
        else
        {
          if (f->m_id < face_id_range.i || f->m_id >= face_id_range.j)
            break;
        }
        bSkip = false;
        break;
      }
      if (bSkip)
      {
        if (0 == skipped_face_id.j)
        {
          skipped_face_id.i = f->m_id;
          skipped_face_id.j = f->m_id;
        }
        else if (f->m_id < skipped_face_id.i)
          skipped_face_id.i = f->m_id;
        else if (f->m_id > skipped_face_id.j)
          skipped_face_id.j = f->m_id;
        bSkippedPreviousComponent = true;
        continue;
      }
    }

    if (0 == face_dump_count)
      text_log.Print(L"Faces:\n");
    face_dump_count++;
    ON_TextLogIndent eindent(text_log);

    if (bSkippedPreviousComponent)
    {
      text_log.Print(L"...\n");
      bSkippedPreviousComponent = false;
    }
    if (bIsDamaged)
    {
      text_log.Print(
        "f%u (DAMAGED):\n",
        f->m_id
      );
    }
    else
    {
      text_log.Print(
        "f%u:\n",
        f->m_id
      );
    }

    text_log.PushIndent();

    ON_3dVector D(ON_3dVector::NanVector);
    if (f->GetSubdivisionDisplacement(&D.x) && D.IsValid())
      text_log.Print( "f.SubdivisionDisplacement: (%g, %g, %g)\n", D.x, D.y, D.z );

    ON_3dPoint P1(ON_3dPoint::NanPoint);
    if (f->GetSavedSubdivisionPoint(&P1.x) && P1.IsValid())
      text_log.Print( "f.SubdivisionPoint: (%g, %g, %g)\n", P1.x, P1.y, P1.z );


    const unsigned int face_edge_count = f->m_edge_count;
    text_log.Print("f.Edges[%u] = {", face_edge_count);
    prefix[0] = ON_String::Space;
    prefix[1] = error_code_point;
    prefix[2] = 'e';
    prefix[3] = 0;
    for (unsigned int fei = 0; fei < face_edge_count; fei++)
    {
      prefix[1] = error_code_point;
      if (1 == fei)
      {
        prefix[0] = ',';
      }
      const ON_SubDEdgePtr eptr = f->EdgePtr(fei);
      const ON_SubDEdge* e = eptr.Edge();
      const ON__UINT_PTR face_edir = eptr.EdgeDirection();
      unsigned int eid = 0;
      if (nullptr != e)
      {
        eid = e->m_id;
        ON_SubDFacePtr fptr = e->FacePtrFromFace(f);
        if (fptr.Face() == f && fptr.FaceDirection() == face_edir)
        {
          prefix[1] = (0 == face_edir) ? '+' : '-';
        }
      }
      if (error_code_point == prefix[1])
        face_error_count++;
      text_log.Print("%s%u", prefix, eid);
    }
    text_log.Print(" }\n");

    const ON_SubDEdgePtr last_eptr = f->EdgePtr(face_edge_count - 1);
    const ON_SubDEdge* last_edge = last_eptr.Edge();
    const ON_SubDVertex* v1
      = (nullptr != last_edge)
      ? last_edge->m_vertex[0 == last_eptr.EdgeDirection() ? 1 : 0]
      : nullptr;

    text_log.Print("f.Vertices[%u] = {", face_edge_count);
    prefix[0] = ON_String::Space;
    prefix[1] = error_code_point;
    prefix[2] = 'v';
    prefix[3] = 0;
    for (unsigned int fei = 0; fei < face_edge_count; fei++)
    {
      prefix[1] = error_code_point;
      if (1 == fei)
      {
        prefix[0] = ',';
      }
      const ON_SubDEdgePtr eptr = f->EdgePtr(fei);
      const ON_SubDEdge* e = eptr.Edge();
      const ON__UINT_PTR face_edir = eptr.EdgeDirection();
      unsigned int vid = 0;
      if (nullptr == e)
      {
        v1 = nullptr;
      }
      else
      {
        const ON_SubDVertex* v0 = e->m_vertex[0 == face_edir ? 0 : 1];
        if (nullptr != v0)
        {
          vid = v0->m_id;
          if (v1 == v0)
            prefix[1] = ON_String::Space;
        }
        v1 = e->m_vertex[0 == face_edir ? 1 : 0];
      }
      if (error_code_point == prefix[1])
        face_error_count++;
      text_log.Print("%s%u", prefix, vid);
    }
    text_log.Print(" }\n");

    if (f->TextureDomainIsSet())
    {
      const ON_wString s = ON_SubD::TextureDomainTypeToString(f->TextureDomainType());
      const bool bGridOrder = true;
      const bool bNormalized = false;
      ON_2dPoint corners[4] = {
        f->TextureDomainCorner(bGridOrder,bNormalized,0),
        f->TextureDomainCorner(bGridOrder,bNormalized,1),
        f->TextureDomainCorner(bGridOrder,bNormalized,2),
        f->TextureDomainCorner(bGridOrder,bNormalized,3)
      };
      text_log.Print("%ls texture domain. Grid corners: (%g,%g), (%g,%g), (%g,%g), (%g,%g)\n",
        static_cast<const wchar_t*>(s),
        corners[0].x, corners[0].y,
        corners[1].x, corners[1].y,
        corners[2].x, corners[2].y,
        corners[3].x, corners[3].y
      );
      ////const ON_SubDMeshFragment *fragments = f->MeshFragments();
      ////if (nullptr != fragments)
      ////{
      ////ON_TextLogIndent indent1(text_log);
      ////  unsigned short fragment_count = 0;
      ////  const unsigned short expected_fragment_count = (unsigned short)((4 == face_edge_count) ? 1 : face_edge_count);
      ////  ON_3dPoint fragmentT[4];
      ////  bool bDamagedOrIncompleteFragments = false;
      ////  for (unsigned short fragdex = 0; fragdex <= expected_fragment_count && nullptr != fragments; fragdex++, fragments = fragments->m_next_fragment)
      ////  {
      ////    ++fragment_count;
      ////    if (expected_fragment_count != fragments->m_face_fragment_count)
      ////    {
      ////      bDamagedOrIncompleteFragments = true;
      ////      break;
      ////    }
      ////    if (fragdex != fragments->m_face_fragment_index)
      ////    {
      ////      bDamagedOrIncompleteFragments = true;
      ////      break;
      ////    }
      ////  }
      ////  if (expected_fragment_count == fragment_count)
      ////  {
      ////    fragments = f->MeshFragments();
      ////    if (1 == fragment_count)
      ////    {
      ////      if (fragments->GetTextureCoordinteCorners(bGridOrder, fragmentT))
      ////      {
      ////        text_log.Print("Quad face fragment texture corners = (%g,%g,%g), (%g,%g,%g), (%g,%g,%g), (%g,%g,%g)\n",
      ////          fragmentT[0].x, fragmentT[0].y, fragmentT[0].z,
      ////          fragmentT[1].x, fragmentT[1].y, fragmentT[1].z,
      ////          fragmentT[2].x, fragmentT[2].y, fragmentT[2].z,
      ////          fragmentT[3].x, fragmentT[3].y, fragmentT[3].z
      ////        );
      ////      }
      ////      else
      ////      {
      ////        text_log.Print("Quad face fragment texture corners not available.\n");
      ////      }
      ////    }
      ////    else
      ////    {
      ////      text_log.Print("%u face fragments.\n", (unsigned)expected_fragment_count);
      ////      ON_TextLogIndent indent2(text_log);
      ////      for (unsigned short fragdex = 0; fragdex <= expected_fragment_count && nullptr != fragments; fragdex++, fragments = fragments->m_next_fragment)
      ////      {
      ////        if (fragments->GetTextureCoordinteCorners(bGridOrder, fragmentT))
      ////        {
      ////          text_log.Print("fragment[%u] texture corners = (%g,%g,%g), (%g,%g,%g), (%g,%g,%g), (%g,%g,%g)\n",
      ////            (unsigned)fragdex,
      ////            fragmentT[0].x, fragmentT[0].y, fragmentT[0].z,
      ////            fragmentT[1].x, fragmentT[1].y, fragmentT[1].z,
      ////            fragmentT[2].x, fragmentT[2].y, fragmentT[2].z,
      ////            fragmentT[3].x, fragmentT[3].y, fragmentT[3].z
      ////          );
      ////        }
      ////        else
      ////        {
      ////          text_log.Print("fragment[%u] texture corners not available.\n", (unsigned)fragdex);
      ////        }
      ////      }
      ////    }
      ////  }
      ////  else if (nullptr == fragments)
      ////  {
      ////    bDamagedOrIncompleteFragments = true;
      ////  }
      ////  if (bDamagedOrIncompleteFragments )
      ////    text_log.Print("Damaged or incomplete mesh fragments.\n");
      ////}
    }

    text_log.PopIndent();
  }

  text_log.PushIndent();
  if (face_dump_count > 0 && face_dump_count < face_count)
  {
    text_log.Print(
      L"... %u additional faces (f%u to f%u).\n",
      face_count - face_dump_count,
      skipped_face_id.i,
      skipped_face_id.j
    );
  }
  text_log.Print("Maximum face id = %u.  ",max_face_id);
  if (validate_max_face_id >= max_face_id)
    text_log.Print("Next id = %u.\n", validate_max_face_id + 1);
  else
    text_log.Print("ERROR Next id = %u.\n", validate_max_face_id + 1);
  text_log.PopIndent();

 
  const unsigned int topology_error_count
    = vertex_error_count
    + edge_error_count
    + face_error_count;

  text_log.PushIndent();
  if (0 == topology_error_count)
  {
    text_log.Print("No topology inconsistencies.\n");
  }
  else
  {
    text_log.Print(
      "ERRORS: %u vertex, %u edge, %u face topology inconsistencies marked with \"%c\".\n", 
      vertex_error_count,
      edge_error_count,
      face_error_count, 
      error_code_point
    );
  }
  text_log.PopIndent();

  return topology_error_count;
}


//virtual
unsigned int ON_SubD::SizeOf() const
{
  size_t sz = ON_Geometry::SizeOf();
  sz += sizeof(*this) - sizeof(ON_Geometry);
  const ON_SubDimple* subdimple = SubDimple();
  if ( subdimple )
    sz += subdimple->SizeOf();
  return (unsigned int)sz;
}

//virtual
ON__UINT32 ON_SubD::DataCRC(ON__UINT32 current_remainder) const
{
  return 0;
}

//virtual
ON::object_type ON_SubD::ObjectType() const
{
  return ON::subd_object;
}

//virtual
void ON_SubD::DestroyRuntimeCache( bool bDelete )
{
  const ON_SubDimple* dimple = SubDimple();
  if (nullptr != dimple)
  {
    unsigned int level_count = dimple->LevelCount();
    for (unsigned int level_index = 0; level_index < level_count; level_index++)
    {
      const ON_SubDLevel* level = dimple->SubDLevel(level_index);
      if (level)
      {
        level->ClearEvaluationCache();
        level->AggregateComponentStatus().MarkAsNotCurrent();
      }
    }
    dimple->ChangeContentSerialNumber();
  }
  return;
}

//virtual 
int ON_SubD::Dimension() const
{
  return 3;
}

//virtual
bool ON_SubD::GetBBox(
        double* boxmin,
        double* boxmax,
        bool bGrowBox
        ) const
{ 
  int j;
  for ( j = 0; j < 3 && bGrowBox; j++ )
  {
    if ( !ON_IsValid(boxmin[j]) || !ON_IsValid(boxmax[j]) || boxmin[j] > boxmax[j])
      bGrowBox = false;
  }

  ON_BoundingBox bbox = ON_BoundingBox::EmptyBoundingBox;
  bool rc = false;

  // GetBBox must always returns the control net box - it contains both the surface and the control net.
  bbox = ActiveLevel().ControlNetBoundingBox();
  rc = bbox.IsValid();
  if (rc)
  {
    if (bGrowBox)
    {
      if (bbox.m_min.x < boxmin[0]) boxmin[0] = bbox.m_min.x;
      if (bbox.m_max.x > boxmax[0]) boxmax[0] = bbox.m_max.x;
      if (bbox.m_min.y < boxmin[1]) boxmin[1] = bbox.m_min.y;
      if (bbox.m_max.y > boxmax[1]) boxmax[1] = bbox.m_max.y;
      if (bbox.m_min.z < boxmin[2]) boxmin[2] = bbox.m_min.z;
      if (bbox.m_max.z > boxmax[2]) boxmax[2] = bbox.m_max.z;
    }
    else
    {
      boxmin[0] = bbox.m_min.x; boxmin[1] = bbox.m_min.y; boxmin[2] = bbox.m_min.z;
      boxmax[0] = bbox.m_max.x; boxmax[1] = bbox.m_max.y; boxmax[2] = bbox.m_max.z;
    }
  }

  return (rc || bGrowBox);
}

//virtual
void ON_SubD::ClearBoundingBox()
{
  // For ON_SubD, ON_SubD::ClearBoundingBox() and ON_SubD::DestroyRuntimeCache(true)

  ON_SubD::DestroyRuntimeCache(true);
}

//virtual
bool ON_SubD::Transform( 
        const ON_Xform& xform
        )
{
  if ( this == &ON_SubD::Empty)
    return true;  // transform applied to empty subd is true on purpose

  // userdata transformation, etc.
  if (!this->ON_Geometry::Transform(xform))
    return false;

  ON_SubDimple* subdimple = SubDimple(false);
  if ( 0 == subdimple )
    return true;  // transform applied to empty subd is true on purpose

  return subdimple->Transform(xform);
}

//virtual
bool ON_SubD::IsDeformable() const
{
  return true;
}

//virtual
bool ON_SubD::MakeDeformable()
{
  return true;
}

//virtual
bool ON_SubD::SwapCoordinates(
      int i,
      int j
      )
{
  return false;
}

//virtual
bool ON_SubD::HasBrepForm() const
{
  return false;
}

//virtual
ON_Brep* ON_SubD::BrepForm( ON_Brep* destination_brep) const
{
  return nullptr;
}

//virtual
ON_COMPONENT_INDEX ON_SubD::ComponentIndex() const
{
  return ON_Geometry::ComponentIndex();
}

//virtual
bool ON_SubD::EvaluatePoint( const class ON_ObjRef& objref, ON_3dPoint& P ) const
{
  return false;
}

//////////////////////////////////////////////////////////////
//
//
//

const class ON_SubDLevel& ON_SubD::ActiveLevel() const
{
  ON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ActiveLevel() : ON_SubDLevel::Empty;
}

class ON_SubDLevel const * ON_SubD::ActiveLevelConstPointer() const
{
  ON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ActiveLevelPointer() : nullptr;
}

class ON_SubDLevel* ON_SubD::ActiveLevelPointer()
{
  ON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ActiveLevelPointer() : nullptr;
}


ON_SubDimple* ON_SubD::SubDimple(bool bCreateIfNeeded)
{
  ON_SubDimple* subdimple = m_subdimple_sp.get();
  if (nullptr == subdimple && bCreateIfNeeded)
  {
    subdimple = new ON_SubDimple();
    m_subdimple_sp = std::shared_ptr<ON_SubDimple>(subdimple);
  }
  return subdimple;
}

const class ON_SubDimple* ON_SubD::SubDimple() const
{
  return m_subdimple_sp.get();
}

unsigned int ON_SubD::SubDimpleUseCount() const
{
  return (unsigned int)m_subdimple_sp.use_count();
}

void ON_SubD::ShareDimple(const ON_SubD& other_subd)
{
  if (m_subdimple_sp.get() != other_subd.m_subdimple_sp.get())
  {
    m_subdimple_sp.reset();
    m_subdimple_sp = other_subd.m_subdimple_sp;
  }
}

void ON_SubD::ShareDimple(const class ON_SubDMeshImpl& subd_limple)
{
  std::shared_ptr<ON_SubDimple> limple_sp(subd_limple.m_subdimple_wp.lock());

  const ON_SubDimple* subd_imple = m_subdimple_sp.get();

  if (nullptr == limple_sp.get())
  {
    // weak pointer is nullptr, meaning there are no known references to the
    // subd used to create this limit mesh.
    const_cast<ON_SubDMeshImpl&>(subd_limple).ClearFragmentFacePointers(true);
  }

  if (subd_imple != limple_sp.get())
  {
    m_subdimple_sp.reset();
    m_subdimple_sp = limple_sp;
  }
}

void ON_SubD::SwapDimple(class ON_SubDMeshImpl& subd_limple )
{
  std::shared_ptr <ON_SubDimple> limple_sp(subd_limple.m_subdimple_wp.lock());
  if (m_subdimple_sp.get() != limple_sp.get())
  {
    m_subdimple_sp.swap(limple_sp);
    subd_limple.m_subdimple_wp = limple_sp;
  }
}


void ON_SubD::SwapDimple(ON_SubD& other_subd)
{
  if (this != &other_subd)
  {
    m_subdimple_sp.swap(other_subd.m_subdimple_sp);
  }
}

void ON_SubD::Clear()
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( subdimple )
    subdimple->Clear();
}

void ON_SubD::ClearHigherSubdivisionLevels(
  unsigned int max_level_index
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( subdimple )
    subdimple->ClearHigherSubdivisionLevels(max_level_index);
}

void ON_SubD::ClearLowerSubdivisionLevels(
  unsigned int min_level_index
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( subdimple )
    subdimple->ClearLowerSubdivisionLevels(min_level_index);
}

void ON_SubD::Destroy()
{
  m_subdimple_sp.reset();
}

class ON_SubDVertex* ON_SubD::AddVertex(
  const double* P
)
{
  return AddVertex(ON_SubD::VertexTag::Unset, P);
}

class ON_SubDVertex* ON_SubD::AddVertex(
  ON_SubD::VertexTag vertex_tag,
  const double* P
  )
{
  ON_SubDimple* subdimple = SubDimple(true);
  if ( 0 == subdimple )
    return 0;
  const unsigned int level_index = subdimple->ActiveLevelIndex();
  class ON_SubDVertex* v = subdimple->AllocateVertex(vertex_tag, level_index, P); // 0 = level
  subdimple->AddVertexToLevel(v);
  return v;
}


class ON_SubDEdge* ON_SubDimple::AddEdge(
  ON_SubD::EdgeTag edge_tag, 
  ON_SubDVertex* v0,
  double v0_sector_weight,
  ON_SubDVertex* v1,
  double v1_sector_weight
  )
{
  if ( false == ON_SubDSectorType::IsValidSectorCoefficientValue(v0_sector_weight,true) )
    return ON_SUBD_RETURN_ERROR(nullptr);

  if ( false == ON_SubDSectorType::IsValidSectorCoefficientValue(v1_sector_weight,true) )
    return ON_SUBD_RETURN_ERROR(nullptr);    

  if ( nullptr != v0 && nullptr != v1 && v0->SubdivisionLevel() != v1->SubdivisionLevel() )
    return ON_SUBD_RETURN_ERROR(nullptr);    

  const bool bEdgeTagSet = ON_SubD::EdgeTagIsSet(edge_tag);

  if ( bEdgeTagSet
    && ON_SubDSectorType::IgnoredSectorCoefficient != v0_sector_weight 
    && ON_SubDSectorType::UnsetSectorCoefficient != v0_sector_weight
    && nullptr != v0
    && ON_SubD::VertexTag::Smooth == v0->m_vertex_tag
    )
  {
    // minimizes checking when building subds because constant crease weights can be passed in
    v0_sector_weight = ON_SubDSectorType::IgnoredSectorCoefficient;
  }

  if ( bEdgeTagSet
    && ON_SubDSectorType::IgnoredSectorCoefficient != v1_sector_weight
    && ON_SubDSectorType::UnsetSectorCoefficient != v1_sector_weight
    && nullptr != v1
    && ON_SubD::VertexTag::Smooth == v1->m_vertex_tag
    )
  {
    // minimizes checking when building subds because constant crease weights can be passed in
    v1_sector_weight = ON_SubDSectorType::IgnoredSectorCoefficient;
  }

  class ON_SubDEdge* e = AllocateEdge(edge_tag);
  if ( nullptr == e)
    return ON_SUBD_RETURN_ERROR(nullptr);

  if ( nullptr != v0 )
    e->SetSubdivisionLevel(v0->SubdivisionLevel());
  else if ( nullptr != v1 )
    e->SetSubdivisionLevel(v1->SubdivisionLevel());
  else if ( ActiveLevelIndex() < ON_UNSET_UINT_INDEX )
    e->SetSubdivisionLevel(ActiveLevelIndex());
  
  for (unsigned int i = 0; i < 2; i++)
  {
    ON_SubDVertex* v = (i ? v1 : v0);
    double vertex_weight = (i ? v1_sector_weight : v0_sector_weight);
    e->m_vertex[i] = v;
    e->m_sector_coefficient[i] = vertex_weight;
    if (nullptr != v)
    {
      if (false == m_heap.GrowVertexEdgeArrayByOne(v))
      {
        v->m_status.SetDamagedState(true);
        ReturnEdge(e);
        return ON_SUBD_RETURN_ERROR(nullptr);
      }
      v->m_edges[v->m_edge_count++] = ON_SubDEdgePtr::Create(e, i);
    }
  }

  if ( nullptr == AddEdgeToLevel(e) )
    return ON_SUBD_RETURN_ERROR(nullptr);

  return e;
}

ON_SubD::EdgeTag ON_SubD::EdgeTagFromContext(
  unsigned int edge_face_count,
  const ON_SubDVertex* v0,
  const ON_SubDVertex* v1
)
{
  return
    (nullptr != v0 && nullptr != v1)
    ? ON_SubD::EdgeTagFromContext(edge_face_count, v0->m_vertex_tag, v1->m_vertex_tag)
    : ON_SubD::EdgeTag::Unset;
}

ON_SubD::EdgeTag ON_SubD::EdgeTagFromContext(
  unsigned int edge_face_count,
  const ON_SubD::VertexTag v0_tag,
  const ON_SubD::VertexTag v1_tag
)
{
  ON_SubD::EdgeTag edge_tag = ON_SubD::EdgeTag::Unset;

  for(;;)
  {
    if (edge_face_count > 0x7FFFU)
      break;

    if (1 == edge_face_count || edge_face_count >= 3 )
    {
      edge_tag = ON_SubD::EdgeTag::Crease;
      break;
    }

    const bool bSmooth0 = ON_SubD::VertexTag::Smooth == v0_tag;
    const bool bSmooth1 = ON_SubD::VertexTag::Smooth == v1_tag;

    if ( bSmooth0 || bSmooth1 )
    {
      if ( 2 == edge_face_count && bSmooth0 && bSmooth1)
        edge_tag = ON_SubD::EdgeTag::Smooth;
      break;
    }

    if ( ON_SubD::VertexTagIsSet(v0_tag) && ON_SubD::VertexTagIsSet(v1_tag) )
    {
      if (2 == edge_face_count)
        edge_tag = ON_SubD::EdgeTag::SmoothX;
      break;
    }

    break;
  }

  return edge_tag;
}

const ON_SubDVertex* ON_SubD::FindVertex(
  const double* control_net_point,
  double distance_tolerance
) const
{
  if (nullptr == control_net_point )
    return nullptr;
  const ON_3dPoint P(control_net_point);
  if ( false == P.IsValid() )
    return nullptr;
  if ( false == (0.0 <= distance_tolerance))
    return nullptr;

  const ON_SubDVertex* best_v = nullptr;
  ON_SubDVertexIterator vit(*this);
  for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
  {
    const double d = P.DistanceTo(v->ControlNetPoint());
    if (0.0 == d)
      return v;
    if (d < distance_tolerance && (nullptr == best_v || d <= distance_tolerance) )
    {
      distance_tolerance = d;
      best_v = v;
    }
  }
  return best_v;
}

const ON_SubDVertex* ON_SubD::FindOrAddVertex(
  const double* control_net_point,
  double distance_tolerance
)
{
  if (nullptr == control_net_point)
    return nullptr;
  const ON_3dPoint P(control_net_point);
  if (false == P.IsValid())
    return nullptr;
  if (false == (0.0 <= distance_tolerance))
    return nullptr;
  const ON_SubDVertex* v = FindVertex(&P.x, distance_tolerance);
  return (nullptr != v) ? v : AddVertex(&P.x);
}

const ON_SubDEdgePtr ON_SubD::FindEdge(
  const class ON_SubDVertex* v0,
  const class ON_SubDVertex* v1
) const
{
  return ON_SubDEdge::FromVertices(v0, v1);
}

const ON_SubDEdgePtr ON_SubD::FindOrAddEdge(
  class ON_SubDVertex* v0,
  class ON_SubDVertex* v1
)
{
  ON_SubDEdgePtr eptr = ON_SubDEdge::FromVertices(v0, v1);
  if (nullptr == eptr.Edge())
    eptr = ON_SubDEdgePtr::Create(AddEdge(v0, v1), 0);
  return eptr;
}

class ON_SubDEdge* ON_SubD::AddEdge(
  ON_SubDVertex* v0,
  ON_SubDVertex* v1
)
{
  return ON_SubD::AddEdge(ON_SubD::EdgeTag::Unset, v0, v1);
}

class ON_SubDEdge* ON_SubD::AddEdge(
  ON_SubD::EdgeTag edge_tag,
  ON_SubDVertex* v0,
  ON_SubDVertex* v1
  )
{
  // NO automatic edge tag setting - causes more problems than it helps.
  // Users can call ON_SubD::EdgeTagFromContext() if they want to
  // preset the edge tag based on context.

  return AddEdgeWithSectorCoefficients(
    edge_tag,
    v0,
    ON_SubDSectorType::UnsetSectorCoefficient,
    v1,
    ON_SubDSectorType::UnsetSectorCoefficient
    );
}

ON_SubDEdge* ON_SubD::AddEdgeWithSectorCoefficients(
    ON_SubD::EdgeTag edge_tag,
    class ON_SubDVertex* v0,
    double v0_sector_coefficient,
    class ON_SubDVertex* v1,
    double v1_sector_coefficient
    )
{
  ON_SubDimple* subdimple = SubDimple(true);
  if (nullptr != subdimple) 
    return subdimple->AddEdge(edge_tag, v0, v0_sector_coefficient, v1, v1_sector_coefficient);
  return ON_SUBD_RETURN_ERROR(nullptr);
}


class ON_SubDFace* ON_SubDimple::AddFace(
  unsigned int edge_count,
  const ON_SubDEdgePtr* edge
)
{
  return AddFace(nullptr, edge_count, edge);
}

class ON_SubDFace* ON_SubDimple::AddFace(
  const ON_SubDFace* candidate_face,
  unsigned int edge_count,
  const ON_SubDEdgePtr* edge
  )
{
  if ( edge_count > 0 && nullptr == edge)
    return ON_SUBD_RETURN_ERROR(nullptr);

  unsigned f_level = 0;
  bool bHaveLevel = false;
  for (unsigned int i = 0; i < edge_count; i++)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(edge[i].m_ptr);
    if (nullptr == e)
      continue;
    if (bHaveLevel)
    {
      if (e->SubdivisionLevel() != f_level)
        return ON_SUBD_RETURN_ERROR(nullptr);
    }
    else
    {
      f_level = e->SubdivisionLevel();
      bHaveLevel = true;
    }
  }

  ON_SubDFace* f = AllocateFace(candidate_face);
  if ( nullptr == f)
    return ON_SUBD_RETURN_ERROR(nullptr);
  f->SetSubdivisionLevel(f_level);

  if (edge_count > 0)
  {
    if (edge_count > 4)
    {
      if (false == m_heap.GrowFaceEdgeArray(f,edge_count))
      {
        ReturnFace(f);
        return ON_SUBD_RETURN_ERROR(nullptr);
      }
    }

    ON_SubDEdgePtr* f_edge = f->m_edge4;
    for (unsigned int i = 0; i < edge_count; i++)
    {
      if (4 == i)
        f_edge = f->m_edgex - 4;

      f_edge[i] = edge[i];
      ON__UINT_PTR eptr = edge[i].m_ptr;
      ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr);
      if ( nullptr == e)
        continue;

      eptr = ON_SUBD_EDGE_DIRECTION(eptr);

      ON_SubDVertex* v = const_cast<ON_SubDVertex*>(e->m_vertex[eptr]);
      if (false == m_heap.GrowVertexFaceArrayByOne(v))
      {
        v->m_status.SetDamagedState(true);
        ReturnFace(f);
        return ON_SUBD_RETURN_ERROR(nullptr);
      }
      v->m_faces[v->m_face_count++] = f;
      //if (1 == v->m_face_count)
      //{
      //  if (4 == f->m_edge_count)
      //    v->m_vertex_facet_type = ON_SubD::VertexFacetType::Quad;
      //  else if (3 == f->m_edge_count)
      //    v->m_vertex_facet_type = ON_SubD::VertexFacetType::Tri;
      //  else if ( f->m_edge_count > 4)
      //    v->m_vertex_facet_type = ON_SubD::VertexFacetType::Ngon;
      //}
      //else
      //{
      //  const ON_SubDFace* f0 = v->m_faces[0];
      //  if (nullptr == f0 || f0->m_edge_count != f->m_edge_count)
      //    v->m_vertex_facet_type = ON_SubD::VertexFacetType::Mixed;
      //}
      //v->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;

      ON_SubDFacePtr* e_faces;
      if (e->m_face_count < 2)
      {
        e_faces = e->m_face2;
      }
      else
      {
        // Dale Lear, April 3, 2019 RH-49843 - we support non-manifold SubD objects now.
        //if (2 == e->m_face_count)
        //{
        //  // Getting this error in a valid situation means it is time
        //  // to support non-manifold subdivision objects.
        //  ON_SubDIncrementErrorCount();
        //  ON_WARNING("creating non-manifold subdivision object");
        //}
        if (false == m_heap.GrowEdgeFaceArrayByOne(e))
        {
          e->m_status.SetDamagedState(true);
          continue;
        }
        e_faces = e->m_facex - 2;
      }
      e_faces[e->m_face_count++] = ON_SubDFacePtr::Create(f, eptr);
    }
    f->m_edge_count = (unsigned short)edge_count;
  }

  if ( nullptr == AddFaceToLevel(f))
    return ON_SUBD_RETURN_ERROR(nullptr);

  return f;
}

bool ON_SubDEdge::UpdateEdgeSectorCoefficientsForExperts(bool bUnsetEdgeSectorCoefficientsOnly)
{
  const double  input_sector_coefficient[2] = { m_sector_coefficient[0], m_sector_coefficient[1] };
  if (bUnsetEdgeSectorCoefficientsOnly)
  {
    if (input_sector_coefficient[0] >= 0.0 && input_sector_coefficient[0] <= 1.0
      && input_sector_coefficient[1] >= 0.0 && input_sector_coefficient[1] <= 1.0
      )
      return false;
  }
  m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;
  m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
  if (ON_SubD::EdgeTag::Smooth == m_edge_tag || ON_SubD::EdgeTag::SmoothX == m_edge_tag)
  {
    const unsigned int tagged_end_index = TaggedEndIndex();
    if (tagged_end_index < 2)
    {
      m_sector_coefficient[tagged_end_index] = ON_SubDSectorType::Create( this, tagged_end_index).SectorCoefficient();
    }
    else if (2 == tagged_end_index)
    {
      if (ON_SubD::EdgeTag::Smooth == m_edge_tag && 2 == m_face_count )
        m_edge_tag = ON_SubD::EdgeTag::SmoothX;

      if (ON_SubD::EdgeTag::Smooth == m_edge_tag)
       m_edge_tag = ON_SubD::EdgeTag::Crease;
      else if (ON_SubD::EdgeTag::SmoothX == m_edge_tag)
      {
        m_sector_coefficient[0] = ON_SubDSectorType::Create( this, 0).SectorCoefficient();
        m_sector_coefficient[1] = ON_SubDSectorType::Create( this, 1).SectorCoefficient();
      }
    }
  }

  const bool bNoChanges 
    =  input_sector_coefficient[0] == m_sector_coefficient[0]
    && input_sector_coefficient[1] == m_sector_coefficient[1];

  return (false == bNoChanges);
}

unsigned int ON_SubDLevel::UpdateEdgeSectorCoefficients(
  bool bUnsetEdgeSectorCoefficientsOnly
  )
{
  unsigned int changed_edge_count = 0;
  for (const ON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    if (const_cast<ON_SubDEdge*>(edge)->UpdateEdgeSectorCoefficientsForExperts(bUnsetEdgeSectorCoefficientsOnly))
      ++changed_edge_count;
  }
  return changed_edge_count;
}

class ON_SubDFace* ON_SubD::AddTriangleFace(
  class ON_SubDEdge* edge0,
  class ON_SubDEdge* edge1,
  class ON_SubDEdge* edge2
)
{
  ON_SubDEdge* edges[3] = { edge0,edge1,edge2 };
  return AddFace(edges, 3);
}

class ON_SubDFace* ON_SubD::AddQuadFace(
  class ON_SubDEdge* edge0,
  class ON_SubDEdge* edge1,
  class ON_SubDEdge* edge2,
  class ON_SubDEdge* edge3
  )
{
  ON_SubDEdge* edges[4] = { edge0,edge1,edge2,edge3 };
  return AddFace(edges, 4);
}

class ON_SubDFace* ON_SubD::AddTriangleFace(
  ON_SubDEdgePtr edge0,
  ON_SubDEdgePtr edge1,
  ON_SubDEdgePtr edge2
)
{
  ON_SubDEdgePtr eptr3[3] = { edge0,edge1,edge2 };
  return AddFace(eptr3, 3);
}

class ON_SubDFace* ON_SubD::AddQuadFace(
  ON_SubDEdgePtr edge0,
  ON_SubDEdgePtr edge1,
  ON_SubDEdgePtr edge2,
  ON_SubDEdgePtr edge3
  )
{
  ON_SubDEdgePtr eptr4[4] = { edge0,edge1,edge2,edge3 };
  return AddFace(eptr4, 4);
}

class ON_SubDFace* ON_SubD::AddFace(
  const ON_SimpleArray<ON_SubDEdge*>& edges
)
{
  return AddFace(edges.Array(), edges.UnsignedCount());
}

class ON_SubDFace* ON_SubD::AddFace(
  const ON_SimpleArray<ON_SubDEdgePtr>& edges
)
{
  return AddFace(edges.Array(), edges.UnsignedCount());
}

class ON_SubDFace* ON_SubD::AddFace(
  ON_SubDEdge *const* edge,
  unsigned int edge_count
  )
{
  if (edge_count < 3 || nullptr == edge)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge[0] || nullptr == edge[0]->m_vertex[0] || nullptr == edge[0]->m_vertex[1] || edge[0]->m_vertex[0] == edge[0]->m_vertex[1])
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( edge[0] == edge[edge_count-1] )
    return ON_SUBD_RETURN_ERROR(nullptr);
  ON_SubDEdgePtr* eptr = (ON_SubDEdgePtr*)onmalloc(edge_count * sizeof(*eptr));
  if (nullptr == eptr)
    return ON_SUBD_RETURN_ERROR(nullptr);
  eptr[0] = ON_SubDEdgePtr::Create(
    edge[0],
    (edge[0]->m_vertex[0] == edge[1]->m_vertex[0] || edge[0]->m_vertex[0] == edge[1]->m_vertex[1]) ? 1 : 0
  );
  eptr[edge_count - 1] = ON_SubDEdgePtr::Null;
  for (unsigned int fei = 1; fei < edge_count; ++fei)
  {
    if (nullptr == edge[fei] || nullptr == edge[fei]->m_vertex[0] || nullptr == edge[fei]->m_vertex[1] || edge[fei]->m_vertex[0] == edge[fei]->m_vertex[1])
      break;
    if (edge[fei - 1] == edge[fei])
      break;
    const ON_SubDVertex* v = eptr[fei - 1].RelativeVertex(1);
    if (nullptr == v)
      break;
    eptr[fei] = ON_SubDEdgePtr::Create(edge[fei], v == edge[fei]->m_vertex[0] ? 0 : 1);
    if (v != eptr[fei].RelativeVertex(0))
      break;
  }

  ON_SubDFace* face
    = (eptr[edge_count - 1].IsNotNull() && eptr[0].RelativeVertex(0) == eptr[edge_count - 1].RelativeVertex(1))
    ? AddFace(eptr, edge_count)
    : nullptr;
  onfree(eptr);
  if (nullptr == face)
  {
    ON_SUBD_ERROR("Invalid input edge[] array");
  }
  return face;
}

class ON_SubDFace* ON_SubD::AddFace(
  const ON_SubDEdgePtr* edge,
  unsigned int edge_count
  )
{
  ON_SubDimple* subdimple = SubDimple(true);
  return (nullptr != subdimple) ? subdimple->AddFace(edge_count, edge) : nullptr;
}


class ON_SubDFace* ON_SubD::AddFace(
  const ON_SubDFace* candidate_face,
  const ON_SubDEdgePtr* edge,
  unsigned int edge_count
  )
{
  ON_SubDimple* subdimple = SubDimple(true);
  return (nullptr != subdimple) ? subdimple->AddFace(candidate_face, edge_count, edge) : nullptr;
}

bool ON_SubD::AddFaceEdgeConnection(
  ON_SubDFace* face,
  unsigned int i,
  ON_SubDEdge* edge,
  ON__UINT_PTR edge_direction
  )
{
  return AddFaceEdgeConnection(face, i, ON_SubDEdgePtr::Create(edge, edge_direction));
}

bool ON_SubD::AddFaceEdgeConnection(
  ON_SubDFace* face,
  unsigned int i,
  ON_SubDEdgePtr eptr
  )
{
  if (nullptr == face && i >= ON_SubDFace::MaximumEdgeCount)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  unsigned int face_edge_count = (unsigned int)face->m_edge_count + 1U;
  if ( face_edge_count <= i )
    face_edge_count = i+1;

  ON_SubDEdge* edge = eptr.Edge();
  if (nullptr != edge)
  {
    if (edge->m_face_count >= edge->m_facex_capacity + (unsigned short)2)
    {
      if (false == GrowEdgeFaceArray(edge, 0))
        return ON_SUBD_RETURN_ERROR(false);
    }

    ON_SubDFacePtr fptr = ON_SubDFacePtr::Create(face,eptr.EdgeDirection());

    unsigned short efi = edge->m_face_count;
    if ( efi < 2 )
      edge->m_face2[efi] = fptr;
    else
    {
      if ( nullptr == edge->m_facex )
        return ON_SUBD_RETURN_ERROR(false);
      edge->m_facex[efi - 2] = fptr;
    }
    edge->m_face_count++;
  }

  if (face_edge_count > ((unsigned int)face->m_edgex_capacity) + 4U)
  {
    if (false == GrowFaceEdgeArray(face,face_edge_count))
      return ON_SUBD_RETURN_ERROR(false);
  }

  if (i >= ((unsigned int)face->m_edge_count))
  {
    unsigned int j = face->m_edge_count;
    for (/*empty init*/;j < 4; j++)
      face->m_edge4[j] = ON_SubDEdgePtr::Null;
    for (/*empty init*/;j < i; j++)
      face->m_edgex[j-4] = ON_SubDEdgePtr::Null;
  }
  else
  {
    for (unsigned int j = face_edge_count - 1; j > i; j--)
    {
      if (j > 4)
        face->m_edgex[j - 4] = face->m_edgex[j - 5];
      else if (4 == j)
        face->m_edgex[0] = face->m_edge4[3];
      else
        face->m_edge4[j] = face->m_edge4[j - 1];
    }
  }

  if ( i < 4 )
    face->m_edge4[i] = eptr;
  else 
    face->m_edgex[i-4] = eptr;
  face->m_edge_count = (unsigned short)face_edge_count;

  return true;
}


bool ON_SubD::RemoveFaceEdgeConnection(
  ON_SubDFace* face,
  ON_SubDEdge* edge
  )
{
  ON_SubDEdgePtr removed_edge;
  return RemoveFaceEdgeConnection(face, face->EdgeArrayIndex(edge), removed_edge);
}

bool ON_SubD::RemoveFaceEdgeConnection(
  ON_SubDFace* face,
  unsigned int i
  )
{
  ON_SubDEdgePtr removed_edge;
  return RemoveFaceEdgeConnection(face, i, removed_edge);
}

bool ON_SubD::RemoveFaceEdgeConnection(
  ON_SubDFace* face,
  unsigned int i,
  ON_SubDEdgePtr& removed_edge
  )
{
  removed_edge = ON_SubDEdgePtr::Null;
  if ( nullptr == face && i >= (unsigned int)face->m_edge_count )
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  if ( false == face->RemoveEdgeFromArray(i,removed_edge) )
    return ON_SUBD_RETURN_ERROR(false);

  ON_SubDEdge* edge = removed_edge.Edge();
  if (nullptr != edge)
  {
    if (false == edge->RemoveFaceFromArray(face))
      return ON_SUBD_RETURN_ERROR(false);
  }

  return true;
}

bool ON_SubD::RemoveFaceConnections(
  ON_SubDFace* face
)
{
  if ( nullptr == face )
  {
    return ON_SUBD_RETURN_ERROR(false);
  }
  if (face->m_edge_count > 0)
  {
    ON_SubDEdgePtr removed_edge;
    for (unsigned short fei = face->m_edge_count; fei > 0; --fei)
    {
      removed_edge = ON_SubDEdgePtr::Null;
      if (false == face->RemoveEdgeFromArray(fei - 1, removed_edge))
        return ON_SUBD_RETURN_ERROR(false);
      ON_SubDEdge* edge = removed_edge.Edge();
      if (nullptr != edge)
      {
        if (false == edge->RemoveFaceFromArray(face))
          return ON_SUBD_RETURN_ERROR(false);

        for (int evi = 0; evi < 2; ++evi)
        {
          ON_SubDVertex* v = const_cast<ON_SubDVertex*>(edge->m_vertex[evi]);
          if (nullptr != v)
          {
            for (unsigned short vfi = 0; vfi < v->m_face_count; ++vfi)
            {
              if (face == v->m_faces[vfi])
              {
                for (++vfi; vfi < v->m_face_count; ++vfi)
                  v->m_faces[vfi - 1] = v->m_faces[vfi];
                v->m_face_count--;
                break;
              }
            }
          }
        }

      }
    }
    face->m_edge_count = 0;
  }

  return true;
}



static bool ON_SubDFace_GetSubdivisionPointError(
  const class ON_SubDFace* face,
  double subdivision_point[3],
  bool bDamagedState
  )
{
  if (nullptr == subdivision_point)
    return ON_SUBD_RETURN_ERROR(false); // caller passed a null pointer - face is not necessarily damaged

  // make sure returned point is not used by a caller who doesn't bother to check return codes.
  subdivision_point[0] = ON_DBL_QNAN;
  subdivision_point[1] = ON_DBL_QNAN;
  subdivision_point[2] = ON_DBL_QNAN;

  if (nullptr == face) 
    return ON_SUBD_RETURN_ERROR(false);

  // face is damaged in some way - mark it
  face->m_status.SetDamagedState(bDamagedState);

  return ON_SUBD_RETURN_ERROR(false);
}

const ON_3dPoint ON_SubDFace::SubdivisionPoint() const
{
  ON_3dPoint S;
  return (GetSubdivisionPoint(&S.x) && S.IsValid()) ? S : ON_3dPoint::NanPoint;
}

bool ON_SubDFace::GetSubdivisionPoint(
  double subdivision_point[3]
) const
{
  if (nullptr == subdivision_point)
    return ON_SubDFace_GetSubdivisionPointError(this, subdivision_point, false);

  if (GetSavedSubdivisionPoint(subdivision_point))
    return true;
    
  if (EvaluateCatmullClarkSubdivisionPoint(subdivision_point))
  {
    SetSavedSubdivisionPoint(subdivision_point);
    return true;
  }

  return false;
}

bool ON_SubDFace::EvaluateCatmullClarkSubdivisionPoint(double subdivision_point[3]) const
{
  if (nullptr == subdivision_point)
    return ON_SubDFace_GetSubdivisionPointError(this,subdivision_point,false);

  const unsigned int count = m_edge_count;
  if (count < 3)
    return ON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);

  double displacementV[3] = { 0 };
  const bool bApplyDisplacement = GetSubdivisionDisplacement(displacementV);

  const class ON_SubDEdgePtr* edge_ptr = m_edge4;

  ON__UINT_PTR e_ptr;
  const ON_SubDEdge* e;
  ON__UINT_PTR edir;
  const double* vertexP[4];

  // Use faster code for the case when the face is a quad.
  // Since this is a Catmull-Clark subdivision scheme, this
  // case is the most common by far and code that gives quads
  // special treatment will run noticably faster.
  e_ptr = edge_ptr[0].m_ptr;
  e = ON_SUBD_EDGE_POINTER(e_ptr);
  if ( nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] )
    return ON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
  edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
  vertexP[0] = e->m_vertex[edir]->m_P;
  vertexP[1] = e->m_vertex[1 - edir]->m_P;

  e_ptr = edge_ptr[2].m_ptr;
  e = ON_SUBD_EDGE_POINTER(e_ptr);
  if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
    return ON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
  edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
  vertexP[2] = e->m_vertex[edir]->m_P;
  vertexP[3] = e->m_vertex[1 - edir]->m_P;

  if (4 == count)
  {
    // most common case in quad subdivision schemes
    subdivision_point[0] = (vertexP[0][0] + vertexP[1][0] + vertexP[2][0] + vertexP[3][0])*0.25;
    subdivision_point[1] = (vertexP[0][1] + vertexP[1][1] + vertexP[2][1] + vertexP[3][1])*0.25;
    subdivision_point[2] = (vertexP[0][2] + vertexP[1][2] + vertexP[2][2] + vertexP[3][2])*0.25;

    if (bApplyDisplacement)
    {
      subdivision_point[0] += displacementV[0];
      subdivision_point[1] += displacementV[1];
      subdivision_point[2] += displacementV[2];
    }

    return true;
  }

  if (3 == count)
  {
    // 2nd most common case in quad subdivision schemes
    subdivision_point[0] = (vertexP[0][0] + vertexP[1][0] + vertexP[2][0]) / 3.0;
    subdivision_point[1] = (vertexP[0][1] + vertexP[1][1] + vertexP[2][1]) / 3.0;
    subdivision_point[2] = (vertexP[0][2] + vertexP[1][2] + vertexP[2][2]) / 3.0;

    if (bApplyDisplacement)
    {
      subdivision_point[0] += displacementV[0];
      subdivision_point[1] += displacementV[1];
      subdivision_point[2] += displacementV[2];
    }

    return true;
  }

  // count > 4
  double faceP[3]
    = {
    (vertexP[0][0] + vertexP[1][0] + vertexP[2][0] + vertexP[3][0]),
    (vertexP[0][1] + vertexP[1][1] + vertexP[2][1] + vertexP[3][1]),
    (vertexP[0][2] + vertexP[1][2] + vertexP[2][2] + vertexP[3][2]) 
    };

  if (nullptr == m_edgex)
  {
    return ON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
  }

  edge_ptr = m_edgex - 4; // -4 because index i begins at 4
  unsigned int i;
  for (i = 4; i + 1 < count; i += 2)
  {
    e_ptr = edge_ptr[i].m_ptr;
    e = ON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return ON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
    edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
    vertexP[0] = e->m_vertex[edir]->m_P;
    vertexP[1] = e->m_vertex[1 - edir]->m_P;

    faceP[0] += vertexP[0][0];
    faceP[1] += vertexP[0][1];
    faceP[2] += vertexP[0][2];

    faceP[0] += vertexP[1][0];
    faceP[1] += vertexP[1][1];
    faceP[2] += vertexP[1][2];
  }

  if (i < count)
  {
    // odd number of edges and vertices
    e_ptr = edge_ptr[count - 1].m_ptr;
    e = ON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return ON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
    edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
    vertexP[0] = e->m_vertex[edir]->m_P;
    faceP[0] += vertexP[0][0];
    faceP[1] += vertexP[0][1];
    faceP[2] += vertexP[0][2];
  }

  const double n = count;

  subdivision_point[0] = faceP[0] / n;
  subdivision_point[1] = faceP[1] / n;
  subdivision_point[2] = faceP[2] / n;

  if (bApplyDisplacement)
  {
    subdivision_point[0] += displacementV[0];
    subdivision_point[1] += displacementV[1];
    subdivision_point[2] += displacementV[2];
  }
  return true;
}

int ON_SubDComponentBase::CompareId(
  const ON_SubDComponentBase* lhs,
  const ON_SubDComponentBase* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls at end of list
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  if (lhs->m_id < rhs->m_id)
    return -1;
  if (lhs->m_id > rhs->m_id)
    return 1;
  return 0;
}

void ON_SubDComponentBase::ClearSavedSubdivisionPoint() const
{
  Internal_ClearSubdivisionPointAndSurfacePointFlags();
}

bool ON_SubDComponentBase::SetSavedSubdivisionPoint(
  const double subdivision_point[3]
  ) const
{
  if (nullptr == subdivision_point)
  {
    Internal_ClearSubdivisionPointAndSurfacePointFlags();
    return true;
  }
  
  if ( ON_IsValid(subdivision_point[0])
    && ON_IsValid(subdivision_point[1])
    && ON_IsValid(subdivision_point[2])
    )
  {
    m_saved_subd_point1[0] = subdivision_point[0];
    m_saved_subd_point1[1] = subdivision_point[1];
    m_saved_subd_point1[2] = subdivision_point[2];
    m_saved_points_flags |= ON_SUBD_CACHE_POINT_FLAG_BIT;
    return true;
  }

  Internal_ClearSubdivisionPointAndSurfacePointFlags();
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDComponentBase::GetSavedSubdivisionPoint(
  double subdivision_point[3]
  ) const
{
  if ( 0 == (ON_SUBD_CACHE_POINT_FLAG_BIT & m_saved_points_flags) )
    return false;

  if (nullptr != subdivision_point)
  {
    subdivision_point[0] = m_saved_subd_point1[0];
    subdivision_point[1] = m_saved_subd_point1[1];
    subdivision_point[2] = m_saved_subd_point1[2];
  }

  return true;
}

const ON_3dPoint ON_SubDComponentBase::SavedSubdivisionPoint() const
{
  ON_3dPoint p(ON_3dPoint::NanPoint);
  return GetSavedSubdivisionPoint(&p.x) ? p : ON_3dPoint::NanPoint;
}

unsigned const ON_SubDComponentBase::SubdivisionLevel() const
{
  return (unsigned)m_level;
}

void ON_SubDComponentBase::SetSubdivisionLevel(unsigned level)
{
  if (level <= 255U)
    m_level = ((unsigned char)level);
}

const ON_ComponentStatus ON_SubDComponentBase::Status() const
{
  return m_status;
}

bool ON_SubDComponentBase::IsActive() const
{
  return (m_id >= 0 && m_archive_id != ON_UNSET_UINT_INDEX);
}


bool ON_SubDComponentBase::Mark() const
{
  return m_status.RuntimeMark();
}

bool ON_SubDComponentBase::ClearMark() const
{
 return m_status.ClearRuntimeMark();
}

bool ON_SubDComponentBase::SetMark() const
{
  return m_status.SetRuntimeMark();
}

bool ON_SubDComponentBase::SetMark(
  bool bMark
) const
{
  return m_status.SetRuntimeMark(bMark);
}

bool ON_SubDComponentPtr::IsActive() const
{
  const ON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->IsActive() : false;
}

bool ON_SubDVertexPtr::IsActive() const
{
  const ON_SubDVertex* v = ON_SUBD_VERTEX_POINTER(m_ptr);
  return (nullptr != v) ? v->IsActive() : false;
}

bool ON_SubDEdgePtr::IsActive() const
{
  const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsActive() : false;
}

bool ON_SubDFacePtr::IsActive() const
{
  const ON_SubDFace* f = ON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr != f) ? f->IsActive() : false;
}


const ON_ComponentStatus ON_SubDVertex::NeighborhoodStatusLogicalOr(
  bool bIncludeEdges,
  bool bIncludeFaces
) const
{
  ON_ComponentStatus s(m_status);
  if (bIncludeEdges && nullptr != m_edges)
  {
    for (unsigned short vei = 0; vei < m_edge_count; vei++)
    {
      const ON_SubDEdge* e = m_edges[vei].Edge();
      if (nullptr != e)
        s = ON_ComponentStatus::LogicalOr(s, e->m_status);
    }
  }
  if (bIncludeFaces && nullptr != m_faces)
  {
    for (unsigned short vfi = 0; vfi < m_face_count; vfi++)
    {
      const ON_SubDFace* f = m_faces[vfi];
      if (nullptr != f)
        s = ON_ComponentStatus::LogicalOr(s, f->m_status);
    }
  }
  return s;
}
  
const ON_ComponentStatus ON_SubDEdge::NeighborhoodStatusLogicalOr(
  bool bIncludeVertices,
  bool bIncludeFaces
) const
{
  ON_ComponentStatus s(m_status);
  if (bIncludeVertices)
  {
    for (unsigned int evi = 0; evi < 2; evi++)
    {
      const ON_SubDVertex* v = m_vertex[evi];
      if (nullptr != v)
        s = ON_ComponentStatus::LogicalOr(s, v->m_status);
    }
  }
  if (bIncludeFaces)
  {
    const ON_SubDFacePtr* fptr = m_face2;
    for (unsigned short vfi = 0; vfi < m_face_count; vfi++)
    {
      const ON_SubDFace* f = fptr->Face();
      if (nullptr != f)
        s = ON_ComponentStatus::LogicalOr(s, f->m_status);
      if (1 == vfi)
      {
        fptr = m_facex;
        if (nullptr == fptr)
          break;
      }
      else
        fptr++;
    }
  }
  return s;
}

const ON_ComponentStatus ON_SubDFace::NeighborhoodStatusLogicalOr(bool bIncludeVertices, bool bIncludeEdges) const
{
  ON_ComponentStatus s(m_status);
  if (bIncludeVertices || bIncludeEdges)
  {
    const ON_SubDEdgePtr* eptr = m_edge4;
    for (unsigned int fei = 0; fei < m_edge_count; fei++)
    {
      if (4 == fei)
      {
        eptr = m_edgex;
        if (nullptr == eptr)
          break;
      }
      const ON_SubDEdge* e = eptr->Edge();
      if (nullptr != e)
      {
        if (bIncludeEdges)
        {
          s = ON_ComponentStatus::LogicalOr(s, e->m_status);
        }
        if (bIncludeVertices)
        {
          const ON_SubDVertex* v = e->m_vertex[(0!=eptr->EdgeDirection())?1:0];
          if (nullptr != v)
            s = ON_ComponentStatus::LogicalOr(s, v->m_status);
        }
      }
      eptr++;
    }
  }
  return s;
}

static void Internal_ClearFaceNeighborhoodCache(const ON_SubDFace* face)
{
  // Clear cached values for every component associated with this face.
  face->ClearSavedSubdivisionPoints();
  const ON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned int efi = 0; efi < face->m_edge_count; efi++)
  {
    if (4 == efi)
    {
      eptr = face->m_edgex;
      if ( nullptr == eptr)
        break;
    }
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != edge)
    {
      edge->ClearSavedSubdivisionPoints();
      edge->UnsetSectorCoefficientsForExperts();
      for (unsigned int evi = 0; evi < 2; evi++)
      {
        const ON_SubDVertex* vertex = edge->m_vertex[evi];
        if (nullptr != vertex)
          vertex->ClearSavedSubdivisionPoints();
      }
    }
    eptr++;
  }
}

void ON_SubDVertex::VertexModifiedNofification() const
{
  ClearSavedSubdivisionPoints();
  if (nullptr != m_edges)
  {
    for (unsigned short vei = 0; vei < m_edge_count; vei++)
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == edge)
        continue;
      edge->ClearSavedSubdivisionPoints();
      edge->UnsetSectorCoefficientsForExperts();
      const ON_SubDVertex* v1 = edge->m_vertex[1-ON_SUBD_EDGE_DIRECTION(m_edges[vei].m_ptr)];
      if (nullptr != v1)
        v1->ClearSavedSubdivisionPoints();
    }

    // This is needed to clear cached information in the Catmull-Clark 
    // ring that is not immediately adjacent to this vertex but whose values
    // this vertex affects.
    for (unsigned short vfi = 0; vfi < m_face_count; vfi++)
    {
      const ON_SubDFace* face = this->m_faces[vfi];
      if (nullptr != face)
        Internal_ClearFaceNeighborhoodCache(face);
    }
  }
}

void ON_SubDEdge::EdgeModifiedNofification() const
{
  ClearSavedSubdivisionPoints();
  UnsetSectorCoefficientsForExperts();
  for (unsigned int evi = 0; evi < 2; evi++)
  {
    if (nullptr != m_vertex[evi])
      m_vertex[evi]->VertexModifiedNofification();
  }

  // If the topology pointers are complete and accurate, then the following
  // is not required. It's here because this SubD may be under construction
  // and we cannot assume the topology pointers are complete and accurate.
  const ON_SubDFacePtr* fptr = m_face2;
  for (unsigned int efi = 0; efi < 2; efi++)
  {
    if (2 == efi)
    {
      fptr = m_facex;
      if ( nullptr == fptr)
        break;
    }
    const ON_SubDFace* face = ON_SUBD_FACE_POINTER(fptr->m_ptr);
    if ( nullptr != face )
      Internal_ClearFaceNeighborhoodCache(face);
    fptr++;
  }
}

void ON_SubDEdge::UnsetSectorCoefficientsForExperts() const
{
  const_cast<ON_SubDEdge*>(this)->m_sector_coefficient[0] = ON_SubDSectorType::UnsetSectorCoefficient;
  const_cast<ON_SubDEdge*>(this)->m_sector_coefficient[1] = ON_SubDSectorType::UnsetSectorCoefficient;
}

void ON_SubDVertex::UnsetSectorCoefficientsForExperts(unsigned int relative_edge_end_dex) const
{
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr == e)
      continue;
    ON__UINT_PTR edir = ON_SUBD_EDGE_DIRECTION(m_edges[vei].m_ptr);
    const unsigned evi
      = (relative_edge_end_dex < 2)
      ? ((0 == edir ? false : true) == (0 == relative_edge_end_dex ? false : true) ? 0U : 1U)
      : 2U;
    if ( evi < 2)
      e->m_sector_coefficient[evi] = ON_SubDSectorType::UnsetSectorCoefficient;
    else
    {
      e->m_sector_coefficient[0] = ON_SubDSectorType::UnsetSectorCoefficient;
      e->m_sector_coefficient[1] = ON_SubDSectorType::UnsetSectorCoefficient;
    }
  }
}

void ON_SubDFace::FaceModifiedNofification() const
{
  Internal_ClearFaceNeighborhoodCache(this);

  // This is needed to clear cached information in the Catmull-Clark 
  // ring that is not immediately adjacent to this face but whose values
  // this face affects.
  const ON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned int efi = 0; efi < m_edge_count; efi++)
  {
    if (4 == efi)
    {
      eptr = m_edgex;
      if ( nullptr == eptr)
        break;
    }
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != edge)
    {
      const ON_SubDFacePtr* fptr = edge->m_face2;
      for (unsigned short fei = 0; fei < edge->m_face_count; fei++)
      {
        if (2 == fei)
        {
          fptr = edge->m_facex;
          if (nullptr == fptr)
            break;
        }
        const ON_SubDFace* f = ON_SUBD_FACE_POINTER(fptr->m_ptr);
        if (nullptr != f && f != this)
          Internal_ClearFaceNeighborhoodCache(f);
      }
    }
    eptr++;
  }
}

void ON_SubDComponentBase::Internal_ClearSubdivisionPointAndSurfacePointFlags() const
{
  ON_SUBD_CACHE_CLEAR_POINT_FLAG(m_saved_points_flags);
  ON_SUBD_CACHE_CLEAR_LIMITLOC_FLAG(m_saved_points_flags);
}

bool ON_SubDComponentBase::Internal_SubdivisionPointFlag() const
{
  return (0 != ON_SUBD_CACHE_POINT_FLAG(m_saved_points_flags));
}

void ON_SubDComponentBase::Internal_ClearSubdivisionPointFlag() const
{
  ON_SUBD_CACHE_CLEAR_POINT_FLAG(m_saved_points_flags);
}

bool ON_SubDComponentBase::Internal_SurfacePointFlag() const
{
  return (0 != ON_SUBD_CACHE_LIMITLOC_FLAG(m_saved_points_flags));
}

void ON_SubDComponentBase::Internal_ClearSurfacePointFlag() const
{
  ON_SUBD_CACHE_CLEAR_LIMITLOC_FLAG(m_saved_points_flags);
}

bool ON_SubDComponentBase::SavedSubdivisionPointIsSet() const
{
  return 
    (0 != ON_SUBD_CACHE_POINT_FLAG(m_saved_points_flags))
    ? (ON_IS_VALID(m_saved_subd_point1[0]) && ON_IS_VALID(m_saved_subd_point1[1]) && ON_IS_VALID(m_saved_subd_point1[2]))
    : false;
}

bool ON_SubDComponentBase::SubdivisionDisplacementIsNonzero() const
{
  return (0 != ON_SUBD_CACHE_DISPLACEMENT_FLAG(m_saved_points_flags))
    ? (
    (0.0 != m_displacement_V[0] || 0.0 != m_displacement_V[1] || 0.0 != m_displacement_V[2])
      && ON_IS_VALID(m_displacement_V[0]) && ON_IS_VALID(m_displacement_V[1]) && ON_IS_VALID(m_displacement_V[2])
      )
    : false;
}

bool ON_SubDComponentBase::SetSubdivisionDisplacement(
    const double displacement[3]
    )
{
  if (
    nullptr == displacement
    || (0.0 == displacement[0] && 0.0 == displacement[1] && 0.0 == displacement[2])
    )
  {
    ClearSubdivisionDisplacement();
    return true;
  }

  if ( ON_IsValid(displacement[0]) && ON_IsValid(displacement[1]) && ON_IsValid(displacement[2]) )
  {
    m_saved_points_flags |= ON_SUBD_CACHE_DISPLACEMENT_FLAG_BIT;
    m_displacement_V[0] = displacement[0];
    m_displacement_V[1] = displacement[1];
    m_displacement_V[2] = displacement[2];
    return true;
  }

  ClearSubdivisionDisplacement();
  
  return ON_SUBD_RETURN_ERROR(false);
}

void ON_SubDComponentBase::ClearSubdivisionDisplacement() const
{
  if (0 != (m_saved_points_flags & ON_SUBD_CACHE_DISPLACEMENT_FLAG_BIT))
  {
    ON_SUBD_CACHE_CLEAR_POINT_FLAG(m_saved_points_flags);
    ON_SUBD_CACHE_CLEAR_DISPLACEMENT_FLAG(m_saved_points_flags);
  }
}

bool ON_SubDComponentBase::GetSubdivisionDisplacement(
    double displacement[3]
    ) const
{
  const bool rc = (0 != (ON_SUBD_CACHE_DISPLACEMENT_FLAG_BIT & m_saved_points_flags));
  if (nullptr != displacement)
  {
    if (rc)
    {
      displacement[0] = m_displacement_V[0];
      displacement[1] = m_displacement_V[1];
      displacement[2] = m_displacement_V[2];
    }
    else
    {
      displacement[0] = 0.0;
      displacement[1] = 0.0;
      displacement[2] = 0.0;
    }
  }
  return rc;
}

const ON_3dVector ON_SubDComponentBase::SubdivisionDisplacement() const
{
  ON_3dVector v(ON_3dVector::ZeroVector);
  return GetSubdivisionDisplacement(&v.x) ? v : ON_3dVector::ZeroVector;
}

void ON_SubDComponentBase::Internal_SetSavedSurfacePointFlag(bool bSavedSurfacePointFlag) const
{
  if (bSavedSurfacePointFlag)
    m_saved_points_flags |= ON_SUBD_CACHE_LIMITLOC_FLAG_BIT;
  else
    Internal_ClearSurfacePointFlag();
}


void ON_SubDComponentBase::Internal_SetModified1Flag() const
{
  m_saved_points_flags |= ON_SubDComponentBase::ModifiedFlags::Modified1Bit;
}

void ON_SubDComponentBase::Internal_SetModified2Flag() const
{
  m_saved_points_flags |= ON_SubDComponentBase::ModifiedFlags::Modified1Bit;
}

void ON_SubDComponentBase::Internal_ClearModifiedFlags() const
{
  m_saved_points_flags &= ~ON_SubDComponentBase::ModifiedFlags::ModifiedFlagsMask;
}

bool ON_SubDComponentBase::Internal_Modified1IsSet() const
{
  return (0 != (m_saved_points_flags & ON_SubDComponentBase::ModifiedFlags::Modified1Bit));
}

bool ON_SubDComponentBase::Internal_Modified1or2IsSet() const
{
  return (0 != (m_saved_points_flags & ON_SubDComponentBase::ModifiedFlags::ModifiedFlagsMask));
}

bool ON_SubDFace::ReverseEdgeList()
{
  const unsigned int edge_count = m_edge_count;
  if ( 0 == edge_count)
    return true;
  if (edge_count > 4 && nullptr == m_edgex)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  ON_SubDEdgePtr buffer[16];
  ON_SubDEdgePtr* reversed_eptrs;
  if ( edge_count <= sizeof(buffer)/sizeof(buffer[0]) )
    reversed_eptrs = buffer;
  else
  {
    reversed_eptrs = new(std::nothrow) ON_SubDEdgePtr[edge_count];
    if ( nullptr == reversed_eptrs)
      return ON_SUBD_RETURN_ERROR(false);
  }

  ON_SubDEdgePtr* face_eptrs = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++, face_eptrs++)
  {
    if (4 == fei)
      face_eptrs = m_edgex;

    ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(face_eptrs->m_ptr);
    if ( nullptr == e)
      continue;
    ON__UINT_PTR edir = ON_SUBD_EDGE_DIRECTION(face_eptrs->m_ptr);
    reversed_eptrs[edge_count-1-fei] = ON_SubDEdgePtr::Create(e,1-edir);

    ON_SubDFacePtr* edges_fptrs = e->m_face2;
    const unsigned int face_count = e->m_face_count;
    for (unsigned int efi = 0; efi < face_count; efi++, edges_fptrs++)
    {
      if (2 == efi)
      {
        edges_fptrs = e->m_facex;
        if ( nullptr == edges_fptrs)
          break;
      }
      if ( this != ON_SUBD_FACE_POINTER(edges_fptrs->m_ptr) )
        continue;
      *edges_fptrs = ON_SubDFacePtr::Create(this,1-ON_SUBD_FACE_DIRECTION(edges_fptrs->m_ptr));
      break;
    } 
  }

  face_eptrs = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++)
  {
    if (4 == fei)
      face_eptrs = m_edgex;
    *face_eptrs++ = reversed_eptrs[fei];
  }

  if ( reversed_eptrs != buffer )
    delete[] reversed_eptrs;

  return true;
}

static bool ON_SubDEdge_GetSubdivisionPointError(
  const class ON_SubDEdge* edge,
  double edge_point[3],
  const double* edgeP[2],
  bool bDamagedState
  )
{
  if (nullptr == edge || nullptr == edge_point)
    return false; // caller passed a null pointer - edge is not necessarily damaged

  ON_SubDIncrementErrorCount();
  edge->m_status.SetDamagedState(bDamagedState);

  if (nullptr != edgeP && nullptr != edgeP[0] && nullptr != edgeP[1])
  {
    const double edgePsum[3] = { edgeP[0][0] + edgeP[1][0], edgeP[0][1] + edgeP[1][1], edgeP[0][2] + edgeP[1][2] };
    edge_point[0] = 0.5*edgePsum[0];
    edge_point[1] = 0.5*edgePsum[1];
    edge_point[2] = 0.5*edgePsum[2];
  }
  return true;
}

unsigned int ON_SubDEdge::GetFacePointSum(
  const ON_SubDFace* face,
  const ON_SubDEdge* edge,
  double* facePsum
  )
{
  const ON_SubDEdge* e;
  ON__UINT_PTR e_ptr, edir;
  const double* vertexP[2];

  if (nullptr == face)
    return 0;
  
  const unsigned int n = face->m_edge_count;

  if (3 == n)
  {
    if (edge == ON_SUBD_EDGE_POINTER(face->m_edge4[0].m_ptr))
      e_ptr = face->m_edge4[1].m_ptr;
    else if (edge == ON_SUBD_EDGE_POINTER(face->m_edge4[1].m_ptr))
      e_ptr = face->m_edge4[2].m_ptr;
    else if (edge == ON_SUBD_EDGE_POINTER(face->m_edge4[2].m_ptr))
      e_ptr = face->m_edge4[0].m_ptr;
    else
      return 0;
      e = ON_SUBD_EDGE_POINTER(e_ptr);
      if (nullptr == e)
        return 0;
      if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
        return 0;
      edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
      if (edge->m_vertex[0] != e->m_vertex[edir] && edge->m_vertex[1] != e->m_vertex[edir])
        return 0;
      vertexP[0] = e->m_vertex[1 - edir]->m_P;
      facePsum[0] = vertexP[0][0];
      facePsum[1] = vertexP[0][1];
      facePsum[2] = vertexP[0][2];

      return n;
  }

  if (4 == n)
  {
    if (edge == ON_SUBD_EDGE_POINTER(face->m_edge4[0].m_ptr))
      e_ptr = face->m_edge4[2].m_ptr;
    else if (edge == ON_SUBD_EDGE_POINTER(face->m_edge4[1].m_ptr))
      e_ptr = face->m_edge4[3].m_ptr;
    else if (edge == ON_SUBD_EDGE_POINTER(face->m_edge4[2].m_ptr))
      e_ptr = face->m_edge4[0].m_ptr;
    else if (edge == ON_SUBD_EDGE_POINTER(face->m_edge4[3].m_ptr))
      e_ptr = face->m_edge4[1].m_ptr;
    else
      return 0;
    e = ON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e)
      return 0;
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return 0;
    edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
    vertexP[0] = e->m_vertex[edir]->m_P;
    vertexP[1] = e->m_vertex[1 - edir]->m_P;
    facePsum[0] = vertexP[0][0] + vertexP[1][0];
    facePsum[1] = vertexP[0][1] + vertexP[1][1];
    facePsum[2] = vertexP[0][2] + vertexP[1][2];

    return n;
  }

  if (n < 3)
    return 0;

  const ON_SubDEdgePtr* edgeptr = face->m_edge4;
  const ON_SubDVertex* edge_vertex[2] = { edge->m_vertex[0], edge->m_vertex[1] };
  facePsum[0] = 0.0;
  facePsum[1] = 0.0;
  facePsum[2] = 0.0;
  for (unsigned i = 0; i < n; i++)
  {
    if (4 == i)
      edgeptr = face->m_edgex - 4;
    e = ON_SUBD_EDGE_POINTER(edgeptr[i].m_ptr);
    if (nullptr == e)
      return 0;
    if (edge == e)
      continue;
    edir = ON_SUBD_EDGE_DIRECTION(edgeptr[i].m_ptr);
    const ON_SubDVertex* e_vertex[2] = { e->m_vertex[edir], e->m_vertex[1 - edir] };
    if (nullptr == e_vertex[0] || nullptr == e_vertex[1])
      return 0;
    if (edge_vertex[0] != e_vertex[0] && edge_vertex[1] != e_vertex[0])
    {
      vertexP[0] = e_vertex[0]->m_P;
      facePsum[0] += vertexP[0][0];
      facePsum[1] += vertexP[0][1];
      facePsum[2] += vertexP[0][2];
    }
    if (i + 1 < n)
    {
      // start of next edge = end of this edge
      if (edge_vertex[0] != e_vertex[1] && edge_vertex[1] != e_vertex[1])
      {
        vertexP[0] = e_vertex[1]->m_P;
        facePsum[0] += vertexP[0][0];
        facePsum[1] += vertexP[0][1];
        facePsum[2] += vertexP[0][2];
      }
      i++;
      if (4 == i && n > 4)
        edgeptr = face->m_edgex - 4;
    }
  }

  return n;
}

const ON_3dPoint ON_SubDEdge::SubdivisionPoint() const
{
  ON_3dPoint S;
  return (GetSubdivisionPoint(&S.x) && S.IsValid()) ? S : ON_3dPoint::NanPoint;
}

bool ON_SubDEdge::GetSubdivisionPoint(
  double subdivision_point[3]
) const
{
  if (nullptr == subdivision_point)
    return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  if (GetSavedSubdivisionPoint(subdivision_point))
    return true;

  if (EvaluateCatmullClarkSubdivisionPoint(subdivision_point))
  {
    SetSavedSubdivisionPoint(subdivision_point);
    return true;
  }

  return false;
}

const ON_3dPoint ON_SubDEdge::ControlNetCenterPoint() const
{
  return 0.5*(ControlNetPoint(0) + ControlNetPoint(1));
}

const ON_3dVector ON_SubDEdge::ControlNetCenterNormal(
  unsigned int edge_face_index
) const
{
  const ON_SubDFace* face = Face(edge_face_index);
  return (nullptr != face) ? face->ControlNetCenterNormal() : ON_3dVector::NanVector;
}

bool ON_SubDEdge::EvaluateCatmullClarkSubdivisionPoint(double subdivision_point[3]) const
{
  if (nullptr == subdivision_point)
    return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  const ON_SubDVertex* edge_vertex[2] = { m_vertex[0], m_vertex[1] };
  if (nullptr == edge_vertex[0] || nullptr == edge_vertex[1])
    return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, nullptr, true);

  double displacementV[3] = { 0 };
  const bool bApplyDisplacement = GetSubdivisionDisplacement(displacementV);

  const double* edgeP[2] = { edge_vertex[0]->m_P, edge_vertex[1]->m_P };
  const double edgePsum[3] = { edgeP[0][0] + edgeP[1][0], edgeP[0][1] + edgeP[1][1], edgeP[0][2] + edgeP[1][2] };

  if ( IsSmooth() )
  {
    // A smooth edge must have exactly two neighboring faces and
    // at most one end vertex can be tagged.

    if (2 != m_face_count)
      return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);

    const ON_SubDFace* faces[2] = { ON_SUBD_FACE_POINTER(m_face2[0].m_ptr), ON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
    if (nullptr == faces[0] || nullptr == faces[1])
      return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);

    // for each neighbor face, sum the vertex locations that are not on this edge
    double facePsum[2][3];
    const unsigned int face_edge_count[2]
      = { ON_SubDEdge::GetFacePointSum(faces[0], this, facePsum[0]),
      ON_SubDEdge::GetFacePointSum(faces[1], this, facePsum[1])
    };
    if (0 == face_edge_count[0] || 0 == face_edge_count[1])
      return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);

    const unsigned int tagged_end
      = (ON_SubD::VertexTag::Smooth != edge_vertex[0]->m_vertex_tag)
      ? 0
      : ((ON_SubD::VertexTag::Smooth != edge_vertex[1]->m_vertex_tag) ? 1 : ON_UNSET_UINT_INDEX);
    double EP[3];
    if (
      ON_UNSET_UINT_INDEX == tagged_end
      || 0.5 == m_sector_coefficient[tagged_end]
      || (ON_SubD::EdgeTag::SmoothX == m_edge_tag)
      )
    {
      // ignore edge weights
      EP[0] = 0.375*edgePsum[0];
      EP[1] = 0.375*edgePsum[1];
      EP[2] = 0.375*edgePsum[2];
    }
    else if (ON_SubD::VertexTag::Smooth == edge_vertex[1 - tagged_end]->m_vertex_tag
      && m_sector_coefficient[tagged_end] > 0.0
      && m_sector_coefficient[tagged_end] < 1.0
      )
    {
      double w[2];
      w[tagged_end] = m_sector_coefficient[tagged_end];
      w[1 - tagged_end] = 1.0 - w[tagged_end];
      EP[0] = 0.75*(w[0] * edgeP[0][0] + w[1] * edgeP[1][0]);
      EP[1] = 0.75*(w[0] * edgeP[0][1] + w[1] * edgeP[1][1]);
      EP[2] = 0.75*(w[0] * edgeP[0][2] + w[1] * edgeP[1][2]);
    }
    else
    {
      // error:
      //   Both ends of a smooth vertex are tagged
      //   or weights are incorrectly set
      //   or ...
      return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);
    }

    if (4 == face_edge_count[0] && 4 == face_edge_count[1])
    {
      // common case when both neighboring faces are quads
      subdivision_point[0] = EP[0] + 0.0625*(facePsum[0][0] + facePsum[1][0]);
      subdivision_point[1] = EP[1] + 0.0625*(facePsum[0][1] + facePsum[1][1]);
      subdivision_point[2] = EP[2] + 0.0625*(facePsum[0][2] + facePsum[1][2]);

      if (bApplyDisplacement)
      {
        subdivision_point[0] += displacementV[0];
        subdivision_point[1] += displacementV[1];
        subdivision_point[2] += displacementV[2];
      }

      return true;
    }
    
    if (3 == face_edge_count[0] && 3 == face_edge_count[1])
    {
      // common case when both neighboring faces are triangles

      //// bug in evaluation prior to Nov 11, 2019
      ////subdivision_point[0] = EP[0] + 0.125*(facePsum[0][0] + facePsum[1][0]);
      ////subdivision_point[1] = EP[1] + 0.125*(facePsum[0][1] + facePsum[1][1]);
      ////subdivision_point[2] = EP[2] + 0.125*(facePsum[0][2] + facePsum[1][2]);
      subdivision_point[0] = EP[0] + (0.5*edgePsum[0] + facePsum[0][0] + facePsum[1][0]) / 12.0;
      subdivision_point[1] = EP[1] + (0.5*edgePsum[1] + facePsum[0][1] + facePsum[1][1]) / 12.0;
      subdivision_point[2] = EP[2] + (0.5*edgePsum[2] + facePsum[0][2] + facePsum[1][2]) / 12.0;

      if (bApplyDisplacement)
      {
        subdivision_point[0] += displacementV[0];
        subdivision_point[1] += displacementV[1];
        subdivision_point[2] += displacementV[2];
      }

      return true;
    }

    // general formula works for all cases including face_edge_count[0] != face_count[2]
      //// bug in evaluation prior to Nov 11, 2019
    ////const double f0 = 0.125 / ((double)(face_edge_count[0] - 2));
    ////const double f1 = 0.125 / ((double)(face_edge_count[1] - 2));
    ////subdivision_point[0] = EP[0] + f0 * facePsum[0][0] + f1 * facePsum[1][0];
    ////subdivision_point[1] = EP[1] + f0 * facePsum[0][1] + f1 * facePsum[1][1];
    ////subdivision_point[2] = EP[2] + f0 * facePsum[0][2] + f1 * facePsum[1][2];

    const double f0 = (double)(face_edge_count[0] * 4U);
    const double f1 = (double)(face_edge_count[1] * 4U);
    const double x = 1.0 / f0 + 1.0 / f1 - 0.125;
    subdivision_point[0] = EP[0] + x * edgePsum[0] + facePsum[0][0] / f0 + facePsum[1][0] / f1;
    subdivision_point[1] = EP[1] + x * edgePsum[1] + facePsum[0][1] / f0 + facePsum[1][1] / f1;
    subdivision_point[2] = EP[2] + x * edgePsum[2] + facePsum[0][2] / f0 + facePsum[1][2] / f1;

    if (bApplyDisplacement)
    {
      subdivision_point[0] += displacementV[0];
      subdivision_point[1] += displacementV[1];
      subdivision_point[2] += displacementV[2];
    }

    return true;
  }

  if ( IsCrease() )
  {
    subdivision_point[0] = 0.5*edgePsum[0];
    subdivision_point[1] = 0.5*edgePsum[1];
    subdivision_point[2] = 0.5*edgePsum[2];

    if (bApplyDisplacement)
    {
      subdivision_point[0] += displacementV[0];
      subdivision_point[1] += displacementV[1];
      subdivision_point[2] += displacementV[2];
    }

    return true;
  }

  // invalid edge->m_edge_tag
  return ON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);
}











static unsigned int GetSectorBoundaryEdgesError()
{
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubDEdge::GetSectorBoundaryEdges(
  unsigned int edge_vertex_index,
  ON_SubDEdgePtr* edge_ptr0,
  ON_SubDEdgePtr* edge_ptr1
  ) const
{
  if (nullptr != edge_ptr0)
    *edge_ptr0 = ON_SubDEdgePtr::Null;
  if (nullptr != edge_ptr1)
    *edge_ptr1 = ON_SubDEdgePtr::Null;

  const unsigned int edge_face_count = m_face_count;
  if (edge_face_count <= 0 || edge_face_count > 2)
    return GetSectorBoundaryEdgesError();

  if (2 == edge_face_count && ON_SubD::EdgeTag::Crease == m_edge_tag)
    return GetSectorBoundaryEdgesError();

  if (0 != edge_vertex_index && 1 != edge_vertex_index)
    return GetSectorBoundaryEdgesError();

  const ON_SubDVertex* vertex = m_vertex[edge_vertex_index];
  if (nullptr == vertex || vertex->m_face_count <= 0)
    return GetSectorBoundaryEdgesError();

  const unsigned int vertex_face_count = vertex->m_face_count;
  unsigned int sector_face_count = 0;
  ON_SubDEdgePtr sector_boundary[2] = {};
  for (unsigned int edge_face_index = 0; edge_face_index < edge_face_count; edge_face_index++)
  {
    const ON_SubDEdge* edge0 = this;
    unsigned int edge0_end_index = edge_vertex_index;
    unsigned int edge0_face_index = edge_face_index;
    ON_SubDFacePtr face_ptr = edge0->m_face2[edge0_face_index];
    while (sector_face_count < vertex_face_count)
    {
      const ON_SubDFace* face = ON_SUBD_FACE_POINTER(face_ptr.m_ptr);
      if (0 == face)
        return GetSectorBoundaryEdgesError();
      ON__UINT_PTR face_dir = ON_SUBD_FACE_DIRECTION(face_ptr.m_ptr);

      sector_face_count++;

      unsigned int face_edge0_index = face->EdgeArrayIndex(edge0);
      if (ON_UNSET_UINT_INDEX == face_edge0_index)
        return 0;

      unsigned int face_edge1_index
        = face_edge0_index;
      face_edge1_index +=
        (1 == (edge0_end_index + face_dir))
        ? 1
        : (face->m_edge_count - 1);
      face_edge1_index %= face->m_edge_count;

      ON_SubDEdgePtr edge1_ptr = face->EdgePtr(face_edge1_index);
      const ON_SubDEdge* edge1 = ON_SUBD_EDGE_POINTER(edge1_ptr.m_ptr);
      if (nullptr == edge1)
        return GetSectorBoundaryEdgesError();

      unsigned int edge1_end_index = (0 == face_dir) ? (1 - edge0_end_index) : edge0_end_index;
      if (1 == ON_SUBD_EDGE_DIRECTION(edge1_ptr.m_ptr))
        edge1_end_index = 1 - edge1_end_index;
      if (vertex != edge1->m_vertex[edge1_end_index])
        return GetSectorBoundaryEdgesError();

      if ( edge1->IsSmooth() && 2 == edge1->m_face_count )
      {
        const ON_SubDFace* edge1_faces[2] = { ON_SUBD_FACE_POINTER(edge1->m_face2[0].m_ptr), ON_SUBD_FACE_POINTER(edge1->m_face2[1].m_ptr) };
        unsigned int edge1_face_index = (face == edge1_faces[0] ? 1 : 0);
        if (nullptr == edge1_faces[edge1_face_index] || face == edge1_faces[edge1_face_index])
          return GetSectorBoundaryEdgesError();
        face_ptr = edge1->m_face2[edge1_face_index];
        edge0 = edge1;
        edge0_face_index = edge1_face_index;
        edge0_end_index = edge1_end_index;
        continue;
      }

      sector_boundary[edge_face_index] = ON_SubDEdgePtr::Create(edge1, edge1_end_index);
      break;
    }
  }
  
  if (sector_face_count <= 0 || sector_boundary[0].IsNull())
    return GetSectorBoundaryEdgesError();
  if (1 == edge_face_count)
    sector_boundary[1] = ON_SubDEdgePtr::Create(this, edge_vertex_index);
  else if (sector_boundary[1].IsNull())
    return GetSectorBoundaryEdgesError();

  if (nullptr != edge_ptr0)
    *edge_ptr0 = sector_boundary[0];
  if (nullptr != edge_ptr1)
    *edge_ptr1 = sector_boundary[1];

  return sector_face_count;
}

class ON_ScratchBuffer
{
public:
  ON_ScratchBuffer(
    size_t sizeof_buffer,
    void* stack_buffer,
    size_t sizeof_stack_buffer
    )
    : m_buffer(nullptr)
    , m_heap_buffer(nullptr)
  {
    m_buffer
      = (sizeof_buffer > sizeof_stack_buffer || nullptr == stack_buffer)
      ? stack_buffer
      : (m_heap_buffer = new (std::nothrow) double[1 + sizeof_buffer / sizeof(double)]);
  }

  void* Buffer()
  {
    return m_buffer;
  }

  ~ON_ScratchBuffer()
  {
    if (nullptr != m_heap_buffer)
    {
      double* p = m_heap_buffer;
      m_heap_buffer = nullptr;
      delete[] p;
    }
  }

private:
  void* m_buffer;
  double* m_heap_buffer;

private:
  // prohibit use - no implementation
  ON_ScratchBuffer(const ON_ScratchBuffer&);
  ON_ScratchBuffer& operator-(const ON_ScratchBuffer&);
};

class FACE_AND_FACE_POINT
{
public:
  const ON_SubDFace* m_face;
  double m_faceP[3];
  static int CompareFacePointer(const void* a, const void* b);
};

int FACE_AND_FACE_POINT::CompareFacePointer(const void* a, const void* b)
{
  ON__UINT_PTR fa = (ON__UINT_PTR)(((const FACE_AND_FACE_POINT*)a)->m_face);
  ON__UINT_PTR fb = (ON__UINT_PTR)(((const FACE_AND_FACE_POINT*)b)->m_face);
  if (fa < fb)
    return -1;
  if (fa > fb)
    return 1;
  return 0;
}

bool ON_SubDSectorSurfacePoint::IsUnset() const
{
  return (m_limitP[0] == ON_UNSET_VALUE);
}

bool ON_SubDSectorSurfacePoint::IsNan() const
{
  return !(m_limitP[0] == m_limitP[0]);
}

bool ON_SubDSectorSurfacePoint::IsZero() const
{
  const double* p = m_limitP;
  const double* p1 = p+12;
  while (p < p1)
  {
    if (!(0.0 == *p++))
      return false;
  }
  return true;
}

bool ON_SubDSectorSurfacePoint::IsSet(
  bool bUndefinedNormalIsPossible
) const
{
  double x, y;
  const double* p = m_limitP;
  const double* p1 = p+3;
  while (p < p1)
  {
    x = *p++;
    if (ON_UNSET_VALUE == x || !(x == x))
      return false;
  }

  p = m_limitT1;
  p1 = p + 6;
  while (p < p1)
  {
    const double* p2 = p + 3;
    y = 0.0;
    while (p < p2)
    {
      x = *p++;
      if (ON_UNSET_VALUE == x || !(x == x))
        return false;
      if (0.0 != x)
        y = x;
    }
    if (false == bUndefinedNormalIsPossible)
    {
      if (!(y != 0.0))
        return false;
    }

    p = m_limitN;
    p1 = p + 3;
    y = 0.0;
    while (p < p1)
    {
      x = *p++;
      if (ON_UNSET_VALUE == x || !(x == x))
        return false;
      y += x * x;
    }
    if (false == bUndefinedNormalIsPossible)
    {
      if (!(fabs(y - 1.0) <= 1e-4))
        return false;
    }
  }

  return true;
}

void ON_SubDVertex::CopyFrom(
  const ON_SubDVertex* src,
  bool bCopyEdgeArray,
  bool bCopyFaceArray,
  bool bCopyLimitPointList
  )
{
  if (nullptr == src)
    src = &ON_SubDVertex::Empty;

  ClearSavedSubdivisionPoints();
  CopyBaseFrom(src);

  m_vertex_tag = src->m_vertex_tag;

  m_P[0] = src->m_P[0];
  m_P[1] = src->m_P[1];
  m_P[2] = src->m_P[2];


  if (bCopyLimitPointList)
  {
    if ( src->SurfacePointIsSet() )
    {
      for (const ON_SubDSectorSurfacePoint* p = &src->m_limit_point; nullptr != p; p = p->m_next_sector_limit_point)
      {
        ON_SubDSectorSurfacePoint limit_point = *p;
        limit_point.m_next_sector_limit_point = (ON_SubDSectorSurfacePoint*)1; // disable checks
        SetSavedSurfacePoint( true, limit_point);
      }
    }
  }

  if (bCopyEdgeArray)
  {
    if (src->m_edge_count > 0 && nullptr != src->m_edges && nullptr != m_edges && src->m_edge_count <= m_edge_capacity)
    {
      m_edge_count = src->m_edge_count;
      const unsigned int edge_count = src->m_edge_count;
      for (unsigned int vei = 0; vei < edge_count; vei++)
        m_edges[vei] = src->m_edges[vei];
    }
    else
      m_edge_count = 0;
  }

  if (bCopyFaceArray)
  {
    if (src->m_face_count > 0 && nullptr != src->m_faces && nullptr != m_faces && src->m_face_count <= m_face_capacity)
    {
      m_face_count = src->m_face_count;
      const unsigned int face_count = src->m_face_count;
      for (unsigned int vfi = 0; vfi < face_count; vfi++)
        m_faces[vfi] = src->m_faces[vfi];
    }
    else
      m_face_count = 0;
  }
}


static bool ON_SubDVertex_GetSubdivisionPointError(
  const class ON_SubDVertex* vertex,
  double vertex_point[3],
  const double* vertexP,
  bool bDamagedState
  )
{
  if (nullptr == vertex || nullptr == vertex_point)
    return false; // caller passed a null pointer - vertex is not necessarily damaged

  ON_SubDIncrementErrorCount();
  vertex->m_status.SetDamagedState(bDamagedState);

  vertex->ClearSavedSubdivisionPoints();

  if (nullptr != vertexP)
  {
    vertex_point[0] = vertexP[0];
    vertex_point[1] = vertexP[1];
    vertex_point[2] = vertexP[2];
  }

  return true;
}

bool ON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(
  const class ON_SubDVertex* vertex,
  double vertex_point[3]
)
{
  if (nullptr != vertex_point)
  {
    vertex_point[0] = ON_DBL_QNAN;
    vertex_point[1] = ON_DBL_QNAN;
    vertex_point[2] = ON_DBL_QNAN;
  }

  if (nullptr == vertex)
  {
    ON_SUBD_ERROR("input vertex is nullptr.");
    return false;
  }


  const unsigned int n = vertex->m_face_count;
  if (nullptr == vertex
    || nullptr == vertex->m_faces
    || nullptr == vertex->m_edges
    || vertex->m_face_count != vertex->m_edge_count
    || n < ON_SubDSectorType::MinimumSectorFaceCount(ON_SubD::VertexTag::Smooth)
    )
  {
    ON_SUBD_ERROR("input vertex is not valid.");
    return false;
  }

  const double* vertexP = vertex->m_P;


  // It is critical to use the centroids of the neighboring faces
  // in this step because the number of edges in each face's
  // boundary may not be constant.
  double facePsum[3] = { 0 };
  const ON_SubDFace*const* vertex_faces = vertex->m_faces;
  for (unsigned int i = 0; i < n; i++)
  {
    const ON_SubDFace* face = vertex_faces[i];
    if (nullptr != face)
    {
      double faceC[3];
      if (face->GetSubdivisionPoint( faceC))
      {
        facePsum[0] += faceC[0];
        facePsum[1] += faceC[1];
        facePsum[2] += faceC[2];
        continue;
      }
    }
    // treat missing or damaged face as infinitesimally small
    facePsum[0] += vertexP[0];
    facePsum[1] += vertexP[1];
    facePsum[2] += vertexP[2];
  }

  double edgePsum[3] = { 0 };
  class ON_SubDEdgePtr* edges = vertex->m_edges;
  for (unsigned int i = 0; i < n; i++)
  {
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(edges[i].m_ptr);
    if (nullptr != edge)
    {
      const ON_SubDVertex* edge_vertex = edge->OtherEndVertex(vertex);
      if (nullptr != edge_vertex)
      {
        const double* edgeP = edge_vertex->m_P;
        edgePsum[0] += edgeP[0];
        edgePsum[1] += edgeP[1];
        edgePsum[2] += edgeP[2];
        continue;
      }
    }
    // treat missing or damaged edge as infinitesimally small
    edgePsum[0] += vertexP[0];
    edgePsum[1] += vertexP[1];
    edgePsum[2] += vertexP[2];
  }

  const double v_weight = 1.0 - 2.0 / ((double)n);
  const double ef_weight = 1.0 / ((double)(n*n));
  vertex_point[0] = v_weight*vertexP[0] + ef_weight*(edgePsum[0] + facePsum[0]);
  vertex_point[1] = v_weight*vertexP[1] + ef_weight*(edgePsum[1] + facePsum[1]);
  vertex_point[2] = v_weight*vertexP[2] + ef_weight*(edgePsum[2] + facePsum[2]);

  return true;
}

bool ON_SubDVertex::Internal_GetCatmullClarkSubdivisionPoint(
  const class ON_SubDVertex* vertex,
  double vertex_point[3]
  )
{
  // This function is used to convert an arbitrary control polygon into the
  // "level 1" quad subD.  It cannot use the faster sub-D formulas because
  // a face can have an arbitrary number of edges.
  if (nullptr == vertex || nullptr == vertex_point)
    return ON_SubDVertex_GetSubdivisionPointError(vertex,vertex_point,nullptr,false);

  const double* vertexP = vertex->m_P;

  const unsigned int n = (nullptr != vertex->m_edges ? vertex->m_edge_count : 0);
  if (ON_SubD::VertexTag::Smooth == vertex->m_vertex_tag || ON_SubD::VertexTag::Dart == vertex->m_vertex_tag)
  {
    const unsigned int minimum_n = ON_SubDSectorType::MinimumSectorEdgeCount(vertex->m_vertex_tag);
    if (n < minimum_n || n != vertex->m_face_count || nullptr == vertex->m_faces)
      return ON_SubDVertex_GetSubdivisionPointError(vertex, vertex_point, vertexP, true);
    
    double facePsum[3] = { 0 };
    const ON_SubDFace*const* vertex_faces = vertex->m_faces;

    const ON_SubDFace* face = vertex_faces[0];
    if (nullptr == face)
      return ON_SubDVertex_GetSubdivisionPointError(vertex, vertex_point, vertexP, true);

    ////// for debugging code below, uncomment this line
    ////// and look for differences in results.
    ////return GetGeneralQuadSubdivisionPoint(vertex, vertex_point);

    const unsigned int k = (nullptr == face) ? 0U : face->m_edge_count;
    if (4 == k)
    {
      // possibly (probably?) every face is a quad
      double sum[3];
      for (unsigned int i = 0; i < n; i++)
      {
        const ON_SubDFace* vface = vertex_faces[i];
        const unsigned int face_n = ON_SubDVertex::Internal_GetFacePointSum(vface, vertex, sum);
        if (4 != face_n)
        {
          // The first face is a quadrangle and this face is not a quadrangle.
          //
          // It is critical to use the centroids of the neighboring faces
          // for this vertex subdivision point because the number of edges
          // in each face's boundary is not constant.
          return ON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(vertex, vertex_point);
        }
        facePsum[0] += sum[0];
        facePsum[1] += sum[1];
        facePsum[2] += sum[2];
      }
    }
    else if (3 == k)
    {
      // possibly (probably?) every face is a triangle
      for (unsigned int i = 0; i < n; i++)
      {
        const ON_SubDFace* vface = vertex_faces[i];
        if (k != ((nullptr == vface) ? 0U : vface->m_edge_count))
        {
          // The first face is a triangle and this face is not a triangle.
          //
          // It is critical to use the centroids of the neighboring faces
          // for this vertex subdivision point because the number of edges
          // in each face's boundary is not constant.
          return ON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(vertex, vertex_point);
        }
      }
    }
    else
    {
      // The first face has 5 or more edges.  
      // It is likely this is the initial subdivision being applied
      // to the level zero SubD control polygon.
      //
      // It may be critical to use the centroids of the neighboring faces
      // for this vertex subdivision point because the number of edges
      // in each face's boundary may not constant.  In any case, this
      // situation is not common and typically happens only on the
      // first subdivision step.
      return ON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(vertex, vertex_point);
    }

    double edgePsum[3] = { 0 };
    class ON_SubDEdgePtr* edges = vertex->m_edges;
    for (unsigned int i = 0; i < n; i++)
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(edges[i].m_ptr);
      if (nullptr != edge)
      {
        const ON_SubDVertex* edge_vertex = edge->OtherEndVertex(vertex);
        if (nullptr != edge_vertex)
        {
          const double* edgeP = edge_vertex->m_P;
          edgePsum[0] += edgeP[0];
          edgePsum[1] += edgeP[1];
          edgePsum[2] += edgeP[2];
          continue;
        }
      }
      // treat missing or damaged edge as infinitesimally small
      edgePsum[0] += vertexP[0];
      edgePsum[1] += vertexP[1];
      edgePsum[2] += vertexP[2];
    }

    if (4 == k)
    {
      // all faces were quads
      const double v_weight = 1.0 - 1.75 / ((double)n);
      const double e_weight = 1.5 / ((double)(n*n));
      const double f_weight = 0.25 / ((double)(n*n));
      vertex_point[0] = v_weight*vertexP[0] + e_weight*edgePsum[0] + f_weight*facePsum[0];
      vertex_point[1] = v_weight*vertexP[1] + e_weight*edgePsum[1] + f_weight*facePsum[1];
      vertex_point[2] = v_weight*vertexP[2] + e_weight*edgePsum[2] + f_weight*facePsum[2];
    }
    else
    {
      // all faces were triangles
      const double v_weight = 1.0 - 5.0 / ((double)(3 * n));
      const double e_weight = 5.0 / ((double)(3*n*n));
      vertex_point[0] = v_weight*vertexP[0] + e_weight*edgePsum[0];
      vertex_point[1] = v_weight*vertexP[1] + e_weight*edgePsum[1];
      vertex_point[2] = v_weight*vertexP[2] + e_weight*edgePsum[2];
    }

    return true;
  }

  // vertex->m_vertex_tag is damaged
  return ON_SubDVertex_GetSubdivisionPointError(vertex, vertex_point, vertexP, true);
}


const ON_3dPoint ON_SubDVertex::SubdivisionPoint() const
{
  ON_3dPoint S;
  return (GetSubdivisionPoint(&S.x) && S.IsValid()) ? S : ON_3dPoint::NanPoint;
}

bool ON_SubDVertex::GetSubdivisionPoint(
  double subdivision_point[3]
) const
{
  if (nullptr == subdivision_point)
    return ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  if (GetSavedSubdivisionPoint(subdivision_point))
    return true;

  if (EvaluateCatmullClarkSubdivisionPoint(subdivision_point))
  {
    SetSavedSubdivisionPoint(subdivision_point);
    return true;
  }

  return false;
}

bool ON_SubDVertex::EvaluateCatmullClarkSubdivisionPoint(double subdivision_point[3]) const
{
  // This function is used to convert an arbitrary control polygon into the
  // "level 1" subD.  It cannot use the faster sub-D formulas because
  // a face can have an arbitrary number of edges.
  if (nullptr == subdivision_point )
    return ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  double displacementV[3] = { 0 };
  const bool bApplyDisplacement = GetSubdivisionDisplacement(displacementV);

  const double* vertexP = m_P;

  const unsigned int n = (nullptr != m_edges ? m_edge_count : 0);
  if (n < 2)
    return ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);

  if (ON_SubD::VertexTag::Smooth == m_vertex_tag || ON_SubD::VertexTag::Dart == m_vertex_tag)
  {
    return ON_SubDVertex::Internal_GetCatmullClarkSubdivisionPoint(this, subdivision_point);
  }

  if (ON_SubD::VertexTag::Crease == m_vertex_tag)
  {
    class ON_SubDEdgePtr* edges = m_edges;
    const ON_SubDVertex* edge0_vertex = nullptr;

    for (unsigned int i = 0; i < n; i++)
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(edges[i].m_ptr);
      if (nullptr == edge)
      {
        ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
        continue;
      }

      if (ON_SubD::EdgeTag::Crease != edge->m_edge_tag)
        continue;
      const ON_SubDVertex* edge_vertex = edge->OtherEndVertex(this);

      if (nullptr == edge_vertex)
      {
        ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
        continue;
      }

      if (nullptr == edge0_vertex)
      {
        edge0_vertex = edge_vertex;
        continue;
      }

      if (edge0_vertex == edge_vertex)
      {
        ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
        continue;
      }

      // We found the two crease edges that share this crease vertex.
      // (The parenthesis around the edgeP sum is to insure this result 
      // is independent of the order of the edges.)
      vertexP = m_P;
      const double* edgeP[2] = { edge0_vertex->m_P, edge_vertex->m_P };
      subdivision_point[0] = (vertexP[0] * 6.0 + (edgeP[0][0] + edgeP[1][0]))*0.125;
      subdivision_point[1] = (vertexP[1] * 6.0 + (edgeP[0][1] + edgeP[1][1]))*0.125;
      subdivision_point[2] = (vertexP[2] * 6.0 + (edgeP[0][2] + edgeP[1][2]))*0.125;

      if (bApplyDisplacement)
      {
        subdivision_point[0] += displacementV[0];
        subdivision_point[1] += displacementV[1];
        subdivision_point[2] += displacementV[2];
      }

      return true;
    }

    return ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
  }

  if (ON_SubD::VertexTag::Corner == m_vertex_tag)
  {
    vertexP = m_P;
    subdivision_point[0] = vertexP[0];
    subdivision_point[1] = vertexP[1];
    subdivision_point[2] = vertexP[2];

    if (bApplyDisplacement)
    {
      subdivision_point[0] += displacementV[0];
      subdivision_point[1] += displacementV[1];
      subdivision_point[2] += displacementV[2];
    }

    return true;
  }

  // vertex is damaged
  return ON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
}

unsigned int ON_SubDVertex::Internal_GetFacePointSum(
  const ON_SubDFace* face,
  const ON_SubDVertex* vertex,
  double* facePsum
  )
{
  const ON_SubDEdge* e;
  ON__UINT_PTR e_ptr, edir;
  const double* faceP;

  if (nullptr == face)
    return 0;

  const unsigned int n = face->m_edge_count;

  facePsum[0] = 0.0;
  facePsum[1] = 0.0;
  facePsum[2] = 0.0;

  if (3 == n)
  {
    return n;
  }

  if (4 == n)
  {
    for (unsigned int i = 0; i < 4; i++)
    {
      e_ptr = face->m_edge4[i].m_ptr;
      e = ON_SUBD_EDGE_POINTER(e_ptr);
      if (nullptr != e && (vertex == e->m_vertex[0] || vertex == e->m_vertex[1]))
      {
        edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
        e_ptr = face->m_edge4[(i + ((vertex == e->m_vertex[edir]) ? 2 : 3)) % 4].m_ptr;
        e = ON_SUBD_EDGE_POINTER(e_ptr);
        edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
        if (nullptr == e || nullptr == e->m_vertex[edir])
          return 0;
        faceP = e->m_vertex[edir]->m_P;
        facePsum[0] = faceP[0];
        facePsum[1] = faceP[1];
        facePsum[2] = faceP[2];
        return n;
      }
    }
    return 0;
  }

  if (n <= 4 || nullptr == face->m_edgex)
    return 0;

  e_ptr = face->m_edgex[n-5].m_ptr;
  e = ON_SUBD_EDGE_POINTER(e_ptr);
  if (nullptr == e)
    return 0;
  edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
  unsigned int skipped_edge_count = (vertex == e->m_vertex[edir]) ? 1 : 0;
  unsigned int facePcount = 0;
  const ON_SubDEdgePtr* edge_ptrs = face->m_edge4;
  for (unsigned int i = skipped_edge_count; i < n; i++)
  {
    if (4 == i)
      edge_ptrs = face->m_edgex - 4;
    e_ptr = edge_ptrs[i].m_ptr;
    e = ON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e)
      return 0;
    edir = ON_SUBD_EDGE_DIRECTION(e_ptr);
    if (vertex == e->m_vertex[0] || vertex == e->m_vertex[1])
    {
      skipped_edge_count++;
      if (skipped_edge_count > 2)
      {
        facePsum[0] = 0.0;
        facePsum[1] = 0.0;
        facePsum[2] = 0.0;
        return 0;
      }
      if (vertex == e->m_vertex[edir])
      {
        i++;
        if (4 == i)
          edge_ptrs = face->m_edgex - 4;
      }
      continue;
    }
    faceP = e->m_vertex[edir]->m_P;
    facePsum[0] += faceP[0];
    facePsum[1] += faceP[1];
    facePsum[2] += faceP[2];
    facePcount++;
  }

  if (n == facePcount + 3)
    return n;

  facePsum[0] = 0.0;
  facePsum[1] = 0.0;
  facePsum[2] = 0.0;
  return 0;
}

bool ON_SubDimple::LocalSubdivide(
  ON_SubDFace const*const* face_list,
  size_t face_list_count
)
{
  if (nullptr == face_list || face_list_count < 1 || m_levels.UnsignedCount() < 1)
    return false;

  unsigned int level0_index = ON_UNSET_UINT_INDEX;
  for (size_t i = 0; i < face_list_count; ++i)
  {
    const ON_SubDFace* f = face_list[i];
    if (nullptr == f || f->m_edge_count < 3 || f->SubdivisionLevel() >= m_levels.UnsignedCount())
      continue;
    level0_index = f->SubdivisionLevel();
    break;
  }
  
  if (level0_index >= m_levels.UnsignedCount() || nullptr == m_levels[level0_index])
    return ON_SUBD_RETURN_ERROR(false);

  ClearHigherSubdivisionLevels(level0_index + 1);
  if (level0_index + 1 != m_levels.UnsignedCount())
    return ON_SUBD_RETURN_ERROR(false);

  m_active_level = m_levels[level0_index];
  if ( nullptr == m_active_level || 0 == m_active_level->m_face_count)
    return ON_SUBD_RETURN_ERROR(false);

  ON_SubDLevel& level0 = *m_levels[level0_index];
  m_active_level = &level0;

  level0.ClearRuntimeMarks(true, true, true);

  unsigned face_count = 0;
  unsigned edge_count = 0;
  unsigned vertex_count = 0;
  ON_3dPoint P;
  for (size_t i = 0; i < face_list_count; ++i)
  {
    const ON_SubDFace* f = face_list[i];
    if (nullptr == f || f->m_edge_count < 3 || level0_index != f->SubdivisionLevel() )
      continue;
    if (f->m_status.RuntimeMark())
      continue;
    f->m_status.SetRuntimeMark();
    if ( false == f->GetSubdivisionPoint(P))
      return ON_SUBD_RETURN_ERROR(false);
    const ON_SubDEdgePtr* eptr = f->m_edge4;
    for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++eptr)
    {
      if (4 == fei)
      {
        eptr = f->m_edgex;
        if (nullptr == eptr)
          return ON_SUBD_RETURN_ERROR(false);
      }
      ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
      if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
        return ON_SUBD_RETURN_ERROR(false);
      if (e->m_status.RuntimeMark())
        continue;
      e->UpdateEdgeSectorCoefficientsForExperts(true);
      e->m_status.SetRuntimeMark();
      if (false == e->GetSubdivisionPoint(P))
        return ON_SUBD_RETURN_ERROR(false);
      for (unsigned evi = 0; evi < 2; ++evi)
      {
        if (e->m_vertex[evi]->m_status.RuntimeMark())
          continue;
        e->m_vertex[evi]->m_status.SetRuntimeMark();
        if ( false == e->m_vertex[evi]->GetSubdivisionPoint(P) )
          return ON_SUBD_RETURN_ERROR(false);
        ++vertex_count;
      }
      ++edge_count;
    }
    ++face_count;
  }
  if (face_count < 1 || edge_count < 3 || vertex_count < 3)
    return false;
  if (face_count >= level0.m_face_count)
    return GlobalSubdivide();

  // Get face subdivision points
  ON_SimpleArray<ON_SubDFace*> faces(face_count);
  ON_SimpleArray<ON_3dPoint> face_points(face_count);
  for (const ON_SubDFace* f0 = level0.m_face[0]; nullptr != f0; f0 = f0->m_next_face)
  {
    if (false == f0->m_status.RuntimeMark())
      continue;
    faces.Append(const_cast<ON_SubDFace*>(f0));
    P = f0->SubdivisionPoint();
    if ( false == P.IsValid())
      return ON_SUBD_RETURN_ERROR(false);
    face_points.Append(P);
  }
  if (face_count != face_points.UnsignedCount())
    return ON_SUBD_RETURN_ERROR(false);

  // Get edge subdivision points
  ON_SimpleArray<ON_3dPoint> edge_points(edge_count);
  ON_SimpleArray<ON_SubDEdge*> edges(edge_count);
  for (const ON_SubDEdge* e0 = level0.m_edge[0]; nullptr != e0; e0 = e0->m_next_edge)
  {
    if (false == e0->m_status.RuntimeMark())
      continue;
    P = (e0->IsSmooth() && 2 == e0->m_face_count && 2 == e0->MarkedFaceCount()) ? e0->SubdivisionPoint() : e0->ControlNetCenterPoint();
    edge_points.Append(P);
    edges.Append(const_cast<ON_SubDEdge*>(e0));
  }
  if ( edge_count != edge_points.UnsignedCount())
    return ON_SUBD_RETURN_ERROR(false);

  // Set vertex points
  for (const ON_SubDVertex* v0 = level0.m_vertex[0]; nullptr != v0; v0 = v0->m_next_vertex)
  {
    if (false == v0->m_status.RuntimeMark() || v0->m_edge_count < 2 || ((unsigned)v0->m_edge_count) != v0->MarkedEdgeCount())
      continue;
    P = v0->SubdivisionPoint();
    ON_SubDVertex* v = const_cast<ON_SubDVertex*>(v0);
    v->m_P[0] = P.x;
    v->m_P[1] = P.y;
    v->m_P[2] = P.z;
  }

  // split edges
  for (unsigned edex = 0; edex < edge_count; ++edex)
  {
    ON_SubDEdge* e = edges[edex];    
    e->EdgeModifiedNofification();
    const ON_SubDEdge* new_edge = SplitEdge(e, edge_points[edex]);
    if (nullptr == new_edge)
      return ON_SUBD_RETURN_ERROR(false);
    new_edge->m_status.ClearRuntimeMark();
    e->m_status.SetRuntimeMark();
  }

  ON_SimpleArray<ON_SubDEdgePtr> fbdry(32);
  ON_SimpleArray<ON_SubDEdge*> radial_edges(32);
  for (unsigned fdex = 0; fdex < face_count; ++fdex)
  {
    ON_SubDFace* f = faces[fdex];
    f->FaceModifiedNofification();
    P = face_points[fdex];
    fbdry.SetCount(0);
    const unsigned e_count = f->GetEdgeArray(fbdry);
    if (e_count < 6 || 0 != e_count %2)
      return ON_SUBD_RETURN_ERROR(false);

    // Get edges[] with edge[0] = marked edges.
    const ON_SubDEdgePtr* eptrs = fbdry.Array();
    if (nullptr != eptrs[0].RelativeVertex(0) && false == eptrs[0].RelativeVertex(0)->Mark() )
    {
      fbdry.Append(eptrs[0]);
      eptrs = fbdry.Array() + 1;
    }

    // save face status and candidate_face
    const ON_ComponentStatus fstatus = f->m_status;
    const ON_SubDFace* candidate_face = f;

    for (unsigned fei = 0; fei < e_count; ++fei)
    {
      ON_SubDEdge* e = eptrs[fei].Edge();
      if ( nullptr == e)
        return ON_SUBD_RETURN_ERROR(false);
      const ON_SubDVertex* ev[2] = { eptrs[fei].RelativeVertex(0), eptrs[fei].RelativeVertex(1) };
      if ( nullptr == ev[0] || nullptr == ev[1] || ev[0] == ev[1])
        return ON_SUBD_RETURN_ERROR(false);
      if (0 == fei % 2)
      {
        if (false == ev[0]->Mark())
          return ON_SUBD_RETURN_ERROR(false);
        if (ev[1]->Mark())
          return ON_SUBD_RETURN_ERROR(false);
      }
      else
      {
        if (ev[0]->Mark())
          return ON_SUBD_RETURN_ERROR(false);
        if (false == ev[1]->Mark())
          return ON_SUBD_RETURN_ERROR(false);
      }
    }

    // remove face that will be subdivided;
    for (unsigned short fei = 0; fei < f->m_edge_count; ++fei)
    {
      eptrs[fei].Edge()->RemoveFaceFromArray(f);
      const_cast<ON_SubDVertex*>(eptrs[fei].RelativeVertex(0))->RemoveFaceFromArray(f);
    }
    ReturnFace(f);
    
    ON_SubDVertex* center = AllocateVertex(ON_SubD::VertexTag::Smooth, level0_index, &P.x);
    AddVertexToLevel(center);

    radial_edges.SetCount(0);
    radial_edges.Reserve(e_count /2);
    for (unsigned fei = 0; fei < e_count; fei += 2)
    {
      ON_SubDEdge* r = AddEdge(ON_SubD::EdgeTag::Smooth, center, ON_SubDSectorType::UnsetSectorCoefficient, const_cast<ON_SubDVertex*>(eptrs[fei].RelativeVertex(1)), ON_SubDSectorType::UnsetSectorCoefficient);
      radial_edges.Append(r);
    }

    ON_SubDEdge* r[2] = { nullptr,radial_edges[e_count / 2 - 1] };
    for (unsigned fei = 0; fei < e_count; fei += 2)
    {
      r[0] = r[1];
      r[1] = radial_edges[fei / 2];
      const ON_SubDEdgePtr qbdry[4] = {
        ON_SubDEdgePtr::Create(r[0],0),
        eptrs[(fei + e_count - 1) % e_count],
        eptrs[fei],
        ON_SubDEdgePtr::Create(r[1],1) 
      };
      ON_SubDFace* q = AddFace(candidate_face, 4, qbdry);
      candidate_face = nullptr;
      q->m_status = fstatus;
    }

  }

  level0.ClearRuntimeMarks(true, true, false);
  level0.ClearBoundingBox();
  level0.ClearEvaluationCache();
  level0.ClearEdgeFlags();

  return true; 
}

unsigned int ON_SubDimple::GlobalSubdivide()
{
  if (m_levels.UnsignedCount() <= 0)
    return ON_SUBD_RETURN_ERROR(0U);
  const unsigned int level0_index = m_levels.UnsignedCount()-1;

  if ( nullptr == m_levels[level0_index])
    return ON_SUBD_RETURN_ERROR(0U);

  ON_SubDLevel& level0 = *m_levels[level0_index];
  if (level0.IsEmpty())
    return ON_SUBD_RETURN_ERROR(0U);
  if ( level0.m_edge_count <= 0U )
    return ON_SUBD_RETURN_ERROR(0U);

  level0.UpdateEdgeSectorCoefficients(true);

  const unsigned int level1_index = level0_index+1;
  
  ON_SubDLevel* level1 = SubDLevel(level1_index,true);
  if ( nullptr == level1 )
    return ON_SUBD_RETURN_ERROR(0);

  double P[3];
  ON_SubDVertex* v;

  // Add face points
  for (const ON_SubDFace* f0 = level0.m_face[0]; nullptr != f0; f0 = f0->m_next_face)
  {
    if (false == f0->GetSubdivisionPoint(P))
      continue;
    if (nullptr == f0->m_subd_point1)
    {
      const_cast<ON_SubDFace*>(f0)->m_subd_point1 = v = AllocateVertex(ON_SubD::VertexTag::Smooth, level1_index, P);
      AddVertexToLevel(v);
    }
    else
    {
      v = const_cast<ON_SubDVertex*>(f0->m_subd_point1);
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }
  }

  // Add edge points
  for (const ON_SubDEdge* e0 = level0.m_edge[0]; nullptr != e0; e0 = e0->m_next_edge)
  {
    if (false == e0->GetSubdivisionPoint(P))
      continue;
    // (the subdivision point of an edge tagged as ON_SubD::EdgeTag::SmoothX is a smooth vertex.)
    const ON_SubD::VertexTag vertex_tag
      = ON_SubD::EdgeTag::Crease == e0->m_edge_tag
      ? ON_SubD::VertexTag::Crease
      : ON_SubD::VertexTag::Smooth;
    if (nullptr == e0->m_subd_point1)
    {
      const_cast<ON_SubDEdge*>(e0)->m_subd_point1 = v = AllocateVertex(vertex_tag, level1_index, P );
      AddVertexToLevel(v);
    }
    else
    {
      v = const_cast<ON_SubDVertex*>(e0->m_subd_point1);
      v->m_vertex_tag = vertex_tag;
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }
  }

  // Add vertex points
  for (const ON_SubDVertex* v0 = level0.m_vertex[0]; nullptr != v0; v0 = v0->m_next_vertex)
  {
    if (false == v0->GetSubdivisionPoint(P))
      continue;
    if (nullptr == v0->m_subd_point1)
    {
      const_cast<ON_SubDVertex*>(v0)->m_subd_point1 = v = AllocateVertex(v0->m_vertex_tag, level1_index, P);
      AddVertexToLevel(v);
    }
    else
    {
      v = const_cast<ON_SubDVertex*>(v0->m_subd_point1);
      v->m_vertex_tag = v0->m_vertex_tag;
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }    
  }

  // subdivide edges
  for (const ON_SubDEdge* e0 = level0.m_edge[0]; nullptr != e0; e0 = e0->m_next_edge)
  {
    if (nullptr == e0->m_subd_point1)
      continue;
    ON_SubDVertex* end_vertex[2] = { const_cast<ON_SubDVertex*>(e0->m_vertex[0]->m_subd_point1), const_cast<ON_SubDVertex*>(e0->m_vertex[1]->m_subd_point1) };
    ON_SubDVertex* mid_vertex = const_cast<ON_SubDVertex*>(e0->m_subd_point1);
    double w[2] = { e0->m_sector_coefficient[0], e0->m_sector_coefficient[1] };
    ON_SubD::EdgeTag edge_tag = e0->m_edge_tag;
    if (ON_SubD::EdgeTag::SmoothX == edge_tag && 2 == e0->m_face_count)
    {
      if ( nullptr != mid_vertex && ON_SubD::VertexTag::Smooth == mid_vertex->m_vertex_tag )
        edge_tag = ON_SubD::EdgeTag::Smooth;
    }
    AddEdge(edge_tag, end_vertex[0], w[0], mid_vertex, 0.0);
    AddEdge(edge_tag, mid_vertex, 0.0, end_vertex[1], w[1]);
  }

  for (const ON_SubDFace* f0 = level0.m_face[0]; nullptr != f0; f0 = f0->m_next_face)
  {
    Internal_GlobalQuadSubdivideFace(f0);
  }

  return level1_index;
}

unsigned int ON_SubDimple::Internal_GlobalQuadSubdivideFace(
  const ON_SubDFace* f0
  )
{
  // This is a private member function.  
  // The caller insures f0 != nullptr.

  const unsigned int f0_edge_count = f0->m_edge_count;
  if (f0_edge_count < 3)
    return 0;

  const unsigned int parent_face_id = f0->m_id;
  const unsigned int zero_face_id = (0 == f0->SubdivisionLevel()) ? parent_face_id : f0->m_zero_face_id;

  if (nullptr == f0->m_subd_point1)
  {
    // add face centroid
    double faceC[3];
    if (false == f0->GetSubdivisionPoint( faceC))
      return 0;
    f0->SetSavedSubdivisionPoint(faceC);
    unsigned int level1_index = f0->SubdivisionLevel() + 1;
    ON_SubDVertex* v = AllocateVertex(ON_SubD::VertexTag::Smooth, level1_index, faceC );
    AddVertexToLevel(v);
    const_cast<ON_SubDFace*>(f0)->m_subd_point1 = v;
  }


  ON_SubDEdge* E0[2];
  ON__UINT_PTR E0dir[2];
  ON_SubDEdge* E1[4];
  ON__UINT_PTR E1dir[4];
  ON_SubDEdgePtr f1edges[4];
  ON__UINT_PTR e_ptr;
  ON_SubDEdge* FirstE1(nullptr);
  double w;

  e_ptr = f0->EdgePtr(f0_edge_count - 1).m_ptr;
  E0[1] = ON_SUBD_EDGE_POINTER(e_ptr);
  E0dir[1] = ON_SUBD_EDGE_DIRECTION(e_ptr);
  E1[2] = nullptr;

  unsigned int f1_count = 0;

  const double w_2facesector = ON_SubDSectorType::CreaseSectorCoefficient(2);

  for (unsigned int i = 0; i < f0_edge_count; i++)
  {
    E0[0] = E0[1];
    E0dir[0] = E0dir[1];
    e_ptr = f0->EdgePtr(i).m_ptr;
    E0[1] = ON_SUBD_EDGE_POINTER(e_ptr);
    E0dir[1] = ON_SUBD_EDGE_DIRECTION(e_ptr);

    if (nullptr == E0[0] || nullptr == E0[1])
      continue;
    if (nullptr == E0[0]->m_subd_point1 || nullptr == E0[1]->m_subd_point1)
      continue;

    e_ptr = E0[0]->m_subd_point1->m_edges[0 == E0dir[0] ? 1 : 0].m_ptr;
    E1[0] = ON_SUBD_EDGE_POINTER(e_ptr);
    E1dir[0] = E0dir[0];

    e_ptr = E0[1]->m_subd_point1->m_edges[0 == E0dir[1] ? 0 : 1].m_ptr;
    E1[1] = ON_SUBD_EDGE_POINTER(e_ptr);
    E1dir[1] = E0dir[1];

    E1[3] = E1[2];
    if (nullptr == E1[3])
    {
      //  The value of E0[0]->m_subd_point1->m_vertex_tag should be either 
      //  ON_SubD::VertexTag::Smooth or ON_SubD::VertexTag::Crease. In the
      //  case when it's value is "crease", the resulting edge end weight
      //  will be 0.5 because the edge has two adjacent faces and "theta"
      //  will be pi/2.  
      //  The resulting quad edge weight is 0.5 = 1/2 + 1/3*cos(pi/2).
      w = (ON_SubD::VertexTag::Crease == E0[0]->m_subd_point1->m_vertex_tag) ? w_2facesector : 0.0;
      E1[3] = AddEdge(ON_SubD::EdgeTag::Smooth, const_cast<ON_SubDVertex*>(f0->m_subd_point1), 0.0, const_cast<ON_SubDVertex*>(E0[0]->m_subd_point1), w);
      if (nullptr == FirstE1)
        FirstE1 = E1[3];
    }
    E1dir[3] = 0;

    if (i + 1 < f0_edge_count || nullptr == FirstE1)
    {
      //  The value of E0[0]->m_subd_point1->m_vertex_tag should be either 
      //  ON_SubD::VertexTag::Smooth or ON_SubD::VertexTag::Crease. In the
      //  case when it's value is "crease", the resulting edge end weight
      //  will be zero because the edge has two adjacent faces and "theta"
      //  will be pi/2.  The resulting edge weight is 0.5.
      w = (ON_SubD::VertexTag::Crease == E0[1]->m_subd_point1->m_vertex_tag) ? w_2facesector : 0.0;
      E1[2] = AddEdge(ON_SubD::EdgeTag::Smooth, const_cast<ON_SubDVertex*>(f0->m_subd_point1), 0.0, const_cast<ON_SubDVertex*>(E0[1]->m_subd_point1), w);
    }
    else
    {
      E1[2] = FirstE1;
    }
    E1dir[2] = 1;

    f1edges[0] = ON_SubDEdgePtr::Create(E1[0], E1dir[0]);
    f1edges[1] = ON_SubDEdgePtr::Create(E1[1], E1dir[1]);
    f1edges[2] = ON_SubDEdgePtr::Create(E1[2], E1dir[2]);
    f1edges[3] = ON_SubDEdgePtr::Create(E1[3], E1dir[3]);

    ON_SubDFace* f1 = AddFace(4, f1edges);
    if (nullptr != f1)
    {
      f1->m_zero_face_id = zero_face_id;
      f1->m_parent_face_id = parent_face_id;
      f1_count++;
    }
  }

  // return number of new faces
  return f1_count;
}

bool ON_SubD::GlobalSubdivide()
{
  return GlobalSubdivide(1U);
}

bool ON_SubD::GlobalSubdivide(
  unsigned int count
)
{
  ON_SubDimple* subdimple = SubDimple(false);
  if (nullptr == subdimple)
    return ON_SUBD_RETURN_ERROR(false);
  return subdimple->GlobalSubdivide(count);
}

bool ON_SubD::LocalSubdivide(
  const ON_SimpleArray<ON_COMPONENT_INDEX>& face_list
)
{
  const int count = face_list.Count();
  ON_SimpleArray< const ON_SubDFace* > ptr_list(count);
  for (int i = 0; i < count; ++i)
  {
    const ON_COMPONENT_INDEX ci = face_list[i];
    if (ON_COMPONENT_INDEX::TYPE::subd_face != ci.m_type)
      continue;
    if (ci.m_index <= 0)
      continue;
    const ON_SubDFace* f = this->FaceFromId(ci.m_index);
    if (nullptr == f)
      continue;
    ptr_list.Append(f);
  }
  const bool rc = LocalSubdivide(ptr_list);
  UpdateAllTagsAndSectorCoefficients(true);
  return rc;
}

bool ON_SubD::LocalSubdivide(const ON_SimpleArray< const ON_SubDFace* >& face_list)
{
  return LocalSubdivide(face_list.Array(), face_list.UnsignedCount());
}

bool ON_SubD::LocalSubdivide( ON_SubDFace const*const* face_list, size_t face_count)
{
  ON_SubDimple* subdimple = SubDimple(false);
  if (nullptr == subdimple)
    return ON_SUBD_RETURN_ERROR(false);
  return subdimple->LocalSubdivide(face_list, face_count);
}

bool ON_SubDimple::GlobalSubdivide(
  unsigned int count
  )
{
  if (m_levels.UnsignedCount() < 1)
    return ON_SUBD_RETURN_ERROR(false);
  if (nullptr == m_active_level)
  {
    m_active_level = m_levels[m_levels.UnsignedCount() - 1];
    if (nullptr == m_active_level)
      return ON_SUBD_RETURN_ERROR(false);
  }

  const unsigned level0_index = m_active_level->m_level_index;
  if (level0_index >= m_levels.UnsignedCount() || nullptr == m_levels[level0_index])
    return ON_SUBD_RETURN_ERROR(false);

  if (count <= 0)
    return ON_SUBD_RETURN_ERROR(false);

  if (level0_index + count > ON_SubD::maximum_subd_level)
    return ON_SUBD_RETURN_ERROR(false);

  ClearHigherSubdivisionLevels(level0_index + 1);
  if (level0_index + 1 != m_levels.UnsignedCount() )
    return ON_SUBD_RETURN_ERROR(false);

  m_active_level = m_levels[level0_index];
 
  for (unsigned int i = level0_index +1; i <= level0_index +count; i++)
  {
    unsigned int rc = GlobalSubdivide();
    if (i != rc)
      return ON_SUBD_RETURN_ERROR(false);
    m_active_level = m_levels[i];
  }
  return true;
}

ON_SubDEdgePtr ON_SubDimple::MergeConsecutiveEdges(
  ON_SubDEdgePtr eptr0,
  ON_SubDEdgePtr eptr1
  )
{
  if ( false == ON_SubD::EdgesAreConsecutive(eptr0,eptr1) )
    return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);

  ON_SubDEdge* e[2] = { ON_SUBD_EDGE_POINTER(eptr0.m_ptr), ON_SUBD_EDGE_POINTER(eptr1.m_ptr) };
  ON__UINT_PTR edir[2] = { ON_SUBD_EDGE_DIRECTION(eptr0.m_ptr), ON_SUBD_EDGE_DIRECTION(eptr1.m_ptr) };
  const ON_SubDVertex* end_v[2] = {e[0]->m_vertex[edir[0]], e[1]->m_vertex[1 - edir[1]]};
  if (nullptr == end_v[0] || nullptr == end_v[1] || end_v[0] == end_v[1] )
    return ON_SUBD_RETURN_ERROR(ON_SubDEdgePtr::Null);

  ON_SubD::EdgeTag merged_edge_tag 
    = (e[0]->IsSmooth() || e[1]->IsSmooth())
    ? ON_SubD::EdgeTag::Smooth
    : ON_SubD::EdgeTag::Crease;

  for (unsigned int j = 0; j < e[1]->m_face_count; j++)
  {
    ON_SubDFace* f = const_cast<ON_SubDFace*>(e[1]->Face(j));
    if (nullptr == f)
      continue;
    f->RemoveEdgeFromArray(e[1]);
  }
  
  // remove middle vertex
  ON_SubDVertex* middle_v = const_cast< ON_SubDVertex* >(e[1]->m_vertex[edir[1]]);
  if (nullptr != middle_v && middle_v != end_v[0] && middle_v != end_v[1] )
  {
    if (middle_v->m_edge_count > 0 && nullptr != middle_v->m_edges)
    {
      unsigned int vei0 = middle_v->EdgeArrayIndex(e[0]);
      unsigned int vei1 = middle_v->EdgeArrayIndex(e[1]);      
      unsigned int vei_count = middle_v->m_edge_count;
      middle_v->m_edge_count = 0;
      for (unsigned int vei = 0; vei < vei_count; vei++)
      {
        if ( vei == vei0 || vei == vei1 )
          continue;

        // happens when middle_v is a multiple sector corner, non-manifold, or other rare cases
        if (vei > middle_v->m_edge_count)
          middle_v->m_edges[middle_v->m_edge_count] = middle_v->m_edges[vei];
        middle_v->m_edge_count++;
      }
    }
    if (0 == middle_v->m_edge_count || nullptr == middle_v->m_edges)
    {
      ReturnVertex(middle_v);
      middle_v = nullptr;
    }
  }

  e[0]->m_vertex[1-edir[0]] = nullptr;
  e[1]->m_vertex[edir[1]] = nullptr;
  e[1]->m_vertex[1-edir[1]] = nullptr;

  for (unsigned int i = 0; i < end_v[1]->m_edge_count; i++)
  {
    if (e[1] == ON_SUBD_EDGE_POINTER(end_v[1]->m_edges[i].m_ptr))
    {
      const_cast< ON_SubDVertex* >(end_v[1])->m_edges[i] = ON_SubDEdgePtr::Create(e[0], 1 - edir[0]);
      e[0]->m_vertex[1 - edir[0]] = end_v[1];
      break;
    }
  }

  e[0]->m_sector_coefficient[1 - edir[0]] = e[1]->m_sector_coefficient[1 - edir[1]];

  const bool bTagged[2] = { end_v[0]->IsCreaseOrCorner(), end_v[1]->IsCreaseOrCorner() };
  if (ON_SubD::EdgeTag::Smooth == merged_edge_tag || false == bTagged[0] || false == bTagged[1])
  {
    e[0]->m_edge_tag
      = (bTagged[0] && bTagged[1])
      ? ON_SubD::EdgeTag::SmoothX
      : ON_SubD::EdgeTag::Smooth;
    if ( false == bTagged[0])
      e[0]->m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;
    else if (!(e[0]->m_sector_coefficient[0] > 0.0 && e[0]->m_sector_coefficient[0] < 1.0))
      e[0]->m_sector_coefficient[0] = ON_SubDSectorType::UnsetSectorCoefficient;
    if ( false == bTagged[1])
      e[0]->m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
    else if (!(e[0]->m_sector_coefficient[1] > 0.0 && e[0]->m_sector_coefficient[1] < 1.0))
      e[0]->m_sector_coefficient[1] = ON_SubDSectorType::UnsetSectorCoefficient;
  }
  else
  {
    e[0]->m_edge_tag = ON_SubD::EdgeTag::Crease;
    e[0]->m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;
    e[0]->m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
  }

  ReturnEdge(e[1]);

  return eptr0;
}

ON_SubDEdgePtr ON_SubD::MergeConsecutiveEdges(
  ON_SubDEdgePtr eptr0,
  ON_SubDEdgePtr eptr1
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  return (nullptr != subdimple) ? subdimple->MergeConsecutiveEdges(eptr0, eptr1) : ON_SubDEdgePtr::Null;
}

static bool Internal_EdgesAreConsecutive(
  const ON_SubDEdgePtr eptr[2],
  bool bTestColinearity,
  double distance_tolerance,
  double maximum_aspect,
  double sin_angle_tolerance
  )
{
  const ON_SubDEdge* e[2] = { ON_SUBD_EDGE_POINTER(eptr[0].m_ptr), ON_SUBD_EDGE_POINTER(eptr[1].m_ptr) };
  ON__UINT_PTR edir[2] = { ON_SUBD_EDGE_DIRECTION(eptr[0].m_ptr), ON_SUBD_EDGE_DIRECTION(eptr[1].m_ptr) };
  if (
    nullptr == e[0] || nullptr == e[1] 
    || e[0] == e[1]
    || edir[0] > 1 || edir[1] > 1
    || e[0]->m_face_count != e[1]->m_face_count
    //|| e[0]->m_edge_tag != e[1]->m_edge_tag
    )
  {
    return false;
  }

  if ( nullptr == e[1]->m_vertex[0] || nullptr == e[1]->m_vertex[1] )
  {
    return false;
  }

  // v[0] = start
  // v[1] = end
  // v[2] = middle (will be removed)
  const ON_SubDVertex* v[4] = { e[0]->m_vertex[edir[0]], e[1]->m_vertex[1 - edir[1]], e[0]->m_vertex[1 - edir[0]], e[1]->m_vertex[edir[1]] };

  if (nullptr == v[0] || nullptr == v[1] || nullptr == v[2] || v[0] == v[1] || v[2] != v[3])
  {
    return false;
  }

  if (bTestColinearity)
  {
    if (ON_UNSET_UINT_INDEX == v[2]->EdgeArrayIndex(e[0]) || ON_UNSET_UINT_INDEX == v[2]->EdgeArrayIndex(e[1]))
      bTestColinearity = false;
  }

  // edges must have the same faces
  switch (e[0]->m_face_count)
  {
  case 0:
    break;

  case 1:
    if (ON_SUBD_FACE_POINTER(e[0]->m_face2[0].m_ptr) == ON_SUBD_FACE_POINTER(e[1]->m_face2[0].m_ptr))
      break;

    return false;

  case 2:
    if (ON_SUBD_FACE_POINTER(e[0]->m_face2[0].m_ptr) == ON_SUBD_FACE_POINTER(e[1]->m_face2[0].m_ptr)
      && ON_SUBD_FACE_POINTER(e[0]->m_face2[1].m_ptr) == ON_SUBD_FACE_POINTER(e[1]->m_face2[1].m_ptr))
      break;

    if (ON_SUBD_FACE_POINTER(e[0]->m_face2[0].m_ptr) == ON_SUBD_FACE_POINTER(e[1]->m_face2[1].m_ptr)
      && ON_SUBD_FACE_POINTER(e[0]->m_face2[1].m_ptr) == ON_SUBD_FACE_POINTER(e[1]->m_face2[0].m_ptr))
      break;

    return false;

  default:
    // non-manifold edge
    {
      unsigned int j, k;
      for (j = 0; j < e[0]->m_face_count; j++)
      {
        const ON_SubDFace* f = e[0]->Face(j);
        for (k = 0; k < e[1]->m_face_count; k++)
        {
          if (f == e[1]->Face(k))
            break;
        }
        if (k < e[1]->m_face_count)
          continue;
        break;
      }
      if (j != e[0]->m_face_count)
        return false;
    }
    break;
  }

  if (bTestColinearity)
  {
    const ON_3dPoint* P[3] = { (const ON_3dPoint*)v[0]->m_P, (const ON_3dPoint*)v[2]->m_P, (const ON_3dPoint*)v[1]->m_P };

    ON_3dVector D(*P[2] - *P[0]);
    const double d = D.Length();
    if (!(d > 0.0))
      return false;
    const ON_3dVector V(*P[1] - *P[0]);
    const double t = (V*D) / (d*d);
    if (!(t > ON_EPSILON && t < 1.0 - ON_EPSILON))
      return false;

    const ON_3dPoint M = (1.0 - t)*(*P[0]) + t*(*P[2]);
    const double h = P[1]->DistanceTo(M);

    if (0.0 == h)
      return true;

    if (!(h > 0.0))
      return false;

    const double miniscule_distance_tolerance = ON_ZERO_TOLERANCE;
    if (h <= miniscule_distance_tolerance && !(distance_tolerance >= 0.0 && distance_tolerance < miniscule_distance_tolerance))
    {
      // skip parameter tests for miniscule h.
      return true;
    }

    const double miniscule_maximum_aspect = 1e-4;
    if (h <= miniscule_maximum_aspect * d && !(maximum_aspect >= 0.0 && maximum_aspect < miniscule_maximum_aspect))
    {
      // skip parameter tests for miniscule h/d.
      return true;
    }

    if (distance_tolerance >= 0.0 && !(h <= distance_tolerance))
      return false; // failed distance to chord test

    if (maximum_aspect >= 0.0 && !(h <= maximum_aspect * d))
      return false; // failed maximum aspect test

    if (sin_angle_tolerance >= 0.0 && sin_angle_tolerance < 1.0 && !(ON_CrossProduct(V, (*P[1] - *P[2])).Length() <= sin_angle_tolerance))
      return false; // failed minimum angle test
  }
  return true;
}

bool ON_SubD::EdgesAreConsecutive(
  ON_SubDEdgePtr eptr0,
  ON_SubDEdgePtr eptr1
  )
{
  ON_SubDEdgePtr eptr[2] = { eptr0,eptr1 };
  return Internal_EdgesAreConsecutive(eptr,false,ON_DBL_QNAN,ON_DBL_QNAN,ON_DBL_QNAN);
}

static bool Internal_EdgesPassTypeFilter(
  const ON_SubDEdgePtr eptr[2],
  bool bMergeBoundaryEdges,
  bool bMergeInteriorCreaseEdges,
  bool bMergeInteriorSmoothEdges
)
{
  for (unsigned i = 0; i < 2; ++i)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr[i].m_ptr);
    if (nullptr == e || e->m_face_count < 1)
      return false;
    if (1 == e->m_face_count)
    {
      if (false == bMergeBoundaryEdges)
        return false;
    }
    else if (e->m_face_count >= 2)
    {
      if (false == (e->IsSmooth() ? bMergeInteriorSmoothEdges : bMergeInteriorCreaseEdges))
        return false;
    }
    else
      return false;
  };
  return true;
}

unsigned int ON_SubDimple::MergeColinearEdges(
  bool bMergeBoundaryEdges,
  bool bMergeInteriorCreaseEdges,
  bool bMergeInteriorSmoothEdges,
  double distance_tolerance,
  double maximum_aspect,
  double sin_angle_tolerance
  )
{
  if (1 != m_levels.UnsignedCount())
    return false;

  unsigned int removed_edge_count = 0;

  for (const ON_SubDFace* f = ActiveLevel().m_face[0]; nullptr != f; f = f->m_next_face)
  {
    unsigned int edge_count = f->m_edge_count;
    if (edge_count < 3)
      continue;
    
    // First - find a pair of edges that should not be merged.
    ON_SubDEdgePtr eptr[2] = { ON_SubDEdgePtr::Null, f->EdgePtr(edge_count - 1) };
    unsigned int fei0 = 0;
    while (fei0 < edge_count)
    {
      eptr[0] = eptr[1];
      eptr[1] = f->EdgePtr(fei0);
      if (false == Internal_EdgesAreConsecutive(eptr, true, distance_tolerance, maximum_aspect, sin_angle_tolerance))
        break;
      if (false == Internal_EdgesPassTypeFilter(eptr, bMergeBoundaryEdges, bMergeInteriorCreaseEdges, bMergeInteriorSmoothEdges))
        break;
      ++fei0;
    }
    if (fei0 >= edge_count)
    {
      // face is degenerate or corrupt.
      continue;
    }
    if (0 != fei0)
    {
      if (false == (const_cast<ON_SubDFace*>(f)->RotateEdgeArray(fei0)))
      {
        // face is degenerate or corrupt.
        continue;
      }
    }

    // At this point, we know the last edge and the first edge should not me merged.
    eptr[1] = f->EdgePtr(0U);
    unsigned int fei = 1;
    while ( fei < edge_count)
    {
      eptr[0] = eptr[1];
      eptr[1] = f->EdgePtr(fei);
      if (
        Internal_EdgesAreConsecutive(eptr, true, distance_tolerance, maximum_aspect, sin_angle_tolerance)
        && Internal_EdgesPassTypeFilter(eptr, bMergeBoundaryEdges, bMergeInteriorCreaseEdges, bMergeInteriorSmoothEdges)
        )
      {
        // merge edges f->Edge(fei-1) and f->Edge(fei) into f->Edge(fei-1).
        if (eptr[0].m_ptr != MergeConsecutiveEdges(eptr[0], eptr[1]).m_ptr)
        {
          ON_SUBD_ERROR("Bug in consecutive edge merging.");
          break;
        }
        ++removed_edge_count;
        --edge_count;
        eptr[1] = eptr[0];
      }
      else
      {
        ++fei;
      }
    }
  }

  return removed_edge_count;
}

unsigned int ON_SubD::MergeColinearEdges(
  bool bMergeBoundaryEdges,
  bool bMergeInteriorCreaseEdges,
  bool bMergeInteriorSmoothEdges,
  double distance_tolerance,
  double maximum_aspect,
  double sin_angle_tolerance
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  return 
    (nullptr != subdimple) 
    ? subdimple->MergeColinearEdges( bMergeBoundaryEdges, bMergeInteriorCreaseEdges, bMergeInteriorSmoothEdges, distance_tolerance, maximum_aspect, sin_angle_tolerance) 
    : 0;
}

unsigned int ON_SubD::LevelCount() const
{
  const ON_SubDimple* subdimple = SubDimple();
  return (0 != subdimple ? subdimple->LevelCount() : 0);
}


unsigned int ON_SubD::ActiveLevelIndex() const
{
  return ActiveLevel().m_level_index;
}

bool ON_SubD::IsEmpty() const
{
  return (nullptr == SubDimple());
}

bool ON_SubD::IsNotEmpty() const
{
  return (nullptr != SubDimple());
}

/////////////////////////////////////////////////////////
//
// Element (Vertex, Edge, Face) access
//
const ON_COMPONENT_INDEX ON_SubDComponentPtr::ComponentIndex() const
{
  switch (ComponentType())
  {
  case ON_SubDComponentPtr::Type::Vertex:
    {
      const ON_SubDVertex* vertex = ON_SUBD_VERTEX_POINTER(m_ptr);
      if ( nullptr != vertex)
        return ON_COMPONENT_INDEX(ON_COMPONENT_INDEX::TYPE::subd_vertex,vertex->m_id);
    }
    break;
  case ON_SubDComponentPtr::Type::Edge:
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(m_ptr);
      if ( nullptr != edge)
        return ON_COMPONENT_INDEX(ON_COMPONENT_INDEX::TYPE::subd_edge,edge->m_id);
    }
    break;
  case ON_SubDComponentPtr::Type::Face:
    {
      const ON_SubDFace* face = ON_SUBD_FACE_POINTER(m_ptr);
      if ( nullptr != face)
        return ON_COMPONENT_INDEX(ON_COMPONENT_INDEX::TYPE::subd_face,face->m_id);
    }
    break;
  default:
    if (IsNull() )
      return ON_COMPONENT_INDEX::UnsetComponentIndex;
    break;
  }
  return ON_SUBD_RETURN_ERROR(ON_COMPONENT_INDEX::UnsetComponentIndex);
}

const ON_SubDComponentPtr ON_SubD::ComponentPtrFromComponentIndex(
  ON_COMPONENT_INDEX component_index
  ) const
{
  if (0 != component_index.m_index && -1 != component_index.m_index)
  {
    switch (component_index.m_type)
    {
    case ON_COMPONENT_INDEX::TYPE::subd_vertex:
      return ON_SubDComponentPtr::Create(VertexFromId(component_index.m_index));
    case ON_COMPONENT_INDEX::TYPE::subd_edge:
      return ON_SubDComponentPtr::Create(EdgeFromId(component_index.m_index));
    case ON_COMPONENT_INDEX::TYPE::subd_face:
      return ON_SubDComponentPtr::Create(FaceFromId(component_index.m_index));
    default:
      break;
    }
  }
  else if (ON_COMPONENT_INDEX::TYPE::invalid_type == component_index.m_type)
  {
    return ON_SubDComponentPtr::Null;
  }

  return ON_SUBD_RETURN_ERROR(ON_SubDComponentPtr::Null);
}

/////////////////////////////////////////////////////////
//
// Vertex access
//

unsigned int ON_SubD::VertexCount() const
{
  return ActiveLevel().m_vertex_count;
}

const ON_SubDVertex* ON_SubD::FirstVertex() const
{
  return ActiveLevel().m_vertex[0];
}

const ON_SubDVertex* ON_SubD::LastVertex() const
{
  return ActiveLevel().m_vertex[1];
}

ON_SubDVertexIterator ON_SubD::VertexIterator() const
{
  return ON_SubDVertexIterator(*this);
}

ON_SubDVertexIterator ON_SubDRef::VertexIterator() const
{
  return ON_SubDVertexIterator(*this);
}

ON_SubDVertexArray ON_SubD::VertexArray() const
{
  return ON_SubDVertexArray(*this);
}

const class ON_SubDVertex* ON_SubD::VertexFromId(
  unsigned int vertex_id
  ) const
{
  for (;;)
  {
    if (0 == vertex_id || ON_UNSET_UINT_INDEX == vertex_id )
      break;
    const ON_SubDimple* subdimple = SubDimple();
    if (nullptr == subdimple)
      break;
    return subdimple->VertexFromId(vertex_id);
  }
  return nullptr;
}

const class ON_SubDVertex* ON_SubD::VertexFromComponentIndex(
  ON_COMPONENT_INDEX component_index
) const
{
  return (ON_COMPONENT_INDEX::TYPE::subd_vertex == component_index.m_type) ? VertexFromId(component_index.m_index) : nullptr;
}

const ON_COMPONENT_INDEX ON_SubDVertex::ComponentIndex() const
{
  return ON_COMPONENT_INDEX(ON_COMPONENT_INDEX::TYPE::subd_vertex,m_id);
}

const ON_SubDComponentPtr ON_SubDVertex::ComponentPtr() const
{
  return ON_SubDComponentPtr::Create(this);
}

/////////////////////////////////////////////////////////
//
// Edge access
//

unsigned int ON_SubD::EdgeCount() const
{
  return ActiveLevel().m_edge_count;
}

const ON_SubDEdge* ON_SubD::FirstEdge() const
{
  return ActiveLevel().m_edge[0];
}

const ON_SubDEdge* ON_SubD::LastEdge() const
{
  return ActiveLevel().m_edge[1];
}

ON_SubDEdgeIterator ON_SubD::EdgeIterator() const
{
  return ON_SubDEdgeIterator(*this);
}

ON_SubDEdgeIterator ON_SubDRef::EdgeIterator() const
{
  return ON_SubDEdgeIterator(*this);
}

ON_SubDEdgeArray ON_SubD::EdgeArray() const
{
  return ON_SubDEdgeArray(*this);
}

const class ON_SubDEdge* ON_SubD::EdgeFromId(
  unsigned int edge_id
  ) const
{
  for (;;)
  {
    if (0 == edge_id || ON_UNSET_UINT_INDEX == edge_id )
      break;
    const ON_SubDimple* subdimple = SubDimple();
    if (nullptr == subdimple)
      break;
    return subdimple->EdgeFromId(edge_id);
  }
  return nullptr;
}

const class ON_SubDEdge* ON_SubD::EdgeFromComponentIndex(
  ON_COMPONENT_INDEX component_index
) const
{
  return (ON_COMPONENT_INDEX::TYPE::subd_edge == component_index.m_type) ? EdgeFromId(component_index.m_index) : nullptr;
}

const ON_COMPONENT_INDEX ON_SubDEdge::ComponentIndex() const
{
  return ON_COMPONENT_INDEX(ON_COMPONENT_INDEX::TYPE::subd_edge,m_id);
}

const ON_SubDComponentPtr ON_SubDEdge::ComponentPtr() const
{
  return ON_SubDComponentPtr::Create(this);
}

/////////////////////////////////////////////////////////
//
// Face access
//

unsigned int ON_SubD::FaceCount() const
{
  return ActiveLevel().m_face_count;
}

const ON_SubDFace* ON_SubD::FirstFace() const
{
  return ActiveLevel().m_face[0];
}

const ON_SubDFace* ON_SubD::LastFace() const
{
  return ActiveLevel().m_face[1];
}

ON_SubDFaceIterator ON_SubD::FaceIterator() const
{
  return ON_SubDFaceIterator(*this);
}

ON_SubDFaceIterator ON_SubDRef::FaceIterator() const
{
  return ON_SubDFaceIterator(*this);
}


ON_SubDFaceArray ON_SubD::FaceArray() const
{
  return ON_SubDFaceArray(*this);
}

const class ON_SubDFace* ON_SubD::FaceFromId(
  unsigned int face_id
  ) const
{
  for (;;)
  {
    if (0 == face_id || ON_UNSET_UINT_INDEX == face_id )
      break;
    const ON_SubDimple* subdimple = SubDimple();
    if (nullptr == subdimple)
      break;
    return subdimple->FaceFromId(face_id);
  }
  return nullptr;
}

const class ON_SubDFace* ON_SubD::FaceFromComponentIndex(
  ON_COMPONENT_INDEX component_index
) const
{
  return (ON_COMPONENT_INDEX::TYPE::subd_face == component_index.m_type) ? FaceFromId(component_index.m_index) : nullptr;
}

const ON_COMPONENT_INDEX ON_SubDFace::ComponentIndex() const
{
  return ON_COMPONENT_INDEX(ON_COMPONENT_INDEX::TYPE::subd_face,m_id);
}

const ON_SubDComponentPtr ON_SubDFace::ComponentPtr() const
{
  return ON_SubDComponentPtr::Create(this);
}

/////////////////////////////////////////////////////////
//
// ON_SubD properties
//

bool ON_SubD::IsOriented() const
{
  for (const ON_SubDEdge* edge = FirstEdge(); nullptr != edge; edge = edge->m_next_edge)
  {
    if ( 2 != edge->m_face_count )
      continue;
    if (nullptr == ON_SUBD_FACE_POINTER(edge->m_face2[0].m_ptr) || nullptr == ON_SUBD_FACE_POINTER(edge->m_face2[1].m_ptr) )
      continue;
    if ( ON_SUBD_FACE_DIRECTION(edge->m_face2[0].m_ptr) == ON_SUBD_FACE_DIRECTION(edge->m_face2[1].m_ptr) )
      return false;
  }
  return true;
}

// reverses the orientation of all facets
bool ON_SubD::ReverseOrientation() const
{
  // Limit point normals and limit surface mesh fragments will need to be recalculated.
  // DestroyRuntimeCache() will clear all this information.
  const_cast<ON_SubD*>(this)->DestroyRuntimeCache(true);

  for (const ON_SubDFace* face = FirstFace(); nullptr != face; face = face->m_next_face)
  {
    const_cast<ON_SubDFace*>(face)->ReverseEdgeList();
  }
  return true;
}

// Attempts to orient all facet to match the first facet.

static unsigned int OrientFaceNeighbors(
  unsigned int recursion_level,
  const ON_SubDFace** face_list,
  unsigned int id0,
  const ON_SubDFace* face
  )
{
  ON_SubDFace* next_set[4];
  const unsigned int next_set_capacity = (unsigned int)(sizeof(next_set) / sizeof(next_set[0]));
  unsigned int next_set_count = 0;

  const unsigned int edge_count = face->m_edge_count;
  const ON_SubDEdgePtr* face_eptr = face->m_edge4;
  const ON_SubDEdge* e;
  ON_SubDFace* neighbor_face;

  if (nullptr != face_list[face->m_id - id0])
  {
    // search for an oriented neighbor
    neighbor_face = nullptr;
    for (unsigned int fei = 0; fei < edge_count; fei++, face_eptr++)
    {
      if (4 == fei)
      {
        face_eptr = face->m_edgex;
        if (nullptr == face_eptr)
          break;
      }
      e = ON_SUBD_EDGE_POINTER(face_eptr->m_ptr);
      if (nullptr == e || 2 != e->m_face_count)
        continue;
      neighbor_face = ON_SUBD_FACE_POINTER(e->m_face2[0].m_ptr);
      if (face == neighbor_face)
        neighbor_face = ON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr);
      else if (face != ON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr))
        continue;
      if (nullptr == neighbor_face)
        continue;

      if (nullptr == face_list[neighbor_face->m_id - id0])
        return OrientFaceNeighbors(recursion_level,face_list,id0,neighbor_face);
    }

    // nothing near face is oriented. 
    return 0;
  }

  unsigned int orient_count = 0;
  for (unsigned int fei = 0; fei < edge_count; fei++, face_eptr++)
  {
    if (4 == fei)
    {
      face_eptr = face->m_edgex;
      if ( nullptr == face_eptr)
        break;
    }
    e = ON_SUBD_EDGE_POINTER(face_eptr->m_ptr);
    if (nullptr == e || 2 != e->m_face_count)
      continue;
    neighbor_face = ON_SUBD_FACE_POINTER(e->m_face2[0].m_ptr);
    if (face == neighbor_face)
      neighbor_face = ON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr);
    else if (face != ON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr))
      continue;
    if (nullptr == neighbor_face)
      continue;

    if (nullptr == face_list[neighbor_face->m_id - id0])
      continue;

    if (ON_SUBD_FACE_DIRECTION(e->m_face2[0].m_ptr) == ON_SUBD_FACE_DIRECTION(e->m_face2[1].m_ptr))
      neighbor_face->ReverseEdgeList();
    face_list[neighbor_face->m_id - id0] = nullptr;
    orient_count++;

    if (recursion_level < 12)
    {
      if (next_set_count >= next_set_capacity)
      {
        for (unsigned i = 0; i < next_set_capacity; i++)
          orient_count += OrientFaceNeighbors(recursion_level + 1, face_list, id0, next_set[i]);
        next_set_count = 0;
      }
      next_set[next_set_count++] = neighbor_face;
    }
  }

  for ( unsigned i = 0; i < next_set_count; i++)
    orient_count += OrientFaceNeighbors(recursion_level+1,face_list,id0,next_set[i]);

  return orient_count;
}

bool ON_SubD::Orient() const
{
  const ON_SubDFace* first_face = FirstFace();
  if ( nullptr == first_face || nullptr == first_face->m_next_face)
    return true;

  unsigned int nonzero_face_count = 0;
  ON_SimpleArray< const ON_SubDFace* > faces_array(FaceCount());
  unsigned int face_id0 = first_face->m_id;
  unsigned int face_id1 = first_face->m_id;
  for (const ON_SubDFace* face = FirstFace(); nullptr != face; face = face->m_next_face)
  {
    faces_array.Append(face);
    if (face->m_id > face_id1)
      face_id1 = face->m_id;
    else if (face->m_id < face_id1)
      face_id0 = face->m_id;
    nonzero_face_count++;
  }

  const unsigned int face_count = faces_array.UnsignedCount();
  if (face_count <= 1)
    return true;

  const ON_SubDFace** faces = faces_array.Array();
  if (face_id1 - face_id0 > faces_array.UnsignedCount())
  {
    faces_array.Reserve(face_id1 - face_id0);
    faces_array.SetCount(face_id1 - face_id0);
    faces_array.Zero();
    for (const ON_SubDFace* face = FirstFace(); nullptr != face; face = face->m_next_face)
    {
      faces[face->m_id-face_id0] = face;
    }
  }

  unsigned int orient_count = 0;
  unsigned int connected_region_count = 0;
  bool bSearchForNewComponent = true;
  unsigned int first_face_index = 0;

  for (;;)
  {
    unsigned int orient_count0 = orient_count;
    while (first_face_index < face_count && nullptr == faces[first_face_index])
      first_face_index++;

    if ( first_face_index >= face_count)
      break;

    for (unsigned int i = first_face_index; i < face_count && orient_count < nonzero_face_count; i++)
    {
      const ON_SubDFace* face = faces[i];
      if (nullptr == face)
        continue;
      if (bSearchForNewComponent)
      {
        // first face in new connected component
        orient_count++;
        connected_region_count++;
        faces[i] = nullptr;
        bSearchForNewComponent = false;
        first_face_index = i+1;
      }
      orient_count += OrientFaceNeighbors(0, faces, face_id0, face);
    }
    
    if ( orient_count >= nonzero_face_count)
      break;

    if (orient_count0 >= orient_count)
    {
      if (bSearchForNewComponent)
        break;
      bSearchForNewComponent = true;
    }
  }

  return (connected_region_count > 0 && orient_count > 0);
}

const ON_SubDVertex * ON_SubD::ReplaceFaceWithTriangleFan(ON_SubDFace * face, ON_3dPoint fan_center_point, bool bMarkFaces)
{
  const unsigned edge_count = face->m_edge_count;
  if (edge_count < 3)
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDimple* subdimple = SubDimple(false);
  if (nullptr == subdimple)
    return ON_SUBD_RETURN_ERROR(nullptr);

  // validate and get centroid (which may not be needed).
  ON_3dPoint P = ON_3dPoint::Origin;
  ON_SimpleArray<ON_SubDEdgePtr> edges(edge_count);
  const ON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    if (4U == i)
    {
      eptr = face->m_edgex;
      if (nullptr == eptr)
        return ON_SUBD_RETURN_ERROR(nullptr);
    }
    const ON_SubDVertex* v = eptr->RelativeVertex(0);
    if (nullptr == v)
      return ON_SUBD_RETURN_ERROR(nullptr);
    P += v->ControlNetPoint();
    edges.Append(*eptr);
  }

  if (fan_center_point.IsValid())
    P = fan_center_point;
  else
    P /= ((double)edge_count);

  ON_SubDVertex* v0 = AddVertex(ON_SubD::VertexTag::Smooth, P);
  if (nullptr == v0)
    return ON_SUBD_RETURN_ERROR(nullptr);

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    if (nullptr == AddEdge(ON_SubD::EdgeTag::Smooth, v0, const_cast<ON_SubDVertex*>(edges[i].RelativeVertex(0))))
    {
      ON_SubDComponentPtr cptr = ON_SubDComponentPtr::Create(v0);
      DeleteComponents(&cptr, 1, false);
      return ON_SUBD_RETURN_ERROR(nullptr);
    }
  }

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    if (i < 4)
      face->m_edge4[i] = ON_SubDEdgePtr::Null;
    else
      face->m_edgex[i - 4] = ON_SubDEdgePtr::Null;;
    ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(edges[i].m_ptr);
    e->RemoveFaceFromArray(face);
    const_cast<ON_SubDVertex*>(edges[i].RelativeVertex(0))->RemoveFaceFromArray(face);
  }

  subdimple->ReturnFace(face);
  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    AddTriangleFace(v0->m_edges[i], edges[i], v0->m_edges[(i + 1) % edge_count].Reversed());
  }

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(edges[i].m_ptr);
    e->EdgeModifiedNofification();
    e->UpdateEdgeSectorCoefficientsForExperts(false);
  }

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
    ON_SUBD_EDGE_POINTER(edges[i].m_ptr)->UpdateEdgeSectorCoefficientsForExperts(false);

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
    ON_SUBD_EDGE_POINTER(v0->m_edges[i].m_ptr)->UpdateEdgeSectorCoefficientsForExperts(false);

  return v0;
}



const ON_SubDEdge* ON_SubDimple::SplitEdge(
  ON_SubDEdge* edge,
  ON_3dPoint vertex_location
  )
{
  if ( nullptr == edge )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge->m_vertex[0] || nullptr == edge->m_vertex[1] )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge->m_vertex[0]->m_edges || edge->m_vertex[0]->m_edge_count <= 0 || edge->m_vertex[0]->m_edge_capacity < edge->m_vertex[0]->m_edge_count )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge->m_vertex[1]->m_edges || edge->m_vertex[1]->m_edge_count <= 0 || edge->m_vertex[1]->m_edge_capacity < edge->m_vertex[1]->m_edge_count )
    return ON_SUBD_RETURN_ERROR(nullptr);

  if ( vertex_location.IsUnsetOrNan() )
  {
    ON_Line L;
    L.from = ON_3dPoint(edge->m_vertex[0]->m_P);
    L.to = ON_3dPoint(edge->m_vertex[1]->m_P);
    vertex_location = L.PointAt(0.5);
  }
  if ( false == vertex_location.IsValid() )
    return ON_SUBD_RETURN_ERROR(nullptr);

  if ( vertex_location == ON_3dPoint(edge->m_vertex[0]->m_P) || vertex_location == ON_3dPoint(edge->m_vertex[1]->m_P) )
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubD::EdgeTag edge_tag = edge->m_edge_tag;
  ON_SubD::VertexTag vertex_tag;
  switch (edge->m_edge_tag)
  {
  case ON_SubD::EdgeTag::Smooth:
    vertex_tag = ON_SubD::VertexTag::Smooth;
    break;
  case ON_SubD::EdgeTag::Crease:
    vertex_tag = ON_SubD::VertexTag::Crease;
    break;
  case ON_SubD::EdgeTag::SmoothX:
    vertex_tag = ON_SubD::VertexTag::Smooth;
    edge_tag = ON_SubD::EdgeTag::Smooth;
    break;
  default:
    return ON_SUBD_RETURN_ERROR(nullptr);
    break;
  }

  ON_SubDVertex* end_vertex[2] = { const_cast<ON_SubDVertex*>(edge->m_vertex[0]), const_cast<ON_SubDVertex*>(edge->m_vertex[1]) };

  ON_SubDVertex* new_vertex = nullptr;
  ON_SubDEdge* new_edge = nullptr;  
  for (;;)
  {
    new_vertex = AllocateVertex(vertex_tag, edge->SubdivisionLevel(), static_cast<const double*>(vertex_location), 2, edge->m_face_count);
    if (nullptr == new_vertex)
     break;

    new_edge = AllocateEdge(edge_tag, edge->SubdivisionLevel(), edge->m_face_count);
    if (nullptr == new_edge)
      break;

    // change end_vertex[1] edge reference from edge to new_edge
    bool bConnectedNewEdgeToEndVertex = false;
    const ON_SubDEdgePtr old_edge_reference = ON_SubDEdgePtr::Create(edge,1);
    for (unsigned short vei = 0; vei < end_vertex[1]->m_edge_count; vei++)
    {
      if (old_edge_reference.m_ptr == end_vertex[1]->m_edges[vei].m_ptr)
      {
        // change end_vertex[1]->m_edges[vei] reference
        // from "edge" to "new_edge".
        bConnectedNewEdgeToEndVertex = true;
        end_vertex[1]->m_edges[vei] = ON_SubDEdgePtr::Create(new_edge,1);
        break;
      }
    }

    if (false == bConnectedNewEdgeToEndVertex)
    {
      // end_vertex[1]->m_edges[] does not reference edge
      break;
    }

    // finish setting new_vertex and end_vertex[] <-> new_edge connections
    new_edge->m_vertex[0] = new_vertex;
    new_edge->m_vertex[1] = end_vertex[1];
    new_vertex->m_edges[new_vertex->m_edge_count++] = ON_SubDEdgePtr::Create(new_edge,0);

    // finish setting new_vertex <-> input edge and connections
    edge->m_edge_tag = edge_tag; // changes "X" to "Smooth" if required
    edge->m_vertex[1] = new_vertex;
    new_vertex->m_edges[new_vertex->m_edge_count++] = ON_SubDEdgePtr::Create(edge,1);
       
    // add new_vertex and new_edge <-> edge->m_face[] connections.
    const ON_SubDFacePtr* fptr0 = edge->m_face2;
    ON_SubDFacePtr* fptr1 = new_edge->m_face2;
    for (unsigned short efi = 0; efi < edge->m_face_count; efi++)
    {
      if (2 == efi)
      {
        fptr0 = edge->m_facex;
        fptr1 = new_edge->m_facex;
      }

      ON_SubDFace* face = ON_SUBD_FACE_POINTER(fptr0->m_ptr);

      if (nullptr != face)
      {
        face->FaceModifiedNofification();

        // add new_vertex reference to face
        new_vertex->m_faces[new_vertex->m_face_count++] = face;

        // insert new_edge into face->m_edge[] list after "edge"
        if (GrowFaceEdgeArray(face, face->m_edge_count + 1))
        {
          if ( face->m_edge_count < 4 )
            face->m_edge4[face->m_edge_count] = ON_SubDEdgePtr::Null;
          else
            face->m_edgex[face->m_edge_count-4] = ON_SubDEdgePtr::Null;
          face->m_edge_count++;
          ON_SubDEdgePtr* face_edge = face->m_edge4;
          for (unsigned short fei = 0; fei < face->m_edge_count; fei++)
          {
            if (4 == fei)
              face_edge = face->m_edgex;
            if (edge == ON_SUBD_EDGE_POINTER(face_edge->m_ptr))
            {
              ON__UINT_PTR edir = ON_SUBD_EDGE_DIRECTION(face_edge->m_ptr);
              ON_SubDEdgePtr eptr = ON_SubDEdgePtr::Create(new_edge,edir);
              if (0 == edir)
              {
                fei++;
                face_edge++;
              }
              for (/*empty init*/; fei < face->m_edge_count; fei++)
              {
                if (4 == fei)
                  face_edge = face->m_edgex;
                const ON_SubDEdgePtr tmp = *face_edge;
                *face_edge = eptr;
                eptr = tmp;
                face_edge++;
              }
              break;
            }
            face_edge++;
          }
        }
      }

      *fptr1++ = *fptr0++;
      new_edge->m_face_count++;
    }

    // original ending vertex
    new_edge->m_sector_coefficient[1] = edge->m_sector_coefficient[1];

    // Either edge was a crease, new_edge is a crease, and sector weights do not applie
    // or edge was X or Smooth, edge is smooth, new_edge is smooth, new_vertex is smooth,
    // and the sector weights at this vertex do not apply.
    edge->m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
    new_edge->m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;

    AddVertexToLevel(new_vertex);
    AddEdgeToLevel(new_edge);

    end_vertex[0]->VertexModifiedNofification();
    end_vertex[1]->VertexModifiedNofification();

    // TODO
    //   Delete any levels greater than this level.
    return new_edge;
  }

  if ( nullptr != new_vertex)
    ReturnVertex(new_vertex);
  if ( nullptr != new_edge)
    ReturnEdge(new_edge);
  return ON_SUBD_RETURN_ERROR(nullptr);
}

const ON_SubDEdge* ON_SubDimple::SplitFace(
  ON_SubDFace* face,
  unsigned int fvi0,
  unsigned int fvi1
  )
{
  if ( nullptr == face || fvi0 == fvi1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int edge_count = face->m_edge_count;
  if (edge_count < 4 || (edge_count > 4 && nullptr == face->m_edgex))
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (fvi0 >= edge_count || fvi1 >= edge_count)
    return ON_SUBD_RETURN_ERROR(nullptr);

  if ((fvi0 + 1) % edge_count == fvi1 || (fvi1 + 1) % edge_count == fvi0)
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (face->SubdivisionLevel() >= m_levels.UnsignedCount())
    return ON_SUBD_RETURN_ERROR(nullptr);
  ON_SubDLevel* level = m_levels[face->SubdivisionLevel()];
  if ( nullptr == level)
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDVertex* v[2] = { const_cast<ON_SubDVertex*>(face->Vertex(fvi0)), const_cast<ON_SubDVertex*>(face->Vertex(fvi1)) };

  if (nullptr == v[0] || v[0]->m_face_count <= 0 || nullptr == v[0]->m_faces || v[0]->m_edge_count < 2 || nullptr == v[0]->m_edges )
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (nullptr == v[1] || v[1]->m_face_count <= 0 || nullptr == v[1]->m_faces || v[1]->m_edge_count < 2 || nullptr == v[1]->m_edges)
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (v[0] == v[1])
    return ON_SUBD_RETURN_ERROR(nullptr);

  unsigned new_edge_count[2];
  if (fvi0 < fvi1)
  {
    new_edge_count[1] = fvi1 - fvi0 + 1;
    new_edge_count[0] = (edge_count + 2) - new_edge_count[1];
  }
  else
  {
    new_edge_count[0] = fvi0 - fvi1 + 1;
    new_edge_count[1] = (edge_count + 2) - new_edge_count[0];
  }
  // make sure each side is at least a triangle and no overflows occured
  if (new_edge_count[0] < 3 || new_edge_count[0] >= edge_count)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (new_edge_count[1] < 3 || new_edge_count[1] >= edge_count)
    return ON_SUBD_RETURN_ERROR(nullptr);
  if (new_edge_count[0] + new_edge_count[1] != edge_count+2 )
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  // make sure face topology is valid
  ON_SimpleArray< ON_SubDEdgePtr > edges(edge_count);
  ON_SubDEdgePtr* eptr = face->m_edge4;
  edges.SetCount(edge_count);
  for (unsigned int fei = 0; fei < edge_count; ++fei, ++eptr)
  {
    if (4 == fei)
      eptr = face->m_edgex;
    const ON_SubDEdge* face_e = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr == face_e)
      return ON_SUBD_RETURN_ERROR(nullptr);
    if (ON_UNSET_UINT_INDEX == face_e->FaceArrayIndex(face) )
      return ON_SUBD_RETURN_ERROR(nullptr);
    const ON_SubDVertex* face_v = face_e->m_vertex[ON_SUBD_EDGE_DIRECTION(eptr->m_ptr)];
    if ( nullptr == face_v)
      return ON_SUBD_RETURN_ERROR(nullptr);
    if (ON_UNSET_UINT_INDEX == face_v->FaceArrayIndex(face) )
      return ON_SUBD_RETURN_ERROR(nullptr);
    edges[fei] = *eptr;
  }

  // create diagonal edge
  ON_SubDEdge* new_e = nullptr;
  ON_SubDFace* new_f = nullptr;
  for (;;)
  {
    if (false == m_heap.GrowVertexFaceArrayByOne(v[0]))
      break;
    if (false == m_heap.GrowVertexFaceArrayByOne(v[1]))
      break;

    new_f = AllocateFace();
    if (nullptr == new_f)
      break;
    new_f->SetSubdivisionLevel( face->SubdivisionLevel() );
    AddFaceToLevel(new_f);
    if (new_edge_count[1] > 4)
    {
      if (false == m_heap.GrowFaceEdgeArray(new_f, new_edge_count[1]))
        break;
    }

    new_e = AddEdge(
      ((v[0]->IsSmooth() || v[1]->IsSmooth()) ? ON_SubD::EdgeTag::Smooth : ON_SubD::EdgeTag::SmoothX),
      v[0], ON_SubDSectorType::UnsetSectorCoefficient, 
      v[1], ON_SubDSectorType::UnsetSectorCoefficient);
    if (nullptr == new_e)
      break;

    face->FaceModifiedNofification();

    v[0]->m_faces[v[0]->m_face_count++] = new_f;
    v[1]->m_faces[v[1]->m_face_count++] = new_f;

    new_e->m_face2[0] = ON_SubDFacePtr::Create(face, 0);
    new_e->m_face2[1] = ON_SubDFacePtr::Create(new_f, 1);
    new_e->m_face_count = 2;
    const ON_SubDEdgePtr new_eptr = ON_SubDEdgePtr::Create(new_e);

    eptr = face->m_edge4;
    for (unsigned int fei = 0; fei < edge_count; ++fei, ++eptr)
    {
      if (4 == fei)
        eptr = face->m_edgex;
      *eptr = ON_SubDEdgePtr::Null;
    }
    face->m_edge_count = 0;
    if (new_edge_count[0] <= 4 && nullptr != face->m_edgex)
      m_heap.ReturnFaceExtraArray(face);

    // update old face
    face->m_edge4[0] = new_eptr;
    eptr = &(face->m_edge4[1]);
    for (unsigned fei = 1; fei < new_edge_count[0]; ++fei, ++eptr)
    {
      if (4 == fei)
        eptr = face->m_edgex;
      *eptr = edges[(fvi1 + fei - 1) % edge_count];
    }
    face->m_edge_count = (unsigned short)new_edge_count[0];

    // initialize new_f
    new_f->m_edge4[0] = new_eptr.Reversed();
    eptr = &(new_f->m_edge4[1]);
    for (unsigned fei = 1; fei < new_edge_count[1]; ++fei, ++eptr)
    {
      if (4 == fei)
        eptr = new_f->m_edgex;
      *eptr = edges[(fvi0 + fei - 1) % edge_count];

      // change edge's face reference from old face to new_f.
      ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
      e->ReplaceFaceInArray(face, new_f);

      ON_SubDVertex* vtx = const_cast<ON_SubDVertex*>(eptr->RelativeVertex(0));
      if (nullptr != vtx && v[0] != vtx && v[1] != vtx)
        vtx->ReplaceFaceInArray(face, new_f);
    }
    new_f->m_edge_count = (unsigned short)new_edge_count[1];
    
    // update sector weights because they depend on the number of edges
    for (unsigned int vi = 0; vi < 2; vi++)
    {
      for (unsigned short evi = 0; evi < v[vi]->m_edge_count; ++evi)
      {
        ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(v[vi]->m_edges[evi].m_ptr);
        if (nullptr != edge)
          edge->UpdateEdgeSectorCoefficientsForExperts(true);
      }
    }

    // Debugging code
    const ON_SubDVertex* v0[6] = {
      face->m_edge4[0].RelativeVertex(0),face->m_edge4[0].RelativeVertex(1),
      face->m_edge4[1].RelativeVertex(0),face->m_edge4[1].RelativeVertex(1),
      face->EdgePtr(face->m_edge_count-1).RelativeVertex(0),face->EdgePtr(face->m_edge_count - 1).RelativeVertex(1),
    };
    const ON_SubDEdge* e0[3] = {
      face->m_edge4[0].Edge(),
      face->m_edge4[1].Edge(),
      face->EdgePtr(face->m_edge_count - 1).Edge()
    };
    const ON_SubDVertex* v1[6] = {
      new_f->m_edge4[0].RelativeVertex(0),new_f->m_edge4[0].RelativeVertex(1),
      new_f->m_edge4[1].RelativeVertex(0),new_f->m_edge4[1].RelativeVertex(1),
      new_f->EdgePtr(new_f->m_edge_count - 1).RelativeVertex(0),new_f->EdgePtr(new_f->m_edge_count - 1).RelativeVertex(1),
    };
    const ON_SubDEdge* e1[3] = {
      new_f->m_edge4[0].Edge(),
      new_f->m_edge4[1].Edge(),
      new_f->EdgePtr(new_f->m_edge_count - 1).Edge()
    };
    if (nullptr == v0[5] && nullptr == v1[5] && nullptr == e0[2] && nullptr == e1[2])
      return nullptr;



    return new_e;
  }

  if ( nullptr != new_f  )
   ReturnFace(new_f);

  if (nullptr != new_e)
  {
    v[0]->m_edge_count--;
    v[0]->m_edges[v[0]->m_edge_count] = ON_SubDEdgePtr::Null;
    v[1]->m_edge_count--;
    v[1]->m_edges[v[1]->m_edge_count] = ON_SubDEdgePtr::Null;
    new_e->m_vertex[0] = nullptr;
    new_e->m_vertex[1] = nullptr;
    new_e->m_face_count = 0;
    ReturnEdge(new_e);
  }

  return ON_SUBD_RETURN_ERROR(nullptr);
}



const ON_SubDEdge* ON_SubD::SplitEdge(
  ON_SubDEdge* edge,
  ON_3dPoint vertex_location
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return ON_SUBD_RETURN_ERROR(nullptr);
  return subdimple->SplitEdge(edge,vertex_location);
}

const ON_SubDEdge * ON_SubD::SplitFace(
  ON_SubDFace * face, 
  const ON_SubDVertex * v0,
  const ON_SubDVertex * v1
)
{
  if (nullptr == face || nullptr == v0 || nullptr == v1 || v0 == v1)
    return ON_SUBD_RETURN_ERROR(nullptr);
  unsigned int fvi0 = ON_UNSET_UINT_INDEX;
  unsigned int fvi1 = ON_UNSET_UINT_INDEX;
  ON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned short fei = 0; fei < face->m_edge_count; ++fei, ++eptr)
  {
    if (4 == fei)
    {
      eptr = face->m_edgex;
      if (nullptr == eptr)
        return ON_SUBD_RETURN_ERROR(nullptr);
    }
    const ON_SubDVertex* v = eptr->RelativeVertex(0);
    if (v == v0)
    {
      if (ON_UNSET_UINT_INDEX != fvi0)
        return ON_SUBD_RETURN_ERROR(nullptr);
      fvi0 = fei;
    }
    else if (v == v1)
    {
      if (ON_UNSET_UINT_INDEX != fvi1)
        return ON_SUBD_RETURN_ERROR(nullptr);
      fvi1 = fei;
    }
  }
  return (ON_UNSET_UINT_INDEX != fvi0 && ON_UNSET_UINT_INDEX != fvi1)
    ? SplitFace(face, fvi0, fvi1)
    : ON_SUBD_RETURN_ERROR(nullptr);
}

const ON_SubDEdge* ON_SubD::SplitFace(
  ON_SubDFace* face,
  unsigned int fvi0,
  unsigned int fvi1
  )
{
  ON_SubDimple* subdimple = SubDimple(false);
  if ( 0 == subdimple )
    return ON_SUBD_RETURN_ERROR(nullptr);
  return subdimple->SplitFace(face,fvi0,fvi1);
}

void ON_SubD::MarkAggregateComponentStatusAsNotCurrent() const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();
  if ( level )
    level->MarkAggregateComponentStatusAsNotCurrent();
}

unsigned int ON_SubDLevel::ClearStates(
  ON_ComponentStatus states_to_clear
  ) const
{
  unsigned int rc = 0;
  m_aggregates.m_aggregate_status.ClearAggregateStatus(states_to_clear);
  for (const ON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    rc += vertex->m_status.ClearStates(states_to_clear);
  for (const ON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    rc += edge->m_status.ClearStates(states_to_clear);
  for (const ON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    rc += face->m_status.ClearStates(states_to_clear);
  return rc;
}

unsigned int ON_SubDLevel::ClearRuntimeMarks(
  bool bClearVertexMarks,
  bool bClearEdgeMarks,
  bool bClearFaceMarks
) const
{
  unsigned int rc = 0;
  if (bClearVertexMarks)
  {
    for (const ON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      if (vertex->m_status.ClearRuntimeMark())
        ++rc;
    }
  }
  if (bClearEdgeMarks)
  {
    for (const ON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    {
      if (edge->m_status.ClearRuntimeMark())
        ++rc;
    }
  }
  if (bClearFaceMarks)
  {
    for (const ON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    {
      if (face->m_status.ClearRuntimeMark())
        ++rc;
    }
  }
  return rc;
}

unsigned int ON_SubD::ClearComponentStates(
  ON_ComponentStatus states_to_clear
  ) const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();
  if ( level )
    return level->ClearStates(states_to_clear);
  return ON_SUBD_RETURN_ERROR(0);
}

ON_AggregateComponentStatus ON_SubD::AggregateComponentStatus() const
{  
  return ActiveLevel().AggregateComponentStatus();
}

unsigned int ON_SubDLevel::GetComponentsWithSetStates(
  ON_ComponentStatus states_filter,
  bool bAllEqualStates,
  ON_SimpleArray< ON_SubDComponentPtr >& components_with_set_states
  ) const
{
  components_with_set_states.SetCount(0);
  if (states_filter.IsClear())
    return 0;

  ON_AggregateComponentStatus acs = AggregateComponentStatus();

  ON_ComponentStatus as = acs.AggregateStatus();
  if (bAllEqualStates)
  {
    if ( false == as.AllEqualStates(states_filter, states_filter) )
      return 0;
  }
  else
  {
    if ( false == as.SomeEqualStates(states_filter, states_filter) )
      return 0;
  }

  unsigned int c = 0;
  if ( states_filter.IsSelected() && c < m_aggregates.m_aggregate_status.SelectedCount() )
    c = m_aggregates.m_aggregate_status.SelectedCount();
  if ( states_filter.IsHighlighted() && c < m_aggregates.m_aggregate_status.HighlightedCount() )
    c = m_aggregates.m_aggregate_status.HighlightedCount();
  if ( states_filter.IsHidden() && c < m_aggregates.m_aggregate_status.HiddenCount() )
    c = m_aggregates.m_aggregate_status.HiddenCount();
  if ( states_filter.IsLocked() && c < m_aggregates.m_aggregate_status.LockedCount() )
    c = m_aggregates.m_aggregate_status.LockedCount();
  if ( states_filter.IsDamaged() && c < m_aggregates.m_aggregate_status.DamagedCount() )
    c = m_aggregates.m_aggregate_status.DamagedCount();
  if ( states_filter.IsSelected() && c < m_aggregates.m_aggregate_status.SelectedCount() )
    c = m_aggregates.m_aggregate_status.SelectedCount();
  components_with_set_states.Reserve(c);

  if (bAllEqualStates)
  {
    for (const ON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      if (vertex->m_status.AllEqualStates(states_filter, states_filter))
        components_with_set_states.Append(ON_SubDComponentPtr::Create(vertex));
    }
    for (const ON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    {
      if (edge->m_status.AllEqualStates(states_filter, states_filter))
        components_with_set_states.Append(ON_SubDComponentPtr::Create(edge));
    }
    for (const ON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    {
      if (face->m_status.AllEqualStates(states_filter, states_filter))
        components_with_set_states.Append(ON_SubDComponentPtr::Create(face));
    }
  }
  else
  {
    for (const ON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      if (vertex->m_status.SomeEqualStates(states_filter, states_filter))
        components_with_set_states.Append(ON_SubDComponentPtr::Create(vertex));
    }
    for (const ON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    {
      if (edge->m_status.SomeEqualStates(states_filter, states_filter))
        components_with_set_states.Append(ON_SubDComponentPtr::Create(edge));
    }
    for (const ON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    {
      if (face->m_status.SomeEqualStates(states_filter, states_filter))
        components_with_set_states.Append(ON_SubDComponentPtr::Create(face));
    }
  }

  return components_with_set_states.UnsignedCount();
}


unsigned int ON_SubD::GetComponentsWithSetStates(
  ON_ComponentStatus states_filter,
  bool bAllEqualStates,
  ON_SimpleArray< ON_SubDComponentPtr >& components_with_set_states
  ) const
{
  return ActiveLevel().GetComponentsWithSetStates(
    states_filter,
    bAllEqualStates,
    components_with_set_states
    );
}


unsigned int ON_SubD::GetComponentsWithSetStates(
  ON_ComponentStatus states_filter,
  bool bAllEqualStates,
  ON_SimpleArray< ON_COMPONENT_INDEX >& components_with_set_states
  ) const
{
  components_with_set_states.SetCount(0);
  ON_SimpleArray< ON_SubDComponentPtr > cptr;
  GetComponentsWithSetStates(
    states_filter,
    bAllEqualStates,
    cptr
    );
  unsigned int count = cptr.UnsignedCount();
  if (count > 0)
  {
    components_with_set_states.Reserve(count);
    components_with_set_states.SetCount(count);
    const ON_SubDComponentPtr* cp = cptr.Array();
    ON_COMPONENT_INDEX* ci = components_with_set_states.Array();
    for ( const ON_SubDComponentPtr* cp1 = cp+count; cp < cp1; cp++ )
      *ci++ = cp->ComponentIndex();
  }
  return count;
}

unsigned int ON_SubDLevel::SetStates(
  ON_SubDComponentPtr component_ptr,
  ON_ComponentStatus states_to_set
  ) const
{
  if (0 != component_ptr.SetStates(states_to_set))
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
    return 1;
  }
  return 0;
}

unsigned int ON_SubDLevel::ClearStates(
  ON_SubDComponentPtr component_ptr,
  ON_ComponentStatus states_to_clear
  ) const
{
  if (0 != component_ptr.ClearStates(states_to_clear))
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
    return 1;
  }
  return 0;
}

unsigned int ON_SubDLevel::SetStatus(
  ON_SubDComponentPtr component_ptr,
  ON_ComponentStatus status_to_copy
  ) const
{
  if (0 != component_ptr.SetStatus(status_to_copy))
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
    return 1;
  }
  return 0;
}

unsigned int ON_SubD::SetComponentStates(
  ON_COMPONENT_INDEX component_index,
  ON_ComponentStatus states_to_set
  ) const
{
  return SetComponentStates(ComponentPtrFromComponentIndex(component_index),states_to_set);
}

unsigned int ON_SubD::SetComponentStates(
  ON_SubDComponentPtr component_ptr,
  ON_ComponentStatus states_to_set
  ) const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();
  if ( nullptr != level )
    return level->SetStates(component_ptr,states_to_set);
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubD::ClearComponentStates(
  ON_COMPONENT_INDEX component_index,
  ON_ComponentStatus states_to_clear
  ) const
{
  return ClearComponentStates(ComponentPtrFromComponentIndex(component_index),states_to_clear);
}

unsigned int ON_SubD::ClearComponentStates(
  ON_SubDComponentPtr component_ptr,
  ON_ComponentStatus states_to_clear
  ) const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();
  if ( nullptr != level )
    return level->ClearStates(component_ptr,states_to_clear);
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubD::SetComponentStatus(
  ON_COMPONENT_INDEX component_index,
  ON_ComponentStatus status_to_copy
  ) const
{
  return ClearComponentStates(ComponentPtrFromComponentIndex(component_index),status_to_copy);
}

unsigned int ON_SubD::SetComponentStatus(
  ON_SubDComponentPtr component_ptr,
  ON_ComponentStatus status_to_copy
  ) const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();
  if ( nullptr != level )
    return level->SetStatus(component_ptr,status_to_copy);
  return ON_SUBD_RETURN_ERROR(0);
}

void ON_SubDLevel::ClearEvaluationCache() const
{
  ClearEdgeFlags();
  ClearBoundingBox();
  m_surface_mesh = ON_SubDMesh::Empty;
  m_control_net_mesh = ON_SubDMesh::Empty;

  for (const ON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
  {
    vertex->ClearSavedSubdivisionPoints();
  }

  for (const ON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    edge->ClearSavedSubdivisionPoints();
    // NO // edge->UnsetSectorCoefficients(); 
    // Leave these set - they are not "cached" values and except for corner case below
    // the are independent vertex locations.
    if ( edge->IsSmooth() )
    {
      for (unsigned evi = 0; evi < 2; evi++)
      {
        if ( false == (edge->m_sector_coefficient[evi] > 0.0 && edge->m_sector_coefficient[evi] < 1.0) )
          continue;
        const ON_SubDVertex* v = edge->m_vertex[evi];
        if (nullptr == v)
          continue;
        if (ON_SubD::VertexTag::Corner != v->m_vertex_tag)
          continue;
        // corner sector coefficients depend on the subtended angle of the sector's crease boundary.
        // All other sector coefficients are independent of vertex location.
        const_cast<ON_SubDEdge* >(edge)->m_sector_coefficient[evi] = ON_SubDSectorType::Create(edge, evi).SectorCoefficient();
      }
    }
  }

  for (const ON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
  {
    face->ClearSavedSubdivisionPoints();
  }
}

void ON_SubDLevel::ClearNeighborhoodEvaluationCache(const ON_SubDVertex * vertex0, bool bTagChanged ) const
{
  ClearEdgeFlags();
  ClearBoundingBox();
  m_surface_mesh = ON_SubDMesh::Empty;
  m_control_net_mesh = ON_SubDMesh::Empty;

  if (nullptr == vertex0)
    return;

  vertex0->ClearSavedSubdivisionPoints();

  for (unsigned short v0ei = 0; v0ei < vertex0->m_edge_count; ++v0ei)
  {
    const ON_SubDEdge* edge0 = ON_SUBD_EDGE_POINTER(vertex0->m_edges[v0ei].m_ptr);
    if (nullptr == edge0)
      continue;
    edge0->ClearSavedSubdivisionPoints();
    if (bTagChanged)
      edge0->UnsetSectorCoefficientsForExperts();
    else if (edge0->IsSmooth())
    {
      for (unsigned evi = 0; evi < 2; evi++)
      {
        if (false == (edge0->m_sector_coefficient[evi] > 0.0 && edge0->m_sector_coefficient[evi] < 1.0))
          continue;
        const ON_SubDVertex* v = edge0->m_vertex[evi];
        if (nullptr == v)
          continue;
        if (ON_SubD::VertexTag::Corner != v->m_vertex_tag)
          continue;
        // corner sector coefficients depend on the subtended angle of the sector's crease boundary.
        // All other sector coefficients are independent of vertex location.
        edge0->UnsetSectorCoefficientsForExperts();
      }
    }
  }

  for (unsigned short v0fi = 0; v0fi < vertex0->m_face_count; ++v0fi)
  {
    const ON_SubDFace* face0 = vertex0->m_faces[v0fi];
    if (nullptr == face0)
      continue;
    face0->ClearSavedSubdivisionPoints();
    const ON_SubDEdgePtr* face0_eptr = face0->m_edge4;
    for (unsigned short f0ei = 0; f0ei < face0->m_edge_count; ++f0ei, ++face0_eptr)
    {
      if (4 == f0ei)
      {
        face0_eptr = face0->m_edgex;
        if (nullptr == face0_eptr)
          break;
      }
      const ON_SubDEdge* edge0 = ON_SUBD_EDGE_POINTER(face0_eptr->m_ptr);
      if (nullptr == edge0)
        continue;
      edge0->ClearSavedSubdivisionPoints();
      const ON_SubDVertex* vertex1 = edge0->m_vertex[ON_SUBD_EDGE_DIRECTION(face0_eptr->m_ptr)];
      if (vertex0 == vertex1 || nullptr == vertex1)
        continue;
      vertex1->ClearSavedSubdivisionPoints();
      for (unsigned short v1fi = 0; v1fi < vertex1->m_face_count; ++v1fi)
      {
        const ON_SubDFace* face1 = vertex1->m_faces[v1fi];
        if (nullptr == face1 || face0 == face1)
          continue;
        face1->ClearSavedSubdivisionPoints();
      }
      for (unsigned short v1ei = 0; v1ei < vertex1->m_edge_count; ++v1ei)
      {
        const ON_SubDEdge* edge1 = ON_SUBD_EDGE_POINTER(vertex1->m_edges[v1ei].m_ptr);
        if (nullptr == edge1)
          continue;
        edge1->ClearSavedSubdivisionPoints();
      }
    }
  }
}

unsigned int ON_SubD::ComponentPtrFromComponentIndex(
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  bool bIncludeVertices,
  bool bIncludeEdges,
  bool bIncludeFaces,
  ON_SimpleArray<ON_SubDComponentPtr>& cptr_list
) const
{
  if ( ci_count <= 0 )
    return 0;

  if (
    false == bIncludeVertices
    && false == bIncludeEdges
    && false == bIncludeFaces
    )
    return 0;

  if ( nullptr == ci_list )
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned int count0 = cptr_list.UnsignedCount();

  cptr_list.Reserve(count0 + ci_count);

  const bool bFilter
    = false == bIncludeVertices
    || false == bIncludeEdges
    || false == bIncludeFaces;



  for (size_t i = 0; i < ci_count; i++)
  {
    const ON_COMPONENT_INDEX ci = ci_list[i];
    if (bFilter)
    {
      if (false == bIncludeVertices || ON_COMPONENT_INDEX::TYPE::subd_vertex == ci.m_type)
        continue;
      if (false == bIncludeEdges || ON_COMPONENT_INDEX::TYPE::subd_edge == ci.m_type)
        continue;
      if (false == bIncludeFaces || ON_COMPONENT_INDEX::TYPE::subd_face == ci.m_type)
        continue;
    }
    ON_SubDComponentPtr cptr = ComponentPtrFromComponentIndex(ci_list[i]);
    if (cptr.IsNull())
      continue;
    cptr_list.Append(cptr);
  }

  return (cptr_list.UnsignedCount() - count0);
}

unsigned int ON_SubD::ComponentPtrFromComponentIndex(
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  ON_SimpleArray<ON_SubDComponentPtr>& cptr_list
) const
{
  return ComponentPtrFromComponentIndex(ci_list, ci_count, true, true, true, cptr_list);
}



bool ON_SubD::DeleteComponents(
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count
  )
{
  ON_SimpleArray<ON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return true; // nothing to delete

  return DeleteComponents(cptr_list.Array(),cptr_list.UnsignedCount(),false);
}

bool ON_SubD::DeleteComponents(
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bMarkDeletedFaceEdges
)
{
  const bool bDeleteIsolatedEdges = true;
  const bool bUpdateTagsAndCoefficients = true;
  return DeleteComponentsForExperts(cptr_list, cptr_count, bDeleteIsolatedEdges, bUpdateTagsAndCoefficients, bMarkDeletedFaceEdges);
}

bool ON_SubD::DeleteComponentsForExperts(
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bDeleteIsolatedEdges,
  bool bUpdateTagsAndCoefficients,
  bool bMarkDeletedFaceEdges
)
{
  if (bMarkDeletedFaceEdges)
    ClearComponentMarks(false,true,false,nullptr);

  if ( cptr_count <= 0 )
    return true;

  if ( nullptr == cptr_list )
    return ON_SUBD_RETURN_ERROR(false);

  ON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int level_count = subdimple->LevelCount();
  if (level_count <= 0)
    return ON_SUBD_RETURN_ERROR(false);
  
  unsigned level_index = level_count;
  for (size_t i = 0; i < cptr_count; i++)
  {
    const ON_SubDComponentBase* c = cptr_list[i].ComponentBase();
    if ( nullptr == c)
      continue;
    if ( c->SubdivisionLevel() < level_index )
      level_index = c->SubdivisionLevel();
  }
  if ( level_index == level_count )
    return ON_SUBD_RETURN_ERROR(false);

  if ( false == subdimple->SetActiveLevel(level_index) )
    return ON_SUBD_RETURN_ERROR(false);

  subdimple->ClearHigherSubdivisionLevels(level_index);
  const ON_SubDLevel* level = subdimple->ActiveLevelPointer();
  if ( nullptr == level || level->m_level_index != level_index)
    return ON_SUBD_RETURN_ERROR(false);

  // Make sure no components have a status = ON_ComponentStatus::AllSet
  // because this uncommon status value is used to mark components that will be be deleted.
  ON_SubDComponentIterator cit(*this);
  for (ON_SubDComponentPtr cptr = cit.FirstComponent(); cptr.IsNotNull(); cptr = cit.NextComponent())
  {
    if ( ON_ComponentStatus::AllSet == cptr.Status() )
      cptr.ClearStates(ON_ComponentStatus::Damaged);
  }

  // Set the status of every compoent in cptr_list[] to ON_ComponentStatus::AllSet.
  // If that component is a vertex, set the status of every edge and face that
  // touch the vertex to ON_ComponentStatus::AllSet.
  // If that component is an edge, set the status of every face that
  // touches the edge to ON_ComponentStatus::AllSet.
  for (size_t i = 0; i < cptr_count; i++)
  {
    ON_SubDComponentPtr cptr = cptr_list[i];
    const ON_SubDComponentBase* c = cptr.ComponentBase();
    if (nullptr == c)
      continue;
    if (c->SubdivisionLevel() != level_index)
      continue;
    c->m_status = ON_ComponentStatus::AllSet;
    switch (cptr.ComponentType())
    {
    case ON_SubDComponentPtr::Type::Vertex:
      {
        const ON_SubDVertex* vertex = cptr.Vertex();
        if (nullptr == vertex)
          continue;
        for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
        {
          const ON_SubDEdge* edge = vertex->Edge(vei);
          if (nullptr == edge)
            continue;
          edge->m_status = ON_ComponentStatus::AllSet;
        }
        for (unsigned short vfi = 0; vfi < vertex->m_face_count; vfi++)
        {
          const ON_SubDFace* face = vertex->Face(vfi);
          if (nullptr == face)
            continue;
          face->m_status = ON_ComponentStatus::AllSet;
        }
      }
      break;

    case ON_SubDComponentPtr::Type::Edge:
      {
        const ON_SubDEdge* edge = cptr.Edge();
        if (nullptr == edge)
          continue;
        edge->m_status = ON_ComponentStatus::AllSet;
        for (unsigned short efi = 0; efi < edge->m_face_count; efi++)
        {
          const ON_SubDFace* face = edge->Face(efi);
          if (nullptr == face)
            continue;
          face->m_status = ON_ComponentStatus::AllSet;
        }
      }
      break;

    case ON_SubDComponentPtr::Type::Face:
      {
        const ON_SubDFace* face = cptr.Face();
        if (nullptr == face)
          continue;
        face->m_status = ON_ComponentStatus::AllSet;
      }
      break;
    }
  }

  // Minimum count of what will be deleted. (
  unsigned int deleted_vertex_count = 0;
  unsigned int deleted_edge_count = 0;
  unsigned int deleted_face_count = 0;
  for (ON_SubDComponentPtr cptr = cit.FirstComponent(); cptr.IsNotNull(); cptr = cit.NextComponent())
  {
    if (ON_ComponentStatus::AllSet == cptr.Status())
    {
      switch (cptr.ComponentType())
      {
      case ON_SubDComponentPtr::Type::Vertex:
        deleted_vertex_count++;
        break;

      case ON_SubDComponentPtr::Type::Edge:
        deleted_edge_count++;
        break;

      case ON_SubDComponentPtr::Type::Face:
        deleted_face_count++;
        break;
      }
      continue;
    }
  }

  if ( 0 == deleted_vertex_count && 0 == deleted_edge_count && 0 ==  deleted_face_count )
    return false;

  const bool bDestroy
    = deleted_vertex_count >= level->m_vertex_count
    || deleted_edge_count >= level->m_edge_count
    || (deleted_face_count >= level->m_face_count && bDeleteIsolatedEdges)
    ;
  if (bDestroy)
  {
    Destroy();
    return true;
  }

  unsigned int deleted_component_count = subdimple->DeleteComponents(level_index,bDeleteIsolatedEdges,bUpdateTagsAndCoefficients,bMarkDeletedFaceEdges);

  if (0 == subdimple->LevelCount())
  {
    Destroy();
    return true;
  }

  if (deleted_component_count > 0 || level_index > 0)
  {
    // remove lower levels
    subdimple->ClearLowerSubdivisionLevels(level_index);
  }

  return (deleted_component_count > 0);
}

unsigned int ON_SubDLevel::UpdateEdgeTags(
  bool bUnsetEdgeTagsOnly
  )
{
  // Update edge flags and sector weights.
  unsigned int edge_change_count = 0;
  ON_SubDEdge* next_edge = m_edge[0];
  for (ON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast<ON_SubDEdge*>(edge->m_next_edge);

    const ON_SubD::EdgeTag edge_tag0 = edge->m_edge_tag;
    if (bUnsetEdgeTagsOnly && ON_SubD::EdgeTag::Unset != edge_tag0 )
    {
      continue;
    }

    if (nullptr == edge->m_vertex[0] || nullptr == edge->m_vertex[1])
    {
      ON_SUBD_ERROR("nullptr edge->m_vertex[] values");
      continue;
    }

    const double edge_sector_coefficient0[2] = { edge->m_sector_coefficient[0], edge->m_sector_coefficient[1] };

    if (2 != edge->m_face_count)
    {
      edge->m_edge_tag = ON_SubD::EdgeTag::Crease;
      edge->m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;
      edge->m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
    }
    else
    {
      edge->m_sector_coefficient[0] = ON_SubDSectorType::UnsetSectorCoefficient;
      edge->m_sector_coefficient[1] = ON_SubDSectorType::UnsetSectorCoefficient;
      const bool bBothVertexTagsAreSet
        =  ON_SubD::VertexTag::Unset != edge->m_vertex[0]->m_vertex_tag
        && ON_SubD::VertexTag::Unset != edge->m_vertex[1]->m_vertex_tag
        ;
      const unsigned int tagged_end_index = edge->TaggedEndIndex();
      if (0 == tagged_end_index || 1 == tagged_end_index)
        edge->m_sector_coefficient[tagged_end_index] = ON_SubDSectorType::IgnoredSectorCoefficient;

      switch (edge_tag0)
      {
      case ON_SubD::EdgeTag::Unset:
        if (2 == tagged_end_index)
        {
          edge->m_edge_tag = ON_SubD::EdgeTag::SmoothX;
        }
        else if ( bBothVertexTagsAreSet )
        {
          edge->m_edge_tag = ON_SubD::EdgeTag::Smooth;
          if (3 == tagged_end_index)
          {
            edge->m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;
            edge->m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
          }
        }
        break;

      case ON_SubD::EdgeTag::Smooth:
        if (2 == tagged_end_index)
        {
          edge->m_edge_tag = ON_SubD::EdgeTag::SmoothX;
        }
        else if (3 == tagged_end_index && bBothVertexTagsAreSet)
        {
          edge->m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;
          edge->m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
        }
        break;

      case ON_SubD::EdgeTag::Crease:
        edge->m_sector_coefficient[0] = ON_SubDSectorType::IgnoredSectorCoefficient;
        edge->m_sector_coefficient[1] = ON_SubDSectorType::IgnoredSectorCoefficient;
        break;

      //case ON_SubD::EdgeTag::Sharp:
      //  ON_SUBD_ERROR("ON_SubD::EdgeTag::Sharp is not valid in this version of opennurbs.");
      //  break;

      case ON_SubD::EdgeTag::SmoothX:
        if ( 2 != tagged_end_index && bBothVertexTagsAreSet)
          edge->m_edge_tag = ON_SubD::EdgeTag::Smooth;
        break;

      default:
        break;
      }
    }

    if (!(edge_tag0 == edge->m_edge_tag
      && edge_sector_coefficient0[0] == edge->m_sector_coefficient[0]
      && edge_sector_coefficient0[1] == edge->m_sector_coefficient[1]))
      edge_change_count++;
  }

  return edge_change_count;
}


unsigned int ON_SubDLevel::UpdateVertexTags(
  bool bUnsetVertexTagsOnly
  )
{
  // Update edge flags and sector weights.
  unsigned int vertex_change_count = 0;
 

  ON_SubDVertex* next_vertex = m_vertex[0];
  for (ON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
  {
    next_vertex = const_cast<ON_SubDVertex*>(vertex->m_next_vertex);

    const ON_SubD::VertexTag vertex_tag0 = vertex->m_vertex_tag;
    if (bUnsetVertexTagsOnly && ON_SubD::VertexTag::Unset != vertex_tag0 )
    {
      continue;
    }

    const unsigned int edge_count = vertex->m_edge_count;
    unsigned int creased_edge_count = 0;
    unsigned int sharp_edge_count = 0;
    for (unsigned int vei = 0; vei < edge_count; vei++)
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
      if (nullptr == edge)
      {
        ON_SUBD_ERROR("nullptr vertex->m_edges[] values");
        continue;
      }
      if ( ON_SubD::EdgeTag::Crease == edge->m_edge_tag || 2 != edge->m_face_count )
        creased_edge_count++;
      else if (((ON_SubD::EdgeTag)3) == edge->m_edge_tag) // ON_SubD::EdgeTag::Sharp
      {
        ON_SUBD_ERROR("ON_SubD::EdgeTag::Sharp is not valid in this version of opennurbs.");
        sharp_edge_count++;
      }

      // NOTE: 
      //  edges tagged as ON_SubD::EdgeTag::Unset with two faces
      //  ending at a vertex with 3 or more edges
      //  will be tagged as smooth in subsequent passes
      //  once this vertex is tagged as smooth.
    }


    ON_SubD::VertexTag vertex_tag1 = vertex_tag0;
    if ( (creased_edge_count+sharp_edge_count) >= 2 )
    {
      if ( ON_SubD::VertexTag::Corner != vertex_tag0 )
        vertex_tag1 = ON_SubD::VertexTag::Crease;
    }
    else if ( edge_count >= 2 )
    {
      if ( 1 == creased_edge_count && 0 == sharp_edge_count )
        vertex_tag1 = ON_SubD::VertexTag::Dart;
      else
        vertex_tag1 = ON_SubD::VertexTag::Smooth;
    }

    if (vertex_tag0 != vertex_tag1)
    {
      vertex->m_vertex_tag = vertex_tag1;
      vertex_change_count++;
    }
  }

  return vertex_change_count;
}


unsigned int ON_SubDLevel::UpdateAllTagsAndSectorCoefficients(
  bool bUnsetValuesOnly
)
{
  unsigned int change_count = 0;

  bool bUpdateEdges = true;
  bool bUpdateVertices = true;
  for ( unsigned int it_count = 0; it_count < 8; it_count++)
  {
    const unsigned int edge_change_count 
      = bUpdateEdges
      ? UpdateEdgeTags(bUnsetValuesOnly)
      : 0;
    bUpdateVertices = (edge_change_count > 0 || 0 == it_count);
    change_count += edge_change_count;

    const unsigned int vertex_change_count 
      = bUpdateVertices
      ? UpdateVertexTags(bUnsetValuesOnly)
      : 0;
    bUpdateEdges = (vertex_change_count > 0);    
    change_count += vertex_change_count;

    bUpdateVertices = false;
    if ( false == bUpdateEdges)
      break;
  }

  if (bUpdateVertices && bUpdateEdges)
  {
    ON_SUBD_ERROR("Recursion limit exceeded.");
  }

  // Adjust edge tag smooth/X settings
  // This must be done before UpdateEdgeSectorCoefficients().
  // It is done between the heavy handed setting above so as not to disturb that delicate code.
  ON_SubDEdge* next_edge = m_edge[0];
  for (ON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast<ON_SubDEdge*>(edge->m_next_edge);
    if (edge->IsSmooth())
    {
      const ON_SubD::EdgeTag etag = (2 == edge->TaggedEndIndex()) ? ON_SubD::EdgeTag::SmoothX : ON_SubD::EdgeTag::Smooth;
      if (etag != edge->m_edge_tag)
      {
        edge->m_edge_tag = etag;
        ++change_count;
      }
    }
  }

  change_count += UpdateEdgeSectorCoefficients(false);

  if (change_count > 0)
  {
    m_surface_mesh = ON_SubDMesh::Empty;
    m_control_net_mesh = ON_SubDMesh::Empty;
  }    

  return change_count;
}

unsigned int ON_SubDLevel::ClearComponentDamagedState() const
{
  return ClearComponentDamagedState(true, true, true);
}

unsigned int ON_SubDLevel::ClearComponentDamagedState(
  bool bClearVerticesDamagedState,
  bool bClearEdgesDamagedState,
  bool bClearFacesDamagedState
) const
{
  unsigned int change_count = 0;
  unsigned int i;
  if (bClearVerticesDamagedState)
  {
    i = 0;
    for (const ON_SubDVertex* v = m_vertex[0]; nullptr != v && i++ < m_vertex_count; v = v->m_next_vertex)
      if (0 != v->m_status.SetDamagedState(false))
        ++change_count;
  }
  if (bClearEdgesDamagedState)
  {
    i = 0;
    for (const ON_SubDEdge* e = m_edge[0]; nullptr != e && i++ < m_edge_count; e = e->m_next_edge)
      if (0 != e->m_status.SetDamagedState(false))
        ++change_count;
  }
  if (bClearFacesDamagedState)
  {
    i = 0;
    for (const ON_SubDFace* f = m_face[0]; nullptr != f && i++ < m_face_count; f = f->m_next_face)
      if (0 != f->m_status.SetDamagedState(false))
        ++change_count;
  }
  return change_count;
}


unsigned int ON_SubD::UpdateVertexTags(
    bool bUnsetVertexTagsOnly
    )
{
  ON_SubDLevel* level = ActiveLevelPointer();
  if ( nullptr == level )
    return ON_SUBD_RETURN_ERROR(0);

  return level->UpdateVertexTags(bUnsetVertexTagsOnly);
}

unsigned int ON_SubD::UpdateEdgeTags(
  bool bUnsetEdgeTagsOnly
  )
{
  ON_SubDLevel* level = ActiveLevelPointer();
  if ( nullptr == level )
    return ON_SUBD_RETURN_ERROR(0);

  return level->UpdateEdgeTags(bUnsetEdgeTagsOnly);
}


unsigned int ON_SubD::UpdateEdgeSectorCoefficients(
    bool bUnsetSectorCoefficientsOnly
    )
{
  ON_SubDLevel* level = ActiveLevelPointer();
  if ( nullptr == level )
    return ON_SUBD_RETURN_ERROR(0);

  return level->UpdateEdgeSectorCoefficients(bUnsetSectorCoefficientsOnly);
}

void ON_SubD::SubDModifiedNofification()
{
  // DestroyRuntimeCache() 
  // Clears
  //  Saved subdivision points.
  //  Saved limit surface information.
  //  Bounding boxes.
  //  
  DestroyRuntimeCache();

  // This is a heavy handed tag update.
  UpdateAllTagsAndSectorCoefficients(false);
}

unsigned int ON_SubD::UpdateAllTagsAndSectorCoefficients(
  bool bUnsetValuesOnly
)
{
  ON_SubDLevel* level = ActiveLevelPointer();
  if ( nullptr == level )
    return ON_SUBD_RETURN_ERROR(0);

  return level->UpdateAllTagsAndSectorCoefficients(bUnsetValuesOnly);
}

unsigned int ON_SubDimple::DeleteComponents(
  unsigned int level_index,
  bool bDeleteIsolatedEdges,
  bool bUpdateTagsAndCoefficients,
  bool bMarkDeletedFaceEdges
  )
{ 
  unsigned int deleted_component_count = 0;

  if (level_index >= m_levels.UnsignedCount())
    return ON_SUBD_RETURN_ERROR(0);

  ON_SubDLevel* level = m_levels[level_index];
  if (nullptr == level)
    return ON_SUBD_RETURN_ERROR(0);

  if (bMarkDeletedFaceEdges)
    level->ClearRuntimeMarks(false,true,false);

  ON_SubDFace* next_face = level->m_face[0];
  for (ON_SubDFace* face = next_face; nullptr != face; face = next_face)
  {
    next_face = const_cast< ON_SubDFace* >(face->m_next_face);
    bool bDelete = (ON_ComponentStatus::AllSet == face->m_status || 0 == face->m_edge_count);
    if (false == bDelete)
    {
      const ON_SubDEdgePtr* eptr = face->m_edge4;
      for (unsigned short fei = 0; fei < face->m_edge_count && false == bDelete; ++fei, ++eptr)
      {
        if (4 == fei)
        {
          eptr = face->m_edgex;
          if (nullptr == eptr)
            break;
        }
        const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
        if (nullptr == edge
          || nullptr == edge->m_vertex[0]
          || nullptr == edge->m_vertex[1]
          || ON_ComponentStatus::AllSet == edge->m_status
          || ON_ComponentStatus::AllSet == edge->m_vertex[0]->m_status
          || ON_ComponentStatus::AllSet == edge->m_vertex[1]->m_status
          )
        {
          bDelete = true;
          if (nullptr != edge && ON_ComponentStatus::AllSet != edge->m_status)
            edge->m_status = ON_ComponentStatus::AllSet;
        }
      }
      if (false == bDelete)
        continue;
    }

    if (bMarkDeletedFaceEdges)
    {
      // Set runtime mark on face's boundary edges.
      const ON_SubDEdgePtr* eptr = face->m_edge4;
      for (unsigned short fei = 0; fei < face->m_edge_count ; ++fei, ++eptr)
      {
        if (4 == fei)
        {
          eptr = face->m_edgex;
          if (nullptr == eptr)
            break;
        }
        const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
        if (nullptr != edge)
          edge->m_status.SetRuntimeMark();
      }
    }

    level->RemoveFace(face);
    m_heap.ReturnFace(face);
    deleted_component_count++;
  }

  ON_ComponentStatus allsetcheck;
  ON_SubDEdge* next_edge = level->m_edge[0];
  for (ON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast< ON_SubDEdge* >(edge->m_next_edge);
    allsetcheck = ON_ComponentStatus::LogicalAnd(ON_ComponentStatus::AllSet, edge->m_status);
    bool bDelete = (ON_ComponentStatus::AllSet == allsetcheck || (bDeleteIsolatedEdges && 0 == edge->m_face_count) );
    if (false == bDelete)
    {
      for (unsigned short evi = 0; evi < 2 && false == bDelete; evi++)
      {
        if (nullptr == edge->m_vertex[0]
          || nullptr == edge->m_vertex[1]
          || ON_ComponentStatus::AllSet == edge->m_vertex[0]->m_status
          || ON_ComponentStatus::AllSet == edge->m_vertex[1]->m_status
          )
          bDelete = true;
      }
      if (false == bDelete)
        continue;
    }

    level->RemoveEdge(edge);
    m_heap.ReturnEdge(edge);
    deleted_component_count++;
  }

  ON_SubDVertex* next_vertex = level->m_vertex[0];
  for (ON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
  {
    next_vertex = const_cast<ON_SubDVertex*>(vertex->m_next_vertex);
    allsetcheck = ON_ComponentStatus::LogicalAnd(ON_ComponentStatus::AllSet, vertex->m_status);
    bool bDelete = (ON_ComponentStatus::AllSet == allsetcheck || (bDeleteIsolatedEdges && 0 == vertex->m_face_count) || 0 == vertex->m_edge_count );
    if ( false == bDelete )
      continue;

    level->RemoveVertex(vertex);
    m_heap.ReturnVertex(vertex);
    deleted_component_count++;
  }

  if ( 0 == deleted_component_count )
    return 0;

  // Remove edge references to deleted faces
  next_edge = level->m_edge[0];
  for (ON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast<ON_SubDEdge*>(edge->m_next_edge);
    ON_SubDFacePtr* fptr0 = edge->m_face2;
    ON_SubDFacePtr* fptr1 = edge->m_face2;
    const unsigned short edge_face_count = edge->m_face_count;
    edge->m_face_count = 0;
    for (unsigned short efi = 0; efi < edge_face_count; efi++, fptr0++)
    {
      if (2 == efi)
        fptr0 = edge->m_face2;
      const ON_SubDFace* face = fptr0->Face();
      if (nullptr == face || ON_UNSET_UINT_INDEX == face->ArchiveId())
        continue;
      *fptr1++ = *fptr0;
      edge->m_face_count++;
      if (2 == edge->m_face_count)
        fptr1 = edge->m_facex;
    }

    if (0 == edge->m_face_count && bDeleteIsolatedEdges)
    {
      level->RemoveEdge(edge);
      m_heap.ReturnEdge(edge);
      deleted_component_count++;
      continue;
    }
    
    if (edge->m_face_count <= 2 && nullptr != edge->m_facex)
      m_heap.ReturnEdgeExtraArray(edge);

    if (bUpdateTagsAndCoefficients)
    {
      if (edge->m_face_count != 2)
        edge->m_edge_tag = ON_SubD::EdgeTag::Crease;

      edge->m_sector_coefficient[0] = ON_SubDSectorType::UnsetSectorCoefficient;
      edge->m_sector_coefficient[1] = ON_SubDSectorType::UnsetSectorCoefficient;
    }
  }

  // Remove vertex references to deleted edges and faces
  next_vertex = level->m_vertex[0];
  for (ON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
  {
    next_vertex = const_cast<ON_SubDVertex*>(vertex->m_next_vertex);

    unsigned int count = vertex->m_edge_count;
    vertex->m_edge_count = 0;

    bool bInteriorVertex = true;
    unsigned int crease_count = 0;
    for (unsigned short vei = 0; vei < count; vei++)
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
      if (nullptr == edge || ON_UNSET_UINT_INDEX == edge->ArchiveId())
      {
        bInteriorVertex = false;
        continue;
      }
      if (edge->IsCrease())
        crease_count++;
      if (2 != edge->m_face_count)
        bInteriorVertex = false;
      vertex->m_edges[vertex->m_edge_count++] = vertex->m_edges[vei];
    }


    count = vertex->m_face_count;
    vertex->m_face_count = 0;
    for (unsigned short vfi = 0; vfi < count; vfi++)
    {
      const ON_SubDFace* face = vertex->m_faces[vfi];
      if (nullptr == face || ON_UNSET_UINT_INDEX == face->ArchiveId()) 
        continue;
      vertex->m_faces[vertex->m_face_count++] = vertex->m_faces[vfi];
    }

    if (0 == vertex->m_face_count && 0 == vertex->m_edge_count)
    {
      level->RemoveVertex(vertex);
      m_heap.ReturnVertex(vertex);
      deleted_component_count++;
    }
    else
    {
      if (1 == crease_count && 1 == vertex->m_edge_count && 0 == vertex->m_face_count)
        vertex->m_vertex_tag = ON_SubD::VertexTag::Corner;
      else if (crease_count > 2)
        vertex->m_vertex_tag = ON_SubD::VertexTag::Corner;
      else if (false == bInteriorVertex || crease_count > 1)
      {
        if (false == vertex->IsCreaseOrCorner())
          vertex->m_vertex_tag = ON_SubD::VertexTag::Crease;
      }
    }
  }

  if (0 == level->m_vertex_count || 0 == level->m_edge_count || (bDeleteIsolatedEdges && 0 == level->m_face_count))
  {
    Destroy();
  }
  else
  {
    // remove all information that is no longer valid
    level->MarkAggregateComponentStatusAsNotCurrent();
    level->ClearEvaluationCache();

    ClearHigherSubdivisionLevels(level_index);

    if (bUpdateTagsAndCoefficients)
    {
      // Update vertex tags, edge tags, and sector weights.
      level->UpdateAllTagsAndSectorCoefficients(false);
    }
  }

  ChangeContentSerialNumber();

  return deleted_component_count;
}



unsigned int ON_SubD::ClearComponentMarks() const
{
  return ClearComponentMarks(true, true, true, nullptr);
}

unsigned int ON_SubD::ClearVertexMarks() const
{
  return ClearComponentMarks(true, false, false, nullptr);
}

unsigned int ON_SubD::ClearEdgeMarks() const
{
  return ClearComponentMarks(false, true, false, nullptr);
}

unsigned int ON_SubD::ClearFaceMarks() const
{
  return ClearComponentMarks(false, false, true, nullptr);
}


unsigned int ON_SubD::ClearComponentMarks(
  bool bClearVertexMarks,
  bool bClearEdgeMarks,
  bool bClearFaceMarks,
  ON_SimpleArray< const ON_SubDComponentBase* >* marked_component_list
) const
{
  unsigned int clear_count = 0;

  if (bClearVertexMarks)
  {
    ON_SubDVertexIterator vit(*this);
    for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (v->m_status.ClearRuntimeMark())
      {
        if (nullptr != marked_component_list)
          marked_component_list->Append(v);
        clear_count++;
      }
    }
  }

  if (bClearEdgeMarks)
  {
    ON_SubDEdgeIterator eit(*this);
    for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (e->m_status.ClearRuntimeMark())
      {
        if (nullptr != marked_component_list)
          marked_component_list->Append(e);
        clear_count++;
      }
    }
  }

  if (bClearFaceMarks)
  {
    ON_SubDFaceIterator fit(*this);
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (f->m_status.ClearRuntimeMark())
      {
        if (nullptr != marked_component_list)
          marked_component_list->Append(f);
        clear_count++;
      }
    }
  }

  return clear_count;
}

unsigned int ON_SubD::UnselectComponents(
    bool bUnselectAllVertices,
    bool bUnselectAllEdges,
    bool bUnselectAllFaces
  ) const
{
  unsigned int unselected_count = 0;

  if (bUnselectAllVertices)
  {
    ON_SubDVertexIterator vit(*this);
    for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (v->m_status.SetSelectedState(false, false, false))
        ++unselected_count;
    }
  }

  if (bUnselectAllEdges)
  {
    ON_SubDEdgeIterator eit(*this);
    for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (e->m_status.SetSelectedState(false, false, false))
        ++unselected_count;
    }
  }

  if (bUnselectAllFaces)
  {
    ON_SubDFaceIterator fit(*this);
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (f->m_status.SetSelectedState(false, false, false))
        ++unselected_count;
    }
  }

  return unselected_count;
}


unsigned int ON_SubD::SetComponentMarks(
  bool bClearBeforeSet,
  const ON_SimpleArray< const ON_SubDComponentBase* >& marked_component_list
) const
{
  unsigned int set_count = 0;

  if (bClearBeforeSet)
    ClearComponentMarks(true, true, true, nullptr);

  const unsigned count = marked_component_list.Count();
  if (count <= 0)
    return 0;
  const ON_SubDComponentBase*const* a = marked_component_list.Array();
  if (nullptr == a)
    return 0;

  for (const ON_SubDComponentBase*const* a1 = a; a < a1; a++)
  {
    const ON_SubDComponentBase* c = *a;
    if (nullptr == c)
      continue;
    if (c->m_status.SetRuntimeMark())
      set_count++;
  }

  return set_count;
}



unsigned int ON_SubD::GetMarkedComponents(
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
  ON_SimpleArray< const ON_SubDComponentBase* >& marked_component_list
) const
{
  unsigned int mark_count = 0;

  if (bIncludeVertices)
  {
    ON_SubDVertexIterator vit(*this);
    for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (v->m_status.RuntimeMark())
      {
        marked_component_list.Append(v);
        mark_count++;
      }
    }
  }

  if (bIncludeEdges)
  {
    ON_SubDEdgeIterator eit(*this);
    for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (e->m_status.RuntimeMark())
      {
        marked_component_list.Append(e);
        mark_count++;
      }
    }
  }

  if (bIncludeFaces)
  {
    ON_SubDFaceIterator fit(*this);
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (f->m_status.RuntimeMark())
      {
        marked_component_list.Append(f);
        mark_count++;
      }
    }
  }

  return mark_count;
}

unsigned int ON_SubD::GetComponentStatus(
  bool bGetVertexStatus,
  bool bGetEdgeStatus,
  bool bGetFaceStatus,
  bool bClearStatus,
  ON_ComponentStatus status_mask,
  ON_SimpleArray< const class ON_SubDComponentBase* >& component_list,
  ON_SimpleArray< ON_ComponentStatus >& status_list
) const
{
  component_list.SetCount(0);
  status_list.SetCount(0);
  if ( ON_ComponentStatus::NoneSet == status_mask )
    return 0;

  ON_ComponentStatus s;

  if (bGetVertexStatus)
  {
    ON_SubDVertexIterator vit(*this);
    for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      s = ON_ComponentStatus::LogicalAnd(status_mask, v->m_status);
      if (ON_ComponentStatus::NoneSet == s)
        continue;
      component_list.Append(v);
      status_list.Append(s);
    }
  }

  if (bGetEdgeStatus)
  {
    ON_SubDEdgeIterator eit(*this);
    for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      s = ON_ComponentStatus::LogicalAnd(status_mask, e->m_status);
      if (ON_ComponentStatus::NoneSet == s)
        continue;
      component_list.Append(e);
      status_list.Append(s);
    }
  }

  if (bGetFaceStatus)
  {
    ON_SubDFaceIterator fit(*this);
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      s = ON_ComponentStatus::LogicalAnd(status_mask, f->m_status);
      if (ON_ComponentStatus::NoneSet == s)
        continue;
      component_list.Append(f);
      status_list.Append(s);
    }
  }

  const unsigned int count = component_list.UnsignedCount();
  if (bClearStatus && count > 0)
  {
    const bool bRuntimeMark = status_mask.RuntimeMark();
    for (unsigned int i = 0; i < count; ++i)
    {
      const ON_SubDComponentBase* c = component_list[i];
      if (nullptr == c)
        continue;
      c->m_status.ClearStates(status_mask);
      if (bRuntimeMark)
        c->m_status.ClearRuntimeMark();
    }
  }

  return count;
}

unsigned int ON_SubD::SetComponentStatus(
  ON_ComponentStatus status_mask,
  const ON_SimpleArray< const class ON_SubDComponentBase* >& component_list,
  const ON_SimpleArray< ON_ComponentStatus >& status_list
) const
{
  const unsigned int count = component_list.UnsignedCount();
  if (count < 1 || count != status_list.UnsignedCount())
    return 0;

  const bool bRuntimeMark = status_mask.RuntimeMark();

  for (unsigned int i = 0; i < count; ++i)
  {
    const ON_SubDComponentBase* c = component_list[i];
    if (nullptr == c)
      continue;
    const ON_ComponentStatus s = status_list[i];
    c->m_status.ClearStates(status_mask);
    c->m_status.SetStates(s);
    if (bRuntimeMark)
    {
      if ( s.RuntimeMark())
        c->m_status.SetRuntimeMark();
      else
        c->m_status.ClearRuntimeMark();
    }
  }

  return count;
}

ON_SubDComponentMarksClearAndRestore::ON_SubDComponentMarksClearAndRestore(
  const ON_SubD& subd
)  
{
  m_subd.ShareDimple(subd);
  m_subd.ClearComponentMarks(true, true, true, &m_component_list);
}

ON_SubDComponentMarksClearAndRestore::~ON_SubDComponentMarksClearAndRestore()
{
  Restore(true);
}

const ON_SimpleArray<const class ON_SubDComponentBase*>& ON_SubDComponentMarksClearAndRestore::ComponentList() const
{
  return m_component_list;
}

bool ON_SubDComponentMarksClearAndRestore::Restore(
  bool bDisableFutureRestore )
{
  const bool rc = m_bRestore;
  if (rc)
  {
    if ( bDisableFutureRestore)
      m_bRestore = false;
    
    m_subd.ClearComponentMarks(true, true, true, nullptr);

//    if ( ON_ComponentStatus::Marked == m_status_mask )
    {
      // RuntimeMark is the only bit being managed
      if (m_component_list.UnsignedCount() > 0)
        m_subd.SetComponentMarks(false, m_component_list);
    }
    //else if ( m_status_mask.IsNotClear() )
    //{
    //  // something fancier is going on

    //  // clear current settings
    //  ON_SubDVertexIterator vit(m_subd);
    //  for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    //    v->m_status.ClearStates(m_status_mask);

    //  ON_SubDEdgeIterator eit(m_subd);
    //  for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    //    e->m_status.ClearStates(m_status_mask);

    //  ON_SubDFaceIterator fit(m_subd);
    //  for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    //    f->m_status.ClearStates(m_status_mask);

    //  // restore settings
    //  m_subd.SetComponentStatus(m_status_mask, m_component_list, m_status_list);
    //}

    if (bDisableFutureRestore)
    {
      m_component_list.Destroy();
      //m_status_list.Destroy();
    }
  }
  return rc;
}

void ON_SubDComponentMarksClearAndRestore::DisableRestore()
{
  m_bRestore = false;
}

unsigned int ON_SubD::TransformComponents(
  const ON_Xform& xform,
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  ON_SubDComponentLocation component_location
)
{
  if (
    false ==  xform.IsValidAndNotZeroAndNotIdentity()
    || ci_count <= 0
    || nullptr == ci_list
    )
    return 0;

  ON_SimpleArray<ON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return true; // nothing to delete

  return TransformComponents(xform,cptr_list.Array(),cptr_list.UnsignedCount(),component_location);
}



static unsigned int Internal_MarkStuffAndMaybeMoveVertices(
  const ON_SubD& subd,
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  const ON_Xform* xform,
  ON_SubDComponentLocation component_location,
  bool bExtrusionMarking,
  bool bExtrudeBoundaries,
  unsigned int& list_vertex_count,
  unsigned int& list_edge_count,
  unsigned int& list_face_count
)
{
  list_vertex_count = 0;
  list_edge_count = 0;
  list_face_count = 0;

  if (false == bExtrusionMarking)
    bExtrudeBoundaries = false;

  const bool bTransform 
    = false == bExtrusionMarking
    && nullptr != xform 
    && xform->IsValidAndNotZeroAndNotIdentity()
    ;

  if ((bTransform ? 1 : 0) == (bExtrusionMarking ? 1 : 0))
  {
    ON_SUBD_ERROR("Invalid input.");
    return 0;
  }

  unsigned int marked_vertex_count = 0;
  //unsigned int potential_isolated_vertex_count = 0;
  unsigned int potential_isolated_edge_count = 0;

  if (bExtrusionMarking && 0 == cptr_count && nullptr == cptr_list)
  {
    // entire subd is being extruded
    ON_SubDFaceIterator fit(subd);
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      ++list_face_count;
      f->m_status.SetRuntimeMark();
      const unsigned int face_vertex_count = f->m_edge_count;
      for (unsigned int fvi = 0; fvi < face_vertex_count; ++fvi)
      {
        // used when extruding selected components
        const ON_SubDEdge* e = f->Edge(fvi);
        if (nullptr != e && false == e->m_status.RuntimeMark())
          e->m_status.SetRuntimeMark();
        const ON_SubDVertex* v = f->Vertex(fvi);
        if (nullptr != v && false == v->m_status.RuntimeMark())
        {
          v->m_status.SetRuntimeMark();
          ++marked_vertex_count;
        }
      }
    }
  }
  else
  {

    for (size_t i = 0; i < cptr_count; i++)
    {
      switch (cptr_list[i].ComponentType())
      {
      case ON_SubDComponentPtr::Type::Vertex:
        {
          const ON_SubDVertex* v = cptr_list[i].Vertex();
          if (nullptr == v)
            continue;
          ++list_vertex_count;
          if (v->m_status.RuntimeMark())
            continue;
          if (bTransform)
          {
            v->m_status.SetRuntimeMark();
            if (bTransform)
              const_cast<ON_SubDVertex*>(v)->Transform(false, *xform);
            ++marked_vertex_count;
          }
        }
        break;

      case ON_SubDComponentPtr::Type::Edge:
        {
          const ON_SubDEdge* e = cptr_list[i].Edge();
          if (nullptr == e)
            continue;
          ++list_edge_count;
          if (e->m_status.RuntimeMark())
            continue;
          if (bTransform)
          {
            e->m_status.SetRuntimeMark();
            for (unsigned int evi = 0; evi < 2; ++evi)
            {
              const ON_SubDVertex* v = e->m_vertex[evi];
              if (nullptr != v && false == v->m_status.RuntimeMark())
              {
                v->m_status.SetRuntimeMark();
                const_cast<ON_SubDVertex*>(v)->Transform(false, *xform);
                ++marked_vertex_count;
              }
            }
          }
          else if (bExtrudeBoundaries && 1 == e->m_face_count && nullptr != e->m_face2[0].Face())
            ++potential_isolated_edge_count;
        }
        break;

      case ON_SubDComponentPtr::Type::Face:
        {
          const ON_SubDFace* f = cptr_list[i].Face();
          if (nullptr != f)
          {
            ++list_face_count;
            f->m_status.SetRuntimeMark();
            const unsigned int face_vertex_count = f->m_edge_count;
            for (unsigned int fvi = 0; fvi < face_vertex_count; ++fvi)
            {
              if (bExtrusionMarking)
              {
                // used when extruding selected components
                const ON_SubDEdge* e = f->Edge(fvi);
                if (nullptr != e && false == e->m_status.RuntimeMark())
                  e->m_status.SetRuntimeMark();
              }
              const ON_SubDVertex* v = f->Vertex(fvi);
              if (nullptr != v && false == v->m_status.RuntimeMark())
              {
                v->m_status.SetRuntimeMark();
                if (bTransform)
                  const_cast<ON_SubDVertex*>(v)->Transform(false, *xform);
                ++marked_vertex_count;
              }
            }
          }
        }
        break;
      }
    }

    if (bExtrusionMarking && potential_isolated_edge_count > 0)
    {
      for (size_t i = 0; i < cptr_count; i++)
      {
        if (ON_SubDComponentPtr::Type::Edge != cptr_list[i].ComponentType())
          continue;

        const ON_SubDEdge* e = cptr_list[i].Edge();
        if (nullptr == e)
          continue;

        if (e->m_status.RuntimeMark())
          continue; // this edge us part of a boundary belonging to a face in  cptr_list[]

        if (1 == e->m_face_count && nullptr != e->m_face2[0].Face())
        {
          // this boundary edge was explicitly picked its attached face was not picked.
          // It will be extruded to a face.
          e->m_status.SetRuntimeMark();
          for (unsigned int evi = 0; evi < 2; ++evi)
          {
            const ON_SubDVertex* v = e->m_vertex[evi];
            if (nullptr != v && false == v->m_status.RuntimeMark())
            {
              v->m_status.SetRuntimeMark();
              ++marked_vertex_count;
            }
          }
          if (0 == --potential_isolated_edge_count)
            break;
        }
      }
    }

  }

  const_cast<ON_SubD&>(subd).ChangeContentSerialNumberForExperts();

  return marked_vertex_count;
}

static unsigned int Internal_MarkExtrudeComponents(
  const ON_SubD& subd,
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bExtrudeBoundaries,
  unsigned int& list_vertex_count,
  unsigned int& list_edge_count,
  unsigned int& list_face_count
)
{
  const bool bExtrusionMarking = true;
  const int marked_vertex_count = Internal_MarkStuffAndMaybeMoveVertices(
    subd,
    cptr_list,
    cptr_count,
    nullptr,
    ON_SubDComponentLocation::Unset,
    bExtrusionMarking,
    bExtrudeBoundaries,
    list_vertex_count,
    list_edge_count,
    list_face_count
  );

  // It appears the best "hurestic" is to require the user to pick edges and faces.
  // isolated vertices will be ignored and no attempts to guess if a user wants to
  // extrude the boundary of a face (all its edges) or the face itself.

  // lots of delete "hurestic" code here :)

  return marked_vertex_count;
}

static unsigned int Internal_TransformComponents(
  const ON_SubD& subd,
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  const ON_Xform& xform,
  ON_SubDComponentLocation component_location
)
{
  // This version is used by SDK tools that simply transform components in cptr_list
  if (false == xform.IsValidAndNotZeroAndNotIdentity())
    return 0;

  const bool bExtrusionMarking = false;
  unsigned int list_vertex_count = 0;
  unsigned int list_edge_count = 0;
  unsigned int list_face_count = 0;

  return Internal_MarkStuffAndMaybeMoveVertices(
    subd,
    cptr_list,
    cptr_count,
    &xform,
    component_location,
    bExtrusionMarking, false,
    list_vertex_count,
    list_edge_count,
    list_face_count
  );
}

unsigned int ON_SubD::TransformComponents(
  const ON_Xform& xform,
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  ON_SubDComponentLocation component_location
)
{
  if ( 
    false == xform.IsValidAndNotZeroAndNotIdentity()
    || cptr_count <= 0
    || nullptr == cptr_list
    )
    return 0;

  ON_SimpleArray<const ON_SubDComponentBase*> marked_components;
  const bool bRestoreMarks = ClearComponentMarks(true, true, true, &marked_components) > 0;

  const unsigned int v_count = Internal_TransformComponents(*this, cptr_list, cptr_count, xform, component_location);

  if (v_count > 0)
  {
    this->ClearEvaluationCache();
  }

  if (bRestoreMarks)
    SetComponentMarks(true, marked_components);

  return (v_count > 0);
}


unsigned int ON_SubD::ExtrudeComponents(
  const ON_Xform& xform,
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count
)
{
  const bool bExtrudeBoundaries = true;
  const bool bPermitNonManifoldEdgeCreation = false;
  const ON_SubD::EdgeTag original_edge_tag = ON_SubD::EdgeTag::Unset;
  const ON_SubD::EdgeTag moved_edge_tag = ON_SubD::EdgeTag::Unset;
  return ExtrudeComponents(xform, ci_list, ci_count, bExtrudeBoundaries, bPermitNonManifoldEdgeCreation, original_edge_tag, moved_edge_tag);
}

unsigned int ON_SubD::ExtrudeComponents(
  const ON_Xform& xform,
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  bool bExtrudeBoundaries,
  bool bPermitNonManifoldEdgeCreation,
  ON_SubD::EdgeTag original_edge_tag,
  ON_SubD::EdgeTag moved_edge_tag
)
{
  if ( 
    false == xform.IsValidAndNotZeroAndNotIdentity()
    || xform.IsIdentity()
    || ci_count <= 0
    || nullptr == ci_list
    )
    return 0;

  ON_SimpleArray<ON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return true; // nothing to extrude

  return ExtrudeComponents(
    xform,
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    bExtrudeBoundaries,
    bPermitNonManifoldEdgeCreation,
    original_edge_tag,
    moved_edge_tag
    );
}

class ON_Internal_ExtrudedVertexPair
{
public:
  ON_Internal_ExtrudedVertexPair() = default;
  ~ON_Internal_ExtrudedVertexPair() = default;
  ON_Internal_ExtrudedVertexPair(const ON_Internal_ExtrudedVertexPair&) = default;
  ON_Internal_ExtrudedVertexPair& operator=(const ON_Internal_ExtrudedVertexPair&) = default;

  static const ON_Internal_ExtrudedVertexPair Unset;

  // the marked vertex was in the original subd and will be moved.
  ON_SubDVertex* m_marked_vertex = nullptr;

  // the unmarked vertex replaces the marked vertex at the original location.
  ON_SubDVertex* m_unmarked_vertex = nullptr;

  // from new vertex to original vertex
  ON_SubDEdge* m_new_side = nullptr;

  static int CompareMarkedVertexId(
    const ON_Internal_ExtrudedVertexPair* lhs,
    const ON_Internal_ExtrudedVertexPair* rhs
  );
};

const ON_Internal_ExtrudedVertexPair ON_Internal_ExtrudedVertexPair::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Internal_ExtrudedVertexPair);


class ON_Internal_ExtrudedSide
{
public:
  ON_Internal_ExtrudedSide() = default;
  ~ON_Internal_ExtrudedSide() = default;
  ON_Internal_ExtrudedSide(const ON_Internal_ExtrudedSide&) = default;
  ON_Internal_ExtrudedSide& operator=(const ON_Internal_ExtrudedSide&) = default;

  static const ON_Internal_ExtrudedSide Unset;

  bool m_bExtrudedBoundaryEdge = false;
  bool m_bHasMovedFace = false; // true if the edge touches a fae that will be moved.
  bool m_bHasStationaryFace = false; // true if the edge touches a face that will remain stationary.
  ON_SubD::EdgeTag m_original_marked_edge_tag = ON_SubD::EdgeTag::Unset;

  ON_SubD::EdgeTag MarkedEdgeTag() const
  {
    // this tag is calculated just before the side face is made
    if ( m_bExtrudedBoundaryEdge )
    {
      const ON_SubDVertex* v0 = nullptr != m_marked_edge ? m_marked_edge->m_vertex[0] : nullptr;
      const ON_SubDVertex* v1 = nullptr != m_marked_edge ? m_marked_edge->m_vertex[1] : nullptr;
      if ( 
        m_bHasMovedFace 
        && false == m_bHasStationaryFace
        && nullptr != v0
        && nullptr != v1
        )
      {
        return
          (v0->IsDartOrCreaseOrCorner() && v1->IsDartOrCreaseOrCorner())
          ? ON_SubD::EdgeTag::SmoothX
          : ON_SubD::EdgeTag::Smooth;
      }
    }

    return m_original_marked_edge_tag;
  }

  ON_SubD::EdgeTag UnmarkedEdgeTag(
    const ON_SubDVertex* v0,
    const ON_SubDVertex* v1
  ) const
  {
    // This tag is calculate before m_unmarked_edge is created.
    if (
      m_bExtrudedBoundaryEdge 
      && m_bHasStationaryFace
      && false == m_bHasMovedFace
      && nullptr != v0
      && nullptr != v1
      )
    {
      return
        (v0->IsDartOrCreaseOrCorner() && v1->IsDartOrCreaseOrCorner())
        ? ON_SubD::EdgeTag::SmoothX
        : ON_SubD::EdgeTag::Smooth;
    }

    return 
      (ON_SubD::EdgeTag::Crease == m_original_marked_edge_tag) 
      ? ON_SubD::EdgeTag::Crease 
      : ON_SubD::EdgeTag::Unset
      ;
  }

  // the marked edge was in the original object and will be moved.
  ON_SubDEdge* m_marked_edge = nullptr;

  // the unmarked edge replaces the marked edge at the original location.
  ON_SubDEdge* m_unmarked_edge = nullptr;

  // start at new vertex and end at original vertex;
  ON_SubDEdge* m_new_side0 = nullptr;
  ON_SubDEdge* m_new_side1 = nullptr;

  ON_SubDFace* m_new_face = nullptr;
};

const ON_Internal_ExtrudedSide ON_Internal_ExtrudedSide::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Internal_ExtrudedSide);

int ON_Internal_ExtrudedVertexPair::CompareMarkedVertexId(
  const ON_Internal_ExtrudedVertexPair* lhs,
  const ON_Internal_ExtrudedVertexPair* rhs
)
{
  const unsigned int lhs_id = lhs->m_marked_vertex->m_id;
  const unsigned int rhs_id = rhs->m_marked_vertex->m_id;
  if (lhs_id < rhs_id)
    return -1;
  if (lhs_id > rhs_id)
    return 1;
  return 0;
}

static  ON_SubD::EdgeTag Internal_AdjustedEdgeTag(const ON_SubDEdge* edge)
{
  if (nullptr == edge || nullptr == edge->m_vertex[0] || nullptr == edge->m_vertex[1])
    return ON_SubD::EdgeTag::Unset;


  // adjust moved edge tag because vertex tags can change when they get moved.
  const ON_SubD::VertexTag evtag[2] = { edge->m_vertex[0]->m_vertex_tag ,edge->m_vertex[1]->m_vertex_tag };
  
  if (ON_SubD::VertexTag::Unset == evtag[0] || ON_SubD::VertexTag::Unset == evtag[1])
    return ON_SubD::EdgeTag::Unset;

  if (ON_SubD::VertexTag::Smooth == evtag[0] || ON_SubD::VertexTag::Smooth == evtag[1])
    return ON_SubD::EdgeTag::Smooth;
  
  const ON_SubD::EdgeTag etag = edge->m_edge_tag;
  if (ON_SubD::EdgeTag::Smooth == etag || ON_SubD::EdgeTag::SmoothX == etag )
    return ON_SubD::EdgeTag::SmoothX;

  return etag;
}

static void Internal_SetEdgeVertices(
  ON_SubD& subd,
  ON_Internal_ExtrudedVertexPair& vertex_pair
  )
{
  // marked edges use the marked vertex.
  ON_SubDVertex* marked_vertex = vertex_pair.m_marked_vertex;
  ON_SubDVertex* unmarked_vertex =  vertex_pair.m_unmarked_vertex;
  const unsigned int vertex_edge_count = marked_vertex->EdgeCount();
  unsigned int marked_edge_count = 0;
  unsigned int unmarked_edge_count = 0;
  unsigned int new_edge_count = 0;
  for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
  {
    ON_SubDEdgePtr eptr = marked_vertex->m_edges[vei];
    const ON_SubDEdge* e = eptr.Edge();
    if (nullptr == e)
      continue;
    if (vertex_pair.m_new_side == e)
      new_edge_count++;
    else if (e->m_status.RuntimeMark())
      marked_edge_count++;
    else
      unmarked_edge_count++;
  }

  if (unmarked_edge_count <= 0)
    return;

  unmarked_edge_count += unmarked_vertex->m_edge_count;

  if ( unmarked_vertex->m_edge_capacity < (unmarked_edge_count+new_edge_count) )
  {
    subd.GrowVertexEdgeArray(unmarked_vertex, unmarked_edge_count);    
  }

  marked_vertex->m_edge_count = 0;
  for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
  {
    ON_SubDEdgePtr eptr = marked_vertex->m_edges[vei];
    ON_SubDEdge* e = eptr.Edge();
    if (nullptr == e)
      continue;
    if (vertex_pair.m_new_side == e || e->m_status.RuntimeMark())
    {
      marked_vertex->m_edges[marked_vertex->m_edge_count] = eptr;
      marked_vertex->m_edge_count++;
    }
    else
    {
      if (e->m_vertex[0] == marked_vertex)
        e->m_vertex[0] = unmarked_vertex;
      else if (e->m_vertex[1] == marked_vertex)
        e->m_vertex[1] = unmarked_vertex;
      unmarked_vertex->m_edges[unmarked_vertex->m_edge_count] = eptr;
      unmarked_vertex->m_edge_count++;
    }
  }
}

static ON_SubDFace* Internal_AddNewFace(
  ON_SubD& subd,
  ON_Internal_ExtrudedSide& side
)
{
  // All components that will be moved have the runtime mark set.
  // All other components have a clear runtime mark.
  // The original_edge will be moved.
  // The new_edge will not be moved.
  // new edge and original edge go the same direction.
  // new_side_edges[2] run from new to original edges.
  
  // change edges of unmarked faces to use the new edge
  ON__UINT_PTR edir = 0;
  ON_SubDEdge* marked_edge = side.m_marked_edge; // will be moved
  ON_SubDEdge* unmarked_edge = side.m_unmarked_edge; // fixed
  unsigned int marked_edge_face_count0 = marked_edge->m_face_count;
  ON_SubDFacePtr* marked_edge_fptr1 = marked_edge->m_face2;
  const ON_SubDFacePtr* marked_edge_fptr0 = marked_edge_fptr1;
  unsigned int marked_edge_face_count1 = 0;
  subd.GrowEdgeFaceArray(unmarked_edge, marked_edge_face_count0);
  for (unsigned int efi = 0; efi < marked_edge_face_count0; efi++, marked_edge_fptr0++)
  {
    if (2 == efi)
      marked_edge_fptr0 = marked_edge->m_facex;

    if (2 == marked_edge_face_count1)
      marked_edge_fptr1 = marked_edge->m_facex;

    ON_SubDFace* f = marked_edge_fptr0->Face();
    if (nullptr == f)
    {
      ON_SUBD_ERROR("null face pointer");
      continue;
    }

    if (f->m_status.RuntimeMark())
    {
      edir = marked_edge_fptr0->FaceDirection();
      marked_edge_face_count1++;
      *marked_edge_fptr1 = *marked_edge_fptr0;
      marked_edge_fptr1++;
      continue; // this face will be moved and keeps edge e1
    }

    // f is unmarked.
    // change referenced edge from marked_edge to unmarked_edge.
    f->ReplaceEdgeInArray(0, marked_edge, unmarked_edge);

    unmarked_edge->AddFaceToArray(*marked_edge_fptr0);
  }

  // When marked_edge is a manifold edge, face_count goes from 2 to 1.
  marked_edge->m_face_count = static_cast<unsigned short>(marked_edge_face_count1);

  ON_SubDEdge* side0 = (0 == edir) ? side.m_new_side0 : side.m_new_side1;
  ON_SubDEdge* side1 = (0 == edir) ? side.m_new_side1 : side.m_new_side0;
  ON_SubDEdgePtr new_face_eptr[4];
  new_face_eptr[0] = ON_SubDEdgePtr::Create(side.m_marked_edge, 1-edir);
  new_face_eptr[1] = ON_SubDEdgePtr::Create(side0, 1);
  new_face_eptr[2] = ON_SubDEdgePtr::Create(side.m_unmarked_edge, edir);
  new_face_eptr[3] = ON_SubDEdgePtr::Create(side1, 0);
  
  const ON_SubD::EdgeTag marked_edge_tag = side.MarkedEdgeTag();
  if (marked_edge_tag != marked_edge->m_edge_tag)
    marked_edge->m_edge_tag = marked_edge_tag;

  side.m_new_face = subd.AddFace(new_face_eptr, 4);

  if (nullptr != side.m_new_face)
  {
    // When isolated edges are extruded, we need to flip the face.
    // In all other cases, we don't.
    bool bFlip = false;
    bool bFlipSet = false;
    for (unsigned int fei = 0; fei < 4; fei++)
    {
      const ON_SubDEdgePtr eptr = side.m_new_face->m_edge4[fei];
      const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr.m_ptr);
      if (nullptr == e || e->m_face_count > 2)
      {
        bFlipSet = false;
        break;
      }
      if (2 != e->m_face_count)
        continue;
      const ON__UINT_PTR fedir = ON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
      const ON_SubDFacePtr fptr[2] = { e->m_face2[0], e->m_face2[1] };
      const ON_SubDFace* f[2] = { ON_SUBD_FACE_POINTER(fptr[0].m_ptr), ON_SUBD_FACE_POINTER(fptr[1].m_ptr) };
      if (nullptr == f[0] || nullptr == f[1] || f[0] == f[1])
      {
        bFlipSet = false;
        break;
      }
      if (side.m_new_face != f[0] && side.m_new_face != f[1] )
      {
        bFlipSet = false;
        break;
      }
      const ON__UINT_PTR fdir[2] = { ON_SUBD_FACE_DIRECTION(fptr[0].m_ptr), ON_SUBD_FACE_DIRECTION(fptr[1].m_ptr) };
      if (fedir != fdir[(f[0] == side.m_new_face) ? 0 : 1])
      {
        bFlipSet = false;
        break;
      }
      const bool bSameDir = (fdir[0] == fdir[1]) ? true : false;
      if (false == bFlipSet)
      {
        bFlipSet = true;
        bFlip = bSameDir;
      }
      else if (bSameDir != bFlip)
      {
        bFlipSet = false;
        break;
      }
    }
    if (bFlip)
      side.m_new_face->ReverseEdgeList();
  }

  return side.m_new_face;
}

static ON_SubD::EdgeTag Internal_ConnectingEdgeTagAtVertex(
  bool bExtrudeBoundaries,
  const ON_SubDVertex* v,
  ON_SubD::VertexTag& moved_vertex_tag,
  ON_SubD::VertexTag& stationary_vertex_tag
)
{
  moved_vertex_tag = ON_SubD::VertexTag::Unset;
  stationary_vertex_tag = ON_SubD::VertexTag::Unset;
  if (ON_SubD::VertexTag::Crease != v->m_vertex_tag && ON_SubD::VertexTag::Dart != v->m_vertex_tag)
  {
    ON_SUBD_ERROR("This function requires a crease or dart vertex as input.");
    return ON_SubD::EdgeTag::Unset;
  }

  const unsigned int vertex_edge_count = v->m_edge_count;

  // total_count = number of edges currently attached to v that are creases
  unsigned int total_count = 0;


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // stationary, moved_to_crease, moved_to_smooth, and split are mutually exclusive sitations.

  // stationary_count = number of edges currently attached to v that are creases and remain in current locations
  unsigned int stationary_count = 0;

  // moved_to_crease_count = number of edges currently attached to v that are creases and move and remain creases
  unsigned int moved_to_crease_count = 0; 

  // moved_to_crease_count = number of edges currently attached to v that are creases and move and become smooth
  unsigned int moved_to_smooth_count = 0; 

  // split_count = number of edges currently attached to v that are creases and are between a moved and stationary face
  unsigned int split_count = 0;

  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // bdry, interior, wire, nonman are mutually exclusive sitations.

  // bdry_count = number of edges currently attached to v that are creases and have 1 face.
  unsigned int bdry_count = 0;

  // interior_count = number of edges currently attached to v that are creases and have 2 faces
  unsigned int interior_count = 0;

  // wire_count = number of edges currently attached to v that are creases and have 0 faces
  unsigned int wire_count = 0;

  // nonman_count = number of edges currently attached to v that are creases and have 3 or more faces
  unsigned int nonman_count = 0;


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // tally up what is happening at each crease edge attached to vertex v
  //
  for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
    if (nullptr == e)
      continue;

    if (ON_SubD::EdgeTag::Crease != e->m_edge_tag)
      continue;

    const bool bBoundaryEdge = (1 == e->m_face_count) && (nullptr != e->m_face2[0].Face());
    const bool bMovedBoundaryEdge = bBoundaryEdge && e->m_status.RuntimeMark();
    //const bool bStationaryBoundaryEdge = bBoundaryEdge && false == e->m_status.RuntimeMark();

    ++total_count;

    if (0 == e->m_face_count)
      ++wire_count;
    else if (1 == e->m_face_count)
      ++bdry_count;
    else if (2 == e->m_face_count)
      ++interior_count;
    else if (2 == e->m_face_count)
      ++nonman_count;

    unsigned int moved_face_count = 0;
    unsigned int stationary_face_count = 0;
    for (unsigned short evi = 0; evi < e->m_face_count; evi++)
    {
      const ON_SubDFace* f = e->Face(evi);
      if (nullptr == f)
      {
        ON_SUBD_ERROR("Edge has null face.");
        return ON_SubD::EdgeTag::Unset;
      }
      if (f->m_status.RuntimeMark())
        ++moved_face_count;
      else
        ++stationary_face_count;
    }

    if (moved_face_count > 0 && stationary_face_count > 0)
    {
      // This is edge is between a moved face and a stationary face.
      // It will "split" into two edges
      ++split_count; 
    }
    else if (moved_face_count > 0)
    {
      // This edge and all faced currently attached to it are moving.
      if (bExtrudeBoundaries && bMovedBoundaryEdge)
        ++moved_to_smooth_count;
      else
        ++moved_to_crease_count;
    }
    else if (stationary_face_count > 0)
    {
      // This edge is moving and all faced currently attached to it are stationary.
      // A new face will be attached to this edge
      if (bExtrudeBoundaries && bMovedBoundaryEdge)
      {
        // This boundary edge will move and a new face will be createed between it
        // and the face the edge is currently attached to.
        ++moved_to_crease_count; 
      }
      else
        ++stationary_count; // this crease (interior or boundary) and its attached face or faces do not move
    }
  }

  if (total_count != wire_count + split_count + moved_to_smooth_count + moved_to_crease_count + stationary_count)
  {
    ON_SUBD_ERROR("Bug in counting code above or invalid topology near this vertex.");
    return ON_SubD::EdgeTag::Unset;
  }

  if (ON_SubD::VertexTag::Dart == v->m_vertex_tag)
  {
    if (
      1 == total_count
      && 1 == interior_count
      && 0 == bdry_count 
      && 0 == wire_count 
      && 0 == nonman_count 
      )
    {
      if (0 == split_count)
      {
        if (1 == moved_to_crease_count && 0 == moved_to_smooth_count && 0 == stationary_count)
        {
          // dart crease and both attached faces are moving
          moved_vertex_tag = ON_SubD::VertexTag::Dart;
          stationary_vertex_tag = ON_SubD::VertexTag::Smooth;
          return ON_SubD::EdgeTag::Smooth;
        }
        else if (0 == moved_to_crease_count && 0 == moved_to_smooth_count && 1 == stationary_count)
        {
          // dart crease and both attached faces are stationary
          moved_vertex_tag = ON_SubD::VertexTag::Smooth;
          stationary_vertex_tag = ON_SubD::VertexTag::Dart;
          return ON_SubD::EdgeTag::Smooth;
        }
      }
      else if (1 == split_count)
      {
        // Along the dart crease, one attached face is moving and the other is stationary.
        // The dart crease will get split into two darts and the new edge will be smooth.
        moved_vertex_tag = ON_SubD::VertexTag::Dart;
        stationary_vertex_tag = ON_SubD::VertexTag::Dart;
        return ON_SubD::EdgeTag::SmoothX; // will X becomes Smooth on 1st subdivision
      }
    }
    ON_SUBD_ERROR("Unexpected dart vertex edge tags - bug in counting code above or current tags or topology are not invalid.");
    return ON_SubD::EdgeTag::Unset;
  }


  /////////////////////////////////////////////////////////////////////////////////////////////
  //
  // The rest of this function is for the case when ON_SubD::VertexTag::Crease == v->m_vertex_tag
  //
  if (1 == wire_count)
  {
    moved_vertex_tag = ON_SubD::VertexTag::Crease;
    stationary_vertex_tag = ON_SubD::VertexTag::Crease;
    return ON_SubD::EdgeTag::Crease;
  }
  if (2 == wire_count)
  {
    moved_vertex_tag = ON_SubD::VertexTag::Crease;
    stationary_vertex_tag = ON_SubD::VertexTag::Crease;
    return ON_SubD::EdgeTag::SmoothX; // will X becomes Smooth on 1st subdivision
  }

  if (0 != wire_count || 0 != nonman_count)
  {
    // If extrusions involving nonmanifold regions need to be supported, lots of changes are required.
    ON_SUBD_ERROR("Currently, non-manifold cases are not supported.");
    return ON_SubD::EdgeTag::Unset;
  }

  for(;;)
  {
    if ( 
      ON_SubD::VertexTag::Crease == v->m_vertex_tag
      && 2 == (split_count + moved_to_crease_count + moved_to_smooth_count + stationary_count) 
      )
    {
      if (2 == interior_count && 0 == bdry_count)
        break;
      if (0 == interior_count && 2 == bdry_count)
        break;
    }
    ON_SUBD_ERROR("Unexpected crease vertex edge tags - bug in counting code above or current tags or topology are not invalid.");
    return ON_SubD::EdgeTag::Unset;
  }

  if (0 == interior_count && 2 == bdry_count)
  {
    // The vertex v is currently a boundary crease vertex and split_count always 0 in this case.
    //
    // Since 2 = split_count + stationary_count + moved_to_crease_count + moved_to_smooth_count,
    // stationary_count = 0,1,2 is used as the next "case reduction" filter.

    if (0 != split_count || 2 != (stationary_count + moved_to_crease_count + moved_to_smooth_count) )
    {
      ON_SUBD_ERROR("Bug in boundary crease case counting code above.");
      return ON_SubD::EdgeTag::Unset;
    }


    if (0 == stationary_count)
    {
      // both attached crease edges are moving
      // 3 possibilities (moved_to_crease_count,moved_to_smooth_count) = (2,0), (0,2), or (1,1)
      if (2 == moved_to_crease_count )
      {
        moved_vertex_tag = ON_SubD::VertexTag::Crease;
        stationary_vertex_tag = ON_SubD::VertexTag::Smooth;
        return ON_SubD::EdgeTag::Smooth;
      }

      if (2 == moved_to_smooth_count )
      {
        // attached faces are moving and moved edges will be copied.
        // The moved edges will convert from crease to smooth and the stationary copies will become creases.
        // New faces will be created between the moved edge and the stationary copy.
        moved_vertex_tag = ON_SubD::VertexTag::Smooth;
        stationary_vertex_tag = ON_SubD::VertexTag::Crease;
        return ON_SubD::EdgeTag::Smooth;
      }

      if ( 1 == moved_to_crease_count && 1 == moved_to_smooth_count )
      {
        // new edge runs from moved crease to crease that replaces moved to smooth edge
        moved_vertex_tag = ON_SubD::VertexTag::Crease;
        stationary_vertex_tag = ON_SubD::VertexTag::Crease;
        return ON_SubD::EdgeTag::Crease;
      }
    }
    else if (1 == stationary_count)
    {
      // 1 attached crease edge is moving and the other attached crease edge is stationary
      // 2 possibilities (moved_to_crease_count,moved_to_smooth_count) = (1,0) or (0,1)
      if (1 == moved_to_crease_count && 0 == moved_to_smooth_count)
      {
        // moved edge continues to be a boundary (and crease) edge.
        // The new edge connecting the stationary crease and the moved edge is a crease.
        moved_vertex_tag = ON_SubD::VertexTag::Crease;
        stationary_vertex_tag = ON_SubD::VertexTag::Crease;
        return ON_SubD::EdgeTag::Crease;
      }
      if (0 == moved_to_crease_count && 1 == moved_to_smooth_count)
      {
        // moved crease becomes interior smooth edge
        // The new edge connecting the stationary crease and the moved edge is a crease.
        moved_vertex_tag = ON_SubD::VertexTag::Smooth;
        stationary_vertex_tag = ON_SubD::VertexTag::Crease;
        return ON_SubD::EdgeTag::Smooth;
      }
    }
    else if (2 == stationary_count)
    {
      // neither attached crease edge is moving
      // 1 possibility (moved_to_crease_count,moved_to_smooth_count) = (0,0)
      moved_vertex_tag = ON_SubD::VertexTag::Smooth;
      stationary_vertex_tag = ON_SubD::VertexTag::Crease;
      return ON_SubD::EdgeTag::Smooth;
    }
  }
  else if (2 == interior_count && 0 == bdry_count)
  {
    // The vertex v is currently an interior crease vertex and moved_to_smooth_count always 0 in this case.
    // (move_to_smooth_count>0 requires a boundary edge)
    //
    // Since 2 = split_count + stationary_count + moved_to_crease_count + moved_to_smooth_count,
    // split_count = 0,1,2 is used as the next "case reduction" filter.

    if (0 != moved_to_smooth_count || 2 != (split_count + stationary_count + moved_to_crease_count) )
    {
      ON_SUBD_ERROR("Bug in interior crease case counting code above.");
      return ON_SubD::EdgeTag::Unset;
    }
    
    if (0 == split_count)
    {
      // stationary_count can be 0, 1 or 2.
      if (0 == stationary_count)
      {
        // both creases and all attached faces are moving
        moved_vertex_tag = ON_SubD::VertexTag::Crease;
        stationary_vertex_tag = ON_SubD::VertexTag::Smooth;
        return ON_SubD::EdgeTag::Smooth;
      }
      else if (1 == stationary_count)
      {
        // TODO
      }
      else if (2 == stationary_count)
      {
        // both creases and all attached faces are stationary
        moved_vertex_tag = ON_SubD::VertexTag::Smooth;
        stationary_vertex_tag = ON_SubD::VertexTag::Crease;
        return ON_SubD::EdgeTag::Smooth;
      }
    }
    else if (1 == split_count)
    {
      // stationary_count can be 0 or 1
      if (0 == stationary_count)
      {
        // 1 = moved_to_crease_count, 0 = moved_to_smooth_count.
        moved_vertex_tag = ON_SubD::VertexTag::Dart;
        stationary_vertex_tag = ON_SubD::VertexTag::Crease;
        return ON_SubD::EdgeTag::SmoothX; // will X becomes Smooth on 1st subdivision
      }
      else if (1 == stationary_count)
      {
        // moved_to_crease_count = 0 and moved_to_smooth_count = 0
        moved_vertex_tag = ON_SubD::VertexTag::Dart;
        stationary_vertex_tag = ON_SubD::VertexTag::Crease;
        return ON_SubD::EdgeTag::SmoothX; // will X becomes Smooth on 1st subdivision
      }
    }
    else if (2 == split_count)
    {
      // stationary_count = 0, moved_to_crease_count = 0, and moved_to_smooth_count = 0
      moved_vertex_tag = ON_SubD::VertexTag::Crease;
      stationary_vertex_tag = ON_SubD::VertexTag::Crease;
      return ON_SubD::EdgeTag::SmoothX; // will X becomes Smooth on 1st subdivision
    }
  }

  ON_SUBD_ERROR("Unexpected crease vertex edge tags - bug in counting code above or current tags or topology are not invalid.");
  return ON_SubD::EdgeTag::Unset;
}

static bool Internal_NonManifoldEdgeWillBeCreated( const ON_SubDVertex* v )
{
  if (nullptr == v || false == v->m_status.RuntimeMark())
    return false;
  v->m_status.ClearRuntimeMark();

  const unsigned int vertex_edge_count = v->m_edge_count;
  unsigned int boundary_count = 0;
  for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
  {
    const ON_SubDEdge* e = v->Edge(vei);
    if (nullptr == e || 0 == e->m_face_count)
      continue;
    if (e->m_face_count > 2)
      return true;
    const ON_SubDFace* f = e->Face(0);
    const bool b0 = (nullptr != f) ? f->m_status.RuntimeMark() : false;
    f = (e->m_face_count > 1) ? e->Face(1) : nullptr;
    const bool b1 = (nullptr != f) ? f->m_status.RuntimeMark() : false;
    if (b0 == b1)
      continue;
    boundary_count++;
    if (boundary_count > 2)
      return true;
  }
  return false;
}

unsigned int ON_SubD::ExtrudeComponents(
  const ON_Xform& xform,
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count
)
{
  const bool bExtrudeBoundaries = true;
  const bool bPermitNonManifoldEdgeCreation = false;
  const ON_SubD::EdgeTag original_edge_tag = ON_SubD::EdgeTag::Unset;
  const ON_SubD::EdgeTag moved_edge_tag = ON_SubD::EdgeTag::Unset;
  return ExtrudeComponents(xform, cptr_list, cptr_count, bExtrudeBoundaries, bPermitNonManifoldEdgeCreation, original_edge_tag, moved_edge_tag);
}

unsigned int ON_SubD::ExtrudeComponents(
  const ON_Xform& xform,
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bExtrudeBoundaries,
  bool bPermitNonManifoldEdgeCreation,
  ON_SubD::EdgeTag original_edge_tag,
  ON_SubD::EdgeTag moved_edge_tag
)
{
  if (nullptr == cptr_list || cptr_count <= 0)
    return 0;
  return Internal_ExtrudeComponents(
    xform,
    cptr_list,
    cptr_count,
    bExtrudeBoundaries,
    bPermitNonManifoldEdgeCreation,
    original_edge_tag,
    moved_edge_tag
    );
}

unsigned int ON_SubD::Extrude(const ON_Xform & xform)
{
  if (IsSolid())
    return 0;
  const bool bExtrudeBoundaries = true;
  const bool bPermitNonManifoldEdgeCreation = false;
  return Internal_ExtrudeComponents(
    xform,
    nullptr,
    0,
    bExtrudeBoundaries,
    bPermitNonManifoldEdgeCreation,
    ON_SubD::EdgeTag::Unset, ON_SubD::EdgeTag::Unset
    );
}

unsigned int ON_SubD::Internal_ExtrudeComponents(
  const ON_Xform& xform,
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bExtrudeBoundaries,
  bool bPermitNonManifoldEdgeCreation,
  ON_SubD::EdgeTag original_edge_tag,
  ON_SubD::EdgeTag moved_edge_tag
)
{
  const bool bHaveCptrList = (cptr_count > 0 && nullptr != cptr_list);
  bool bExtrudeAll = false;
  if (false == bHaveCptrList && 0 == cptr_count)
  {
    bool bIsManifold = false;
    bool bIsOriented = false;
    bool bHasBoundary = false;
    int solid_orientation = 0;
    ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
    bExtrudeAll = bHasBoundary;
  }
  if (
    false == xform.IsValidAndNotZeroAndNotIdentity()
    || (false == bHaveCptrList && false == bExtrudeAll)
    )
    return 0;

  // The extrusion is initially calculated as if bot original_edge_tag and moved_edge_tag
  // are Unset. A post process (will be added later to) sets the tags as specified
  if (ON_SubD::EdgeTag::Crease != original_edge_tag && ON_SubD::EdgeTag::Smooth != original_edge_tag)
    original_edge_tag = ON_SubD::EdgeTag::Unset;

  if (ON_SubD::EdgeTag::Crease != moved_edge_tag && ON_SubD::EdgeTag::Smooth != moved_edge_tag)
    moved_edge_tag = ON_SubD::EdgeTag::Unset;
  
  ON_SubDComponentMarksClearAndRestore mark_guard(*this);

  // Marks very vertex touching a component in the cptr_list.
  // Skips applying the transform because it is the identity.
  
  unsigned int list_vertex_count = 0;
  unsigned int list_edge_count = 0;
  unsigned int list_face_count = 0;
  const unsigned int marked_vertex_count = Internal_MarkExtrudeComponents(
    *this,
    cptr_list, cptr_count,
    bExtrudeBoundaries,
    list_vertex_count, list_edge_count, list_face_count
  );
  
  unsigned int moved_face_count = 0; // moved face count
  unsigned int new_face_count = 0; // number of new faces on moved boundary
  for (;;)
  {
    if (0 == marked_vertex_count)
      break;

    // Set moved_faces[] = list of faces that will move.
    ON_SimpleArray<const ON_SubDFace*> moved_faces(list_face_count + 128);
    ON_SubDFaceIterator fit(*this);
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      const unsigned int face_vertex_count = f->m_edge_count;
      if (face_vertex_count < 3 || false == f->m_status.RuntimeMark())
        continue;
      moved_faces.Append(f);
    }

    moved_face_count = moved_faces.UnsignedCount();

    // Mark edges on the boundary of the moved subset.

    // Set moved_edges[] = list of edges that are either
    // 1) between a moved face and a stationary face
    // 2) are a boundary edge to be extruded.
    ON_SimpleArray<const ON_SubDEdge*> moved_edges(list_edge_count + list_vertex_count);

    ON_SimpleArray<ON_Internal_ExtrudedSide> new_sides(64);
    ON_SubDEdgeIterator eit(*this);
    for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      const bool bExtrudedBoundaryEdge
        = bExtrudeBoundaries
        && e->m_status.RuntimeMark()
        && 1 == e->m_face_count
        && ON_SubD::EdgeTag::Crease == e->m_edge_tag
        ;
      bool bExtrudeEdge = bExtrudedBoundaryEdge;

      e->m_status.ClearRuntimeMark();

      bool bMarkedFace = false;
      bool bUnmarkedFace = false;
      const unsigned int edge_face_count = e->m_face_count;
      for (unsigned int efi = 0; efi < edge_face_count; efi++)
      {
        const ON_SubDFace* f = e->Face(efi);
        if (nullptr == f)
          continue;
        if (f->m_status.RuntimeMark())
          bMarkedFace = true; // f is in the subset of moved faces.
        else
          bUnmarkedFace = true; // f is in the subset of stationary faces.

        if (bMarkedFace && bUnmarkedFace)
        {
          // e is on the boundary between the subset of moved faces and stationary faces.
          bExtrudeEdge = true;
          break;
        }
      }
            
      if (bExtrudeEdge)
      {
        if (false == bMarkedFace)
          moved_edges.Append(e); // e is not part of a moved face's boundary
        e->m_status.SetRuntimeMark();
        ON_Internal_ExtrudedSide& newside = new_sides.AppendNew();
        newside.m_marked_edge = const_cast<ON_SubDEdge*>(e);
        newside.m_bExtrudedBoundaryEdge = bExtrudedBoundaryEdge;
        newside.m_bHasMovedFace = bMarkedFace;
        newside.m_bHasStationaryFace = bUnmarkedFace;
        newside.m_original_marked_edge_tag = e->m_edge_tag;
      }
    }

    new_face_count = new_sides.UnsignedCount();

    if (0 == new_face_count)
    {
      // no new faces will be made
      if (moved_face_count > 0)
      {
        // Every face is moving 
        Transform(xform);
      }
      break;
    }

    if (false == bPermitNonManifoldEdgeCreation)
    {
      bool bNonManifoldEdgeWillBeCreated = false;
      for (unsigned int fi = 0; fi < moved_face_count; fi++)
      {
        const ON_SubDFace* f = moved_faces[fi];
        const unsigned int face_vertex_count = f->m_edge_count;
        for (unsigned int fvi = 0; fvi < face_vertex_count; fvi++)
        {
          bNonManifoldEdgeWillBeCreated = Internal_NonManifoldEdgeWillBeCreated(f->Vertex(fvi));
          if (bNonManifoldEdgeWillBeCreated)
            break;
        }
        if (bNonManifoldEdgeWillBeCreated)
          break;
      }

      if (bNonManifoldEdgeWillBeCreated)
        break; // not allowd to create non-manifold edges

      for (unsigned int ei = 0; ei < moved_edges.UnsignedCount(); ei++)
      {
        const ON_SubDEdge* e = moved_edges[ei];
        for (unsigned int evi = 0; evi < 2; evi++)
        {
          bNonManifoldEdgeWillBeCreated = Internal_NonManifoldEdgeWillBeCreated(e->m_vertex[evi]);
          if (bNonManifoldEdgeWillBeCreated)
            break;
        }
        if (bNonManifoldEdgeWillBeCreated)
          break;
      }

      if (bNonManifoldEdgeWillBeCreated)
        break; // not allowd to create non-manifold edges
    }

    // clear vertex  marks.
    ClearComponentMarks(true, false, false, nullptr);    

    // Duplicate vertices that are on an edge between a marked and unmarked face
    // or a moved boundary edge
    ON_SimpleArray<ON_Internal_ExtrudedVertexPair> vertex_pairs(new_face_count+8);
    for (unsigned int i = 0; i < new_face_count; i++)
    {
      const ON_SubDEdge* e = new_sides[i].m_marked_edge;
      for (unsigned int evi = 0; evi < 2; evi++)
      {
        ON_SubDVertex* v = const_cast<ON_SubDVertex*>(e->m_vertex[evi]);
        if (nullptr == v)
          continue;
        if (v->m_status.RuntimeMark())
          continue;

        // Mark this vertex. It will eventually get moved
        v->m_status.SetRuntimeMark();
        ON_Internal_ExtrudedVertexPair& vpair = vertex_pairs.AppendNew();
        vpair.m_marked_vertex = v;

        ON_SubD::VertexTag moved_vertex_tag = ON_SubD::VertexTag::Unset; // the original vertex gets moved
        ON_SubD::VertexTag stationary_vertex_tag = ON_SubD::VertexTag::Unset; // (this one get allocated)
        ON_SubD::EdgeTag connecting_edge_tag = ON_SubD::EdgeTag::Unset; // from stationary to moved vertex
        switch (v->m_vertex_tag)
        {
        case ON_SubD::VertexTag::Dart:
          connecting_edge_tag = Internal_ConnectingEdgeTagAtVertex(bExtrudeBoundaries, v, moved_vertex_tag, stationary_vertex_tag);
          break;
        case ON_SubD::VertexTag::Crease:
          connecting_edge_tag = Internal_ConnectingEdgeTagAtVertex(bExtrudeBoundaries, v, moved_vertex_tag, stationary_vertex_tag);
          break;
        case ON_SubD::VertexTag::Corner:
          moved_vertex_tag = v->m_vertex_tag;
          stationary_vertex_tag = v->m_vertex_tag;
          connecting_edge_tag = ON_SubD::EdgeTag::Crease;
          break;
        case ON_SubD::VertexTag::Smooth:
          moved_vertex_tag = ON_SubD::VertexTag::Smooth;
          stationary_vertex_tag = ON_SubD::VertexTag::Smooth;
          connecting_edge_tag = ON_SubD::EdgeTag::Smooth;
          break;
        default:
          moved_vertex_tag = ON_SubD::VertexTag::Unset;
          stationary_vertex_tag = ON_SubD::VertexTag::Unset;
          connecting_edge_tag = ON_SubD::EdgeTag::Unset;
          break;
        }

        // original vertex will eventually be moved.
        v->m_vertex_tag = moved_vertex_tag;

        // new vertex will become part of the stationary subset.
        // It is not marked.
        vpair.m_unmarked_vertex = this->AddVertex(stationary_vertex_tag, v->m_P);

        // transform the marked boundary vertex
        v->Transform(false, xform);

        // edge from stationary subset to moved subset.
        ON_SubDEdge* connecting_edge = this->AddEdge(connecting_edge_tag, vpair.m_unmarked_vertex, vpair.m_marked_vertex);
        vpair.m_new_side = connecting_edge;
      }
    }

    // sort vertex pairs so they can be located by the original vertex id.
    vertex_pairs.QuickSort(ON_Internal_ExtrudedVertexPair::CompareMarkedVertexId);

    // remove unmarked faces from marked vertices
    for (unsigned int i = 0; i < vertex_pairs.UnsignedCount(); i++)
    {
      ON_SubDVertex* marked_vertex = vertex_pairs[i].m_marked_vertex;
      ON_SubDVertex* unmarked_vertex = vertex_pairs[i].m_unmarked_vertex;
      const unsigned int vertex_face_count0 = marked_vertex->m_face_count;
      GrowVertexFaceArray(unmarked_vertex, vertex_face_count0);
      marked_vertex->m_face_count = 0;
      for (unsigned int vfi = 0; vfi < vertex_face_count0; vfi++)
      {
        const ON_SubDFace* f = marked_vertex->m_faces[vfi];
        if (nullptr == f )
          continue;
        ON_SubDVertex* v
          = (f->m_status.RuntimeMark())
          ? marked_vertex
          : unmarked_vertex;
        v->m_faces[v->m_face_count] = f;
        v->m_face_count++;
      }
    }

    // build new side edges
    ON_Internal_ExtrudedVertexPair key[2];
    for (unsigned int i = 0; i < new_face_count; i++)
    {
      const ON_SubDEdge* e = new_sides[i].m_marked_edge;
      for (unsigned int evi = 0; evi < 2; evi++)
      {
        key[evi].m_marked_vertex = const_cast<ON_SubDVertex*>(e->m_vertex[evi]);
        const int i0 =
          (nullptr != key[evi].m_marked_vertex)
          ? vertex_pairs.BinarySearch(&key[evi], ON_Internal_ExtrudedVertexPair::CompareMarkedVertexId)
          : -1;
        if (i0 < 0)
        {
          key[evi] = ON_Internal_ExtrudedVertexPair::Unset;
          continue;
        }
        key[evi] = vertex_pairs[i0];
      } 

      const ON_SubD::EdgeTag unmarked_edge_tag = new_sides[i].UnmarkedEdgeTag(key[0].m_unmarked_vertex, key[1].m_unmarked_vertex);
      new_sides[i].m_unmarked_edge = this->AddEdge(unmarked_edge_tag, key[0].m_unmarked_vertex, key[1].m_unmarked_vertex);
      new_sides[i].m_new_side0 = key[0].m_new_side;
      new_sides[i].m_new_side1 = key[1].m_new_side;
    }

    // Mark everything a moved face touches
    // including interior edges and vertices.
    // Transform any vertices that are not already marked.
    for (unsigned int i = 0; i < moved_face_count; i++)
    {
      const ON_SubDFace* f = moved_faces[i];
      const unsigned int face_edge_count = f->m_edge_count;
      for (unsigned int fei = 0; fei < face_edge_count; ++fei)
      {
        const ON_SubDEdge* e = f->Edge(fei);
        if (nullptr == e)
          continue;
        e->m_status.SetRuntimeMark();
        for (unsigned int evi = 0; evi < 2; evi++)
        {
          ON_SubDVertex* v = const_cast<ON_SubDVertex*>(e->m_vertex[evi]);
          if (nullptr !=v && false == v->m_status.RuntimeMark())
          {
            v->Transform(false, xform);
            v->m_status.SetRuntimeMark();
          }
        }
      }
    }

    // Mark everything vertex a moved edge touches
    // Transform any vertices that are not already marked.
    for (unsigned int i = 0; i < moved_edges.UnsignedCount(); i++)
    {
      const ON_SubDEdge* e = moved_edges[i];
      const unsigned int edge_face_count = e->m_face_count;
      for (unsigned int efi = 0; efi < edge_face_count; ++efi)
      {
        e->m_status.SetRuntimeMark();
        for (unsigned int evi = 0; evi < 2; evi++)
        {
          ON_SubDVertex* v = const_cast<ON_SubDVertex*>(e->m_vertex[evi]);
          if (nullptr !=v && false == v->m_status.RuntimeMark())
          {
            v->Transform(false, xform);
            v->m_status.SetRuntimeMark();
          }
        }
      }
    }

    // For the original boundary vertrex, move unmarked edges to use the new vertex.
    for (unsigned int i = 0; i < vertex_pairs.UnsignedCount(); i++)
    {
      Internal_SetEdgeVertices(*this, vertex_pairs[i]);
    }

    // build new side faces
    for (unsigned int i = 0; i < new_face_count; i++)
    {
      Internal_AddNewFace(*this, new_sides[i]);
    }

    // Any edge touching the vertices in vertex_pairs[] may need its tag adjusted
    // because those vertices may have had their tags adjusted.
    // Some edges get checked twice, but adding code to check just once
    // takes more time than checking twice.
    for (unsigned int i = 0; i < vertex_pairs.UnsignedCount(); i++)
    {
      for (unsigned int j = 0; j < 2; j++)
      {
        const ON_SubDVertex* v = (0 == j) ? vertex_pairs[i].m_marked_vertex : vertex_pairs[i].m_unmarked_vertex;
        if (nullptr == v || nullptr == v->m_edges)
          continue;
        for (unsigned int vei = 0; vei < v->m_edge_count; vei++)
        {
          ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
          if (nullptr == e)
            continue;
          const ON_SubD::EdgeTag adjusted_etag = Internal_AdjustedEdgeTag(e);
          if (ON_SubD::EdgeTag::Unset != adjusted_etag && e->m_edge_tag != adjusted_etag)
            e->m_edge_tag = adjusted_etag;
        }
      }
    }

    // remove cached subdivision calculations
    ClearEvaluationCache();

    // Calculate vertex tags, edge tags, edge sector weights.
    this->UpdateAllTagsAndSectorCoefficients(true);
    break;
  }

#if defined(ON_DEBUG)
  IsValid();
#endif

  // TODO - run through new_sides[] array and attempt to adjust tags
  //if (ON_SubD::EdgeTag::Unset != original_edge_tag || ON_SubD::EdgeTag::Unset != moved_edge_tag)
  //{
  //  
  //}


  // number of moved faces and new boundary faces
  return moved_face_count + new_face_count;
}

unsigned int ON_SubD::SetVertexTags(
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  ON_SubD::VertexTag vertex_tag
)
{
  if (
    ON_SubD::VertexTag::Smooth != vertex_tag
    && ON_SubD::VertexTag::Crease != vertex_tag
    && ON_SubD::VertexTag::Corner != vertex_tag
    )
    return 0;

  if (
    ci_count <= 0
    || nullptr == ci_list
    || VertexCount() <= 0
    )
    return 0;

  ON_SimpleArray<ON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return 0; // nothing to change

  return SetVertexTags(
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    vertex_tag
    );
}

unsigned int ON_SubD::SetVertexTags(
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  ON_SubD::VertexTag vertex_tag
)
{
  if (
    ON_SubD::VertexTag::Smooth != vertex_tag
    && ON_SubD::VertexTag::Crease != vertex_tag
    && ON_SubD::VertexTag::Corner != vertex_tag
    )
    return 0;

  if (cptr_count <= 0 || nullptr == cptr_list)
    return 0;

  ON_SubDComponentMarksClearAndRestore mark_guard(*this);

  const bool bNewVertexTagIsSmooth = (ON_SubD::VertexTag::Smooth == vertex_tag);

  // count and mark vertex candidates
  // mark edges that may need to have their tag changed
  unsigned int candidate_count = 0;
  for (size_t i = 0; i < cptr_count; i++)
  {
    ON_SubDVertex* vertex = cptr_list[i].Vertex();
    if (nullptr == vertex)
      continue;
    if (vertex->m_vertex_tag == vertex_tag)
      continue;

    if (ON_SubD::VertexTag::Corner != vertex_tag)
    {
      // new vertex_tag is Smooth or Crease
      if (nullptr == vertex->m_edges || vertex->m_edge_count < 2)
        continue;

      const ON_SubDVertexEdgeProperties ep = vertex->EdgeProperties();

      if (ep.m_nonmanifold_edge_count > 0)
        continue; // nonmanifold edge

      if (ep.m_boundary_edge_count + ep.m_wire_edge_count > 2)
        continue;

      if (ON_SubD::VertexTag::Smooth == vertex_tag)
      {
        if (ep.m_interior_edge_count != vertex->m_edge_count)
          continue;
      }
      else if (ON_SubD::VertexTag::Crease == vertex_tag)
      {
        if (2 == ep.m_crease_edge_count)
        {
          // attempt change - further refinement may be needed here
        }
        else if (2 == ep.m_boundary_edge_count)
        {
          // attempt change - further refinement may be needed here
        }
        else if (2 == ep.m_wire_edge_count)
        {
          // attempt change - further refinement may be needed here
        }
        else
        {
          // dont' attempt change - further refinement may be needed here
          continue;
        }
      }
    }

    candidate_count++;
    vertex->m_status.SetRuntimeMark();
    if (nullptr != vertex->m_edges)
    {
      if (ON_SubD::VertexTag::Corner == vertex_tag)
      {
        const unsigned int crease_count = vertex->EdgeCount(ON_SubD::EdgeTag::Crease);
        if (2 == crease_count)
          continue; // do not crease additional edges
      }

      for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
      {
        const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
        if (nullptr == edge)
          continue;
        if (bNewVertexTagIsSmooth)
        {
          // new vertex_tag is Smooth
          if (edge->IsSmoothNotX())
            continue;
        }
        else
        {
          // new vertex_tag is Crease or Corner
          if (edge->IsCrease())
            continue;
        }

        // This edge tag will need to be changed
        edge->m_status.SetRuntimeMark();
      }
    }
  }

  if (0 == candidate_count)
    return 0;

  bool bUpdateTags = (ON_SubD::VertexTag::Crease != vertex_tag);

  // This for loop is used when new vertex_tag is ON_SubD::VertexTag::Crease.
  for (int pass = 0; pass < 2 && false == bUpdateTags; pass++)
  {
    // More careful analysis is neeeded to accurately mark smooth edges that will become creases
    ON_SubDEdgeIterator eit(*this);
    for (const ON_SubDEdge* edge = eit.FirstEdge(); nullptr != edge; edge = eit.NextEdge())
    {
      if (false == edge->m_status.RuntimeMark())
        continue;
      
      edge->m_status.ClearRuntimeMark();
      if (false == edge->IsSmooth())
        continue;

      const ON_SubDVertex* v[2] = { edge->m_vertex[0], edge->m_vertex[1] };
      if (nullptr == v[0] || nullptr == v[1])
        continue;

      const ON_SubD::VertexTag vtag[2] = {
        (v[0]->m_status.RuntimeMark() ? vertex_tag : v[0]->m_vertex_tag),
        (v[1]->m_status.RuntimeMark() ? vertex_tag : v[1]->m_vertex_tag)
      };

      // At least one of v[0] and v[1] had m_vertex_tag changed.

      ON_SubD::EdgeTag edge_tag;
      for (;;)
      {
        if (2 != edge->m_face_count)
        {
          edge_tag = ON_SubD::EdgeTag::Crease;
          break;
        }

        if (2 == v[0]->m_edge_count && (ON_SubD::VertexTag::Crease == vtag[0] || ON_SubD::VertexTag::Corner == vtag[0]))
        {
          edge_tag = ON_SubD::EdgeTag::Crease;
          break;
        }

        if (2 == v[1]->m_edge_count && (ON_SubD::VertexTag::Crease == vtag[1] || ON_SubD::VertexTag::Corner == vtag[1]))
        {
          edge_tag = ON_SubD::EdgeTag::Crease;
          break;
        }

        if (
          (ON_SubD::VertexTag::Crease == vtag[0] || ON_SubD::VertexTag::Corner == vtag[0] || ON_SubD::VertexTag::Dart == vtag[0])
          &&
          (ON_SubD::VertexTag::Crease == vtag[1] || ON_SubD::VertexTag::Corner == vtag[1] || ON_SubD::VertexTag::Dart == vtag[1])
          )
        {
          edge_tag = ON_SubD::EdgeTag::Crease;
          break;
        }

        edge_tag = ON_SubD::EdgeTag::Smooth;
        break;
      }

      if (ON_SubD::EdgeTag::Crease == edge_tag)
        edge->m_status.SetRuntimeMark();
    }

    // make sure new crease vertices will have the right number of creased edges
    bUpdateTags = true;
    for (size_t i = 0; i < cptr_count; i++)
    {
      ON_SubDVertex* vertex = cptr_list[i].Vertex();
      if (nullptr == vertex)
        continue;
      if (false == vertex->m_status.RuntimeMark())
        continue;
      unsigned int crease_count = 0;
      unsigned int marked_count = 0;
      for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
      {
        const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
        if (nullptr == edge)
          continue;
        if (edge->IsCrease())
          ++crease_count;
        else if (edge->m_status.RuntimeMark())
          ++marked_count;
      }
      if (crease_count + marked_count <= 2 && (0 != crease_count || 0 != marked_count))
        continue;

      if (pass > 0)
        return 0;
      bUpdateTags = false;
      if (2 == crease_count)
      {
        for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
        {
          const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
          if (nullptr != edge)
            edge->m_status.ClearRuntimeMark();
        }
      }
      else
      {
        vertex->m_status.ClearRuntimeMark();
        candidate_count--;
      }
    }

    if (0 == candidate_count)
      return 0;
  }

  if (false == bUpdateTags)
    return 0;

  unsigned int changed_vertex_count = 0;
  for (size_t i = 0; i < cptr_count; i++)
  {
    ON_SubDVertex* vertex = cptr_list[i].Vertex();
    if (nullptr == vertex)
      continue;
    if (false == vertex->m_status.RuntimeMark())
      continue;
    changed_vertex_count++;
    vertex->m_vertex_tag = vertex_tag;
    vertex->VertexModifiedNofification();
    for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
      if (nullptr == edge)
        continue;
      if ( false == bNewVertexTagIsSmooth
        && edge->m_status.RuntimeMark() 
        && false == edge->IsCrease()
        )
      {
        const_cast<ON_SubDEdge*>(edge)->m_edge_tag = ON_SubD::EdgeTag::Crease;
        edge->EdgeModifiedNofification();
      }
      edge->m_status.SetRuntimeMark();

      const ON_SubDVertex* other_vertex = edge->OtherEndVertex(vertex);
      other_vertex->m_status.SetRuntimeMark();

      if ( false == bNewVertexTagIsSmooth
        && ON_SubD::EdgeTag::Crease == edge->m_edge_tag
        && other_vertex->IsSmooth()
        )
      {
        const_cast<ON_SubDVertex*>(other_vertex)->m_vertex_tag = ON_SubD::VertexTag::Dart;
        other_vertex->VertexModifiedNofification();
      }
    }
  }

  if (0 == changed_vertex_count)
    return 0;

  ON_SubDEdgeIterator eit(*this);
  for (const ON_SubDEdge* edge = eit.FirstEdge(); nullptr != edge; edge = eit.NextEdge())
  {
    if (false == edge->m_status.RuntimeMark())
      continue;
    const ON_SubDVertex* v[2] = { edge->m_vertex[0], edge->m_vertex[1] };
    if (nullptr == v[0] || nullptr == v[1])
      continue;

    ON_SubD::EdgeTag edge_tag;
    if (v[0]->IsDartOrCreaseOrCorner() && v[1]->IsDartOrCreaseOrCorner())
      edge_tag = ON_SubD::EdgeTag::Crease;
    else
      edge_tag = ON_SubD::EdgeTag::Smooth;

    if (edge->m_edge_tag == edge_tag)
      continue;
    const_cast<ON_SubDEdge*>(edge)->m_edge_tag = edge_tag;
    edge->EdgeModifiedNofification();
  }

  ON_SubDVertexIterator vit(*this);
  for (const ON_SubDVertex* vertex = vit.FirstVertex(); nullptr != vertex; vertex = vit.NextVertex())
  {
    if (false == vertex->m_status.RuntimeMark())
      continue;
    const unsigned int crease_count = vertex->EdgeCount(ON_SubD::EdgeTag::Crease);
    ON_SubD::VertexTag vtag = vertex->m_vertex_tag;
    if (2 == crease_count)
    {
      if ( false == vertex->IsCreaseOrCorner() )
        vtag = ON_SubD::VertexTag::Crease;
    }
    else if (1 == crease_count)
      vtag = ON_SubD::VertexTag::Dart;
    else if (crease_count > 2)
      vtag = ON_SubD::VertexTag::Corner;
    else
      vtag = ON_SubD::VertexTag::Smooth;
    if (vertex->m_vertex_tag == vtag)
      continue;
    const_cast<ON_SubDVertex*>(vertex)->m_vertex_tag = vtag;
    vertex->ClearSavedSubdivisionPoints();
  }

  ClearEvaluationCache();
  UpdateAllTagsAndSectorCoefficients(false);

  return changed_vertex_count;
}

unsigned int ON_SubD::SetEdgeTags(
  const ON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  ON_SubD::EdgeTag edge_tag
)
{
  if (ON_SubD::EdgeTag::Smooth != edge_tag && ON_SubD::EdgeTag::Crease != edge_tag)
    return 0;

  if (
    ci_count <= 0
    || nullptr == ci_list
    || EdgeCount() <= 0
    )
    return 0;

  ON_SimpleArray<ON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return 0; // nothing to change

  return SetEdgeTags(
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    edge_tag
    );
}

unsigned int ON_SubD::SetEdgeTags(
  const ON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  ON_SubD::EdgeTag edge_tag
)
{
  if (ON_SubD::EdgeTag::Smooth != edge_tag && ON_SubD::EdgeTag::Crease != edge_tag)
    return 0;

  if (
    cptr_count <= 0
    || nullptr == cptr_list
    || EdgeCount() <= 0
    )
    return 0;

  unsigned int changed_edge_count = 0;

  const bool bChangeToSmooth = (ON_SubD::EdgeTag::Smooth == edge_tag) ? true : false;

  for (size_t i = 0; i < cptr_count; i++)
  {
    ON_SubDEdge* edge = cptr_list[i].Edge();
    if (nullptr == edge)
      continue;
    if (bChangeToSmooth == edge->IsSmooth())
      continue;
    if (bChangeToSmooth && 2 != edge->FaceCount())
      continue;

    edge->EdgeModifiedNofification();

    changed_edge_count++;
    edge->m_edge_tag = edge_tag;
    edge->UnsetSectorCoefficientsForExperts();
    for (int evi = 0; evi < 2; evi++)
    {
      ON_SubDVertex* v = const_cast<ON_SubDVertex*>(edge->m_vertex[evi]);
      if (nullptr == v)
        continue;
      v->m_vertex_tag = ON_SubD::VertexTag::Unset;
      v->ClearSavedSubdivisionPoints();
    }
  }

  if (0 == changed_edge_count)
    return 0;

  ClearEvaluationCache();

  ON_SubDVertexIterator vit(*this);
  for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
  {
    if (ON_SubD::VertexTag::Unset != v->m_vertex_tag)
      continue;
    unsigned crease_count = 0;
    const unsigned vertex_edge_count = v->EdgeCount();
    for (unsigned vei = 0; vei < vertex_edge_count; vei++)
    {
      const ON_SubDEdge* e = v->Edge(vei);
      if (nullptr == e)
        continue;
      if (e->IsCrease())
      {
        crease_count++;
        if (crease_count > 2)
          break;
      }
    }
    ON_SubD::VertexTag vertex_tag;
    switch (crease_count)
    {
    case 0:
      vertex_tag = ON_SubD::VertexTag::Smooth;
      break;
    case 1:
      vertex_tag = ON_SubD::VertexTag::Dart;
      break;
    case 2:
      vertex_tag = ON_SubD::VertexTag::Crease;
      break;
    default:
      vertex_tag = ON_SubD::VertexTag::Corner;
      break;
    }
    if (v->m_vertex_tag != vertex_tag)
    {
      const_cast<ON_SubDVertex*>(v)->m_vertex_tag = vertex_tag;
      v->ClearSavedSubdivisionPoints();
    }
  }

  ON_SubDEdgeIterator eit(*this);
  for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    ON_SubD::EdgeTag e_tag = e->m_edge_tag;
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      e_tag = ON_SubD::EdgeTag::Unset;
    else if (ON_SubD::EdgeTag::Smooth == e_tag
      && ON_SubD::VertexTag::Smooth != e->m_vertex[0]->m_vertex_tag
      && ON_SubD::VertexTag::Smooth != e->m_vertex[1]->m_vertex_tag)
      e_tag = ON_SubD::EdgeTag::Unset;
    if (e_tag != e->m_edge_tag)
    {
      const_cast<ON_SubDEdge*>(e)->m_edge_tag = e_tag;
      e->UnsetSectorCoefficientsForExperts();
      e->ClearSavedSubdivisionPoints();
    }
  }

  UpdateAllTagsAndSectorCoefficients(false);
  return changed_edge_count;
}

unsigned int ON_SubD::RemoveAllCreases()
{
  unsigned int changed_count = 0;
  ON_SubDEdgeIterator eit(*this);
  for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    if ( false == e->IsCrease() || 2 != e->m_face_count)
      continue;
    const_cast<ON_SubDEdge*>(e)->m_edge_tag = ON_SubD::EdgeTag::Smooth;
    e->UnsetSectorCoefficientsForExperts();
    for (int evi = 0; evi < 2; evi++)
    {
      if (nullptr == e->m_vertex[evi])
        continue;
      const_cast<ON_SubDVertex*>(e->m_vertex[evi])->m_vertex_tag = ON_SubD::VertexTag::Unset;
    }
    ++changed_count;
  }

  if (changed_count > 0)
  {
    this->DestroyRuntimeCache(true);
    this->UpdateAllTagsAndSectorCoefficients(true);
  }

  return changed_count;
}

const ON_SubDEdgePtr ON_SubDEdgeChain::EdgeChainNeighbor(ON_SubDEdgePtr starting_edge, ON_ChainDirection search_direction, ON_SubD::ChainType chain_type)
{
  return ON_SubDEdgeChain::EdgeChainNeighbor(
    starting_edge,
    search_direction,
    chain_type,
    false,
    ON_ComponentStatus::NoneSet,
    ON_ComponentStatus::NoneSet
  );
}

const ON_SubDEdgePtr ON_SubDEdgeChain::EdgeChainNeighbor(
    ON_SubDEdgePtr starting_edge,
    ON_ChainDirection search_direction,
    ON_SubD::ChainType chain_type,
    bool bEnableStatusCheck,
    ON_ComponentStatus status_pass,
    ON_ComponentStatus status_fail
  )
{
  for (;;)
  {
    if (ON_ChainDirection::Previous != search_direction && ON_ChainDirection::Next != search_direction)
      break;
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(starting_edge.m_ptr);
    if (nullptr == edge)
      break;
    bool bReverse = (ON_ChainDirection::Previous == search_direction);    
    if (0 != ON_SUBD_EDGE_DIRECTION(starting_edge.m_ptr))
      bReverse = !bReverse;
    const ON_SubDVertex* v = edge->m_vertex[bReverse ? 0 : 1];
    if (nullptr == v)
      break;
    if (v->m_edge_count <= 1 || nullptr == v->m_edges)
      break;
    
    const bool bIsSmooth = edge->IsSmooth();
    const bool bIsCrease = edge->IsCrease() || 2 != edge->m_face_count;
    if (bIsSmooth != (bIsCrease?false:true))
      break;

    const unsigned short vertex_edge_count 
      = ((ON_SubD::ChainType::EqualEdgeTagAndOrdinary == chain_type || ON_SubD::ChainType::EqualEdgeAndVertexTagAndOrdinary == chain_type)
        && (1 == edge->m_face_count || 2 == edge->m_face_count))
        ? (edge->m_face_count + 2)
        : ((unsigned short)0);

    if (vertex_edge_count > 0 && vertex_edge_count != v->m_edge_count)
      break;

    if (ON_SubD::ChainType::EqualEdgeAndVertexTag == chain_type || ON_SubD::ChainType::EqualEdgeAndVertexTagAndOrdinary == chain_type)
    {
      if (bIsSmooth)
      {
        // edge is smooth so vertex must be smooth
        if (ON_SubD::VertexTag::Smooth != v->m_vertex_tag)
          break;
      }
      else
      {
        // edge is crease so vertex must be crease
        if (ON_SubD::VertexTag::Crease != v->m_vertex_tag)
          break;
      }
    }

    // Look for a single neighbor with same crease/smooth property and same face count
    // This lets chains turn the right way when there are both creases and smooth
    // edges.
    const ON_SubDEdge* nxt = nullptr;
    for (unsigned short vei = 0; vei < v->m_edge_count; vei++)
    {
      const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
      if (edge == e)
        continue;
      if (bIsSmooth != e->IsSmooth())
        continue;
      if (bIsCrease != e->IsCrease())
        continue;
      if (e->m_face_count != edge->m_face_count)
        continue;
      if (e->m_vertex[0] != v && e->m_vertex[1] != v)
        continue; // bogus edge
      if (nullptr == nxt)
      {
        nxt = e;
        continue;
      }
      // ambiguous options here
      nxt = nullptr;
      break;
    }
    const int nxt_connecting_vertex_index = (ON_ChainDirection::Next == search_direction) ? 0 : 1;
    if (nullptr != nxt)
    {
      if (false == bEnableStatusCheck || ON_ComponentStatus::StatusCheck(nxt->m_status, status_pass, status_fail))
        return ON_SubDEdgePtr::Create(nxt, (v == nxt->m_vertex[nxt_connecting_vertex_index]) ? 0 : 1);
    }

    if (2 != edge->m_face_count || 4 != v->m_edge_count)
      break;
    const ON_SubDFace* f[2] = { ON_SUBD_FACE_POINTER(edge->m_face2[0].m_ptr),ON_SUBD_FACE_POINTER(edge->m_face2[1].m_ptr) };
    if (nullptr == f[0] || nullptr == f[1] || f[0] == f[1])
      break;
    for (unsigned short vei = 0; vei < v->m_edge_count; vei++)
    {
      const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
      if (edge == e)
        continue;
      if (2 != e->m_face_count)
        continue;
      if (e->m_vertex[0] != v && e->m_vertex[1] != v)
        continue; // bogus edge
      const ON_SubDFace* nxtf[2] = { ON_SUBD_FACE_POINTER(e->m_face2[0].m_ptr),ON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr) };
      if (nullptr == nxtf[0] || nullptr == nxtf[1] || nxtf[0] == nxtf[1])
        continue;
      if (f[0] == nxtf[0] || f[1] == nxtf[0])
        continue;
      if (f[0] == nxtf[1] || f[1] == nxtf[1])
        continue;
      if (nullptr == nxt)
      {
        nxt = e;
        continue;
      }
      // ambiguous options here
      nxt = nullptr;
      break;
    }
    if (nullptr != nxt)
    {
      if (bIsSmooth != nxt->IsSmooth())
      {
        // edge tag changed
        if ( 
          ON_SubD::ChainType::EqualEdgeTag == chain_type 
          || ON_SubD::ChainType::EqualEdgeAndVertexTag == chain_type
          || ON_SubD::ChainType::EqualEdgeTagAndOrdinary == chain_type
          || ON_SubD::ChainType::EqualEdgeAndVertexTagAndOrdinary == chain_type
          )
          break;
      }
      if (false == bEnableStatusCheck || ON_ComponentStatus::StatusCheck(nxt->m_status, status_pass, status_fail))
        return ON_SubDEdgePtr::Create(nxt, (v == nxt->m_vertex[nxt_connecting_vertex_index]) ? 0 : 1);
    }

    break;
  }

  return ON_SubDEdgePtr::Null;
}

void ON_UniqueTester::Block::DeleteBlock(Block* blk)
{
  if (nullptr != blk)
    onfree(blk);
}

ON_UniqueTester::Block* ON_UniqueTester::Block::NewBlock()
{
  size_t sz1 = sizeof(Block);
  while (0 != sz1 % 8)
    sz1++;
  size_t sz2 = ON_UniqueTester::Block::BlockCapacity * sizeof(m_a[0]);
  void* p = onmalloc(sz1 + sz2);
  Block* blk = new (p) Block();
  blk->m_a = (ON__UINT_PTR*)((char*)(p)) + sz1;
  return blk;
}

int ON_UniqueTester::Block::Compare(ON__UINT_PTR* lhs, ON__UINT_PTR* rhs)
{
  if (*lhs < *rhs)
    return -1;
  if (*lhs > *rhs)
    return 1;
  return 0;
}

bool ON_UniqueTester::Block::InBlock(size_t sorted_count,ON__UINT_PTR x) const
{
  if (nullptr != m_a && m_count > 0)
  {
    if ( sorted_count > 0 && x >= m_a[0] && x <= m_a[sorted_count - 1])
    {
      if (nullptr != bsearch(&x, m_a, sorted_count, sizeof(m_a[0]), (int(*)(const void*, const void*))ON_UniqueTester::Block::Compare))
        return true;
    }
    if (sorted_count < m_count)
    {
      const ON__UINT_PTR* p = m_a + sorted_count;
      const ON__UINT_PTR* p1 = m_a + m_count;
      while (p < p1)
      {
        if (x == *p++)
          return true;
      }
    }
  }
  return false;
}

void ON_UniqueTester::Block::SortBlock()
{
  ON_qsort(m_a, m_count, sizeof(m_a[0]), (int(*)(const void*, const void*))ON_UniqueTester::Block::Compare);
}

void ON_UniqueTester::Internal_CopyFrom(
  const ON_UniqueTester & src
)
{
  m_block_list = nullptr;
  m_sorted_count = 0;
 
  Block* first_blk = nullptr;

  for ( Block* src_blk = src.m_block_list; nullptr != src_blk; src_blk = src_blk->m_next)
  {
    Block* blk = Block::NewBlock();
    memcpy( blk->m_a, src_blk->m_a, src_blk->m_count * sizeof(blk->m_a[0]) );
    blk->m_count = src_blk->m_count;
    if (nullptr == first_blk)
    {
      first_blk = blk;
    }
    else
    {
      blk->m_next = m_block_list;
      m_block_list = blk;
    }
  }  

  if (nullptr != first_blk)
  {
    if (src.m_sorted_count != first_blk->m_count)
      first_blk->SortBlock();
    first_blk->m_next = m_block_list;
    m_block_list = first_blk;
    m_sorted_count = first_blk->m_count;
  }
}

void ON_UniqueTester::Internal_Destroy()
{
  Block* nxt = m_block_list;
  m_block_list = nullptr;
  m_sorted_count = 0;
  for ( Block* blk = nxt; nullptr != blk; blk = nxt)
  {
    nxt = blk->m_next;
    Block::DeleteBlock(blk);
  }  
}

ON_UniqueTester::~ON_UniqueTester()
{
  Internal_Destroy();
}

ON_UniqueTester::ON_UniqueTester(const ON_UniqueTester& src)
{
  Internal_CopyFrom(src);
}

ON_UniqueTester& ON_UniqueTester::operator=(const ON_UniqueTester& src)
{
  if (this != &src)
  {
    Internal_Destroy();
    Internal_CopyFrom(src);
  }
  return *this;
}

bool ON_UniqueTester::InList(ON__UINT_PTR x) const
{
  size_t sorted_count = m_sorted_count;
  for ( const Block* blk = m_block_list; nullptr != blk; blk = blk->m_next)
  {
    if (blk->InBlock(sorted_count, x))
      return true;
    sorted_count = ON_UniqueTester::Block::BlockCapacity;
  }  
  return false;
}

bool ON_UniqueTester::AddToList(ON__UINT_PTR x)
{
  if (nullptr != m_block_list && m_sorted_count + 50 == m_block_list->m_count)
  {
    m_block_list->SortBlock();
    m_sorted_count = m_block_list->m_count;
  }
  if (InList(x))
    return false;
  Internal_AddValue(x);
  return true;
}


void ON_UniqueTester::ExpertAddNewToList(ON__UINT_PTR x)
{
  Internal_AddValue(x);
}

void ON_UniqueTester::Internal_AddValue(ON__UINT_PTR x)
{
  if (nullptr == m_block_list || ON_UniqueTester::Block::BlockCapacity == m_block_list->m_count)
  {
    if (nullptr != m_block_list && m_sorted_count < ON_UniqueTester::Block::BlockCapacity)
      m_block_list->SortBlock();
    ON_UniqueTester::Block* blk = ON_UniqueTester::Block::NewBlock();
    blk->m_next = m_block_list;
    m_block_list = blk;
    m_sorted_count = 0;
  }
  m_block_list->m_a[m_block_list->m_count++] = x;
  if ( 1 == m_block_list->m_count
    || (m_sorted_count+1 == m_block_list->m_count && x > m_block_list->m_a[m_sorted_count-1])
    )
    ++m_sorted_count;
}

void ON_UniqueTester::ClearList()
{
  Internal_Destroy();
}

unsigned int ON_UniqueTester::Count() const
{
  size_t count = 0;
  for ( const Block* blk = m_block_list; nullptr != blk; blk = blk->m_next)
  {
    count += blk->m_count;
  }
  return (unsigned int)count;
}


const ON_SimpleArray<ON_SubDEdgePtr>& ON_SubDEdgeChain::EdgeChain() const
{
  return m_edge_chain;
}

const ON_SubD& ON_SubDEdgeChain::SubD() const
{
  return m_subd_ref.SubD();
}

const ON_SubDRef ON_SubDEdgeChain::SubDRef() const
{
  return m_subd_ref;
}

bool ON_SubDEdgeChain::InChain(
  const ON_SubDEdgePtr edge_ptr
) const
{
  return InChain(ON_SUBD_EDGE_POINTER(edge_ptr.m_ptr));
}

bool ON_SubDEdgeChain::InChain(
  const ON_SubDEdge* edge
) const
{
  return (nullptr == edge) ? false : m_unique_tester.InList((ON__UINT_PTR)edge);
}

bool ON_SubDEdgeChain::InChain(
  const ON_SubDVertex* vertex
) const
{
  return (nullptr == vertex) ? false : m_unique_tester.InList((ON__UINT_PTR)vertex);
}

bool ON_SubDEdgeChain::IsClosedLoop() const
{
  const unsigned int count = m_edge_chain.UnsignedCount();
  return (count >= 3 && m_edge_chain[0].RelativeVertex(0) == m_edge_chain[count - 1].RelativeVertex(1));
}

bool ON_SubDEdgeChain::IsConvexLoop(bool bStrictlyConvex) const
{
  if (false == IsClosedLoop())
    return false;

  const unsigned int count = m_edge_chain.UnsignedCount();
  ON_SimpleArray<ON_3dPoint> points(count);
  for (unsigned int i = 0; i < count; ++i)  
  {
    const ON_SubDVertex* v = m_edge_chain[i].RelativeVertex(0);
    if (nullptr == v)
      return false;
    points.Append(v->ControlNetPoint());
  }
  if ( false == (points[0] != points[count - 1]) )
    return false;

  return ON_IsConvexPolyline(points, bStrictlyConvex);
}


unsigned int ON_SubDEdgeChain::BeginEdgeChain(
  ON_SubDRef subd_ref,
  const ON_SubDEdge* initial_edge
)
{
  return ON_SubDEdgeChain::BeginEdgeChain(subd_ref, ON_SubDEdgePtr::Create(initial_edge, 0));
}

unsigned int ON_SubDEdgeChain::BeginEdgeChain(
  ON_SubDRef subd_ref,
  const ON_SimpleArray<const ON_SubDEdge* >& initial_edge_chain
)
{
  return BeginEdgeChain(subd_ref, initial_edge_chain.Array(), initial_edge_chain.UnsignedCount());
}

unsigned int ON_SubDEdgeChain::BeginEdgeChain(
  ON_SubDRef subd_ref,
  const ON_SubDEdge*const* initial_edge_chain,
  size_t edge_count
)
{
  ClearEdgeChain();

  if (
    edge_count <= 0 
    || subd_ref.SubD().IsEmpty() 
    || subd_ref.SubD().EdgeCount() < (unsigned int)edge_count
    )
    return 0;

  if ( 1 == edge_count)
    return ON_SubDEdgeChain::BeginEdgeChain(subd_ref, ON_SubDEdgePtr::Create(initial_edge_chain[0], 0));

  const ON_SubDEdge* e0 = initial_edge_chain[0];
  if (nullptr == e0 || nullptr == e0->m_vertex[0] || nullptr == e0->m_vertex[1] )
    return 0;
  const ON_SubDEdge* e1 = initial_edge_chain[1];
  if (nullptr == e1 || nullptr == e1->m_vertex[0] || nullptr == e1->m_vertex[1] )
    return 0;


  ON_SubDEdgePtr eptr = ON_SubDEdgePtr::Create(e0, (e0->m_vertex[1] == e1->m_vertex[0] || e0->m_vertex[1] == e1->m_vertex[1]) ? 0 : 1);

  ON_SimpleArray<ON_SubDEdgePtr> eptr_chain(edge_count);
  eptr_chain.Append(eptr);
  const ON_SubDVertex* v = eptr.RelativeVertex(1);
  for (unsigned int i = 1; i < edge_count; i++)
  {
    e1 = initial_edge_chain[i];
    if (nullptr == e1 || nullptr == e1->m_vertex[0] || nullptr == e1->m_vertex[1] )
      return 0;
    if (v != e1->m_vertex[0] && v != e1->m_vertex[1])
      return 0;
    eptr = ON_SubDEdgePtr::Create(e1, (v == e1->m_vertex[0]) ? 0 : 1);
    eptr_chain.Append(eptr);
  }

  return ON_SubDEdgeChain::BeginEdgeChain(subd_ref,eptr_chain);
}

unsigned int ON_SubDEdgeChain::BeginEdgeChain(
  ON_SubDRef subd_ref,
  ON_SubDEdgePtr eptr
)
{
  return ON_SubDEdgeChain::BeginEdgeChain(subd_ref, &eptr, 1);
}

unsigned int ON_SubDEdgeChain::BeginEdgeChain(
  ON_SubDRef subd_ref,
  const ON_SimpleArray<ON_SubDEdgePtr>& initial_edge_chain
)
{
  return ON_SubDEdgeChain::BeginEdgeChain(subd_ref, initial_edge_chain.Array(), initial_edge_chain.UnsignedCount() );
}

unsigned int ON_SubDEdgeChain::BeginEdgeChain(
  ON_SubDRef subd_ref,
  const ON_SubDEdgePtr* initial_edge_chain,
  size_t edge_count
  )
{
  ClearEdgeChain();

  m_subd_ref = subd_ref;

  if (edge_count <= 0 || m_subd_ref.SubD().IsEmpty())
    return 0;

  if ( ((size_t)subd_ref.SubD().EdgeCount()) < edge_count )
    return 0;

  m_edge_chain.Reserve(edge_count + 128);
  const ON_SubDVertex* first_vertex = nullptr;
  const ON_SubDVertex* last_vertex = nullptr;
  for (size_t i = 0; i < edge_count; i++)
  {
    const ON_SubDEdgePtr eptr = initial_edge_chain[i];
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(eptr.m_ptr);
    if (nullptr == e)
      continue;
    if (m_unique_tester.InList((ON__UINT_PTR)e))
      continue;
    const ON__UINT_PTR edir = ON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
    const ON_SubDVertex* v[2] = { e->m_vertex[edir], e->m_vertex[1 - edir] };
    if (nullptr == v[0] || nullptr == v[1] || v[0] == v[1] )
      continue;
    if (nullptr == first_vertex)
    {
      first_vertex = v[0];
      last_vertex = v[1];
      m_unique_tester.ExpertAddNewToList((ON__UINT_PTR)first_vertex);
      m_unique_tester.ExpertAddNewToList((ON__UINT_PTR)last_vertex);
    }
    else 
    {
      if (last_vertex != v[0])
        continue;
      if (v[1] != first_vertex)
      {
        if (false == m_unique_tester.AddToList((ON__UINT_PTR)v[1]))
          continue;
      }
    }
    m_edge_chain.Append(eptr);
    m_unique_tester.ExpertAddNewToList((ON__UINT_PTR)e);
    last_vertex = v[1];
    if (last_vertex == first_vertex)
      break;
  };

  return m_edge_chain.UnsignedCount();
}

void ON_SubDEdgeChain::ClearEdgeChain()
{
  m_edge_chain.SetCount(0);
  m_unique_tester.ClearList();
}

unsigned int ON_SubDEdgeChain::EdgeCount() const
{
  return m_edge_chain.UnsignedCount();
}

void ON_SubDEdgeChain::SetStatusCheck(
  bool bEnableStatusCheck,
  ON_ComponentStatus status_check_pass,
  ON_ComponentStatus status_check_fail
)
{
  m_bEnableStatusCheck = bEnableStatusCheck ? true : false;
  m_status_check_pass = status_check_pass;
  m_status_check_fail = status_check_fail;  
}

bool ON_SubDEdgeChain::StatusCheckEnabled() const
{
  return m_bEnableStatusCheck;
}

void ON_SubDEdgeChain::Reverse()
{
  ON_SubDEdgeChain::ReverseEdgeChain(m_edge_chain);
}

const ON_SubDEdgePtr ON_SubDEdgeChain::FirstEdgePtr() const
{
  return m_edge_chain.UnsignedCount() > 0 ? m_edge_chain[0] : ON_SubDEdgePtr::Null;
}
const ON_SubDEdgePtr ON_SubDEdgeChain::LastEdgePtr() const
{
  return m_edge_chain.UnsignedCount() > 0 ? *(m_edge_chain.Last()) : ON_SubDEdgePtr::Null;
}

const ON_SubDEdgePtr ON_SubDEdgeChain::EdgePtr(int edge_index) const
{
  return (edge_index >= 0 && edge_index < m_edge_chain.Count()) ? m_edge_chain[edge_index] : ON_SubDEdgePtr::Null;
}

const ON_SubDEdge* ON_SubDEdgeChain::FirstEdge() const
{
  return FirstEdgePtr().Edge();
}

const ON_SubDEdge* ON_SubDEdgeChain::LastEdge() const
{
  return LastEdgePtr().Edge();
}

const ON_SubDEdge* ON_SubDEdgeChain::Edge(int edge_index) const
{
  return EdgePtr(edge_index).Edge();
}

const ON_SubDVertex* ON_SubDEdgeChain::FirstVertex() const
{
  return Vertex(0);
}

const ON_SubDVertex* ON_SubDEdgeChain::LastVertex() const
{
  return Vertex(m_edge_chain.Count());
}

const ON_SubDVertex* ON_SubDEdgeChain::Vertex(int vertex_index) const
{
  const int edge_count = m_edge_chain.Count();
  if ( vertex_index >= 0 && vertex_index <= edge_count && edge_count > 0 )
  {
    return 
      (vertex_index == edge_count)
      ? m_edge_chain[edge_count - 1].RelativeVertex(1)
      : m_edge_chain[vertex_index].RelativeVertex(0);
  }
  return nullptr;
}


unsigned int ON_SubDEdgeChain::AddOneNeighbor(
  ON_ChainDirection direction,
  ON_SubD::ChainType chain_type
)
{
  const unsigned int count0 = m_edge_chain.UnsignedCount();
  if (count0 <= 0 || IsClosedLoop() )
    return 0;

  ON_SubDEdgePtr eptr;
  const ON_SubDEdge* e;
  const ON_SubDVertex* v;

  const ON_SubDVertex* chain_ends[2] = { FirstVertex() ,LastVertex() };

  eptr
    = (ON_ChainDirection::Previous != direction)
    ? ON_SubDEdgeChain::EdgeChainNeighbor(LastEdgePtr(), ON_ChainDirection::Next, chain_type, m_bEnableStatusCheck, m_status_check_pass, m_status_check_fail)
    : ON_SubDEdgePtr::Null;
  e = eptr.Edge();
  v = eptr.RelativeVertex(0);
  if ( nullptr != v && v == chain_ends[1] && false == InChain(e) )
  {
    v = eptr.RelativeVertex(1);
    if (v == chain_ends[0] || m_unique_tester.AddToList((ON__UINT_PTR)v))
    {
      m_unique_tester.ExpertAddNewToList((ON__UINT_PTR)e);
      m_edge_chain.Append(eptr);
    }
  }

  eptr
    = (ON_ChainDirection::Next != direction)
    ? ON_SubDEdgeChain::EdgeChainNeighbor(FirstEdgePtr(), ON_ChainDirection::Previous, chain_type, m_bEnableStatusCheck, m_status_check_pass, m_status_check_fail)
    : ON_SubDEdgePtr::Null;
  e = eptr.Edge();
  v = eptr.RelativeVertex(1);
  if ( nullptr != v && v == chain_ends[0] && false == InChain(e) )
  {
    v = eptr.RelativeVertex(0);
    if (v == chain_ends[1] || m_unique_tester.AddToList((ON__UINT_PTR)v))
    {
      m_unique_tester.ExpertAddNewToList((ON__UINT_PTR)e);
      m_edge_chain.Insert(0, eptr);
    }
  }

  return m_edge_chain.UnsignedCount() - count0;
}

unsigned int ON_SubDEdgeChain::AddAllNeighbors(
  ON_ChainDirection direction,
  ON_SubD::ChainType chain_type
)
{
  const unsigned int count0 = m_edge_chain.UnsignedCount();
  if (count0 <= 0 || IsClosedLoop())
    return 0;

  if (ON_ChainDirection::Previous != direction)
    while (1 == AddOneNeighbor(ON_ChainDirection::Next, chain_type)) {}      
  if (ON_ChainDirection::Next != direction)
    while (1 == AddOneNeighbor(ON_ChainDirection::Previous, chain_type)) {}      

  return m_edge_chain.UnsignedCount() - count0;
}

unsigned int ON_SubDEdgeChain::AddEdge(
  const ON_SubDEdge* edge
) 
{
  const unsigned int count0 = m_edge_chain.UnsignedCount();
  if (count0 <= 0)
    return 0;

  if (
    nullptr == edge 
    || nullptr == edge->m_vertex[0]
    || nullptr == edge->m_vertex[1] 
    || edge->m_vertex[0] == edge->m_vertex[1]
    )
    return 0;

  const ON_SubDVertex* v[2] = { FirstVertex(),LastVertex() };
  if (v[0] == v[1])
    return 0;

  if ( m_bEnableStatusCheck && false == ON_ComponentStatus::StatusCheck(edge->m_status, m_status_check_pass, m_status_check_fail))
    return 0;

  ON_SubDEdgePtr eptr = ON_SubDEdgePtr::Null;

  if (v[1] == edge->m_vertex[0])
    eptr = ON_SubDEdgePtr::Create(edge, 0);
  else if (v[1] == edge->m_vertex[1])
    eptr = ON_SubDEdgePtr::Create(edge, 1);
  else if (v[0] == edge->m_vertex[1])
    eptr = ON_SubDEdgePtr::Create(edge, 0);
  else if (v[0] == edge->m_vertex[0])
    eptr = ON_SubDEdgePtr::Create(edge, 1);
  else
    return 0;

  if (m_unique_tester.InList((ON__UINT_PTR)edge))
    return 0;

  if (v[1] == eptr.RelativeVertex(0) )
  {
    if (v[0] == eptr.RelativeVertex(1) || m_unique_tester.AddToList((ON__UINT_PTR)eptr.RelativeVertex(1)))
    {
      m_edge_chain.Append(eptr);
      m_unique_tester.ExpertAddNewToList((ON__UINT_PTR)edge);
    }
  }
  else if (v[0] == eptr.RelativeVertex(1) )
  {
    if (v[1] == eptr.RelativeVertex(0) || m_unique_tester.AddToList((ON__UINT_PTR)eptr.RelativeVertex(0)))
    {
      m_edge_chain.Insert(0, eptr);
      m_unique_tester.ExpertAddNewToList((ON__UINT_PTR)edge);
    }
  }

  return m_edge_chain.UnsignedCount() - count0;
}

unsigned int ON_SubDEdgeChain::RemoveEdges(
  const ON_SubDEdge* first_edge,
  const ON_SubDEdge* last_edge
) 
{
  unsigned int count0 = m_edge_chain.UnsignedCount();
  unsigned int i0 = 0;
  unsigned int i1 = count0;
  if (nullptr != first_edge)
  {
    while (i0 < count0 && first_edge != m_edge_chain[i0].Edge())
      ++i0;
  }
  if (nullptr != last_edge)
  {
    while (i1 > i0 && last_edge != m_edge_chain[i1 - 1].Edge())
      --i1;
  }

  const unsigned int count1 = i1 - i0;
  if (count1 >= count0)
    return 0;

  if (i0 > 0)
  {
    for (unsigned int i = i0; i < i1; i++)
      m_edge_chain[i - i0] = m_edge_chain[i];
  }

  m_edge_chain.SetCount(count1);
  m_unique_tester.ClearList();
  for (unsigned int i = 0; i < count1; i++)
  {
    m_unique_tester.AddToList((ON__UINT_PTR)m_edge_chain[i].Edge());
    m_unique_tester.AddToList((ON__UINT_PTR)m_edge_chain[i].RelativeVertex(0));
  }
  if ( FirstVertex() != LastVertex() )
    m_unique_tester.AddToList((ON__UINT_PTR)LastVertex());

  return count0 - count1;
}


void ON_SubDEdgeChain::ReverseEdgeChain(
  ON_SimpleArray< ON_SubDEdgePtr >& edge_chain
)
{
  ON_SubDEdgeChain::ReverseEdgeChain(edge_chain.Array(), edge_chain.UnsignedCount());
}

void ON_SubDEdgeChain::ReverseEdgeChain(
  ON_SubDEdgePtr* edge_chain,
  size_t edge_count
  )
{
  if (edge_count <= 0 || nullptr == edge_chain)
    return;
  ON_SubDEdgePtr* p0 = edge_chain;
  ON_SubDEdgePtr* p1 = p0 + (edge_count - 1);
  while ( p0 < p1)
  {
    ON_SubDEdgePtr eptr = p0->Reversed();
    *p0 = p1->Reversed();
    *p1 = eptr;
    ++p0;
    --p1;
  }
  if (p0 == p1)
    *p0 = p0->Reversed();
}

bool ON_SubDEdgeChain::IsValidEdgeChain(
  const ON_SimpleArray< ON_SubDEdgePtr >& edge_chain,
  bool bCheckForDuplicateEdges
)
{
  return ON_SubDEdgeChain::IsValidEdgeChain(edge_chain.Array(), edge_chain.UnsignedCount(), bCheckForDuplicateEdges);
}

bool ON_SubDEdgeChain::IsValidEdgeChain(
  const ON_SubDEdgePtr* edge_chain,
  size_t edge_count,
  bool bCheckForDuplicateEdges
)
{
  if (edge_count <= 0)
    return true;
  if (nullptr == edge_chain)
    return false;

  const ON_SubDVertex* first_vertex = edge_chain->RelativeVertex(0);
  if (nullptr == first_vertex)
    return false;

  const ON_SubDVertex* v = first_vertex;
  const ON_SubDEdgePtr* p0 = edge_chain;
  const ON_SubDEdgePtr* p1 = edge_chain+1;

  for (const ON_SubDEdgePtr* p = p0; p < p1; ++p)
  {
    const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(p->m_ptr);
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return false;
    ON__UINT_PTR edir = ON_SUBD_EDGE_DIRECTION(p->m_ptr);
    const ON_SubDVertex* v0 = e->m_vertex[edir];
    const ON_SubDVertex* v1 = e->m_vertex[1 - edir];
    if (v0 != v || nullptr == v1 || v0 == v1)
      return false;
    v = v1;
  }

  if (bCheckForDuplicateEdges)
  {
    const ON_SubDVertex* last_vertex = v;
    ON_UniqueTester tester;
    for (const ON_SubDEdgePtr* p = p0; p < p1; ++p)
    {
      const ON_SubDEdge* e = ON_SUBD_EDGE_POINTER(p->m_ptr);
      if (false == tester.AddToList((ON__UINT_PTR)e))
        return false; // duplicate edge
      if (false == tester.AddToList((ON__UINT_PTR)e->m_vertex[ON_SUBD_EDGE_DIRECTION(p->m_ptr)]))
        return false; // duplicate vertex
    }    
    if (first_vertex != last_vertex)
    {
      if (false == tester.AddToList((ON__UINT_PTR)last_vertex))
        return false; // duplicate vertex
    }
  }

  return true;
}

class ON_SubDMeshImpl* ON_SubDMesh::SubLimple() const
{
  return m_impl_sp.get();
}

unsigned int ON_SubDMesh::SubLimpleUseCount() const
{
  return (unsigned int)(m_impl_sp.use_count());
}


bool ON_SubD::IsSolid() const
{
  bool bIsManifold = false;
  bool bIsOriented = false;
  bool bHasBoundary = false;
  int solid_orientation = 0;
  ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
  return (bIsManifold && bIsOriented && false == bHasBoundary);
}

int ON_SubD::SolidOrientation() const
{
  bool bIsManifold = false;
  bool bIsOriented = false;
  bool bHasBoundary = false;
  int solid_orientation = 0;
  ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
  return solid_orientation;
}

bool ON_SubD::IsManifold( bool& bIsOriented, bool& bHasBoundary ) const
{
  bool bIsManifold = false;
  bIsOriented = false;
  bHasBoundary = false;
  int solid_orientation = 0;
  ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
  return bIsManifold;
}

bool ON_SubD::IsManifold() const
{
  bool bIsOriented = false;
  bool bHasBoundary = false;
  return IsManifold(bIsOriented, bHasBoundary);
}

class ON_SubDEdgePtrLink
{
public:
  ON_SubDEdgePtrLink() = default;
  ~ON_SubDEdgePtrLink() = default;
  ON_SubDEdgePtrLink(const ON_SubDEdgePtrLink&) = default;
  ON_SubDEdgePtrLink& operator=(const ON_SubDEdgePtrLink&) = default;

public:
  static const ON_SubDEdgePtrLink Empty;

public:
  ON_SubDEdgePtr m_ep = ON_SubDEdgePtr::Null;
  unsigned int m_index = 0;
  unsigned int m_nbr_index = 0;

  static int CompareVertex(
    const ON_SubDEdgePtrLink* lhs,
    const ON_SubDEdgePtrLink* rhs
  )
  {
    const ON_SubDVertex* lhs_v = lhs->m_ep.RelativeVertex(0);
    const ON_SubDVertex* rhs_v = rhs->m_ep.RelativeVertex(0);
    if (lhs_v < rhs_v)
      return -1;
    if (lhs_v > rhs_v)
      return 1;
    return 0;
  }

  static int CompareIndex(
    const ON_SubDEdgePtrLink* lhs,
    const ON_SubDEdgePtrLink* rhs
  )
  {
    const unsigned int lhs_i = lhs->m_index;
    const unsigned int rhs_i = rhs->m_index;
    if (lhs_i < rhs_i)
      return -1;
    if (lhs_i > rhs_i)
      return 1;
    return 0;
  }

  static void Resolve3OrMoreEdges(
    const unsigned int unset_nbr1_index,
    unsigned int count,
    const ON_SubDVertex* v,
    ON_SubDEdgePtrLink* links
  );  
};

const ON_SubDEdgePtrLink ON_SubDEdgePtrLink::Empty;

void ON_SubDEdgePtrLink::Resolve3OrMoreEdges(
  const unsigned int unset_nbr1_index,
  unsigned int count,
  const ON_SubDVertex* v,
  ON_SubDEdgePtrLink* links
)
{
  // If the case can't be resolved by Resolve3OrMoreEdges(), 
  // then the vertex will not appear in the middle of a chain.

  if (count < 3 || nullptr == v || count != (unsigned int)v->m_edge_count)
    return;

  switch (count)
  {
  case 3:
    if (false == v->IsCrease() && false == v->IsDart())
      return;
    break;

  case 4:
    if (false == v->IsCrease() && false == v->IsSmooth())
      return;
    break;

  default:
    if (false == v->IsCrease())
      return;
    break;
  }

  const ON_SubDEdge* link_edges[4] = {};
  const ON_SubDEdge* vertex_edges[4] = {};
  unsigned int crease_edge_count = 0;
  unsigned int smooth_edge_count = 0;
  unsigned int smooth_edge_link_index[4] = {};
  unsigned int crease_edge_link_index[4] = {};
  for (unsigned int j = 0; j < count; j++)
  {
    const ON_SubDEdge* e = links[j].m_ep.Edge();
    if (nullptr == e)
      return;
    const ON_SubDEdge* ve = v->Edge(j);
    if (nullptr == ve)
      return;
    if (j < 4)
    {
      link_edges[j] = e;
      vertex_edges[j] = ve;
    }

    if (e->IsSmooth() && 2 == e->m_face_count)
    {
      if ( smooth_edge_count < 4)
        smooth_edge_link_index[smooth_edge_count] = j;
      ++smooth_edge_count;
    }
    else if (e->IsCrease())
    {
      if (crease_edge_count < 4)
        crease_edge_link_index[crease_edge_count] = j;
      ++crease_edge_count;
    }
    else
      return;
  }

  if ( 2 == crease_edge_count && v->IsCrease() )
  {
    // Link the two creased edges.
    // The vertex will be interior in a chain and the edges
    // will be next to each other.
    links[crease_edge_link_index[0]].m_nbr_index = links[crease_edge_link_index[1]].m_index;
    links[crease_edge_link_index[1]].m_nbr_index = links[crease_edge_link_index[0]].m_index;
    if (1 == smooth_edge_count)
    {
      // this edge will be at the end of a chain.
      links[smooth_edge_link_index[0]].m_nbr_index = unset_nbr1_index;
    }
  }

  if (2 == smooth_edge_count)
  {
    // Link the two smooth edges.
    // The vertex will be interior in a chain and the edges
    // will be next to each other.
    links[smooth_edge_link_index[0]].m_nbr_index = links[smooth_edge_link_index[1]].m_index;
    links[smooth_edge_link_index[1]].m_nbr_index = links[smooth_edge_link_index[0]].m_index;
    if (1 == crease_edge_count)
    {
      // this edge will be at the end of a chain
      links[crease_edge_link_index[0]].m_nbr_index = unset_nbr1_index;
    }
   }

  if (
    4 != count
    || 4 != smooth_edge_count
    || 0 != crease_edge_count
    || 4 != v->m_face_count
    || 4 != v->m_edge_count
    || false == v->IsSmooth()
    )
    return;

  // make sure vertex_edges[] and link_edges[] are the same list.
  unsigned int match_count = 0;
  for (unsigned int j = 0; j == match_count && j < count; j++)
  {
    for (unsigned int k = 0; k < count; k++)
    {
      if (vertex_edges[k] == link_edges[j])
      {
        vertex_edges[k] = nullptr;
        match_count++;
        break;
      }
    }
  }
  if (match_count != count)
    return;
  
  // vertex has 4 faces and 4 smooth edges. Link opposite edges.
  const ON_SubDFace* edge_faces[4][2];
  for (unsigned int j = 0; j < 4; j++)
  {
    edge_faces[j][0] = link_edges[j]->Face(0);
    edge_faces[j][1] = link_edges[j]->Face(1);
    if (nullptr == edge_faces[j][0] || nullptr == edge_faces[j][1])
      return;
  }
  ON_2udex pairs[2];
  unsigned int pair_count = 0;
  ON_2udex pair;
  for (pair.i = 0; pair.i < 4; ++pair.i) for (pair.j = pair.i+1; pair.j < 4; ++pair.j)
  {
    if (
      edge_faces[pair.i][0] != edge_faces[pair.j][0]
      && edge_faces[pair.i][0] != edge_faces[pair.j][1]
      && edge_faces[pair.i][1] != edge_faces[pair.j][0]
      && edge_faces[pair.i][1] != edge_faces[pair.j][1]
      )
    {
      // the associated edges share no faces.
      if ( pair_count < 2)
        pairs[pair_count] = pair;
      if (++pair_count > 2)
        break;
    }
  }
  if (2 == pair_count)
  {
    links[pairs[0].i].m_nbr_index = links[pairs[0].j].m_index;
    links[pairs[0].j].m_nbr_index = links[pairs[0].i].m_index;
    links[pairs[1].i].m_nbr_index = links[pairs[1].j].m_index;
    links[pairs[1].j].m_nbr_index = links[pairs[1].i].m_index;
  }

  return;
}

unsigned int ON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const ON_SubD& subd,
  const ON_SimpleArray< ON_COMPONENT_INDEX >& unsorted_edges,
  unsigned int minimum_chain_length,
  ON_SimpleArray< ON_SubDEdgePtr >& sorted_edges
)
{
  const unsigned count = unsorted_edges.UnsignedCount();
  ON_SimpleArray< const ON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
  {
    const ON_SubDEdge* e = subd.EdgeFromComponentIndex(unsorted_edges[i]);
    if (nullptr != e)
      a.Append(e);
  }
  return SortEdgesIntoEdgeChains(a, minimum_chain_length, sorted_edges);
}

unsigned int ON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const ON_SimpleArray< ON_SubDComponentPtr >& unsorted_edges,
  unsigned int minimum_chain_length,
  ON_SimpleArray< ON_SubDEdgePtr >& sorted_edges
)
{
  const unsigned count = unsorted_edges.UnsignedCount();
  ON_SimpleArray< const ON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
    a.Append(unsorted_edges[i].Edge());
  return ON_SubDEdgeChain::SortEdgesIntoEdgeChains(a, minimum_chain_length, sorted_edges);
}

unsigned int ON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const ON_SimpleArray< const ON_SubDEdge* >& unsorted_edges,
  unsigned int minimum_chain_length,
  ON_SimpleArray< ON_SubDEdgePtr >& sorted_edges
)
{
  const unsigned int unsorted_edge_count = unsorted_edges.Count();
  ON_SimpleArray< ON_SubDEdgePtr > unsorted_eptrs(unsorted_edge_count);
  for (unsigned i = 0; i < unsorted_edge_count; ++i)
  {
    const ON_SubDEdge* e = unsorted_edges[i];
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] || e->m_vertex[0] == e->m_vertex[1])
      continue;
    ON_SubDEdgePtr eptr = ON_SubDEdgePtr::Create(e, 0);
    if (1 == e->m_face_count && 0 == ON_SUBD_FACE_DIRECTION(e->m_face2[0].m_ptr))
      eptr = eptr.Reversed();
    unsorted_eptrs.Append(eptr);
  }
  return ON_SubDEdgeChain::SortEdgesIntoEdgeChains(unsorted_eptrs, minimum_chain_length, sorted_edges);
}

unsigned int ON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const ON_SimpleArray< ON_SubDEdgePtr >& unsorted_edges,
  unsigned int minimum_chain_length,
  ON_SimpleArray< ON_SubDEdgePtr >& sorted_edges
)
{
  // NOTE:
  //  unsorted_edges[] and sorted_edges[] may reference the same array.
  ////const ON_SubDEdge* ee[2] = {};
  ////const ON_SubDVertex* vv[2] = {};

  const unsigned int unsorted_edge_count = unsorted_edges.Count();
  if (unsorted_edge_count <= 0)
  {
    sorted_edges.SetCount(0);
    return 0;
  }

  ON_SimpleArray< ON_SubDEdgePtrLink > links(2*unsorted_edge_count);
  const unsigned int unset_nbr1_index = 0xFFFFFFFEU;
  const unsigned int unset_nbrX_index = unset_nbr1_index+1;
  ON_SubDEdgePtrLink epl;
  epl.m_nbr_index = unset_nbrX_index;
  for (unsigned int i = 0; i < unsorted_edge_count; i++)
  {
    ON_SubDEdgePtr ep = unsorted_edges[i];
    const ON_SubDEdge* e = ep.Edge();
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] || e->m_vertex[0] == e->m_vertex[1])
      continue;

    // unsorted_edges[i] has 2 links with m_index = 2*i and m_index = 2*+1.

    // links with even m_index have opposite orientation as unsorted_edges[].
    // links[2*i].m_ep.RelativeVertex(0) = unsorted_edges[i].RelativeVertex(1)
    epl.m_ep = ep.Reversed();
    links.Append(epl);
    ++epl.m_index;
    ////ee[0] = epl.m_ep.Edge();
    ////vv[1] = epl.m_ep.RelativeVertex(0);

    // links with odd m_index have same orientation as unsorted_edges[].
    // links[2*i+1].m_ep.RelativeVertex(0) = unsorted_edges[i].RelativeVertex(0)
    epl.m_ep = ep;
    links.Append(epl);
    ++epl.m_index;
    ////ee[1] = epl.m_ep.Edge();
    ////vv[0] = epl.m_ep.RelativeVertex(0);
    ////if (e != ee[0] || e != ee[1] || vv[0] == vv[1])
    ////  return false;
  }

  // NOTE:
  // unsorted_edges[] and sorted_edges[] may reference the same array.
  // At this point, I'm finished with unsorted_edges[] so it's ok to 
  // modify sorted_edges[] here.
  sorted_edges.SetCount(0);
  sorted_edges.Reserve(unsorted_edge_count);

  // sort links by ON_SubDEdgePtr.RelativeVertex(0) and set m_nbr_index
  links.QuickSort(ON_SubDEdgePtrLink::CompareVertex);
  // link_count = even number
  const unsigned int link_count = links.UnsignedCount();
  unsigned int i1 = link_count;
  for (unsigned int i0 = 0; i0 < link_count; i0 = i1)
  {
    ON_SubDEdgePtrLink& epl0 = links[i0];
    const ON_SubDVertex* v = epl0.m_ep.RelativeVertex(0);
    for (i1 = i0 + 1; i1 < link_count; ++i1)
    {
      if (0 != ON_SubDEdgePtrLink::CompareVertex(&epl0, &links[i1]))
        break;
    }
    if (nullptr == v)
    {
      ON_SUBD_ERROR("Bug in code that creates the links[] array.");
      continue;
    }
    if (v->IsCorner())
    {
      // These edges will be at the ends of chains.
      while (i0 < i1)
        links[i0++].m_nbr_index = unset_nbr1_index;
      continue;
    }

    if (i0 + 1 == i1)
    {
      // The vertex is referenced by exactly 1 edge in unsorted_edges[]
      // This edge will appear in sorted_edges[] at the start or end of a chain.
      epl0.m_nbr_index = unset_nbr1_index;
      continue; 
    }
    if (i0 + 2 == i1)
    {
      // This vertex is referenced by exactly 2 edges in  unsorted_edges[].
      // The vertex will be in the interior of a chain and the edges will 
      // appear in sorted_edges[] next to each other in the same chain.
      ON_SubDEdgePtrLink& epl1 = links[i0 + 1];
      epl0.m_nbr_index = epl1.m_index;
      epl1.m_nbr_index = epl0.m_index;
      continue;
    }

    // The vertex referenced by 3 or more edges in unsorted_edges[].
    // If the case cannot be resolved by Resolve3OrMoreEdges(), 
    // then this vertex will not be in the interior of a chain.
    ON_SubDEdgePtrLink::Resolve3OrMoreEdges(
      unset_nbr1_index,
      i1 - i0,
      v,
      links.Array() + i0
    );
  }

  // Sort links[] by m_index valut to restore links[] to its original order.
  links.QuickSort(ON_SubDEdgePtrLink::CompareIndex);
  ON_SubDEdgePtrLink* links_array = links.Array();

  unsigned chain_count = 0;
  ON_SimpleArray<ON_SubDEdgePtr> chain(unsorted_edge_count);

  for (unsigned int i = 0; i < link_count; ++i)
  {
    // epl0 and epl1 are the links for edges[i/2]
    const ON_SubDEdgePtrLink epl0 = links_array[i];
    links_array[i].m_ep = ON_SubDEdgePtr::Null;
    const ON_SubDEdgePtrLink epl1 = links_array[++i];
    links_array[i].m_ep = ON_SubDEdgePtr::Null;

    if (nullptr == epl0.m_ep.Edge())
      continue; // this edge has already been inserted in sorted_edges[].

    chain.SetCount(0);

    // Add edges that come "before" edges[i/2] to chain[]
    epl = epl1;
    for (;;)
    {
      if (epl.m_nbr_index >= unset_nbr1_index)
        break;
      unsigned int j = epl.m_nbr_index;
      unsigned int j1 = (0 == (j % 2)) ? (j + 1) : (j - 1);
      // epl = "previous" link
      epl = links_array[j1];
      links_array[j].m_ep = ON_SubDEdgePtr::Null;
      links_array[j1].m_ep = ON_SubDEdgePtr::Null;
      if (nullptr == epl.m_ep.Edge())
        break;
      ////ee[0] = epl.m_ep.Edge();
      ////vv[0] = epl.m_ep.RelativeVertex(0);
      ////vv[1] = epl.m_ep.RelativeVertex(1);
      ////if (vv[0] == vv[1] || nullptr == ee[0])
      ////  return false;
      chain.Append(epl.m_ep);
    }

    const bool bClosedChain = (epl.m_index == epl1.m_index);
    const bool bReverseFinalChain
      = false == bClosedChain
      && unset_nbr1_index == epl.m_nbr_index
      && (0 == (epl.m_index % 2));
    if (false == bClosedChain)
    {
      chain.Reverse();
    }

    ////ee[0] = epl1.m_ep.Edge();
    ////vv[0] = epl1.m_ep.RelativeVertex(0);
    ////vv[1] = epl1.m_ep.RelativeVertex(1);
    ////if (vv[0] == vv[1] || nullptr == ee[0])
    ////  return false;
    chain.Append(epl1.m_ep); // matches input edge orientation

    if (bClosedChain)
    {
      // put edges[i/2] at the start of the closed chain.
      chain.Reverse();
    }
    else
    {
      // Add edges that come "after" edges[i/2] to chain[]
      epl = epl0;
      for (;;)
      {
        if (epl.m_nbr_index >= unset_nbr1_index)
          break;
        unsigned int j = epl.m_nbr_index;
        unsigned int j1 = (0 == (j % 2)) ? (j + 1) : (j - 1);
        // epl = "next" link
        epl = links_array[j1];
        links_array[j].m_ep = ON_SubDEdgePtr::Null;
        links_array[j1].m_ep = ON_SubDEdgePtr::Null;
        if (nullptr == epl.m_ep.Edge())
          break;
        ////ee[0] = epl.m_ep.Edge();
        ////vv[0] = epl.m_ep.RelativeVertex(1);
        ////vv[1] = epl.m_ep.RelativeVertex(0);
        ////if (vv[0] == vv[1] || nullptr == ee[0])
        ////  return false;
        chain.Append(epl.m_ep.Reversed());
      }

      if (bReverseFinalChain)
        chain.Reverse();
    }

    const unsigned int chain_edge_count = chain.UnsignedCount();
    if (chain_edge_count > 0)
    {
      for (;;)
      {
        if (chain_edge_count < 3)
          break;
        const ON_SubDVertex* c0 = chain[0].RelativeVertex(0);
        if (nullptr == c0)
          break;
        if (c0->IsCorner())
          break;
        const ON_SubDVertex* c1 = chain[chain_edge_count-1].RelativeVertex(1);
        if (c0 != c1)
          break;
        const ON_SubDEdge* e0 = chain[0].Edge();
        if (nullptr == e0)
          break;
        const ON_SubDEdge* e1 = chain[chain_edge_count-1].Edge();
        if (nullptr == e1)
          break;
        const bool bSmooth = e0->IsSmooth();
        if (bSmooth != e1->IsSmooth())
          break;
        if (bSmooth && c0->IsCrease())
          break;
        // Check for an embedded crease vertex.
        for (unsigned int k = 1; k < chain_edge_count; ++k)
        {
          const ON_SubDVertex* v = chain[k].RelativeVertex(0);
          if (nullptr == v)
            break;
          const ON_SubDEdge* e = chain[k].Edge();
          if (nullptr == e)
            break;
          if (
            bSmooth != e->IsSmooth()
            || (bSmooth && v->IsCreaseOrCorner())
            )
          {
            // shift chain[] so it begins at chain[k];
            ON_SimpleArray<ON_SubDEdgePtr> tail;
            tail.Append(k, chain.Array());
            for (unsigned n = k; n < chain_edge_count; ++n)
              chain[n - k] = chain[n];
            chain.SetCount(chain_edge_count - k);
            chain.Append(tail.Count(), tail.Array());
            break;
          }
        }
        break;
      }
      if (chain.UnsignedCount() >= minimum_chain_length)
      {
        ++chain_count;
        sorted_edges.Append(chain.Count(), chain.Array());
      }
    }

    if ( link_count == 2*sorted_edges.UnsignedCount() )
      break; // we've used all the links - no need to "skip over the rest".
  }

  return chain_count;
}

unsigned int ON_SubDEdgeChain::OrientEdgesIntoEdgeChains(
  const ON_SubD& subd,
  const ON_SimpleArray< ON_COMPONENT_INDEX >& edges,
  ON_SimpleArray< ON_SubDEdgePtr >& edge_chain
)
{
  const unsigned count = edges.UnsignedCount();
  ON_SimpleArray< const ON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
    a.Append(subd.EdgeFromComponentIndex(edges[i]));
  return ON_SubDEdgeChain::OrientEdgesIntoEdgeChains(a, edge_chain);
}

unsigned int ON_SubDEdgeChain::OrientEdgesIntoEdgeChains(
  const ON_SimpleArray< ON_SubDComponentPtr >& edges,
  ON_SimpleArray< ON_SubDEdgePtr >& edge_chain
)
{
  const unsigned count = edges.UnsignedCount();
  ON_SimpleArray< const ON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
    a.Append(edges[i].Edge());
  return ON_SubDEdgeChain::OrientEdgesIntoEdgeChains(a, edge_chain);
}

unsigned int ON_SubDEdgeChain::OrientEdgesIntoEdgeChains(
  const ON_SimpleArray< const ON_SubDEdge* >& edges,
  ON_SimpleArray< ON_SubDEdgePtr >& edge_chains
)
{
  const unsigned count = edges.UnsignedCount();
  edge_chains.SetCount(0);
  edge_chains.Reserve(count);
  unsigned int chain_count = 0;
  unsigned chain_length = 0;
  ON_SubDEdgePtr* prev_eptr = nullptr;
  for (unsigned i = 0; i < count; ++i)
  {
    const ON_SubDEdge* e = edges[i];
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] || e->m_vertex[0] == e->m_vertex[1])
      continue;
    ON_SubDEdgePtr& eptr = edge_chains.AppendNew();
    eptr = ON_SubDEdgePtr::Create(e);
    if (nullptr != prev_eptr && prev_eptr->RelativeVertex(1) != eptr.RelativeVertex(0) )
    {
      const ON_SubDVertex* prev_v[2] = { prev_eptr->RelativeVertex(0), prev_eptr->RelativeVertex(1) };
      const ON_SubDVertex* v[2] = { eptr.RelativeVertex(0), eptr.RelativeVertex(1) };
      if (prev_v[1] == v[1])
        eptr = eptr.Reversed();
      else if (1 == chain_length)
      {
        if (prev_v[0] == v[0])
          *prev_eptr = prev_eptr->Reversed();
        else if (prev_v[0] == v[1])
        {
          *prev_eptr = prev_eptr->Reversed();
          eptr = eptr.Reversed();
        }
        else
          prev_eptr = nullptr;
      }
      else
        prev_eptr = nullptr;
    }

    if (nullptr == prev_eptr)
    {
      chain_count = 1;
      chain_length = 0;
    }
    prev_eptr = &eptr;
    ++chain_length;
  }
  return chain_count;
}



ON_SubDComponentList::ON_SubDComponentList(const ON_SubDComponentList& src)
  : m_subd_runtime_serial_number(src.m_subd_runtime_serial_number)
  , m_subd_content_serial_number(src.m_subd_content_serial_number)
  , m_subd_vertex_count(src.m_subd_vertex_count)
  , m_subd_edge_count(src.m_subd_edge_count)
  , m_subd_face_count(src.m_subd_face_count)
  , m_component_list(src.m_component_list)
{
  m_subd.ShareDimple(src.m_subd);
}

ON_SubDComponentList& ON_SubDComponentList::operator=(const ON_SubDComponentList& src)
{
  if (this != &src)
  {
    m_subd_runtime_serial_number = src.m_subd_runtime_serial_number;
    m_subd_content_serial_number = src.m_subd_content_serial_number;
    m_subd_vertex_count = src.m_subd_vertex_count;
    m_subd_edge_count = src.m_subd_edge_count;
    m_subd_face_count = src.m_subd_face_count;
    m_component_list = src.m_component_list;
    m_subd.ShareDimple(src.m_subd);
  }
  return *this;
}

ON__UINT64 ON_SubDComponentList::SubDRuntimeSerialNumber() const
{
  return m_subd_runtime_serial_number;
}

ON__UINT64 ON_SubDComponentList::SubDContentSerialNumber() const
{
  return m_subd_content_serial_number;
}

unsigned int ON_SubDComponentList::Count() const
{
  return m_component_list.UnsignedCount();
}

const ON_SubDComponentPtr ON_SubDComponentList::operator[](int i) const
{
  return i >= 0 && i < m_component_list.Count() ? m_component_list[i] : ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentList::operator[](unsigned int i) const
{
  return i < m_component_list.UnsignedCount() ? m_component_list[i] : ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentList::operator[](ON__INT64 i) const
{
  return i >= 0 && i < ((ON__INT64)m_component_list.Count()) ? m_component_list[i] : ON_SubDComponentPtr::Null;
}

const ON_SubDComponentPtr ON_SubDComponentList::operator[](ON__UINT64 i) const
{
  return i < ((ON__UINT64)m_component_list.UnsignedCount()) ? m_component_list[i] : ON_SubDComponentPtr::Null;
}

#if defined(ON_RUNTIME_APPLE)
const ON_SubDComponentPtr ON_SubDComponentList::operator[](size_t i) const
{
  return i >= 0 && i < m_component_list.Count() ? m_component_list[i] : ON_SubDComponentPtr::Null;
}
#endif

const ON_SimpleArray< ON_SubDComponentPtr >& ON_SubDComponentList::ComponentList() const
{
  return this->m_component_list;
}

const ON_SubD& ON_SubDComponentList::SubD() const
{
  return m_subd;
}

ON__UINT64 ON_SubDComponentList::UpdateContentSerialNumber()
{
  m_subd_content_serial_number = m_subd.ContentSerialNumber();
  return m_subd_content_serial_number;
}

unsigned int ON_SubDComponentList::UpdateSubDForExperts(const ON_SubD & subd, bool bUpdateDeletedComponents)
{
  const unsigned count0 = Count();
  if (subd.RuntimeSerialNumber() == m_subd.RuntimeSerialNumber())
    return count0; // the components in this list are in subd.

  // Use the component ids to update the list to reference components in subd.
  unsigned count1 = 0;
  for (unsigned i = 0; i < count0; ++i)
  {
    ON_SubDComponentPtr cptr0 = m_component_list[i];
    const ON_SubDComponentBase* c0 = cptr0.ComponentBase();
    if (nullptr == c0)
      continue;
    if (false == bUpdateDeletedComponents && false == c0->IsActive())
      continue;
    ON_COMPONENT_INDEX ci = cptr0.ComponentIndex();
    if (0 == ci.m_index)
      continue;
    ON_SubDComponentPtr cptr1 = subd.ComponentPtrFromComponentIndex(ci);
    if (cptr1.IsNull())
      continue;
    if (0 != cptr0.ComponentDirection())
      cptr1.SetComponentDirection();
    m_component_list[count1++] = cptr1;
  }
  m_component_list.SetCount(count1);
  m_subd.ShareDimple(subd);
  m_subd_runtime_serial_number = m_subd.RuntimeSerialNumber();
  m_subd_content_serial_number = m_subd.ContentSerialNumber();
  return Count();
}

unsigned ON_SubDComponentList::CreateFromComponentList(const ON_SubD& subd, const ON_SimpleArray<ON_COMPONENT_INDEX>& component_list)
{
  ON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = component_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const ON_COMPONENT_INDEX ci = component_list[i];
    if (ON_COMPONENT_INDEX::TYPE::subd_vertex != ci.m_type)
      continue;
    const unsigned vertex_id = (unsigned)ci.m_index;
    const ON_SubDVertex* v = subd.VertexFromId(vertex_id);
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, true, true, true, marked_count);
}

unsigned ON_SubDComponentList::CreateFromComponentList(const ON_SubD& subd, const ON_SimpleArray<ON_SubDComponentPtr>& component_list)
{
  ON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = component_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const ON_SubDComponentBase* c = component_list[i].ComponentBase();
    if (nullptr == c)
      continue;
    if (c->m_status.RuntimeMark())
      continue;
    c->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, true, true, true, marked_count);
}

unsigned ON_SubDComponentList::CreateFromVertexIdList(const ON_SubD& subd, const ON_SimpleArray<unsigned>& vertex_id_list)
{
  ON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = vertex_id_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const unsigned vertex_id = vertex_id_list[i];
    if (vertex_id <= 0 || vertex_id >= ON_UNSET_UINT_INDEX)
      continue;
    const ON_SubDVertex* v = subd.VertexFromId(vertex_id);
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, true, marked_count);
}


unsigned ON_SubDComponentList::CreateFromVertexList(const ON_SubD& subd, const ON_SimpleArray<ON_SubDVertexPtr>& vertex_list)
{
  ON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = vertex_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const ON_SubDVertex* v = vertex_list[i].Vertex();
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, true, marked_count);
}

unsigned ON_SubDComponentList::CreateFromVertexList(const ON_SubD& subd, const ON_SimpleArray<const ON_SubDVertex*>& vertex_list)
{
  ON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = vertex_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const ON_SubDVertex* v = vertex_list[i];
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, true, marked_count);
}

unsigned ON_SubDComponentList::CreateFromMarkedComponents(const ON_SubD& subd, bool bComponentInListMark)
{
  unsigned marked_count = 0;
  ON_SubDComponentIterator cit(subd);
  if (bComponentInListMark)
    bComponentInListMark = true; // avoid other byte values.
  for (ON_SubDComponentPtr c = cit.FirstComponent(); c.IsNotNull(); c = cit.NextComponent())
  {
    if (bComponentInListMark != c.Mark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, true, true, true, bComponentInListMark, marked_count);
}

unsigned ON_SubDComponentList::CreateFromMarkedVertices(const ON_SubD& subd, bool bVertexInListMark)
{
  unsigned marked_count = 0;
  ON_SubDVertexIterator vit(subd);
  if (bVertexInListMark)
    bVertexInListMark = true; // avoid other byte values.
  for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
  {
    if (bVertexInListMark != v->m_status.RuntimeMark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, bVertexInListMark, marked_count);
}

unsigned ON_SubDComponentList::CreateFromMarkedEdges(const ON_SubD& subd, bool bEdgeInListMark)
{
  unsigned marked_count = 0;
  ON_SubDEdgeIterator eit(subd);
  if (bEdgeInListMark)
    bEdgeInListMark = true; // avoid other byte values.
  for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    if (bEdgeInListMark != e->m_status.RuntimeMark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, false, true, false, bEdgeInListMark, marked_count);
}

unsigned ON_SubDComponentList::CreateFromMarkedFaces(const ON_SubD& subd, bool bFaceInListMark)
{
  unsigned marked_count = 0;
  ON_SubDFaceIterator fit(subd);
  if (bFaceInListMark)
    bFaceInListMark = true; // avoid other byte values.
  for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
  {
    if (bFaceInListMark != f->m_status.RuntimeMark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, false, false, true, bFaceInListMark, marked_count);
}

unsigned ON_SubDComponentList::Internal_Create(const ON_SubD & subd, bool bAddVertices, bool bAddEdges, bool bAddFaces, bool bComponentInListMark, unsigned marked_component_count)
{
  Destroy();

  if (0 == marked_component_count)
    return 0;

  const unsigned face_count = bAddFaces ? subd.FaceCount() : 0U;
  const unsigned edge_count = bAddEdges ? subd.EdgeCount() : 0U;
  const unsigned vertex_count = bAddVertices ? subd.VertexCount() : 0U;
  if (0 == vertex_count && 0 == edge_count && 0 == face_count)
    return 0;

  if (marked_component_count > vertex_count + edge_count + face_count)
    return 0;

  bComponentInListMark = bComponentInListMark ? true : false;
  m_component_list.Reserve(marked_component_count);
  m_component_list.SetCount(0);
  if (vertex_count > 0)
  {
    ON_SubDVertexIterator vit(subd);
    for (const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (bComponentInListMark != v->m_status.RuntimeMark())
        continue;
      m_component_list.Append(v->ComponentPtr());
    }
  }
  if (edge_count > 0)
  {
    ON_SubDEdgeIterator eit(subd);
    for (const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (bComponentInListMark != e->m_status.RuntimeMark())
        continue;
      m_component_list.Append(e->ComponentPtr());
    }
  }
  if (face_count > 0)
  {
    ON_SubDFaceIterator fit(subd);
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (bComponentInListMark != f->m_status.RuntimeMark())
        continue;
      m_component_list.Append(f->ComponentPtr());
    }
  }

  if (m_component_list.UnsignedCount() > 0)
  {
    m_subd.ShareDimple(subd);
    m_subd_runtime_serial_number = subd.RuntimeSerialNumber();
    m_subd_content_serial_number = subd.ContentSerialNumber();
  }
  return m_component_list.UnsignedCount();
}

unsigned int ON_SubDComponentList::RemoveAllComponents()
{
  const unsigned count0 = Count();
  m_component_list.SetCount(0);
  return count0;
}

unsigned int ON_SubDComponentList::RemoveAllVertices()
{
  return Internal_RemoveComponents(true, false, false);
}

unsigned int ON_SubDComponentList::RemoveAllEdges()
{
  return Internal_RemoveComponents(false, true, false);
}

unsigned int ON_SubDComponentList::RemoveAllFaces()
{
  return Internal_RemoveComponents(false, false, true);
}

unsigned ON_SubDComponentList::Internal_RemoveComponents(
  bool bRemoveVertices,
  bool bRemoveEdges,
  bool bRemoveFaces
)
{
  unsigned int count0 = Count();
  if (bRemoveVertices || bRemoveEdges || bRemoveFaces)
  {
    unsigned count1 = 0;
    for (unsigned i = 0; i < count0; ++i)
    {
      const ON_SubDComponentPtr cptr = m_component_list[i];
      bool bRemove = false;
      switch (cptr.ComponentType())
      {
      case ON_SubDComponentPtr::Type::Vertex:
        bRemove = bRemoveVertices;
        break;
      case ON_SubDComponentPtr::Type::Edge:
        bRemove = bRemoveEdges;
        break;
      case ON_SubDComponentPtr::Type::Face:
        bRemove = bRemoveFaces;
        break;
      default:
        bRemove = true;
      }
      if (bRemove)
        continue;
      m_component_list[count1++] = cptr;
    }
    m_component_list.SetCount(count1);
  }
  return count0 - Count();
}


void ON_SubDComponentList::Destroy()
{
  m_subd_runtime_serial_number = 0;
  m_subd_content_serial_number = 0;
  m_component_list.SetCount(0);
  m_subd.ShareDimple(ON_SubD::Empty);
}

const ON_SubDComponentFilter ON_SubDComponentFilter::Create(
  bool bAcceptVertices,
  bool bAcceptEdges,
  bool bAcceptFaces
)
{
  ON_SubDComponentFilter f;
  if (false == bAcceptVertices)
    f.m_bRejectVertices = true;
  if (false == bAcceptEdges)
    f.m_bRejectEdges = true;
  if (false == bAcceptFaces)
    f.m_bRejectFaces = true;
  return f;
}

bool ON_SubDComponentFilter::AcceptComponent(ON_COMPONENT_INDEX component_index, const class ON_Geometry* geometry) const
{
  if (false == component_index.IsSubDComponentIndex())
    return false;
  const ON_SubDComponentRef* cref = ON_SubDComponentRef::Cast(geometry);
  if (nullptr == cref)
    return false;
  const ON_SubDComponentPtr cptr = cref->ComponentPtr();
  if (component_index.m_index != (int)cptr.ComponentId())
    return false;
  switch (component_index.m_type)
  {
  case ON_COMPONENT_INDEX::TYPE::subd_vertex:
    if (ON_SubDComponentPtr::Type::Vertex != cptr.ComponentType())
      return false;
    break;
  case ON_COMPONENT_INDEX::TYPE::subd_edge:
    if (ON_SubDComponentPtr::Type::Edge != cptr.ComponentType())
      return false;
    break;
  case ON_COMPONENT_INDEX::TYPE::subd_face:
    if (ON_SubDComponentPtr::Type::Face != cptr.ComponentType())
      return false;
    break;
  }
  return AcceptComponent(cptr);
}

bool ON_SubDComponentFilter::AcceptComponent(const class ON_Geometry* geometry) const
{
  return AcceptComponent(ON_SubDComponentRef::Cast(geometry));
}


bool ON_SubDComponentFilter::AcceptComponent(const class ON_SubDComponentRef* cref) const
{
  return (nullptr != cref) ? AcceptComponent(cref->ComponentPtr()) : false;
}

bool ON_SubDComponentFilter::AcceptComponent(ON_SubDComponentPtr cptr) const
{
  switch (cptr.ComponentType())
  {
  case ON_SubDComponentPtr::Type::Vertex:
    return AcceptVertex(cptr.Vertex());
    break;
  case ON_SubDComponentPtr::Type::Edge:
    return AcceptEdge(cptr.Edge());
    break;
  case ON_SubDComponentPtr::Type::Face:
    return AcceptFace(cptr.Face());
    break;
  }
  return false;
}

bool ON_SubDComponentFilter::AcceptVertex(ON_SubDVertexPtr vptr) const
{
  return AcceptVertex(vptr.Vertex());
}

bool ON_SubDComponentFilter::AcceptEdge(ON_SubDEdgePtr eptr) const
{
  return AcceptEdge(eptr.Edge());
}


bool ON_SubDComponentFilter::AcceptFace(ON_SubDFacePtr fptr) const
{
  return AcceptFace(fptr.Face());
}

bool ON_SubDComponentFilter::AcceptVertex(const ON_SubDVertex * v) const
{
  if (m_bRejectVertices)
    return false;

  if (nullptr == v)
    return false;

  if (false == AcceptVertexTag(v->m_vertex_tag))
    return false;

  if (ON_SubDComponentFilter::Topology::Unset != m_vertex_topology_filter)
  {
    // check boundary/interior/nonmanifold
    if (v->HasBoundaryVertexTopology())
    {
      if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Boundary)& static_cast<unsigned char>(m_vertex_topology_filter)))
        return false;
    }
    else if (v->HasInteriorVertexTopology())
    {
      if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Interior)& static_cast<unsigned char>(m_vertex_topology_filter)))
        return false;
    }
    else
    {
      if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Nonmanifold)& static_cast<unsigned char>(m_vertex_topology_filter)))
        return false;
    }
  }

  return true;
}

bool ON_SubDComponentFilter::AcceptEdge(const ON_SubDEdge * e) const
{
  if (m_bRejectEdges)
    return false;

  if (nullptr == e)
    return false;

  if (false == AcceptEdgeTag(e->m_edge_tag))
    return false;

  if (ON_SubDComponentFilter::Topology::Unset != m_edge_topology_filter)
  {
    // check boundary/interior/nonmanifold
    if (1 == e->m_face_count)
    {
      if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Boundary)& static_cast<unsigned char>(m_edge_topology_filter)))
        return false;
    }
    else if (2 == e->m_face_count)
    {
      if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Interior)& static_cast<unsigned char>(m_edge_topology_filter)))
        return false;
    }
    else
    {
      if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Nonmanifold)& static_cast<unsigned char>(m_edge_topology_filter)))
        return false;
    }
  }

  return true;
}

bool ON_SubDComponentFilter::AcceptFace(const ON_SubDFace * f) const
{
  if (m_bRejectFaces)
    return false;

  if (nullptr == f)
    return false;

  if (m_maximum_face_edge_count > 0U)
  {
    const unsigned face_edge_count = f->m_edge_count;
    if (face_edge_count < m_minimum_face_edge_count || face_edge_count > m_maximum_face_edge_count)
      return false;
  }

  if (ON_SubDComponentFilter::Topology::Unset != m_edge_topology_filter)
  {
    const ON_SubDEdgePtr* eptr = f->m_edge4;
    for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++eptr)
    {
      if (4 == fei)
      {
        eptr = f->m_edgex;
        if (nullptr == eptr)
          break;
      }
      const ON_SubDEdge* e = eptr->Edge();
      if (nullptr == e)
        continue;
      if (1 == e->m_face_count)
      {
        if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Boundary)& static_cast<unsigned char>(m_edge_topology_filter)))
          return false;
      }
      else if (2 == e->m_face_count)
      {
        if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Interior)& static_cast<unsigned char>(m_edge_topology_filter)))
          return false;
      }
      else
      {
        if (0 == (static_cast<unsigned char>(ON_SubDComponentFilter::Topology::Nonmanifold)& static_cast<unsigned char>(m_edge_topology_filter)))
          return false;
      }
    }
  }

  return true;
}

void ON_SubDComponentFilter::SetAcceptVertices(bool bAcceptVertices)
{
  m_bRejectVertices = bAcceptVertices ? false : true;
}

bool ON_SubDComponentFilter::AcceptVertices() const
{
  return false == m_bRejectVertices;
}

void ON_SubDComponentFilter::SetAcceptEdges(bool bAcceptEdges)
{
  m_bRejectEdges = bAcceptEdges ? false : true;
}

bool ON_SubDComponentFilter::AcceptEdges() const
{
  return false == m_bRejectEdges;
}

void ON_SubDComponentFilter::SetAcceptFaces(bool bAcceptFaces)
{
  m_bRejectFaces = bAcceptFaces ? false : true;
}

bool ON_SubDComponentFilter::AcceptFaces() const
{
  return false == m_bRejectFaces;
}

void ON_SubDComponentFilter::SetVertexTopologyFilter(ON_SubDComponentFilter::Topology vertex_topology_filter)
{
  m_vertex_topology_filter = vertex_topology_filter;
}

void ON_SubDComponentFilter::ClearVertexTopologyFilter()
{
  m_vertex_topology_filter = ON_SubDComponentFilter::Topology::Unset;
}

ON_SubDComponentFilter::Topology ON_SubDComponentFilter::VertexTopologyFilter() const
{
  return m_vertex_topology_filter;
}

void ON_SubDComponentFilter::SetEdgeTopologyFilter(ON_SubDComponentFilter::Topology edge_topology_filter)
{
  m_edge_topology_filter = edge_topology_filter;
}

ON_SubDComponentFilter::Topology ON_SubDComponentFilter::EdgeTopologyFilter() const
{
  return m_edge_topology_filter;
}

void ON_SubDComponentFilter::ClearEdgeTopologyFilter()
{
  m_edge_topology_filter = ON_SubDComponentFilter::Topology::Unset;
}

void ON_SubDComponentFilter::SetFaceTopologyFilter(ON_SubDComponentFilter::Topology face_topology_filter)
{
  m_face_topology_filter = face_topology_filter;
}

ON_SubDComponentFilter::Topology ON_SubDComponentFilter::FaceTopologyFilter() const
{
  return m_face_topology_filter;
}

void ON_SubDComponentFilter::ClearFaceTopologyFilter()
{
  m_face_topology_filter = ON_SubDComponentFilter::Topology::Unset;
}

bool ON_SubDComponentFilter::AcceptVertexTag(ON_SubD::VertexTag vertex_tag) const
{
  if (ON_SubD::VertexTag::Unset == m_vertex_tag_filter[0])
    return true; // no tag filter

  for (size_t i = 0; i < sizeof(m_vertex_tag_filter) / sizeof(m_vertex_tag_filter[0]); ++i)
  {
    if (ON_SubD::VertexTag::Unset == m_vertex_tag_filter[i])
      break;
    if (m_vertex_tag_filter[i] != vertex_tag)
      continue;
    return true;
  }
  return false;
}

void ON_SubDComponentFilter::AddAcceptedVertexTag(ON_SubD::VertexTag vertex_tag)
{
  for (size_t i = 0; i < sizeof(m_vertex_tag_filter) / sizeof(m_vertex_tag_filter[0]); ++i)
  {
    if (vertex_tag == m_vertex_tag_filter[i])
      break;
    if (ON_SubD::VertexTag::Unset == m_vertex_tag_filter[i])
    {
      m_vertex_tag_filter[i] = vertex_tag;
      break;
    }
  }
}

void ON_SubDComponentFilter::ClearVertexTagFilter()
{
  for (size_t i = 0; i < sizeof(m_vertex_tag_filter) / sizeof(m_vertex_tag_filter[0]); ++i)
    m_vertex_tag_filter[i] = ON_SubD::VertexTag::Unset;
}


bool ON_SubDComponentFilter::AcceptEdgeTag(ON_SubD::EdgeTag edge_tag) const
{
  if (ON_SubD::EdgeTag::Unset == m_edge_tag_filter[0])
    return true; // no tag filter

  for (size_t i = 0; i < sizeof(m_edge_tag_filter) / sizeof(m_edge_tag_filter[0]); ++i)
  {
    if (ON_SubD::EdgeTag::Unset == m_edge_tag_filter[i])
      break;
    if (m_edge_tag_filter[i] != edge_tag)
      continue;
    return true;
  }
  return false;
}

void ON_SubDComponentFilter::AddAcceptedEdgeTag(ON_SubD::EdgeTag edge_tag)
{
  for (size_t i = 0; i < sizeof(m_edge_tag_filter) / sizeof(m_edge_tag_filter[0]); ++i)
  {
    if (edge_tag == m_edge_tag_filter[i])
      break;
    if (ON_SubD::EdgeTag::Unset == m_edge_tag_filter[i])
    {
      m_edge_tag_filter[i] = edge_tag;
      break;
    }
  }
}

void ON_SubDComponentFilter::ClearEdgeTagFilter()
{
  for (size_t i = 0; i < sizeof(m_edge_tag_filter) / sizeof(m_edge_tag_filter[0]); ++i)
    m_edge_tag_filter[i] = ON_SubD::EdgeTag::Unset;
}

bool ON_SubDComponentFilter::AcceptFaceEdgeCount(
  unsigned face_edge_count
) const
{
  return (m_maximum_face_edge_count >= 3U) ? (face_edge_count >= m_minimum_face_edge_count && face_edge_count <= m_maximum_face_edge_count) : false;
}

void ON_SubDComponentFilter::SetFaceEdgeCountFilter(
  unsigned minimum_face_edge_count,
  unsigned maximum_face_edge_count
)
{
  if (minimum_face_edge_count <= maximum_face_edge_count && maximum_face_edge_count >= 3U)
  {
    m_minimum_face_edge_count = minimum_face_edge_count;
    m_maximum_face_edge_count = maximum_face_edge_count;
  }
}

void ON_SubDComponentFilter::ClearFaceEdgeCountFilter()
{
  m_minimum_face_edge_count = 0U;
  m_maximum_face_edge_count = 0U;
}



#if defined(ON_SUBD_CENSUS)
//////////////////////////////////////////////////////////////////////////
//
// ON_CensusCounter
//


class ON_PointerHashElement
{
public:
  ON__UINT_PTR m_sn = 0;
  ON__UINT_PTR m_ptr = 0;
  ON_PointerHashElement* m_next = nullptr;
};

class ON_PointerHashTable
{
public:

  void Add(ON_CensusCounter::Class c, ON__UINT_PTR ptr);
  void Remove(ON_CensusCounter::Class c, ON__UINT_PTR ptr);

  ON__UINT_PTR SerialNumber(ON_CensusCounter::Class c, ON__UINT_PTR ptr) const;
  
  enum 
  {
    hash_count = 1024
  };
  ON_PointerHashElement* m_table[(unsigned int)ON_CensusCounter::Class::count][hash_count];


  unsigned int m_count = 0;


  static bool TheOneExists();
  static ON_PointerHashTable* TheOne();
  static void DestroyTheOne();

private:
  ON_PointerHashTable();
  ~ON_PointerHashTable();
  ON_PointerHashTable(const ON_PointerHashTable&) = delete;
  ON_PointerHashTable& operator=(const ON_PointerHashTable&) = delete;

  static unsigned int PointerHash(ON__UINT_PTR ptr);
  ON_FixedSizePool m_fsp;
  static ON_PointerHashTable* m_the_one;
};

ON_PointerHashTable* ON_PointerHashTable::m_the_one = nullptr;

ON_PointerHashTable::ON_PointerHashTable()
{
  memset(m_table,0,sizeof(m_table));
  m_fsp.Create(sizeof(ON_PointerHashElement),0,0);
}

ON_PointerHashTable::~ON_PointerHashTable()
{
  memset(m_table,0,sizeof(m_table));
  m_count = 0;
  m_fsp.Destroy();
}

bool ON_PointerHashTable::TheOneExists()
{
  return (nullptr != ON_PointerHashTable::m_the_one);
}

ON_PointerHashTable* ON_PointerHashTable::TheOne()
{
  if (nullptr == ON_PointerHashTable::m_the_one)
  {
    ON_PointerHashTable::m_the_one = new ON_PointerHashTable();
  }
  return ON_PointerHashTable::m_the_one;
}

void ON_PointerHashTable::DestroyTheOne()
{
  if (nullptr != ON_PointerHashTable::m_the_one)
  {
    delete ON_PointerHashTable::m_the_one;
    ON_PointerHashTable::m_the_one = nullptr;
  }
}

unsigned int ON_PointerHashTable::PointerHash(ON__UINT_PTR ptr)
{
  return (ON_CRC32(0, sizeof(ptr),&ptr) % ON_PointerHashTable::hash_count);
}

void ON_PointerHashTable::Add(ON_CensusCounter::Class c, ON__UINT_PTR ptr)
{
  static ON__UINT_PTR sn = 0;
  // not thread safe - a crude debugging tool
  ON_PointerHashElement* phe = (ON_PointerHashElement*)m_fsp.AllocateDirtyElement();
  phe->m_sn = ++sn;
  phe->m_ptr = ptr;
  const unsigned int hash_dex = ON_PointerHashTable::PointerHash(ptr);
  phe->m_next = m_table[(unsigned int)c][hash_dex];
  m_table[(unsigned int)c][hash_dex] = phe;
  m_count++;
}

void ON_PointerHashTable::Remove(ON_CensusCounter::Class c, ON__UINT_PTR ptr)
{
  const unsigned int hash_dex = ON_PointerHashTable::PointerHash(ptr);
  ON_PointerHashElement* phe = m_table[(unsigned int)c][hash_dex];
  for (ON_PointerHashElement* phe0 = nullptr; nullptr != phe; phe = phe->m_next)
  {
    if (ptr == phe->m_ptr)
    {
      if ( nullptr == phe0 )
        m_table[(unsigned int)c][hash_dex] = phe->m_next;
      else
        phe0 = phe->m_next;
      m_fsp.ReturnElement(phe);
      m_count--;
      return;
    }
    phe0 = phe;
  }
  // pointer not in the table
  ON_SubDIncrementErrorCount();
  return;
}


ON__UINT_PTR ON_PointerHashTable::SerialNumber(ON_CensusCounter::Class c, ON__UINT_PTR ptr) const
{
  const unsigned int hash_dex = ON_PointerHashTable::PointerHash(ptr);
  for ( ON_PointerHashElement* phe = m_table[(unsigned int)c][hash_dex]; nullptr != phe; phe = phe->m_next)
  {
    if (ptr == phe->m_ptr)
      return phe->m_sn;
  }
  // pointer not in the table
  return 0;
}

void ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class c, ON__UINT_PTR ptr)
{
  ON_PointerHashTable::TheOne()->Add(c,ptr);
}

void ON_CensusCounter::RegisterDeath(ON_CensusCounter::Class c, ON__UINT_PTR ptr)
{
  ON_PointerHashTable::TheOne()->Remove(c,ptr);
}


void ON_CensusCounter::Clear()
{
  ON_PointerHashTable::DestroyTheOne();
}

void ON_CensusCounter::CensusReport(
  class ON_TextLog& text_log
  )
{
  ON_PointerHashTable* ht = ON_PointerHashTable::TheOneExists() ? ON_PointerHashTable::TheOne() : nullptr;
  if ( nullptr == ht )
  {
    text_log.Print("No class census information exists.\n");
    return;
  }

  text_log.Print("%d items exist.\n",ht->m_count);
  for (unsigned int i = 0; i < (unsigned int)ON_CensusCounter::Class::count; i++)
  {
    bool bPrintClassName = true;
    const char* sClassName = nullptr;
    const ON_CensusCounter::Class c = (ON_CensusCounter::Class)i;
    switch (c)
    {
    case ON_CensusCounter::Class::subd:
      sClassName = "ON_SubD";
      break;
    case ON_CensusCounter::Class::subd_impl:
      sClassName = "ON_SubDimple";
      break;
    case ON_CensusCounter::Class::subd_limit_mesh:
      sClassName = "ON_SubDMesh";
      break;
    case ON_CensusCounter::Class::subd_limit_mesh_impl:
      sClassName = "ON_SubDMeshImpl";
      break;
    case ON_CensusCounter::Class::subd_ref:
      sClassName = "ON_SubDef";
      break;
    default:
      sClassName = "Bug in ON_CensusCounter";
      break;
    }

    for (unsigned int j = 0; j < ON_PointerHashTable::hash_count; j++)
    {
      for (ON_PointerHashElement* e = ht->m_table[i][j]; nullptr != e; e = e->m_next)
      {
        const unsigned int sn = (unsigned int)e->m_sn;

        if (bPrintClassName)
        {
          text_log.Print("\n\n%s census:\n",sClassName);
          bPrintClassName = false;
        }

        switch (c)
        {
        case ON_CensusCounter::Class::subd:
          {
            const ON_SubD* subd = (const ON_SubD*)e->m_ptr;
            if ( subd == &ON_SubD::Empty )
              text_log.Print("ON_SubD::Empty (%u) ", sn);
            else
              text_log.Print("ON_SubD(%u) ", sn);
            const ON_SubDimple* dimple = subd->SubDimple();
            if ( nullptr == dimple )
              text_log.Print(" ON_SubDimple(nullptr)\n");
            else
            {
              unsigned int dimple_sn = (unsigned int)ht->SerialNumber(ON_CensusCounter::Class::subd_impl, (ON__UINT_PTR)dimple);
              text_log.Print(" ON_SubDimple(%u)x%u\n", dimple_sn, subd->SubDimpleUseCount());
            }            
          }
          break;

        case ON_CensusCounter::Class::subd_impl:
          {
           text_log.Print("ON_SubDimple(%u)\n", sn);
          }
          break;

        case ON_CensusCounter::Class::subd_limit_mesh:
          {
            const ON_SubDMesh* subd_limit_mesh = (const ON_SubDMesh*)e->m_ptr;
            if ( subd_limit_mesh == &ON_SubDMesh::Empty )
              text_log.Print("ON_SubDMesh::Empty (%u) ", sn);
            else
              text_log.Print("ON_SubDMesh(%u) ", sn);
            const class ON_SubDMeshImpl* limple = subd_limit_mesh->SubLimple();
            if ( nullptr == limple )
              text_log.Print(" ON_SubDMeshImpl(nullptr)\n");
            else
            {
              unsigned int limple_sn = (unsigned int)ht->SerialNumber(ON_CensusCounter::Class::subd_limit_mesh_impl, (ON__UINT_PTR)limple);
              text_log.Print(" ON_SubDMeshImpl(%u)x%u\n", limple_sn, subd_limit_mesh->SubLimpleUseCount());
            }            
          }
          break;

        case ON_CensusCounter::Class::subd_limit_mesh_impl:
          {
            const ON_SubDMeshImpl* subd_limple = (const ON_SubDMeshImpl*)e->m_ptr;
            text_log.Print("ON_SubDMeshImpl(%u)", sn);

            std::shared_ptr<ON_SubDimple> limple_sp(subd_limple->m_subdimple_wp.lock());
            const ON_SubDimple* dimple = limple_sp.get();
            if ( nullptr == dimple )
              text_log.Print(" ON_SubDimple(nullpr)\n");
            else
            {
              unsigned int dimple_sn = (unsigned int)ht->SerialNumber(ON_CensusCounter::Class::subd_impl, (ON__UINT_PTR)dimple);
              text_log.Print(" ON_SubDimple(%u)x%u+1\n", dimple_sn, (unsigned int)(limple_sp.use_count()-1));
            }            

          }
          break;

        case ON_CensusCounter::Class::subd_ref:
          {
            const ON_SubDRef* subd_ref = (const ON_SubDRef*)e->m_ptr;
            const ON_SubD* subd = &subd_ref->SubD();
            ON__UINT_PTR subd_sn = ht->SerialNumber(ON_CensusCounter::Class::subd, (ON__UINT_PTR)subd);
            text_log.Print("ON_SubDRef(%u) ON_SubD(%u)x%u\n", sn, subd_sn, subd_ref->ReferenceCount());
          }
          break;
        }
      }
    }
  }
}


static ON__UINT_PTR ON_SubDCensusCounter_This(ON_SubDCensusCounter* p)
{
  const ON_SubD* pSubD = (const ON_SubD*)((unsigned char*)p - sizeof(ON_Geometry));
  return (ON__UINT_PTR)pSubD;
}

ON_SubDCensusCounter::ON_SubDCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd,ON_SubDCensusCounter_This(this));
}

ON_SubDCensusCounter::~ON_SubDCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterDeath(ON_CensusCounter::Class::subd,ON_SubDCensusCounter_This(this));
}

ON_SubDCensusCounter::ON_SubDCensusCounter(const ON_SubDCensusCounter&) ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd,ON_SubDCensusCounter_This(this));
}

ON_SubDRefCensusCounter::ON_SubDRefCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_ref,(ON__UINT_PTR)this);
}

ON_SubDRefCensusCounter::~ON_SubDRefCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterDeath(ON_CensusCounter::Class::subd_ref,(ON__UINT_PTR)this);
}

ON_SubDRefCensusCounter::ON_SubDRefCensusCounter(const ON_SubDRefCensusCounter&) ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_ref,(ON__UINT_PTR)this);
}


ON_SubDImpleCensusCounter::ON_SubDImpleCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_impl,(ON__UINT_PTR)this);
}

ON_SubDImpleCensusCounter::~ON_SubDImpleCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterDeath(ON_CensusCounter::Class::subd_impl,(ON__UINT_PTR)this);
}

ON_SubDImpleCensusCounter::ON_SubDImpleCensusCounter(const ON_SubDImpleCensusCounter&) ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_impl,(ON__UINT_PTR)this);
}



ON_SubDMeshCensusCounter::ON_SubDMeshCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_limit_mesh,(ON__UINT_PTR)this);
}

ON_SubDMeshCensusCounter::~ON_SubDMeshCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterDeath(ON_CensusCounter::Class::subd_limit_mesh,(ON__UINT_PTR)this);
}

ON_SubDMeshCensusCounter::ON_SubDMeshCensusCounter(const ON_SubDMeshCensusCounter&) ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_limit_mesh,(ON__UINT_PTR)this);
}




ON_SubDMeshCensusCounter::ON_SubDMeshCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_limit_mesh_impl,(ON__UINT_PTR)this);
}

ON_SubDMeshCensusCounter::~ON_SubDMeshCensusCounter() ON_NOEXCEPT
{
  ON_CensusCounter::RegisterDeath(ON_CensusCounter::Class::subd_limit_mesh_impl,(ON__UINT_PTR)this);
}

ON_SubDMeshCensusCounter::ON_SubDMeshCensusCounter(const ON_SubDMeshCensusCounter&) ON_NOEXCEPT
{
  ON_CensusCounter::RegisterBirth(ON_CensusCounter::Class::subd_limit_mesh_impl,(ON__UINT_PTR)this);
}

#endif

