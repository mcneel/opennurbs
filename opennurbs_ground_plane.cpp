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

#define ON_RDK_GROUND_PLANE           L"ground-plane"
#define    ON_RDK_GP_ON                 L"on"
#define    ON_RDK_GP_ALTITUDE           L"altitude"
#define    ON_RDK_GP_MATERIAL           L"material"
#define    ON_RDK_GP_TEXTURE_OFFSET     L"texture-offset"
#define    ON_RDK_GP_TEXTURE_SIZE       L"texture-size"
#define    ON_RDK_GP_TEXTURE_ROTATION   L"texture-rotation"
#define    ON_RDK_GP_OFFSET_LOCK        L"offset-lock"
#define    ON_RDK_GP_REPEAT_LOCK        L"repeat-lock"
#define    ON_RDK_GP_SHOW_UNDERSIDE     L"show-underside"
#define    ON_RDK_GP_AUTO_ALTITUDE      L"auto-altitude"
#define    ON_RDK_GP_SHADOW_ONLY        L"shadow-only"

class ON_GroundPlane::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(ON_XMLNode& n) : ON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath(void)
{
  return ON_RDK_DOCUMENT  ON_RDK_SLASH  ON_RDK_SETTINGS  ON_RDK_SLASH  ON_RDK_GROUND_PLANE;
}

ON_GroundPlane::ON_GroundPlane()
{
  m_impl = new CImpl;
}

ON_GroundPlane::ON_GroundPlane(ON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

ON_GroundPlane::ON_GroundPlane(const ON_GroundPlane& gp)
{
  m_impl = new CImpl;
  operator = (gp);
}

ON_GroundPlane::~ON_GroundPlane()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_GroundPlane& ON_GroundPlane::operator = (const ON_GroundPlane& gp)
{
  if (this != &gp)
  {
    SetOn                 (gp.On());
    SetShowUnderside      (gp.ShowUnderside());
    SetAutoAltitude       (gp.AutoAltitude());
    SetShadowOnly         (gp.ShadowOnly());
    SetMaterialInstanceId (gp.MaterialInstanceId());
    SetTextureOffset      (gp.TextureOffset());
    SetTextureOffsetLocked(gp.TextureOffsetLocked());
    SetTextureRepeatLocked(gp.TextureRepeatLocked());
    SetTextureSize        (gp.TextureSize());
    SetTextureRotation    (gp.TextureRotation());
    SetAltitude           (gp.Altitude());
  }

  return *this;
}

bool ON_GroundPlane::operator == (const ON_GroundPlane& gp)
{
  if (On()                  != gp.On()                 ) return false;
  if (ShowUnderside()       != gp.ShowUnderside()      ) return false;
  if (AutoAltitude()        != gp.AutoAltitude()       ) return false;
  if (ShadowOnly()          != gp.ShadowOnly()         ) return false;
  if (MaterialInstanceId()  != gp.MaterialInstanceId() ) return false;
  if (TextureOffset()       != gp.TextureOffset()      ) return false;
  if (TextureOffsetLocked() != gp.TextureOffsetLocked()) return false;
  if (TextureRepeatLocked() != gp.TextureRepeatLocked()) return false;
  if (TextureSize()         != gp.TextureSize()        ) return false;

  if (!IsDoubleEqual(Altitude(),        gp.Altitude()))        return false;
  if (!IsDoubleEqual(TextureRotation(), gp.TextureRotation())) return false;

  return true;
}

bool ON_GroundPlane::operator != (const ON_GroundPlane& gp)
{
  return !(operator == (gp));
}

bool ON_GroundPlane::On(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_ON, false).AsBool();
}

bool ON_GroundPlane::ShowUnderside(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_SHOW_UNDERSIDE, false).AsBool();
}

double ON_GroundPlane::Altitude(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_ALTITUDE, 0.0).AsDouble();
}

bool ON_GroundPlane::AutoAltitude(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_AUTO_ALTITUDE, false).AsBool();
}

bool ON_GroundPlane::ShadowOnly(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_SHADOW_ONLY, true).AsBool();
}

ON_UUID ON_GroundPlane::MaterialInstanceId(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_MATERIAL, ON_nil_uuid).AsUuid();
}

ON_2dVector ON_GroundPlane::TextureOffset(void) const
{
  return ON_2dVector(m_impl->GetParameter(XMLPath(), ON_RDK_GP_TEXTURE_OFFSET, ON_nil_uuid).As2dPoint());
}

bool ON_GroundPlane::TextureOffsetLocked(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_OFFSET_LOCK, false).AsBool();
}

bool ON_GroundPlane::TextureRepeatLocked(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_REPEAT_LOCK, false).AsBool();
}

ON_2dVector ON_GroundPlane::TextureSize(void) const
{
  return ON_2dVector(m_impl->GetParameter(XMLPath(), ON_RDK_GP_TEXTURE_SIZE, ON_nil_uuid).As2dPoint());
}

double ON_GroundPlane::TextureRotation(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_GP_TEXTURE_ROTATION, false).AsDouble();
}

void ON_GroundPlane::SetOn(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_ON, b);
}

void ON_GroundPlane::SetShowUnderside(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_SHOW_UNDERSIDE, b);
}

void ON_GroundPlane::SetAltitude(double d)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_ALTITUDE, d);
}

void ON_GroundPlane::SetAutoAltitude(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_AUTO_ALTITUDE, b);
}

void ON_GroundPlane::SetShadowOnly(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_SHADOW_ONLY, b);
}

void ON_GroundPlane::SetMaterialInstanceId(const ON_UUID& u)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_MATERIAL, u);
}

void ON_GroundPlane::SetTextureOffset(const ON_2dVector& v)
{
  const ON_2dPoint p = v;
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_TEXTURE_OFFSET, p);
}

void ON_GroundPlane::SetTextureOffsetLocked(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_OFFSET_LOCK, b);
}

void ON_GroundPlane::SetTextureRepeatLocked(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_REPEAT_LOCK, b);
}

void ON_GroundPlane::SetTextureSize(const ON_2dVector& v)
{
  const ON_2dPoint p = v;
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_TEXTURE_SIZE, p);
}

void ON_GroundPlane::SetTextureRotation(double d)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_GP_TEXTURE_ROTATION, d);
}
