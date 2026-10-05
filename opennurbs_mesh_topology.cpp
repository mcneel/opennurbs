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
#include <algorithm>

// The face-side sorts below dominate the cost of building topology for a dense
// mesh, and topology is what snapping, picking, selection and the wireframe
// display all wait on.
#include "opennurbs_parallel_sort.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

static ON_MeshFaceSide ON_MeshFaceSide_UnsetInitializer()
{
  ON_MeshFaceSide unset;
  memset(&unset, 0, sizeof(unset));
  return unset;
}

const ON_MeshFaceSide ON_MeshFaceSide::Unset = ON_MeshFaceSide_UnsetInitializer();

static int ON__MeshFaceSide_compare_m_vi(const ON_MeshFaceSide* side1, const ON_MeshFaceSide* side2)
{
  if (side1->m_vi[0] < side2->m_vi[0])
    return -1;
  if (side1->m_vi[0] > side2->m_vi[0])
    return 1;
  if (side1->m_vi[1] < side2->m_vi[1])
    return -1;
  if (side1->m_vi[1] > side2->m_vi[1])
    return 1;
  if (side1->m_fi < side2->m_fi)
    return -1;
  if (side1->m_fi > side2->m_fi)
    return 1;
  if (side1->m_side < side2->m_side)
    return -1;
  if (side1->m_side > side2->m_side)
    return 1;
  if (side1->m_dir < side2->m_dir)
    return -1;
  if (side1->m_dir > side2->m_dir)
    return 1;
  return 0;
}

static int ON__MeshFaceSide_compare_m_fi( ON_MeshFaceSide const* side1, ON_MeshFaceSide const* side2)
{
  if (side1->m_fi < side2->m_fi)
    return -1;
  if (side1->m_fi > side2->m_fi)
    return 1;
  if (side1->m_vi[0] < side2->m_vi[0])
    return -1;
  if (side1->m_vi[0] > side2->m_vi[0])
    return 1;
  if (side1->m_vi[1] < side2->m_vi[1])
    return -1;
  if (side1->m_vi[1] > side2->m_vi[1])
    return 1;
  if (side1->m_side < side2->m_side)
    return -1;
  if (side1->m_side > side2->m_side)
    return 1;
  if (side1->m_dir < side2->m_dir)
    return -1;
  if (side1->m_dir > side2->m_dir)
    return 1;
  return 0;
}

int ON_MeshFaceSide::CompareFaceIndex(const ON_MeshFaceSide* a, const ON_MeshFaceSide* b)
{
  if (0 == a)
    a = &ON_MeshFaceSide::Unset;
  if (0 == b)
    b = &ON_MeshFaceSide::Unset;
  return ON__MeshFaceSide_compare_m_fi(a, b);
}

int ON_MeshFaceSide::CompareVertexIndex(const ON_MeshFaceSide* a, const ON_MeshFaceSide* b)
{
  if (0 == a)
    a = &ON_MeshFaceSide::Unset;
  if (0 == b)
    b = &ON_MeshFaceSide::Unset;
  return ON__MeshFaceSide_compare_m_vi(a, b);
}

void ON_MeshFaceSide::SortByVertexIndex(ON_MeshFaceSide* face_sides, size_t face_sides_count)
{
  if (face_sides_count >= 2 && 0 != face_sides)
  {
    ON_ParallelSort(face_sides, face_sides + face_sides_count, [](const ON_MeshFaceSide& a, const ON_MeshFaceSide& b) { return 0 > CompareVertexIndex(&a, &b); });
  }
}

void ON_MeshFaceSide::SortByFaceIndex(ON_MeshFaceSide* face_sides, size_t face_sides_count)
{
  if (face_sides_count >= 2 && 0 != face_sides)
  {
    ON_ParallelSort(face_sides, face_sides + face_sides_count, [](const ON_MeshFaceSide& a, const ON_MeshFaceSide& b) { return 0 > CompareFaceIndex(&a, &b); });
  }
}

//
// ON_MeshFaceSide
// 
///////////////////////////////////////////////////////////////////
