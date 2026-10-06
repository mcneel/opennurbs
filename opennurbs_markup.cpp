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

class ON_MarkupPrivate
{
public:
  bool operator==(const ON_MarkupPrivate& other) const;

public:
  ON_WindowsBitmap m_background_bitmap;
  ON_Viewport m_viewport;
  ON::view_type m_view_type = ON::model_view_type;
  ON_wString m_comments;
  ON_3dmRevisionHistory m_revision_history;
};

bool ON_MarkupPrivate::operator==(const ON_MarkupPrivate& other) const
{
  return m_background_bitmap == other.m_background_bitmap &&
    m_viewport.ViewProjectionContentHash() == other.m_viewport.ViewProjectionContentHash();
}

static ON_MarkupPrivate DefaultMarkupPrivate;

//////////////////////////////////////////////////////////////////////
// class ON_Markup

ON_OBJECT_IMPLEMENT(ON_Markup, ON_ModelComponent, "F3BEC9AB-2CF7-4F08-A9DF-EE19FFE2CA25");

ON_Markup::ON_Markup() ON_NOEXCEPT
  : ON_ModelComponent(ON_ModelComponent::Type::Markup)
{
}

ON_Markup::ON_Markup(const ON_Markup& src)
  : ON_ModelComponent(ON_ModelComponent::Type::Markup, src)
{
  if (src.m_private)
    m_private = new ON_MarkupPrivate(*src.m_private);
}

ON_Markup::~ON_Markup()
{
  if (m_private)
    delete m_private;
}

ON_Markup& ON_Markup::operator=(const ON_Markup& other)
{
  if (this != &other)
  {
    ON_ModelComponent::operator=(other);
    if (m_private)
    {
      delete m_private;
      m_private = nullptr;
    }
    if (other.m_private)
      m_private = new ON_MarkupPrivate(*other.m_private);
  }
  return *this;
}

bool ON_Markup::IsValid(ON_TextLog* text_log) const
{
  if (false == ON_ModelComponent::IsValid(text_log))
    return false;
  return true;
}

// When adding new fields written to 3dm files, always add information to this
// Dump function. Dump is used by the opennurbs file testing framework to
// perform comparisons and is useful for manual comparison in when tests fail.
void ON_Markup::Dump(ON_TextLog& dump) const
{
  ON_ModelComponent::Dump(dump);
}

// This enum is patterned off of ON_3dmObjectAttributeTypeCode
enum ON_MarkupTypeCodes : unsigned char
{
  BackgroundBitmap = 1,
  Viewport = 2,
  ViewType = 3,
  Comments = 4,
  RevisionHistory = 5,
  LastMarkupTypeCode = 5
};

