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
#include "opennurbs_internal_defines.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

// ON_Decal

static ON_4dPoint UNSET_4D_POINT = ON_4dPoint(ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE);

ON_DECAL_CRC ON_DecalCRCFromNode(const ON_XMLNode& node)
{
	return ON_Decal::ComputeDecalCRC(0, node);
}

static ON_XMLNode* FindDecalNodeByCRC(const ON_XMLNode& decals_node, ON_DECAL_CRC decal_crc)
{
  auto it = decals_node.GetChildIterator();
  ON_XMLNode* decal_node = nullptr;
  while (nullptr != (decal_node = it.GetNextChild()))
  {
    if (decal_crc == ON_DecalCRCFromNode(*decal_node))
      break;
  }

  return decal_node;
}

static double PrincipalValueAngleRad(double angleRad)
{
  // Returns the given angle modulo 2 * PI.

  if (angleRad >= ON_2PI)
  {
    angleRad = angleRad - floor(angleRad / ON_2PI) * ON_2PI;
  }
  else
  if (angleRad <= 0.0)
  {
    angleRad = angleRad + ceil(-angleRad / ON_2PI) * ON_2PI;
  }

  if (angleRad < 1e-12)
    angleRad = 0.0;

  if (angleRad > ON_2PI)
    angleRad = ON_2PI;

  return angleRad;
}

static void UpdateValuesForTransformation(
            const ON_Xform& xform, ON_Decal::Mappings mapping, double& radius, double& height,
            ON_3dPoint& ptOrigin, ON_3dVector& vecUp, ON_3dVector& vecAcross)
{
  ptOrigin.Transform(xform);
  vecUp.Transform(xform);
  vecAcross.Transform(xform);

  ON_ASSERT(ON_Decal::Mappings::UV != mapping);

  if ((ON_Decal::Mappings::Cylindrical == mapping) || (ON_Decal::Mappings::Spherical == mapping))
  {
    // If spherical we assume uniform scaling so as to preserve the spherical shape.
    // If cylindrical we assume uniform scaling in x and y so as to preserve the cylindrical shape.
    const double radius_scale = abs(xform[0][0]);
    radius *= radius_scale;

    if (ON_Decal::Mappings::Cylindrical == mapping)
    {
      // Transform the height. The unit cylinder stands up with height along the z-axis.
      ON_3dVector vecHeight = ON_3dVector::ZAxis;
      vecHeight.Transform(xform);
      height *= vecHeight.Length();
    }

    vecUp.Unitize();
    vecAcross.Unitize();
  }
}

static ON_Decal::Mappings MappingFromString(const ON_wString& s)
{
       if (s == ON_RDK_DECAL_MAPPING_PLANAR)      return ON_Decal::Mappings::Planar;
  else if (s == ON_RDK_DECAL_MAPPING_SPHERICAL)   return ON_Decal::Mappings::Spherical;
  else if (s == ON_RDK_DECAL_MAPPING_CYLINDRICAL) return ON_Decal::Mappings::Cylindrical;
  else if (s == ON_RDK_DECAL_MAPPING_UV)          return ON_Decal::Mappings::UV;
  else if (s == ON_RDK_DECAL_MAPPING_NONE)        return ON_Decal::Mappings::None;

  ON_ASSERT(false);
  return ON_Decal::Mappings::None;
}

class ON_Decal::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl();
  CImpl(ON_DecalCollection* dc,       ON_XMLNode& node);
  CImpl(ON_DecalCollection* dc, const ON_XMLNode& node);

  ON_UUID TextureInstanceId(void) const;
  void SetTextureInstanceId(const ON_UUID& id);
  Mappings Mapping(void) const;
  void SetMapping(Mappings v);
  Projections Projection(void) const;
  void SetProjection(Projections v);
  bool MapToInside(void) const;
  void SetMapToInside(bool v);
  double Transparency(void) const;
  void SetTransparency(double v);
  ON_3dPoint Origin(void) const;
  void SetOrigin(const ON_3dPoint& v);
  ON_3dVector VectorUp(void) const;
  void SetVectorUp(const ON_3dVector& v);
  ON_3dVector VectorAcross(void) const;
  void SetVectorAcross(const ON_3dVector& v);
  double Height(void) const;
  void SetHeight(double d);
  double Radius(void) const;
  void SetRadius(double d);
  void GetSavedVectorLengths(double& across, double& up) const;
  void SetSavedVectorLengths(double across, double up);
  bool IsVisible(void) const;
  void SetIsVisible(bool b);
  bool IsTemporary(void) const;
  void SetIsTemporary(bool b);
  void GetHorzSweep(double& sta, double& end) const;
  void SetHorzSweep(double sta, double end);
  void GetVertSweep(double& sta, double& end) const;
  void SetVertSweep(double sta, double end);
  void GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const;
  void SetUVBounds(double min_u, double min_v, double max_u, double max_v);

  ON_UUID Id(void) const { return _decal_id; }
  ON_XMLVariant GetParameter(const wchar_t* param_name, const ON_XMLVariant& def) const;
  void SetParameter(const wchar_t* param_name, const ON_XMLVariant& value);
  ON_XMLNode* FindCustomNodeForRenderEngine(const ON_UUID& renderEngineId) const;

  bool GetTextureMapping(ON_TextureMapping& mappingOut) const;

  void SetCollectionChanged(void);

  bool HitTest(const ON_3dPoint& point, const ON_3dVector& normal, ON_2dPoint& uvOut) const;

  void ApplyTransformation(const ON_Xform& xform);

  virtual ON_wString NameOfRootNode(void) const override { return ON_RDK_DECAL; }

  ON_3dVector GetOriginOffset(void) const;

  static const int unset_bool = 2;
  mutable struct Cache
  {
    double radius       = ON_UNSET_VALUE;
    double height       = ON_UNSET_VALUE;
    double transparency = ON_UNSET_VALUE;
    ON_UUID texture_instance_id = ON_nil_uuid;
    ON_3dPoint origin         = ON_3dPoint ::UnsetPoint;
    ON_3dVector vector_up     = ON_3dVector::UnsetVector;
    ON_3dVector vector_across = ON_3dVector::UnsetVector;
    ON_2dPoint horz_sweep     = ON_2dPoint ::UnsetPoint;
    ON_2dPoint vert_sweep     = ON_2dPoint ::UnsetPoint;
    ON_4dPoint uv_bounds      = UNSET_4D_POINT;
    Mappings mapping = Mappings::None;
    Projections projection = Projections::None;
    bool texture_instance_id_set = false;
    int visible = unset_bool;
    int temporary = unset_bool;
    int map_to_inside = unset_bool;
  }
  _cache;

  ON_DecalCollection* _collection = nullptr;
  ON_UUID _decal_id;
  bool _read_only = false;
  bool _cache_only = false;

private:
  bool HitTestUV(const ON_3dPoint& point, ON_3dPoint& uvwOut) const;
  bool HitTestPlanar(const ON_3dVector& normal, const ON_3dPoint& point, ON_3dPoint& uvwOut) const;
  bool HitTestCylindrical(const ON_3dVector& normal, const ON_3dPoint& point, ON_3dPoint& uvwOut) const;
  bool HitTestSpherical(const ON_3dVector& normal, const ON_3dPoint& point, ON_3dPoint& uvwOut) const;
  bool Writeable(void) const { ON_ASSERT(!_read_only); return !_read_only; }

};

ON_Decal::CImpl::CImpl()
{
  ON_CreateUuid(_decal_id);
}

ON_Decal::CImpl::CImpl(ON_DecalCollection* dc, ON_XMLNode& node)
  :
  _collection(dc),
  ON_InternalXMLImpl(&node)
{
  ON_CreateUuid(_decal_id);
}

ON_Decal::CImpl::CImpl(ON_DecalCollection* dc, const ON_XMLNode& node)
  :
  _collection(dc),
  _read_only(true),
  ON_InternalXMLImpl(&const_cast<ON_XMLNode&>(node))
{
  ON_CreateUuid(_decal_id);
}

ON_XMLVariant ON_Decal::CImpl::GetParameter(const wchar_t* param_name, const ON_XMLVariant& def) const
{
  return ON_InternalXMLImpl::GetParameter(L"", param_name, def);
}

void ON_Decal::CImpl::SetParameter(const wchar_t* param_name, const ON_XMLVariant& value)
{
  ON_ASSERT(!_cache_only);
  ON_ASSERT(!_read_only);

  ON_InternalXMLImpl::SetParameter(L"", param_name, value);
}

ON_UUID ON_Decal::CImpl::TextureInstanceId(void) const
{
  if (!_cache.texture_instance_id_set)
  {
    _cache.texture_instance_id = GetParameter(ON_RDK_DECAL_TEXTURE_INSTANCE, ON_nil_uuid).AsUuid();
    _cache.texture_instance_id_set = true;
  }

  return _cache.texture_instance_id;
}

