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

#define ON_RDK_SKYLIGHT_ON                      L"skylight-on"
#define ON_RDK_SKYLIGHT_SHADOW_INTENSITY        L"skylight-shadow-intensity"
#define ON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT_ON   L"skylight-custom-environment-on"
#define ON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT      L"skylight-custom-environment"

class ON_Skylight::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(ON_XMLNode& n) : ON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath(void)
{
  return ON_RDK_DOCUMENT  ON_RDK_SLASH  ON_RDK_SETTINGS  ON_RDK_SLASH  ON_RDK_SUN;
}

ON_Skylight::ON_Skylight()
{
  m_impl = new CImpl;
}

ON_Skylight::ON_Skylight(ON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

ON_Skylight::ON_Skylight(const ON_Skylight& sl)
{
  m_impl = new CImpl;
  operator = (sl);
}

ON_Skylight::~ON_Skylight()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_Skylight& ON_Skylight::operator = (const ON_Skylight& sl)
{
  if (this != &sl)
  {
    SetOn                 (sl.On());
    SetCustomEnvironmentOn(sl.CustomEnvironmentOn());
    SetCustomEnvironment  (sl.CustomEnvironment());
    SetShadowIntensity    (sl.ShadowIntensity());
  }

  return *this;
}

bool ON_Skylight::operator == (const ON_Skylight& sl)
{
  if (On()                  != sl.On()                 ) return false;
  if (CustomEnvironmentOn() != sl.CustomEnvironmentOn()) return false;
  if (CustomEnvironment()   != sl.CustomEnvironment()  ) return false;
  if (ShadowIntensity()     != sl.ShadowIntensity()    ) return false;

  return true;
}

bool ON_Skylight::operator != (const ON_Skylight& gp)
{
  return !(operator == (gp));
}

bool ON_Skylight::On(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SKYLIGHT_ON, false).AsBool();
}

void ON_Skylight::SetOn(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SKYLIGHT_ON, b);
}

bool ON_Skylight::CustomEnvironmentOn(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT_ON, false).AsBool();
}

void ON_Skylight::SetCustomEnvironmentOn(bool on)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT_ON, on);
}

ON_UUID ON_Skylight::CustomEnvironment(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT, ON_nil_uuid).AsUuid();
}

void ON_Skylight::SetCustomEnvironment(const ON_UUID& uuid)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT, uuid);
}

double ON_Skylight::ShadowIntensity(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SKYLIGHT_SHADOW_INTENSITY, 1.0).AsDouble();
}

void ON_Skylight::SetShadowIntensity(double d)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SKYLIGHT_SHADOW_INTENSITY, d);
}
