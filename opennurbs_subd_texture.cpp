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



ON_SubDTextureDomainType ON_SubD::TextureDomainTypeFromUnsigned
(
  unsigned int texture_domain_type_as_unsigned
)
{
  switch (texture_domain_type_as_unsigned)
  {
    ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDTextureDomainType::Unset);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDTextureDomainType::PerFace);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDTextureDomainType::Packed);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDTextureDomainType::Zero);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDTextureDomainType::Nan);
    ON_ENUM_FROM_UNSIGNED_CASE(ON_SubDTextureDomainType::Custom);
  }
  return ON_SUBD_RETURN_ERROR(ON_SubDTextureDomainType::Unset);
}

const ON_wString ON_SubD::TextureDomainTypeToString(
  ON_SubDTextureDomainType texture_domain_type
)
{
  const wchar_t* s;
  switch (texture_domain_type)
  {
  case ON_SubDTextureDomainType::Unset:
    s = L"Unset";
    break;
  case ON_SubDTextureDomainType::PerFace:
    s = L"PerFace";
    break;
  case ON_SubDTextureDomainType::Packed:
    s = L"Packed";
    break;
  case ON_SubDTextureDomainType::Zero:
    s = L"Zero";
    break;
  case ON_SubDTextureDomainType::Nan:
    s = L"Nan";
    break;
  case ON_SubDTextureDomainType::Custom:
    s = L"Custom";
    break;
  default:
    s = nullptr;
    break;
  }
  return (nullptr != s && 0 != s[0]) 
    ? ON_wString(s)
    : ON_wString::FormatToString(L"ON_SubDTextureDomainType(%u)", ((unsigned)static_cast<unsigned char>(texture_domain_type)));
}

//////////////////////////////////////////////////////////////////////////////
//
// ON_SubDimple - texture coordinates
// 
#if 1
#pragma region ON_SubD - texture coordinates

void ON_SubDimple::SetTextureMappingTag(const ON_MappingTag &mapping_tag) const
{
  this->m_texture_mapping_tag = mapping_tag;
}

const ON_MappingTag ON_SubDimple::TextureMappingTag() const
{
  return m_texture_mapping_tag;
}

#pragma endregion
#endif

//////////////////////////////////////////////////////////////////////////////
//
// ON_SubD - texture coordinates
// 
#if 1
#pragma region ON_SubD - texture coordinates

ON_SubDTextureDomainType ON_SubD::TextureDomainType() const
{
  const ON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->TextureDomainType() : ON_SubDTextureDomainType::Unset;
}

ON_SubDTextureDomainType ON_SubDimple::TextureDomainType() const
{
  return m_texture_domain_type;
}

void ON_SubDimple::SetTextureDomainType(
  ON_SubDTextureDomainType texture_domain_type
) const
{
  m_texture_domain_type = texture_domain_type;
}

unsigned int ON_SubD::TextureImageSuggestedMinimumSize() const
{
  const ON_2udex grid_size = ON_SubD::TextureDomainGridSize(FaceCount(), 0.0, 0.0);
  return ON_SubD::TextureImageSuggestedMinimumSize(grid_size);
}

unsigned int ON_SubD::TextureImageSuggestedMinimumSize(
  ON_2udex grid_size
)
{
  const unsigned min_pixels = 16;
  const unsigned max_pixels = 4098;
  unsigned pixels = grid_size.i >= grid_size.j ? grid_size.i : grid_size.j;
  if (0 == pixels)
    return ON_SUBD_RETURN_ERROR(1);

  if (pixels < max_pixels / min_pixels)
    pixels *= min_pixels;
  else
    pixels = max_pixels;

  if (pixels < min_pixels)
  {
    pixels = min_pixels;
    const unsigned smallgrid_min_pixels = 128;
    if (grid_size.i > 0 && grid_size.j > 0)
    {
      while (grid_size.i * grid_size.j * pixels < smallgrid_min_pixels)
        pixels *= 2;
    }
  }
  
  if (pixels > max_pixels)
    pixels = max_pixels;

  return pixels;
}

const ON_2udex ON_SubD::TextureDomainGridSize(
  unsigned minimum_rectangle_count,
  double image_width,
  double image_height
)
{
  if (0 == minimum_rectangle_count)
    return ON_2udex(1, 1);

  unsigned i = (unsigned)floor(sqrt((double)(minimum_rectangle_count)));
  while (i < minimum_rectangle_count && i*i < minimum_rectangle_count)
    ++i;
  unsigned int j = i;
  if (j > 1 && (j - 1)*i >= minimum_rectangle_count)
    --j;
  return (image_height > image_width) ? ON_2udex(j, i) : ON_2udex(i, j);
}

