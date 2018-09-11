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

const ON_SubDLevel ON_SubDLevel::Empty;

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDimple
//

ON_SubDimple::~ON_SubDimple()
{
  Destroy();
}

void ON_SubDimple::Clear()
{
  m_heap.Clear();
}

void ON_SubDimple::ClearSubdivisionLevels(
  unsigned int max_level_index
  )
{
  if (max_level_index+1 < m_levels.UnsignedCount())
  {
    unsigned int level_count = m_levels.UnsignedCount();
    if (nullptr != m_active_level && m_active_level->m_level_index > max_level_index)
    {
      if ( level_count > max_level_index )
      m_active_level = m_levels[max_level_index];
    }

    while (level_count > max_level_index+1)
    {
      const unsigned int level_index = (level_count-1);
      ON_SubDLevel* level = m_levels[level_index];
      m_levels[level_index] = nullptr;
      m_levels.Remove();
      level_count--;
      if (level_count != m_levels.UnsignedCount())
      {
        Clear();
        return;
      }

      if ( nullptr == level )
        continue;

      level->ResetFaceArray();
      level->ResetEdgeArray();
      level->ResetVertexArray();

      ON_SubDVertex* next_vertex = level->m_vertex[0];
      level->m_vertex[0] = nullptr;
      level->m_vertex[1] = nullptr;

      ON_SubDEdge* next_edge = level->m_edge[0];
      level->m_edge[0] = nullptr;
      level->m_edge[1] = nullptr;

      ON_SubDFace* next_face = level->m_face[0];
      level->m_face[0] = nullptr;
      level->m_face[1] = nullptr;

      for (ON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
      {
        next_vertex = const_cast<ON_SubDVertex*>(vertex->m_next_vertex);
        ReturnVertex(vertex);
      }

      for (ON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
      {
        next_edge = const_cast<ON_SubDEdge*>(edge->m_next_edge);
        ReturnEdge(edge);
      }

      for (ON_SubDFace* face = next_face; nullptr != face; face = next_face)
      {
        next_face = const_cast<ON_SubDFace*>(face->m_next_face);
        ReturnFace(face);
      }

      delete level;
    }
  }
}

void ON_SubDimple::Destroy()
{
  const unsigned int level_count = m_levels.Count();
  for (unsigned int level_index = 0; level_index < level_count; level_index++)
  {
    ON_SubDLevel* level = m_levels[level_index];
    if ( nullptr == level )
      continue;
    m_levels[level_index] = nullptr;
    delete level;
  }
  m_levels.Destroy();
  m_heap.Destroy();
}

ON_SubDLevel* ON_SubDimple::ActiveLevel(bool bCreateIfNeeded)
{
  if (nullptr == m_active_level)
  {
    unsigned int level_index = (m_levels.UnsignedCount() > 0) ? (m_levels.UnsignedCount()-1) : 0U;
    m_active_level = SubDLevel(level_index,bCreateIfNeeded && 0 == m_levels.UnsignedCount());    
  }
  return m_active_level;
}

class ON_SubDLevel* ON_SubDimple::SubDLevel(
  unsigned int level_index,
  bool bCreateIfNeeded
  )
{
  ON_SubDLevel* level = nullptr;
  if (level_index < m_levels.UnsignedCount())
    level = m_levels[level_index];
  else if (bCreateIfNeeded && level_index == m_levels.UnsignedCount())
  {
    level = new ON_SubDLevel();
    level->m_level_index = level_index;
    m_levels.Append(level);
    if ( nullptr == m_active_level )
      m_active_level = level;
  }

  return level;
}

class ON_SubDLevel const * ON_SubDimple::SubDLevel(
  unsigned int level_index
  ) const
{
  if (level_index < m_levels.UnsignedCount())
    return m_levels[level_index];
  return nullptr;
}

void ON_SubDAggregates::UpdateBoundingBox(
  const ON_SubDLevel* level
  )
{
  ON_BoundingBox bbox = ON_BoundingBox::EmptyBoundingBox;
  if (nullptr != level)
  {
    double x;
    for (const ON_SubDVertex* v = level->m_vertex[0]; nullptr != v; v = v->m_next_vertex)
    {
      if (v->m_P[0] == v->m_P[0] && v->m_P[1] == v->m_P[1] && v->m_P[2] == v->m_P[2])
      {
        bbox.m_min.x = v->m_P[0];
        bbox.m_min.y = v->m_P[1];
        bbox.m_min.z = v->m_P[2];
        bbox.m_max.x = bbox.m_min.x;
        bbox.m_max.y = bbox.m_min.y;
        bbox.m_max.z = bbox.m_min.z;
        for (v = v->m_next_vertex; nullptr != v; v = v->m_next_vertex)
        {
          x = v->m_P[0];
          if (x < bbox.m_min.x) bbox.m_min.x = x; else if (x > bbox.m_max.x) bbox.m_max.x = x;
          x = v->m_P[1];
          if (x < bbox.m_min.y) bbox.m_min.y = x; else if (x > bbox.m_max.y) bbox.m_max.y = x;
          x = v->m_P[2];
          if (x < bbox.m_min.z) bbox.m_min.z = x; else if (x > bbox.m_max.z) bbox.m_max.z = x;
        }
        break;
      }
    }
  }
  m_bbox = bbox;
  m_bDirtyBoundingBox = false;
}

ON_BoundingBox ON_SubDLevel::BoundingBox() const
{
  if ( m_aggregates.m_bDirtyBoundingBox )
    m_aggregates.UpdateBoundingBox(this);
  return m_aggregates.m_bbox;
}

ON_AggregateComponentStatus ON_SubDLevel::AggregateComponentStatus() const
{
  if (false == m_aggregates.m_aggregate_status.IsCurrent())
    m_aggregates.UpdateAggregateComponentStatus(this);
  return m_aggregates.m_aggregate_status;
}

void ON_SubDAggregates::UpdateEdgeFlags(
  const ON_SubDLevel* level
  )
{
  if (nullptr != level)
  {
    unsigned int edge_flags = 0;
    for (const ON_SubDEdge* e = level->m_edge[0]; nullptr != e; e = e->m_next_edge)
      edge_flags |= e->EdgeFlags();
    m_edge_flags = edge_flags;
  }
  m_bDirtyEdgeFlags = 0;
}

unsigned int ON_SubDLevel::EdgeFlags() const
{
  if (m_aggregates.m_bDirtyEdgeFlags)
    m_aggregates.UpdateEdgeFlags(this);
  return m_aggregates.m_edge_flags;
}

unsigned int ON_SubD::EdgeFlags() const
{
  return ActiveLevel().EdgeFlags();
}

void ON_SubDAggregates::UpdateAggregateComponentStatus(
  const ON_SubDLevel* level
  )
{
  m_aggregate_status = ON_AggregateComponentStatus::Empty;
  if (nullptr != level)
  {
    for (const ON_SubDVertex* v = level->m_vertex[0]; nullptr != v; v = v->m_next_vertex)
      m_aggregate_status.Add(v->m_status);
    for (const ON_SubDEdge* e = level->m_edge[0]; nullptr != e; e = e->m_next_edge)
      m_aggregate_status.Add(e->m_status);
    for (const ON_SubDFace* f = level->m_face[0]; nullptr != f; f = f->m_next_face)
      m_aggregate_status.Add(f->m_status);
  }
}


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDLevel
//

std::shared_ptr<const ON_SubDVertex*> ON_SubDLevel::VertexArray() const
{
  if (m_vertex_count != m_vertex_array_count || nullptr == m_vertex_array.get())
  {
    ON_SubDVertex const** a = new ON_SubDVertex const*[m_vertex_count];
    ON_SubDVertex const** a1 = a + m_vertex_count;
    const_cast<ON_SubDLevel*>(this)->m_vertex_array = std::shared_ptr<const ON_SubDVertex*>(a);
    for (const ON_SubDVertex* v = m_vertex[0]; nullptr != v && a < a1; v = v->m_next_vertex)
      *a++ = v;
    while (a < a1)
      *a++ = nullptr;
    const_cast<ON_SubDLevel*>(this)->m_vertex_array_count = m_vertex_count;
  }
  return m_vertex_array;
}

std::shared_ptr<const ON_SubDEdge*> ON_SubDLevel::EdgeArray() const
{
  if (m_edge_count != m_edge_array_count || nullptr == m_edge_array.get())
  {
    ON_SubDEdge const** a = new ON_SubDEdge const*[m_edge_count];
    ON_SubDEdge const** a1 = a + m_edge_count;
    const_cast<ON_SubDLevel*>(this)->m_edge_array = std::shared_ptr<const ON_SubDEdge*>(a);
    for (const ON_SubDEdge* v = m_edge[0]; nullptr != v && a < a1; v = v->m_next_edge)
      *a++ = v;
    while (a < a1)
      *a++ = nullptr;
    const_cast<ON_SubDLevel*>(this)->m_edge_array_count = m_edge_count;
  }
  return m_edge_array;
}

std::shared_ptr<const ON_SubDFace*> ON_SubDLevel::FaceArray() const
{
  if (m_face_count != m_face_array_count || nullptr == m_face_array.get())
  {
    ON_SubDFace const** a = new ON_SubDFace const*[m_face_count];
    ON_SubDFace const** a1 = a + m_face_count;
    const_cast<ON_SubDLevel*>(this)->m_face_array = std::shared_ptr<const ON_SubDFace*>(a);
    for (const ON_SubDFace* v = m_face[0]; nullptr != v && a < a1; v = v->m_next_face)
      *a++ = v;
    while (a < a1)
      *a++ = nullptr;
    const_cast<ON_SubDLevel*>(this)->m_face_array_count = m_face_count;
  }
  return m_face_array;
}

//////////////////////////////////////////////////////////////////////////
//
// ON_SubD::Tranxform
// ON_SubDimple::Transform
// ON_SubDLevel::Transform
// ON_SubDVertex::Transform
// ON_SubDEdge::Transform
// ON_SubDFace::Transform
//

static void TransformPoint(
  const double* xform,
  double P[3]
  )
{
  const double x = xform[0] * P[0] + xform[1] * P[1] + xform[2] * P[2] + xform[3];
  const double y = xform[4] * P[0] + xform[5] * P[1] + xform[6] * P[2] + xform[7];
  const double z = xform[8] * P[0] + xform[9] * P[1] + xform[10] * P[2] + xform[11];
  const double w = xform[12] * P[0] + xform[13] * P[1] + xform[14] * P[2] + xform[15];
  if (1.0 == w)
  {
    P[0] = x;
    P[1] = y;
    P[2] = z;
  }
  else
  {
    P[0] = x / w;
    P[1] = y / w;
    P[2] = z / w;
  }
}

static void TransformVector(
  const double* xform,
  double V[3]
  )
{
  const double x = xform[0] * V[0] + xform[1] * V[1] + xform[2] * V[2];
  const double y = xform[4] * V[0] + xform[5] * V[1] + xform[6] * V[2];
  const double z = xform[8] * V[0] + xform[9] * V[1] + xform[10] * V[2];
  V[0] = x;
  V[1] = y;
  V[2] = z;
}

bool ON_SubDSectorLimitPoint::Transform(
  const ON_Xform& xform
  )
{
  TransformPoint(&xform.m_xform[0][0],m_limitP);
  TransformVector(&xform.m_xform[0][0],m_limitT1);
  TransformVector(&xform.m_xform[0][0],m_limitT2);
  ON_3dVector N = ON_CrossProduct(m_limitT1,m_limitT2);
  bool rc = N.Unitize();
  m_limitN[0] = N.x;
  m_limitN[1] = N.y;
  m_limitN[2] = N.z;
  return rc;
}


bool ON_SubDVertex::Transform(
  bool bTransformationSavedSubdivisionPoint,
  const class ON_Xform& xform
  )
{
  TransformPoint(&xform.m_xform[0][0],m_P);
  if (0 != ON_SUBD_CACHE_DISPLACEMENT_FLAG(m_saved_points_flags))
    TransformVector(&xform.m_xform[0][0],m_displacement_V);

  if (ON_SubD::SubDType::Unset != SavedSubdivisionPointType())
  {
    if (bTransformationSavedSubdivisionPoint)
      TransformPoint(&xform.m_xform[0][0], m_saved_subd_point1);
    else
      ClearSavedSubdivisionPoint();
  }
  if (ON_SubD::SubDType::Unset != this->SavedLimitPointType())
  {
    if (bTransformationSavedSubdivisionPoint)
    {
      for (const ON_SubDSectorLimitPoint* lp = &m_limit_point; nullptr != lp; lp = lp->m_next_sector_limit_point)
        const_cast< ON_SubDSectorLimitPoint* >(lp)->Transform(xform);
    }
    else
      ClearSavedLimitPoints();
  }
  return true;
}

bool ON_SubDVertex::SetLocation(
  ON_3dPoint location,
  bool bClearNeighborhoodCache
)
{
  if (false == location.IsValid())
    return false;

  if (!(m_P[0] == location.x && m_P[1] == location.y && m_P[2] == location.z))
  {
    m_P[0] = location.x;
    m_P[1] = location.y;
    m_P[2] = location.z;
    ClearSavedSubdivisionPoint();
    ClearSavedLimitPoints();

    if (bClearNeighborhoodCache)
    {
      for (unsigned short vei = 0; vei < m_edge_count; vei++)
      {
        ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
        if (nullptr == edge)
          continue;
        edge->ClearSavedSubdivisionPoint();
        ON_SubDFacePtr* fptr = edge->m_face2;
        for (unsigned short efi = 0; efi < edge->m_face_count; efi++, fptr++)
        {
          if (2 == efi)
          {
            fptr = edge->m_facex;
            if (nullptr == fptr)
              break;
          }
          ON_SubDFace* face = ON_SUBD_FACE_POINTER(fptr->m_ptr);
          if (nullptr == face)
            continue;
          face->ClearSavedSubdivisionPoint();

          ON_SubDEdgePtr* eptr = face->m_edge4;
          for (unsigned short fei = 0; fei < face->m_edge_count; fei++, eptr++)
          {
            if (4 == fei)
            {
              eptr = face->m_edgex;
              if (nullptr == eptr)
                break;
            }
            ON_SubDEdge* fedge = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
            if (nullptr == fedge)
              continue;
            ON_SubDVertex* fvertex = const_cast<ON_SubDVertex*>(fedge->m_vertex[ON_SUBD_EDGE_DIRECTION(eptr->m_ptr)]);
            if (nullptr == fvertex)
              continue;
            fvertex->ClearSavedSubdivisionPoint();
            fvertex->ClearSavedLimitPoints();
          }
        }
      }
    }
  }

  return true;
}

bool ON_SubDEdge::Transform(
  bool bTransformationSavedSubdivisionPoint,
  const class ON_Xform& xform
  )
{
  if (0 != ON_SUBD_CACHE_DISPLACEMENT_FLAG(m_saved_points_flags))
    TransformVector(&xform.m_xform[0][0],m_displacement_V);

  if (ON_SubD::SubDType::Unset != SavedSubdivisionPointType())
  {
    if (bTransformationSavedSubdivisionPoint)
      TransformPoint(&xform.m_xform[0][0], m_saved_subd_point1);
    else
      ClearSavedSubdivisionPoint();
  }
  return true;
}

bool ON_SubDFace::Transform(
  bool bTransformationSavedSubdivisionPoint,
  const class ON_Xform& xform
  )
{
  if (0 != ON_SUBD_CACHE_DISPLACEMENT_FLAG(m_saved_points_flags))
    TransformVector(&xform.m_xform[0][0],m_displacement_V);

  if (ON_SubD::SubDType::Unset != SavedSubdivisionPointType())
  {
    if (bTransformationSavedSubdivisionPoint)
      TransformPoint(&xform.m_xform[0][0], m_saved_subd_point1);
    else
      ClearSavedSubdivisionPoint();
  }
  return true;
}

bool ON_SubDLevel::Transform(
  bool bTransformationSavedSubdivisionPoint,
  const class ON_Xform& xform
  )    
{
  bool rc = true;

  m_aggregates.m_bDirtyBoundingBox = true;

  for (const ON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex && rc; vertex = vertex->m_next_vertex)
  {
    rc = const_cast<ON_SubDVertex*>(vertex)->Transform(bTransformationSavedSubdivisionPoint,xform);
  }
  
  for (const ON_SubDEdge* edge = m_edge[0]; nullptr != edge && rc; edge = edge->m_next_edge)
  {
    rc = const_cast<ON_SubDEdge*>(edge)->Transform(bTransformationSavedSubdivisionPoint,xform);
  }
  
  for (const ON_SubDFace* face = m_face[0]; nullptr != face && rc; face = face->m_next_face)
  {
    rc = const_cast<ON_SubDFace*>(face)->Transform(bTransformationSavedSubdivisionPoint,xform);
  }

  if (false == m_limit_mesh.Transform(xform))
    rc = false;

  if (rc)
    return true;

  return ON_SUBD_RETURN_ERROR(false);
}


bool ON_SubDLimitMesh::Transform(
  const ON_Xform& xform
  )
{
  if (false == xform.IsValid())
    return false;
  if (xform.IsIdentity())
    return true;
  if (xform.IsZero())
    return false;
  ON_SubDLimitMeshImpl* impl = m_impl_sp.get();
  if ( nullptr == impl )
    return true; // transform applied to empty mesh is true on purpose.  Changing to false will break other code.
  return impl->Transform(xform);
}

bool ON_SubDimple::Transform(
  const ON_Xform& xform
  )
{
  if (false == xform.IsValid())
    return false;
  if (xform.IsZero())
    return true;
  if (xform.IsIdentity())
    return true;

  const unsigned int level_count = m_levels.UnsignedCount();
  if ( level_count <= 0 )
    return true; // transform applied to empty subd is true on purpose.

  bool rc = true;

  // If 
  // 1) The transformation is being applied to every vertex, edge and 
  //    face in every level of a subdivision object, and
  // 2) the transformation is an isometry (rotation, translation, ...),
  //   a uniform scale, or a composition of these types, 
  // then set bTransformationSavedSubdivisionPoint = true to apply the
  // transformation to saved subdivision and saved limit point information.
  // In all other cases, set bTransformationSavedSubdivisionPoint = false
  // and any saved subdivision points or saved limit points will be
  // deleted.
  const bool bTransformationSavedSubdivisionPoint = false; // todo - set this correctly

  for (unsigned int level_index = 0; level_index < level_count; level_index++)
  {
    ON_SubDLevel* level = m_levels[level_index];
    if (nullptr == level)
    {
      ON_SubDIncrementErrorCount();
      continue;
    }

    if (false == level->Transform(bTransformationSavedSubdivisionPoint, xform))
    {
      rc = false;
      break;
    }

  }

  return rc;

}

bool ON_SubDLimitMeshFragment::Transform(
  const ON_Xform& xform
  )
{
  if (0 == m_P_count)
  {
    m_bbox = ON_BoundingBox::EmptyBoundingBox;
    return true;
  }
  if ( false == ON_TransformPointList(3,false,m_P_count,(int)m_P_stride,m_P,xform) )
    return ON_SUBD_RETURN_ERROR(false);
  if ( false == ON_TransformVectorList(3,m_P_count,(int)m_P_stride,m_N,xform) )
    return ON_SUBD_RETURN_ERROR(false);
  ON_GetPointListBoundingBox(3,0,m_P_count,(int)m_P_stride,m_P,&m_bbox.m_min.x,&m_bbox.m_max.x,false);
  return true;
}

bool ON_SubDLimitMeshImpl::Transform(
  const ON_Xform& xform
  )
{
  m_bbox = ON_BoundingBox::EmptyBoundingBox;
  ON_BoundingBox bbox = ON_BoundingBox::EmptyBoundingBox;
  for ( const ON_SubDLimitMeshFragment* fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
  {
    if ( false == const_cast<ON_SubDLimitMeshFragment*>(fragment)->Transform(xform) )
      return ON_SUBD_RETURN_ERROR(false);
    if ( fragment == m_first_fragment )
      bbox = fragment->m_bbox;
    else
      bbox.Union(fragment->m_bbox);
  }
  m_bbox = bbox;
  m_limit_mesh_content_serial_number = ON_SubDLimitMeshImpl::Internal_NextContentSerialNumber();
  return true;
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//

ON_BoundingBox ON_SubDVertex::ControlNetBoundingBox() const
{
  ON_BoundingBox bbox;
  bbox.m_min = m_P;
  bbox.m_min = bbox.m_min;
  return bbox;
}


ON_BoundingBox ON_SubDVertex::LimitSurfaceBoundingBox(
  const ON_SubD& subd
  ) const
{
  ON_BoundingBox bbox;

  for (;;)
  {
    const ON_SubDimple* dimple = subd.SubDimple();
    if (nullptr == dimple)
      break;
    const ON_SubDLevel* level = dimple->SubDLevel(m_level);
    if ( nullptr == level )
      break;
    ON_SubDSectorLimitPoint limit_point;
    if (false == this->GetLimitPoint(
      level->m_subdivision_type, Face(0),true,limit_point))
      break;
    bbox.m_min = limit_point.m_limitP;
    bbox.m_max = bbox.m_min;
    break;
  }

  return bbox;
}

ON_BoundingBox ON_SubDEdge::ControlNetBoundingBox() const
{
  ON_BoundingBox bbox;
  if (nullptr != m_vertex[0] && nullptr != m_vertex[1])
  {
    ON_3dPoint P[2];
    P[0] = m_vertex[0]->m_P;
    P[1] = m_vertex[1]->m_P;
    ON_GetPointListBoundingBox(3, 0, 2, 3, &P[0].x, &bbox.m_min.x, &bbox.m_max.x,false);
  }
  return bbox;
}


ON_BoundingBox ON_SubDEdge::LimitSurfaceBoundingBox(
  const ON_SubD& subd
  ) const
{
  
  ON_BoundingBox bbox;

  for (;;)
  {
    const ON_SubDFace* face = Face(0);
    if ( nullptr == face )
      break;

    // TODO = restrict to just the edge
    bbox = face->LimitSurfaceBoundingBox(subd);
    break;

    //const ON_SubDimple* dimple = subd.SubDimple();
    //if (nullptr == dimple)
    //  break;
    //const ON_SubDLevel* level = dimple->SubDLevel(m_level);
    //if ( nullptr == level )
    //  break;
    //if (level->m_limit_mesh.IsEmpty())
    //{
    //  unsigned int display_density = 4;
    //  if (m_level < display_density)
    //    display_density -= m_level;
    //  else
    //    display_density = 1;
    //  ON_SubDDisplayParameters display_parameters = ON_SubDDisplayParameters::CreateFromDisplayDensity(display_density);
    //  level->UpdateLimitSurfaceMesh(subd, display_parameters);
    //  if ( level->m_limit_mesh.IsEmpty() )
    //    break;
    //}
    //bbox.m_min = limit_point.m_limitP;
    //bbox.m_max = bbox.m_min;
    //break;
  }

  return bbox;
}

ON_BoundingBox ON_SubDFace::ControlNetBoundingBox() const
{
  ON_BoundingBox bbox;
  ON_3dPoint P[16];
  unsigned int P_count = 0;
  const unsigned int P_capacity = (unsigned int)(sizeof(P) / sizeof(P[0]));
  bool bGrowBox = false;

  const unsigned int count = m_edge_count;
  for (unsigned int i = 0; i < count; i++)
  {
    const ON_SubDVertex* vertex = Vertex(i);
    if (nullptr == vertex)
      continue;
    P[P_count++] = vertex->m_P;
    if (P_count == P_capacity)
    {
      ON_GetPointListBoundingBox(3, 0, P_count, 3, &P[0].x, &bbox.m_min.x, &bbox.m_max.x, bGrowBox);
      P_count = 0;
      bGrowBox = true;
    }
  }

  if ( P_count > 0)
    ON_GetPointListBoundingBox(3, 0, P_count, 3, &P[0].x, &bbox.m_min.x, &bbox.m_max.x, bGrowBox);

  return bbox;
}


ON_BoundingBox ON_SubDFace::LimitSurfaceBoundingBox(
  const ON_SubD& subd
  ) const
{
  ON_BoundingBox bbox;

  for (;;)
  {
    const ON_SubDimple* dimple = subd.SubDimple();
    if (nullptr == dimple)
      break;
    const ON_SubDLevel* level = dimple->SubDLevel(m_level);
    if ( nullptr == level )
      break;
    if (level->m_limit_mesh.IsEmpty())
    {
      unsigned int display_density = 4;
      if (m_level < display_density)
        display_density -= m_level;
      else
        display_density = 1;
      ON_SubDDisplayParameters display_parameters = ON_SubDDisplayParameters::CreateFromDisplayDensity(display_density);
      level->UpdateLimitSurfaceMesh(subd, display_parameters);
      if ( level->m_limit_mesh.IsEmpty() )
        break;
    }
    for (const ON_SubDLimitMeshFragment* fragment = level->m_limit_mesh.FirstFragment(); nullptr != fragment; fragment = fragment->m_next_fragment)
    {
      if (this == fragment->m_face)
        bbox.Union(fragment->m_bbox);
    }
    break;
  }

  return bbox;

}
