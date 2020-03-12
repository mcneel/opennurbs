/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2019 Robert McNeel & Associates. All rights reserved.
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

#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

ON_Symmetry::Type ON_Symmetry::SymmetryTypeFromUnsigned(unsigned int symmetry_type_as_unsigned)
{
  switch (symmetry_type_as_unsigned)
  {
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Type::Unset);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Type::Reflect);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Type::Rotate);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Type::ReflectAndRotate);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Type::Inversion);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Type::Cyclic);
  }

  ON_ERROR("Invalid type_as_unsigned parameter");
  return ON_Symmetry::Type::Unset;
}

const ON_wString ON_Symmetry::SymmetryTypeToString(ON_Symmetry::Type symmetry_type)
{
  const wchar_t* s;
  switch (symmetry_type)
  {
  case ON_Symmetry::Type::Unset:
    s = L"Unset";
    break;
  case ON_Symmetry::Type::Reflect:
    s = L"Reflect";
    break;
  case ON_Symmetry::Type::Rotate:
    s = L"Rotate";
    break;
  case ON_Symmetry::Type::ReflectAndRotate:
    s = L"ReflectAndRotate";
    break;
  case ON_Symmetry::Type::Inversion:
    s = L"Inversion";
    break;
  case ON_Symmetry::Type::Cyclic:
    s = L"Cyclic";
    break;
  default:
    s = nullptr;
    break;
  }
  return ON_wString(s);
}


ON_Symmetry::Coordinates ON_Symmetry::SymmetryCoordinatesFromUnsigned(unsigned int symmetry_coordinates_as_unsigned)
{
  switch (symmetry_coordinates_as_unsigned)
  {
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Coordinates::Unset);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Coordinates::Object);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_Symmetry::Coordinates::World);
  }

  ON_ERROR("Invalid symmetry_coordinates_as_unsigned parameter");
  return ON_Symmetry::Coordinates::Unset;
}

const ON_wString ON_Symmetry::SymmetryCoordinatesToString(ON_Symmetry::Coordinates symmetry_coordinates)
{
  const wchar_t* s;
  switch (symmetry_coordinates)
  {
  case ON_Symmetry::Coordinates::Unset:
    s = L"Unset";
    break;
  case ON_Symmetry::Coordinates::Object:
    s = L"Object";
    break;
  case ON_Symmetry::Coordinates::World:
    s = L"World";
    break;
  default:
    s = nullptr;
    break;
  }
  return ON_wString(s);
}