const ON_2udex ON_SubD::GetTextureDomainAndDelta(
  unsigned quad_count,
  double image_width,
  double image_height,
  ON_2dVector& quad_texture_domain,
  ON_2dVector& quad_texture_delta
)
{
  ON_2udex grid_size(0, 0);
  quad_texture_domain = ON_2dVector::ZeroVector;
  quad_texture_delta = ON_2dVector::ZeroVector;

  const bool bImageSizeKnown
    = image_width > 0.0 && image_width < ON_UNSET_POSITIVE_VALUE
    && image_width > 0.0 && image_width < ON_UNSET_POSITIVE_VALUE;
  if (false == bImageSizeKnown)
  {
    // unknown image size - assume it's square.
    grid_size = ON_SubD::TextureDomainGridSize(quad_count, 0.0, 0.0);

    // use grid size to set minimum sugggested square image size
    image_width = image_height = (double)ON_SubD::TextureImageSuggestedMinimumSize(grid_size);
  }
  else
  {
    // use image size to help set grid size
    grid_size = ON_SubD::TextureDomainGridSize(quad_count, image_width, image_height);
  }

  if (0 == grid_size.i || 0 == grid_size.j)
    return ON_SUBD_RETURN_ERROR(grid_size);

  const double image_size[2] = { image_width ,image_height };

  // quad_image_size = size of image for each quad
  const ON_2dVector quad_image_size(image_size[0] / ((double)grid_size.i), image_size[1] / ((double)grid_size.j));

  if (1U == quad_count)
  {
    // quad uses entire image_delta x image_delta region
    quad_texture_delta.Set(1.0, 1.0);
    quad_texture_domain = quad_texture_delta;
  }
  else
  {
    for (int k = 0; k < 2; ++k)
    {
      if (quad_image_size[k] >= 16.0)
      {
        // An image with image_delta.x X image_delta.y pixels
        // doesn't have to share a pixel with its neighbors.
        quad_texture_delta[k] = floor(quad_image_size[k]) / image_size[k];
        quad_texture_domain[k] = (floor(quad_image_size[k]) - 1.0) / image_size[k];
      }
      else
      {
        // Not as good, but a quad is getting at most 16x16 pixels in from a texture image
        // with image_delta x image_delta pixels, so the texture will look bad anyway.
        quad_texture_delta[k] = (quad_image_size[k] / image_size[k]);
        quad_texture_domain[k] = (15.0 / 16.0)*quad_texture_delta[k];
      }
    }
  }

  return grid_size;
}

bool ON_SubD::SetTextureDomains(
  ON_SubDTextureDomainType texture_domain_type,
  bool bLazy,
  bool bSetFragmentTextureCoordinates
) const
{
  const ON_SubDimple* subdimple = SubDimple();
  if (nullptr == subdimple)
    return (ON_SubDTextureDomainType::Unset == texture_domain_type);

  if (ON_SubDTextureDomainType::Unset == texture_domain_type || ON_SubDTextureDomainType::Custom == texture_domain_type)
    bLazy = true;
  if (bLazy && texture_domain_type == subdimple->TextureDomainType())
    return true;

  if (0 == FaceCount())
  {
    subdimple->SetTextureDomainType(ON_SubDTextureDomainType::Unset);
    return (ON_SubDTextureDomainType::Unset == texture_domain_type);
  }

  bool rc = false;

  switch (texture_domain_type)
  {
  case ON_SubDTextureDomainType::Unset:
    rc = true;
    break;

  case ON_SubDTextureDomainType::PerFace:
  case ON_SubDTextureDomainType::Packed:
  case ON_SubDTextureDomainType::Zero:
  case ON_SubDTextureDomainType::Nan:
    {
      ON_SubDFaceIterator fit(*this);
      rc = ON_SubD::SetTextureDomains(fit, texture_domain_type, bSetFragmentTextureCoordinates);
    }
    break;

  case ON_SubDTextureDomainType::Custom:
    rc = false;
    break;

  default:
    rc = false;
    break;
  }

  if (rc && nullptr != subdimple)
    subdimple->SetTextureDomainType(texture_domain_type);

  return rc;
}

