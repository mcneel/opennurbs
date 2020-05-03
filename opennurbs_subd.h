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
class ON_CLASS ON_CensusCounter
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

class ON_CLASS ON_SubDCensusCounter
{
public:
  ON_SubDCensusCounter() ON_NOEXCEPT;
  ~ON_SubDCensusCounter() ON_NOEXCEPT;
  ON_SubDCensusCounter(const ON_SubDCensusCounter&) ON_NOEXCEPT;
  ON_SubDCensusCounter& operator=(const ON_SubDCensusCounter&) = default;
  //ON_SubDCensusCounter( ON_SubDCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDCensusCounter& operator=( ON_SubDCensusCounter&& ) ON_NOEXCEPT;
};

class ON_CLASS ON_SubDRefCensusCounter
{
public:
  ON_SubDRefCensusCounter() ON_NOEXCEPT;
  ~ON_SubDRefCensusCounter() ON_NOEXCEPT;
  ON_SubDRefCensusCounter(const ON_SubDRefCensusCounter&) ON_NOEXCEPT;
  ON_SubDRefCensusCounter& operator=(const ON_SubDRefCensusCounter&) = default;
  //ON_SubDRefCensusCounter( ON_SubDRefCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDRefCensusCounter& operator=( ON_SubDRefCensusCounter&& ) ON_NOEXCEPT;
};


class ON_CLASS ON_SubDImpleCensusCounter
{
public:
  ON_SubDImpleCensusCounter() ON_NOEXCEPT;
  ~ON_SubDImpleCensusCounter() ON_NOEXCEPT;
  ON_SubDImpleCensusCounter(const ON_SubDImpleCensusCounter&) ON_NOEXCEPT;
  ON_SubDImpleCensusCounter& operator=(const ON_SubDImpleCensusCounter&) = default;
  //ON_SubDImplCensusCounter( ON_SubDImplCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDImplCensusCounter& operator=( ON_SubDImplCensusCounter&& ) ON_NOEXCEPT;
};

class ON_CLASS ON_SubDMeshCensusCounter
{
public:
  ON_SubDMeshCensusCounter() ON_NOEXCEPT;
  ~ON_SubDMeshCensusCounter() ON_NOEXCEPT;
  ON_SubDMeshCensusCounter(const ON_SubDMeshCensusCounter&) ON_NOEXCEPT;
  ON_SubDMeshCensusCounter& operator=(const ON_SubDMeshCensusCounter&) = default;
  //ON_SubDMeshCensusCounter( ON_SubDMeshCensusCounter&& ) ON_NOEXCEPT;
  //ON_SubDMeshCensusCounter& operator=( ON_SubDMeshCensusCounter&& ) ON_NOEXCEPT;
};


class ON_CLASS ON_SubDMeshCensusCounter
{
public:
  ON_SubDMeshCensusCounter() ON_NOEXCEPT;
  ~ON_SubDMeshCensusCounter() ON_NOEXCEPT;
  ON_SubDMeshCensusCounter(const ON_SubDMeshCensusCounter&) ON_NOEXCEPT;
  ON_SubDMeshCensusCounter& operator=(const ON_SubDMeshCensusCounter&) ON_NOEXCEPT;
  ON_SubDMeshCensusCounter( ON_SubDMeshCensusCounter&& ) ON_NOEXCEPT;
  ON_SubDMeshCensusCounter& operator=( ON_SubDMeshCensusCounter&& ) ON_NOEXCEPT;
};

#endif


////////////////////////////////////////////////////////////////
//
//   Definition of subdivision surface
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_SUBD_INC_)
#define OPENNURBS_SUBD_INC_

/// <summary>
/// ON_SubDTextureDomainType identifies the way face 2d texture coordinate domains are set.
/// </summary>
enum class ON_SubDTextureDomainType : unsigned char
{
  ///<summary>
  /// Texture domains are not set.
  ///</summary>
  Unset = 0,

  ///<summary>
  /// Each face texture domain is a unique rectangle of normlized texture space. 
  ///</summary>
  PerFace = 1,

  ///<summary>
  /// Each face texture domain is a unique rectangle of normlized texture space. 
  /// When possible, faces are partitioned into quad groups. Adjactent members
  /// of the group are assigned adjacent rectangles in texture space.
  ///</summary>
  Packed = 2,

  ///<summary>
  /// All face texture domain values are zero.
  ///</summary>
  Zero = 3,

  ///<summary>
  /// All face texture domain values are nan.
  ///</summary>
  Nan = 4,

  ///<summary>
  /// Code outside of opennurbs set the values. No other information is available.
  ///</summary>
  Custom = 7,
};

class ON_CLASS ON_SubDVertexPtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDVertexPtr::Null
  // or x = ON_SubDVertexPtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDVertexPtr Null;

  bool IsNull() const;
  bool IsNotNull() const;

  /*
  Returns:
    True if this vertex is active in its parent subd or other
    relevent context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;

  class ON_SubDVertex* Vertex() const;

  ON__UINT_PTR VertexDirection() const;

  const ON_ComponentStatus Status() const;

  static const ON_SubDVertexPtr Create(
    const class ON_SubDVertex* vertex
    );

  /*
  Parameters:
    vertex - [in]
    vertex_direction - [in]
      zero or one
  */
  static const ON_SubDVertexPtr Create(
    const class ON_SubDVertex* vertex,
    ON__UINT_PTR vertex_direction
    );

  static const ON_SubDVertexPtr Create(
    const class ON_SubDComponentPtr& vertex_component
    );

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;


  ON__UINT8 MarkBits() const;

  ON__UINT8 SetMarkBits(
    ON__UINT8 mark_bits
  ) const;

  ON__UINT8 ClearMarkBits() const;
};


#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_SubDVertexPtr>;
#endif

class ON_CLASS ON_SubDEdgePtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDEdgePtr::Null
  // or x = ON_SubDEdgePtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDEdgePtr Null;

  /*
  Returns:
    True if this->Edge() is nullptr.
  */
  bool IsNull() const;

  /*
  Returns:
    True if this->Edge() is not nullptr.
  */
  bool IsNotNull() const;


  /*
  Returns:
    True if this->Edge() is not nullptr and both vertex pointers are not null as well.
  */
  bool IsNotNullAndVerticesAreNotNull() const;

  /*
  Returns:
    True if this edge is active in its parent subd or other
    relevent context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;


  /*
  Returns:
    The ON_SubDEdge this points at.
  */
  class ON_SubDEdge* Edge() const;

  /*
  Returns:
    If Edge() is not nullptr, Edge()->m_id is returned.
    Otherwise, 0 is returned.
  */
  unsigned int EdgeId() const;
  
  /*
  Returns:
    If Edge() is not nullptr, Edge()->FaceCount() is returned.
    Otherwise, 0 is returned.
  */
  unsigned int EdgeFaceCount() const;
    
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsSmooth() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsSmooth() const;
    
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsCrease() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsCrease() const;
   
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsHardCrease() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsHardCrease() const;
   
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsDartCrease() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsDartCrease() const;

  /*
  Returns:
    0: this ON_SubDEdgePtr is oriented from Edge()->Vertex(0) to Edge()->Vertex(1).
    1: this ON_SubDEdgePtr is oriented from Edge()->Vertex(1) to Edge()->Vertex(0).
  */
  ON__UINT_PTR EdgeDirection() const;

  /*
  Parameters:
    relative_vertex_index - [in]
      0: return Edge()->Vertex(EdgeDirection())
      1: return Edge()->Vertex(1-EdgeDirection())
  Returns:
    The requested vertex with EdgeDirection() taken into account.
    nullptr if relative_vertex_index, Edge() is nullptr, or Edge()->Vertex() is nullptr.
  */
  const class ON_SubDVertex* RelativeVertex(
    int relative_vertex_index
  ) const;

  /*
  Parameters:
    relative_vertex_index - [in]
      0: return Edge()->Vertex(EdgeDirection())
      1: return Edge()->Vertex(1-EdgeDirection())
  Returns:
    The requested vertex control net point EdgeDirection() taken into account.
    ON_3dPoint::NanPoint if relative_vertex_index, Edge() is nullptr, or Edge()->Vertex() is nullptr.
  */
  const ON_3dPoint RelativeControlNetPoint(
    int relative_vertex_index
  ) const;

  const ON_Line RelativeControlNetLine() const;

  const ON_3dVector RelativeControlNetDirection() const;

  /*
  Parameters:
    relative_vertex_index - [in]
  Returns:
    If Edge() not nullptr, then
    If (relative_vertex_index = 0), returns Edge()->m_sector_coefficient(EdgeDirection())
    If (relative_vertex_index = 0), returns Edge()->m_sector_coefficient(1-EdgeDirection())
    Otherwise ON_SubDSectorType::ErrorSectorCoefficient is returned.
  Remarks:
    The name "sector coefficient" is used because is is a property of the
    vertex's sector (every edge in vertex sector has the same value at the tagged vertex).
    The sector coefficient does not change which a subdivision is applied.
  */
  double RelativeSectorCoefficient(
    int relative_vertex_index
    ) const;

  /*
  Returns:
    The vector from RelativeVertex(0)->ControlNetPoint() to RelativeVertex(1)->ControlNetPoint(),
    or ON_3dVector::NanVector if the relative vertex pointers are nullptr.
  */
  const ON_3dVector RelativeDirection() const;

  /*
  Returns:
    this->Edge()->m_status.
  */
  const ON_ComponentStatus Status() const;

  /*
  Returns:
    A ON_SubDEdgePtr pointing at the same edge with the direction reversed from this.
  */
  const ON_SubDEdgePtr Reversed() const;


  /*
  Parameters:
    edge - [in]
  Returns:
    An ON_SubDEdgePtr pointing at edge with direction = 0 (not reversed).
  */
  static const ON_SubDEdgePtr Create(
    const class ON_SubDEdge* edge
    );

  /*
  Parameters:
    edge - [in]
    direction - [in]
      0: not reversed
      1: reversed
  Returns:
    An ON_SubDEdgePtr pointing at edge with the specified direction.
  */
  static const ON_SubDEdgePtr Create(
    const class ON_SubDEdge* edge,
    ON__UINT_PTR direction
    );

  static const ON_SubDEdgePtr Create(
    const class ON_SubDComponentPtr& edge_component
    );

  /*
  Parameters:
    edge - [in]
    start_vertex - [in]
      One of the edge's vertices.
  Returns:
     An ON_SubDEdgePtr pointing at edge with RelativeVertex(0) = start_vertex.
  */
  static const ON_SubDEdgePtr CreateFromStartVertex(
    const class ON_SubDEdge* edge,
    const ON_SubDVertex* start_vertex
  );

  /*
  Parameters:
    edge - [in]
    end_vertex - [in]
      One of the edge's vertices.
  Returns:
     An ON_SubDEdgePtr pointing at edge with RelativeVertex(1) = end_vertex.
  */
  static const ON_SubDEdgePtr CreateFromEndVertex(
    const class ON_SubDEdge* edge,
    const ON_SubDVertex* end_vertex
  );


  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  ON__UINT8 MarkBits() const;

  ON__UINT8 SetMarkBits(
    ON__UINT8 mark_bits
  ) const;

  ON__UINT8 ClearMarkBits() const;
};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_SubDEdgePtr>;
#endif

class ON_CLASS ON_SubDFacePtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDFacePtr::Null
  // or x = ON_SubDFacePtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDFacePtr Null;

  bool IsNull() const;

  bool IsNotNull() const;

  /*
  Returns:
    True if this face is active in its parent subd or other
    relevent context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;


  class ON_SubDFace* Face() const;
  
  /*
  Returns:
    If Face() is not nullptr, Face()->m_id is returned.
    Otherwise, 0 is returned.
  */
  unsigned int FaceId() const;

  /*
  Returns:
    If Face() is not nullptr, Face()->EdgeCount() is returned.
    Otherwise, 0 is returned.
  */
  unsigned int FaceEdgeCount() const;
  
  ON__UINT_PTR FaceDirection() const;

  const ON_ComponentStatus Status() const;

  /*
  Returns:
    A ON_SubDFacePtr pointing at the same face with the direction reversed from this.
  */
  const ON_SubDFacePtr Reversed() const;

  static const ON_SubDFacePtr Create(
    const class ON_SubDFace* face,
    ON__UINT_PTR direction
    );

  static const ON_SubDFacePtr Create(
    const class ON_SubDComponentPtr& face_component
    );

  static int Compare(
    const ON_SubDFacePtr* lhs,
    const ON_SubDFacePtr* rhs
  );

  static int CompareFacePointer(
    const ON_SubDFacePtr* lhs,
    const ON_SubDFacePtr* rhs
  );

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  ON__UINT8 MarkBits() const;

  ON__UINT8 SetMarkBits(
    ON__UINT8 mark_bits
  ) const;

  ON__UINT8 ClearMarkBits() const;
};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_SubDFacePtr>;
#endif


class ON_CLASS ON_SubDComponentPtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDComponentPtr::Null
  // or x = ON_SubDComponentPtr::Create(...).
  ON__UINT_PTR m_ptr;

  static const ON_SubDComponentPtr Null; //  // nullptr, type = unset, mark = 0

  /// <summary>
  /// ON_SubDComponentPtr::Type identifies the type of subdivision component referenced by
  /// the ON_SubDComponentPtr.
  /// </summary>
  enum class Type : unsigned char
  {
    Unset = 0,
    Vertex = 2,
    Edge = 4,
    Face = 6
  };

  static ON_SubDComponentPtr::Type ComponentPtrTypeFromUnsigned(
    unsigned int component_pointer_type_as_unsigned
    );

   /*
   Description:
     ON_SubDComponentPtr::Type::Vertex 
     < ON_SubDComponentPtr::Type::Edge
     < ON_SubDComponentPtr::Type::Face
     < ON_SubDComponentPtr::Type::Unset
     < invalid
   */
  static int CompareComponentPtrType(
    ON_SubDComponentPtr::Type a,
    ON_SubDComponentPtr::Type b
    );

  static int CompareType(
    const ON_SubDComponentPtr* a,
    const ON_SubDComponentPtr* b
    );

  /*
  Description:
    Dictionary compares type and ComponentBase() pointer as an unsigned.
  */
  static int CompareComponent(
    const ON_SubDComponentPtr* a,
    const ON_SubDComponentPtr* b
  );

  /*
  Description:
    Dictionary compares type, ComponentBase() pointer as an unsigned,
    and ComponentDirection().
  */
  static int CompareComponentAndDirection(
    const ON_SubDComponentPtr* a,
    const ON_SubDComponentPtr* b
  );


  /*
  Returns:
    True if the ComponentBase() pointer is nullptr. Note that type and mark may be set.
  */
  bool IsNull() const;

  /*
  Returns:
    True if type is set and ComponentBase() pointer is not nullptr. Note that mark may be set as well.
  */
  bool IsNotNull() const;

  /*
  Returns:
    True if this component is active in its parent subd or other
    relevent context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;

  ON_SubDComponentPtr::Type ComponentType() const;

  class ON_SubDComponentBase* ComponentBase() const;
  class ON_SubDVertex* Vertex() const;
  class ON_SubDEdge* Edge() const;
  class ON_SubDFace* Face() const;

  const ON_SubDVertexPtr VertexPtr() const;
  const ON_SubDEdgePtr EdgePtr() const;
  const ON_SubDFacePtr FacePtr() const;

  unsigned int ComponentId() const;

  const ON_COMPONENT_INDEX ComponentIndex() const;

  const ON_3dPoint ControlNetCenterPoint() const;
  const ON_BoundingBox ControlNetBoundingBox() const;

  /*
  Returns:
    A value suitable for hash table used based on the component type and id.
  */
  ON__UINT16 Hash16FromTypeAndId() const;


  /*
  Returns:
    A value suitable for hash table used based on the value of ComponentBase().
  */
  ON__UINT32 Hash32FromPointer() const;

  /*
  Returns:
    0 or 1.
    A runtime bit property on this ON_SubDComponentPtr. 
    The use of this value varies depending on the context.
    Frequently, 0 means the referenced component is being used with its
    natural orientation and 1 means the referenced component is being used
    with the reverse of its natrual oreientation.
  */
  ON__UINT_PTR ComponentDirection() const;

  /*
  Returns:
    An ON_SubDComponentPtr referencing the same ON_SubDComponentBase
    with ON_SubDComponentPtr.ComponentDirection() = 0.
  */
  const ON_SubDComponentPtr ClearComponentDirection() const;

  /*
  Returns:
    An ON_SubDComponentPtr referencing the same ON_SubDComponentBase
    with ON_SubDComponentPtr.ComponentDirection() = 1.
  */
  const ON_SubDComponentPtr SetComponentDirection() const;

  /*
  Returns:
    An ON_SubDComponentPtr referencing the same ON_SubDComponentBase
    with ON_SubDComponentPtr.ComponentDirection() = 1.
  */
  const ON_SubDComponentPtr SetComponentDirection(ON__UINT_PTR dir) const;
   
  /*
  Returns:
    An ON_SubDComponentPtr referencing the same ON_SubDComponentBase
    with ComponentDirection() = 1 - this->ComponentDirection().
  */
  const ON_SubDComponentPtr Reversed() const;

  const ON_ComponentStatus Status() const;

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

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  ON__UINT8 MarkBits() const;

  ON__UINT8 SetMarkBits(
    ON__UINT8 mark_bits
  ) const;

  ON__UINT8 ClearMarkBits() const;

  static
  const ON_SubDComponentPtr CreateNull(
    ON_SubDComponentPtr::Type component_type,
    ON__UINT_PTR component_direction
    );

  static
  const ON_SubDComponentPtr Create(
    const class ON_SubDVertex* vertex
    );

  static
  const ON_SubDComponentPtr Create(
    const class ON_SubDEdge* edge
    );

  static
  const ON_SubDComponentPtr Create(
    const class ON_SubDFace* face
    );

  static
  const ON_SubDComponentPtr Create(
    const class ON_SubDVertex* vertex,
    ON__UINT_PTR vertex_direction
    );

  static
  const ON_SubDComponentPtr Create(
    const class ON_SubDEdge* edge,
    ON__UINT_PTR edge_direction
    );

  static
  const ON_SubDComponentPtr Create(
    const class ON_SubDFace* face,
    ON__UINT_PTR face_direction
    );
  
  static
  const ON_SubDComponentPtr Create(
    ON_SubDVertexPtr vertexptr
    );
  
  static
  const ON_SubDComponentPtr Create(
    ON_SubDEdgePtr edgeptr
    );

  static
  const ON_SubDComponentPtr Create(
    ON_SubDFacePtr faceptr
    );

  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const ON_wString ToString() const;

};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_SubDComponentPtr>;
#endif

class ON_CLASS ON_SubDComponentPtrPair
{
public:
  // For performance reasons, m_ptrpair is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = ON_SubDComponentPtrPair::Null
  // or x = ON_SubDComponentPtr::Create(first_ptr,second_ptr).
  ON_SubDComponentPtr m_pair[2];

public:
  static const ON_SubDComponentPtrPair Create(ON_SubDComponentPtr first_ptr, ON_SubDComponentPtr second_ptr);

  /*
  Description:
    Dictionary order compare using ON_SubDComponentPtr::CompareComponent() on each element.
  */
  static int CompareComponent(const ON_SubDComponentPtrPair * lhs, const ON_SubDComponentPtrPair * rhs);

  /*
  Description:
    Dictionary order compare using ON_SubDComponentPtr::CompareComponentAndDirection() on each element.
  */
  static int CompareComponentAndDirection(const ON_SubDComponentPtrPair * lhs, const ON_SubDComponentPtrPair * rhs);

  /*
  Description:
    Compare first pointer value.
  */
  static int CompareFirstPointer(const ON_SubDComponentPtrPair * lhs, const ON_SubDComponentPtrPair * rhs);

  /*
  Returns:
    A pair with components in the opposite order
  */
  const ON_SubDComponentPtrPair SwapPair() const;


  /*
  Returns:
    A pair with components reversed.
  */
  const ON_SubDComponentPtrPair ReversedPair() const;

  /*
  Returns:
    First ON_SubDComponentPt in the pair.
  */
  const ON_SubDComponentPtr First() const;

  /*
  Returns:
    Second ON_SubDComponentPt in the pair.
  */
  const ON_SubDComponentPtr Second() const;

  /*
  Returns:
    If both points have the same type, that type is returned.
    Otherwise ON_SubDComponentPtr::Type::Unset is returned.
  */
  ON_SubDComponentPtr::Type ComponentType() const;

  /*
    Returns FIrst().IsNull().
  */
  bool FirstIsNull() const;

  /*
    Returns Second().IsNull().
  */
  bool SecondIsNull() const;

  /*
    Returns First().IsNull() && Second().IsNull().
  */
  bool BothAreNull() const;
  
  /*
    Returns First().IsNotNull().
  */
  bool FirstIsNotNull() const;

  /*
    Returns Second().IsNotNull().
  */
  bool SecondIsNotNull() const;

  /*
    Returns FirstIsNotNull() && SecondIsNotNull().
  */
  bool BothAreNotNull() const;

public:
  const static ON_SubDComponentPtrPair Null;

};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_SubDComponentPtrPair>;
#endif

class ON_CLASS ON_SubDComponentPtrPairHashTable : ON_Hash32Table
{
public:
  ON_SubDComponentPtrPairHashTable();

  ON_SubDComponentPtrPairHashTable(const class ON_SubD& subd);
  
  ~ON_SubDComponentPtrPairHashTable() = default;
  
  bool AddComponentPair(
    ON_SubDComponentPtr first_component,
    ON_SubDComponentPtr second_component
  );

  bool AddVertexPair(
    const class ON_SubDVertex* first_v,
    const class ON_SubDVertex* second_v
  );

  bool AddEdgePair(
    const class ON_SubDEdge* first_e,
    const class ON_SubDEdge* second_e
  );

  bool AddEdgePair(
    const class ON_SubDEdge* first_e,
    const ON_SubDEdgePtr second_eptr
  );

  bool AddFacePair(
    const class ON_SubDFace* first_f,
    const class ON_SubDFace* second_f
  );

  const ON_SubDComponentPtrPair PairFromSecondComponentPtr(
    ON_SubDComponentPtr second_component
  );

  const ON_SubDComponentPtrPair PairFromSecondVertex(
    const class ON_SubDVertex* second_v
  );

  const ON_SubDComponentPtrPair PairFromSecondEdge(
    const class ON_SubDEdge* second_e
  );

private:
  ON_FixedSizePool m_pairs_fsp;

private:
  ON_SubDComponentPtrPairHashTable(const ON_SubDComponentPtrPairHashTable&) = delete;
  ON_SubDComponentPtrPairHashTable& operator=(const ON_SubDComponentPtrPairHashTable&) = delete;
};


class ON_CLASS ON_SubDVertexSurfacePointCoefficient
{
public: 
  ON_SubDVertexSurfacePointCoefficient() = default;
  ~ON_SubDVertexSurfacePointCoefficient() = default;
  ON_SubDVertexSurfacePointCoefficient(const ON_SubDVertexSurfacePointCoefficient&) = default;
  ON_SubDVertexSurfacePointCoefficient& operator=(const ON_SubDVertexSurfacePointCoefficient&) = default;

public:

  // ON_SubDVertexSurfacePointCoefficient::Zero.m_c = 0.0
  static const ON_SubDVertexSurfacePointCoefficient Zero;

  // ON_SubDVertexSurfacePointCoefficient::Nan.m_c = ON_DBL_QNAN
  static const ON_SubDVertexSurfacePointCoefficient Nan;

  // ON_SubDVertexSurfacePointCoefficient::Unset.m_c = ON_UNSET_VALUE
  static const ON_SubDVertexSurfacePointCoefficient Unset;

  static const ON_SubDVertexSurfacePointCoefficient Create(
    const ON_SubDVertex* limit_point_vertex,
    const ON_SubDVertex* ring_vertex,
    double x
  );

public: 
  static int CompareSurfacePointVertexId(
    const ON_SubDVertexSurfacePointCoefficient* lhs,
    const ON_SubDVertexSurfacePointCoefficient* rhs
  );

  static int CompareRingVertexId(
    const ON_SubDVertexSurfacePointCoefficient* lhs,
    const ON_SubDVertexSurfacePointCoefficient* rhs
  );

  static int CompareSurfacePointAndRingVertexId(
    const ON_SubDVertexSurfacePointCoefficient* lhs,
    const ON_SubDVertexSurfacePointCoefficient* rhs
  );

  static int CompareRingAndSurfacePointVertexId(
    const ON_SubDVertexSurfacePointCoefficient* lhs,
    const ON_SubDVertexSurfacePointCoefficient* rhs
  );


public:
  const ON_SubDVertex* m_limit_point_vertex = nullptr;
  const ON_SubDVertex* m_ring_vertex = nullptr;
  // The limit point of m_limit_point_vertex
  // = sum of m_c*m_ring_vertex->ControlNetPoint()
  // for every point in the ring of m_limit_point_vertex, including m_limit_point_vertex.
  double m_c = 0.0;

public:
  unsigned int SurfacePointVertexId() const;
  unsigned int RingVertexId() const;
  double Coefficient() const;
};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_SubDVertexSurfacePointCoefficient>;
#endif

class ON_CLASS ON_SubDComponentRegionIndex
{
public:
  ON_SubDComponentRegionIndex() = default;
  ~ON_SubDComponentRegionIndex() = default;
  ON_SubDComponentRegionIndex(const ON_SubDComponentRegionIndex&) = default;
  ON_SubDComponentRegionIndex& operator=(const ON_SubDComponentRegionIndex&) = default;

public:
  enum : unsigned short
  {
    /// Capacity of the m_index[] array;
    IndexCapacity = 9
  };

  // All values are zero
  static const ON_SubDComponentRegionIndex Zero;

  // All values are 0xFFFF
  static const ON_SubDComponentRegionIndex Unset;

  /*
  Description:
    Compares subdivision counts. If the counts are the same, compares m_indices[].    
  */
  static int Compare(
    const ON_SubDComponentRegionIndex* lhs,
    const ON_SubDComponentRegionIndex* rhs
  );

  /*
  Description:
    Compares subdivision indices for minimum(lhs->m_subdivision_count,rhs->m_subdivision_count).
  */
  static int CompareMinimumSubregion(
    const ON_SubDComponentRegionIndex* lhs,
    const ON_SubDComponentRegionIndex* rhs
  );

  unsigned short Index(
    unsigned short i
  ) const;

  unsigned short m_subdivision_count = 0;

  // If m_subdivision_count > 0, then m_index[0], ..., m_index[m_subdivision_count-1]
  // identifies a subregion of the level 0 component.
  //
  // Faces with quad subdivision:
  //   m_index[n] is the subdivision quad for the region that contains
  //   the parent face's corner at face->m_vertex[m_index[n]].
  // Edges
  //   m_region_index[n] = 0 indicates the beginning half of the parent edge.
  //                       (begins at edge->Vertex(0))
  //   m_region_index[n] = 1 indicates the ending half of the parent edge.
  //                       (ends at edge->Vertex(1))
  //
  // When a component is created during a subdivision step, the value 0xFFFF
  // is used to mark the non-existent regions at earlier subdivision levels.
  // For example, if a level 1 edge is created by connecting 
  // a level0 edge subdivision point (middle-ish of the edge)
  // to a level0 face subdivision point (center-ish of the face),
  // then the level 1 edge would have 
  //   m_level0_component = ON_SubDComponentPtr::CreateNull(ON_SubDComponentPtr::Type::Edge, bReversed),
  //     (m_level0_component.IsNull() will be true)
  //   m_level0_component_id = ON_SubDComponentRegion::NewTransientId()
  //   m_subdivision_count = 1,
  //   m_region_index[0] = 0xFFFF.
  // 
  unsigned short m_index[ON_SubDComponentRegionIndex::IndexCapacity] = {};

  void Push(
    unsigned int region_index
  );
  
  void Pop();

  /*
  Description:
    Get a string of the form .a.b.c .a.b.c = m_index[] values.
  */
  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const ON_wString ToString() const;

  /*
  Description:
    Encodes ON_SubDComponentRegionIndex information in 32 bits.
    (m_subdivision_count) << 24
    | (0x00FF0000 & ((m_region_index[0]) << 16))
    | (m_region_index[1] & 0x0003) << (14)
    | (m_region_index[2] & 0x0003) << (12)
    ...
    | (m_index[m_subdivision_count] & 0x0003) <<(16-(2*m_subdivision_count))
  Remarks:
    This is useful when quick compare and sorting of regions is required,
    m_subdivision_count < 256, m_index[0] < 256, m_index[1] < 4, ..., m_index[m_subdivision_count] < 4
    Regions of N-gons with N < 256 and regions of edges
    satisify these condition when m_subdivision_count < 256 
    (which is always in real world situations).
  */
  ON__UINT32 ToCompressedRegionIndex() const;

  static const ON_SubDComponentRegionIndex FromCompressedRegionIndex(
    ON__UINT32 compressed_region_index
  );

  static ON__UINT32 ToCompressedRegionIndex(
    unsigned short subdivision_count,
    const unsigned short* region_index
  );

  /*
  Description:
    Decompress a 32-bit region.
  Parameters:
    region32 - [in]
      Value returned from To32BitRegion().
    subdivision_count - [out]
      Subdivision count
    region_index[] - out
      Region indices. The region_index[] array must have a capcity of at 
      least ON_SubDComponentRegion::region_index_capacity elements.
  */
  static void FromCompressedRegionIndex(
    ON__UINT32 compressed_region_index,
    unsigned short* subdivision_count,
    unsigned short* region_index
  );
};

class ON_CLASS ON_SubDComponentRegion
{
public:
  ON_SubDComponentRegion() = default;
  ~ON_SubDComponentRegion() = default;
  ON_SubDComponentRegion(const ON_SubDComponentRegion&) = default;
  ON_SubDComponentRegion& operator=(const ON_SubDComponentRegion&) = default;

public:
  static const ON_SubDComponentRegion Create(
    const class ON_SubDFace* level0_face
  );

  static const ON_SubDComponentRegion Create(
    unsigned int component_id,
    ON_SubDComponentPtr::Type component_type,
    bool bComponentMark
  );

  /*
  Description:
    Creates a region that can be used to identify a component
    created at a certain level of subdivision that does not
    come from dividing a component from the previous level.
    For example, Catmull Clark subdivision edges on level N+1
    that run from the level N edge subdivision point to the 
    level N face subdivision point.

    m_level0_component = ON_SubDComponentPtr::CreateNull(component_type, bComponentDirection?1:0),
       (m_level0_component.IsNull() will be true)
    m_level0_component_id = ON_SubDComponentRegion::NewTransientId()
    m_subdivision_count = subdivision_count,
     m_region_index[0, ..., (subdivision_count-1)] = 0xFFFF.
   
  */
  static const ON_SubDComponentRegion CreateSubdivisionRegion(
    ON_SubDComponentPtr::Type component_type,
    bool bComponentDirection,
    unsigned short subdivision_count,
    bool bAssignTransientId
  );

public:
  static const ON_SubDComponentRegion Empty;

public:
  ON_SubDComponentPtr m_level0_component = ON_SubDComponentPtr::Null;

  unsigned int m_level0_component_id = 0;

  unsigned short SubdivisionCount() const;

  ON_SubDComponentRegionIndex m_region_index;

  /*
  Returns:
    True if m_level0_component_id is a transient id.
  */
  bool IsTransientId() const;

  /*
  Returns:
    True if m_level0_component_id is the id of a persistent ON_SubD level 0 component.
  */
  bool IsPersistentId() const;

  /*
  Description:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection(),
    the entire sub region,
    and m_level0_component.m_ptr.
  */
  static int Compare(
    const ON_SubDComponentRegion* lhs,
    const ON_SubDComponentRegion* rhs
    );

  /*
  Descriptions:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection().
  */
  static int CompareTypeIdDirection(
    const ON_SubDComponentRegion* lhs,
    const ON_SubDComponentRegion* rhs
    );

  /*
  Description:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection(),
    and the m_region_index[] values for the 
    minimum subdivision count lhs and rhs.    
  */
  static int CompareTypeIdDirectionMinimumSubregion(
    const ON_SubDComponentRegion* lhs,
    const ON_SubDComponentRegion* rhs
    );
  
  /*
  Description:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection(),
    and the entire sub region.
  */
  static int CompareTypeIdDirectionSubregion(
    const ON_SubDComponentRegion* lhs,
    const ON_SubDComponentRegion* rhs
    );

  void SetLevel0Component(
    ON_SubDComponentPtr component_ptr
  );

  void SetLevel0Face(
    const ON_SubDFace* face
  );

