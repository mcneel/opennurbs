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

#if defined(OPENNURBS_SUBD_WIP)

unsigned int ON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(ON__UINT_PTR ptr)
{
  return (unsigned int)(ptr/(ON_SUBD_ELEMENT_FLAGS_MASK+1));
}

ON_SubDComponentPtr ON_SubDArchiveIdMap::FromVertex(
  const ON_SubDVertex* vertex
  )
{
  ON__UINT_PTR archive_id = (nullptr == vertex) ? 0 : vertex->ArchiveId();
  return ON_SubDComponentPtr::Create((const ON_SubDVertex*)(archive_id*(ON_SUBD_ELEMENT_FLAGS_MASK+1)));
}

ON_SubDComponentPtr ON_SubDArchiveIdMap::FromEdge(
  const ON_SubDEdge* edge
  )
{
  ON__UINT_PTR archive_id = (nullptr == edge) ? 0 : edge->ArchiveId();
  return ON_SubDComponentPtr::Create((const ON_SubDEdge*)(archive_id*(ON_SUBD_ELEMENT_FLAGS_MASK+1)));
}

ON_SubDComponentPtr ON_SubDArchiveIdMap::FromFace(
  const ON_SubDFace* face
  )
{
  ON__UINT_PTR archive_id = (nullptr == face) ? 0 : face->ArchiveId();
  return ON_SubDComponentPtr::Create((const ON_SubDFace*)(archive_id*(ON_SUBD_ELEMENT_FLAGS_MASK+1)));
}

ON_SubDComponentPtr ON_SubDArchiveIdMap::FromVertex(
  ON_SubDVertexPtr vertex_ptr
  )
{
  ON_SubDComponentPtr ptr = FromVertex(vertex_ptr.Vertex());
  ptr.m_ptr |= vertex_ptr.VertexPtrMark();
  return ptr;
}

ON_SubDComponentPtr ON_SubDArchiveIdMap::FromEdge(
  ON_SubDEdgePtr edge_ptr
  )
{
  ON_SubDComponentPtr ptr = FromEdge(edge_ptr.Edge());
  ptr.m_ptr |= edge_ptr.EdgeDirection();
  return ptr;
}

ON_SubDComponentPtr ON_SubDArchiveIdMap::FromFace(
  ON_SubDFacePtr face_ptr
  )
{
  ON_SubDComponentPtr ptr = FromFace(face_ptr.Face());
  ptr.m_ptr |= face_ptr.FaceDirection();
  return ptr;
}

ON_SubDVertex* ON_SubDArchiveIdMap::CopyVertex(
  const ON_SubDVertex* source_vertex,
  class ON_SubDimple& subdimple
  )
{
  if ( nullptr == source_vertex )
    return ON_SUBD_RETURN_ERROR(nullptr);
  ON_SubDVertex* vertex = subdimple.AllocateVertex(
    source_vertex->m_vertex_tag,
    source_vertex->m_level,
    source_vertex->m_P,
    source_vertex->m_edge_count,
    source_vertex->m_face_count
    );
  if ( nullptr == vertex )
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  const bool bCopyEdgeArray = true;
  const bool bCopyFaceArray = true;
  const bool bCopyLimitPointList = true;
  vertex->CopyFrom(source_vertex,bCopyEdgeArray,bCopyFaceArray,bCopyLimitPointList);

  // convert vertex->m_edges[] pointers to archive_id values
  ON_SubDComponentPtr ptr;
  for (unsigned int vei = 0; vei < vertex->m_edge_count; vei++)
  {
    ptr = ON_SubDArchiveIdMap::FromEdge(vertex->m_edges[vei]);
    vertex->m_edges[vei].m_ptr = ptr.m_ptr;
  }

  // convert vertex->m_faces[] pointers to archive_id values
  for (unsigned int vfi = 0; vfi < vertex->m_face_count; vfi++)
  {
    ptr = ON_SubDArchiveIdMap::FromFace(vertex->m_faces[vfi]);
    vertex->m_faces[vfi] = (const ON_SubDFace*)ptr.m_ptr;
  }

  // convert vertex->m_limit_point[].m_sector_face pointers to archive_id values
  for (const ON_SubDSectorLimitPoint* p = &vertex->m_limit_point; nullptr != p; p = p->m_next_sector_limit_point)
  {
    ptr = ON_SubDArchiveIdMap::FromFace(p->m_sector_face);
    const_cast<ON_SubDSectorLimitPoint*>(p)->m_sector_face = (const ON_SubDFace*)ptr.m_ptr;
  }
  
  return vertex;
}