bool ON_SubD::SetTextureDomains(
  ON_SubDFaceIterator& fit,
  ON_SubDTextureDomainType texture_domain_type,
  bool bSetFragmentTextureCoordinates
)
{
  if (ON_SubDTextureDomainType::Custom == texture_domain_type)
    return ON_SUBD_RETURN_ERROR(false);

  bool bFragmentsExist = false;
  const double default_tc = (ON_SubDTextureDomainType::Zero == texture_domain_type) ? 0.0 : ON_DBL_QNAN;
  const ON_2dPoint default_origin(default_tc, default_tc);
  const ON_2dVector default_delta(default_tc, default_tc);
  unsigned face_count = 0;
  for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
  {
    f->SetTextureDomain(texture_domain_type, default_origin, default_delta, 0);
    if (f->m_edge_count >= 3)
      ++face_count;
    if (f->MeshFragments())
      bFragmentsExist = true;
  }

  bool rc
    = ON_SubDTextureDomainType::Unset == texture_domain_type
    || ON_SubDTextureDomainType::Zero == texture_domain_type
    || ON_SubDTextureDomainType::Nan == texture_domain_type;
  if (ON_SubDTextureDomainType::Packed == texture_domain_type)
  {
    texture_domain_type = ON_SubDTextureDomainType::PerFace;
  }

  if (ON_SubDTextureDomainType::PerFace == texture_domain_type)
  {
    const ON_2udex grid_size = ON_SubD::TextureDomainGridSize(face_count, 0.0, 0.0);
    const double image_width = ON_SubD::TextureImageSuggestedMinimumSize(grid_size);
    const double image_height = image_width;
    ON_2dVector face_texture_domain = ON_2dVector::NanVector;
    ON_2dVector face_texture_delta = ON_2dVector::NanVector;
    ON_SubD::GetTextureDomainAndDelta(
      face_count,
      image_height,
      image_width,
      face_texture_domain,
      face_texture_delta
    );

    ON_2udex k(0, 0);
    unsigned int face_dex = 0;
    for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace(), ++face_dex)
    {
      if (0 != face_dex)
      {
        k.i = (++k.i % grid_size.i);
        if (0 == k.i)
          ++k.j;
      }
      const ON_2dPoint face_texture_origin(k.i*face_texture_delta.x, k.j*face_texture_delta.y);
      f->SetTextureDomain(texture_domain_type, face_texture_origin, face_texture_domain, 0);
      if (false == bFragmentsExist && nullptr != f->MeshFragments())
        bFragmentsExist = true;
    }
    rc = true;
  }

  // Use the domains to set fragment texture coordinates
  if (bFragmentsExist && bSetFragmentTextureCoordinates)
    ON_SubD::SetTextureCoordinatesFromFaceDomains(fit);

  return rc;
}

bool ON_SubD::SetTextureCoordinatesFromFaceDomains() const
{
  if (ON_SubDTextureDomainType::Unset == this->TextureDomainType())
  {
    // uset default to packed
    if (false == SetTextureDomains(ON_SubDTextureDomainType::Packed, false, false))
      return false;
  }
  ON_SubDFaceIterator fit(*this);
  const bool rc = ON_SubD::SetTextureCoordinatesFromFaceDomains(fit);
  const ON_MappingTag& mapping_tag 
    = (rc)
    ? ON_MappingTag::SurfaceParameterMapping
    : ON_MappingTag::Unset;
  SetTextureMappingTag(mapping_tag);
  return rc;
}


bool ON_SubD::SetTextureCoordinatesFromFaceDomains(ON_SubDFaceIterator& fit)
{
  // estimate face image size - used to add empty space around ngon-subdfragments
  const unsigned int face_image_size = ON_SubD::TextureImageSuggestedMinimumSize(ON_SubD::TextureDomainGridSize(fit.FaceCount(), 0, 0));

  for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
  {
    if (f->m_edge_count < 3)
      continue;
    const ON_SubDMeshFragment* fragment = f->MeshFragments();
    if (nullptr == fragment)
      continue;
    const ON_2dPoint face_corners[4] = {
      f->TextureDomainCorner(true, false, 0),
      f->TextureDomainCorner(true, false, 1),
      f->TextureDomainCorner(true, false, 2),
      f->TextureDomainCorner(true, false, 3)
    };
    if (4 == f->m_edge_count)
    {
      // This face in a quad with a single fragment and that fragment gets the entire face_texture_domain
      fragment->SetTextureCoordinateCorners(true, face_corners, true);
    }
    else
    {
      // This face in an n-gon with n fragments that each get a portion of the face_texture_domain
      ON_2dVector frag_texture_domain = ON_2dVector::NanVector;
      ON_2dVector frag_texture_delta = ON_2dVector::NanVector;
      const ON_2udex fragment_grid_size = ON_SubD::GetTextureDomainAndDelta(
        f->m_edge_count,
        face_image_size,
        face_image_size,
        frag_texture_domain,
        frag_texture_delta
      );
      ON_2udex frag_k(0, 0);
      for (unsigned short frag_dex = 0; frag_dex < f->m_edge_count && nullptr != fragment; ++frag_dex, fragment = fragment->m_next_fragment)
      {
        if (0 != frag_dex)
        {
          frag_k.i = (++frag_k.i % fragment_grid_size.i);
          if (0 == frag_k.i)
            ++frag_k.j;
        }
        const double s0 = frag_k.i*frag_texture_delta.x;
        const double t0 = frag_k.j*frag_texture_delta.y;
        const double s2 = s0 + frag_texture_domain.x;
        const double t2 = t0 + frag_texture_domain.y;
        fragment->SetTextureCoordinateCorners(true, face_corners, s0, s2, t0, t2, true);
      }
    }
  }

  return true;
}

