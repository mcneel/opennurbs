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

#if !defined(OPENNURBS_MARKUP_INC_)
#define OPENNURBS_MARKUP_INC_

class ON_WindowsBitmap;
class ON_3dmRevisionHistory;

//////////////////////////////////////////////////////////////////////
// class ON_Markup

class ON_CLASS ON_Markup : public ON_ModelComponent
{
  ON_OBJECT_DECLARE(ON_Markup);

public:
  ON_Markup() ON_NOEXCEPT;
  ~ON_Markup();
  ON_Markup(const ON_Markup& other);
  ON_Markup& operator=(const ON_Markup& other);

  static const ON_Markup Unset;   // index = ON_UNSET_INT_INDEX, id = nil
  static const ON_Markup Default; // index = -1, unique and persistent id.

  // Description:
  //  Tests that name is set and there is at least one non-zero length segment
  bool IsValid(class ON_TextLog* text_log = nullptr) const override;

  void Dump(ON_TextLog&) const override; // for debugging

  // Description:
  //  Write to file (serialize definition to binary archive)
  bool Write(ON_BinaryArchive&) const override;

  // Description:
  //  Read from file (restore definition from binary archive)
  bool Read(ON_BinaryArchive&) override;

  // Description:
  //  Test only the markup attributes below for equality. Does not
  //  perform any testing of the ON_ModelComponent fields
  // Parameters:
  //  other: other markup to compare against this
  bool MarkupAttributesEqual(const ON_Markup& other) const;

  const ON_WindowsBitmap& BackgroundBitmap() const;
  void SetBackgroundBitmap(const class ON_WindowsBitmap& bitmap);

  const ON_Viewport& Viewport() const;
  void SetViewport(const ON_Viewport& viewport);
  void SetViewport(const ON_Viewport& viewport, bool bPageView);

  const bool IsPageView() const;

  ON::view_type ViewType() const;
  void SetViewType(ON::view_type view_type);

  const ON_3dmRevisionHistory& RevisionHistory() const;
  void SetRevisionHistory(const ON_3dmRevisionHistory& revision_history);

  void RecordRevisionHistory();

  ON_wString Comments() const;
  void SetComments(const wchar_t* comments);

private:
  void EnsureMarkupPrivate();

private:
  mutable class ON_MarkupPrivate* m_private = nullptr;
};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_Markup*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<const ON_Markup*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_ObjectArray<ON_Markup>;
#endif

#endif