ON_SubDEdge* ON_SubDArchiveIdMap::CopyEdge(
  const ON_SubDEdge* source_edge,
  class ON_SubDimple& subdimple
  )
{
  if ( nullptr == source_edge )
    return ON_SUBD_RETURN_ERROR(nullptr);
  ON_SubDEdge* edge = subdimple.AllocateEdge(
    source_edge->m_edge_tag,
    source_edge->m_level,
    source_edge->m_face_count);
  if ( nullptr == edge )
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  const bool bReverseEdge = false;
  const bool bCopyVertexArray = true;
  const bool bCopyEdgeArray = true;
  edge->CopyFrom(source_edge,bReverseEdge,bCopyVertexArray,bCopyEdgeArray);

  // convert edge->m_vertex[] pointers to archive_id values
  ON_SubDComponentPtr ptr;
  for (unsigned int evi = 0; evi < 2; evi++)
  {
    ptr = ON_SubDArchiveIdMap::FromVertex(edge->m_vertex[evi]);
    edge->m_vertex[evi] = (const ON_SubDVertex*)ptr.m_ptr;
  }

  // convert edge->m_faces[] pointers to archive_id values
  ON_SubDFacePtr* fptr = edge->m_face2;
  for (unsigned int efi = 0; efi < edge->m_face_count; efi++, fptr++)
  {
    if (2 == efi)
      fptr = edge->m_facex;
    fptr->m_ptr = ON_SubDArchiveIdMap::FromFace(*fptr).m_ptr;
  }

  return edge;
}

ON_SubDFace* ON_SubDArchiveIdMap::CopyFace(
  const ON_SubDFace* source_face,
  class ON_SubDimple& subdimple
  )
{
  if ( nullptr == source_face )
    return ON_SUBD_RETURN_ERROR(nullptr);
  ON_SubDFace* face = subdimple.AllocateFace(source_face->m_level,source_face->m_edge_count);
  if ( nullptr == face )
    return ON_SUBD_RETURN_ERROR(nullptr);
  
  const bool bCopyEdgeArray = true;
  face->CopyFrom(source_face,bCopyEdgeArray);

  // convert face->m_edges[] pointers to archive_id values
  ON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned int fei = 0; fei < face->m_edge_count; fei++, eptr++)
  {
    if (4 == fei)
      eptr = face->m_edgex;
    eptr->m_ptr = ON_SubDArchiveIdMap::FromEdge(*eptr).m_ptr;
  }

  return face;
}
  
bool ON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeVertexPtr(
  unsigned int vertex_count,
  size_t vertex_capacity,
  ON_SubDVertex** vertex
  )
{
  if ( 0 == vertex_count )
    return true;
  if ( 0 == vertex_capacity || nullptr == vertex )
    return ON_SUBD_RETURN_ERROR(false);
  if ( vertex_count > vertex_capacity )
    return ON_SUBD_RETURN_ERROR(false);
  for (unsigned int i = 0; i < vertex_count; i++)
  {
    ON__UINT_PTR vptr = (ON__UINT_PTR)(vertex[i]);
    vertex[i] = nullptr;
    const unsigned int archive_id = ON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(vptr);
    // future use // ON__UINT_PTR flags = ON_SUBD_ELEMENT_FLAGS(vptr);
    if (0 == archive_id || archive_id < m_archive_id_partition[0] || archive_id >= m_archive_id_partition[1])
    {
      ON_ERROR("Invalid vertex archive id.");
      continue;
    }
    const ON_SubDComponentPtr* eleptr = ComponentPtrFromArchiveId(archive_id);
    if (nullptr == eleptr)
    {
      ON_ERROR("null element pointer.");
      continue;
    }
    ON_SubDVertex* v = eleptr->Vertex();
    if (nullptr == v)
    {
      ON_ERROR("null vertex pointer.");
      continue;
    }
    if (archive_id != v->ArchiveId())
    {
      ON_ERROR("archive_id != v->ArchiveId().");
      continue;
    }
    vertex[i] = v;
  }
  return true;
}