void ON_SubD::SetTextureMappingTag(const ON_MappingTag &mapping_tag) const
{
  const ON_SubDimple* subdimple = SubDimple();
  if (nullptr != subdimple)
    subdimple->SetTextureMappingTag(mapping_tag);
}

bool ON_SubD::SetTextureCoordinates(
  const class ON_TextureMapping& mapping,
  const class ON_Xform* subd_xform,
  bool bLazy
) const
{
  const ON_TextureMapping::TYPE mt = mapping.m_type;

  if (nullptr != subd_xform)
  {
    if (ON_TextureMapping::TYPE::srfp_mapping == mt || ON_TextureMapping::TYPE::no_mapping == mt)
      subd_xform = nullptr;
    else if (false == subd_xform->IsValid() || subd_xform->IsIdentity() || subd_xform->IsZero())
      subd_xform = nullptr;
  }

  const ON_MappingTag tag(mapping, subd_xform);

  if (0 == TextureMappingTag().Compare(tag))
    return true;

  if (ON_TextureMapping::TYPE::srfp_mapping == mt || ON_TextureMapping::TYPE::no_mapping == mt)
    SetTextureCoordinatesFromFaceDomains();

  const bool bApplyUVW = (mapping.m_uvw.IsValid() && !mapping.m_uvw.IsIdentity() && !mapping.m_uvw.IsZero());
  if (ON_TextureMapping::TYPE::srfp_mapping != mt || bApplyUVW)
  {
    //
    // NOTE WELL:
    //  ON_SubDMeshFragment store the mesh used to render ON_SubD objects
    //  The mesh topology, 3d vertex locations, 3d vertex normals
    //  CANNOT be modified to insert "texture seams."
    // 
    ON_3dPoint tc;
    ON_SubDMeshFragmentIterator frit(*this);
    for (const ON_SubDMeshFragment* fragment = frit.FirstFragment(); nullptr != fragment; fragment = frit.NextFragment())
    {
      const unsigned P_count = fragment->PointCount();
      if (P_count < 4)
        continue;
      const double* P = fragment->m_P;
      const size_t P_stride = fragment->m_P_stride;
      unsigned T_count = fragment->TextureCoordinateCount();
      if (P_count != T_count)
        continue;
      double* T = fragment->m_T;
      if (nullptr == T)
        continue;
      size_t T_stride = fragment->m_T_stride;
      if (0 == T_stride)
      {
        T_stride = 3;
        T_count = 1;
      }
      const unsigned N_count = fragment->NormalCount();
      const double* N = (N_count == P_count) ? fragment->m_N : &ON_3dVector::ZeroVector.x;
      const size_t N_stride = (N_count == P_count) ? fragment->m_N_stride : 0;
      for (double* T1 = T + T_stride * T_count; T < T1; T += T_stride, P += P_stride, N += N_stride)
      {
        if (ON_TextureMapping::TYPE::srfp_mapping != mt)
          tc = ON_3dPoint(T[0], T[1], 0.0);
        else if (false == mapping.Evaluate(ON_3dPoint(P), ON_3dVector(N), &tc))
          tc = ON_3dPoint::NanPoint;
        if (bApplyUVW)
          tc = mapping.m_uvw * tc;
        T[0] = tc.x;
        T[1] = tc.y;
        T[2] = tc.z;
      }
    }
  }

  SetTextureMappingTag(tag);

  return true;
}


const ON_MappingTag ON_SubD::TextureMappingTag() const
{
  const ON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->TextureMappingTag() : ON_MappingTag();
}

#pragma endregion
#endif

//////////////////////////////////////////////////////////////////////////////
//
// ON_SubDFace - texture coordinates
// 
#if 1
#pragma region ON_SubDFace - texture coordinates

