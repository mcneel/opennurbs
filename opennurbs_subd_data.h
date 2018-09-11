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

#if defined(OPENNURBS_INC_IN_PROGRESS)
///////////////////////////////////////////////////////////////////
//
// Including opennurbs.h must NEVER include this file.
//
///////////////////////////////////////////////////////////////////
#error Please read the comment above this line.
#endif

#if defined(ON_COMPILING_OPENNURBS)
#if !defined(OPENNURBS_SUBD_UNSTABLE_CORE_AVAIALABLE)
#define OPENNURBS_SUBD_UNSTABLE_CORE_AVAIALABLE
#endif
#endif


#if !defined(OPENNURBS_SUBD_UNSTABLE_CORE_AVAIALABLE)
///////////////////////////////////////////////////////////////////
//
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
//
// The definitions in opennurbs_subd_data.h change randomly and unpredictably.
// Never include this file in any code you write, even as a hack.
// If you ignore this warning, your code will crash randomly and unpredictably
//
///////////////////////////////////////////////////////////////////
#error Please read the comment above this line.
#endif

#if !defined(OPENNURBS_SUBD_DATA_INC_)
#define OPENNURBS_SUBD_DATA_INC_

#if defined(OPENNURBS_SUBD_WIP)


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFaceNeighborhood
//
class ON_CLASS ON_SubDFaceNeighborhood
{
public:
  ON_SubDFaceNeighborhood() = default;
  ~ON_SubDFaceNeighborhood();

  // initial face
  const ON_SubDFace* m_face0 = nullptr;

  // subdivision algorithm
  ON_SubD::SubDType m_subd_type = ON_SubD::SubDType::Unset;
  
  // When the subd_type is ON_SubD::SubDType::QuadCatmullClark, 
  // the center vertex will be a smooth vertex with valence = m_face0->m_edge_count.
  // The edges and faces are sorted radially and all faces are quads.
  //
  // When the subd_type is ON_SubD::SubDType::TriLoopWarren and m_face0 is not a triangle,
  // m_center_vertex is a smooth vertex with valence = m_face0->m_edge_count. 
  // The edges and faces are sorted radially and all faces are tris.
  //
  // In all other cases, m_center_vertex is null.
  const ON_SubDVertex* m_center_vertex1 = nullptr;

  // m_face1[] is a list of the subdivision faces that subdivide the
  // original face. When the subd_type is ON_SubD::SubDType::TriLoopWarren
  // and the initial face is a triangle, the first face is the interior
  // subdivision triangle.  In all othr cases, m_face1 is identical
  // to m_center_vertex1->m_faces[]. The m_next_face pointers are set so
  // that m_face1[i]->m_next_face = m_face1[i+1].  Note that
  // m_face1[m_face1_count-1]->m_next_face may not be null.
  unsigned int m_face1_count = 0;
  const class ON_SubDFace** m_face1 = nullptr;

  /*
  Description:
    Apply a single iteration of the subdivision algorithm to face
    and save the results in this ON_SubDFaceNeighborhood.  If a vertex
    of the initial face is extraordinary, the limit point and normal 
    are calculated from face and saved on the subdivision.

    The resulting section of a subdivision surface is suitable for
    limit surface and limit mesh evaluation of the original face.
    The ON_SubDFaceNeighborhood is designed to be used to calculate
    the limit surface and limit mesh for faces that have an extraordinary
    number of edges.
  */
  bool Subdivide(
    ON_SubD::SubDType subd_type,
    const ON_SubDFace* face
    );

private:
  bool TriSubdivideHelper(
    const ON_SubDFace* face
    );

  bool QuadSubdivideHelper(
    const ON_SubDFace* face
    );

  bool ReserveCapacity(
    ON_SubD::SubDType subd_type,
    const ON_SubDFace* face
    );

  ON_SubD_FixedSizeHeap m_fsh;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDQuadNeighborhood
//

class ON_CLASS ON_SubDQuadNeighborhood
{
public:
  ON_SubDQuadNeighborhood() = default;
  ~ON_SubDQuadNeighborhood();
private:
  void Internal_Destroy(bool bReinitialize);

private:
  ON_SubDQuadNeighborhood(const ON_SubDQuadNeighborhood&) = delete;
  ON_SubDQuadNeighborhood& operator=(const ON_SubDQuadNeighborhood&) = delete;

public:
  // true if the limit surface of the center quad is a smooth bi-cubic surface with CVs
  // at the m_vertex_grid[][] locations.
  bool m_bIsCubicPatch = false;
  unsigned char m_initial_subdivision_level = 0; // parent SubD subdivsion level at source
  unsigned char m_current_subdivision_level = 0; // current subdivision level of contents

  // m_face_grid[1][1] is extraordinary and interpolation through the limit point
  // will eventually be reqired to get an approximate cubic patch or an exact
  // quad mesh vertex.
  unsigned char m_extraordinary_corner_vertex_count = 0;
  // m_bExtraordinaryCornerVertex[] = true if the corresponding corner vertex of 
  // the center quad is extraordinary.
  bool m_bExtraordinaryCornerVertex[4]; 

  // m_bExactCubicPatchCorner[] = true if the sub 4x4 grid of subdivision points
  // for the corresponding quadrant can be used to create an exact quadrant cubic patch.
  unsigned char m_exact_quadrant_patch_count = 0;
  bool m_bExactQuadrantPatch[4];

  // m_bBoundaryCrease[] = true if the corresponding m_center_edges[] is a crease
  // and both end vertices are not darts.
  unsigned char m_boundary_crease_count = 0;
  bool m_bBoundaryCrease[4];

private:
  unsigned short m_reserved2 = 0;

public:
  const ON_SubDVertex* m_vertex_grid[4][4]; // vertex net m_quad_face corners = ([1][1], [2][1], [2][2], [1][2])

  const ON_SubDEdge* m_edge_grid[4][2];

  // m_face[][] is a 3x3 grid of faces.
  // Center face
  //   m_face[1][1] is the "central" quad face.
  // Side faces
  //   Side faces are null if the edge is a crease or has 1 == m_face_count.
  //   m_face[1][0] = the neighbor across m_center_edges[0]
  //   m_face[2][1] = the neighbor across m_center_edges[1]
  //   m_face[1][2] = the neighbor across m_center_edges[2]
  //   m_face[0][1] = the neighbor across m_center_edges[3]
  const ON_SubDFace* m_face_grid[3][3];

  // edges of center face;
  // m_center_edge[0] connects m_vertex_grid[1][1] and m_vertex_grid[2][1]
  // m_center_edge[1] connects m_vertex_grid[2][1] and m_vertex_grid[2][2]
  // m_center_edge[2] connects m_vertex_grid[2][2] and m_vertex_grid[1][2]
  // m_center_edge[3] connects m_vertex_grid[1][2] and m_vertex_grid[1][1]
  const ON_SubDEdge* m_center_edges[4]; 

  // If not null, the storage for the referenced subd information is from m_fsh.
  class ON_SubD_FixedSizeHeap* m_fsh = nullptr;

  // level 1 subdivision control points
  double m_srf_cv1[5][5][3];

  /*
  Returns:
    The center quad face m_face[1][1].
  */
  const ON_SubDFace* CenterQuad() const;

  const ON_SubDVertex* CenterVertex(
    int vi
    ) const;

  /*
  Parameters:
    fei - [in]
      center quad face edge index (0 to 3)
  Returns:
     A pointer to the neighbor face across m_face[1][1]->Edge(fei)
     This face will be null if the edge is a crease or has 2 != m_face_count
       m_face[0][1] = the neighbor across m_face[1][1]->Edge(0) or null if the Edge(0) is a crease.
       m_face[1][2] = the neighbor across m_face[1][1]->Edge(1) or null if the Edge(1) is a crease
       m_face[2][1] = the neighbor across m_face[1][1]->Edge(2) or null if the Edge(2) is a crease
       m_face[1][0] = the neighbor across m_face[1][1]->Edge(3) or null if the Edge(3) is a crease
  */
  const ON_SubDFace* SideFace(
    unsigned int fei
    ) const;

  /*
  Parameters:
    fei - [in]
      center quad face vertex index (0 to 3)
  Returns:
     A pointer to the neighbor face opposite m_face[1][1]->Vertex(fvi).
     This face will be null if the vertex valence is not 4 or any of its edges are creases.
     m_face[0][0] = the neighbor diagonal from m_face[1][1]->Vertex(0)
     m_face[0][2] = the neighbor diagonal from m_face[1][1]->Vertex(1)
     m_face[2][2] = the neighbor diagonal from m_face[1][1]->Vertex(2)
     m_face[2][0] = the neighbor diagonal from m_face[1][1]->Vertex(3)
  */
  const ON_SubDFace* CornerFace(
    unsigned int fvi
    ) const;

