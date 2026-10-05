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

#if !defined(OPENNURBS_PAGEVIEWGROUP_INC_)
#define OPENNURBS_PAGEVIEWGROUP_INC_

class ON_CLASS ON_PageViewGroup : public ON_ModelComponent
{
  ON_OBJECT_DECLARE(ON_PageViewGroup);

public:
  static const ON_PageViewGroup Unset;   // nil id

  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if ON_PageViewGroup::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If ON_PageViewGroup::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned.
  */
  static const ON_PageViewGroup* FromModelComponentRef(
    const class ON_ModelComponentReference& model_component_reference,
    const ON_PageViewGroup* none_return_value
  );

public:
  ON_PageViewGroup() ON_NOEXCEPT;
  ON_PageViewGroup(const ON_PageViewGroup& src);
  ~ON_PageViewGroup() = default;
  ON_PageViewGroup& operator=(const ON_PageViewGroup& src) = default;

public:

  //////////////////////////////////////////////////////////////////////
  //
  // ON_Object overrides

  bool IsValid(
    class ON_TextLog* text_log = nullptr
  ) const override;

  void Dump(
    ON_TextLog& text_log
  ) const override;

  bool Write(
    ON_BinaryArchive& archive
  ) const override;

  bool Read(
    ON_BinaryArchive& archive
  ) override;

  //////////////////////////////////////////////////////////////////////
  //
  // Properties

  /*
  Description:
    If true, when the pageview group is displayed in a tree control,
    then the list of members is shown in the control.
  */
  bool m_bExpanded = true;

  /*
  Description:
    Description of pageview group.
  */
  ON_wString m_description;

private:
  ON__UINT_PTR m_reserved = 0;
};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_PageViewGroup*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<const ON_PageViewGroup*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_ObjectArray<ON_PageViewGroup>;
#endif

#endif // OPENNURBS_PAGEVIEWGROUP_INC_/