void ON_SubDFace::SetTextureDomain(
  ON_SubDTextureDomainType texture_domain_type,
  ON_2dPoint origin,
  ON_2dVector delta,
  int packing_rotation_degrees
) const
{
  m_texture_coordinate_bits = 0;
  unsigned char domain_type_bits = static_cast<unsigned char>(texture_domain_type);
  if (domain_type_bits < 8)
  {
    domain_type_bits <<= 4;
    if (domain_type_bits != (domain_type_bits & TextureCoordinateBits::DomainTypeMask))
      domain_type_bits = 0;
  }
  if (
    0 != domain_type_bits
    && origin.IsValid()
    && delta.x >= 0.0 && delta.x < ON_UNSET_POSITIVE_VALUE
    && delta.y >= 0.0 && delta.y < ON_UNSET_POSITIVE_VALUE
    && 0 == packing_rotation_degrees % 90
    )
  {
    m_texture_coordinate_origin[0] = origin.x;
    m_texture_coordinate_origin[1] = origin.y;
    m_texture_coordinate_delta[0] = delta.x;
    m_texture_coordinate_delta[1] = delta.y;

    ON_SubDFace::TextureCoordinateBits packing_rotation = ON_SubDFace::TextureCoordinateBits::PackingRotate0;
    switch (((packing_rotation_degrees % 360) + 360) % 360)
    {
    case 90:
      packing_rotation = ON_SubDFace::TextureCoordinateBits::PackingRotate90;
      break;
    case 180:
      packing_rotation = ON_SubDFace::TextureCoordinateBits::PackingRotate180;
      break;
    case 270:
      packing_rotation = ON_SubDFace::TextureCoordinateBits::PackingRotate270;
      break;
    }
    m_texture_coordinate_bits |= packing_rotation;
    m_texture_coordinate_bits |= domain_type_bits;
  }
  else
  {
    m_texture_coordinate_origin[0] = ON_DBL_QNAN;
    m_texture_coordinate_origin[1] = ON_DBL_QNAN;
    m_texture_coordinate_delta[0] = ON_DBL_QNAN;
    m_texture_coordinate_delta[1] = ON_DBL_QNAN;
  }
}


const bool ON_SubDFace::TextureDomainIsSet() const
{
  return (ON_SubDTextureDomainType::Unset != TextureDomainType());
}

const ON_2dPoint ON_SubDFace::TextureDomainOrigin() const
{
  return ON_2dPoint(m_texture_coordinate_origin);
}

const ON_2dVector ON_SubDFace::TextureDomainDelta() const
{
  return ON_2dVector(m_texture_coordinate_delta);
}

ON_SubDTextureDomainType ON_SubDFace::TextureDomainType() const
{
  return ON_SubD::TextureDomainTypeFromUnsigned((m_texture_coordinate_bits & TextureCoordinateBits::DomainTypeMask)  >> 4);
}

unsigned int ON_SubDFace::TextureDomainRotationDegrees() const
{
  unsigned int packing_rotation_degrees = 0;
  switch (m_texture_coordinate_bits & ON_SubDFace::TextureCoordinateBits::PackingRotateMask)
  {
  case ON_SubDFace::TextureCoordinateBits::PackingRotate90:
    packing_rotation_degrees = 90;
    break;
  case ON_SubDFace::TextureCoordinateBits::PackingRotate180:
    packing_rotation_degrees = 180;
    break;
  case ON_SubDFace::TextureCoordinateBits::PackingRotate270:
    packing_rotation_degrees = 270;
    break;
  }
  return packing_rotation_degrees;
}

double ON_SubDFace::TextureDomainRotationRadians() const
{
  double x = 0.0;
  switch (m_texture_coordinate_bits & ON_SubDFace::TextureCoordinateBits::PackingRotateMask)
  {
  case ON_SubDFace::TextureCoordinateBits::PackingRotate90:
    x = 1.0;
    break;
  case ON_SubDFace::TextureCoordinateBits::PackingRotate180:
    x = 2.0;
    break;
  case ON_SubDFace::TextureCoordinateBits::PackingRotate270:
    x = 3.0;
    break;
  }
  return x * 0.5*ON_PI;
}