void ON_Decal::CImpl::SetTextureInstanceId(const ON_UUID& id)
{
  if (!Writeable())
    return;

  if (!_cache.texture_instance_id_set || (_cache.texture_instance_id != id))
  {
    _cache.texture_instance_id = id;
    _cache.texture_instance_id_set = true;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_TEXTURE_INSTANCE, id);
    }

    SetCollectionChanged();
  }
}

ON_Decal::Mappings ON_Decal::CImpl::Mapping(void) const
{
  if (Mappings::None == _cache.mapping)
  {
    const ON_wString s = GetParameter(ON_RDK_DECAL_MAPPING, ON_RDK_DECAL_MAPPING_UV).AsString();
    _cache.mapping = MappingFromString(s);
  }

  return _cache.mapping;
}

void ON_Decal::CImpl::SetMapping(Mappings m)
{
  if (!Writeable())
    return;

  if (_cache.mapping != m)
  {
    _cache.mapping = m;

    const wchar_t* s = L"";
    if (!_cache_only)
    {
      switch (m)
      {
      default: ON_ASSERT(false);
      case Mappings::None:        s = ON_RDK_DECAL_MAPPING_NONE;        break;
      case Mappings::Planar:      s = ON_RDK_DECAL_MAPPING_PLANAR;      break;
      case Mappings::Spherical:   s = ON_RDK_DECAL_MAPPING_SPHERICAL;   break;
      case Mappings::Cylindrical: s = ON_RDK_DECAL_MAPPING_CYLINDRICAL; break;
      case Mappings::UV:          s = ON_RDK_DECAL_MAPPING_UV;          break;
      }

      SetParameter(ON_RDK_DECAL_MAPPING, s);
    }

    SetCollectionChanged();
  }
}

ON_Decal::Projections ON_Decal::CImpl::Projection(void) const
{
  if (Projections::None == _cache.projection)
  {
    const ON_wString s = GetParameter(ON_RDK_DECAL_PROJECTION, ON_RDK_DECAL_PROJECTION_NONE).AsString();
         if (s == ON_RDK_DECAL_PROJECTION_FORWARD)  _cache.projection = ON_Decal::Projections::Forward;
    else if (s == ON_RDK_DECAL_PROJECTION_BACKWARD) _cache.projection = ON_Decal::Projections::Backward;
    else if (s == ON_RDK_DECAL_PROJECTION_BOTH)     _cache.projection = ON_Decal::Projections::Both;
    else if (s == ON_RDK_DECAL_PROJECTION_NONE)     _cache.projection = ON_Decal::Projections::None;
    else ON_ASSERT(false);
  }

  return _cache.projection;
}

void ON_Decal::CImpl::SetProjection(Projections v)
{
  if (!Writeable())
    return;

  if (_cache.projection != v)
  {
    _cache.projection = v;

    const wchar_t* s = L"";
    if (!_cache_only)
    {
      switch (v)
      {
      default: ON_ASSERT(false);
      case ON_Decal::Projections::None:     s = ON_RDK_DECAL_PROJECTION_NONE;     break;
      case ON_Decal::Projections::Forward:  s = ON_RDK_DECAL_PROJECTION_FORWARD;  break;
      case ON_Decal::Projections::Backward: s = ON_RDK_DECAL_PROJECTION_BACKWARD; break;
      case ON_Decal::Projections::Both:     s = ON_RDK_DECAL_PROJECTION_BOTH;     break;
      }

      SetParameter(ON_RDK_DECAL_PROJECTION, s);
    }

    SetCollectionChanged();
  }
}

bool ON_Decal::CImpl::MapToInside(void) const
{
  if (unset_bool == _cache.map_to_inside)
  {
    _cache.map_to_inside = GetParameter(ON_RDK_DECAL_MAP_TO_INSIDE_ON, false).AsBool() ? 1 : 0;
  }

  return 0 != _cache.map_to_inside;
}

void ON_Decal::CImpl::SetMapToInside(bool b)
{
  if (!Writeable())
    return;

  const int i = b ? 1 : 0;
  if (_cache.map_to_inside != i)
  {
    _cache.map_to_inside = i;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_MAP_TO_INSIDE_ON, b);
    }

    SetCollectionChanged();
  }
}

double ON_Decal::CImpl::Transparency(void) const
{
  if (ON_UNSET_VALUE == _cache.transparency)
  {
    _cache.transparency = GetParameter(ON_RDK_DECAL_TRANSPARENCY, 0.0).AsDouble();
  }

  return _cache.transparency;
}

void ON_Decal::CImpl::SetTransparency(double v)
{
  if (!Writeable())
    return;

  if (_cache.transparency != v)
  {
    _cache.transparency = v;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_TRANSPARENCY, v);
    }

    SetCollectionChanged();
  }
}

ON_3dPoint ON_Decal::CImpl::Origin(void) const
{
  if (ON_3dPoint::UnsetPoint == _cache.origin)
  {
    _cache.origin = GetParameter(ON_RDK_DECAL_ORIGIN, ON_3dPoint::Origin).As3dPoint();
  }

  return _cache.origin;
}

void ON_Decal::CImpl::SetOrigin(const ON_3dPoint& pt)
{
  if (!Writeable())
    return;

  if (_cache.origin != pt)
  {
    _cache.origin = pt;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_ORIGIN, pt);
    }

    SetCollectionChanged();
  }
}

ON_3dVector ON_Decal::CImpl::VectorUp(void) const
{
  if (ON_3dVector::UnsetVector == _cache.vector_up)
  {
     _cache.vector_up = GetParameter(ON_RDK_DECAL_VECTOR_UP, ON_3dPoint::Origin).As3dPoint();
  }

  return _cache.vector_up;
}

void ON_Decal::CImpl::SetVectorUp(const ON_3dVector& v)
{
  if (!Writeable())
    return;

  if (_cache.vector_up != v)
  {
    _cache.vector_up = v;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_VECTOR_UP, ON_3dPoint(v));
    }

    SetCollectionChanged();
  }
}

ON_3dVector ON_Decal::CImpl::VectorAcross(void) const
{
  if (ON_3dVector::UnsetVector == _cache.vector_across)
  {
     _cache.vector_across = GetParameter(ON_RDK_DECAL_VECTOR_ACROSS, ON_3dPoint::Origin).As3dPoint();
  }

  return _cache.vector_across;
}

void ON_Decal::CImpl::SetVectorAcross(const ON_3dVector& v)
{
  if (!Writeable())
    return;

  if (_cache.vector_across != v)
  {
    _cache.vector_across = v;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_VECTOR_ACROSS, ON_3dPoint(v));
    }

    SetCollectionChanged();
  }
}

double ON_Decal::CImpl::Height(void) const
{
  if (ON_UNSET_VALUE == _cache.height)
  {
    _cache.height = GetParameter(ON_RDK_DECAL_HEIGHT, 1.0).AsDouble();
  }

  return _cache.height;
}

void ON_Decal::CImpl::SetHeight(double v)
{
  if (!Writeable())
    return;

  if (_cache.height != v)
  {
    _cache.height = v;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_HEIGHT, v);
    }

    SetCollectionChanged();
  }
}

double ON_Decal::CImpl::Radius(void) const
{
  if (ON_UNSET_VALUE == _cache.radius)
  {
    _cache.radius = GetParameter(ON_RDK_DECAL_RADIUS, 1.0).AsDouble();
  }

  return _cache.radius;
}

void ON_Decal::CImpl::SetRadius(double v)
{
  if (!Writeable())
    return;

  if (_cache.radius != v)
  {
    _cache.radius = v;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_RADIUS, v);
    }

    SetCollectionChanged();
  }
}

void ON_Decal::CImpl::GetSavedVectorLengths(double& across, double& up) const
{
  across = GetParameter(ON_RDK_DECAL_SAVED_VECTOR_LENGTH_ACROSS, 0.0);
  up     = GetParameter(ON_RDK_DECAL_SAVED_VECTOR_LENGTH_UP, 0.0);
}

void ON_Decal::CImpl::SetSavedVectorLengths(double across, double up)
{
  SetParameter(ON_RDK_DECAL_SAVED_VECTOR_LENGTH_ACROSS, across);
  SetParameter(ON_RDK_DECAL_SAVED_VECTOR_LENGTH_UP, up);
}

ON_3dVector ON_Decal::CImpl::GetOriginOffset(void) const
{
  ON_3dVector vec = { };
  vec.x = 0.5 * VectorAcross().Length();
  vec.y = 0.5 * VectorUp().Length();

  return vec;
}

bool ON_Decal::CImpl::IsVisible(void) const
{
  if (unset_bool == _cache.visible)
  {
    _cache.visible = GetParameter(ON_RDK_DECAL_IS_VISIBLE, true).AsBool();
  }

  return 0 != _cache.visible;
}