bool ON_Symmetry::Write(ON_BinaryArchive& archive) const
{
  if (false == archive.BeginWrite3dmAnonymousChunk(3))
    return false;

  bool rc = false;
  for (;;)
  {
    const ON_Symmetry::Type symmetry_type = IsSet() ? SymmetryType() : ON_Symmetry::Type::Unset;
    const unsigned char utype = static_cast<unsigned char>(symmetry_type);
    if (false == archive.WriteChar(utype))
      break;
    if (ON_Symmetry::Type::Unset ==  symmetry_type)
    {
      rc = true;
      break;
    }
    if (false == archive.WriteInt(m_inversion_order))
      break;
    if (false == archive.WriteInt(m_cyclic_order))
      break;
    if (false == archive.WriteUuid(m_id))
      break;

    if (archive.BeginWrite3dmAnonymousChunk(1))
    {
      switch (m_type)
      {
      case ON_Symmetry::Type::Unset:
        break;
      case ON_Symmetry::Type::Reflect:
        rc = archive.WritePlaneEquation(m_reflection_plane);
        break;
      case ON_Symmetry::Type::Rotate:
        rc = archive.WriteLine(m_rotation_axis);
        break;
      case ON_Symmetry::Type::ReflectAndRotate:
        rc = archive.WritePlaneEquation(m_reflection_plane) && archive.WriteLine(m_rotation_axis);
        break;
      case ON_Symmetry::Type::Inversion:
        rc = archive.WriteXform(m_inversion_transform);
        break;
      case ON_Symmetry::Type::Cyclic:
        rc = archive.WriteXform(m_cyclic_transform);
        break;
      default:
        ON_ERROR("You added a new enum value but failed to update archive IO code.");
        break;
      }
      if (false == archive.EndWrite3dmChunk())
        rc = false;
    }

    // ON_Symmetry::Coordinates added Dec 16, 2019 chunk version 2
    const ON_Symmetry::Coordinates symmetry_coordinates = IsSet() ? SymmetryCoordinates() : ON_Symmetry::Coordinates::Unset;
    const unsigned char ucoordinates = static_cast<unsigned char>(symmetry_coordinates);
    if (false == archive.WriteChar(ucoordinates))
      break;

    // ON_Symmetry::Coordinates added Feb 11, 2020 chunk version 3
    if ( false == archive.WriteBigInt(SymmetricObjectContentSerialNumber()) )
      break;

    rc = true;
    break;
  }
  if (false == archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}


bool ON_Symmetry::Read(ON_BinaryArchive& archive)
{
  *this = ON_Symmetry::Unset;

  int chunk_version = 0;
  if (false == archive.BeginRead3dmAnonymousChunk(&chunk_version))
    return false;

  ON_Symmetry::Type symmetry_type = ON_Symmetry::Type::Unset;
  unsigned int inversion_order = 0;
  unsigned int cyclic_order = 0;
  ON_UUID symmetry_id = ON_nil_uuid;
  ON_Xform inversion_transform = ON_Xform::Nan;
  ON_Xform cyclic_transform = ON_Xform::Nan;
  ON_PlaneEquation reflection_plane = ON_PlaneEquation::NanPlaneEquation;
  ON_Line rotation_axis = ON_Line::NanLine;

  bool rc = false;
  for (;;)
  {
    if (chunk_version <= 0)
      break;
    unsigned char utype = 0;
    if (false == archive.ReadChar(&utype))
      break;
    symmetry_type = ON_Symmetry::SymmetryTypeFromUnsigned(utype);
    if (ON_Symmetry::Type::Unset == symmetry_type)
    {
      rc = true;
      break;
    }


    if (false == archive.ReadInt(&inversion_order))
      break;
    if (false == archive.ReadInt(&cyclic_order))
      break;
    if (false == archive.ReadUuid(symmetry_id))
      break;

    int inner_chunk_version = 0;
    if (false == archive.BeginRead3dmAnonymousChunk(&inner_chunk_version))
      break;

    ON_Symmetry symmetry;

    ON_Symmetry::Coordinates symmetry_coordinates = ON_Symmetry::Coordinates::Object;

    switch (symmetry_type)
    {
    case ON_Symmetry::Type::Unset:
      break;

    case ON_Symmetry::Type::Reflect:
      rc = archive.ReadPlaneEquation(reflection_plane);
      if (rc)
        symmetry = ON_Symmetry::CreateReflectSymmetry(reflection_plane, symmetry_coordinates);
      break;

    case ON_Symmetry::Type::Rotate:
      rc = archive.ReadLine(rotation_axis);
      if (rc)
        symmetry = ON_Symmetry::CreateRotateSymmetry(rotation_axis, cyclic_order, symmetry_coordinates);
      break;

    case ON_Symmetry::Type::ReflectAndRotate:
      rc = archive.ReadPlaneEquation(reflection_plane) && archive.ReadLine(rotation_axis);
      if (rc)
        symmetry = ON_Symmetry::CreateReflectAndRotateSymmetry(reflection_plane, rotation_axis, cyclic_order, symmetry_coordinates);
      break;

    case ON_Symmetry::Type::Inversion:
      rc = archive.ReadXform(inversion_transform);
      if (rc)
        symmetry = ON_Symmetry::CreateInversionSymmetry(symmetry_id, inversion_transform, symmetry_coordinates);
      break;

    case ON_Symmetry::Type::Cyclic:
      rc = archive.ReadXform(cyclic_transform);
      if (rc)
        symmetry = ON_Symmetry::CreateCyclicSymmetry(symmetry_id, cyclic_transform, cyclic_order, symmetry_coordinates);
      break;

    default:
      // Old code reading a file containing a future type.
      symmetry_type = ON_Symmetry::Type::Unset;
      rc = true; // means no media reading error
      break;
    }

    if (
      rc
      && ON_Symmetry::Type::Unset != symmetry_type
      && symmetry.SymmetryType() == symmetry_type
      && symmetry.InversionOrder() == inversion_order
      && symmetry.CyclicOrder() == cyclic_order
      && symmetry.SymmetryId() == symmetry_id
      )
    {
      *this = symmetry;
    }

    if (false == archive.EndRead3dmChunk())
      rc = false;

    if (chunk_version < 2)
      break;

    unsigned char ucoordinates = 0;
    rc = archive.ReadChar(&ucoordinates);
    if (false == rc)
      break;
    symmetry_coordinates = ON_Symmetry::SymmetryCoordinatesFromUnsigned(ucoordinates);
    if (ON_Symmetry::Coordinates::Unset != symmetry_coordinates && m_coordinates != symmetry_coordinates)
      m_coordinates = symmetry_coordinates;
    
    if (chunk_version < 3)
      break;

    ON__UINT64 symmetric_object_content_serial_number = 0;
    rc = archive.ReadBigInt(&symmetric_object_content_serial_number);
    if (rc)
      SetSymmetricObjectContentSerialNumber(symmetric_object_content_serial_number);

    break;
  }

  if (false == archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

void ON_PlaneEquation::Dump(class ON_TextLog& text_log) const
{
  // print -0 as 0.
  double c[4] = { (0.0==x) ? 0.0 : x,(0.0 == y) ? 0.0 : y,(0.0 == z) ? 0.0 : z,(0.0 == d) ? 0.0 : d };
  for (int i = 0; i < 3; ++i)
  {
    if (false == (0.0 != c[i] && 0.0 == c[(i + 1) % 3] && 0.0 == c[(i + 2) % 3]) )
      continue;
    const char* coord = (0 == i) ? "x" : ((1 == i) ? "y" : "z");
    if (0.0 == c[3])
      text_log.Print(L"%s = 0", coord);
    else if (1.0 == c[i])
      text_log.Print(L"%s = %g", coord, -c[3]);
    else
      text_log.Print(L"%g*%s = %g", c[i] , coord, -c[3]);
    return;
  }

  // general case
  text_log.Print(L"%g*x + %g*y + %g*z + %g = 0", c[0], c[1], c[2], c[3]);
}

void ON_Symmetry::Dump(ON_TextLog& text_log) const
{
  const ON_wString type = ON_Symmetry::SymmetryTypeToString(m_type);
  const ON_wString coordinates = ON_Symmetry::SymmetryCoordinatesToString(m_coordinates);
  text_log.Print(L"%ls %ls symmetry\n",static_cast<const wchar_t*>(type), static_cast<const wchar_t*>(coordinates));
  if (IsUnset())
    return;

  text_log.Print(L"Motif count: %u\n", MotifCount());

  switch (m_type)
  {
  case ON_Symmetry::Type::Unset:
    break;

  case ON_Symmetry::Type::Reflect:
  {
    const ON_PlaneEquation e = ReflectionPlane();
    text_log.Print(L"plane: ");
    ReflectionPlane().Dump(text_log);
    text_log.PrintNewLine();
  }
  break;

  case ON_Symmetry::Type::Rotate:
  {
    text_log.Print(L"rotation count: %u (%g degrees)\n", RotationCount(), RotationAngleDegrees());
    const ON_Line axis = RotationAxis();
    text_log.Print(L"axis: ");
    text_log.Print(axis.from);
    text_log.Print(L", ");
    text_log.Print(axis.to);
    text_log.PrintNewLine();
  }
  break;

  case ON_Symmetry::Type::ReflectAndRotate:
  {
    const ON_PlaneEquation e = ReflectionPlane();
    text_log.Print(L"plane: ");
    ReflectionPlane().Dump(text_log);
    text_log.PrintNewLine();
    text_log.Print(L"rotation count: %u (%g degrees)\n", RotationCount(), RotationAngleDegrees());
    const ON_Line axis = RotationAxis();
    text_log.Print(L"axis: ");
    text_log.Print(axis.from);
    text_log.Print(L", ");
    text_log.Print(axis.to);
    text_log.PrintNewLine();
  }
  break;

  case ON_Symmetry::Type::Inversion:
  {
    const ON_Line line = RotationAxis();
    text_log.Print(InversionTransform());
    text_log.PrintNewLine();
  }
  break;

  case ON_Symmetry::Type::Cyclic:
  {
    const ON_Line line = RotationAxis();
    text_log.Print(CyclicTransform());
    text_log.PrintNewLine();
  }
  break;

  default:
    break;
  }
}

const ON_Symmetry ON_Symmetry::TransformConditionally(const ON_Xform& xform) const
{
  return
    (ON_Symmetry::Coordinates::Object == SymmetryCoordinates())
    ? ON_Symmetry::TransformUnconditionally(xform)
    : ON_Symmetry(*this);
}

const ON_Symmetry ON_Symmetry::TransformUnconditionally(const ON_Xform& xform) const
{
  switch (m_type)
  {
  case ON_Symmetry::Type::Unset:
    break;

  case ON_Symmetry::Type::Reflect:
  {
    if (false == m_reflection_plane.IsValid())
      break;
    ON_PlaneEquation e = m_reflection_plane;
    e.Transform(xform);
    if (false == e.IsValid())
      break;
    return ON_Symmetry::CreateReflectSymmetry(e, m_coordinates);
  }
  break;

  case ON_Symmetry::Type::Rotate:
  {
    if (false == m_rotation_axis.IsValid())
      break;
    ON_Line a = m_rotation_axis;
    a.Transform(xform);
    if (false == a.IsValid())
      break;
    return ON_Symmetry::CreateRotateSymmetry(a, RotationCount(), m_coordinates);
  }
  break;

  case ON_Symmetry::Type::ReflectAndRotate:
  {
    if (false == m_reflection_plane.IsValid())
      break;
    if (false == m_rotation_axis.IsValid())
      break;
    ON_PlaneEquation e = m_reflection_plane;
    e.Transform(xform);
    if (false == e.IsValid())
      break;
    ON_Line a = m_rotation_axis;
    a.Transform(xform);
    if (false == a.IsValid())
      break;
    return ON_Symmetry::CreateReflectAndRotateSymmetry(e, a, RotationCount(), m_coordinates);
  }
  break;

  case ON_Symmetry::Type::Inversion:
  {
    const ON_Xform xform_inverse = xform.Inverse();
    const ON_Xform inversion_xform = xform * InversionTransform()*xform_inverse;
    return ON_Symmetry::CreateInversionSymmetry(SymmetryId(), inversion_xform, m_coordinates);
  }
  break;

  case ON_Symmetry::Type::Cyclic:
  {
    const ON_Xform xform_inverse = xform.Inverse();
    const ON_Xform cyclic_xform = xform * CyclicTransform()*xform_inverse;
    return ON_Symmetry::CreateCyclicSymmetry(SymmetryId(), cyclic_xform, CyclicOrder(), m_coordinates);
  }
  break;

  default:
    break;
  }
  return ON_Symmetry::Unset;
}

static bool Internal_SamePlane(const ON_Symmetry* lhs, const ON_Symmetry* rhs, double zero_tolerance)
{
  const ON_PlaneEquation lhs_e = lhs->ReflectionPlane().UnitizedPlaneEquation();
  const ON_PlaneEquation rhs_e = rhs->ReflectionPlane().UnitizedPlaneEquation();
  return
    fabs(lhs_e.x - rhs_e.x) <= zero_tolerance
    && fabs(lhs_e.y - rhs_e.y) <= zero_tolerance
    && fabs(lhs_e.z - rhs_e.z) <= zero_tolerance
    && fabs(lhs_e.d - rhs_e.d) <= zero_tolerance
    ;
}

static bool Internal_SameRotation(const ON_Symmetry* lhs, const ON_Symmetry* rhs, double zero_tolerance)
{
  const ON_Line lhs_l = lhs->RotationAxis();
  const ON_Line rhs_l = rhs->RotationAxis();
  if (
    lhs_l.DistanceTo(rhs_l.from) <= zero_tolerance
    && lhs_l.DistanceTo(rhs_l.to) <= zero_tolerance
    && rhs_l.DistanceTo(lhs_l.from) <= zero_tolerance
    && rhs_l.DistanceTo(lhs_l.to) <= zero_tolerance
    )
  {
    const ON_3dVector lhs_t = lhs->RotationAxis().Tangent();
    const ON_3dVector rhs_t = lhs->RotationAxis().Tangent();
    const double lhs_a = lhs->RotationAngleRadians();
    const double rhs_a = ((lhs_t * rhs_t < 0.0) ? -1.0 : 1.0) * rhs->RotationAngleRadians();
    if (fabs(lhs_a - rhs_a) <= zero_tolerance)
    {
      // a point 1 unit from the common axis will rotate within zero tolrance
      return true;
    }
  }
  return false;
}

static bool Internal_SameTransformation(const ON_Xform lhs_x, const ON_Xform rhs_x, double zero_tolerance)
{
  return (lhs_x * rhs_x.Inverse()).IsIdentity(zero_tolerance) && (rhs_x * lhs_x.Inverse()).IsIdentity(zero_tolerance);
}

static bool Internal_SameTransformation(const ON_Symmetry* lhs, const ON_Symmetry* rhs, double zero_tolerance)
{
  ON_Xform lhs_x;
  ON_Xform rhs_x;
  if (lhs->InversionOrder() != rhs->InversionOrder())
    return false;
  if (lhs->CyclicOrder() != rhs->CyclicOrder())
    return false;
  if (lhs->InversionOrder() > 1 && false == Internal_SameTransformation(lhs->InversionTransform(), rhs->InversionTransform(), zero_tolerance))
    return false;
  if (lhs->CyclicOrder() > 1 && false == Internal_SameTransformation(lhs->CyclicTransform(), rhs->CyclicTransform(), zero_tolerance))
    return false;
  return true;
}


int ON_Symmetry::CompareSymmetryTransformation(const ON_Symmetry* lhs, const ON_Symmetry* rhs, double zero_tolerance)
{
  for (;;)
  {
    const ON_Symmetry::Type lhs_type = (nullptr != lhs) ? lhs->SymmetryType() : ON_Symmetry::Type::Unset;
    const ON_Symmetry::Type rhs_type = (nullptr != rhs) ? rhs->SymmetryType() : ON_Symmetry::Type::Unset;
    if (lhs_type != rhs_type)
      break;

    if (ON_Symmetry::Type::Unset == lhs_type)
      return 0; // both are unset

    if (false == (zero_tolerance >= 0.0 && zero_tolerance < ON_UNSET_POSITIVE_FLOAT))
      zero_tolerance = ON_Symmetry::ZeroTolerance;

    switch (lhs_type)
    {
    case ON_Symmetry::Type::Unset:
      break;

    case ON_Symmetry::Type::Reflect:
      if (Internal_SamePlane(lhs, rhs, zero_tolerance))
        return 0;
      break;

    case ON_Symmetry::Type::Rotate:
      if (Internal_SameRotation(lhs, rhs, zero_tolerance))
        return 0;
      break;

    case ON_Symmetry::Type::ReflectAndRotate:
      if (Internal_SamePlane(lhs, rhs, zero_tolerance) && Internal_SameRotation(lhs, rhs, zero_tolerance))
        return 0;
      break;

    case ON_Symmetry::Type::Inversion:
    case ON_Symmetry::Type::Cyclic:
      if (Internal_SameTransformation(lhs, rhs, zero_tolerance))
        return 0;

    default:
      break;
    }
  }

  return ON_Symmetry::Compare(lhs, rhs);
}

const ON_Symmetry ON_Symmetry::CreateInversionSymmetry(
  ON_UUID symmetry_id, 
  ON_Xform inversion_transform, 
  ON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    if (false == inversion_transform.IsValid())
      break;

    const double det = inversion_transform.Determinant();
    if (false == (det < 0.0))
      break;

    ON_Xform x = inversion_transform* inversion_transform;
    if (false == x.IsIdentity(ON_Symmetry::ZeroTolerance))
      break;

    if (false == (ON_nil_uuid == symmetry_id) )
    {
      // prohibit using built-in ids
      if (ON_Symmetry::ReflectId == symmetry_id)
        break;
      if (ON_Symmetry::RotateId == symmetry_id)
        break;
      if (ON_Symmetry::ReflectAndRotateId == symmetry_id)
        break;
    }

    ON_Symmetry symmetry;
    symmetry.m_type = ON_Symmetry::Type::Cyclic;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_inversion_order = 2;
    symmetry.m_cyclic_order = 1;
    symmetry.m_id = symmetry_id;
    symmetry.m_inversion_transform = inversion_transform;
    symmetry.m_cyclic_transform = ON_Xform::IdentityTransformation;
    return symmetry;
  }

  return ON_Symmetry::Unset;
}


const ON_Symmetry ON_Symmetry::CreateCyclicSymmetry(
  ON_UUID symmetry_id, 
  ON_Xform cyclic_transform,
  unsigned int cyclic_order,
  ON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    if (cyclic_order < 2)
      break;
    if (cyclic_order > ON_Symmetry::MaximumOrder)
      break;
    if (false == cyclic_transform.IsValid())
      break;

    const double det = cyclic_transform.Determinant();
    if (2 == cyclic_order || 1 == (cyclic_order % 2))
    {
      if (false == (det > 0.0))
        break;
    }
    else
    {
      if (false == (det != 0.0))
        break;
    }

    unsigned n = 1;
    ON_Xform x = cyclic_transform;
    while (n < cyclic_order && x.IsValid() && false == x.IsIdentity(ON_Symmetry::ZeroTolerance))
    {
      x = cyclic_transform * x;
      ++n;
    }
    if (n != cyclic_order)
      break;
    if (false == x.IsIdentity(ON_Symmetry::ZeroTolerance))
      break;

    if (false == (ON_nil_uuid == symmetry_id))
    {
      // prohibit using built-in ids
      if (ON_Symmetry::ReflectId == symmetry_id)
        break;
      if (ON_Symmetry::RotateId == symmetry_id)
        break;
      if (ON_Symmetry::ReflectAndRotateId == symmetry_id)
        break;
    }

    ON_Symmetry symmetry;
    symmetry.m_type = ON_Symmetry::Type::Cyclic;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_inversion_order = 1;
    symmetry.m_cyclic_order = cyclic_order;
    symmetry.m_id = symmetry_id;
    symmetry.m_inversion_transform = ON_Xform::IdentityTransformation;
    symmetry.m_cyclic_transform = cyclic_transform;
    return symmetry;
  }

  return ON_Symmetry::Unset;
}

const ON_Symmetry ON_Symmetry::CreateReflectSymmetry(
  ON_PlaneEquation reflection_plane, 
  ON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    if (false == reflection_plane.IsValid())
      break;
    const ON_Xform xform(ON_Xform::MirrorTransformation(reflection_plane));
    ON_Symmetry symmetry = ON_Symmetry::CreateInversionSymmetry(ON_nil_uuid, xform, symmetry_coordinates);
    if (ON_Symmetry::Type::Cyclic != symmetry.m_type)
      break;
    symmetry.m_type = ON_Symmetry::Type::Reflect;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_id = ON_Symmetry::ReflectId;
    symmetry.m_reflection_plane = reflection_plane;
    return symmetry;
  }
  return ON_Symmetry::Unset;
}

