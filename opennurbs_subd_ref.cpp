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

#if defined(OPENNURBS_SUBD_WIP)

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDRef
//

ON_SubDRef::ON_SubDRef() ON_NOEXCEPT
{}

ON_SubDRef::~ON_SubDRef()
{
  m_subd_sp.reset();
}

ON_SubDRef::ON_SubDRef(const ON_SubDRef& src) ON_NOEXCEPT
  : m_subd_sp(src.m_subd_sp)
{}

ON_SubDRef& ON_SubDRef::operator=(const ON_SubDRef& src)
{
  if ( this != &src )
    m_subd_sp = src.m_subd_sp;
  return *this;
}

#if defined(ON_HAS_RVALUEREF)
// rvalue copy constructor
ON_SubDRef::ON_SubDRef( ON_SubDRef&& src) ON_NOEXCEPT
  : m_subd_sp(std::move(src.m_subd_sp))
{}

// rvalue assignment operator
ON_SubDRef& ON_SubDRef::operator=(ON_SubDRef&& src)
{
  m_subd_sp.reset();
  m_subd_sp = std::move(src.m_subd_sp);
  return *this;
}
#endif

const class ON_SubD& ON_SubDRef::SubD() const
{
  const ON_SubD* subd = m_subd_sp.get();
  if ( nullptr == subd )
    subd = &ON_SubD::Empty;
  return *subd;
}

unsigned int ON_SubDRef::ReferenceCount() const
{
  return (unsigned int)m_subd_sp.use_count();
}

void ON_SubDRef::Clear()
{
  m_subd_sp.reset();
}

class ON_SubD& ON_SubDRef::NewSubD()
{
  ON_SubD* subd = new ON_SubD();
  ON_SubD* managed_subd = SetSubDForExperts(subd);
  return *managed_subd;
}

class ON_SubD& ON_SubDRef::CopySubD(
  const ON_SubDRef& src
  )
{
  return CopySubD(src.SubD());
}

class ON_SubD& ON_SubDRef::CopySubD(
  const ON_SubD& src
  )
{
  ON_SubD* subd_copy = new ON_SubD(src);
  ON_SubD* managed_subd = SetSubDForExperts(subd_copy);
  return *managed_subd;
}

class ON_SubD& ON_SubDRef::UniqueSubD()
{
  const ON_SubD& subd = SubD();
  if (m_subd_sp.use_count() > 1 )
    return CopySubD(subd);
  if (subd.m_subdimple_sp.use_count() > 1)
    return CopySubD(subd);
  return const_cast< ON_SubD& >(subd);
}

class ON_SubD* ON_SubDRef::SetSubDForExperts(
  class ON_SubD*& subd
  )
{
  Clear();
  ON_SubD* managed_subd = ( subd == &ON_SubD::Empty ) ? nullptr : subd;
  subd = nullptr;
  if (nullptr != managed_subd )
    m_subd_sp = std::shared_ptr<class ON_SubD>(managed_subd);
  return managed_subd;
}

ON_SubDRef::ON_SubDRef(
  const ON_SubD& subd
  )
{
  const ON_SubDimple* subdimple = subd.SubDimple();
  if (nullptr != subdimple)
  {
    ON_SubD* managed_subd = new ON_SubD();
    managed_subd->ShareDimple(subd);
    this->SetSubDForExperts(managed_subd);
    if (nullptr != managed_subd)
      delete managed_subd;
  }
}