const ON_2dPoint ON_SubDFace::TextureDomainCorner(bool bGridOrder, bool bNormalized, int corner_index) const
{
  if (false == TextureDomainIsSet())
    return ON_2dPoint::Origin;

  corner_index = ((corner_index % 4) + 4) % 4;
  // now corner_index = 0,1,2 or 3.

  if (bGridOrder)
  {
    if (2 == corner_index)
      corner_index = 3;
    else if (3 == corner_index)
      corner_index = 2;
  }
  // now corner index is a counter-clockwise corner index

  int packrot_dex = 0;
  switch (m_texture_coordinate_bits & ON_SubDFace::TextureCoordinateBits::PackingRotateMask)
  {
  case ON_SubDFace::TextureCoordinateBits::PackingRotate90:
    packrot_dex = 3;
    break;
  case ON_SubDFace::TextureCoordinateBits::PackingRotate180:
    packrot_dex = 2;
    break;
  case ON_SubDFace::TextureCoordinateBits::PackingRotate270:
    packrot_dex = 1;
    break;
  }

  corner_index = (corner_index + packrot_dex) % 4;
  // now the packing rotation is taken into account.

  ON_2dPoint corner = bNormalized ? ON_2dPoint::Origin : TextureDomainOrigin();
  const ON_2dVector delta = bNormalized ? ON_2dVector(1.0, 1.0) : TextureDomainDelta();
  switch (corner_index)
  {
  case 1:
    corner.x += delta.x;
    break;
  case 2:
    corner.x += delta.x;
    corner.y += delta.y;
    break;
  case 3:
    corner.y += delta.y;
    break;
  }

  return corner;
}

#pragma endregion
#endif

//////////////////////////////////////////////////////////////////////////////
//
// ON_SubDMeshFragment - texture coordinates
// 
#if 1
#pragma region ON_SubDMeshFragment - texture coordinates

bool ON_SubDMeshFragment::TextureCoordinatesExist() const
{
  return (0 != (m_vertex_count_etc & ON_SubDMeshFragment::EtcTextureCoordinatesExistBit));
}

void ON_SubDMeshFragment::SetTextureCoordinatesExist(bool TextureCoordinatesExist) const
{
  if (TextureCoordinatesExist)
    m_vertex_count_etc |= ON_SubDMeshFragment::EtcTextureCoordinatesExistBit;
  else
    m_vertex_count_etc &= ~ON_SubDMeshFragment::EtcTextureCoordinatesExistBit;
}


unsigned int ON_SubDMeshFragment::TextureCoordinateCount() const
{
  return (TextureCoordinatesExist() && nullptr != m_T && (0 == m_T_stride || m_T_stride  >= 3)) ? VertexCount() : 0U;
}

unsigned int ON_SubDMeshFragment::TextureCoordinateCapacity() const
{
  return (nullptr != m_T && m_T_stride >= 3) ? VertexCapacity() : 0U;
}

const double* ON_SubDMeshFragment::TextureCoordinateArray(ON_SubDComponentLocation subd_appearance)const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? &m_ctrlnetT[0][0] : m_T;
}

size_t ON_SubDMeshFragment::TextureCoordinateArrayStride(ON_SubDComponentLocation subd_appearance)const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? 3 : m_T_stride;
}

unsigned ON_SubDMeshFragment::TextureCoordinateArrayCount(ON_SubDComponentLocation subd_appearance) const
{
  return (ON_SubDComponentLocation::ControlNet == subd_appearance) ? (TextureCoordinatesExist()?4U:0U) : TextureCoordinateCount();
}

bool  ON_SubDMeshFragment::GetTextureCoordinteCorners(
  bool bGridOrder,
  ON_3dPoint texture_coordinate_coeners[4]
) const
{
  // mutable double m_T_default_bbox[2][2];
  if (nullptr != texture_coordinate_coeners)
  {
    int i;
    texture_coordinate_coeners[0].x = m_ctrlnetT[0][0];
    texture_coordinate_coeners[0].y = m_ctrlnetT[0][1];
    texture_coordinate_coeners[0].z = m_ctrlnetT[0][2];

    texture_coordinate_coeners[1].x = m_ctrlnetT[1][0];
    texture_coordinate_coeners[1].y = m_ctrlnetT[1][1];
    texture_coordinate_coeners[1].z = m_ctrlnetT[1][2];

    i = bGridOrder ? 2 : 3;
    texture_coordinate_coeners[i].x = m_ctrlnetT[2][0];
    texture_coordinate_coeners[i].y = m_ctrlnetT[2][1];
    texture_coordinate_coeners[i].z = m_ctrlnetT[2][2];

    i = bGridOrder ? 3 : 2;
    texture_coordinate_coeners[i].x = m_ctrlnetT[3][0];
    texture_coordinate_coeners[i].y = m_ctrlnetT[3][1];
    texture_coordinate_coeners[i].z = m_ctrlnetT[3][2];

    return true;
  }
  return false;
}

static const ON_2dPoint InternalFragTextureCorner(
  bool bGridOrder,
  const ON_2dPoint Tcorners[4],
  double s,
  double t
)
{
  return (1 - s)*(1 - t)*Tcorners[0] + s * (1 - t)*Tcorners[1] + (1 - s)*t*Tcorners[bGridOrder ? 2 : 3] + s * t*Tcorners[bGridOrder ? 3 : 2];
}