const ON_Symmetry ON_Symmetry::CreateRotateSymmetry(
  ON_Line rotation_axis,
  unsigned int rotation_count, 
  ON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    if (rotation_count < 2 || rotation_count > ON_Symmetry::MaximumOrder)
      break;
    if (false == rotation_axis.IsValid())
      break;
    const ON_Xform R = Internal_RotationXform(rotation_axis, 1, rotation_count);
    ON_Symmetry symmetry = ON_Symmetry::CreateCyclicSymmetry(ON_nil_uuid, R, rotation_count, symmetry_coordinates);
    if (ON_Symmetry::Type::Cyclic != symmetry.m_type)
      break;
    symmetry.m_type = ON_Symmetry::Type::Rotate;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_id = ON_Symmetry::RotateId;
    symmetry.m_rotation_axis = rotation_axis;
    return symmetry;
  }
  return ON_Symmetry::Unset;
}


const ON_Symmetry ON_Symmetry::CreateReflectAndRotateSymmetry(
  ON_PlaneEquation reflection_plane,
  ON_Line rotation_axis, 
  unsigned int rotation_count,
  ON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    if (false == reflection_plane.IsValid())
      break;
    if (false == rotation_axis.IsValid())
      break;

    // rotation axis must be in the reflection plane
    const double h0 = reflection_plane.ValueAt(rotation_axis.from);
    const double h1 = reflection_plane.ValueAt(rotation_axis.to);
    if (false == (fabs(h0) <= ON_ZERO_TOLERANCE))
      break;
    if (false == (fabs(h1) <= ON_ZERO_TOLERANCE))
      break;

    const ON_Symmetry reflection = CreateReflectSymmetry(reflection_plane, symmetry_coordinates);
    if (ON_Symmetry::Type::Reflect != reflection.SymmetryType())
      break;
    const ON_Symmetry rotation = CreateRotateSymmetry(rotation_axis,rotation_count, symmetry_coordinates);
    if (ON_Symmetry::Type::Rotate != rotation.SymmetryType())
      break;

    ON_Symmetry symmetry;
    symmetry.m_type = ON_Symmetry::Type::ReflectAndRotate;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_inversion_order = reflection.m_inversion_order;
    symmetry.m_cyclic_order = rotation.m_cyclic_order;
    symmetry.m_id = ON_Symmetry::ReflectAndRotateId;
    symmetry.m_inversion_transform = reflection.m_inversion_transform;
    symmetry.m_cyclic_transform = rotation.m_cyclic_transform;
    symmetry.m_reflection_plane = reflection.m_reflection_plane;
    symmetry.m_rotation_axis = rotation.m_rotation_axis;
    return symmetry;
  }
  return ON_Symmetry::Unset;
}

