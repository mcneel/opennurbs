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

struct ON_MeshNGonEdge
{
  unsigned int i;
  unsigned int j;
  unsigned int Ni;
  unsigned int Nj;
  unsigned int ngon_index;
  ON_SubD::EdgeTag edge_tag;
  class ON_SubDEdge* e;
};

static int compareUnorderedEdge(const void* a, const void* b)
{
  // compare location ids
  unsigned int ea[2] = { ((const unsigned int*)a)[0], ((const unsigned int*)a)[1] };
  unsigned int eb[2] = { ((const unsigned int*)b)[0], ((const unsigned int*)b)[1] };

  // unordered compare
  unsigned int k;
  if (ea[0] > ea[1])
  {
    k = ea[0];
    ea[0] = ea[1];
    ea[1] = k;
  }
  if (eb[0] > eb[1])
  {
    k = eb[0];
    eb[0] = eb[1];
    eb[1] = k;
  }

  // compare
  if (ea[0] < eb[0])
    return -1;
  if (ea[0] > eb[0])
    return 1;

  if (ea[1] < eb[1])
    return -1;
  if (ea[1] > eb[1])
    return 1;

  return 0;
}

static bool TagCoincidentEdgeAsCrease(
  const ON_MeshNGonEdge& a,
  const ON_MeshNGonEdge& b
  )
{
  if (a.i == b.i && a.j == b.j)
  {
    // a and b are coincident and have the same direction
    if (a.Ni != b.Ni && a.Nj != b.Nj)
      return true;
  }
  else if (a.i == b.j && a.j == b.i)
  {
    // a and b are coincident and have opposite directions
    if (a.Ni != b.Nj && a.Nj != b.Ni)
      return true;
  }
  else
  {
    // a and b are not coincident
    // The calling code expects a and be to be coninicdent
    ON_SubDIncrementErrorCount();
  }

  return false;
}

