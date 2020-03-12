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

#if !defined(OPENNURBS_SYMMETRY_INC_)
#define OPENNURBS_SYMMETRY_INC_

//////////////////////////////////////////////////////////////////////////
//
// ON_Symmetry
//
class ON_CLASS ON_Symmetry
{
public:
  ON_Symmetry() = default;
  ~ON_Symmetry() = default;
  ON_Symmetry(const ON_Symmetry&) = default;
  ON_Symmetry& operator=(const ON_Symmetry&) = default;

public:
  static const ON_Symmetry Unset;

  enum : unsigned int
  {
    MaximumOrder = 4096
  };

  enum class Type : unsigned char
  {
    Unset = 0,

    /// <summary>
    /// Reflection about a plane.
    /// The symmetric object has 2 copies of the motif.
    /// Points on the reflection plane are fixed.
    /// </summary>
    Reflect = 1,

    /// <summary>
    /// Rotation around an axis.
    /// The symmetric object has reflection copies of the motif.
    /// Points on the axis are fixed.
    /// </summary>
    Rotate = 2,

    /// <summary>
    /// Reflection and rotation alternate.
    /// Rotation angle is 360/N degrees
    /// The symmetric object has 2N copies of the motif.
    /// Points on the rotation axis are fixed.
    /// </summary>
    ReflectAndRotate = 3,

    /// <summary>
    /// General case inversion symmetry.
    /// (inversion transformation)^2 = identity.
    /// Det(inversion transformation) = -1.
    /// The symmetric object has 2 copies of the motif.
    /// </summary>
    Inversion = 4,

    /// <summary>
    /// General case cyclic symmtry (order >= 2)
    /// (cyclic transformation)^N = identity. 
    /// When N is 2 or odd, Det(cyclic transformation) = 1.
    /// When N is even and greater than 2, Det(cyclic transformation) = 1 or -1.
    /// The symmetric object has N copies of the motif.
    /// </summary>
    Cyclic = 5,
  };

  static ON_Symmetry::Type SymmetryTypeFromUnsigned(unsigned int symmetry_type_as_unsigned);

  static const ON_wString SymmetryTypeToString(
    ON_Symmetry::Type symmetry_type
  );


  enum class Coordinates : unsigned char
  {
    Unset = 0,

    /// <summary>
    /// The symmetry is associated with an object is is applied to.
    /// If that object is transformed, the symmetry's planes and rotation axes
    /// are also transformed.
    /// </summary>
    Object = 1,

    /// <summary>
    /// The symmetry is indepenent of any objects is it applied to.
    /// The symmetry's planes and rotation axes are not changed when
    /// any of those objects are transformed.
    /// </summary>
    World = 2,
  };

  static ON_Symmetry::Coordinates SymmetryCoordinatesFromUnsigned(unsigned int coordinates_as_unsigned);

  static const ON_wString SymmetryCoordinatesToString(
    ON_Symmetry::Coordinates symmetry_coordinates
  );

  static const ON_UUID ReflectId;
  static const ON_UUID RotateId;
  static const ON_UUID ReflectAndRotateId;

  /*
  Description:
    Reflection about a plane.
    The symmetric object has 2 copies of the motif.
    Points on the reflection plane are fixed.
  Parameters:
    reflection_plane - [in]
    symmetry_coordinates - [in]
      object or world.
  Example:
    If the reflection_plane is the y-z plane,
    then (x,y,z) -> (-x,y,z) -> (x,y,z).
  Remarks:
    InversionTransform() = reflection.
    CyclicTransform() = identity.
  */
  static const ON_Symmetry CreateReflectSymmetry(
    ON_PlaneEquation reflection_plane,
    ON_Symmetry::Coordinates symmetry_coordinates
  );

