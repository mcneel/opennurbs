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

#if !defined(ON_XML_INC_)
#define ON_XML_INC_

typedef bool (*ON_XMLRecurseChildrenCallback)(class ON_XMLNode*, void*);

class ON_CLASS ON_UnicodeTextFile
{
public:
  enum class Modes { Load, Save };
  enum class Types { Unknown, UTF8, UTF16 };

  ON_UnicodeTextFile(Types type = Types::Unknown);
  virtual ~ON_UnicodeTextFile();

  bool Open(const wchar_t* filename, Modes mode);
  bool Close(void);

  bool ReadString(ON_wString& s);
  bool WriteString(const wchar_t* s);

  // Emergency virtual function for future expansion.
  virtual void* EVF(const wchar_t* func, void* data);

  ON_UnicodeTextFile(const ON_UnicodeTextFile&) = delete;
  const ON_UnicodeTextFile& operator = (const ON_UnicodeTextFile&) = delete;

private:
  class CImpl;
  CImpl* m_impl;
  ON__UINT8 m_Impl[32];
};

class ON_CLASS ON_Base64 final
{
public:
  // Decode a base64 string.
  // buffer_out must be large enough to accomodate the decoded data. It is safe to use the
  // length of base64_in because this Base64 string will always be about 33% bigger than the
  // data it was created from. Returns the number of bytes written to buffer_out.
  // The function stops when max_length bytes have been decoded.
  // If buffer_out is null, the function simply calculates the exact required buffer size.
  static size_t Decode(const wchar_t* base64_in, void* buffer_out, size_t max_length = UINT_MAX);

  // Encode data to a base64 string. If append is true, the encoded string is appended to base64_out.
  // Otherwise base64_out is replaced with the encoded string.
  // Returns true if ok, false if unable to allocate the output buffer.
  static bool Encode(const void* buffer_in, size_t num_bytes, ON_wString& base64_out, bool append);
};

class ON_CLASS ON_XMLVariant final
{
public:
  enum class Types : unsigned int
  {
    Null,
    Bool, Integer, Float, Double, String,
    DoubleArray2, DoubleArray3, DoubleArray4, DoubleColor4,
    Matrix, Uuid, Time, Buffer
  };

  enum class ArrayTypes : unsigned int
  {
    Array2, Array3, Array4, Array16
  };

public:
  ON_XMLVariant(); // Creates an empty string type.
  ON_XMLVariant(bool value);
  ON_XMLVariant(int value);
  ON_XMLVariant(float value);
  ON_XMLVariant(double value);
  ON_XMLVariant(time_t time);
  ON_XMLVariant(const ON_wString& value);
  ON_XMLVariant(const ON_UUID& uuid);
  ON_XMLVariant(const ON_2dPoint& p);
  ON_XMLVariant(const ON_3dPoint& p);
  ON_XMLVariant(const ON_4fColor& c);
  ON_XMLVariant(const ON_Xform& xform);
  ON_XMLVariant(const double* point, ArrayTypes at=ArrayTypes::Array3);
  ON_XMLVariant(const float*  point, ArrayTypes at=ArrayTypes::Array3);
  ON_XMLVariant(const wchar_t* wsz);
  ON_XMLVariant(const void* buffer, size_t size);
  ON_XMLVariant(const ON_Buffer& buffer);
  ON_XMLVariant(const ON_XMLVariant& src);
  ~ON_XMLVariant();

  const ON_XMLVariant& operator = (const ON_XMLVariant& src);

  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

public:
  Types Type(void) const;
  ON_wString TypeAsString(void) const;
  bool IsEmpty(void) const;
  bool IsNull(void) const;

  ON::LengthUnitSystem Units(void) const;
  void SetUnits(ON::LengthUnitSystem units);

  // Optimized version of ON_wString::NeedsXMLEncode(). Always use this if possible
  // because it knows from the type of the variant whether entity ref encoded output is even possible.
  bool NeedsXMLEncode(void) const;

  ON_Buffer& GetBuffer(void) const;
  void ClearBuffers(void);

public:
  void SetNull(void);
  void SetValue(int v);
  void SetValue(double v);
  void SetValue(float v);
  void SetValue(const wchar_t* s);
  void SetValue(const ON_wString& s);
  void SetValue(const double* p, ArrayTypes at);
  void SetValue(const float* p, ArrayTypes at);
  void SetValue(const ON_2dPoint& p);
  void SetValue(const ON_3dPoint& p);
  void SetValue(const ON_4dPoint& p);
  void SetValue(const ON_4fColor& c);
  void SetValue(bool b);
  void SetValue(time_t time);
  void SetValue(const ON_UUID& uuid);
  void SetValue(const ON_Xform& xform);
  void SetValue(const void* buffer, size_t size);
  void SetValue(const ON_Buffer& buffer);

