/*
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
*/

#if !defined(ON_CURRENT_ENVIRONMENT_INC_)
#define ON_CURRENT_ENVIRONMENT_INC_

class ON_CLASS ON_CurrentEnvironment final
{
public:
  ON_CurrentEnvironment();
  ON_CurrentEnvironment(ON_XMLNode& model_node, ON_3dmRenderSettings& rs);
  ON_CurrentEnvironment(const ON_CurrentEnvironment& ce);
  ~ON_CurrentEnvironment();

  const ON_CurrentEnvironment& operator = (const ON_CurrentEnvironment& lw);

  bool operator == (const ON_CurrentEnvironment& lw);
  bool operator != (const ON_CurrentEnvironment& lw);

  enum class Usage : unsigned int
  {
    Background,  // Specifies the background environment.
    Reflection,  // Specifies the custom reflective environment. Also used for refraction.
    Skylighting, // Specifies the custom skylighting environment.
  };

  // Get if the current environment is 'on' for a particular usage.
  bool On(Usage usage) const;

  // Set if the current environment is 'on' for a particular usage.
  void SetOn(Usage usage, bool bOn);

  // Get the current environment instance id. If there is no current environment,
  // the function returns the default environment's instance id. */
  ON_UUID Get(Usage usage) const;

  // Set the current environment instance id for a specific usage. Note that passing ON_nil_uuid
  // for a usage will remove that specialization.
  void Set(Usage usage, const ON_UUID& uuidInstance);

public:
  class CImpl;
  CImpl* m_impl;
};

#endif