void ON_SubDMeshFragment::SetTextureCoordinateCorners(
  bool bGridOrder,
  const ON_2dPoint texture_coordinate_corners[4],
  double s0,
  double s1,
  double t0,
  double t1,
  bool bSetTextureCoordinates
) const
{
  const ON_2dPoint c[4] = {
    InternalFragTextureCorner(bGridOrder,texture_coordinate_corners,s0,t0),
    InternalFragTextureCorner(bGridOrder,texture_coordinate_corners,s1,t0),
    InternalFragTextureCorner(bGridOrder,texture_coordinate_corners,s0,t1),
    InternalFragTextureCorner(bGridOrder,texture_coordinate_corners,s0,t1)
  };
  
  bGridOrder = true; // c[] is in grid order
  SetTextureCoordinateCorners(bGridOrder, c, bSetTextureCoordinates);
}

void ON_SubDMeshFragment::SetTextureCoordinateCorners(
  bool bGridOrder,
  const ON_2dPoint texture_coordinate_corners[4],
  bool bSetTextureCoordinates
) const
{
  if (nullptr != texture_coordinate_corners)
  {
    // m_ctrlnetT[] is in grid order.

    // lower left
    m_ctrlnetT[0][0] = texture_coordinate_corners[0].x;
    m_ctrlnetT[0][1] = texture_coordinate_corners[0].y;
    m_ctrlnetT[0][2] = 0.0;

    // lower right
    m_ctrlnetT[1][0] = texture_coordinate_corners[1].x;
    m_ctrlnetT[1][1] = texture_coordinate_corners[1].y;
    m_ctrlnetT[1][2] = 0.0;

    // upper left
    int i = bGridOrder ? 2 : 3;
    m_ctrlnetT[2][0] = texture_coordinate_corners[i].x;
    m_ctrlnetT[2][1] = texture_coordinate_corners[i].y;
    m_ctrlnetT[2][2] = 0.0;

    // upper right
    i = bGridOrder ? 3 : 2;
    m_ctrlnetT[3][0] = texture_coordinate_corners[i].x;
    m_ctrlnetT[3][1] = texture_coordinate_corners[i].y;
    m_ctrlnetT[3][2] = 0.0;

    if (bSetTextureCoordinates)
      SetTextureCoordinatesFromCorners();
  }
}

void ON_SubDMeshFragment::SetTextureCoordinateCorners(
  bool bGridOrder,
  const ON_3dPoint texture_coordinate_corners[4],
  bool bSetTextureCoordinates
) const
{
  if (nullptr != texture_coordinate_corners)
  {
    // m_ctrlnetT[] is in grid order.

    // lower left
    m_ctrlnetT[0][0] = texture_coordinate_corners[0].x;
    m_ctrlnetT[0][1] = texture_coordinate_corners[0].y;
    m_ctrlnetT[0][2] = texture_coordinate_corners[0].z;

    // lower right
    m_ctrlnetT[1][0] = texture_coordinate_corners[1].x;
    m_ctrlnetT[1][1] = texture_coordinate_corners[1].y;
    m_ctrlnetT[1][2] = texture_coordinate_corners[1].z;

    // upper left
    int i = bGridOrder ? 2 : 3;
    m_ctrlnetT[2][0] = texture_coordinate_corners[i].x;
    m_ctrlnetT[2][1] = texture_coordinate_corners[i].y;
    m_ctrlnetT[2][2] = texture_coordinate_corners[i].z;

    // upper right
    i = bGridOrder ? 3 : 2;
    m_ctrlnetT[3][0] = texture_coordinate_corners[i].x;
    m_ctrlnetT[3][1] = texture_coordinate_corners[i].y;
    m_ctrlnetT[3][2] = texture_coordinate_corners[i].z;

    if (bSetTextureCoordinates)
      SetTextureCoordinatesFromCorners();
  }
}

const ON_3dPoint ON_SubDMeshFragment::VertexTextureCoordinate(
  ON_2udex grid2dex
) const
{
  return VertexTextureCoordinate(m_grid.PointIndexFromGrid2dex(grid2dex.i, grid2dex.j));
}

const ON_3dPoint ON_SubDMeshFragment::VertexTextureCoordinate(
  unsigned grid2dex_i,
  unsigned grid2dex_j
) const
{
  return VertexTextureCoordinate(m_grid.PointIndexFromGrid2dex(grid2dex_i, grid2dex_j));
}

const ON_3dPoint ON_SubDMeshFragment::VertexTextureCoordinate(
  unsigned grid_point_index
) const
{
  return
    (grid_point_index >= TextureCoordinateCount())
    ? ON_3dPoint::NanPoint
    : ON_3dPoint(m_T + grid_point_index * m_T_stride);
}

