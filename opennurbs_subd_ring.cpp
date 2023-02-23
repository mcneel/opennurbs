//
// Copyright (c) 1993-2022 Robert McNeel & Associates. All rights reserved.
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

#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_subd_data.h"

static void Internal_PointRingSharpSubdivision(
  const ON_SubDVertexTag center_vertex_tag,
  double sector_coefficient,
  const ON_SubDEdgePtr* edges,
  size_t edges_stride,
  const unsigned int N,
  const unsigned int F,
  ON_SubDEdgeSharpness* edge_sharpness,
  double* point_ring,
  size_t point_ring_stride
)
{
  // All ring vertices are either smooth or creases.
  // 
  // The center vertex can be smooth, crease or dart.
  // 
  // In all cases, the sector coefficient is passed in. 
  // 
  // All ring faces are quads.
  // 
  // edges[] is used only to get the edge tag. 
  // In some cases, point_ring[] is the result 
  // of 1 subdivision from the elements in edges[].
  // //
  // This function subdivides the point ring until all sharpness
  // values become zero. At that point we can apply the usual
  // sector limit surface matrix to calculate limit surface
  // point and tangent plane normal.

  if (N < 2)
  {
    ON_SUBD_ERROR("Invalid input. N must be at least 2.");
    return;
  }
  if (ON_SubDVertexTag::Smooth == center_vertex_tag || ON_SubDVertexTag::Dart == center_vertex_tag)
  {
    // These checks insure the pointer arithmetic below never dereferences an invalid
    // pointer due to invalid input.
    if (N != F)
    {
      ON_SUBD_ERROR("Invalid input. At least on of the center vertex tag, N, or F is wrong.");
      return;
    }
  }
  else if(ON_SubDVertexTag::Crease == center_vertex_tag)
  {
    // These checks insure the pointer arithmetic below never dereferences an invalid
    // pointer due to invalid input.
    if (N != F + 1)
    {
      ON_SUBD_ERROR("Invalid input. At least on of the center vertex tag, N, or F is wrong.");
      return;
    }
    if (ON_SubDEdgeTag::Crease != ON_SUBD_EDGE_POINTER(edges->m_ptr)->m_edge_tag)
    {
      ON_SUBD_ERROR("Invalid input. In a crease sector the first edge must be a crease.");
      return;
    }
    if (ON_SubDEdgeTag::Crease != ON_SUBD_EDGE_POINTER((edges + ((N - 1)* edges_stride) )->m_ptr)->m_edge_tag)
    {
      ON_SUBD_ERROR("Invalid input. In a crease sector the last edge must be a crease.");
      return;
    }
  }
  else
  {
    ON_SUBD_ERROR("Invalid input. Center vertex tag must be smooth, dart, or crease.");
    return;
  }

  const unsigned expected_vertex_C_count
    = (ON_SubDVertexTag::Crease == center_vertex_tag)
    ? 2U
    : (ON_SubDVertexTag::Dart == center_vertex_tag ? 1U : 0U);

  if (ON_SubDVertexTag::Smooth == center_vertex_tag)
    sector_coefficient = 0.5;
  else if (false == (sector_coefficient > 0.0 && sector_coefficient < 1.0))
  {
    // Input edges passed to  ON_SubD::GetQuadSectorPointRing()
    // had incorrect sector coefficients. Using 1/2 will be good enough
    // to get some sort of answer.
    ON_SUBD_ERROR("An input edges to ON_SubD::GetQuadSectorPointRing() had incorrect sector coeffient value.");
    sector_coefficient = 0.5;
  }

  const unsigned R = 1U + N + F;
  ON_SimpleArray<ON_3dPoint> R1_buffer;
  ON_3dPoint* R1 = R1_buffer.Reserve(R);
  const size_t AQstride = 2 * point_ring_stride;

  // (V0[0],V0[1],V0[2]) = control net position of center vertex
  const double* V0 = point_ring;

  // A0 is used to get the ends of the radial edges on the input point ring.
  // In this function, "start of edge" means the central vertex control point (V0)
  // and "end of edge" means the edge control points on the point ring.
  // The stride for the A0 array is AQstride = 2*point_ring_stride.
  // (A0[0],A0[1],A0[2]) = control net position of end of 1st edge
  const double* A0 = V0 + point_ring_stride;

  // Q0 is used to get the quad points that are diagonally opposite from
  // the center vertex on the input point ring.
  // The stride for the Q0 array is AQstride = 2*point_ring_stride.
  const double* Q0 = A0 + point_ring_stride;

  for (;;)
  {
    bool bSubdividePointRingAgain = false;

    // Face subdivision points = quad centroids.
    for (unsigned i = 0; i < F; ++i)
    {
      R1 += 2;
      const double* A0next = (i+1<N) ? (A0 + AQstride) : (V0 + point_ring_stride);
      R1->x = 0.25 * (V0[0] + A0[0] + Q0[0] + A0next[0]);
      R1->y = 0.25 * (V0[1] + A0[1] + Q0[1] + A0next[1]);
      R1->z = 0.25 * (V0[2] + A0[2] + Q0[2] + A0next[2]);
      A0 += AQstride;
      Q0 += AQstride;
    }

    // reset pointers
    R1 = R1_buffer.Array();
    A0 = V0 + point_ring_stride;
    Q0 = A0 + point_ring_stride;

    // Edge subdivision points
    unsigned vertex_A_count = 0; // number of crease and sharp edges.
    unsigned vertex_C_count = 0; // number of crease edges.
    ON_3dPoint vertex_A[2] = {}; // end points of the 1st two crease and/or sharp edges
    ON_3dPoint vertex_C[2] = {}; // end points of the crease edges (at most two per sector)
    double min_vertex_s = 0.0; // "min" means 2nd largest
    double max_vertex_s = 0.0;
    unsigned vertex_s_count = 0;

    ++R1; // Move P1 to point at end of 1st subdivided edge
    const double* A0prev = A0 + (N - 1) * AQstride;

    // If a "lint" detector complains about Q0prev being a nullptr, it's wrong.
    // When N != F, it is certain the first edge has a crease tag and Q0prev
    // is not dereferenced. By the time Q0 is dereferenced, it is not nullptr.
    const double* Q0prev = (N==F) ? (Q0 + (N - 1) * AQstride) : nullptr;

    for (unsigned i = 0; i < N; ++i)
    {
      if (ON_SubDEdgeTag::Crease == ON_SUBD_EDGE_POINTER(edges->m_ptr)->m_edge_tag)
      {
        // crease edge
        if (vertex_C_count >= 2)
        {
          ON_SUBD_ERROR("Invalid input. A sector has at most 2 creased edges.");
          return;
        }
        vertex_C[vertex_C_count] = ON_3dPoint(A0);
        ++vertex_C_count;
        if (vertex_A_count < 2)
          vertex_A[vertex_A_count] = ON_3dPoint(A0);
        ++vertex_A_count;

        R1->x = 0.5 * (V0[0] + A0[0]);
        R1->y = 0.5 * (V0[1] + A0[1]);
        R1->z = 0.5 * (V0[2] + A0[2]);
      }
      else
      {
        // smooth edge

        const double edge_s = edge_sharpness[i].Average();

        if (edge_s >= 1.0)
        {
          // we don't need the ordinary edge subdivision point.
          R1->x = 0.0;
          R1->y = 0.0;
          R1->z = 0.0;
        }
        else
        {
          // set R1 = ordinary edge subdivision point

          if (0.5 == sector_coefficient)
          {
            // smooth vertex or tagged vertex with "ordinary" number of edges.
            R1->x = 0.375 * (V0[0] + A0[0]);
            R1->y = 0.375 * (V0[1] + A0[1]);
            R1->z = 0.375 * (V0[2] + A0[2]);
          }
          else
          {
            // tagged vertex with extraordinary number of edges.
            const double w1 = 1.0 - sector_coefficient;
            R1->x = 0.75 * (sector_coefficient * V0[0] + w1 * A0[0]);
            R1->y = 0.75 * (sector_coefficient * V0[1] + w1 * A0[1]);
            R1->z = 0.75 * (sector_coefficient * V0[2] + w1 * A0[2]);
          }

          if (nullptr != Q0prev)
          {
            // The "if (nullptr != Q0prev)" check is always true. Dale Lear put the 
            // "if (nullptr != Q0prev)" check here to suppress incorrect compiler / lint warnings.
            // The 2023 lint detectors are not sophisticated enought to figure out
            // that if we get here, then Q0prev is not nullptr.
            const double* A0next = (i + 1 < N) ? (A0 + AQstride) : (V0 + point_ring_stride);
            R1->x += 0.0625 * (A0next[0] + Q0[0] + A0prev[0] + Q0prev[0]);
            R1->y += 0.0625 * (A0next[1] + Q0[1] + A0prev[1] + Q0prev[1]);
            R1->z += 0.0625 * (A0next[2] + Q0[2] + A0prev[2] + Q0prev[2]);
          }
        }


        if (edge_s > 0.0)
        {
          // This edge is sharp, modify P1 to take account of sharpness.
          if (vertex_A_count < 2)
            vertex_A[vertex_A_count] = ON_3dPoint(A0);
          ++vertex_A_count;
          const double vertex_s = edge_sharpness[i][0];
          if (vertex_s > 0.0)
          {
            // NOTE: "min_vertex_s" is the 2nd largest value, not the actual minimum value
            // The value is used only when there are exactly 2 nonzero vertex_s values.
            if (0 == vertex_s_count)
            {
              min_vertex_s = vertex_s;
              max_vertex_s = vertex_s;
            }
            else if (vertex_s > max_vertex_s)
            {
              min_vertex_s = max_vertex_s;
              max_vertex_s = vertex_s;
            }
            else if (vertex_s < min_vertex_s)
            {
              min_vertex_s = vertex_s;
            }
            ++vertex_s_count;
          }

          const double M[3] = {
            0.5 * (V0[0] + A0[0]),
            0.5 * (V0[1] + A0[1]),
            0.5 * (V0[2] + A0[2])
          };
          if (edge_s >= 1.0)
          {
            // crease subdivision point.
            R1->x = M[0];
            R1->y = M[1];
            R1->z = M[2];
          }
          else
          {
            // blend of smooth and crease edge subdivision points.
            const double r = 1.0 - edge_s;
            R1->x = r * R1->x + edge_s * M[0];
            R1->y = r * R1->y + edge_s * M[1];
            R1->z = r * R1->z + edge_s * M[2];
          }

          // sibdivide sharpness
          edge_sharpness[i] = edge_sharpness[i].Subdivided(0);

          if (false == bSubdividePointRingAgain)
          {
            // When bSubdividePointRingAgain is true, we need to subdivide
            // the point_ring at least one more time after finishing the
            // current subdivision.
            bSubdividePointRingAgain = edge_sharpness[i].IsNotZero();
          }
        }
      }

      // increment pointers for the next edge.
      A0prev = A0;
      A0 += AQstride;
      Q0prev = Q0;
      Q0 += AQstride;
      edges += edges_stride;
      R1 += 2;
    }

    if (expected_vertex_C_count != vertex_C_count)
    {
      ON_SUBD_ERROR("Invalid input. Sector tag and number of crease edges are incompatible.");
      return;
    }

    // reset pointers
    R1 = R1_buffer.Array();
    A0 = V0 + point_ring_stride;
    Q0 = A0 + point_ring_stride;
    edges -= N * edges_stride;

    const double vertex_s
      = (2 == vertex_s_count && min_vertex_s < max_vertex_s)
      ? 0.5 * (min_vertex_s + max_vertex_s)
      : (vertex_A_count >= 2 ? max_vertex_s : 0.0);

    if (vertex_s >= 1.0)
    {
      // we don't need the ordinary vertex subdivision point.
      R1->x = 0.0;
      R1->y = 0.0;
      R1->z = 0.0;
    }
    else
    {
      // set R1 = ordinary vertex subdivision point
      if (2 == vertex_C_count)
      {
        // crease vertex
        R1->x = 0.125 * (6.0 * V0[0] + vertex_C[0].x + vertex_C[1].x);
        R1->y = 0.125 * (6.0 * V0[1] + vertex_C[0].y + vertex_C[1].y);
        R1->z = 0.125 * (6.0 * V0[2] + vertex_C[0].z + vertex_C[1].z);
      }
      else
      {
        // smooth or dart vertex. (We know N = F)
        double Asum[3] = {}; // Asum = sum of edge ring points
        double Qsum[3] = {}; // Qsum = sum of quad face ring points
        for (unsigned i = 0; i < N; ++i)
        {
          Asum[0] += A0[0];
          Asum[1] += A0[1];
          Asum[2] += A0[2];
          A0 += AQstride;
          Qsum[0] += Q0[0];
          Qsum[1] += Q0[1];
          Qsum[2] += Q0[2];
          Q0 += AQstride;
        }
        A0 -= N * AQstride;
        Q0 -= N * AQstride;
        const double v = 1.0 - 1.75 / ((double)N);
        const double nn = N * N;
        const double a = 1.5 / nn;
        const double q = 0.25 / nn;
        R1->x = v * V0[0] + a * Asum[0] + q * Qsum[0];
        R1->y = v * V0[1] + a * Asum[1] + q * Qsum[1];
        R1->z = v * V0[2] + a * Asum[2] + q * Qsum[2];
      }
    }

    if (vertex_s > 0.0)
    {
      double S[3];
      if (vertex_A_count == 2)
      {
        // Exactly 2 edges are creases or sharp
        S[0] = 0.125 * (6.0 * V0[0] + vertex_A[0].x + vertex_A[1].x);
        S[1] = 0.125 * (6.0 * V0[1] + vertex_A[0].y + vertex_A[1].y);
        S[2] = 0.125 * (6.0 * V0[2] + vertex_A[0].z + vertex_A[1].z);
      }
      else
      {
        // 3 or more edges are creases or sharp (vertex_A_count > 2)
        S[0] = V0[0];
        S[1] = V0[1];
        S[2] = V0[2];
      }
      if (vertex_s >= 1.0)
      {
        R1->x = S[0];
        R1->y = S[1];
        R1->z = S[2];
      }
      else
      {
        const double r = 1.0 - vertex_s;
        R1->x = r * R1->x + vertex_s * S[0];
        R1->y = r * R1->y + vertex_s * S[1];
        R1->z = r * R1->z + vertex_s * S[2];
      }
    }

    // R1 = subdivided point ring with sharpness taken into account
    // Copy R1 to point_ring.
    for (unsigned i = 0; i < R; ++i)
    {
      point_ring[0] = R1->x;
      point_ring[1] = R1->y;
      point_ring[2] = R1->z;
      ++R1;
      point_ring += point_ring_stride;
    }

    if (false == bSubdividePointRingAgain)
    {
      // The point ring subdivisions have eliminated sharpness
      // and the sector limit surface matrix can be applied to 
      // the point ring to calculate the limit surface point
      // and tangent plane normal.
      break;
    }

    // reset pointers for next subdivision
    // Each value in edge_sharpness has been reduced by 1 or is zero.
    R1 -= R;
    point_ring -= R * point_ring_stride;
  }
}