  bool Set(
    const ON_SubDFace* center_quad_face
    );

  /*
  Description:
    Clear current settings so the ON_SubDQuadNeighborhood can be reused.
  Parameters:
    subd_quad_nbd - [in/out]
      ON_SubDQuadNeighborhood to clear.
    bRetainFixedSizeHeap - [in]
      The m_fsh heap is always reset. When bRetainFixedSizeHeap is true,
      m_fsh is not set to nullptr.
  */
  static void Clear(
    ON_SubDQuadNeighborhood* subd_quad_nbd,
    bool bRetainFixedSizeHeap
    );

  bool IsValid() const;

  bool IsSet() const
  {
    return (nullptr != m_face_grid[1][1]);
  }

  /*
  Description:
    Get the limit surface for the entrie quad
  */
  bool GetLimitSurfaceCV(
    //double srf_cv[4][4][3]
    double* srf_cv,
    unsigned int srf_cv_grid_size // 4 or 5
    ) const;

  /*
  Description:
    Get the limit sub surface exact patch for the specifed corner.
  Returns:
    true when srf_cv are set to a the CVs for a bicubic uniform cubic NURBS bispan patch
  */
  bool GetLimitSubSurfaceSinglePatchCV(
    unsigned int fvi,
    double srf_cv[4][4][3]
    );

  /*
  Parameters:
    unset_cv - [in]
      If a patch cv srf_cv[j][j] does not exist or cannot be set, then all three
      coordinates of srf_cv[j][j] are set to unset_cv.  Use a value like
      ON_UNSET_VALUE or ON_DBL_QNAN.
    bEnableApproximatePatch - [in]
      If true, an approximate patches may be generated.
      This parameter should be true only when all permitted subdivisions have
      been performed and a mininimum of 2 subdivisions have been performed.
      This insures all faces are quads and each quad has at most one extraordinary
      vertex and the approximate patches will have C2 continuity with any
      neighboring exact patches.
    srf_cv[5][5]
      CVs for a uniform cubic NURBS patch.
    patch_type - [out]
      patch_type[0] 
        the type of cubic bispan for the 4x4 grid (srf_cv[0][0] ... srf_cv[3][3])
      patch_type[1] 
        the type of cubic bispan for the 4x4 grid (srf_cv[1][0] ... srf_cv[4][3])
      patch_type[2] 
        the type of cubic bispan for the 4x4 grid (srf_cv[1][1] ... srf_cv[4][4])
      patch_type[3] 
        the type of cubic bispan for the 4x4 grid (srf_cv[0][1] ... srf_cv[3][4])
  Returns:
    0 - 4:
    Number of bispans set in srf_cv[5][5]
  */
  unsigned int GetLimitSubSurfaceMultiPatchCV(
    double unset_cv,
    bool bEnableApproximatePatch,
    double srf_cv[5][5][3],
    ON_SubDLimitPatchFragment::PatchType patch_type[4]
    );

private:
  unsigned int SetLimitSubSurfaceExactCVs(
    unsigned int  quadrant_index // 0,1,2,3 sets quadrant, 4 sets all non extraordinary patches
    );

private:
  /*
  Parameters:
    i - [in] (0 <= i < 5)      
    j - [in] (0 <= j < 5)
      srf_cv[i][j]
    unset_cv - [in]
      unset cv coordinate value
      (uses when false is returned)
    approximate_cv - [out]
      location for a cubic NURBS CV that will yield a smooth result but 
      approximate the true subdivision surface.      
  */
  bool GetApproximateCV(
    int i,
    int j,
    double unset_cv,
    double approximate_cv[3]
    ) const;

private:
  // After m_face_grid, m_vertex_grid, m_edge_grid and m_bBoundaryCrease[] are set,
  // this helper function sets the remaining information.
  void SetPatchStatus(
    const unsigned int fvi0
    );

  // SetPatchStatus() uses this helper to set m_bIsCubicPatch
  bool VertexGridIsExactCubicPatch(
    const ON_2dex min_face_grid_dex,
    const ON_2dex max_face_grid_dex,
    unsigned int boundary_corner_index
    ) const;

public:

  /*
  Description:
    Apply a single iteration of the built-in quad subdivision algorithm to
    this.
  */
  bool Subdivide(
    const unsigned int q0fvi,
    ON_SubD_FixedSizeHeap& fsh,
    class ON_SubDQuadNeighborhood* q1ft
    ) const;

  static ON_2dex GridDex(
    unsigned int grid_size,
    unsigned int corner_index,
    unsigned int i,
    unsigned int j
    );

  static ON_2dex DeltaDex(
    unsigned int corner_index,
    int delta_i,
    int delta_j
    );

  static bool GetSubdivisionPoint(
    const ON_SubDVertex* vertex,
    double P1[3]
    );

  static bool GetLimitPoint(
    const ON_SubDVertex* vertex,
    double limitP[3],
    double limitN[3]
    );

  static bool GetSubdivisionPoint(
    const ON_SubDEdge* edge,
    double P1[3]
    );

  static bool GetSubdivisionPoint(
    const ON_SubDFace* face,
    double P1[3]
    );
};

#if defined(ON_COMPILING_OPENNURBS)

////////////////////////////////////////////////////////////////
//
//   Implementation of subdivision surface
//   
//   This implementation will change randomly and unpredictably.
//
////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDIncrementErrorCount() 
//
//  Appears in places where the code traps error conditions.
//  Putting a breakpoint on the line indicated below is an easy way
//  to have the debugger break on all error conditions and inspect
//  the first place something goes wrong in a complex calculation.
//
void ON_SubDIncrementErrorCount(); // defined in opennurbs_subd.cpp
#define ON_SUBD_RETURN_ERROR(rc) (ON_SubDIncrementErrorCount(),rc)
#define ON_SUBD_ERROR(msg) (ON_SubDIncrementErrorCount(),ON_ERROR(msg))

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdgePtr and ON_SubDFacePtr are unsigned ints that store a 
// pointer to an ON_SubDEdge or ON_SubDFace along with a bit that 
// is 0 or 1 which is used to indicate the end of the edge that is 
// the "start" or if an edge is "reversed" along the side of a face.
//
// This code assumes that ON_SubDVertex, ON_SubDEdge, and ON_SubDFace
// classes begin on an 8 bytes boundary because they contain doubles.
// If this assumption is false, you will get lots of crashes.
//
#define ON_SUBD_ELEMENT_MARK_MASK (0x1U)
#define ON_SUBD_ELEMENT_TYPE_MASK (0x6U)
#define ON_SUBD_ELEMENT_FLAGS_MASK (0x7U)
#if (8 == ON_SIZEOF_POINTER)
#define ON_SUBD_ELEMENT_POINTER_MASK (0xFFFFFFFFFFFFFFF8ULL)
#else
#define ON_SUBD_ELEMENT_POINTER_MASK (0xFFFFFFF8U)
#endif

#define ON_SUBD_ELEMENT_TYPE_VERTEX (0x2U)
#define ON_SUBD_ELEMENT_TYPE_EDGE (0x4U)
#define ON_SUBD_ELEMENT_TYPE_FACE (0x6U)

#define ON_SUBD_ELEMENT_POINTER(p) ((void*)((p) & ON_SUBD_ELEMENT_POINTER_MASK))
#define ON_SUBD_ELEMENT_MARK(p) ((p) & ON_SUBD_ELEMENT_MARK_MASK)
#define ON_SUBD_ELEMENT_TYPE(p) ((p) & ON_SUBD_ELEMENT_TYPE_MASK)
#define ON_SUBD_ELEMENT_FLAGS(p) ((p) & ON_SUBD_ELEMENT_FLAGS_MASK)

#define ON_SUBD_VERTEX_POINTER(p) ((class ON_SubDVertex*)ON_SUBD_ELEMENT_POINTER(p))
#define ON_SUBD_VERTEX_MARK(p) ON_SUBD_ELEMENT_MARK(p)

#define ON_SUBD_EDGE_POINTER(p) ((class ON_SubDEdge*)ON_SUBD_ELEMENT_POINTER(p))
#define ON_SUBD_FACE_POINTER(p) ((class ON_SubDFace*)ON_SUBD_ELEMENT_POINTER(p))

#define ON_SUBD_EDGE_DIRECTION(p) ON_SUBD_ELEMENT_MARK(p)
#define ON_SUBD_FACE_DIRECTION(p) ON_SUBD_ELEMENT_MARK(p)


//////////////////////////////////////////////////////////////////////////
//
// m_saved_points_flags
//
#define ON_SUBD_CACHE_TYPE_MASK (0x1FU)

#define ON_SUBD_CACHE_POINT_FLAG_MASK (0x20U)
#define ON_SUBD_CACHE_DISPLACEMENT_FLAG_MASK (0x40U)
#define ON_SUBD_CACHE_LIMIT_FLAG_MASK (0x80U)
#define ON_SUBD_CACHE_FLAGS_MASK (ON_SUBD_CACHE_POINT_FLAG_MASK|ON_SUBD_CACHE_DISPLACEMENT_FLAG_MASK|ON_SUBD_CACHE_LIMIT_FLAG_MASK)

#define ON_SUBD_CACHE_TYPE(cache_subd_flags) (ON_SUBD_CACHE_TYPE_MASK&(cache_subd_flags))
#define ON_SUBD_CACHE_FLAGS(cache_subd_flags) (ON_SUBD_CACHE_FLAGS_MASK&(cache_subd_flags))
#define ON_SUBD_CACHE_POINT_FLAG(cache_subd_flags) (ON_SUBD_CACHE_POINT_FLAG_MASK&(cache_subd_flags))
#define ON_SUBD_CACHE_DISPLACEMENT_FLAG(cache_subd_flags) (ON_SUBD_CACHE_DISPLACEMENT_FLAG_MASK&(cache_subd_flags))
#define ON_SUBD_CACHE_LIMIT_FLAG(cache_subd_flags) (ON_SUBD_CACHE_LIMIT_FLAG_MASK&(cache_subd_flags))

#define ON_SUBD_CACHE_CLEAR_POINT_FLAG(cache_subd_flags) (cache_subd_flags &= (ON_SUBD_CACHE_TYPE_MASK|ON_SUBD_CACHE_DISPLACEMENT_FLAG_MASK|ON_SUBD_CACHE_LIMIT_FLAG_MASK))
#define ON_SUBD_CACHE_CLEAR_DISPLACEMENT_FLAG(cache_subd_flags) (cache_subd_flags &= (ON_SUBD_CACHE_TYPE_MASK|ON_SUBD_CACHE_POINT_FLAG_MASK|ON_SUBD_CACHE_LIMIT_FLAG_MASK))
#define ON_SUBD_CACHE_CLEAR_LIMIT_FLAG(cache_subd_flags) (cache_subd_flags &= (ON_SUBD_CACHE_TYPE_MASK|ON_SUBD_CACHE_POINT_FLAG_MASK|ON_SUBD_CACHE_DISPLACEMENT_FLAG_MASK))


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDAggregates
//
class ON_SubDAggregates
{
public:
  void UpdateBoundingBox(
    const ON_SubDLevel* level
    );