  /*
  Description:
    Rotation around an axis.
    The symmetric object has reflection copies of the motif.
    Points on the axis are fixed.    
  Parameters:
    rotation_axis - [in]
    rotation_count - [in]
      rotation_count must be >= 2 and the rotation angle is (360/rotation_count) degrees.
    symmetry_coordinates - [in]
      object or world.
  Example:
    If the rotation axis is the z-axis and the order is N, then
    then (x,y,z) -> (r*cos(a), r*sin(a), z) -> (r*cos(2*a), r*sin(2*a), z) -> ... -> (x,y,z),
    where r = sqrt(x*x + y*y) and a = (360 degrees)/N.
  Remarks:
    CyclicTransform() = rotation.
    InversionTransform() = identity.
  */
  static const ON_Symmetry CreateRotateSymmetry(
    ON_Line rotation_axis, 
    unsigned int rotation_count,
    ON_Symmetry::Coordinates symmetry_coordinates
  );

  /*
  Description:
    Reflection and rotation alternate.
    The symmetric object has 2*reflection copies of the motif.
    Points on the rotation axis are fixed.    
  Parameters:
    reflection_plane - [in]
    rotation_axis - [in]
      A line in the reflection plane.
    rotation_count - [in]
      rotation_count must be >= 2 and the rotation angle is (360/rotation_count) degrees.
    symmetry_coordinates - [in]
      object or world.
  Example:
    If the reflection_plane is the y-z plane, the rotation axis is the z-axis,
    and rotation_count = 2, 
    then (x,y,z) -> (-x,y,z) -> (-x-y,z) -> (x,-y,z) -> (x,y,z).
  Remarks:
    InversionTransform() = reflection.
    CyclicTransform() = rotation by (360/rotation_count) degrees.
  */
  static const ON_Symmetry CreateReflectAndRotateSymmetry(
    ON_PlaneEquation reflection_plane, 
    ON_Line rotation_axis, 
    unsigned int rotation_count,
    ON_Symmetry::Coordinates symmetry_coordinates
  );

  /*
  Description:
    Create an inversion symmetry from a transformation.
    The symmetric object has 2 copies of the modif.
  Parameters:
    symmetry_id - [in]
      An id you can assign to the symmetry
    inversion_transform - [in]
      inversion_transform^2 = identity.
      Det(inversion_transform) = -1.
    symmetry_coordinates - [in]
      object or world.
  Remarks:
    If inversion_transform is a reflection, consider using CreateReflectSymmetry() instead.
    When Det(transformation) = 1 and transformtion^2 = identity, use CreateCyclicSymmetry() instead.
  */
  static const ON_Symmetry CreateInversionSymmetry(
    ON_UUID symmetry_id, 
    ON_Xform inversion_transform,
    ON_Symmetry::Coordinates symmetry_coordinates
  );

  /*
  Description:
    Create a cyclic symmetry from a transformation.
    The symmetric object has order copies of the modif.
  Parameters:
    symmetry_id - [in]
      An id you can assign to the symmetry
    cyclic_transform - [in]
      cyclic_transform^order = identity
      cyclic_transform^i != identity when 0 < i < order
      If order is even and at least 4, then Det(cyclic_transform) = 1 or -1.
      Otherwise Det(cyclic_transform) = 1.
    cyclic_order - [in]
      cyclic_order >= 2
    symmetry_coordinates - [in]
      object or world.
  Remarks:
    If cyclic_transform is a rotation, use CreateRotationSymmetry().
    If cyclic_transform is a reflection, use CreateReflectionSymmetry().
    If 2 = cyclic_order and Det(cyclic_transform) = -1, use CreateInversionSymmetry().
  */
  static const ON_Symmetry CreateCyclicSymmetry(
    ON_UUID symmetry_id, 
    ON_Xform cyclic_transform, 
    unsigned int cyclic_order,
    ON_Symmetry::Coordinates symmetry_coordinates
  );

  static int Compare(const ON_Symmetry* lhs, const ON_Symmetry* rhs);

  /*
  Returns:
    0 if the symmetry transformations are the same to tolerance.
    Otherwise returns ON_Symmetry::Compare(lhs,rhs). 
  */
  static int CompareSymmetryTransformation(const ON_Symmetry* lhs, const ON_Symmetry* rhs, double zero_tolerance);

public:
  /*
  Returns:
    Symmetry type.
  */
  ON_Symmetry::Type SymmetryType() const;

  /*
  Returns:
    Symmetry type.
  */
  ON_Symmetry::Coordinates SymmetryCoordinates() const;

