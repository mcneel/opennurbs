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

// These settings are inside the ON_RDK_RENDERING section.

#define ON_USE_DITHERING                L"use-dithering"
#define ON_DITHERING_METHOD             L"dithering"
#define    ON_DITHERING_FLOYD_STEINBERG   L"floyd-steinberg"
#define    ON_DITHERING_SIMPLE_NOISE      L"simple-noise"

class ON_Dithering::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(ON_XMLNode& n) : ON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPathDit(void)
{
  return ON_RDK_DOCUMENT  ON_RDK_SLASH  ON_RDK_SETTINGS  ON_RDK_SLASH  ON_RDK_RENDERING;
}

ON_Dithering::ON_Dithering()
{
  m_impl = new CImpl;
}

ON_Dithering::ON_Dithering(ON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

ON_Dithering::ON_Dithering(const ON_Dithering& dit)
{
  m_impl = new CImpl;
  operator = (dit);
}

ON_Dithering::~ON_Dithering()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_Dithering& ON_Dithering::operator = (const ON_Dithering& dit)
{
  if (this != &dit)
  {
    SetOn    (dit.On());
    SetMethod(dit.Method());
  }

  return *this;
}

bool ON_Dithering::operator == (const ON_Dithering& dit)
{
  if (On()     != dit.On())     return false;
  if (Method() != dit.Method()) return false;

  return true;
}

bool ON_Dithering::operator != (const ON_Dithering& dit)
{
  return !(operator == (dit));
}

bool ON_Dithering::On(void) const
{
  return m_impl->GetParameter(XMLPathDit(), ON_USE_DITHERING, false);
}

void ON_Dithering::SetOn(bool b)
{
  m_impl->SetParameter(XMLPathDit(), ON_USE_DITHERING, b);
}

ON_Dithering::Methods ON_Dithering::Method(void) const
{
  const ON_wString s = m_impl->GetParameter(XMLPathDit(), ON_DITHERING_METHOD, L"").AsString();
  if (ON_DITHERING_FLOYD_STEINBERG == s)
    return Methods::FloydSteinberg;

  return Methods::SimpleNoise;
}

void ON_Dithering::SetMethod(Methods m)
{
  const wchar_t* wsz = ON_DITHERING_SIMPLE_NOISE;
  if (Methods::FloydSteinberg == m)
    wsz = ON_DITHERING_FLOYD_STEINBERG;

  m_impl->SetParameter(XMLPathDit(), ON_DITHERING_METHOD, wsz);
}

ON__UINT32 ON_Dithering::DataCRC(ON__UINT32 crc) const
{
  const bool b = On();
  crc = ON_CRC32(crc, sizeof(b), &b);

  const auto m = Method();
  crc = ON_CRC32(crc, sizeof(m), &m);

  return crc;
}
