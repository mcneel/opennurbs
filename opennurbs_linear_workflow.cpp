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

#define ON_RDK_GAMMA                   L"gamma"
#define ON_RDK_USE_POST_PROCESS_GAMMA  L"use-post-process-gamma"
#define ON_RDK_USE_LINEAR_WORKFLOW     L"use-linear-workflow"

class ON_LinearWorkflow::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(ON_XMLNode& n) : ON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath(void)
{
  return ON_RDK_DOCUMENT  ON_RDK_SLASH  ON_RDK_SETTINGS  ON_RDK_SLASH  ON_RDK_RENDERING;
}

ON_LinearWorkflow::ON_LinearWorkflow()
{
  m_impl = new CImpl;
}

ON_LinearWorkflow::ON_LinearWorkflow(ON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

ON_LinearWorkflow::ON_LinearWorkflow(const ON_LinearWorkflow& lw)
{
  m_impl = new CImpl;
  operator = (lw);
}

ON_LinearWorkflow::~ON_LinearWorkflow()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_LinearWorkflow& ON_LinearWorkflow::operator = (const ON_LinearWorkflow& lw)
{
  if (this != &lw)
  {
    SetPreProcessTextures(lw.PreProcessTextures());
    SetPreProcessColors  (lw.PreProcessColors());
    SetPreProcessGamma   (lw.PreProcessGamma());
    SetPostProcessGamma  (lw.PostProcessGamma());
    SetPostProcessGammaOn(lw.PostProcessGammaOn());
  }

  return *this;
}

bool ON_LinearWorkflow::operator == (const ON_LinearWorkflow& lw)
{
  if (PreProcessTextures()     != lw.PreProcessTextures())      return false;
  if (PreProcessColors()       != lw.PreProcessColors())        return false;
  if (PostProcessGammaOn()     != lw.PostProcessGammaOn())      return false;
  if (!IsFloatEqual(PreProcessGamma(),  lw.PreProcessGamma()))  return false;
  if (!IsFloatEqual(PostProcessGamma(), lw.PostProcessGamma())) return false;

  return true;
}

bool ON_LinearWorkflow::operator != (const ON_LinearWorkflow& sf)
{
  return !(operator == (sf));
}

bool ON_LinearWorkflow::PreProcessTextures(void) const
{
	return m_impl->GetParameter(XMLPath(), ON_RDK_USE_LINEAR_WORKFLOW, false);
}

void ON_LinearWorkflow::SetPreProcessTextures(bool b)
{
		m_impl->SetParameter(XMLPath(), ON_RDK_USE_LINEAR_WORKFLOW, b);
}

bool ON_LinearWorkflow::PreProcessColors(void) const
{
	return PreProcessTextures();
}

void ON_LinearWorkflow::SetPreProcessColors(bool b)
{
  return SetPreProcessTextures(b);
}

float ON_LinearWorkflow::PreProcessGamma(void) const
{
  const float f = m_impl->GetParameter(XMLPath(), ON_RDK_GAMMA, 2.2f).AsFloat();
  return std::min(5.0f, std::max(0.2f, f));
}

void ON_LinearWorkflow::SetPreProcessGamma(float gamma)
{
  const float f = std::min(5.0f, std::max(0.2f, gamma));
  m_impl->SetParameter(XMLPath(), ON_RDK_GAMMA, f);
}

bool ON_LinearWorkflow::PostProcessGammaOn(void) const
{
	return m_impl->GetParameter(XMLPath(), ON_RDK_USE_POST_PROCESS_GAMMA, true);
}

void ON_LinearWorkflow::SetPostProcessGammaOn(bool on)
{
	m_impl->SetParameter(XMLPath(), ON_RDK_USE_POST_PROCESS_GAMMA, on);
}

float ON_LinearWorkflow::PostProcessGamma(void) const
{
  return PreProcessGamma();
}

void ON_LinearWorkflow::SetPostProcessGamma(float gamma)
{
  SetPreProcessGamma(gamma);
}

float ON_LinearWorkflow::PostProcessGammaReciprocal(void) const
{
  return 1.0f / PostProcessGamma();
}

void ON_LinearWorkflow::ApplyPreProcessGamma(ON_4fColor& col, bool for_texture) const
{
  const bool check = for_texture ? PreProcessTextures() : PreProcessColors();
  if (!check)
    return;

   const auto gamma = PreProcessGamma();
   if (!IsFloatEqual(gamma, 1.0f))
   {
     auto* f = col.FloatArray();

     ON_ASSERT((f[0] >= 0.0) && (f[1] >= 0.0) && (f[2] >= 0.0));

     if (f[0] > 0.0) f[0] = powf(f[0], gamma);
     if (f[1] > 0.0) f[1] = powf(f[1], gamma);
     if (f[2] > 0.0) f[2] = powf(f[2], gamma);
   }
}

ON__UINT32 ON_LinearWorkflow::DataCRC(ON__UINT32 crc) const
{
  bool b[] = { PreProcessTextures(), PostProcessGammaOn(), PreProcessColors() };
  crc = ON_CRC32(crc, sizeof(b), b);

  ON__INT64 f[] = { Integerize(PreProcessGamma()), Integerize(PostProcessGamma()) };
  crc = ON_CRC32(crc, sizeof(f), f);

  return crc;
}