int ON_Symmetry::Internal_CompareDouble(const double* lhs, const double* rhs, size_t count)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  for (size_t i = 0; i < count; ++i)
  {
    const double x = lhs[i];
    const double y = rhs[i];
    if (x < y)
      return -1;
    if (x > y)
      return 1;
    const bool xok = (x == x) ? true : false;
    const bool yok = (y == y) ? true : false;
    if (xok == yok)
      continue;
    if (false == xok)
      return 1; // lhs is a nan
    if (false == yok)
      return -1; // rhs is a nan
  }
  return 0;
}

int ON_Symmetry::Compare(const ON_Symmetry* lhs, const ON_Symmetry* rhs)
{
  if (lhs == rhs)
    return 0;

  // sort nulls to end
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;

  if (static_cast<unsigned char>(lhs->m_type) < static_cast<unsigned char>(rhs->m_type))
    return -1;
  if (static_cast<unsigned char>(lhs->m_type) > static_cast<unsigned char>(rhs->m_type))
    return 1;
  if (ON_Symmetry::Type::Unset == lhs->m_type)
    return 0;

  if (static_cast<unsigned char>(lhs->m_coordinates) < static_cast<unsigned char>(rhs->m_coordinates))
    return -1;
  if (static_cast<unsigned char>(lhs->m_coordinates) > static_cast<unsigned char>(rhs->m_coordinates))
    return 1;


  if (lhs->m_inversion_order < rhs->m_inversion_order)
    return -1;
  if (lhs->m_inversion_order > rhs->m_inversion_order)
    return 1;

  if (lhs->m_cyclic_order < rhs->m_cyclic_order)
    return -1;
  if (lhs->m_cyclic_order > rhs->m_cyclic_order)
    return 1;

  if (0U == lhs->m_inversion_order || 0U == lhs->m_cyclic_order)
    return 0;
   
  if (ON_Symmetry::Type::Reflect == lhs->m_type || ON_Symmetry::Type::ReflectAndRotate == lhs->m_type )
  {
    const int rc = ON_Symmetry::Internal_CompareDouble(&lhs->m_reflection_plane.x, &rhs->m_reflection_plane.x, 4);
    if (0 != rc)
      return rc;
  }

  if (ON_Symmetry::Type::Rotate == lhs->m_type || ON_Symmetry::Type::ReflectAndRotate == lhs->m_type)
  {
    const int rc = ON_Symmetry::Internal_CompareDouble(&lhs->m_rotation_axis.from.x, &rhs->m_rotation_axis.from.x, 6);
    if (0 != rc)
      return rc;
  }

  if (
    ON_Symmetry::Type::Reflect == lhs->m_type
    || ON_Symmetry::Type::Rotate == lhs->m_type
    || ON_Symmetry::Type::ReflectAndRotate == lhs->m_type
    )
    return 0;

  if (lhs->m_inversion_order > 1)
  {
    const int rc = ON_Symmetry::Internal_CompareDouble(&lhs->m_inversion_transform.m_xform[0][0], &rhs->m_inversion_transform.m_xform[0][0], 16);
    if (0 != rc)
      return rc;
  }

  if (lhs->m_cyclic_order > 1)
  {
    const int rc = ON_Symmetry::Internal_CompareDouble(&lhs->m_inversion_transform.m_xform[0][0], &rhs->m_inversion_transform.m_xform[0][0], 16);
    if (0 != rc)
      return rc;
  }

  return 0;
}