void ON_Decal::CImpl::SetIsVisible(bool b)
{
  if (!Writeable())
    return;

  const int i = b ? 1 : 0;
  if (_cache.visible != i)
  {
    _cache.visible = i;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_IS_VISIBLE, b);
    }

    SetCollectionChanged();
  }
}

bool ON_Decal::CImpl::IsTemporary(void) const
{
  if (unset_bool == _cache.temporary)
  {
    _cache.temporary = GetParameter(ON_RDK_DECAL_IS_TEMPORARY, false).AsBool();
  }

  return 0 != _cache.temporary;
}

void ON_Decal::CImpl::SetIsTemporary(bool b)
{
  if (!Writeable())
    return;

  const int i = b ? 1 : 0;
  if (_cache.temporary != i)
  {
    _cache.temporary = i;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_IS_TEMPORARY, b);
    }

    SetCollectionChanged();
  }
}

void ON_Decal::CImpl::GetHorzSweep(double& sta, double& end) const
{
  if (ON_2dPoint::UnsetPoint == _cache.horz_sweep)
  {
    _cache.horz_sweep.x = GetParameter(ON_RDK_DECAL_HORZ_SWEEP_STA, 0.0).AsDouble();
    _cache.horz_sweep.y = GetParameter(ON_RDK_DECAL_HORZ_SWEEP_END, 0.0).AsDouble();
  }

  sta = _cache.horz_sweep.x;
  end = _cache.horz_sweep.y;
}

void ON_Decal::CImpl::SetHorzSweep(double sta, double end)
{
  if (!Writeable())
    return;

  const ON_2dPoint sweep = ON_2dPoint(sta, end);
  if (_cache.horz_sweep != sweep)
  {
    _cache.horz_sweep = sweep;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_HORZ_SWEEP_STA, sta);
      SetParameter(ON_RDK_DECAL_HORZ_SWEEP_END, end);
    }

    SetCollectionChanged();
  }
}

void ON_Decal::CImpl::GetVertSweep(double& sta, double& end) const
{
  if (ON_2dPoint::UnsetPoint == _cache.vert_sweep)
  {
    _cache.vert_sweep.x = GetParameter(ON_RDK_DECAL_VERT_SWEEP_STA, 0.0).AsDouble();
    _cache.vert_sweep.y = GetParameter(ON_RDK_DECAL_VERT_SWEEP_END, 0.0).AsDouble();
  }

  sta = _cache.vert_sweep.x;
  end = _cache.vert_sweep.y;
}

void ON_Decal::CImpl::SetVertSweep(double sta, double end)
{
  if (!Writeable())
    return;

  const ON_2dPoint sweep = ON_2dPoint(sta, end);
  if (_cache.vert_sweep != sweep)
  {
    _cache.vert_sweep = sweep;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_VERT_SWEEP_STA, sta);
      SetParameter(ON_RDK_DECAL_VERT_SWEEP_END, end);
    }

    SetCollectionChanged();
  }
}

void ON_Decal::CImpl::GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const
{
  if (UNSET_4D_POINT == _cache.uv_bounds)
  {
    _cache.uv_bounds.x = GetParameter(ON_RDK_DECAL_MIN_U, 0.0).AsDouble();
    _cache.uv_bounds.y = GetParameter(ON_RDK_DECAL_MIN_V, 0.0).AsDouble();
    _cache.uv_bounds.z = GetParameter(ON_RDK_DECAL_MAX_U, 1.0).AsDouble();
    _cache.uv_bounds.w = GetParameter(ON_RDK_DECAL_MAX_V, 1.0).AsDouble();
  }

  min_u = _cache.uv_bounds.x;
  min_v = _cache.uv_bounds.y;
  max_u = _cache.uv_bounds.z;
  max_v = _cache.uv_bounds.w;
}

void ON_Decal::CImpl::SetUVBounds(double min_u, double min_v, double max_u, double max_v)
{
  if (!Writeable())
    return;

  const ON_4dPoint bounds = ON_4dPoint(min_u, min_v, max_u, max_v);
  if (_cache.uv_bounds != bounds)
  {
    _cache.uv_bounds = bounds;

    if (!_cache_only)
    {
      SetParameter(ON_RDK_DECAL_MIN_U, min_u);
      SetParameter(ON_RDK_DECAL_MIN_V, min_v);
      SetParameter(ON_RDK_DECAL_MAX_U, max_u);
      SetParameter(ON_RDK_DECAL_MAX_V, max_v);
    }

    SetCollectionChanged();
  }
}

ON_XMLNode* ON_Decal::CImpl::FindCustomNodeForRenderEngine(const ON_UUID& renderEngineId) const
{
  ON_XMLNode* child_node = nullptr;
  auto it = Node().GetChildIterator();
  while (nullptr != (child_node = it.GetNextChild()))
  {
    if (child_node->TagName() == ON_RDK_DECAL_CUSTOM)
    {
      if (ON_UuidIsNil(renderEngineId))
        return child_node;

      const ON_XMLProperty* prop = child_node->GetNamedProperty(ON_RDK_DECAL_CUSTOM_RENDERER);
      if ((nullptr != prop) && (prop->GetValue().AsUuid() == renderEngineId))
        return child_node;
    }
  }

  return nullptr;
}

bool ON_Decal::CImpl::GetTextureMapping(ON_TextureMapping& mappingOut) const
{
  using M = ON_Decal::Mappings;

  switch (Mapping())
  {
  case M::Cylindrical:
    {
    // Orthogonal vectors in the end plane of cylinder.
    const ON_3dVector vecPlaneXAxis = VectorAcross();
    const ON_3dVector vecPlaneYAxis = ON_CrossProduct(VectorAcross(), -VectorUp());

    // Center for the end of the cylinder.
    const ON_3dPoint ptCylinderEndCenter = Origin() - VectorUp() * Height() * 0.5;

    // Plane for the end of the cylinder.
    ON_Plane plane(ptCylinderEndCenter, vecPlaneXAxis, vecPlaneYAxis);

    // Circle for the end of the cylinder.
    ON_Circle circle(plane, Radius());

    // The cylinder itself..
    ON_Cylinder cylinder(circle, Height());

    // Cylindrical mapping without caps.
    mappingOut.SetCylinderMapping(cylinder, false);

    return true;
    }

  case M::Spherical:
    {
    // Orthogonal vectors in the equatorial plane.
    const ON_3dVector vecPlaneXAxis = VectorAcross();
    const ON_3dVector vecPlaneYAxis = ON_CrossProduct(VectorAcross(), -VectorUp());

    // Equatorial plane.
    ON_Plane plane(Origin(), vecPlaneXAxis, vecPlaneYAxis);

    // The sphere itself.
    ON_Sphere sphere;
    sphere.plane = plane;
    sphere.radius = Radius();

    // Spherical mapping.
    mappingOut.SetSphereMapping(sphere);

    return true;
    }

  case M::Planar:
    {
    const ON_3dVector vecAcross = VectorAcross();
    const ON_3dVector vecUp = VectorUp();
    const ON_Plane plane(Origin(), vecAcross, vecUp);
    const ON_Interval xInterval(0.0, vecAcross.Length());
    const ON_Interval yInterval(0.0, vecUp.Length());
    const ON_Interval zInterval(0.0, 1.0);
    mappingOut.SetPlaneMapping(plane, xInterval, yInterval, zInterval);

    return true;
    }

  default: break;
  }

  return false;
}

void ON_Decal::CImpl::SetCollectionChanged(void)
{
  if (nullptr != _collection)
  {
    _collection->SetChanged();
  }
}

bool ON_Decal::CImpl::HitTest(const ON_3dPoint& point, const ON_3dVector& normal, ON_2dPoint& uvOut) const
{
  bool hit = false;

  ON_3dPoint uvw(0.0, 0.0, 0.0);

  switch (Mapping())
  {
  case Mappings::UV:          hit = HitTestUV         (        point, uvw); break;
  case Mappings::Planar:      hit = HitTestPlanar     (normal, point, uvw); break;
  case Mappings::Cylindrical: hit = HitTestCylindrical(normal, point, uvw); break;
  case Mappings::Spherical:   hit = HitTestSpherical  (normal, point, uvw); break;
  default: break;
  }

  if (hit)
  {
    uvOut.x = uvw.x;
    uvOut.y = uvw.y;
  }

  return hit;
}

void ON_Decal::CImpl::ApplyTransformation(const ON_Xform& xform)
{
  if (!Writeable())
    return;

  if (xform.IsIdentity())
    return;

  ON_3dPoint origin = Origin();
  ON_3dVector up = VectorUp();
  ON_3dVector across = VectorAcross();

  double radius = 0.0, height = 0.0;

  const ON_Decal::Mappings mapping = Mapping();

  if ((Mappings::Cylindrical == mapping) || (Mappings::Spherical == mapping))
  {
    radius = Radius();

    if (Mappings::Cylindrical == mapping)
    {
      height = Height();
    }
  }

  UpdateValuesForTransformation(xform, mapping, radius, height, origin, up, across);

  SetOrigin(origin);
  SetVectorUp(up);
  SetVectorAcross(across);

  if ((Mappings::Cylindrical == mapping) || (Mappings::Spherical == mapping))
  {
    SetRadius(radius);

    if (Mappings::Cylindrical == mapping)
    {
      SetHeight(height);
    }
  }
}

