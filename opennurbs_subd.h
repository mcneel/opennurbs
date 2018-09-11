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


#if 0
#define ON_SUBD_CENSUS
//////////////////////////////////////////////////////////////////////////
//
// ON_CensusCounter
//
//  This tool is used to study memory leaks and other shared ptr issues.
//  The classes have no size but effect performance.  Use only in
//  debugging situations. Never ship a release with ON_SUBD_CENSUS defined.
//
class ON_SUBD_CLASS ON_CensusCounter
{
public:
  enum class Class : unsigned int
  {
    unset = 0,
    subd = 1,
    subd_impl = 2,
    subd_limit_mesh = 3,
    subd_limit_mesh_impl = 4,
    subd_ref = 5,

    count
  };

  static void RegisterBirth(ON_CensusCounter::Class,ON__UINT_PTR);

  static void RegisterDeath(ON_CensusCounter::Class,ON__UINT_PTR);

  static void CensusReport(
    class ON_TextLog&
    );

  static void Clear();
};

class ON_SUBD_CLASS ON_SubDCensusCounter
{
public:
  ON_SubDCensusCounter() ON_NOEXCEPT;
  ~ON_SubDCensusCounter() ON_NOEXCEPT;
  ON_SubDCensusCounter(const ON_SubDCensusCounter&) ON_NOEXCEPT;
  ON_SubDCensusCounter& operator=(const ON_SubDCensusCounter&) = default;
  //ON_SubDCensusCounter( ON_SubDCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDCensusCounter& operator=( ON_SubDCensusCounter&& ) ON_NOEXCEPT;
};

class ON_SUBD_CLASS ON_SubDRefCensusCounter
{
public:
  ON_SubDRefCensusCounter() ON_NOEXCEPT;
  ~ON_SubDRefCensusCounter() ON_NOEXCEPT;
  ON_SubDRefCensusCounter(const ON_SubDRefCensusCounter&) ON_NOEXCEPT;
  ON_SubDRefCensusCounter& operator=(const ON_SubDRefCensusCounter&) = default;
  //ON_SubDRefCensusCounter( ON_SubDRefCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDRefCensusCounter& operator=( ON_SubDRefCensusCounter&& ) ON_NOEXCEPT;
};


class ON_SUBD_CLASS ON_SubDImpleCensusCounter
{
public:
  ON_SubDImpleCensusCounter() ON_NOEXCEPT;
  ~ON_SubDImpleCensusCounter() ON_NOEXCEPT;
  ON_SubDImpleCensusCounter(const ON_SubDImpleCensusCounter&) ON_NOEXCEPT;
  ON_SubDImpleCensusCounter& operator=(const ON_SubDImpleCensusCounter&) = default;
  //ON_SubDImplCensusCounter( ON_SubDImplCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDImplCensusCounter& operator=( ON_SubDImplCensusCounter&& ) ON_NOEXCEPT;
};

class ON_SUBD_CLASS ON_SubDLimitMeshCensusCounter
{
public:
  ON_SubDLimitMeshCensusCounter() ON_NOEXCEPT;
  ~ON_SubDLimitMeshCensusCounter() ON_NOEXCEPT;
  ON_SubDLimitMeshCensusCounter(const ON_SubDLimitMeshCensusCounter&) ON_NOEXCEPT;
  ON_SubDLimitMeshCensusCounter& operator=(const ON_SubDLimitMeshCensusCounter&) = default;
  //ON_SubDLimitMeshCensusCounter( ON_SubDLimitMeshCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDLimitMeshCensusCounter& operator=( ON_SubDLimitMeshCensusCounter&& ) ON_NOEXCEPT;
};


class ON_SUBD_CLASS ON_SubDLimitMeshImplCensusCounter
{
public:
  ON_SubDLimitMeshImplCensusCounter() ON_NOEXCEPT;
  ~ON_SubDLimitMeshImplCensusCounter() ON_NOEXCEPT;
  ON_SubDLimitMeshImplCensusCounter(const ON_SubDLimitMeshImplCensusCounter&) ON_NOEXCEPT;
  ON_SubDLimitMeshImplCensusCounter& operator=(const ON_SubDLimitMeshImplCensusCounter&) ON_NOEXCEPT;
  ON_SubDLimitMeshImplCensusCounter( ON_SubDLimitMeshImplCensusCounter&& ) ON_NOEXCEPT;
  ON_SubDLimitMeshImplCensusCounter& operator=( ON_SubDLimitMeshImplCensusCounter&& ) ON_NOEXCEPT;
};

#endif


////////////////////////////////////////////////////////////////
//
//   Definition of subdivision surface
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_SUBD_INC_)
#define OPENNURBS_SUBD_INC_

#if defined(OPENNURBS_SUBD_WIP)

#if 1
// SuD is exported from opennurbs DLL
#define ON_SUBD_CLASS ON_CLASS
#else
// SuD is not exported from opennurbs DLL
#define ON_SUBD_CLASS
#endif

class ON_SUBD_CLASS ON_SubDVertexPtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDVertexPtr::Null
  // or x = ON_SubDVertexPtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDVertexPtr Null;

  bool IsNull() const;

  class ON_SubDVertex* Vertex() const;

  ON__UINT_PTR VertexPtrMark() const;

  ON_ComponentStatus Status() const;

  static
  class ON_SubDVertexPtr Create(
    const class ON_SubDVertex* vertex
    );

  /*
  Parameters:
    vertex - [in]
    mark - [in]
      zero or one
  */
  static
  class ON_SubDVertexPtr Create(
    const class ON_SubDVertex* vertex,
    ON__UINT_PTR vertex_mark
    );

  static
  class ON_SubDVertexPtr Create(
    const class ON_SubDComponentPtr& vertex_component
    );
};

class ON_SUBD_CLASS ON_SubDEdgePtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDEdgePtr::Null
  // or x = ON_SubDEdgePtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDEdgePtr Null;

  bool IsNull() const;

  class ON_SubDEdge* Edge() const;

  ON__UINT_PTR EdgeDirection() const;

  ON_ComponentStatus Status() const;

  /*
  Returns:
    A pointer to the same edge with the direction flipped
  */
  ON_SubDEdgePtr Reversed() const;

  static ON_SubDEdgePtr Create(
    const class ON_SubDEdge* edge,
    ON__UINT_PTR direction
    );

  static ON_SubDEdgePtr Create(
    const class ON_SubDComponentPtr& edge_component
    );
};

class ON_SUBD_CLASS ON_SubDFacePtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDFacePtr::Null
  // or x = ON_SubDFacePtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDFacePtr Null;

  bool IsNull() const;

  class ON_SubDFace* Face() const;

  ON__UINT_PTR FaceDirection() const;

  ON_ComponentStatus Status() const;

  static
  class ON_SubDFacePtr Create(
    const class ON_SubDFace* face,
    ON__UINT_PTR direction
    );

  static ON_SubDFacePtr Create(
    const class ON_SubDComponentPtr& face_component
    );
};

class ON_SUBD_CLASS ON_SubDComponentPtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDComponentPtr::Null
  // or x = ON_SubDComponentPtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDComponentPtr Null;

  enum class ComponentPtrType : unsigned char
  {
    unset = 0,
    vertex = 1,
    edge = 2,
    face = 3
  };

  static ON_SubDComponentPtr::ComponentPtrType ComponentPtrTypeFromUnsigned(
    unsigned int component_pointer_type_as_unsigned
    );

   /*
   Description:
     ON_SubDComponentPtr::ComponentPtrType::vertex 
     < ON_SubDComponentPtr::ComponentPtrType::edge
     < ON_SubDComponentPtr::ComponentPtrType::face
     < ON_SubDComponentPtr::ComponentPtrType::unset
     < invalid
   */
  static int CompareComponentPtrType(
    ON_SubDComponentPtr::ComponentPtrType a,
    ON_SubDComponentPtr::ComponentPtrType b
    );

  static int CompareType(
    const ON_SubDComponentPtr* a,
    const ON_SubDComponentPtr* b
    );

  static int Compare(
    const ON_SubDComponentPtr* a,
    const ON_SubDComponentPtr* b
    );


  bool IsNull() const;
  bool IsNotNull() const;

  ON_SubDComponentPtr::ComponentPtrType ComponentType() const;

  class ON_SubDComponentBase* ComponentBase() const;
  class ON_SubDVertex* Vertex() const;
  class ON_SubDEdge* Edge() const;
  class ON_SubDFace* Face() const;

  ON_SubDVertexPtr VertexPtr() const;
  ON_SubDEdgePtr EdgePtr() const;
  ON_SubDFacePtr FacePtr() const;

  ON_COMPONENT_INDEX ComponentIndex() const;

  /*
  Returns:
    0 or 1.
    The interpretation of the mark varies depending on the context.
    For vertices, this is the vertex mark.
    For edges, this is generally an index into ON_SubDEdge.m_vertex[]
    or a direction flag with 1 indicating a reversed direction.
    For face, this is generally an orientation flag with 1 indicating
    a reversed (clockwise) orientation.
  */
  ON__UINT_PTR ComponentMark() const;

  ON_ComponentStatus Status() const;

  /*
  Returns:
    1: status changed.
    0: status not changed.
  */
  unsigned int SetStates(
    ON_ComponentStatus states_to_set
    );

  /*
  Returns:
    1: status changed.
    0: status not changed.
  */
  unsigned int ClearStates(
    ON_ComponentStatus states_to_clear
    );

  /*
  Description:
    Makes "this" an exact copy of status.
  Parameters:
    status - [in]
  Returns:
    1: status changed.
    0: status not changed.
  */
  unsigned int SetStatus(
    ON_ComponentStatus status
    );

  static ON_SubDComponentPtr ClearMark(
    ON_SubDComponentPtr component_ptr
    );

  static
  class ON_SubDComponentPtr Create(
    const class ON_SubDVertex* vertex
    );

  static
  class ON_SubDComponentPtr Create(
    const class ON_SubDEdge* edge
    );

  static
  class ON_SubDComponentPtr Create(
    const class ON_SubDFace* face
    );

  static
  class ON_SubDComponentPtr Create(
    const class ON_SubDVertex* vertex,
    ON__UINT_PTR vertex_mark
    );

  static
  class ON_SubDComponentPtr Create(
    const class ON_SubDEdge* edge,
    ON__UINT_PTR edge_mark
    );

  static
  class ON_SubDComponentPtr Create(
    const class ON_SubDFace* face,
    ON__UINT_PTR face_mark
    );
  
  static
  class ON_SubDComponentPtr Create(
    ON_SubDVertexPtr vertexptr
    );
  
  static
  class ON_SubDComponentPtr Create(
    ON_SubDEdgePtr edgeptr
    );

  static
  class ON_SubDComponentPtr Create(
    ON_SubDFacePtr faceptr
    );
};

/*
Description:
  The ON_SubDComponentLocation enum is used when an ON_SubD component 
  is referenced and it is important to distinguish between the
  component's location in the SubD control net and its location
  in the SubD limit surface.
*/
enum class ON_SubDComponentLocation : unsigned char
{
  Unset = 0,
  ControlNet = 1,
  LimitSurface = 2
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubD
//
class ON_SUBD_CLASS ON_SubD : public ON_Geometry
{
  ON_OBJECT_DECLARE(ON_SubD);

#if defined(ON_SUBD_CENSUS)
private:
  ON_SubDCensusCounter m_census_counter;
#endif

public:
  static const ON_SubD Empty;

  enum : unsigned int
  {
    maximum_subd_level = 128        // uses as a sanity check on input parameters
  };

#pragma region RH_C_SHARED_ENUM [ON_SubD::VertexTag] [Rhino.Geometry.SubD.SubDVertexTag] [nested:byte]
  /// <summary>
  /// SubD::VertexTag identifies the type of subdivision vertex.  Different tags use
  /// different subdivision algorithms to determine where the subdivision point and
  /// limit point are located.  There are toplological constraints that restrict which
  /// tags can be assigned.
  /// </summary>
  enum class VertexTag : unsigned char
  {
    ///<summary>
    /// Not a valid vertex tag and the default value for ON_SubDVertex::m_vertex_tag.
    /// This encourages developers to thoughtfully initialize ON_SubDVertex::m_vertex_tag.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Must be an interior vertex.
    /// All edges ending at a smooth vertex must be tagged as ON_SubD::EdgeTag::Smooth. 
    ///</summary>
    Smooth = 1,

    ///<summary>
    /// Can be an iterior or a boundary vertex.
    /// Exactly two edges ending at a crease vertex must be tagged as ON_SubD::EdgeTag::Crease. 
    /// All other edges ending at a crease must be tagged as tagON_SubD::EdgeTag::smooth.
    ///</summary>
    Crease = 2,

    ///<summary>
    /// Can be an iterior, boundary or isolated vertex.
    /// The location of a corner vertex is fixed. 
    /// The all subdivision points and the limit point are at the initial vertex location.
    /// The edges ending at a corner vertex can be smooth or crease edges.
    ///</summary>
    Corner = 3,

    ///<summary>
    /// Must be an interior vertex.  
    /// Exactly one edge ending at a dart vertex must be tagged as ON_SubD::EdgeTag::Smooth. 
    /// All other edges ending at a dart vertex must be tagged as tagON_SubD::EdgeTag::smooth.
    ///</summary>
    Dart = 4
  };
#pragma endregion

  static ON_SubD::VertexTag VertexTagFromUnsigned( 
    unsigned int vertex_tag_as_unsigned
    );

  /*
  Parameters:
    vertex_tag - [in]
  Returns:
    True if vertex_tag is Smooth, Crease, Corner, or Dart.
    False otherwise.
  */
  static bool VertexTagIsSet(
    ON_SubD::VertexTag vertex_tag
  );

#pragma region RH_C_SHARED_ENUM [ON_SubD::EdgeTag] [Rhino.Geometry.SubD.SubDEdgeTag] [nested:byte]
  /// <summary>
  /// SubD::EdgeTag identifies the type of subdivision edge.  Different tags use
  /// different subdivision algorithms to determine where the subdivision point is located.
  /// </summary>  
  enum class EdgeTag : unsigned char
  {
    ///<summary>
    /// Not a valid edge tag and the default value for ON_SubDEdge::m_edge_tag.
    /// This encourages developers to thoughtfully initialize ON_SubDEdge::m_edge_tag.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// One or two of the edge's vertices must be tagged as ON_SubD::VertexTag::Smooth.
    /// The edge must have exactly two faces.
    ///</summary>
    Smooth = 1,

    ///<summary>
    /// Both of the edge's vertices must be tagged as not ON_SubD::VertexTag::Smooth.
    /// The edge can have any number of faces.
    ///</summary>
    Crease = 2,

    ///<summary>
    /// Reserved for version 2 of the ON_SubD project.
    /// Currently this tag is not used and is invalid.
    ///
    /// FUTURE: The edge is a "soft crease" or "semi-sharp".
    /// At lease one end vertex must be tagged as ON_SubD::VertexTag::Smooth
    /// The edge must have exactly two faces.
    /// The value of ON_SubDEdge::m_sharpness controls how
    /// soft/hard the edge appears.
    /// ON_SubDEdge::m_sharpness = 0 is identical to ON_SubD::EdgeTag::Smooth.
    /// ON_SubDEdge::m_sharpness = 1 is identical to ON_SubD::EdgeTag::Crease.
    ///</summary>
    Sharp = 3,
                 
    ///<summary>
    /// This tag appears only on edges that have exactly two neighboring faces
    /// and neither end vertex is tagged as ON_SubD::VertexTag::Smooth.
    /// The level 1 subdivision point for a level 0 edge tagged as ON_SubD::EdgeTag::X 
    /// is the standard smooth edge subdivision point.
    /// When subdivided, the new subdivision vertex will be tagged
    /// as ON_SubD::VertexTag::Smooth and the subdivided edges will
    /// be tagged as ON_SubD::EdgeTag::Smooth.  Thus, the tag ON_SubD::EdgeTag::X
    /// should only appear at level 0.
    /// This tag exists because the ON_SubD subdivision
    /// algorithm requires any edge with both end vertices
    /// tagged as not smooth must be subdivided at its midpoint.
    /// Sector iterators treat "X" edges as smooth.
    /// Both edge end weights must be set so the smooth
    /// subdivided edges will be valid.    
    ///</summary>
    X = 4
  };
#pragma endregion

  static ON_SubD::EdgeTag EdgeTagFromUnsigned( 
    unsigned int edge_tag_as_unsigned
    );


  /*
  Parameters:
    edge_tag - [in]
  Returns:
    True if edge_tag is Smooth, Crease, Sharp, or X.
    False otherwise.
  */
  static bool EdgeTagIsSet(
    ON_SubD::EdgeTag edge_tag
  );

  
#pragma region RH_C_SHARED_ENUM [ON_SubD::FacetType] [Rhino.Geometry.SubD.SubDFacetType] [nested:byte]
  /// <summary>
  /// SubD::FacetType reports the default facet type for subdivision algorithms.
  /// </summary>  
  enum class FacetType : unsigned char
  {
    ///<summary> Not a valid facet type. </summary>
    Unset = 0,

    ///<summary> Triangle </summary>
    Tri = 3,

    ///<summary> Quadrangle </summary>
    Quad = 4
  };
#pragma endregion

  static ON_SubD::FacetType FacetTypeFromUnsigned( 
    unsigned int facet_type_as_unsigned
    );
  
  //enum class VertexEdgeOrder : unsigned char
  //{
  //  unset = 0,
  //  radial, // The ON_SubDVertex edge and face information satisfies:
  //          // 1) m_face_count = m_edge_count or m_face_count+1 == m_edge_count
  //          // 2) m_faces[i] is between m_edges[i] and m_edges[(i+1)%m_edge_count]
  //          // 3) When 0 < i < m_edge_count-1, m_edges[i].m_edge_count = 2
  //          //    and m_edges[i].m_face2[] references m_faces[i-1] and m_faces[i]
  //          //    in an unspecified order.
  //  notradial // one of the conditions conditions for radial is not satisfied.
  //};

  //static ON_SubD::VertexEdgeOrder VertexEdgeOrderFromUnsigned( 
  //  unsigned int vertex_edge_order_as_unsigned
  //  );

#pragma region RH_C_SHARED_ENUM [ON_SubD::VertexFacetType] [Rhino.Geometry.SubD.VertexFacetType] [nested:byte]

  ///<summary>Summarizes the number of edges in faces in the whole object.</summary>
  enum class VertexFacetType : unsigned char
  {
    ///<summary>Not a valid vertex face type.</summary>
    Unset = 0,

    ///<summary>All faces are triangular.</summary>
    Tri = 3,

    ///<summary>All faces are quads.</summary>
    Quad = 4,

    ///<summary>Edge count of faces is constant and &gt; 4.</summary>
    Ngon = 5,

    ///<summary>Edge count of faces is not constant.</summary>
    Mixed = 0xFF 
  };
#pragma endregion

  static ON_SubD::VertexFacetType VertexFacetTypeFromUnsigned( 
    unsigned int vertex_facet_type_as_unsigned
    );

#pragma region RH_C_SHARED_ENUM [ON_SubD::SubDType] [Rhino.Geometry.SubD.SubDType] [nested:byte]
  /// <summary>
  /// Subdivision algorithm.
  /// </summary>  
  enum class SubDType : unsigned char
  {
    ///<summary>
    /// Not a valid subdivision type.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Built-in Loop-Warren triangle with Bernstein-Levin-Zorin creases and darts.
    ///</summary>
    TriLoopWarren = 3,

    ///<summary>
    /// Built-in Catmull-Clark quad with Bernstein-Levin-Zorin creases and darts.
    ///</summary>
    QuadCatmullClark = 4,

    ///<summary>
    /// Custom triangle face algorithm. (Not built-in. Provided for use by 3rd party developers.)
    ///</summary>
    CustomTri = 5,
    
    ///<summary>
    /// Custom quad facet algorithm. (Not built-in. Provided for use by 3rd party developers.)
    ///</summary>
    CustomQuad = 6,

    ///<summary>
    /// Custom algorithm. (Not built-in. Provided for use by 3rd party developers.)
    ///</summary>
    Custom = 7

    // All values must be <= 31; i.e., (((unsigned char)0xE0U) & subd_type)) must be zero.
  };
#pragma endregion

  static ON_SubD::SubDType SubDTypeFromUnsigned( 
    unsigned int subd_type_as_unsigned
    );

  static ON_SubD::SubDType DefaultSubDType();

  static unsigned int FacetEdgeCount(
    ON_SubD::FacetType facet_type
    );

  static unsigned int FacetEdgeCount(
    ON_SubD::SubDType subd_type
    );

  /*
  Parameters:
    sit - [in]
      vertex sector iterator
    component_ring_capacity - [in]
      capacity of component_ring[] array
      1 + center_vertex.m_edge_count + center_vertex.m_face_count 
      will be large enough.
    component_ring - [out]
      A sorted list of ON_SubDComponentPtr values are returned in component_ring[]
      component_ring[0] is the central vertex.
      component_ring[1] and subsequent components with odd indices are sector edges.
      component_ring[2] and subsequent components with even indices are sector faces.
      For edge components (i is odd), component_ring[i].ComponentMark() is the index of
      the center vertex in ON_SubDEge.m_vertex[].
  Returns:
    Number of components set in component_ring[].
    
    0: failure

    >= 4 and even:
      component_ring[0] = center vertex
      component_ring[1] = starting crease edge
      component_ring[2] = starting face
      ... zero or more interior smooth edge, face pairs ...
      component_ring[component_count-1] = ending crease edge

    >= 5 and odd:
      component_ring[0] = vit.CenterVertex()
      component_ring[1] = first edge (smooth)
      component_ring[2] = first face
      ... zero or more smooth edge, face, pairs ...
      component_ring[component_count-2] = last edge (smooth)
      component_ring[component_count-1] = last face

  Example:
    unsinged int component_ring_count = GetVertexComponentRing(vit,component_ring);
    unsinged int N = component_ring_count/2; // number of edges in ring
    const bool bSectorHasCreaseBoundary = (0 == (component_ring_count % 2));
  */
  static unsigned int GetSectorComponentRing(
    const class ON_SubDSectorIterator& sit,
    size_t component_ring_capacity,
    ON_SubDComponentPtr* component_ring
    );

 /*
  Parameters:
    sit - [in]
      vertex sector iterator
    component_ring - [out]
      A sorted listof ON_SubDComponentPtr values are returned in component_ring[]



  Returns:
    Number of components set in component_ring[].
    
    0: failure

    >= 4 and even:
      component_ring[0] = vit.CenterVertex()
      component_ring[1] = starting crease edge
      component_ring[2] = starting face
      ... zero or more interior smooth edge, face pairs ...
      component_ring[component_count-1] = ending crease edge

    >= 5 and odd:
      component_ring[0] = center vertex
      component_ring[1] = first edge (smooth)
      component_ring[2] = first face
      ... zero or more smooth edge, face, pairs ...
      component_ring[component_count-2] = last edge (smooth)
      component_ring[component_count-1] = last face

  Example:
    unsinged int component_ring_count = GetVertexComponentRing(vit,component_ring);
    unsinged int N = component_ring_count/2; // number of edges in ring
    const bool bSectorHasCreaseBoundary = (0 == (component_ring_count % 2));
  */
  static unsigned int GetSectorComponentRing(
    const class ON_SubDSectorIterator&  sit,
    ON_SimpleArray<ON_SubDComponentPtr>& component_ring
    );

  /*
  Returns:
    Number of edges in an component ring returned by ON_SubD::GetVertexComponentRing();
  */
  static unsigned int ComponentRingEdgeCount(
    size_t component_ring_count
    );

  /*
  Returns:
    Number of faces in an component ring returned by ON_SubD::GetVertexComponentRing();
  */
  static unsigned int ComponentRingFaceCount(
    size_t component_ring_count
    );

  static bool ComponentRingIsValid(
    size_t component_ring_count,
    const ON_SubDComponentPtr* component_ring
    );

  /*
  Returns:
    Number of points in the subdivision ring or 0 if the call fails.
  */
  static unsigned int GetSectorSubdivsionPointRing(
    ON_SubD::SubDType subd_type,
    size_t component_ring_count,
    const ON_SubDComponentPtr* component_ring,
    size_t point_ring_capacity,
    size_t point_ring_stride,
    double* point_ring
    );

  static unsigned int GetSectorSubdivisionPointRing(
    ON_SubD::SubDType subd_type,
    size_t component_ring_count,
    const ON_SubDComponentPtr* component_ring,
    ON_SimpleArray<ON_3dPoint>& subd_point_ring
    );

  static unsigned int GetSectorPointRing(
    ON_SubD::SubDType subd_type,
    bool bSubdivideIfNeeded,
    size_t component_ring_count,
    const ON_SubDComponentPtr* component_ring,
    size_t subd_point_ring_capacity,
    size_t subd_point_ring_stride,
    double* subd_point_ring
    );

  static unsigned int GetSectorPointRing(
    ON_SubD::SubDType subd_type,
    bool bSubdivideIfNeeded,
    size_t component_ring_count,
    const ON_SubDComponentPtr* component_ring,
    ON_SimpleArray<ON_3dPoint>& point_ring
    );

  static unsigned int GetSectorPointRing(
    ON_SubD::SubDType subd_type,
    bool bSubdivideIfNeeded,
    const class ON_SubDSectorIterator& sit,
    size_t point_ring_capacity,
    size_t point_ring_stride,
    double* point_ring
    );

  static unsigned int GetSectorPointRing(
    ON_SubD::SubDType subd_type,
    bool bSubdivideIfNeeded,
    const class ON_SubDSectorIterator& sit,
    ON_SimpleArray<ON_3dPoint>& point_ring
    );

