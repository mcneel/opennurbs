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

#if !defined(ON_SKYLIGHT_INC_)
#define ON_SKYLIGHT_INC_

class ON_CLASS ON_Skylight final
{
public:
  ON_Skylight();
  ON_Skylight(ON_XMLNode& model_node);
  ON_Skylight(const ON_Skylight& sl);
  ~ON_Skylight();

  const ON_Skylight& operator = (const ON_Skylight& sl);

  bool operator == (const ON_Skylight& sl);
  bool operator != (const ON_Skylight& sl);

  // Returns true if the skylight is enabled, else false.
  bool On(void) const;

  // Set the skylight enabled state.
  void SetOn(bool b);

  // Returns true if the skylight custom environment is on, else false.
  bool CustomEnvironmentOn(void) const;

  // Returns the skylight custom environment instance id. If none has been set, returns ON_nil_uuid.
  ON_UUID CustomEnvironment(void) const;

  // Set the skylight custom environment on or off.
  void SetCustomEnvironmentOn(bool on);

  // Set the skylight custom environment instance id.
  void SetCustomEnvironment(const ON_UUID& uuid);

  // Returns the skylight shadow intensity. This is unused at present.
  double ShadowIntensity(void) const;

  // Set the skylight shadow intensity. This is unused at present.
  void SetShadowIntensity(double d);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