bool ON_Decal::CImpl::HitTestUV(const ON_3dPoint& point, ON_3dPoint& uvwOut) const
{
  double minU = 0.0, maxU = 1.0, minV = 0.0, maxV = 1.0;
  GetUVBounds(minU, minV, maxU, maxV);

  if ((minU <= point.x) && (maxU >= point.x) && (minV <= point.y) && (maxV >= point.y))
  {
    uvwOut.x = (point.x - minU) / (maxU - minU);
    uvwOut.y = (point.y - minV) / (maxV - minV);

    return true;
  }

  return false;
}

bool ON_Decal::CImpl::HitTestPlanar(const ON_3dVector& normal,
                                    const ON_3dPoint& point, ON_3dPoint& uvwOut) const
{
  const ON_3dVector vecAcross = VectorAcross();
  const ON_3dVector vecUp = VectorUp();
  const ON_Decal::Projections projection = Projection();

  const ON_3dVector vecCrossProduct = ON_CrossProduct(vecAcross, vecUp);
  const double dotProduct = ON_DotProduct(vecCrossProduct, normal);

  if ((projection == ON_Decal::Projections::Forward) && (dotProduct < 0))
    return false;

  if ((projection == ON_Decal::Projections::Backward) && (dotProduct > 0))
    return false;

  if (dotProduct == 0.0)
    return false;

  // Map world coordinates to uvw.
  ON_TextureMapping m;
  GetTextureMapping(m);
  m.Evaluate(point, normal, &uvwOut);

  if ((uvwOut.x >= 0.0) && (uvwOut.x <= 1.0) && (uvwOut.y >= 0.0) && (uvwOut.y <= 1.0))
    return true;

  return false;
}

bool ON_Decal::CImpl::HitTestCylindrical(const ON_3dVector& normal,
                                         const ON_3dPoint& point, ON_3dPoint& uvwOut) const
{
  // Check that the face normal is pointing to correct direction.
  const double faceNormalDotOriginDirection = ON_DotProduct(normal, point - Origin());

  const bool bInside = faceNormalDotOriginDirection < 0.0;
  const bool bMapToInside = MapToInside();

  if (bInside && !bMapToInside)
    return false;

  if (!bInside && bMapToInside)
    return false;

  // Map world coordinates to uvw.
  ON_TextureMapping m;
  GetTextureMapping(m);
  m.Evaluate(point, normal, &uvwOut);

  // Map u coordinate to longitude.
  const double point_horz = uvwOut.x * ON_2PI;

  // Check if decal gets hit.
  double horz_sta = 0.0, horz_end = 0.0;
  GetHorzSweep(horz_sta, horz_end);

  // Longitude relative to decal start longitude.
  const double relative_sta_horz = 0.0;
  double relative_end_horz = PrincipalValueAngleRad(horz_end - horz_sta);
  const double relative_point_horz = PrincipalValueAngleRad(point_horz - horz_sta);

  // If end and start longitudes have same value then decal is supposed to go around the cylinder.
  if (relative_end_horz == 0.0)
    relative_end_horz = ON_2PI;

  if ((relative_sta_horz <= relative_point_horz) &&
      (relative_end_horz >= relative_point_horz))
  {
    // Scale longitude to texture u-coordinate
    uvwOut.x = relative_point_horz / relative_end_horz;
    if (uvwOut.x >= 0.0 && uvwOut.x <= 1.0 && uvwOut.y >= 0.0 && uvwOut.y <= 1.0)
      return true;
  }

  return false;
}

bool ON_Decal::CImpl::HitTestSpherical(const ON_3dVector& normal,
                                       const ON_3dPoint& point, ON_3dPoint& uvwOut) const
{
  // Check that the face normal is pointing to correct direction.
  const double faceNormalDotOriginDirection = ON_DotProduct(normal, point - Origin());

  const bool bInside = faceNormalDotOriginDirection < 0.0;
  const bool bMapToInside = MapToInside();

  if (bInside && !bMapToInside)
    return false;

  if (!bInside && bMapToInside)
    return false;

  // Map world coordinates to uvw.
  ON_TextureMapping m;
  GetTextureMapping(m);
  m.Evaluate(point, normal, &uvwOut);

  double horz_sta = 0.0, horz_end = 0.0;
  GetHorzSweep(horz_sta, horz_end);

  // Map u coordinate to longitude.
  const double point_horz = uvwOut.x * ON_2PI;

  // Longitudes (horz) relative to decal start horz.
  const double relative_horz_sta = 0.0;
  double relative_horz_end = PrincipalValueAngleRad(horz_end - horz_sta);
  const double relative_point_horz = PrincipalValueAngleRad(point_horz - horz_sta);

  // If end and start horz have same value then decal is supposed to go full circle around the vertical axis.
  if (relative_horz_end == 0.0)
    relative_horz_end = ON_2PI;

  double vert_sta = 0.0, vert_end = 0.0;
  GetVertSweep(vert_sta, vert_end);

  const double sta = vert_sta / ON_PI + 0.5;
  const double end = vert_end / ON_PI + 0.5;

  // If the mapped texture coordinate is contained in the decal area we have to do some more calculations.
  if ((relative_horz_sta <= relative_point_horz) &&
      (relative_horz_end >= relative_point_horz) &&
      ((sta <= uvwOut.y) && (end >= uvwOut.y)))
  {
    // These are the texture coordinates inside the decal area.
    const double uDecal = relative_point_horz / relative_horz_end;
    const double vDecal = (uvwOut.y - sta) / (end - sta);

    // Now we should have (uDecal,vDecal) in [0,1]x[0,1].
    // We have to store the correct calculated values of u and v to uvwOut.
    uvwOut.x = uDecal;
    uvwOut.y = vDecal;

    // Decal is hit.
    return true;
  }

  return false;
}

ON_Decal::ON_Decal()
{
  _private = new CImpl;
}

ON_Decal::ON_Decal(ON_XMLNode& node)
{
  ON_ASSERT(node.TagName() == ON_RDK_DECAL);
  _private = new CImpl(nullptr, node);
}

ON_Decal::ON_Decal(const ON_XMLNode& node)
{
  ON_ASSERT(node.TagName() == ON_RDK_DECAL);
  _private = new CImpl(nullptr, node);
}

ON_Decal::ON_Decal(ON_DecalCollection& coll, ON_XMLNode& node)
{
  ON_ASSERT(node.TagName() == ON_RDK_DECAL);
  _private = new CImpl(&coll, node);
}

ON_Decal::ON_Decal(const ON_Decal& other)
{
  _private = new CImpl;
  operator = (other);
}

ON_Decal::ON_Decal(ON_DecalCollection& coll, const ON_Decal& other)
{
  _private = new CImpl;
  _private->_collection = &coll;
  operator = (other);
}

ON_Decal::~ON_Decal()
{
  delete _private;
  _private = nullptr;
}

const ON_Decal& ON_Decal::operator = (const ON_Decal& d)
{
  if (this != &d)
  {
    // Copy the node even if _cache_only is true; otherwise the object won't be made truly equal.
    _private->Node() = d._private->Node();
    _private->_cache = d._private->_cache;
  }

  return *this;
}

bool ON_Decal::operator == (const ON_Decal& d) const
{
  // This only checks if the basic parameters are equal. It ignores any custom data.

  if (TextureInstanceId() != d.TextureInstanceId()) return false;
  if (Mapping()           != d.Mapping())           return false;
  if (Projection()        != d.Projection())        return false;
  if (MapToInside()       != d.MapToInside())       return false;
  if (Transparency()      != d.Transparency())      return false;
  if (Origin()            != d.Origin())            return false;
  if (VectorUp()          != d.VectorUp())          return false;
  if (VectorAcross()      != d.VectorAcross())      return false;
  if (Height()            != d.Height())            return false;
  if (Radius()            != d.Radius())            return false;

  double sta1 = 0.0, end1 = 0.0, sta2 = 0.0, end2 = 0.0;
    GetHorzSweep(sta1, end1);
  d.GetHorzSweep(sta2, end2);
  if (!IsDoubleEqual(sta1, sta2)) return false;
  if (!IsDoubleEqual(end1, end2)) return false;

    GetVertSweep(sta1, end1);
  d.GetVertSweep(sta2, end2);
  if (!IsDoubleEqual(sta1, sta2)) return false;
  if (!IsDoubleEqual(end1, end2)) return false;

  double min_u1 = 0.0, min_v1 = 0.0, max_u1 = 0.0, max_v1 = 0.0;
  double min_u2 = 0.0, min_v2 = 0.0, max_u2 = 0.0, max_v2 = 0.0;
    GetUVBounds(min_u1, min_v1, max_u1, max_v1);
  d.GetUVBounds(min_u2, min_v2, max_u2, max_v2);
  if (!IsDoubleEqual(min_u1, min_u2)) return false;
  if (!IsDoubleEqual(min_v1, min_v2)) return false;
  if (!IsDoubleEqual(max_u1, max_u2)) return false;
  if (!IsDoubleEqual(max_v1, max_v2)) return false;

  return true;
}

