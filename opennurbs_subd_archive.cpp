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

static bool WriteDouble3(
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

static bool ReadDouble3(
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

static bool WriteBase(
  const ON_SubDComponentBase* base,
  ON_BinaryArchive& archive
  )
{
  for (;;)
  {
    unsigned int archive_id = base->ArchiveId();
    unsigned int id = base->m_id;
    unsigned short level = base->m_level;
    if (!archive.WriteInt(archive_id))
      break;
    if (!archive.WriteInt(id))
      break;
    if (!archive.WriteShort(level))
      break;

    double P[3], V[3];
    bool bHaveP = false;
    bool bHaveV = false;
    ON_SubD::SubDType subd_P_type = base->SavedSubdivisionPointType();
    ON_SubD::SubDType subd_V_type = base->DisplacementType();
    if (ON_SubD::SubDType::Unset != subd_P_type)
      bHaveP = base->GetSavedSubdivisionPoint(subd_P_type, P);

    if (ON_SubD::SubDType::Unset != subd_V_type)
      bHaveV = base->GetSavedSubdivisionPoint(subd_V_type, V);

    unsigned char cP = bHaveP ? ((unsigned char)subd_P_type) : 0U;
    if (!archive.WriteChar(cP))
      break;
    if (0 != cP)
    {
      if (!WriteDouble3(P,archive))
        break;
    }

    unsigned char cV = bHaveV ? ((unsigned char)subd_V_type) : 0U;
    if (!archive.WriteChar(cV))
      break;
    if (0 != cV)
    {
      if (!WriteDouble3(V,archive))
        break;
    }

    return true;
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

    unsigned char cP = 0U;
    unsigned char cV = 0U;
    double P[3], V[3];

    if (!archive.ReadChar(&cP))
      break;
    if (0 != cP)
    {
      if (!ReadDouble3(archive,P))
        break;
    }

    if (!archive.ReadChar(&cV))
      break;
    if (0 != cV)
    {
      if (!ReadDouble3(archive,V))
        break;
    }

    base.m_id = id;
    base.SetArchiveId(archive_id);
    base.m_level = level;
    if ( 0 != cP )
      base.SetSavedSubdivisionPoint(ON_SubD::SubDTypeFromUnsigned(cP),P);
    if ( 0 != cV )
      base.SetDisplacement(ON_SubD::SubDTypeFromUnsigned(cV),V);


    return true;
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
  unsigned char flags = (unsigned char)ON_SUBD_ELEMENT_FLAGS(ptr_flags);
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
  element_ptr *= (ON_SUBD_ELEMENT_FLAGS_MASK+1);
  element_ptr += (flags & ON_SUBD_ELEMENT_FLAGS_MASK);
  return true;
}

static bool WriteSavedLimitPointList(
  unsigned int vertex_face_count,
  ON_SubD::SubDType subd_type,
  const ON_SubDSectorLimitPoint& limit_point,
  ON_BinaryArchive& archive
  )
{
  unsigned int limit_point_count = 0;
  const ON_SubDSectorLimitPoint* p;

  if (ON_SubD::SubDType::Unset != subd_type)
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
  if ( 0 == limit_point_count )
    subd_type = ON_SubD::SubDType::Unset;

  for (;;)
  {
    unsigned char c = (unsigned char)subd_type;
    if (!archive.WriteChar(c))
      break;

    if (0 == c)
      return true;

    if (!archive.WriteInt(limit_point_count))
      break;
    
    p = &limit_point;
    for (unsigned int i = 0; i < limit_point_count; i++, p = p->m_next_sector_limit_point )
    {
      if (!WriteDouble3(limit_point.m_limitP, archive))
        break;
      if (!WriteDouble3(limit_point.m_limitT1, archive))
        break;
      if (!WriteDouble3(limit_point.m_limitT2, archive))
        break;
      if (!WriteDouble3(limit_point.m_limitN, archive))
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
  ON_SubD::SubDType& limitP_type,
  ON_SimpleArray< ON_SubDSectorLimitPoint > limit_points
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
      ON_SubDSectorLimitPoint limit_point = ON_SubDSectorLimitPoint::Unset;
      if (!ReadDouble3(archive,limit_point.m_limitP))
        break;
      if (!ReadDouble3(archive,limit_point.m_limitT1))
        break;
      if (!ReadDouble3(archive,limit_point.m_limitT2))
        break;
      if (!ReadDouble3(archive,limit_point.m_limitN))
        break;
      ON_SubDFacePtr fptr = ON_SubDFacePtr::Null;
      if (!ReadArchiveIdAndFlagsIntoComponentPtr(archive,fptr.m_ptr))
        break;
      limit_points.Append(limit_point);
    }

    if (limit_point_count != limit_points.UnsignedCount() )
      break;

    limitP_type = ON_SubD::SubDTypeFromUnsigned(c);
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

static bool SkipReadingLaterAdditions(
  ON_BinaryArchive& archive,
  unsigned char skip_mark
  )
{
  if ( 0 == skip_mark)
    return true;

  if (1 == skip_mark)
  {
    // TODO ADD THIS // return archive.SkipReadingChunk(); //
    return ON_SUBD_RETURN_ERROR(false);
  }

  // TODO ADD THIS return archive.SkipReadingBytes( skip_mark );
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
    if (!WriteDouble3(m_P,archive))
      break;
    if (!archive.WriteShort(m_edge_count))
      break;
    if (!archive.WriteShort(m_face_count))
      break;
    if (!WriteSavedLimitPointList(m_face_count,SavedLimitPointType(),m_limit_point, archive))
      break;
    if (!WriteEdgePtrList(m_edge_count,m_edge_capacity,m_edges,0,nullptr, archive))
      break;
    if (!WriteFacePtrList(m_face_count,m_face_capacity,(const ON_SubDFacePtr*)m_faces,0,nullptr, archive))
      break;

    // mark end with a 0 byte
    // If (when) new stuff is added, the value will be the number of bytes that are added
    // or 1 if a chunk is added.
    if (!archive.WriteChar((unsigned char)0U))
      break;
    return true;
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

    ON_SubD::SubDType limitP_type = ON_SubD::SubDType::Unset;
    ON_SimpleArray<ON_SubDSectorLimitPoint> limit_points;

    if (!ReadBase(archive,base))
      break;
    if (!archive.ReadChar(&vertex_tag))
      break;
    //if (!archive.ReadChar(&vertex_edge_order))
    //  break;
    //if (!archive.ReadChar(&vertex_facet_type))
    //  break;
    if (!ReadDouble3(archive,P))
      break;
    if (!archive.ReadShort(&edge_count))
      break;
    if (!archive.ReadShort(&face_count))
      break;

    if (!ReadSavedLimitPointList(archive, face_count, limitP_type, limit_points))
      break;

    ON_SubDVertex* v = subdimple->AllocateVertex(
      ON_SubD::VertexTagFromUnsigned(vertex_tag),
      base.m_level,
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


    unsigned char skip_mark = 0;
    if (!archive.ReadChar(&skip_mark))
      break;

    if (!SkipReadingLaterAdditions(archive,skip_mark))
      break;

    if (ON_SubD::SubDType::Unset != limitP_type)
    {
      for (unsigned int i = 0; i < limit_points.UnsignedCount(); i++)
      {
        ON_SubDSectorLimitPoint limit_point = limit_points[i];
        limit_point.m_next_sector_limit_point = (const ON_SubDSectorLimitPoint*)1U; // skips checks
        if (false == v->SetSavedLimitPoint(limitP_type, limit_point))
        {
          v->ClearSavedLimitPoints();
          break;
        }
      }
    }

    vertex = v;

    return true;
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

    // mark end with a 0 byte
    // If (when) new stuff is added, the value will be the number of bytes that are added
    // or 1 if a chunk is added.
    if (!archive.WriteChar((unsigned char)0U))
      break;

    return true;
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
      ON_SubD::EdgeTagFromUnsigned(edge_tag),
      base.m_level,
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
   
    unsigned char skip_mark = 0;
    if (!archive.ReadChar(&skip_mark))
      break;

    if (!SkipReadingLaterAdditions(archive,skip_mark))
      break;

    edge = e;

    return true;

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
    if (!archive.WriteInt(m_zero_face_id))
      break;
    if (!archive.WriteInt(m_parent_face_id))
      break;
    if (!archive.WriteShort(m_edge_count))
      break;
    if (!WriteEdgePtrList(m_edge_count,sizeof(m_edge4)/sizeof(m_edge4[0]),m_edge4,m_edgex_capacity,m_edgex, archive))
      break;

    // mark end with a 0 byte
    // If (when) new stuff is added, the value will be the number of bytes that are added
    // or 1 if a chunk is added.
    if (!archive.WriteChar((unsigned char)0U))
      break;

    return true;
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
    unsigned int zero_face_id = 0;
    unsigned int parent_face_id = 0;
    unsigned short edge_count = 0;

    if (!ReadBase(archive,base))
      break;
    if (!archive.ReadInt(&zero_face_id))
      break;
    if (!archive.ReadInt(&parent_face_id))
      break;
    if (!archive.ReadShort(&edge_count))
      break;

    ON_SubDFace* f = subdimple->AllocateFace(
      base.m_level,
      edge_count
      );

    if ( nullptr == f )
      break;

    f->ON_SubDComponentBase::operator=(base);

    f->m_zero_face_id = zero_face_id;
    f->m_parent_face_id = parent_face_id;

    if (!ReadEdgePtrList(archive,edge_count,sizeof(f->m_edge4)/sizeof(f->m_edge4[0]),f->m_edge4,f->m_edgex_capacity,f->m_edgex))
      break;
    f->m_edge_count = edge_count;
    
    unsigned char skip_mark = 0;
    if (!archive.ReadChar(&skip_mark))
      break;

    if (!SkipReadingLaterAdditions(archive,skip_mark))
      break;

    face = f;

    return true;

  }
  return ON_SUBD_RETURN_ERROR(false);
}

unsigned int ON_SubDLevel::SetArchiveId(
  unsigned int archive_id_partition[4]
  ) const
{
  unsigned int archive_id = 1;
  //archive_id_partition[0] = 0;
  //archive_id_partition[1] = 0;
  //archive_id_partition[2] = 0;
  //archive_id_partition[3] = 0;

  archive_id_partition[0] = archive_id;
  for (const ON_SubDVertex* v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
  {
    v->SetArchiveId(archive_id++);
  }
  archive_id_partition[1] = archive_id;
  for (const ON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
  {
    e->SetArchiveId(archive_id++);
  }
  archive_id_partition[2] = archive_id;
  for (const ON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
  {
    f->SetArchiveId(archive_id++);
  }
  archive_id_partition[3] = archive_id;

  return archive_id-1;
}


void ON_SubDLevel::ClearArchiveId() const
{
  for (const ON_SubDVertex* v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
  {
    v->SetArchiveId(0);
  }
  for (const ON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
  {
    e->SetArchiveId(0);
  }
  for (const ON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
  {
    f->SetArchiveId(0);
  }
}

bool ON_SubDLevel::Write(
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
    if (!archive.WriteChar((unsigned char)m_subdivision_type))
      break;
    if (!archive.WriteChar(m_ordinary_vertex_valence))
      break;
    if (!archive.WriteChar(m_ordinary_face_edge_count))
      break;   
    ON_BoundingBox bbox = m_aggregates.m_bDirtyBoundingBox ? ON_BoundingBox::EmptyBoundingBox : m_aggregates.m_bbox;
    if (!archive.WriteDouble(3,bbox[0]))
      break;
    if (!archive.WriteDouble(3,bbox[1]))
      break;


    unsigned int archive_id_partition[4] = { 0 };
    SetArchiveId(archive_id_partition);

    if (!archive.WriteInt(4,archive_id_partition))
      break;

    const ON_SubDVertex* v = nullptr;
    const ON_SubDEdge* e = nullptr;
    const ON_SubDFace* f = nullptr;
    for (v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
    {
      if( !v->Write(archive) )
        break;
    }
    if ( nullptr != v )
      break;

    for (e = m_edge[0]; nullptr != e; e = e->m_next_edge)
    {
      if( !e->Write(archive) )
        break;
    }
    if ( nullptr != e )
      break;

    for (f = m_face[0]; nullptr != f; f = f->m_next_face)
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
      if (false == m_limit_mesh.IsEmpty())
      {
        c = 0;
        // c = 1; TODO change to c = 1 when ON_SubDLimitMesh::Write()/Read() actually work
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
    unsigned char c = 0;
    if (!archive.ReadChar(&c))
      break;
    m_subdivision_type = ON_SubD::SubDTypeFromUnsigned(c);
    if (!archive.ReadChar(&m_ordinary_vertex_valence))
      break;
    if (!archive.ReadChar(&m_ordinary_face_edge_count))
      break;   
    ON_BoundingBox bbox;
    if (!archive.ReadDouble(3,bbox[0]))
      break;
    if (!archive.ReadDouble(3,bbox[1]))
      break;
    if (bbox.IsValid())
    {
      m_aggregates.m_bDirtyBoundingBox = false;
      m_aggregates.m_bbox = bbox;
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

    c = 0;
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

  if ( !archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 0) )
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

    if (!archive.WriteInt(m_max_vertex_id))
      break;
    if (!archive.WriteInt(m_max_edge_id))
      break;
    if (!archive.WriteInt(m_max_face_id))
      break;

    // a global bounding box was saved before May 2015.
    // Something has to be here so file IO is not broken.
    if (!archive.WriteBoundingBox(ON_BoundingBox::EmptyBoundingBox))
      break;

    for (level_index = 0; level_index < level_count; level_index++)
    {
      if ( !m_levels[level_index]->Write(archive) )
        break;
    }
    if (level_index < level_count)
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

  unsigned int max_vertex_id = 0;
  unsigned int max_edge_id = 0;
  unsigned int max_face_id = 0;

  for (;;)
  {
    if (1 != major_version)
      break;

    unsigned int i;
    if (!archive.ReadInt(&i))
      break;
    const unsigned int level_count = i;

    if (!archive.ReadInt(&max_vertex_id))
      break;
    if (!archive.ReadInt(&max_edge_id))
      break;
    if (!archive.ReadInt(&max_face_id))
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

    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;

  m_max_vertex_id = max_vertex_id;
  m_max_edge_id = max_edge_id;
  m_max_face_id = max_face_id;

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
    const ON_SubDDisplayParameters dp = ON_SubDMeshProxyUserData::MeshProxyDisplayParameters();
    mesh = subd_copy->GetLimitSurfaceMesh(dp, nullptr);
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
 

#endif