  /*
  Returns:
    Symmetry unique id
  */
  const ON_UUID SymmetryId() const;

  /*
  Descripton:
    Set this instance to ON_Symmetry::Unset.
  */
  void Clear();

  /*
  Rturns:
    True if this instance is set to a symmetry.
  */
  bool IsSet() const;

  /*
  Rturns:
    True if this instance is not set.
  */
  bool IsUnset() const;

  /*
  Returns:
    Number of types the motif appears in the symmetric object.
  Remarks:
    MotifCount() = CyclicOrder()*InversionOrder().
  */
  unsigned int MotifCount() const;

  /*
  Returns:
    0: unset symmetry
    1: InversionTransform() = identity
    2: InversionTransform()^2 = identity and InversionTransform() != identity
  Remarks:
    In common cases, InversionTransform() is either the identity or a reflection.
  */
  unsigned int InversionOrder() const;

  /*
  Returns:
    0: unset symmetry
    1: the cyclic transformation is the identity.
    N >= 2: CyclicTransform()^N = idenity and CyclicTransform()^i != idenity when 0 < i < N.
  Remarks:
    In common cases, CyclicTransform() is either the identity or a rotation.
  */
  unsigned int CyclicOrder() const;

  /*
  Description:
    Get the transformation that maps the starting motif to the specified copy.
  Parameters:
    index - [in]
      0 based index of the copy (negative values are supported)
  Remarks:
    "0 based" means when index is a multiple of MotifCount(), the identity is returned.
  */
  const ON_Xform MotifTransformation(
    int index
  ) const;

  /*
  Returns:
    The inversion transformation is returned.
    InversionTransform()^InversionOrder() = identity.
  Remarks:
    In common cases, InversionTransform() is either the identity or a reflection (mirror).
  Remarks:
    NOTE: A symmetry with SymmetryOrder() = 2 and transformation S can be represented
    as either InversionTransform() = S and CyclicTransform() = identity or
    or CyclicTransform() = S and InversionTransform() = idenity.
    The convention is to use the cyclic transforms when S is a 180 rotations
    and inversion transforms otherwise.
  */
  const ON_Xform InversionTransform() const;

  /*
  Returns:
    The cyclic transformation is returned.
    CyclicTransform()^CyclicOrder() = identity.
  Remarks:
    In common cases, CyclicTransform() is either the identity or a rotation.
  Remarks:
    NOTE: A symmetry with SymmetryOrder() = 2 and transformation S can be represented
    as either InversionTransform() = S and CyclicTransform() = identity or
    or CyclicTransform() = S and InversionTransform() = idenity.
    The convention is to use the cyclic transforms when S is a 180 rotations
    and inversion transforms otherwise.
  */
  const ON_Xform CyclicTransform() const;

  /*
  Returns:
    A SHA1 hash uniquely identifying the symmetry
  */
  const ON_SHA1_Hash Hash() const;

  /*
  Returns:
    If the symmetry is type is Reflect or ReflectAndRotate, then the reflection plane is returned.
    Othewise ON_Plane::Nan is returned.
  */
  const ON_PlaneEquation ReflectionPlane() const;

  /*
  Returns:
    If the symmetry is type is Rotate or ReflectAndRotate, then the rotation axis is returned.
    Othewise ON_Line::Nan is returned.
  */
  const ON_Line RotationAxis() const;

  /*
  Returns:
    If the symmetry is type is Rotate or ReflectAndRotate, then a point on the rotation axis is returned.
    Othewise ON_3dPoint::Nan is returned.
  */
  const ON_3dPoint RotationAxisPoint() const;

  /*
  Returns:
    If the symmetry is type is Rotate or ReflectAndRotate, then the direction of the rotation axis is returned.
    Othewise ON_3dVector::Nan is returned.
  Remarks:
    This vector may have length != 1
  */
  const ON_3dVector RotationAxisDirection() const;

  /*
  Returns:
    If the symmetry is type is Rotate or ReflectAndRotate, then a unit vector in the direction of the rotation axis is returned.
    Othewise ON_3dVector::Nan is returned.
  */
  const ON_3dVector RotationAxisTangent() const;

