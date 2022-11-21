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

#if !defined(ON_GROUND_PLANE_INC_)
#define ON_GROUND_PLANE_INC_

class ON_CLASS ON_GroundPlane final
{
public:
  ON_GroundPlane();
  ON_GroundPlane(ON_XMLNode& model_node);
  ON_GroundPlane(const ON_GroundPlane& gp);
  ~ON_GroundPlane();

  const ON_GroundPlane& operator = (const ON_GroundPlane& gp);

  bool operator == (const ON_GroundPlane& gp);
  bool operator != (const ON_GroundPlane& gp);

  // Returns true if the ground plane is enabled, else false.
  bool On(void) const;

  // Returns true if ground plane backface is enabled, else false.
  bool ShowUnderside(void) const;

  // Returns the altitude of the ground plane.
  double Altitude(void) const;

  // Returns true if auto-altitude is enabled.
  bool AutoAltitude(void) const;

  // Returns true if the ground plane is set to shadow-only.
  bool ShadowOnly(void) const;

  // Returns the instance id of the ground plane's material.
  ON_UUID MaterialInstanceId(void) const;

  // Returns the texture offset of the ground plane in model units.
  ON_2dVector TextureOffset(void) const;

  // Returns true if the texture offset x and y are locked together.
  bool TextureOffsetLocked(void) const;

  // Returns true if the texture repeat x and y are locked together.
  bool TextureRepeatLocked(void) const;

  // Returns the texture size of the ground plane in model units.
  ON_2dVector TextureSize(void) const;

  // Returns the texture rotation of the ground plane in degrees.
  double TextureRotation(void) const;

  // Set the ground plane enabled state.
  void SetOn(bool b);

  // Set if the ground plane backface is enabled.
  void SetShowUnderside(bool b);

  // Set the ground plane's altitude.
  void SetAltitude(double d);

  // Set if the ground plane is set to auto-altitude.
  void SetAutoAltitude(bool b);

  // Set if the ground plane is set to shadow-only.
  void SetShadowOnly(bool b);

  // Set the instance id of the ground plane's material.
  void SetMaterialInstanceId(const ON_UUID& uuid);

  // Set the texture offset of the ground plane in model units.
  void SetTextureOffset(const ON_2dVector& vec);

  // Set if the texture offset x and y are locked together.
  void SetTextureOffsetLocked(bool b);

  // Set if the texture repeat x and y are locked together.
  void SetTextureRepeatLocked(bool b);

  // Set the texture size of the ground plane in model units.
  void SetTextureSize(const ON_2dVector& vec);

  // Set the texture rotation of the ground plane in degrees.
  void SetTextureRotation(double d);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