ON_Symmetry::Type ON_Symmetry::SymmetryType() const
{
  return m_type;
}

ON_Symmetry::Coordinates ON_Symmetry::SymmetryCoordinates() const
{
  return m_coordinates;
}

const ON_UUID ON_Symmetry::SymmetryId() const
{
  return m_id;
}

void ON_Symmetry::Clear()
{
  *this = ON_Symmetry::Unset;
}

bool ON_Symmetry::IsSet() const
{
  return 
    ON_Symmetry::Type::Unset != m_type
    && (1 == m_inversion_order || 2 == m_inversion_order)
    && m_cyclic_order >= 1
    && MotifCount() >= 2
    ;
}

bool ON_Symmetry::IsUnset() const
{
  return (false == IsSet());
}

unsigned int ON_Symmetry::MotifCount() const
{
  return InversionOrder()*CyclicOrder();
}


unsigned int ON_Symmetry::InversionOrder() const
{
  return m_inversion_order;
}


unsigned int ON_Symmetry::CyclicOrder() const
{
  return m_cyclic_order;
}

const ON_Xform ON_Symmetry::InversionTransform() const
{
  return IsSet() ? m_inversion_transform : ON_Xform::Nan;
}

const ON_Xform ON_Symmetry::CyclicTransform() const
{
  return IsSet() ? m_cyclic_transform : ON_Xform::Nan;
}

