//
// Copyright (c) 1993-2025 Robert McNeel & Associates. All rights reserved.
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

#if defined(OPENNURBS_TAG_WIP)

ON_OBJECT_IMPLEMENT(ON_Tag, ON_ModelComponent, "ED680EBA-8616-4D71-B2DA-C03BAB456E6E");

const ON_Tag* ON_Tag::FromModelComponentRef(
  const ON_ModelComponentReference& model_component_reference,
  const ON_Tag* none_return_value
)
{
  const ON_Tag* tag = ON_Tag::Cast(model_component_reference.ModelComponent());
  return (nullptr != tag) ? tag : none_return_value;
}

ON_Tag::ON_Tag() ON_NOEXCEPT
  : ON_ModelComponent(ON_ModelComponent::Type::Tag)
{
}

ON_Tag::ON_Tag(const ON_Tag& src)
  : ON_ModelComponent(ON_ModelComponent::Type::Tag, src)
{
}

//////////////////////////////////////////////////////////////////////
//
// ON_Object overrides

bool ON_Tag::IsValid(ON_TextLog* text_log) const
{
  return (IdIsNotNil() && NameIsSet() && Index() >= 0);
}

void ON_Tag::Dump(ON_TextLog& text_log) const
{
  ON_ModelComponent::Dump(text_log);
}

bool ON_Tag::Write(
  ON_BinaryArchive& archive // serialize definition to binary archive
) const
{
  bool rc = archive.Write3dmChunkVersion(1, 0);
  // version 1.0 fields
  if (rc) rc = archive.Write3dmReferencedComponentIndex(ON_ModelComponent::Type::Group, Index());
  if (rc) rc = archive.WriteString(Name());
  if (rc) rc = archive.WriteUuid(Id());
  return rc;
}

bool ON_Tag::Read(
  ON_BinaryArchive& archive // restore definition from binary archive
)
{
  *this = ON_Tag::Unset;

  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.Read3dmChunkVersion(&major_version, &minor_version);
  if (major_version == 1)
  {
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
    {
      // modern times require unique ids.
      SetId();
    }
  }
  else
  {
    rc = false;
  }
  return rc;
}

#endif // OPENNURBS_TAG_WIP