bool ON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeEdgePtr(
  unsigned int edge_count,
  size_t edgeN_capacity,
  ON_SubDEdgePtr* edgeN,
  unsigned int edgeX_capacity,
  ON_SubDEdgePtr* edgeX
  )
{
  if ( 0 == edge_count )
    return true;
  if ( 0 == edgeN_capacity || nullptr == edgeN )
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 != edgeX_capacity && nullptr == edgeX )
    return ON_SUBD_RETURN_ERROR(false);
  if ( edge_count > edgeN_capacity + edgeX_capacity )
    return ON_SUBD_RETURN_ERROR(false);
  ON_SubDEdgePtr* eptr = edgeN;
  for (unsigned int i = 0; i < edge_count; i++, eptr++)
  {
    if ( i == edgeN_capacity )
      eptr = edgeX;
    const unsigned int archive_id = ON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(eptr->m_ptr);
    ON__UINT_PTR flags = ON_SUBD_ELEMENT_FLAGS(eptr->m_ptr);
    eptr->m_ptr = 0;
    if (0 == archive_id || archive_id < m_archive_id_partition[1] || archive_id >= m_archive_id_partition[2])
    {
      ON_ERROR("Invalid edge archive id.");
      continue;
    }
    const ON_SubDComponentPtr* eleptr = ComponentPtrFromArchiveId(archive_id);
    if (nullptr == eleptr)
    {
      ON_ERROR("null element pointer.");
      continue;
    }
    ON_SubDEdge* edge = eleptr->Edge();
    if (nullptr == edge)
    {
      ON_ERROR("null edge pointer.");
      continue;
    }
    if (archive_id != edge->ArchiveId())
    {
      ON_ERROR("archive_id != edge->ArchiveId().");
      continue;
    }
    *eptr = ON_SubDEdgePtr::Create(edge,ON_SUBD_ELEMENT_MARK(flags));
  }
  return true;
}

bool ON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeFacePtr(
  unsigned int face_count,
  size_t faceN_capacity,
  ON_SubDFacePtr* faceN,
  unsigned int faceX_capacity,
  ON_SubDFacePtr* faceX
  )
{
  if ( 0 == face_count )
    return true;
  if ( 0 == faceN_capacity || nullptr == faceN )
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 != faceX_capacity && nullptr == faceX )
    return ON_SUBD_RETURN_ERROR(false);
  if ( face_count > faceN_capacity + faceX_capacity )
    return ON_SUBD_RETURN_ERROR(false);
  ON_SubDFacePtr* fptr = faceN;
  for (unsigned int i = 0; i < face_count; i++, fptr++)
  {
    if ( i == faceN_capacity )
      fptr = faceX;
    const unsigned int archive_id = ON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(fptr->m_ptr);
    ON__UINT_PTR flags = ON_SUBD_ELEMENT_FLAGS(fptr->m_ptr);
    fptr->m_ptr = 0;
    if (0 == archive_id || archive_id < m_archive_id_partition[2] || archive_id >= m_archive_id_partition[3])
    {
      ON_ERROR("Invalid face archive id.");
      continue;
    }
    const ON_SubDComponentPtr* eleptr = ComponentPtrFromArchiveId(archive_id);
    if (nullptr == eleptr)
    {
      ON_ERROR("null element pointer.");
      continue;
    }
    ON_SubDFace* face = eleptr->Face();
    if (nullptr == face)
    {
      ON_ERROR("null face pointer.");
      continue;
    }
    if (archive_id != face->ArchiveId())
    {
      ON_ERROR("archive_id != face->ArchiveId().");
      continue;
    }
    *fptr = ON_SubDFacePtr::Create(face,ON_SUBD_ELEMENT_MARK(flags));
  }
  return true;
}
void ON_SubDArchiveIdMap::ValidateArrayCounts(
  unsigned short& array_count,
  size_t arrayN_capacity,
  const void* arrayN,
  unsigned short arrayX_capacity,
  const void* arrayX
  )
{
  if (arrayN_capacity >= 0xFFFFU)
  {
    ON_ERROR("Invalid arrayN_capacity.");
    arrayN_capacity = 0xFFFFU;
  }

  unsigned short arrayN_cap = (unsigned short)arrayN_capacity;
  if ( array_count > arrayN_cap + arrayX_capacity 
    || (0 == arrayN_cap && 0 != arrayX_capacity)
    || (0 != arrayN_cap && nullptr == arrayN)
    || (0 != arrayX_capacity && nullptr == arrayX)
    )
  {
    ON_ERROR("Invalid array counts.");
    if ( nullptr == arrayN )
      arrayN_cap = 0;
    if ( 0 == arrayN_cap || nullptr == arrayX )
      arrayX_capacity = 0;
    if (array_count > arrayN_cap + arrayX_capacity )
      array_count = arrayN_cap + arrayX_capacity;
  }
}