  void UpdateEdgeFlags(
    const ON_SubDLevel* level
    );

  void UpdateAggregateComponentStatus(
    const ON_SubDLevel* level
    );

  void MarkAllAsNotCurrent()
  {
    m_bDirtyEdgeFlags = true;
    m_bDirtyBoundingBox = true;
    m_aggregate_status.MarkAsNotCurrent();
  }

  bool m_bDirtyEdgeFlags = false;
  bool m_bDirtyBoundingBox = false;
  ON_AggregateComponentStatus m_aggregate_status = ON_AggregateComponentStatus::Empty;
  unsigned int m_edge_flags;
  ON_BoundingBox m_bbox = ON_BoundingBox::EmptyBoundingBox;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDLevel
//

class ON_SubDLevel
{
public:
  static const ON_SubDLevel Empty;

  bool IsEmpty() const;

  ON_SubDLevel()
  {
    m_vertex[0] = m_vertex[1] = nullptr;
    m_edge[0] = m_edge[1] = nullptr;
    m_face[0] = m_face[1] = nullptr;
  }

private:
  ON_SubDLevel(const ON_SubDLevel&) = delete;
  ON_SubDLevel& operator=(const ON_SubDLevel&) = delete;

public:
  unsigned int m_level_index = ON_UNSET_UINT_INDEX;

  ON_SubD::SubDType m_subdivision_type = ON_SubD::SubDType::Unset;

  unsigned char m_ordinary_vertex_valence = 0; // 0 = none, 4 = quads, 6 = triangles
  unsigned char m_ordinary_face_edge_count = 0; // 0 = none, 4 = quads, 3 = triangles

private:
  unsigned char m_reserved1 = 0;

public:
  bool CopyHelper(
    const ON_SubDLevel& src, 
    class ON_SubDArchiveIdMap& eptrlist,
    class ON_SubDimple& subdimple,
    bool bCopyComponentStatus
    );

public:
  /*
  Returns:
    Number of changes to vertex tag, edge tag and edge sector coefficent values.
  */
  unsigned int UpdateEdgeTags(
    bool bUnsetEdgeTagsOnly
    );

  unsigned int UpdateVertexTags(
    bool bVertexEdgeTagsOnly
    );

  unsigned int UpdateAllTagsAndSectorCoefficients(
    bool bUnsetValuesOnly
    );

private:
  void DestroyOnError()
  {
    ON_SubDIncrementErrorCount();
    m_vertex_count = 0;
    m_edge_count = 0;
    m_face_count = 0;
    m_vertex_array_count = 0;
    m_edge_array_count = 0;
    m_face_array_count = 0;    m_vertex[0] = m_vertex[1] = nullptr;
    m_edge[0] = m_edge[1] = nullptr;
    m_face[0] = m_face[1] = nullptr;
    ResetVertexArray();
    ResetEdgeArray();
    ResetFaceArray();
  }

public:

  // m_vertex[2] = (head,tail) of linked list of vertices for this level
  class ON_SubDVertex* m_vertex[2];

  // m_edge[2] = (head,tail) of linked list of edges for this level
  class ON_SubDEdge* m_edge[2];

  // m_face[2] = (head,tail) of linked list of faces for this level
  class ON_SubDFace* m_face[2];

  unsigned int m_vertex_count = 0;
  unsigned int m_edge_count = 0;
  unsigned int m_face_count = 0;
  unsigned int m_vertex_array_count = 0;
  unsigned int m_edge_array_count = 0;
  unsigned int m_face_array_count = 0;

  std::shared_ptr<const ON_SubDVertex*> VertexArray() const;
  std::shared_ptr<const ON_SubDEdge*> EdgeArray() const;
  std::shared_ptr<const ON_SubDFace*> FaceArray() const;

  mutable ON_SubDLimitMesh m_limit_mesh;

  /*
  Description:
    Sets the mutable m_archive_id value for every vertex, edge and face 
    in this level.
  Parameters:
    archive_id_partition - [out]
      archive_id_partition[0] = 1 = first vertex archive_id
      archive_id_partition[1]-1 = last vertex archive_id
      archive_id_partition[1] = first edge archive_id
      archive_id_partition[2]-1 = last edge archive_id
      archive_id_partition[2] = first face archive_id
      archive_id_partition[3]-1 = last face archive_id
  Returns:
    The number of vertices, edges and faces on this level.
  */
  unsigned int SetArchiveId(
    unsigned int archive_id_partition[4]
    ) const;

  void ClearArchiveId() const;

  bool Read(
    ON_BinaryArchive& archive,
    class ON_SubDArchiveIdMap& element_list,
    ON_SubD& subd
    );

  bool Write(
    ON_BinaryArchive& archive
    ) const;

  /*
  Description:
    Apply a tranxfomration matrix to the entire level.
  Parameters:
    bGlobalTransformationIsIsometricOrUniformScale - [in]
      true if the transformation is isomectirc (rotation, translation),
      a dialoation (uniforma scale), or a compbination of those, and
      it will be applied to every component in the subdivision
      object.
      false otherwise.  
      If you don't know, pass false.
    xform - [in]
  */
  bool Transform(
    bool bGlobalTransformationIsIsometricOrDilation,
    const class ON_Xform& xform
    );

  bool SetSubDType(
    ON_SubD::SubDType subdivision_type
    );

  void ResetVertexArray()
  {
    if (m_vertex_array_count)
    {
      m_vertex_array_count = 0;
      m_vertex_array.reset();
    }
  }

