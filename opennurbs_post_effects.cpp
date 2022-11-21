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

#define ON_RDK_PEP_PLUG_IN  L"plug-in"
#define ON_RDK_PEP_NAME     L"name"
#define ON_RDK_PEP_ID       L"id"
#define ON_RDK_PEP_SHOWN    L"shown"
#define ON_RDK_PEP_ON       L"on"
#define ON_RDK_PEP_STATE    L"state"

// ON_PostEffect
//
// TODO:
// NOTE TO JOHN: On writing this I see I have made a terrible assumption that the model will only
// be loaded and saved once! It will need to be changed so that the nodes are not stolen out of the
// document, but they are copied.

ON_XMLNode* FindPostEffectNodeForId(const ON_XMLNode& pep_section_node, const ON_UUID& id)
{
  auto it = pep_section_node.GetChildIterator();

  auto* pep_node = it.GetNextChild();
  while (nullptr != pep_node)
  {
    const auto* prop = pep_node->GetNamedProperty(ON_RDK_PEP_ID);
    if (nullptr != prop)
    {
      if (prop->GetValue().AsUuid() == id)
        return pep_node;
    }

    pep_node = it.GetNextChild();
  }

  return nullptr;
}

class ON_PostEffect::CImpl
{
public:
  CImpl(const ON_XMLNode& n, Types t) : m_node(n), m_type(t) { }

  ON_XMLVariant GetPropertyValue(const wchar_t* name) const;

public:
  ONX_Model* m_model = nullptr;
  ON_XMLNode m_node;
  ON_PostEffect::Types m_type = ON_PostEffect::Types::Early;
};

ON_XMLVariant ON_PostEffect::CImpl::GetPropertyValue(const wchar_t* name) const
{
  ON_XMLVariant v;

  const auto* pProp = m_node.GetNamedProperty(name);
  if (nullptr != pProp)
  {
    v = pProp->GetValue();
  }

  return v;
}

void SetModel(const ON_PostEffect& pep, ONX_Model& m)
{
  pep.m_impl->m_model = &m;
}

ON_OBJECT_IMPLEMENT(ON_PostEffect, ON_ModelComponent, "A7755211-7C60-4C46-8705-1C91151C9CD9");

ON_PostEffect::ON_PostEffect()
  :
  ON_ModelComponent(ON_ModelComponent::Type::PostEffect)
{
  m_impl = new CImpl(nullptr, Types::Unset);
}

ON_PostEffect::ON_PostEffect(const ON_XMLNode& node, Types type)
  :
  ON_ModelComponent(ON_ModelComponent::Type::PostEffect)
{
  m_impl = new CImpl(node, type);

  // Copy the name to the component name.
  SetName(m_impl->GetPropertyValue(ON_RDK_PEP_NAME).AsString());

  // Copy the XML instance id to the component id. This is the unique id of the PEP and as such
  // it's not supposed to be changed. There is a lock flag in the ON_ModelComponent but how to use
  // it is an impenetrable mystery to me. Anyway, if the user does set the id, it will be ignored
  // because we don't copy it back to the XML.
  const auto uuid = m_impl->GetPropertyValue(ON_RDK_PEP_ID).AsUuid();
  SetId(uuid);
}

ON_PostEffect::ON_PostEffect(const ON_PostEffect& pep)
  :
  ON_ModelComponent(ON_ModelComponent::Type::PostEffect, pep)
{
  m_impl = new CImpl(pep.m_impl->m_node, pep.Type());
  operator = (pep);
}

ON_PostEffect::~ON_PostEffect()
{
  delete m_impl;
  m_impl = nullptr;
}

ON_PostEffect::Types ON_PostEffect::Type(void) const
{
  return m_impl->m_type;
}

const ON_PostEffect& ON_PostEffect::operator = (const ON_PostEffect& pep)
{
  ON_ModelComponent::operator = (pep);

  m_impl->m_type = pep.m_impl->m_type;
  m_impl->m_node = pep.m_impl->m_node;

  return *this;
}

ON_wString ON_PostEffect::LocalName(void) const
{
  return m_impl->GetPropertyValue(ON_RDK_PEP_NAME);
}

bool ON_PostEffect::IsVisible(void) const
{
  return m_impl->GetPropertyValue(ON_RDK_PEP_ON);
}

bool ON_PostEffect::IsActive(void) const
{
  return m_impl->GetPropertyValue(ON_RDK_PEP_SHOWN);
}

ON_XMLVariant ON_PostEffect::GetParameter(const wchar_t* param_name) const
{
  ON_XMLVariant value;
  value.SetNull();

  auto* node = m_impl->m_node.GetNodeAtPath(ON_RDK_PEP_STATE);
  if (nullptr != node)
  {
    ON_XMLParameters p(*node);
    p.GetParam(param_name, value);
  }

  return value;
}

bool ON_PostEffect::SetParameter(const wchar_t* param_name, const ON_XMLVariant& param_value)
{
  auto* node = m_impl->m_node.GetNodeAtPath(ON_RDK_PEP_STATE);
  if (nullptr == node)
    return false;

  ON_XMLParameters p(*node);

  ON_XMLVariant current_value;
  if (!p.GetParam(param_name, current_value))
    return false; // If you can't get it, you can't set it.

  if (!p.SetParam(param_name, param_value))
    return false;

  return true;
}

const ON_XMLNode& ON_PostEffect::XML(void) const
{
  return m_impl->m_node;
}

ON__UINT32 ON_PostEffect::DataCRC(ON__UINT32 crc) const
{
  crc = ON_ModelComponent::DataCRC(crc);

  return m_impl->m_node.DataCRC(crc);
}

const ON_PostEffect* ON_PostEffect::FromModelComponentRef(const ON_ModelComponentReference& ref,
                                                          const ON_PostEffect* none_return_value) // Static.
{
  const auto* pep = ON_PostEffect::Cast(ref.ModelComponent());
  if (nullptr != pep)
    return pep;

  return none_return_value;
}