ON_SubD* ON_SubD::CreateFromMesh(
  const class ON_Mesh* level_zero_mesh,
  const class ON_SubDFromMeshOptions* from_mesh_options,
  ON_SubD* subd
  )
{
  if (nullptr != subd)
  {
    ON_SubDimple* subdimple = subd->SubDimple(false);
    if (nullptr != subdimple)
      subdimple->Clear();
  }

  if (nullptr == level_zero_mesh)
    return nullptr;

  ON_Workspace ws;

  ON_3dPointListRef mesh_points(level_zero_mesh);
  const unsigned int mesh_point_count = mesh_points.PointCount();
  if (mesh_point_count < 3)
    return nullptr;

  const ON_MeshFaceList mesh_face_list(level_zero_mesh);
  const unsigned int mesh_face_count = mesh_face_list.FaceCount();
  if ( mesh_face_count < 1 )
    return nullptr;

  const ON_3fVector* pointNormal
    = level_zero_mesh->HasVertexNormals()
    ? level_zero_mesh->m_N.Array()
    : nullptr;

  const_cast<ON_Mesh*>(level_zero_mesh)->NgonMap(true);
  ON_MeshNgonIterator ngonit(level_zero_mesh);
  if (nullptr == ngonit.FirstNgon())
    return nullptr;
  
  unsigned int* Vindex = (unsigned int*)ws.GetIntMemory(mesh_point_count);
  unsigned int* Vid = level_zero_mesh->GetVertexLocationIds(0, (unsigned int*)ws.GetIntMemory(mesh_point_count), Vindex);
  if (nullptr == Vid)
    return nullptr;
  unsigned int VidCount = Vid[Vindex[mesh_point_count - 1]] + 1;
  unsigned char* vertexIsReferenced = (unsigned char*)ws.GetMemory(VidCount*sizeof(vertexIsReferenced[0]));
  memset(vertexIsReferenced, 0, VidCount*sizeof(vertexIsReferenced[0]));
  // Vid[]
  //   Vid[] has mesh_point_count values.
  //   Vid[i] = Vid[j] if and only if mesh->m_V[i] and mesh->m_V[j] are coincident.
  //   Values in Vid[] run from 0 to VidCount-1.
  //   There are VidCount unique locations.
  // Vindex[] is a permutation of (0, ..., mesh_point_count-1)
  //   0 == Vid[Vindex[0]] <= ... <= Vid[Vindex[mesh_point_count-1]] = VidCount-1.


  //const bool bConcaveCornerTest 
  //  =  nullptr != crease_parameters 
  //  && crease_parameters->ConcaveCornerTestIsEnabled();

  //const double min_cos_concave_corner_angle 
  //  = bConcaveCornerTest
  //  ? (crease_parameters->MaximumConcaveCornerAngleRadians() < ON_PI ? cos(crease_parameters->MaximumConcaveCornerAngleRadians()) : -2.0)
  //  : 2.0;

  double max_cos_crease_angle = ON_UNSET_VALUE;
  double min_crease_angle_radians = -ON_UNSET_VALUE;

  ON_SubDFromMeshOptions::InteriorCreaseOption crease_test 
    = (nullptr != from_mesh_options)
    ? from_mesh_options->InteriorCreaseTest()
    : ON_SubDFromMeshOptions::InteriorCreaseOption::None;

  if (ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshCrease == crease_test && nullptr != pointNormal )
  {
    double min_angle = from_mesh_options->MinimumCreaseAngleRadians();
    if (min_angle >= 0.0 && min_angle < ON_PI)
    {
      min_crease_angle_radians = min_angle;
      if ( 0.0 == min_crease_angle_radians)
        max_cos_crease_angle = 1.0;
      else
      {
        max_cos_crease_angle = cos(min_crease_angle_radians);
        if ( max_cos_crease_angle >= 1.0 )
          max_cos_crease_angle = 1.0-ON_EPSILON;
      }
    }
    else
    {
      crease_test = ON_SubDFromMeshOptions::InteriorCreaseOption::None;
    }
  }
  else if (ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshEdge != crease_test)
  {
    crease_test = ON_SubDFromMeshOptions::InteriorCreaseOption::None;
  }
  
  // Get sub-D edge list
  unsigned int subd_vertex_count = 0;
  unsigned int mesh_edge_count = 0;
  unsigned int max_subd_face_edge_count = 0;
  ON_SimpleArray<struct ON_MeshNGonEdge> mesh_edges(4 * level_zero_mesh->m_F.UnsignedCount());
  struct ON_MeshNGonEdge mesh_edge = {};

  unsigned int quad_vi[4];
  ON_MeshNGonEdge quad_edges[4] = {};

  bool bMergeColinearEdges = false;

  const ON_MeshFaceList level_zero_mesh_face_list(level_zero_mesh);

  unsigned int subd_face_index = 0;
  for (const ON_MeshNgon* ngon = ngonit.FirstNgon(); nullptr != ngon; ngon = ngonit.NextNgon())
  {
    if (ngon->m_Vcount < 3 || ngon->m_Fcount < 1)
      continue;

    const int ngon_orientation = ngon->Orientation(level_zero_mesh_face_list, false);

    if (0 != ngon_orientation)
    {
      mesh_edge.ngon_index = subd_face_index;

      unsigned int ngon_edge_count = 0;
      mesh_edge.j = ngon->m_vi[0];
      for (unsigned int nvi = 1; nvi <= ngon->m_Vcount; nvi++)
      {
        mesh_edge.i = mesh_edge.j;
        mesh_edge.j = ngon->m_vi[nvi % ngon->m_Vcount];
        if (Vid[mesh_edge.i] == Vid[mesh_edge.j])
          continue;
        mesh_edges.Append(mesh_edge);
        ngon_edge_count++;
      }
      if (ngon_edge_count < 3)
      {
        mesh_edges.SetCount(mesh_edge_count);
        continue;
      }

      if (ngon_orientation < 0)
      {
        // ngon and mesh have opposite orientations - mesh orientation wins
        // reverese edges
        unsigned int i0 = mesh_edge_count;
        unsigned int i1 = mesh_edge_count + ngon_edge_count - 1;
        while (i0 < i1)
        {
          mesh_edge = mesh_edges[i0];
          mesh_edges[i0] = mesh_edges[i1];
          int k = mesh_edge.i;
          mesh_edge.i = mesh_edge.j;
          mesh_edge.j = k;
          mesh_edges[i1] = mesh_edge;
          k = mesh_edges[i0].i;
          mesh_edges[i0].i = mesh_edges[i0].j;
          mesh_edges[i0].j = k;
          i0++;
          i1--;
        }
      }

      // the ngon created a single subd face
      subd_face_index++;

      if (ngon_edge_count >= 4)
        bMergeColinearEdges = true;

      if (mesh_edges.UnsignedCount() - mesh_edge_count > max_subd_face_edge_count)
        max_subd_face_edge_count = mesh_edges.UnsignedCount() - mesh_edge_count;
    }
    else if ( ngon->m_Fcount >= 1 )
    {
      // This generally happens when the "ngon" has holes and it cannot be used as a subd control net polygon.
      //
      // Each tri or quad in the ngon will get added as a subd face.
      for (unsigned int nfi = 0; nfi < ngon->m_Fcount; nfi++)
      {
        if ( nullptr == mesh_face_list.QuadFvi(ngon->m_fi[nfi],quad_vi))
          continue;

        unsigned int quad_edge_count = 0;        
        mesh_edge.ngon_index = subd_face_index;
        mesh_edge.j = quad_vi[0];
        for (unsigned int fvi = 1; fvi <= 4; fvi++)
        {
          mesh_edge.i = mesh_edge.j;
          mesh_edge.j = quad_vi[fvi % 4];
          if (Vid[mesh_edge.i] == Vid[mesh_edge.j])
            continue;
          quad_edges[quad_edge_count++] = mesh_edge;
        }
        if (quad_edge_count >= 3)
        {
          // each quad/triangle in the ON_Mesh ngon created a subd face
          mesh_edges.Append(quad_edge_count,quad_edges);
          subd_face_index++;
          if( quad_edge_count > max_subd_face_edge_count)
            max_subd_face_edge_count = quad_edge_count;
        }
      }

      if ( mesh_edge_count == mesh_edges.UnsignedCount() )
        continue;
    }


    for (/*empty init*/; mesh_edge_count < mesh_edges.UnsignedCount(); mesh_edge_count++)
    {
      mesh_edge = mesh_edges[mesh_edge_count];
      if (0 == vertexIsReferenced[Vid[mesh_edge.i]])
      {
        vertexIsReferenced[Vid[mesh_edge.i]] = 1;
        subd_vertex_count++;
      }
      if (0 == vertexIsReferenced[Vid[mesh_edge.j]])
      {
        vertexIsReferenced[Vid[mesh_edge.j]] = 1;
        subd_vertex_count++;
      }
    }
  }

  const unsigned int subd_face_count = subd_face_index;

  if (subd_vertex_count < 3 || mesh_edge_count < 3 || subd_face_count < 1)
    return nullptr;

#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_CLANG("-Wpessimizing-move")
#pragma ON_PRAGMA_WARNING_DISABLE_GNU("-Wpessimizing-move")
  // Ignore the CLang warning about preventing elision
  std::unique_ptr< ON_SubD > uptr;
  ON_SubD* new_subd
    = (nullptr != subd)
    ? subd // use subd supplied by the caller
    : (uptr = std::move(std::unique_ptr< ON_SubD >(new ON_SubD()))).get(); // new ON_SubD on the heap managed by uptr - ignore CLang warning
#pragma ON_PRAGMA_WARNING_POP

  bool bHasTaggedVertices = false;

  unsigned int* Nid = nullptr;
  unsigned int nextNid = 0;
  if (ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshCrease == crease_test)
  {
    Nid = (unsigned int*)ws.GetIntMemory(mesh_point_count);
    memset(Nid, 0, mesh_point_count*sizeof(Nid[0]));
    nextNid = 1;
  }

  ON_SimpleArray< ON_SubDVertex* > V(subd_vertex_count);
  VidCount = 0;

  for (unsigned int i = 0; i < mesh_point_count;/*empty iterator*/)
  {
    const unsigned int vid0 = Vid[Vindex[i]];
    unsigned int j;
    for (j = i + 1; j < mesh_point_count; j++)
    {
      if (vid0 != Vid[Vindex[j]])
        break;
    }

    if (1 == vertexIsReferenced[vid0])
    {
      // vertex is referenced by an edge
      if (nullptr != Nid)
      {
        // When there are 2 or more coincident vertices, 
        // set normal ids used to detect creased edges.
        // This for loop finds normals that should be considered "equal" because
        // the angle between them is <= crease_parameters->MinimumCreaseAngleRadians()
        for (unsigned int k = i; k < j; k++)
        {
          if (ON_UNSET_UINT_INDEX == Nid[Vindex[k]])
            continue;

          ON_3dVector N0 = pointNormal[Vindex[k]];
          if (false == N0.Unitize())
          {
            Nid[Vindex[k]] = ON_UNSET_UINT_INDEX;
            continue;
          }

          unsigned int thisNid = Nid[Vindex[k]];

          // search for "equal" normals
          for (unsigned int n = k + 1; n < j; n++)
          {
            if (0 != Nid[Vindex[n]] && 0 != thisNid)
              continue;
            ON_3dVector N1 = pointNormal[Vindex[n]];
            if (false == N1.Unitize())
            {
              Nid[Vindex[k]] = ON_UNSET_UINT_INDEX;
              continue;
            }
            double cos_N0_N1_angle = (N0 == N1) ? 1.0 : N0*N1;
            if (cos_N0_N1_angle >= max_cos_crease_angle)
            {
              // Angle between N0 and N1 is <= crease_parameters->MinimumCreaseAngleRadians()
              // so they must have the same id.
              if (0 == thisNid)
              {
                if (0 == Nid[Vindex[n]])
                {
                  thisNid = nextNid++;
                  Nid[Vindex[n]] = thisNid;
                }
                else
                {
                  thisNid = Nid[Vindex[n]];
                }
                Nid[Vindex[k]] = thisNid;
              }
              continue;
            }
          }

          if (0 == thisNid)
						Nid[Vindex[k]] = nextNid++;
        }
      }

      const ON_3dPoint P = mesh_points[Vindex[i]];
      ON_SubDVertex* subd_vertex = new_subd->AddVertex(ON_SubD::VertexTag::Smooth, &P.x);
      V.Append(subd_vertex);
      while (i < j)
        Vid[Vindex[i++]] = VidCount;
      VidCount++;
    }
    else
    {
      // unreferenced vertex
      while (i < j)
        Vid[Vindex[i++]] = ON_UNSET_UINT_INDEX;
    }
  }

  // change mesh_edges[].i and .j from mesh vertex index to to sub-D vertex id.
  for (unsigned int i = 0; i < mesh_edges.UnsignedCount(); i++)
  {
    // set the normal ids from the ON_Mesh m_V[] indices
    struct ON_MeshNGonEdge& mesh_edge_ref = mesh_edges[i];
    if (ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshEdge == crease_test)
    {
      // All coincident mesh vertices generate interior creases
      mesh_edge_ref.Ni = mesh_edge_ref.i;
      mesh_edge_ref.Nj = mesh_edge_ref.j;
    }
    else if (nullptr != Nid)
    {
      // Coincident mesh vertices with different vertex normals generate interior creases
      mesh_edge_ref.Ni = Nid[mesh_edge_ref.i];
      mesh_edge_ref.Nj = Nid[mesh_edge_ref.j];
    }
    else
    {
      // no interior creases
      mesh_edge_ref.Ni = 0;
      mesh_edge_ref.Nj = 0;
    }

    // convert ON_Mesh m_V[] indices into sub-D vertex ids.
    mesh_edge_ref.i = Vid[mesh_edge_ref.i];
    mesh_edge_ref.j = Vid[mesh_edge_ref.j];
  }

  // sort the edges
  unsigned int* mesh_edge_map = (unsigned int*)ws.GetMemory(mesh_edges.UnsignedCount()*sizeof(mesh_edge_map[0]));
  ON_Sort(ON::sort_algorithm::quick_sort, mesh_edge_map, mesh_edges.Array(), mesh_edges.UnsignedCount(), sizeof(mesh_edge), compareUnorderedEdge);

  // change mesh_edges[].e to a temporary edge id
  ON__UINT_PTR subd_edge_index = 0;
  for (unsigned int i = 0; i < mesh_edges.UnsignedCount(); /*empty iterator*/)
  {
    // first instance of a new edge
    mesh_edge = mesh_edges[mesh_edge_map[i]];
    mesh_edge.edge_tag = ON_SubD::EdgeTag::Smooth;
    mesh_edges[mesh_edge_map[i]].e = (ON_SubDEdge*)subd_edge_index;

    unsigned int i0 = i;
    for (i++; i < mesh_edges.UnsignedCount() && 0 == compareUnorderedEdge(&mesh_edge, &mesh_edges[mesh_edge_map[i]]); i++)
    {
      // There were multiple ON_Mesh vertices at at least one end of this edge.
      // If the crease_parmeters specified to search for a crease and the
      // angle between ON_Mesh vertex normals exceeded the crease tolerance,
      // then the edge will be a crease.
      if (ON_SubD::EdgeTag::Smooth == mesh_edge.edge_tag)
      {
        if (TagCoincidentEdgeAsCrease(mesh_edges[mesh_edge_map[i]],mesh_edge))
          mesh_edge.edge_tag = ON_SubD::EdgeTag::Crease;
      }
      mesh_edges[mesh_edge_map[i]].e = (ON_SubDEdge*)subd_edge_index; // duplicate edge
    }

    while (i0 < i)
      mesh_edges[mesh_edge_map[i0++]].edge_tag = mesh_edge.edge_tag;

    subd_edge_index++;
  }

  // Create the sub-D edges.
  for (unsigned int i = 0; i < mesh_edges.UnsignedCount(); /*empty iterator*/)
  {
    mesh_edge = mesh_edges[mesh_edge_map[i]];
    subd_edge_index = (ON__UINT_PTR)mesh_edge.e;
    // Later, some of the ON_SubD::EdgeTag::Smooth tags are changed to ON_SubD::EdgeTag::Crease or ON_SubD::EdgeTag::X.
    mesh_edge.e
      = (mesh_edge.i <= mesh_edge.j)
      ? new_subd->AddEdgeWithSectorCoefficients(mesh_edge.edge_tag, V[mesh_edge.i], ON_SubDSectorType::IgnoredSectorWeight, V[mesh_edge.j], ON_SubDSectorType::IgnoredSectorWeight)
      : new_subd->AddEdgeWithSectorCoefficients(mesh_edge.edge_tag, V[mesh_edge.j], ON_SubDSectorType::IgnoredSectorWeight, V[mesh_edge.i], ON_SubDSectorType::IgnoredSectorWeight);
    mesh_edges[mesh_edge_map[i]].e = mesh_edge.e;
    for (i++; i < mesh_edges.UnsignedCount(); i++)
    {
      if (subd_edge_index == (ON__UINT_PTR)mesh_edges[mesh_edge_map[i]].e)
      {
        mesh_edges[mesh_edge_map[i]].e = mesh_edge.e;
        continue;
      }
      break;
    }
  }

  // Create the sub-D faces.
  ON_SimpleArray< ON_SubDEdgePtr > EP(max_subd_face_edge_count);
  unsigned int mesh_edge_index = 0;
  for ( subd_face_index = 0; subd_face_index < subd_face_count; subd_face_index++ )
  {
    while (mesh_edge_index < mesh_edges.UnsignedCount() && mesh_edges[mesh_edge_index].ngon_index < subd_face_index)
      mesh_edge_index++;

    if (mesh_edges[mesh_edge_index].ngon_index != subd_face_index)
      continue;

    EP.SetCount(0);
    while (mesh_edge_index < mesh_edges.UnsignedCount() && mesh_edges[mesh_edge_index].ngon_index == subd_face_index)
    {
      mesh_edge = mesh_edges[mesh_edge_index];
      EP.Append(ON_SubDEdgePtr::Create(mesh_edge.e, mesh_edge.i <= mesh_edge.j ? 0 : 1));
      mesh_edge_index++;
    }

    if (EP.UnsignedCount() >= 3)
      new_subd->AddFace(EP.UnsignedCount(), EP.Array());
  }

  // Apply "ON_SubD::EdgeTag::Crease" tag to boundary and non-manifold edges and their vertices.
  unsigned int interior_crease_count = 0;
  for (const ON_SubDEdge* edge = new_subd->FirstEdge(); nullptr != edge; edge = edge->m_next_edge)
  {
    if (2 == edge->m_face_count && ON_SubD::EdgeTag::Smooth == edge->m_edge_tag)
      continue;

    bHasTaggedVertices = true;

    const_cast<ON_SubDEdge*>(edge)->m_edge_tag = ON_SubD::EdgeTag::Crease;
    for (unsigned int j = 0; j < 2; j++)
    {
      const ON_SubDVertex* vertex = edge->m_vertex[j];
      const_cast<ON_SubDVertex*>(vertex)->m_vertex_tag = ON_SubD::VertexTag::Crease;
    }

    if ( 2 == edge->m_face_count )
      interior_crease_count++;
  }

  if (interior_crease_count > 0)
  {
    // Any interior vertex that has exactly one creased edges must be 
    // tagged as a dart.
    unsigned int k = 0;
    for (const ON_SubDEdge* edge = new_subd->FirstEdge(); nullptr != edge; edge = edge->m_next_edge)
    {
      if (2 != edge->m_face_count || ON_SubD::EdgeTag::Crease != edge->m_edge_tag)
        continue;

      if ( ON_SubD::VertexTag::Crease != edge->m_vertex[0]->m_vertex_tag
          && ON_SubD::VertexTag::Crease != edge->m_vertex[1]->m_vertex_tag)
        continue;

      unsigned int dart_index = 0;
      unsigned int dart_count = 0;
      for (unsigned int j = 0; j < 2; j++)
      {
        const ON_SubDVertex* vertex = edge->m_vertex[j];
        bool bIsDart = false;
        for (unsigned int vei = 0; vei < vertex->m_edge_count; vei++)
        {
          const ON_SubDEdge* v_edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
          if (v_edge == edge)
          {
            bIsDart = true;
            continue;
          }
          if (nullptr == v_edge)
          {
            bIsDart = false;
            break;
          }
          if (2 != v_edge->m_face_count)
          {
            bIsDart = false;
            break;
          }
          if (ON_SubD::EdgeTag::Crease == v_edge->m_edge_tag)
          {
            bIsDart = false;
            break;
          }
        }
        if (bIsDart)
        {
          dart_count++;
          if ( 1 == dart_count )
            dart_index = j;
          else if ( 2 != dart_count )
          {
            ON_SubDIncrementErrorCount();
            break;
          }
        }
      }

      if (dart_count == 1)
      {
        const_cast<ON_SubDVertex*>(edge->m_vertex[dart_index])->m_vertex_tag = ON_SubD::VertexTag::Dart;
        k++;
        if (k == interior_crease_count)
          break;
      }
      else if (dart_count == 2)
      {
        const_cast<ON_SubDVertex*>(edge->m_vertex[0])->m_vertex_tag = ON_SubD::VertexTag::Dart;
        const_cast<ON_SubDVertex*>(edge->m_vertex[1])->m_vertex_tag = ON_SubD::VertexTag::Dart;
        k++;
        if (k == interior_crease_count)
          break;
      }
    }
  }

  if (bHasTaggedVertices)
  {
    for (const ON_SubDEdge* edge = new_subd->FirstEdge(); nullptr != edge; edge = edge->m_next_edge)
    {
      if (ON_SubD::EdgeTag::Smooth != edge->m_edge_tag)
        continue;
      const unsigned int tagged_end_index = edge->TaggedEndIndex();
      if (tagged_end_index < 2)
      {
        // sector weight will be calculated when facet type is set
        const_cast<ON_SubDEdge*>(edge)->m_sector_coefficient[tagged_end_index] = ON_SubDSectorType::UnsetSectorWeight;
      }
      else if (2 == tagged_end_index)
      {
        // both ends are tagged
        if (2 == edge->m_face_count)
        {
          // first subdivision will convert edge to smooth
          const_cast<ON_SubDEdge*>(edge)->m_edge_tag = ON_SubD::EdgeTag::X;
          // sector weights will be calculated when facet type is set
          const_cast<ON_SubDEdge*>(edge)->m_sector_coefficient[0] = ON_SubDSectorType::UnsetSectorWeight;
          const_cast<ON_SubDEdge*>(edge)->m_sector_coefficient[1] = ON_SubDSectorType::UnsetSectorWeight;
        }
        else
        {
          const_cast<ON_SubDEdge*>(edge)->m_edge_tag = ON_SubD::EdgeTag::Crease;
        }
      }
    }

    for (const ON_SubDVertex* vertex = new_subd->FirstVertex(); nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      if (ON_SubD::VertexTag::Crease != vertex->m_vertex_tag)
        continue;
      unsigned int vertex_creased_edge_count = 0;
      const unsigned int vertex_edge_count = vertex->m_edge_count;
      for (unsigned int j = 0; j < vertex_edge_count; j++)
      {
        const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(vertex->m_edges[j].m_ptr);
        if (ON_SubD::EdgeTag::Crease == edge->m_edge_tag)
        {
          if (vertex_creased_edge_count >= 2)
          {
            // Three or more creased edges end at this vertex.
            // It must be subdivided as a corner vertex.
            const_cast<ON_SubDVertex*>(vertex)->m_vertex_tag = ON_SubD::VertexTag::Corner;
            break;
          }
          vertex_creased_edge_count++;
        }
      }
    }
  }

  // Discard interior smooth vertices with 2 edges
  const ON_SubDVertex* next_vertex = new_subd->FirstVertex();
  for (const ON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
  {
    next_vertex = vertex->m_next_vertex;
    if (2 == vertex->m_edge_count && 2 == vertex->m_face_count && vertex->IsSmoothOrDart())
    {
      ON_SubDEdgePtr eptr0 = vertex->m_edges[0].Reversed();
      ON_SubDEdgePtr eptr1 = vertex->m_edges[1];
      if (ON_SubD::EdgesCanBeMerged(eptr0, eptr1))
        new_subd->MergeEdges(eptr0, eptr1);
    }
  }


  if (bMergeColinearEdges)
    new_subd->MergeColinearEdges(1e-6, 0.01, sin(0.25*ON_PI));

  // All interior vertices must have at least 2 faces and three edges

  // If the ON_SubD was allocated in this function, do not delete it.
  uptr.release();

  // If the input mesh is not oriented, fix the subd so it is.
  if ( false == new_subd->IsOriented(0) )
    new_subd->Orient(0);

  new_subd->RepairInvalidSectors(0);

  ON_SubD::SubDType subd_type
    = (nullptr != from_mesh_options)
    ? from_mesh_options->SubDType()
    : ON_SubD::DefaultSubDType();
  new_subd->SetSubDType(subd_type);

  return new_subd;
}