  void ResetEdgeArray()
  {
    if (m_edge_array_count)
    {
      m_edge_array_count = 0;
      m_edge_array.reset();
    }
  }

  void ResetFaceArray()
  {
    if (m_face_array_count)
    {
      m_face_array_count = 0;
      m_face_array.reset();
    }
  }

  std::shared_ptr<const ON_SubDVertex*> m_vertex_array;
  std::shared_ptr<const ON_SubDEdge*> m_edge_array;
  std::shared_ptr<const ON_SubDFace*> m_face_array;

  bool RemoveVertex(class ON_SubDVertex* vertex)
  {
    m_aggregates.m_bDirtyBoundingBox = true;
    if (nullptr == vertex || vertex->m_level != this->m_level_index )
      return ON_SUBD_RETURN_ERROR(false);
    if ( 0 == m_vertex_count)
      return ON_SUBD_RETURN_ERROR(false);
    ON_SubDVertex* prev_vertex = const_cast<ON_SubDVertex*>(vertex->m_prev_vertex);
    ON_SubDVertex* next_vertex = const_cast<ON_SubDVertex*>(vertex->m_next_vertex);
    vertex->m_prev_vertex = nullptr;
    vertex->m_next_vertex = nullptr;
    if (1 == m_vertex_count)
    {
      if (m_vertex[0] == vertex && m_vertex[1] == vertex && nullptr == prev_vertex && nullptr == next_vertex)
      {
        m_vertex[0] = nullptr;
        m_vertex[1] = nullptr;
      }
      else
      {
        // error - same action taken
        DestroyOnError();
        return false;
      }
    }
    else
    {
      if (m_vertex[0] == vertex)
      {
        if (m_vertex_count >= 2 && nullptr == prev_vertex && nullptr != next_vertex )
        {
          m_vertex[0] = next_vertex;
          next_vertex->m_prev_vertex = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else if (m_vertex[1] == vertex)
      {
        if (m_vertex_count >= 2 && nullptr == next_vertex && nullptr != prev_vertex)
        {
          m_vertex[1] = prev_vertex;
          prev_vertex->m_next_vertex = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else
      {
        if (m_vertex_count > 2 && nullptr != prev_vertex && nullptr != next_vertex)
        {
          prev_vertex->m_next_vertex = next_vertex;
          next_vertex->m_prev_vertex = prev_vertex;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
    }
    m_vertex_count--;
    ResetVertexArray();
    return true;
  }


  bool RemoveEdge(class ON_SubDEdge* edge)
  {
    m_aggregates.m_bDirtyEdgeFlags = true;
    if (nullptr == edge || edge->m_level != this->m_level_index )
      return ON_SUBD_RETURN_ERROR(false);
    if ( 0 == m_edge_count)
      return ON_SUBD_RETURN_ERROR(false);
    ON_SubDEdge* prev_edge = const_cast<ON_SubDEdge*>(edge->m_prev_edge);
    ON_SubDEdge* next_edge = const_cast<ON_SubDEdge*>(edge->m_next_edge);
    edge->m_prev_edge = nullptr;
    edge->m_next_edge = nullptr;
    if (1 == m_edge_count)
    {
      if (m_edge[0] == edge && m_edge[1] == edge && nullptr == prev_edge && nullptr == next_edge)
      {
        m_edge[0] = nullptr;
        m_edge[1] = nullptr;
      }
      else
      {
        // error - same action taken
        DestroyOnError();
        return false;
      }
    }
    else
    {
      if (m_edge[0] == edge)
      {
        if (m_edge_count >= 2 && nullptr == prev_edge && nullptr != next_edge )
        {
          m_edge[0] = next_edge;
          next_edge->m_prev_edge = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else if (m_edge[1] == edge)
      {
        if (m_edge_count >= 2 && nullptr == next_edge && nullptr != prev_edge)
        {
          m_edge[1] = prev_edge;
          prev_edge->m_next_edge = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else
      {
        if (m_edge_count > 2 && nullptr != prev_edge && nullptr != next_edge)
        {
          prev_edge->m_next_edge = next_edge;
          next_edge->m_prev_edge = prev_edge;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
    }
    m_edge_count--;
    ResetEdgeArray();
    return true;
  }


  bool RemoveFace(class ON_SubDFace* face)
  {
    if (nullptr == face || face->m_level != this->m_level_index )
      return ON_SUBD_RETURN_ERROR(false);
    if ( 0 == m_face_count)
      return ON_SUBD_RETURN_ERROR(false);
    ON_SubDFace* prev_face = const_cast<ON_SubDFace*>(face->m_prev_face);
    ON_SubDFace* next_face = const_cast<ON_SubDFace*>(face->m_next_face);
    face->m_prev_face = nullptr;
    face->m_next_face = nullptr;
    if (1 == m_face_count)
    {
      if (m_face[0] == face && m_face[1] == face && nullptr == prev_face && nullptr == next_face)
      {
        m_face[0] = nullptr;
        m_face[1] = nullptr;
      }
      else
      {
        // error - same action taken
        DestroyOnError();
        return false;
      }
    }
    else
    {
      if (m_face[0] == face)
      {
        if (m_face_count >= 2 && nullptr == prev_face && nullptr != next_face )
        {
          m_face[0] = next_face;
          next_face->m_prev_face = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else if (m_face[1] == face)
      {
        if (m_face_count >= 2 && nullptr == next_face && nullptr != prev_face)
        {
          m_face[1] = prev_face;
          prev_face->m_next_face = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else
      {
        if (m_face_count > 2 && nullptr != prev_face && nullptr != next_face)
        {
          prev_face->m_next_face = next_face;
          next_face->m_prev_face = prev_face;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
    }
    m_face_count--;
    ResetFaceArray();
    return true;
  }

  const ON_SubDVertex* AddVertex(class ON_SubDVertex* vertex)
  {
    m_aggregates.m_bDirtyBoundingBox = true;
    if (nullptr == vertex)
      return nullptr;
    if (nullptr == m_vertex[1])
    {
      m_vertex[0] = vertex;
      vertex->m_prev_vertex = nullptr;
    }
    else
    {
      m_vertex[1]->m_next_vertex = vertex;
      vertex->m_prev_vertex = m_vertex[1];
    }
    m_vertex[1] = vertex;
    vertex->m_next_vertex = nullptr;
    m_vertex_count++;
    ResetVertexArray();
    return vertex;
  }

  const ON_SubDEdge* AddEdge(class ON_SubDEdge* edge)
  {
    m_aggregates.m_bDirtyEdgeFlags = true;
    if (nullptr == edge)
      return nullptr;
    if (nullptr == m_edge[1])
    {
      m_edge[0] = edge;
      edge->m_prev_edge = nullptr;
    }
    else
    {
      m_edge[1]->m_next_edge = edge;
      edge->m_prev_edge = m_edge[1];
    }
    m_edge[1] = edge;
    edge->m_next_edge = nullptr;
    m_edge_count++;
    ResetEdgeArray();
    return edge;
  }

  const ON_SubDFace* AddFace(class ON_SubDFace* face)
  {
    if (nullptr == face)
      return nullptr;
    if (nullptr == m_face[1])
    {
      m_face[0] = face;
      face->m_prev_face = nullptr;
    }
    else
    {
      m_face[1]->m_next_face = face;
      face->m_prev_face = m_face[1];
    }
    m_face[1] = face;
    face->m_next_face = nullptr;
    m_face_count++;
    ResetFaceArray();
    return face;
  }

private:
  unsigned int m_reserved4 = 0;
public:

  unsigned int UpdateEdgeSectorCoefficients(
    bool bUnsetEdgeSectorCoefficientsOnly
    );


  void ClearSubdivisonAndLimitPoints() const;

  /*
  Description:
    If a state is set in the status parameter, it will be cleared
    from all components on the level.
  Returns:
    Number of components where a state setting chanaged.
  */
  unsigned int ClearStates(
    ON_ComponentStatus states_to_clear
    ) const;

  unsigned int GetComponentsWithSetStates(
    ON_ComponentStatus states_filter,
    bool bAllEqualStates,
    ON_SimpleArray< ON_SubDComponentPtr >& components_with_set_states
    ) const;

  unsigned int SetStates(
    ON_SubDComponentPtr component_ptr,
    ON_ComponentStatus states_to_set
    ) const;

  unsigned int ClearStates(
    ON_SubDComponentPtr component_ptr,
    ON_ComponentStatus states_to_clear
    ) const;

  unsigned int SetStatus(
    ON_SubDComponentPtr component_ptr,
    ON_ComponentStatus status_to_copy
    ) const;

  ON_AggregateComponentStatus AggregateComponentStatus() const;

  void MarkAggregateComponentStatusAsNotCurrent() const
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
  }

  void ClearBoundingBox() const
  {
    m_aggregates.m_bDirtyBoundingBox = true;
  }

  ON_BoundingBox BoundingBox() const;

  void ClearEdgeFlags() const
  {
    m_aggregates.m_bDirtyEdgeFlags = true;
  }

  unsigned int EdgeFlags() const;

  ON_SubDLimitMesh UpdateLimitSurfaceMesh(
      const ON_SubD& subd,
      const class ON_SubDDisplayParameters& display_parameters
      ) const;

  unsigned int LimitSurfaceMeshFragmentCount() const;

private:
  mutable ON_SubDAggregates m_aggregates;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDLevelIterator
//
class ON_SubDLevelIterator
{
public:
  class ON_SubDLevel* First()
  {
    for (m_level_index = 0; m_level_index < m_level_count; m_level_index++)
    {
      if (nullptr != m_levels[m_level_index])
        return m_levels[m_level_index];
    }
    return nullptr;    
  }

  class ON_SubDLevel* Next()
  {
    for (m_level_index++; m_level_index < m_level_count; m_level_index++)
    {
      if (nullptr != m_levels[m_level_index])
        return m_levels[m_level_index];
    }
    return nullptr;    
  }

  unsigned int LevelIndex()
  {
    return m_level_index;
  }

  unsigned int LevelCount()
  {
    return m_level_count;
  }

private:
  friend class ON_SubDimple;
  ON_SubDLevelIterator() = default;
  ~ON_SubDLevelIterator() = default;
  ON_SubDLevelIterator(const ON_SubDLevelIterator&) = default;
  ON_SubDLevelIterator& operator=(const ON_SubDLevelIterator&) = default;

  class ON_SubDLevel** m_levels = nullptr;
  unsigned int m_level_count = 0;
  unsigned int m_level_index = 0;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDHeap
//

class ON_SubDHeap
{
public:
  ON_SubDHeap();
  ~ON_SubDHeap();

  class ON_SubDVertex* AllocateVertexAndSetId(unsigned int& max_vertex_id);

  void ReturnVertex(class ON_SubDVertex* v);

  class ON_SubDEdge* AllocateEdgeAndSetId(unsigned int& max_edge_id);

  void ReturnEdge(class ON_SubDEdge* e);

  class ON_SubDFace* AllocateFaceAndSetId(unsigned int& max_face_id);

  void ReturnFace(class ON_SubDFace* f);

  /*
  Description:
    Sets mutable m_archive_id values to 0.
  */
  void ClearArchiveId();

  const class ON_SubDVertex* VertexFromId(
    unsigned int vertex_id
    ) const;

  const class ON_SubDEdge* EdgeFromId(
    unsigned int edge_id
    ) const;

  const class ON_SubDFace* FaceFromId(
    unsigned int face_id
    ) const;

private:


  /*
  Returns:
  Array of count ON__UINT_PTR
  */
  ON__UINT_PTR* AllocateArray(
    size_t* capacity
    );
  void ReturnArray(
    size_t capacity,
    ON__UINT_PTR* a
    );
  static ON__UINT_PTR ArrayCapacity(
    size_t capacity,
    ON__UINT_PTR* a
    );
  ON__UINT_PTR* ResizeArray(
    size_t current_count,
    size_t current_capacity,
    ON__UINT_PTR* current_a,
    size_t* new_capacity
    );

public:
  bool GrowVertexEdgeArray(
    ON_SubDVertex* v,
    size_t capacity
    );
  bool GrowVertexFaceArray(
    ON_SubDVertex* v,
    size_t capacity
    );
  bool GrowEdgeFaceArray(
    ON_SubDEdge* e,
    size_t capacity
    );
  bool GrowFaceEdgeArray(
    ON_SubDFace* f,
    size_t capacity
    );

  bool GrowVertexEdgeArrayByOne(
    ON_SubDVertex* v
    );
  bool GrowVertexFaceArrayByOne(
    ON_SubDVertex* v
    );
  bool GrowEdgeFaceArrayByOne(
    ON_SubDEdge* e
    );
  bool GrowFaceEdgeArrayByOne(
    ON_SubDFace* f
    );

  bool ReturnVertexEdgeAndFaceArrays(
    ON_SubDVertex* v
    );

  bool ReturnEdgeExtraArray(
    ON_SubDEdge* e
    );

  bool ReturnFaceExtraArray(
    ON_SubDFace* f
    );


  /*
  Description:
    Discard all contents of this ON_SubDHeap.
  Remarks:
    More efficient than Destroy() if this ON_SubDHeap will be reused soon.
  */
  void Clear();

  /*
  Description:
    Delete all contents release all memory used by this ON_SubDHeap.
  */
  void Destroy();

  size_t SizeOf() const
  {
    size_t sz = sizeof(*this);
    sz += m_fsp5.SizeofElement()*m_fsp5.TotalElementCount();
    sz += m_fsp9.SizeofElement()*m_fsp9.TotalElementCount();
    sz += m_fsp17.SizeofElement()*m_fsp17.TotalElementCount();
    // todo - include m_ws;
    return sz;
  }

private:
  class tagWSItem
  {
  public:
    class tagWSItem* m_prev;
    class tagWSItem* m_next;
  };

  class tagWSItem* m_ws = nullptr;   // oversized arrays of ON__UINT_PTRs

  ON_FixedSizePool m_fspv;  // element = ON_SubDVertex
  ON_FixedSizePool m_fspe;  // element = ON_SubDEdge
  ON_FixedSizePool m_fspf;  // element = ON_SubDFace
  ON_FixedSizePool m_fsp5;  // element = capacity + array of 4 ON__UINT_PTRs
  ON_FixedSizePool m_fsp9;  // element = capacity + array of 8 ON__UINT_PTRs
  ON_FixedSizePool m_fsp17; // element = capacity + array of 16 ON__UINT_PTRs

  ON_SubDVertex* m_unused_vertex = nullptr;
  ON_SubDEdge* m_unused_edge = nullptr;
  ON_SubDFace* m_unused_face = nullptr;

  ON__UINT_PTR* AllocateOversizedElementQWERTY(
    size_t* capacity
    );
  void  ReturnOversizedElement(
    size_t capacity,
    ON__UINT_PTR* a
    );
  static size_t OversizedElementCapacityQWERTY(
    size_t count
    );

  static size_t m_offset_vertex_id;
  static size_t m_offset_edge_id;
  static size_t m_offset_face_id;
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDimple
//

class ON_SubDimple
{
#if defined(ON_SUBD_CENSUS)
  ON_SubDImpleCensusCounter m_census_counter;
#endif
public:
  ON_SubDimple() = default;
  ~ON_SubDimple();
  ON_SubDimple(const ON_SubDimple&);

  bool IsValid(
    const ON_SubD& subd,
    bool bSilentError,
    ON_TextLog*
    ) const;

  //unsigned int MaximumVertexId() const
  //{
  //  return m_max_vertex_id;
  //}

  //unsigned int MaximumEdgeId() const
  //{
  //  return m_max_edge_id;
  //}

  //unsigned int MaximumFaceId() const
  //{
  //  return m_max_face_id;
  //}

  bool Write(
         ON_BinaryArchive& archive
         ) const;

  bool Read(
         ON_BinaryArchive& archive,
         class ON_SubD& subd
         );

  bool Transform( 
    const ON_Xform& xform
    );

  const class ON_SubDVertex* VertexFromId(
    unsigned int vertex_id
    ) const
  {
    return m_heap.VertexFromId(vertex_id);
  }

  const class ON_SubDEdge* EdgeFromId(
    unsigned int edge_id
    ) const
  {
    return m_heap.EdgeFromId(edge_id);
  }

  const class ON_SubDFace* FaceFromId(
    unsigned int face_id
    ) const
  {
    return m_heap.FaceFromId(face_id);
  }

  ON_SubDLevelIterator LevelIterator() const
  {
    ON_SubDLevelIterator lit;
    lit.m_levels = (ON_SubDLevel**)m_levels.Array();
    lit.m_level_count = m_levels.UnsignedCount();
    return lit;
  }

  unsigned int LevelCount() const
  {
    return m_levels.UnsignedCount();
  }

public:
  class ON_SubDEdge* AddEdge(
    ON_SubD::EdgeTag edge_tag,
    ON_SubDVertex* v0,
    double v0_sector_weight,
    ON_SubDVertex* v1,
    double v1_sector_weight
    );

  /*
  Description:
    Split and edge.
    The input edge is modifed to terminate at the input vertex.
    The new edge begins at the input vertex and ends at the final vertex
    of the original input edge.
  edge - [in]
    edge to split.
  vertex_location - [in]
    location of inserted vertex.
  Returns:
    A pointer to the new edge or nullptr if the input is not valid.
  */
  const ON_SubDEdge* SplitEdge(
    ON_SubDEdge* edge,
    ON_3dPoint vertex_location
    );

  class ON_SubDFace* AddFace(
    unsigned int edge_count,
    const ON_SubDEdgePtr* edge
    );

  /*
  Description:
    Split a face into two faces by inserting and edge connecting the
    specified vertices.
  Parameters:
    face - [in]
      A face with at least four edges.
    fvi0 - [in]
    fvi1 - [in]
      Indices of the inserted edge ends.
  Returns:
    A pointer to the inserted edge.
    The inserted edge runs from face->Vertex(fvi0) to face->Vertex(fvi1).
    ON_SubDEdge.Face(0) is the original face and ON_SubDEdge::Face(1) is 
    the added face.
    The first edge of the input face remains the first edge of face.  
    The inserted edge is the first edge of the added face.
  */
  const ON_SubDEdge* SplitFace(
    ON_SubDFace* face,
    unsigned int fvi0,
    unsigned int fvi1
    );

  bool SetSubDType(
    ON_SubD::SubDType subdivision_type
    );

  class ON_SubDVertex* AllocateVertex(
    ON_SubD::VertexTag vertex_tag,
    unsigned int level,
    const double* P
    )
  {
    class ON_SubDVertex* v = m_heap.AllocateVertexAndSetId(m_max_vertex_id);
    v->m_level = (unsigned short)level;
    v->m_vertex_tag = vertex_tag;
    if (nullptr != P)
    {
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }
    return v;
  }

  class ON_SubDVertex* AllocateVertex(
    ON_SubD::VertexTag vertex_tag,
    unsigned int level,
    const double* P,
    unsigned int edge_capacity,
    unsigned int face_capacity
    )
  {
    class ON_SubDVertex* v = AllocateVertex(vertex_tag,level,P);

    if (edge_capacity > 0 && edge_capacity < ON_SubDVertex::MaximumEdgeCount )
      m_heap.GrowVertexEdgeArray(v,edge_capacity);
    if (face_capacity > 0 && face_capacity < ON_SubDVertex::MaximumFaceCount )
      m_heap.GrowVertexFaceArray(v,face_capacity);
    return v;
  }

  const class ON_SubDVertex* AddVertexToLevel(class ON_SubDVertex* v)
  {
    class ON_SubDLevel* subd_level = SubDLevel(v->m_level,true);
    return (subd_level) ? subd_level->AddVertex(v) : nullptr;
  }

  void ReturnVertex(class ON_SubDVertex* v)
  {
    if (nullptr != v && v->m_level < m_levels.UnsignedCount())
    {
      ON_SubDLevel* level = m_levels[v->m_level];
      if (level)
        level->RemoveVertex(v);
    }
    v->ClearSavedLimitPoints(); // return extras to pool
    m_heap.ReturnVertex(v);
  }

  class ON_SubDEdge* AllocateEdge(
    ON_SubD::EdgeTag edge_tag
    )
  {
    class ON_SubDEdge* e = m_heap.AllocateEdgeAndSetId(m_max_edge_id);
    e->m_edge_tag = edge_tag;
    return e;
  }

  class ON_SubDEdge* AllocateEdge(
    ON_SubD::EdgeTag edge_tag,
    unsigned int level,
    unsigned int face_capacity
    )
  {
    class ON_SubDEdge* e = AllocateEdge(edge_tag);
    e->m_level = (unsigned short)level;
    if (face_capacity > 0 && face_capacity <= ON_SubDEdge::MaximumFaceCount )
      m_heap.GrowEdgeFaceArray(e,face_capacity);
    return e;
  }

  const class ON_SubDEdge* AddEdgeToLevel(class ON_SubDEdge* e)
  {
    class ON_SubDLevel* subd_level = SubDLevel(e->m_level,true);
    return (subd_level) ? subd_level->AddEdge(e) : nullptr;
  }

  void ReturnEdge(class ON_SubDEdge* e)
  {
    if (nullptr != e && e->m_level < m_levels.UnsignedCount())
    {
      ON_SubDLevel* level = m_levels[e->m_level];
      if (level)
        level->RemoveEdge(e);
    }
    m_heap.ReturnEdge(e);
  }

  class ON_SubDFace* AllocateFace()
  {
    class ON_SubDFace* f = m_heap.AllocateFaceAndSetId(m_max_face_id);
    return f;
  }

  class ON_SubDFace* AllocateFace(
    unsigned int level,
    unsigned int edge_capacity
    )
  {
    class ON_SubDFace* f = AllocateFace();
    if (nullptr != f)
    {
      f->m_level = (unsigned short)level;
      if (edge_capacity > sizeof(f->m_edge4)/sizeof(f->m_edge4[0]) && edge_capacity <= ON_SubDFace::MaximumEdgeCount)
        m_heap.GrowFaceEdgeArray(f,edge_capacity);
    }
    return f;
  }

  const class ON_SubDFace* AddFaceToLevel(class ON_SubDFace* f)
  {
    class ON_SubDLevel* subd_level = SubDLevel(f->m_level,true);
    return (subd_level) ? subd_level->AddFace(f) : nullptr;
  }

  void ReturnFace(class ON_SubDFace* f)
  {
    if (nullptr != f && f->m_level < m_levels.UnsignedCount())
    {
      ON_SubDLevel* level = m_levels[f->m_level];
      if (level)
        level->RemoveFace(f);
    }
    m_heap.ReturnFace(f);
  }

  unsigned int DeleteComponents(
    unsigned int level_index
    );

  /*
  Description:
    Discard all contents of this ON_SubDimple.
  Remarks:
    More efficient than Destroy() if this ON_SubDimple will be reused soon.
  */
  void Clear();

  void ClearSubdivisionLevels(
    unsigned int max_level_index
    );

  size_t SizeOf() const
  {
    size_t sz = sizeof(*this) + m_heap.SizeOf() - sizeof(m_heap);
    return sz;
  }

  bool Subdivide(
    ON_SubD::SubDType subd_type,
    unsigned int level_index,
    unsigned int count
    );

  /*
  Description:
    Apply global subdivision to m_levels[].Last().
  */
  unsigned int GlobalSubdivide(
    ON_SubD::SubDType subdivision_type,
    bool bUseSavedSubdivisionPoints
    );

  unsigned int MergeColinearEdges(
    double distance_tolerance,
    double maximum_aspect,
    double sin_angle_tolerance
    );

  ON_SubDEdgePtr MergeEdges(
    ON_SubDEdgePtr eptr0,
    ON_SubDEdgePtr eptr1
    );

public:
  bool GrowVertexEdgeArray(
    ON_SubDVertex* v,
    size_t capacity
    );
  bool GrowVertexFaceArray(
    ON_SubDVertex* v,
    size_t capacity
    );
  bool GrowEdgeFaceArray(
    ON_SubDEdge* e,
    size_t capacity
    );
  bool GrowFaceEdgeArray(
    ON_SubDFace* f,
    size_t capacity
    );

private:
  ON_SubDHeap m_heap;

  unsigned int m_max_vertex_id = 0;
  unsigned int m_max_edge_id = 0;
  unsigned int m_max_face_id = 0;
  
  ON_SimpleArray< ON_SubDLevel* > m_levels;
  ON_SubDLevel* m_active_level = nullptr; // m_active_level = nullptr or m_active_level = m_levels[m_active_level->m_level_index].

public:
  /*
  Returns:
    Active level
  */
  const ON_SubDLevel& ActiveLevel() const
  {
    return (nullptr != m_active_level) ? *m_active_level : ON_SubDLevel::Empty;
  }
  
  ON_SubDLevel* ActiveLevelPointer()
  {
    return m_active_level;
  }

  bool SetActiveLevel(
    unsigned int level_index
    )
  {
    if (level_index < m_levels.UnsignedCount())
    {
      m_active_level = m_levels[level_index];
      return true;
    }
    return false;
  }
  
  class ON_SubDLevel const * SubDLevel(
    unsigned int level_index
    ) const;

private:
  ON_SubDLevel* ActiveLevel(
    bool bCreateIfNeeded
    );

  class ON_SubDLevel* SubDLevel(
    unsigned int level_index,
    bool bCreateIfNeeded
    );

  /*
  Description:
    Delete all contents release all memory used by this ON_SubDimple.
  */
  void Destroy();

  bool IsValidLevel(
    const ON_SubD& subd,
    unsigned int level_index,
    bool bSilentError,
    ON_TextLog* text_log
    ) const;

  /*
  Returns:
  Number of quads added. When all input is valid the
  returned value is >= 4 and equal to face->m_edge_count.
  */
  unsigned int GlobalQuadSubdivideFace(
    bool bUseSavedSubdivisionPoint,
    const ON_SubDFace* face
    );

  /*
  Parameters:
    face - [in]
    bUseSavedSubdivisionPoint - [in]
    bUnsetEdgeWeight - [out]
      false - new edges have weights set
      true  - the value of one or more ON_SubDEdge::m_vertex_weight[]
      was set to ON_SubDSectorType::UnsetSectorWeight because it cannot
      be calculated until the the rest of the subdivision is complete.
      This happens when sector face counts are required
      and a non-triangluar face is present in the level bing subdivided.
  Returns:
    Number of triangles added.
    If the input is valid and face->m_edge_count = 3, then the
    returned value will be 4.
    If the input is valid and face->m_edge_count > 3, then the
    returned value will be 2*face->m_edge_count.
  */
  unsigned int GlobalTriSubdivideFace(
    const ON_SubDFace* face,
    bool bUseSavedSubdivisionPoint,
    bool* bUnsetEdgeWeight
    );

 private:
   ON_SubDimple& operator=(const ON_SubDimple&) = delete;
};



class ON_SubDQuadFaceSubdivisionCounter
{
public:
  const ON_SubDFace* m_level0_face = nullptr;
  unsigned int m_level0_face_id = 0;

  unsigned short m_subdivision_count = 0;
  unsigned short m_corner_index[9];

  void SetLevel0Face(
    const ON_SubDFace* face
    )
  {
    m_level0_face = face;
    m_level0_face_id = (nullptr == face) ? 0 : face->m_id;
    m_subdivision_count = 0;
  }

  bool BreakpointTest();

  void Push(
    unsigned int face_corner_index
    )
  {
    if ( face_corner_index >= 0xFFFFU )
      face_corner_index = 0xFFFFU;
    if ( m_subdivision_count < sizeof(m_corner_index)/sizeof(m_corner_index[0]) )
      m_corner_index[m_subdivision_count] = (unsigned short)face_corner_index;
    m_subdivision_count++;
    BreakpointTest();
  }

  void Pop()
  {
    if ( m_subdivision_count > 0 )
      m_subdivision_count--;
  }
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDQuadFaceMesher
//

class ON_SubDQuadFacePatcher
{
public:
  ON_SubDLimitPatchFragment m_patch_fragment = ON_SubDLimitPatchFragment::Empty;
  unsigned int m_display_density = 0;
  unsigned int m_patch_count = 0;
  ON__UINT_PTR m_fragment_callback_context = 0;
  bool (*m_fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitPatchFragment*) = nullptr;
  bool m_bCallbackResult = false;
  
  bool SendSinglePatch(
    unsigned int display_density,
    const class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
    ON_SubDLimitPatchFragment::PatchType patch_type
    );
  
  bool SendMultiPatch(
    unsigned int display_density,
    const class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
    const ON_SubDLimitPatchFragment::PatchType patch_type[4]
    );
};

class ON_SubDQuadFaceMesher
{
public:
  ON_SubDQuadFaceMesher() = default;

  ~ON_SubDQuadFaceMesher()
  {
    if (nullptr != m__buffer)
    {
      delete[] m__buffer;
      m__buffer = nullptr;
      m__buffer_capacity = 0;
    }
  }

  enum class Output : unsigned int
  {
    unset = 0,
    mesh = 1,
    patches = 2
  };

  ON_SubDQuadFaceMesher::Output m_output = ON_SubDQuadFaceMesher::Output::unset;

  // The m_display_density parameter determines the density of output.
  unsigned int m_display_density = 0;

private:
  unsigned int m_reserved = 0;
public:

  //////////////////////////////////////////////////////////////////////////////////
  //
  // SubD limit surface mesh output
  //
  // The mesh has m_count x m_count quads, where m_count = 2^m_mesh_density.
  // There are (m_count+1)*(m_count+1) points in the mesh.
  unsigned int m_count = 0;
  unsigned int m_capacity = 0;


  // Point(i,j) = (m_points + i*m_point_stride0 + j*m_point_stride1)[0,1,2]
  size_t m_point_stride0 = 0;
  size_t m_point_stride1 = 0;
  double* m_points = nullptr;

  // Normal(i,j) = (m_normals + i*m_normal_stride0 + j*m_normal_stride1)[0,1,2]
  size_t m_normal_stride0 = 0;
  size_t m_normal_stride1 = 0;
  double* m_normals = nullptr;

  //////////////////////////////////////////////////////////////////////////////////
  //
  // SubD limit surface NURBS patch output
  //
  class ON_SubDQuadFacePatcher* m_patcher = nullptr;

  /*
  Description:
    Allocates memory this ON_SubDQuadFaceMesher manages (m_buffer) and
    uses it for the m_points and m_normals arrays.
  */
  bool SetDestinationToLocalMeshBuffer(
    unsigned int mesh_density
    );

  /*
  Description:
    Set the first coordinate of every point to ON_UNSET_VALUE.
  */
  bool UnsetMeshPoints();

  /*
  Description:
    Uses memory on fragment as the destination and sets the m_points and m_normals
    arrays on this to point to appropriate locations in fragment.m_P and fragment.m_N.
  */
  bool SetDestinationToMeshFragment(
    unsigned int mesh_density,
    class ON_SubDLimitMeshFragment& mesh_fragment
    );

  bool SetDestinationToPatchFragment(
    class ON_SubDQuadFacePatcher& patcher
    );

  bool GetLimitMesh(
    class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
    const ON_SubDFace* face
    );

  bool GetLimitPatches(
    class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
    const ON_SubDFace* face
    );

private:
  double* Internal_Buffer(size_t buffer_capacity);

  size_t m__buffer_capacity = 0;
  double* m__buffer = nullptr;

  /*
  Description:
    Get values of cubic uniform B-spline basis functions times 6.
  Parameters:
    t - [in] 0.0 <= t <= 1.0
      Evaluation parameter
    b - [out]
      t^3,
      (1 + 3t + 3t^2 - 3t^3)
      (1 + 3(1-t) + 3(1-t)^2 - 3(1-t)^3)
      (1-t)^3
  Remarks:
    If C0, C1, C2, C3 are the control points for a span
    of a uniform cubic B-spline and the domain of the
    span is 0 <= t <= 1, then
    C(t) = (b[0]*C0 + b[1]*C1 + b[2]*C2 + b[3]*C3)/6.
  */
  static void Get6xCubicBasis(
    double t,
    double b6[4]
    );

 /*
  Description:
    Get values of derviatives of cubic uniform B-spline basis functions.
  Parameters:
    t - [in] 0.0 <= t <= 1.0
      Evaluation parameter
    b - [out]
      t^3,
      (1 + 3t + 3t^2 - 3t^3)
      (1 + 3(1-t) + 3(1-t)^2 - 3(1-t)^3)
      (1-t)^3
  Remarks:
    If C0, C1, C2, C3 are the control points for a span
    of a uniform cubic B-spline and the domain of the
    span is 0 <= t <= 1, then the first derivative is
    C'(t) = (b[0]*C0 + b[1]*C1 + b[2]*C2 + b[3]*C3).
  */
  static void GetCubicBasisDerivative(
    double t,
    double b[4]
    );
  
  /*
  Description:
    Calcululate a quad mesh from the bi-cubic uniform B-spline surface.
  Parameters:
  Returns:
    true:
      successful
    false:
      failure
  */
  bool EvaluateSurface(
    unsigned int count,
    unsigned int point_i0,
    unsigned int point_j0
    ) const; 

  bool GetLimitSubMeshOrPatch(
    class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
    class ON_SubDQuadNeighborhood* qft, // may be destroyed
    unsigned int display_density,
    unsigned int point_i0,
    unsigned int point_j0
    );

private:
  bool Internal_EvaluateSurfaceNormalBackup1(
    double s,
    double t,
    unsigned int count,
    unsigned int i,
    unsigned int j,
    double* N
    ) const; 
  bool Internal_EvaluateSurfaceNormalBackup2(
    const double* P00,
    unsigned int count,
    unsigned int i,
    unsigned int j,
    double* N
    ) const; 
  // Workspaces used by GetLimitSubMesh()
  double m_srf_cv[4][4][3];
  ON_SubD_FixedSizeHeap m_fshx[5];

  ON_SubD_FixedSizeHeap* CheckOutLocalFixedSizeHeap()
  {
    const size_t count = sizeof(m_fshx) / sizeof(m_fshx[0]);
    for (size_t i = 0; i < count; i++)
    {
      if ( false == m_fshx[i].InUse() )
        return &m_fshx[i];
    }
    return ON_SUBD_RETURN_ERROR(nullptr);
  }

  bool ReturnLocalFixedSizeHeap(ON_SubD_FixedSizeHeap* fsh)
  {
    if ( nullptr == fsh)
      return ON_SUBD_RETURN_ERROR(false);

    const size_t count = sizeof(m_fshx) / sizeof(m_fshx[0]);
    for (size_t i = 0; i < count; i++)
    {
      if (fsh == &m_fshx[i])
      {
        m_fshx[i].Reset();
        return true;
      }
    }

    return false; // not an error
  }

  void ReturnAllFixedSizeHeaps()
  {
    const size_t count = sizeof(m_fshx) / sizeof(m_fshx[0]);
    for (size_t i = 0; i < count; i++)
    {
      m_fshx[i].Reset();
    }
  }

private:
  void SetDestinationInitialize(
    ON_SubDQuadFaceMesher::Output output
    );


private:
  // copies not supported
  ON_SubDQuadFaceMesher(const ON_SubDQuadFaceMesher&) = delete;
  ON_SubDQuadFaceMesher& operator=(const ON_SubDQuadFaceMesher&) = delete;
};


class ON_SubDArchiveIdMap
{
  // used for file IO, copy construction, and operator= where the problem
  // of converting to/from pointers to archive_ids is necessary.
public:
  static unsigned int ArchiveIdFromComponentPtr(
    ON__UINT_PTR ptr
    );

  bool ConvertArchiveIdToRuntimeVertexPtr(
    unsigned int vertex_count,
    size_t vertex_capacity,
    ON_SubDVertex** vertex
    );

  bool ConvertArchiveIdToRuntimeEdgePtr(
    unsigned int edge_count,
    size_t edgeN_capacity,
    ON_SubDEdgePtr* edgeN,
    unsigned int edgeX_capacity,
    ON_SubDEdgePtr* edgeX
    );

  bool ConvertArchiveIdToRuntimeFacePtr(
    unsigned int face_count,
    size_t faceN_capacity,
    ON_SubDFacePtr* faceN,
    unsigned int faceX_capacity,
    ON_SubDFacePtr* faceX
    );

  static void ValidateArrayCounts(
    unsigned short& array_count,
    size_t arrayN_capacity,
    const void* arrayN,
    unsigned short arrayX_capacity,
    const void* arrayX
    );

  static ON_SubDComponentPtr FromVertex(
    ON_SubDVertexPtr vertex_ptr
    );

  static ON_SubDComponentPtr FromEdge(
    ON_SubDEdgePtr edge_ptr
    );

  static ON_SubDComponentPtr FromFace(
    ON_SubDFacePtr face_ptr
    );

  static ON_SubDComponentPtr FromVertex(
    const ON_SubDVertex* vertex
    );

  static ON_SubDComponentPtr FromEdge(
    const ON_SubDEdge* edge
    );

  static ON_SubDComponentPtr FromFace(
    const ON_SubDFace* face
    );

  static ON_SubDVertex* CopyVertex(
    const ON_SubDVertex* src,
    class ON_SubDimple& subdimple
    );

  static ON_SubDEdge* CopyEdge(
    const ON_SubDEdge* src,
    class ON_SubDimple& subdimple
    );

  static ON_SubDFace* CopyFace(
    const ON_SubDFace* src,
    class ON_SubDimple& subdimple
    );

  ON_SubDArchiveIdMap();

  bool Reset();

  unsigned int Count();

  const ON_SubDComponentPtr* First();
  const ON_SubDComponentPtr* Next();

  bool Add(const ON_SubDVertex* vertex);
  bool Add(const ON_SubDEdge* edge);
  bool Add(const ON_SubDFace* face);

  class ON_SubDVertex* AddCopy(const class ON_SubDVertex* source_vertex, class ON_SubDimple& subdimple);
  class ON_SubDEdge* AddCopy(const class ON_SubDEdge* source_edgeclass, class ON_SubDimple& subdimple);
  class ON_SubDFace* AddCopy(const class ON_SubDFace* source_faceclass, class ON_SubDimple& subdimple);

  const ON_SubDComponentPtr* ComponentPtrFromArchiveId(
    unsigned int archive_id
    ) const;

  unsigned int m_archive_id_partition[4];

  unsigned int ConvertArchiveIdsToRuntimePointers();

private:
  bool AddComponentPtr(ON_SubDComponentPtr eptr, unsigned int archive_id);
  
  unsigned int m_element_index = ON_UNSET_UINT_INDEX;
  unsigned int m_element_count = 0;
  ON_FixedSizePool m_fsp;
  ON_FixedSizePoolIterator m_fsp_it;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDLimitMesh
//

class /*DO NOT EXPORT*/ON_SubDLimitMeshImpl
{
#if defined(ON_SUBD_CENSUS)
  ON_SubDLimitMeshImplCensusCounter m_census_counter;
#endif

public:
  ON_SubDLimitMeshImpl();
  ~ON_SubDLimitMeshImpl() = default;
  ON_SubDLimitMeshImpl(const ON_SubDLimitMeshImpl& src);

private:
  // no operator =
  ON_SubDLimitMeshImpl& operator=(const ON_SubDLimitMeshImpl&) = delete;

public:
  ON_SubD::FacetType m_facet_type = ON_SubD::FacetType::Unset;
private:
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;
  unsigned int m_reserved3 = 0;
public:
  unsigned int m_limit_mesh_content_serial_number;
private:
  static unsigned int Internal_NextContentSerialNumber();
public:
  unsigned int m_display_density = 0;
  unsigned int m_fragment_count = 0;
  unsigned int m_fragment_point_count = 0;
  ON_SubDLimitMeshFragment* m_first_fragment = nullptr;
  ON_SubDLimitMeshFragment* m_last_fragment = nullptr;
  
  bool ReserveCapacity(
    unsigned int subd_fragment_count,
    ON_SubD::FacetType facet_type,
    unsigned int display_density
    );
  
  /*
  Description:
    ON_SubDLimitMeshImpl_CallbackContext::FragmentCallbackFunction()
    uses CopyCallbackFragment() to make a copy of callback_fragment 
    delivered by ON_SubD::GetLimitSurfaceMeshInFragments().
  */
  ON_SubDLimitMeshFragment* CopyCallbackFragment(
    const ON_SubDLimitMeshFragment* callback_fragment
    );

  /*
  Description:
    ON_SubDLimitMeshImpl_CallbackContext::FragmentCallbackFunction()
    uses AddFinishedFragment() to add finished fragments to this
    ON_SubDLimitMeshImpl's m_first_fragment ... m_list_fragment list.
  */
  bool AddFinishedFragment(
    ON_SubDLimitMeshFragment* fragment
    );

  const ON_RTree& FragmentTree() const;

  void ClearTree();
  
  bool Transform( 
    const ON_Xform& xform
    );


  bool GetTightBoundingBox(
    ON_BoundingBox& bbox,
    bool bGrowBox,
    const ON_Xform* xform
    ) const;

  ON_BoundingBox m_bbox;

  // The weak pointer to the ON_SubDimple is used to
  // check that the ON_SubDimple managing the
  // ON_SubDLimitMeshFragment.m_face pointers is valid 
  // before those pointers are used.  This must be a weak
  // pointer and not a shared_ptr because limit meshes
  // are stored on ON_SubDLevels that are members of
  // ON_SubDimple.
  std::weak_ptr<class ON_SubDimple> m_subdimple_wp;

  void ClearFragmentFacePointers();

private:
  ON_RTree* m_fragment_tree = nullptr;

private:
  ON_FixedSizePool m_fsp;
};

#endif // ON_COMPILING_OPENNURBS)

#endif // OPENNURBS_SUBD_WIP

#endif // OPENNURBS_SUBD_DATA_INC_