  /*
  Returns:
    If the symmetry is type is Rotate or ReflectAndRotate, then the rotation count is returned.
    Othewise 0 is returned.
  */
  unsigned int RotationCount() const;

  /*
  Returns:
    If the symmetry is type is Rotate or ReflectAndRotate, then the rotation angle in degrees is returned.
    Othewise ON_DBL_QNAN is returned.
  Remarks:
    RotationAngleDegrees() = 360.0/RotationCount()
  */
  double RotationAngleDegrees() const;

  /*
  Returns:
    If the symmetry is type is Rotate or ReflectAndRotate, then the rotation angle in radians is returned.
    Othewise ON_DBL_QNAN is returned.
  Remarks:
    RotationAngleRadians() = (2.0*ON_PI)/RotationCount()
  */
  double RotationAngleRadians() const;

public:
  bool Write(class ON_BinaryArchive&) const;
  bool Read(class ON_BinaryArchive&);
  void Dump(class ON_TextLog&) const;

  /*
  Description:
    If SymmetryCoordinates() = ON_Symmetry::Coordinates::Object, then the symmetry defition
    is transformed.
  */
  const ON_Symmetry TransformConditionally(const ON_Xform& xform) const;

  /*
  Description:
    The the symmetry defition is transformed.
  */
  const ON_Symmetry TransformUnconditionally(const ON_Xform& xform) const;

public:
  /*
  Returns:
    A SHA1 hash value that uniquely identifies the symmetry settings.
  Remarks:
    The symmetric object content serial number is not incuded in the hash.
  */
  ON_SHA1_Hash Sha1Hash() const;

  /*
  Description:
    Set the mutable value returned by SymmetricObjectContentSerialNumber().
  */
  void SetSymmetricObjectContentSerialNumber(ON__UINT64 symmetric_object_content_serial_number) const;

  /*
  Description:
    Set the mutable value returned by SymmetricObjectContentSerialNumber() to 0.
  */
  void ClearSymmetricObjectContentSerialNumber() const;

  /*
  Returns:
    Set a runtime serial number that corresponded to the content of the symmetric object 
    after it the application verified it was symmetric.
  */
  ON__UINT64 SymmetricObjectContentSerialNumber() const;

private:
  static const double ZeroTolerance;
  
  ON_Symmetry::Type m_type = ON_Symmetry::Type::Unset;

  ON_Symmetry::Coordinates m_coordinates = ON_Symmetry::Coordinates::Unset;

  // m_inversion_order (0 = unset, 1 = identity (no inversion), 2 = (non-identity inversion)
  unsigned char m_inversion_order = 0;

  unsigned char m_reserved1 = 0;

  // m_cyclic_order (0 = unset, 1 = identity (no cyclic), >= 2 cyclic order (non-identity cyclic)
  unsigned int m_cyclic_order = 0;

  mutable ON__UINT64 m_symmetric_object_content_serial_number = 0;

  // id is a preset value for the 3 built in symmetries and user defined for others
  ON_UUID m_id = ON_nil_uuid;
  
  // m_inversion_transform^2 = identity
  ON_Xform m_inversion_transform = ON_Xform::Nan;
  
  // m_cyclic_transform^m_cyclic_order = identity
  ON_Xform m_cyclic_transform = ON_Xform::Nan;

  // Set when type is Reflect or ReflectAndRotate
  ON_PlaneEquation m_reflection_plane = ON_PlaneEquation::NanPlaneEquation;

  // Set when type is Rotate or ReflectAndRotate
  ON_Line m_rotation_axis = ON_Line::NanLine;

private:
  ON__UINT_PTR m_reservedX = 0;
  ON__UINT_PTR m_reservedY = 0;
  double m_reservedA = 0.0;
  double m_reservedB = 0.0;
  double m_reservedC = 0.0;
  double m_reservedD = 0.0;

private:
  static int Internal_CompareDouble(const double* lhs, const double* rhs, size_t count);
  const ON_Xform Internal_ReflectAndRotateTransformation(unsigned index) const;
  static const ON_Xform Internal_RotationXform(ON_Line axis, int rotation_index, int rotation_count);
  const ON_Xform Internal_RotationXform(int rotation_index, int rotation_count) const;
};

#endif
