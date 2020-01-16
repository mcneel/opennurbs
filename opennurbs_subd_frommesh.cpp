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

static bool Internal_CandidateTagIsBetterCreaseEnd(
  ON_SubD::VertexTag current_tag,
  const ON_SubDVertex* candidate
)
{
  if (nullptr == candidate)
    return false;
  switch(current_tag)
  {
  case ON_SubD::VertexTag::Unset:
    if (ON_SubD::VertexTag::Unset != candidate->m_vertex_tag )
      return true;
    break;
  case ON_SubD::VertexTag::Smooth:
    if (candidate->IsDartOrCreaseOrCorner())
      return true;
    break;
  case ON_SubD::VertexTag::Dart:
    if (candidate->IsCreaseOrCorner())
      return true;
    break;
  case ON_SubD::VertexTag::Crease:
    if (candidate->IsCorner())
      return true;
    break;
  case ON_SubD::VertexTag::Corner:
    break;
  default:
    break;
  }
  return false;
}

static bool Internal_CreateFromMesh_ValidateNonmanifoldVertexSector(
  const ON_SubDVertex* v,
  const ON_SubDEdge* e,
  ON_SubDSectorIterator& sit
)
{
  // e is non manifold edge
  // v = nonmanifold corner vertex on e
  // sit is a sector of v with e as a starting boundary
  if (nullptr == v || v != sit.CenterVertex() || e != sit.CurrentEdge(0))
    return false;

  // k is used to protect against infinite looping if the topology
  // around v is invalid.
  const ON_SubDEdge* other_crease = nullptr;
  const ON_SubDEdge* best_candidate_edge = nullptr;
  const ON_SubDVertex* best_canditate_v1 = nullptr;
  const ON_3dVector dir = -e->ControlNetDirectionFrom(v);
  double best_dot = ON_DBL_QNAN;
  for (unsigned short k = 0; k <= v->m_face_count; ++k)
  {
    const ON_SubDEdge* e1 = sit.CurrentEdge(1);
    if (e1->IsCrease())
    {
      other_crease = e1;
      break;
    }
    const ON_SubDVertex* v1 = e1->OtherEndVertex(v);
    if (nullptr == v1)
    {
      ON_SUBD_ERROR("invalid subd topology.");
      return false; // invalid topology
    }
    const double d = dir * e1->ControlNetDirectionFrom(v);
    if (
      nullptr == best_candidate_edge
      || Internal_CandidateTagIsBetterCreaseEnd(best_canditate_v1->m_vertex_tag,v1)
      || (nullptr != best_candidate_edge && best_canditate_v1->m_vertex_tag == v1->m_vertex_tag && d > best_dot)
      )
    {
      best_candidate_edge = e1;
      best_dot = d;
      best_canditate_v1 = v1;
    }
    if (nullptr == sit.NextFace(ON_SubDSectorIterator::StopAt::AnyCrease))
      break;
  }

  if (nullptr == other_crease)
  {
    ON_SUBD_ERROR("bug in nonmanifold mesh to subd code.");
    return false;
  }
  if (other_crease != e)
    return true; // this sector is a valid corner vertex sector.

  if (nullptr == best_candidate_edge)
  {
    ON_SUBD_ERROR("bug in nonmanifold mesh to subd code.");
    return false;
  }

  // make  best_candidate_edge a crease so corner sector is valid
  const_cast<ON_SubDEdge*>(best_candidate_edge)->m_edge_tag = ON_SubD::EdgeTag::Crease;
  const ON_SubDVertexEdgeProperties best_ep = best_canditate_v1->EdgeProperties();

  ON_SubD::VertexTag vtag;
  if ( 1 == best_ep.m_crease_edge_count && 2 == best_ep.m_min_edge_face_count && 2 == best_ep.m_max_edge_face_count)
    vtag = ON_SubD::VertexTag::Dart;
  else if ( 2 == best_ep.m_crease_edge_count && best_ep.m_max_edge_face_count <= 2 )
    vtag = ON_SubD::VertexTag::Crease;
  else
    vtag = ON_SubD::VertexTag::Corner;

  if (false == Internal_CandidateTagIsBetterCreaseEnd(vtag, best_canditate_v1))
    const_cast<ON_SubDVertex*>(best_canditate_v1)->m_vertex_tag = vtag;

  return true;
}

