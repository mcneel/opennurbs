//
// Copyright (c) 1993-2026 Robert McNeel & Associates. All rights reserved.
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

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif


ON_OBJECT_IMPLEMENT(ON_PageViewGroup, ON_ModelComponent, "3532B61C-B153-4F4D-8A36-974EBD5F9BE1");

const ON_PageViewGroup* ON_PageViewGroup::FromModelComponentRef(
  const ON_ModelComponentReference& model_component_reference,
  const ON_PageViewGroup* none_return_value
)
{
  const ON_PageViewGroup* page_view_group = ON_PageViewGroup::Cast(model_component_reference.ModelComponent());
  return (nullptr != page_view_group) ? page_view_group : none_return_value;
}

ON_PageViewGroup::ON_PageViewGroup() ON_NOEXCEPT
  : ON_ModelComponent(ON_ModelComponent::Type::PageViewGroup)
{
}

ON_PageViewGroup::ON_PageViewGroup(const ON_PageViewGroup& src)
  : ON_ModelComponent(ON_ModelComponent::Type::PageViewGroup, src)
  , m_description(src.m_description)
  , m_bExpanded(src.m_bExpanded)
{
}

//////////////////////////////////////////////////////////////////////
//
// ON_Object overrides

bool ON_PageViewGroup::IsValid(ON_TextLog* text_log) const
{
  return (IdIsNotNil() && NameIsSet() && Index() >= 0);
}

void ON_PageViewGroup::Dump(ON_TextLog& text_log) const
{
  ON_ModelComponent::Dump(text_log);
}

bool ON_PageViewGroup::Write(
  ON_BinaryArchive& archive // serialize definition to binary archive
) const
{
  bool rc = archive.Write3dmChunkVersion(1, 0);
  // version 1.0 fields
  if (rc) rc = archive.Write3dmReferencedComponentIndex(ON_ModelComponent::Type::PageViewGroup, Index());
  if (rc) rc = archive.WriteString(Name());
  if (rc) rc = archive.WriteUuid(Id());
  if (rc) rc = archive.WriteBool(m_bExpanded);
  if (rc) rc = archive.WriteString(m_description);
  return rc;
}

bool ON_PageViewGroup::Read(
  ON_BinaryArchive& archive // restore definition from binary archive
)
{
  *this = ON_PageViewGroup::Unset;

  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.Read3dmChunkVersion(&major_version, &minor_version);
  if (major_version == 1)
  {
    // version 1.0 fields

    int group_index = Index();
    if (rc) rc = archive.ReadInt(&group_index);
    if (rc) SetIndex(group_index);

    ON_wString group_name;
    if (rc) rc = archive.ReadString(group_name);
    if (rc) SetName(group_name);

    ON_UUID group_id = ON_nil_uuid;
    if (rc) rc = archive.ReadUuid(group_id);
    if (rc) SetId(group_id);
    if (rc && IdIsNil())
      SetId();

    if (rc) rc = archive.ReadBool(&m_bExpanded);
    if (rc) rc = archive.ReadString(m_description);
  }
  else
  {
    rc = false;
  }
  return rc;
}

