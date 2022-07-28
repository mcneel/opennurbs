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

#if !defined(ON_POST_EFFECTS_INC_)
#define ON_POST_EFFECTS_INC_

class ON_CLASS ON_PostEffect : public ON_ModelComponent
{
  ON_OBJECT_DECLARE(ON_PostEffect);

public:
  enum class Types : int
  {
    Unset,
    Early,
    ToneMapping,
    Late
  };

  ON_PostEffect();
  ON_PostEffect(const ON_XMLNode& node, Types type);
  ON_PostEffect(const ON_PostEffect& pep);
  virtual ~ON_PostEffect();

  const ON_PostEffect& operator = (const ON_PostEffect& pep);

  bool operator == (const ON_PostEffect& pep);
  bool operator != (const ON_PostEffect& pep);

  // Returns the type of this post effect. 
  Types Type(void) const;

  // Returns the localized name of this post effect.
  ON_wString LocalName(void) const;

  // Returns true if the post effect is visible to the user. For early and late post effects, this is equivalent to 'shown'.
  // For tone-mapping post effects, this is equivalent to 'selected'. 
  bool IsVisible(void) const;

  // Returns true if the post effect is active. For early and late post effects, this is equivalent to 'shown' and 'on'.
  // For tone-mapping post effects, this is equivalent to 'selected'. 
  bool IsActive(void) const;

  // Get a parameter.
  // - param_name is the name of the parameter to set.
  // Returns the value if successful or null if the parameter was not found.
  ON_XMLVariant GetParameter(const wchar_t* param_name) const;

  // Set a parameter.
  // - param_name is the name of the parameter to set.
  // - param_value specifies the type and value to set.
  // Returns true if successful or false if the parameter could not be set.
  bool SetParameter(const wchar_t* param_name, const ON_XMLVariant& param_value);

  // Returns the XML node that stores the state of this post effect.
  const ON_XMLNode& XML(void) const;

  // Returns a CRC of the state of this post effect.
  virtual ON__UINT32 DataCRC(ON__UINT32 current_remainder) const override;

  // If ON_PostEffect::Cast(ref.ModelComponent()) is not null,
  // that pointer is returned. Otherwise, none_return_value is returned.
  static const ON_PostEffect* FromModelComponentRef(const ON_ModelComponentReference& ref,
                                                    const ON_PostEffect* none_return_value);

public:
  class CImpl;
  CImpl* m_impl;
};

#endif