static void Internal_CreateFromMesh_ValidateNonmanifoldVertex(
  const ON_SubDVertex* v
)
{
  if (
    nullptr == v
    || ON_SubD::VertexTag::Corner != v->m_vertex_tag
    )
    return;

  for (unsigned short vei = 0; vei < v->m_edge_count; ++vei)
  {
    const ON_SubDEdge* e = v->Edge(vei);
    if (
      nullptr == e
      || ON_SubD::EdgeTag::Crease != e->m_edge_tag
      || e->m_face_count <= 2
      )
      continue;
    // e is non manifold - verify every attached face has a valid corner sector
    for (unsigned short efi = 0; efi < e->m_face_count; ++efi)
    {
      const ON_SubDFace* f = e->Face(efi);
      if (nullptr == f)
        continue;
      ON_SubDSectorIterator sit;
      sit.Initialize(f, 0, v);
      if (e != sit.CurrentEdge(0))
      {
        sit.Initialize(f, 1, v);
        if (e != sit.CurrentEdge(0))
        {
          ON_SUBD_ERROR("bug in nonmanifold mesh to subd code.");
          continue;
        }
      }
      Internal_CreateFromMesh_ValidateNonmanifoldVertexSector(v,e,sit);
      // convert best_candidate to a crease to make this a valid corner sector;
    }
  }

  return;
}

