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

#if defined(OPENNURBS_TAG_WIP)

#if !defined(OPENNURBS_TAG_INC_)
#define OPENNURBS_TAG_INC_

class ON_CLASS ON_Tag : public ON_ModelComponent
{
  ON_OBJECT_DECLARE(ON_Tag);

public:
  static const ON_Tag Unset;   // nil id

  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if ON_Tag::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If ON_Tag::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned.
  */
  static const ON_Tag* FromModelComponentRef(
    const class ON_ModelComponentReference& model_component_reference,
    const ON_Tag* none_return_value
  );

public:
  ON_Tag() ON_NOEXCEPT;
  ON_Tag(const ON_Tag& src);
  ~ON_Tag() = default;
  ON_Tag& operator=(const ON_Tag& src) = default;

private:

  //////////////////////////////////////////////////////////////////////
  //
  // ON_Object overrides
  bool IsValid(class ON_TextLog* text_log = nullptr) const override;

  void Dump(
    ON_TextLog& text_log
  ) const override;

  bool Write(
    ON_BinaryArchive& archive
  ) const override;

  bool Read(
    ON_BinaryArchive& archive
  ) override;

private:
  ON__UINT_PTR m_reserved = 0;
};

#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_Tag*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<const ON_Tag*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_ObjectArray<ON_Tag>;
#endif

#endif // OPENNURBS_TAG_INC_/

#endif // OPENNURBS_TAG_WIP