ON_SubDRef ON_SubDRef::CreateReferenceForExperts(
  const ON_SubD& subd
  )
{
  ON_SubDRef subd_ref(subd);
  return subd_ref;
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentRef
//

ON_OBJECT_IMPLEMENT(ON_SubDComponentRef,ON_Geometry,"C221FC6D-36B7-47E8-90AA-AC8EC784E3DD");

ON_SubDComponentRef::ON_SubDComponentRef(const ON_SubDComponentRef& src) ON_NOEXCEPT
  : ON_Geometry(src)
  , m_subd_ref(src.m_subd_ref)
  , m_component_ptr(src.m_component_ptr)
  , m_component_index(src.m_component_index)
{}

ON_SubDComponentRef& ON_SubDComponentRef::operator=(const ON_SubDComponentRef& src)
{
  if (this != &src)
  {
    ON_Geometry::operator=(src);
    m_subd_ref = src.m_subd_ref;
    m_component_ptr = src.m_component_ptr;
    m_component_index = src.m_component_index;
  }
  return *this;
}

#if defined(ON_HAS_RVALUEREF)
ON_SubDComponentRef::ON_SubDComponentRef( ON_SubDComponentRef&& src ) ON_NOEXCEPT
  : ON_Geometry(std::move(src))
  , m_subd_ref(std::move(src.m_subd_ref))
  , m_component_ptr(src.m_component_ptr)
  ,  m_component_index(src.m_component_index)
{}

ON_SubDComponentRef& ON_SubDComponentRef::operator=(ON_SubDComponentRef&& src)
{
  if ( this != &src )
  {
    Clear();
    ON_Geometry::operator=(std::move(src));
    m_subd_ref = std::move(src.m_subd_ref);
    m_component_ptr = src.m_component_ptr;
    m_component_index = src.m_component_index;
  }
  return *this;
}

#endif

ON_SubDComponentRef ON_SubDComponentRef::Create(
  const ON_SubDRef& subd_ref,
  ON_COMPONENT_INDEX component_index,
  ON_SubDComponentLocation component_location
  )
{
  ON_SubDComponentPtr component_ptr = subd_ref.SubD().ComponentPtrFromComponentIndex(component_index);
  return ON_SubDComponentRef::Create(subd_ref,component_ptr,component_location);
}

ON_SubDComponentRef ON_SubDComponentRef::Create(
  const ON_SubDRef& subd_ref,
  ON_SubDComponentPtr component_ptr,
  ON_SubDComponentLocation component_location
  )
{
  ON_SubDComponentRef component_ref;
  component_ref.m_subd_ref = subd_ref;
  bool bValidInput = false;
  switch (component_ptr.ComponentType())
  {
  case ON_SubDComponentPtr::ComponentPtrType::vertex:
    {
      const ON_SubDVertex* vertex = ON_SUBD_VERTEX_POINTER(component_ptr.m_ptr);
      if (nullptr != vertex && vertex->m_id > 0 && vertex->m_id < ON_UNSET_UINT_INDEX)
      {
        component_ref.m_component_ptr = component_ptr;
        component_ref.m_component_index.Set(ON_COMPONENT_INDEX::TYPE::subd_vertex,(int)vertex->m_id);
        component_ref.m_component_location = component_location;
        bValidInput = true;
      }
    }
    break;
  case ON_SubDComponentPtr::ComponentPtrType::edge:
    {
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(component_ptr.m_ptr);
      if (nullptr != edge && edge->m_id > 0 && edge->m_id < ON_UNSET_UINT_INDEX)
      {
        component_ref.m_component_ptr = component_ptr;
        component_ref.m_component_index.Set(ON_COMPONENT_INDEX::TYPE::subd_edge,(int)edge->m_id);
        component_ref.m_component_location = component_location;
        bValidInput = true;
      }
    }
    break;
  case ON_SubDComponentPtr::ComponentPtrType::face:
    {
      const ON_SubDFace* face = ON_SUBD_FACE_POINTER(component_ptr.m_ptr);
      if (nullptr != face && face->m_id > 0 && face->m_id < ON_UNSET_UINT_INDEX)
      {
        component_ref.m_component_ptr = component_ptr;
        component_ref.m_component_index.Set(ON_COMPONENT_INDEX::TYPE::subd_face,(int)face->m_id);
        component_ref.m_component_location = component_location;
        bValidInput = true;
      }
    }
    break;
  default:
    if ( component_ptr.IsNull() )
      bValidInput = true;
  }

  if (bValidInput)
  {
    return component_ref;
  }

  return ON_SUBD_RETURN_ERROR(component_ref);
}

ON_SubDComponentRef ON_SubDComponentRef::Create(
  const ON_SubDRef& subd_ref,
  const class ON_SubDVertex* vertex,
  ON_SubDComponentLocation component_location
  )
{
  return ON_SubDComponentRef::Create(subd_ref, ON_SubDComponentPtr::Create(vertex),component_location);
}

ON_SubDComponentRef ON_SubDComponentRef::Create(
  const ON_SubDRef& subd_ref,
  const class ON_SubDEdge* edge,
  ON_SubDComponentLocation component_location
  )
{
  return ON_SubDComponentRef::Create(subd_ref, ON_SubDComponentPtr::Create(edge),component_location);
}

ON_SubDComponentRef ON_SubDComponentRef::Create(
  const ON_SubDRef& subd_ref,
  const class ON_SubDFace* face,
  ON_SubDComponentLocation component_location
  )
{
  return ON_SubDComponentRef::Create(subd_ref, ON_SubDComponentPtr::Create(face),component_location);
}

void ON_SubDComponentRef::Clear()
{
  ON_Geometry::DestroyRuntimeCache();
  PurgeUserData();
  m_subd_ref.Clear();
  m_component_ptr = ON_SubDComponentPtr::Null;
  m_component_index = ON_COMPONENT_INDEX::UnsetComponentIndex;
}

ON_SubDRef ON_SubDComponentRef::SubDRef() const
{
  return m_subd_ref;
}

const class ON_SubD& ON_SubDComponentRef::SubD() const
{
  return m_subd_ref.SubD();
}

ON_COMPONENT_INDEX ON_SubDComponentRef::ComponentIndex() const
{
  return m_component_index;
}

ON_SubDComponentPtr ON_SubDComponentRef::ComponentPtr() const
{
  return m_component_ptr;
}

ON_SubDComponentLocation ON_SubDComponentRef::ComponentLocation() const
{
  return m_component_location;
}

const class ON_SubDVertex* ON_SubDComponentRef::Vertex() const
{
  return m_component_ptr.Vertex();
}

const class ON_SubDEdge* ON_SubDComponentRef::Edge() const
{
  return m_component_ptr.Edge();
}

const class ON_SubDFace* ON_SubDComponentRef::Face() const
{
  return m_component_ptr.Face();
}

bool ON_SubDComponentRef::IsValid(ON_TextLog* text_log) const
{
  return (
    m_component_ptr.IsNotNull() 
    && (ON_SubDComponentLocation::ControlNet == m_component_location || ON_SubDComponentLocation::LimitSurface == m_component_location)
    && false == SubD().IsEmpty()
    );
}

void ON_SubDComponentRef::Dump(ON_TextLog& text_log) const
{
  return;
}

unsigned int ON_SubDComponentRef::SizeOf() const
{
  size_t sz = ON_Geometry::SizeOf() + sizeof(*this) - sizeof(ON_Geometry);
  return (unsigned int)sz;
}

ON::object_type ON_SubDComponentRef::ObjectType() const
{
  return ON::object_type::subd_object;
}

// overrides of virtual ON_Geometry functions
int ON_SubDComponentRef::Dimension() const
{
  return 3;
}

bool ON_SubDComponentRef::GetBBox(
  double* boxmin,
  double* boxmax,
  bool bGrowBox
  ) const
{
  if ( nullptr == boxmin || nullptr == boxmax )
    return false;

  ON_BoundingBox bbox = ON_BoundingBox::EmptyBoundingBox;

  switch (m_component_ptr.ComponentType())
  {
  case ON_SubDComponentPtr::ComponentPtrType::vertex:
    {
      const ON_SubDVertex* vertex = m_component_ptr.Vertex();
      if ( nullptr == vertex )
        break;
      switch (m_component_location)
      {
      case ON_SubDComponentLocation::ControlNet:
        bbox = vertex->ControlNetBoundingBox();
        break;
      case ON_SubDComponentLocation::LimitSurface:
        bbox = vertex->LimitSurfaceBoundingBox(SubD());
        break;
      }
    }
    break;
  case ON_SubDComponentPtr::ComponentPtrType::edge:
    {
      const ON_SubDEdge* edge = m_component_ptr.Edge();
      if ( nullptr == edge )
        break;
      switch (m_component_location)
      {
      case ON_SubDComponentLocation::ControlNet:
        bbox = edge->ControlNetBoundingBox();
        break;
      case ON_SubDComponentLocation::LimitSurface:
        bbox = edge->LimitSurfaceBoundingBox(SubD());
        break;
      }
    }
    break;
  case ON_SubDComponentPtr::ComponentPtrType::face:
    {
      const ON_SubDFace* face = m_component_ptr.Face();
      if ( nullptr == face )
        break;
      switch (m_component_location)
      {
      case ON_SubDComponentLocation::ControlNet:
        bbox = face->ControlNetBoundingBox();
        break;
      case ON_SubDComponentLocation::LimitSurface:
        bbox = face->LimitSurfaceBoundingBox(SubD());
        break;
      }
    }
    break;
  }

  if (bGrowBox)
  {
    ON_BoundingBox bbox1;
    bbox1.m_min = boxmin;
    bbox1.m_max = boxmax;
    if (bbox1.IsValid())
      bbox.Union(bbox1);
  }

  boxmin[0] = bbox.m_min.x;
  boxmin[1] = bbox.m_min.y;
  boxmin[2] = bbox.m_min.z;

  boxmax[0] = bbox.m_max.x;
  boxmax[1] = bbox.m_max.y;
  boxmax[2] = bbox.m_max.z;

  return bbox.IsValid();
}


#endif