const ON_3dPoint ON_SubDMeshFragment::VertexTextureCoordinateFromCorners(unsigned grid2dex_i, unsigned grid2dex_j) const
{
  const unsigned n = m_grid.SideSegmentCount();
  if (n <= 0U || grid2dex_i > n || grid2dex_j > n)
    return ON_3dPoint::NanPoint;
  const double s = ((double)grid2dex_i) / ((double)n);
  const double t = ((double)grid2dex_j) / ((double)n);
  const double c[4] = { (1.0 - s)*(1.0 - t), s*(1.0 - t), (1.0 - s)*t, s*t };
  return ON_3dPoint(
    c[0] * m_ctrlnetT[0][0] + c[1] * m_ctrlnetT[1][0] + c[2] * m_ctrlnetT[2][0] + c[3] * m_ctrlnetT[3][0],
    c[0] * m_ctrlnetT[0][1] + c[1] * m_ctrlnetT[1][1] + c[2] * m_ctrlnetT[2][1] + c[3] * m_ctrlnetT[3][1],
    c[0] * m_ctrlnetT[0][2] + c[1] * m_ctrlnetT[1][2] + c[2] * m_ctrlnetT[2][2] + c[3] * m_ctrlnetT[3][2]
  );
}

const ON_3dPoint ON_SubDMeshFragment::VertexTextureCoordinateFromCorners(ON_2udex grid2dex) const
{
  return VertexTextureCoordinateFromCorners(grid2dex.i, grid2dex.j);
}

const ON_3dPoint ON_SubDMeshFragment::VertexTextureCoordinateFromCorners(unsigned grid_point_index) const
{
  return VertexTextureCoordinateFromCorners(m_grid.Grid2dexFromPointIndex(grid_point_index));
}


void ON_SubDMeshFragment::SetTextureCoordinatesFromCorners() const
{
  const unsigned n = m_grid.SideSegmentCount();
  if (n <= 0U || n > ON_SubDMeshFragment::MaximumSideSegmentCount)
    return;
  if (TextureCoordinateCapacity() < n*n)
    return;
  SetTextureCoordinatesExist(true);
  double * T = m_T;
  const double d = (double)n;
  double s, t;
  ON_3dPoint tc;
  for (unsigned j = 0U; j <= n; ++j)
  {
    t = ((double)j) / d;
    for (unsigned i = 0U; i <= n; ++i)
    {
      s = ((double)i) / d;
      const double c[4] = { (1.0 - s)*(1.0 - t), s*(1.0 - t), (1.0 - s)*t, s*t };
      tc = ON_3dPoint(
        c[0] * m_ctrlnetT[0][0] + c[1] * m_ctrlnetT[1][0] + c[2] * m_ctrlnetT[2][0] + c[3] * m_ctrlnetT[3][0],
        c[0] * m_ctrlnetT[0][1] + c[1] * m_ctrlnetT[1][1] + c[2] * m_ctrlnetT[2][1] + c[3] * m_ctrlnetT[3][1],
        c[0] * m_ctrlnetT[0][2] + c[1] * m_ctrlnetT[1][2] + c[2] * m_ctrlnetT[2][2] + c[3] * m_ctrlnetT[3][2]
      );
      T[0] = tc.x;
      T[1] = tc.y;
      T[2] = tc.z;
      T += m_T_stride;
    }
  }
  return;
}

bool ON_SubDMeshFragment::SetVertexTextureCoordinate(
  ON_2udex grid2dex,
  ON_3dPoint texture_coordinate
) const
{
  return SetVertexTextureCoordinate(m_grid.PointIndexFromGrid2dex(grid2dex.i, grid2dex.j), texture_coordinate);
}

bool ON_SubDMeshFragment::SetVertexTextureCoordinate(
  unsigned grid2dex_i,
  unsigned grid2dex_j,
  ON_3dPoint texture_coordinate
) const
{
  return SetVertexTextureCoordinate(m_grid.PointIndexFromGrid2dex(grid2dex_i, grid2dex_j), texture_coordinate);
}

bool ON_SubDMeshFragment::SetVertexTextureCoordinate(
  unsigned grid_point_index,
  ON_3dPoint texture_coordinate
) const
{
  if (grid_point_index >= TextureCoordinateCapacity())
    return false;
  double* T = (m_T + grid_point_index * m_T_stride);
  T[0] = texture_coordinate.x;
  T[1] = texture_coordinate.y;
  T[2] = texture_coordinate.z;
  SetTextureCoordinatesExist(true);
  return true;
}

#pragma endregion
#endif