ON_SubDArchiveIdMap::ON_SubDArchiveIdMap()
{
  m_fsp.Create(sizeof(ON_SubDComponentPtr),0,0);
  m_archive_id_partition[0] = 0;
  m_archive_id_partition[1] = 0;
  m_archive_id_partition[2] = 0;
  m_archive_id_partition[3] = 0;
}

bool ON_SubDArchiveIdMap::Reset()
{
  m_fsp.ReturnAll();
  m_element_index = ON_UNSET_UINT_INDEX;
  m_element_count = 0;
  m_archive_id_partition[0] = 0;
  m_archive_id_partition[1] = 0;
  m_archive_id_partition[2] = 0;
  m_archive_id_partition[3] = 0;
  return AddComponentPtr(ON_SubDComponentPtr::Null,0);
}

unsigned int ON_SubDArchiveIdMap::Count()
{
  return m_element_count;
}

const ON_SubDComponentPtr* ON_SubDArchiveIdMap::First()
{
  m_fsp_it.Create(&m_fsp);
  m_element_index = 0;
  const ON_SubDComponentPtr* p = (const ON_SubDComponentPtr*)m_fsp_it.FirstElement();
  if (nullptr != p)
    m_element_index++;
  return p;
}

const ON_SubDComponentPtr* ON_SubDArchiveIdMap::Next()
{
  if ( ON_UNSET_UINT_INDEX == m_element_index)
    return First();
  const ON_SubDComponentPtr* p = (const ON_SubDComponentPtr*)m_fsp_it.NextElement();
  if (nullptr != p)
    m_element_index++;
  return p;
}

bool ON_SubDArchiveIdMap::Add(const ON_SubDVertex* vertex)
{
  return AddComponentPtr(ON_SubDComponentPtr::Create(vertex,0),vertex ? vertex->ArchiveId() : 0);
}

bool ON_SubDArchiveIdMap::Add(const ON_SubDEdge* edge)
{
  return AddComponentPtr(ON_SubDComponentPtr::Create(edge,0),edge ? edge->ArchiveId() : 0);
}

bool ON_SubDArchiveIdMap::Add(const ON_SubDFace* face)
{
  return AddComponentPtr(ON_SubDComponentPtr::Create(face,0),face ? face->ArchiveId() : 0);
}


class ON_SubDVertex* ON_SubDArchiveIdMap::AddCopy(const class ON_SubDVertex* source_vertex, class ON_SubDimple& subdimple)
{
  ON_SubDVertex* vertex = ON_SubDArchiveIdMap::CopyVertex(source_vertex,subdimple);
  vertex->SetArchiveId(source_vertex->ArchiveId());
  Add(vertex);
  return vertex;
}

class ON_SubDEdge* ON_SubDArchiveIdMap::AddCopy(const class ON_SubDEdge* source_edge, class ON_SubDimple& subdimple)
{
  ON_SubDEdge* edge = ON_SubDArchiveIdMap::CopyEdge(source_edge,subdimple);
  edge->SetArchiveId(source_edge->ArchiveId());
  Add(edge);
  return edge;
}

class ON_SubDFace* ON_SubDArchiveIdMap::AddCopy(const class ON_SubDFace* source_face, class ON_SubDimple& subdimple)
{
  ON_SubDFace* face = ON_SubDArchiveIdMap::CopyFace(source_face,subdimple);
  face->SetArchiveId(source_face->ArchiveId());
  Add(face);
  return face;
}

