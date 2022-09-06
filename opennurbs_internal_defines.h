/*
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

// Internal header; not in the public SDK.

#if !defined(OPENNURBS_INTERNAL_DEFINES_INC_)
#define OPENNURBS_INTERNAL_DEFINES_INC_

#if defined(ON_COMPILING_OPENNURBS)

#include <unordered_map>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#define ON_KIND_MATERIAL     L"material"
#define ON_KIND_ENVIRONMENT  L"environment"
#define ON_KIND_TEXTURE      L"texture"

#define ON_RDK_SLASH                L"/"
#define ON_RDK_POSTFIX_SECTION      L"-section"
#define ON_RDK_DOCUMENT             L"render-content-manager-document"
#define ON_RDK_UD_ROOT              L"render-content-manager-data"
#define   ON_RDK_CURRENT_CONTENT    L"content"
#define   ON_RDK_SETTINGS           L"settings"
#define     ON_RDK_POST_EFFECTS     L"post-effects-new"
#define     ON_RDK_PEP_TYPE_EARLY   L"early"
#define     ON_RDK_PEP_TYPE_TONE    L"tone-mapping"
#define     ON_RDK_PEP_TYPE_LATE    L"late"
#define     ON_RDK_RENDERING        L"rendering"
#define     ON_RDK_SUN              L"sun"

inline bool IsFloatEqual (float  f1, float  f2) { return (fabsf(f1 - f2) < 1e-6); }
inline bool IsDoubleEqual(double d1, double d2) { return (fabs (d1 - d2) < 1e-10); }

ON_UUID RdkPlugInId(void);
ON_UUID UniversalRenderEngineId(void);

ON__INT64 Integerize(float dirty);
ON__INT64 Integerize(double dirty);

void SetModel(const class ON_RenderContent&, ONX_Model&);
void SetModel(const class ON_PostEffect&, ONX_Model&);
ON_3dmObjectAttributes* GetComponentAttributes(const ON_ModelComponent& component);
ON_RenderContent* NewRenderContentFromNode(const class ON_XMLNode& node);
ON_PostEffect* NewPostEffectFromNode(ON_XMLNode& node);
void SetRenderContentNodeRecursive(const ON_RenderContent& rc, ON_XMLNode& node);
bool CreateArchiveBufferFromXML(const ON_wString& xml, ON_Buffer& buf, int archive_3dm_version);
ON_XMLNode* FindPostEffectNodeForId(const ON_XMLNode& sectionNode, const ON_UUID& id);
bool GetRDKObjectInformation(const ON_Object& object, ON_wString& xml, int archive_3dm_version);
bool SetRDKObjectInformation(ON_Object& object, const ON_wString& xml, int archive_3dm_version);
bool GetRDKEmbeddedFiles(const ONX_Model_UserData& docud, ON_ClassArray<ON_wString>& paths, ON_SimpleArray<unsigned char*>& embedded_files_as_buffers, ON_SimpleArray<size_t>& buffer_sizes);
void CreateDecalsFromXML(const ONX_Model& model, int archive_3dm_version);
void CreateXMLFromDecals(const ONX_Model& model, int archive_3dm_version);
void CreateMeshModifiersFromXML(const ONX_Model& model, int archive_3dm_version);
void CreateXMLFromMeshModifiers(const ONX_Model& model, int archive_3dm_version);
bool GetMeshModifierObjectInformation(const ON_Object& object, ON_wString& xml, int archive_3dm_version);
void SetMeshModifierObjectInformation(ON_Object& object, const ON_MeshModifier* mm, const ON_XMLNode& node, int archive_3dm_version, const wchar_t* mm_node_name);
bool IsRDKDocumentInformation(const ONX_Model_UserData& docud);

template <class T> inline T Lerp(float  t, const T& l, const T& h) { return l + T(t) * (h - l); }
template <class T> inline T Lerp(double t, const T& l, const T& h) { return l + T(t) * (h - l); }

class ON_InternalXMLImpl
{
public:
  ON_InternalXMLImpl(ON_XMLNode* n=nullptr) : m_model_node(n) { }
  virtual ~ON_InternalXMLImpl() { }

  ON_XMLVariant GetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const ON_XMLVariant& def) const;
  bool SetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const ON_XMLVariant& value);

  ON_XMLVariant GetParameter(const wchar_t* param_name, const ON_XMLVariant& def) const { return GetParameter(L"", param_name, def); }
  bool SetParameter(const wchar_t* param_name, const ON_XMLVariant& value) { return SetParameter(L"", param_name, value); }

  ON_XMLVariant GetParameter_NoType(const wchar_t* path_to_node, const wchar_t* param_name, const wchar_t* default_type, const ON_XMLVariant& def) const;
  bool SetParameter_NoType(const wchar_t* path_to_node, const wchar_t* param_name, const ON_XMLVariant& value);

  ON_XMLNode& Node(void) const;
  ON_XMLNode& NodeAt(const wchar_t* path_to_node) const;

private:
  ON_XMLVariant InternalGetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const wchar_t* default_type, const ON_XMLVariant& def) const;
  bool InternalSetParameter(const wchar_t* path_to_node, const wchar_t* param_name, bool write_type, const ON_XMLVariant& value);

public:
  mutable ON_XMLRootNode m_local_node; // Used when m_model_node is null.
  ON_XMLNode* m_model_node;
};

class ON_DecalCollection final
{
public:
  ON_DecalCollection() { }
  ON_DecalCollection(const ON_DecalCollection& dc) = delete;
  ~ON_DecalCollection();

  const ON_DecalCollection& operator = (const ON_DecalCollection& dc);

  void DeleteAllDecals(void);
  void Populate(const ON_XMLRootNode& node);
  ON_Decal* AddDecal(void);
  const ON_SimpleArray<ON_Decal*>& GetDecalArray(void) const { return m_decals; }
  static bool NodeContainsDecals(const ON_XMLRootNode& node);

private:
  ON_SimpleArray<ON_Decal*> m_decals;
};

template<class T> inline void hash_combine(size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
}

class UuidHasher // Hasher for using ON_UUID as key with std::map
{
public:
	inline size_t operator()(const ON_UUID& uuid) const
	{
		size_t seed = 0;
		::hash_combine(seed, uuid.Data1);
		::hash_combine(seed, uuid.Data2);
		::hash_combine(seed, uuid.Data3);

		for (int i = 0; i < 8; i++)
		{
			::hash_combine(seed, uuid.Data4[i]);
		}

		return seed;
	}
};

//--------------------------------------------------------------------------------------------------

class ON_INTERNAL_OBSOLETE
{
public:

  //// OBSOLETE V5 Dimension Types ///////////////////////////////////////////////////////////
  enum class V5_eAnnotationType : unsigned char
  {
    dtNothing,
    dtDimLinear,
    dtDimAligned,
    dtDimAngular,
    dtDimDiameter,
    dtDimRadius,
    dtLeader,
    dtTextBlock,
    dtDimOrdinate,
  };

  // convert integer to eAnnotationType enum
  static ON_INTERNAL_OBSOLETE::V5_eAnnotationType V5AnnotationTypeFromUnsigned(
    unsigned int v5_annotation_type_as_unsigned
  );

  //// dim text locations ///////////////////////////////////////////////////////////
  enum class V5_TextDisplayMode : unsigned char
  {
    kNormal = 0, // antique name - triggers use of current default
    kHorizontalToScreen = 1,         // Horizontal to the screen
    kAboveLine = 2,
    kInLine = 3,
    kHorizontalInCplane = 4   // horizontal in the dimension's plane 
  };

  static ON_INTERNAL_OBSOLETE::V5_TextDisplayMode V5TextDisplayModeFromUnsigned(
    unsigned int text_display_mode_as_unsigned
  );

  static ON_INTERNAL_OBSOLETE::V5_TextDisplayMode V5TextDisplayModeFromV6DimStyle(
    const ON_DimStyle& V6_dim_style
  );
  
  /// <summary>
  /// Attachment of content
  /// </summary>
  enum class V5_vertical_alignment : unsigned char
  {
    /// <summary>
    /// Text centered on dimension line (does not apply to leaders or text)
    /// </summary>
    Centered = 0,
    /// <summary>
    /// Text above dimension line (does not apply to leaders or text)
    /// </summary>
    Above = 1,
    /// <summary>
    /// Text below dimension line (does not apply to leaders or text)
    /// </summary>
    Below = 2,
    /// <summary>
    /// Leader tail at top of text  (does not apply to text or dimensions)
    /// </summary>
    Top = 3, // = TextVerticalAlignment::Top
    /// <summary>
    /// Leader tail at middle of first text line (does not apply to text or dimensions)
    /// </summary>
    FirstLine = 4, // = MiddleOfTop
    /// <summary>
    /// Leader tail at middle of text or content (does not apply to text or dimensions)
    /// </summary>
    Middle = 5, // = Middle
    /// <summary>
    /// Leader tail at middle of last text line (does not apply to text or dimensions)
    /// </summary>
    LastLine = 6, // = MiddleOfBottom
    /// <summary>
    /// Leader tail at bottom of text (does not apply to text or dimensions)
    /// </summary>
    Bottom = 7, // = Bottom
    /// <summary>
    /// Leader tail at bottom of text, text underlined (does not apply to text or dimensions)
    /// </summary>
    Underlined = 8 // Underlined

    // nothing matched BottomOfTop
  };

  static ON_INTERNAL_OBSOLETE::V5_vertical_alignment V5VerticalAlignmentFromUnsigned(
    unsigned int vertical_alignment_as_unsigned
  );

  static ON_INTERNAL_OBSOLETE::V5_vertical_alignment V5VerticalAlignmentFromV5Justification(
    unsigned int v5_justification_bits
  );

  static ON_INTERNAL_OBSOLETE::V5_vertical_alignment V5VerticalAlignmentFromV6VerticalAlignment(
    const ON::TextVerticalAlignment text_vertical_alignment
  );

  static ON::TextVerticalAlignment V6VerticalAlignmentFromV5VerticalAlignment(
    ON_INTERNAL_OBSOLETE::V5_vertical_alignment V5_vertical_alignment
  );


  enum class V5_horizontal_alignment : unsigned char
  {
    /// <summary>
    /// Left aligned
    /// </summary>
    Left = 0, // Left
    /// <summary>
    /// Centered
    /// </summary>
    Center = 1,
    /// <summary>
    /// Right aligned
    /// </summary>
    Right = 2,
    /// <summary>
    /// Determined by orientation
    /// Primarily for leaders to make
    /// text right align when tail is to the left 
    /// and left align when tail is to the right
    /// </summary>
    Auto = 3,
  };

  static ON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5HorizontalAlignmentFromUnsigned(
    unsigned int horizontal_alignment_as_unsigned
  );

  static ON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5HorizontalAlignmentFromV5Justification(
    unsigned int v5_justification_bits
  );

  static ON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5HorizontalAlignmentFromV6HorizontalAlignment(
    const ON::TextHorizontalAlignment text_horizontal_alignment
  );

  static ON::TextHorizontalAlignment V6HorizontalAlignmentFromV5HorizontalAlignment(
    ON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5_vertical_alignment
  );
};

#endif

#endif
