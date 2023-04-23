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
    SetOn(sl.On());
    SetShadowIntensity(sl.ShadowIntensity());
  }

  return *this;
}

bool ON_Skylight::operator == (const ON_Skylight& sl) const
{
  if (On()              != sl.On())              return false;
  if (ShadowIntensity() != sl.ShadowIntensity()) return false;

  return true;
}

bool ON_Skylight::operator != (const ON_Skylight& sl) const
{
  return !(operator == (sl));
}

bool ON_Skylight::On(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_ON, false).AsBool();
}

void ON_Skylight::SetOn(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_ON, b);
}

double ON_Skylight::ShadowIntensity(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_SHADOW_INTENSITY, 1.0).AsDouble();
}

void ON_Skylight::SetShadowIntensity(double d)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_SHADOW_INTENSITY, d);
}

void* ON_Skylight::EVF(const wchar_t*, void*)
{
  return nullptr;
}

bool ON_Skylight::EnvironmentOverride(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_ENVIRONMENT_OVERRIDE, false).AsBool();
}

void ON_Skylight::SetEnvironmentOverride(bool on)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_ENVIRONMENT_OVERRIDE, on);
}

ON_UUID ON_Skylight::EnvironmentId(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_ENVIRONMENT_ID, ON_nil_uuid).AsUuid();
}

void ON_Skylight::SetEnvironmentId(const ON_UUID& id)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_SUN_SKYLIGHT_ENVIRONMENT_ID, id);
}

void ON_Skylight::InvalidateCache(void)
{
}