  bool       AsBool(void) const;
  int        AsInteger(void) const;
  double     AsDouble(void) const;
  float      AsFloat(void) const;
  ON_2dPoint As2dPoint(void) const;
  ON_3dPoint As3dPoint(void) const;
  ON_4dPoint As4dPoint(void) const;
  ON_UUID    AsUuid(void) const;
  ON_Xform   AsXform(void) const;
  time_t     AsTime(void) const;
  ON_4fColor AsColor(void) const;
  ON_Buffer  AsBuffer(void) const;
  void*      AsBuffer(size_t& size_out) const;
  ON_wString AsString(void) const;

  operator bool() const;
  operator double() const;
  operator float() const;
  operator int() const;
  operator ON_2dPoint() const;
  operator ON_3dPoint() const;
  operator ON_4dPoint() const;
  operator ON_4fColor() const;
  operator ON_UUID() const;
  operator time_t() const;
  operator ON_Buffer() const;
  operator ON_wString() const;

  void DoAutoTyping(Types t) const;

public:
  bool TypePending(void) const;
  void SetTypePendingFlag(bool bTypePending) const;

  // Format string as type and value. Useful for debugging.
  void Format(ON_wString& s) const;

protected:
  void StringToPoint(int iValues) const;

private:
  class CImpl;
  CImpl* m_impl;
  ON__UINT8 m_Impl[176];
};

class ON_CLASS ON_XMLProperty final
{
public:
  ON_XMLProperty();
  ON_XMLProperty(const ON_XMLVariant& value);
  ON_XMLProperty(const ON_wString& sName, const ON_XMLVariant& value);
  ON_XMLProperty(const ON_XMLProperty& prop);
  ~ON_XMLProperty();

  const ON_XMLProperty& operator = (const ON_XMLProperty& prop);

  const ON_wString& Name(void) const;
  void SetName(const wchar_t* name);

  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  ON_XMLProperty* Next(void) const;

  bool IsDefaultProperty(void) const;

  const ON_XMLVariant& GetValue(void) const;
  void SetValue(const ON_XMLVariant& value);

  bool operator < (const ON_XMLProperty& prop) const;

  ON_XMLVariant& GetNonConstValue(void);

  void SetHugeStringValue(const ON_wString& s);

private:
  class CImpl;
  CImpl* m_impl;
  ON__UINT8 m_Impl[32];

public:
  CImpl& Impl(void) const;
};

class ON_CLASS ON_XMLSegmentedStream
{
public:
  ON_XMLSegmentedStream();
  virtual ~ON_XMLSegmentedStream();

  int  Count(void) const;
  void Append(wchar_t* s);
  wchar_t* Segment(int index) const;

  // Emergency virtual function for future expansion.
  virtual void* EVF(const wchar_t* func, void* data);

  ON_XMLSegmentedStream(const ON_XMLSegmentedStream&) = delete;
  const ON_XMLSegmentedStream& operator = (const ON_XMLSegmentedStream&) = delete;

private:
  class CImpl;
  CImpl* m_impl;
  ON__UINT8 m_Impl[32];
};

class ON_CLASS ON_XMLNode
{
public:
  ON_XMLNode(const wchar_t* name); // Always creates empty default property.
  ON_XMLNode(const ON_XMLNode&);
  ON_XMLNode(ON_XMLNode&&) noexcept = delete;
  virtual ~ON_XMLNode();

  const ON_XMLNode& operator = (const ON_XMLNode&);

  bool MergeFrom(const ON_XMLNode& src); // src node must have the same name

  const ON_wString& TagName(void) const;
  void SetTagName(const wchar_t* name);

  static bool IsValidXMLNameWithDebugging(const wchar_t* name);
  static bool IsValidXMLName(const wchar_t* name);
  static ON_wString GetNameFromTag(const wchar_t* tag);

public: // Hierarchy.
  // Get the first child of this node or null if none.
  ON_XMLNode* FirstChild(void) const;

  // Get the previous sibling of this node or null if none.
  ON_XMLNode* PrevSibling(void) const;

