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

#if !defined(ON_RENDER_CONTENT_INC_)
#define ON_RENDER_CONTENT_INC_

#define ON_RDK_TYPE_NAME                    L"type-name"
#define ON_RDK_TYPE_ID                      L"type-id"
#define ON_RDK_INSTANCE_ID                  L"instance-id"
#define ON_RDK_RENDER_ENGINE_ID             L"render-engine-id"
#define ON_RDK_PLUG_IN_ID                   L"plug-in-id"
#define ON_RDK_GROUP_ID                     L"group-id"
#define ON_RDK_INSTANCE_NAME                L"instance-name"
#define ON_RDK_CHILD_SLOT_NAME              L"child-slot-name"
#define ON_RDK_NOTES                        L"notes"
#define ON_RDK_TAGS                         L"tags"
#define ON_RDK_HIDDEN                       L"hidden"
#define ON_RDK_REFERENCE                    L"reference"
#define ON_RDK_AUTO_DELETE                  L"auto-delete"

#define ON_RDK_ENV_BACKGROUND_COLOR         L"background-color"

// ON_Environment

class ON_CLASS ON_Environment : public ON_Object
{
  ON_OBJECT_DECLARE(ON_Environment);

public:
  ON_Environment();
  ON_Environment(const ON_Environment& src);
  virtual ~ON_Environment();

  const ON_Environment& operator = (const ON_Environment& src);

  bool operator == (const ON_Environment& src) const;
  bool operator != (const ON_Environment& src) const;

  enum class BackgroundProjections : unsigned int
  {
    Planar = 0,
    Spherical = 1, // Equirectangular projection.
    Emap = 2,      // Mirror ball.
    Box = 3,
    Automatic = 4,
    LightProbe = 5,
    CubeMap = 6,
    VerticalCrossCubeMap = 7,
    HorizontalCrossCubeMap = 8,
    Hemispherical = 9,
  };

  ON_Color BackgroundColor(void) const;
  void SetBackgroundColor(ON_Color color);

  ON_Texture BackgroundImage(void) const;
  void SetBackgroundImage(const ON_Texture& tex);

  BackgroundProjections BackgroundProjection(void) const;
  void SetBackgroundProjection(BackgroundProjections proj);

  static BackgroundProjections ProjectionFromString(const wchar_t* proj);
  static const wchar_t* StringFromProjection(BackgroundProjections proj);

  /** Emergency virtual function for future expansion. */
  virtual void* EVF(const wchar_t* wszFunc, void* pvData);

private:
  class CImpl;
  CImpl* m_impl;
};

class ON_CLASS ON_RenderContent : public ON_ModelComponent
{
  ON_OBJECT_DECLARE(ON_RenderContent);

public:
  ON_RenderContent(const wchar_t* kind);
  ON_RenderContent(const ON_RenderContent& c);
  virtual ~ON_RenderContent();

  const ON_RenderContent& operator = (const ON_RenderContent& rc);

  // Returns: The internal name of the content type.
  ON_wString TypeName(void) const;

  // Set the content's type name.
  void SetTypeName(const wchar_t* name);

  // Returns: The unique id of the content type.
  ON_UUID TypeId(void) const;

  // Set the content's type id.
  void SetTypeId(const ON_UUID& uuid);

  // Returns: The content's render-engine id.
  ON_UUID RenderEngineId(void) const;

  // Set the content's render-engine id.
  void SetRenderEngineId(const ON_UUID& uuid);

  // Returns: The content's plug-in id.
  ON_UUID PlugInId(void) const;

  // Set the content's plug-in id.
  void SetPlugInId(const ON_UUID& uuid);

  // Returns: The content's notes.
  ON_wString Notes(void) const;

  // Sets the content's notes.
  void SetNotes(const wchar_t* notes);

  // Returns: The content's tags.
  ON_wString Tags(void) const;

  // Sets the content's tags.
  void SetTags(const wchar_t* tags);

  // Returns: The content's group id.
  ON_UUID GroupId(void) const;

  // Sets the content's group id.
  void SetGroupId(const ON_UUID& group);

  // Returns: True if the content is hidden.
  bool Hidden(void) const;

  // Sets whether or not the content is hidden.
  void SetHidden(bool hidden);

  // Returns: True if the content is a reference content.
  bool Reference(void) const;

  // Sets whether or not the content is a reference content.
  void SetReference(bool ref);

  // Returns: True if the content is automatically deleted when not in use.
  bool AutoDelete(void) const;

  // Sets whether or not the content is automatically deleted when not in use.
  void SetAutoDelete(bool autodel);

  // Gets a variant giving the type and value of the parameter, if found.
  // If the parameter is not found, the function returns a null variant.
  ON_XMLVariant GetParameter(const wchar_t* name) const;