  void SetLevel0EdgePtr(
    const ON_SubDEdgePtr edge_ptr
  );

  void SetLevel0Vertex(
    const ON_SubDVertex* vertex
  );

  /*
  Description:
    region_index - [in]
      If m_level0_component identifies an edge, region_index is 0 or 1,
      and the edge is reversed (1=m_level0_component.ComponentMark()),
      then PushAbsolute(1-region_index) is called.
      In every other case, PushAbsolute(region_index) is called.
  */
  void PushAdjusted(
    unsigned int region_index
  );

  /*
  Parameters:
    region_index - [in]
      If m_level0_component identifies a face, then region_index is the index of the
      corner vertex for the subdivision quad.
      If m_level0_component identifies an edge, then region_index must be 0 or 1.
  Description:
    Increments if m_subdivision_count and appends region_index to m_region_index[]
    (m_region_index[m_subdivision_count++] = region_index)
  */
  void PushAbsolute(
    unsigned int region_index
  );

  /*
  Description:
    Get a string of the form fN.a.b.c where N = m_level0_face-m_id, a.b.c = m_region_index[] values.
  */
  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const ON_wString ToString() const;

  void Pop();

  bool IsEmptyRegion() const;

  enum : unsigned int
  {
    TransientIdBit = 0x80000000U
  };


  /*
  Returns:
    A value that can be used to identify transient subdivision components that do not
    exist in the persistent levels of a SubD.
    Transient ids always satisfy (ON_SubDComponentRegion::TransientIdBit & transient_id) is not zero and 
    (~ON_SubDComponentRegion::TransientIdBit & transient_id) is not zero.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the ON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static const unsigned int NewTransientId();

  /*
  Description:
    Resets the value used to generate transient ids.
    This is useful during debugging session so that transient id
    values are predictable. Otherwise, use of this function 
    should be avoided.
  */
  static void ResetTransientId();

  /*
  Parameters:
    id - [in]
      Value to test to see if it is a transient subd component id.
  Returns:
    True if (ON_SubDComponentRegion::TransientIdBit & id) is not zero and 
    (~ON_SubDComponentRegion::TransientIdBit & id) is not zero.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the ON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static bool IsTransientId(unsigned int id);

  /*
  Parameters:
    id - [in]
      Value to test to see if it is a transient subd component id.
  Returns:
    If the id is a transient id, then its id value is returned.
    Otherwise, 0 is returned.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the ON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static unsigned int TransientId(unsigned int id);

  /*
  Parameters:
    id - [in]
      Value to test to see if it is a persitsent subd component id.
  Returns:
    True if (ON_SubDComponentRegion::TransientIdBit & id) is not zero and 
    (~ON_SubDComponentRegion::TransientIdBit & id) is not zero.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the ON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static bool IsPersistentId(unsigned int id);
};

class ON_CLASS ON_SubDFaceRegion
{
public:
  ON_SubDFaceRegion() = default;
  ~ON_SubDFaceRegion() = default;
  ON_SubDFaceRegion(const ON_SubDFaceRegion&) = default;
  ON_SubDFaceRegion& operator=(const ON_SubDFaceRegion&) = default;

  static const ON_SubDFaceRegion Empty;

public:
  // Identifies a region of an ON_SubDFace
  ON_SubDComponentRegion m_face_region;

  // When the face region is a quad, m_edge_region[4] identifies regions of ON_SubDEdge elements.
  // When the face region is a sub-quad, these edges may be null or have null ON_SubDEdge pointers 
  // and the ids will be zero or ON_SubDComponentRegion::IsTransientId() will be true.
  // When ON_SubDComponentRegion::IsTransientId() is true, the id does not identify
  // a persistent edge in the ON_SubD.
  ON_SubDComponentRegion m_edge_region[4];

  unsigned int m_level0_edge_count = 0;

  // If set, these are the vertice ids at the region's limit surface corners.
  // m_vertex_id[] is mutable because these values appear during recursive calculations.
  // When the face region is a sub-quad, these ids will be zero or ON_SubDComponentRegion::IsTransientId()
  // will be true. 
  // When ON_SubDComponentRegion::IsTransientId() is true, the id does not identify
  // a persistent vertex in the ON_SubD.
  mutable unsigned int m_vertex_id[4] = {};

public:
  void Push(unsigned int quadrant_index);

  bool IsValid(
    bool bSilentError
  ) const;

  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const ON_wString ToString() const;
};


class ON_CLASS ON_SubDFaceRegionAndNurbs
{
public:
  ON_SubDFaceRegionAndNurbs() = default;
  ~ON_SubDFaceRegionAndNurbs() = default;
  ON_SubDFaceRegionAndNurbs(const ON_SubDFaceRegionAndNurbs&) = default;
  ON_SubDFaceRegionAndNurbs& operator=(const ON_SubDFaceRegionAndNurbs&) = default;

  static const ON_SubDFaceRegionAndNurbs Empty;

public:
  ON_SubDFaceRegion m_face_region;
  // This pointer is not managed by ON_SubDFaceRegionAndNurbs
  class ON_NurbsSurface* m_nurbs_surface = nullptr;
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFromMeshParameters
//
class ON_CLASS ON_SubDFromSurfaceParameters
{
public:

  // Default construction is identical to ON_SubDFromMeshParameters::Smooth.
  ON_SubDFromSurfaceParameters() = default;
  ~ON_SubDFromSurfaceParameters() = default;
  ON_SubDFromSurfaceParameters(const ON_SubDFromSurfaceParameters&) = default;
  ON_SubDFromSurfaceParameters& operator=(const ON_SubDFromSurfaceParameters&) = default;

  static const ON_SubDFromSurfaceParameters Default;
  static const ON_SubDFromSurfaceParameters DefaultWithCorners;
  static const ON_SubDFromSurfaceParameters ControlNet;
  static const ON_SubDFromSurfaceParameters ControlNetWithCorners;

#pragma region RH_C_SHARED_ENUM [ON_SubDFromSurfaceParameters::Methods] [Rhino.Geometry.SubDFromSurfaceMethods] [byte]
  /// <summary>
  /// ON_SubDFromSurfaceParameters::Method are ways to create a SubD from a surface.
  /// </summary>
  enum class Methods : unsigned char
  {
    /// <summary>
    /// Used to indicate the method is not set.
    /// </summary>
    Unset = 0,

    /// <summary>
    /// The surface is approximated with a SubD friendly NURBS surface and the SubD is created
    /// to match the subd friendly nurbs surface. 
    /// If the input surface is a subd friendly NURBS surface, the subd and surface have the same geometry.
    /// </summary>
    SubDFriendlyFit = 1,

    /// <summary>
    /// The surface is converted to a NURBS surface and then a subd with one face per NURBS bispan 
    /// is created by using an appropriate subset of the NURBS surface control net.
    /// If the input surface is a subd friendly NURBS surface, the subd and surface have the same geometry.
    /// </summary>
    FromNurbsControlNet = 2
  };
#pragma endregion

  ON_SubDFromSurfaceParameters::Methods Method() const;

  void SetMethod(
    ON_SubDFromSurfaceParameters::Methods method
    );

  bool Corners() const;

  void SetCorners(
    bool bCorners
    );

private:
  ON_SubDFromSurfaceParameters::Methods m_method = ON_SubDFromSurfaceParameters::Methods::SubDFriendlyFit;
  bool m_bCorners = false;
  unsigned short m_reserved1 = 0;
  unsigned int m_reserved2 = 0;
  double m_reserved3 = 0.0;
  double m_reserved4 = 0.0;
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubD
//
class ON_CLASS ON_SubD : public ON_Geometry
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

  /*
  Returns:
    A runtime serial number identifying this subd.
  Remarks:
    ON_SubD is a shared pointer to an implementation. As such, there can
    be multiple ON_SubD instances that reference the same implementation.
    The runtime serial number uniquely identifies a particular instance
    of an implementation.
    The empty subd has runtime serial number = 0.
  */
  ON__UINT64 RuntimeSerialNumber() const;

  /*
  Returns:
    A runtime serial number that is incremented every time a the active level,
    vertex location, vertex or edge flag, or subd topology is changed.
  */
  ON__UINT64 ContentSerialNumber() const;

  /*
  Returns:
    A runtime serial number that is incremented every time a component status
    (locked, hidden, selected, highlighted, ...) is changed.
  */
  ON__UINT64 ComponentStatusSerialNumber() const;

  /*
  Description:
    Change the content serial number.
    This should be done ONLY when the active level,
    vertex location, vertex or edge flag, or subd topology is changed.
  Parameters:
    bChangePreservesSymmetry - [in]
      When in doubt, pass false.
      If the change preserves global symmtery, then pass true.
      (For example, a global subdivide preserves all types of symmetry.)
      Note well:
        Transformations do not preserve symmetries that are
        set with respect to world coordinate systems.
  Returns:
    The new value of ConentSerialNumber().
  Remarks:
    The value can change by any amount and core editing
    functions typically take care of changing the content serial number.
    A "top level" user of ON_SubD should never need to call this function.
  */
  ON__UINT64 ChangeContentSerialNumberForExperts(
    bool bChangePreservesSymmetry
  );

  /*
  Description:
    Get the SubD appearance (surface or control net);
  Returns:
    ON_SubDComponentLocation::Surface or ON_SubDComponentLocation::ControlNet.
  */
  ON_SubDComponentLocation SubDAppearance() const;

  /*
  Description:
    Set the SubD appearance (surface or control net).
  Parameters:
    subd_appearance - [in]
      ON_SubDComponentLocation::Surface or ON_SubDComponentLocation::ControlNet.
  Remarks:
    This makes no changes to the information that defines the SubD.
    It does not require regeneration of the ON_SubDMeshFragments.
    Application display will need to be updated.
  */
  void SetSubDAppearance(ON_SubDComponentLocation subd_appearance) const;

  /*
  Description:
    Get the SubD appearance (surface or control net);
  Returns:
    ON_SubDComponentLocation::Surface or ON_SubDComponentLocation::ControlNet.
  */
  static ON_SubDComponentLocation ToggleSubDAppearanceValue(ON_SubDComponentLocation subd_appearance);
  static ON_SubDComponentLocation DefaultSubDAppearance; // = ON_SubDComponentLocation::Surface

public:
#pragma region RH_C_SHARED_ENUM [ON_SubD::VertexTag] [Rhino.Geometry.SubDVertexTag] [byte]
  /// <summary>
  /// SubD::VertexTag identifies the type of subdivision vertex.  Different tags use
  /// different subdivision algorithms to determine where the subdivision point and
  /// limit point are located.  There are toplological constraints that restrict which
  /// tags can be assigned.
  /// </summary>
  enum class VertexTag : unsigned char
  {
    ///<summary>
    /// Not a valid vertex tag and the default value for ON_SubDVertex.m_vertex_tag.
    /// This encourages developers to thoughtfully initialize ON_SubDVertex.m_vertex_tag
    /// or use ON_SubD.UpdateAllTagsAndSectorCoefficients() to automatically set the
    /// m_vertex_tag values at an appropriate time.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Must be an interior vertex.
    /// All edges attached to a smooth vertex must be tagged as ON_SubD::EdgeTag::Smooth
    /// and must have 2 faces. 
    ///</summary>
    Smooth = 1,

    ///<summary>
    /// Can be an interior or a boundary vertex.
    /// Exactly two edges ending at a crease vertex must be tagged as ON_SubD::EdgeTag::Crease and may
    /// have 1 or 2 faces. 
    /// All other edges ending at a crease must be tagged as tagON_SubD::EdgeTag::Smooth and have 2 faces.
    /// Below P = ON_SubDVertex.ControlNetPoint() and Ai = ON_SubDVertex.Edge(i)->OtherEndVertex()->ControlNetPoint().
    /// A crease vertex subdivision point is (6*P + A1 + A2)/8.
    /// A crease vertex limit surface point is (4*P + A1 + A2)/6.
    ///</summary>
    Crease = 2,

    ///<summary>
    /// Can be an interior, boundary, nonmanifold, or isolated vertex.
    /// The location of a corner vertex is fixed. 
    /// The all subdivision points and the limit point are at the initial vertex location.
    /// The edges ending at a corner vertex can be smooth or crease edges.
    /// A corner vertex subdivision point is P where P = ON_SubDVertex.ControlNetPoint().
    /// A corner vertex limit surface point is P where P = ON_SubDVertex.ControlNetPoint().
    ///</summary>
    Corner = 3,

    ///<summary>
    /// Must be an interior vertex.  
    /// Every edge attached to a dart vertex must have 2 faces.
    /// Exactly one edge attached to a dart vertex must be tagged as ON_SubD::EdgeTag::Crease
    /// and every other attached edge must be tagged as tagON_SubD::EdgeTag::smooth.
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


  #if 0
  // .NET code that generates enums in hash pragma region R-H_C_S-H-ARED_ENUM cannot handle if 0.
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
#endif

#pragma region RH_C_SHARED_ENUM [ON_SubD::EdgeTag] [Rhino.Geometry.SubDEdgeTag] [byte]
  /// <summary>
  /// SubD::EdgeTag identifies the type of subdivision edge.  Different tags use
  /// different subdivision algorithms to calculate the subdivision point.
  /// </summary>  
  enum class EdgeTag : unsigned char
  {
    ///<summary>
    /// Not a valid edge tag and the default value for ON_SubDEdge.m_edge_tag.
    /// This encourages developers to thoughtfully initialize ON_SubDEdge.m_edge_tag.
    /// or use ON_SubD.UpdateAllTagsAndSectorCoefficients() to automatically set the
    /// m_edge_tag values at an appropriate time.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// At least one the edge's vertices must be tagged as ON_SubD::VertexTag::Smooth.
    /// The edge must have exactly two faces.
    /// The edge's subdivision point is (A1 + A2 + S(f1) + S(f2))/4, where 
    /// Ai = ON_SubDEdge.Vertex(i)->ControlNetPoint() and
    /// S(fi) = ON_SubDEdge.Face(i)->SubdivisionPoint().
    ///</summary>
    Smooth = 1,

    ///<summary>
    /// Both of the edge's vertices must be tagged as ON_SubD::VertexTag::Dart,
    /// ON_SubD::VertexTag::Crease, or ON_SubD::VertexTag::Corner. 
    /// (The vertex tags can be different.) The edge can have any number of faces.
    /// The edge's subdivision point is (A1+A2)/2 where Ai = ON_SubDEdge.Vertex(i)->ControlNetPoint().
    ///</summary>
    Crease = 2,
                 
    ///<summary>
    /// This tag appears only on level 0 edges that have exactly two neighboring faces
    /// and both of the edge's vertices are tagged as ON_SubD::VertexTag::Dart,
    /// ON_SubD::VertexTag::Crease, or ON_SubD::VertexTag::Corner.
    /// The level 1 subdivision point for a level 0 edge tagged as ON_SubD::EdgeTag::SmoothX 
    /// is the standard smooth edge subdivision point.
    /// When subdivided, the new subdivision vertex will be tagged
    /// as ON_SubD::VertexTag::Smooth and the subdivided edges will
    /// be tagged as ON_SubD::EdgeTag::Smooth.  
    /// The tag ON_SubD::EdgeTag::SmoothX can only appear on a level 0 edge.
    /// This tag exists because the ON_SubD subdivision
    /// algorithm requires any edge with both end vertices
    /// tagged as not smooth must be subdivided at its midpoint.
    /// Sector iterators treat "SmoothX" edges as smooth.
    /// Both edge m_sector_coefficient[] values must be set so the smooth subdivided edges will be valid.
    ///</summary>
    SmoothX = 4
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

#pragma region RH_C_SHARED_ENUM [ON_SubD::VertexFacetType] [Rhino.Geometry.SubDVertexFacetType] [byte]
  /// <summary>
  /// Summarizes the number of edges in faces in the whole object.
  /// </summary>
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
  
#pragma region RH_C_SHARED_ENUM [ON_SubD::ChainType] [Rhino.Geometry.SubDChainType] [byte]
  /// <summary>
  /// SubD::ChainType specifies what edge and vertex tag tests are used when creating edge chains.
  /// </summary>
  enum class ChainType : unsigned char
  {
    ///<summary>
    /// Unset.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// All types of edges and vertices can be in the chain.
    ///</summary>
    MixedTag = 1,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease property.
    ///</summary>
    EqualEdgeTag = 2,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease edge tag 
    /// and interior vertices have the corresponding smooth/crease vertex tag.
    ///</summary>
    EqualEdgeAndVertexTag = 3,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease property
    /// and every edge has the same number of faces.
    /// If the edges have 1 face, then interior vertices have valence = 3.
    /// If the edges have 2 faces, then interior vertices have valence = 4.
    ///</summary>
    EqualEdgeTagAndOrdinary = 4,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease edge tag,
    /// every edge has the same number of faces,
    /// and interior vertices have the corresponding smooth/crease vertex tag.
    /// If the edges have 1 face, then interior vertices have valence = 3.
    /// If the edges have 2 faces, then interior vertices have valence = 4.
    ///</summary>
    EqualEdgeAndVertexTagAndOrdinary = 5
  };
#pragma endregion

  /*
  Parameters:
    sit - [in]
      vertex sector iterator
    component_ring - [out]
      A sorted list of ON_SubDComponentPtr values are returned in component_ring[]
      component_ring[0] is the central vertex.
      component_ring[1] and subsequent components with odd indices are sector edges.
      component_ring[2] and subsequent components with even indices are sector faces.
      For edge components (i is odd), component_ring[i].ComponentMark() is the index of
      the center vertex in ON_SubDEge.m_vertex[].
    component_ring_capacity - [in]
      capacity of component_ring[] array
      1 + center_vertex.m_edge_count + center_vertex.m_face_count
      will be large enough.
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
    ON_SubDComponentPtr* component_ring,
    size_t component_ring_capacity
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
    const ON_SubDComponentPtr* component_ring,
    size_t component_ring_count
    );

  /*
  Returns:
    Number of points in the subdivision ring or 0 if the call fails.
  */
  static unsigned int GetSectorSubdivsionPointRing(
    const ON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    double* point_ring,
    size_t point_ring_capacity,
    size_t point_ring_stride
    );

  static unsigned int GetSectorSubdivisionPointRing(
    const ON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    ON_SimpleArray<ON_3dPoint>& subd_point_ring
    );

  static unsigned int GetSectorPointRing(
    bool bSubdivideIfNeeded,
    const ON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    double* subd_point_ring,
    size_t subd_point_ring_capacity,
    size_t subd_point_ring_stride
    );

  static unsigned int GetSectorPointRing(
    bool bSubdivideIfNeeded,
    size_t component_ring_count,
    const ON_SubDComponentPtr* component_ring,
    ON_SimpleArray<ON_3dPoint>& point_ring
    );

  static unsigned int GetSectorPointRing(
    bool bSubdivideIfNeeded,
    const class ON_SubDSectorIterator& sit,
    double* point_ring,
    size_t point_ring_capacity,
    size_t point_ring_stride
    );

  static unsigned int GetSectorPointRing(
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
    bool bFirstPass,
    bool bSecondPass,
    const class ON_SubDVertex* vertex0,
    const class ON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    double* point_ring,
    size_t point_ring_stride
    );