bool ON_Decal::operator != (const ON_Decal& d) const
{
  return !(operator == (d));
}

void ON_Decal::SetCacheOnly(void)
{
  _private->_cache_only = true;
}

ON_UUID ON_Decal::TextureInstanceId(void) const
{
  return _private->TextureInstanceId();
}

void ON_Decal::SetTextureInstanceId(const ON_UUID& id)
{
  _private->SetTextureInstanceId(id);
}

ON_Decal::Mappings ON_Decal::Mapping(void) const
{
  return _private->Mapping();
}

void ON_Decal::SetMapping(Mappings m)
{
  _private->SetMapping(m);
}

ON_Decal::Projections ON_Decal::Projection(void) const
{
  return _private->Projection();
}

void ON_Decal::SetProjection(Projections p)
{
  _private->SetProjection(p);
}

bool ON_Decal::MapToInside(void) const
{
  return _private->MapToInside();
}

void ON_Decal::SetMapToInside(bool b)
{
  _private->SetMapToInside(b);
}

double ON_Decal::Transparency(void) const
{
  return _private->Transparency();
}

void ON_Decal::SetTransparency(double d)
{
  _private->SetTransparency(d);
}

ON_3dPoint ON_Decal::Origin(void) const
{
  return _private->Origin();
}

void ON_Decal::SetOrigin(const ON_3dPoint& pt)
{
  _private->SetOrigin(pt);
}

ON_3dVector ON_Decal::VectorUp(void) const
{
  return _private->VectorUp();
}

void ON_Decal::SetVectorUp(const ON_3dVector& vec)
{
  _private->SetVectorUp(vec);
}

ON_3dVector ON_Decal::VectorAcross(void) const
{
  return _private->VectorAcross();
}

void ON_Decal::SetVectorAcross(const ON_3dVector& vec)
{
  _private->SetVectorAcross(vec);
}

double ON_Decal::Height(void) const
{
  return _private->Height();
}

void ON_Decal::SetHeight(double d)
{
  _private->SetHeight(d);
}

double ON_Decal::Radius(void) const
{
  return _private->Radius();
}

void ON_Decal::SetRadius(double d)
{
  _private->SetRadius(d);
}

void ON_Decal::GetHorzSweep(double& sta, double& end) const
{
  _private->GetHorzSweep(sta, end);
}

void ON_Decal::SetHorzSweep(double sta, double end)
{
  _private->SetHorzSweep(sta, end);
}

void ON_Decal::GetVertSweep(double& sta, double& end) const
{
  _private->GetVertSweep(sta, end);
}

void ON_Decal::SetVertSweep(double sta, double end)
{
  _private->SetVertSweep(sta, end);
}

void ON_Decal::GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const
{
  _private->GetUVBounds(min_u, min_v, max_u, max_v);
}

void ON_Decal::SetUVBounds(double min_u, double min_v, double max_u, double max_v)
{
  _private->SetUVBounds(min_u, min_v, max_u, max_v);
}

bool ON_Decal::IsVisible(void) const
{
  return _private->IsVisible();
}

void ON_Decal::SetIsVisible(bool b)
{
  _private->SetIsVisible(b);
}

bool ON_Decal::IsTemporary(void) const
{
  return _private->IsTemporary();
}

void ON_Decal::SetIsTemporary(bool b)
{
  _private->SetIsTemporary(b);
}

ON_UUID ON_Decal::Id(void) const
{
  return _private->Id();
}

// Copied from IRhRdkDecal::GetTextureMapping -- TODO: Refactor. [JOHN-DECAL-FIX]
bool ON_Decal::GetTextureMapping(ON_TextureMapping& mappingOut) const
{
  return _private->GetTextureMapping(mappingOut);
}

ON_DECAL_CRC ON_Decal::DecalCRC(void) const
{
  return ComputeDecalCRC(0, _private->Node());
}

ON__UINT32 ON_Decal::DataCRC(ON__UINT32 current_remainder) const
{
  return ComputeDecalCRC(current_remainder, _private->Node());
}

void ON_Decal::GetCustomXML(const ON_UUID& renderEngineId, ON_XMLNode& custom_param_node) const
{
  custom_param_node.Clear();
  custom_param_node.SetTagName(ON_RDK_DECAL_CUSTOM_PARAMS);

  // Find the node for 'renderEngineId'.
  const ON_XMLNode* custom_node = _private->FindCustomNodeForRenderEngine(renderEngineId);
  if (nullptr != custom_node)
  {
    // Get the parameter node and copy it to 'custom_param_node'.
    const ON_XMLNode* param_node = custom_node->GetNamedChild(ON_RDK_DECAL_CUSTOM_PARAMS);
    if (nullptr != param_node)
    {
      custom_param_node = *param_node;
    }
  }
}

bool ON_Decal::SetCustomXML(const ON_UUID& renderEngineId, const ON_XMLNode& custom_param_node)
{
  if (custom_param_node.TagName() != ON_RDK_DECAL_CUSTOM_PARAMS)
    return false;

  // If there is already a custom node for 'renderEngineId' then delete it.
  ON_XMLNode* custom_node = _private->FindCustomNodeForRenderEngine(renderEngineId);
  if (nullptr != custom_node)
  {
    ON_XMLNode* parent = custom_node->Parent();
    if (nullptr != parent)
    {
      delete parent->DetachChild(*custom_node);
    }
  }

  // Attach the new custom node and set its 'renderer' property to be the render engine id.
  custom_node = _private->Node().AttachChildNode(new ON_XMLNode(ON_RDK_DECAL_CUSTOM));
  ON_XMLProperty prop(ON_RDK_DECAL_CUSTOM_RENDERER, renderEngineId);
  custom_node->SetProperty(prop);

  // Attach a copy of the custom param node to the custom node.
  custom_node->AttachChildNode(new ON_XMLNode(custom_param_node));

  return true;
}

void ON_Decal::GetEntireCustomXML(ON_XMLNode& custom_xml) const
{
  // This function only exists to support the RDK.

  const ON_XMLNode& decal_node = _private->Node();

  auto it = decal_node.GetChildIterator();
  ON_XMLNode* child_node = nullptr;
  while (nullptr != (child_node = it.GetNextChild()))
  {
    if (child_node->TagName() == ON_RDK_DECAL_CUSTOM)
    {
      custom_xml.AttachChildNode(new ON_XMLNode(*child_node));
    }
  }
}

void ON_Decal::AppendCustomXML(const ON_XMLNode& custom_node)
{
  // This function only exists to support the RDK.

  ON_ASSERT(custom_node.TagName() == L"entire-custom-xml");

  ON_XMLNode* child = custom_node.FirstChild();
  while (nullptr != child)
  {
    _private->Node().AttachChildNode(new ON_XMLNode(*child));

    child = custom_node.NextSibling();
  }
}

bool ON_Decal::HitTest(const ON_3dPoint& point, const ON_3dVector& normal, ON_2dPoint& uvOut) const
{
  return _private->HitTest(point, normal, uvOut);
}

void ON_Decal::ApplyTransformation(const ON_Xform& xform)
{
  _private->ApplyTransformation(xform);
}

void ON_Decal::GetSavedVectorLengths(double& across, double& up) const
{
  _private->GetSavedVectorLengths(across, up);
}

void ON_Decal::SetSavedVectorLengths(double across, double up)
{
  _private->SetSavedVectorLengths(across, up);
}

ON_3dVector ON_Decal::GetOriginOffset(void) const
{
  return _private->GetOriginOffset();
}

/* Class ON_DecalNodeReader

   This object encapsulates the reading of all decal properties from XML nodes.
   It is used by the decal CRC calculation in ComputeDecalCRC().

   TODO: It could also be used by the ON_Decal XML node access (for Rhino 9).

*/
class ON_DecalNodeReader
{
public:
  ON_DecalNodeReader(const ON_XMLNode* decal_node);