bool ON_Markup::Write(ON_BinaryArchive& file) const
{
  const int minor_version = 0;
  if (!file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, minor_version))
    return false;

  bool rc = false;

  for (;;)
  {
    // chunk version 1.0 fields
    if (!file.WriteModelComponentAttributes(*this, ON_ModelComponent::Attributes::BinaryArchiveAttributes))
      break;

    // Only write non-default values in a similar fashion as ON_3dmObjectAttributes

    // ON_MarkupTypeCodes::BackgroundBitmap
    {
      const ON_WindowsBitmap& background_bitmap = BackgroundBitmap();
      if (background_bitmap.IsValid())
      {
        const unsigned char item_id = ON_MarkupTypeCodes::BackgroundBitmap; // 1
        if (!file.WriteChar(item_id))
          break;
        if (!background_bitmap.WriteCompressed(file))
          break;
      }
    }

    // ON_MarkupTypeCodes::Viewport
    {
      const ON_Viewport& viewport = Viewport();
      if (viewport.ViewProjectionContentHash() != DefaultMarkupPrivate.m_viewport.ViewProjectionContentHash())
      {
        const unsigned char item_id = ON_MarkupTypeCodes::Viewport; // 2
        if (!file.WriteChar(item_id))
          break;
        if (!viewport.Write(file))
          break;
      }
    }

    // ON_MarkupTypeCodes::ViewType
    {
      ON::view_type view_type = ViewType();
      if (view_type != DefaultMarkupPrivate.m_view_type)
      {
        const unsigned char item_id = ON_MarkupTypeCodes::ViewType; // 3
        if (!file.WriteChar(item_id))
          break;
        if (!file.WriteInt(view_type))
          break;
      }
    }

    // ON_MarkupTypeCodes::Comments
    {
      ON_wString comments = Comments();
      if (comments != DefaultMarkupPrivate.m_comments)
      {
        const unsigned char item_id = ON_MarkupTypeCodes::Comments; // 4
        if (!file.WriteChar(item_id))
          break;
        if (!file.WriteString(comments))
          break;
      }
    }

    // ON_MarkupTypeCodes::RevisionHistory
    {
      const ON_3dmRevisionHistory& revision_history = RevisionHistory();
      if (!revision_history.IsEmpty())
      {
        const unsigned char item_id = ON_MarkupTypeCodes::RevisionHistory; // 5
        if (!file.WriteChar(item_id))
          break;
        if (!revision_history.Write(file))
          break;
      }
    }

    // 0 indicates end of markup attributes
    const unsigned char attributes_end = 0;
    if (!file.WriteChar(attributes_end))
      break;

    rc = true;
    break;
  }

  if (!file.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool ON_Markup::Read(ON_BinaryArchive& file)
{
  *this = ON_Markup();

  int major_version = 0;
  int minor_version = 0;
  if (!file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;

  bool rc = false;

  if (1 == major_version)
  {
    for (;;)
    {
      // chunk version 1.0 fields
      unsigned int model_component_attributes_filter = 0;
      if (!file.ReadModelComponentAttributes(*this, &model_component_attributes_filter))
        break;

      unsigned char item_id = 0;
      if (!file.ReadChar(&item_id)) // read first/last item_id
        break;

      // ON_MarkupTypeCodes::BackgroundBitmap
      if (item_id == ON_MarkupTypeCodes::BackgroundBitmap) // 1
      {
        ON_WindowsBitmap background_bitmap;
        if (!background_bitmap.ReadCompressed(file))
          break;
        SetBackgroundBitmap(background_bitmap);
        if (!file.ReadChar(&item_id)) // read next item_id
          break;
      }

      // ON_MarkupTypeCodes::Viewport
      if (item_id == ON_MarkupTypeCodes::Viewport) // 2
      {
        ON_Viewport viewport;
        if (!viewport.Read(file))
          break;
        SetViewport(viewport);
        if (!file.ReadChar(&item_id)) // read next item_id
          break;
      }

      // ON_MarkupTypeCodes::ViewType
      if (item_id == ON_MarkupTypeCodes::ViewType) // 3
      {
        int view_type = 0;
        if (!file.ReadInt(&view_type))
          break;
        SetViewType(ON::ViewType(view_type));
        if (!file.ReadChar(&item_id)) // read next item_id
          break;
      }

      // ON_MarkupTypeCodes::Comments
      if (item_id == ON_MarkupTypeCodes::Comments) // 4
      {
        ON_wString comments;
        if (!file.ReadString(comments))
          break;
        SetComments(comments);
        if (!file.ReadChar(&item_id)) // read next item_id
          break;
      }

      // ON_MarkupTypeCodes::RevisionHistory
      if (item_id == ON_MarkupTypeCodes::RevisionHistory) // 5
      {
        ON_3dmRevisionHistory revision_history;
        if (!revision_history.Read(file))
          break;
        SetRevisionHistory(revision_history);
        if (!file.ReadChar(&item_id)) // read next item_id
          break;
      }

      if (item_id > ON_MarkupTypeCodes::LastMarkupTypeCode)
      {
        // we are reading file written with code newer
        // than this code (minor_version > 0)
        item_id = 0;
      }

      rc = true;
      break;
    }
  }

  if (!file.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool ON_Markup::MarkupAttributesEqual(const ON_Markup& other) const
{
  if (nullptr == m_private && nullptr == other.m_private)
    return true;

  if (m_private && other.m_private)
    return (*m_private) == (*other.m_private);

  return false;
}

void ON_Markup::EnsureMarkupPrivate()
{
  if (nullptr == m_private)
    m_private = new ON_MarkupPrivate();
}

const ON_WindowsBitmap& ON_Markup::BackgroundBitmap() const
{
  return m_private
    ? m_private->m_background_bitmap
    : DefaultMarkupPrivate.m_background_bitmap;
}

void ON_Markup::SetBackgroundBitmap(const ON_WindowsBitmap& bitmap)
{
  EnsureMarkupPrivate();
  m_private->m_background_bitmap = bitmap;
  IncrementContentVersionNumber();
}

const ON_Viewport& ON_Markup::Viewport() const
{
  return m_private
    ? m_private->m_viewport
    : DefaultMarkupPrivate.m_viewport;
}

void ON_Markup::SetViewport(const ON_Viewport& viewport)
{
  SetViewport(viewport, false);
}

void ON_Markup::SetViewport(const ON_Viewport& viewport, bool bPageView)
{
  EnsureMarkupPrivate();
  m_private->m_viewport = viewport;
  m_private->m_view_type = bPageView ? ON::page_view_type : ON::model_view_type;
  IncrementContentVersionNumber();
}

const bool ON_Markup::IsPageView() const
{
  return m_private
    ? m_private->m_view_type == ON::page_view_type
    : DefaultMarkupPrivate.m_view_type == ON::page_view_type;
}

ON::view_type ON_Markup::ViewType() const
{
  return m_private
    ? m_private->m_view_type
    : DefaultMarkupPrivate.m_view_type;
}

void ON_Markup::SetViewType(ON::view_type view_type)
{
  EnsureMarkupPrivate();
  m_private->m_view_type = view_type;
  IncrementContentVersionNumber();
}

const ON_3dmRevisionHistory& ON_Markup::RevisionHistory() const
{
  return m_private
    ? m_private->m_revision_history
    : DefaultMarkupPrivate.m_revision_history;
}

void ON_Markup::SetRevisionHistory(const ON_3dmRevisionHistory& revision_history)
{
  EnsureMarkupPrivate();
  m_private->m_revision_history = revision_history;
  IncrementContentVersionNumber();
}

void ON_Markup::RecordRevisionHistory()
{
  EnsureMarkupPrivate();
  m_private->m_revision_history.NewRevision();
  IncrementContentVersionNumber();
}

ON_wString ON_Markup::Comments() const
{
  return m_private
    ? m_private->m_comments
    : DefaultMarkupPrivate.m_comments;
}

void ON_Markup::SetComments(const wchar_t* comments)
{
  EnsureMarkupPrivate();
  m_private->m_comments = comments;
  IncrementContentVersionNumber();
}