  // Sets the value of a named parameter.
  // Returns: True if successful, else false.
  bool SetParameter(const wchar_t* name, const ON_XMLVariant& value);

  class ON_CLASS ChildIterator
  {
  public:
    ChildIterator(const ON_RenderContent* pParent);
    virtual ~ChildIterator();

    virtual ON_RenderContent* GetNextChild(void);

    /** Emergency virtual function for future expansion. */
    virtual void* EVF(const wchar_t* func, void* data);

  private:
    class CImpl;
    CImpl* m_impl;
  };

  // Returns: An iterator for iterating over the content's children.
  ChildIterator GetChildIterator(void) const;

  // Returns: The parent content or null if this is the top level object.
  const ON_RenderContent* Parent(void) const;

  // Returns: The first child of this content or null if none.
  const ON_RenderContent* FirstChild(void) const;

  // Returns: The first sibling of this content or null if none.
  const ON_RenderContent* NextSibling(void) const;

  // Returns: The top level parent of this content.
  const ON_RenderContent& TopLevel(void) const;

  // Returns: True if this is a top-level content (i.e., has no parent; is not a child).
  bool IsTopLevel(void) const;

  // Returns: True if this is a child content (i.e., has a parent; is not top-level).
  bool IsChild(void) const;

  // Sets another content as a child of this content.
  // Param child is the content to set as a child of this content. This content is copied and the
  // copy is attached to the model. If the child is already attached to a model the function will fail.
  // If the child is already a child of this or another content, the function will fail.
  // Param childSlotName is the child slot name that will be assigned to this child.
  // The child slot name cannot be an empty string. If it is, the function will fail.
  // Returns: True if successful, else false.
  bool SetChild(const ON_RenderContent& child, const wchar_t* child_slot_name);

  // Returns: The content's child-slot name.
  ON_wString ChildSlotName(void) const;

  // Sets the content's child-slot name.
  void SetChildSlotName(const wchar_t* child_slot_name);

  // Returns true if the child slot with the specified child slot name is turned on, else false.
  // Also returns false if there is no child with the specified child slot name.
  bool ChildSlotOn(const wchar_t* child_slot_name) const;

  // Turns the child slot with the specified child slot name on or off.
  // Returns: True if successful, else false.
  bool SetChildSlotOn(bool on, const wchar_t* child_slot_name);

  // Deletes any existing child with the specified child slot name.
  // Returns: True if successful, else false.
  bool DeleteChild(const wchar_t* child_slot_name);

  // Returns: The child with the specified child slot name, or null if no such child exists.
  const ON_RenderContent* FindChild(const wchar_t* child_slot_name) const;

  // Get the render content's state as an XML string.
  ON_wString XML(bool recursive) const;

  // Set the render content's state from an XML string.
  bool SetXML(const wchar_t* xml);

  // Returns the XML node that stores the state of this render content. Note that this does not include
  // any children of the render content. An ON_RenderContent only stores its own XML.
  const ON_XMLNode& XMLNode(void) const;

  // Returns the kind of render content as a string.
  ON_wString Kind(void) const;

  // If ON_RenderContent::Cast(ref.ModelComponent()) is not null,
  // that pointer is returned. Otherwise, none_return_value is returned.
  static const ON_RenderContent* FromModelComponentRef(const ON_ModelComponentReference& ref,
                                                       const ON_RenderContent* none_return_value);

public:
  /** Emergency virtual function for future expansion. */
  virtual void* EVF(const wchar_t* func, void* data);

public:
  class CImpl;
  CImpl* m_impl;

protected:
  ON__UINT8 m_Impl[328];
};

class ON_CLASS ON_RenderMaterial : public ON_RenderContent
{
  ON_OBJECT_DECLARE(ON_RenderMaterial);

public:
  ON_RenderMaterial();
  ON_RenderMaterial(const ON_RenderMaterial& m);
  virtual ~ON_RenderMaterial();

  void operator = (const ON_RenderMaterial&);

  ON_Material SimulatedMaterial(void) const;
};

class ON_CLASS ON_RenderEnvironment : public ON_RenderContent
{
  ON_OBJECT_DECLARE(ON_RenderEnvironment);

public:
  ON_RenderEnvironment();
  ON_RenderEnvironment(const ON_RenderEnvironment& e);
  virtual ~ON_RenderEnvironment();

  void operator = (const ON_RenderEnvironment&);

  ON_Environment SimulatedEnvironment(void) const;
};

class ON_CLASS ON_RenderTexture : public ON_RenderContent
{
  ON_OBJECT_DECLARE(ON_RenderTexture);

public:
  ON_RenderTexture();
  ON_RenderTexture(const ON_RenderTexture& t);
  virtual ~ON_RenderTexture();

  void operator = (const ON_RenderTexture&);

  ON_Texture SimulatedTexture(void) const;
};

#endif
