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

#if !defined(ON_DECALS_INC_)
#define ON_DECALS_INC_

class ON_CLASS ON_Decal final
{
public:
  ON_Decal();
  ON_Decal(ON_XMLNode& model_node);
  ON_Decal(const ON_Decal& d);
  ~ON_Decal();

  const ON_Decal& operator = (const ON_Decal& d);

  bool operator == (const ON_Decal& d);
  bool operator != (const ON_Decal& d);

  enum class Mappings : ON__INT32
  {
    None        = -1,
    Planar      =  0, // Planar mapping. Uses projection, origin, up and across vectors (not unitized).
    Cylindrical =  1, // Cylindrical mapping. Uses origin, up, across, height, radius, latitude start and stop.
    Spherical   =  2, // Spherical mapping. Uses origin, up, across, radius, latitude/longitude start and stop.
    UV          =  3, // UV mapping.
  };

  enum class Projections : ON__INT32
  {
    None     = -1,
    Forward  =  0, // Project forward.
    Backward =  1, // Project backward.
    Both     =  2, // Project forward and backward.
  };

  // Returns the decal texture's instance id.
  ON_UUID TextureInstanceId(void) const;

  // Sets the decal texture's instance id.
  void SetTextureInstanceId(const ON_UUID& id);

  // Returns the decal's mapping.
  Mappings Mapping(void) const;

  // Sets the decal's mapping.
  void SetMapping(Mappings m);

  // Returns the decal's projection. Used only when mapping is planar.
  Projections Projection(void) const;

  // Sets the decal's projection. Used only when mapping is planar.
  void SetProjection(Projections p);

  // Returns true if the texture is mapped to inside of sphere or cylinder, else false.
  // Used only when mapping is cylindrical or spherical.
  bool MapToInside(void) const;

  // Sets if the texture is mapped to inside of sphere or cylinder.
  // Used only when mapping is cylindrical or spherical.
  void SetMapToInside(bool b);

  // Returns the decal's transparency in the range 0..1.
  double Transparency(void) const;

  // Sets the decal's transparency (clamped to the range 0..1).
  void SetTransparency(double d);

  // Returns the origin of the decal in world space. Not used when the mapping is UV.
  ON_3dPoint Origin(void) const;

  // Sets the origin of the decal in world space. Not used when the mapping is UV.
  void SetOrigin(const ON_3dPoint& pt);

  // Returns the 'up' vector of the decal. For planar mapping the length of the vector is relevant.
  // Not used when the mapping is UV. For cylindrical and spherical mapping, the vector is unitized.
  ON_3dVector VectorUp(void) const;

  // Sets the 'up' vector of the decal. Not used when the mapping is UV.
  void SetVectorUp(const ON_3dVector& vec);

  // Returns the 'across' vector of the decal. For planar mapping the length of the vector is relevant.
  // Not used when the mapping is UV.
  // For cylindrical and spherical mapping, the vector is unitized.
  ON_3dVector VectorAcross(void) const;

  // Sets the 'across' vector of the decal. Not used when the mapping is UV.
  void SetVectorAcross(const ON_3dVector& vec);

  // Returns the height of the decal. Only used when mapping is cylindrical.
  double Height(void) const;

  // Sets the height of the decal. Only used when mapping is cylindrical.
  void SetHeight(double d);

  // Returns the radius of the decal. Only used when mapping is cylindrical or spherical.
  double Radius(void) const;

  // Sets the radius of the decal. Only used when mapping is cylindrical or spherical.
  void SetRadius(double d);

  // Gets the start and end angles of the decal's 'horizontal sweep' (these are angles of longitude in radians).
  // Only used when mapping is cylindrical or spherical.
  void GetHorzSweep(double& sta, double& end) const;

  // Sets the start and end angles of the decal's 'horizontal sweep' (these are angles of longitude in radians).
  // Only used when mapping is cylindrical or spherical.
  void SetHorzSweep(double sta, double end);

  // Gets the start and end angles of the decal's 'vertical sweep' (these are angles of latitude in radians).
  // Only used when mapping is spherical.
  void GetVertSweep(double& sta, double& end) const;

  // Sets the start and end angles of the decal's 'vertical sweep' (these are angles of latitude in radians).
  // Only used when mapping is spherical.
  void SetVertSweep(double sta, double end);

  // Returns the UV bounds of the decal. Only used when mapping is UV.
  void UVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const;

  // Sets the UV bounds of the decal. Only used when mapping is UV.
  void SetUVBounds(double min_u, double min_v, double max_u, double max_v);

  // Returns the CRC of the decal.
  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  // Returns the unique id of the decal. This is a run-time id that is not persistent and is
  // only used for looking decals up in the model.
  ON_UUID Id(void) const;

private:
  class CImpl;
  CImpl* m_impl;
  friend class ON_DecalIterator;
};

class ON_CLASS ON_DecalIterator final
{
public:
  ON_DecalIterator(class ONX_Model& model, const ON_ModelComponent& component);
  ~ON_DecalIterator();

  // Returns a pointer to the next decal or null if there are no more. The returned pointer
  // points to an object inside the ONX_Model. You should not store this pointer.
  ON_Decal* Next(void) const;

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