const ON_SubDComponentPtr* ON_SubDArchiveIdMap::ComponentPtrFromArchiveId(
  unsigned int archive_id
  ) const
{
  return (const ON_SubDComponentPtr*)m_fsp.Element(archive_id);
}


bool ON_SubDArchiveIdMap::AddComponentPtr(ON_SubDComponentPtr eptr, unsigned int archive_id)
{
  if (m_element_count != archive_id)
  {
    ON_ERROR("Archive id is not valid and ON_SubD::Read will fail.");
    return false;
  }
  ON_SubDComponentPtr* p = (ON_SubDComponentPtr*)m_fsp.AllocateElement();
  *p = eptr;

#if defined(ON_DEBUG)
  if (0 != archive_id)
  {
    const ON_SubDComponentPtr* p1 = (const ON_SubDComponentPtr*)m_fsp.Element(archive_id);
    unsigned int archive_id1 = 0;
    if (p1 == p)
    {
      switch (p1->ComponentType())
      {
      case ON_SubDComponentPtr::ComponentPtrType::vertex:
        archive_id1 = p1->Vertex()->ArchiveId();
        break;
      case ON_SubDComponentPtr::ComponentPtrType::edge:
        archive_id1 = p1->Edge()->ArchiveId();
        break;
      case ON_SubDComponentPtr::ComponentPtrType::face:
        archive_id1 = p1->Face()->ArchiveId();
        break;
      default:
        ON_ERROR("invalid element type");
        break;
      }
    }
    if (archive_id1 != archive_id)
    {
      // break here and then see what went wrong
      ON_SubDIncrementErrorCount();
      m_fsp.Element(archive_id);
      m_fsp.Element(archive_id);
    }
  }
#endif

  m_element_count++;
  return true;
}

unsigned int ON_SubDArchiveIdMap::ConvertArchiveIdsToRuntimePointers()
{
  // Convert archive_id references to runtime pointers.

  // The first element is ON_SubDComponentPtr::Null.  This is done so the index of the elements
  // in element_list[] is equal to the element's archive_id.
  const ON_SubDComponentPtr* element = First();
  if (nullptr == element || ON_SubDComponentPtr::ComponentPtrType::unset != element->ComponentType())
    return ON_SUBD_RETURN_ERROR(0);
  
  element = Next();

  unsigned int archive_id;

  for (archive_id = m_archive_id_partition[0]; nullptr != element && archive_id < m_archive_id_partition[1]; archive_id++, element = Next())
  {
    ON_SubDVertex* v = element->Vertex();
    if (nullptr == v)
      break;
    if (archive_id != v->ArchiveId())
      break;
    // convert ON_SubDVertex.m_edges[]
    ConvertArchiveIdToRuntimeEdgePtr(v->m_edge_count,v->m_edge_capacity,v->m_edges,0,nullptr);
    // convert ON_SubDVertex.m_faces[]
    ConvertArchiveIdToRuntimeFacePtr(v->m_face_count,v->m_face_capacity,(ON_SubDFacePtr*)v->m_faces,0,nullptr);

    for (const ON_SubDSectorLimitPoint* p = &v->m_limit_point; nullptr != p; p = p->m_next_sector_limit_point)
    {
      if ( 0 != p->m_sector_face )
        ConvertArchiveIdToRuntimeFacePtr(1,1,(ON_SubDFacePtr*)&p->m_sector_face,0,nullptr);
    }
  }
  if ( archive_id != m_archive_id_partition[1] )
    return ON_SUBD_RETURN_ERROR(0);

  for (archive_id = m_archive_id_partition[1]; nullptr != element && archive_id < m_archive_id_partition[2]; archive_id++, element = Next())
  {
    ON_SubDEdge* e = element->Edge();
    if (nullptr == e)
      break;
    if (archive_id != e->ArchiveId())
      break;
    // convert ON_SubDEdge.m_vertex[2]
    ConvertArchiveIdToRuntimeVertexPtr(2,2,(ON_SubDVertex**)(e->m_vertex));
    // convert ON_SubDEdge.m_face2[] and ON_SubDEdge.m_facex[]
    ConvertArchiveIdToRuntimeFacePtr(e->m_face_count,sizeof(e->m_face2)/sizeof(e->m_face2[0]),e->m_face2,e->m_facex_capacity,e->m_facex);
  }
  if ( archive_id != m_archive_id_partition[2] )
    return ON_SUBD_RETURN_ERROR(0);

  for (archive_id = m_archive_id_partition[2]; nullptr != element && archive_id < m_archive_id_partition[3]; archive_id++, element = Next())
  {
    ON_SubDFace* f = element->Face();
    if (nullptr == f)
      break;
    if (archive_id != f->ArchiveId())
      break;
    // convert ON_SubDFace.m_edge4[] and ON_SubDFace.m_edgex[]
    ON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeEdgePtr(f->m_edge_count,sizeof(f->m_edge4)/sizeof(f->m_edge4[0]),f->m_edge4,f->m_edgex_capacity,f->m_edgex);
  }
  if ( archive_id != m_archive_id_partition[3] )
    return ON_SUBD_RETURN_ERROR(0);

  return archive_id;
}