  /*
  Parameters:
    subd_type - [in]
      A quad based subdivision algorithm.
    bFirstPass - [in]
      If bFirstPass is true and the components are in standard form for the vertex
      and subdivision type, then locations of the component vertices opposite the 
      center vertex are returned in the point ring.
    bSecondPass - [in]
      If bSecondtPass is true and the first pass is disable or does not succeed,
      then the component subdivision locations are returned in the point ring.
    vertex0 - [in]
      If not null, then vertex0->m_edges and vertex0->m_faces must
      be radially sorted and span a single sector and component_ring[]
      is ignored.
    component_ring_count - [in]
      If vertex0 is null, then component_ring_count specifies the number
      of components in the component_ring[] array.
    component_ring[] - [in]
      If vertex0 is null, then component_ring[0] is the central vertex,
      component_ring[1] and subsequent components with odd indices are
      sector edges, component_ring[2] and subsequent components with even
      indices are sector faces, all sorted radially.
    point_ring_stride - [in]
    point_ring - [out]
      point locations are returned here.
  Returns:
    Number of points in the subdivision ring or 0 if the call fails.
    The number of  points is 
    1 + ON_SubD::ComponentRingEdgeCount(component_ring_count) + ON_SubD::ComponentRingFaceCount(component_ring_count).
  Remarks:
    No validation checking is performed.  This function will crash
    if the input is not valid.  Call GetSubdivisionPointRing() if
    you want a crash proof call.
  */
  static unsigned int GetQuadSectorPointRing(
    ON_SubD::SubDType subd_type,
    bool bFirstPass,
    bool bSecondPass,
    const class ON_SubDVertex* vertex0,
    size_t component_ring_count,
    const class ON_SubDComponentPtr* component_ring,
    size_t point_ring_stride,
    double* point_ring
    );

  /*
  Parameters:
    subd_type - [in]
      A tri based subdivision algorithm.
    bFirstPass - [in]
      If bFirstPass is true and the components are in standard form for the vertex
      and subdivision type, then locations of the component vertices opposite the 
      center vertex are returned in the point ring.
    bSecondPass - [in]
      If bSecondPass is true and the first pass is disable or does not succeed,
      then the component subdivision locations are returned in the point ring.
    vertex0 - [in]
      If not null, then vertex0->m_edges and vertex0->m_faces must
      be radially sorted and span a single sector and component_ring[]
      is ignored.
    component_ring_count - [in]
      If vertex0 is null, then component_ring_count specifies the number
      of components in the component_ring[] array.
    component_ring[] - [in]
      If vertex0 is null, then component_ring[0] is the central vertex,
      component_ring[1] and subsequent components with odd indices are
      sector edges, component_ring[2] and subsequent components with even
      indices are sector faces, all sorted radially.
    point_ring_stride - [in]
    point_ring - [out]
      point locations are returned here.
  Returns:
    Number of points in the subdivision ring or 0 if the call fails.
    The number of  points is 1 + ON_SubD::ComponentRingEdgeCount(component_ring_count).
  Remarks:
    No validation checking is performed.  This function will crash
    if the input is not valid.  Call GetSubdivisionPointRing() if
    you want a crash proof call.
  */
  static unsigned int GetTriSectorPointRing(
    ON_SubD::SubDType subd_type,
    bool bFirstPass,
    bool bSecondPass,
    const class ON_SubDVertex* vertex0,
    size_t component_ring_count,
    const class ON_SubDComponentPtr* component_ring,
    size_t point_ring_stride,
    double* point_ring
    );

  static const class ON_SubDVertex* SubdivideSector(
    ON_SubD::SubDType subd_type,
    const class ON_SubDVertex* center_vertex,
    size_t component_ring_count,
    const class ON_SubDComponentPtr* component_ring,
    class ON_SubD_FixedSizeHeap& fsh
    );

  /*
  Returns:
    true if sector_edge_count is valid for the vertex type
  */
  static bool IsValidSectorEdgeCount(
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_edge_count
    );  
  
  static bool IsValidSectorFaceCount(
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_face_count
    );

  /*
  Returns:
    Type of facets the basic subdivision algorithm requires.  
    ON_SubD::FacetType::Quad if subd_type is ON_SubD::SubDType::TriLoopWarren.
    ON_SubD::FacetType::Tri if subd_type is ON_SubD::SubDType::QuadCatmullClark.
    ON_SubD::FacetType::Unset otherwise.
  Remark:
    All built in subdivision algorithm will handle faces with 3 or more edges.
  */  
  static ON_SubD::FacetType FacetTypeFromSubDType(
    ON_SubD::SubDType subd_type
    );

  static ON_SubD::SubDType SubDTypeFromFacetType(
    ON_SubD::FacetType facet_type
    );

  static bool PointRingHasFacePoints(
    ON_SubD::SubDType subd_type
    );

  /*
  Returns:
    true if facet_type is ON_SubD::FacetType::Tri or ON_SubD::FacetType::Quad.
  */  
  static bool IsQuadOrTriFacetType(
    ON_SubD::FacetType facet_type
    );

  /*
  Returns:
    true if subd_type is ON_SubD::SubDType::TriLoopWarren or ON_SubD::SubDType::QuadCatmullClark.
  */  
  static bool IsQuadOrTriSubDType(
    ON_SubD::SubDType subd_type
    );

  ON_SubD() ON_NOEXCEPT;
  virtual ~ON_SubD();

  /*
  Description:
    Creates an independent copy of src.
  */
  ON_SubD( const ON_SubD& src );

  /*
  Description:
    Creates an independent copy of src.
  */
  ON_SubD& operator=(const ON_SubD& src);

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubD( ON_SubD&& ) ON_NOEXCEPT;

  // The rvalue assignment operator calls ON_Object::operator=(ON_Object&&)
  // which could throw exceptions.  See the implementation of
  // ON_Object::operator=(ON_Object&&) for details.
  ON_SubD& operator=( ON_SubD&& );
#endif

  /*
  Description:
    The subdivision information referenced by src_subd will be shared with this
  Remarks:
    ON_Geometry base class information, like ON_UserData, is not copied or shared.
  */
  void ShareContentsFrom(
    ON_SubD& subd
    );

  static void SwapContents(
    ON_SubD& a,
    ON_SubD& b
    );

  //virtual 
  void MemoryRelocate() override;

  //virtual
  bool IsValid( class ON_TextLog* text_log = nullptr ) const override;

  //virtual
  void Dump(
    ON_TextLog&
    ) const override;

  //virtual
  unsigned int SizeOf() const override;

  //virtual
  ON__UINT32 DataCRC(
    ON__UINT32 current_remainder
    ) const override;

  //virtual
  bool Write(
         ON_BinaryArchive& archive
         ) const override;

  //virtual
  bool Read(
         ON_BinaryArchive& archive
         ) override;

  //virtual
  ON::object_type ObjectType() const override;


  //virtual
  unsigned int ClearComponentStates(
    ON_ComponentStatus states_to_clear
    ) const override;

  //virtual
  unsigned int GetComponentsWithSetStates(
    ON_ComponentStatus states_filter,
    bool bAllEqualStates,
    ON_SimpleArray< ON_COMPONENT_INDEX >& components
    ) const override;
  
  //virtual
  unsigned int SetComponentStates(
    ON_COMPONENT_INDEX component_index,
    ON_ComponentStatus states_to_set
    ) const override;

  //virtual
  unsigned int ClearComponentStates(
    ON_COMPONENT_INDEX component_index,
    ON_ComponentStatus states_to_clear
    ) const override;

  //virtual
  unsigned int SetComponentStatus(
    ON_COMPONENT_INDEX component_index,
    ON_ComponentStatus status_to_copy
    ) const override;

  //virtual
  ON_AggregateComponentStatus AggregateComponentStatus() const override;

  //virtual
  void MarkAggregateComponentStatusAsNotCurrent() const override;

  //virtual
  bool DeleteComponents(
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count
    ) override;

  /*
  Remarks:
    For ON_SubD objects, ClearBoundingBox() and DestroyRuntimeCache()
    are identical.
  */
  //virtual
  void DestroyRuntimeCache(
    bool bDelete = true
    ) override;

  //virtual 
  int Dimension() const override;

  // virtual ON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  // virtual ON_Geometry GetTightBoundingBox override		
  bool GetTightBoundingBox( class ON_BoundingBox& tight_bbox, bool bGrowBox = false, const class ON_Xform* xform = nullptr ) const override;

  /*
  Description:
    Clears all saved information that depends on vertex locations,
    subdivision algorithms, vertex or edge tags, or control net topology.  
    If you modify any of the above, then call ClearBoundingBox().
  Remarks:
    For ON_SubD objects, ClearBoundingBox() and DestroyRuntimeCache()
    are identical.
  */
  //virtual
  void ClearBoundingBox() override;

  //virtual
  bool Transform( 
         const ON_Xform& xform
         ) override;

  //virtual
  bool IsDeformable() const override;

  //virtual
  bool MakeDeformable() override;

  //virtual
  bool SwapCoordinates(
        int i,
        int j
        ) override;
  


  //virtual
  bool HasBrepForm() const override;

  //virtual
  ON_Brep* BrepForm(
    ON_Brep* brep = nullptr
  ) const override;

  //virtual
  ON_COMPONENT_INDEX ComponentIndex() const override;

  //virtual
  bool EvaluatePoint(
    const class ON_ObjRef& objref, 
    ON_3dPoint& P
    ) const override;

  /*
  Description:
    Uses the input mesh to define the level zero control polygon.
  Parameters:
    level_zero_mesh - [in]
    from_mesh_parameters - [in]
      To get the smoothest possible result, pass nullptr 
      or ON_SubDFromMeshOptions::Smooth. To get a sub-D with interior 
      creases use other static ON_SubDFromMeshOptions values or
      create one with custom settings.
  */
  static ON_SubD* CreateFromMesh( 
    const class ON_Mesh* level_zero_mesh,
    const class ON_SubDFromMeshOptions* from_mesh_parameters,
    ON_SubD* subd
    );

  unsigned int DumpTopology(
    ON_TextLog&
    ) const;

  unsigned int DumpTopology(
    ON_2udex vertex_id_range,
    ON_2udex edge_id_range,
    ON_2udex face_id_range,
    ON_TextLog&
    ) const;

  /*
  Description:
    Discard all contents of this ON_SubD.
  Remarks:
    More efficient than Destroy() if this ON_SubD will be reused soon.
  */
  void Clear();

  /*
  Description:
    Delete all contents release all memory used by this ON_SubD.
  */
  void Destroy();

  ON_SubD::SubDType ActiveLevelSubDType() const;

  /*
  Returns:
    The number of explicitly computed levels that are currently available.
    A value of 0 indicates this SubD is empty.
  */
  unsigned int LevelCount() const;

  /*
  Returns:
    If the SubD is not empty, then the index of the active level is returned. This value will be < LevelCount().
    If the SubD is empty, then ON_UNSET_UINT_INDEX is returned.
  */
  unsigned int ActiveLevelIndex() const;

  /*
  Description:
    Remove subdivision levels
  Paramters:
    max_level_count - [in] 
      Maximum number of levels to keep.
  */
  void ClearSubdivisionLevels(
    unsigned int max_level_index
    );

  bool IsEmpty() const;


  /*
  Description:
    Get aggregate edge demographics for the subd.
  Returns:
    Bitwise or of ON_ComponentAttributes::EdgeFlags values for every edge in the subd.
  */
  unsigned int EdgeFlags() const;
  
  /////////////////////////////////////////////////////////
  //
  // Component (Vertex, Edge, Face) access
  //
  ON_SubDComponentPtr ComponentPtrFromComponentIndex(
    ON_COMPONENT_INDEX component_index
    ) const;

  unsigned int ComponentPtrFromComponentIndex(
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    ON_SimpleArray<ON_SubDComponentPtr>& cptr_list
  ) const;

  unsigned int ComponentPtrFromComponentIndex(
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
    ON_SimpleArray<ON_SubDComponentPtr>& cptr_list
  ) const;

  /////////////////////////////////////////////////////////
  //
  // Vertex access
  //

  unsigned int VertexCount() const;

  const class ON_SubDVertex* FirstVertex() const;

  class ON_SubDVertexIterator VertexIterator() const;

  class ON_SubDVertexArray VertexArray() const;

  /*
  Parameters:
    vertex_id - [in]
  Returns:
    If vertex_id identifies a valid vertex in this ON_SubD, then
    a pointer to that vertex is returned.
    Otherwise, nullptr is returned.
  */
  const class ON_SubDVertex* VertexFromId(
    unsigned int vertex_id
    ) const;

  /////////////////////////////////////////////////////////
  //
  // Edge access
  //

  unsigned int EdgeCount() const;

  const class ON_SubDEdge* FirstEdge() const;

  class ON_SubDEdgeIterator EdgeIterator() const;

  class ON_SubDEdgeArray EdgeArray() const;

  /*
  Parameters:
    edge_id - [in]
  Returns:
    If edge_id identifies a valid edge in this ON_SubD, then
    a pointer to that edge is returned.
    Otherwise, nullptr is returned.
  */
  const class ON_SubDEdge* EdgeFromId(
    unsigned int edge_id
    ) const;

  /////////////////////////////////////////////////////////
  //
  // Face access
  //

  unsigned int FaceCount() const;

  const class ON_SubDFace* FirstFace() const;

  class ON_SubDFaceIterator FaceIterator() const;

  class ON_SubDFaceArray FaceArray() const;

  /*
  Parameters:
    face_id - [in]
  Returns:
    If face_id identifies a valid face in this ON_SubD, then
    a pointer to that face is returned.
    Otherwise, nullptr is returned.
  */
  const class ON_SubDFace* FaceFromId(
    unsigned int face_id
    ) const;

  /////////////////////////////////////////////////////////
  //
  // Component (vertex, edge, face) state ( selected, highlighted, ... ) tools
  //   NOTE: 
  //    All component status settings are mutable
  //    All are copied. 
  //    None are saved.
  //

  /*
  Parameters:
    states_filter - [in]
    bAllEqualStates - [in]
      If a state is set in states_filter, all active level components
      with the same state set will be included in the 
      components_with_set_states[] array.  
      If bAllEqualStates is true, then ON_ComponentStatus::AllEqualStates()
      is used to test for inclusion.  
      If bAllEqualStates is false, then ON_ComponentStatus::SomeEqualStates()
      is used to test for inclusion.
    components_with_set_states - [out]
  Returns:
    Number of returned components.
  */
  unsigned int GetComponentsWithSetStates(
    ON_ComponentStatus states_filter,
    bool bAllEqualStates,
    ON_SimpleArray< ON_SubDComponentPtr >& components_with_set_states
    ) const;

  
  /*
  Description:
    Set states on an individual component.
  Parameters:
    component_ptr - [in]
      The states will be set on this component.
    states_to_set - [in]
      If a state is set in the states_to_set parameter, the same
      state will be set on the component.      
  Returns:
    0: no state settings changed on the component.
    1: some state setting changed on the component.
  */
  unsigned int SetComponentStates(
    ON_SubDComponentPtr component_ptr,
    ON_ComponentStatus states_to_set
    ) const;

  /*
  Description:
    Clear states on an individual component.
  Parameters:
    component_ptr - [in]
      The states will be cleared on this component.
    states_to_clear - [in]
      If a state is set in the states_to_clear parameter, the same
      state will be cleared on the component.      
  Returns:
    0: no state settings changed on the component.
    1: some state setting changed on the component.
  */
  unsigned int ClearComponentStates(
    ON_SubDComponentPtr component_ptr,
    ON_ComponentStatus states_to_clear
    ) const;
  
  /*
  Description:
    Copy status settings to an individual component.
  Parameters:
    component_ptr - [in]
      The states will be copied to this component.
    status_to_copy - [in]
  Returns:
    1: some state settings changed on the component.
    1: some state setting changed on the component.
  */
  unsigned int SetComponentStatus(
    ON_SubDComponentPtr component_ptr,
    ON_ComponentStatus status_to_copy
    ) const;

  bool DeleteComponents(
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count
    );


  /////////////////////////////////////////////////////////
  //
  // Editing tools
  //

  unsigned int MergeColinearEdges(
    double distance_tolerance,
    double maximum_aspect,
    double sin_angle_tolerance
    );

  ON_SubDEdgePtr MergeEdges(
    ON_SubDEdgePtr eptr0,
    ON_SubDEdgePtr eptr1
    );

  static bool EdgesCanBeMerged(
    ON_SubDEdgePtr eptr0,
    ON_SubDEdgePtr eptr1
    );

  // returns true if all facets are consistently oriented
  bool IsOriented(
    unsigned int level_index
    ) const;

  // reverses the orientation of all facets
  bool ReverseOrientation(
    unsigned int level_index
    ) const;

  // Attempts to orient all facet to match the first facet.
  bool Orient(
    unsigned int level_index
    ) const;

