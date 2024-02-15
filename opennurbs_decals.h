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

class ON_CLASS ON_Decal
{
public:
  ON_Decal();
  ON_Decal(class ON_DecalCollection& coll, ON_XMLNode& node);
  ON_Decal(const ON_Decal& d);
  virtual ~ON_Decal();

  virtual const ON_Decal& operator = (const ON_Decal& d);

  virtual bool operator == (const ON_Decal& d) const;
  virtual bool operator != (const ON_Decal& d) const;

  enum class Mappings : ON__INT32
  {
    None        = -1,
    Planar      =  0, // Planar mapping. Uses projection, origin, up and across vectors (not unitized).
    Cylindrical =  1, // Cylindrical mapping. Uses origin, up, across, height, radius, horz-sweep.
    Spherical   =  2, // Spherical mapping. Uses origin, up, across, radius, horz-sweep, vert-sweep.
    UV          =  3, // UV mapping. Uses UV bounds.
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
  void GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const;

  // Sets the UV bounds of the decal. Only used when mapping is UV.
  void SetUVBounds(double min_u, double min_v, double max_u, double max_v);

  // Gets a texture mapping based on the properties of this decal. Only works and returns true if
  // the decal mapping is Planar, Spherical or Cylindrical. Otherwise returns false.
  bool GetTextureMapping(ON_TextureMapping& tm) const;

  // Returns the Decal CRC of the decal.
  ON__UINT32 DecalCRC(void) const;

  // Returns the Data CRC of the decal. This is not necessarily the same as the decal CRC
  // because it allows a starting current remainder.
  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  // Returns true if the decal is visible in the rendering.
  bool IsVisible(void) const;

  // Sets whether or not the decal is visible in the rendering.
  void SetIsVisible(bool visible);

  // Returns the unique id of the decal. This is a run-time id that is not persistent and is
  // only used for looking decals up in the model.
  ON_UUID Id(void) const;

  // Get the custom XML for the specified render engine. The format of the XML is described below.
  void GetCustomXML(const ON_UUID& renderEngineId, ON_XMLNode& custom_param_node) const;

  // Set the custom XML for the specified render engine. This XML should have the following format:
  //
  //  <parameters>
  //    <param-name type="type"></param-name>
  //    ...
  //  </parameters>
  //
  // Therefore 'custom_param_node' must have a tag name of "<parameters>". The easiest way to produce
  // such XML is by using ON_XMLParameters.
  bool SetCustomXML(const ON_UUID& renderEngineId, const ON_XMLNode& custom_param_node);

public: // For internal use only.
  static ON__UINT32 ComputeDecalCRC(ON__UINT32, const ON_XMLNode&);

private:
  class CImpl;
  CImpl* _impl;
};

// For internal use only.
ON__UINT32 ON_DECL ON_DecalCRCFromNode(const ON_XMLNode& node);

#endif