  ON_XMLVariant Mapping(void) const           { return Value(ON_RDK_DECAL_MAPPING, ON_RDK_DECAL_MAPPING_NONE); }
  ON_XMLVariant Projection(void) const        { return Value(ON_RDK_DECAL_PROJECTION, ON_RDK_DECAL_PROJECTION_NONE); }
  ON_XMLVariant MapToInside(void) const       { return Value(ON_RDK_DECAL_MAP_TO_INSIDE_ON, _def.MapToInside()); } 
  ON_XMLVariant Transparency(void) const      { return Value(ON_RDK_DECAL_TRANSPARENCY    , _def.Transparency()); }
  ON_XMLVariant TextureInstanceId(void) const { return Value(ON_RDK_DECAL_TEXTURE_INSTANCE, _def.TextureInstanceId()); }
  ON_XMLVariant Height(void) const            { return Value(ON_RDK_DECAL_HEIGHT          , _def.Height()); }
  ON_XMLVariant Radius(void) const            { return Value(ON_RDK_DECAL_RADIUS          , _def.Radius()); }
  ON_XMLVariant Origin(void) const            { return Value(ON_RDK_DECAL_ORIGIN          , _def.Origin()); }
  ON_XMLVariant VectorUp(void) const          { return Value(ON_RDK_DECAL_VECTOR_UP       , ON_3dPoint(_def.VectorUp())); }
  ON_XMLVariant VectorAcross(void) const      { return Value(ON_RDK_DECAL_VECTOR_ACROSS   , ON_3dPoint(_def.VectorAcross())); }
  ON_XMLVariant HorzSweepSta(void) const      { return Value(ON_RDK_DECAL_HORZ_SWEEP_STA  , DefaultHorzSweepSta()); }
  ON_XMLVariant HorzSweepEnd(void) const      { return Value(ON_RDK_DECAL_HORZ_SWEEP_END  , DefaultHorzSweepEnd()); }
  ON_XMLVariant VertSweepSta(void) const      { return Value(ON_RDK_DECAL_VERT_SWEEP_STA  , DefaultVertSweepSta()); }
  ON_XMLVariant VertSweepEnd(void) const      { return Value(ON_RDK_DECAL_VERT_SWEEP_END  , DefaultVertSweepEnd()); }
  ON_XMLVariant MinU(void) const              { return Value(ON_RDK_DECAL_MIN_U           , DefaultMinU()); }
  ON_XMLVariant MinV(void) const              { return Value(ON_RDK_DECAL_MIN_V           , DefaultMinV()); }
  ON_XMLVariant MaxU(void) const              { return Value(ON_RDK_DECAL_MAX_U           , DefaultMaxU()); }
  ON_XMLVariant MaxV(void) const              { return Value(ON_RDK_DECAL_MAX_V           , DefaultMaxV()); }
  ON_XMLVariant IsTemporary(void) const       { return Value(ON_RDK_DECAL_IS_TEMPORARY    , false); }
  ON_XMLVariant IsVisible(void) const         { return Value(ON_RDK_DECAL_IS_VISIBLE      , _def.IsVisible()); }
  ON_XMLVariant InstanceId(void) const        { return Value(ON_RDK_DECAL_INSTANCE_ID     , _def.Id()); }

private:
  ON_XMLVariant Value(const wchar_t* wszName, const ON_XMLVariant& vDefault) const;

  double DefaultHorzSweepSta(void) const { double a, b; _def.GetHorzSweep(a, b); return a; }
  double DefaultHorzSweepEnd(void) const { double a, b; _def.GetHorzSweep(a, b); return b; }
  double DefaultVertSweepSta(void) const { double a, b; _def.GetVertSweep(a, b); return a; }
  double DefaultVertSweepEnd(void) const { double a, b; _def.GetVertSweep(a, b); return b; }

  double DefaultMinU(void) const { double a, b, c, d; _def.GetUVBounds(a, b, c, d); return a; }
  double DefaultMinV(void) const { double a, b, c, d; _def.GetUVBounds(a, b, c, d); return b; }
  double DefaultMaxU(void) const { double a, b, c, d; _def.GetUVBounds(a, b, c, d); return c; }
  double DefaultMaxV(void) const { double a, b, c, d; _def.GetUVBounds(a, b, c, d); return d; }

private:
  const ON_XMLNode* _decal_node;
  const ON_Decal _def;
};

ON_DecalNodeReader::ON_DecalNodeReader(const ON_XMLNode* decal_node)
  :
  _decal_node(decal_node)
{
  ON_ASSERT(_decal_node && (_decal_node->TagName() == ON_RDK_DECAL));
}

ON_XMLVariant ON_DecalNodeReader::Value(const wchar_t* wszName, const ON_XMLVariant& vDefault) const
{
  ON_XMLVariant vValue = vDefault;

  if (nullptr != _decal_node)
  {
    const ON_XMLParameters p(*_decal_node);
    p.GetParam(wszName, vValue);
  }

  return vValue;
}

#if (defined _DEBUG) && (defined HUMAN_READABLE_DECAL_CRC)
#define ON_DECAL_PROP_NAME(s) , s
static void DecalUpdateCRC(ON_DECAL_CRC& crc, const ON_XMLVariant value, const wchar_t* name)
{
  crc = value.DataCRC(crc);
  crc._info1 += ON_wString(name) + L"=" + value.AsString() + ON_wString(L" ");
  crc._info2 += ON_wString(name) + L"=" + value.AsString() + ON_wString(L"\n");
}
#else
#define ON_DECAL_PROP_NAME(s)
static void DecalUpdateCRC(ON_DECAL_CRC& crc, const ON_XMLVariant& value)
{
  crc = value.DataCRC(crc);
}
#endif

static void DecalUpdateCRC_Custom(const ON_XMLNode& decal_node, ON_DECAL_CRC& crc)
{
  // Look for custom data nodes and for each one, find the parameter node and then iterate over its
  // children and CRC the properties. For now, we will have to rely on the raw XML. A better solution
  // would be to have the plug-in that created this XML calculate the CRC itself.

  const ON_wString custom = L"[CUSTOM] ";

  auto it = decal_node.GetChildIterator();
  ON_XMLNode* pChildNode = nullptr;
  while (nullptr != (pChildNode = it.GetNextChild()))
  {
    if (pChildNode->TagName() != ON_RDK_DECAL_CUSTOM)
      continue; // Not a custom data node.

    const ON__UINT32 crc_before_custom_params = crc;

    // Find the custom parameter node.
    const ON_XMLNode* pParamNode = pChildNode->GetNamedChild(ON_RDK_DECAL_CUSTOM_PARAMS);
    if (nullptr != pParamNode)
    {
      // Iterate over the nodes inside the custom parameter node.
      const ON_XMLParameters p(*pParamNode);
      auto* iterator = p.NewIterator();
      if (nullptr != iterator)
      {
        ON_wString sParamName;
        ON_XMLVariant vParamValue;
        while (iterator->Next(sParamName, vParamValue))
        {
          DecalUpdateCRC(crc, vParamValue ON_DECAL_PROP_NAME(custom + sParamName));
        }

        delete iterator;
      }

      if (crc != crc_before_custom_params)
      {
        // 20th January 2025 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-71351
        // Since the crc has changed, there must be some custom params, so only now do we include the render
        // engine id. Prior to this, the render engine id was getting included even when there were no custom
        // params. This caused the UI to overlook decals which were programatically created by clients.
        const ON_XMLProperty* prop = pChildNode->GetNamedProperty(ON_RDK_DECAL_CUSTOM_RENDERER);
        if (nullptr != prop)
        {
          // Include the render engine id.
          const ON_UUID uuid = prop->GetValue().AsUuid();
          DecalUpdateCRC(crc, uuid ON_DECAL_PROP_NAME(custom + L"render_engine_id"));
        }
      }
    }
  }
}