  static const class ON_SubDVertex* SubdivideSector(
    const class ON_SubDVertex* center_vertex,
    const class ON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
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

#pragma region RH_C_SHARED_ENUM [ON_SubD::SubDFriendlyKnotType] [Rhino.Geometry.SubDFriendlyKnotType] [byte]
  /// <summary>
  /// ON_SubD::SubDFriendlyKnotType identifies the types of subd friendly NURBS knot vectors.
  /// SubD friendly NURBS curves and surfacaes are always cubic and nonrational.
  /// Any time there is a multiple knot, the 2nd derivative is zero at the corresponding parameter.
  /// SubD friendly NURBS curves are either periodic or have zero 2nd derivative at the ends.
  /// </summary>    
  enum class SubDFriendlyKnotType : unsigned char
  {
    ///<summary>
    /// Not a valid type. Used to indicate the type has not been set and to encourage developers to explicitly specify a type.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// NURBS knot vector is an unclamped uniform cubic knot vector. 
    /// Every knot interval has the same length.
    /// Every knot has multiplicity 1. 
    ///</summary>
    UnclampedUniform = 1,

    ///<summary>
    /// NURBS knot vector is a clamped uniform cubic knot vector. 
    /// Every interior knot interval has the same length.
    /// End knots have multiplicity 3 and interior knots have multiplicity 1.
    ///</summary>
    ClampedUniform = 2,

    ///<summary>
    /// NURBS knot vector is a clamped piecewise uniform cubic knot vector.
    /// All nonzero knot intervals have the same length.
    /// End knots have multiplicity 3 and interior knots have multiplicity 1 or 3.
    /// Interior knots with multiplicity 3 correspond to interior SubD creases.
    ///</summary>
    ClampedPiecewiseUniform = 4,

    ///<summary>
    /// NURBS knot vector is not subd friendly.
    ///</summary>
    Unfriendly = 127
  };
 #pragma endregion

  /*
  Parameters:
    order - [in]
      NURBS knot vector order.
    cv_count - [in]
      Number of NURBS knot vector control points.
    knots - [in]
      NURBS knot vector. This is an array of (cv_count+2) knot values.
  Returns:
    SubD friendly knot vector type.
  Remarks:
    If order is not 4, cv_count is not valid, or knot is nullptr, then
    ON_SubD::SubDFriendlyKnotType::Unfriendly is returned.
  */
  static ON_SubD::SubDFriendlyKnotType NurbsKnotType(
    int order,
    int cv_count,
    const double* knots
  );

  static ON_SubD::SubDFriendlyKnotType NurbsKnotType(
    int order,
    int cv_count,
    const double* knots,
    ON_SimpleArray<double>* triple_knots
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
      or ON_SubDFromMeshParameters::Smooth. To get a sub-D with interior 
      creases use other static ON_SubDFromMeshParameters values or
      create one with custom settings.
  */
  static ON_SubD* CreateFromMesh( 
    const class ON_Mesh* level_zero_mesh,
    const class ON_SubDFromMeshParameters* from_mesh_parameters,
    ON_SubD* subd
    );

public:
#pragma region RH_C_SHARED_ENUM [ON_SubD::AutomaticMeshToSubDContext] [Rhino.Geometry.SubDAutomaticMeshToSubDContext] [byte]
  /// <summary>
  /// ON_SubD::AutomaticMeshToSubDContext indentifies a context where meshes can automatically
  /// be converted to subds.
  /// </summary>
  enum class AutomaticMeshToSubDContext : unsigned char
  {
    ///<summary>
    /// Indicates the context has not been initialized.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// A mesh in a Rhino 5 3dm file that is a representation of a box mode T-spline.
    /// By default, these meshes are automatically converted to subds.
    ///</summary>
    Rhino5BoxModeTSpline = 1,

    ///<summary>
    /// A mesh in an FBX file that has nonzero values for either preview division levels or render division levels.
    /// Some FBX files created by Maya save subdivision objects as meshes with nonzero division level values.
    /// By default, FBX division levels are ignored.
    ///</summary>
    FBXMeshWithDivisionLevels = 2
  };
#pragma endregion

  /*
  Returns:
    true if SubDs are automatically created when an ON_Mesh is found in the specified context.
    false otherwise.
  */
  static bool AutomaticMeshToSubD(
    ON_SubD::AutomaticMeshToSubDContext context
  );

  /*
  Parameters:
    context - [in]
      Situation where an ON_Mesh can automatically be converted into a subd.
    bAutomaticallyCreateSubD - [in]
      true if SubDs are automatically created when an ON_Mesh is found in the specified context.
      false otherwise.
  */
  static void SetAutomaticMeshToSubD(
    ON_SubD::AutomaticMeshToSubDContext context,
    bool bAutomaticallyCreateSubD
    );

  /*
  Parameters:
    context - [in]
      If context is ON_SubD::AutomaticMeshToSubDContext::Unset, all defaults will be restored.
      Otherwise, the default for the specific context will be restored.
  */
  static void AutomaticMeshToSubDRestoreDefaults(
    ON_SubD::AutomaticMeshToSubDContext context
    );

private:
  static const  bool AutomaticRhino5BoxModeTSplineToSubDDefault; // = true
  static const bool AutomaticFBXMeshWithDivisionLevelsToSubDDefault; // = false
  static bool AutomaticRhino5BoxModeTSplineToSubD; // current setting
  static bool AutomaticFBXMeshWithDivisionLevelsToSubD; // current setting

private:
  static ON_SubD* Internal_CreateFromMeshWithValidNgons(
    const class ON_Mesh* level_zero_mesh_with_valid_ngons,
    const class ON_SubDFromMeshParameters* from_mesh_parameters,
    ON_SubD* subd
  );

public:

  /*
  Description:
    Creates a SubD box
  Parameters:
    corners - [in] 
      Box corners.
      The bottom quad is specified by the first 4 points
      and the top quad specified by the last 4 points.
    edge_tag - [in]
      If edge_tag = ON_SubD::EdgeTag::Crease, then the box will have 
      creases and corners. Otherwise the box will be smooth.
    facecount_x - [in] Number of faces in x direction
    facecount_y - [in] Number of faces in y direction
    facecount_z - [in] Number of faces in z direction
    destination_subd [out] - 
      If destination_subd is not null, make the SubD box there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  static ON_SubD* CreateSubDBox(
    const ON_3dPoint corners[8],
    ON_SubD::EdgeTag edge_tag,
    unsigned int facecount_x,
    unsigned int facecount_y,
    unsigned int facecount_z,
    ON_SubD* destination_subd
  );

  /*
  Description:
    Creates a SubD sphere with 24 quad faces
  Parameters:
    sphere - [in]
      Location, size and orientation of the sphere
    destination_subd [out] -
      If destination_subd is not null, make the SubD box there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static ON_SubD* CreateSubDSphere(
  //  const ON_Sphere sphere,
  //  ON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD cylinder
  Parameters:
    box - [in]
      Location, size and orientation of the cylinder
    facecount_around - [in] Number of faces around the cylinder
    facecount_length - [in] Number of faces in the axis direction
    facecouont_z - [in] Number of faces in z direction
    destination_subd [out] -
      If destination_subd is not null, make the SubD box there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static ON_SubD* CreateSubDCylinder(
  //  const ON_Cylinder& cylinder,
  //  unsigned int facecount_around,
  //  unsigned int facecount_length,
  //  ON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD cone
  Parameters:
    cone - [in]
      Location, size and orientation of the cone
    facecount_around - [in] Number of faces around the cone
    facecount_length - [in] Number of faces in the axis direction
    destination_subd [out] -
      If destination_subd is not null, make the SubD cone there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static ON_SubD* CreateSubDCone(
  //  const ON_Cone& cone,
  //  unsigned int facecount_around,
  //  unsigned int facecount_length,
  //  ON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD truncated cone
  Parameters:
    cone - [in]
      Location, size and orientation of the cone
    truncate_param - [in] 0.0 < truncate_param <= 1.0
        Normalized parameter for truncation
        0.0: Base of cone
        1.0: Tip of cone
    facecount_around - [in] Number of faces around the cone
    facecount_length - [in] Number of faces in the axis direction
    destination_subd [out] -
      If destination_subd is not null, make the SubD cone there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static ON_SubD* CreateSubDTruncatedCone(
  //  const ON_Cone& cone,
  //  const double truncate_param,
  //  unsigned int facecount_around,
  //  unsigned int facecount_length,
  //  ON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD torus
  Parameters:
    torus - [in]
      Location, size and orientation of the torus
    major_facecount - [in] Number of faces around the major axis
    minor_facecount - [in] Number of faces around the minor axis
    destination_subd [out] -
      If destination_subd is not null, make the SubD torus there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static ON_SubD* CreaptSubDTorus(
  //  ON_Torus& torus,
  //  unsigned int major_facecount,
  //  unsigned int minor_facecount,
  //  ON_SubD* destination_subd);

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
    max_level_index - [in] 
      Remove all levels after max_level_index
  Returns:
    Number of removed levels.
  */
  unsigned int ClearHigherSubdivisionLevels(
    unsigned int max_level_index
    );

  /*
  Description:
    Remove subdivision levels
  Paramters:
    min_level_index - [in] 
      Remove all levels before min_level_index
  Returns:
    Number of removed levels.
  */
  unsigned int ClearLowerSubdivisionLevels(
    unsigned int min_level_index
    );

  /*
  Remove all levels except the active level.
  Returns: 
    Number of removed levels.
  */
  unsigned ClearInactiveLevels();

  bool IsEmpty() const;
  bool IsNotEmpty() const;


  /*
  Description:
    Get aggregate edge demographics for the subd.
  Returns:
    Bitwise or of ON_ComponentAttributes::EdgeAttributes values for every edge in the subd.
  */
  unsigned int AggregateEdgeAttributes() const;
  
  /////////////////////////////////////////////////////////
  //
  // Component (Vertex, Edge, Face) access
  //
  const ON_SubDComponentPtr ComponentPtrFromComponentIndex(
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
  const class ON_SubDVertex* LastVertex() const;

  /*
  Example:
    ON_SubDVertexIterator vit = subd.VertexIterator();
    for ( const ON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      ...
    }
  */
  class ON_SubDVertexIterator VertexIterator() const;

  /*
  Description:
    Avoid using this class. It is more efficient to use
    an ON_SubDVertexIterator returned by VertexIterator().
  */
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

  const class ON_SubDVertex* VertexFromComponentIndex(
    ON_COMPONENT_INDEX component_index
  ) const;

  /////////////////////////////////////////////////////////
  //
  // Edge access
  //

  unsigned int EdgeCount() const;

  const class ON_SubDEdge* FirstEdge() const;
  const class ON_SubDEdge* LastEdge() const;

  /*
  Example:
    ON_SubDEdgeIterator eit = subd.EdgeIterator();
    for ( const ON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      ...
    }
  */
  class ON_SubDEdgeIterator EdgeIterator() const;

  /*
  Description:
    Avoid using this class. It is more efficient to use
    an ON_SubDEdgeIterator returned by EdgeIterator().
  */
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

  const class ON_SubDEdge* EdgeFromComponentIndex(
    ON_COMPONENT_INDEX component_index
  ) const;

  /////////////////////////////////////////////////////////
  //
  // Face access
  //

  unsigned int FaceCount() const;

  const class ON_SubDFace* FirstFace() const;
  const class ON_SubDFace* LastFace() const;

  /*
  Example:
    ON_SubDFaceIterator fit = subd.FaceIterator();
    for ( const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      ...
    }
  */
  class ON_SubDFaceIterator FaceIterator() const;

  /*
  Description:
    Avoid using this class. It is more efficient to use
    an ON_SubDFaceIterator returned by FaceIterator().
  */
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


  const class ON_SubDFace* FaceFromComponentIndex(
    ON_COMPONENT_INDEX component_index
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

  /*
  Description:
    Delete components in cptr_list[]. 
    If a vertex is in cptr_list[], the vertex and every edge and face attached
    to the vertex are deleted.
    If an edge is in cptr_list[], the edge and every face attached
    to the edge are deleted.
    If a face is in cptr_list[], the face is deleted.
  Parameters:
    cptr_list - [in]
    cptr_count - [in]
      length of cptr_list[] array.
    bMarkDeletedFaceEdges - [in]
      If true, surviving edges attached to delete faces 
      have their runtmime mark set.
  Returns:
    1: some state settings changed on the component.
    1: some state setting changed on the component.
  */
  bool DeleteComponents(
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bMarkDeletedFaceEdges
    );

  bool DeleteComponents(
    const ON_SimpleArray<ON_SubDComponentPtr>& cptr_list,
    bool bMarkDeletedFaceEdges
  );

  bool DeleteComponentsForExperts(
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bDeleteIsolatedEdges,
    bool bUpdateTagsAndCoefficients,
    bool bMarkDeletedFaceEdges
    );

  /*
  Description:
    Delete marked components.
  Parameters:
    bDeleteMarkedComponents - [in]
      If true, marked components are deleted.
      If false, unmarked components are deleted.      
    mark_bits - [in]
      A component is marked if component.m_status.IsMarked(mark_bits) is true.
  */
  bool DeleteMarkedComponents(
    bool bDeleteMarkedComponents,
    ON__UINT8 mark_bits,
    bool bMarkDeletedFaceEdges
  );

  /*
  Description:
    Delete marked components.
  Parameters:
    bDeleteMarkedComponents - [in]
      If true, marked components are deleted.
      If false, unmarked components are deleted.
    mark_bits - [in]
      A component is marked if component.m_status.IsMarked(mark_bits) is true.
  */
  bool DeleteMarkedComponentsForExperts(
    bool bDeleteMarkedComponents,
    ON__UINT8 mark_bits,
    bool bDeleteIsolatedEdges,
    bool bUpdateTagsAndCoefficients,
    bool bMarkDeletedFaceEdges
  );

public:
  /*
  Description:
    Removes all per face material channel index overrides on the active level.
  Returns:
    Number of changed faces.
  Remarks:
    Per face material channel indices are a mutable property on ON_SubDFace and are set with ON_SubDFace.SetMaterialChannelIndex().
  */
  unsigned int ClearPerFaceMaterialChannelIndices();

  /*
  Returns:
    True if one or more faces on the active level have per face material channel index overrides.
  Remarks:
    Per face material channel indices are a mutable property on ON_SubDFace and are set with ON_SubDFace.SetMaterialChannelIndex().
  */
  bool HasPerFaceMaterialChannelIndices() const;

  /*
  Description:
    Removes all per face color overrides on the active level.
  Returns:
    Number of changed faces.
  Remarks:
    Per face colors are a mutable property on ON_SubDFace and are set with ON_SubDFace.SetPerFaceColor().
  */
  unsigned int ClearPerFaceColors() const;

  /*
  Returns:
    True if one or more faces on the active level have per face color overrides.
  Remarks:
    Per face colors are a mutable property on ON_SubDFace and are set with ON_SubDFace.SetPerFaceColor().
  */
  bool HasPerFaceColors() const;


  /////////////////////////////////////////////////////////
  //
  // Topology Queries
  //

  /*
  Description:
    Determine solid orientation of the active level.
  Returns:
    +2     subd is a solid but orientation cannot be computed
    +1     subd is a solid with outward facing normals
    -1     subd is a solid with inward facing normals
     0     subd is not a solid
  See Also:
    ON_SubD::IsSolid
  */
 int SolidOrientation() const;

  /*
  Description:
    Test subd to see if the active level is a solid.  
    A "solid" is a closed oriented manifold.
  Returns:
    true       subd is a solid
    fals       subd is not a solid
  See Also:
    ON_SubDp::SolidOrientation
    ON_SubDp::IsManifold
  */
  bool IsSolid() const;
  
  /*
  Description:
    Test subd to see if the active level is an oriented manifold.
  Parameters:
    bIsOriented - [out]
      True if every edge that has two faces is oriented.
      Note that non-manifold edges are ignored.
    bHasBoundary - [in]
      True if there is at least one edge with a single face.
  Returns:
    True if the subd is a manifold (has at lease one face and every edge has 1 or 2 faces).
    False if the subd is not a manifold (has no faces or at least one edge with 0 or 3 or more faces)
  See Also:
    ON_SubDp::IsSolid
  */
  bool IsManifold(
    bool& bIsOriented,
    bool& bHasBoundary
    ) const;

  bool IsManifold() const;



    /*
  Description:
    Automatically get a  boundary from a seed edge.
  Parameters:
    first_edge - [in]
      An edge with FaceCount() <= 1. 
      The search for the second edge occurs and first_edge.RelativeVertex(1)
      and all edges added to boundary_edge_chain[] have FaceCount() = first_edge.Edge()->FaceCount().
    bUseEdgeMarks -[in]
      If true, only unmarked edges will be added to boundary_edge_chain[]
      and they will be marked when added to boundary_edge_chain[].
    boundary_edge_chain - [out]
      Edge chain beginning with first_edge. 
      When true is returned, boundary_edge_chain[] has 3 or more edges and is a closed loop.
      When false is returned, boundary_edge_chain[] will contain an open chain with 0 or more edges.
  Returns:
    true if boundary_edge_chain[] is a closed loop of 3 or more boundary edges.
  */
  static bool GetBoundaryEdgeChain(
    ON_SubDEdgePtr first_edge,
    bool bUseEdgeMarks,
    ON_SimpleArray< ON_SubDEdgePtr >& boundary_edge_chain
  );

  /////////////////////////////////////////////////////////
  //
  // Editing tools
  //

  unsigned int MergeColinearEdges(
    bool bMergeBoundaryEdges,
    bool bMergeInteriorCreaseEdges,
    bool bMergeInteriorSmoothEdges,
    double distance_tolerance,
    double maximum_aspect,
    double sin_angle_tolerance
    );

  /*
  Description:
    Merge consecutive edges into a single edge.
  eptr0 - [in]
    first edge (will not be deleted)
  eptr1 - [in]
    second edge (will be deleted if edges can be merged)
  Returns:
    Merged edge (eptr0) or ON_SubDEdgePtr::Null if edges could not be merged
  */
  ON_SubDEdgePtr MergeConsecutiveEdges(
    ON_SubDEdgePtr eptr0,
    ON_SubDEdgePtr eptr1
    );

  /*
  Returns:
    True if eptr0.RelativeVetex(1) == eptr1.RelativeVetex(0) and both edges
    have the same set of faces.
  */
  static bool EdgesAreConsecutive(
    ON_SubDEdgePtr eptr0,
    ON_SubDEdgePtr eptr1
    );

  // returns true if all facets are consistently oriented
  bool IsOriented() const;

  // reverses the orientation of all facets
  bool ReverseOrientation() const;

  // Attempts to orient all facet to match the first facet.
  bool Orient() const;

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
  Remarks:    
    After all editing operations are completed, you must call this->UpdateEdgeSectorCoefficients(true) before
    evaluation.
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
    The first edge of both faces is the inserted edge.
  */
  const class ON_SubDEdge* SplitFace(
    class ON_SubDFace* face,
    unsigned int fvi0,
    unsigned int fvi1
    );

  /*
  Description:
    Split a face into two faces by inserting and edge connecting the
    specified vertices.
  Parameters:
    face - [in]
      A face with at least four edges.
    v0 - [in]
    v1 - [in]
      Vertices on the face boundary.
  Returns:
    A pointer to the inserted edge.
    The inserted edge runs from v0 to v1.
    ON_SubDEdge.Face(0) is the original face and ON_SubDEdge::Face(1) is 
    the added face.
    The first edge of the input face remains the first edge of face.  
    The inserted edge is the first edge of the added face.
  */
  const class ON_SubDEdge* SplitFace(
    class ON_SubDFace* face,
    const class ON_SubDVertex* v0,
    const class ON_SubDVertex* v1
    );

  /*
  Description:
    Replace a face with a triangle fan  by adding a single new vertex at fan_center_point 
    and adding tringle made from the face's edes to the center point.
  Parameters:
    face - [in]
      This face is replaced with a triangle fan and becomes the first triangle in the fan.
    fan_center_point - [in]
      If valid, this point is used as the fan's center.
      Otherwise the centriod of the face's vertices is used s the fan's center.
      When in doubt, pass ON_3dPoint::UnsetPoint.
    bMarkFaces - [in]
      If true, face and new triangles are marked.
      Existing marks are not modified.
  Returns:
    If successfull, the new vertex at the center of the triangle fan.
    Otherwise, nullptr is returned.
  */
  const class ON_SubDVertex* ReplaceFaceWithTriangleFan(
    class ON_SubDFace* face,
    ON_3dPoint fan_center_point,
    bool bMarkFaces
    );



  /*
  Description:
    When finished editing a SubD, call this function to delete all cached evaluation
    values and update all vertex tag, edge tags, edge sector coefficients.
  Remarks:
    This function is the most reliable (and heavy handed) way to update SubD component
    information. Expert users can choose to be more selective when certain about 
    exactly what needs to be modified.
  */
  void SubDModifiedNofification();


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
    It is easiest to call UpdateAllTagsAndSectorCoefficients().
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
    coefficient set to ON_SubDSectorType::UnsetSectorCoefficient.
  Remarks:
    It is easiest to call UpdateAllTagsAndSectorCoefficients().
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
    coefficient set to ON_SubDSectorType::UnsetSectorCoefficient.
  Remarks:
    It is easiest to call UpdateAllTagsAndSectorCoefficients().
  */
  unsigned int UpdateEdgeSectorCoefficients(
    bool bUnsetSectorCoefficientsOnly
    );


  /*
  Descripiton:
    Sets the m_group_id value to 0 for every vertex, edge and face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for every vertex.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearVertexGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for every edge.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearEdgeGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for every face.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearFaceGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for the specified components.
  Parameters:
    bClearVertexGroupIds - [in]
      If true, m_group_id for every vertex is set to zero.
    bClearEdgeGroupIds - [in]
      If true, m_group_id for every edge is set to zero.
    bClearFaceGroupIds - [in]
      If true, m_group_id for every face is set to zero.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearComponentGroupIds(
    bool bClearVertexGroupIds,
    bool bClearEdgeGroupIds,
    bool bClearFaceGroupIds
  ) const;


  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every vertex, edge and face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every vertex.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearVertexMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every edge.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearEdgeMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every face.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearFaceMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for the specified components.
  Parameters:
    bClearVertexMarkBits - [in]
      If true, m_status.MarkBits() for every vertex is set to zero.
    bClearEdgeMarkBits - [in]
      If true, m_status.MarkBits() for every edge is set to zero.
    bClearFaceMarkBits - [in]
      If true, m_status.MarkBits() for every face is set to zero.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearComponentMarkBits(
    bool bClearVertexMarkBits,
    bool bClearEdgeMarkBits,
    bool bClearFaceMarkBits
  ) const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every vertex, edge and face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearComponentMarks() const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every vertex.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearVertexMarks() const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every edge.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearEdgeMarks() const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearFaceMarks() const;

  /*
  Descripiton:
    Selectively clear m_status.RuntimeMark()
  Parameters:
    bClearVertexMarks - [in]
      If true, m_status.ClearRuntimeMark() is called for every vertex.
    bClearEdgeMarks - [in]
      If true, m_status.ClearRuntimeMark() is called for every edge.
    bClearFaceMarks - [in]
      If true, m_status.ClearRuntimeMark() is called for every face.
    marked_component_list - [out]
      If not nullptr, then pointer to components that were marked 
      are returned in this marked_component_list[]
  Returns:
    Number of marks that were cleared.
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
  Parameters:
    bAddMarkedComponents - [in]
      If true, marked components are added to component_list[].
      If false, unmarked components are added to component_list[].
    mark_bits - [in]
      If mark_bits is zero, then a component is "marked" if component.Mark() is true.
      Otherwise a component is "marked" if mark_bits = component.MarkBits().
   */
   unsigned int GetMarkedComponents(
    bool bAddMarkedComponents,
    ON__UINT8 mark_bits,
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
    ON_SimpleArray< class ON_SubDComponentPtr >& component_list
  ) const;

  unsigned int UnselectComponents(
    bool bUnselectAllVertices,
    bool bUnselectAllEdges,
    bool bUnselectAllFaces
  ) const;

  /*
  Description:
    Save the current component status of the indictated subd components.
  Parameters:
    bGetVertexStatus - [in]
    bGetEdgeStatus - [in]
    bGetFaceStatus - [in]
    bClearStatus - [in]
      If true, the bits in status_mask will also be cleared for the components.
    status_mask - [in]
      Status bits to save.
    component_list - [out]
    status_list - [out]
      component_list[] and status_list[] are parallel arrays for components with
      a matching status bit set.
  */
  unsigned int GetComponentStatus(
    bool bGetVertexStatus,
    bool bGetEdgeStatus,
    bool bGetFaceStatus,
    bool bClearStatus,
    ON_ComponentStatus status_mask,
    ON_SimpleArray< const class ON_SubDComponentBase* >& component_list,
    ON_SimpleArray< ON_ComponentStatus >& status_list
  ) const;

  unsigned int SetComponentStatus(
    ON_ComponentStatus status_mask,
    const ON_SimpleArray< const class ON_SubDComponentBase* >& component_list,
    const ON_SimpleArray< ON_ComponentStatus >& status_list
  ) const;

  /*
  Description:
    Transforms the SubD components in ci_list[].
  Parameters:
    xform - [in]
    ci_list - [in]
    ci_count - [in]
    component_location - [in]
      Select between applying the tranform to the control net (faster)
      or the surface points (slower).
  Returns:
    Number of vertex locations that changed.
  */
  unsigned int TransformComponents(
    const ON_Xform& xform,
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    ON_SubDComponentLocation component_location
    );

  unsigned int TransformComponents(
    const ON_Xform& xform,
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    ON_SubDComponentLocation component_location
    );

  /*
  Description:
    Extrude entire subd bay adding a ring of faces around the boundary and moving the original subd.
  */
  unsigned int Extrude(
    const ON_Xform& xform
  );

  unsigned int ExtrudeComponents(
    const ON_Xform& xform,
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count
    );

  unsigned int ExtrudeComponents(
    const ON_Xform& xform,
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count
    );

  unsigned int ExtrudeComponents(
    const ON_Xform& xform,
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    bool bExtrudeBoundaries,
    bool bPermitNonManifoldEdgeCreation
    );

  unsigned int ExtrudeComponents(
    const ON_Xform& xform,
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bExtrudeBoundaries,
    bool bPermitNonManifoldEdgeCreation
  );

private:
  unsigned int Internal_ExtrudeComponents(
    const ON_Xform& xform,
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bExtrudeBoundaries,
    bool bPermitNonManifoldEdgeCreation
  );

public:

  /*
  Parameters:
    ci_list - [in]
      Array of ci_count ON_COMPONENT_INDEX values that identify the vertices. 
      Elements that do not identifiy a SubD vertex are ignored.
    ci_count - [in]
      Number of elements in the ci_list[] array.
    vertex_tag - [in]
      Desired tag. If a vertex has the desired tag or cannot accept the desired tag, 
      then that vertex is skipped. 
      If vertex_tag is ON_SubD::VertexTag::Corner, then every edge touching 
      that vertex is converted to a crease.
  Returns:
    number of vertex tags that were changed.
  */
  unsigned int SetVertexTags(
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    ON_SubD::VertexTag vertex_tag
  );

  /*
  Parameters:
    ci_list - [in]
      Array of ci_count ON_COMPONENT_INDEX values that identify the vertices. 
      Elements that do not identifiy a SubD vertex are ignored.
    ci_count - [in]
      Number of elements in the ci_list[] array.
    vertex_tag - [in]
      Desired tag. If a vertex has the desired tag or cannot accept the desired tag, 
      then that vertex is skipped.
      If vertex_tag is ON_SubD::VertexTag::Corner, then every edge touching 
      that vertex is converted to a crease.
  Returns:
    number of vertex tags that were changed.
  */
  unsigned int SetVertexTags(
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    ON_SubD::VertexTag vertex_tag
  );

  /*
  Returns:
    number of tags that were changed.
  */
  unsigned int SetEdgeTags(
    const ON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    ON_SubD::EdgeTag edge_tag
  );

  /*
  Returns:
    number of tags that were changed.
  */
  unsigned int SetEdgeTags(
    const ON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    ON_SubD::EdgeTag edge_tag
  );


  /*
  Description:
    Remove all interior creases.
  Returns:
    Number of edges converted from crease to smooth.
  */
  unsigned int RemoveAllCreases();

  /*
  Description:
    Apply the Catmull-Clark subdivision algorithm and save the results
    in this ON_SubD.
  Parameters:
    level_index - [in]
      Level where subdivision starts
    count - [in] > 0
      Number of times to subdivide.
  Returns:
    Number of subdivision steps that succeeded. 
    (= count when everything works, < count when input is not valid)
  */
  bool GlobalSubdivide(
    unsigned int count
    );

  bool GlobalSubdivide();

  /*
  Description:
    Apply the Catmull-Clark subdivision algorithm to the faces in face_list[].
  Parameters:
    face_list - [in]
      faces to subdivide
    face_count - [in]
      number of components.
  Returns:
    true if successful.
  */
  bool LocalSubdivide(
    class ON_SubDFace const*const* face_list,
    size_t face_count
  );

  bool LocalSubdivide(
    const ON_SimpleArray< const class ON_SubDFace* >& face_list
  );

  bool LocalSubdivide(
    const ON_SimpleArray<ON_COMPONENT_INDEX>& face_list
  );

  /*
  Description:
    Adds a vertex with tag = ON_SubD::VertexTag::Unset.
  */
  class ON_SubDVertex* AddVertex(
    const double* P
    );

  /*
  Description:
    Adds a vertex with specified tag.
  */
  class ON_SubDVertex* AddVertex(
    ON_SubD::VertexTag vertex_tag,
    const double* P
    );

  /*
  Description:
    Expert user tool to add a vertex with specified information. This function
    is useful when copying portions of an existing SubD to a new SubD.
  Parameters:
    candidate_vertex_id - [in]
      If candidate_vertex_id is > 0 and is available, 
      the returned value with have id = candidate_vertex_id.
      Otherwise a new id will be assigned.
    vertex_tag - [in]
      Pass ON_SubD::VertexTag::Unset if not known.
    P - [in]
      nullptr or a 3d point.
   initial_edge_capacity - [in]
     Initial capacity of the m_edges[] array. Pass 0 if unknown.
   initial_face_capacity - [in]
     Initial capacity of the m_faces[] array. Pass 0 if unknown.
  */
  class ON_SubDVertex* AddVertexForExperts(
    unsigned int candidate_vertex_id,
    ON_SubD::VertexTag vertex_tag,
    const double* P,
    unsigned int initial_edge_capacity,
    unsigned int initial_face_capacity
    );

  /*
  Description:
    Search for a vertex with a specificed control net point.
  Parameters:
    control_net_point - [in]
      Look for a vertex with this value for ControlNetPoint().
    tolerance - [in]
      Use 0.0 when in doubt. 
      If > 0.0, then the vertex closest to control_net_point 
      will be returned if the distance from that vertex to control_net_point
      is <= distance_tolerance.
  Returns:
    An ON_SubDVertex pointer or nullptr if none exists.
  */
  const class ON_SubDVertex* FindVertex(
    const double* control_net_point,
    double distance_tolerance
  ) const;

  /*
  Description:
    Search for a vertex with a specificed control net point. If one does not
    exist, add a new one.
  Parameters:
    control_net_point - [in]
      Look for a vertex with this value for ControlNetPoint().
    tolerance - [in]
      Use 0.0 when in doubt.
      If > 0.0, then the vertex closest to control_net_point
      will be returned if the distance from that vertex to control_net_point
      is <= distance_tolerance.
  Returns:
    An ON_SubDVertex pointer or nullptr if none exists.
    */
  const class ON_SubDVertex* FindOrAddVertex(
    const double* control_net_point,
    double distance_tolerance
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
   then ON_SubD::EdgeTag::SmoothX is returned.

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
    Add an edge with tag = ON_SubD::EdgeTag::Unset to the subd.
  Parameters:
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
    class ON_SubDVertex* v0,
    class ON_SubDVertex* v1
    );

  /*
  Description:
    Search for an edge connecting v0 and v1.
  Parameters:
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
  Returns:
    An ON_SubDEdgePtr to a connecting edge or ON_SubDEdgePtr::Null if none exists.
  */
  const ON_SubDEdgePtr FindEdge(
    const class ON_SubDVertex* v0,
    const class ON_SubDVertex* v1
    ) const;

  /*
  Description:
    Search for an edge connecting v0 and v1. If none exists, then add one.
  Parameters:
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
  Returns:
    An ON_SubDEdgePtr to a connecting edge or ON_SubDEdgePtr::Null if none exists.
  */
  const ON_SubDEdgePtr FindOrAddEdge(
    class ON_SubDVertex* v0,
    class ON_SubDVertex* v1
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
        tag on the returned edge will be ON_SubD::EdgeTag::SmoothX.  This
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
      The edge begins at v0 and ends at v1.
    v0_sector_coefficient - [in]
      Pass ON_SubDSectorType::UnsetSectorCoefficient if unknown.
    v1 - [in]
      The edge begins at v0 and ends at v1.
    v1_sector_coefficient - [in]
      Pass ON_SubDSectorType::UnsetSectorCoefficient if unknown.
  */
  class ON_SubDEdge* AddEdgeWithSectorCoefficients(
    ON_SubD::EdgeTag edge_tag,
    class ON_SubDVertex* v0,
    double v0_sector_coefficient,
    class ON_SubDVertex* v1,
    double v1_sector_coefficient
    );

  /*
  Description:
    Expert user tool to add an edge with specified information. This function
    is useful when copying portions of an existing SubD to a new SubD.
  Parameters:
    candidate_edge_id - [in]
      If candidate_edge_id is > 0 and is available,
      the returned edge with have id = candidate_edge_id.
      Otherwise a new id will be assigned.
    edge_tag - [in]
      Pass ON_SubD::EdgeTag::Unset if not known.
    v0 - [in]
      The edge begins at v0 and ends at v1.
    v0_sector_coefficient - [in]
      Pass ON_SubDSectorType::UnsetSectorCoefficient if unknown.
    v1 - [in]
      The edge begins at v0 and ends at v1.
    v1_sector_coefficient - [in]
      Pass ON_SubDSectorType::UnsetSectorCoefficient if unknown.
   initial_face_capacity - [in]
     Initial face capacity. Pass 0 if unknown.
  */
  class ON_SubDEdge* AddEdgeForExperts(
    unsigned int candidate_edge_id,
    ON_SubD::EdgeTag edge_tag,
    class ON_SubDVertex* v0,
    double v0_sector_coefficient,
    class ON_SubDVertex* v1,
    double v1_sector_coefficient,
    unsigned int initial_face_capacity
  );

  /*
  Parameters:
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2).
     Consecutive edges must have a single common vertex.
     The orientations of the ON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddTriangleFace(
    class ON_SubDEdge* edge0,
    class ON_SubDEdge* edge1,
    class ON_SubDEdge* edge2
    );

  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
     The ON_SubDEdgePtr parameters must be oriented so that for consecutive pairs of edges, 
     edge0.RelativeVertex(1) and edges1.RelativeVertex(0) are same vertex. 
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddTriangleFace(
    ON_SubDEdgePtr edge0,
    ON_SubDEdgePtr edge1,
    ON_SubDEdgePtr edge2
    );

  /*
  Parameters:
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
    edge3 - [in]
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2. edge3).
     Consecutive edges must have a single common vertex.
     The orientations of the ON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddQuadFace(
    class ON_SubDEdge* edge0,
    class ON_SubDEdge* edge1,
    class ON_SubDEdge* edge2,
    class ON_SubDEdge* edge3
    );
 
  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
    edge3 - [in]
     The ON_SubDEdgePtr parameters must be oriented so that for consecutive pairs of edges, 
     edge0.RelativeVertex(1) and edges1.RelativeVertex(0) are same vertex. 
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2,edge3).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddQuadFace(
    ON_SubDEdgePtr edge0,
    ON_SubDEdgePtr edge1,
    ON_SubDEdgePtr edge2,
    ON_SubDEdgePtr edge3
    );

  /*
  Parameters:
    edges[] - [in]
     edges[] must have 3 or more elements.
     edges[i] and edges[(i+1)%edge_count] must have a single common vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
     The orientations of the ON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddFace(
    const ON_SimpleArray<ON_SubDEdge*>& edges
    );

  /*
  Parameters:
    edges[] - [in]
     edges[] must have 3 or more elements.
     The ON_SubDEdgePtr parameters must be oriented so that
     edges[i].RelativeVertex(1) and edges[(i+1)%edge_count].RelativeVertex(0)
     are the same vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddFace(
    const ON_SimpleArray<ON_SubDEdgePtr>& edges
    );

  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edges[] - [in]
     edges[i] and edges[(i+1)%edge_count] must have a single common vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
     The orientations of the ON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddFace(
    class ON_SubDEdge * const * edges,
    unsigned int edge_count
    );

  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edges[] - [in]
     The ON_SubDEdgePtr parameters must be oriented so that
     edges[i].RelativeVertex(1) and edges[(i+1)%edge_count].RelativeVertex(0)
     are the same vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class ON_SubDFace* AddFace(
    const class ON_SubDEdgePtr* edges,
    unsigned int edge_count
    );

  /*
  Parameters:
    candidate_face_id - [in]
      If candidate_face_id is > 0 and is available,
      the returned face with have id = candidate_face_id.
      Otherwise a new id will be assigned.
    edge[] - [in]
      The edge[] array must be sorted and correct oriented
      (edge[i].RelativeVertex(1) == edge[(i+1)%edge_count].RelativeVertex(0)).
    edge_count - [in]
      Must be >= 3.
  */
  class ON_SubDFace* AddFaceForExperts(
    unsigned candidate_face_id,
    const class ON_SubDEdgePtr* edge,
    unsigned int edge_count
    );

public:

#pragma region RH_C_SHARED_ENUM [ON_SubD::PatchStyle] [Rhino.Geometry.SubDPatchStyle] [byte]
  /// <summary>
  /// SubD::PatchStyle identifies the style of patch used to fill holes.
  /// </summary>
  enum class PatchStyle : unsigned char
  {
    ///<summary>
    /// Not a valid style.
    /// This encourages developers to thoughtfully select a patch style and can 
    /// be used to indicate a UI control is not initialized.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Automatically choose a patch style that will generally create a good looking result.
    /// If a hole boundary is not convex, it is triangulated. Otherwise:
    /// If a hole has 3 edges, a single triangle face is used.
    /// If a hole has 4 edges, a single quad face is used.
    /// If a hole has 5 or more edges and an odd number of edges, a triangle fan is used. 
    /// If a hole has 6 or more edges and an even number of edges, a quad fan is used. 
    ///</summary>
    Automatic = 1, 

    ///<summary>
    /// A single face is used under all conditions.
    ///</summary>
    SingleFace = 2,

    ///<summary>
    /// A triangle fan used under all conditions. The center of the fan
    /// is the average of the hole boundary vertex control net points.
    ///</summary>
    TriangleFan = 3,

    ///<summary>
    /// If the hole boundary has an even mumber of edges, a quad fan is used.
    /// Otherwise a triangle fan is used. The center of the fan
    /// is the average of the hole boundary vertex control net points.
    ///</summary>
    QuadOrTriangleFan = 4,

    ///<summary>
    /// The hole boundary is triangluated.
    /// Typically this style is selected when a boundary not convex 
    /// and the other styles produce faces with overlapping regions.
    ///</summary>
    Triangulated = 5
  };
#pragma endregion


  
public:
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


  /*
  Description:
    Expert user tool to remove all edge and vertex connnections from a face
  Parameters:
    face - [in]
  Remarks:
    This tool is used during editing of a SubD and the 
    connections are removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceConnections(
    ON_SubDFace* face
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
    Clear all cached evaluation information (meshes, surface points, boundiang boxes, ...) 
    that depends on edge tags, vertex tags, and the location of vertex control points.
  */
  void ClearEvaluationCache() const;


  /*
  Description:
    Clear all cached evaluation information (meshes, surface points, boundiang boxes, ...)
    that depends on the vertex's control point location or tag. 
  Parameter:
    vertex - [in]
  */
  void ClearNeighborhoodEvaluationCache(
    const ON_SubDVertex* vertex,
    bool bTagChanged
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







#pragma region RH_C_SHARED_ENUM [ON_SubD::NurbsSurfaceType] [Rhino.Geometry.SubD.NurbsSurfaceType] [nested:byte]
  /// <summary>
  /// ON_SubD::NurbsSurfaceType specifies what type of NURBS surfaces are returned by ON_SubD.GetSurfaceNurbsFragments()
  /// </summary>
  enum class NurbsSurfaceType : unsigned char
  {
    ///<summary>
    /// Not a valid type. Used to indicate the type has not been set and to encourage developers to explicitly specify a type.
    /// When in doubt, specify NurbsSurfaceType::Large.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// A single NURBS surface will be created for each SubD quad. Near extraordinary vertices, the surfaces may
    /// have lots of knots.
    ///</summary>
    Large = 1,

    ///<summary>
    /// NURBS surfaces will be as large as possible without the addition of extra knots. 
    /// Near extraordinary vertices, the surfaces may
    /// have lots of knots.
    /// This option is prefered when a user wants larger NURBS surfaces but not at the cost of addtional NURBS control points.
    ///</summary>
    Medium = 2,

    ///<summary>
    /// NURBS surfaces will not be merged and will have clamped knots.
    ///</summary>
    Small = 3,

    ///<summary>
    /// NURBS surfaces will not be merged and will have unclamped uniform knots.
    /// This is useful as a starting point for customized merging and modifying
    /// continuity at extraordinary vertices.
    ///</summary>
    Unprocessed = 4
  };
#pragma endregion

  






public:
  /*
  Parameters:
    minimum_rectangle_count - [in]
      >= 1 minimum number of rectangles in texture domain
    image_width - [in]
    image_height = [in]
      If a texture image size is known, pass it here. Otherwise pass 0.0 for both parameters.
  Returns:
    Suggested way to partition a texture coodinate domain into rectangles.
    ON_2udex.i = "x" count
    ON_2udex.j = "y" count
    For example (3,2) would mean divide the 2d texture domain into 3 segments across and 2 segments vertically.
  */
  static const ON_2udex TextureDomainGridSize(
    unsigned minimum_rectangle_count,
    double image_width,
    double image_height
  );

  static unsigned int TextureImageSuggestedMinimumSize(
    ON_2udex grid_size
  );

  /*
  Returns:
    Suggesting minimum number of pixels for a texture image width and height for this SubD.
  */
  unsigned int TextureImageSuggestedMinimumSize() const;

  /*
  Parameters:
    normalize_texture_domain_delta - [in]
      > 0.0 and <= 1.0
    count - [in]
      >= 1 number of rectangles
    image_width - [in]
    image_height = [in]
      If a texture image size is known, pass it here. Otherwise pass 0.0 for both parameters.
      When available, it is used to avoid have different rectangles share pixels.
    quad_texture_domain - [out]
      Each quad will have texture domain
      minimum point = (x0,y0)
      maximum point = (x0+quad_texture_domain,y0+quad_texture_domain)
      where (x0,y0) is the normalized coord
    quad_texture_delta - [out]
      To move from one quad to the next in the same row, increment the x-coordinate by quad_texture_delta.
      To move from the last quad in a row to the first quad in the next row,
      set x-coordinate = to the next in the same row, increment the x-coordinate by quad_texture_delta.

  Returns:
    number of quads per row and column in the region.
  */
  static const ON_2udex GetTextureDomainAndDelta(
    unsigned minimum_rectangle_count,
    double image_width,
    double image_height,
    ON_2dVector& quad_texture_domain,
    ON_2dVector& quad_texture_delta
  );


  static ON_SubDTextureDomainType TextureDomainTypeFromUnsigned(
    unsigned int texture_domain_type_as_unsigned
  );

  static const ON_wString TextureDomainTypeToString(
    ON_SubDTextureDomainType texture_domain_type
    );


  /*
  Description:
    Set the default texture coordinate domain on each face.
  Parameters:
    texture_domain_type - [in]
      Type of texture coordinates. 
      If ON_SubDTextureDomainType::Unset or ON_SubDTextureDomainType::Custom,
      is passed, the type setting  is changed but no changes are made to texture coordinats.
      When in doubt and performance may be an issue, pass false.
      If true, then, where possible, texture domains are set so
      quad grid regions have neighboring texture domains.
      On subds with large regions quad grids, this produces a result that
      looks better when default surface parameter texture coordinates are used.
      However, this requires a calculation that can be slow on subs with lots of faces.
    bLazy - [in]
      If true and if texture_domain_type == TextureDomainType(), 
      then nothing is done and true is returned.
    bSetFragmentTextureCoordinates
      When in doubt, pass true.
      If true and if the faces have cached fragments, then after the domains are set the
      fragments texture domains and their texture coordinates are set as well.
  Remarks:
    SubD texture domains and coordinates are a mutable property.
    They can be changed by rendering applications as needed.
    Call SetTextureCoordinates() to restore them to the default values.
  */
  bool SetTextureDomains(
    ON_SubDTextureDomainType texture_domain_type,
    bool bLazy,
    bool bSetFragmentTextureCoordinates
  ) const;

  ON_SubDTextureDomainType TextureDomainType() const;

  static bool SetTextureDomains(
    ON_SubDFaceIterator& fit,
    ON_SubDTextureDomainType texture_domain_type,
    bool bSetFragmentTextureCoordinates
  );

  /*
  Description:
    Use the current default texture domain values on each face
    to set the currently cached ON_MeshFragment texture coordinates.
    The subd's TextureMappingTag() property is set to
    ON_TextureMapping::SurfaceParameterTextureMapping.
  Remarks:
    SubD texture domains and coordinates are a mutable property.
    They can be changed by rendering applications as needed.
    Call SetTextureCoordinatesFromFaceDomains() to restore them to the default values.
  */
  bool SetTextureCoordinatesFromFaceDomains() const;

  /*
  Description:
    Use the current default texture domain values on each face
    to set the currently cached ON_MeshFragment texture coordinates.
  Parameters:
    fit - [in]
      faces to set.
  */
  static bool SetTextureCoordinatesFromFaceDomains(
    ON_SubDFaceIterator& fit
  );

  /*
  Description:
    Use a texture mapping function to set currently cached
    ON_SubDMeshFragment m_T[] values.
  Parameters:
    mapping - [in]
    subd_xform - [in]
      If not nullptr, the mapping calculation is performed as
      if the subd were transformed by subd_xform; the
      location of the subd is not changed.
    bLazy - [in]
      If true and the m_T[] values were set using the same
      mapping parameters, then no calculation is performed.
  Returns:
    True if successful.
  Remarks:
    SubD texture domains and coordinates are a mutable property.
    They can be changed by rendering applications as needed.
    Call SetTextureCoordinatesFromFaceDomains() to restore them to the default values.
  */
  bool SetTextureCoordinates(
    const class ON_TextureMapping& mapping,
    const class ON_Xform* subd_xform,
    bool bLazy
  ) const;
     
  /*
  Returns:
    The current texture mapping tag. This tag is set by SetTextureCoordinatesFromFaceDomains()
    and SetTextureCoordinates().
  Remarks:
    SubD texture domains and coordinates are a mutable property.
    They can be changed by rendering applications as needed.
    Call SetTextureCoordinatesFromFaceDomains() to restore them to the default values.
  */
  const ON_MappingTag TextureMappingTag() const;

  void SetTextureMappingTag(const class ON_MappingTag&) const;

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

  void ShareDimple(const class ON_SubDMeshImpl&);
  void SwapDimple(class ON_SubDMeshImpl& );

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
class ON_CLASS ON_SubDRef
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
    Number of references to the managed ON_SubD, including the one by this ON_SubDRef.
  */
  unsigned int ReferenceCount() const;

  /*
  Description:
    Allocates a new empty ON_SubD and has this ON_SubDRef reference it.
  */
  class ON_SubD& NewSubD();

  /*
  Description:
    Allocates a new ON_SubD and has this ON_SubDRef reference it.
  Parameters:
    src - [in]
      The new ON_SubD managed by this ON_SubDRef will be a copy of src.SubD().
  Returns:
    A reference to the new ON_SubD managed by this ON_SubDRef.
  */
  class ON_SubD& CopySubD(
    const ON_SubDRef& src
    );

  /*
  Description:
    Allocates a new ON_SubD and has this ON_SubDRef reference it.
  Parameters:
    src - [in]
      The new ON_SubD managed by this ON_SubDRef will be a copy of src.
  Returns:
    A reference to the new ON_SubD managed by this ON_SubDRef.
  */
  class ON_SubD& CopySubD(
    const ON_SubD& src
    );

  /*
  Description:
    If ReferenceCount() > 1, then have this ON_SubDRef reference a 
    new copy. Otherwise do nothing. The result being that this will
    be the unique reference to the ON_SubD managed by this ON_SubDRef.
  Returns:
    A reference to the ON_SubD uniquely managed by this ON_SubDRef.
  */
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
// ON_SubDComponentList
//
class ON_CLASS ON_SubDComponentList
{
public:
  ON_SubDComponentList() = default;
  ~ON_SubDComponentList() = default;
  ON_SubDComponentList(const ON_SubDComponentList&);
  ON_SubDComponentList& operator=(const ON_SubDComponentList&);

public:
  static const ON_SubDComponentList Empty;

public:
  unsigned CreateFromMarkedComponents(const ON_SubD& subd, bool bComponentInListMark);
  unsigned CreateFromMarkedVertices(const ON_SubD& subd, bool bVertexInListMark);
  unsigned CreateFromMarkedEdges(const ON_SubD& subd, bool bEdgeInListMark);
  unsigned CreateFromMarkedFaces(const ON_SubD& subd, bool bFaceInListMark);

  unsigned CreateFromComponentList(const ON_SubD& subd, const ON_SimpleArray<ON_COMPONENT_INDEX>& component_list);
  unsigned CreateFromComponentList(const ON_SubD& subd, const ON_SimpleArray<ON_SubDComponentPtr>& component_list);

  unsigned CreateFromVertexIdList(const ON_SubD& subd, const ON_SimpleArray<unsigned>& free_vertex_ids);
  unsigned CreateFromVertexList(const ON_SubD& subd, const ON_SimpleArray<ON_SubDVertexPtr>& free_vertices);
  unsigned CreateFromVertexList(const ON_SubD& subd, const ON_SimpleArray<const ON_SubDVertex*>& free_vertices);

  void Destroy();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllComponents();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllVertices();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllEdges();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllFaces();

  /*
  Returns:
    SubD runtime serial number.
  */
  ON__UINT64 SubDRuntimeSerialNumber() const;

  /*
  Returns:
    SubD content serial number when this list was created or the last
    time UpdateContentSerialNumber() was run.
  */
  ON__UINT64 SubDContentSerialNumber() const;

  unsigned int Count() const;

  /*
  operator[] returns ON_SubDComponentPtr::Null when index is out of bounds.
  */
  const ON_SubDComponentPtr operator[](int) const;
  const ON_SubDComponentPtr operator[](unsigned int) const;
  const ON_SubDComponentPtr operator[](ON__INT64) const;
  const ON_SubDComponentPtr operator[](ON__UINT64) const;
#if defined(ON_RUNTIME_APPLE)
  const ON_SubDComponentPtr operator[](size_t) const;
#endif

  const ON_SimpleArray< ON_SubDComponentPtr >& ComponentList() const;

  const ON_SubD& SubD() const;

  /*
  Description:
    Update the saved subd content serial number to the current value of SubD().ContentSerialNumber().
  Returns:
    Updated value of subd content serial number.
  */
  ON__UINT64 UpdateContentSerialNumber();

  /*
  Description:
    Change the component list to reference components in a different subd.
  Parameters:
    new_subd - [in]
      subd to replace current referenced subd
    bUpdateDeletedComponents - [in]
      false: current components that are deleted will be ignored.
      true: if the corresponding component in new_sub is not deleted, it
      will be added to the list.
  Returns:
    Number of components in list after updating.
  */
  unsigned int UpdateSubDForExperts(const ON_SubD& subd, bool bUpdateDeletedComponents);


private:
  unsigned Internal_Create(
    const ON_SubD& subd,
    bool bAddVertices,
    bool bAddEdges,
    bool bAddFaces,
    bool bComponentInListMark,
    unsigned marked_component_count
  );

  unsigned Internal_RemoveComponents(
    bool bRemoveVertices,
    bool bRemoveEdges,
    bool bRemoveFaces
  );

private:
  ON__UINT64 m_subd_runtime_serial_number = 0;
  ON__UINT64 m_subd_content_serial_number = 0;

  unsigned m_subd_vertex_count = 0;
  unsigned m_subd_edge_count = 0;
  unsigned m_subd_face_count = 0;
  unsigned m_reserved = 0;

private:
  ON_SubD m_subd; // keeps subd dimple in scope while m_component_list[] is active
  ON_SimpleArray< ON_SubDComponentPtr > m_component_list;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentMarksClearAndRestore
//
class ON_SubDComponentMarksClearAndRestore
{
public:
  // Constructor saves current component RuntimeMark() settings and then clears them.
  ON_SubDComponentMarksClearAndRestore(
    const ON_SubD& subd
  );

  // Destructor restores saved marks.
  ~ON_SubDComponentMarksClearAndRestore();

  /*
  Description:
    Restore marks saved by the constructor.
  Parameters:
    bDisableFutureRestores - [in]
      If true, no additional restores, including by the destructor, will occur.
  */
  bool Restore(
    bool bDisableFutureRestores
  );

  // Call DisableRestore() to prevent the destructor from restoring saved marks.
  void DisableRestore();

  const ON_SimpleArray< const class ON_SubDComponentBase* >& ComponentList() const;

private:
  ON_SubD m_subd;

  ON_SimpleArray< const class ON_SubDComponentBase* > m_component_list;

  bool m_bRestore = true;
  unsigned char m_reserved1 = 0;
  unsigned char m_reserved2 = 0;
  unsigned char m_reserved3 = 0;
  unsigned int m_reserved4 = 0;

private:
  ON_SubDComponentMarksClearAndRestore(const ON_SubDComponentMarksClearAndRestore&) = delete;
  ON_SubDComponentMarksClearAndRestore& operator=(const ON_SubDComponentMarksClearAndRestore&) = delete;
};


//////////////////////////////////////////////////////////////////////////
//
// ON_SubDSectorType
//
class ON_CLASS ON_SubDSectorType
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
  // Sector Coefficients
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
    ON_SubDSectorType::ErrorSectorCoefficient
      This ON_SubDSectorType is not valid and the calculation failed.
  */
  double SectorCoefficient() const;


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
    a value >= 0 and <= ON_SubDSectorType::MaximumCornerAngleIndex
  */
  unsigned int CornerSectorAngleIndex() const;

  /*
  Description:
    An angle index value of ON_SubDSectorType::MaximumCornerAngleIndex indicates
    the angle is 2pi radians.
  */
  enum : unsigned int
  {
    MaximumCornerAngleIndex = 72
  };

  // ON_SubDSectorType::MinimumCornerAngleRadians = (2.0*ON_PI)/((double)(ON_SubDSectorType::MaximumCornerAngleIndex));
  static const double MinimumCornerAngleRadians;

  // ON_SubDSectorType::MaximumCornerAngleRadians = 2.0*ON_PI - ON_SubDSectorType::MinimumCornerAngleRadians;
  static const double MaximumCornerAngleRadians;

  /*
  Parameters:
    angle_radians - [in] (0.0 <= angle_radians <= 2*ON_PI)
      The angle between the bounding crease edges
  Returns:
    If angle_radians is valid input, then the value angle_index is returned.
    The value angle_index is selected so that
    (0 < angle_index < ON_SubDSectorType::MaximumCornerSectorIndex) and
    fabs(angle_index*angle_quantum - angle_radians) is as small as possible, where
    angle_quantum =  (2.0*ON_PI)/ON_SubDSectorType::MaximumCornerSectorIndex.
    Otherwise ON_UNSET_UINT_INDEX is returned.
  */
  static unsigned int CornerAngleIndexFromCornerAngleRadians(
    double angle_radians
    );

  /*
  Convert and angle index into radians
  Parameters:
    corner_angle_index - [in]
      0 to ON_SubDSectorType::MaximumCornerAngleIndex.
  Returns:
    If angle_index is valid, the corresponding angle in radians is returned.
      = (angle_index / ((double)ON_SubDSectorType::MaximumCornerAngleIndex)) * ON_2PI
    Otherwise ON_UNSET_VALUE is returned.
  */
  static double AngleRadiansFromCornerAngleIndex(
    unsigned int corner_angle_index
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
    ON_SubD::VertexTag vertex_tag,
    unsigned int sector_edge_count
    );

  static unsigned int SectorPointRingCountFromFaceCount(
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
    ON_SubDSectorType::IgnoredSectorCoefficient
  */
  static double SmoothSectorCoefficient();
  
  /*
  Parameters:
  face_type - [in]
  sector_face_count - [in]
    number of faces in the smooth sector.
  Returns:
    0: 
      failed to caclulate weight
    ON_SubDSectorType::UnsetSectorCoefficient:
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
  static double CreaseSectorCoefficient(
    unsigned int sector_face_count
    );

  static double DartSectorCoefficient(
    unsigned int sector_face_count
    );

  static double CornerSectorCoefficient(
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


  // This value is is used to set edge sector coefficients when the
  // actual value is not needed. This occurs at both ends
  // of a creased edge and when the end of a smooth edge
  // is a smooth vertex.
  static const double IgnoredSectorCoefficient; // = 0.0;

  // This value is used to mark edge sector coefficients that need to be
  // set in the future when more information is available.
  // It is typically used when creating a subD control net
  // and the facet type is not known. Any value < 0.0 and not
  // equal to ON_UNSET_VALUE would work. The fact that the actual
  // value is -999.0 has no other significance.
  static const double UnsetSectorCoefficient; // = -8883.0;

  // This value indicates an edge sector coefficient calculation failed.
  static const double ErrorSectorCoefficient; // = -9993.0;

  static bool IsValidSectorCoefficientValue(
    double weight_value,
    bool bAllowUnsetTaggedEndCoefficient
    );

  /*
  Returns:
    ON_SubDSectorType::ErrorSectorCoefficient and calls debug breakpoint
  */
  static double SectorCoefficientCalculationError();

  
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
    const class ON_SubDFace* face,
    unsigned int face_vertex_index
    );
  
  static ON_SubDSectorType Create(
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
    S*Transpose(V, E[0], F[0], E[1], F[1], ..., E[N-1], F[N-1]).
    For a dart vertex, E[0] is the point at the end of the creased edge.


    For a boundary vertex (crease or corner), the coefficents are ordered
    so that one iteration of subdivision is given by:
    S*Transpose(V, E[0], F[0], E[1], F[1], ..., F[N-2], E[N-1]).

    N = edge valence = number of edges in the sector.
    E[i] = end of i-th edge radiating from V.
    F[i] = point on the quad that is opposite V.
    The edges and faces are ordered radially so that the face for F[i]
    lies between the edges for E[i] and E[(i+1)%N].

  Parameters:
    S - [out]
      subdivision matrix
      Matrix coefficent (i,j) = S[i][j]
      0 <= i < R, 0 <= j < R, R = ON_SubDSectorType.PointRingCount()
    matrix_capacity - [in]
      S[] can store any RxR matrix with R <= matrix_capacity.

  Returns:
    R > 0: 
      R = PointRingCount() and S is the RxR subdivision matrix for the sector type.
    0: Invalid input
  */
  unsigned int GetSubdivisionMatrix(
    double** S,
    size_t matrix_capacity
    ) const;

  /*
  Parameters:
    S - [out]
      subdivision matrix. 
      Matrix coefficent (i,j) = S[i*R + j],
      0 <= i < R, 0 <= j < R, R = ON_SubDSectorType.PointRingCount()
    S_capacity - [in]
      Number of elements in S[] array
  Returns:
     0: Invalid input.
    >0: Number of rows and columns in S.
        This number is always ON_SubDSectorType.PointRingCount().
  */
  unsigned int GetSubdivisionMatrix(
    double* S,
    size_t S_capacity
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
    double* E1,
    size_t E1_capacity,
    double* E2,
    size_t E2_capacity
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
  unsigned int GetSurfaceEvaluationCoefficients(
    double* LPev,
    size_t LPev_capacity,
    double* LT0ev,
    size_t LT0ev_capacity,
    double* LT1ev,
    size_t LT1ev_capacity
    ) const;

  // SurfaceNormalSign() is a debugging tool - slow and not useful in general
  double SurfaceNormalSign() const;

  bool SurfaceEvaluationCoefficientsAvailable() const;

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
    double* eigenvalues,
    size_t eigenvalues_capacity
    );

  /////*
  ////Description:
  ////  The subdivision matrix for all cases is known.
  ////  A complete set of eigenvalues are available for some cases.
  ////Parameters:
  ////  facet_type - [in]
  ////  vertex_tag - [in]
  ////  sector_edge_count - [in]
  ////    The input parameters identify the subdivision case.
  ////Returns:
  ////  R > 0: Eigenvalues are known.  There subdivison matrix is R x R.
  ////  0: Eigenvalues for this case are not known.
  ////*/
  ////static unsigned int AllEigenvaluesAvailableKnown(
  ////  ON_SubD::VertexTag vertex_tag,
  ////  unsigned int sector_edge_count
  ////  );

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
  ON_SubD::VertexTag m_vertex_tag = ON_SubD::VertexTag::Unset;
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;
  unsigned int m_hash = 0; // SetHash() sets this field, SectorTypeHash() returns its value.
  unsigned int m_corner_sector_angle_index = 0; // >= 0 and <= ON_SubDSectorType::MaximumCornerAngleIndex
  unsigned int m_sector_face_count = 0;
  double m_sector_coefficient = 0.0;
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
    sector_theta - [in] 0 < sector_theta <= 2*ON_PI
      value from one of the sector theta functions.
      ON_SubDEdge::SectorTheta()
      ON_SubDEdge::SmoothSectorTheta()
      ON_SubDEdge::CreaseSectorTheta()
      ON_SubDEdge::CornerSectorTheta()
      ON_SubDEdge::DartSectorTheta()
  Returns:
    0: 
      failed to caclulate weight
    ON_SubDSectorType::ErrorSectorCoefficient: 
      sector_theta is not valid.
    0 < w < 1:
      The returned value is 
      1/2 + 1/3*cos(sector_angle_radians). (1/6 <= w <= 5/6)
  Remarks:
    This is a useful tool when calling AddEdge while a subdivision
    level is being constructed.
  */
  static double SectorCoefficientFromTheta(
    double sector_theta
    );
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDMeshFragment
//
//  Meshes of ON_SubD limit surface are calculated in fragments.
//
class ON_CLASS ON_SubDMeshFragmentGrid
{
public:
  // No construction for performance reasons. 
  // use = ON_SubDMeshFragmentGrid::Empty
  // QuadGridFromSideSegmentCount(...)
  // or QuadGridFromDisplayDensity(...) to initialize.
  //
  //ON_SubDMeshFragmentGrid() = default;
  //~ON_SubDMeshFragmentGrid() = default;
  //ON_SubDMeshFragmentGrid(const ON_SubDMeshFragmentGrid&) = default;
  //ON_SubDMeshFragmentGrid& operator=(const ON_SubDMeshFragmentGrid&) = default;


  static const ON_SubDMeshFragmentGrid Empty;

  static const ON_SubDMeshFragmentGrid OneQuadGrid;

  /*
  Description:
    Get mesh facet quads that index into a grid of points.
  Parameters:
    side_segment_count - [in]
      number quads in each row and column of the quad grid.
        side_segment_count >= 1
        side_segment_count <= ON_SubDMesh::MaximumSideSegmentCount
        side_segment_count must be a power of 2

    level_of_detail - [in]
      0: quad count = maximum quad count = (side_count x side_count)
      1: quad count = 1/4 maximum quad count
      1: quad count = 1/16 maximum quad count
      ...
      If 4^level_of_detail > maximum quad count, then a single quad is returned.
  */
  static ON_SubDMeshFragmentGrid QuadGridFromSideSegmentCount(
    unsigned int side_segment_count,
    unsigned int level_of_detail
    );

  static ON_SubDMeshFragmentGrid QuadGridFromDisplayDensity(
    unsigned int display_density,
    unsigned int level_of_detail
    );

private:
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
    unsigned int* quads,
    size_t quad_capacity,
    size_t quad_stride,
    unsigned int* sides,
    size_t side_capacity,
    size_t side_stride
    );

public:
  unsigned int SideSegmentCount() const;

  /*
  Returns:
    SideSegmentCount() + 1;
  */
  unsigned int SidePointCount() const;

  /*
  Description:
    The GridId() is persistent and unique based on the contents of the
    grid. It is intended to be used in render applications that store
    copies of ON_SubDMeshFragmentGrid settings in things like
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


  unsigned int GridFaceCount() const;

  /*
  Returns:
    Total number of points in the grid = SidePointCount()*SidePointCount().
  */
  unsigned int GridPointCount() const;

  /*
  Parameters:
    grid_point_index - [in]
      0 <= grid_point_index < GridPointCount().
    grid_parameters = [out]
      normalize parameters for that point.
      These could be used in the role of surface evaluation parameters
      for texture mapping applications.
  Returns:
    True if grid_point_index was valid and grid_parameters was returned.
  */
  bool GetGridParameters(
    unsigned int grid_point_index,
    double grid_parameters[2]
    ) const;

  /*
  Parameters:
    grid_point_index - [in]
      0 <= grid_point_index < GridPointCount().
  Returns:
    Grid (i,j) for that grid_point_index.  
    0 <= i < SidePointCount()
    0 <= j < SidePointCount()
  */
  const ON_2udex Grid2dexFromPointIndex(
    unsigned int grid_point_index
  )  const;

  /*
  Parameters:
    i - [in]
      0 <= i < SidePointCount()
    j - [in]
      0 <= j < SidePointCount()
  Returns:
    0 <= grid_point_index < GridPointCount().
  */
  unsigned int PointIndexFromGrid2dex(
    unsigned int i,
    unsigned int j
  )  const;

  /*
  Returns:
    A number between 0 and 8 or ON_UNSET_INT_INDEX.
    SideSegmentCount() = 2^DisplayDensity().
  */
  unsigned int DisplayDensity() const;

  unsigned int LevelOfDetail() const;

private:
  unsigned char m_reserved;

public:
  unsigned char m_reserved1 = 0;
  unsigned char m_side_segment_count; // = 2^n for non-empty grids (0 <= n <= 8)
  unsigned short m_F_count;   // = m_side_segment_count*m_side_segment_count
  unsigned short m_F_level_of_detail; // 0 = highest, > 0 = reduced
  unsigned short m_F_stride;
  const unsigned int* m_F;
  const unsigned int* m_S; // [4*m_side_segment_count + 1] indices that form the polyline boundary.
  const ON_SubDMeshFragmentGrid* m_prev_level_of_detail; // nullptr or the previous level with more facets.
  const ON_SubDMeshFragmentGrid* m_next_level_of_detail; // nullptr or the next level with fewer facets.
};

class ON_CLASS ON_SubDMeshFragment
{
public:
  // No construction for performance reasons. 
  // If you require initialization, use = ON_SubDMeshFragment::Empty
  //
  //ON_SubDMeshFragment() = default;
  //~ON_SubDMeshFragment() = default;
  //ON_SubDMeshFragment(const ON_SubDMeshFragment&) = default;
  //ON_SubDMeshFragment& operator=(const ON_SubDMeshFragment&) = default;

  bool CopyFrom( const ON_SubDMeshFragment& src_fragment );

  /*
  Parameters:
    src_fragment - [in]
      fragment to copy
    display_density - [in]
      The desired display density of the copy.

      If display_density = ON_UNSET_UINT_INDEX, then this->m_P_capacity must
      be at least srf_fragment.m_P_count, all points are copied, and 
      this->m_grid = srf_fragment.m_grid.

      Otherwise, src_fragment must have enough points to provide 
      the specified denisity and this must have enough 
      point capacity to store the specified density.

    this - [out]
      This must have a point capacity large enough to accomodate the
      requested display density.

  */
  bool CopyFrom( 
    const ON_SubDMeshFragment& src_fragment,
    unsigned int display_density
  );
  
  // Every field of ON_SubDMeshFragment::Empty is zero.
  static const ON_SubDMeshFragment Empty;

  enum : unsigned int
  {
    MaximumSideSegmentCount = (1U << ON_SubDDisplayParameters::MaximumDensity)
  };
  
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

  /*
  Description:
    Copies location and optionally normal information from a side of src_fragment
    to a side of dst_fragment. This is often used for "sealing" adjacent fragments
    where the same edge is evaluated from different faces. All evaluations are as accurate
    as IEEE double precision arithmetic can produce. However there are often tiny differences
    (last few bits of the coordinate values differ) in vertext locations because we are 
    using finite precision arithmetic with operations in different orders. 
    In general, rendering, mesho topology, and edge analysis calculations perform poorly
    when tinity differences exist.

  Parameters:
    bTestNearEqual - [in]
      If true, then the copy is performed only when the points or normals are nearly
      identical. In this case if the discrepance between coordinates is too large,
      then this function returns false.
    bCopyNormals - [in]
      If true, then normal information is copied along with point information.
    src_fragment - [in] 
      source fragment
    i0, i1 - [in]
      indices for the src_fragment.m_S[] array.      
    dst_fragment - [in] 
      destination fragment
    j0, j1 - [in]
      indices for dst_fragment.m_S[] array.
      abs(j0-j1) = abs(i0-i1);
  Remarks:
    It is required that max(i0,i1) - min(i0,i1) = max(j0,j1) - min(j0,j1) and
    all indices are valid for the respective fragments.
    The src_fragment point/normal identified by src_fragment.m_S[i] 
    is copied to he dst_fragment point/normal identified by dst_fragment.m_S[j], 
    where i0->j0, i1->j1. 
  */
  static bool SealAdjacentSides(
    bool bTestNearEqual,
    bool bCopyNormals,
    const ON_SubDMeshFragment& src_fragment,
    unsigned int i0,
    unsigned int i1,
    ON_SubDMeshFragment& dst_fragment,
    unsigned int j0,
    unsigned int j1
  );

  /*
  Parameters:
    bTestNearEqual - [in]
      If true, then the copy is performed only when the points or normals are nearly
      identical. In this case if the discrepance between coordinates is too large,
      then this function returns false.
    src - [in]
    dst - [in/out]
      The 3d point (src[0],src[1],src2[2]) is copied to (dst[0],dst[1],dst[2]).
  Returns:
    True if a copy occured.
  */
  static bool SealPoints(
    bool bTestNearEqual,
    const double* src, 
    double* dst
    );

  /*
  Parameters:
    bTestNearEqual - [in]
      If true, then the copy is performed only when the points or normals are nearly
      identical. In this case if the discrepance between coordinates is too large,
      then this function returns false.
    src - [in]
    dst - [in/out]
      The 3d point (src[0],src[1],src2[2]) is copied to (dst[0],dst[1],dst[2]).
  Returns:
    True if a copy occured.
  */
  static bool SealNormals(
    bool bTestNearEqual,
    const double* src, 
    double* dst
    );

  /*
  Returns:
    Number of mesh quads in a full sized fragment with the specified mesh density.
  */
  static unsigned int FullFragmentMeshQuadCountFromDensity(
    unsigned int mesh_density
  );

  /*
  Returns:
    Number of mesh quads in a half sized fragment with the specified mesh density.
  */
  static unsigned int HalfFragmentMeshQuadCountFromDensity(
    unsigned int mesh_density
  );

  /*
  Returns:
    Number of mesh points in a full sized fragment with the specified mesh density.
  */
  static unsigned int FullFragmentMeshPointCountFromDensity(
    unsigned int mesh_density
  );

  /*
  Returns:
    Number of mesh points in a half sized fragment with the specified mesh density.
  */
  static unsigned int HalfFragmentMeshPointCountFromDensity(
    unsigned int mesh_density
  );

private:
  // This field overlaps with ON_FixedSizePoolElement.m_next when a fixed size pool is managing the fragments.
  // When m_reserved != 0, the framgment is uninitialized. 
  ON__UINT64 m_reserved;

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
  // will be > ON_SubDFace::MaximumEdgeCount.  
  // Catmull-Clark limit meshes:
  //   When the original SubD face is a quad, a full fragment is created and
  //   m_face_vertex_index[4] = {0,1,2,3}.
  //   When the original SuD face is an N-gon with N != 4, a partial fragment 
  //   is delivered and m_face_vertex_index[2] identifies the face vertex 
  //   for that fragment.  m_face_vertex_index[0,1,3] = a value > ON_SubDFace::MaximumEdgeCount
  unsigned short m_face_vertex_index[4];

  const class ON_SubDFace* SubDFace() const;

  const bool HasValidPointAndNormalGrid() const;

  /*
  Returns:
    True if the fragment covers the entire SubD face.
  */
  bool IsFullFaceFragment() const;

  /*
  Returns:
    True if the fragment covers a corner of the SubD face.
  */
  bool IsFaceCornerFragment() const;
  
  /*
  Returns:
    If IsFaceCornerFragment() is true, then the index of the face's vertex for the corner is returned.
    Otherwise, ON_UNSET_UINT_INDEX is returned.
  */
  unsigned int FaceCornerIndex() const;

  /*
  Returns:
    Number of fragments that cover this face. 1 for quads and N>= 3 for faces with N sides when N != 4.
  */
  unsigned int FaceFragmentCount() const;

  /*
  Returns:
    First fragment for this->m_face.
  */
  const ON_SubDMeshFragment* FirstFaceFragment() const;

  /*
  Returns:
    Last fragment for this->m_face.
  */
  const ON_SubDMeshFragment* LastFaceFragment() const;

  /*
  Parameters:
    bReturnLastFromFirstFirst - in
      If false and this is the first fragment, then nullptr is returned.
      If true and this is the first fragment, then LastFaceFragment() is returned.
  Returns:
    Previous fragment for this->m_face.
  */
  const ON_SubDMeshFragment* PreviousFaceFragment(
    bool bReturnLastFromFirstFirst
  ) const;

  /*
  Parameters:
    bReturnFirstFromLast - in
      If false and this is the last fragment, then nullptr is returned.
      If true and this is the last fragment, then FirstFaceFragment() is returned.
  Returns:
    Next fragment for this->m_face.
  */
  const ON_SubDMeshFragment* NextFaceFragment(
    bool bReturnFirstFromLast
  ) const;

  /*
  Parameters:
    fragments_capacity - in
      Capacity of the fragments[] array.
    fragments[] - out
      Fragments are returned here.
  */
  unsigned int GetFaceFragments(
    const ON_SubDMeshFragment** fragments,
    size_t fragments_capacity
    ) const;

  unsigned int GetFaceFragments(
    ON_SimpleArray<const ON_SubDMeshFragment*>& fragments
  ) const;

  /*
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
  */
  const ON_3dPoint VertexPoint(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const ON_3dPoint VertexPoint(
    ON_2udex grid2dex
  ) const;
  const ON_3dPoint VertexPoint(
    unsigned grid_point_index
  ) const;

  /*
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
  */
  const ON_3dVector VertexNormal(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const ON_3dVector VertexNormal(
    ON_2udex grid2dex
  ) const;
  const ON_3dVector VertexNormal(
    unsigned grid_point_index
  ) const;
  
  /*
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
    The texture coordinates calculated by iterpolating the m_ctrlnetT[] values.
  */
  const ON_3dPoint VertexTextureCoordinateFromCorners(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const ON_3dPoint VertexTextureCoordinateFromCorners(
    ON_2udex grid2dex
  ) const;
  const ON_3dPoint VertexTextureCoordinateFromCorners(
    unsigned grid_point_index
  ) const;
  
  /*
  Description:
    Get the texture coordinate for the specified fragment grid point.
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
    The texture coordinate for the specified fragment grid point.
    TextureCoordinateDimension() reports the number of coordinates to set.
    When it is 2, the z coordinate of the returned point is 0.0.
  */
  const ON_3dPoint VertexTextureCoordinate(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const ON_3dPoint VertexTextureCoordinate(
    ON_2udex grid2dex
  ) const;
  const ON_3dPoint VertexTextureCoordinate(
    unsigned grid_point_index
  ) const;

  /*
  Description:
    Set the texture coordinate for the specified fragment grid point.
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
    unsigned texture_coordinate_dimension - [in]
      2 or 3
    texture_coordinate - [in]
      Texture coordinates are mutable and are often modified on an otherwise const fragment.
  Returns:
    True if input was valid and the texture coordinate was set.
  */
  bool SetVertexTextureCoordinate(
    unsigned grid2dex_i,
    unsigned grid2dex_j,
    ON_3dPoint texture_coordinate
  ) const;
  bool SetVertexTextureCoordinate(
    ON_2udex grid2dex,
    ON_3dPoint texture_coordinate
  ) const;
  bool SetVertexTextureCoordinate(
    unsigned grid_point_index,
    ON_3dPoint texture_coordinate
  ) const;

  bool TextureCoordinatesExist() const;
  void SetTextureCoordinatesExist(bool TextureCoordinatesExist) const;

  /*
  Parameters:
    grid_side_index - [in]
      0 to 3 for quad grids.
      0 to 2 for tri grids
  Returns:
    The subd edge that is on the identified side of the grid.
  */
  const class ON_SubDEdgePtr SubDEdgePtr(
    unsigned int grid_side_index
    ) const;
  const class ON_SubDEdge* SubDEdge(
    unsigned int grid_side_index
    ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const class ON_SubDVertexPtr SubDVertexPtr(
    unsigned int grid_corner_index
    ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const class ON_SubDVertex* SubDVertex(
    unsigned int grid_corner_index
    ) const;


  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface location at the grid corner or ON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const ON_3dPoint CornerPoint(
    unsigned int grid_corner_index
    ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface normal at the grid corner or ON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const ON_3dVector CornerNormal(
    unsigned int grid_corner_index
  ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface frame at the grid corner or ON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const ON_Plane CornerFrame(
    unsigned int grid_corner_index
  ) const;

  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface location at the midde of the grid side or ON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const ON_3dPoint SidePoint(
    unsigned int grid_side_index
  ) const;

  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface normal at the grid corner or ON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const ON_3dVector SideNormal(
    unsigned int grid_side_index
  ) const;

  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface frame at the grid corner or ON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const ON_Plane SideFrame(
    unsigned int grid_side_index
  ) const;

  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface location at the center of the grid side or ON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const ON_3dPoint CenterPoint(
  ) const;

  const ON_3dVector CenterNormal() const;

  const ON_Plane CenterFrame() const;

private:
  bool Internal_GetFrameHelper(
    unsigned int P_dex,
    unsigned int Q_dex,
    ON_Plane& frame
  ) const;

public:

  /*
  Returns:
    Status of the face.
  */
  ON_ComponentStatus Status() const;

  bool Transform(
    const ON_Xform& xform
    );  

  ON_SubDMeshFragment* m_next_fragment;
  ON_SubDMeshFragment* m_prev_fragment;

  unsigned short m_face_fragment_count; // Number of fragments that will be delivered for this face.
  unsigned short m_face_fragment_index; // First fragment has index = 0. Last fragment has index = m_face_fragment_count-1.

  // The mesh fragment is a grid of quads.
  // There are m_side_count quad edges along each side of the tesselation,
  // There are a total of m_side_count X m_side_count quads, and
  // m_P_count = (m_side_count+1)*(m_side_count+1).

public:
  enum : unsigned
  {
    MaximumVertexCount = 0x3FFF
  };

  /*
  Returns:
    Number of vertices in the mesh fragment grid.
    VertexCount() should be identical to m_grid.GridPointCount().
    VertexCapacity() should be >= VertexCount().
  */
  unsigned VertexCount() const;

  /*
  Description:
    Sets number of fragment vertices being used (number of elements being used in the m_P[], m_N[], and m_T[] arrays).
  Parameters:
    vertex_count - [in]
      A value no larger than ON_SubDMeshFragment::MaximumVertexCount.
  */
  bool SetVertexCount(size_t vertex_count);

  /*
  Returns:
    Capactity for vertices in the mesh fragment grid.
    VertexCapacity() should be >= VertexCount().
    VertexCount() should be identical to m_grid.PointCount().
  */
  unsigned VertexCapacity() const;

  /*
  Description:
    Sets number of fragment vertices available (number of elements available in the m_P[], m_N[], and m_T[] arrays).
    The memory for the arrays is managed by something besides this ON_SubDManagedMeshFragment instance.
  Parameters:
    vertex_capacity - [in]
      A value no larger than ON_SubDMeshFragment::MaximumVertexCount.
  Returns:
    True if successful
  */
  bool SetUnmanagedVertexCapacity(size_t vertex_capacity);


  /*
  Description:
    Sets number of fragment vertices available (number of elements available in the m_P[], m_N[], and m_T[] arrays).
    The memory for the arrays is managed by something besides this ON_SubDManagedMeshFragment instance.
  Parameters:
    vertex_capacity - [in]
      A value no larger than ON_SubDMeshFragment::MaximumVertexCount.
  Returns:
    True if successful
  */
  bool ReserveManagedVertexCapacity(size_t vertex_capacity);

  /*
  Returns:
    True if the memory in the m_P[], m_N[], and m_T[] is managed by this ON_SubDManagedMeshFragment instance.
  */
  bool ManagedArrays() const;

  bool DeleteManagedArrays();

  /*
  Returns:
    True if the memory in the m_P[], m_N[], and m_T[] is managed by something besides this ON_SubDManagedMeshFragment instance.
  */
  bool UnmanagedArrays() const;

private:
  friend class ON_SubDManagedMeshFragment;
  friend class ON_SubDMeshImpl;
  // Number of grid vertices and capacity of arrays in certain conditions.
  enum : unsigned short
  {
    ValueMask = 0x3FFF, // maximum vertex count and capacity
    EtcMask = 0xC000,
    EtcControlNetQuadBit = 0x8000, // bit is on m_vertex_count_etc. Set means 4 m_ctrlnetP[] points are set.
    EtcTextureCoordinatesExistBit = 0x4000, // bit is on m_vertex_count_etc
    EtcManagedArraysBit = 0x8000, // bit is on m_vertex_capacity_etc. Set means this class manages the m_P[], m_N[], and m_T[] arrays.
  };
  mutable unsigned short m_vertex_count_etc;
  mutable unsigned short m_vertex_capacity_etc;
  static void Internal_Set3dPointArrayToNan(double* a, size_t a_count, size_t a_stride);

private:
  // corners for control net display in grid order (counter-clockwise quad order must swap [2] and[3])
  double m_ctrlnetP[4][3];

  // Normal used for shading the control net display.
  double m_ctrlnetN[3];

  // The fragment's default corner texture coordinates in grid order.
  //  m_ctrlnetT[0] (bbox min = lower left)
  //  m_ctrlnetT[1] (lower right)
  //  m_ctrlnetT[2] (upper left)
  //  m_ctrlnetT[3] (bbox max = upper right)
  mutable double m_ctrlnetT[4][3]; 

public:
  ///////////////////////////////////////////////////////////////////////////////////
  //
  // 3-dimensional grid vertex points
  //
  // Depending on the strides, m_P[], m_N[], and m_T[] can be separate or interlaced.
  //
  // The stride m_P_stride and memory m_P references is managed by some other class or function.
  // Never modify m_P_stride, m_P, or the values in m_P.
  // Use m_grid functions to get point indices and quad face indices.
  double* m_P; // surface points
  size_t m_P_stride;
  const double* PointArray(ON_SubDComponentLocation subd_appearance)const;
  size_t PointArrayStride(ON_SubDComponentLocation subd_appearance)const;
  unsigned PointArrayCount(ON_SubDComponentLocation subd_appearance) const;

public:

  /*
  Description:
    Create a one quad fragment with 
    m_P = this->m_ctrlnetP[], m_P_stride = 3
    m_N = this->m_ctrlnetN, m_N_stride = 0
    m_T = this->m_ctrlnetT, m_T_stride = 3
    NOTE WELL: 
    1) "this" must remain in scope and unchanged while the returned value
    is used because the returned value references memory in thism_ctrlnetX[] arrays.
    2) The next/prev pointers on the returned fragment are copied from this,
    but the returned value is not reciprocally referenced by next/prev in the linked list.
  Returns:
    A control net quad fragment that can be used locally when the SubDAppearance
    is ON_SubDComponentLocation::ControlNet.
    The points, normals, and texture_coordinates of the returned fragment
    are the control net quad points, normals, and texture coordinates
    of this fragment. m_grid is a single quad grid.
  */
  const ON_SubDMeshFragment ControlNetQuadFragmentForExperts() const;

  /*
  Parameters:
    bGridOrder - [in]
      If true, then points are returned in fragment grid order.
      Otherwise, points are returned in counter-clockwise quad order.
    quad_points - [out]
    quad_normal - [out]
  */
  bool GetControlNetQuad(
    bool bGridOrder,
    ON_3dPoint quad_points[4],
    ON_3dVector& quad_normal
  ) const;

  const ON_3dPoint ControlNetQuadPoint(
    bool bGridOrder,
    unsigned point_index
  ) const;

  /*
  Parameters:
    bGridOrder - [in]
      If true, then points are returned in fragment grid order.
      Otherwise, points are returned in counter-clockwise quad order.
    quad_points - [in]
  */
  void SetControlNetQuad(
    bool bGridOrder,
    const ON_3dPoint quad_points[4],
    ON_3dVector quad_normal
  );

  void UnsetControlNetQuad();

  const ON_BoundingBox SurfaceBoundingBox() const;
  const ON_BoundingBox ControlNetQuadBoundingBox() const;
  const ON_BoundingBox BoundingBox(ON_SubDComponentLocation subd_appearance) const;

public:
  /*
  Returns:
    If grid vertex points are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int PointCount() const;
  unsigned int PointCapacity() const;

  ///////////////////////////////////////////////////////////////////////////////////
  //
  // 3-dimensional grid vertex surface normals
  //
  // The m_N[] and m_P[] arrays are parallel. 
  // Depending on the strides, m_P[], m_N[], and m_T[] can be separate or interlaced.
  //
  // If m_N is not nullptr and m_N_stride>0, then m_N[] can accomodate up to m_P_capacity normals.
  // If m_N is not nullptr and m_N_stride=0, then m_N[] can accomodate a single normal (flat shaded polygon face).
  // The stride m_N_stride and memory m_N references is managed by some other class or function.
  // Never modify m_N_stride, m_N, or the values in m_N.
  // Use m_grid functions to get normal indices and quad face indices.
  // Note well: m_N_stride can be 0 when the normal is constant (control net face normal for example).
  double* m_N; // surface normals
  size_t m_N_stride;
  const double* NormalArray(ON_SubDComponentLocation subd_appearance)const;
  size_t NormalArrayStride(ON_SubDComponentLocation subd_appearance)const;
  unsigned NormalArrayCount(ON_SubDComponentLocation subd_appearance) const;

public:
  /*
  Returns:
    If grid vertex surface normals are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int NormalCount() const;
  unsigned int NormalCapacity() const;

  ///////////////////////////////////////////////////////////////////////////////////
  //
  // 3d vertex texture coordinates.
  //
  // Depending on the strides, m_P[], m_N[], and m_T[] can be separate or interlaced.
  //
  // If m_T is not nullptr and m_T_stride>0, then m_T[] can accomodate up to m_P_capacity textures coordinates.
  // Otherwise there are no texture coordinates.
  // Never modify m_T_stride, m_T.
  // Use m_grid functions to get texture indices and quad face indices.
  // Note well: m_T_stride can be 0 when the texture coordinate is constant (one color per face for example)
  mutable double* m_T;
  mutable size_t m_T_stride;

  const double* TextureCoordinateArray(ON_SubDComponentLocation subd_appearance)const;
  size_t TextureCoordinateArrayStride(ON_SubDComponentLocation subd_appearance)const;
  unsigned TextureCoordinateArrayCount(ON_SubDComponentLocation subd_appearance) const;   
public:
  /*
  Returns:
    If grid texture coordinates are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int TextureCoordinateCount() const;
  unsigned int TextureCoordinateCapacity() const;


  /*
  Description:
    Sets the values of each fragment's m_ctrlnetT[] member.
  Parameters:
    bSetTextureCoordinates
      If true, the texture coordinate corners are used ti set the fragment's m_T[]
      values after m_ctrlnetT[] is set.
  */
  void SetTextureCoordinateCorners(
    bool bGridOrder,
    const ON_2dPoint texture_coordinate_corners[4],
    bool bSetTextureCoordinates
  ) const;

  void SetTextureCoordinateCorners(
    bool bGridOrder,
    const ON_2dPoint texture_coordinate_corners[4],
    double s0,
    double s1,
    double t0,
    double t1,
    bool bSetTextureCoordinates
  ) const;

  void SetTextureCoordinateCorners(
    bool bGridOrder,
    const ON_3dPoint texture_coordinate_corners[4],
    bool bSetTextureCoordinates
  ) const;

  /*
  Description:
    Get the texture coordinate corners.
  Parameters:
    bGridOrder - [in]
      If true, then points are returned in fragment grid order.
      Otherwise, points are returned in counter-clockwise quad order.
    texture_coordinate_corners - [out]
  */

  bool GetTextureCoordinteCorners(
    bool bGridOrder,
    ON_3dPoint texture_coordinate_corners[4]
  ) const;

  /*
  Description:
    Set the texture coordinates in m_T[] from the values in m_ctrlnetT[].
  */
  void SetTextureCoordinatesFromCorners() const;

  // Normalized grid parameters useful for appling a texture to the grid are available
  // from m_grid functions.
  
  // Information to resolve m_P[], m_N[], and m_T[] into a grid of NxN quads.
  ON_SubDMeshFragmentGrid m_grid;

  const ON_SubDMeshFragmentGrid& Grid(ON_SubDComponentLocation subd_appearance) const;

  // 3d bounding box of grid vertex points.
  ON_BoundingBox m_surface_bbox;
public:
  /*
  Returns:
    Amount of memory needed for the fragment, the m_P[], and the m_N[] arrays.
  */
  static size_t SizeofFragment(unsigned int display_density);
};

class ON_CLASS ON_SubDManagedMeshFragment : public ON_SubDMeshFragment
{
public:
  ON_SubDManagedMeshFragment() ON_NOEXCEPT;
  ~ON_SubDManagedMeshFragment() ON_NOEXCEPT;
  ON_SubDManagedMeshFragment(const ON_SubDManagedMeshFragment&) ON_NOEXCEPT;
  ON_SubDManagedMeshFragment& operator=(const ON_SubDManagedMeshFragment&) ON_NOEXCEPT;

  static ON_SubDManagedMeshFragment Create(const ON_SubDMeshFragment& src) ON_NOEXCEPT;

#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDManagedMeshFragment( ON_SubDManagedMeshFragment&& ) ON_NOEXCEPT;

  // rvalue assignment operator
  ON_SubDManagedMeshFragment& operator=( ON_SubDManagedMeshFragment&& ) ON_NOEXCEPT;
#endif

  void Clear() ON_NOEXCEPT;

  void Destroy() ON_NOEXCEPT;

  bool ReserveCapacity(
    unsigned int mesh_density
    ) ON_NOEXCEPT;

private:
  void CopyHelper(const ON_SubDMeshFragment& src);
  size_t m_storage_capacity = 0;
  double* m_storage = nullptr;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDMesh
//

/// <summary>
/// ON_SubDMesh is used to store a traditional quad mesh of either
/// a SubgD surface or SubD control net. It is typically used for 
/// rendering and user interface selection cacluations.
/// It is a list of ON_SubDMeshFragment classes.
/// </summary>
class ON_CLASS ON_SubDMesh
{
#if defined(ON_SUBD_CENSUS)
private:
  ON_SubDMeshCensusCounter m_census_counter;
#endif

public:
  static const ON_SubDMesh Empty;

  /*
  Returns:
    A runtime number that changes if the limit mesh content changes.
    0: Empty limit mesh
  Remarks:
    This is a runtime number. It is not saved in archives and begins
    at 1 with each new runtime instance of the opennurbs library.
  */
  ON__UINT64 ContentSerialNumber() const;



  /*
  Description:
  Parameters:
    destination_mesh - [in]
      If destination_mesh is not nullptr, then the returned mesh
      will be store here. Otherwise the returned mesh will be 
      allocated with a call to new ON_Mesh().
  Returns:
    If this limit mesh is not empty, an ON_Mesh representation of this limit mesh.
    Othewise, nullptr.
  */
  ON_Mesh* ToMesh(
    ON_Mesh* destination_mesh
  ) const;


  /*
  Description:
  Parameters:
    mesh_density - [in]
      Larger numbers return denser meshes.
    destination_mesh - [in]
      If destination_mesh is not nullptr, then the returned mesh
      will be store here. Otherwise the returned mesh will be 
      allocated with a call to new ON_Mesh().
  Returns:
    If this limit mesh is not empty, an ON_Mesh representation of this limit mesh.
    Othewise, nullptr.
  */
  ON_Mesh* ToMesh(
    unsigned int mesh_density,
    ON_Mesh* destination_mesh
  ) const;

  /*
  Description:
  Parameters:
    frit - [in]
      A fragment iterator from an ON_SubDMesh or ON_SubD.
    mesh_density - [in]
      Larger numbers return denser meshes.
      MinimumMeshDensity() <= mesh_density <= MaximumMeshDensity()
    destination_mesh - [in]
      If destination_mesh is not nullptr, then the returned mesh
      will be store here. Otherwise the returned mesh will be 
      allocated with a call to new ON_Mesh().
  Returns:
    If this limit mesh is not empty, an ON_Mesh representation of this limit mesh.
    Othewise, nullptr.
  */
  static ON_Mesh* ToMesh(
    class ON_SubDMeshFragmentIterator& frit,
    unsigned int mesh_density,
    ON_Mesh* destination_mesh
  );

  ON_SubDMesh() = default;
  ~ON_SubDMesh() = default;
  ON_SubDMesh(const ON_SubDMesh&) = default;
  ON_SubDMesh& operator=(const ON_SubDMesh&) = default;

  
#if defined(ON_HAS_RVALUEREF)
  // rvalue copy constructor
  ON_SubDMesh( ON_SubDMesh&& ) ON_NOEXCEPT;
  // rvalue assignment operator
  ON_SubDMesh& operator=( ON_SubDMesh&& );
#endif

  ON_SubDMesh Copy() const;

  ON_SubDMesh& CopyFrom(
    const ON_SubDMesh& src
    );

  static void Swap(
    ON_SubDMesh& a,
    ON_SubDMesh& b
    );

  bool Transform( 
    const ON_Xform& xform
    );

  unsigned int DisplayDensity() const;
  ON_SubDDisplayParameters DisplayParameters() const;
  unsigned int FragmentCount() const;
  const ON_SubDMeshFragment* FirstFragment() const; // linked list of mesh fragments

  /*
  Parameters:
    face - [in]
  Returns:
    The first fragment for this face or nullptr if none is found.
    If the face is not a quad, then there will be multiple partial
    fragments for the face and this is the first on. The others
    follow using m_next_fragment.
  */
  const ON_SubDMeshFragment* FaceFragment(
  const class ON_SubDFace* face
  ) const;

  /*
  Description:
    Get the limit surface point location and normal for 
    the face's center from the limit mesh grid.
  Parameters:
    face - [in]
    P - [out]
      P = limit surface location or ON_3dPoint::NanPoint if not available.
    N - [out]
      N = limit surface unit normal or ON_3dVector::NanVector if not available.
  Returns:
    True if the point and normal were set from the limit mesh frament.
    False if the limit mesh fragment was not found and nan values were returned.
  */
  bool GetFaceCenterPointAndNormal(
    const class ON_SubDFace* face,
    double* P,
    double* N
  ) const;

  /*
  Description:
    Get the limit surface point location and normal for 
    the edge's midpoint from the limit mesh grid.
  Parameters:
    edge - [in]
    edge_face_index - [in]
      Index of the face to use for the normal. If the edge is a crease, then
      each attached face may have a different normal. Pass 0 when in doubt.
    P - [out]
      P = limit surface location or ON_3dPoint::NanPoint if not available.
    N - [out]
      N = limit surface unit normal or ON_3dVector::NanVector if not available.
  Returns:
    True if the point and normal were set from the limit mesh frament.
    False if the limit mesh fragment was not found and nan values were returned.
  */
  bool GetEdgeCenterPointAndNormal(
    const class ON_SubDEdge* edge,
    unsigned int edge_face_index,
    double* P,
    double* N
  ) const;
  
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
    The ON__INT_PTRs in the tree are const ON_SubDMeshFragment* pointers.
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
    Pretend this function and ON_SubDMeshImpl do not exist.
  Returns:
    Something that you are pretending does not exist.
  Remarks:
    It is intentional that the definition of ON_SubDMeshImpl class is not
    available in the opennurbs library interface (not in a header file).
    The size and design of ON_SubDMeshImpl will change constantly.
    If you choose to hack and whack so you can dereference an
    ON_SubDMeshImpl* pointer, then your code will crash unpredictably.
  */
  class ON_SubDMeshImpl* SubLimple() const;
  unsigned int SubLimpleUseCount() const;

private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  friend class ON_SubDMeshImpl;
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_impl_sp is private and all code that manages m_impl_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< class ON_SubDMeshImpl > m_impl_sp;
#pragma ON_PRAGMA_WARNING_POP

};


class ON_CLASS ON_SubDSectorSurfacePoint
{
public:
  // For performance reasons, the default the data members are
  // not initialized by the default constructor
  // Use = ON_SubDSectorSurfacePoint::Unset when initialization is required
  static const ON_SubDSectorSurfacePoint Unset; // all doubles are ON_UNSET_VALUE, all pointer are nullptr
  static const ON_SubDSectorSurfacePoint Nan;   // all doubles are ON_DBL_QNAN, all pointer are nullptr
  static const ON_SubDSectorSurfacePoint Zero;  // all doubles are 0.0, all pointer are nullptr
  
  /*
  Returns:
    true if m_limitP[0] is ON_UNSET_VALUE.
    false otherwise.
  */
  bool IsUnset() const;

  /*
  Returns:
    true if m_limitP[0] is a nan (like ON_DBL_QNAN).
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
  bool IsSet(
    bool bUndefinedNormalIsPossible
  ) const;

  bool Transform(
    const ON_Xform& xform
    );

  // limit surface point, tangents and normal
  double m_limitP[3];  // point
  double m_limitT1[3]; // first unit tangent
  double m_limitT2[3]; // second unit tangent
  double m_limitN[3];  // unit normal (same direction as m_limitT1 x m_limitT2)

  // When an ON_SubDVertex has a single sector, (ON_SubDVertex.IsSingleSectorVertex() is true),
  // these pointers are both null.
  // When an ON_SubDVertex has a multiple sectors, 
  // m_sector_face is the "first" face in the sector and
  // m_next_sector_limit_point is used to create a linked list.
  // (The "first" face in a sector is the one ON_SubDSectorIterator.IncrementToCrease(-1) returns.)
  const class ON_SubDSectorSurfacePoint* m_next_sector_limit_point; // nullptr for vertices with one sector
  const class ON_SubDFace* m_sector_face;                         // nullptr for vertices with one sector
};

////////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentBase
//
class ON_CLASS ON_SubDComponentBase
{
public:
  static const ON_SubDComponentBase Unset;
  
  /*
  Returns:
    True if this component is active in its parent subd or other
    relevent context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;
    
public:
  ON_SubDComponentBase() = default;
  ~ON_SubDComponentBase() = default;
  ON_SubDComponentBase(const ON_SubDComponentBase&) = default;
  ON_SubDComponentBase& operator=(const ON_SubDComponentBase&) = default;

public:
  static int CompareId(
    const ON_SubDComponentBase* lhs,
    const ON_SubDComponentBase* rhs
  );


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

  // Id assigned to this component. NEVER MODIFY THE m_id VALUE.  
  // It is assigned by allocators and used to find the component 
  // from an ON_COMPONENT_INDEX.
  unsigned int m_id = 0;

private:
  // The m_archive_id must be immediately after the m_id field.
  // A value of ON_UNSET_UINT_INDEX indicate the component was
  // in use and then deleted. See ON_SubDHeap Return...() functions
  // for more details.
  mutable unsigned int m_archive_id = 0;  
  
public:

  /*
  Returns:
    Subdivision level (0 to 255)
  */
  unsigned const SubdivisionLevel() const;

  /*
  Parameters:
    level - [in]
      Subdivision level (0 to 255)
  */
  void SetSubdivisionLevel(unsigned level);

  
public:
  const ON_ComponentStatus Status() const;


public:
  mutable ON_ComponentStatus m_status = ON_ComponentStatus::NoneSet;

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  /*
  Returns:
    The current value of the component mark bits ( m_status->MarkBits() ).
  Remarks:
    Mark() and MarkBits() are independent.

    SubD components have a mutable runtime mark bits that can be used
    in any context where a single thread cares about the mark bits value.
    Any code can use MarkBits() at any time. You cannot assume that
    other functions including const will not change their value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state. 

    MarkBits() is used in more complex calculations where the single true/false
    provided by Mark() is not sufficient. Typically MarkBits() is used when
    a collecection of components needs to be partitioned into more than two 
    sets or when the value of Mark() cannot be changed.
  */
  ON__UINT8 MarkBits() const;

  /*
  Returns:
    Set the component mark bits ( m_status->SetMarkBits( mark_bits ) ).
  Remarks:
    Mark() and MarkBits() are independent.

    SubD components have a mutable runtime mark bits that can be used
    in any context where a single thread cares about the mark bits value.
    Any code can use MarkBits() at any time. You cannot assume that
    other functions including const will not change their value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.

    MarkBits() is used in more complex calculations where the single true/false
    provided by Mark() is not sufficient. Typically MarkBits() is used when
    a collecection of components needs to be partitioned into more than two
    sets or when the value of Mark() cannot be changed.
  Returns:
    Input value of MarkBits().
  */
  ON__UINT8 SetMarkBits(
    ON__UINT8 mark_bits
  ) const;

  /*
  Returns:
    Set the component mark bits to 0 ( m_status->SetMarkBits( 0 ) ).
  Remarks:
    Mark() and MarkBits() are independent.

    SubD components have a mutable runtime mark bits that can be used
    in any context where a single thread cares about the mark bits value.
    Any code can use MarkBits() at any time. You cannot assume that
    other functions including const will not change their value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specfic state.

    MarkBits() is used in more complex calculations where the single true/false
    provided by Mark() is not sufficient. Typically MarkBits() is used when
    a collecection of components needs to be partitioned into more than two
    sets or when the value of Mark() cannot be changed.
  Returns:
    Input value of MarkBits().
  */
  ON__UINT8 ClearMarkBits() const;


public:
  
  //////////////////////////////////////////////////////////////
  //
  // Saved subdivision point
  //
  /*
  Description:
    Set the saved subdivision point.
  Parameters:
    subdivision_point - [in]
      includes displacement if it exists
  Returns:
    true if successful
  */
  bool SetSavedSubdivisionPoint(
    const double subdivision_point[3]
    ) const;

  bool GetSavedSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  void ClearSavedSubdivisionPoint() const;

  /*
  Returns:
    Saved subdivision point. If a point has not been saved,
    ON_3dPoint::NanPoint is returned.
  */
  const ON_3dPoint SavedSubdivisionPoint() const;

  /*
  Returns:
    True if the subdivision point is cached.
  */
  bool SavedSubdivisionPointIsSet() const;

  //////////////////////////////////////////////////////////////
  //
  // displacement applied to subdivision point
  //
  bool SetSubdivisionDisplacement(
    const double displacement[3]
    );

  bool GetSubdivisionDisplacement(
    double displacement[3]
    ) const;
  
  /*
  Returns:
    subdivision displacement. If a displacement has not been set,
    ON_3dPoint::ZeroVector is returned.
  */
  const ON_3dVector SubdivisionDisplacement() const;

  /*
  Returns:
    True if the subdivision displacment vector is valid and not zero.
  */
  bool SubdivisionDisplacementIsNonzero() const;


  void ClearSubdivisionDisplacement() const;

protected:
  friend class ON_Internal_SubDFaceMeshFragmentAccumulator;
  friend class ON_SubDHeap;
  enum SavedPointsFlags : unsigned char
  {
    // if ( 0 != (m_saved_points_flags & SubDDisplacementVIsSet), then m_displacementV is set.
    SubdivisionDisplacementBit = 0x20,

    // if ( 0 != (m_saved_points_flags & SubdivisionPointBit), then m_cache_subd_P is set.
    SubdivisionPointBit = 0x40,

    // if ( 0 != (m_saved_points_flags & SurfacePointBit), then ON_subDVertex.m_limit* values are set.
    // ON_SubDVertex: Set means one or more sector limit surface points are saved in ON_SubDVertex.m_limit_point.
    // ON_SubDEdge: Set means the limit surface NURBS curve control points are cached.
    // ON_SubDFace: Set means limit surface mesh fragments are saved in ON_SubDFace.m_surface_mesh_fragments.
    // Unset means any information in the fields identified above is invalid and must be recalculated.
    SurfacePointBit = 0x80,

    // ControlNetFragmentBit | SubdivisionPointBit | SubdivisionDisplacementBit | SurfacePointBit
    CachedPointMask = 0xF0
  };

  enum ModifiedFlags : unsigned char
  {
    // if ( 0 != (m_saved_points_flags & Modified1Bit), then the component has been modified and
    // cached subdivision information needs to be recalculated.
    Modified1Bit = 0x01,

    // if ( 0 != (m_saved_points_flags & Modified2Bit), then the component is adjacent to
    // a modified component and cached subdivision information needs to be recalculated.
    Modified2Bit = 0x02,

    // ModifiedFlagsMask = Modified1Bit | Modified2Bit
    // if ( 0 != (m_saved_points_flags & ModifiedFlagsMask), then any cached subdivision information
    // on that component needs to be recalculated.
    ModifiedFlagsMask = 0x03
  };

  // m_saved_points_flags is a bit field based on ON_SubDComponentBase::SavePointsFlags values.
  // GetSurfacePoint( bUseSavedSurfacePoint=true ) can change the value of m_saved_points_flags
  void Internal_SetSavedSurfacePointFlag(bool bSavedSurfacePointFlag) const;
  void Internal_SetModified1Flag() const;
  void Internal_SetModified2Flag() const;

  /*
  Returns:
    True if Modified1Bit or Modified2Bit is set.
  */
  bool Internal_Modified1IsSet() const;

  /*
  Returns:
    True if Modified1Bit or Modified2Bit is set.
  */
  bool Internal_Modified1or2IsSet() const;

  void Internal_ClearModifiedFlags() const;
  mutable unsigned char m_saved_points_flags = 0U;

  unsigned char m_level = 0U;
public:

  // All the faces with the same nonzero value of m_group_id are in the same "group".
  // SDK interface on ON_SubD will be added after we get a better idea of how this
  // feature will be used.
  mutable unsigned int m_group_id = 0U;

protected:
  /*
  Description:
    Clears the flags indicating the saved subdivision point and surface point information 
    is current.
  */
  void Internal_ClearSubdivisionPointAndSurfacePointFlags() const;

  /*
  Description:
    Clears the flag indicating the saved subdivision point is current.
  */
  void Internal_ClearSubdivisionPointFlag() const;

  /*
  Description:
    Clears the flag indicating the saved surface point information is current.
  */
  void Internal_ClearSurfacePointFlag() const;

  bool Internal_SubdivisionPointFlag() const;

  bool Internal_SurfacePointFlag() const;

  void Internal_TransformComponentBase(bool bTransformationSavedSubdivisionPoint, const class ON_Xform& xform);
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

private:
  ON_SubDComponentPtr m_reserved; // reserved for the symmetric component when symmetry is implemnted.
};

////////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertexEdgeProperties
//
class ON_CLASS ON_SubDVertexEdgeProperties
{
public:
  ON_SubDVertexEdgeProperties() = default;
  ~ON_SubDVertexEdgeProperties() = default;
  ON_SubDVertexEdgeProperties(const ON_SubDVertexEdgeProperties&) = default;
  ON_SubDVertexEdgeProperties& operator=(const ON_SubDVertexEdgeProperties&) = default;

public:
  static const ON_SubDVertexEdgeProperties Zero; // all member values are zero.

  /*
  Returns:
    True if there are no null edges and there are two edges with a single face and all remaining edges have two faces.
  Remarks:
    Tags are ignored.
  */
  bool HasInteriorVertexTopology() const;

  /*
  Returns:
    True if there are no null edges, exactly two boundary edges, and any other edges have two faces.
  Remarks:
    Tags are ignored.
  */
  bool HasBoundaryVertexTopology() const;

  /*
  Returns:
    HasInteriorVertexTopology() || HasBoundaryVertexTopology().
  */
  bool HasManifoldVertexTopology() const;

  /*
  Returns:
    True if there are no null edges and there is an edge with zero faces or an edge with three or more faces.
  Remarks:
    Tags are ignored.
  */
  bool HasNonmanifoldVertexTopology() const;

  /*
  Returns:
    Number of edges.
  */
  unsigned EdgeCount() const;

public:
  // Number of null edges
  unsigned short m_null_edge_count = 0;


  /////////////////////////////////////////////////////
  //
  // Vertex attached component counts
  //

  // vertex->m_edge_count 
  unsigned short m_edge_count = 0;

  // vertex->m_face_count 
  unsigned short m_face_count = 0;

  /////////////////////////////////////////////////////
  //
  // Edge tag counts
  //

  // Number of edges tags ON_SubD::EdgeTag::Unset
  unsigned short m_unset_edge_count = 0;

  // Number of edges tags ON_SubD::EdgeTag::Smooth or ON_SubD::EdgeTag::SmoothX
  unsigned short m_smooth_edge_count = 0;

  // Number of edges tags ON_SubD::EdgeTag::Crease
  unsigned short m_crease_edge_count = 0;

  /////////////////////////////////////////////////////
  //
  // Edge topology counts
  //

  // Number of wire edges (0 attached faces)
  unsigned short m_wire_edge_count = 0;

  // Number of boundary edges (1 attached face)
  unsigned short m_boundary_edge_count = 0;

  // Number of interior edges (2 attached faces)
  unsigned short m_interior_edge_count = 0;

  // Number of nonmanifold edges (3 or more attached faces)
  unsigned short m_nonmanifold_edge_count = 0;


  /////////////////////////////////////////////////////
  //
  // Edge face counts
  //

  // Minimum value of attached edges's m_face_count.
  unsigned short m_min_edge_face_count = 0;

  // Maximum value of attached edges's m_face_count.
  unsigned short m_max_edge_face_count = 0;
};

////////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertex
//
class ON_CLASS ON_SubDVertex : public ON_SubDComponentBase
{
public:
  ON_SubDVertex() = default;
  ~ON_SubDVertex() = default;
  ON_SubDVertex(const ON_SubDVertex&) = default;
  ON_SubDVertex& operator=(const ON_SubDVertex&) = default;

public:
  unsigned int VertexId() const;


public:
  /*
  Description:
    Clears saved subdivision and limit surface information 
    for this component.
  */
  void ClearSavedSubdivisionPoints() const;  

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
  Parameters:
    bIncludeEdges - [in]
      If true, then attached edges are included.
    bIncludeFaces - [in]
      If true, then attached faces are included.
  Returns:
    A ON_ComponentStatusLogicalOr() of this vertex's status and the 
    specified attached components.
  See Also:
    ON_SubDComponentBase::Status()
  */
  const ON_ComponentStatus NeighborhoodStatusLogicalOr(
    bool bIncludeEdges,
    bool bIncludeFaces
  ) const;

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

  bool SetControlNetPoint(
    ON_3dPoint control_net_point,
    bool bClearNeighborhoodCache
  );


  ON_BoundingBox ControlNetBoundingBox() const;


public:
  const ON_COMPONENT_INDEX ComponentIndex() const;
  const ON_SubDComponentPtr ComponentPtr() const;

public:
  // m_prev_vertex, m_next_vertex must be the first data members of ON_SubDVertex
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

  /*
  Description:
    Expert user tool to remove an edge from the vertex's m_edges[] array.
  Remarks:
    Does not modify the edge. If the vertex is referenced by the edge,
    then the vertex must be removed from edge's m_vertex[] array.
  */
  bool RemoveEdgeFromArray(
    const class ON_SubDEdge* f
    );

  /*
  Description:
    Expert user tool to remove a face from the vertex's m_faces[] array.
  Remarks:
    Does not modify the face or any edges. The faces edges must be updated accordingly.
  */
  bool RemoveFaceFromArray(
    const class ON_SubDFace* f
    );

  /*
  Parameters:
    bApplyInputTagBias - [in]
      If bApplyInputTagBias is true, the current tag value is used
      to choose between possible output results. When in doubt, pass false.
    bReturnBestGuessWhenInvalid
      If bReturnBestGuessWhenInvalid is true and the topology and current edges
      tags do not meet the conditions for a valid vertex, then a best guess for 
      a vertex tag is returned. Otherwise ON_SubD::VertexTag::Unset is returned.
      When in doubt pass false and check for unset before using.
  Returns:
    The suggested value for this vertices tag based on its current tag value and
    its current edges. Assumes the vertex and edge topology are correct and the 
    edge tags are correctly set.
  */
  ON_SubD::VertexTag SuggestedVertexTag(
    bool bApplyInputTagBias,
    bool bReturnBestGuessWhenInvalid
  ) const;

public:
  double m_P[3]; // vertex control net location
  
  ///<summary>
  /// The SubD vertex control net point.
  ///</summary>
  const ON_3dPoint ControlNetPoint() const;

private:
  // Cached limit point and limit normal
  // GetSurfacePoint( bUseSavedSurfacePoint=true ) can change the value of m_limit_point.
  // If the limit point is set and vertex has a single sector, then 
  // m_limit_point.m_sector_face = nullptr and m_limit_point.m_next_sector_limit_point = nullptr.
  // If the limit point is set and vertex has a multiple sectors, then 
  // m_limit_point.m_sector_face = first face in the sector.
  // If multiple limit points are set, then they are in a linked list
  // traversed using the ON_SubDSectorSurfacePoint.m_next_sector_limit_point.
  // The second and any additional limit points are managed by a fixed size pool.
  // Calling ClearSurfacePoint() will return these to the pool.
  mutable ON_SubDSectorSurfacePoint m_limit_point = ON_SubDSectorSurfacePoint::Unset;

public:
  enum : unsigned int
  {
    MaximumEdgeCount = 0xFFF0U,
    MaximumFaceCount = 0xFFF0U
  };

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

  /*
  Description:
    Expert user tool to replace reference to old_face with a reference to new_face.
  Parameters:
    old_face = [in]      
      Cannot be nullptr.
    new_face = [in]
      If new_face is nullptr, old_face is simply removed.
  Returns:
    If the replacement was successful, then the m_faces[] array index where old_face/new_face replacement occured is returned.
    Otherwise ON_UNSET_UINT_INDEX is returned.
  Remarks:
    No modifications are made to old_face or new_face.
  */
  unsigned int ReplaceFaceInArray(
    const ON_SubDFace* old_face,
    const ON_SubDFace* new_face
    );

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
  bool IsDartOrCreaseOrCorner() const;

  /*
  Returns:
    True if m_vertex_tag is ON_SubD::VertexTag::Smooth or ON_SubD::VertexTag::Dart.
  */
  bool IsSmoothOrDart() const;

  const ON_SubDVertexEdgeProperties EdgeProperties() const;

  /*
  Parameters:
    eptr0 - [out]
    eptr1 - [out]
      If a vertex has exactly two attached edges, each of which has a single attached face,
      then these edges are returned in the order the appear in the vertex's edge list. 
      (RelativeVertex(0) = this vertex). Othwerise the parameters are set to null.
  Returns:
    True if the vertex has exactly two attached edges, each of which has a single attached face.
    False otherwise.
  */
  bool GetBoundaryVertexEdges(
    ON_SubDEdgePtr* eptr0,
    ON_SubDEdgePtr* eptr1
  ) const;

  /*
  Parameters:
    vei0 - [out]
    vei1 - [out]
      If a vertex has exactly two attached edges, each of which has a single attached face,
      then the indices of those edges in the vertex's edge list are returned. 
      Othewise ON_UNSET_UINT_INDEX is returned.
  Returns:
    True if the vertex has exactly two attached edges, each of which has a single attached face.
    False otherwise.
  */
  bool GetBoundaryVertexEdgeIndices(
    unsigned* vei0,
    unsigned* vei1
  ) const;

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
  bool IsStandard() const;

  /*
  Description:
    A single sector vertex is a smooth vertex, a dart vertex, or a manifold boundary
    vertex.
  Returns:
    True if the vertex has a single sector and, consequently, a single
    limit surface normal.

  */
  bool IsSingleSectorVertex() const;

  /*
  Description:
    A manifold boundary vertex is a crease or corner vertex with 2 creased edges, 
    each attached to a single face, and all other edges are smooth edges attached 
    to two faces. There is a single sector at a manifold boundary vertex.
  Returns:
    True if the vertex has a single sector and, consequently, a single
    limit surface normal.

  */
  bool IsManifoldBoundaryVertex() const;

  /*
  Description:
    A vertex has interior vertex topology if
    EdgeCount() >= 2,
    EdgeCount() == FaceCount(),
    and every attached edge has two attached faces.
  Returns:
    True if the vertex has interior vertex toplology.
  Remarks:
    Tags are ignored. This property is often used during construction
    and modification when tags are not set.
  */
  bool HasInteriorVertexTopology() const;

  /*
  Description:
    A vertex has boundary vertex topology if
    EdgeCount() >= 2,
    EdgeCount() == 1+FaceCount(),
    two attached edges are attached to one face,
    the remaining edges are attached to two faces.
  Returns:
    True if the vertex has interior vertex toplology.
  Remarks:
    Tags are ignored. This property is often used during construction
    and modification when tags are not set.
  */
  bool HasBoundaryVertexTopology() const;

  /*
  Returns:
    If this vertex has two boundary edges, they are returned in the pair with
    BoundaryEdgePair().First().EdgePtr().RelativeVetex(0) and
    BoundaryEdgePair().Second().EdgePtr().RelativeVetex(0)
    equal to this vertex.
    Otherwise ON_SubDComponentPtrPair::Null is returned.
  */
  const ON_SubDComponentPtrPair BoundaryEdgePair() const;

  /*
  Returns:
    If this vertex has two creased edges, they are returned in the pair.
    Otherwise ON_SubDComponentPtrPair::Null is returned.
  */
  const ON_SubDComponentPtrPair CreasedEdgePair(
    bool bInteriorEdgesOnly
  ) const;


  /*
  Returns:
    If this vertex has one creased edge, it is returned.
    Otherwise ON_SubDEdgePtr::Null is returned.
  */
  const ON_SubDEdgePtr CreasedEdge(
    bool bInteriorEdgesOnly
  ) const;


  /*
  Parameters:
    bUseSavedSubdivisionPoint - [in]
      If there is a saved subdivision point and bUseSavedSubdivisionPoint
      is true, then the saved value is returned.
    subdivision_point - [out]
      The SubD vertex Catmull-Clark subdivision point is returned here.
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  ///<summary>
  /// The SubD vertex Catmull-Clark subdivision point.
  ///</summary>
  const ON_3dPoint SubdivisionPoint() const;

  /*
  Description:
    Evaluates the Catmull-Clark subdivision point ignoring all cached information.
    This function is typically used in testing and debugging code and 
    in ordinary cases, it is faster and better to call SubdivisionPoint() 
    or GetSubdivisionPoint().
  Parameters:
    subdivision_point - [out]
      The vertex Catmull-Clark subdivision point is returned here.
  */
  bool EvaluateCatmullClarkSubdivisionPoint(
    double subdivision_point[3]
  ) const;

  /*
  Parameters:
   sector_face - [in]
      A face in the sector of interest
    limit_point - [out]
  Returns:
    true if successful
  */
  bool GetSurfacePoint(
    const ON_SubDFace* sector_face,
    class ON_SubDSectorSurfacePoint& limit_point
    ) const;

  bool GetSurfacePoint(
    const ON_SubDFace* sector_face,
    bool bUndefinedNormalIsPossible,
    class ON_SubDSectorSurfacePoint& limit_point
    ) const;

  /*
  Description:
    If there is a saved limit surface point, then its location is returned in surface_point[].
  Parameters:
    surface_point - [out]
  Returns:
    True if a saved limit surface point is returned.
    False if there is no saved limit surface point. The input value of surface_point[] is not changed.
  */
  bool GetSavedSurfacePoint(
    double surface_point[3]
  ) const;

  bool GetSurfacePoint(
    double surface_point[3]
  ) const;

  ///<summary>
  /// The SubD vertex Catmull-Clark limit surface point.
  ///</summary>
  const ON_3dPoint SurfacePoint() const;

  /*
  Parameters:
    subd_appearance - [in]
      If ON_SubDComponentLocation::ControlNet, then this->ControlNetPoint() is returned.
      If ON_SubDComponentLocation::Surface, then this->SurfacePoint() is returned.
      Otherwise ON_3dPoint::NanPoint is returned.
  */
  const ON_3dPoint Point(ON_SubDComponentLocation subd_appearance) const;


  /*
  Description:
    Save limit surface point and limit normal for future use.
  Parameters:
    bUndefinedNormalIsPossible - [in]
      true if an undefined normal might occur
    surface_point - [in]
  Returns:
    true if successful
  Remarks:
    ClearSavedSubdivisionPoints() clears any saved limit points.
  */
  bool SetSavedSurfacePoint(
    bool bUndefinedNormalIsPossible,
    const ON_SubDSectorSurfacePoint surface_point
    ) const;

  void ClearSavedSurfacePoints() const;
   
  /*
  Returns:
    true
      The vertex limit point and at least one sector normal are saved for Catmull-Clark quad subdivision.
  */
  bool SurfacePointIsSet() const;


  const ON_Plane VertexFrame(
    ON_SubDComponentLocation subd_appearance
  ) const;


  /*
  Description:
    Call this function if the vertex is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void VertexModifiedNofification() const;
  
  /*
  Returns:
    Number of edges attached to this vertex with Edge().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedEdgeCount() const;

  /*
  Description:
    Clears all marks on edges.
  Returns:
    true if all edges are not null.
    false if any edges are null.
  */
  bool ClearEdgeMarks() const;

  /*
  Returns:
    Number of faces attached to this vertex with Face().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedFaceCount() const;


  /*
  Description:
    Clears all marks on faces.
  Returns:
    true if all faces are not null.
    false if any faces are null.
  */
  bool ClearFaceMarks() const;

  /*
  Returns:
    Minimum number of edges for any face attached to this vertex.
  */
  unsigned int MinimumFaceEdgeCount() const;

  /*
  Returns:
    Maximum number of edges for any face attached to this vertex.
  */
  unsigned int MaximumFaceEdgeCount() const;


  /*
  Returns:
    Minimum number of faces for any edge attached to this vertex.
  */
  unsigned int MinimumEdgeFaceCount() const;

  /*
  Returns:
    Maximum number of faces for any edge attached to this vertex.
  */
  unsigned int MaximumEdgeFaceCount() const;

  /*
  Description:
    Expert user tool to unset ON_SubEdge.m_sector_coefficent[] values for
    edges attached to this vertex.
  Parameters:
    relative_edge_end_dex - [in]
      0: unset vertex edge sector coefficient at the end where the edges
      attaches to this vertex.
      1: unset vertex edge sector coefficient at the end where the edges
      attaches to the other vertex.
      2: unset vertex edge sector coefficients at both ends of the edge.
  */
  void UnsetSectorCoefficientsForExperts(
    unsigned int relative_edge_end_dex
  ) const;

private:
  static bool Internal_GetCatmullClarkSubdivisionPoint(
    const class ON_SubDVertex* vertex, // smooth or dart
    double vertex_point[3]
    );

  static unsigned int Internal_GetFacePointSum(
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
  static bool Internal_GetGeneralQuadSubdivisionPoint(
    const class ON_SubDVertex* vertex,
    double vertex_point[3]
    );

private:
  friend class ON_SubDArchiveIdMap;
  friend class ON_SubDEdge;
  friend class ON_SubDFace;
  void CopyFrom(
    const ON_SubDVertex* src,
    bool bCopyEdgeArray,
    bool bCopyFaceArray,
    bool bCopySurfacePointList
    );
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdge
//
class ON_CLASS ON_SubDEdge : public ON_SubDComponentBase
{
public:
  ON_SubDEdge() = default;
  ~ON_SubDEdge() = default;
  ON_SubDEdge(const ON_SubDEdge&) = default;
  ON_SubDEdge& operator=(const ON_SubDEdge&) = default;

public:
  unsigned int EdgeId() const;

public:
  /*
  Description:
    Clears saved subdivision and limit surface information 
    for this component.
  */
  void ClearSavedSubdivisionPoints() const;  

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
  Parameters:
    bIncludeVertices - [in]
      If true, then attached vertices are included.
    bIncludeFaces - [in]
      If true, then attached faces are included.
  Returns:
    A ON_ComponentStatusLogicalOr() of this vertex's status and the 
    specified attached components.
  See Also:
    ON_SubDComponentBase::Status()
  */
  const ON_ComponentStatus NeighborhoodStatusLogicalOr(
    bool bIncludeVertices,
    bool bIncludeFaces
  ) const;

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

  const ON_BoundingBox ControlNetBoundingBox() const;

  const ON_Plane CenterFrame(
    ON_SubDComponentLocation subd_appearance
  ) const;



  /*
  Description:
    Call this function if the edge is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void EdgeModifiedNofification() const;

  /*
  Description:
    Expert user tool to unset sector coefficients.
  */
  void UnsetSectorCoefficientsForExperts() const;

  /*
  Description:
    Expert user tool to set sector coefficients.
  Returns:
    True if values were modified.
  */
  bool UpdateEdgeSectorCoefficientsForExperts(
    bool bUnsetEdgeSectorCoefficientsOnly
    );

public:
  const ON_COMPONENT_INDEX ComponentIndex() const;
  const ON_SubDComponentPtr ComponentPtr() const;

public:
  // m_prev_edge, m_next_edge must be the first data members of ON_SubDEdge
  const class ON_SubDEdge* m_prev_edge = nullptr; // linked list of edges on this level
  const class ON_SubDEdge* m_next_edge = nullptr; // linked list of edges on this level

public:
  // When checking the edge tag, it is important to consider what
  // should happen in the ON_SubD::EdgeTag::SmoothX case.  It is strongly
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
  enum : unsigned int
  {
    MaximumFaceCount = 0xFFF0U
  };
  unsigned short m_face_count = 0;
  unsigned short m_facex_capacity = 0;
  ON_SubDFacePtr m_face2[2] = {};
  ON_SubDFacePtr* m_facex = nullptr;

  // m_vertex[0] = vertex at the start of the edge.
  // m_vertex[1] = vertex at the end of the edge.
  const class ON_SubDVertex* m_vertex[2] = {};

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
  // value for the tagged end is calculated by ON_SubDSectorType::SectorCoefficient().
  // tagged_weight*tagged_vertex + (1.0 - tagged_weight)*untagged_vertex
  // is used when combining the edge ends.
  // The edge's subdivision vertex will be tagged as ON_SubD::VertexTag::Smooth
  // and both subdivision edges will be tagged as ON_SubD::EdgeTag::Smooth.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::SmoothX, then the edge
  // must have exactly two neighboring faces,
  // both vertices must be tagged and the m_sector_coefficient[]
  // values are calculated by ON_SubDSectorType::SectorCoefficient().
  // When the edge is subdivided, the midpoint of the edge is the 
  // location of the edge.s subdivision point.
  // The edge's subdivision vertex will be tagged as ON_SubD::VertexTag::Smooth
  // and both subdivision edges will be tagged as ON_SubD::EdgeTag::Smooth.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::Smooth
  // and both end vertices are tagged, that is a severe error
  // condition and the edge is subdivided at its midpoint.
  //
  // If the value of m_edge_tag is ON_SubD::EdgeTag::SmoothX
  // and both end vertices are not tagged, that is a severe error
  // condition and the edge is subdivided at its midpoint.
  //
  // m_sector_coefficient[tagged_end] = 1/2 + 1/3*cos(theta)
  // where "theta" = tagged end "theta" (which depends on vertex tag (dart/crease/corner), 
  // the number of faces in the sector, and the control net crease angle when the tagged end is a corner.  
  //
  // The name "sector coefficient" is used because the value is a property of the
  // vertex's sector (every smooth edge inside a vertex sector has the same value at the tagged vertex).
  // The sector coefficient does not change which a subdivision is applied.
  double m_sector_coefficient[2] = {};

  // If m_edge_tag is not ON_SubD::EdgeTag::Sharp, then m_sharpness is ignored.
  // If m_edge_tag is ON_SubD::EdgeTag::Sharp, then m_sharpness controls how hard/soft
  // the edge appears.  
  // The behavior of a "sharp" edge with m_sharpness = 1 is identical to a crease edge.
  // A "sharp" edge with m_sharpness = 0 is identical to a smooth edge.
  // For this reason, m_sharpness must be > 0 and < 1.
  double m_sharpness = 0.0; 

private:
  // Cached limit curve
  // GetEdgeSurfaceCurveControlPoints( bUseSavedSurfacePoint=true ) can change the value of m_limit_curve.
  // If 0 != ON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags), then
  // m_limit_curve is the edge's limit surface curve.
  // The memory is managed by the parent ON_SubD. 
  // If 0 == ON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags),
  // then any information in m_limit_mesh_fragments is dirty
  // and should not be used.
  // ClearSavedSubdivisionPoints() zeros
  // the appropriate bit of m_saved_points_flags.

  friend class ON_Internal_SubDFaceMeshFragmentAccumulator;
  friend class ON_SubDHeap;
  mutable class ON_SubDEdgeSurfaceCurve* m_limit_curve = nullptr;

public:

  /*
  Returns:
    Number of distinct non-nullptr vertices.
    If the edge is valid, this will be 2.
  */
  unsigned int VertexCount() const;
   
  unsigned int FaceCount() const;

  /*
  Parameters:
    bRequireSameFaceOrientation - [in]
      If true, the attached faces must use the edge with opposite directions (oriented manifold).
  Returns:
    True if the edge has two distinct faces.
  */
  bool HasInteriorEdgeTopology(
    bool bRequireOppositeFaceDirections
  ) const;

  /*
  Parameters:
    bRequireSameFaceOrientation - [in]
      If true, the attached faces must use the edge with opposite directions (oriented manifold).
  Returns:
    True if the edge has two distinct faces.
  */
  bool HasBoundaryEdgeTopology() const;

  const ON_SubDFacePtr FacePtr(
    unsigned int i
    ) const;

  const ON_SubDFacePtr FacePtrFromFace(
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

  /*
  Description:
    Expert user tool to replace reference to old_face with a reference to new_face.
    Existing orientation is copied.  No changes are made to old_face and new_face and
    their edge references must be updated accordingly.
  Parameters:
    old_face = [in]
      Cannot be nullptr.
    new_face = [in]
      If new_face is nullptr, old_face is simply removed.
  Returns:
    If the replacement was successful, then the m_faces[] array index where old_face/new_face replacement occured is returned.
    Otherwise ON_UNSET_UINT_INDEX is returned.
  Remarks:
    No modifications are made to old_face or new_face.
  */
  unsigned int ReplaceFaceInArray(
    const ON_SubDFace* old_face,
    const ON_SubDFace* new_face
  );

  const class ON_SubDVertex* Vertex(
    unsigned int i
    ) const;

  unsigned int VertexArrayIndex(
    const class ON_SubDVertex* v
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
  See Also:
    ON_SubDEdge.NeighborFace()
  */
  const ON_SubDVertex* OtherEndVertex(
    const ON_SubDVertex* vertex
    ) const;

  /*
  Parameters:
    vertex0 - [in]
    vertex1 - [in]
    bIgnoreOrientation - [in]
      If false, then the returned edge must have vertices (vertex0, vertex1).
      If true, then the returned edge may have vertices (vertex0, vertex1) or (vertex1, vertex0).
  Returns:
    If an edge connects the input vertices, it is returned. Otherwise nullptr is returned.
  */
  static const ON_SubDEdge* FromVertices(
    const ON_SubDVertex* vertex0,
    const ON_SubDVertex* vertex1,
    bool bIgnoreOrientation
  );

  static const ON_SubDEdgePtr FromVertices(
    const ON_SubDVertex* vertex0,
    const ON_SubDVertex* vertex1
  );

  /*
  Parameters:
    i - [in]
      0 or 1.
  Returns:
    If i is 0 or 1 and vertex[i] is set, then vertex[i]->ControlNetPoint() is returned. 
    Otherwise ON_3dPoint::NanPoint is returned.
  */
  const ON_3dPoint ControlNetPoint( unsigned int i ) const;

  const ON_Line ControlNetLine() const;

  /*
  Returns:
    If vertices are set, then the vector from m_vertex[0]->ControlNetPoint() 
    to m_vertex[1]->ControlNetPoint() is returned.
    Otherwise ON_3dVector::NanVector is returned.
  */
  const ON_3dVector ControlNetDirection() const;

  /*
  Returns:
    If vertices are set and v is an end of the edge,
    then the vector from v to OtherEndVertex(v) is returned.
    Otherwise ON_3dVector::NanVector is returned.
  */
  const ON_3dVector ControlNetDirectionFrom(
    const ON_SubDVertex* v
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
  Parameters:
    edge_vertex_index - [in]
      0 or 1 identifying which end of this edge to check.
    i - [in]
      Index of the face in this edge's face array.
  Returns:
    The edge adjacent to this edge in this->Face(i).
    The orientation is with respect to this->Face(i).
  */
  const ON_SubDEdgePtr AdjacentEdgePtr(
    unsigned int edge_vertex_index,
    unsigned int i
    ) const;

  const ON_SubDEdge* AdjacentEdge(
    unsigned int edge_vertex_index,
    unsigned int i
    ) const;
  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::Smooth or ON_SubD::EdgeTag::SmoothX.
    False in all other cases.
  */
  bool IsSmooth() const;
  
  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::Smooth.
  Remarks:
    Expert user function.
    This is used in rare cases when level 0 edges tagged as ON_SubD::EdgeTag::SmoothX
    need special handling in low level evaluation code. Typical SDK level functions
    and anything related to runtime user interface should call IsSmooth().
  */
  bool IsSmoothNotX() const;
  
  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::SmoothX.
  Remarks:
    Expert user function.
    This is used in rare cases when level 0 edges tagged as ON_SubD::EdgeTag::SmoothX
    need special handling in low level evaluation code. Typical SDK level functions
    and anything related to runtime user interface should call IsSmooth().
    An edge tagged as "X" can occur at level 0. It is subdivided as a smooth
    vertex and both of its end vertices are tagged as ON_SubD::VertexTag::Crease, 
    ON_SubD::VertexTag::Corner, or ON_SubD::VertexTag::Dart. 
    This tag cannot appear at level N with N >= 1.
  */
  bool IsSmoothX() const;

  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::Crease.
  */
  bool IsCrease() const;

  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::Crease and both of its end vertices 
    are tagged as ON_SubD::VertexTag::Crease, or ON_SubD::VertexTag::Corner. 
  */
  bool IsHardCrease() const;

  /*
  Returns:
    True if m_edge_tag is ON_SubD::EdgeTag::Crease and at least one of its end vertices 
    are tagged as ON_SubD::VertexTag::Dart.
  */
  bool IsDartCrease() const;

  /*
  Returns:
    0: end vertices are not tagged as darts
    1: one end vertex is tagged as a dart.
    2: both end vertices are tagged as a darts.
  */
  unsigned int DartCount() const;

  /*
  Returns:
    bitwise or of applicable ON_ComponentAttributes::EdgeAttributes values.
  Remarks:
    ON_ComponentAttributes::EdgeAttributes has subsets of mutually exclusive
    edge attributes.  If the edge is valid, then exactly one bit from each 
    mutually exclusive set of properties will be set.
    If an edge is not valie, then all bits for a set may be clear. 
    For example, if the edge has nullptr values in m_vertex[] 
    or the vertex control point locations are unset or nan, 
    then neither the ON_ComponentAttributes::EdgeAttributes::Open bit
    nor ON_ComponentAttributes::EdgeAttributes::Closed bit will be set.
  */
  unsigned int EdgeAttributes() const;

  /*
  Parameters:
    bUseSavedSubdivisionPoint - [in]
      If there is a saved subdivision point and bUseSavedSubdivisionPoint
      is true, then the saved value is returned.
    subdivision_point - [out]
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  ///<summary>
  /// The SubD edge Catmull-Clark subdivision point.
  ///</summary>
  const ON_3dPoint SubdivisionPoint() const;

  /*
  Description:
    Evaluates the Catmull-Clark subdivision point ignoring all cached information.
    This function is typically used in testing and debugging code and
    in ordinary cases, it is faster and better to call SubdivisionPoint()
    or GetSubdivisionPoint().
  Parameters:
    subdivision_point - [out]
      The edge Catmull-Clark subdivision point is returned here.
  */
  bool EvaluateCatmullClarkSubdivisionPoint(
    double subdivision_point[3]
  ) const;

  const ON_3dPoint ControlNetCenterPoint() const;
  const ON_3dVector ControlNetCenterNormal(
    unsigned int edge_face_index
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
    Set bTagged[i] = m_vertex[0]->IsDartOrCreaseOrCorner().
    0: bTagged[0] is true and bTagged[1] is false.
    1: bTagged[0] is false and bTagged[1] is true.
    2: bTagged[0] and Tagged[1] are both true.
    3: bTagged[0] and Tagged[1] are both false.
  */
  unsigned int TaggedEndIndex() const;

  /*
  Returns:
    Number of end vertices with Vertex().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedVertexCount() const;

  /*
  Returns:
    Number of faces attached to this edge with Face().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedFaceCount() const;

private:
  static unsigned int GetFacePointSum(
    const ON_SubDFace* face,
    const ON_SubDEdge* edge,
    double* facePsum // sum of face vertex points not on the edge
    );

private:
  friend class ON_SubDArchiveIdMap;
  friend class ON_SubDVertex;
  friend class ON_SubDFace;
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
class ON_CLASS ON_SubDFace : public ON_SubDComponentBase
{
public:
  ON_SubDFace() = default;
  ~ON_SubDFace() = default;
  ON_SubDFace(const ON_SubDFace&) = default;
  ON_SubDFace& operator=(const ON_SubDFace&) = default;

public:
  unsigned int FaceId() const;

public:
  /*
  Description:
    Clears saved subdivision and limit surface information
    for this component.
  */
  void ClearSavedSubdivisionPoints() const;


public:
  static const ON_SubDFace Empty;

  bool Write(
    class ON_BinaryArchive& archive
  ) const;

  static bool Read(
    class ON_BinaryArchive& archive,
    class ON_SubD& subd,
    class ON_SubDFace*& face
  );


  /*
  Parameters:
    bIncludeVertices - [in]
      If true, then attached vertices are included.
    bIncludeEdges - [in]
      If true, then attached edges are included.
  Returns:
    A ON_ComponentStatusLogicalOr() of this vertex's status and the
    specified attached components.
  See Also:
    ON_SubDComponentBase::Status()
  */
  const ON_ComponentStatus NeighborhoodStatusLogicalOr(
    bool bIncludeVertices,
    bool bIncludeEdges
  ) const;

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

  const ON_BoundingBox ControlNetBoundingBox() const;


  const ON_COMPONENT_INDEX ComponentIndex() const;
  const ON_SubDComponentPtr ComponentPtr() const;

  /*
  Description:
    Call this function if the face is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void FaceModifiedNofification() const;

public:
  // m_prev_face, m_next_face must be the first data members of ON_SubDFace
  const class ON_SubDFace* m_prev_face = nullptr;  // linked list of faces on this level
  const class ON_SubDFace* m_next_face = nullptr;  // linked list of faces on this level

private:
  // Location of this face's default 2d texture coordinates ("surface parameter") in normaized 2d texture coordinates
  mutable double m_texture_coordinate_origin[2] = {};
  mutable double m_texture_coordinate_delta[2] = {};

  enum TextureCoordinateBits : unsigned char
  {
    None = 0,

    // 4 ways the texture domain can be rotated for a face when packing is applied.
    // These enum values must be 0,1,2,3
    PackingRotate0 = 0,
    PackingRotate90 = 1,
    PackingRotate180 = 2,
    PackingRotate270 = 3,

    // bits mask for packing rotation setting
    PackingRotateMask = 3,

    UnusedBit1 = 0x04U,
    UnusedBit2 = 0x08U,
    UnusedBitsMask = 0x0CU,

    // bits for TextureCoordinateDomainType()
    DomainTypeMask = 0xF0U,
  };
  mutable unsigned char m_texture_coordinate_bits = 0;

private:
  unsigned char m_reserved1 = 0;

private:
  // The application specifies a base ON_Material used to render the subd this face belongs to.
  // If m_material_channel_index > 0 AND face_material_id = base.MaterialChannelIdFromIndex(m_material_channel_index)
  // is not nil, then face_material_id identifies an override rendering material for this face.
  // Othewise base will be used to render this face.
  mutable unsigned short m_material_channel_index = 0;

public:
  /*
  Description:
    Set the per face rendering material channel index.

  Parameters:
    material_channel_index - [in]
      A value between 0 and ON_Material::MaximumMaterialChannelIndex, inclusive.
      This value is typically 0 or the value returned from ON_Material::MaterialChannelIndexFromId().

  Remarks:
    If base_material is the ON_Material assigned to render this subd and
    ON_UUID face_material_id = base_material.MaterialChannelIdFromIndex( material_channel_index )
    is not nil, then face_material_id identifies an override rendering material for this face.
    Otherwise base_material is used to reneder this face.
  */
  void SetMaterialChannelIndex(int material_channel_index) const;

  /*
  Description:
    Remove the per face rendering material channel index.
    The face will use the material assigned to the subd object.
  */
  void ClearMaterialChannelIndex() const;

  /*
  Returns:
    This face's rendering material channel index.

    Remarks:
    If base_material is the ON_Material assigned to render this subd, MaterialChannelIndex() > 0,
    and ON_UUID face_material_id = base_material.MaterialChannelIdFromIndex( face.MaterialChannelIndex() )
    is not nil, then face_material_id identifies an override rendering material for this face.
    Otherwise base_material is used to reneder this face.
  */
  int MaterialChannelIndex() const;

private:
  // The application specifies a base ON_Material used to render the subd this face belongs to.
  // If m_material_channel_index > 0 AND face_material_id = base.MaterialChannelIdFromIndex(m_material_channel_index)
  // is not nil, then face_material_id identifies an override rendering material for this face.
  // Othewise base will be used to render this face.
  mutable ON_Color m_per_face_color = ON_Color::UnsetColor;

public:

  /*
  Description:
    Set per face color.

  Parameters:
    color - [in]
  */
  void SetPerFaceColor(
    ON_Color color
    ) const;

  /*
  Description:
    Remove per face color setting. The face will use the color assigned to the subd object.
  */
  void ClearPerFaceColor() const;

  /*
  Returns:
    Per face color. A value of ON_Color::UnsetColor indicates the face uses the color assigned to the subd object.
  */
  const ON_Color PerFaceColor() const;




public:
  const bool TextureDomainIsSet() const;

  /*
  Description:
    Set the rectangle in 2d texture space that will be used by this face
    for generating default "surface mapping" texture coordinates.
    The lower left corner will be origin.
    The upper right corner will be delta.
  Parameters:
    origin - [in]
    delta - [in]
      >= 0.0;
    packing_rotation_degrees - [in]
      must be a mulitple of 90.
  */
  void SetTextureDomain(
    ON_SubDTextureDomainType texture_domain_type,
    ON_2dPoint origin,
    ON_2dVector delta,
    int packing_rotation_degrees
  ) const;

  const ON_2dPoint TextureDomainOrigin() const;

  const ON_2dVector TextureDomainDelta() const;

  ON_SubDTextureDomainType TextureDomainType() const;

  /*
  Returns:
    0, 90, 180, or 270
  */
  unsigned TextureDomainRotationDegrees() const;

  double TextureDomainRotationRadians() const;
  /*
  Parameters:
    bGridOrder - [in]
      false: counter clockwise quad order.
      true: fragment grid order
    bNormalized - [in]
      false: corners of the (x0,x0+dx) x (y0,y0+dy) square are returned where
      (x0,y0) = TextureCoordinateDomainOrigin() and
      (dx,dy) = TextureCoordinateDomainDelta().
      true: corners of the (0,1) x (0,1) square are returned.
    corner_index - [in]
  Returns:
    Specified texture coordinate corner value.
  */
  const ON_2dPoint TextureDomainCorner(
    bool bGridOrder,
    bool bNormalized,
    int corner_index
  ) const;

public:
  unsigned int m_level_zero_face_id = 0;   // id of level zero face

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
  enum : unsigned int
  {
    MaximumEdgeCount = 0xFFF0U
  };
  unsigned short m_edge_count = 0;
  unsigned short m_edgex_capacity = 0;

  ON_SubDEdgePtr m_edge4[4];
  ON_SubDEdgePtr* m_edgex = nullptr;

  /*
  Returns:
    A linked list of limit mesh fragments that cover this face.
    Nullptr if none are currently available.

    If the face is a quad, there is a single fragment. If the
    face is an n-gon with 4 != n, then the list has n fragments.
    The i-th fragment covers the subdivision quad with its 
    corner at Vertex(i).

  Remarks:
    Use the returned fragments immediately or make a 
    copies you manage for later use.

    Use ON_SubD.UpdateSurfaceMeshCache(false) or ON_SubD.UpdateSurfaceMeshCache(true)
    to create the face fragments.

    ON_SubDFace.ClearSavedSubdivisionPoints() removes any saved
    fragments and is called when the face or nearby components
    are modified.
  */
  const class ON_SubDMeshFragment* MeshFragments() const;


  const ON_3dPoint ControlNetCenterPoint() const;

  const ON_3dVector ControlNetCenterNormal() const;

  const ON_Plane ControlNetCenterFrame() const;

  bool IsConvex() const;

  bool IsNotConvex() const;

  bool IsPlanar(double planar_tolerance = ON_ZERO_TOLERANCE) const;

  bool IsNotPlanar(double planar_tolerance = ON_ZERO_TOLERANCE) const;

private:
  // If 0 != ON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags), then
  // m_limit_mesh_fragments is a linked list of m_edge_count
  // fragments available from MeshFragments() and managed 
  // by the parent ON_SubD. 
  // If 0 == ON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags),
  // then any information in m_limit_mesh_fragments is dirty
  // and should not be used.
  // ClearSavedSubdivisionPoints() zeros
  // the appropriate bit of m_saved_points_flags.
  friend class ON_Internal_SubDFaceMeshFragmentAccumulator;
  friend class ON_SubDHeap;

  // Mesh fragment(s) for this face
  mutable class ON_SubDMeshFragment* m_mesh_fragments = nullptr;

private:

public:
  unsigned int EdgeCount() const;

  const ON_SubDEdgePtr EdgePtr(
    unsigned int i
    ) const;

  const ON_SubDEdgePtr EdgePtrFromEdge(
    const class ON_SubDEdge* e
    ) const;

  const class ON_SubDVertex* Vertex(
    unsigned int i
    ) const;

  const ON_3dPoint ControlNetPoint(
    unsigned int i
  ) const;

  unsigned int VertexIndex(
    const ON_SubDVertex* vertex
    ) const;

  /*
  Returns;
    If the vertex is in this face's boundary, pair of face boundary edges at the vertex is returned 
    with face boundary orientations, that is vertex = pair.First().EdgePtr().RelativeVertex(1)
    and vertex = pair.Second().EdgePtr().RelativeVertex(0). Otherwise, ON_SubDComponentPtrPair::Null
    is returned.
  */
  const ON_SubDComponentPtrPair VertexEdgePair(
    const ON_SubDVertex* vertex
  ) const;

  /*
  Returns;
    If the vertex is in this face's boundary, pair of face boundary edges at the vertex is returned
    with face boundary orientations, that is vertex = pair.First().EdgePtr().RelativeVertex(1)
    and vertex = pair.Second().EdgePtr().RelativeVertex(0). Otherwise, ON_SubDComponentPtrPair::Null
    is returned.
  */
  const ON_SubDComponentPtrPair VertexEdgePair(
    unsigned vertex_index
  ) const;

  const class ON_SubDEdge* Edge(
    unsigned int i
    ) const;

  ON__UINT_PTR EdgeDirection(
    unsigned int i
    ) const;

  /*
  Returns:
    If e is part of the face's boundary, then the index of the edge is returned.
    Otherwise, ON_UNSET_UINT_INDEX is returned.
  */
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

  /////*
  ////Description:
  ////  Expert user tool to replace one edge with another in the face's edge array.
  ////Parameters:
  ////  edge_to_remove - [in]
  ////  edge_to_insert - [in]
  ////   The inserted edge is assigned the same boundary orientation specified
  ////   in edgeptr_to_insert.
  ////Remarks:
  ////  Does not modify the edge. The corresponding reference to this face must
  ////  be removed from the first edge and added to the second edge.
  ////*/
  ////bool ReplaceEdgeInArray(
  ////  unsigned int fei0,
  ////  ON_SubDEdge* edge_to_remove,
  ////  ON_SubDEdgePtr edgeptr_to_insert
  ////);

  /*
  Description:
    Rotates the edge array so that Edge(fei0) becomes the first edge in the edge array.
  */
  bool RotateEdgeArray(
    unsigned int fei0
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

  /*
  Parameters:
    subdivision_point - [out]
      The average of the face vertex locations.
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  ///<summary>
  /// The SubD face Catmull-Clark subdivision point.
  ///</summary>
  const ON_3dPoint SubdivisionPoint() const;
  
  /*
  Description:
    Evaluates the Catmull-Clark subdivision point ignoring all cached information.
    This function is typically used in testing and debugging code and
    in ordinary cases, it is faster and better to call SubdivisionPoint()
    or GetSubdivisionPoint().
  Parameters:
    subdivision_point - [out]
      The Catmull-Clark face subdivision point is returned here.
  */
  bool EvaluateCatmullClarkSubdivisionPoint(
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
  bool GetQuadSurface(
    double* limit_surface_cv,
    size_t limit_surface_cv_stride0,
    size_t limit_surface_cv_stride1
    ) const;

  bool GetQuadSurface(
    class ON_NurbsSurface& limit_surface
    ) const;

  bool GetQuadSurface(
    class ON_BezierSurface& limit_surface
    ) const;

  /*
  Returns:
    Number of edges in the face's boundary with Edge().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedEdgeCount() const;

  /*
  Returns:
    Number of vertices in the face's boundary with Vertex().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedVertexCount() const;

  /*
  Description:
    Set the mark of every vertex attached to this face.
  Returns:
    Number of marks changed.
  */
  unsigned int SetVertexMarks(
    bool bMark
    ) const;

  /*
  Description:
    Set the mark of every boundary edge attached to this face.
  Returns:
    Number of marks changed.
  */
  unsigned int SetEdgeMarks(
    bool bMark
    ) const;

  unsigned int GetEdgeArray(
    ON_SimpleArray< ON_SubDEdgePtr >& face_edge_array
  ) const;

private:
  friend  class ON_SubDArchiveIdMap;
  friend class ON_SubDVertex;
  friend class ON_SubDEdge;

  void CopyFrom(
    const ON_SubDFace* src,
    bool bCopyEdgeArray
    );
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDVertexArray
//

/*
Description:
  Avoid using ON_SubDVertexArray. ON_SubDVertexIterator is more efficient and a better choice.
*/
class ON_CLASS ON_SubDVertexArray
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

/*
Description:
  Avoid using ON_SubDEdgeArray. ON_SubDEdgeIterator is more efficient and a better choice.
*/
class ON_CLASS ON_SubDEdgeArray
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

/*
Description:
  Avoid using ON_SubDFaceArray. ON_SubDFaceIterator is more efficient and a better choice.
*/
class ON_CLASS ON_SubDFaceArray
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
class ON_CLASS ON_SubDVertexIterator
{
public:
  // The best way to get an ON_SubDVertexIterator is so use the ON_SubD member function
  // ON_SubDVertexIterator vit = subd.VertexIterator();
  ON_SubDVertexIterator(
    const class ON_SubD& subd
    );

  // The best way to get an ON_SubDVertexIterator is so use the ON_SubD member function
  // ON_SubDVertexIterator vit = subd.VertexIterator();
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
// ON_SubDVertexIdIterator
//

class ON_SubDVertexIdIterator : private ON_FixedSizePoolIterator
{
public:
  ON_SubDVertexIdIterator() = default;
  ~ON_SubDVertexIdIterator() = default;
  ON_SubDVertexIdIterator(const ON_SubDVertexIdIterator&) = default;
  ON_SubDVertexIdIterator& operator=(const ON_SubDVertexIdIterator&) = default;

public:
  ON_SubDVertexIdIterator(const ON_SubDRef& subd_ref);
  ON_SubDVertexIdIterator(const ON_SubD& subd);

public:
  /*
  Description:
    In general, you want to use a ON_SubDVertexIterator to loop through SubD vertices.
    This is a special tool for unusual sitiations wheh it is necessary to
    iteratate through every vertex on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The vertex with the smallest id.
  */
  const ON_SubDVertex* FirstVertex();

  /*
  Description:
    In general, you want to use a ON_SubDVertexIterator to loop through SubD vertices.
    This is a special tool for unusual sitiations wheh it is necessary to
    iteratate through every vertex on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The vertex in order of increasing id.
  */
  const ON_SubDVertex* NextVertex();

  /*
  Returns:
    The most recently returned vertex from a call to FirstVertex() or NextVertex().
  */
  const ON_SubDVertex* CurrentVertex() const;

private:
  friend class ON_SubDHeap;
  ON_SubDRef m_subd_ref;
  void Internal_Init();
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDEdgeIterator
//
class ON_CLASS ON_SubDEdgeIterator
{
public:
  // The best way to get an ON_SubDEdgeIterator is so use the ON_SubD member function
  // ON_SubDEdgeIterator eit = subd.EdgeIterator();
  ON_SubDEdgeIterator(
    const class ON_SubD& subd
    );

  // The best way to get an ON_SubDEdgeIterator is so use the ON_SubD member function
  // ON_SubDEdgeIterator eit = subd.EdgeIterator();
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
// ON_SubDEdgeIdIterator
//

class ON_SubDEdgeIdIterator : private ON_FixedSizePoolIterator
{
public:
  ON_SubDEdgeIdIterator() = default;
  ~ON_SubDEdgeIdIterator() = default;
  ON_SubDEdgeIdIterator(const ON_SubDEdgeIdIterator&) = default;
  ON_SubDEdgeIdIterator& operator=(const ON_SubDEdgeIdIterator&) = default;

public:
  ON_SubDEdgeIdIterator(const ON_SubDRef& subd_ref);
  ON_SubDEdgeIdIterator(const ON_SubD& subd);

public:
  /*
  Description:
    In general, you want to use a ON_SubDEdgeIterator to loop through SubD edges.
    This is a special tool for unusual sitiations wheh it is necessary to
    iteratate through every edge on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The edge with the smallest id.
  */
  const ON_SubDEdge* FirstEdge();

  /*
  Description:
    In general, you want to use a ON_SubDEdgeIterator to loop through SubD edges.
    This is a special tool for unusual sitiations wheh it is necessary to
    iteratate through every edge on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The edge in order of increasing id.
  */
  const ON_SubDEdge* NextEdge();

  /*
  Returns:
    The most recently returned edge from a call to FirstEdge() or NextEdge().
  */
  const ON_SubDEdge* CurrentEdge() const;

private:
  friend class ON_SubDHeap;
  ON_SubDRef m_subd_ref;
  void Internal_Init();
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDFaceIterator
//
class ON_CLASS ON_SubDFaceIterator
{
public:
  // The best way to get an ON_SubDFaceIterator is so use the ON_SubD member function
  // ON_SubDFaceIterator fit = subd.FaceIterator();
  ON_SubDFaceIterator(
    const class ON_SubD& subd
    );

  // The best way to get an ON_SubDFaceIterator is so use the ON_SubD member function
  // ON_SubDFaceIterator fit = subd.FaceIterator();
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
// ON_SubDFaceIdIterator
//

class ON_SubDFaceIdIterator : private ON_FixedSizePoolIterator
{
public:
  ON_SubDFaceIdIterator() = default;
  ~ON_SubDFaceIdIterator() = default;
  ON_SubDFaceIdIterator(const ON_SubDFaceIdIterator&) = default;
  ON_SubDFaceIdIterator& operator=(const ON_SubDFaceIdIterator&) = default;

public:
  ON_SubDFaceIdIterator(const ON_SubDRef& subd_ref);
  ON_SubDFaceIdIterator(const ON_SubD& subd);

public:
  /*
  Description:
    In general, you want to use a ON_SubDFaceIterator to loop through SubD faces.
    This is a special tool for unusual sitiations wheh it is necessary to
    iteratate through every face on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The face with the smallest id.
  */
  const ON_SubDFace* FirstFace();

  /*
  Description:
    In general, you want to use a ON_SubDFaceIterator to loop through SubD faces.
    This is a special tool for unusual sitiations wheh it is necessary to
    iteratate through every face on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The face in order of increasing id.
  */
  const ON_SubDFace* NextFace();

  /*
  Returns:
    The most recently returned face from a call to FirstFace() or NextFace().
  */
  const ON_SubDFace* CurrentFace() const;

private:
  friend class ON_SubDHeap;
  ON_SubDRef m_subd_ref;
  void Internal_Init();
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentIterator
//
class ON_CLASS ON_SubDComponentIterator
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
// ON_SubDMeshFragmentIterator
//

class ON_CLASS ON_SubDMeshFragmentIterator
{
public:
  ON_SubDMeshFragmentIterator() = default;
  ~ON_SubDMeshFragmentIterator() = default;
  ON_SubDMeshFragmentIterator(const ON_SubDMeshFragmentIterator&);
  ON_SubDMeshFragmentIterator& operator=(const ON_SubDMeshFragmentIterator&);

public:
  ON_SubDMeshFragmentIterator(const class ON_SubDMesh limit_mesh);
  ON_SubDMeshFragmentIterator(ON_SubDRef& subd_ref);
  ON_SubDMeshFragmentIterator(const ON_SubD& subd);
  ON_SubDMeshFragmentIterator(const ON_SubDFaceIterator& fit);

public:
  const ON_SubDMeshFragment* FirstFragment();
  const ON_SubDMeshFragment* NextFragment();
  const ON_SubDMeshFragment* CurrentFragment();

  bool IsEmpty() const;

  const ON_SubD& SubD() const;

  /*
  Returns
    ON_SubDComponentLocation::Surface or ON_SubDComponentLocation::ControlNet
  */
  ON_SubDComponentLocation SubDAppearance() const;
  
  /*
  Parameters:
    subd_appearance_override - [in]
      ON_SubDComponentLocation::Unset - appearance controlled by SubD().SubDAppearance().
      ON_SubDComponentLocation::Surface
      ON_SubDComponentLocation::ControlNet
  Returns
    ON_SubDComponentLocation::Surface or ON_SubDComponentLocation::ControlNet
  */
  void SetSubDAppearanceOverride(ON_SubDComponentLocation subd_appearance_override);


  /*
  Returns:
    Density setting used to create the fragments and the maximum mesh density
    these fragments can deliver.
  Remarks:
    0: a single mesh quad per SubD quad 
    1: 4 mesh quads per SubD quad (n mesh quads per SubD n-gon when n != 4)
    d: 4^d mesh quads per SubD quad (n*(4^(d-1)) mesh quads per SubD n-gon when n != 4)
  */
  unsigned int MaximumMeshDensity() const;


  /*
  Returns:
    Minimum mesh density that can be extracted from these fragments.
  Remarks:
    0 = all fragments are full sized.
    1 = there are half sixed fragments because the SubD has n-gons with n != 4.
  */
  unsigned int MinimumMeshDensity() const;

  /*
  Parameters:
    candidate_mesh_density - [in]
  Returns:
    If candidate_mesh_density > MeshDensity(), then MeshDensity() is returned.
    If candidate_mesh_density < MinimumMeshDensity(), then MinimumMeshDensity() is returned.
    Otherwise, candidate_mesh_density is returned.
  */
  unsigned int ClampMeshDensity(
    unsigned int candidate_mesh_density
  ) const;

  /*
  Returns:
    Total number of fragments.
  */
  unsigned int FragmentCount() const;

  /*
  Returns:
    Total number of full sized fragments. 
  Remarks:
    A full sized fragment covers an entire quad face.
  */
  unsigned int FullSizeFragmentCount() const;

  /*
  Returns:
    Total number of half sized fragments. 
  Remarks:
    A half sized fragment covers the corner of an N-gon and are used when N != 4.
  */
  unsigned int HalfSizeFragmentCount() const;


  /*
  Parameters:
    full_size_fragment_count - [out]
      Number of full sized fragments. These cover an entire quad face.
    half_size_fragment_count - [out]
      Number of half sized fragments. These cover a corner of an N-gon
      and are used when N != 4.
  Returns:
    Total number of fragments.
  */
  unsigned int GetFragmentCounts(
    unsigned int& full_size_fragment_count,
    unsigned int& half_size_fragment_count
  ) const;

  /*
  Parameters:
    mesh_density - [in]
      MinimumMeshDensity() <= mesh_density <= MeshDensity()
  Returns:
    Total number of mesh quads in at the specified mesh density.
  */
  unsigned int TotalQuadCount(unsigned int mesh_density) const;

  /*
  Parameters:
    mesh_density - [in]
      MinimumMeshDensity() <= mesh_density <= MeshDensity()
  Returns:
    Total number of mesh points delivered at the specified mesh density.
  */
  unsigned int TotalPointCount(unsigned int mesh_density) const;

  /*
  Returns:
    Total number of mesh quads delivered at MaximumMeshDensity().
  */
  unsigned int MaximumDensityQuadCount() const;

  /*
  Parameters:
    mesh_density - [in]
      MinimumMeshDensity() <= mesh_density <= MeshDensity()
  Returns:
    Total number of mesh points delivered at MaximumMeshDensity().
  */
  unsigned int MaximumDensityPointCount() const;

  const ON_BoundingBox SurfaceBoundingBox() const;
  const ON_BoundingBox ControlNetQuadBoundingBox() const;
  /*
  Returns:
    If this->SubDAppearance() is ON_SubDComponentLocation::ControlNet, then this->ControlNetQuadBoundingBox() is returned.
    Otherwise this->SurfaceBoundingBox() returned.
  */
  const ON_BoundingBox BoundingBox() const;

private:
  void Internal_CopyFrom(const ON_SubDMeshFragmentIterator& src);

  ON_SubDMesh m_limit_mesh;
  ON_SubD m_subd;
  ON_SubDFaceIterator m_fit;
  const ON_SubDMeshFragment* m_current_fragment = nullptr;
  bool m_bFromFaceFragments = false;
  mutable bool m_bHaveCounts = false;

  // used to override the appearance of SubD().SubDApperance().
  ON_SubDComponentLocation m_subd_appearance_override = ON_SubDComponentLocation::Unset;

  mutable unsigned int m_maximum_mesh_density = 0; // See MaximumMeshDensity() comment

  // full sized fragment count (for quads)
  mutable unsigned int m_full_size_fragment_count = 0;

  // half sized fragment count (for n-gons with n != 4)
  mutable unsigned int m_half_size_fragment_count = 0;
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDSectorIterator
//

class ON_CLASS ON_SubDSectorIterator
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
    When the sector iterator is initialized and valid, sit.CenterVertex() = CurrentEdge(*).RelativeVertex(0).
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

  enum class StopAt : unsigned char
  {
    ///<summary>
    /// Sector iteration will terminate when the edge being crossed does not have two faces.
    ///</summary>
    Boundary = 0,

    ///<summary>
    /// Sector iteration will terminate when the edge being crossed does not have two faces
    /// or that edge is tagged as ON_SubD::EdgeTag::Crease.
    ///</summary>
    AnyCrease = 1,

    ///<summary>
    /// Sector iteration will terminate when the edge being crossed does not have two faces
    /// or that edge is tagged as ON_SubD::EdgeTag::Crease and has no dart vertices.
    ///</summary>
    HardCrease = 2,
  };

  /*
  Description:
    Advance the "current" face to the "next" face in the ring
    by crossing CurrentEdge(1).  
    
    If the current face is not reversed (1 == CurrentFaceDirection),
    then this rotation is counter-clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then this rotation is clockwise with respect to 
    the current face's orientation.
  Parameters:
    stop_at - [in]
      Determines that type of CurrentEdge(1) will terminate iteration.
  Returns:
    When the input CurrentEdge(1) has exactly two faces and
    is not tagged as a crease or bStopAtCrease is false, the 
    face on the other side of CurrentEdge(1) becomes the new
    current face and a pointer to this face is returned.
  Remarks:
    Identical to calling IncrementFace(+1,bStopAtCrease);
  */
  const ON_SubDFace* NextFace(
    ON_SubDSectorIterator::StopAt stop_at
    );

  /*
  Description:
    Advance the "current" face to the "previous" face in the ring
    by crossing CurrentEdge(0).  
    
    If the current face is not reversed (0 == CurrentFaceDirection),
    then this rotation is clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then this rotation is counter-clockwise with respect to 
    the current face's orientation.
  Parameters:
    stop_at - [in]
      Determines that type of CurrentEdge(0) will terminate iteration.
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
    ON_SubDSectorIterator::StopAt stop_at
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
    stop_at - [in]
      Determines that type of CurrentEdge((increment_direction>0) ? 1 : 0) will terminate iteration.
      If true and the input value of CurrentEdge((increment_direction>0) ? 1 : 0) 
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
    ON_SubDSectorIterator::StopAt stop_at
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
  // center vertex = m_current_eptr[i].RelativeVertex(0)
  ON_SubDEdgePtr m_current_eptr[2] = {}; // default = { ON_SubDEdgePtr::Null, ON_SubDEdgePtr::Null };

  unsigned int m_initial_fvi = 0;
  unsigned int m_current_fvi = 0;
  unsigned int m_current_fei[2] = {};  // default = { 0, 0 }; // "prev" and "next"

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
class ON_CLASS ON_SubDFaceEdgeIterator
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
  Parameters:
    bReturnNullAtFirstEdge = [in]
      If true and the next edge would be FirstEdge(), the nullptr is returned
      and CurrentEdge() is not changed.
  Description:
    Increments the iterator and returns the edge.
  */
  const ON_SubDEdge* NextEdge(
    bool bReturnNullAtFirstEdge
  );

  /*
  Parameters:
    bReturnNullAtLastEdge = [in]
      If true and the previous edge would be FirstEdge(), the nullptr is returned
      and CurrentEdge() is not changed.
  Description:
    Decrements the iterator and returns the edge.
  */
  const ON_SubDEdge* PrevEdge(
    bool bReturnNullAtFirstEdge
  );

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
// ON_SubDFromMeshParameters
//
class ON_CLASS ON_SubDFromMeshParameters
{
public:

  // Default construction is identical to ON_SubDFromMeshParameters::Smooth.
  ON_SubDFromMeshParameters() = default;
  ~ON_SubDFromMeshParameters() = default;
  ON_SubDFromMeshParameters(const ON_SubDFromMeshParameters&) = default;
  ON_SubDFromMeshParameters& operator=(const ON_SubDFromMeshParameters&) = default;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Crease options
  //

  // No interior creases and no corners.
  static const ON_SubDFromMeshParameters Smooth;
  
  // Create an interior sub-D crease along coincident input mesh edges
  // where the vertex normal directions at one end differ by at 
  // least 30 degrees.
  static const ON_SubDFromMeshParameters InteriorCreaseAtMeshCrease;

  // Create an interior sub-D crease along all coincident input mesh edges.
  static const ON_SubDFromMeshParameters InteriorCreaseAtMeshEdge;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Custom interior crease options
  //
#pragma region RH_C_SHARED_ENUM [SubD::InteriorCreaseOption] [Rhino.Geometry.SubDCreationOptions.InteriorCreaseOption] [nested:byte]
  /// <summary>
  /// Defines how interior creases are treated.
  /// </summary>
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

  static ON_SubDFromMeshParameters::InteriorCreaseOption InteriorCreaseOptionFromUnsigned(
    unsigned int interior_crease_option_as_unsigned
    );

  /*
  Parameters:
    interior_crease_option - [in]
  */
  void SetInteriorCreaseOption(
    ON_SubDFromMeshParameters::InteriorCreaseOption interior_crease_option
    );

  /*
  Returns:
    The interior crease option.
  */
  ON_SubDFromMeshParameters::InteriorCreaseOption InteriorCreaseTest() const;


  /*
  Description:
    When the interior crease option is 
    ON_SubDFromMeshParameters::InteriorCreaseOption::AtMeshCreases,
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
    ON_SubDFromMeshParameters::InteriorCreaseOption::AtMeshCreases,
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
  ON_SubDFromMeshParameters::InteriorCreaseOption CopyInteriorCreaseTest(
    ON_SubDFromMeshParameters source_options
    );


  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Convex corner options
  //

  // Look for convex corners at sub-D vertices with 2 edges
  // that have an included angle <= 90 degrees.
  static const ON_SubDFromMeshParameters ConvexCornerAtMeshCorner;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Custom convex corner options
  //
#pragma region RH_C_SHARED_ENUM [SubD::ConvexCornerOption] [Rhino.Geometry.SubDCreationOptions.ConvexCornerOption] [nested:byte]
  /// <summary>
  /// Defines how convex corners are treated.
  /// </summary>
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

  static ON_SubDFromMeshParameters::ConvexCornerOption ConvexCornerOptionFromUnsigned(
    unsigned int convex_corner_option_as_unsigned
    );

  /*
  Parameters:
    convex_corner_option - [in]
  */
  void SetConvexCornerOption(
    ON_SubDFromMeshParameters::ConvexCornerOption convex_corner_option
    );

  /*
  Returns:
    The currently selected convex corner option.
  */
  ON_SubDFromMeshParameters::ConvexCornerOption ConvexCornerTest() const;

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
  ON_SubDFromMeshParameters::ConvexCornerOption CopyConvexCornerTest(
    ON_SubDFromMeshParameters source_parameters
    );

  /*
  Returns:
    false - In ON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex control net locations.
    true - In ON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex limit surface locations.
  Remark:
    Interpolation computation is available in Rhino, Rhino compute, Rhino Inside,
    Grasshopper. Interpolation computation is not available in the opennurbs IO toolkit.
  */
  bool InterpolateMeshVertices() const;

  /*
  Parameters:
    false
      In ON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex control net locations.
      This is the fastest and most robust way to create a subd from a mesh.
    true
      In ON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex limit surface locations.
      If there are a large number of vertices, this option can require lots of computation.
  Remark:
    Interpolation computation is available in Rhino, Rhino compute, Rhino Inside,
    Grasshopper. Interpolation computation is not available in the opennurbs IO toolkit.
  */
  void SetInterpolateMeshVertices(
    bool bInterpolateMeshVertices
  );

  /*
  Returns:
    true - In ON_SubD::CreateFromMesh(), colinear boundary edges belonging to the same face are merged into a single edge.
    false - In ON_SubD::CreateFromMesh(), each mesh boundary edge becomes a SubD boundary edge.
  */
  bool MergeColinearBoundaryEdges() const;

  /*
  Parameters:
    bMergeColinearBoundaryEdges - [in]
      true 
        In ON_SubD::CreateFromMesh(), colinear boundary edges belonging to the same face are merged into a single edge.
        In general, this is the best choice and leads to the highest quality of SubD object.
      false
        In ON_SubD::CreateFromMesh(), each mesh boundary edge becomes a SubD boundary edge.
        Use this when the boundary topology of the mesh and SubD should be identical.
  */
  void SetMergeColinearBoundaryEdges(
    bool bMergeColinearBoundaryEdges
  );

  /*
  Returns:
    true - In ON_SubD::CreateFromMesh(), colinear interior edges between two faces are merged into a single edge.
    false - In ON_SubD::CreateFromMesh(), each mesh interior edge becomes a SubD interior edge.
  */
  bool MergeColinearInteriorEdges() const;

  /*
  Parameters:
  bMergeColinearInteriorEdges - [in]
    true
      In ON_SubD::CreateFromMesh(), colinear interior edges between two faces are merged into a single edge.
      In general, this is the best choice and leads to the highest quality of SubD object.
    false
      In ON_SubD::CreateFromMesh(), each mesh interior edge becomes a SubD interior edge.
      Use this when the interior topology of the mesh and SubD should be identical.
  */
  void SetMergeColinearInteriorEdges(
    bool bMergeColinearInteriorEdges
  );

  
private:
  enum : unsigned char
  {
    MergeColinearBoundaryEdgesMask = 1,
    MergeColinearInteriorEdgesMask = 2
  };
  unsigned char m_merge_edges_bits = 0; // clear bit (0) = "true, set bit (1) = "false"

  bool m_bInterpolateMeshVertices = false;

  ON_SubDFromMeshParameters::InteriorCreaseOption m_interior_crease_option = ON_SubDFromMeshParameters::InteriorCreaseOption::None;
  ON_SubDFromMeshParameters::ConvexCornerOption m_convex_corner_option = ON_SubDFromMeshParameters::ConvexCornerOption::None;

  unsigned short m_maximum_convex_corner_edge_count = 2U;

  unsigned short m_reserved3 = 0;

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
class ON_CLASS ON_SubDComponentRef : public ON_Geometry
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
    component_location - [in]
  */
  static const ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    ON_COMPONENT_INDEX component_index,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
    );

  static const ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    ON_SubDComponentPtr component_ptr,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
    );

  static const ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    const class ON_SubDVertex* vertex,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
    );

  static const ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    const class ON_SubDEdge* edge,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
    );

  static const ON_SubDComponentRef Create(
    const ON_SubDRef& subd_ref,
    const class ON_SubDFace* face,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
    );

  void Clear();

  ON_SubDRef SubDRef() const;

  const class ON_SubD& SubD() const;

  ON_COMPONENT_INDEX ComponentIndex() const override;

  ON_SubDComponentPtr ComponentPtr() const;

  ON_SubDComponentLocation ComponentLocation() const;

  /*
  The interpretation of this value depends on the context.
  */
  ON__UINT_PTR ReferenceId() const;

  void SetReferenceId(
    ON__UINT_PTR reference_id
  );

  const class ON_SubDVertex* Vertex() const;

  const class ON_SubDEdge* Edge() const;

  const class ON_SubDFace* Face() const;

  static int Compare(const ON_SubDComponentRef* lhs, const ON_SubDComponentRef* rhs);
  static int Compare2(const ON_SubDComponentRef* const* lhs, const ON_SubDComponentRef* const* rhs);
  
private:
  ON_SubDRef m_subd_ref;
  ON_SubDComponentPtr m_component_ptr = ON_SubDComponentPtr::Null;
  ON_COMPONENT_INDEX m_component_index = ON_COMPONENT_INDEX::UnsetComponentIndex;
  ON_SubDComponentLocation m_component_location =  ON_SubDComponentLocation::Unset;
  ON__UINT_PTR m_reference_id = 0;

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

class ON_CLASS ON_SubDComponentRefList
{
public:
  ON_SubDComponentRefList() = default;
  ~ON_SubDComponentRefList();
  ON_SubDComponentRefList(const ON_SubDComponentRefList& src);
  ON_SubDComponentRefList& operator=(const ON_SubDComponentRefList& src);

  /*
  Description:
    Append a ON_SubDComponentRef to the identified component.
  Returns;
    A pointer to the ON_SubDComponentRef managed by this class or
    nullptr if the input is not valid.
  */
  const ON_SubDComponentRef& Append(
    const ON_SubDRef& subd_ref,
    ON_COMPONENT_INDEX ci,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
  );

  const ON_SubDComponentRef& Append(
    const ON_SubDRef& subd_ref,
    ON_SubDComponentPtr component_ptr,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
  );

  /*
  Description:
    Appends a copy of src_ref and returns a pointer to the ON_SubDComponentRef
    managed by this class.
  */
  const ON_SubDComponentRef& Append(const ON_SubDComponentRef& src_ref);
  const ON_SubDComponentRef& Append(const ON_SubDComponentRef* src_ref);

  /*
  Description:
    Expert user function to transfer managment of an ON_SubDComponentRef on the heap
    to this class.
  */
  const ON_SubDComponentRef& AppendForExperts(ON_SubDComponentRef*& ref);

  /*
  Description:
    Expert user function to append a ON_SubDComponentRef to the identified component.
    The expert user is responsible for insuring subd exists fot the lifetime of this
    class.
  */
  const ON_SubDComponentRef& AppendForExperts(
    const ON_SubD& subd,
    ON_COMPONENT_INDEX ci,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
  );

  const ON_SubDComponentRef& AppendForExperts(
    const ON_SubD& subd,
    ON_SubDComponentPtr component_ptr,
    ON_SubDComponentLocation component_location,
    ON__UINT_PTR reference_id
  );

  /*
  Description:
    Sort by ON_SubDComponentRef::Compare2() and remove duplicates and empty elements.
  Returns:
    Length of clean list.
  */
  int Clean();


  /*
  Returns:
    Number of refs in the list.
  */
  int Count() const;
  
  void Remove(int i);

  /*
  Description:
    Transfers the ref to an expert user who is responsible for properly managing it.
  */
  ON_SubDComponentRef* TransferForExperts(int i);

  const ON_SubDComponentRef& operator[](int) const;

  /*
  Returns:
    If the list is clean, the number of subd objects in the list. Mutiple components can belong to the same SubD.
    If the list is not clean, 0.
  Remarks:
    Use Clean() to get a clean list.
  */
  int SubDCount() const;

  /* 
  Returns:
    Number of vertices.
  */
  int VertexCount() const;

  /* 
  Returns:
    Number of vertices with the specified tag.
  */
  int VertexCount(ON_SubD::VertexTag vertex_tag) const;

  /* 
  Returns:
    Number of edges.
  */
  int EdgeCount() const;

  /* 
  Returns:
    Number of edges with the specified tag.
  */
  int EdgeCount(ON_SubD::EdgeTag edge_tag) const;


  /* 
  Returns:
    Number of faces.
  */
  int FaceCount() const;

  /*
  Returns:
    Number of components.
  */
  int ComponentCount() const;

private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_subdimple_sp is private and all code that manages m_subdimple_sp is explicitly implemented in the DLL.
  ON_SimpleArray< class ON_SubDComponentRef* > m_list;
#pragma ON_PRAGMA_WARNING_POP

  int m_subd_count = 0;
  int m_subd_vertex_smooth_count = 0;
  int m_subd_vertex_dart_count = 0;
  int m_subd_vertex_crease_count = 0;
  int m_subd_vertex_corner_count = 0;
  int m_subd_edge_smooth_count = 0;
  int m_subd_edge_crease_count = 0;
  int m_subd_face_count = 0;

  bool m_bIsClean = false; // true if m_list is clean.

  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;
  unsigned int m_reserved3 = 0;
  ON__UINT_PTR m_reserved4 = 0;

private:
  bool Internal_UpdateCount(const ON_SubDComponentRef& r, int delta);
  void Internal_Destroy();
  void Internal_CopyFrom(const ON_SubDComponentRefList& src);
};

//////////////////////////////////////////////////////////////////////////
//
// ON_SubDComponentPoint
//
//  Used in selection tests to return a point and parameters on a component.
//
class ON_CLASS ON_SubDComponentPoint
{
public:
  static const ON_SubDComponentPoint Unset;

  ON_SubDComponentPoint() = default;
  ~ON_SubDComponentPoint() = default;
  ON_SubDComponentPoint(const ON_SubDComponentPoint&) = default;
  ON_SubDComponentPoint& operator=(const ON_SubDComponentPoint&) = default;

  /*
  Description:
    Dictionary compare of component type, component pointer, and component direction (partial).
    This function is useful for sorting arrays of ON_SubDComponentPoint values remove duplicates.
  */
  static int CompareComponentAndDirection(
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
class ON_CLASS ON_SubDMatrix
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
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride
    ) const;

  bool EvaluateSubdivisionPoint(
    unsigned int component_index,
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride,
    double subd_point[3]
    ) const;

  bool EvaluateSurfacePoint(
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride,
    bool bUndefinedNormalIsPossible,
    double limit_point[3],
    double limit_tangent1[3],
    double limit_tangent2[3],
    double limit_normal[3]
    ) const;

  bool EvaluateSurfacePoint(
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride,
    bool bUndefinedNormalIsPossible,
    class ON_SubDSectorSurfacePoint& limit_point
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
  // (C, P[0], Q[0], ...., P[N-1], Q[N-1]), where Q[I] is the vertex of quad F[i] diagonally across from C.
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
    const ON_SubDComponentPtr* component_ring,
    size_t component_ring_count
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
    const unsigned int subd_recursion_count,
    ON_SubDSectorIterator sit,
    ON_SimpleArray<ON_SubDComponentPtr>& component_ring,
    ON_SimpleArray< ON_3dPoint >& subd_points,
    class ON_SubDSectorSurfacePoint& limit_point
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

/*
Description:
  A ON_SubD_FixedSizeHeap is used to manage heap used for a local subdivision.
*/
class ON_CLASS ON_SubD_FixedSizeHeap
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
    Reserve enough room to for a subdivision of a vertex sector.
  Parameters:
    sector_edge_count - [in]
      Number of edges in the sector.
  */
  bool ReserveSubDWorkspace(
    unsigned int sector_edge_count
    );

  /*
  Description:
    Reserve enough room for a local subdivide the neighborhood of center_face.
  Parameters:
    center_face0 - [in]
  */
  bool ReserveSubDWorkspace(
    const ON_SubDFace* center_face0
    );

private:
  bool Internal_ReserveSubDWorkspace(
    size_t vertex_capacity,
    size_t face_capacity,
    size_t array_capacity,
    bool bEnableHashTable
    );

  bool Internal_ReserveSubDWorkspace_HashTable();

public:

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
    unsigned int edge_capacity
    );

  /*
  Description:
    Allocate a vertex located at the vertex0 subdivision point. 
    
  Parameters:
    vertex0 - [in]
    edge_capacity - [in]
      If the returned vertex will be interior in a local subdivision,
      then pass vertex0->EdgeCount().
      If the returned vertex will be an outer ring vertex in a local subdivision,
      then pass 3.
  */
  ON_SubDVertex* AllocateVertex(
    const ON_SubDVertex* vertex0,
    unsigned int edge_capacity
    );

  /*
  Description:
    Allocate a vertex located at the edge0 subdivision point. 
    The vertex will have an edge and face capacity of 4.
  Parameters:
    edge0 - [in]
  */
  ON_SubDVertex* AllocateVertex(
    const ON_SubDEdge* edge0
    );

  /*
  Description:
    Find or allocate a vertex located at the edge0 subdivision point. 
    The vertex will have an edge and face capacity of 4.
  Parameters:
    edge0 - [in]
  Remarks: 
    In order for FindOrAllocateVertex() to find a vertex, that vertex must 
    have been created  by an earlier call to FindOrAllocateVertex().
    Typically, AllocateVertex(edge0) is used for center face boundary edges
    and FindOrAllocateVertex(edge0) is used for ring edges.
  */
  ON_SubDVertex* FindOrAllocateVertex(
    const ON_SubDEdge* edge0
    );

  /*
  Description:
    Allocate a vertex located at the edge0 subdivision point.
    The vertex will have an edge and face capacity of 4.
  Parameters:
    edge0 - [in]
  */
  ON_SubDVertex* AllocateEdgeSubdivisionVertex(
    bool bUseFindOrAllocate,
    const ON_SubDEdge * edge0
  );

  /*
  Description:
    Find or allocate a vertex and the face subdivision point. The vertex will have an
    edge and face capacity of face0->EdgeCount().
  Parameters:
    face0 - [in]
      outer face in a local subdivision situation
  Remarks: 
    In order for FindOrAllocateVertex() to find a vertex, that vertex must 
    have been created  by an earlier call to FindOrAllocateVertex().
    Typically, AllocateVertex(edge0) is used for the center face and
    and FindOrAllocateVertex(edge0) is used for ring faces.
  */
  ON_SubDVertex* FindOrAllocateVertex(
    const ON_SubDFace* face0
    );


  /*
  Description:
    Allocate a vertex located at the sector_face0 subdivision point. 
    The vertex will have an edge and face capacity of 3.
  Parameters:
    sector_face0 - [in]
      A face in a vertex sector.
  */
  ON_SubDVertex* AllocateSectorFaceVertex(
    const ON_SubDFace* sector_face0
    );


  /*
  Parameters:
    v0 - [in]
    v0_sector_weight - [in]
      If v0 null or ON_SubD::VertexTag::Smooth == v0->m_vertex_tag, and v1 is null or tagged,
      then m_sector_weight[0] is set to v0_sector_weight.
      In all other cases the value of v0_sector_weight is ignored and m_sector_weight[0]
      is set to ON_SubDSectorType::IgnoredSectorCoefficient.
    v1 - [in]
    v1_sector_weight - [in]
      If v1 null or ON_SubD::VertexTag::Smooth == v1->m_vertex_tag, and v0 is null or tagged,
      then m_sector_weight[1] is set to v1_sector_weight.
      In all other cases the value of v1_sector_weight is ignored and m_sector_weight[1]
      is set to ON_SubDSectorType::IgnoredSectorCoefficient.
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
  const ON_SubDEdgePtr AllocateEdge(
    ON_SubDVertex* v0,
    double v0_sector_weight,
    ON_SubDVertex* v1,
    double v1_sector_weight
    );

  /*
  Description:
    NOTE WELL: 
    In order for FindOrAllocateEdge() to find an edge, that edge must have been created 
    by an earlier call to FindOrAllocateEdge().
  */
  const ON_SubDEdgePtr FindOrAllocateEdge(
    ON_SubDVertex* v0,
    double v0_sector_weight,
    ON_SubDVertex* v1,
    double v1_sector_weight
    );

  const ON_SubDEdgePtr AllocateEdge(
    bool bUseFindOrAllocatEdge,
    ON_SubDVertex* v0,
    double v0_sector_weight,
    ON_SubDVertex* v1,
    double v1_sector_weight
  );



  private:
  /*
  Returns:
    A face with all field values zero (same values as ON_SubDEdge::Face), except ON_SubDFace.m_id.
  */
  ON_SubDFace* Internal_AllocateFace(
    unsigned int zero_face_id,
    unsigned int parent_face_id
    );
  public:

  ON_SubDFace* AllocateQuad(
    unsigned int zero_face_id,
    unsigned int parent_face_id,
    const ON_SubDEdgePtr eptrs[4]
    );

  ON_SubDFace* AllocateQuad(
    unsigned int zero_face_id,
    unsigned int parent_face_id,
    ON_SubDEdgePtr e0,
    ON_SubDEdgePtr e1,
    ON_SubDEdgePtr e2,
    ON_SubDEdgePtr e3
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
    void* p,
    unsigned int capacity
    );

private:
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
  // Used to find level 1 subdivision vertex from level 0 component
  class ON_SubD_FixedSizeHeap_ComponentPairHashElement** m_hash_table = nullptr;
  class ON_SubD_FixedSizeHeap_ComponentPairHashElement* m_hash_elements = nullptr;
  unsigned int m_h_capacity = 0; // m_hash_table[] capacity.
  unsigned int m_h_count = 0; // 0xFFFFFFFFU means hash is disabled
  enum : unsigned int
  {
    DisabledHashCount = 0xFFFFFFFFU
  };

  bool Internal_HashEnabled() const;
  unsigned int Internal_Hash(ON_SubDComponentPtr component0);
  class ON_SubDVertex* Internal_HashFindVertex1(unsigned int hash, ON_SubDComponentPtr component0);
  void Internal_HashAddPair(unsigned int hash, ON_SubDComponentPtr component0, class ON_SubDVertex* vertex1);

private:
  // copies not supported
  ON_SubD_FixedSizeHeap(const ON_SubD_FixedSizeHeap&) = delete;
  ON_SubD_FixedSizeHeap& operator=(const ON_SubD_FixedSizeHeap&) = delete;
};

class ON_CLASS ON_SubDEdgeChain
{
public:
  ON_SubDEdgeChain() = default;
  ~ON_SubDEdgeChain() = default;
  ON_SubDEdgeChain(const ON_SubDEdgeChain&) = default;
  ON_SubDEdgeChain& operator=(const ON_SubDEdgeChain&) = default;

public:
  static const ON_SubDEdgeChain Empty;

public:

  /////////////////////////////////////////////////////////
  //
  // Edge chain tools
  //
  /*
  Description:
    Sort edges into a chains

  Parameters:
    unsorted_edges - [in]
      To sort an array in place, pass the same array as both parameters.
      If unsorted_edges[] contains three or more edges that share a common vertex,
      then all of the edges that share that vertex are ignored.
      The edges can be from one or more SubDs.

    unsigned int minimum_chain_length - [in]
      minimum number of edges to consider for a chain.

    edge_chains - [out]
      The edge_chains[] has the edges grouped into edge chains.

      In an edge chain subsequent edges share a common vertex; i.e.
      edge_chains[i].RelativeVertex(1) == edge_chains[i+1].RelativeVertex(0).

      When edge_chains[i].RelativeVertex(1) != edge_chains[i+1].RelativeVertex(0),
      a chain ends at edge_chains[i] and another begins at edge_chains[i+1].

      The first edge in every chain has the same orientation as the input edge
      from edge_chains[].

  Returns:
    Number of chains in edge_chains[].
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const ON_SimpleArray< ON_SubDEdgePtr >& unsorted_edges,
    unsigned int minimum_chain_length,
    ON_SimpleArray< ON_SubDEdgePtr >& edge_chains
  );

  static unsigned int SortEdgesIntoEdgeChains(
    const ON_SimpleArray< const ON_SubDEdge* >& unsorted_edges,
    unsigned int minimum_chain_length,
    ON_SimpleArray< ON_SubDEdgePtr >& edge_chains
  );

  static unsigned int SortEdgesIntoEdgeChains(
    const ON_SimpleArray< ON_SubDComponentPtr >& unsorted_edges,
    unsigned int minimum_chain_length,
    ON_SimpleArray< ON_SubDEdgePtr >& sorted_edges
  );

  static unsigned int SortEdgesIntoEdgeChains(
    const ON_SubD& subd,
    const ON_SimpleArray< ON_COMPONENT_INDEX >& unsorted_edges,
    unsigned int minimum_chain_length,
    ON_SimpleArray< ON_SubDEdgePtr >& edge_chains
  );

  /*
  Description:
    Orient edges[] into edge chains preserving the order of edges[].
  Returns:
    Number of chains in edge_chains[].
  */
  static unsigned int OrientEdgesIntoEdgeChains(
    const ON_SubD& subd,
    const ON_SimpleArray< ON_COMPONENT_INDEX >& edges,
    ON_SimpleArray< ON_SubDEdgePtr >& edge_chains
  );

  /*
  Description:
    Orient edges[] into edge chains preserving the order of edges[].
  Returns:
    Number of chains in edge_chains[].
  */
  static unsigned int OrientEdgesIntoEdgeChains(
    const ON_SimpleArray< const ON_SubDEdge* >& edges,
    ON_SimpleArray< ON_SubDEdgePtr >& edge_chains
  );

  /*
  Description:
    Orient edges[] into edge chains preserving the order of edges[].
  Returns:
    Number of chains in edge_chains[].
  */
  static unsigned int OrientEdgesIntoEdgeChains(
    const ON_SimpleArray< ON_SubDComponentPtr >& edges,
    ON_SimpleArray< ON_SubDEdgePtr >& edge_chains
  );


  /////////////////////////////////////////////////////////
  //
  // Edge chain tools
  //

  /*
  Description:
    Get the neighboring link in an edge chain.
  Parameters:
    starting_edge - [in]
    search_direction - [in]
      ON_ChainDirection::Previous or ON_ChainDirection::Next.
      The search direction is relative to this->EdgeDirection().
    chain_type - [in]
      Determines what edge/vertex tag conditions must be satisified by the neighbor.
  Returns:
    The next or previous edge in the chain if it exists.
    Otherwise, nullptr is returned.
  Remarks:
    When multiple edges are candidates, there is a bias to preserve smooth/crease and a bias to
    preserve face count. If the biases don't reduce the list of candidates to one or bStopAtBreak is true
    and a creaase/smooth break is encountered, then ON_SubDEdgePtr::Null is returned.
  */
  static const ON_SubDEdgePtr EdgeChainNeighbor(
    ON_SubDEdgePtr starting_edge,
    ON_ChainDirection search_direction,
    ON_SubD::ChainType chain_type
  );

  /*
  Description:
    Get the neighboring link in an edge chain.
  Parameters:
    starting_edge - [in]
    search_direction - [in]
      ON_ChainDirection::Previous or ON_ChainDirection::Next.
      The search direction is relative to this->EdgeDirection().
    chain_type - [in]
      Determines what edge/vertex tag conditions must be satisified by the neighbor.
    bEnableStatusCheck - [in]
    status_pass - [in]
    status_fail - [in]
      If bEnableStatusFilter is false, then no status checks are performed.
      If bEnableStatusFilter is true, ON_ComponentStatus::StatusCheck(candidate_edge->m_status,status_pass,status_fail)
      must be true for candidate_edge to be returned.
  Returns:
    The next or previous edge in the chain if it exists.
    Otherwise, nullptr is returned.
  Remarks:
    When multiple edges are candidates, there is a bias to preserve smooth/crease and a bias to
    preserve face count. If the biases don't reduce the list of candidates to one or bStopAtBreak is true
    and a creaase/smooth break is encountered, then ON_SubDEdgePtr::Null is returned.
  */
  static const ON_SubDEdgePtr EdgeChainNeighbor(
    ON_SubDEdgePtr starting_edge,
    ON_ChainDirection search_direction,
    ON_SubD::ChainType chain_type,
    bool bEnableStatusCheck,
    ON_ComponentStatus status_pass,
    ON_ComponentStatus status_fail
  );

  /*
  Description:
    Reverse the orientation of the elements and the order
    of the elements in the edge_chain[] array.
  Parameters:
    edge_chain - [in/out]
  */
  static void ReverseEdgeChain(
    ON_SimpleArray< ON_SubDEdgePtr >& edge_chain
  );

  /*
  Description:
    Reverse the orientation of the elements and the order
    of the elements in the edge_chain[] array.
  Parameters:
    edge_count - [in]
      Number of elements in edge_chain[]
    edge_chain - [in/out]
  */
  static void ReverseEdgeChain(
    ON_SubDEdgePtr* edge_chain,
    size_t edge_count
    );
  
  /*
  Description:
    Checks that all edge and vertex pointers are not nullptr 
    and that adjacent edges in the list share the same vertex.
  Parameters:
    edge_chain - [in]
      Edge chain to test
    bCheckForDuplicateEdges - [in]
      If bCheckForDuplicateEdges true, then false is returned if
      there are duplicate edges or duplicate interior vertices.
  Returns:
    True if edge_chain[] is valid.
  */
  static bool IsValidEdgeChain(
    const ON_SimpleArray< ON_SubDEdgePtr >& edge_chain,
    bool bCheckForDuplicateEdges
  );

  /*
  Description:
    Checks that all edge and vertex pointers are not nullptr 
    and that adjacent edges in the list share the same vertex.
  Parameters:
    edge_count - [in]
      Number of elements in edge_chain[] to test.
    edge_chain - [in]
      Edge chain to test
    bCheckForDuplicateEdges - [in]
      If bCheckForDuplicateEdges true, then false is returned if
      there are duplicate edges or duplicate interior vertices.
  Returns:
    True if edge_chain[] is valid.
  */
  static bool IsValidEdgeChain(
    const ON_SubDEdgePtr* edge_chain,
    size_t edge_count,
    bool bCheckForDuplicateEdges
  );

public:
  /*
  Returns:
    Current EdgeChain
  */
  const ON_SimpleArray<ON_SubDEdgePtr>& EdgeChain() const;


  const ON_SubD& SubD() const;
  const ON_SubDRef SubDRef() const;

  bool InChain(
    const ON_SubDEdgePtr edge_ptr
  ) const;

  bool InChain(
    const ON_SubDEdge* edge
  ) const;

  bool InChain(
    const ON_SubDVertex* vertex
  ) const;

  /*
  Returns:
    True if the edge chain has 3 or more edges that form a closed loop.
  */
  bool IsClosedLoop() const;

  /*
  Parameters:
    bStrictlyConvex - [in]
      If true, then a colinear pair of edges is not considered convex.
  Returns:
    True if the edge chain has 3 or more edges that forma a convex loop.
  */
  bool IsConvexLoop(
    bool bStrictlyConvex
  ) const;


  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be ON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    ON_UUID persistent_subd_id,
    ON_SubDRef subd_ref,
    const ON_SubDEdge* initial_edge
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be ON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    ON_UUID persistent_subd_id,
    ON_SubDRef subd_ref,
    const ON_SimpleArray<const ON_SubDEdge* >& initial_edge_chain
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be ON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    ON_UUID persistent_subd_id,
    ON_SubDRef subd_ref,
    const ON_SubDEdge*const* initial_edge_chain,
    size_t edge_count
    );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be ON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    ON_UUID persistent_subd_id,
    ON_SubDRef subd_ref,
    ON_SubDEdgePtr initial_edge
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be ON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    ON_UUID persistent_subd_id,
    ON_SubDRef subd_ref,
    const ON_SimpleArray<ON_SubDEdgePtr>& initial_edge_chain
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be ON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    ON_UUID persistent_subd_id,
    ON_SubDRef subd_ref,
    const ON_SubDEdgePtr* initial_edge_chain,
    size_t edge_count
    );

  unsigned int EdgeCount() const;

  void SetStatusCheck(
    bool bEnableStatusCheck,
    ON_ComponentStatus status_check_pass,
    ON_ComponentStatus status_check_fail
  );

  bool StatusCheckEnabled() const;

  const ON_ComponentStatus StatusCheckPass() const;

  const ON_ComponentStatus StatusCheckFail() const;

  bool StatusCheck(
    const ON_SubDEdge* edge
  ) const;

  void Reverse();

  const ON_SubDEdgePtr FirstEdgePtr() const;
  const ON_SubDEdgePtr LastEdgePtr() const;
  const ON_SubDEdgePtr EdgePtr(int edge_index) const;

  const ON_SubDEdge* FirstEdge() const;
  const ON_SubDEdge* LastEdge() const;
  const ON_SubDEdge* Edge(int edge_index) const;

  const ON_SubDVertex* FirstVertex() const;
  const ON_SubDVertex* LastVertex() const;
  const ON_SubDVertex* Vertex(int vertex_index) const;

  unsigned int AddOneNeighbor(
    ON_ChainDirection direction,
    ON_SubD::ChainType chain_type
    );

  unsigned int AddAllNeighbors(
    ON_ChainDirection direction,
    ON_SubD::ChainType chain_type
    );

  unsigned int AddEdge(
    const ON_SubDEdge* edge
    );

  unsigned int RemoveEdges(
    const ON_SubDEdge* first_edge,
    const ON_SubDEdge* last_edge
  );

  void ClearEdgeChain();

  /*
  Returns:
    The persistent id of the parent subd object.
    If the context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
    If the context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
  */
  const ON_UUID PersistentSubDId() const;

  bool HasPersistentEdgeIds() const;

  bool HasRuntimeEdgePtrs() const;

  bool SetPersistentEdgeIdsFromRuntimeEdgePtrs() const;
    
  /*
  Description:
    In situations where this edge chain is being read from a 3dm archive,
    or a similar serialization context, this function uses the saved edge
    id information to initialize the runtime ON_SubDEdgePtr information.
  Parameters:
    persistent_subd_id - [in]
      This id is passed to insure it matches the saved persistent_subd_id. 
      The source of the id depends on the context of the model managing the subd.
      If that context is an ONX_Model, then the persistent id is the ON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
  Returns:
    True if successful.
    False if not successful.
  */
  bool SetRuntimeEdgePtrsFromPersistentSubD(
    ON_UUID persistent_subd_id,
    ON_SubDRef persistent_subd_ref
  );

  bool Write(class ON_BinaryArchive& archive) const;
  bool Read(class ON_BinaryArchive& archive);
  void Dump(class ON_TextLog& text_log) const;

private:
  ON_SubDRef m_subd_ref;
  ON_SimpleArray<ON_SubDEdgePtr> m_edge_chain;


  // If m_persistent_subd_id, then the id identifies the parent subd in the model.
  // In an ONX_Model, this is the ON_ModelGeometryComponent.Id() value
  // of the corresponding ON_SubD in the ONX_Model.
  // In Rhino, this is the CRhinoObject.ModelObjectId() value
  // of the corresponding CRhinoSubDObject in the CRhinoDoc.
  ON_UUID m_persistent_subd_id = ON_nil_uuid;

  // If m_persistent_subd_id is set and m_persistent_edge_id[] is not empty,
  // m_persistent_edge_id[] is a list of edge ids and orientations.
  // unsigned id = m_persistent_edge_id[] & 0xFFFFFFFF.
  // reversed orientation = (0 != (m_persistent_edge_id[] & 0x8000000000000000)).
  // The persistent id information is saved in 3dm archives. When that archive is read,
  // the SetRuntimeEdgePtrsFromPersistentSubD() can be used to set the runtime edge chain values.
  // These mutable fields are set by Write and used by Read.
  mutable ON_SimpleArray<unsigned int> m_persistent_edge_id;
  mutable ON_SimpleArray<ON__UINT8> m_persistent_edge_orientation; // 0 = not reversed, 1 = reversed.

  ON_UniqueTester m_unique_tester;
  ON_ComponentStatus m_status_check_pass = ON_ComponentStatus::NoneSet;
  ON_ComponentStatus m_status_check_fail = ON_ComponentStatus::Selected;
  bool m_bEnableStatusCheck = false;
};

class ON_CLASS ON_SubDComponentFilter
{
public:
  ON_SubDComponentFilter() = default;
  ~ON_SubDComponentFilter() = default;
  ON_SubDComponentFilter(const ON_SubDComponentFilter&) = default;
  ON_SubDComponentFilter& operator=(const ON_SubDComponentFilter&) = default;

public:

  ///<summary>
  /// No filters are set and all components are accepted.
  ///</summary>
  static const ON_SubDComponentFilter Unset;

  ///<summary>
  /// Only vertices are accepted.
  ///</summary>
  static const ON_SubDComponentFilter OnlyVertices;

  ///<summary>
  /// Only edges are accepted.
  ///</summary>
  static const ON_SubDComponentFilter OnlyEdges;

  ///<summary>
  /// Only faces are accepted.
  ///</summary>
  static const ON_SubDComponentFilter OnlyFaces;

  /*
  Parameters:
    bAcceptVertices - [in]
      If true, all vertices are accepted. Otherwise, all vertices are rejected.
    bAcceptEdges - [in]
      If true, all edges are accepted. Otherwise all edges are rejected.
    bAcceptFaces - [in]
      If true, all faces are accepted. Otherwise all faces are rejected.
  */
  static const ON_SubDComponentFilter Create(
    bool bAcceptVertices,
    bool bAcceptEdges,
    bool bAcceptFaces
  );

public:
  ///<summary>
  /// Topology filters.
  ///</summary>
  enum class Topology : unsigned char
  {
    ///<summary>
    /// No topology filter.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// A boundary vertex has a single sector bounded by two boundary edges.
    /// A boundary edge has a single face.
    /// A boundary face has at least one boundary edge.
    ///</summary>
    Boundary = 1,

    ///<summary>
    /// An interior vertex has the same number of edges and faces and all edges are interior.
    /// An interior edge has two faces.
    /// An interior face has all interior edges.
    ///</summary>
    Interior = 2,

    ///<summary>
    /// A nonmanifold vertex is a vertex that is neither boundary nor interior.
    /// A nonmanifold edge is an edge that is neither boundary nor interior.
    /// A nonmanifold face is a face that is neither boundary nor interior.
    ///</summary>
    Nonmanifold = 4,

    ///<summary>
    /// A component that is either boundary or interior.
    ///</summary>
    BoundaryOrInterior = 3,

    ///<summary>
    /// A component that is either boundary or nonmanifold.
    ///</summary>
    BoundaryOrNonmanifold = 5,

    ///<summary>
    /// A component that is either interior or nonmanifold
    ///</summary>
    InteriorOrNonmanifold = 6
  };

  bool AcceptComponent(
    const class ON_Geometry* geometry
  ) const;

  bool AcceptComponent(
    ON_COMPONENT_INDEX component_index,
    const class ON_Geometry* geometry
  ) const;

  bool AcceptComponent(
    const class ON_SubDComponentRef* cref
  ) const;

  /*
  Returns:
    True if the filter accepts the component. False otherwise.
  */
  bool AcceptComponent(ON_SubDComponentPtr cptr) const;

  /*
  Returns:
    True if the filter accepts the vertex. False otherwise.
  */
  bool AcceptVertex(ON_SubDVertexPtr vptr) const;

  /*
  Returns:
    True if the filter accepts the edge. False otherwise.
  */
  bool AcceptEdge(ON_SubDEdgePtr eptr) const;

  /*
  Returns:
    True if the filter accepts the face. False otherwise.
  */
  bool AcceptFace(ON_SubDFacePtr fptr) const;

  /*
  Returns:
    True if the filter accepts the vertex. False otherwise.
  */
  bool AcceptVertex(const ON_SubDVertex* v) const;

  /*
  Returns:
    True if the filter accepts the edge. False otherwise.
  */
  bool AcceptEdge(const ON_SubDEdge* e) const;
  
  /*
  Returns:
    True if the filter accepts the face. False otherwise.
  */
  bool AcceptFace(const ON_SubDFace* f) const;  

  void SetAcceptVertices(bool bAcceptVertices);

  bool AcceptVertices() const;

  void SetAcceptEdges(bool bAcceptEdges);

  bool AcceptEdges() const;

  void SetAcceptFaces(bool bAcceptFaces);

  bool AcceptFaces() const;

  void SetVertexTopologyFilter(ON_SubDComponentFilter::Topology vertex_topology_filter);

  void ClearVertexTopologyFilter();

  ON_SubDComponentFilter::Topology VertexTopologyFilter() const;

  void SetEdgeTopologyFilter(ON_SubDComponentFilter::Topology edge_topology_filter);

  ON_SubDComponentFilter::Topology EdgeTopologyFilter() const;

  void ClearEdgeTopologyFilter();

  void SetFaceTopologyFilter(ON_SubDComponentFilter::Topology face_topology_filter);

  ON_SubDComponentFilter::Topology FaceTopologyFilter() const;

  void ClearFaceTopologyFilter();

  bool AcceptVertexTag(ON_SubD::VertexTag vertex_tag) const;

  void AddAcceptedVertexTag(ON_SubD::VertexTag vertex_tag);

  void ClearVertexTagFilter();

  bool AcceptEdgeTag(ON_SubD::EdgeTag edge_tag) const;

  void AddAcceptedEdgeTag(ON_SubD::EdgeTag edge_tag);

  void ClearEdgeTagFilter();

  bool AcceptFaceEdgeCount(
    unsigned face_edge_count
  ) const;

  void SetFaceEdgeCountFilter(
    unsigned minimum_face_edge_count,
    unsigned maximum_face_edge_count
  );
  
  void ClearFaceEdgeCountFilter();

private:
  bool m_bRejectVertices = false;
  ON_SubDComponentFilter::Topology m_vertex_topology_filter = ON_SubDComponentFilter::Topology::Unset;
  ON_SubD::VertexTag m_vertex_tag_filter[4] = {};

  bool m_bRejectEdges = false;
  ON_SubDComponentFilter::Topology m_edge_topology_filter = ON_SubDComponentFilter::Topology::Unset;
  ON_SubD::EdgeTag m_edge_tag_filter[2] = {};

  bool m_bRejectFaces = false;
  ON_SubDComponentFilter::Topology m_face_topology_filter = ON_SubDComponentFilter::Topology::Unset;
  unsigned m_minimum_face_edge_count = 0U;
  unsigned m_maximum_face_edge_count = 0U;
};


#if defined(ON_COMPILING_OPENNURBS)
/*
The ON_SubDAsUserData class is used to attach a subd to it proxy mesh
when writing V6 files in commercial rhino.
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
