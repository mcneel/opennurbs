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

static bool Internal_WriteDouble3(
  const double x[3],
  ON_BinaryArchive& archive
  )
{
  for (;;)
  {
    if (!archive.WriteDouble(3, x))
      break;
    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}

static bool Internal_ReadDouble3(
  ON_BinaryArchive& archive,
  double x[3]
  )
{
  for (;;)
  {
    if (!archive.ReadDouble(3, x))
      break;
    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}

enum : unsigned char
{
  ON_SubDComponentArchiveAnonymousChunkMark = 254U,
  ON_SubDComponentArchiveAdditionEndMark = 255U
};

static bool Internal_ReadComponentAdditionSize(ON_BinaryArchive& archive, unsigned char valid_sz, unsigned char* sz)
{
  if (archive.Archive3dmVersion() < 70)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  if (0 == valid_sz)
    return ON_SUBD_RETURN_ERROR(false);
  if (false == archive.ReadChar(sz))
    return ON_SUBD_RETURN_ERROR(false);
  if ( 0 != *sz && valid_sz != *sz && ON_SubDComponentArchiveAdditionEndMark != *sz )
    return ON_SUBD_RETURN_ERROR(false);
  return true;
}

static bool Internal_WriteComponentAdditionSize(bool bHaveAddition, ON_BinaryArchive& archive, unsigned char sz)
{
  if (archive.Archive3dmVersion() < 70)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  if (0 == sz)
    return ON_SUBD_RETURN_ERROR(false);
  if (false == bHaveAddition)
    sz = 0;
  if (false == archive.WriteChar(sz))
    return ON_SUBD_RETURN_ERROR(false);
  return true;
}

static bool Internal_FinishReadingComponentAdditions(ON_BinaryArchive& archive)
{
  if (archive.Archive3dmVersion() < 70)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  unsigned char sz = 1;
  if ( false == archive.ReadChar(&sz))
    return ON_SUBD_RETURN_ERROR(false);

  for (;;)
  {
    if (ON_SubDComponentArchiveAdditionEndMark == sz)
      return true;
    if (ON_SubDComponentArchiveAnonymousChunkMark == sz)
    {
      // skip an addition a future version added as an anonymous chunk
      int v = 0;
      if (false == archive.BeginRead3dmAnonymousChunk(&v))
        break;
      if (false == archive.EndRead3dmChunk())
        break;
    }
    else if ( sz > 0 )
    {
      // skip an addition a future version added as a fixed number of bytes
      // use archive.ReadByte(sz,buffer) instead of archive.SeekForward(sz) so CRC is properly calculated.
      char buffer[256];
      if (false == archive.ReadByte(sz,buffer))
        break;
    }
    sz = 0;
    if (false == archive.ReadChar(&sz))
      break;
  }
  return ON_SUBD_RETURN_ERROR(false);
}

static bool Internal_FinishWritingComponentAdditions(ON_BinaryArchive& archive)
{
  if (archive.Archive3dmVersion() < 70)
    return ON_SUBD_RETURN_ERROR(false);
  const unsigned char sz = ON_SubDComponentArchiveAdditionEndMark;
  return archive.WriteChar(sz);
}

static bool WriteBase(
  const ON_SubDComponentBase* base,
  ON_BinaryArchive& archive
  )
{
  for (;;)
  {
    unsigned int archive_id = base->ArchiveId();
    unsigned int id = base->m_id;
    unsigned short level = (unsigned short)base->SubdivisionLevel();
    if (!archive.WriteInt(archive_id))
      break;
    if (!archive.WriteInt(id))
      break;
    if (!archive.WriteShort(level))
      break;

    if (archive.Archive3dmVersion() < 70)
    {
      // version 6 3dm files
      double P[3], V[3];
      const bool bHaveP = base->GetSavedSubdivisionPoint(P);
      const bool bHaveV = base->GetSubdivisionDisplacement(V);

      unsigned char cP = bHaveP ? 4U : 0U;
      if (!archive.WriteChar(cP))
        break;
      if (0 != cP)
      {
        if (!Internal_WriteDouble3(P, archive))
          break;
      }

      unsigned char cV = bHaveV ? 4U : 0U;
      if (!archive.WriteChar(cV))
        break;
      if (0 != cV)
      {
        if (!Internal_WriteDouble3(V, archive))
          break;
      }
      return true;
    }
    
    // version 7 3dm files and later

    // 24 byte displacement addition
    double V[3];
    const bool bWriteDisplacement = base->GetSubdivisionDisplacement(V);
    if ( false == Internal_WriteComponentAdditionSize(bWriteDisplacement,archive,24) )
      break;
    if (bWriteDisplacement)
    {
      if (!archive.WriteDouble(3,V))
        break;
    }

    // 4 byte group id addition
    const bool bWriteGroupId = base->m_group_id > 0;
    if (false == Internal_WriteComponentAdditionSize(bWriteGroupId, archive, 4))
      break;
    if (bWriteGroupId)
    {
      if (!archive.WriteInt(base->m_group_id))
        break;
    }

    return Internal_FinishWritingComponentAdditions(archive);
  }
  return ON_SUBD_RETURN_ERROR(false);
}


static bool ReadBase(
  ON_BinaryArchive& archive,
  ON_SubDComponentBase& base
  )
{
  unsigned int archive_id = 0;
  unsigned int id = 0;
  unsigned short level = 0;
  for (;;)
  {
    if (!archive.ReadInt(&archive_id))
      break;
    if (!archive.ReadInt(&id))
      break;
    if (!archive.ReadShort(&level))
      break;

    base.m_id = id;
    base.SetArchiveId(archive_id);
    base.SetSubdivisionLevel(level);

    if (archive.Archive3dmVersion() < 70)
    {
      unsigned char cP = 0U;
      unsigned char cV = 0U;
      double P[3], V[3];

      if (!archive.ReadChar(&cP))
        break;
      if (0 != cP)
      {
        if (!Internal_ReadDouble3(archive, P))
          break;
      }

      if (!archive.ReadChar(&cV))
        break;
      if (0 != cV)
      {
        if (!Internal_ReadDouble3(archive, V))
          break;
        const unsigned int no_displacement_version = ON_VersionNumberConstruct(7, 0, 2019, 6, 12, 0);
        const unsigned int archive_opennurbs_version = archive.ArchiveOpenNURBSVersion();
        if (archive_opennurbs_version <= no_displacement_version)
        {
          cV = 0;
          V[0] = 0.0;
          V[1] = 0.0;
          V[2] = 0.0;
        }
      }
      if (4 == cP)
        base.SetSavedSubdivisionPoint(P);
      if (4 == cV)
        base.SetSubdivisionDisplacement(V);
      return true;
    }

    // read additions
    unsigned char sz;

    // 24 byte displacement addition
    sz = 0;
    if (false == Internal_ReadComponentAdditionSize(archive, 24, &sz))
      break;
    if (ON_SubDComponentArchiveAdditionEndMark == sz)
      return true; // end of additions
    if (0 != sz)
    {
      double V[3] = {};
      if (!archive.ReadDouble(V))
        break;
      base.SetSubdivisionDisplacement(V);
    }

    // 4 byte group id addition
    sz = 0;
    if (false == Internal_ReadComponentAdditionSize(archive, 4, &sz))
      break;
    if (ON_SubDComponentArchiveAdditionEndMark == sz)
      return true; // end of additions
    if (0 != sz)
    {
      if (!archive.ReadInt(&base.m_group_id))
        break;
    }
    
    return Internal_FinishReadingComponentAdditions(archive);
  }

  return ON_SUBD_RETURN_ERROR(false);
}


static bool WriteArchiveIdAndFlags(
  unsigned int archive_id,
  ON__UINT_PTR ptr_flags,
  ON_BinaryArchive& archive
  )
{
  if (!archive.WriteInt(archive_id))
    return ON_SUBD_RETURN_ERROR(false);
  unsigned char flags = (unsigned char)ON_SUBD_COMPONENT_FLAGS(ptr_flags);
  if (!archive.WriteChar(flags))
    return ON_SUBD_RETURN_ERROR(false);
  return true;
}



static bool ReadArchiveIdAndFlagsIntoComponentPtr(
  ON_BinaryArchive& archive,
  ON__UINT_PTR& element_ptr
  )
{
  element_ptr = 0;
  unsigned int archive_id = 0;
  if (!archive.ReadInt(&archive_id))
    return ON_SUBD_RETURN_ERROR(false);
  unsigned char flags = 0;
  if (!archive.ReadChar(&flags))
    return ON_SUBD_RETURN_ERROR(false);
  element_ptr = archive_id;
  element_ptr *= (ON_SUBD_COMPONENT_FLAGS_MASK+1);
  element_ptr += (flags & ON_SUBD_COMPONENT_FLAGS_MASK);
  return true;
}

static bool WriteSavedLimitPointList(
  unsigned int vertex_face_count,
  bool bHaveLimitPoint,
  const ON_SubDSectorSurfacePoint& limit_point,
  ON_BinaryArchive& archive
  )
{
  unsigned int limit_point_count = 0;
  const ON_SubDSectorSurfacePoint* p;

  if (bHaveLimitPoint)
  {
    for (p = &limit_point; nullptr != p && limit_point_count <= vertex_face_count; p = p->m_next_sector_limit_point)
    {
      if (!ON_IsValid(p->m_limitP[0]))
        break;
      if (limit_point_count > 0 && nullptr == p->m_sector_face)
        break;
      limit_point_count++;
    }
    if (limit_point_count > vertex_face_count || nullptr != p)
      limit_point_count = 0;

    if (limit_point_count > vertex_face_count)
      limit_point_count = 0;
  }
  if (0 == limit_point_count)
    bHaveLimitPoint = false;

  for (;;)
  {
    unsigned char c = bHaveLimitPoint ? 4 : 0;
    if (!archive.WriteChar(c))
      break;

    if (0 == c)
      return true;

    if (!archive.WriteInt(limit_point_count))
      break;
    
    p = &limit_point;
    for (unsigned int i = 0; i < limit_point_count; i++, p = p->m_next_sector_limit_point )
    {
      if (!Internal_WriteDouble3(limit_point.m_limitP, archive))
        break;
      if (!Internal_WriteDouble3(limit_point.m_limitT1, archive))
        break;
      if (!Internal_WriteDouble3(limit_point.m_limitT2, archive))
        break;
      if (!Internal_WriteDouble3(limit_point.m_limitN, archive))
        break;
      if (!WriteArchiveIdAndFlags(limit_point.m_sector_face ? limit_point.m_sector_face->ArchiveId() : 0, 0, archive))
        break;
    }
    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}

static bool ReadSavedLimitPointList(
  ON_BinaryArchive& archive,
  unsigned int vertex_face_count,
  ON_SimpleArray< ON_SubDSectorSurfacePoint > limit_points
  )
{
  limit_points.SetCount(0);

  for (;;)
  {
    unsigned char c = 0;
    if (!archive.ReadChar(&c))
      break;

    if ( 0 == c)
      return true;

    unsigned int limit_point_count = 0;
    if (!archive.ReadInt(&limit_point_count))
      break;

    if ( 0 == limit_point_count )
      break;

    if (limit_point_count > vertex_face_count)
      break;

    limit_points.Reserve(limit_point_count);

    for ( unsigned int i = 0; i < limit_point_count; i++ )
    {
      ON_SubDSectorSurfacePoint limit_point = ON_SubDSectorSurfacePoint::Unset;
      if (!Internal_ReadDouble3(archive,limit_point.m_limitP))
        break;
      if (!Internal_ReadDouble3(archive,limit_point.m_limitT1))
        break;
      if (!Internal_ReadDouble3(archive,limit_point.m_limitT2))
        break;
      if (!Internal_ReadDouble3(archive,limit_point.m_limitN))
        break;
      ON_SubDFacePtr fptr = ON_SubDFacePtr::Null;
      if (!ReadArchiveIdAndFlagsIntoComponentPtr(archive,fptr.m_ptr))
        break;
      limit_points.Append(limit_point);
    }

    if (limit_point_count != limit_points.UnsignedCount() )
      break;

    if (4 != c)
      limit_points.SetCount(0);

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}

static bool WriteVertexList(
  unsigned short vertex_count,
  const ON_SubDVertex*const* vertex,
  ON_BinaryArchive& archive
  )
{
  for (;;)
  {
    ON_SubDArchiveIdMap::ValidateArrayCounts(vertex_count,vertex_count,vertex,0,nullptr);

    if (!archive.WriteShort(vertex_count))
      break;

    if ( 0 == vertex_count )
      return true;
    
    const ON__UINT_PTR ptr_flags = 0; // for future use
    unsigned short i = 0;
    for (i = 0; i < vertex_count; i++)
    {
      const ON_SubDVertex* v = vertex[i];
      if (!WriteArchiveIdAndFlags((nullptr != v) ? v->ArchiveId() : 0, ptr_flags, archive))
        break;
    }
    if ( i < vertex_count )
      break;

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}


static bool ReadVertexList(
  ON_BinaryArchive& archive,
  unsigned short& vertex_count,
  unsigned short vertex_capacity,
  ON_SubDVertex* vertex[]
  )
{
  for (;;)
  {
    unsigned short archive_vertex_count = 0;
    if (!archive.ReadShort(&archive_vertex_count))
      break;

    if (archive_vertex_count != vertex_count)
    {
      ON_ERROR("Archive vertex count != expected vertex count.");
      if ( archive_vertex_count < vertex_count)
        vertex_count = archive_vertex_count;
    }

    ON_SubDArchiveIdMap::ValidateArrayCounts(vertex_count,vertex_capacity,vertex,0,nullptr);

    unsigned short i = 0;
    for (i = 0; i < vertex_count; i++)
    {
      ON__UINT_PTR vptr = 0;
      if (!ReadArchiveIdAndFlagsIntoComponentPtr(archive,vptr))
        break;
      vertex[i] = (ON_SubDVertex*)vptr;
    }
    if ( i < vertex_count )
      break;

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}


static bool WriteEdgePtrList(
  unsigned short edge_count,
  unsigned short edgeN_capacity,
  const ON_SubDEdgePtr* edgeN,
  unsigned short edgeX_capacity,
  const ON_SubDEdgePtr* edgeX,
  ON_BinaryArchive& archive
  )
{
  for (;;)
  {
    ON_SubDArchiveIdMap::ValidateArrayCounts(edge_count,edgeN_capacity,edgeN,edgeX_capacity,edgeX);

    if (!archive.WriteShort(edge_count))
      break;

    if ( 0 == edge_count )
      return true;

    const ON_SubDEdgePtr* eptr = edgeN;
    unsigned short i = 0;
    for (i = 0; i < edge_count; i++, eptr++)
    {
      if ( i == edgeN_capacity)
        eptr = edgeX;
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr->m_ptr);
      if (!WriteArchiveIdAndFlags((nullptr != edge) ? edge->ArchiveId() : 0,eptr->m_ptr,archive))
        break;
    }
    if ( i < edge_count )
      break;

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}


static bool ReadEdgePtrList(
  ON_BinaryArchive& archive,
  unsigned short& edge_count,
  unsigned short edgeN_capacity,
  ON_SubDEdgePtr* edgeN,
  unsigned short edgeX_capacity,
  ON_SubDEdgePtr* edgeX
  )
{
  for (;;)
  {
    unsigned short archive_edge_count = 0;
    if (!archive.ReadShort(&archive_edge_count))
      break;

    if (archive_edge_count != edge_count)
    {
      ON_ERROR("Archive edge count != expected edge count.");
      if ( archive_edge_count < edge_count)
        edge_count = archive_edge_count;
    }

    ON_SubDArchiveIdMap::ValidateArrayCounts(edge_count,edgeN_capacity,edgeN,edgeX_capacity,edgeX);


    ON_SubDEdgePtr* eptr = edgeN;
    unsigned short i = 0;
    for (i = 0; i < edge_count; i++, eptr++)
    {
      if ( i == edgeN_capacity)
        eptr = edgeX;
      if (!ReadArchiveIdAndFlagsIntoComponentPtr(archive,eptr->m_ptr))
        break;
    }
    if ( i < edge_count )
      break;

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}


static bool WriteFacePtrList(
  unsigned short face_count,
  size_t faceN_capacity,
  const ON_SubDFacePtr* faceN,
  unsigned short faceX_capacity,
  const ON_SubDFacePtr* faceX,
  ON_BinaryArchive& archive
  )
{
  for (;;)
  {
    ON_SubDArchiveIdMap::ValidateArrayCounts(face_count,faceN_capacity,faceN,faceX_capacity,faceX);

    if (!archive.WriteShort(face_count))
      break;

    if ( 0 == face_count )
      return true;

    const ON_SubDFacePtr* fptr = faceN;
    unsigned short i = 0;
    for (i = 0; i < face_count; i++, fptr++)
    {
      if ( i == faceN_capacity)
        fptr = faceX;
      const ON_SubDFace* face = ON_SUBD_FACE_POINTER(fptr->m_ptr);
      if (!WriteArchiveIdAndFlags((nullptr != face) ? face->ArchiveId() : 0,fptr->m_ptr,archive))
        break;
    }
    if ( i < face_count )
      break;

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}


static bool ReadFacePtrList(
  ON_BinaryArchive& archive,
  unsigned short& face_count,
  unsigned short faceN_capacity,
  ON_SubDFacePtr* faceN,
  unsigned short faceX_capacity,
  ON_SubDFacePtr* faceX
  )
{
  for (;;)
  {
    unsigned short archive_face_count = 0;
    if (!archive.ReadShort(&archive_face_count))
      break;

    if (archive_face_count != face_count)
    {
      ON_ERROR("Archive face count != expected face count.");
      if ( archive_face_count < face_count)
        face_count = archive_face_count;
    }

    ON_SubDArchiveIdMap::ValidateArrayCounts(face_count,faceN_capacity,faceN,faceX_capacity,faceX);

    ON_SubDFacePtr* fptr = faceN;
    unsigned short i = 0;
    for (i = 0; i < face_count; i++, fptr++)
    {
      if ( i == faceN_capacity)
        fptr = faceX;
      if (!ReadArchiveIdAndFlagsIntoComponentPtr(archive,fptr->m_ptr))
        break;
    }
    if ( i < face_count )
      break;

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}


bool ON_SubDVertex::Write(
  ON_BinaryArchive& archive
  ) const
{
  for (;;)
  {
    if (!WriteBase(this,archive))
      break;
    if (!archive.WriteChar((unsigned char)m_vertex_tag))
      break;
    //if (!archive.WriteChar((unsigned char)m_vertex_edge_order))
    //  break;
    //if (!archive.WriteChar((unsigned char)m_vertex_facet_type))
    //  break;
    if (!Internal_WriteDouble3(m_P,archive))
      break;
    if (!archive.WriteShort(m_edge_count))
      break;
    if (!archive.WriteShort(m_face_count))
      break;
    if (!WriteSavedLimitPointList(m_face_count, this->SurfacePointIsSet(), m_limit_point, archive))
      break;
    if (!WriteEdgePtrList(m_edge_count,m_edge_capacity,m_edges,0,nullptr, archive))
      break;
    if (!WriteFacePtrList(m_face_count,m_face_capacity,(const ON_SubDFacePtr*)m_faces,0,nullptr, archive))
      break;

    if (archive.Archive3dmVersion() < 70)
    {
      // mark end with a 0 byte
      if (!archive.WriteChar((unsigned char)0U))
        break;
      return true;
    }


    return Internal_FinishWritingComponentAdditions(archive);
  }
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDVertex::Read(
  class ON_BinaryArchive& archive,
  class ON_SubD& subd,
  class ON_SubDVertex*& vertex
  )
{
  vertex = nullptr;

  for (;;)
  {
    ON_SubDimple* subdimple = const_cast<ON_SubDimple*>(subd.SubDimple());
    if ( nullptr == subdimple)
      break;

    ON_SubDComponentBase base = ON_SubDComponentBase::Unset;
    unsigned char vertex_tag = 0;
    //unsigned char vertex_edge_order = 0;
    //unsigned char vertex_facet_type = 0;
    double P[3];
    unsigned short edge_count = 0;
    unsigned short face_count = 0;

    ON_SimpleArray<ON_SubDSectorSurfacePoint> limit_points;

    if (!ReadBase(archive,base))
      break;
    if (!archive.ReadChar(&vertex_tag))
      break;
    //if (!archive.ReadChar(&vertex_edge_order))
    //  break;
    //if (!archive.ReadChar(&vertex_facet_type))
    //  break;
    if (!Internal_ReadDouble3(archive,P))
      break;
    if (!archive.ReadShort(&edge_count))
      break;
    if (!archive.ReadShort(&face_count))
      break;

    if (!ReadSavedLimitPointList(archive, face_count, limit_points))
      break;

    ON_SubDVertex* v = subdimple->AllocateVertex(
      base.m_id, // serialization must preserve ON_SubDVertex.m_id
      ON_SubD::VertexTagFromUnsigned(vertex_tag),
      base.SubdivisionLevel(),
      P,
      edge_count,
      face_count
      );

    if ( nullptr == v )
      break;

    v->ON_SubDComponentBase::operator=(base);

    //v->m_vertex_edge_order = ON_SubD::VertexEdgeOrderFromUnsigned(vertex_edge_order);
    //v->m_vertex_facet_type = ON_SubD::VertexFacetTypeFromUnsigned(vertex_facet_type);

    if (!ReadEdgePtrList(archive,edge_count,v->m_edge_capacity,v->m_edges,0,nullptr))
      break;
    v->m_edge_count = edge_count;

    if (!ReadFacePtrList(archive,face_count,v->m_face_capacity,(ON_SubDFacePtr*)v->m_faces,0,nullptr))
      break;
    v->m_face_count = face_count;

    for (unsigned int i = 0; i < limit_points.UnsignedCount(); i++)
    {
      ON_SubDSectorSurfacePoint limit_point = limit_points[i];
      limit_point.m_next_sector_limit_point = (const ON_SubDSectorSurfacePoint*)1U; // skips checks
      if (false == v->SetSavedSurfacePoint( true, limit_point))
      {
        v->ClearSavedSurfacePoints();
        break;
      }
    }

    vertex = v;

    if (archive.Archive3dmVersion() < 70)
    {
      unsigned char sz = 1;
      if (!archive.ReadChar(&sz) || 0 != sz)
        break;
      return true;
    }

    // read additions
    return Internal_FinishReadingComponentAdditions(archive);
  }
  return ON_SUBD_RETURN_ERROR(false);
}


bool ON_SubDEdge::Write(
  ON_BinaryArchive& archive
  ) const
{
  for (;;)
  {
    if (!WriteBase(this,archive))
      break;
    if (!archive.WriteChar((unsigned char)m_edge_tag))
      break;
    if (!archive.WriteShort(m_face_count))
      break;
    if (!archive.WriteDouble(2,m_sector_coefficient))
      break;
    if (!archive.WriteDouble(m_sharpness))
      break;
    if (!WriteVertexList(2, m_vertex, archive))
      break;
    if (!WriteFacePtrList(m_face_count,sizeof(m_face2)/sizeof(m_face2[0]),m_face2,m_facex_capacity,m_facex, archive))
      break;

    if (archive.Archive3dmVersion() < 70)
    {
      // mark end with a 0 byte
      if (!archive.WriteChar((unsigned char)0U))
        break;
      return true;
    }

    return Internal_FinishWritingComponentAdditions(archive);
  }
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDEdge::Read(
  class ON_BinaryArchive& archive,
  class ON_SubD& subd,
  class ON_SubDEdge*& edge
  )
{
  edge = nullptr;

  for (;;)
  {
    ON_SubDimple* subdimple = const_cast<ON_SubDimple*>(subd.SubDimple());
    if ( nullptr == subdimple)
      break;

    ON_SubDComponentBase base = ON_SubDComponentBase::Unset;
    unsigned char edge_tag = 0;
    unsigned short face_count = 0;
    double sector_weight[2] = { 0 };
    double sharpness = 0.0;

    if (!ReadBase(archive,base))
      break;
    if (!archive.ReadChar(&edge_tag))
      break;
    if (!archive.ReadShort(&face_count))
      break;
    if (!archive.ReadDouble(2,sector_weight))
      break;
    if (!archive.ReadDouble(&sharpness))
      break;

    ON_SubDVertex* v[2] = { 0 };
    unsigned short vertex_count = 2;
    if (!ReadVertexList(archive, vertex_count, 2, v))
      break;

    ON_SubDEdge* e = subdimple->AllocateEdge(
      base.m_id, // serialization must preserve ON_SubDEdge.m_id
      ON_SubD::EdgeTagFromUnsigned(edge_tag),
      base.SubdivisionLevel(),
      face_count
      );

    if ( nullptr == e )
      break;

    e->ON_SubDComponentBase::operator=(base);

    for ( unsigned short evi = 0; evi < 2 && evi < vertex_count; evi++ )
      e->m_vertex[evi] = v[evi];

    e->m_sector_coefficient[0] = sector_weight[0];
    e->m_sector_coefficient[1] = sector_weight[1];
    e->m_sharpness = sharpness;

    if (!ReadFacePtrList(archive,face_count,sizeof(e->m_face2)/sizeof(e->m_face2[0]),e->m_face2,e->m_facex_capacity,e->m_facex))
      break;
    e->m_face_count = face_count;
   
    edge = e;

    if (archive.Archive3dmVersion() < 70)
    {
      unsigned char sz;
      if (false == archive.ReadChar(&sz) || 0 != sz)
        break;
      return true;
    }

    return Internal_FinishReadingComponentAdditions(archive);
  }
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDFace::Write(
  ON_BinaryArchive& archive
  ) const
{
  for (;;)
  {
    if (!WriteBase(this,archive))
      break;
    if (!archive.WriteInt(m_level_zero_face_id))
      break;

    // OBSOLETE parent face id
    const int obsolete_parent_face_id = 0;
    if (!archive.WriteInt(obsolete_parent_face_id))
      break;

    if (!archive.WriteShort(m_edge_count))
      break;
    if (!WriteEdgePtrList(m_edge_count,sizeof(m_edge4)/sizeof(m_edge4[0]),m_edge4,m_edgex_capacity,m_edgex, archive))
      break;

    if (archive.Archive3dmVersion() < 70)
    {
      unsigned char sz = 0;
      if (!archive.WriteChar(sz))
        break;
      return true;
    }

    // write 34 byte texture domain
    const bool bWriteTextureDomain = TextureDomainIsSet();
    if (false == Internal_WriteComponentAdditionSize(bWriteTextureDomain, archive, 34))
      break;
    if (bWriteTextureDomain)
    {
      const unsigned char domain_type = static_cast<unsigned char>(TextureDomainType());
      if (!archive.WriteChar(domain_type))
        break;

      const unsigned packing_rot = TextureDomainRotationDegrees();
      const unsigned char packing_rot_dex = (unsigned char)(packing_rot/90U);
      if (!archive.WriteChar(packing_rot_dex))
        break;
      const ON_2dPoint texture_domain_origin = TextureDomainOrigin();
      if (!archive.WriteDouble(2, &texture_domain_origin.x))
        break;
      const ON_2dVector texture_domain_delta = TextureDomainDelta();
      if (!archive.WriteDouble(2, &texture_domain_delta.x))
        break;
    }

    // 4 byte render material channel index
    const int material_channel_index = MaterialChannelIndex();
    const bool bWriteMaterialChannelIndex = (material_channel_index > 0 && material_channel_index <= ON_Material::MaximumMaterialChannelIndex);
    if (false == Internal_WriteComponentAdditionSize(bWriteMaterialChannelIndex, archive, 4))
      break;
    if (bWriteMaterialChannelIndex)
    {
      if (!archive.WriteInt(material_channel_index))
        break;
    }

    // 4 byte per face color
    const ON_Color per_face_color = PerFaceColor();
    const bool bWritePerFaceColor = (ON_Color::UnsetColor != per_face_color);
    if (false == Internal_WriteComponentAdditionSize(bWritePerFaceColor, archive, 4))
      break;
    if (bWritePerFaceColor)
    {
      if (!archive.WriteColor(per_face_color))
        break;
    }

    return Internal_FinishWritingComponentAdditions(archive);
  }
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDFace::Read(
  class ON_BinaryArchive& archive,
  class ON_SubD& subd,
  class ON_SubDFace*& face
  )
{
  face = nullptr;

  for (;;)
  {
    ON_SubDimple* subdimple = const_cast<ON_SubDimple*>(subd.SubDimple());
    if ( nullptr == subdimple)
      break;

    ON_SubDComponentBase base = ON_SubDComponentBase::Unset;
    unsigned int level_zero_face_id = 0;
    unsigned int obsolete_parent_face_id = 0;
    unsigned short edge_count = 0;

    if (!ReadBase(archive,base))
      break;
    if (!archive.ReadInt(&level_zero_face_id))
      break;
    if (!archive.ReadInt(&obsolete_parent_face_id))
      break;
    if (!archive.ReadShort(&edge_count))
      break;

    ON_SubDFace* f = subdimple->AllocateFace(
      base.m_id, // serialzation must preserve ON_SubDFace.m_id
      base.SubdivisionLevel(),
      edge_count
      );

    if ( nullptr == f )
      break;

    f->ON_SubDComponentBase::operator=(base);

    f->m_level_zero_face_id = level_zero_face_id;

    if (!ReadEdgePtrList(archive,edge_count,sizeof(f->m_edge4)/sizeof(f->m_edge4[0]),f->m_edge4,f->m_edgex_capacity,f->m_edgex))
      break;
    f->m_edge_count = edge_count;
    
    face = f;

    if (archive.Archive3dmVersion() < 70)
    {
      unsigned char sz;
      if (false == archive.ReadChar(&sz) || 0 != sz)
        break;
      return true;
    }

    // read additions
    unsigned char sz;
    
    sz = 0;
    if (false == Internal_ReadComponentAdditionSize(archive, 34, &sz))
      break;
    if (ON_SubDComponentArchiveAdditionEndMark == sz)
      return true; // end of additions
    if ( 0 != sz)
    {
      // 34 bytes of texture domain information
      unsigned char domain_type;
      if (!archive.ReadChar(&domain_type))
        break;
      unsigned char packing_rot_dex = 0;
      if (!archive.ReadChar(&packing_rot_dex))
        break;
      const unsigned packing_rot = ((unsigned int)packing_rot_dex) * 90U;
      ON_2dPoint texture_domain_origin(ON_2dPoint::Origin);
      if (!archive.ReadDouble(2, &texture_domain_origin.x))
        break;
      ON_2dVector texture_domain_delta(ON_2dVector::ZeroVector);
      if (!archive.ReadDouble(2, &texture_domain_delta.x))
        break;
      f->SetTextureDomain(ON_SubD::TextureDomainTypeFromUnsigned(domain_type), texture_domain_origin, texture_domain_delta, packing_rot);
    }

    sz = 0;
    if (false == Internal_ReadComponentAdditionSize(archive, 4, &sz))
      break;
    if (ON_SubDComponentArchiveAdditionEndMark == sz)
      return true; // end of additions
    if (0 != sz)
    {
      // 4 bytes of material channel index
      int material_channel_index = 0;
      if (false == archive.ReadInt(&material_channel_index))
        break;
      f->SetMaterialChannelIndex(material_channel_index);
    }

    sz = 0;
    if (false == Internal_ReadComponentAdditionSize(archive, 4, &sz))
      break;
    if (ON_SubDComponentArchiveAdditionEndMark == sz)
      return true; // end of additions
    if (0 != sz)
    {
      // 4 bytes of per face color
      ON_Color per_face_color = ON_Color::UnsetColor;
      if (false == archive.ReadColor(per_face_color))
        break;
      f->SetPerFaceColor(per_face_color);
    }

    return Internal_FinishReadingComponentAdditions(archive);
  }
  return ON_SUBD_RETURN_ERROR(false);
}

unsigned int ON_SubDLevel::SetArchiveId(
  const ON_SubDimple& subdimple,
  unsigned int archive_id_partition[4],
  bool bLevelLinkedListIncreasingId[3]
  ) const
{
  unsigned int archive_id = 1;
  //archive_id_partition[0] = 0;
  //archive_id_partition[1] = 0;
  //archive_id_partition[2] = 0;
  //archive_id_partition[3] = 0;

  const ON_SubDComponentPtr::Type component_type[3] = {
    ON_SubDComponentPtr::Type::Vertex,
    ON_SubDComponentPtr::Type::Edge,
    ON_SubDComponentPtr::Type::Face
  };

  const ON_SubDComponentBaseLink* first_link[3] = {
    (const ON_SubDComponentBaseLink*)m_vertex[0],
    (const ON_SubDComponentBaseLink*)m_edge[0],
    (const ON_SubDComponentBaseLink*)m_face[0]
  };


  for (unsigned int listdex = 0; listdex < 3; listdex++)
  {
    bLevelLinkedListIncreasingId[listdex] 
      = nullptr != first_link[listdex] 
      && first_link[listdex]->m_id > 0U;
    unsigned int prev_id = 0;
    archive_id_partition[listdex] = archive_id;
    unsigned int linked_list_count = 0;
    for (const ON_SubDComponentBaseLink* clink = first_link[listdex]; nullptr != clink; clink = clink->m_next)
    {
      ++linked_list_count;
      if (prev_id < clink->m_id)
      {
        prev_id = clink->m_id;
        clink->SetArchiveId(archive_id++);
        continue;
      }

      // the for(..) scope we are currently in is exited below.
      bLevelLinkedListIncreasingId[listdex] = false;

      // m_id values are not increasing in the linked list. 
      // This happens when the subd is edited  and components are deleted 
      // and then added back later. 
      // Finish counting components in the linked list.
      for (clink = clink->m_next; nullptr != clink; clink = clink->m_next)
        ++linked_list_count;

      // Now iteratate the fixed size pool (which always iterates in increasing id order),
      // skip components not on this level, and set archive id of the ones on this level.
      unsigned int cidit_level_count = 0;
      archive_id = archive_id_partition[listdex];
      ON_SubDComponentIdIterator cidit;
      subdimple.InitializeComponentIdIterator(component_type[listdex],cidit);
      const unsigned level_index = this->m_level_index;
      prev_id = 0;
      for (const ON_SubDComponentBase* c = cidit.FirstComponent(); nullptr != c; c = cidit.NextComponent())
      {
        if (prev_id >= c->m_id)
        {
          // This is a serious error!
          // Contine because this allows us to save something do the disk in these bad cases.
          ON_SUBD_ERROR("The m_id values of the active components in the fixed size pool are corrupt.");
        }
        else
        {
          prev_id = c->m_id;
        }
        if (level_index != c->SubdivisionLevel())
          continue;
        ++cidit_level_count;
        c->SetArchiveId(archive_id++);
      }
      if (cidit_level_count != linked_list_count)
      {
        // This is a serious error!
        // Contine because this allows us to save something do the disk in these bad cases.
        ON_SUBD_ERROR("The m_level values of the active components in the fixed size pool are corrupt.");
      }
      break;
    }
  }

  archive_id_partition[3] = archive_id;

  return archive_id-1;
}


void ON_SubDLevel::ClearArchiveId() const
{
  // archive ids can be cleared in any order.
  for (const ON_SubDVertex* v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
    v->SetArchiveId(0);
  for (const ON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
    e->SetArchiveId(0);
  for (const ON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
    f->SetArchiveId(0);
}

bool ON_SubDLevel::Write(
  const ON_SubDimple& subdimple,
  ON_BinaryArchive& archive
  ) const
{
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,1))
    return ON_SUBD_RETURN_ERROR(false);

  bool rc = false;
  for (;;)
  {
    if (!archive.WriteShort((unsigned short)m_level_index))
      break;

    // from early days when there was a possibility of different types of subdivision algorithm
    // 4,4,4 means catmull clark quad
    if (!archive.WriteChar((unsigned char)4))
      break;
    if (!archive.WriteChar((unsigned char)4))
      break;
    if (!archive.WriteChar((unsigned char)4))
      break;   

    ON_BoundingBox bbox = m_aggregates.m_bDirtyBoundingBox ? ON_BoundingBox::EmptyBoundingBox : m_aggregates.m_controlnet_bbox;
    if (!archive.WriteDouble(3,bbox[0]))
      break;
    if (!archive.WriteDouble(3,bbox[1]))
      break;


    unsigned int archive_id_partition[4] = {};
    bool bLevelLinkedListIncreasingId[3] = {};
    SetArchiveId(subdimple, archive_id_partition, bLevelLinkedListIncreasingId);

    if (!archive.WriteInt(4,archive_id_partition))
      break;

    const ON_SubDVertex* v = nullptr;
    const ON_SubDEdge* e = nullptr;
    const ON_SubDFace* f = nullptr;

    // Have to use idit because subd editing (deleting and then adding) can leave the level's linked lists
    // with components in an order that is not increasing in id and it is critical that the next three for
    // loops iterate the level's components in order of increasing id.
    ON_SubDLevelComponentIdIterator idit;

    // must iterate vertices in order of increasing id
    idit.Initialize(bLevelLinkedListIncreasingId[0], ON_SubDComponentPtr::Type::Vertex, subdimple, *this);
    for (v = idit.FirstVertex(); nullptr != v; v = idit.NextVertex())
    {
      if( !v->Write(archive) )
        break;
    }
    if ( nullptr != v )
      break;

    // must iterate edges in order of increasing id
    idit.Initialize(bLevelLinkedListIncreasingId[1], ON_SubDComponentPtr::Type::Edge, subdimple, *this);
    for (e = idit.FirstEdge(); nullptr != e; e = idit.NextEdge())
    {
      if( !e->Write(archive) )
        break;
    }
    if ( nullptr != e )
      break;

    // must iterate faces in order of increasing id
    idit.Initialize(bLevelLinkedListIncreasingId[2], ON_SubDComponentPtr::Type::Face, subdimple, *this);
    for (f = idit.FirstFace(); nullptr != f; f = idit.NextFace())
    {
      if( !f->Write(archive) )
        break;
    }
    if ( nullptr != f )
      break;

    // chunk 1.1 has meshes
    unsigned char c = 0;
    if (archive.Save3dmRenderMesh(ON::object_type::subd_object) || archive.Save3dmAnalysisMesh(ON::object_type::subd_object))
    {
      // no reason to save the m_control_net_mesh
      if (false == m_surface_mesh.IsEmpty())
      {
        c = 0;
        // c = 1; TODO change to c = 1 when ON_SubDMesh::Write()/Read() actually work
      }
    }
    
    if (!archive.WriteChar(c))
      break;

    if (1 == c)
    {
      //if (!m_limit_mesh.Write(archive))
      //  break;
    }

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;
  ClearArchiveId();

  if (rc)
    return rc;
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDLevel::Read(
  ON_BinaryArchive& archive,
  class ON_SubDArchiveIdMap& element_list,
  ON_SubD& subd
  )
{
  if ( false == element_list.Reset())
    return ON_SUBD_RETURN_ERROR(false);

  int major_version = 1;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version))
    return ON_SUBD_RETURN_ERROR(false);

  bool rc = false;
  for (;;)
  {
    if ( 1 != major_version)
      break;

    unsigned short level_index = 0;
    if (!archive.ReadShort(&level_index))
      break;
    m_level_index = level_index;

    // from early days when there was a possibility of different types of subdivision algorithm
    unsigned char ignored_c[3] = {};
    if (!archive.ReadChar(&ignored_c[0]))
      break;
    if (!archive.ReadChar(&ignored_c[1]))
      break;
    if (!archive.ReadChar(&ignored_c[2]))
      break;   

    ON_BoundingBox controlnet_bbox;
    if (!archive.ReadDouble(3, controlnet_bbox[0]))
      break;
    if (!archive.ReadDouble(3, controlnet_bbox[1]))
      break;
    if (controlnet_bbox.IsValid())
    {
      m_aggregates.m_bDirtyBoundingBox = false;
      m_aggregates.m_controlnet_bbox = controlnet_bbox;
    }
    else
    {
      m_aggregates.m_bDirtyBoundingBox = true;
    }

    if (!archive.ReadInt(4,element_list.m_archive_id_partition))
      break;

    unsigned int archive_id = 0;

    for (archive_id = element_list.m_archive_id_partition[0]; archive_id < element_list.m_archive_id_partition[1]; archive_id++ )
    {
      ON_SubDVertex* v = nullptr;
      if ( false == ON_SubDVertex::Read(archive, subd, v) )
        break;
      if ( nullptr == v )
        break;
      if (archive_id != v->ArchiveId())
        break;
      if ( !element_list.Add(v) )
        break;
      AddVertex(v);      
    }
    if ( archive_id != element_list.m_archive_id_partition[1] )
      break;    

    for (archive_id = element_list.m_archive_id_partition[1]; archive_id < element_list.m_archive_id_partition[2]; archive_id++ )
    {
      ON_SubDEdge* e = nullptr;
      if ( false == ON_SubDEdge::Read(archive, subd, e) )
        break;
      if ( nullptr == e )
        break;
      if (archive_id != e->ArchiveId())
        break;
      if ( !element_list.Add(e) )
        break;
      AddEdge(e);
    }
    if ( archive_id != element_list.m_archive_id_partition[2] )
      break;

    for (archive_id = element_list.m_archive_id_partition[2]; archive_id < element_list.m_archive_id_partition[3]; archive_id++ )
    {
      ON_SubDFace* f = nullptr;
      if ( false == ON_SubDFace::Read(archive, subd, f) )
        break;
      if ( nullptr == f )
        break;
      if (archive_id != f->ArchiveId())
        break;
      if ( !element_list.Add(f) )
        break;
      AddFace(f);
    }
    if ( archive_id != element_list.m_archive_id_partition[3] )
      break;

    if (archive_id != element_list.Count())
      break;

    // Convert archive_id references to runtime pointers.
    archive_id = element_list.ConvertArchiveIdsToRuntimePointers();
    if ( archive_id <= 0 )
      break;    

    if (0 == minor_version )
      break;

    unsigned char c = 0;
    if (!archive.ReadChar(&c))
      break;

    if (1 == c)
    {
      //if (!m_limit_mesh.Read(archive))
      //  break; 
    }

    rc = true;
    break;
  }

  ClearArchiveId();

  if (!archive.EndRead3dmChunk())
    rc = false;

  if (rc)
    return rc;
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDimple::Write(
  ON_BinaryArchive& archive
  ) const
{
  const_cast< ON_SubDHeap* >(&m_heap)->ClearArchiveId();

  const int minor_version = (archive.Archive3dmVersion() < 70) ? 0 : 3;
  if ( !archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, minor_version) )
    return ON_SUBD_RETURN_ERROR(false);
  bool rc = false;
  for (;;)
  {
    unsigned int level_count = m_levels.UnsignedCount();
    unsigned int level_index;
    for (level_index = 0; level_index < level_count; level_index++)
    {
      if (nullptr == m_levels[level_index])
      {
        level_count = level_index;
        break;
      }
    }
    if (!archive.WriteInt(level_count))
      break;

    if (!archive.WriteInt(MaximumVertexId()))
      break;
    if (!archive.WriteInt(MaximumEdgeId()))
      break;
    if (!archive.WriteInt(MaximumFaceId()))
      break;

    // a global bounding box was saved before May 2015.
    // Something has to be here so file IO is not broken.
    if (!archive.WriteBoundingBox(ON_BoundingBox::EmptyBoundingBox))
      break;

    for (level_index = 0; level_index < level_count; level_index++)
    {
      if ( !m_levels[level_index]->Write(*this,archive) )
        break;
    }
    if (level_index < level_count)
      break;


    if (minor_version <= 0)
    {
      rc = true;
      break;
    }

    // minor version = 1 addtions
    const unsigned char texture_domain_type = static_cast<unsigned char>(TextureDomainType());
    if (false == archive.WriteChar(texture_domain_type))
      break;

    if (false == m_texture_mapping_tag.Write(archive))
      break;

    // minor version = 2 addtions
    if (false == m_symmetry.Write(archive))
      break;

    // runtime content number used to compare with the on on m_symmetry
    if (false == archive.WriteBigInt(ContentSerialNumber()))
      break;

    rc = true;
    break;
  }
  if (!archive.EndWrite3dmChunk())
    rc = false;
  if (rc)
    return true;
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDimple::Read(
  ON_BinaryArchive& archive,
  class ON_SubD& subd
  )
{
  m_heap.Clear();
  int major_version = 0;
  int minor_version = 0;
  if ( !archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version) )
    return ON_SUBD_RETURN_ERROR(false);
  bool rc = false;


  // Code before Feb 10, 2020 cared about these values
  unsigned int obsolete_archive_max_vertex_id = 0;
  unsigned int obsolete_archive_max_edge_id = 0;
  unsigned int obsolete_archive_max_face_id = 0;
  ON__UINT64 saved_content_serial_number = 0;

  for (;;)
  {
    if (1 != major_version)
      break;

    unsigned int i;
    if (!archive.ReadInt(&i))
      break;
    const unsigned int level_count = i;

    if (!archive.ReadInt(&obsolete_archive_max_vertex_id))
      break;
    if (!archive.ReadInt(&obsolete_archive_max_edge_id))
      break;
    if (!archive.ReadInt(&obsolete_archive_max_face_id))
      break;


    ON_BoundingBox bbox_unsued_after_May_2015;
    if (!archive.ReadBoundingBox(bbox_unsued_after_May_2015))
      break;

    ON_SubDArchiveIdMap element_list;

    unsigned int level_index;
    for (level_index = 0; level_index < level_count; level_index++)
    {
      ON_SubDLevel* level = SubDLevel(level_index,true);
      if ( nullptr == level )
        break;
      if (false == level->Read(archive, element_list, subd ) )
        break;
      m_active_level = level;
    }

    if ( level_index != level_count)
      break;

    if (minor_version >= 1)
    {
      unsigned char texture_domain_type = static_cast<unsigned char>(ON_SubDTextureDomainType::Unset);
      if (false == archive.ReadChar(&texture_domain_type))
        break;
      m_texture_domain_type = ON_SubD::TextureDomainTypeFromUnsigned(texture_domain_type);

      if (false == m_texture_mapping_tag.Read(archive))
        break;

      if (minor_version >= 2)
      {
        if (false == m_symmetry.Read(archive))
          break;

        if (minor_version >= 3)
        {
          if (false == archive.ReadBigInt(&saved_content_serial_number))
            break;
        }
      }
    }

    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;

  // Heap id validation. Always an error if max_heap_..._id > m_max_..._id.
  if (false == m_heap.IsValid(false,nullptr))
  {
    ON_SUBD_ERROR("m_heap.IsValid() is false.");
    m_heap.ResetIds(); // breaks component id references, but this is a serious error.
  }

  const bool bSetSymmetricObjectContentSerialNumber
    = saved_content_serial_number > 0
    && m_symmetry.IsSet()
    && saved_content_serial_number == m_symmetry.SymmetricObjectContentSerialNumber()
    ;

  ChangeContentSerialNumber(false);

  if ( bSetSymmetricObjectContentSerialNumber )
    m_symmetry.SetSymmetricObjectContentSerialNumber(ContentSerialNumber());
  else
    m_symmetry.ClearSymmetricObjectContentSerialNumber();

  if (rc)
    return true;
  return ON_SUBD_RETURN_ERROR(false);
}

//virtual
bool ON_SubD::Write(
  ON_BinaryArchive& archive
  ) const // override
{
  for (;;)
  {
    const ON_SubDimple* subdimple = SubDimple();
    unsigned char c = (nullptr == subdimple) ? 0 : 1;
    if (!archive.WriteChar(c))
      break;
    if (nullptr != subdimple)
    {
      if (!subdimple->Write(archive))
        break;
    }
    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}

//virtual
bool ON_SubD::Read(
  ON_BinaryArchive& archive
  ) // override
{
  Destroy();
  for (;;)
  {
    unsigned char c = 0;
    if (!archive.ReadChar(&c))
      break;
    if (1 == c)
    {
      ON_SubDimple* subdimple = SubDimple(true);
      if ( nullptr == subdimple)
        break;
      if (false == subdimple->Read(archive,*this))
      {
        Destroy();
        break;
      }
    }
    else if ( 0 != c )
      break;

    return true;
  }
  return ON_SUBD_RETURN_ERROR(false);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// ON_SubDMeshProxyUserData
//

ON_OBJECT_IMPLEMENT(ON_SubDMeshProxyUserData,ON_UserData,"2868B9CD-28AE-4EA7-8073-BD390B3E97C8");

const bool ON_SubDMeshProxyUserData::Internal_MeshHasFaces(const ON_Mesh* mesh)
{
  for (;;)
  {
    if (nullptr == mesh)
      break;
    if (mesh->m_F.UnsignedCount() <= 0)
      break;
    if (mesh->m_V.UnsignedCount() <= 2)
      break;
    return true;
  }
  return false;
}

const ON_SHA1_Hash ON_SubDMeshProxyUserData::Internal_FaceSHA1(const ON_Mesh* mesh)
{
  if (false == ON_SubDMeshProxyUserData::Internal_MeshHasFaces(mesh))
    return ON_SHA1_Hash::EmptyContentHash;

  ON_SHA1 sha1;
  const ON_MeshFace* f = mesh->m_F.Array();
  sha1.AccumulateBytes(f, mesh->m_F.UnsignedCount() * sizeof(*f));
  return sha1.Hash();
}

const ON_SHA1_Hash ON_SubDMeshProxyUserData::Internal_VertexSHA1(const ON_Mesh* mesh)
{
  if (false == ON_SubDMeshProxyUserData::Internal_MeshHasFaces(mesh))
    return ON_SHA1_Hash::EmptyContentHash;

  ON_SHA1 sha1;
  const ON_3fPoint* v = mesh->m_V.Array();
  sha1.AccumulateBytes(v, mesh->m_V.UnsignedCount() * sizeof(*v));
  return sha1.Hash();
}


void ON_SubDMeshProxyUserData::Internal_CopyFrom(const ON_SubDMeshProxyUserData& src)
{
  if ( src.IsValid() )
  {
    m_subd = new ON_SubD(*src.m_subd);
    m_mesh_face_count = src.m_mesh_face_count;
    m_mesh_vertex_count = src.m_mesh_vertex_count;
    m_mesh_face_array_sha1 = src.m_mesh_face_array_sha1;
    m_mesh_vertex_array_sha1 = src.m_mesh_vertex_array_sha1;
  }
}

void ON_SubDMeshProxyUserData::Internal_Destroy()
{
  if (nullptr != m_subd)
  {
    delete m_subd;
    m_subd = nullptr;
  }
  m_mesh_face_count = 0;
  m_mesh_vertex_count = 0;
  m_mesh_face_array_sha1 = ON_SHA1_Hash::EmptyContentHash;
  m_mesh_vertex_array_sha1 = ON_SHA1_Hash::EmptyContentHash;
}


const ON_SubDDisplayParameters ON_SubDMeshProxyUserData::MeshProxyDisplayParameters()
{
  return ON_SubDDisplayParameters::CreateFromDisplayDensity(4);

}

ON_Mesh* ON_SubDMeshProxyUserData::MeshProxyFromSubD(
  const ON_SubD* subd
)
{
  ON_Mesh* mesh = nullptr;
  ON_SubD* subd_copy = nullptr;

  for (;;)
  {
    if (nullptr == subd)
      break;
    subd_copy = new ON_SubD(*subd);
    if (nullptr == subd_copy)
      break;
    mesh = subd_copy->GetControlNetMesh(nullptr);
    if (false == ON_SubDMeshProxyUserData::Internal_MeshHasFaces(mesh))
      break;
    ON_SubDMeshProxyUserData* ud = new ON_SubDMeshProxyUserData();
    ud->m_subd = subd_copy;
    ud->m_mesh_face_count = mesh->FaceUnsignedCount();
    ud->m_mesh_vertex_count = mesh->VertexUnsignedCount();
    ud->m_mesh_face_array_sha1 = ON_SubDMeshProxyUserData::Internal_FaceSHA1(mesh);
    ud->m_mesh_vertex_array_sha1 = ON_SubDMeshProxyUserData::Internal_VertexSHA1(mesh);
    if (false == mesh->AttachUserData(ud))
    {
      ud->m_subd = nullptr;
      delete ud;
      break;
    }
    return mesh;
  }
  
  if (nullptr != mesh)
    delete mesh;
  
  if (nullptr != subd_copy)
    delete subd_copy;

  return nullptr;
}


ON_SubD* ON_SubDMeshProxyUserData::SubDFromMeshProxy(
  const ON_Mesh* mesh
)
{
  ON_SubD* subd = nullptr;
  ON_SubDMeshProxyUserData* ud = nullptr;
  
  for (;;)
  {
    if (nullptr == mesh)
      break;
    const ON_UUID udid = ON_CLASS_ID(ON_SubDMeshProxyUserData);
    ud = ON_SubDMeshProxyUserData::Cast(mesh->GetUserData(udid));
    if (nullptr == ud)
      break;
    if (false == ud->IsValid())
      break;
    if (false == ud->ParentMeshValid())
      break;
    subd = ud->m_subd;
    ud->m_subd = nullptr;
  }
  
  if (nullptr != ud)
    delete ud;

  return subd;
}


bool ON_SubDMeshProxyUserData::IsSubDMeshProxy(
  const ON_Mesh* mesh
)
{
  return false;
}


ON_SubDMeshProxyUserData::ON_SubDMeshProxyUserData()
{
  m_userdata_uuid = ON_CLASS_ID(ON_SubDMeshProxyUserData);
  m_application_uuid = ON_opennurbs6_id; // opennurbs.dll reads/writes this userdata
                                         // The id must be the version 4 id because
                                         // V5 SaveAs V4 needs to work.
  m_userdata_copycount = 1;
}


ON_SubDMeshProxyUserData::~ON_SubDMeshProxyUserData()
{
  Internal_Destroy();
}


ON_SubDMeshProxyUserData::ON_SubDMeshProxyUserData(const ON_SubDMeshProxyUserData& src)
  : ON_UserData(src)
{
  Internal_CopyFrom(src);
}


ON_SubDMeshProxyUserData& ON_SubDMeshProxyUserData::operator=(const ON_SubDMeshProxyUserData& src)
{
  if (this != &src)
  {
    Internal_Destroy();
    Internal_CopyFrom(src);
  }
  return *this;
}
 
bool ON_SubDMeshProxyUserData::Write(ON_BinaryArchive& archive) const
{
  const int chunk_version = 1;
  if ( false == archive.BeginWrite3dmAnonymousChunk(chunk_version) )
    return false;

  bool rc = false;

  for (;;)
  {
    const bool bIsValid = IsValid();
    if (!archive.WriteBool(bIsValid))
      break;

    if (false == bIsValid)
    {
      rc = true;
      break;
    }

    if (!m_subd->Write(archive))
      break;
    if (!archive.WriteInt(m_mesh_face_count))
      break;
    if (!archive.WriteInt(m_mesh_vertex_count))
      break;
    if (!m_mesh_face_array_sha1.Write(archive))
      break;
    if (!m_mesh_vertex_array_sha1.Write(archive))
      break;

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool ON_SubDMeshProxyUserData::Read(ON_BinaryArchive& archive)
{
  Internal_Destroy();

  int chunk_version = 0;
  if ( false == archive.BeginRead3dmAnonymousChunk(&chunk_version) )
    return false;

  bool rc = false;

  for (;;)
  {
    if (chunk_version <= 0)
      break;
    bool bIsValid = false;

    if (!archive.ReadBool(&bIsValid))
      break;

    if (false == bIsValid)
    {
      rc = true;
      break;
    }

    m_subd = new ON_SubD();
    if (!m_subd->Read(archive))
      break;
    if (!archive.ReadInt(&m_mesh_face_count))
      break;
    if (!archive.ReadInt(&m_mesh_vertex_count))
      break;
    if (!m_mesh_face_array_sha1.Read(archive))
      break;
    if (!m_mesh_vertex_array_sha1.Read(archive))
      break;

    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;
  
  if (!rc || !IsValid())
    Internal_Destroy();

  return rc;
}


bool ON_SubDMeshProxyUserData::ParentMeshValid() const
{
  for (;;)
  {
    if (!IsValid())
      break;

    const ON_Mesh* mesh = ON_Mesh::Cast(Owner());
    if (false == ON_SubDMeshProxyUserData::Internal_MeshHasFaces(mesh))
      break;

    if (m_mesh_face_count != mesh->m_F.UnsignedCount())
      break;

    if (m_mesh_vertex_count != mesh->m_V.UnsignedCount())
      break;

    const ON_SHA1_Hash f_sha1 = ON_SubDMeshProxyUserData::Internal_FaceSHA1(mesh);
    if (f_sha1 != m_mesh_face_array_sha1)
      break;
    
    const ON_SHA1_Hash v_sha1 = ON_SubDMeshProxyUserData::Internal_VertexSHA1(mesh);
    if (v_sha1 != m_mesh_vertex_array_sha1)
      break;

    return true;
  }

  m_mesh_face_count = 0;
  m_mesh_vertex_count = 0;
  m_mesh_face_array_sha1 = ON_SHA1_Hash::EmptyContentHash;
  m_mesh_vertex_array_sha1 = ON_SHA1_Hash::EmptyContentHash;

  return false;
}

bool ON_SubDMeshProxyUserData::IsValid(
  class ON_TextLog* text_log
) const
{
  for (;;)
  {
    if (nullptr == m_subd)
      break;
    if (m_mesh_face_count <= 0 )
      break;
    if (m_mesh_vertex_count <= 2 )
      break;
    if (ON_SHA1_Hash::EmptyContentHash == m_mesh_face_array_sha1)
      break;
    if (ON_SHA1_Hash::EmptyContentHash == m_mesh_vertex_array_sha1)
      break;
    if (false == m_userdata_xform.IsIdentity())
      break;

    return true;
  }

  return false;
}


bool ON_SubDMeshProxyUserData::GetDescription(ON_wString& description)
{
  if (IsValid())
    description = L"SubD attached to a valid proxy mesh.";
  else
    description = L"SubD attached to an invalid proxy mesh.";
  return true;
}


bool ON_SubDMeshProxyUserData::WriteToArchive(
  const class ON_BinaryArchive& archive,
  const class ON_Object* parent_object
) const
{
  for (;;)
  {
    if (archive.Archive3dmVersion() < 60)
      break;
    if (false == IsValid())
      return false;
    if (false == ParentMeshValid())
      return false;
    return true;
  }

  return false;
}
 