ON_DECAL_CRC ON_Decal::ComputeDecalCRC(ON__UINT32 current_remainder, const ON_XMLNode& decal_node) // Static.
{
  // The CRC of a decal is a unique value based on its state. It's created by CRC-ing all the decal properties
  // that affect the decal's appearance. We do not include the 'IsTemporary' property in the CRC because whether
  // or not a decal is temporary has nothing to do with what it looks like. We do however, include the 'IsVisible'
  // property because a decal being visible or invisible actually affects its appearance. Also, the RDK change
  // queue relies on the CRC to update the viewport so including the visibility is critical. Furthermore, the
  // CRC only includes the properties that are relevant for the decal's mapping type.

  ON_DECAL_CRC crc = current_remainder;

  if (decal_node.TagName() == ON_RDK_DECAL)
  {
    const ON_DecalNodeReader d(&decal_node);

    DecalUpdateCRC(crc, d.Mapping()            ON_DECAL_PROP_NAME(L"mapping"));
    DecalUpdateCRC(crc, d.IsVisible()          ON_DECAL_PROP_NAME(L"visible"));
    DecalUpdateCRC(crc, d.Transparency()       ON_DECAL_PROP_NAME(L"transparency"));
    DecalUpdateCRC(crc, d.TextureInstanceId()  ON_DECAL_PROP_NAME(L"texture_id"));

    const ON_Decal::Mappings mapping = MappingFromString(d.Mapping().AsString());

    if (Mappings::UV == mapping)
    {
      DecalUpdateCRC(crc, d.MinU()             ON_DECAL_PROP_NAME(L"min_u"));
      DecalUpdateCRC(crc, d.MinV()             ON_DECAL_PROP_NAME(L"min_v"));
      DecalUpdateCRC(crc, d.MaxU()             ON_DECAL_PROP_NAME(L"max_u"));
      DecalUpdateCRC(crc, d.MaxV()             ON_DECAL_PROP_NAME(L"max_v"));
    }
    else
    {
      DecalUpdateCRC(crc, d.Origin()           ON_DECAL_PROP_NAME(L"origin"));
      DecalUpdateCRC(crc, d.VectorUp()         ON_DECAL_PROP_NAME(L"up"));
      DecalUpdateCRC(crc, d.VectorAcross()     ON_DECAL_PROP_NAME(L"across"));

      if ((Mappings::Cylindrical == mapping) || (Mappings::Spherical == mapping))
      {
        DecalUpdateCRC(crc, d.MapToInside()    ON_DECAL_PROP_NAME(L"map_to_inside"));
        DecalUpdateCRC(crc, d.Radius()         ON_DECAL_PROP_NAME(L"radius"));
        DecalUpdateCRC(crc, d.HorzSweepSta()   ON_DECAL_PROP_NAME(L"horz_sweep_sta"));
        DecalUpdateCRC(crc, d.HorzSweepEnd()   ON_DECAL_PROP_NAME(L"horz_sweep_end"));

        if (Mappings::Cylindrical == mapping)
        {
          DecalUpdateCRC(crc, d.Height()       ON_DECAL_PROP_NAME(L"height"));
        }
        else
        if (Mappings::Spherical == mapping)
        {
          DecalUpdateCRC(crc, d.VertSweepSta() ON_DECAL_PROP_NAME(L"vert_sweep_sta"));
          DecalUpdateCRC(crc, d.VertSweepEnd() ON_DECAL_PROP_NAME(L"vert_sweep_end"));
        }
      }
      else
      if (Mappings::Planar == mapping)
      {
        DecalUpdateCRC(crc, d.Projection()     ON_DECAL_PROP_NAME(L"projection"));
      }
    }

    DecalUpdateCRC_Custom(decal_node, crc);

    // Make sure it's not nil.
    if (crc == ON_NIL_DECAL_CRC)
      crc = 0xFFFFFFFF;
  }

  return crc;
}

// ON_DecalCollection

ON_DecalCollection::ON_DecalCollection(ON_3dmObjectAttributes* attr)
  :
  m_attr(attr)
{
}

ON_DecalCollection::~ON_DecalCollection()
{
  ClearDecalArray();
}

int ON_DecalCollection::FindDecalIndex(const ON_DECAL_CRC decal_crc) const
{
  for (int i = 0; i < m_decals.Count(); i++)
  {
    if (m_decals[i]->DecalCRC() == decal_crc)
      return i;
  }

  return -1;
}

void ON_DecalCollection::ClearDecalArray(void)
{
  // 12th July 2023 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-75697
  // Only call SetChanged() if a decal is actually deleted.
  const int count = m_decals.Count();
  if (count > 0)
  {
    for (int i = 0; i < count; i++)
    {
      delete m_decals[i];
    }

    m_decals.Destroy();

    SetChanged();
  }

  m_cache_valid = false;
}

const ON_DecalCollection& ON_DecalCollection::operator = (const ON_DecalCollection& coll)
{
  ClearDecalArray(); // Critical.

  // 25th February 2025 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-64608
  // The commented out code below is, in my opinion, correct. It copies the incoming collection's decals to
  // this collection, and also copies the 'cache valid' flag. This should result in this collection being
  // identical to the incoming one, which is what the purpose of operator = is. However, it doesn't work and
  // caused my changes for RH-64608 to fail under certain conditions. After hours and hours of debugging I
  // finally ended up carefuly reading the code at the calling site in CRhinoDoc::ModifyObjectAttributes() and
  // found a ton of complex jiggery-pokery involving not copying the user data when one would expect so as not
  // to copy stuff that 'shouldn't be copied' etc., etc. -- optimizations no doubt. It seems that these
  // optimizations stop client code from working as one would expect -- I'm not sure because it's too complex
  // for me to understand, but I'm going to assume the following:
  //
  // TLDR; don't copy the decal array because doing so somehow causes it to get out of sync with the underlying
  // user data (XML). But still clear the array (above) so it gets re-populated the next time it's used.
  //
  //for (int i = 0; i < coll.m_decals.Count(); i++)
  //{
  //  ON_Decal* decal = coll.m_decals[i];
  //  m_decals.Append(new ON_Decal(*this, *decal));
  //}
  //
  //m_cache_valid = coll.m_cache_valid;

  return *this;
}

const ON_SimpleArray<ON_Decal*>& ON_DecalCollection::GetDecalArray(void)
{
  if (!m_cache_valid)
  {
    ClearDecalArray();
    Populate();

    m_cache_valid = true;
  }

  return m_decals;
}

void ON_DecalCollection::Populate(void)
{
  if (nullptr == m_attr)
    return;

  ON_DecalObjectAttributesWrapper w(*m_attr);

  // 25th February 2025 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-64608
  // I'm calling DecalsNodeForWrite() deliberately to circumvent the problem with the user data copy count.
  // See the comments in ON_XMLUserData::XMLRootForWrite(). If I don't do this, the Python use of this code
  // doesn't work. I know, it's a hack, but I'm literally at my wits' end trying to work around these problems.

  const ON_XMLNode* decals_node = w.DecalsNodeForWrite(); // <---- Yes, ForWrite.
  if (nullptr != decals_node)
  {
    // Iterate over the decals under the decals node adding a new decal for each one.
    ON_ASSERT(m_decals.Count() == 0);
    auto it = decals_node->GetChildIterator();
    ON_XMLNode* decal_node = nullptr;
    while (nullptr != (decal_node = it.GetNextChild()))
    {
      ON_Decal* decal = new ON_Decal(*this, *decal_node);
      m_decals.Append(decal);
    }
  }
}

void ON_DecalCollection::SetChanged(void)
{
  m_changed = true;
}

void ON_DecalCollection::InvalidateCache(void)
{
  m_cache_valid = false;
}

// ON_DecalObjectAttributesWrapper

static const wchar_t* decals_path = ON_RDK_USER_DATA_ROOT ON_XML_SLASH ON_RDK_DECALS;

class ON_DecalObjectAttributesWrapperPrivate
{
public:
  ON_DecalObjectAttributesWrapperPrivate(      ON_3dmObjectAttributes& a) : _read_only(false), _attr(a) { }
  ON_DecalObjectAttributesWrapperPrivate(const ON_3dmObjectAttributes& a) : _read_only(true) , _attr(const_cast<ON_3dmObjectAttributes&>(a)) { }

  const ON_RdkUserData* UserDataForRead(void) const;
        ON_RdkUserData* UserDataForWrite(void);

  ON_3dmObjectAttributes& _attr;
  bool _read_only;
};

ON_DecalObjectAttributesWrapper::ON_DecalObjectAttributesWrapper(ON_3dmObjectAttributes& attr)
{
  _private = new ON_DecalObjectAttributesWrapperPrivate(attr);
}

ON_DecalObjectAttributesWrapper::ON_DecalObjectAttributesWrapper(const ON_3dmObjectAttributes& attr)
{
  _private = new ON_DecalObjectAttributesWrapperPrivate(attr);
  _private->_read_only = true;
}

const ON_RdkUserData* ON_DecalObjectAttributesWrapperPrivate::UserDataForRead(void) const
{
  return static_cast<const ON_RdkUserData*>(_attr.GetUserData(ON_RdkUserData::Uuid()));
}

ON_RdkUserData* ON_DecalObjectAttributesWrapperPrivate::UserDataForWrite(void)
{
  if (_read_only)
  {
    ON_ASSERT(false); // Attempt to modify read-only decal user data.
    return nullptr;
  }

  ON_RdkUserData* user_data = static_cast<ON_RdkUserData*>(_attr.GetUserData(ON_RdkUserData::Uuid()));
  if (nullptr == user_data)
  {
    user_data = new ON_RdkUserData;

    if (!_attr.AttachUserData(user_data))
    {
      delete user_data;
      return nullptr;
    }
  }

  return user_data;
}

const ON_XMLNode* ON_DecalObjectAttributesWrapper::DecalsNodeForRead(void) const
{
  const ON_RdkUserData* user_data = _private->UserDataForRead();
  if (nullptr == user_data)
    return nullptr;

  return user_data->XMLRootForRead().GetNodeAtPath(decals_path);
}