void ON_SubD::ShareContentsFrom(ON_SubD& src_subd)
{
  if (this == &ON_SubD::Empty || &src_subd != &ON_SubD::Empty)
  {
    ON_SubDIncrementErrorCount();
  }
  else if (m_subdimple_sp.get() != src_subd.m_subdimple_sp.get())
  {
    m_subdimple_sp = src_subd.m_subdimple_sp;
  }
}

void ON_SubD::SwapContents(
  ON_SubD& a,
  ON_SubD& b
  )
{
  if (&a == &ON_SubD::Empty || &b == &ON_SubD::Empty)
  {
    ON_SubDIncrementErrorCount();
  }
  else
  {
    std::swap(a.m_subdimple_sp, b.m_subdimple_sp);
  }
}


ON_SubD::ON_SubD(const ON_SubD& src)
  : ON_Geometry(src)
{
  this->CopyHelper(src);
}

ON_SubD& ON_SubD::operator=(const ON_SubD& src)
{
  if ( this != &src )
  {
    this->Destroy();
    this->CopyHelper(src);
  }
  return *this;
}

void ON_SubD::CopyHelper(const ON_SubD& src)
{
  ON_SubDimple* subdimple = nullptr;
  const ON_SubDimple* src_subdimple = src.SubDimple();
  if (nullptr != src_subdimple)
  {
    subdimple = new ON_SubDimple(*src_subdimple);
  }
  m_subdimple_sp = std::shared_ptr<ON_SubDimple>(subdimple);
}

ON_SubDimple::ON_SubDimple(const ON_SubDimple& src)
{
  const bool bCopyComponentStatus = true;

  unsigned level_count = src.m_levels.UnsignedCount();
  for (/*empty init*/; level_count > 0; level_count--)
  {
    const ON_SubDLevel* src_level = src.m_levels[level_count - 1];
    if (nullptr == src_level)
      continue;
    if (nullptr == src_level->m_vertex[0])
      continue;
    if (nullptr == src_level->m_edge[0])
      continue;
    if (nullptr == src_level->m_face[0])
      continue;
    break;
  }

  ON_SubDArchiveIdMap eptrlist;
  m_levels.Reserve(level_count);
  for (unsigned int level_index = 0; level_index < level_count; level_index++)
  {
    const ON_SubDLevel* src_level = src.m_levels[level_index];
    ON_SubDLevel* level = SubDLevel(level_index,true);
    if (nullptr == level)
      break;
    if (false == level->CopyHelper(*src_level, eptrlist, *this, bCopyComponentStatus))
      break;
    if ( src.m_active_level == src_level )
      m_active_level = level;
  }
}

bool ON_SubDLevel::IsEmpty() const
{
  return (
    nullptr == m_vertex[0] 
    || 0U == m_vertex_count 
    || this == &ON_SubDLevel::Empty 
    );
}