  // Get the next sibling of this node or null if none.
  ON_XMLNode* NextSibling(void) const;

  // Get the parent of this node or null if none.
  ON_XMLNode* GetParent(void) const;

  // Get the topmost parent of this node or null if none.
  const ON_XMLNode& TopmostParent(void) const;

  // Recursively iterate over the children of this node, calling the supplied callback function for each child.
  // If the callback function returns false, this function will fail.
  // Returns true if successful, false on failure.
  bool RecurseChildren(ON_XMLRecurseChildrenCallback callback, void* data) const;

public: // Change data.
  // Adds a node as a child of this node. Takes ownership of node.
  // Returns a pointer to node for convenience.
  ON_XMLNode* AttachChildNode(ON_XMLNode* node);

  // DEPRECATED, use AttachChildNode()
  ON_DEPRECATED ON_XMLNode* AddChildNode(ON_XMLNode* node);

  // Attaches a property directly to the node. Takes ownership of the property.
  // Returns a pointer to the copy of the property.
  ON_XMLProperty* AttachProperty(ON_XMLProperty* prop);

  // Adds a property to the node. The property object is copied and ownership remains with the caller.
  // Returns a pointer to the copy of the property.
  ON_XMLProperty* AddProperty(const ON_XMLProperty& prop);

  // Removes and deletes this node.
  void Remove(void);

  // Removes and deletes the child node, if possible.
  // Returns true if successful, else false.
  bool RemoveChild(ON_XMLNode* child);

  // Removes and deletes the named property, if possible.
  // Returns true if successful, else false.
  bool RemoveProperty(const wchar_t* prop_name);

  // Removes the child node and passes ownership to the caller.
  // Returns the detached node or null on failure.
  ON_XMLNode* DetachChild(ON_XMLNode& child); // WAS UnhookChild

  // Removes and deletes all child nodes.
  void RemoveAllChildren(void);

  // Removes and deletes all properties and adds an empty default property.
  void RemoveAllProperties(void);

  // Removes and deletes all child nodes and properties, and clears the tag name.
  void Clear(void);

  // Moves this node before another node.
  void MoveBefore(ON_XMLNode& other);

  // Moves this node after another node.
  void MoveAfter(ON_XMLNode& other);

public: // Serialization.
  static constexpr ON__UINT32 ReadError = 0xFFFFFFFF; // Indicates ReadFromStream() failure.
  ON__UINT32 ReadFromStream(const wchar_t* buf, bool warnings_as_errors=false, bool validate_tags=true);

  void* LastReadBufferPointer(void) const;

  // This function is called on the top-most node during the reading process.
  virtual void OnNodeReadFromStream(const ON_XMLNode* node) const;

  ON_wString String(bool include_formatting=true, bool force_long_format=false, bool sorted_props=false) const;
  operator ON_wString() const;