ON_XMLNode* ON_DecalObjectAttributesWrapper::DecalsNodeForWrite(void)
{
  // 22nd January 2025 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-67878
  // Per conversation with Dale Lear, this is bad because we are actually going to change the
  // user data while it's already attached to the attributes. We're actually expected to delete
  // the old user data and create new user data with the changes in it, because otherwise Rhino
  // can't know that it was changed. However, Dale suggested I change ON_XMLUserData instead.

  ON_RdkUserData* user_data = _private->UserDataForWrite();
  if (nullptr == user_data)
    return nullptr;

  ON_XMLNode* node = user_data->XMLRootForWrite().CreateNodeAtPath(decals_path);
  if (nullptr != node)
  {
    _private->_attr.UserDataChanged(ON_3dmObjectAttributes::UserDataType::Decals);
  }

  return node;
}

ON_DecalObjectAttributesWrapper::AddDecalResults ON_DecalObjectAttributesWrapper::AddDecal(const ON_Decal& decal_in)
{
  if (_private->_read_only)
    return AddDecalResults::ReadOnly;

  ON_XMLNode* decals_node = DecalsNodeForWrite();
  if (nullptr == decals_node)
    return AddDecalResults::Failure;

  if (nullptr != FindDecalNodeByCRC(*decals_node, decal_in.DecalCRC()))
    return AddDecalResults::Duplicate; // A decal for this decal CRC already exists.

  // Create a new XML node for the new decal.
  ON_XMLNode* decal_node = decals_node->AttachChildNode(new ON_XMLNode(ON_RDK_DECAL));
  if (nullptr == decal_node)
    return AddDecalResults::Failure;

  // Wrap a decal around the new XML node and copy the incoming decal data into it.
  ON_Decal decal(*decal_node);
  decal = decal_in;

  // Move the new XML node to the head of the list so the decal is created on top of any others.
  ON_XMLNode* first_node = decals_node->FirstChild();
  if (nullptr != first_node)
  {
    decal_node->MoveBefore(*first_node);
  }

  return AddDecalResults::Success;
}

bool ON_DecalObjectAttributesWrapper::UpdateDecal(ON_DECAL_CRC decal_crc, const ON_Decal& decal_in)
{
  if (_private->_read_only)
    return false;

  ON_XMLNode* decals_node = DecalsNodeForWrite();
  if (nullptr == decals_node)
    return false;

  ON_XMLNode* decal_node = FindDecalNodeByCRC(*decals_node, decal_crc);
  if (nullptr == decal_node)
    return false;

  // Wrap a decal around the new XML node and copy the incoming decal data into it.
  ON_Decal decal(*decal_node);
  decal = decal_in;

  return true;
}

bool ON_DecalObjectAttributesWrapper::RemoveDecal(ON_DECAL_CRC decal_crc)
{
  if (ON_NIL_DECAL_CRC == decal_crc)
    return false;

  if (_private->_read_only)
    return false;

  ON_XMLNode* decals_node = DecalsNodeForWrite();
  if (nullptr == decals_node)
    return false;

  ON_XMLNode* decal_node = FindDecalNodeByCRC(*decals_node, decal_crc);
  if (nullptr == decal_node)
    return false;

  decal_node->Remove();

  return true;
}

bool ON_DecalObjectAttributesWrapper::RemoveAllDecals(void)
{
  if (_private->_read_only)
    return false;

  ON_XMLNode* decals_node = DecalsNodeForWrite();
  if (nullptr != decals_node)
  {
    decals_node->RemoveAllChildren();
    decals_node->RemoveAllProperties();
  }

  return true;
}

int ON_DecalObjectAttributesWrapper::CountDecalsOnAttributesUserData(void) const
{
  const ON_XMLNode* decals_node = DecalsNodeForRead();
  if (nullptr == decals_node)
    return 0;

  return decals_node->ChildCount();
}

bool ON_DecalObjectAttributesWrapper::CullIdenticalDecals(void)
{
  if (_private->_read_only)
    return false;

  ON_XMLNode* decals_node = DecalsNodeForWrite();
  if (nullptr == decals_node)
    return false;

  auto it = decals_node->GetChildIterator();
  ON_XMLNode* decal_node1 = nullptr;
  while (nullptr != (decal_node1 = it.GetNextChild()))
  {
    const ON_DECAL_CRC decal_crc = ON_DecalCRCFromNode(*decal_node1);

    auto it2 = decals_node->GetChildIterator();
    ON_XMLNode* decal_node2 = nullptr;
    while (nullptr != (decal_node2 = it2.GetNextChild()))
    {
      if (decal_node1 == decal_node2)
        continue;

      if (decal_crc == ON_DecalCRCFromNode(*decal_node2))
      {
        decal_node2->Remove();

        CullIdenticalDecals();

        return true;
      }
    }
  }

  return false;
}

ON_XMLNode* ON_DecalObjectAttributesWrapper::FindDecalNodeByCRC(
            ON_XMLNode& decals_node, ON_DECAL_CRC decal_crc) // Static.
{
  return ::FindDecalNodeByCRC(decals_node, decal_crc);
}

const ON_XMLNode* ON_DecalObjectAttributesWrapper::FindDecalNodeByCRC(
      const ON_XMLNode& decals_node, ON_DECAL_CRC decal_crc) // Static.
{
  return ::FindDecalNodeByCRC(decals_node, decal_crc);
}

void ON_DecalObjectAttributesWrapper::GetDecalCRCs(ON_SimpleArray<ON_DECAL_CRC>& crcs) const
{
  crcs.Destroy();

  const ON_XMLNode* decals_node = DecalsNodeForRead();
  if (nullptr != decals_node)
  {
    auto it = decals_node->GetChildIterator();
    ON_XMLNode* decal_node = nullptr;
    while (nullptr != (decal_node = it.GetNextChild()))
    {
      const ON_DECAL_CRC decal_crc = ON_DecalCRCFromNode(*decal_node);
      ON_ASSERT(ON_NIL_DECAL_CRC != decal_crc);
      crcs.Append(decal_crc);
    }
  }
}

const ON_Decal* ON_DecalObjectAttributesWrapper::NewDecalForRead(ON_DECAL_CRC decal_crc) const
{
  if (ON_NIL_DECAL_CRC == decal_crc)
    return nullptr;

  const ON_XMLNode* decals_node = DecalsNodeForRead();
  if (nullptr == decals_node)
    return nullptr;

  const ON_XMLNode* decal_node = FindDecalNodeByCRC(*decals_node, decal_crc);
  if (nullptr == decal_node)
    return nullptr;

  return new ON_Decal(*decal_node);
}

ON_Decal* ON_DecalObjectAttributesWrapper::NewDecalForWrite(ON_DECAL_CRC decal_crc)
{
  if (_private->_read_only)
    return nullptr;

  if (ON_NIL_DECAL_CRC == decal_crc)
    return nullptr;

  ON_XMLNode* decals_node = DecalsNodeForWrite();
  if (nullptr == decals_node)
    return nullptr;

  ON_XMLNode* decal_node = FindDecalNodeByCRC(*decals_node, decal_crc);
  if (nullptr == decal_node)
    return nullptr;

  return new ON_Decal(*decal_node);
}

bool ON_DecalObjectAttributesWrapper::MoveDecalAfter(ON_DECAL_CRC decal_crc, ON_DECAL_CRC decal_crc_after)
{
  if (_private->_read_only)
    return false;

  ON_XMLNode* decals_node = DecalsNodeForWrite();
  if (nullptr == decals_node)
    return false;

  ON_XMLNode* decal_node_from = FindDecalNodeByCRC(*decals_node, decal_crc);
  if (nullptr == decal_node_from)
    return false;

  // Nil 'decal_crc_after' means move to head of list.
  if (ON_NIL_DECAL_CRC == decal_crc_after)
  {
    ON_XMLNode* first_child = decals_node->FirstChild();
    if (nullptr != first_child)
    {
      decal_node_from->MoveBefore(*first_child);
    }
  }
  else
  {
    ON_XMLNode* decal_node_to = FindDecalNodeByCRC(*decals_node, decal_crc_after);
    if (nullptr != decal_node_to)
    {
      decal_node_from->MoveAfter(*decal_node_to);
    }
  }

  return true;
}

bool ON_DecalObjectAttributesWrapper::RemoveValue(ON_DECAL_CRC decal_crc, const wchar_t* param_name)
{
  if (_private->_read_only)
    return false;

  const auto* decals_node = DecalsNodeForRead();
  if (nullptr == decals_node)
    return false;

  const auto* decal_node = FindDecalNodeByCRC(*decals_node, decal_crc);
  if (nullptr == decal_node)
    return false;

  const auto* param_node = decals_node->GetNamedChild(param_name);
  if (nullptr == param_node)
    return false;

  // Only get node for write once we are certain the node needs to be removed.
  auto* decals_node_write = DecalsNodeForWrite();
  if (nullptr == decals_node_write)
    return false;

  auto* decal_node_write = FindDecalNodeByCRC(*decals_node_write, decal_crc);
  if (nullptr == decal_node)
    return false;

  decal_node_write->GetNamedChild(param_name)->Remove();

  return true;
}