ON_SubD* ON_SubD::CreateFromMesh(
  const class ON_Mesh* level_zero_mesh,
  const class ON_ToSubDParameters* from_mesh_options,
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

  if (nullptr == from_mesh_options)
    from_mesh_options = &ON_ToSubDParameters::Smooth;

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

  ON_ToSubDParameters::InteriorCreaseOption crease_test 
    = (nullptr != from_mesh_options)
    ? from_mesh_options->InteriorCreaseTest()
    : ON_ToSubDParameters::InteriorCreaseOption::None;

  if (ON_ToSubDParameters::InteriorCreaseOption::AtMeshCrease == crease_test && nullptr != pointNormal )
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
      crease_test = ON_ToSubDParameters::InteriorCreaseOption::None;
    }
  }
  else if (ON_ToSubDParameters::InteriorCreaseOption::AtMeshEdge != crease_test)
  {
    crease_test = ON_ToSubDParameters::InteriorCreaseOption::None;
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
        // Flip middle edge if odd number of edges
        if (i0 == i1)
        {
          int k = mesh_edges[i0].i;
          mesh_edges[i0].i = mesh_edges[i0].j;
          mesh_edges[i0].j = k;
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

  // Make sure the subdimple is created before adding components.
  if (nullptr == new_subd->SubDimple(true))
    return nullptr;

  bool bHasTaggedVertices = false;
  bool bHasNonmanifoldCornerVertices = false;

  unsigned int* Nid = nullptr;
  unsigned int nextNid = 0;
  if (ON_ToSubDParameters::InteriorCreaseOption::AtMeshCrease == crease_test)
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
    if (ON_ToSubDParameters::InteriorCreaseOption::AtMeshEdge == crease_test)
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
    // Later, some of the ON_SubD::EdgeTag::Smooth tags are changed to ON_SubD::EdgeTag::Crease or ON_SubD::EdgeTag::SmoothX.
    mesh_edge.e
      = (mesh_edge.i <= mesh_edge.j)
      ? new_subd->AddEdgeWithSectorCoefficients(mesh_edge.edge_tag, V[mesh_edge.i], ON_SubDSectorType::IgnoredSectorCoefficient, V[mesh_edge.j], ON_SubDSectorType::IgnoredSectorCoefficient)
      : new_subd->AddEdgeWithSectorCoefficients(mesh_edge.edge_tag, V[mesh_edge.j], ON_SubDSectorType::IgnoredSectorCoefficient, V[mesh_edge.i], ON_SubDSectorType::IgnoredSectorCoefficient);
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
      new_subd->AddFace(EP.Array(), EP.UnsignedCount());
  }

  // Apply "ON_SubD::EdgeTag::Crease" tag to boundary and non-manifold edges and their vertices.
  unsigned int interior_crease_count = 0;
  for (const ON_SubDEdge* edge = new_subd->FirstEdge(); nullptr != edge; edge = edge->m_next_edge)
  {
    if (2 == edge->m_face_count && ON_SubD::EdgeTag::Smooth == edge->m_edge_tag)
      continue;

    bHasTaggedVertices = true;

    const ON_SubD::VertexTag vtag
      = (edge->m_face_count > 2)
      ? ON_SubD::VertexTag::Corner
      : ON_SubD::VertexTag::Crease;

    const_cast<ON_SubDEdge*>(edge)->m_edge_tag = ON_SubD::EdgeTag::Crease;
    for (unsigned int j = 0; j < 2; j++)
    {
      const ON_SubDVertex* vertex = edge->m_vertex[j];
      if (ON_SubD::VertexTag::Smooth == vertex->m_vertex_tag)
      {
        const_cast<ON_SubDVertex*>(vertex)->m_vertex_tag = vtag;
        if (ON_SubD::VertexTag::Corner == vtag && edge->m_face_count > 2)
          bHasNonmanifoldCornerVertices = true;
      }
    }

    if ( 2 == edge->m_face_count )
      interior_crease_count++;
  }

  if (bHasNonmanifoldCornerVertices)
  {
    // may need to crease more edges to get valid corners RH-49843
    for (const ON_SubDVertex* v = new_subd->FirstVertex(); nullptr != v; v = v->m_next_vertex)
      Internal_CreateFromMesh_ValidateNonmanifoldVertex(v);
  }

  if (interior_crease_count > 0)
  {
    // Any interior vertex that has exactly one creased edges must be tagged as a dart.
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
        if (ON_SubD::VertexTag::Crease != vertex->m_vertex_tag)
          continue;
        const ON_SubDVertexEdgeProperties ep = vertex->EdgeProperties();

        if ( 0 == ep.m_null_edge_count && 0 == ep.m_unset_edge_count )
        {
          if (1 == ep.m_crease_edge_count && ep.m_smooth_edge_count >= 1 && 2 == ep.m_min_edge_face_count && 2 == ep.m_max_edge_face_count)
          {
            dart_index = j;
            ++dart_count;
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
        const_cast<ON_SubDEdge*>(edge)->m_sector_coefficient[tagged_end_index] = ON_SubDSectorType::UnsetSectorCoefficient;
      }
      else if (2 == tagged_end_index)
      {
        // both ends are tagged
        if (2 == edge->m_face_count)
        {
          // first subdivision will convert edge to smooth
          const_cast<ON_SubDEdge*>(edge)->m_edge_tag = ON_SubD::EdgeTag::SmoothX;
          // sector weights will be calculated when facet type is set
          const_cast<ON_SubDEdge*>(edge)->m_sector_coefficient[0] = ON_SubDSectorType::UnsetSectorCoefficient;
          const_cast<ON_SubDEdge*>(edge)->m_sector_coefficient[1] = ON_SubDSectorType::UnsetSectorCoefficient;
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

  if (bMergeColinearEdges)
  {
    const bool bMergeBoundaryEdges = from_mesh_options->MergeColinearBoundaryEdges();
    const bool bMergeInteriorCreaseEdges = from_mesh_options->MergeColinearInteriorEdges();
    const bool bMergeInteriorSmoothEdges = from_mesh_options->MergeColinearInteriorEdges();
    new_subd->MergeColinearEdges(bMergeBoundaryEdges, bMergeInteriorCreaseEdges, bMergeInteriorSmoothEdges, 1e-6, 0.01, sin(0.25*ON_PI));
  }

  // All interior vertices must have at least 2 faces and three edges

  // If the ON_SubD was allocated in this function, do not delete it.
  uptr.release();

  // If the input mesh is not oriented, fix the subd so it is.
  if ( false == new_subd->IsOriented() )
    new_subd->Orient();

  if (ON_ToSubDParameters::ConvexCornerOption::AtMeshCorner == from_mesh_options->ConvexCornerTest())
  {
    // Add corners
    ON_SubDVertexIterator vit(*new_subd);
    ON_SubDEdge* e[2];
    const ON_SubDFace* f;
    const ON_SubDVertex* v[4];
    ON_3dPoint P[4];
    ON_3dVector T[4];
    ON_3dVector N[4];
    double NoN[4];
    const double a = from_mesh_options->MaximumConvexCornerAngleRadians();
    if (a > 0.0 && a < ON_PI)
    {
      const double NoNtol = 0.2588190451; // sin(15 degrees)
      const double min_cos_corner_angle = cos(a);
      for (ON_SubDVertex* vertex = const_cast<ON_SubDVertex*>(vit.FirstVertex()); nullptr != vertex; vertex = const_cast<ON_SubDVertex*>(vit.NextVertex()))
      {
        if (ON_SubD::VertexTag::Crease != vertex->m_vertex_tag)
          continue;
        if (2 != vertex->m_edge_count)
          continue;
        e[0] = ON_SUBD_EDGE_POINTER(vertex->m_edges[0].m_ptr);
        e[1] = ON_SUBD_EDGE_POINTER(vertex->m_edges[1].m_ptr);
        if (nullptr == e[0] || 1 != e[0]->m_face_count || ON_SubD::EdgeTag::Crease != e[0]->m_edge_tag)
          continue;
        if (nullptr == e[1] || 1 != e[1]->m_face_count || ON_SubD::EdgeTag::Crease != e[1]->m_edge_tag)
          continue;
        f = ON_SUBD_FACE_POINTER(e[0]->m_face2[0].m_ptr);
        if (nullptr == f)
          continue;
        if (f != ON_SUBD_FACE_POINTER(e[1]->m_face2[0].m_ptr))
          continue;

        const unsigned int vi = f->VertexIndex(vertex);
        if (vi >= 4)
          continue;
        ON_SubDEdgePtr eptr[2];
        if (e[0] == f->Edge(vi))
        {
          eptr[0] = ON_SubDEdgePtr::Create(e[0], vertex == e[0]->m_vertex[1] ? 1 : 0);
          eptr[1] = ON_SubDEdgePtr::Create(e[1], vertex == e[1]->m_vertex[1] ? 1 : 0);
        }
        else if (e[1] == f->Edge(vi))
        {
          eptr[1] = ON_SubDEdgePtr::Create(e[0], vertex == e[0]->m_vertex[1] ? 1 : 0);
          eptr[0] = ON_SubDEdgePtr::Create(e[1], vertex == e[1]->m_vertex[1] ? 1 : 0);
        }
        const double corner_angle_radians = ON_SubDSectorType::CornerSectorAngleRadiansFromEdges(eptr[0], eptr[1]);
        if (!(corner_angle_radians > 0.0 && corner_angle_radians < ON_PI))
          continue;

        // ocnvex quad restriction - for now
        if (4 != f->m_edge_count)
          continue;
        v[0] = vertex;
        v[1] = f->Vertex((vi + 1) % 4);
        v[2] = f->Vertex((vi + 2) % 4);
        v[3] = f->Vertex((vi + 3) % 4);
        if (nullptr == v[0] || nullptr == v[1] || nullptr == v[2] || nullptr == v[3])
          continue;
        for (int i = 0; i < 4; i++)
          P[i] = ON_3dPoint(v[i]->m_P);
        for (int i = 0; i < 4; i++)
          T[i] = P[(i + 1) % 4] - P[i];
        for (int i = 0; i < 4; i++)
          N[i] = -ON_CrossProduct(T[i], T[(i + 3) % 4]).UnitVector();
        for (int i = 0; i < 4; i++)
          NoN[i] = N[i] * N[(i + 1) % 4];
        if (false == (NoN[0] >= NoNtol && NoN[1] >= NoNtol && NoN[2] >= NoNtol && NoN[3] >= NoNtol))
          continue;
        const double cos_corner_angle = ON_CrossProduct(T[0], T[3]).Length();
        if (false == (cos_corner_angle >= min_cos_corner_angle))
          continue;
        vertex->m_vertex_tag = ON_SubD::VertexTag::Corner;
      }
    }
  }

  new_subd->UpdateEdgeSectorCoefficients(false);


  return new_subd;
}


static ON_SubDVertex* IndexVertex(
  ON_SimpleArray< ON_SubDVertex* >& vertex,
  ON_ClassArray< ON_ClassArray< ON_SimpleArray < int > > >& vert_index,
  int x, int y, int z
  )
{
  int vi = vert_index[x][y][z];
  if (vi < 0)
    return nullptr;
  if (vi >= vertex.Count())
    return nullptr;
  return vertex[vi];
}


ON_SubD* ON_SubD::CreateSubDBox(
  const ON_3dPoint corners[8],
  ON_SubD::EdgeTag edge_tag,
  unsigned int facecount_x,
  unsigned int facecount_y,
  unsigned int facecount_z,
  ON_SubD* subd)
{
  if (ON_SubD::EdgeTag::Crease != edge_tag)
    edge_tag = ON_SubD::EdgeTag::Smooth;

  if (nullptr == subd)
    subd = new ON_SubD;

  ON_3dVector xdir = corners[1] - corners[0];
  ON_3dVector ydir = corners[3] - corners[0];
  ON_3dVector zdir = corners[4] - corners[0];

  double x_len = xdir.LengthAndUnitize();
  double y_len = ydir.LengthAndUnitize();
  double z_len = zdir.LengthAndUnitize();

  double dx = x_len / (double)facecount_x;
  double dy = y_len / (double)facecount_y;
  double dz = z_len / (double)facecount_z;

  ON_ClassArray< ON_ClassArray< ON_SimpleArray < int > > > vert_index;
  ON_SimpleArray< ON_SubDVertex* > vertex;
  // Allocate index arrays
  vert_index.Reserve(facecount_x + 1);
  vert_index.SetCount(facecount_x + 1);
  for (unsigned int ix = 0; ix <= facecount_x; ix++)
  {
    vert_index[ix].Reserve(facecount_y + 1);
    vert_index[ix].SetCount(facecount_y + 1);
    for (unsigned int iy = 0; iy <= facecount_y; iy++)
    {
      vert_index[ix][iy].Reserve(facecount_z + 1);
      vert_index[ix][iy].SetCount(facecount_z + 1);
      for (unsigned int iz = 0; iz <= facecount_z; iz++)
      {
        vert_index[ix][iy][iz] = -1;
      }
    }
  }

  // Make interior vertexes and store 3d indexes
  for (unsigned int ix = 0; ix <= facecount_x; ix++)
  {
    for (unsigned int iy = 0; iy <= facecount_y; iy++)
    {
      for (unsigned int iz = 0; iz <= facecount_z; iz++)
      {
        int ccnt = 0;
        if (ix == 0 || ix == facecount_x)
          ccnt++;
        if (iy == 0 || iy == facecount_y)
          ccnt++;
        if (iz == 0 || iz == facecount_z)
          ccnt++;
        if (ccnt > 0) // On some face
        {
          ON_SubD::VertexTag vtag = ON_SubD::VertexTag::Smooth;
          if (edge_tag == ON_SubD::EdgeTag::Crease)
          {
            if(ccnt == 2)  // On some edge
              vtag = ON_SubD::VertexTag::Crease;
            else if(ccnt == 3)  // On some corner
              vtag = ON_SubD::VertexTag::Corner;
          }
          ON_3dPoint P(corners[0] + (xdir * (dx * ix)) + (ydir * (dy * iy)) + (zdir * (dz * iz)));
          vert_index[ix][iy][iz] = vertex.Count();
          vertex.AppendNew() = subd->AddVertex(vtag, &P.x);
          if (nullptr == vertex.Last())
            return nullptr;
        }
      }
    }
  }

  ON_ClassArray< ON_SubDEdgePtr > box_edges[12];
  ON_SubDEdge* e = nullptr;

  // 4 edge chains parallel to x
  for (unsigned int ix = 0; ix < facecount_x; ix++)
  {
    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, ix, 0, 0), IndexVertex(vertex, vert_index, ix + 1, 0, 0));
    box_edges[0].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, ix, facecount_y, 0), IndexVertex(vertex, vert_index, ix + 1, facecount_y, 0));
    box_edges[2].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, ix, 0, facecount_z), IndexVertex(vertex, vert_index, ix + 1, 0, facecount_z));
    box_edges[8].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, ix, facecount_y, facecount_z), IndexVertex(vertex, vert_index, ix + 1, facecount_y, facecount_z));
    box_edges[10].Append(ON_SubDEdgePtr::Create(e, 0));
  }

  // 4 edge chains parallel to y
  for (unsigned int iy = 0; iy < facecount_y; iy++)
  {
    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, 0, iy, 0), IndexVertex(vertex, vert_index, 0, iy + 1, 0));
    box_edges[3].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, facecount_x, iy, 0), IndexVertex(vertex, vert_index, facecount_x, iy + 1, 0));
    box_edges[1].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, facecount_x, iy, facecount_z), IndexVertex(vertex, vert_index, facecount_x, iy + 1, facecount_z));
    box_edges[9].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, 0, iy, facecount_z), IndexVertex(vertex, vert_index, 0, iy + 1, facecount_z));
    box_edges[11].Append(ON_SubDEdgePtr::Create(e, 0));
  }

  // 4 edge chains parallel to z
  for (unsigned int iz = 0; iz < facecount_z; iz++)
  {
    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, 0, 0, iz), IndexVertex(vertex, vert_index, 0, 0, iz + 1));
    box_edges[4].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, facecount_x, 0, iz), IndexVertex(vertex, vert_index, facecount_x, 0, iz + 1));
    box_edges[5].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, facecount_x, facecount_y, iz), IndexVertex(vertex, vert_index, facecount_x, facecount_y, iz + 1));
    box_edges[6].Append(ON_SubDEdgePtr::Create(e, 0));

    e = subd->AddEdge(edge_tag, IndexVertex(vertex, vert_index, 0, facecount_y, iz), IndexVertex(vertex, vert_index, 0, facecount_y, iz + 1));
    box_edges[7].Append(ON_SubDEdgePtr::Create(e, 0));
  }

  ON_ClassArray< ON_ClassArray< ON_SubDEdgePtr > > face_edges[2];

  // Bottom face
  {
    for (unsigned int iy = 0; iy <= facecount_y; iy++)
    {
      if (iy == 0)
        face_edges[0].Append(box_edges[0]);
      else if (iy == facecount_y)
        face_edges[0].Append(box_edges[2]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& row = face_edges[0].AppendNew();
        for (unsigned int ix = 0; ix < facecount_x; ix++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, iy, 0), IndexVertex(vertex, vert_index, ix + 1, iy, 0));
          row.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int ix = 0; ix <= facecount_x; ix++)
    {
      if (ix == 0)
        face_edges[1].Append(box_edges[3]);
      else if (ix == facecount_x)
        face_edges[1].Append(box_edges[1]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& col = face_edges[1].AppendNew();
        for (unsigned int iy = 0; iy < facecount_y; iy++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, iy, 0), IndexVertex(vertex, vert_index, ix, iy + 1, 0));
          col.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int iy = 0; iy < facecount_y; iy++)
    {
      for (unsigned int ix = 0; ix < facecount_x; ix++)
      {
        ON_SubDEdgePtr edge_ptrs[4];
        edge_ptrs[0] = face_edges[1][ix][iy];
        edge_ptrs[1] = face_edges[0][iy + 1][ix];
        edge_ptrs[2] = face_edges[1][ix + 1][iy].Reversed();
        edge_ptrs[3] = face_edges[0][iy][ix].Reversed();
        ON_SubDFace* f0 = subd->AddFace(edge_ptrs, 4);
        if (nullptr == f0)
          return nullptr;
      }
    }
  }

  // Top face
  {
    face_edges[0].Empty();
    face_edges[1].Empty();

    for (unsigned int iy = 0; iy <= facecount_y; iy++)
    {
      if (iy == 0)
        face_edges[0].Append(box_edges[8]);
      else if (iy == facecount_y)
        face_edges[0].Append(box_edges[10]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& row = face_edges[0].AppendNew();
        for (unsigned int ix = 0; ix < facecount_x; ix++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, iy, facecount_z), IndexVertex(vertex, vert_index, ix + 1, iy, facecount_z));
          row.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int ix = 0; ix <= facecount_x; ix++)
    {
      if (ix == 0)
        face_edges[1].Append(box_edges[11]);
      else if (ix == facecount_x)
        face_edges[1].Append(box_edges[9]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& col = face_edges[1].AppendNew();
        for (unsigned int iy = 0; iy < facecount_y; iy++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, iy, facecount_z), IndexVertex(vertex, vert_index, ix, iy + 1, facecount_z));
          col.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int iy = 0; iy < facecount_y; iy++)
    {
      for (unsigned int ix = 0; ix < facecount_x; ix++)
      {
        ON_SubDEdgePtr edge_ptrs[4];
        edge_ptrs[0] = face_edges[0][iy][ix];
        edge_ptrs[1] = face_edges[1][ix + 1][iy];
        edge_ptrs[2] = face_edges[0][iy + 1][ix].Reversed();
        edge_ptrs[3] = face_edges[1][ix][iy].Reversed();
        ON_SubDFace* f0 = subd->AddFace(edge_ptrs, 4);
        if (nullptr == f0)
          return nullptr;
      }
    }
  }

  // Front face
  {
    face_edges[0].Empty();
    face_edges[1].Empty();

    for (unsigned int iz = 0; iz <= facecount_z; iz++)
    {
      if (iz == 0)
        face_edges[0].Append(box_edges[0]);
      else if (iz == facecount_z)
        face_edges[0].Append(box_edges[8]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& row = face_edges[0].AppendNew();
        for (unsigned int ix = 0; ix < facecount_x; ix++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, 0, iz), IndexVertex(vertex, vert_index, ix + 1, 0, iz));
          row.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int ix = 0; ix <= facecount_x; ix++)
    {
      if (ix == 0)
        face_edges[1].Append(box_edges[4]);
      else if (ix == facecount_x)
        face_edges[1].Append(box_edges[5]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& col = face_edges[1].AppendNew();
        for (unsigned int iz = 0; iz < facecount_z; iz++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, 0, iz), IndexVertex(vertex, vert_index, ix, 0, iz + 1));
          col.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int iz = 0; iz < facecount_z; iz++)
    {
      for (unsigned int ix = 0; ix < facecount_x; ix++)
      {
        ON_SubDEdgePtr edge_ptrs[4];
        edge_ptrs[0] = face_edges[0][iz][ix];
        edge_ptrs[1] = face_edges[1][ix + 1][iz];
        edge_ptrs[2] = face_edges[0][iz + 1][ix].Reversed();
        edge_ptrs[3] = face_edges[1][ix][iz].Reversed();
        ON_SubDFace* f0 = subd->AddFace(edge_ptrs, 4);
        if (nullptr == f0)
          return nullptr;
      }
    }
  }

  // Back face
  {
    face_edges[0].Empty();
    face_edges[1].Empty();

    for (unsigned int iz = 0; iz <= facecount_z; iz++)
    {
      if (iz == 0)
        face_edges[0].Append(box_edges[2]);
      else if (iz == facecount_z)
        face_edges[0].Append(box_edges[10]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& row = face_edges[0].AppendNew();
        for (unsigned int ix = 0; ix < facecount_x; ix++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, facecount_y, iz), IndexVertex(vertex, vert_index, ix + 1, facecount_y, iz));
          row.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int ix = 0; ix <= facecount_x; ix++)
    {
      if (ix == 0)
        face_edges[1].Append(box_edges[7]);
      else if (ix == facecount_x)
        face_edges[1].Append(box_edges[6]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& col = face_edges[1].AppendNew();
        for (unsigned int iz = 0; iz < facecount_z; iz++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, ix, facecount_y, iz), IndexVertex(vertex, vert_index, ix, facecount_y, iz + 1));
          col.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int iz = 0; iz < facecount_z; iz++)
    {
      for (unsigned int ix = 0; ix < facecount_x; ix++)
      {
        ON_SubDEdgePtr edge_ptrs[4];
        edge_ptrs[0] = face_edges[1][ix][iz];
        edge_ptrs[1] = face_edges[0][iz + 1][ix];
        edge_ptrs[2] = face_edges[1][ix + 1][iz].Reversed();
        edge_ptrs[3] = face_edges[0][iz][ix].Reversed();
        ON_SubDFace* f0 = subd->AddFace(edge_ptrs, 4);
        if (nullptr == f0)
          return nullptr;
      }
    }
  }

  // Left face
  {
    face_edges[0].Empty();
    face_edges[1].Empty();

    for (unsigned int iz = 0; iz <= facecount_z; iz++)
    {
      if (iz == 0)
        face_edges[0].Append(box_edges[3]);
      else if (iz == facecount_z)
        face_edges[0].Append(box_edges[11]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& row = face_edges[0].AppendNew();
        for (unsigned int iy = 0; iy < facecount_y; iy++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, 0, iy, iz), IndexVertex(vertex, vert_index, 0, iy + 1, iz));
          row.Append(ON_SubDEdgePtr::Create(e, 0));

          // mac compile warning // ON_3dPoint p0 = row.Last()->RelativeVertex(0)->ControlNetPoint();
          // mac compile warning // ON_3dPoint p1 = row.Last()->RelativeVertex(1)->ControlNetPoint();
          // mac compile warning // iy = iy;
        }
      }
    }

    for (unsigned int iy = 0; iy <= facecount_y; iy++)
    {
      if (iy == 0)
        face_edges[1].Append(box_edges[4]);
      else if (iy == facecount_y)
        face_edges[1].Append(box_edges[7]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& col = face_edges[1].AppendNew();
        for (unsigned int iz = 0; iz < facecount_z; iz++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, 0, iy, iz), IndexVertex(vertex, vert_index, 0, iy, iz + 1));
          col.Append(ON_SubDEdgePtr::Create(e, 0));

          // mac compile warning // ON_3dPoint p0 = col.Last()->RelativeVertex(0)->ControlNetPoint();
          // mac compile warning // ON_3dPoint p1 = col.Last()->RelativeVertex(1)->ControlNetPoint();
          // mac compile warning // iy = iy;
        }
      }
    }

    for (unsigned int iz = 0; iz < facecount_z; iz++)
    {
      for (unsigned int iy = 0; iy < facecount_y; iy++)
      {
        ON_SubDEdgePtr edge_ptrs[4];
        edge_ptrs[0] = face_edges[1][iy][iz];
        edge_ptrs[1] = face_edges[0][iz + 1][iy];
        edge_ptrs[2] = face_edges[1][iy + 1][iz].Reversed();
        edge_ptrs[3] = face_edges[0][iz][iy].Reversed();

        // mac compile warning // ON_3dPoint p00 = edge_ptrs[0].RelativeVertex(0)->ControlNetPoint();
        // mac compile warning // ON_3dPoint p01 = edge_ptrs[0].RelativeVertex(1)->ControlNetPoint();

        // mac compile warning // ON_3dPoint p10 = edge_ptrs[1].RelativeVertex(0)->ControlNetPoint();
        // mac compile warning // ON_3dPoint p11 = edge_ptrs[1].RelativeVertex(1)->ControlNetPoint();

        // mac compile warning // ON_3dPoint p20 = edge_ptrs[2].RelativeVertex(0)->ControlNetPoint();
        // mac compile warning // ON_3dPoint p21 = edge_ptrs[2].RelativeVertex(1)->ControlNetPoint();

        // mac compile warning // ON_3dPoint p30 = edge_ptrs[3].RelativeVertex(0)->ControlNetPoint();
        // mac compile warning // ON_3dPoint p31 = edge_ptrs[3].RelativeVertex(1)->ControlNetPoint();

        ON_SubDFace* f0 = subd->AddFace(edge_ptrs, 4);
        if (nullptr == f0)
          return nullptr;
      }
    }
  }

  // Right face
  {
    face_edges[0].Empty();
    face_edges[1].Empty();

    for (unsigned int iz = 0; iz <= facecount_z; iz++)
    {
      if (iz == 0)
        face_edges[0].Append(box_edges[1]);
      else if (iz == facecount_z)
        face_edges[0].Append(box_edges[9]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& row = face_edges[0].AppendNew();
        for (unsigned int iy = 0; iy < facecount_y; iy++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, facecount_x, iy, iz), IndexVertex(vertex, vert_index, facecount_x, iy + 1, iz));
          row.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int iy = 0; iy <= facecount_y; iy++)
    {
      if (iy == 0)
        face_edges[1].Append(box_edges[5]);
      else if (iy == facecount_y)
        face_edges[1].Append(box_edges[6]);
      else
      {
        ON_ClassArray< ON_SubDEdgePtr >& col = face_edges[1].AppendNew();
        for (unsigned int iz = 0; iz < facecount_z; iz++)
        {
          e = subd->AddEdge(ON_SubD::EdgeTag::Smooth, IndexVertex(vertex, vert_index, facecount_x, iy, iz), IndexVertex(vertex, vert_index, facecount_x, iy, iz + 1));
          col.Append(ON_SubDEdgePtr::Create(e, 0));
        }
      }
    }

    for (unsigned int iz = 0; iz < facecount_z; iz++)
    {
      for (unsigned int iy = 0; iy < facecount_y; iy++)
      {
        ON_SubDEdgePtr edge_ptrs[4];
        edge_ptrs[0] = face_edges[0][iz][iy];
        edge_ptrs[1] = face_edges[1][iy + 1][iz];
        edge_ptrs[2] = face_edges[0][iz + 1][iy].Reversed();
        edge_ptrs[3] = face_edges[1][iy][iz].Reversed();
        ON_SubDFace* f0 = subd->AddFace(edge_ptrs, 4);
        if (nullptr == f0)
          return nullptr;
      }
    }
  }

  subd->SubDModifiedNofification();
  //subd->UpdateAllTagsAndSectorCoefficients(true);

 return subd;
}