  // Writes the node to a wide buffer (AKA 'stream') as XML text.
  // max_chars is the maximum number of wide characters that the buffer can accomodate.
  // To find out how much space is needed without actually writing anything, pass zero.
  // When writing to the buffer, a terminator is written if there is room for it, but the
  // terminator is not included in the returned number of characters.
  ON__UINT32 WriteToStream(wchar_t* stream, ON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;

  class ON_CLASS CharacterCounts
  {
  public:
    void operator += (const CharacterCounts& cw);

    // Number of characters that would have been written if the buffer was big enough.
    // This value does not include the terminator.
    ON__UINT32 m_logical = 0;

    // Number of characters that were physically written. Always zero if max_chars is zero.
    // It is otherwise usually the same as m_logical, but less when the buffer is too small.
    // This value does not include the terminator.
    ON__UINT32 m_physical = 0;

    ON__UINT64 m_reserved = 0;
  };

  CharacterCounts WriteToStreamEx      (wchar_t* stream, ON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;
  CharacterCounts WriteHeaderToStream  (wchar_t* stream, ON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;
  CharacterCounts WriteChildrenToStream(wchar_t* stream, ON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;
  CharacterCounts WriteFooterToStream  (wchar_t* stream, ON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false) const;

  bool WriteToSegmentedStream(ON_XMLSegmentedStream& stream, bool include_formatting=true, bool force_long_format=false, bool sorted_props=false) const;

public: // Utilities.
  int PropertyCount(void) const;
  int ChildCount(void) const;

  int GetNestedDepth(void) const;

  // Get the CRC of this node. This assumes the node has the following format:
  // <node prop1=value1 prop2=value2 ... >
  //   <param-name><type="float">1.23456</param-name>
  //   ...
  // </node>
  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  // Use this when you are looking for a node that is only one child below - non-recursive, but fast.
  ON_XMLNode* GetNamedChild(const wchar_t* name) const;

  ON_XMLProperty* GetNamedProperty(const wchar_t* name) const;

  ON_XMLProperty& GetDefaultProperty(void) const;

  // Gets at nodes deep into the tree using a slash delimited path - ie "child/grandchild/great-grandchild"
  // There's no checking for multiple nodes with the same name at each level of the tree, so if you use this
  // stuff, you have to make sure you have unique node names at each level.
  // relative to the current node, use "/" as a separator.

  ON_XMLNode* GetNodeAtPath(const wchar_t* path) const;
  ON_XMLNode* CreateNodeAtPath(const wchar_t* path);
  ON_wString GetPathFromRoot(void) const;

  // Emergency virtual function for future expansion.
  virtual void* EVF(const wchar_t* func, void* data);

public: // Iteration.
  class ON_CLASS ChildIterator
  {
  public:
    ChildIterator(const ON_XMLNode* parent);
    virtual ~ChildIterator();

    virtual ON_XMLNode* GetNextChild(void);

    // Emergency virtual function for future expansion.
    virtual void* EVF(const wchar_t* func, void* data);

  private:
    class CImpl;
    CImpl* m_impl;
    ON__UINT8 m_Impl[24];
  };

  class ON_CLASS PropertyIterator final
  {
  public:
    PropertyIterator(const ON_XMLNode* parent, bool sorted = false);
    ~PropertyIterator();

    ON_XMLProperty* GetNextProperty(void);

  private:
    class CImpl;
    CImpl* m_impl;
    ON__UINT8 m_Impl[64];
  };

  ChildIterator GetChildIterator(void) const;
  PropertyIterator GetPropertyIterator(bool alphabetized = false) const;

  static bool AutoTypePropValue(void);
  static void SetAutoTypePropValue(bool b=true);

  // For internal use only.
  void SetInternalDebuggingFlags(ON__UINT64);

private:
  class CImpl;
  CImpl* m_impl;
  ON__UINT8 m_Impl[168];
  CImpl& Impl(void) const;
};

class ON_CLASS ON_XMLRootNode : public ON_XMLNode
{
public:
  ON_XMLRootNode();
  ON_XMLRootNode(const ON_XMLRootNode&);
  virtual ~ON_XMLRootNode();

  const ON_XMLRootNode& operator = (const ON_XMLRootNode&);

  const ON_XMLRootNode& NodeForRead(void) const;
  ON_XMLRootNode& NodeForWrite(void);

  bool ReadFromFile(const wchar_t* path, bool warnings_as_errors=false, bool validate_tags=true);
  bool WriteToFile (const wchar_t* path, bool include_formatting=true, bool utf8=false, bool sorted_properties=false) const;

private:
  class CImpl;
  CImpl* m_impl;
  ON__UINT8 m_Impl[24];
  CImpl& Impl(void) const;
};

class ON_CLASS ON_XMLUserData : public ON_UserData
{
public:
  ON_XMLUserData();
  ON_XMLUserData(const ON_XMLUserData& ud);
  virtual ~ON_XMLUserData();

  const ON_XMLUserData& operator = (const ON_XMLUserData& ud);

  const ON_XMLRootNode& XMLRootForRead(void) const;
  ON_XMLRootNode& XMLRootForWrite(void) const;

  ON_XMLProperty* Property(const wchar_t* xml_path, const wchar_t* prop_name) const;

  ON_XMLVariant Value(const wchar_t* xml_path, const wchar_t* prop_name = L"") const;
  bool SetValue(const wchar_t* xml_path, const wchar_t* prop_name, const ON_XMLVariant& value);
  void SetValue(const wchar_t* xml_path, const ON_XMLVariant& value);

  void Clear(void) const;

  int Version(void) const;
  void _Dump(const wchar_t* wszFileName) const;

  virtual bool Archive(void) const override;

  virtual void SetToDefaults(void) const = 0;
  virtual void ReportVersionError(void) const = 0;

  virtual bool Write(ON_BinaryArchive&) const override;
  virtual bool Read(ON_BinaryArchive&) override;

  // Emergency virtual function for future expansion.
  virtual void* EVF(const wchar_t* wszFunc, void* pvData);

protected:
  ON_XMLProperty* InternalProperty(const wchar_t* xml_path, const wchar_t* prop_name) const;

public:
  void SetToDefaultsImpl(int) const;

private:
  class CImpl;
  CImpl* m_impl;
  ON__UINT8 m_Impl[224];
  CImpl& Impl(void) const;
};

ON_DECL const wchar_t* ON_StringFromUnits(ON::LengthUnitSystem units);
ON_DECL ON::LengthUnitSystem ON_UnitsFromString(const ON_wString& s);

class ON_CLASS ON_XMLParameters
{
public:
  ON_XMLParameters(ON_XMLNode& node);
  ON_XMLParameters(const ON_XMLNode& node);
  virtual ~ON_XMLParameters();

  void SetWriteTypeProperty(bool b);
  void SetDefaultReadType(const wchar_t* type);

  virtual ON_wString AsString(void) const;
  virtual void SetAsString(const wchar_t* s);

  virtual ON_XMLNode* SetParam(const wchar_t* param_name, const ON_XMLVariant& param_value);
  virtual bool GetParam(const wchar_t* param_name, ON_XMLVariant& param_value_out) const;

  class ON_CLASS CIterator
  {
  public:
    CIterator(const ON_XMLParameters& p);
    virtual ~CIterator();

    bool Next(ON_wString& param_name_out, ON_XMLVariant& param_value_out) const;
    virtual void* EVF(const wchar_t*, void*);

  private:
    class CImpl;
    CImpl* m_impl;
  };
  CIterator* NewIterator(void) const;

  virtual void* EVF(const wchar_t*, void*);

  ON_XMLParameters(const ON_XMLParameters&) = delete;
  const ON_XMLParameters& operator = (const ON_XMLParameters&) = delete;

protected:
  const ON_XMLNode& Node(void) const;
  virtual bool GetParamNode(const ON_XMLNode& node, ON_XMLVariant& param_value) const;
  virtual ON_XMLNode* SetParamNode(ON_XMLNode& node, const wchar_t* param_name, const ON_XMLVariant& param_value);
  virtual ON_XMLNode* ObtainChildNodeForWrite(ON_XMLNode& node, const wchar_t* param_name) const;

private:
  class CImpl;
  CImpl* m_impl;
};

class ON_CLASS ON_XMLParametersV8 : public ON_XMLParameters
{
public:
  ON_XMLParametersV8(ON_XMLNode& node);
  ON_XMLParametersV8(const ON_XMLNode& node);

  virtual bool GetParam(const wchar_t* param_name, ON_XMLVariant& param_value_out) const override;

  ON_XMLParametersV8(const ON_XMLParametersV8&) = delete;
  const ON_XMLParametersV8& operator = (const ON_XMLParametersV8&) = delete;

protected:
  virtual ON_XMLNode* ObtainChildNodeForWrite(ON_XMLNode& node, const wchar_t* param_name) const override;
  ON_XMLNode* FindNodeByNameProperty(const wchar_t* param_name) const;
};

ON_DECL bool ON_RunXMLTests(const wchar_t* test_folder);

class ON_CLASS ON_RdkUserData : public ON_XMLUserData
{
private:
  ON_OBJECT_DECLARE(ON_RdkUserData);

public:
  ON_RdkUserData();
  ON_RdkUserData(const ON_RdkUserData& ud);
  virtual ~ON_RdkUserData();

  const ON_RdkUserData& operator = (const ON_RdkUserData& ud);

  static ON_UUID Uuid(void);

  virtual void SetToDefaults(void) const override;
  virtual bool GetDescription(ON_wString& s) override;
  virtual bool Read(ON_BinaryArchive&) override;
  virtual bool Write(ON_BinaryArchive&) const override;
  virtual bool DeleteAfterWrite(const ON_BinaryArchive& archive, const ON_Object* parent_object) const override;
  virtual bool DeleteAfterRead(const ON_BinaryArchive& archive, ON_Object* parent_object) const override;
  virtual bool Transform(const ON_Xform& xform) override;
  virtual void ReportVersionError(void) const override;

  typedef bool (*ReadCallback)(ON_RdkUserData& ud);
  typedef bool (*WriteCallback)(const ON_RdkUserData& ud);
  typedef bool (*TransformCallback)(ON_RdkUserData& ud, const ON_Xform& xform);

  static void SetCallbacks(ReadCallback rc, WriteCallback wc, TransformCallback tc);
};

#endif