  /*
  Description:
    Interior vertices (smooth and dart) must have at least three faces.
    Concave corner vertices must have at least two faces.
  */
  bool RepairInvalidSectors(
    unsigned int level_index
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
    If vertex_location == ON_ON_3dPoint::UnsetPoint,
    then the edge's midpoint is used.
  Returns:
    A pointer to the new edge or nullptr if the input is not valid.
  */
  const class ON_SubDEdge* SplitEdge(
    class ON_SubDEdge* edge,
    ON_3dPoint vertex_location
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
  const class ON_SubDEdge* SplitFace(
    class ON_SubDFace* face,
    unsigned int fvi0,
    unsigned int fvi1
    );

  const class ON_SubDVertex* TriangulateFace(
    class ON_SubDFace* face
    );

  const class ON_SubDFace* MergeFaces(
    class ON_SubDEdge* edge
    );

  /*
  Description:
    Updates vertex tag, edge tag, and edge coefficient values
    on the active level.

    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.

  Parameters:
    bUnsetValuesOnly - [in]
      If true, the update is restricted to vertices tagged as 
      ON_SubD::VertexTag::Unset and edges tagged as ON_SubD::EdgeTag::Unset.

  Returns:
    Number of vertices and edges that were changed during the update.
  */
  unsigned int UpdateAllTagsAndSectorCoefficients(
    bool bUnsetValuesOnly
  );

  /*
  Description:
    This tool if for expert users writing advanced editing tools.
    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.
  Parameters:
    bUnsetTagsOnly - [in]
      If bUnsetTagsOnly is true, then only unset tags and
      ill be updated.
      If bUnsetTagsOnly is false, then all tags and
      will be checked and updated as needed.
  Returns:
    Number of vertices that changed during the update.
  Remarks:
    It is easiest to call UpdateTagsAndSectorCoefficients().
  */
  unsigned int UpdateVertexTags(
    bool bUnsetVertexTagsOnly
    );

  /*
  Description:
    This tool if for expert users writing advanced editing tools.
    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.
  Parameters:
    bUnsetValuesOnly - [in]
      If bUnsetValuesOnly is true, then only unset tags and
      sector weights will be updated.
      If bUnsetValuesOnly is false, then all tags and
      sector weights will be checked and updated as needed.
  Returns:
    Number of edges that had a tag value changed or sector
    coefficient set to ON_SubDSectorType::UnsetSectorWeight.
  Remarks:
    It is easiest to call UpdateTagsAndSectorCoefficients().
  */
  unsigned int UpdateEdgeTags(
    bool bUnsetEdgeTagsOnly
    );

  /*
  Description:
    This tool if for expert users writing advanced editing tools.
    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.
  Parameters:
    bUnsetValuesOnly - [in]
      If bUnsetValuesOnly is true, then only unset tags and
      sector weights will be updated.
      If bUnsetValuesOnly is false, then all tags and
      sector weights will be checked and updated as needed.
  Returns:
    Number of edges that had a tag value changed or sector
    coefficient set to ON_SubDSectorType::UnsetSectorWeight.
  Remarks:
    It is easiest to call UpdateTagsAndSectorCoefficients().
  */
  unsigned int UpdateEdgeSectorCoefficients(
    bool bUnsetSectorCoefficientsOnly
    );

  /*
  Descripiton:
    Clears the ON_ComponentState
  */
  unsigned int ClearComponentMarks(
    bool bClearVertexMarks,
    bool bClearEdgeMarks,
    bool bClearFaceMarks,
    ON_SimpleArray< const class ON_SubDComponentBase* >* marked_component_list
  ) const;

  unsigned int SetComponentMarks(
    bool bClearBeforeSet,
    const ON_SimpleArray< const class ON_SubDComponentBase* >& marked_component_list
  ) const;

  unsigned int GetMarkedComponents(
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
    ON_SimpleArray< const class ON_SubDComponentBase* >& marked_component_list
  ) const;


  /*
  Description:
    Transforms the SubD components in ci_list[].
  Parameters:
    xform - [in]
    ci_list - [in]
    ci_count - [in]
  Returns:
    Number of vertex locations that changed.
  */
  unsigned int TransformComponents(
    const ON_Xform& xform,
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count
    );

  unsigned int TransformComponents(
    const ON_Xform& xform,
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count
    );

  unsigned int ExtrudeComponents(
    const ON_Xform& xform,
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    bool bPermitNonManifoldEdgeCreation,
    ON_SubD::EdgeTag original_edge_tag,
    ON_SubD::EdgeTag moved_edge_tag
    );

  unsigned int ExtrudeComponents(
    const ON_Xform& xform,
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bPermitNonManifoldEdgeCreation,
    ON_SubD::EdgeTag original_edge_tag,
    ON_SubD::EdgeTag moved_edge_tag
    );

  /////*
  ////Description:
  ////  Apply the built-in triangle subdivision subdivision algorithm globally.
  ////Returns:
  ////  New level.
  ////*/
  ////unsigned int TriSubdivision();

  ////unsigned int GetSector(
  ////  const class ON_SubDFace* face,
  ////  ON__UINT_PTR face_vertex_index,
  ////  class ON_SubDVertex& sector
  ////  ) const;

  ////unsigned int GetSector(
  ////  const class ON_SubDVertex* vertex,
  ////  const ON_SubDFace* face,
  ////  class ON_SubDVertex& sector
  ////  ) const;

  ////unsigned int GetSector(
  ////  const ON_SubDVertex* vertex,
  ////  ON_SubDFacePtr face_ptr,
  ////  class ON_SubDVertex& sector
  ////  ) const;

  ////unsigned int GetSector(
  ////  const class ON_SubDVertex* vertex,
  ////  const class ON_SubDEdge* smooth_edge,
  ////  ON_SubDVertex& sector
  ////  ) const;

  ////unsigned int GetSector(
  ////  const ON_SubDEdge* smooth_edge,
  ////  ON__UINT_PTR smooth_edge_end_index,
  ////  ON_SubDVertex& sector
  ////  ) const;

  ////unsigned int GetSector(
  ////  ON_SubDEdgePtr smooth_edge_ptr,
  ////  class ON_SubDVertex& sector
  ////  ) const;

  /*
  Description:
    Apply the built-in subdivision algorithm and save the results
    in this ON_SubD.
  Parameters:
    subd_type - [in]
      unset will use the current subdivision type.
    level_index - [in]
      Level where subdivision starts
    count - [in] > 0
      Number of times to subdivide.
  Returns:
    Number of subdivision steps that succeeded. 
    (= count when everything works, < count when input is not valid)
  */
  bool Subdivide(
    ON_SubD::SubDType subd_type,
    unsigned int level_index,
    unsigned int count
    );

  /*
  Returns:
    Active level subdivison type.
  */
  bool SetSubDType(
    ON_SubD::SubDType subd_type
    );

  class ON_SubDVertex* AddVertex(
    ON_SubD::VertexTag vertex_tag,
    const double* P
    );

  /*
  Parameters:
    edge_face_count - [in]
      Number of faces the edge will eventually have.
      Pass 0 if the value is not known.
    v0 - [in]
      starting vertex
    v1 - [in]
      ending vertex
  Returns:
    If edge_face_count > 0x7FFFU, then ON_SubD::EdgeTag::Unset is returned.

    If edge_face_count is 1 or >= 3, then ON_SubD::EdgeTag::Crease is returned.

    If both vertex tags are ON_SubD::VertexTag::Smooth, then ON_SubD::EdgeTag::Smooth is returned.
   
    If edge_face_count is 1 and both vertex tags are ON_SubD::VertexTag::Crease or ON_SubD::VertexTag::Corner,
    then ON_SubD::EdgeTag::Crease is returned.
   
    If edge_face_count is 2 and both vertex tags are set and both are not ON_SubD::VertexTag::Smooth,
   then ON_SubD::EdgeTag::X is returned.

   Otherwise, ON_SubD::EdgeTag::Unset is returned.
  */
  static ON_SubD::EdgeTag EdgeTagFromContext(
    unsigned int edge_face_count,
    const ON_SubD::VertexTag v0_tag,
    const ON_SubD::VertexTag v1_tag
  );

  static ON_SubD::EdgeTag EdgeTagFromContext(
    unsigned int edge_face_count,
    const ON_SubDVertex* v0,
    const ON_SubDVertex* v1
  );

  /*
  Description:
    Add an edge to the subd.
  Parameters:
    edge_tag - [in]
      ON_SubD::EdgeTag::Unset
        Edge tag is not known at this time.
      ON_SubD::EdgeTag::Smooth
        Smooth edge. If both vertices are tagged as not smooth, the
        tag on the returned edge will be ON_SubD::EdgeTag::X.  This
        tag is changed to ON_SubD::EdgeTag::Smooth on the first
        subdivision step.
      ON_SubD::EdgeTag::Crease.
        Crease edge.  Both vertices must be tagged as not smooth.
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
  Returns:
    Pointer to the allocated edge.
  Remarks:
    ON_SubD::EdgeTagFromContext() can be used to determine edge
    tag values in simple situations.
  */
  class ON_SubDEdge* AddEdge(
    ON_SubD::EdgeTag edge_tag,
    class ON_SubDVertex* v0,
    class ON_SubDVertex* v1
    );

  /*
  Description:
    Expert use tool to add an edge with precomputed sector coefficients.
  Parameters:
    edge_tag - [in]
      This expert user function does not automatically set the edge tag.
    v0 - [in]
    v0_sector_coefficient - [in]
    v1 - [in]
    v1_sector_coefficient - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
      The edges sector weights are set
  */
  class ON_SubDEdge* AddEdgeWithSectorCoefficients(
    ON_SubD::EdgeTag edge_tag,
    class ON_SubDVertex* v0,
    double v0_sector_coefficient,
    class ON_SubDVertex* v1,
    double v1_sector_coefficient
    );
 
  class ON_SubDFace* AddFace(
    unsigned int edge_count,
    const class ON_SubDEdgePtr* edge
    );

  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    edge - [in]
    edge_direction -[in]
    i - [in]
      index where the edge should be inserted.
  Returns:
    true if successful.
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is added even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool AddFaceEdgeConnection(
    ON_SubDFace* face,
    unsigned int i,
    ON_SubDEdge* edge,
    ON__UINT_PTR edge_direction
    );

  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    eptr - [in]
      direction must be set correctly
    i - [in]
      index where the edge should be inserted.
  Returns:
    true if successful.
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is added even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool AddFaceEdgeConnection(
    ON_SubDFace* face,
    unsigned int i,
    ON_SubDEdgePtr eptr
    );

  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    edge - [in]
      edge to remove
  Returns:
    true if successful.
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceEdgeConnection(
    ON_SubDFace* face,
    ON_SubDEdge* edge
    );

  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    i - [in]
      index where the edge should be removed.
    removed_edge - [out]
      removed edge
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceEdgeConnection(
    ON_SubDFace* face,
    unsigned int i
    );

  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    i - [in]
      index where the edge should be removed.
    removed_edge - [out]
      removed edge
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceEdgeConnection(
    ON_SubDFace* face,
    unsigned int i,
    ON_SubDEdgePtr& removed_edge
    );

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

  /*
  Description:
    Get the limit surface mesh for this subD.
  Parameters:
    minimum_display_density - [in]
  Returns:
    A mesh of the subdivision limit surface.
  Remarks:
    The mesh is a reference counted mesh managed by this ON_SubD.
  */
  class ON_SubDLimitMesh LimitSurfaceMesh() const;

  ON_SubDLimitMesh UpdateLimitSurfaceMesh(
    unsigned int minimum_display_density
    ) const;

  void ClearLimitSurfaceMesh() const;

  void ClearEvaluationCache() const;

  /*
  Description:
    Get an ON_Mesh of the subdivision limit surface
  Parameters:
    display_parameters - [in]
    mesh - [in]
      If not null, the returned mesh will be stored on
      the input class.
  Returns:
    A mesh of the subdivision limit surface.
  */
  class ON_Mesh* GetLimitSurfaceMesh(
    const class ON_SubDDisplayParameters& display_parameters,
    class ON_Mesh* mesh
    ) const;


 /*
  Description:
    Get a mesh of the subdivision control net.
  Parameters:
    level_index - [in] (>=0)
    mesh - [in]
      If not null, the returned mesh will be stored on
      the input class.
  Returns:
    The subdivision level as a mesh.
  */
  class ON_Mesh* GetControlNetMesh(
    class ON_Mesh* mesh
    ) const;


  /*
  Description:
    Get the limit surface mesh as a set of fragments.
  Parameters:
    display_parameters - [in]
   
   fragment_callback_context - [in]
      first parameter for the FragmentCallback function
    
    fragment_callback_function - [in]
      A function pointer with prototype:

        bool fragment_callback_function(
          void *fragment_callback_context, 
          const class ON_SubDLimitMeshFragment* fragment
          );

      For each fragment that is produced, fragment_callback_function() is called.
      You must copy the retuned fragment if you want to keep it for future use.
      If fragment_callback_function returns false, the calculation is canceled.
  Returns:
    Number of fragments produced.
  */
  unsigned int GetLimitSurfaceMeshInFragments(
    const class ON_SubDDisplayParameters& display_parameters,
    ON__UINT_PTR fragment_callback_context,
    bool(*fragment_callback_function)(ON__UINT_PTR , const class ON_SubDLimitMeshFragment*)
    ) const;
  
  /*
  Returns:
    The number of limit surface mesh fragments (ON_SubDLimitMeshFragment) that
    GetLimitSurfaceMeshFragments() will produce.
  */
  unsigned int LimitSurfaceMeshFragmentCount() const;


  /*
  Description:
    Get the limit surface as a set of bicubic patch fragments.
  Parameters:
    display_parameters - [in]
   
   fragment_callback_context - [in]
      first parameter for the FragmentCallback function
    
    fragment_callback_function - [in]
      A function pointer with prototype:

        bool fragment_callback_function(
          void *fragment_callback_context, 
          const class ON_SubDLimitPatchFragment* fragment
          );

      For each fragment that is produced, fragment_callback_function() is called.
      You must copy the retuned fragment if you want to keep it for future use.
      If fragment_callback_function returns false, the calculation is canceled.
  Returns:
    Number of fragments produced.
  */
  unsigned int GetLimitSurfaceInPatches(
    const class ON_SubDDisplayParameters& display_parameters,
    ON__UINT_PTR fragment_callback_context,
    bool(*fragment_callback_function)(ON__UINT_PTR , const class ON_SubDLimitPatchFragment*)
    ) const;

  /*
  Description:
    Get the SubD limit surface as a list of bicubic NURBS patches.
  Parameters:
    display_parameters - [in]
    bClampPatchKnots - [in]
      true to clamp knots
    sUserStringPatchKey - [in]
      If non empty, a user string with this key will be added that
      contains a description of which portion of which SubD face generated
      the patch.
    patches - [out]
      The bicubic NURBS patches are appended to this array.
  Returns:
    Number of patches appended to patches[]
  */
  unsigned int GetLimitSurfacePatches(
    const class ON_SubDDisplayParameters& display_parameters,
    bool bClampPatchKnots,
    const wchar_t* sUserStringPatchKey,
    ON_SimpleArray< ON_NurbsSurface* >& patches
    ) const;

public:
  /*
  Description:
    Pretend this function and ON_SubDimple do not exist.
  Returns:
    Something that you are pretending does not exist.
  Remarks:
    It is intentional that the definition of ON_SubDimple class is not
    available in the opennurbs library interface (not in a header file).
    The size and design of ON_SubDimple will change constantly.
    If you choose to hack and whack so you can dereference an
    ON_SubDimple* pointer, then your code will crash unpredictably.
  */
  const class ON_SubDimple* SubDimple() const;
  const class ON_SubDLevel& ActiveLevel() const;
  unsigned int SubDimpleUseCount() const;

  void ShareDimple(const ON_SubD&);
  void SwapDimple(ON_SubD&);

  void ShareDimple(const class ON_SubDLimitMeshImpl&);
  void SwapDimple(class ON_SubDLimitMeshImpl& );

private:
  class ON_SubDimple* SubDimple(bool bCreateIfNeeded);
  class ON_SubDLevel const * ActiveLevelConstPointer() const;
  class ON_SubDLevel* ActiveLevelPointer();

  void CopyHelper(const ON_SubD&);

private:
  friend class ON_SubDRef;
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_subdimple_sp is private and all code that manages m_subdimple_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr<class ON_SubDimple> m_subdimple_sp;
#pragma ON_PRAGMA_WARNING_POP

public:
  // The ON_SubD code increments ON_SubD::ErrorCount everytime something
  // unexpected happens. This is useful for debugging.
  static unsigned int ErrorCount;
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDRef
//
class ON_SUBD_CLASS ON_SubDRef
{
#if defined(ON_SUBD_CENSUS)
private:
  ON_SubDRefCensusCounter m_census_counter;
#endif

public:
  static const ON_SubDRef Empty;

  ON_SubDRef() ON_NOEXCEPT;
  ~ON_SubDRef();
  ON_SubDRef(const ON_SubDRef& src) ON_NOEXCEPT;
  ON_SubDRef& operator=(const ON_SubDRef& src);

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDRef( ON_SubDRef&& ) ON_NOEXCEPT;
  // rvalue assignment operator
  ON_SubDRef& operator=( ON_SubDRef&& );
#endif

  const class ON_SubD& SubD() const;

  /*
  Returns:
    Number of references to the ON_SubD, including the one by this ON_SubDRef.
  */
  unsigned int ReferenceCount() const;

  /*
  Description:
    Allocates a new ON_SubD and has this ON_SubDRef reference it.
  */
  class ON_SubD& NewSubD();


  /*
  Description:
    Allocates a new ON_SubD and has this ON_SubDRef reference it.
  */
  class ON_SubD& CopySubD(
    const ON_SubDRef& src
    );
  class ON_SubD& CopySubD(
    const ON_SubD& src
    );

  class ON_SubD& UniqueSubD();
  
  /*
  Description:
    Remove this reference to the managed ON_SubD. 
    If this is the last reference, then the managed ON_SubD is deleted.
  */
  void Clear();

public:
  class ON_SubDVertexIterator VertexIterator() const;
  class ON_SubDEdgeIterator EdgeIterator() const;
  class ON_SubDFaceIterator FaceIterator() const;

  /*
  Description:
    Expert user function to have this ON_SubDRef manage the lifetime of subd.
  Parameters:
    subd - [in/out]
      subd must point to an ON_SubD that was constructed on the heap using
      an operator new call with a public ON_SubD constructor.
  Returns:
    a pointer to the managed subd or nullptr subd in not valid.
  Example:
    ON_SubD* subd = new ON_SubD(...);
    ON_SubDRef subr;
    ON_SubD* managed_subd = subdr.SetSubD(subd);
    // subd = nullptr
    // managed_subd = pointer you can use
  */
  class ON_SubD* SetSubDForExperts(
    class ON_SubD*& subd
    );

  /*
  Description:
    Expert user function to have this ON_SubDRef reference the
    contents of an existing ON_SubD.
    Do not use if user data on the referenced subd needs to be accessed.
  Parameters:
    subd - [in]
      Any subd on the heap or the stack.
  Returns:
    true if successful.
  */
  static ON_SubDRef CreateReferenceForExperts(
    const ON_SubD& subd
    );

private:
 /*
  Description:
    Expert user function to have this ON_SubDRef reference the
    contents of an existing ON_SubD.
    Do not use if user data on the referenced subd needs to be accessed.
  Parameters:
    subd - [in]
      Any subd on the heap or the stack.
  */
  ON_SubDRef(
    const class ON_SubD& subd 
    );
  
private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_subd_sp is private and all code that manages m_subd_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr<class ON_SubD> m_subd_sp;
#pragma ON_PRAGMA_WARNING_POP
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDSectorType
//
class ON_SUBD_CLASS ON_SubDSectorType
{
public:
  ON_SubDSectorType() = default;
  ON_SubDSectorType(const ON_SubDSectorType&) = default;
  ON_SubDSectorType& operator=(const ON_SubDSectorType&) = default;

  static const ON_SubDSectorType Empty;

  bool IsValid() const;

  unsigned int SectorTypeHash() const;

  static int Compare(const ON_SubDSectorType*, const ON_SubDSectorType*);


  /////////////////////////////////////////////////////////////////////////////////////
  //
  // Sector Weights
  //
  /////////////////////////////////////////////////////////////////////////////////////
  //
  //   In the comment below, 
  //   F = number of faces in the sector,
  //   E = number of edges in the sector.
  //
  //   There are five valid sector configurations of edges and faces. In all
  //   configurations, the edges have one end at the center vertex and the
  //   faces have one corner at the center vertex.
  //
  //   SMOOTH
  //     1) The center vertex is smooth.
  //     2) F >= 2
  //     3) E = F
  //     4) Every edge is smooth.
  //     5) Every edge is an edge of two different faces in the sector.
  //
  //   DART
  //     1) The center vertex is a dart.
  //     2) F >= 2
  //     3) E = F
  //     4) One edge is a crease.
  //     5)  The crease edge is an edge of two geometrically adjacent sector faces.
  //
  //   DART* (The same as "DART", but the crease edge has been duplicated.)
  //     1) The center vertex is a dart.
  //     2) F >= 2
  //     3) E = F+1
  //     4) Two edges are creases that have the same end locations.
  //     5) Each crease edge is an edge of a single face in the sector,
  //        these faces are different and are geometrically adjacent.
  //
  //   BOUNDED
  //     1) The center vertex is a crease or corner vertex.
  //     2) F >= 2
  //     3) E = F+1
  //     4) Two edges are crease edges that have different vertices at their ends.
  //     5) Each crease edge is an edge of a single face in the sector,
  //        these faces are different and not geometrically adjacent.
  //
  //   BOUNDED*
  //     1) The center vertex is a crease or corner vertex.
  //     2) F = 1
  //     3) E = 2
  //     4) The edges are crease edges that have different vertices at their ends.
  //     5) The edges a edges of the face.
  //
  /////////////////////////////////////////////////////////////////////////////////////
  //
  //   The sector weight is used when subdividing smooth edges in sectors
  //   with a DART, DART* or BOUNDED configuration. In these cases the 
  //   sector weight is a value strictly between 0.0 and 1.0 that depends on
  //     1) the center vertex tag (crease, corner or dart), 
  //     2) the value of F,
  //     3) and when the center vertex is a corner, the angle between 
  //        the boundary edges.  
  //
  //   The sector weight is ignored when dividing smooth edges in SMOOTH sectors.
  //   The sector weight is ignored when subdividing crease edges.
  //
  //   For a smooth edge in a sector with a DART, DART* or BOUNDED configuration,
  //   with w = sector weight, C = location of the center vertex
  //   and P = location of the smooth vertex at the other end
  //   of the smooth edge, the point
  //   
  //     Q = 3/4 * (w*C + (1-w)*P) 
  //
  //   is the contribution of C and P  to the edge's subdivision point.
  //   
  //   When a smooth edge has smooth vertices at both ends located
  //   at A and B, the contribution of A and B to the edge's subdivision
  //   point is 
  //
  //     Q = 3/8 * (A + B) = 3/4 * (1/2*A + 1/2*B)
  //
  //   A crease edge's subdivision point is alwasy the edge's midpoint.
  /*
  Description:
    Calculates sector weight value for the sector type
    identified by this ON_SubDSectorType.
  Returns:
    w: 0.0 <= w < 1.0
      w = sector theta value.
    ON_SubDSectorType::ErrorSectorWeight
      This ON_SubDSectorType is not valid and the calculation failed.
  */
  double SectorWeight() const;


  ON_SubD::SubDType SubDType() const;

  ON_SubD::FacetType FacetType() const;

  unsigned int FacetEdgeCount() const;

  ON_SubD::VertexTag VertexTag() const;


  unsigned int EdgeCount() const;

  unsigned int FaceCount() const;

  /*
  Returns:
   Number of points in the point ring.  
   For quad subds, this is 1 + FaceCount() + EdgeCount().
   For tri subds, this is 1 + EdgeCount().
  */
  unsigned int PointRingCount() const;

  /*
  Returns:
   1 + FaceCount() + EdgeCount()
  */
  unsigned int ComponentRingCount() const;

  /*
  Returns:
    If the sector vertex tag is ON_SubD::VertexTag::Corner,
    the angle between the corner crease boundary edges is
    returned.  
    Otherwise, ON_SubDSectorType::ErrorCornerSectorAngle is returned.
  */
  double CornerSectorAngleRadians() const;

  /*
  Returns:
    a value >= 0 and <= ON_SubDSectorType::MaximumAngleIndex
  */
  unsigned int CornerSectorAngleIndex() const;

  /*
  Description:
    An angle index value of ON_SubDSectorType::MaximumAngleIndex indicates
    the angle is 2pi radians.
  */
  static const unsigned int MaximumAngleIndex; // = 72

  /*
  Parameters:
    angle_radians - [in] (0.0 <= angle_radians <= 2*ON_PI
      The angle between the bounding crease edges
  Returns:
    angle_index: >= 0 and <= ON_SubDSectorType::MaximumCornerSectorIndex
      | angle_radians - angle_index/M * 2pi | <= 1/2 * 1/M * 2pi,
      where M = ON_SubDSectorType::MaximumAngleIndex
    ON_UNSET_UINT_INDEX
      angle_radians is not valid and the calculation failed.
  */
  static unsigned int AngleIndexFromAngleRadians(
    double angle_radians
    );

  /*
  Convert and angle index into radians
  Parameters:
    angle_index - [in]
      0 to ON_SubDSectorType::MaximumAngleIndex.
  Returns:
    If angle_index is valid, the corresponding angle in radians is returned.
      = angle_index / ON_SubDSectorType::MaximumAngleIndex * 2 * ON_PI
        (double division performed)
    Otherwise ON_UNSET_VALUE is returned.
  */
  static double AngleRadiansFromAngleIndex(
    unsigned int angle_index
    );

  /*
  Returns:
    True if this is a smooth interior vertex sector
  */
  bool IsSmoothSector() const;


  /*
  Returns:
    True if this is a dart interior vertex sector
  */
  bool IsDartSector() const;


  /*
  Returns:
    True if this is a crease vertex sector
  */
  bool IsCreaseSector() const;

  /*
  Returns:
    True if this is a corner vertex sector
  */
  bool IsCornerSector() const;

  /*
  Returns:
    True if this is a convex corner vertex sector (sector angle <= pi)
  */
  bool IsConvexCornerSector() const;

  /*
  Returns:
    True if this is a concave corner vertex sector (sector angle > pi)
  */
  bool IsConcaveCornerSector() const;

  /*
  Parameters:
    sector_boundary_edge0_ptr - [in]
    sector_boundary_edge1_ptr - [in]
      Crease edges that bound the sector containing the smooth edge.
      The edge direction must identify the corner vertex.
  Returns:
    tagged end angle for a smooth edge that
    1) ends at a vertex tagged on ON_SubD::VertexTag::Corner
    2) has two adjacent faces.
    3) lies in a sector bounded by 2 distinct crease edges.
  */
  static double CornerSectorAngleRadiansFromEdges(
    ON_SubDEdgePtr sector_boundary_edge0_ptr,
    ON_SubDEdgePtr sector_boundary_edge1_ptr
    );

  static bool IsValidCornerSectorAngleRadians(
    double corner_sector_angle_radians
    );

  static double ClampCornerSectorAngleRadians(
    double corner_sector_angle_radians
    );

  /*
  Returns:
    Number of subdivision points in a sector ring
    facet_type vertex_tag ring count
    tri        smooth     N+1
    tri        crease     N+2
    quad       smooth     2N+1
    quad       crease     2N+2
    (2 * valence + 1) for quad subds
    (valence + 1) for tri subds
  */
  static unsigned int SectorPointRingCountFromEdgeCount(
    ON_SubD::SubDType subd_type,
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_edge_count
    );

  static unsigned int SectorPointRingCountFromFaceCount(
    ON_SubD::SubDType subd_type,
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_face_count
    );

  static unsigned int SectorFaceCountFromEdgeCount(
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_edge_count
    );

  static unsigned int SectorEdgeCountFromFaceCount(
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_face_count
    );
  
  static unsigned int MinimumSectorEdgeCount(
    ON_SubD::VertexTag vertex_tag
    );
  
  static unsigned int MinimumSectorFaceCount(
    ON_SubD::VertexTag vertex_tag
    );

public:
  /*
  Returns:
    ON_SubDSectorType::IgnoredSectorWeight
  */
  static double SmoothSectorWeight();
  
  /*
  Parameters:
  face_type - [in]
  sector_face_count - [in]
    number of faces in the smooth sector.
  Returns:
    0: 
      failed to caclulate weight
    ON_SubDSectorType::UnsetSectorWeight:
      subd_type was set ON_SubD::SubDType::Unset
      and was required to calculate the weight.
      This typically happens when a SubD control net is being 
      created and a facet type is not specified.  
      The weights will be calculated at the first subdivision.
    0 < w < 1:
      1/2 + 1/3*cos(tagged end angle) for quadrangle facets
      1/3 + 1/3*cos(tagged end angle) for triangle facets
  Remarks:
    This is a useful tool when calling AddEdge while a subdivision
    level is being constructed.
  */
  static double CreaseSectorWeight(
    ON_SubD::SubDType subd_type,
    unsigned int sector_face_count
    );

  static double DartSectorWeight(
    ON_SubD::SubDType subd_type,
    unsigned int sector_face_count
    );

  static double CornerSectorWeight(
    ON_SubD::SubDType subd_type,
    unsigned int sector_face_count,
    double corner_sector_angle_radians
    );

  // This value is is used to set sector angles when the
  // actual value is not needed. This occurs at both ends
  // of a creased edge and when the end of a smooth edge
  // is a smooth vertex.
  static const double IgnoredCornerSectorAngle; // = 0.0;

  // This value is used to set sector weights that could not be
  // correctly set because something in the calculation failed.  
  // It is typically used when an invalid component in SubD object
  // was needed to calculate the weight.
  static const double UnsetCornerSectorAngle; // = -8881.0;

  // This value is indicate a corner sector angle calculation failed.
  static const double ErrorCornerSectorAngle; // = -9991.0;


  // This value is used for smooth sector thetas
  static const double SmoothSectorTheta; // = 0.5*ON_PI

  // This value is used to indicate a sector theta needs to be set
  static const double UnsetSectorTheta; // = -8882.0;

  // This value is used to indicate a sector theta calculation failed.
  static const double ErrorSectorTheta; // = -9992.0;


  // This value is is used to set sector weights when the
  // actual value is not needed. This occurs at both ends
  // of a creased edge and when the end of a smooth edge
  // is a smooth vertex.
  static const double IgnoredSectorWeight; // = 0.0;

  // This value is used to mark sector weights that need to be
  // set in the future when more information is available.
  // It is typically used when creating a subD control net
  // and the facet type is not known. Any value < 0.0 and not
  // equal to ON_UNSET_VALUE would work. The fact that the actual
  // value is -999.0 has no other significance.
  static const double UnsetSectorWeight; // = -8883.0;

  // This value is indicate a sector weight calculation failed.
  static const double ErrorSectorWeight; // = -9993.0;

  static bool IsValidSectorWeightValue(
    double weight_value,
    bool bAllowUnsetTaggedEndWeight
    );

  /*
  Returns:
    ON_SubDSectorType::ErrorSectorWeight and calls debug breakpoint
  */
  static double SectorWeightCalculationError();

  
  /*
  Description:
    Create a ON_SubDSectorType from a ON_SubDSectorIterator.
  Parameters:
    subd_type - [in]
    vertex_tag - [in]
    sector_face_count - [in]
      Number of faces in the sector.
    corner_sector_angle_radians - [in]
      If vertex_tag is ON_SubD::VertexTag::Corner, this
      parameter is the angle between the crease edges
      that bound the corner.  
      If vertex_tag is not  ON_SubD::VertexTag::Corner,
      this parameter is ignored.
  Returns:
    An ON_SubDSectorType for the case the input parameters 
    identify.
  */
  static ON_SubDSectorType Create(
    ON_SubD::SubDType subd_type,
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_face_count,
    double corner_sector_angle_radians
    );


  /*
  Description:
    Create a ON_SubDSectorType from a ON_SubDSectorIterator.
  Parameters:
    subd_type - [in]
    sit - [in]
  Returns:
    An ON_SubDSectorType for the sector identified by sit.
  */
  static ON_SubDSectorType Create(
    ON_SubD::SubDType subd_type,
    const ON_SubDSectorIterator& sit
    );

  /*
  Description:
    Create a ON_SubDSectorType for the sector containing the face.
  Parameters:
    subd_type - [in]
    face - [in]
    face_vertex_index - [in]
      face->Vertex(face_vertex_index) will be the sector's
      center vertex.
  Returns:
    An ON_SubDSectorType for the sector containing the face.
  */
  static ON_SubDSectorType Create(
    ON_SubD::SubDType subd_type,
    const class ON_SubDFace* face,
    unsigned int face_vertex_index
    );
  
  static ON_SubDSectorType Create(
    ON_SubD::SubDType subd_type,
    const class ON_SubDFace* face,
    const class ON_SubDVertex* vertex
    );
  
  /*
  Description:
    Create a ON_SubDSectorType for the sector containing the edge.
  Parameters:
    subd_type - [in]
    edge - [in]
    edge_vertex_index - [in]
      edge->Vertex(edge_vertex_index) will be the sector's
      center vertex.
  Returns:
    An ON_SubDSectorType for the sector containing the edge.
  */
  static ON_SubDSectorType Create(
    ON_SubD::SubDType subd_type,
    const class ON_SubDEdge* edge,
    unsigned int edge_vertex_index
    );

  /*
  Description:
    Create a smooth ON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
  Returns:
    An ON_SubDSectorType for the smooth sector case specified
    by the input parameters.
  */
  static ON_SubDSectorType CreateSmoothSectorType(
    ON_SubD::SubDType subd_type,
    unsigned int sector_face_count
    );

  /*
  Description:
    Create a crease ON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
  Returns:
    An ON_SubDSectorType for the crease sector case specified
    by the input parameters.
  */
  static ON_SubDSectorType CreateCreaseSectorType(
    ON_SubD::SubDType subd_type,
    unsigned int sector_face_count
    );

  /*
  Description:
    Create a dart ON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
  Returns:
    An ON_SubDSectorType for the dart sector case specified
    by the input parameters.
  */
  static ON_SubDSectorType CreateDartSectorType(
    ON_SubD::SubDType subd_type,
    unsigned int sector_face_count
    );

  /*
  Description:
    Create a corner ON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
    corner_sector_angle_radians - [in]
      The angle between the crease edges that bound the corner.  
  Returns:
    An ON_SubDSectorType for the corner sector case specified
    by the input parameters.
  */
  static ON_SubDSectorType CreateCornerSectorType(
    ON_SubD::SubDType subd_type,
    unsigned int sector_face_count,
    double sector_corner_angle_radians
    );

  static int Compare(
    const ON_SubDSectorType& a,
    const ON_SubDSectorType& b
    );


  /*
  Description:
    Get the subdivision matrix for the default subdivison algorithms 
    used by ON_SubD.

    The matrix coefficents are ordered so that the matrix acts on
    the left of the points returned by ON_SubDSectorIterator::GetVertexRing().

    For an interior vertex (smooth or dart), the coefficents are ordered
    so that one iteration of subdivision is given by:
    ON_SubD::SubDType::TriLoopWarren case:
      S*Transpose(V, E[0], E[1], ..., E[N-1])
    ON_SubD::SubDType::QuadCatmullClark case:
      S*Transpose(V, E[0], F[0], E[1], F[1], ..., E[N-1], F[N-1]).
    For a dart vertex, E[0] is the point at the end of the creased edge.


    For a boundary vertex (crease or corner), the coefficents are ordered
    so that one iteration of subdivision is given by:
    ON_SubD::SubDType::TriLoopWarren case:
      S*Transpose(V, E[0], E[1], ..., E[N-1]).
    ON_SubD::SubDType::QuadCatmullClark case:
      S*Transpose(V, E[0], F[0], E[1], F[1], ..., F[N-2], E[N-1]).

    N = edge valence = number of edges in the sector.
    E[i] = end of i-th edge radiating from V.
    In the ON_SubD::SubDType::QuadCatmullClark case, F[i] = point on the quad 
    that is opposite V.
    The edges and faces are ordered radially so that the face for F[i]
    lies between the edges for E[i] and E[(i+1)%N].

  Parameters:
    matrix_capacity - [in]
      S[] can store any RxR matrix with R <= matrix_capacity.
    S - [out]
      subdivision matrix
      Matrix coefficent (i,j) = S[i][j]
      0 <= i < R, 0 <= j < R, R = ON_SubDSectorType.PointRingCount()

  Returns:
    R > 0: 
      R = PointRingCount() and S is the RxR subdivision matrix for the sector type.
    0: Invalid input
  */
  unsigned int GetSubdivisionMatrix(
    size_t matrix_capacity,
    double** S
    ) const;

  /*
  Parameters:
    S_capacity - [in]
      Number of elements in S[] array
    S - [out]
      subdivision matrix. 
      Matrix coefficent (i,j) = S[i*R + j],
      0 <= i < R, 0 <= j < R, R = ON_SubDSectorType.PointRingCount()
  Returns:
     0: Invalid input.
    >0: Number of rows and columns in S.
        This number is always ON_SubDSectorType.PointRingCount().
  */
  unsigned int GetSubdivisionMatrix(
    size_t S_capacity,
    double* S
    ) const;

  /*
  Description:
    All the subdivision matrices for the ON_SubD built-in
    subdivision algorithms have eigenvalues (1, lambda1, lambda2, e4, ..., eR),
    where 1 > lambda1 >= lambda2 > |e4| >= ... >= |eR| > 0.

    The subdominant eigenvalue is lambda1 and, 
    with one exception, lambda1 = lambda2.
    The exception is described in the description of 
    ON_SubDSectorType::SubdominantEigenvalueMulitiplicity().

  Returns:
    > 0.0: 
      The subdominant eigenvalue for the subdivision matrix.

    ON_UNSET_VALUE:
      This ON_SubDSectorType is not valid.
  */
  double SubdominantEigenvalue() const;

  /*
  Returns:
    0:
      The sector type is not set.

    2: 
      The subdominant eigenvalue has algebraic and geometric multiplicty = 2.
      This is the most common case.

    1: 
      The subdominant eigenvalue has algebraic and geometric multiplicty = 1.
      This occures in Catmull-Clark subdivision at a crease vertex with 
      two crease edges and a single face.  The subdivision matrix for this
      case is
        S is a 4 x 4 matrix with rows =
           (3/4, 1/8, 0, 1/8), 
           (1/2, 1/2, 0, 0),
           (1/4, 1/4, 1/4, 1/4), 
           (1/2, 0, 0, 1/2).
        S has 4 real eigenvalues = (1, 1/2, 1/4, 1/4), all wtih
        geometric multiplicity = 1.
        The three eigenvectors are
           (1, 1, 1, 1), (0, -1, 0, 1), (0, 0, 1, 0).
  */
  unsigned int SubdominantEigenvalueMulitiplicity() const;

  /*
  Description:
    With one exception, which is described below,
    all the subdivision matrices for the ON_SubD built-in
    subdivision algorithms have eigenvalues (1, lambda, lambda, e4, ..., eR),
    where lambda is real, 1 > lambda > |e4| >= ... >= |eR| > 0, and the
    geometric dimension of the lambda eigenspace is 2 (there are two
    linearly independent lambda eigenvectors). 

    The subdominant eigenvalue is lamda.  This function returns an
    orthogonal basis, (E1, E2), for the subdominant eigenspace.

    An eigenvector for the dominant eigen value 1 has is (1,1,...,1).
    The domainant eignevector is orthogonal to the subdominant eigenspace.
    
    Put another way,
    0 = E1[0] + ... + E1[R-1]
    0 = E2[0] + ... + E2[R-1]
    0 = E1[0]*E2[0] + ... + E1[R-1]*E2[R-1]

    Exceptional case:
      The Catmull-Clark subdivision matrix for a crease vertex with
      two crease edges and a single face is a special case.  
      In this exceptional, this function returns
        lambda = 1/2, E1 = {0,-1,0,-1} and E2 = {1, -2, -5, -2}.
      For more information about the exceptional case, see the description of
      ON_SubDSectorType::SubdominantEigenvalueMulitiplicity().

  Parameters:
    E1_capacity - [in]
      Capacity of the E1[] array.
    E1 - [out]
    E2_capacity - [in]
      Capacity of the E2[] array.
    E2 - [out]
      When E1_capacity > 0 and E2_capacity > 0, two orthoganal eigenvectors
      spanning the subdivision matrix subdominant eigenspace are returned
      in E1[] and E2[].
      If one of E1_capacity or E2_capacity is > 0, then both must be > 0.

  Returns:
    ON_UNSET_VALUE: Invalid input.
    e > 0.0 and e < 1.0:
      subdominant eigenvalue.
  */
  double GetSubdominantEigenvectors(
    size_t E1_capacity,
    double* E1,
    size_t E2_capacity,
    double* E2
    ) const;

  /*
  Parameters:
    LPev_capacity - [in]
      Capacity of the LPev[] array.
    LPev - [out]
      When LPev_capacity > 0, then the limit surface point evaluation coefficients are
      returned in LPev[].  Otherwise LPev is ignored.
    LT0ev_capacity - [in]
      Capacity of the LPev[] array.
    LT0ev - [out]
    LT1ev_capacity - [in]
      Capacity of the LPev[] array.
    LT1ev - [out]
      When LT0ev_capacity > 0 and LT1ev_capacity > 0, then the limit surface
      tangent coefficients are returned in LT0ev[] and LT1ev[]. Otherwise,
      LT0ev[] and LT1ev[] are ignored.
      If one of LT0ev_capacity or LT1ev_capacity is > 0, then both must be > 0.
  Returns:
     0: Invalid input.
    >0: Number of evaluation coefficients in the L*ev[] arrays.
        This number is always ON_SubDSectorType.PointRingCount().
  */
  unsigned int GetLimitSurfaceEvaluationCoefficients(
    size_t LPev_capacity,
    double* LPev,
    size_t LT0ev_capacity,
    double* LT0ev,
    size_t LT1ev_capacity,
    double* LT1ev
    ) const;

  // LimitSurfaceNormalSign() is a debugging tool - slow and not useful in general
  double LimitSurfaceNormalSign() const;

  bool LimitEvaluationCoefficientsAvailable() const;

  /*
  Parameters:
    eigenvalues_capacity - [in]
      Capacity of the eigenvalues[] array.
      Must be 0 or >= PointRingCount()
    eigenvalues - [out]
      If 0 = eigenvalues_capacity, eigenvalues must be nullptr.
      If eigenvalues_capacity > 0, is specifies the capactiy 
      of the eigenvalues[] array.
  Returns:
    R > 0: 
      A complete set of eigenvalues is available for this sector type.
      The eigenvalues are (1, lambda, lambda, e3, ..., eR), where
      1 > lambda > e3 >= ... >= eR > 0.
    0: 
      Invalid input or the eigenvalues for this sector typoe are not available.
  */
  unsigned int GetAllEigenvalues(
    size_t eigenvalues_capacity,
    double* eigenvalues
    );

  /*
  Description:
    The subdivision matrix for all cases is known.
    A complete set of eigenvalues are available for some cases.
  Parameters:
    facet_type - [in]
    vertex_tag - [in]
    sector_edge_count - [in]
      The input parameters identify the subdivision case.
  Returns:
    R > 0: Eigenvalues are known.  There subdivison matrix is R x R.
    0: Eigenvalues for this case are not known.
  */
  static unsigned int AllEigenvaluesAvailableKnown(
    ON_SubD::SubDType subd_type,
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_edge_count
    );

  /*
  Description:
    Create a partial subdivison sector around vertex.
    The resulting ON_SubD has an outer ring with smooth edges and vertices,
    which is not valid as a stand-alone subd.  This is typically used for
    testing.
  Parameters:
    radius - [in]
      The center vertex is located at (0,0,0),
      If radius > 0.0, then the end of the first edge is at (radius,0,0), 
      subsequent edges are in  a radial array and quad face points, if any, 
      are 2*radius from the origin.
    sector_angle_radians - [in]
      If radius > 0, 
      this->VertexTag() is ON_SubD::VertexTag::Crease,
      crease_sector_angle_radians > 0.0 and 
      crease_sector_angle_radians < 2.0*ON_PI,
      then this will be the angle between the crease boundary edges.
      In all other cases, crease_sector_angle_radians is ignored.
    subd - [in]
      If subd is not null, the vertex ring is put in this
      subd.
  Returns:
    a pointer to the vertex ring
    nullptr is returned if the input is not valid.
  */
  ON_SubD* SectorRingSubD(
    double radius,
    double sector_angle_radians,
    ON_SubD* subd
    ) const;

private:
  ON_SubD::SubDType m_subd_type = ON_SubD::SubDType::Unset;
  ON_SubD::FacetType m_facet_type = ON_SubD::FacetType::Unset;
  ON_SubD::VertexTag m_vertex_tag = ON_SubD::VertexTag::Unset;
  unsigned char m_reserved1 = 0;
  unsigned int m_hash = 0; // SetHash() sets this field, SectorTypeHash() returns its value.
  unsigned int m_corner_sector_angle_index = 0; // >= 0 and <= ON_SubDSectorType::MaximumAngleIndex
  unsigned int m_sector_face_count = 0;
  double m_sector_weight = 0.0;
  double m_sector_theta = 0.0;
  double m_corner_sector_angle_radians = 0.0;  
  
private:
  void SetHash();

  /*
  Description:
    Calculates sector theta value for the sector type
    identified by this ON_SubDSectorType.
  Returns:
    theta: 0.0 <= theta <= ON_PI
      The sector theta value.
    ON_SubDSectorType::ErrorSectorTheta
      This ON_SubDSectorType is not valid and the calculation failed.
  */
  double SectorTheta() const;

  /*
  Parameters:
    sector_face_count - [in] >= 1
      Number of faces in the crease sector.
  Returns:
    theta: 0.0 < theta <= ON_PI
      sector theta value for a crease sector with sector_face_count faces.
    ON_SubDSectorType::ErrorSectorTheta
      sector_face_count is not valid and the calculation failed.
  */
  static double CreaseSectorTheta(
    unsigned int sector_face_count
    );
  
  /*
  Parameters:
    sector_face_count - [in] >= 2
      Number of faces in the dart sector.
  Returns:
    theta: 0.0 < theta <= ON_PI
      sector theta value for a dart sector with sector_face_count faces.
    ON_SubDSectorType::ErrorSectorTheta
      sector_face_count is not valid and the calculation failed.
  */
  static double DartSectorTheta(
    unsigned int sector_face_count
    );

  /*
  Parameters:
    sector_face_count - [in] >= 2
      Number of faces in the dart sector.
    corner_sector_angle_radians - [in] (0.0 <= corner_sector_angle_radians <= 2*ON_PI
      The angle between the bounding crease edges
  Returns:
    theta: 0.0 < theta <= ON_PI/2
      sector theta value for the corner sector.
    ON_SubDSectorType::ErrorSectorTheta
      sector_face_count or corner_sector_angle_radians were not valid
      and the calculation failed.
  */
  static double CornerSectorThetaFromCornerAngle(
    unsigned int sector_face_count,
    double corner_sector_angle_radians
    );

  /*
  Parameters:
    subd_type - [in]
    sector_theta - [in]
      value from one of the sector theta functions.
      ON_SubDEdge::SectorTheta()
      ON_SubDEdge::SmoothSectorTheta()
      ON_SubDEdge::CreaseSectorTheta()
      ON_SubDEdge::CornerSectorTheta()
      ON_SubDEdge::DartSectorTheta()
  Returns:
    0: 
      failed to caclulate weight
    ON_UNSET_VALUE: 
      subd_type was set ON_SubD::SubDType::Unset
      and the tagged end weight cannot be calculated until 
      the facet type is known.  This typically happens
      when a SubD control net is being created and
      a facet type is not specified.  The weights will
      be calculated at the first subdivision.
    0 < w < 1:
      If ON_SubD::SubDType::QuadCatmullClark == subd_type, 
      then the returned value is 
      1/2 + 1/3*cos(sector_angle_radians). (1/6 <= w <= 5/6)
      If ON_SubD::SubDType::TriLoopWarren == subd_type, 
      then the returned value is
      1/3 + 1/3*cos(sector_angle_radians). (0 < w <= 2/3)
  Remarks:
    This is a useful tool when calling AddEdge while a subdivision
    level is being constructed.
  */
  static double SectorWeightFromTheta(
    ON_SubD::SubDType subd_type,
    double sector_theta
    );
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDLimitMeshFragment
//
//  Meshes of ON_SubD limit surface are calculated in fragments.
//
class ON_SUBD_CLASS ON_SubDLimitMeshFragmentGrid
{
public:
  // No construction for performance reasons. 
  // If you require initialization, use = ON_SubDLimitMeshFragmentGrid::Empty
  //
  //ON_SubDLimitMeshFragmentGrid() = default;
  //~ON_SubDLimitMeshFragmentGrid() = default;
  //ON_SubDLimitMeshFragmentGrid(const ON_SubDLimitMeshFragmentGrid&) = default;
  //ON_SubDLimitMeshFragmentGrid& operator=(const ON_SubDLimitMeshFragmentGrid&) = default;
  static const ON_SubDLimitMeshFragmentGrid Empty;

  /*
  Description:
    Get mesh facet quads that index into a grid of points.
  Parameters:
    side_segment_count - [in]
      number quads in each row and column of the quad grid.
        side_segment_count >= 1
        side_segment_count <= ON_SubDLimitMesh::MaximumSideSegmentCount
        side_segment_count must be a power of 2

    level_of_detail - [in]
      0: quad count = maximum quad count = (side_count x side_count)
      1: quad count = 1/4 maximum quad count
      1: quad count = 1/16 maximum quad count
      ...
      If 4^level_of_detail > maximum quad count, then a single quad is returned.
  */
  static ON_SubDLimitMeshFragmentGrid Quads(
    unsigned int side_segment_count,
    unsigned int level_of_detail
    );

  static ON_SubDLimitMeshFragmentGrid Tris(
    unsigned int side_segment_count,
    unsigned int level_of_detail
    );

  static ON_SubDLimitMeshFragmentGrid Facets(
    ON_SubD::FacetType facet_type,
    unsigned int side_segment_count,
    unsigned int level_of_detail
    );

  /*
  Description:
    Get mesh facet quads that index into a grid of points.
  Parameters:
    side_segment_count - [in]
      number quads in each row and column of the quad grid
      with the highest level of detail (level_of_detail = 0)
      side_count must be a power of 2
    level_of_detail - [in]
      Desired level of detail of the returned grid
        0: highest (side_count x side_count) quads
        1: 1/4 density (side_count x side_count)/4 quads
        2: 1/16 density (side_count x side_count)/16 quads
        ...
        side_count-2: 4 quads
        side_count-1: 1 quad
        >= side_count: 1 quad
  Returns:
    Number of quads in the grid.
  */
  static unsigned int SetQuads(
    unsigned int side_segment_count,
    unsigned int level_of_detail,
    size_t quad_capacity,
    size_t quad_stride,
    unsigned int* quads,
    size_t side_capacity,
    size_t side_stride,
    unsigned int* sides
    );


  unsigned int SideSegmentCount() const;

  /*
  Description:
    The GridId() is persistent and unique based on the contents of the
    grid. It is intended to be used in render applications that store
    copies of ON_SubDLimitMeshFragmentGrid settings in things like
    vertex object buffers and want a reliable way to index into them.
    The Empty grid has id = 0;
  Returns:
    0:
      when the grid is empty
    32*n + 2*lod + t:
      t = 0 for quads and 1 for tris,
      (0 <= n <= 8) m_F_count = 2^(2n), 
      (0 <= lod <= 8) m_F_level_of_detail = lod
  Remarks:
    m_F_count is always
  */
  unsigned int GridId() const;

  /*
  Returns:
    3 for tris, 4 for quads, 0 for unset.
  */
  unsigned int GridFacetSideCount() const;

  bool GetGridParameters(
    unsigned int grid_point_index,
    double grid_parameters[2]
    ) const;

  
private:
  unsigned char m_reserved;

public:

  ON_SubD::FacetType m_F_type;
  unsigned char m_side_segment_count; // = 2^n for non-empty grids (0 <= n <= 8)
  unsigned short m_F_count;   // = m_side_count*m_side_count
  unsigned short m_F_level_of_detail; // 0 = highest, > 0 = reduced
  unsigned short m_F_stride;
  const unsigned int* m_F;
  const unsigned int* m_S; // [4*m_side_segment_count + 1] indices that form the polyline boundary.
  const ON_SubDLimitMeshFragmentGrid* m_prev_level_of_detail; // nullptr or the previous level with more facets.
  const ON_SubDLimitMeshFragmentGrid* m_next_level_of_detail; // nullptr or the next level with fewer facets.
};

class ON_SUBD_CLASS ON_SubDLimitMeshFragment
{
public:
  // No construction for performance reasons. 
  // If you require initialization, use = ON_SubDLimitMeshFragment::Empty
  //
  //ON_SubDLimitMeshFragment() = default;
  //~ON_SubDLimitMeshFragment() = default;
  //ON_SubDLimitMeshFragment(const ON_SubDLimitMeshFragment&) = default;
  //ON_SubDLimitMeshFragment& operator=(const ON_SubDLimitMeshFragment&) = default;
  
  // Every field of ON_SubDLimitMeshFragment::Empty is zero.
  static const ON_SubDLimitMeshFragment Empty;

  static const unsigned int MaximumSideSegmentCount;
  
  /*
  Returns:
    side_segment_count  = 2^display_density
  */
  static unsigned int SideSegmentCountFromDisplayDensity(
    unsigned int display_density
    );
  
  /*
  Returns:
    base 2 log of side_segment_count.
  Remarks:
    side_segment_count = 2^display_density
  */
  static unsigned int DisplayDensityFromSideSegmentCount(
    unsigned int side_segment_count
    );

  /*
  Parameters:
    facet_type - [in]
      ON_SubD::FacetType::Quad or ON_SubD::FacetType::Tri

    display_density - [in]
      >= 0
  Returns:
    total number of points in the limit mesh fragment.
  Remarks:
    The number of points is the same for quad or tri subdivision limit
    mesh fragments, even though one is a rectanglular collection of 
    quads and the other is a trianglular collection of triangles.
  */
  static unsigned int PointCountFromDisplayDensity(
    ON_SubD::FacetType facet_type,
    unsigned int display_density
    );

  /*
  Parameters:
    display_density - [in]
      >= 0
  Returns:
    total number of faces in the limit mesh fragment.
  */
  static unsigned int FaceCountFromDisplayDensity(
    unsigned int display_density
    );

  /*
  Returns:
    true if side_segment_count is valid.
    Otherwise 0 is returned.
  */
  static bool SideSegmentCountIsValid(
    unsigned int side_segment_count
    );

  /*
  Returns:
    If side_segment_count is valide, then (side_segment_count+1) is returned.
    Otherwise 0 is returned.
  */
  static unsigned int SidePointCountFromSideCount(
    unsigned int side_segment_count
    );

  /*
  Returns:
    If side_segment_count is valide, then (side_segment_count+1)^2 is returned.
    Otherwise 0 is returned.
  */
  static unsigned int QuadGridPointCountFromSideCount(
    unsigned int side_segment_count
    );

  /*
  Returns:
    If side_segment_count is valide, then side_segment_count^2 is returned.
    Otherwise 0 is returned.
  */
  static unsigned int QuadGridQuadCountFromSideCount(
    unsigned int side_segment_count
    );

public:
  const class ON_SubDFace* m_face;

  // m_face_vertex_index[] stores the information needed for the Vertex()
  // and Edge() functions to work.
  //
  // If m_face is nullptr, then m_face_vertex_index[] has no meaning.
  // If m_face is not nullptr and a corner of the grid is on a face
  // vertex, then the corresponding m_face_vertex_index[] value
  // will be <= ON_SubDFace::MaximumEdgeCount and m_face->Vertex(m_face_vertex_index[])
  // is the vertex.  Otherwise,  the corresponding m_face_vertex_index[] value
  // will be > ON_SubDFace::MaximumEdgeCount.  For partial fragments,
  // only some m_face_vertex_index[] identify vertices and the grid extends
  // halfway along the neighboring face edges are 
  unsigned short m_face_vertex_index[4];

  /*
  Parameters:
    grid_side_index - [in]
      0 to 3 for quad grids.
      0 to 2 for tri grids
  Returns:
    The subd edge that is on the identified side of the grid.
  */
  const class ON_SubDEdgePtr EdgePtr(
    unsigned int grid_side_index
    ) const;
  const class ON_SubDEdge* Edge(
    unsigned int grid_side_index
    ) const;

  const class ON_SubDVertexPtr VertexPtr(
    unsigned int grid_corner_index
    ) const;
  const class ON_SubDVertex* Vertex(
    unsigned int grid_corner_index
    ) const;

  ON_3dPoint CornerPoint(
    unsigned int grid_corner_index
    ) const;

  /*
  Returns:
    Status of the face.
  */
  ON_ComponentStatus Status() const;

  /*
  Returns:
    True if this fragment covers a subset of a face.
  */
  bool IsSubFragment() const;

  /*
  Returns:
    True if this fragment covers an entier subd face.
  */
  bool IsCompleteFragment() const;

  bool Transform(
    const ON_Xform& xform
    );  

  unsigned short m_face_fragment_count; // Number of fragments that will be delivered for this face.
  unsigned short m_face_fragment_index; // First fragment has index = 0. Last fragment has index = m_face_fragment_count-1.

  // For quad based subdivision algorithms, the mesh fragment
  // is a tesselation of a rectangular shaped surface,
  // there are m_side_count quad edges along each side of the tesselation,
  // there are a total of m_side_count X m_side_count quads, and
  // m_P_count = (m_side_count+1)*(m_side_count+1).
  //
  // For trangle based subdivision algorithms, the mesh fragment 
  // is a tesselation of a triangular shaped surface,
  // there are m_side_count triangle edges along each side of the tesselation,
  // there are a total of m_side_count X m_side_count triangles, and
  // m_P_count = (m_side_count+1)*(m_side_count+2)/2.
  //

  // Number of points
  unsigned short m_P_count;
  unsigned short m_P_capacity;

  // points
  size_t m_P_stride;
  // The memory m_P references is managed by some other class or function.
  // Never modify the values in m_P.
  double* m_P;

  // surface normals parallel to m_P[] array
  size_t m_N_stride;
  // If m_N is not nullptr, then it can accomodate up to m_P_capacity normals.
  // The memory m_N references is managed by some other class or function.
  // Never modify the values in m_N.
  double* m_N;
  
  // quads or tris
  ON_SubDLimitMeshFragmentGrid m_grid; // 

  ON_BoundingBox m_bbox;

  ON_SubDLimitMeshFragment* m_next_fragment;
  ON_SubDLimitMeshFragment* m_prev_fragment;
};

class ON_SUBD_CLASS ON_SubDManagedLimitMeshFragment : public ON_SubDLimitMeshFragment
{
public:
  ON_SubDManagedLimitMeshFragment() ON_NOEXCEPT;
  ~ON_SubDManagedLimitMeshFragment() ON_NOEXCEPT;
  ON_SubDManagedLimitMeshFragment(const ON_SubDManagedLimitMeshFragment&) ON_NOEXCEPT;
  ON_SubDManagedLimitMeshFragment& operator=(const ON_SubDManagedLimitMeshFragment&) ON_NOEXCEPT;

  static ON_SubDManagedLimitMeshFragment Create(const ON_SubDLimitMeshFragment& src) ON_NOEXCEPT;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDManagedLimitMeshFragment( ON_SubDManagedLimitMeshFragment&& ) ON_NOEXCEPT;

  // rvalue assignment operator
  ON_SubDManagedLimitMeshFragment& operator=( ON_SubDManagedLimitMeshFragment&& ) ON_NOEXCEPT;
#endif

  void Clear() ON_NOEXCEPT;

  void Destroy() ON_NOEXCEPT;

  bool ReserveCapacity(
    ON_SubD::FacetType facet_type,
    unsigned int mesh_density
    ) ON_NOEXCEPT;

private:
  void CopyHelper(const ON_SubDLimitMeshFragment& src);
  size_t m_storage_capacity = 0;
  double* m_storage = nullptr;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDDisplayParameters
//
//  A collection of parameters that are passed to functions that
//  calculate a various representations of the limit surface.
//
class ON_SUBD_CLASS ON_SubDDisplayParameters
{
public:
  static const ON_SubDDisplayParameters Empty;

  // Parameters for the default limit surface display mesh.
  static const ON_SubDDisplayParameters DefaultDisplayMeshParameters;

  /*
  Description:
    In most applications, the caller sets the mesh_density
    and leaves the other parameters set to the default
    values.
  */
  static ON_SubDDisplayParameters CreateFromDisplayDensity(
    unsigned int display_density
    );

  ON_SubDDisplayParameters() = default;
  ~ON_SubDDisplayParameters() = default;
  ON_SubDDisplayParameters(const ON_SubDDisplayParameters&) = default;
  ON_SubDDisplayParameters& operator=(const ON_SubDDisplayParameters&) = default;

  unsigned int m_display_density = 0;

  bool m_bUseMultipleThreads = false;
  ON_Terminator* m_terminator = nullptr;
  // optional progress reporting
  ON_ProgressReporter* m_progress_reporter = nullptr;
  ON_Interval m_progress_reporter_interval = ON_Interval::ZeroToOne;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDLimitMesh
//
class ON_SUBD_CLASS ON_SubDLimitMesh
{
#if defined(ON_SUBD_CENSUS)
private:
  ON_SubDLimitMeshCensusCounter m_census_counter;
#endif

public:
  static const ON_SubDLimitMesh Empty;

  /*
  Returns:
    A runtime number that changes if the limit mesh content changes.
    0: Empty limit mesh
  Remarks:
    This is a runtime number. It is not saved in archives and begins
    at 1 with each new runtime instance of the opennurbs library.
  */
  unsigned int ContentSerialNumber() const;

  enum : unsigned int
  {
    DefaultDisplayDensity = 4, // default limit mesh density 16x16 quads per SubD quad 16 = 2^4
    MaximumDisplayDensity = 8  // 8 (256x256 quads per SubD quad 256 = 2^8)
    //MaximumLevelOfDetail = 0, // 0 = most facets per fragment
    //MinimumLevelOfDetail = 8 // 8 = fewest facets per fragment
  };

  static ON_SubDLimitMesh* Create(
    const ON_SubD& subd,
    const class ON_SubDDisplayParameters& limit_mesh_parameters,
    ON_SubDLimitMesh* destination_mesh
    );

  /*
  Description:
    This version is for expert users who want to take
    responsibility for managing the subd and limit mesh
  */
  static ON_SubDLimitMesh* Create(
    ON_SubDFaceIterator fit,
    const class ON_SubDDisplayParameters& limit_mesh_parameters,
    ON_SubDLimitMesh* destination_mesh
    );

  ON_SubDLimitMesh() = default;
  ~ON_SubDLimitMesh() = default;
  ON_SubDLimitMesh(const ON_SubDLimitMesh&) = default;
  ON_SubDLimitMesh& operator=(const ON_SubDLimitMesh&) = default;

  
#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDLimitMesh( ON_SubDLimitMesh&& ) ON_NOEXCEPT;
  // rvalue assignment operator
  ON_SubDLimitMesh& operator=( ON_SubDLimitMesh&& );
#endif

  ON_SubDLimitMesh Copy() const;

  ON_SubDLimitMesh& CopyFrom(
    const ON_SubDLimitMesh& src
    );

  static void Swap(
    ON_SubDLimitMesh& a,
    ON_SubDLimitMesh& b
    );

  bool Transform( 
    const ON_Xform& xform
    );

  unsigned int DisplayDensity() const;
  ON_SubDDisplayParameters DisplayParameters() const;
  unsigned int FragmentCount() const;
  const ON_SubDLimitMeshFragment* FirstFragment() const; // linked list of mesh fragments
  
  /*
  Description:
    If the subd referenced by m_subd_ref changes, then call
    Update to update the limit mesh. 
  */
  bool Update(
    bool bShareUpdate
    );

  /*
  Description:
    The ON__INT_PTRs in the tree are const ON_SubDLimitMeshFragment* pointers.
  */
  const ON_RTree& FragmentTree() const;

  /*
  Description:
    Clears everything.
  */
  void Clear();

  /*
  Description:
    If the tree is not needed and memory resources are tight, then call ClearTree()
    to remove the RTree.
  */
  void ClearTree();

  bool IsEmpty() const;

  ON_SubD::FacetType GridType() const;

  ON_BoundingBox BoundingBox() const;

  bool GetTightBoundingBox(
    ON_BoundingBox& bbox,
    bool bGrowBox,
    const ON_Xform* xform
    ) const;

  ON_SubDRef SubDRef() const;
  ON_SubD SubD() const;

public:
  /*
  Description:
    Pretend this function and ON_SubDLimitMeshImpl do not exist.
  Returns:
    Something that you are pretending does not exist.
  Remarks:
    It is intentional that the definition of ON_SubDLimitMeshImpl class is not
    available in the opennurbs library interface (not in a header file).
    The size and design of ON_SubDLimitMeshImpl will change constantly.
    If you choose to hack and whack so you can dereference an
    ON_SubDLimitMeshImpl* pointer, then your code will crash unpredictably.
  */
  class ON_SubDLimitMeshImpl* SubLimple() const;
  unsigned int SubLimpleUseCount() const;

private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  friend class ON_SubDLimitMeshImpl;
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_impl_sp is private and all code that manages m_impl_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< class ON_SubDLimitMeshImpl > m_impl_sp;
#pragma ON_PRAGMA_WARNING_POP

};



class ON_SUBD_CLASS ON_SubDLimitPatchFragment
{
public:
  // No construction for performance reasons. 
  // If you require initialization, use = ON_SubDLimitMeshFragment::Empty
  //
  //ON_SubDLimitMeshFragment() = default;
  //~ON_SubDLimitMeshFragment() = default;
  //ON_SubDLimitMeshFragment(const ON_SubDLimitMeshFragment&) = default;
  //ON_SubDLimitMeshFragment& operator=(const ON_SubDLimitMeshFragment&) = default;
  
  // Every field of ON_SubDLimitPatchFragment::Empty is zero.
  static const ON_SubDLimitPatchFragment Empty;

  // Every m_patch_cv[][][] value is ON_UNSET_VALUE. 
  // Every other field of ON_SubDLimitPatchFragment::Unset is zero.
  static const ON_SubDLimitPatchFragment Unset;

  // Every m_patch_cv[][][] value is ON_DBL_QNAN.
  // Every other field of ON_SubDLimitPatchFragment::Unset is zero.
  static const ON_SubDLimitPatchFragment Nan;

#pragma region RH_C_SHARED_ENUM [SubD::PatchType] [Rhino.Geometry.SubD.PatchType] [internal:nested:byte]
  enum class PatchType : unsigned char
  {
    ///<summary>Not a valid patch type.</summary>
    Unset = 0,

    ///<summary>Entire subdivision face is an exact bicubic patch.</summary>
    Bicubic = 1,

    ///<summary>A quadrant of the subdivision face is an exact bicubic patch.</summary>
    BicubicQuadrant = 2,

    ///<summary>Entire subdivision face is approximately a bicubic patch.</summary>
    ApproximateBicubic = 3,

    ///<summary>A quadrant of the subdivision face is approximately a bicubic patch.</summary>
    ApproximateBicubicQuadrant = 4,

    ///<summary>A patch cannot be calculated at the current subdivision level.</summary>
    None = 5
  };
#pragma endregion

public:
  double m_patch_cv[5][5][3];
  const double* m_patch_knots[2]; // nullptr or 7 uniform cubic knots. Never modify these values.

  const class ON_SubDFace* m_level0_face;


  // m_patch_state[] reports what information is returned in m_patch_cv[] and m_patch_knots[].
  // m_patch_state[0] report the state for the cubic patch:
  //   CV[0][0] = m_patch_cv[0][0], knot[0] = m_patch_knots[0],   knot[1] = m_patch_knots[1]
  // m_patch_state[1] report the state for the cubic patch:
  //   CV[0][0] = m_patch_cv[1][0], knot[0] = m_patch_knots[0]+1, knot[1] = m_patch_knots[1]
  // m_patch_state[2] report the state for the cubic patch:
  //   CV[0][0] = m_patch_cv[1][1], knot[0] = m_patch_knots[0]+1, knot[1] = m_patch_knots[1]+1
  // m_patch_state[3] report the state for the cubic patch:
  //   CV[0][0] = m_patch_cv[0][1], knot[0] = m_patch_knots[0],   knot[1] = m_patch_knots[1]+1
  ON_SubDLimitPatchFragment::PatchType m_patch_type[4];

  unsigned short m_patch_level;

  // When the subdivision method is quad based and m_face is a quad, there is one region.
  // When the subdivision method is quad based and m_face is not a quad, there are m_face->m_edge_count regions.
  unsigned short m_level0_face_region_count; // Number of regions in m_face.
  unsigned short m_level0_face_region_index; // First region has index = 0. Last region has index = m_face_region_count-1.

  unsigned short m_face_subdivision_count;
  unsigned short m_face_region_index[10];  // vertex index for subdivision

public:
  ON_SubDLimitPatchFragment* m_next_fragment;
  ON_SubDLimitPatchFragment* m_prev_fragment;
};


class ON_SUBD_CLASS ON_SubDSectorLimitPoint
{
public:
  // For performance reasons, the default the data members are
  // not initialized by the default constructor
  // Use = ON_SubDSectorLimitPoint::Unset when initialization is required
  static const ON_SubDSectorLimitPoint Unset; // all doubles are ON_UNSET_VALUE, all pointer are nullptr
  static const ON_SubDSectorLimitPoint Nan;   // all doubles are ON_DBL_QNAN, all pointer are nullptr
  static const ON_SubDSectorLimitPoint Zero;  // all doubles are 0.0, all pointer are nullptr
  
  /*
  Returns:
    true if m_limitP[0] is a nan (like ON_DBL_QNAN).
    false otherwise.
  */
  bool IsUnset() const;

  /*
  Returns:
    true if m_limitP[0] is ON_UNSET_VALUE.
    false otherwise.
  */
  bool IsNan() const;

  /*
  Returns:
    true if all coordinates are zero.
    false otherwise.
  */
  bool IsZero() const;

  /*
  Returns:
    true if all coordinates are valid doubles and the tangents and normal have at least
    one nonzero coordinate.
    false otherwise.
  */
  bool IsSet() const;

  bool Transform(
    const ON_Xform& xform
    );

  // limit surface point, tangents and normal
  double m_limitP[3];  // point
  double m_limitT1[3]; // first unit tangent
  double m_limitT2[3]; // second unit tangent
  double m_limitN[3];  // unit normal (same direction as m_limitT1 x m_limitT2)

  // When an ON_SubDVertex has a single sector, these pointers are both null.
  // When an ON_SubDVertex has a multiple sectors, 
  // m_sector_face is the "first" face in the sector and
  // m_next_sector_limit_point is used to create a linked list.
  // (The "first" face in a sector is the one ON_SubDSectorIterator.IncrementToCrease(-1) returns.)
  const class ON_SubDSectorLimitPoint* m_next_sector_limit_point; // nullptr for vertices with one sector
  const class ON_SubDFace* m_sector_face;                         // nullptr for vertices with one sector
};



////////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentBase
//
class ON_SUBD_CLASS ON_SubDComponentBase
{
public:
  static const ON_SubDComponentBase Unset;

  ///*
  //Returns:
  //  True if component is not nullptr, component->m_id > 0 and component->m_archive_id != ON_UNSET_UINT_INDEX.
  //*/
  //static bool IsActive(
  //  const ON_SubDComponentBase* component
  //  );

public:
  ON_SubDComponentBase() = default;
  ~ON_SubDComponentBase() = default;
  ON_SubDComponentBase(const ON_SubDComponentBase&) = default;
  ON_SubDComponentBase& operator=(const ON_SubDComponentBase&) = default;

public:
  // The audience for this comment is anybody who wants to change the data
  // fields in ON_SubDComponentBase. Everyone else should ignore this comment.
  //   ON_SubD components come from ON_FixedSizePool and ON_SubD code
  //   uses ON_FixedSizePool.ReturnElement.  The first sizeof(void*) bytes
  //   must be a data field that is not referenced in returned elements.
  //   Since a returned element cannot have a "next level vertex",
  //   m_subd_point1 is a good data member to put first.

  // m_subd_point1 points to the next level's vertex when this component
  // has been subdivided using an algorithm like Catmull-Clark or Loop-Warren.
  const class ON_SubDVertex* m_subd_point1 = nullptr;

public:
  // The audience for this comment is anybody who wants to change the data
  // fields in ON_SubDComponentBase. Everyone else should ignore this comment.
  //   It is critical that the offset of m_id in ON_SubDComponentBase be >= sizeof(void*).
  //   ON_SubD components come from ON_FixedSizePool and ON_SubD code
  //   use ON_FixedSizePool.ElementFromId and ON_FixedSizePool.ReturnElement.
  //   Once assigned, m_id is never changed and that allows ON_SubD component
  //   indices to work.

  // Id assigned to this component.  Never modify this value.  It is assigned
  // by allocators and used to find the component from an ON_COMPONENT_INDEX.
  unsigned int m_id = 0;

private:
  // The m_archive_id must be immediately after the m_id field.
  mutable unsigned int m_archive_id = 0;  
  
public:
  unsigned short m_level = 0;
  
public:
  mutable ON_ComponentStatus m_status = ON_ComponentStatus::NoneSet;

public:
  
  //////////////////////////////////////////////////////////////
  //
  // Saved subdivision point
  //
  /*
  Description:
    Set the saved subdivision point.
  Parameters:
    subdivision_point_type - [in]
      Specifies subdivision algorithm.
      Use ON_SubD::SubDType::Unset to clear the cache.
    subdivision_point - [in]
      includes displacement if it exists
  Returns:
    true if successful
  */
  bool SetSavedSubdivisionPoint(
    ON_SubD::SubDType subd_type,
    const double subdivision_point[3]
    ) const;

  bool GetSavedSubdivisionPoint(
    ON_SubD::SubDType subd_type,
    double subdivision_point[3]
    ) const;

  ON_SubD::SubDType SavedSubdivisionPointType() const;

  /*
  Description:
    Clears saved subdivision information for this component.
  */
  void ClearSavedSubdivisionPoint() const;

  //////////////////////////////////////////////////////////////
  //
  // displacement applied to subdivision point
  //
  bool SetDisplacement(
    ON_SubD::SubDType subd_type,
    const double displacement[3]
    );

  bool GetDisplacement(
    ON_SubD::SubDType subd_type,
    double displacement[3]
    ) const;
  
  ON_SubD::SubDType DisplacementType() const;

  void ClearDisplacement() const;

protected:
  // GetSubdivisionPoint( bUseSavedSubdivisionPoint=true ) can change the value of m_saved_points_flags
  // m_saved_points_flags & 0x1F = ON_SubD::SubDType value
  // m_saved_points_flags & 0x40 != 0 if m_cache_subd_P is set.
  // m_saved_points_flags & 0x80 != 0 if m_displacementV is set.
  // GetLimitPoint( bUseSavedLimitPoint=true ) can change the value of m_saved_points_flags
  // m_saved_points_flags & 0x20 != 0 if ON_subDVertex.m_limit* values are set.
  mutable unsigned char m_saved_points_flags = 0U;

public:

  // All the faces with the same nonzero value of m_group_id are in the same "group".
  // SDK interface on ON_SubD will be added after we get a better idea of how this
  // feature will be used.
  unsigned int m_group_id = 0U;

protected:
  // GetSubdivisionPoint( bUseSavedSubdivisionPoint=true ) can change the value of m_cache_subd_P
  mutable double m_saved_subd_point1[3]; // saved subdivision point

protected:
  // optional displacement applied to standard subdivision point.
  double m_displacement_V[3];

public:
  /*
  Description:
    Pretend ArchiveId() and SetArchiveId() do not exist.
  Returns:
    The ArchiveId is a value set and used by ON_BinaryArchive Read() and Write()
    functions and copy constructors and operator=(). 
    A public interface is supplied because it is not practical to use friends.
  Remarks:
    A value of ON_UNSET_UINT_INDEX indicates the component is not in use.
  */
  unsigned int ArchiveId() const
  {
    return m_archive_id;
  }

  void SetArchiveId(
    unsigned int archive_id
    ) const
  {
    // m_archive_id is mutable
    if ( ON_UNSET_UINT_INDEX != archive_id )
      m_archive_id = archive_id; 
  }

protected:
  void CopyBaseFrom(
    const ON_SubDComponentBase* src
    );
};

////////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertex
//
class ON_SUBD_CLASS ON_SubDVertex : public ON_SubDComponentBase
{
public:
  static const ON_SubDVertex Empty;  

  bool Write (
    class ON_BinaryArchive& archive
    ) const;

  static bool Read (
    class ON_BinaryArchive& archive,
    class ON_SubD& subd,
    class ON_SubDVertex*& vertex
    );

  /*
  Description:
    Apply a tranxfomration matrix to vertex geometry information.
  Parameters:
    bTransformationSavedSubdivisionPoint - [in]
      If the transformation is being applied to every vertex, edge and 
      face in every level of a subdivision object, and the transformation
      is an isometry (rotation, translation, ...), a uniform scale, or a 
      composition of these types, then set 
      bTransformationSavedSubdivisionPoint = true to apply the
      transformation to saved subdivision and saved limit point information.
      In all other cases, set bTransformationSavedSubdivisionPoint = false
      and any saved subdivision points or saved limit points will be
      deleted.  When in doubt, pass false.

    xform - [in]
  */
  bool Transform(
    bool bTransformationSavedSubdivisionPoint,
    const class ON_Xform& xform
    );

  bool SetLocation(
    ON_3dPoint location,
    bool bClearNeighborhoodCache
  );


  ON_BoundingBox ControlNetBoundingBox() const;
  ON_BoundingBox LimitSurfaceBoundingBox(
    const ON_SubD& subd
    ) const;

public:
  ON_COMPONENT_INDEX ComponentIndex() const;
  ON_SubDComponentPtr ComponentPtr() const;

public:
  const class ON_SubDVertex* m_prev_vertex = nullptr; // linked list of vertices on this level
  const class ON_SubDVertex* m_next_vertex = nullptr; // linked list of vertices on this level

public:
  ON_SubD::VertexTag m_vertex_tag = ON_SubD::VertexTag::Unset;

  
private:
  //ON_SubD::VertexEdgeOrder m_vertex_edge_order = ON_SubD::VertexEdgeOrder::unset;
  unsigned char  m_reserved1 = 0;
  unsigned short  m_reserved2 = 0;
  unsigned int m_reserved3 = 0;

public:
  unsigned short m_edge_count = 0;
  unsigned short m_face_count = 0;

  unsigned short m_edge_capacity = 0;
  unsigned short m_face_capacity = 0;

public:
  // Array of m_edge_count edges.
  //   m_edge[i].EdgeDirection() indicates which edge end is located at this vertex
  //   If m_edge_capacity > 0, m_edge_capacity is the number of elements that
  //   may be used in m_edges[].
  class ON_SubDEdgePtr* m_edges = nullptr;

  // Array of m_face_count faces.
  //   If m_face_capacity > 0, m_face_capacity is the number of elements that
  //   may be used in m_faces[].
  const class ON_SubDFace** m_faces = nullptr;

public:
  double m_P[3]; // vertex location

private:
  // Cached limit point and limit normal
  // GetLimitPoint( bUseSavedLimitPoint=true ) can change the value of m_limitP_type
  // If the limit point is set and vertex has a single sector, then 
  // m_limit_point.m_sector_face = nullptr and m_limit_point.m_next_sector_limit_point = nullptr.
  // If the limit point is set and vertex has a multiple sectors, then 
  // m_limit_point.m_sector_face = first face in the sector.
  // If multiple limit points are set, then are in a linked list
  // traversed using the ON_SubDSectorLimitPointm_next_sector_limit_point.
  // The second and any additional limit points are managed by a fixed size pool.
  // Calling ClearLimitPoint() will return these to the pool.
  mutable ON_SubDSectorLimitPoint m_limit_point = ON_SubDSectorLimitPoint::Unset;

public:
  static const unsigned int MaximumEdgeCount;
  static const unsigned int MaximumFaceCount;

  static int CompareUnorderedEdges(
    const ON_SubDVertex* a,
    const ON_SubDVertex* b
    );

  static int CompareUnorderedFaces(
    const ON_SubDVertex* a,
    const ON_SubDVertex* b
    );

  static int CompareUnorderedEdgesAndFaces(
    const ON_SubDVertex* a,
    const ON_SubDVertex* b
    );

  ///*
  //Description:
  //  Sort the m_edges[] and m_faces[] arrays so radial groups are together.
  //  After the sorting is completed, m_vertex_edge_order is set to recored
  //  the current sorting state and its value is returned.  
  //  The sorting is done unconditionally.
  //*/
  //ON_SubD::VertexEdgeOrder SortEdges();

  unsigned int EdgeCount(
    ON_SubD::EdgeTag edge_tag
    ) const;

  unsigned int EdgeCount() const;

  const class ON_SubDEdge* Edge(
    unsigned int i
    ) const;

  const ON_SubDEdgePtr EdgePtr(
    unsigned int i
    ) const;

  ON__UINT_PTR EdgeDirection(
    unsigned int i
    ) const;

  unsigned int EdgeArrayIndex(
    const ON_SubDEdge* edge
    ) const;

  unsigned int FaceCount() const;

  const class ON_SubDFace* Face(
    unsigned int i
    ) const;

  unsigned int FaceArrayIndex(
    const ON_SubDFace* face
    ) const;

  ON_SubD::FacetType FirstFaceFacetType() const;

  /*
  Returns 
    true if m_vertex_tag is ON_SubD::VertexTag::Crease, ON_SubD::VertexTag::Corner or ON_SubD::VertexTag::Dart.
  */
  bool IsTagged() const;

  ///*
  //Parameters:
  //  subd_type - [in]
  //    Specifies subdivision algorithm
  //  vertex_tag_filter - [in]
  //    If vertex_tag is not ON_SubD::VertexTag::Unset and vertex_tag != m_vertex_tag,
  //    then false is returned.  This parameter can be used when a smooth or crease
  //    vertex is explicity required.  
  //  bTestFaces - [in]
  //    If true, and the edge and face count tests succeed, then the faces in the 
  //    vertex m_faces[] array are tested to insure they are 
  //    quads (ccquad subdivisiontype) or tris (lwtri subdivisiontype).
  //Returns:
  //  If m_vertex_tag is ON_SubD::Vertex::Tag::smooth, 
  //  and the number of edges = number of faces, 
  //  and there are 4 (ccquad subdivisiontype) or 6 (lwtri subdivisiontype) edges,
  //  and bTestFaces is false or the faces pass the face test,
  //  then true is returned.
  //  
  //  If m_vertex_tag is ON_SubD::Vertex::Tag::crease, 
  //  and the number of edges = 1 + number of faces, 
  //  and there are 3 (ccquad subdivisiontype) or 4 (lwtri subdivisiontype) edges,
  //  and bTestFaces is false or the faces pass the face test,
  //  then true is returned.

  //  In all other cases, false is returned.
  //*/
  //bool IsOrdinary(
  //  ON_SubD::SubDType subd_type,
  //  ON_SubD::VertexTag vertex_tag_filter,
  //  bool bTestFaces
  //  ) const;


  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Smooth.
  */
  bool IsSmooth() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Crease.
  */
  bool IsCrease() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Corner.
  */
  bool IsCorner() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Dart.
  */
  bool IsDart() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Smooth or ON_SubD::VertexTag::Crease.
  */
  bool IsSmoothOrCrease() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Crease or ON_SubD::VertexTag::Corner.
  */
  bool IsCreaseOrCorner() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Crease or ON_SubD::VertexTag::Corner or ON_SubD::VertexTag::Dart.
  */
  bool IsCreaseOrCornerOrDart() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Smooth or ON_SubD::VertexTag::Dart.
  */
  bool IsSmoothOrDart() const;

  /*
  Description:
    A "standard" vertex is one where the standard subdivsion matrix for that vertex
    can be used to calculate the subdivision point. 
    This function is desinged to be useful for testing and debugging code when
    a certain situation is expected to exist.  It is not used for evaluation
    because it is too slow.

  Returns:
    True if the subdivison point of the vertex can be calulated using the standard
    subdivion matrix for the vertex type and face count.

  Remarks:
    If the vertex is tagged and has multiple sectors, like an interior
    crease or corner vertex, then this function will return false.
    In this situation, it is possible that the vertex, as the center of a
    sector, is standard.
  */
  bool IsStandard(
    ON_SubD::SubDType subd_type
    ) const;

  /*
  Parameters:
    subdivision_point_type - [in]
      Selects subdivision algorithm. Must be either
      ON_SubD::SubDType::TriLoopWarren or ON_SubD::SubDType::QuadCatmullClark.
    bUseSavedSubdivisionPoint - [in]
      If there is a saved subdivision point and bUseSavedSubdivisionPoint
      is true, then the saved value is returned.
    subdivision_point - [out]
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    ON_SubD::SubDType subdivision_point_type,
    bool bUseSavedSubdivisionPoint,
    double subdivision_point[3]
    ) const;

  /*
  Parameters:
    facet_type - [in]
      Selects subdivision algorithm.
    bUseSavedLimitPoint - [in]
      If there is a saved limit point and normal and bUseSavedLimitPoint
      is true, then the saved value is used.
    limit_point - [out]
  Returns:
    true if successful
  */
  bool GetLimitPoint(
    ON_SubD::SubDType subd_type,
    const ON_SubDFace* sector_face,
    bool bUseSavedLimitPoint,
    class ON_SubDSectorLimitPoint& limit_point
    ) const;

  /*
  Description:
    Save limit point and limit normal for future use.
  Parameters:
    subd_type - [in]
    limit_point - [in]
    limit_normal - [in]
  Returns:
    true if successful
  */
  bool SetSavedLimitPoint(
    ON_SubD::SubDType subd_type,
    const ON_SubDSectorLimitPoint limit_point
    ) const;

  void ClearSavedLimitPoints() const;

  /*
  Returns:
    ON_SubD::SubDType::TriLoopWarren
      The vertex limit point and normal are saved for Loop trianglular subdivision.
    ON_SubD::SubDType::QuadCatmullClark
      The vertex limit point and normal are saved for Catmull-Clark quad subdivision.
    ON_SubD::SubDType::Unset
      The vertex limit point and normal are not saved.
  */
  ON_SubD::SubDType SavedLimitPointType() const;

  /*
  Description:
    Report what type of facet or facets use this vertex.
  */
  ON_SubD::VertexFacetType VertexFacetTypes() const;


  /*
  Description:
    Call this function if the vertex is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void VertexModifiedNofification() const;

private:
  static bool GetQuadPoint(
    const class ON_SubDVertex* vertex, // smooth or dart
    bool bUseSavedSubdivisionPoint,
    double vertex_point[3]
    );

  static bool GetTriPoint(
    const class ON_SubDVertex* vertex, // smooth or dart
    bool bUseSavedSubdivisionPoint,
    double vertex_point[3]
    );

  static unsigned int GetFacePointSum(
    const ON_SubDFace* face,
    const ON_SubDVertex* vertex,
    double* facePsum // sum of points that are not immediately adjacent to vertex
    );


  /*
  Description:
    Used for smooth and dart vertices when there are faces 
    that use the vertex have different numbers of sides. 
    This typically happen when a quad subd control net is
    being subdivided for the first time.
  Parameters:
    vertex - [in]
    vertex_point - [out]
  Returns:
    true if successful
  */
  static bool GetGeneralQuadSubdivisionPoint(
    const class ON_SubDVertex* vertex,
    bool bUseSavedSubdivisionPoint,
    double vertex_point[3]
    );

private:
  friend class ON_SubDArchiveIdMap;
  void CopyFrom(
    const ON_SubDVertex* src,
    bool bCopyEdgeArray,
    bool bCopyFaceArray,
    bool bCopyLimitPointList
    );
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdge
//
class ON_SUBD_CLASS ON_SubDEdge : public ON_SubDComponentBase
{
public:
  static const ON_SubDEdge Empty;
  
  bool Write (
    class ON_BinaryArchive& archive
    ) const;

  static bool Read (
    class ON_BinaryArchive& archive,
    class ON_SubD& subd,
    class ON_SubDEdge*& edge
    );

  /*
  Description:
    Apply a tranxfomration matrix to vertex geometry information.

  Parameters:
    bTransformationSavedSubdivisionPoint - [in]
      If the transformation is being applied to every vertex, edge and 
      face in every level of a subdivision object, and the transformation
      is an isometry (rotation, translation, ...), a uniform scale, or a 
      composition of these types, then set 
      bTransformationSavedSubdivisionPoint = true to apply the
      transformation to saved subdivision and saved limit point information.
      In all other cases, set bTransformationSavedSubdivisionPoint = false
      and any saved subdivision points or saved limit points will be
      deleted.  When in doubt, pass false.

    xform - [in]
  */
  bool Transform(
    bool bTransformationSavedSubdivisionPoint,
    const class ON_Xform& xform
    );

  ON_BoundingBox ControlNetBoundingBox() const;
  ON_BoundingBox LimitSurfaceBoundingBox(
    const ON_SubD& subd
    ) const;


  /*
  Description:
    Call this function if the edge is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void EdgeModifiedNofification() const;


public:
  ON_COMPONENT_INDEX ComponentIndex() const;
  ON_SubDComponentPtr ComponentPtr() const;

public:
  const class ON_SubDEdge* m_prev_edge = nullptr; // linked list of edges on this level
  const class ON_SubDEdge* m_next_edge = nullptr; // linked list of edges on this level

public:
  // When checking the edge tag, it is important to consider what
  // should happen in the ON_SubD::EdgeTag::X case.  It is strongly
  // suggested that you use the member functions ON_SubDEdge::IsSmooth()
  // and ON_SubDEdge::IsCrease() instead of comparing m_edge_tag to 
  // ON_SubD::EdgeTag values.
  ON_SubD::EdgeTag m_edge_tag = ON_SubD::EdgeTag::Unset;

private:
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;

public:
  // Array of m_face_count faces.
  //
  //  The first two are in m_face2[0] and m_face2[1].
  //  When m_face_count > 3, the third and additional faces
  //  are in m_facex[0], ..., m_facex[m_face_count-3];
  //
  //  The value of ON_SubDFacePtr.FaceDirection() is 0 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  agrees with the face's boundary orientation.
  //
  //  The value of ON_SubDFacePtr.FaceDirection() is 1 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  is opposited the face's boundary orientation.
  static const unsigned int MaximumFaceCount;
  unsigned short m_face_count = 0;
  unsigned short m_facex_capacity = 0;
  ON_SubDFacePtr m_face2[2];
  ON_SubDFacePtr* m_facex = nullptr;

  // m_vertex[0] = vertex at the start of the edge.
  // m_vertex[1] = vertex at the end of the edge.
  const class ON_SubDVertex* m_vertex[2];

  // If the value of vertex->m_vertex_tag is not ON_SubD::VertexTag::Smooth,
  // then that vertex is "tagged". 
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::Crease,
  // then m_sector_coefficient[] should be {0,0}.  
  // In any case m_sector_coefficient[] values are ignored and the
  // midpoint of the edge is the location of the edge.s subdivision point.
  // The edge's subdivision vertex will be tagged as ON_SubD::VertexTag::Crease
  // and both subdivision edges will be tagged as ON_SubD::EdgeTag::Crease.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::Smooth
  // and neither end vertex is tagged, then m_sector_coefficient[] should be {0,0}.  
  // In any case m_sector_coefficient[] values are ignored on smooth edges
  // with smooth vertices at both ends.
  // The edge's subdivision vertex will be tagged as ON_SubD::VertexTag::Smooth
  // and both subdivision edges will be tagged as ON_SubD::EdgeTag::Smooth.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::Smooth and
  // exactly one end vertex is tagged, then the m_sector_coefficient[]
  // value for the tagged end is calculated by ON_SubDSectorType::SectorWeight().
  // tagged_weight*tagged_vertex + (1.0 - tagged_weight)*untagged_vertex
  // is used when combining the edge ends.
  // The edge's subdivision vertex will be tagged as ON_SubD::VertexTag::Smooth
  // and both subdivision edges will be tagged as ON_SubD::EdgeTag::Smooth.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::X, then the edge
  // must have exactly two neighboring faces,
  // both vertices must be tagged and the m_sector_coefficient[]
  // values are calculated by ON_SubDSectorType::SectorWeight().
  // When the edge is subdivided, the midpoint of the edge is the 
  // location of the edge.s subdivision point.
  // The edge's subdivision vertex will be tagged as ON_SubD::VertexTag::Smooth
  // and both subdivision edges will be tagged as ON_SubD::EdgeTag::Smooth.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::Smooth
  // and both end vertices are tagged, that is a severe error
  // condition and the edge is subdivided at its midpoint.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::X
  // and both end vertices are not tagged, that is a severe error
  // condition and the edge is subdivided at its midpoint.
  double m_sector_coefficient[2];

  // If m_edge_tag is not ON_SubD::EdgeTag::Sharp, then m_sharpness is ignored.
  // If m_edge_tag is ON_SubD::EdgeTag::Sharp, then m_sharpness controls how hard/soft
  // the edge appears.  
  // The behavior of a "sharp" edge with m_sharpness = 1 is identical to a crease edge.
  // A "sharp" edge with m_sharpness = 0 is identical to a smooth edge.
  // For this reason, m_sharpness must be > 0 and < 1.
  double m_sharpness = 0.0; 

public:
  unsigned int FaceCount() const;

  ON_SubDFacePtr FacePtr(
    unsigned int i
    ) const;

  ON_SubDFacePtr FacePtr(
    const class ON_SubDFace* f
    ) const;

  const class ON_SubDFace* Face(
    unsigned int i
    ) const;

  ON__UINT_PTR FaceDirection(
    unsigned int i
    ) const;

  unsigned int FaceArrayIndex(
    const class ON_SubDFace* f
    ) const;

  /*
  Description:
    Expert user tool to remove a face from the edges's face array.
  Remarks:
    Does not modify the face. If the edge is referenced in the face's edge array,
    then the edge must be removed from the face's edge array.
  */
  bool RemoveFaceFromArray(
    const ON_SubDFace* f
    );

  /*
  Description:
    Expert user tool to remove a face from the edges's face array.
  Remarks:
    Does not modify the face.  If the edge is referenced in the face's edge array,
    then the edge must be removed from the face's edge array.
  */
  bool RemoveFaceFromArray(
    unsigned int i,
    ON_SubDFacePtr& removed_face
    );

  /*
  Description:
    Expert user tool to add a face from the edges's face array.
  Remarks:
    Does not modify the face.  If the edge is not referenced in the face's edge array,
    then the edge must be inserted in the correct location in the faces array.
    If you are creating a non-manifold SubD, you must first reserve m_facex[]
    capacity by calling ON_SubD::GrowEdgeFaceArray().
  */
  bool AddFaceToArray(
    ON_SubDFacePtr face_ptr
  );

  const class ON_SubDVertex* Vertex(
    unsigned int i
    ) const;

  /*
  Description:
    Return the vertex at the other end of the edge.
  Parameters:
    vertex - [in]
      A vertex referenced in the edge's m_vertex[] array.
  Returns:
    If vertex is not nullptr and exactly one of m_vertex[] is equal to vertex, 
    then the other m_vertex[] pointer is returned.
    In any other case, nullptr is returned.
  */
  const ON_SubDVertex* OtherEndVertex(
    const ON_SubDVertex* vertex
    ) const;

  /*
  Description:
    Return the neighboring face.
  Parameters:
    face - [in]
      A face referenced in the edge's m_face2[] array.
    bStopAtCrease - [in]
      If true and if m_edge_tag = ON_SubD::EdgeTag::Crease, 
      then nullptr is returned.
  Returns:
    If the m_face_count = 2,
    m_edge_tag is smooth or x or passes the crease tag test,
    one of m_face2[0,1] points a face, then
    the neighboring face is returned.
    In any other case, nullptr is returned.
  */
  const ON_SubDFace* NeighborFace(
    const ON_SubDFace* face,
    bool bStopAtCrease
    ) const;

  /*
  Description:
    Return the neighboring face.
  Parameters:
    face - [in]
      A face referenced in the edge's m_face2[] array.
    bStopAtCrease - [in]
      If true and if m_edge_tag = ON_SubD::EdgeTag::Crease, 
      then nullptr is returned.
  Returns:
    If the m_face_count = 2,
    m_edge_tag is smooth or x or passes the crease tag test,
    one of m_face2[0,1] points a face, then
    the neighboring face is returned.
    In any other case, ON_SubDFacePtr::Null is returned.
  */
  const ON_SubDFacePtr NeighborFacePtr(
    const ON_SubDFace* face,
    bool bStopAtCrease
    ) const;



  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::Smooth.
    bEdgeTagXresult if m_edge_tag is ON_SubD::EdgeTag::X.
    False in all other cases.
  */
  bool IsSmooth(
    bool bEdgeTagXresult
    ) const;

  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::Crease.
    bEdgeTagXresult if m_edge_tag is ON_SubD::EdgeTag::X.
    False in all other cases.
  */
  bool IsCrease(
    bool bEdgeTagXresult
    ) const;

  /*
  Returns:
    0: end vertices are not tagged as darts
    1: one end vertex is tagged as a dart.
    2: both end vertices are tagged as a darts.
  */
  unsigned int DartCount() const;

  /*
  Returns:
    bitwise or of applicable ON_ComponentAttributes::EdgeFlags values.
  */
  unsigned int EdgeFlags() const;

  /*
  Parameters:
    subdivision_point_type - [in]
      Selects subdivision algorithm. Must be either
      ON_SubD::SubDType::TriLoopWarren or ON_SubD::SubDType::QuadCatmullClark.
    bUseSavedSubdivisionPoint - [in]
      If there is a saved subdivision point and bUseSavedSubdivisionPoint
      is true, then the saved value is returned.
    subdivision_point - [out]
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    ON_SubD::SubDType subdivision_point_type,
    bool bUseSavedSubdivisionPoint,
    double subdivision_point[3]
    ) const;

  /*
  Parameters:
    edge_vertex_index - [in]
      0 or 1
    edge_ptr0 - [out]
    edge_ptr1 - [out]
      Crease edges that bound the sector containing this edge.
      The direction value of the edge pointer identifies the end
      of the sector boundary edge this->at m_vertex[edge_vertex_index].
  Returns:
    Number of faces in the sector.
  */
  unsigned int GetSectorBoundaryEdges(
    unsigned int edge_vertex_index,
    ON_SubDEdgePtr* edge_ptr0,
    ON_SubDEdgePtr* edge_ptr1
    ) const;

  /*
  Returns:
    0: m_vertex[0] is tagged and m_vertex[1] is not tagged.
    1: m_vertex[0] is tagged and m_vertex[1] is not tagged.
    2: m_vertex[0] and m_vertex[1] are both tagged.
    3: neither m_vertex[0] nor m_vertex[1] is tagged.
  */
  unsigned int TaggedEndIndex() const;

private:
  static unsigned int GetFacePointSum(
    const ON_SubDFace* face,
    const ON_SubDEdge* edge,
    double* facePsum // sum of face vertex points not on the edge
    );

private:
  friend class ON_SubDArchiveIdMap;
  void CopyFrom(
    const ON_SubDEdge* src,
    bool bReverseEdge,
    bool bCopyVertexArray,
    bool bCopyFaceArray
    );
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFace
//
class ON_SUBD_CLASS ON_SubDFace : public ON_SubDComponentBase
{
public:
  static const ON_SubDFace Empty;

  bool Write (
    class ON_BinaryArchive& archive
    ) const;

  static bool Read (
    class ON_BinaryArchive& archive,
    class ON_SubD& subd,
    class ON_SubDFace*& face
    );

  /*
  Description:
    Apply a tranxfomration matrix to vertex geometry information.

  Parameters:
    bTransformationSavedSubdivisionPoint - [in]
      If the transformation is being applied to every vertex, edge and 
      face in every level of a subdivision object, and the transformation
      is an isometry (rotation, translation, ...), a uniform scale, or a 
      composition of these types, then set 
      bTransformationSavedSubdivisionPoint = true to apply the
      transformation to saved subdivision and saved limit point information.
      In all other cases, set bTransformationSavedSubdivisionPoint = false
      and any saved subdivision points or saved limit points will be
      deleted.  When in doubt, pass false.

    xform - [in]
  */
  bool Transform(
    bool bTransformationSavedSubdivisionPoint,
    const class ON_Xform& xform
    );

  ON_BoundingBox ControlNetBoundingBox() const;
  ON_BoundingBox LimitSurfaceBoundingBox(
    const ON_SubD& subd
    ) const;


  ON_COMPONENT_INDEX ComponentIndex() const;
  ON_SubDComponentPtr ComponentPtr() const;

  /*
  Description:
    Call this function if the face is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void FaceModifiedNofification() const;


public:
  const class ON_SubDFace* m_prev_face = nullptr;  // linked list of faces on this level
  const class ON_SubDFace* m_next_face = nullptr;  // linked list of faces on this level

public:
  unsigned int m_zero_face_id = 0;   // id of level zero face
  unsigned int m_parent_face_id = 0; // id of previous level face

private:
  unsigned int m_reserved = 0; // id of previous level face

public:
  // Array of m_edge_count edges that form the boundary of the face.
  // The edges are in ordered to form a continuous loop.
  //
  //  The first four are in m_edge4[0], ..., m_edge4[3].
  //  When m_edge_count > 4, the fifth and additional edges
  //  are in m_edgex[0], ..., m_edgex[m_edge_count-5];
  //
  //  The value of ON_SubDEdgePtr.EdgeDirection() is 0 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  agrees with the face's boundary orientation.
  //
  //  The value of ON_SubDEdgePtr.EdgeDirection() is 1 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  is opposited the face's boundary orientation.
  static const unsigned int MaximumEdgeCount;
  unsigned short m_edge_count = 0;
  unsigned short m_edgex_capacity = 0;

  ON_SubDEdgePtr m_edge4[4];
  ON_SubDEdgePtr* m_edgex = nullptr;

public:
  unsigned int EdgeCount() const;

  ON_SubDEdgePtr EdgePtr(
    unsigned int i
    ) const;

  ON_SubDEdgePtr EdgePtr(
    const class ON_SubDEdge* e
    ) const;

  const class ON_SubDVertex* Vertex(
    unsigned int i
    ) const;

  unsigned int VertexIndex(
    const ON_SubDVertex* vertex
    ) const;

  const class ON_SubDEdge* Edge(
    unsigned int i
    ) const;

  ON__UINT_PTR EdgeDirection(
    unsigned int i
    ) const;

  unsigned int EdgeArrayIndex(
    const ON_SubDEdge* e
    ) const;

  /*
  Description:
    Expert user tool to remove an edge from the face's edge array.
  Remarks:
    Does not modify the edge.  If the face is referenced in the edge's face array,
    then the face must be removed from the edge's face array.
  */
  bool RemoveEdgeFromArray(
    const ON_SubDEdge* e
    );

  /*
  Description:
    Expert user tool to remove an edge from the face's edge array.
  Remarks:
    Does not modify the edge.  If the face is referenced in the edge's face array,
    then the face must be removed from the edge's face array.
  */
  bool RemoveEdgeFromArray(
    unsigned int i,
    ON_SubDEdgePtr& removed_edge
    );

  /*
  Description:
    Expert user tool to replace one edge with another in the face's edge array.
  Parameters:
    edge_to_remove - [in]
    edge_to_insert - [in]
     The inserted edge is assigned the same boundary orientation as the
     removed edge.
  Remarks:
    Does not modify the edge. The corresponding reference to this face must
    be removed from the first edge and added to the second edge.
  */
  bool ReplaceEdgeInArray(
    unsigned int fei0,
    ON_SubDEdge* edge_to_remove,
    ON_SubDEdge* edge_to_insert
  );

  /*
  Description:
    Expert user tool to replace one edge with another in the face's edge array.
  Parameters:
    edge_to_remove - [in]
    edge_to_insert - [in]
     The inserted edge is assigned the same boundary orientation specified
     in edgeptr_to_insert.
  Remarks:
    Does not modify the edge. The corresponding reference to this face must
    be removed from the first edge and added to the second edge.
  */
  bool ReplaceEdgeInArray(
    unsigned int fei0,
    ON_SubDEdge* edge_to_remove,
    ON_SubDEdgePtr edgeptr_to_insert
  );

  const ON_SubDEdge* PrevEdge(
    const ON_SubDEdge* edge
    ) const;

  const ON_SubDEdge* NextEdge(
    const ON_SubDEdge* edge
    ) const;

  unsigned int PrevEdgeArrayIndex(
    unsigned int edge_array_index
    ) const;

  unsigned int NextEdgeArrayIndex(
    unsigned int edge_array_index
    ) const;

  /*
  Description:
    If the face is a quad, get the opposite corner vertex.
  Parameters:
    vertex - [in]
      a vertex on this face.
  Returns:
    If the face is a quad and vertex is a vertex of the face, then
    the vertex on the opposite corner is returned.
    Otherwise, nullptr is returned.
  */
  const ON_SubDVertex* QuadOppositeVertex(
    const ON_SubDVertex* vertex
    ) const;

  /*
  Description:
    If the face is a quad, get  the opposite side edge.
  Parameters:
    edge - [in]
      an edge on this face.
  Returns:
    If the face is a quad and edge is an edge of the face, then
    the edge on the opposite side is returned.
    Otherwise, nullptr is returned.
  */
  const ON_SubDEdge* QuadOppositeEdge(
    const ON_SubDEdge* edge
    ) const;

  ///*
  //Parameters:
  //  subd_type - [in]
  //  bTestFaces - [in]
  //    If true, then false is returned if any neighboring face is not
  //    a quad (ccquad subdivision type) or tri (lwtri subdivsion type).
  //*/
  //bool IsOrdinary(
  //  ON_SubD::SubDType subd_type,
  //  bool bTestFaces
  //  ) const;

  /*
  Parameters:
    subdivision_point_type - [in]
      Selects subdivision algorithm. Must be either
      ON_SubD::SubDType::TriLoopWarren or ON_SubD::SubDType::QuadCatmullClark.
    bUseSavedSubdivisionPoint - [in]
      If there is a saved subdivision point and bUseSavedSubdivisionPoint
      is true, then the saved value is returned.
    subdivision_point - [out]
      The average of the face vertex locations.
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    ON_SubD::SubDType subdivision_point_type,
    bool bUseSavedSubdivisionPoint,
    double subdivision_point[3]
    ) const;

  /*
  Description:
    Reverse the order and orientation of the edges that form
    the boundary of this face.
  */
  bool ReverseEdgeList();

  /*
  Description:
    Get the bicubic b-spline control points for the limit surface.
    The corresponding knots are uniform.
  Parameters:
    vertex - [in]
    limit_surface_cv_stride0 - [int]
    limit_surface_cv_stride1 - [out]
    limit_surface_cv - [out]
      control points for a cubic spline surface
        CV[i][j][k] = limit_surface_cv[i*limit_bspline_cv_stride0 + j*limit_bspline_cv_stride1 + k]
        0 <= i < 4, 0 <= j < 4, 0 <= k < 3
  Returns:
    true if successful
    false if the limit surface for this face is not a cubic surface
  Remarks:
    The knots for the bicubic b-spline surface are uniform.
  */
  bool GetQuadLimitSurface(
    size_t limit_surface_cv_stride0,
    size_t limit_surface_cv_stride1,
    double* limit_surface_cv
    ) const;

  bool GetQuadLimitSurface(
    class ON_NurbsSurface& limit_surface
    ) const;

  bool GetQuadLimitSurface(
    class ON_BezierSurface& limit_surface
    ) const;

private:
  friend  class ON_SubDArchiveIdMap;
  void CopyFrom(
    const ON_SubDFace* src,
    bool bCopyEdgeArray
    );
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertexArray
//
class ON_SUBD_CLASS ON_SubDVertexArray
{
public:
  ON_SubDVertexArray(
    const ON_SubD& subd
    );
  ON_SubDVertexArray() = default;
  ON_SubDVertexArray(const ON_SubDVertexArray&) = default;
  ON_SubDVertexArray& operator=(const ON_SubDVertexArray&) = default;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDVertexArray(ON_SubDVertexArray&&) ON_NOEXCEPT;

  // rvalue copy operator-=
  ON_SubDVertexArray& operator=(ON_SubDVertexArray&&);
#endif

  const ON_SubD& SubD() const
  {
    return m_subd;
  }

  unsigned int VertexCount() const
  {
    return m_vertex_count;
  }

  const class ON_SubDVertex* operator[](unsigned int i) const
  {
    return (i < m_vertex_count) ? m_a[i] : nullptr;
  }
  
private:
  ON_SubD m_subd;
  const class ON_SubDVertex*const* m_a = nullptr;
  unsigned int m_vertex_count = 0;

#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< const class ON_SubDVertex* > m_sp;
#pragma ON_PRAGMA_WARNING_POP
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdgeArray
//
class ON_SUBD_CLASS ON_SubDEdgeArray
{
public:
  ON_SubDEdgeArray(
    const ON_SubD& subd
    );
  ON_SubDEdgeArray() = default;
  ON_SubDEdgeArray(const ON_SubDEdgeArray&) = default;
  ON_SubDEdgeArray& operator=(const ON_SubDEdgeArray&) = default;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDEdgeArray(ON_SubDEdgeArray&&) ON_NOEXCEPT;

  // rvalue copy operator-=
  ON_SubDEdgeArray& operator=(ON_SubDEdgeArray&&);
#endif

  const ON_SubD& SubD() const
  {
    return m_subd;
  }

  unsigned int EdgeCount() const
  {
    return m_edge_count;
  }

  const class ON_SubDEdge* operator[](unsigned int i) const
  {
    return (i < m_edge_count) ? m_a[i] : nullptr;
  }

private:
  ON_SubD m_subd;
  const class ON_SubDEdge*const* m_a = nullptr;
  unsigned int m_edge_count = 0;

#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< const class ON_SubDEdge* > m_sp;
#pragma ON_PRAGMA_WARNING_POP
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFaceArray
//
class ON_SUBD_CLASS ON_SubDFaceArray
{
public:
  ON_SubDFaceArray(
    const ON_SubD& subd
    );
  ON_SubDFaceArray() = default;
  ON_SubDFaceArray(const ON_SubDFaceArray&) = default;
  ON_SubDFaceArray& operator=(const ON_SubDFaceArray&) = default;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDFaceArray(ON_SubDFaceArray&&) ON_NOEXCEPT;

  // rvalue copy operator-=
  ON_SubDFaceArray& operator=(ON_SubDFaceArray&&);
#endif

  const ON_SubD& SubD() const
  {
    return m_subd;
  }

  unsigned int FaceCount() const
  {
    return m_face_count;
  }

  const class ON_SubDFace* operator[](unsigned int i) const
  {
    return (i < m_face_count) ? m_a[i] : nullptr;
  }

private:
  ON_SubD m_subd;
  const class ON_SubDFace*const* m_a = nullptr;
  unsigned int m_face_count = 0;
  
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< const class ON_SubDFace* > m_sp;
#pragma ON_PRAGMA_WARNING_POP
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertexIterator
//
class ON_SUBD_CLASS ON_SubDVertexIterator
{
public:
  // The ON_SubD member function
  // ON_SubDVertexIterator ON_SubD::VertexIterator(subd_level_index)
  // is the best way to get a vertex iterator.
  ON_SubDVertexIterator(
    const class ON_SubD& subd
    );
  ON_SubDVertexIterator(
    const class ON_SubDRef& subd_ref
    );

  // Construct and interator that iterates over a single vertex.
  ON_SubDVertexIterator(
    const class ON_SubD& subd,
    const class ON_SubDVertex& vertex
    );

  // Construct and interator that iterates over a single vertex.
  ON_SubDVertexIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDVertex& vertex
    );

  // Construct and interator that iterates over the vertices of an edge.
  ON_SubDVertexIterator(
    const class ON_SubD& subd,
    const class ON_SubDEdge& edge
    );

  // Construct and interator that iterates over the vertices of an edge.
  ON_SubDVertexIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDEdge& edge
    );

  // Construct and interator that iterates over the vertices of a face.
  ON_SubDVertexIterator(
    const class ON_SubD& subd,
    const class ON_SubDFace& face
    );

  // Construct and interator that iterates over the vertices of a face.
  ON_SubDVertexIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDFace& face
    );

  ON_SubDVertexIterator() = default;
  ON_SubDVertexIterator(const ON_SubDVertexIterator&) = default;
  ON_SubDVertexIterator& operator=(const ON_SubDVertexIterator&) = default;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDVertexIterator( ON_SubDVertexIterator&& ) ON_NOEXCEPT;
  // rvalue assignment operator
  ON_SubDVertexIterator& operator=( ON_SubDVertexIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class ON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class ON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Current vertex.
  Remarks:
    operator++ and NextVertex() behave differently.
  */
  const class ON_SubDVertex* operator++()
  {
    const class ON_SubDVertex* v = m_v_current;
    NextVertex();
    return v;
  }

  /*
  Return:
    Number of vertices this iterator will iterate through.
  */
  unsigned int VertexCount() const
  {
    return m_vertex_count;
  }

  /*
  Return:
    Interator index of the current vertex.
  */
  unsigned int CurrentVertexIndex() const
  {
    return m_vertex_index;
  }

  /*
  Description:
  Set the iterator to the beginning of the vertex list.
  Returns:
  First vertex in the list.
  */
  const class ON_SubDVertex* FirstVertex()
  {
    m_vertex_index = 0;
    return (m_v_current = m_v_first);
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Next vertex.
  Remarks:
    operator++ and NextVertex() behave differently.
  */
  const class ON_SubDVertex* NextVertex()
  {
    m_vertex_index++;
    if (m_vertex_index < m_vertex_count)
    {
      if (0 == m_component_ptr.m_ptr)
      {
        if (nullptr != m_v_current)
          m_v_current = m_v_current->m_next_vertex;
      }
      else
      {
        const ON_SubDEdge* edge = m_component_ptr.Edge();
        if (nullptr != edge)
        {
          m_v_current = edge->Vertex(m_vertex_index);
        }
        else
        {
          const ON_SubDFace* face = m_component_ptr.Face();
          if (nullptr != face)
            m_v_current = face->Vertex(m_vertex_index);
          else
            m_v_current = nullptr;
        }
      }
    }
    else
    {
      m_vertex_index = m_vertex_count;
      m_v_current = nullptr;
    }
    return m_v_current;
  }

  /*
  Returns:
  Current vertex;
  */
  const class ON_SubDVertex* CurrentVertex() const
  {
    return m_v_current;
  }

  /*
  Description:
  Set the iterator to the end of the vertex list.
  Returns:
  Last vertex in the list.
  */
  const class ON_SubDVertex* LastVertex()
  {
    m_vertex_index = (m_vertex_count > 0) ? (m_vertex_count - 1) : 0;
    return (m_v_current = m_v_last);
  }

private:
  void Internal_Init(
    const ON_SubDRef& subd_ref,
    unsigned int vertex_count,
    const ON_SubDVertex* first,
    const ON_SubDVertex* last,
    ON_SubDComponentPtr component_ptr
  );
  ON_SubDRef m_subd_ref;
  const ON_SubDVertex* m_v_first = nullptr;
  const ON_SubDVertex* m_v_last = nullptr;
  const ON_SubDVertex* m_v_current = nullptr;
  unsigned int m_vertex_index = 0;
  unsigned int m_vertex_count = 0;
  ON_SubDComponentPtr m_component_ptr = ON_SubDComponentPtr::Null;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdgeIterator
//
class ON_SUBD_CLASS ON_SubDEdgeIterator
{
public:
  // The ON_SubD member function
  // ON_SubDEdgeIterator ON_SubD::EdgeIterator()
  // is the best way to get an edge iterator from an ON_SubD.
  ON_SubDEdgeIterator(
    const class ON_SubD& subd
    );

  // The ON_SubDRef member function
  // ON_SubDEdgeIterator ON_SubDRef::EdgeIterator()
  // is the best way to get an edge iterator from an ON_SubDRef.
  ON_SubDEdgeIterator(
    const class ON_SubDRef& subd_ref
    );

  // Construct and interator that iterates over a single edge.
  ON_SubDEdgeIterator(
    const class ON_SubD& subd,
    const class ON_SubDEdge& edge
    );

  // Construct and interator that iterates over a single edge.
  ON_SubDEdgeIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDEdge& edge
    );

  // Construct and interator that iterates over the edges of a vertex.
  ON_SubDEdgeIterator(
    const class ON_SubD& subd,
    const class ON_SubDVertex& vertex
    );

  // Construct and interator that iterates over the edges of a vertex.
  ON_SubDEdgeIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDVertex& vertex
    );

  // Construct and interator that iterates over the edges of a face.
  ON_SubDEdgeIterator(
    const class ON_SubD& subd,
    const class ON_SubDFace& face
    );

  // Construct and interator that iterates over the edges of a face.
  ON_SubDEdgeIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDFace& face
    );

  ON_SubDEdgeIterator() = default;
  ON_SubDEdgeIterator(const ON_SubDEdgeIterator&) = default;
  ON_SubDEdgeIterator& operator=(const ON_SubDEdgeIterator&) = default;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDEdgeIterator( ON_SubDEdgeIterator&& ) ON_NOEXCEPT;
  // rvalue assignment operator
  ON_SubDEdgeIterator& operator=( ON_SubDEdgeIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class ON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class ON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Current edge.
  Remarks:
    operator++ and NextEdge() behave differently.
  */
  const class ON_SubDEdge* operator++()
  {
    const class ON_SubDEdge* e = m_e_current;
    NextEdge();
    return e;
  }

  /*
  Return:
    Number of edges this iterator will iterate through.
  */
  unsigned int EdgeCount() const
  {
    return m_edge_count;
  }

  /*
  Return:
    Interator index of the current edge.
  */
  unsigned int CurrentEdgeIndex() const
  {
    return m_edge_index;
  }

  /*
  Description:
    Set the iterator to the beginning of the edge list.
  Returns:
    First edge in the list.
  */
  const class ON_SubDEdge* FirstEdge()
  {
    m_edge_index = 0;
    return m_e_current = m_e_first;
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Next edge.
  Remarks:
    operator++ and NextEdge() behave differently.
  */
  const class ON_SubDEdge* NextEdge()
  {
    m_edge_index++;
    if (m_edge_index < m_edge_count)
    {
      if (0 == m_component_ptr.m_ptr)
      {
        if (nullptr != m_e_current)
          m_e_current = m_e_current->m_next_edge;
      }
      else
      {
        const ON_SubDVertex* vertex = m_component_ptr.Vertex();
        if (nullptr != vertex)
        {
          m_e_current = vertex->Edge(m_edge_index);
        }
        else
        {
          const ON_SubDFace* face = m_component_ptr.Face();
          if (nullptr != face)
            m_e_current = face->Edge(m_edge_index);
          else
            m_e_current = nullptr;
        }
      }
    }
    else
    {
      m_edge_index = m_edge_count;
      m_e_current = nullptr;
    }
    return m_e_current;
  }

  /*
  Returns:
  Current edge;
  */
  const class ON_SubDEdge* CurrentEdge() const
  {
    return m_e_current;
  }

  /*
  Description:
  Set the iterator to the end of the edge list.
  Returns:
  Last edge in the list.
  */
  const class ON_SubDEdge* LastEdge()
  {
    m_edge_index = (m_edge_count > 0) ? (m_edge_count - 1) : 0;
    return m_e_current = m_e_last;
  }

private:
  void Internal_Init(
    const ON_SubDRef& subd_ref,
    unsigned int edge_count,
    const ON_SubDEdge* first,
    const ON_SubDEdge* last,
    ON_SubDComponentPtr component_ptr
  );
  ON_SubDRef m_subd_ref;
  const ON_SubDEdge* m_e_first = nullptr;
  const ON_SubDEdge* m_e_last = nullptr;
  const ON_SubDEdge* m_e_current = nullptr;
  unsigned int m_edge_index = 0;
  unsigned int m_edge_count = 0;
  ON_SubDComponentPtr m_component_ptr = ON_SubDComponentPtr::Null;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFaceIterator
//
class ON_SUBD_CLASS ON_SubDFaceIterator
{
public:
  // The ON_SubD member function
  //   ON_SubDFaceIterator ON_SubD::FaceIterator()
  // is the best way to get a face iterator from an ON_SubD.
  ON_SubDFaceIterator(
    const class ON_SubD& subd
    );

  // The ON_SubDRef member function
  //   ON_SubDFaceIterator ON_SubDRef::FaceIterator()
  // is the best way to get a face iterator from an ON_SubDRef.
  ON_SubDFaceIterator(
    const class ON_SubDRef& subd_ref
    );

  // Construct and interator that iterates over the single face.
  ON_SubDFaceIterator(
    const class ON_SubD& subd,
    const class ON_SubDFace& face
    );

  // Construct and interator that iterates over the single face.
  ON_SubDFaceIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDFace& face
    );

  // Construct and interator that iterates over the faces of a vertex.
  ON_SubDFaceIterator(
    const class ON_SubD& subd,
    const class ON_SubDVertex& vertex
    );

  // Construct and interator that iterates over the faces of a vertex.
  ON_SubDFaceIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDVertex& vertex
    );

  // Construct and interator that iterates over the faces of an edge.
  ON_SubDFaceIterator(
    const class ON_SubD& subd,
    const class ON_SubDEdge& edge
    );

  // Construct and interator that iterates over the faces of an edge.
  ON_SubDFaceIterator(
    const class ON_SubDRef& subd_ref,
    const class ON_SubDEdge& edge
    );

  ON_SubDFaceIterator() = default;
  ON_SubDFaceIterator(const ON_SubDFaceIterator&) = default;
  ON_SubDFaceIterator& operator=(const ON_SubDFaceIterator&) = default;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDFaceIterator( ON_SubDFaceIterator&& ) ON_NOEXCEPT;
  // rvalue assignment operator
  ON_SubDFaceIterator& operator=( ON_SubDFaceIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class ON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class ON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Description:
    Returns the current face and increment the iterator.
  Returns:
    Current face.
  Remarks:
    operator++ and NextFace() behave differently.
  */
  const class ON_SubDFace* operator++()
  {
    const class ON_SubDFace* f = m_face_current;
    NextFace();
    return f;
  }

  /*
  Return:
    Number of faces this iterator will iterate through.
  */
  unsigned int FaceCount() const
  {
    return m_face_count;
  }

  /*
  Return:
    Interator index of the current face.
  */
  unsigned int CurrentFaceIndex() const
  {
    return m_face_index;
  }


  /*
  Description:
    Set the iterator to the beginning of the face list.
  Returns:
    First face in the list.
  */
  const class ON_SubDFace* FirstFace()
  {
    m_face_index = 0;
    return (m_face_current = m_face_first);
  }

  /*
  Description:
    Returns the next face and incrments the iterator.
  Returns:
    Next face.
  Remarks:
    operator++ and NextFace() behave differently.
  */
  const class ON_SubDFace* NextFace()
  {
    m_face_index++;
    if (m_face_index < m_face_count)
    {
      if (0 == m_component_ptr.m_ptr)
      {
        if (nullptr != m_face_current)
          m_face_current = m_face_current->m_next_face;
      }
      else
      {
        const ON_SubDVertex* vertex = m_component_ptr.Vertex();
        if (nullptr != vertex)
        {
          m_face_current = vertex->Face(m_face_index);
        }
        else
        {
          const ON_SubDEdge* edge = m_component_ptr.Edge();
          if (nullptr != edge)
            m_face_current = edge->Face(m_face_index);
          else
            m_face_current = nullptr;
        }
      }
    }
    else
    {
      m_face_index = m_face_count;
      m_face_current = nullptr;
    }
    return m_face_current;
  }

  /*
  Returns:
    Current face;
  */
  const class ON_SubDFace* CurrentFace() const
  {
    return m_face_current;
  }

  /*
  Description:
    Set the iterator to the end of the face list.
  Returns:
    Last face in the list.
  */
  const class ON_SubDFace* LastFace()
  {
    m_face_index = (m_face_count > 0) ? (m_face_count - 1) : 0;
    return (m_face_current = m_face_last);
  }

  unsigned int LimitSurfaceMeshFragmentCount(
    ON_SubD::FacetType facet_type
    ) const;

private:
  void Internal_Init(
    const ON_SubDRef& subd_ref,
    unsigned int face_count,
    const ON_SubDFace* first,
    const ON_SubDFace* last,
    ON_SubDComponentPtr component_ptr
  );
  ON_SubDRef m_subd_ref;
  const ON_SubDFace* m_face_first = nullptr;
  const ON_SubDFace* m_face_last = nullptr;
  const ON_SubDFace* m_face_current = nullptr;
  unsigned int m_face_index = 0;
  unsigned int m_face_count = 0;
  ON_SubDComponentPtr m_component_ptr = ON_SubDComponentPtr::Null;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentIterator
//
class ON_SUBD_CLASS ON_SubDComponentIterator
{
public:
  static const ON_SubDComponentIterator Empty;

  // The ON_SubD member function
  //   ON_SubDComponentIterator ON_SubD::ComponentIterator(subd_level_index)
  //   is the best way to get a component iterator for a subd level.
  ON_SubDComponentIterator(
    const class ON_SubD& subd
    );
  ON_SubDComponentIterator(
    const class ON_SubDRef& subd_ref
    );

  ON_SubDComponentIterator() = default;
  ON_SubDComponentIterator(const ON_SubDComponentIterator&) = default;
  ON_SubDComponentIterator& operator=(const ON_SubDComponentIterator&) = default;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDComponentIterator( ON_SubDComponentIterator&& ) ON_NOEXCEPT;
  // rvalue assignment operator
  ON_SubDComponentIterator& operator=( ON_SubDComponentIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class ON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class ON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Returns:
  The subD level for this iterator.
  */
  unsigned int SubDLevel() const
  {
    return m_subd_level;
  }

  /*
  Description:
    Returns the current component and increment the iterator.
  Returns:
    Current component.
  Remarks:
    operator++ and NextComponent() behave differently.
  */
  const class ON_SubDComponentPtr operator++()
  {
    const class ON_SubDComponentPtr cptr = m_cptr_current;
    NextComponent();
    return cptr;
  }

  /*
  Description:
    Set the iterator to the beginning of the component list.
  Returns:
    First component in the list.
  */
  const class ON_SubDComponentPtr FirstComponent();

  /*
  Description:
    Returns the next component and incrments the iterator.
  Returns:
    Next component.
  Remarks:
    operator++ and NextComponent() behave differently.
  */
  const class ON_SubDComponentPtr NextComponent();  

  /*
  Returns:
    Current component;
  */
  const class ON_SubDComponentPtr CurrentComponent() const
  {
    return m_cptr_current;
  }

  /*
  Description:
    Set the iterator to the end of the component list.
  Returns:
    Last component in the list.
  */
  const class ON_SubDComponentPtr LastComponent();

private:
  ON_SubDRef m_subd_ref;
  unsigned int m_subd_level = 0;
  const ON_SubDVertex* m_vertex_first = nullptr;
  const ON_SubDVertex* m_vertex_last = nullptr;
  const ON_SubDEdge* m_edge_first = nullptr;
  const ON_SubDEdge* m_edge_last = nullptr;
  const ON_SubDFace* m_face_first = nullptr;
  const ON_SubDFace* m_face_last = nullptr;
  ON_SubDComponentPtr m_cptr_current = ON_SubDComponentPtr::Null;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDSectorIterator
//

class ON_SUBD_CLASS ON_SubDSectorIterator
{
public:
  static const ON_SubDSectorIterator Empty;

  ON_SubDSectorIterator() = default;
  ~ON_SubDSectorIterator() = default;
  ON_SubDSectorIterator(const ON_SubDSectorIterator&) = default;
  ON_SubDSectorIterator& operator=(const ON_SubDSectorIterator&) = default;

  /*
  Parameters:
    center_vertex - [in]
      The vertex on initial_face that will be iterated around.
      center_vertex->Face(0) is used to select the sector.
  Returns:
    If input is valid, a pointer to the center vertex is returned.
    Otherwise, nullptr is returned.
  */
  const ON_SubDVertex* Initialize(
    const ON_SubDVertex* center_vertex
    );

  /*
  Parameters:
    initial_face - [in]
    iterator_orientation - [in]
      0: (more common) 
        "next" means counter-clockwise with respect to the orientation of initial_face
      1: (less common)
        "next" means clockwise with respect to the orientation of initial_face
    center_vertex - [in]
      The vertex on initial_face that will be iterated around.
  Returns:
    If input is valid, a pointer to the center vertex is returned.
    Otherwise, nullptr is returned.
  */
  const ON_SubDVertex* Initialize(
    const ON_SubDFace* initial_face,
    ON__UINT_PTR iterator_orientation,
    const ON_SubDVertex* center_vertex
    );

  /*
  Parameters:
    initial_face - [in]
    iterator_orientation - [in]
      0: (more common) 
        "next" means counter-clockwise with respect to the orientation of initial_face
      1: (less common)
        "next" means clockwise with respect to the orientation of initial_face
    face_vertex_index - [in]
      initial_face->Vertex(face_vertex_index) is the center vertex
      that will be iterated around.
  Returns:
    If input is valid, a pointer to the center vertex is returned.
    Otherwise, nullptr is returned.
  */
  const ON_SubDVertex* Initialize(
    const ON_SubDFace* initial_face,
    ON__UINT_PTR iterator_orientation,
    unsigned int face_vertex_index
    );

  bool InitializeToCurrentFace();

  void Initialize();

  /*
  Description:
  The current ring index reports the total increment from the
  start to the current state.  It can be positive or negative.
  */
  int CurrentRingIndex() const;

  const ON_SubDVertex* CenterVertex() const;

  const ON_SubDFace* InitialFace() const;

  unsigned int InitialFaceCenterVertexIndex() const;

  const ON_SubDFace* CurrentFace() const;

  unsigned int CurrentFaceDirection() const;

  ON_SubDFacePtr CurrentFacePtr() const;

  unsigned int CurrentFaceCenterVertexIndex() const;


  /*
  Parameters:
    face_side_index - [in]
      0: Return the edge entering the center vertex.
      1: Return the edge leaving the center vertex.
  Returns:
    The requested edge.
  */
  ON_SubDEdgePtr CurrentEdgePtr(
    unsigned int face_side_index
    ) const;

  /*
  Description:

                CurrentEdge(1)
                    |
                    |
         NextFace() | CurrentFace()
                    |
                    |
                    *------------- CurrentEdge(0)
                        PrevFace()

    The asterisk (*) marks the center vertex.
    The diagram is With respect to the initial iterator orientation.

  Parameters:
    face_side_index - [in]
      CurrentEdge(0) = edge on the clockwise (PrevFace) side of the current face
      CurrentEdge(1) = edge on the counterclockwise (NextFace) side of the current face
      The directions "counterclockwise" and "clockwise" are with respect to the
      initial iterator orientation.
  Returns:
    The requested edge or nullptr if the iterator is not initialized, 
    has terminated, or is not valid.
  */
  const ON_SubDEdge* CurrentEdge(
    unsigned int face_side_index
    ) const;

  ON__UINT_PTR CurrentEdgeDirection(
    unsigned int face_side_index
    ) const;

  /*
  Returns:
    The vertex on CurrentEdge(face_side_index) that is opposite
    the central vertex.
  */
  const ON_SubDVertex* CurrentEdgeRingVertex(
    unsigned int face_side_index
    ) const;

  /*
  Description:
    Advance the "current" face to the "next" face in the ring
    by "hopping across" CurrentEdge(1).  
    
    If the current face is not reversed (1 == CurrentFaceDirection),
    then this rotation is counter-clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then this rotation is clockwise with respect to 
    the current face's orientation.
  Parameters:
    bStopAtCrease - [in]
      If true and CurrentEdge(1) is tagged ON_SubD:EdgeTag::crease,
      iteration terminates.
  Returns:
    When the input CurrentEdge(1) has exactly two faces and
    is not tagged as a crease or bStopAtCrease is false, the 
    face on the other side of CurrentEdge(1) becomes the new
    current face and a pointer to this face is returned.
  Remarks:
    Identical to calling IncrementFace(+1,bStopAtCrease);
  */
  const ON_SubDFace* NextFace(
    bool bStopAtCrease
    );

  /*
  Description:
    Advance the "current" face to the "previous" face in the ring
    by "hopping across" CurrentEdge(0).  
    
    If the current face is not reversed (0 == CurrentFaceDirection),
    then this rotation is clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then this rotation is counter-clockwise with respect to 
    the current face's orientation.
  Parameters:
    bStopAtCrease - [in]
      If true and CurrentEdge(0) is tagged ON_SubD:EdgeTag::crease,
      iteration terminates.
  Returns:
    When the input CurrentEdge(0) has exactly two faces and
    is not tagged as a crease or bStopAtCrease is false, the 
    face on the other side of CurrentEdge(0) becomes the new
    current face and a pointer to this face is returned.
    In all other cases, nullptr is returned
  Remarks:
    Identical to calling IncrementFace(-1,bStopAtCrease);
  */
  const ON_SubDFace* PrevFace(
    bool bStopAtCrease
    );

  /*
  Description:
    Advance the "current" face by "hopping across" the edge
    CurrentEdge((increment_direction>0) ? 1 : 0).
    
    If the current face is not reversed (0 == CurrentFaceDirection),
    then increment_direction>0 rotates counter-clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then increment_direction>0 rotates clockwise with respect to 
    the current face's orientation.
  Parameters:
    increment_direction - [in]
      > 0 advance the "current" face to next face
      <= 0 advance the "current" face to previous face
    bStopAtCrease - [in]
      If true and the input value of CurrentEdge((increment_direction>0) ? 1 : 0) 
      is tagged as ON_SubD:EdgeTag::crease, iteration terminates. 
      When iteration terminates at a crease, 
      CurrentFace() becomes nullptr
      CurrentEdge((increment_direction>0) ? 1 : 0) becomes nullptr
      CurrentEdge((increment_direction>0) ? 0 : 1) points at the crease
      and nullptr returned.
  Returns:
    nullptr if iteration terminates.
  */
  const ON_SubDFace* IncrementFace(
    int increment_direction,
    bool bStopAtCrease
    );

  /*
  Description:
    Increment the iterator until it reaches a face with 
    a crease 
  Parameters:
    increment_direction - [in]
    > 0 advance next until CurrentEdge(1) is a crease.
    <= 0 advance previous until CurrentEdge(0) is a crease.
  Returns:
    nullptr - the sector has no creases.
    not nullptr - incremented to a crease
  */
  const ON_SubDFace* IncrementToCrease(
    int increment_direction
    );

  /*
  Description:
    Reset iterator to initial face.
  */
  const ON_SubDFace* FirstFace();

  bool IsValid() const;

private:
  const ON_SubDVertex* m_center_vertex = nullptr;
  const ON_SubDFace* m_initial_face = nullptr;
  const ON_SubDFace* m_current_face = nullptr;

  // m_current_eptr[0].Edge() = "prev" side edge
  // m_current_eptr[1].Edge() = "next" side edge
  // When m_current_eptr[i].Edge() is not null, 
  // center vertex = m_current_eptr[i].Edge()->m_vertex[m_current_eptr[i].Direction()]
  ON_SubDEdgePtr m_current_eptr[2]; // default = { ON_SubDEdgePtr::Null, ON_SubDEdgePtr::Null };

  unsigned int m_initial_fvi = 0;
  unsigned int m_current_fvi = 0;
  unsigned int m_current_fei[2];  // default = { 0, 0 }; // "prev" and "next"

  // m_initial_face_dir 
  // 0: "next" means clockwise with respect to the initial face's orientation.
  // 1: "next" means counter-clockwise with respect to the initial face's orientation.
  unsigned int m_initial_face_dir = 0;

  // m_current_face_dir 
  // 0: "next" means clockwise with respect to the initial face's orientation.
  // 1: "next" means counter-clockwise with respect to the initial face's orientation.
  // When the subd faces around the center vertex are consistently oriented, 
  // m_current_face_dir is always equal to m_initial_face_dir.
  unsigned int m_current_face_dir = 0;

  int m_current_ring_index = 0;
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFaceEdgeIterator
//
class ON_SUBD_CLASS ON_SubDFaceEdgeIterator
{
public:
  ON_SubDFaceEdgeIterator();

  /*
  Description:
    Construct an iterator for going around the edges on a face.
  Parameters:
    face - [in]
    first_edge - [in]
      starting edge for the iterator or nullptr to start at face->Edge(0).
  */
  ON_SubDFaceEdgeIterator(
    const ON_SubDFace* face
    );

  ON_SubDFaceEdgeIterator(
    const ON_SubDFace* face,
    const ON_SubDEdge* first_edge
    );


  /*
  Description:
    Initialize an iterator for going around the edges on a face.
  Parameters:
    face - [in]
    first_edge - [in]
      starting edge for the iterator or nullptr to start at face->Edge(0).
  */
  unsigned int Initialize(
    const ON_SubDFace* face
    );

  unsigned int Initialize(
    const ON_SubDFace* face,
    const ON_SubDEdge* first_edge
    );

  unsigned int EdgeCount() const;

  /*
  Returns:
    Resets the iterator and returns the first edge.
  */
  const ON_SubDEdge* FirstEdge();

  /*
  Description:
    Increments the iterator and returns the edge.
  */
  const ON_SubDEdge* NextEdge();

  /*
  Description:
    Decrements the iterator and returns the edge.
  */
  const ON_SubDEdge* PrevEdge();

  /*
  Returns:
  Current edge.
  */
  const ON_SubDEdge* CurrentEdge() const;

  unsigned int FirstEdgeIndex() const;

  unsigned int CurrentEdgeIndex() const;

private:
  const ON_SubDFace* m_face;
  unsigned int m_edge_count;
  unsigned int m_edge_index0;
  unsigned int m_edge_index;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFromMeshOptions
//
class ON_SUBD_CLASS ON_SubDFromMeshOptions
{
public:

  // Default construction is identical to ON_SubDFromMeshOptions::Smooth.
  ON_SubDFromMeshOptions() = default;
  ~ON_SubDFromMeshOptions() = default;
  ON_SubDFromMeshOptions(const ON_SubDFromMeshOptions&) = default;
  ON_SubDFromMeshOptions& operator=(const ON_SubDFromMeshOptions&) = default;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Crease options
  //

  // No interior creases and no corners.
  static const ON_SubDFromMeshOptions Smooth;
  
  // Create an interior sub-D crease along coincident input mesh edges
  // where the vertex normal directions at one end differ by at 
  // least 30 degrees.
  static const ON_SubDFromMeshOptions InteriorCreaseAtMeshCrease;

  // Create an interior sub-D crease along all coincident input mesh edges.
  static const ON_SubDFromMeshOptions InteriorCreaseAtMeshEdge;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Custom interior crease options
  //
#pragma region RH_C_SHARED_ENUM [SubD::InteriorCreaseOption] [Rhino.Geometry.SubD.InteriorCreaseOption] [nested:byte]
  ///<summary>
  ///Defines how interior creases are treated.
  ///</summary>
  enum class InteriorCreaseOption : unsigned char
  {
    ///<summary>The interior creases option is not defined.</summary>
    Unset = 0,

    ///<summary>No interior creases.</summary>
    None = 1,

    ///<summary>An interior subd crease will appear along coincident
    ///mesh edges where the angle between coindident vertex
    ///normals &gt;= MinimumCreaseAngleRadians().</summary>
    AtMeshCrease = 2,

    ///<summary>An interior subd crease will appear all coincident mesh edges.
    ///Input mesh vertex normals are ignored.</summary>
    AtMeshEdge = 3
  };
#pragma endregion

  static ON_SubDFromMeshOptions::InteriorCreaseOption InteriorCreaseOptionFromUnsigned(
    unsigned int interior_crease_option_as_unsigned
    );

  /*
  Parameters:
    interior_crease_option - [in]
  */
  void SetInteriorCreaseOption(
    ON_SubDFromMeshOptions::InteriorCreaseOption interior_crease_option
    );

  /*
  Returns:
    The interior crease option.
  */
  ON_SubDFromMeshOptions::InteriorCreaseOption InteriorCreaseTest() const;


  /*
  Description:
    When the interior crease option is 
    ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshCreases,
    the value of MinimumCreaseAngleRadians() determines which
    coincident input mesh edges generate sub-D creases.

    If the input mesh has vertex normals, and the angle between 
    vertex normals is > MinimumCreaseAngleRadians() at an end 
    of a coincident input mesh edge, the the correspondeing sub-D 
    edge will be a crease.

  Parameters:
    minimum_crease_angle_radians - [in]
      >= 0.0 and < ON_PI
  */
  void SetMinimumCreaseAngleRadians(
    double minimum_crease_angle_radians
    );

  /*
  Description:
    When the interior crease option is 
    ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshCreases,
    the value of MinimumCreaseAngleRadians() determines which
    coincident input mesh edges generate sub-D creases.

    If the input mesh has vertex normals, and the angle between 
    vertex normals is > MinimumCreaseAngleRadians() at an end 
    of a coincident input mesh edge, the the correspondeing sub-D 
    edge will be a crease.
  Returns:
    Current value of 
  */
  double MinimumCreaseAngleRadians() const;

  /*
  Description:
    Copy all interior crease option settings from source_options to this.
  Parameters:
    source_options - [in]
  Returns:
    The currently selected interior crease option.
  */
  ON_SubDFromMeshOptions::InteriorCreaseOption CopyInteriorCreaseTest(
    ON_SubDFromMeshOptions source_options
    );


  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Convex corner options
  //

  // Look for convex corners at sub-D vertices with 2 edges
  // that have an included angle <= 90 degrees.
  static const ON_SubDFromMeshOptions ConvexCornerAtMeshCorner;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Custom convex corner options
  //
#pragma region RH_C_SHARED_ENUM [SubD::ConvexCornerOption] [Rhino.Geometry.SubD.ConvexCornerOption] [nested:byte]
  ///<summary>
  ///Defines how convex corners are treated.
  ///</summary>
  enum class ConvexCornerOption : unsigned char
  {
    ///<summary>The option is not set.</summary>
    Unset = 0,

    ///<summary>No convex coners.</summary>
    None = 1,

    ///<summary>A convext subd corner will appear at input mesh/ boundary vertices 
    /// where the corner angle &lt;= MaximumConvexCornerAngleRadians() and
    /// the number of edges the end at the vertex is &lt;= MaximumConvexCornerEdgeCount().
    ///</summary>
    AtMeshCorner = 2
  };
#pragma endregion

  static ON_SubDFromMeshOptions::ConvexCornerOption ConvexCornerOptionFromUnsigned(
    unsigned int convex_corner_option_as_unsigned
    );

  /*
  Parameters:
    convex_corner_option - [in]
  */
  void SetConvexCornerOption(
    ON_SubDFromMeshOptions::ConvexCornerOption convex_corner_option
    );

  /*
  Returns:
    The currently selected convex corner option.
  */
  ON_SubDFromMeshOptions::ConvexCornerOption ConvexCornerTest() const;

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Parameters:
    maximum_convex_corner_edge_count - [in]
  */
  void SetMaximumConvexCornerEdgeCount(
    unsigned int maximum_convex_corner_edge_count
    );

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Returns:
    The maximum number of edges at a convex corner vertex.
  */
  unsigned int MaximumConvexCornerEdgeCount() const;

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Parameters:
    maximum_convex_corner_angle_radians - [in]
      > 0.0 and < ON_PI
  */
  void SetMaximumConvexCornerAngleRadians(
    double maximum_convex_corner_angle_radians
    );

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Returns:
    The maximum corner angle.
  */
  double MaximumConvexCornerAngleRadians() const;
  
  /*
  Description:
    Copy all convex corner option settings from source_options to this.
  Parameters:
    source_options - [in]
  Returns:
    The currently selected convex corner option.
  */
  ON_SubDFromMeshOptions::ConvexCornerOption CopyConvexCornerTest(
    ON_SubDFromMeshOptions source_parameters
    );

  
  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Sub-D type option
  //
  ON_SubD::SubDType SubDType() const;

  void SetSubDType(
    ON_SubD::SubDType subd_type
    );

private:
  ON_SubD::SubDType m_subd_type = ON_SubD::SubDType::Unset;
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;

  ON_SubDFromMeshOptions::InteriorCreaseOption m_interior_crease_option = ON_SubDFromMeshOptions::InteriorCreaseOption::None;
  ON_SubDFromMeshOptions::ConvexCornerOption m_convex_corner_option = ON_SubDFromMeshOptions::ConvexCornerOption::None;
  unsigned short m_maximum_convex_corner_edge_count = 2U;

  double m_minimum_crease_angle_radians = ON_PI/6.0; // 30 degrees in radians
  double m_maximum_convex_corner_angle_radians = 0.501*ON_PI; // 90 degrees (+ a smidge) in radians
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentRef
//
//  Used when an ON_SubD vertex, edge or face needs to be sent around as
//  a piece of ON_Geometry.
//
class ON_SUBD_CLASS ON_SubDComponentRef : public ON_Geometry
{
  ON_OBJECT_DECLARE(ON_SubDComponentRef);
public:
  static const ON_SubDComponentRef Empty;

  ON_SubDComponentRef() = default;
  ~ON_SubDComponentRef() = default;
  ON_SubDComponentRef(const ON_SubDComponentRef&) ON_NOEXCEPT;
  ON_SubDComponentRef& operator=(const ON_SubDComponentRef&);

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDComponentRef( ON_SubDComponentRef&& ) ON_NOEXCEPT;

  // The rvalue assignment operator calls ON_Object::operator=(ON_Object&&)
  // which could throw exceptions.  See the implementation of
  // ON_Object::operator=(ON_Object&&) for details.
  ON_SubDComponentRef& operator=( ON_SubDComponentRef&& );
#endif
  
  /*
  Parameters:
    subd_ref - [in]
    component_index - [in]
    bLimitSurface - [in]
      true - the reference is to the limit surface location
      false - the reference is to the control net location
  */
  static ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    ON_COMPONENT_INDEX component_index,
    ON_SubDComponentLocation component_location
    );

  static ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    ON_SubDComponentPtr component_ptr,
    ON_SubDComponentLocation component_location
    );

  static ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    const class ON_SubDVertex* vertex,
    ON_SubDComponentLocation component_location
    );

  static ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    const class ON_SubDEdge* edge,
    ON_SubDComponentLocation component_location
    );

  static ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    const class ON_SubDFace* face,
    ON_SubDComponentLocation component_location
    );

  void Clear();

  ON_SubDRef SubDRef() const;

  const class ON_SubD& SubD() const;

  ON_COMPONENT_INDEX ComponentIndex() const override;

  ON_SubDComponentPtr ComponentPtr() const;

  ON_SubDComponentLocation ComponentLocation() const;

  const class ON_SubDVertex* Vertex() const;

  const class ON_SubDEdge* Edge() const;

  const class ON_SubDFace* Face() const;
  
private:
  ON_SubDRef m_subd_ref;
  ON_SubDComponentPtr m_component_ptr = ON_SubDComponentPtr::Null;
  ON_COMPONENT_INDEX m_component_index = ON_COMPONENT_INDEX::UnsetComponentIndex;
  ON_SubDComponentLocation m_component_location =  ON_SubDComponentLocation::Unset;

public:
  // overrides of virtual ON_Object functions
  bool IsValid( class ON_TextLog* text_log = nullptr ) const override;
  void Dump( ON_TextLog& ) const override;
  unsigned int SizeOf() const override;
  ON::object_type ObjectType() const override;

  // overrides of virtual ON_Geometry functions
  int Dimension() const override;

  // virtual ON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentPoint
//
//  Used in selection tests to return a point and parameters on a component.
//
class ON_SUBD_CLASS ON_SubDComponentPoint
{
public:
  static const ON_SubDComponentPoint Unset;

  ON_SubDComponentPoint() = default;
  ~ON_SubDComponentPoint() = default;
  ON_SubDComponentPoint(const ON_SubDComponentPoint&) = default;
  ON_SubDComponentPoint& operator=(const ON_SubDComponentPoint&) = default;

  /*
  Description:
    Compares the m_component_ptr values.  This function is useful for sorting
    arrays of ON_SubDComponentPoint values remove duplicates.
  */
  static int CompareComponentPtr(
    const ON_SubDComponentPoint* a,
    const ON_SubDComponentPoint* b
    );
  
  // m_component_ptr will be face, edge or vertex
  ON_SubDComponentPtr m_component_ptr = ON_SubDComponentPtr::Null;

  //// If the point is on a a face that does not have the ordinary number of 
  //// edges for the subdivision type, then m_face_corner_index identifies the
  //// subfragment corner.
  //unsigned int m_face_corner_index = ON_UNSET_UINT_INDEX;

  //// If m_level_index is ON_UNSET_UINT_INDEX, the point is on the limit surface.
  //// Otherwise the point is on the control net at the specified level.
  //unsigned int m_level_index = ON_UNSET_UINT_INDEX;

  ON_PickPoint m_pick_point = ON_PickPoint::Unset;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDMatrix
//
class ON_SUBD_CLASS ON_SubDMatrix
{
public:
  ON_SubDMatrix() = default;

  static const ON_SubDMatrix Empty;

  /*
  Description:
    Precise evaluation of cos(a) and cos(a) where a = i/n pi.
    These values are required for high qualitiy limit surface evaluation.
  Parameters:
    j - [in]
    n - [in]
    cos_theta - [out]
      cos(j/n pi)
    sin_theta - [out]
      sin(j/n pi)
  */
  static bool EvaluateCosAndSin(
    unsigned int j,
    unsigned int n,
    double* cos_theta,
    double* sin_theta
    );

  bool IsValid() const;

  bool IsValidPointRing(
    size_t point_ring_count,
    size_t point_ring_stride,
    const double* point_ring
    ) const;

  bool EvaluateSubdivisionPoint(
    unsigned int component_index,
    size_t point_ring_count,
    size_t point_ring_stride,
    const double* point_ring,
    double subd_point[3]
    ) const;

  bool EvaluateLimitPoint(
    size_t point_ring_count,
    size_t point_ring_stride,
    const double* point_ring,
    double limit_point[3],
    double limit_tangent1[3],
    double limit_tangent2[3],
    double limit_normal[3]
    ) const;

  bool EvaluateLimitPoint(
    size_t point_ring_count,
    size_t point_ring_stride,
    const double* point_ring,
    class ON_SubDSectorLimitPoint& limit_point
    ) const;

  /*
  Description:
    Get the subdivision matrix information for the case specified
    by the input parameters.  This information is retrieved from
    a cache.  In some cases, it will be calculated the first time
    it is needed.  
  Parameters:
    facet_type - [in]
    vertex_tag - [in]
    valence - [in]
      The input parameters identify the subdivision case.
  Remarks:
    Every member function of ON_SubDMatrix, including this one
    is thread safe.
  */
  static const ON_SubDMatrix& FromCache(
    ON_SubDSectorType sector_type
    );

  ON_SubDSectorType m_sector_type;

  unsigned int m_R = 0; // the matrix m_S is m_R x m_R (m_R = m_sector_type.PointRingCount())

  // The term "standard vertex ring points" is used below. 
  //
  // If "C" is an interior vertex (m_vertex_tag is smooth or dart), 
  // (E[0], ...., E[N-1]) is a radially sorted list of its edges, 
  // (F[0], ..., F[N-1]) is a radially sorted list of its faces,
  // and (P[0], ..., P[N-1]) is a list of the edge vertices opposite C,
  // E0type = smooth for a smooth vertex and crease for a dart vertex,
  // then C is "standard" if E[0] has type E0type, every other
  // edge E[i] is smooth, every outer vertex/ P[i] is smooth, and every 
  // face F[i] has the stadard facet type (tri or quad) for the subdivision
  // algorithm.
  //
  // If If "C" is a boundary vertex (m_vertex_tag is crease or corner), the conditions
  // listed above are satisified except 
  // E[0] and E[N-1] are tagged as crease edges, 
  // P[0] and P[N-1] are tagged as crease vertices (NOT corners), 
  // and there are N-2 faces,
  // then "C" is a standard boundary vertex.
  // 
  // If the facet type is triangle and C is a standard interior or boundary vertex,
  // then the "standard vertex ring" is the list of N+1 points 
  // (C, P[0], ...., P[N-1]).
  //
  // If the facet type is quad, and C is a standard interior vertex,
  // then the "standard vertex ring" is the list of 2*N+1 points 
  // (C, P[0], Q[0], ...., P[N-1], Q[N-1]), where Q[I] is the average of the
  // four corners of the quad F[i].
  //
  // If the facet type is quad, and C is a standard boundary vertex,
  // then the "standard vertex ring" is the list of 2*N points 
  // (C, P[0], Q[0], ...., P[N-1]).

  // m_S = R x R subdivision matrix
  // If (vertexR[0], ..., vertexR[R-1]) is a list of standard vertex ring points,
  // then then the location of the subdivided ring points
  // (vertexR1[0], ..., vertexR1[R-1]) can be calculated from m_S.
  // vertexR1[i] = m_S[i][0]*vertexR[0] + ... + m_S[i][R-1]*vertexR[R-1]
  const double* const* m_S = nullptr;

  // m_LP[] = limit point evaluation vector.
  // The array m_LP[] has m_R elements.
  // If (vertexR[0], ..., vertexR[R-1]) is a list of standard vertex ring points,
  // then Limit point = m_LP[0]*vertexR[0] + ... + m_LP[R-1]*vertexR[R-1].
  // m_LP is the eigenvector of Transpose(m_S) with eigenvalue = 1.
  // Length(m_LP) = 1.
  const double* m_LP = nullptr;

  // m_L1 and m_L2 = tangent space evaluation vectors.
  // The arrays m_L1[] and m_L2[] have m_R elements.
  // If (vertexR[0], ..., vertexR[R-1]) is a list of standard vertex ring points,
  // then the two vectors 
  // V1 = m_L1[0]*vertexR[0] + ... + m_L1[R-1]*vertexR[R-1].
  // V2 = m_L2[0]*vertexR[0] + ... + m_L2[R-1]*vertexR[R-1].
  // span the tangent plane and 
  // N = V1 x V2 is perpindicular to the limit tangent plane.
  // In general and almost always in practice, V1 and V2 are not unit vectors
  // and it is best to noramalize V1 and V2 before taking the cross product.
  // m_L1 and m_L2 are subdominant eigenvectors of Transpose(m_S).
  // When the subdominant eigenvalue has geometric multiplicity 2, 
  // m_L1 and m_L2 span the same space as m_E1 and m_E2. 
  // The values stored in m_L1 and m_L2 are chosen to provide accurate
  // evaluation. In come common cases m_L1 and m_L2 are equal to m_E1 and m_E2,
  // but not in all cases.
  const double* m_L1 = nullptr;
  const double* m_L2 = nullptr;

  /*
  Description:
    Set the values in this ON_SubDMatrix so the information 
    can be used to evaluate the case identified by the input
    parameters.
  Parameters:
    facet_type - [in]
    vertex_tag - [in]
    sector_edge_count - [in]
      The input parameters identify the subdivision case.
  Returns:
    R > 0: Success.  The matrix is R x R.
    0: Failure.
  */
  unsigned int SetFromSectorType(
    ON_SubDSectorType sector_type
    );

  /*
  Returns:
    ON_UNSET_VALUE - serious error
    >= 0:
      Maximum value of numbers that should be zero in and ideal world.  
      When the matrices, eigenvalues and eigenvectors are correctly calculated, 
      this returned value is in the range from 1e-16 to 5e-13 as valence goes 
      from 3 to 100.
      For valences < 100, if a value larger than 1.0e-12 occurs, there is a bug in the code.
  */
  double TestMatrix() const;

  /*
  Description:
    Run evaluation tests on this subdivision case.
  Returns:
    >= 0.0: Test passed. Maximum deviation found in any test is returned.
    ON_UNSET_VALUE: Test failed.
  */
  double TestEvaluation() const;

  /*
  Description:
    Run evaluation tests on a range of subdivision cases.
  Parameters:
    sector_type - [in]
      If ON_SubDSectorType::Empty, then all supported sector types types are tested.
    minimum_sector_face_count - [in]
      If 0, then testing begins at ON_SubDSectorType::MinimumSectorFaceCount(vertex_tag)
      when testing vertex_tag types
    maximum_sector_face_count - [in]
      If 0, then testing stops at ON_SubD::maximum_evaluation_valence.
    text_log - [out]
      If not nullptr, then a brief written report is printed for each test case.
  Returns:
    >= 0.0: Test passed.  Maximum deviation found in any test is returned.
    ON_UNSET_VALUE: Test failed.    
  */
  static double TestEvaluation(
    ON_SubDSectorType sector_type,
    unsigned int minimum_sector_face_count,
    unsigned int maximum_sector_face_count,
    ON_TextLog* text_log
    );

  double TestComponentRing(
    size_t component_ring_count,
    const ON_SubDComponentPtr* component_ring
    ) const;

  /*
  Description:
    Test cached subdivision matrix on sector identified by sit.
  Parameters:
    subd_type - [in]
    subd_recursion_count - [in]
      number of times to subdivide
    sit - [in]
      vertex to subdivide
    component_ring - [out]
    subd_points - [out]
    limit_point - [out]
    limit_tangent0 - [out]
    limit_tangent1 - [out]
    limit_normal - [out]
  */
  static double TestEvaluation(
    ON_SubD::SubDType subd_type,
    const unsigned int subd_recursion_count,
    ON_SubDSectorIterator sit,
    ON_SimpleArray<ON_SubDComponentPtr>& component_ring,
    ON_SimpleArray< ON_3dPoint >& subd_points,
    class ON_SubDSectorLimitPoint& limit_point
    );

private:
  unsigned int m__max_R = 0;
  ON_Matrix m__S; // m_S matrix memory
  ON_SimpleArray<double> m__buffer; // m_LP, m_L1, m_L2, m_E1, m_E2 memory  
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubD_FixedSizeHeap
//

class ON_SUBD_CLASS ON_SubD_FixedSizeHeap
{
private:
  static unsigned int m__sn_factory;

public:
  // The serial number is used for debugging purposes.
  const unsigned int m_sn = ++m__sn_factory;

public:
  ON_SubD_FixedSizeHeap() = default;
  ~ON_SubD_FixedSizeHeap();

  /*
  Description:
    Reserve enough room to accomodate a face
    with one extraordinary vertex. 
  Parameters:
    subd_type - [in]
    extraordinary_valence - [in]
  */
  bool ReserveSubDWorkspace(
    ON_SubD::SubDType subd_type,
    unsigned int extraordinary_valence
    );

  bool ReserveSubDWorkspace(
    size_t vertex_capacity,
    size_t edge_capacity,
    size_t face_capacity,
    size_t array_capacity
    );

  /*
  Description:
    Reset this ON_SubD_FixedSizeHeap so it can be used again.
  */
  void Reset();

  /*
  Description:
    Deallocate all reserved heap.
  */
  void Destroy();

  bool InUse() const;

   ON_SubDVertex* AllocateVertex(
    const double vertexP[3],
    unsigned int edge_capacity,
    unsigned int face_capacity
    );

  ON_SubDVertex* AllocateVertex(
    const ON_SubDVertex* vertex0,
    ON_SubD::SubDType subd_type,
    bool bUseSavedSubdivisionPoint,
    unsigned int edge_capacity,
    unsigned int face_capacity
    );

  ON_SubDVertex* AllocateVertex(
    const ON_SubDEdge* edge0,
    ON_SubD::SubDType subd_type,
    bool bUseSavedSubdivisionPoint,
    unsigned int edge_capacity,
    unsigned int face_capacity
    );

  ON_SubDVertex* AllocateVertex(
    const ON_SubDFace* face0,
    ON_SubD::SubDType subd_type,
    bool bUseSavedSubdivisionPoint,
    unsigned int edge_capacity,
    unsigned int face_capacity
    );

  /*
  Parameters:
    v0 - [in]
    v0_sector_weight - [in]
      If v0 null or ON_SubD::VertexTag::Smooth == v0->m_vertex_tag, and v1 is null or tagged,
      then m_sector_weight[0] is set to v0_sector_weight.
      In all other cases the value of v0_sector_weight is ignored and m_sector_weight[0]
      is set to ON_SubDSectorType::IgnoredSectorWeight.
    v1 - [in]
    v1_sector_weight - [in]
      If v1 null or ON_SubD::VertexTag::Smooth == v1->m_vertex_tag, and v0 is null or tagged,
      then m_sector_weight[1] is set to v1_sector_weight.
      In all other cases the value of v1_sector_weight is ignored and m_sector_weight[1]
      is set to ON_SubDSectorType::IgnoredSectorWeight.
  Returns:
    An edge.
    The vertex parameter information is used to set the ON_SubDEdge.m_vertex[] 
    and ON_SubDEdge.m_sector_weight[] values.
    If v0 and v1 are not null and both are tagged, then ON_SubDEdge.m_edge_tag is 
    set to ON_SubD::EdgeTag::Crease.  
    In all other cases, ON_SubDEdge.m_edge_tag is set to ON_SubD::EdgeTag::Smooth.
    If v0 or v1 is not null, then ON_SubDEdge.m_level is set to the
    maximum of v0->m_level or v1->m_level.
  */
  ON_SubDEdge* AllocateEdge(
    ON_SubDVertex* v0,
    double v0_sector_weight,
    ON_SubDVertex* v1,
    double v1_sector_weight
    );

  /*
  Returns:
    A face with all field values zero (same values as ON_SubDEdge::Face), except ON_SubDFace.m_id.
  */
  ON_SubDFace* AllocateFace(
    unsigned int zero_face_id,
    unsigned int parent_face_id
    );

  ON_SubDFace* AllocateQuad(
    unsigned int zero_face_id,
    unsigned int parent_face_id,
    const ON_SubDEdgePtr eptrs[4]
    );

  ON_SubDFace* AllocateTri(
    unsigned int zero_face_id,
    unsigned int parent_face_id,
    const ON_SubDEdgePtr eptrs[3]
    );

  /*
  Parameters:
    capacity - [in]
      desired array size
    bZeroMemory - [in]
      If true, all array element values are zero.
      If false, array element values are undefined.
  Returns:
    An array of capacity ON__UINT_PTR pointers.
  */
  ON__UINT_PTR* AllocatePtrArray(
    unsigned int capacity,
    bool bZeroMemory
    );

  /*
  Description:
    Return the most recent array obtained from AllocatePtrArray().
    so it can be reused.  
  Returns:
    True: 
      Success.
    False: 
      Failure. The array was not the most recent array obtained
      from AllocatePtrArray().
  */
  bool ReturnPtrArray(
    unsigned int capacity,
    void* p
    );

private:
  //unsigned int m_max_valence = 0;
 
  ON_SubDVertex* m_v = nullptr;
  unsigned int m_v_capacity = 0;
  unsigned int m_v_index = 0;

  ON_SubDEdge* m_e = nullptr;
  unsigned int m_e_capacity = 0;
  unsigned int m_e_index = 0;

  ON_SubDFace* m_f = nullptr;
  unsigned int m_f_capacity = 0;
  unsigned int m_f_index = 0;

  ON__UINT_PTR* m_p = nullptr;
  unsigned int m_p_capacity = 0;
  unsigned int m_p_index = 0;

private:
  // copies not supported
  ON_SubD_FixedSizeHeap(const ON_SubD_FixedSizeHeap&) = delete;
  ON_SubD_FixedSizeHeap& operator=(const ON_SubD_FixedSizeHeap&) = delete;
};

#if defined(ON_COMPILING_OPENNURBS)
/*
The ON_SubDAsUserData class is used to attach a subd to it proxy mesh
when writing V6 files in commerical rhino.
*/
class ON_SubDMeshProxyUserData : public ON_UserData
{
public:
  /*
  Returns:
    A pointer to a mesh that now manages subd.
  */
  static ON_Mesh* MeshProxyFromSubD(
    const ON_SubD* subd
  );

  /*
  Returns:
    A pointer to a subd and deletes mesh.
  */
  static ON_SubD* SubDFromMeshProxy(
    const ON_Mesh* mesh
  );

  /*
  Returns:
    A pointer to a subd and deletes mesh.
  */
  static bool IsSubDMeshProxy(
    const ON_Mesh* mesh
  );

  static const ON_SubDDisplayParameters MeshProxyDisplayParameters();

private:
  ON_OBJECT_DECLARE(ON_SubDMeshProxyUserData);

public:
  ON_SubDMeshProxyUserData();
  ~ON_SubDMeshProxyUserData();
  ON_SubDMeshProxyUserData(const ON_SubDMeshProxyUserData&);
  ON_SubDMeshProxyUserData& operator=(const ON_SubDMeshProxyUserData&);
  
private:
  // ON_Object overrides
  bool Write(ON_BinaryArchive& archive) const override;
  bool Read(ON_BinaryArchive& archive) override;
  bool IsValid( class ON_TextLog* text_log = nullptr ) const override;

  bool ParentMeshValid() const;

private:
  // ON_UserData overrides
  bool GetDescription(ON_wString& description) override;
  bool WriteToArchive(
    const class ON_BinaryArchive& archive,
    const class ON_Object* parent_object
  ) const override;
 
private:
  // The subd
  mutable ON_SubD* m_subd = nullptr;

private:
  // information used to see if the parent mesh has been modified.
  mutable unsigned int m_mesh_face_count = 0;
  mutable unsigned int m_mesh_vertex_count = 0;
  mutable ON_SHA1_Hash m_mesh_face_array_sha1 = ON_SHA1_Hash::EmptyContentHash;
  mutable ON_SHA1_Hash m_mesh_vertex_array_sha1 = ON_SHA1_Hash::EmptyContentHash;

private:
  void Internal_CopyFrom(const ON_SubDMeshProxyUserData& src);
  void Internal_Destroy();
  static const bool Internal_MeshHasFaces(const ON_Mesh* mesh);
  static const ON_SHA1_Hash Internal_FaceSHA1(const ON_Mesh* mesh);
  static const ON_SHA1_Hash Internal_VertexSHA1(const ON_Mesh* mesh);
};
#endif

#endif

#endif
