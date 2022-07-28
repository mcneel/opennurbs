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

#include "opennurbs.h"
#include "opennurbs_internal_defines.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

// This is inside the ON_RDK_CURRENT_CONTENT section.
#define ON_RDK_BACKGROUND_ENVIRONMENT              L"environment"

// These are inside the ON_RDK_RENDERING section.
#define ON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT_ON    L"custom-env-for-refl-and-refr-on"
#define ON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT       L"custom-env-for-refl-and-refr"

// These are inside the ON_RDK_SUN section.
#define ON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON  L"skylight-custom-environment-on"
#define ON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT     L"skylight-custom-environment"

class ON_CurrentEnvironment::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(ON_XMLNode& model_node, ON_3dmRenderSettings& rs) : m_rs(&rs), ON_InternalXMLImpl(&model_node) { }

  ON_3dmRenderSettings* m_rs = nullptr;
  bool m_on = false; // Only used if m_rs is null.
};

static const wchar_t* XMLPathBack360(void) // Not used for 'on'.
{
  return ON_RDK_DOCUMENT  ON_RDK_SLASH  ON_RDK_CURRENT_CONTENT;
}

static const wchar_t* XMLPathReflRefr(void)
{
  return ON_RDK_DOCUMENT  ON_RDK_SLASH  ON_RDK_SETTINGS  ON_RDK_SLASH  ON_RDK_RENDERING;
}

static const wchar_t* XMLPathSkylight(void)
{
  return ON_RDK_DOCUMENT  ON_RDK_SLASH  ON_RDK_SETTINGS  ON_RDK_SLASH  ON_RDK_SUN;
}

ON_CurrentEnvironment::ON_CurrentEnvironment()
{
  m_impl = new CImpl;
}

ON_CurrentEnvironment::ON_CurrentEnvironment(ON_XMLNode& model_node, ON_3dmRenderSettings& rs)
{
  m_impl = new CImpl(model_node, rs);
}

ON_CurrentEnvironment::ON_CurrentEnvironment(const ON_CurrentEnvironment& ce)
{
  m_impl = new CImpl;
  operator = (ce);
}

ON_CurrentEnvironment::~ON_CurrentEnvironment()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_CurrentEnvironment& ON_CurrentEnvironment::operator = (const ON_CurrentEnvironment& ce)
{
  if (this != &ce)
  {
    SetOn(Usage::Background,  ce.On (Usage::Background));
    SetOn(Usage::Reflection,  ce.On (Usage::Reflection));
    SetOn(Usage::Skylighting, ce.On (Usage::Skylighting));

    Set  (Usage::Background,  ce.Get(Usage::Background));
    Set  (Usage::Reflection,  ce.Get(Usage::Reflection));
    Set  (Usage::Skylighting, ce.Get(Usage::Skylighting));
  }

  return *this;
}

bool ON_CurrentEnvironment::operator == (const ON_CurrentEnvironment& ce)
{
  if (On (Usage::Background)  != ce.On (Usage::Background))  return false;
  if (On (Usage::Reflection)  != ce.On (Usage::Reflection))  return false;
  if (On (Usage::Skylighting) != ce.On (Usage::Skylighting)) return false;

  if (Get(Usage::Background)  != ce.Get(Usage::Background))  return false;
  if (Get(Usage::Reflection)  != ce.Get(Usage::Reflection))  return false;
  if (Get(Usage::Skylighting) != ce.Get(Usage::Skylighting)) return false;

  return true;
}

bool ON_CurrentEnvironment::operator != (const ON_CurrentEnvironment& sf)
{
  return !(operator == (sf));
}

bool ON_CurrentEnvironment::On(Usage usage) const
{
  switch (usage)
  {
  case Usage::Background:
    if (nullptr != m_impl->m_rs)
      return (3 == m_impl->m_rs->m_background_style);
    return m_impl->m_on;

  case Usage::Reflection:
    return m_impl->GetParameter(XMLPathReflRefr(), ON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT_ON, false);

  case Usage::Skylighting:
    return m_impl->GetParameter(XMLPathSkylight(), ON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON, false);
  }

  return false;
}

void ON_CurrentEnvironment::SetOn(Usage usage, bool on)
{
  switch (usage)
  {
  case Usage::Background:
    if (nullptr != m_impl->m_rs) // I'm not sure if this is correct.
      ON_ERROR("Unable to set the 360 env on state. The env is on when render settings m_background_mode is 3.");
    else
     m_impl->m_on = on;
    break;

  case Usage::Reflection:
    m_impl->SetParameter(XMLPathReflRefr(), ON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT_ON, on);
    break;

  case Usage::Skylighting:
    m_impl->SetParameter(XMLPathSkylight(), ON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON, on);
    break;
  }
}

ON_UUID ON_CurrentEnvironment::Get(Usage usage) const
{
  switch (usage)
  {
  case Usage::Background: // This uses old-school XML without the type property.
    return m_impl->GetParameter_NoType(XMLPathBack360(), ON_RDK_BACKGROUND_ENVIRONMENT, L"uuid", ON_nil_uuid);

  case Usage::Reflection:
    return m_impl->GetParameter(XMLPathReflRefr(), ON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT, ON_nil_uuid);

  case Usage::Skylighting:
    return m_impl->GetParameter(XMLPathSkylight(), ON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT, ON_nil_uuid);
  }

  return ON_nil_uuid;
}

void ON_CurrentEnvironment::Set(Usage usage, const ON_UUID& uuid)
{
  switch (usage)
  {
  case Usage::Background: // This uses old-school XML without the type property.
    m_impl->SetParameter_NoType(XMLPathBack360(), ON_RDK_BACKGROUND_ENVIRONMENT, uuid);
    break;

  case Usage::Reflection:
    m_impl->SetParameter(XMLPathReflRefr(), ON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT, uuid);
    break;

  case Usage::Skylighting:
    m_impl->SetParameter(XMLPathSkylight(), ON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT, uuid);
    break;
  }
}