bool ON_SubDLevel::CopyHelper(
  const ON_SubDLevel& src,
  class ON_SubDArchiveIdMap& eptrlist,
  class ON_SubDimple& subdimple,
  bool bCopyComponentStatus
  )
{
  bool rc = false;

  eptrlist.Reset();

  m_limit_mesh.Clear();

  for (;;)
  {
    if ( 0 == src.SetArchiveId(eptrlist.m_archive_id_partition) )
      break;

    unsigned int archive_id = 1;
    if ( archive_id != eptrlist.m_archive_id_partition[0])
      break;

    for (const ON_SubDVertex* source_vertex = src.m_vertex[0]; nullptr != source_vertex; source_vertex = source_vertex->m_next_vertex, archive_id++)
    {
      if (archive_id != source_vertex->ArchiveId())
        break;
      ON_SubDVertex* vertex = eptrlist.AddCopy(source_vertex,subdimple);
      if (nullptr == vertex )
        break;
      if (archive_id != vertex->ArchiveId())
        break;
      this->AddVertex(vertex);
      if ( bCopyComponentStatus )
        vertex->m_status = source_vertex->m_status;
    }
    if ( archive_id != eptrlist.m_archive_id_partition[1])
      break;

    for (const ON_SubDEdge* source_edge = src.m_edge[0]; nullptr != source_edge; source_edge = source_edge->m_next_edge, archive_id++)
    {
      if (archive_id != source_edge->ArchiveId())
        break;
      ON_SubDEdge* edge = eptrlist.AddCopy(source_edge,subdimple);
      if (nullptr == edge )
        break;
      if (archive_id != edge->ArchiveId())
        break;
      this->AddEdge(edge);
      if ( bCopyComponentStatus )
        edge->m_status = source_edge->m_status;
    }
    if ( archive_id != eptrlist.m_archive_id_partition[2])
      break;

    for (const ON_SubDFace* source_face = src.m_face[0]; nullptr != source_face; source_face = source_face->m_next_face, archive_id++)
    {
      if (archive_id != source_face->ArchiveId())
        break;
      ON_SubDFace* face = eptrlist.AddCopy(source_face,subdimple);
      if (nullptr == face )
        break;
      if (archive_id != face->ArchiveId())
        break;
      this->AddFace(face);
      if ( bCopyComponentStatus )
        face->m_status = source_face->m_status;
    }
    if ( archive_id != eptrlist.m_archive_id_partition[3])
      break;

    if (0 == eptrlist.ConvertArchiveIdsToRuntimePointers())
      break;

    if (false == src.m_limit_mesh.IsEmpty())
    {
      ON_SubDLimitMesh local_limit_mesh;
      local_limit_mesh.CopyFrom(src.m_limit_mesh);
      for (const ON_SubDLimitMeshFragment* fragment = local_limit_mesh.FirstFragment(); nullptr != fragment; fragment = fragment->m_next_fragment)
      {
        if (nullptr != fragment->m_face)
        {
          archive_id = fragment->m_face->ArchiveId();
          ON_SubDComponentPtr cptr = ON_SubDArchiveIdMap::FromFace(fragment->m_face);
          ON_SubDFacePtr fptr = cptr.FacePtr();
          const_cast< ON_SubDLimitMeshFragment* >(fragment)->m_face = nullptr;
          if (0 != archive_id)
          {
            if (eptrlist.ConvertArchiveIdToRuntimeFacePtr(1, 1, &fptr, 0, nullptr))
            {
              const_cast< ON_SubDLimitMeshFragment* >(fragment)->m_face = fptr.Face();
              if ( nullptr != fragment->m_face)
                continue;
            }
          }
        }
        local_limit_mesh = ON_SubDLimitMesh::Empty;
        break;
      }
      if (false == local_limit_mesh.IsEmpty())
        ON_SubDLimitMesh::Swap(m_limit_mesh,local_limit_mesh);
    }

    this->ClearArchiveId();

    this->m_level_index = src.m_level_index;
    this->m_subdivision_type = src.m_subdivision_type;
    this->m_ordinary_vertex_valence = src.m_ordinary_vertex_valence;
    this->m_ordinary_face_edge_count = src.m_ordinary_face_edge_count;
    this->m_aggregates = src.m_aggregates;
    this->m_aggregates.MarkAllAsNotCurrent();

    rc = true;
    break;
  }

  eptrlist.Reset();
  src.ClearArchiveId();

  if ( false == rc )
    return ON_SUBD_RETURN_ERROR(false);

  return rc;
}

#endif