const ON_SHA1_Hash ON_Symmetry::Hash() const
{
  for(;;)
  {
    if (false == IsSet())
      break;
    
    ON_SHA1 sha1;

    const unsigned char t = static_cast<unsigned char>(m_type);
    sha1.AccumulateBytes(&t, sizeof(t));

    const unsigned char c = static_cast<unsigned char>(m_coordinates);
    sha1.AccumulateBytes(&c, sizeof(c));

    sha1.AccumulateInteger32(InversionOrder());
    sha1.AccumulateInteger32(CyclicOrder());

    if (ON_Symmetry::Type::Reflect == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
      sha1.AccumulateDoubleArray(4, &m_reflection_plane.x);

    if (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
      sha1.AccumulateDoubleArray(6, &m_rotation_axis.from.x);

    if (ON_Symmetry::Type::Reflect != m_type && ON_Symmetry::Type::Rotate != m_type && ON_Symmetry::Type::ReflectAndRotate != m_type)
    {
      if (InversionOrder() > 1)
        sha1.AccumulateDoubleArray(16, &m_inversion_transform.m_xform[0][0]);
      if (CyclicOrder() > 1)
        sha1.AccumulateDoubleArray(16, &m_cyclic_transform.m_xform[0][0]);
    }
    return sha1.Hash();
  }

  return ON_SHA1_Hash::EmptyContentHash;
}

const ON_PlaneEquation ON_Symmetry::ReflectionPlane() const
{
  return (ON_Symmetry::Type::Reflect == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? m_reflection_plane
    : ON_PlaneEquation::NanPlaneEquation;
}

const ON_Line ON_Symmetry::RotationAxis() const
{
  return (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? m_rotation_axis
    : ON_Line::NanLine;
}

const ON_3dPoint ON_Symmetry::RotationAxisPoint() const
{
  return (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? m_rotation_axis.from
    : ON_3dPoint::NanPoint;
}

const ON_3dVector ON_Symmetry::RotationAxisDirection() const
{
  return (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? m_rotation_axis.Direction()
    : ON_3dVector::NanVector;
}

const ON_3dVector ON_Symmetry::RotationAxisTangent() const
{
  return (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? m_rotation_axis.Tangent()
    : ON_3dVector::NanVector;
}

unsigned int ON_Symmetry::RotationCount() const
{
  return (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? m_cyclic_order
    : 0U;
}

double ON_Symmetry::RotationAngleDegrees() const
{
  return (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? (360.0 / ((double)RotationCount()))
    : ON_DBL_QNAN;
}

double ON_Symmetry::RotationAngleRadians() const
{
  return (ON_Symmetry::Type::Rotate == m_type || ON_Symmetry::Type::ReflectAndRotate == m_type)
    ? ((2.0*ON_PI) / ((double)RotationCount()))
    : ON_DBL_QNAN;
}

const ON_Xform ON_Symmetry::Internal_RotationXform(
  int rotation_index, 
  int rotation_count
) const
{
  if (rotation_index < 0 || rotation_index >= rotation_count)
    return ON_Xform::Nan;
  if (0 == rotation_index)
    return ON_Xform::IdentityTransformation;
  if (1 == rotation_index)
    return m_cyclic_transform;

  return ON_Symmetry::Internal_RotationXform(m_rotation_axis, rotation_index, rotation_count);
}

const ON_Xform ON_Symmetry::Internal_RotationXform(
  ON_Line rotation_axis,
  int rotation_index,
  int rotation_count
)
{
  if (rotation_index < 0 || rotation_index >= rotation_count)
    return ON_Xform::Nan;
  if (0 == rotation_index)
    return ON_Xform::IdentityTransformation;

  // calculate from trig functions for maximum precision
  double sin_sign = 1.0;
  if (2 * rotation_index > rotation_count)
  {
    rotation_index = rotation_count - rotation_index;
    sin_sign = -1.0;
  }

  double cos_angle = ON_DBL_QNAN;
  double sin_angle = ON_DBL_QNAN;

  if (2 * rotation_index == rotation_count)
  {
    // angle = pi
    sin_angle = 0.0;
    cos_angle = -1.0;
  }
  else if (4 * rotation_index == rotation_count)
  {
    // angle = pi/2
    sin_angle = 1.0;
    cos_angle = 0.0;
  }
  else if (6 * rotation_index == rotation_count)
  {
    // angle = pi/3
    sin_angle = 0.5*sqrt(3.0);
    cos_angle = 0.5;
  }
  else if (8 * rotation_index == rotation_count)
  {
    // angle = pi/4
    sin_angle = cos_angle = 1.0 / sqrt(2.0);
  }
  else if (12 * rotation_index == rotation_count)
  {
    // angle = pi/3
    sin_angle = 0.5;
    cos_angle = 0.5*sqrt(3.0);
  }
  else
  {
    const double a = (rotation_index*(2.0*ON_PI)) / ((double)rotation_count);
    sin_angle = sin(a);
    cos_angle = cos(a);
  }

  ON_Xform r;
  r.Rotation(sin_sign*sin_angle, cos_angle, rotation_axis.Direction(), rotation_axis.from);
  return r;
}

const ON_Xform ON_Symmetry::MotifTransformation(
  int index
) const
{
  const int count = MotifCount();
  if ( count <= 1)
    return ON_Xform::Nan;

  // convert index to be >= 0
  index = ((index % count) + count) % count;
  
  ON_Xform x = ON_Xform::Nan;
  switch (m_type)
  {
  case ON_Symmetry::Type::Unset:
    break;

  case ON_Symmetry::Type::Reflect:
    x = (0 == index)
      ? ON_Xform::IdentityTransformation
      : m_inversion_transform;
    break;

  case ON_Symmetry::Type::Rotate:
    x = Internal_RotationXform(index, count);
    break;

  case ON_Symmetry::Type::ReflectAndRotate:
    if (0 == index)
      x = ON_Xform::IdentityTransformation;
    else if (1 == index)
      x = m_inversion_transform;
    else if (2 == index)
      x = m_cyclic_transform;
    else if ( index > 2 )
      x = Internal_ReflectAndRotateTransformation((unsigned)index);
    break;

  case ON_Symmetry::Type::Inversion:
    x = (0 == index)
      ? ON_Xform::IdentityTransformation
      : m_inversion_transform;
    break;

  case ON_Symmetry::Type::Cyclic:
    if (0 == index)
    {
      x = ON_Xform::IdentityTransformation;
    }
    else if (1 == index)
    {
      x = m_cyclic_transform;
    }
    else if (index >= 2)
    {
      x = m_cyclic_transform * m_cyclic_transform;
      for (int i = 2; i < index; i++)
        x = m_cyclic_transform * x;
    }
    break;

  default:
    break;
  }
  return x;
}

const ON_Xform ON_Symmetry::Internal_ReflectAndRotateTransformation(unsigned index) const
{
  ON_Xform r = Internal_RotationXform(index / 2, m_cyclic_order);
  if (1 == index % 2)
    r = r * m_inversion_transform;
  return r;
}

ON_SHA1_Hash ON_Symmetry::Sha1Hash() const
{
  ON_SHA1 sha1;
  sha1.AccumulateBytes(&m_type, sizeof(m_type));
  sha1.AccumulateBytes(&m_coordinates, sizeof(m_coordinates));
  sha1.AccumulateInteger8(m_inversion_order);
  sha1.AccumulateInteger32(m_cyclic_order);
  sha1.AccumulateId(m_id);
  sha1.AccumulateDoubleArray(16, &m_inversion_transform.m_xform[0][0]);
  sha1.AccumulateDoubleArray(16, &m_cyclic_transform.m_xform[0][0]);
  sha1.AccumulateDoubleArray(4,&m_reflection_plane.x);
  sha1.Accumulate3dPoint(m_rotation_axis.from);
  sha1.Accumulate3dPoint(m_rotation_axis.to);
  return sha1.Hash();
}

void ON_Symmetry::SetSymmetricObjectContentSerialNumber(ON__UINT64 symmetric_object_content_serial_number) const
{
  if (0 == symmetric_object_content_serial_number)
    ClearSymmetricObjectContentSerialNumber(); // so a debugger breakpoint can be set in one place to watching clearing
  else
    m_symmetric_object_content_serial_number = symmetric_object_content_serial_number;
}

void ON_Symmetry::ClearSymmetricObjectContentSerialNumber() const
{
  m_symmetric_object_content_serial_number = 0U;
}

ON__UINT64 ON_Symmetry::SymmetricObjectContentSerialNumber() const
{
  return m_symmetric_object_content_serial_number;
}