unsigned int ON_SubD::GetQuadSectorPointRing(
  bool bFirstPass,
  bool bSecondPass,
  const ON_SubDVertex* center_vertex,
  const ON_SubDComponentPtr* component_ring,
  size_t component_ring_count,
  double* point_ring,
  size_t point_ring_stride
  )
{
  //// NO VALIDATION CHECKS
  ////  CALLER INSURES INPUT HAS NON-nullptr POINTERS AND CORRECT COUNTS

  // Except for internal testing functions, bSecondPass is always true.

  double subP[3];
  const double* Q = nullptr;

  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = ON_SubD::ComponentRingFaceCount(component_ring_count);
  const unsigned int point_ring_count = 1 + N + F;

  const double* point_ring1 = point_ring + (point_ring_count*point_ring_stride);


  const ON_SubDVertex* vertex0 = center_vertex;
  const size_t element_stride = (nullptr != vertex0) ? 1 : 2;
  if (nullptr == vertex0)
  {
    vertex0 = component_ring[0].Vertex();
    if (nullptr == vertex0)
      return ON_SUBD_RETURN_ERROR(0);
  }

  const ON_SubDEdgePtr* edges;

  // We need to count sharp edges, crease edges, and save edge sharpnesses.
  // edge_sharpness[i].[0] = sharpeness at vertex0 end.
  ON_SubDEdgeSharpness* edge_sharpness = nullptr;
  double maxs = 0.0;
  double sector_coefficient = (vertex0->IsDartOrCreaseOrCorner()) ? 0.0 : 0.5;

  edges = (1 == element_stride) ? vertex0->m_edges : (const ON_SubDEdgePtr*)(component_ring + 1);
  for (unsigned i = 0; i < N; ++i, edges += element_stride)
  {
    const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(edges->m_ptr);
    if (nullptr == edge)
      return ON_SUBD_RETURN_ERROR(0);
    const ON_SubDEdgeTag etag = edge->m_edge_tag;
    if (ON_SubDEdgeTag::Smooth == etag || ON_SubDEdgeTag::SmoothX == etag)
    {
      if (0.0 == sector_coefficient)
      {
        // The sector coefficient for the central vertex is a property
        // of the sector and constant on all smooth edges in the sector.
        // So, it suffices to harvest it from any smooth edge in the sector.
        sector_coefficient = edge->m_sector_coefficient[ON_SUBD_EDGE_DIRECTION(edges->m_ptr)];
      }
      ON_SubDEdgeSharpness s = edge->Sharpness();
      if ( s.IsNotZero() )
      {
        if (nullptr == edge_sharpness)
          edge_sharpness = (ON_SubDEdgeSharpness*)calloc(N, sizeof(edge_sharpness[0]));
        if (1 == ON_SUBD_EDGE_DIRECTION(edges->m_ptr))
          s = s.Reversed();
        edge_sharpness[i] = s;
        if (maxs < s[0])
          maxs = s[0];
        if (maxs < s[1])
          maxs = s[1];
      }
    }
  }

  if (nullptr != edge_sharpness && bSecondPass)
  {
    // Let the subdivision in the 2nd pass do some (or all) of the work.
    bFirstPass = false;
  }

  // Except for internal testing functions, bSecondPass is always true.
  // Sometimes bFirstPass is false when it is already known that
  // an ring edge vertex is tagged.
  for (unsigned int pass = (bFirstPass ? 0U : 1U); pass < (bSecondPass ? 2U : 1U); pass++)
  {
    if (1 == pass && nullptr != edge_sharpness)
    {
      if (maxs <= 1.0)
      {
        // The single subdivision step that occurs during this
        // pass will remove all sharpness from the point ring
        // and Internal_PointRingSharpSubdivision() doesn't need to be called.
        onfree(edge_sharpness);
        edge_sharpness = nullptr;
        maxs = 0.0;
      }
      else
      {
        // we need to subdivide the sharpness values that will get passed
        // to Internal_PointRingSharpSubdivision().
        maxs = 0.0;
        for (unsigned k = 0; k < N; ++k)
        {
          const ON_SubDEdgeSharpness s1 = edge_sharpness[k].Subdivided(0);
          edge_sharpness[k] = s1;
          if (maxs < s1[0])
            maxs = s1[0];
          if (maxs < s1[1])
            maxs = s1[1];
        }
        if (0.0 == maxs)
        {
          // subdivision removed sharpness.
          onfree(edge_sharpness);
          edge_sharpness = nullptr;
        }
      }
    }

    if (0 == pass)
      Q = vertex0->m_P;
    else
    {
      if ( false == bSecondPass)
        return ON_SUBD_RETURN_ERROR(0); // subdivision not permitted

      if (false == vertex0->GetSubdivisionPoint(subP))
        return ON_SUBD_RETURN_ERROR(0);

      Q = subP;
    }

    double* P = point_ring;
    P[0] = Q[0];
    P[1] = Q[1];
    P[2] = Q[2];
    P += point_ring_stride;

    const ON_SubDEdgePtr* edges0 = (1 == element_stride) ? vertex0->m_edges : (const ON_SubDEdgePtr*)(component_ring + 1);
    edges = edges0;
    const ON_SubDFacePtr* faces = (1 == element_stride) ? ((const ON_SubDFacePtr * )vertex0->m_faces) : (const ON_SubDFacePtr*)(component_ring + 2);
    for ( unsigned int i = 0; i < N; ++i, edges += element_stride, faces += element_stride )
    {
      // Get edge point
      ON__UINT_PTR eptr = edges->m_ptr;
      const ON_SubDEdge* edge = ON_SUBD_EDGE_POINTER(eptr);
      if (nullptr == edge)
        return ON_SUBD_RETURN_ERROR(0);
      eptr = 1 - ON_SUBD_EDGE_DIRECTION(eptr);
      const ON_SubDVertex* vertex1 = edge->m_vertex[eptr];
      if ( nullptr == vertex1)
        return ON_SUBD_RETURN_ERROR(0);

      if (0 == pass)
      {
        if (ON_SubDEdgeTag::SmoothX == edge->m_edge_tag)
          break; // need to use subdivision point in 2nd pass

        if (ON_SubDVertexTag::Smooth == vertex1->m_vertex_tag 
          || ON_SubDEdgeTag::Crease == edge->m_edge_tag
          || 0.5 == edge->m_sector_coefficient[eptr]
          )
        {
          // We have one of these cases.
          // 1) edge is smooth, vertex1 is smooth.
          // 2) edge is smooth, vertex1 is creas/corner/dart with an ordinary.
          // configuration (0.5 == edge->m_sector_coefficient[eptr]).
          // 3) edge is crease.
          // 
          // If the edge is not sharp, then the conditions
          // needed to use a matrix to calculate the limit
          // surface from the point ring are valid for this edge.
          // 
          // If the edge is sharp, a later call to
          // Internal_PointRingSharpSubdivision() will subdivide
          // the sharpness away.
          Q = vertex1->m_P;
        }
        else
        {
          // The edge is smooth, vertex1 is a crease/corner/dart, 
          // and there is a subdivision bias introduced by
          // an extraordinary configuration of edges at
          // vertex1. The subdivision in the 2nd pass will
          // remove these complications so the conditions
          // needed to use a matrix to calculate the limit
          // surface from the point ring will be valid.
          break;
        }
      }
      else
      {
        // The 2nd pass subdivides the elements in the initial ring
        // to insure the edge from the center to the ring point 
        // is either a smooth edge going to a smooth ring vertex
        // or a crease edge going to a crease ring vertex.
        // 
        // If the edge is sharp, a later call to
        // Internal_PointRingSharpSubdivision() will subdivide
        // the sharpness away.
        if (false == edge->GetSubdivisionPoint(subP))
          return ON_SUBD_RETURN_ERROR(0);
        // Q = subP set above when vertex0 was subdivided.
      }
      P[0] = Q[0];
      P[1] = Q[1];
      P[2] = Q[2];
      P += point_ring_stride;
      
      if (point_ring1 == P)
      {
        // success on a sector with crease boundary
        if (nullptr != edge_sharpness)
        {
          // At least one of the smooth edges is sharp.
          // Internal_PointRingSharpSubdivision() subdivides
          // the ring enough to remove sharpness so the conditions
          // needed to use a matrix to calculate the limit
          // surface from the point ring will be valid.
          Internal_PointRingSharpSubdivision(
            vertex0->m_vertex_tag,
            sector_coefficient,
            edges - (N - 1) * element_stride,
            element_stride,
            N,
            F,
            edge_sharpness,
            point_ring,
            point_ring_stride
          );
          onfree(edge_sharpness);
        }
        return point_ring_count;
      }

      const ON_SubDFace* face = ON_SUBD_FACE_POINTER(faces->m_ptr);
      if (0 == pass)
      {
        if (4 != face->m_edge_count)
        {
          // face is not a quad.
          // We need 2nd pass to subdivide this face so the center vertex is surrounded by quads.
          break; 
        }

        // find the vertex opposite vertex0
        eptr = face->m_edge4[0].m_ptr;
        edge = ON_SUBD_EDGE_POINTER(eptr);
        if ( nullptr == edge)
          return ON_SUBD_RETURN_ERROR(0);
        eptr = ON_SUBD_EDGE_DIRECTION(eptr);
        if (vertex0 == edge->m_vertex[eptr])
          eptr = 2; // vertex0 = face->Vertex(0), set vertexQ = face->vertex(2)
        else if (vertex0 == edge->m_vertex[1-eptr])
          eptr = 3; // vertex0 = face->Vertex(1), set vertexQ = face->vertex(3)
        else
        {
          eptr = face->m_edge4[2].m_ptr;
          edge = ON_SUBD_EDGE_POINTER(eptr);
          if ( nullptr == edge)
            return ON_SUBD_RETURN_ERROR(0);
          eptr = ON_SUBD_EDGE_DIRECTION(eptr);
          if (vertex0 == edge->m_vertex[eptr])
            eptr = 0; // vertex0 = face->Vertex(2), set vertexQ = face->vertex(0)
          else if (vertex0 == edge->m_vertex[1-eptr])
            eptr = 1; // vertex0 = face->Vertex(3), set vertexQ = face->vertex(1)
          else
            return ON_SUBD_RETURN_ERROR(0);
        }
        eptr = face->m_edge4[eptr].m_ptr;
        edge = ON_SUBD_EDGE_POINTER(eptr);
        if ( nullptr == edge)
          return ON_SUBD_RETURN_ERROR(0);
        eptr = ON_SUBD_EDGE_DIRECTION(eptr);
        const ON_SubDVertex* vertexQ = edge->m_vertex[eptr];
        if ( nullptr == vertexQ)
          return ON_SUBD_RETURN_ERROR(0);
        Q = vertexQ->m_P;
      }
      else
      {
        if (false == face->GetSubdivisionPoint(subP))
          return ON_SUBD_RETURN_ERROR(0);
        // Q = subP set above when vertex0 was subdivided.
      }
      P[0] = Q[0];
      P[1] = Q[1];
      P[2] = Q[2];
      P += point_ring_stride;
    }
    
    if (point_ring1 == P)
    {
      // success on a smooth sector
      if (nullptr != edge_sharpness)
      {
        // At least one of the smooth edges is sharp.
        // Internal_PointRingSharpSubdivision() subdivides
        // the ring enough to remove sharpness so the conditions
        // needed to use a matrix to calculate the limit
        // surface from the point ring will be valid.
        Internal_PointRingSharpSubdivision(
          vertex0->m_vertex_tag,
          sector_coefficient,
          edges - N * element_stride,
          element_stride,
          N,
          F,
          edge_sharpness,
          point_ring,
          point_ring_stride
        );
        onfree(edge_sharpness);
      }
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
  const ON_SubDComponentPtr* component_ring,
  size_t component_ring_count
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

  const ON_SubDEdgeTag edge0_tag 
    = (F+1 == N || (F == N && ON_SubDVertexTag::Dart == vertex->m_vertex_tag))
    ? ON_SubDEdgeTag::Crease
    : ON_SubDEdgeTag::Smooth;
  const ON_SubDEdgeTag edge1_tag 
    = (F+1 == N)
    ? ON_SubDEdgeTag::Crease
    : ON_SubDEdgeTag::Smooth;

  unsigned int component_ring_index = 1;
  for (unsigned int i = 0; i < N; i++, component_ring_index++)
  {
    const ON_SubDEdge* edge = component_ring[component_ring_index].Edge();
    if ( nullptr == edge)
      return ON_SUBD_RETURN_ERROR(false);
    if (vertex != edge->m_vertex[component_ring[component_ring_index].ComponentDirection()])
      return ON_SUBD_RETURN_ERROR(false);

    if (0 == i)
    {
      if (edge0_tag != edge->m_edge_tag)
      {
        if (  ON_SubDEdgeTag::Smooth != edge0_tag || ON_SubDEdgeTag::SmoothX != edge->m_edge_tag )
          return ON_SUBD_RETURN_ERROR(false);
      }
    }
    else if (i+1 == N)
    {
      if (edge1_tag != edge->m_edge_tag)
      {
        if (  ON_SubDEdgeTag::Smooth != edge1_tag || ON_SubDEdgeTag::SmoothX != edge->m_edge_tag )
          return ON_SUBD_RETURN_ERROR(false);
      }
      if ( ON_SubDEdgeTag::Crease == edge1_tag)
        continue;
    }
    else
    {
      if (2 != edge->m_face_count || false == edge->IsSmooth())
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
  unsigned int point_ring_count = GetSectorPointRing( bSubdivideIfNeeded, component_ring, component_ring_count, &point_ring_array[0].x, point_ring_capacity, 3);
  if (point_ring_count > 0)
  {
    point_ring.SetCount(point_ring_count);
    return point_ring_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}


unsigned int ON_SubD::GetSectorSubdivsionPointRing(
  const ON_SubDComponentPtr* component_ring,
  size_t component_ring_count,
  double* subd_point_ring,
  size_t subd_point_ring_capacity,
  size_t subd_point_ring_stride
  )
{
  if (false == ComponentRingIsValid(component_ring,component_ring_count))
    return ON_SUBD_RETURN_ERROR(0);


  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = ON_SubD::ComponentRingFaceCount(component_ring_count);
  const unsigned int point_ring_count = N + F;
  if ( point_ring_count > subd_point_ring_capacity || nullptr == subd_point_ring)
    return ON_SUBD_RETURN_ERROR(0);

  const bool bFirstPass = false;
  const bool bSecondPass = true; // returned ring will be at subdivision level 1 (or greater if there are edges with sharpness > 1)
  unsigned int rc = GetQuadSectorPointRing(bFirstPass,bSecondPass,nullptr,component_ring,component_ring_count, subd_point_ring, subd_point_ring_stride);

  if (0 == rc)
    return ON_SUBD_RETURN_ERROR(0);

  return rc;

}

unsigned int ON_SubD::GetSectorSubdivisionPointRing(
  const ON_SubDComponentPtr* component_ring,
  size_t component_ring_count,
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
  unsigned int subd_point_ring_count = GetSectorSubdivsionPointRing(component_ring, component_ring_count, &subd_point_ring_array[0].x, subd_point_ring_capacity, 3);
  if (subd_point_ring_count > 0)
  {
    subd_point_ring.SetCount(subd_point_ring_count);
    return subd_point_ring_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}

unsigned int ON_SubD::GetSectorPointRing(
  bool bSubdivideIfNeeded,
  const ON_SubDComponentPtr* component_ring,
  size_t component_ring_count,
  double* point_ring,
  size_t point_ring_capacity,
  size_t point_ring_stride
  )
{
  if (false == ComponentRingIsValid(component_ring,component_ring_count))
    return ON_SUBD_RETURN_ERROR(0);

  const unsigned int N = ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = ON_SubD::ComponentRingFaceCount(component_ring_count);
  const unsigned int point_ring_count = N + F;
  if ( point_ring_count > point_ring_capacity || nullptr == point_ring)
    return ON_SUBD_RETURN_ERROR(0);

  const bool bFirstPass = true;
  // Except for internal testing functions, bSubdivideIfNeeded is always true.
  const bool bSecondPass = bSubdivideIfNeeded;
  unsigned int rc = GetQuadSectorPointRing(bFirstPass,bSecondPass,nullptr, component_ring,component_ring_count, point_ring,point_ring_stride);

  if (0 == rc)
    return ON_SUBD_RETURN_ERROR(0);

  return rc;
}


unsigned int ON_SubD::GetSectorPointRing(
  bool bSubdivideIfNeeded,
  const class ON_SubDSectorIterator& sit,
  double* point_ring,
  size_t point_ring_capacity,
  size_t point_ring_stride
  )
{
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
  unsigned int component_ring_count = ON_SubD::GetSectorComponentRing(sit, component_ring,component_ring_capacity);
  if (component_ring_count > 0)
  {
    const bool bFirstPass = true;
    const bool bSecondPass = bSubdivideIfNeeded;
    point_ring_count = ON_SubD::GetQuadSectorPointRing( bFirstPass, bSecondPass, nullptr, component_ring, component_ring_count, point_ring, point_ring_stride);
  }
  
  if ( component_ring != stack_component_ring)
    delete[] component_ring;

  return point_ring_count;
}

unsigned int ON_SubD::GetSectorPointRing(
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
  unsigned int point_ring_count = GetSectorPointRing( bSubdivideIfNeeded, sit, &point_ring_array[0].x, point_ring_capacity, 3);
  if (point_ring_count > 0)
  {
    point_ring.SetCount(point_ring_count);
    return point_ring_count;
  }
  return ON_SUBD_RETURN_ERROR(0);
}

const ON_SubDVertex* ON_SubD::SubdivideSector(
  const ON_SubDVertex* center_vertex,
  const ON_SubDComponentPtr* component_ring,
  size_t component_ring_count,
  ON_SubD_FixedSizeHeap& fsh
  )
{
  const unsigned int N = (nullptr != center_vertex) ? center_vertex->m_edge_count : ON_SubD::ComponentRingEdgeCount(component_ring_count);
  const unsigned int F = (nullptr != center_vertex) ? center_vertex->m_face_count : ON_SubD::ComponentRingFaceCount(component_ring_count);
  if ( N < 2 )
    return ON_SUBD_RETURN_ERROR(nullptr);
  if ( F != N && F+1 != N )
    return ON_SUBD_RETURN_ERROR(nullptr);

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
  }

  // smooth and dart sectors have F = N
  // crease and corner have F = N-1
  if (F != (center_vertex->IsCreaseOrCorner() ? (N-1) : N) )
  {
    return ON_SUBD_RETURN_ERROR(nullptr);
  }
  
  const ON_SubDEdgeTag edge0_tag = (F+1 == N) ? ON_SubDEdgeTag::Crease : ON_SubDEdgeTag::Smooth;

  //const unsigned int face_edge_count = 4;
  const unsigned int K = 3;
  
  const ON_SubDEdge* edge0 = edges->Edge();
  if ( nullptr == edge0)
    return ON_SUBD_RETURN_ERROR(nullptr);
  edges += element_stride;

  if (ON_SubDEdgeTag::Smooth == edge0_tag)
  {
    if (false == edge0->IsSmooth() )
      return ON_SUBD_RETURN_ERROR(nullptr);
  }
  else 
  {
    if (edge0_tag != edge0->m_edge_tag)
      return ON_SUBD_RETURN_ERROR(nullptr);
  }
  
  const ON_SubDFace* face0 = faces->Face();
  if ( nullptr == face0)
    return ON_SUBD_RETURN_ERROR(nullptr);
  edges += element_stride;

  if ( false == fsh.ReserveSubDWorkspace(N) )
    return ON_SUBD_RETURN_ERROR(nullptr);

  ON_SubDVertex* v1[4] = {};
  ON_SubDEdgePtr e1[4] = {};
  ON_SubDEdgePtr f1epts[4] = {};

  const ON_SubDVertex* vertex0 = center_vertex;

  v1[0] = fsh.AllocateVertex(vertex0,N);
  if ( nullptr == v1[0])
    return ON_SUBD_RETURN_ERROR(nullptr);
  //v1[0]->m_vertex_edge_order = ON_SubD::VertexEdgeOrder::radial;

  ON_SubDVertex* vertex1 = fsh.AllocateVertex(edge0);
  if ( nullptr == vertex1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  // at_crease weight is used when the cooresponding vertex is a crease.
  // Otherwise, fsh.AllocateEdge() ignores at_crease_weight.
  ON_SubDEdgeTag edge1_tag = (ON_SubDEdgeTag::SmoothX == edge0_tag) ? ON_SubDEdgeTag::Smooth : edge0_tag;
  const double at_crease_weight 
    = ON_SubDEdgeTag::Crease == edge1_tag
    ? ON_SubDSectorType::CreaseSectorCoefficient(5-K)
    : ON_SubDSectorType::IgnoredSectorCoefficient;
  ON_SubDEdgePtr edge1 = fsh.AllocateEdge(v1[0], ON_SubDSectorType::CopyEdgeSectorCoefficient(edge0,vertex0,ON_UNSET_VALUE), vertex1, ON_SubDSectorType::IgnoredSectorCoefficient );
  if (edge1.IsNull())
    return ON_SUBD_RETURN_ERROR(nullptr);
  edge1.Edge()->m_edge_tag = edge1_tag;
  
  v1[1] = vertex1;
  e1[0] = edge1;
  f1epts[0] = e1[0];
  edge1_tag = ON_SubDEdgeTag::Smooth;
    
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
      if (ON_SubDEdgeTag::Smooth == edge1_tag)
      {
        v1[K] = vertex1;
        e1[K] = edge1;
      }
    }

    if (nullptr == v1[K])
    {
      v1[K] = fsh.AllocateVertex(edge0);
      if (nullptr == v1[K])
        return ON_SUBD_RETURN_ERROR(nullptr);
      e1[K] = fsh.AllocateEdge(v1[0], ON_SubDSectorType::CopyEdgeSectorCoefficient(edge0, vertex0, ON_UNSET_VALUE), v1[K], ON_SubDSectorType::IgnoredSectorCoefficient);
      if (e1[K].IsNull())
        return ON_SUBD_RETURN_ERROR(nullptr);
      e1[K].Edge()->m_edge_tag = edge1_tag;
    }

    f1epts[K] = e1[K].Reversed();

    // quads
    v1[2] = fsh.AllocateSectorFaceVertex(face0 );
    e1[1] = fsh.AllocateEdge(v1[1], at_crease_weight, v1[2], ON_SubDSectorType::IgnoredSectorCoefficient);
    e1[2] = fsh.AllocateEdge(v1[2], ON_SubDSectorType::IgnoredSectorCoefficient, v1[3], at_crease_weight);
    f1epts[1] = e1[1];
    f1epts[2] = e1[2];
    if (nullptr == fsh.AllocateQuad(face0->m_level_zero_face_id, face0->m_id, f1epts) )
      return ON_SUBD_RETURN_ERROR(nullptr);
      
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
    e1[K] = ON_SubDEdgePtr::Null;
  }

  return ON_SUBD_RETURN_ERROR(nullptr);
}

unsigned int ON_SubD::GetSectorComponentRing(
  const class ON_SubDSectorIterator& sit,
  ON_SubDComponentPtr* component_ring,
  size_t component_ring_capacity
  )
{
  if ( component_ring_capacity < 4 || nullptr == component_ring)
    return ON_SUBD_RETURN_ERROR(0);

  const ON_SubDVertex* vertex = sit.CenterVertex();
  if ( nullptr == vertex || vertex->m_edge_count < 2 || vertex->m_face_count < 1)
    return ON_SUBD_RETURN_ERROR(0);

  const ON_SubDVertexTag center_vertex_tag = vertex->m_vertex_tag;

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

  if (bCreases && ON_SubDEdgeTag::Crease != edge0->m_edge_tag)
    return ON_SUBD_RETURN_ERROR(0);

  unsigned int component_ring_count = 0;
  component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(vertex);
  component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(edgeptr);
  component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(faceptr);
  const unsigned int N = vertex->m_edge_count; // for () used to prevent infinite recursion when vertex is not valid
  for (unsigned int i = 0; i < N; i++)
  {
    const ON_SubDFace* face = localsit.NextFace(ON_SubDSectorIterator::StopAt::AnyCrease);

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
        if (ON_SubDVertexTag::Smooth == center_vertex_tag)
        {
          if (face == face0 && ON_SubDEdgeTag::Smooth == edge0->m_edge_tag)
            return component_ring_count; // back to start smooth case.
        }
        if (ON_SubDVertexTag::Dart == center_vertex_tag)
        {
          if (nullptr == face && ON_SubDEdgeTag::Crease == edge0->m_edge_tag)
            return component_ring_count; // back to start dart case.
        }
        if (ON_SubDVertexTag::Corner == center_vertex_tag)
        {
          // occurs in nonmanifold cases like the one in RH-49843
          if (nullptr == face && ON_SubDEdgeTag::Crease == edge0->m_edge_tag)
            return component_ring_count; // back to start corner case.
        }
      }

      return ON_SUBD_RETURN_ERROR(false); // damaged topology information
    }

    if ( component_ring_count >= component_ring_capacity)
      return ON_SUBD_RETURN_ERROR(false);

    component_ring[component_ring_count++] = ON_SubDComponentPtr::Create(edgeptr);
    
    if (nullptr == face)
    {
      if (bCreases && ON_SubDEdgeTag::Crease == edge->m_edge_tag)
        return component_ring_count;
      return ON_SUBD_RETURN_ERROR(0);
    }

    if ( false == edge->IsSmooth() || 2 != edge->m_face_count )
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
  unsigned int component_ring_count = ON_SubD::GetSectorComponentRing(sit, elements.Reserve(component_ring_capacity), component_ring_capacity);
  if (component_ring_count >= 4 && component_ring_count <= component_ring_capacity)
    elements.SetCount(component_ring_count);

  return elements.UnsignedCount();
}
