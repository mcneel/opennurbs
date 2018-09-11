/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2012 Robert McNeel & Associates. All rights reserved.
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

#if !defined(ON_STRING_INC_)
#define ON_STRING_INC_


/*
Description:
  Sort an index array.
Parameters
  method - [in]
    ON::sort_algorithm::quick_sort (best in general) or ON::sort_algorithm::heap_sort.
    Use ON::sort_algorithm::heap_sort only after doing meaningful performance
    testing using optimized release builds that demonstrate
    ON::sort_algorithm::heap_sort is significantly better.
  index - [out] 
    Pass in an array of count integers.  The returned
    index[] is a permutation of (0,1,..,count-1)
    such that compare(B[index[i]],B[index[i+1]) <= 0
    where B[i] = base + i*sizeof_element
  base - [in]
    array of count elements
  count - [in]
    number of elements in the index[] and base[] arrays
  sizeof_element - [in]
    number of bytes between consecutive elements in the
    base[] array.
  compare - [in]
    Comparison function a la qsort().
*/
ON_DECL
void ON_Sort( 
  ON::sort_algorithm method,
  int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int (*compare)(const void*,const void*) // int compar(const void*,const void*)
  );

ON_DECL
void ON_Sort(
  ON::sort_algorithm method,
  unsigned int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int(*compare)(const void*, const void*) // int compar(const void*,const void*)
  );


/*
Description:
  Sort an index array using a compare function
  that takes an additional pointer that can be used to
  pass extra informtation.
Parameters
  method - [in]
    ON::sort_algorithm::quick_sort (best in general) or ON::sort_algorithm::heap_sort.
    Use ON::sort_algorithm::heap_sort only after doing meaningful performance
    testing using optimized release builds that demonstrate
    ON::sort_algorithm::heap_sort is significantly better.
  index - [out] 
    Pass in an array of count integers.  The returned
    index[] is a permutation of (0,1,..,count-1)
    such that compare(B[index[i]],B[index[i+1]) <= 0
    where B[i] = base + i*sizeof_element
  base - [in]
    array of count elements
  count - [in]
    number of elements in the index[] and base[] arrays
  sizeof_element - [in]
    number of bytes between consecutive elements in the
    base[] array.
  compare - [in]
    Comparison function a la qsort().  The context parameter
    is pass as the third argument.
  context - [in]
    pointer passed as the third argument to compare().
*/
ON_DECL
void ON_Sort( 
  ON::sort_algorithm method,
  int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int (*compare)(const void*,const void*,void*), // int compar(const void* a,const void* b, void* ptr)
  void* context
  );

ON_DECL
void ON_Sort(
  ON::sort_algorithm method,
  unsigned int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int(*compare)(const void*, const void*, void*), // int compar(const void* a,const void* b, void* ptr)
  void* context
  );

/*
Description:
  Various sorts. When in doubt, use ON_qsort().
  ON_qsort - quick sort.
  ON_hsort = hearp sort.
Parameters
  base - [in]
    array of count elements
  count - [in]
    number of elements in the index[] and base[] arrays
  sizeof_element - [in]
    number of bytes between consecutive elements in the
    base[] array.
  compare - [in]
    Comparison function a la qsort().  The context parameter
    is pass as the third argument.
  context - [in]
    pointer passed as the third argument to compare().
Remarks:
  As a rule, use quick sort unless extensive tests in your case
  prove that heap sort is faster. 
  
  This implementation of quick sort is generally faster than 
  heap sort, even when the input arrays are nearly sorted.
  The only common case when heap sort is faster occurs when
  the arrays are strictly "chevron" (3,2,1,2,3) or "carat" 
  (1,2,3,2,1) ordered, and in these cases heap sort is about
  50% faster.  If the "chevron" or "caret" ordered arrays 
  have a little randomness added, the two algorithms have 
  the same speed.
*/
ON_DECL
void ON_hsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(const void*,const void*)
        );

ON_DECL
void ON_qsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(const void*,const void*)
        );

ON_DECL
void ON_hsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(void*,const void*,const void*),
        void* context
        );

ON_DECL
void ON_qsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(void*,const void*,const void*),
        void* context
        );

/*
Description:
  Sort an array of doubles in increasing order in place.
Parameters:
  sort_algorithm - [in]  
    ON::sort_algorithm::quick_sort (best in general) or ON::sort_algorithm::heap_sort
    Use ON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
    a[] cannot contain NaNs.
  nel - [in]
    length of array a[]
*/
ON_DECL
void ON_SortDoubleArray(
  ON::sort_algorithm sort_algorithm,
  double* a,
  size_t nel
  );


/*
Description:
  Sort an array of doubles in increasing order in place.
Parameters:
  a - [in / out]
    The values in a[] are sorted so that a[i] <= a[i+1].
    a[] cannot contain NaNs.
  nel - [in]
    length of array a[]
*/
ON_DECL
void ON_SortDoubleArrayIncreasing(
  double* a,
  size_t nel
  );

/*
Description:
  Sort an array of doubles in increasing order in place.
Parameters:
  a - [in / out]
    The values in a[] are sorted so that a[i] >= a[i+1].
    a[] cannot contain NaNs.
  nel - [in]
    length of array a[]
*/
ON_DECL
void ON_SortDoubleArrayDecreasing(
  double* a,
  size_t nel
  );

/*
Description:
  Sort an array of ints in place.
Parameters:
  sort_algorithm - [in]  
    ON::sort_algorithm::quick_sort (best in general) or ON::sort_algorithm::heap_sort
    Use ON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
  nel - [in]
    length of array a[]
*/
ON_DECL
void ON_SortIntArray(
        ON::sort_algorithm sort_algorithm,
        int* a,
        size_t nel
        );

/*
Description:
  Sort an array of unsigned ints in place.
Parameters:
  sort_algorithm - [in]  
    ON::sort_algorithm::quick_sort (best in general) or ON::sort_algorithm::heap_sort
    Use ON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
  nel - [in]
    length of array a[]
*/
ON_DECL
void ON_SortUnsignedIntArray(
        ON::sort_algorithm sort_algorithm,
        unsigned int* a,
        size_t nel
        );

/*
Description:
  Sort an array of unsigned 64-bit ints in place.
Parameters:
  sort_algorithm - [in]  
    ON::sort_algorithm::quick_sort (best in general) or ON::sort_algorithm::heap_sort
    Use ON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
  nel - [in]
    length of array a[]
*/
ON_DECL
void ON_SortUINT64Array(
  ON::sort_algorithm sort_algorithm,
  ON__UINT64* a,
  size_t nel
);



/*
Description:
  Sort an array of unsigned null terminated char strings in place.
Parameters:
  sort_algorithm - [in]  
    ON::sort_algorithm::quick_sort (best in general) or ON::sort_algorithm::heap_sort
    Use ON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that strcmp(a[i],a[i+1]) <= 0.
  nel - [in]
    length of array a[]
*/
ON_DECL
void ON_SortStringArray(
        ON::sort_algorithm sort_algorithm,
        char** a,
        size_t nel
        );

ON_DECL
const int* ON_BinarySearchIntArray( 
          int key, 
          const int* base, 
          size_t nel
          );

ON_DECL
const unsigned int* ON_BinarySearchUnsignedIntArray( 
          unsigned int key, 
          const unsigned int* base, 
          size_t nel
          );

ON_DECL
const void* ON_BinarySearchArrayForUnsingedInt(
  unsigned int key, 
  const void* base, 
  size_t count,
  size_t sizeof_element,
  size_t key_offset
  );

ON_DECL
const double* ON_BinarySearchDoubleArray( 
          double key, 
          const double* base, 
          size_t nel
          );

/*
  This class is intended to be used to determine if a file's
  contents have changed.
*/
class ON_CLASS ON_CheckSum
{
public:
  ON_CheckSum();
  ~ON_CheckSum();

  static const ON_CheckSum UnsetCheckSum;

  // zeros all fields.
  void Zero();

  /*
  Returns:
    True if checksum is set.
  */
  bool IsSet() const;

  // C++ default operator=, operator==,
  // and copy constructor work fine.

  /*
  Descripton:
    Set check sum values for a buffer
  Parameters:
    size - [in] 
      number of bytes in buffer
    buffer - [in]  
    time - [in]
      last modified time in seconds since Jan 1, 1970, UCT
  Returns:
    True if checksum is set.
  */
  bool SetBufferCheckSum( 
    size_t size, 
    const void* buffer,
    time_t time
   );

  /*
  Descripton:
    Set check sum values for a file.
  Parameters:
    fp - [in] pointer to a file opened with ON:FileOpen(...,"rb")
  Returns:
    True if checksum is set.
  */
  bool SetFileCheckSum( 
    FILE* fp
   );

  /*
  Descripton:
    Set check sum values for a file.
  Parameters:
    filename - [in] name of file.
  Returns:
    True if checksum is set.
  */
  bool SetFileCheckSum( 
    const wchar_t* filename
   );

  /*
  Description:
    Test buffer to see if it has a matching checksum.
  Paramters:
    size - [in]   size in bytes
    buffer - [in]
  Returns:
    True if the buffer has a matching checksum.
  */
  bool CheckBuffer( 
    size_t size, 
    const void* buffer
    ) const;

  /*
  Description:
    Test buffer to see if it has a matching checksum.
  Paramters:
    fp - [in] pointer to file opened with ON::OpenFile(...,"rb")
    bSkipTimeCheck - [in] if true, the time of last
       modification is not checked.
  Returns:
    True if the file has a matching checksum.
  */
  bool CheckFile( 
    FILE* fp,
    bool bSkipTimeCheck = false
    ) const;

  /*
  Description:
    Test buffer to see if it has a matching checksum.
  Paramters:
    filename - [in]
    bSkipTimeCheck - [in] if true, the time of last
       modification is not checked.
  Returns:
    True if the file has a matching checksum.
  */
  bool CheckFile( 
    const wchar_t* filename,
    bool bSkipTimeCheck = false
    ) const;

  bool Write(class ON_BinaryArchive&) const;
  bool Read(class ON_BinaryArchive&);

  void Dump(class ON_TextLog&) const;

public:
  size_t     m_size;   // bytes in the file.
  time_t     m_time;   // last modified time in seconds since Jan 1, 1970, UCT
  ON__UINT32 m_crc[8]; // crc's
};


/*
Description:
  Get the length of a UTF-8 encoded char string.
Parameters:
  string - [in]
    null terminated char string.
Returns:
  Number of nonzero char elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
ON_DECL
int ON_StringLengthUTF8(
  const char* string
  );

/*
Description:
  Get the length of a UTF-16 encoded ON__UINT16 string.
Parameters:
  string - [in]
    null terminated ON__UINT16 string.
Returns:
  Number of nonzero ON__UINT16 elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
ON_DECL
int ON_StringLengthUTF16(
  const ON__UINT16* string
  );

/*
Description:
  Get the length of a UTF-32 encoded ON__UINT32 string.
Parameters:
  string - [in]
    null terminated ON__UINT32 string.
Returns:
  Number of nonzero ON__UINT32 elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
ON_DECL
int ON_StringLengthUTF32(
  const ON__UINT32* string
  );

/*
Description:
  Get the length of a wchar_t string.
Parameters:
  string - [in]
    null terminated wchar_t string.
Returns:
  Number of nonzero wchar_t elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
ON_DECL
int ON_StringLengthWideChar(
  const wchar_t* string
  );

/*
Description:
  Get the length of a UTF-8 encoded char string.
Parameters:
  string - [in]
    null terminated char string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
ON_DECL
int ON_StringLengthUTF8(
  const char* string,
  size_t string_capacity
  );

/*
Description:
  Get the length of a UTF-16 encoded ON__UINT16 string.
Parameters:
  string - [in]
    null terminated ON__UINT16 string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
ON_DECL
int ON_StringLengthUTF16(
  const ON__UINT16* string,
  size_t string_capacity
  );

/*
Description:
  Get the length of a UTF-32 encoded ON__UINT32 string.
Parameters:
  string - [in]
    null terminated ON__UINT32 string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
ON_DECL
int ON_StringLengthUTF32(
  const ON__UINT32* string,
  size_t string_capacity
  );

/*
Description:
  Get the length of a wchar_t string.
Parameters:
  string - [in]
    null terminated wchar_t string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
ON_DECL
int ON_StringLengthWideChar(
  const wchar_t* string,
  size_t string_capacity
  );

/*
Description:
  Compare of UTF-8 encoded char strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     ON_StringCompareOrdinalUTF8, ON_StringCompareOrdinalUTF16 and ON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
  4) This function does not compare Unicode code point values the strings have eny elements with values > 127.
*/
ON_DECL
int ON_StringCompareOrdinalUTF8(
  const char* string1,
  int element_count1,
  const char* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/*
Description:
  Compare of UTF-16 encoded ON__UINT16 strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     ON_StringCompareOrdinalUTF8, ON_StringCompareOrdinalUTF16 and ON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
  4) This function does not compare Unicode code point values if the strings have any surrogate pairs.
*/
ON_DECL
int ON_StringCompareOrdinalUTF16(
  const ON__UINT16* string1,
  int element_count1,
  const ON__UINT16* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/*
Description:
  Compare of UTF-32 encoded ON__UINT32 strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     ON_StringCompareOrdinalUTF8, ON_StringCompareOrdinalUTF16 and ON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
*/
ON_DECL
int ON_StringCompareOrdinalUTF32(
  const ON__UINT32* string1,
  int element_count1,
  const ON__UINT32* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/*
Description:
  Compare wchar_t strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     ON_StringCompareOrdinalUTF8, ON_StringCompareOrdinalUTF16 and ON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
  4) This function assumes the sizeof(wchar_t) is 1, 2 or 4,
     that 1 bytes wchar_t strings are UTF-8 encoded, 2 byte wchar_t strings are UTF-16 encoded
     and 4 bytes wchar_t strings are UTF-32 encoded.
*/
ON_DECL
int ON_StringCompareOrdinalWideChar(
  const wchar_t* string1,
  int element_count1,
  const wchar_t* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/////////////////////////////////////////////////////////////////////////////
// 
// ON_String is a UTF-8 char string on all platforms
// ON_wString is a UTF-16 encoded wchar_t string on Windows platforms
// ON_wString is a UTF-32 encoded wchar_t string on Windows platforms
//

class ON_CLASS ON_StringBuffer
{
public:
  ON_StringBuffer();

  ON_StringBuffer(
    char* stack_buffer,
    size_t stack_buffer_capacity
    );

  ~ON_StringBuffer();

  bool GrowBuffer(
    size_t buffer_capacity
    );

  char* m_buffer;
  size_t m_buffer_capacity;

private:
  ON_StringBuffer(const ON_StringBuffer&);
  ON_StringBuffer& operator=(const ON_StringBuffer&);
  char* m_heap_buffer;
  size_t m_heap_buffer_capacity;
};

class ON_CLASS ON_wStringBuffer
{
public:
  ON_wStringBuffer();

  ON_wStringBuffer(
    wchar_t* stack_buffer,
    size_t stack_buffer_capacity
    );

  ~ON_wStringBuffer();

  bool GrowBuffer(
    size_t buffer_capacity
    );

  wchar_t* m_buffer;
  size_t m_buffer_capacity;

private:
  ON_wStringBuffer(const ON_wStringBuffer&);
  ON_wStringBuffer& operator=(const ON_wStringBuffer&);
  wchar_t* m_heap_buffer;
  size_t m_heap_buffer_capacity;
};

ON_DECL
ON__UINT32 ON_UnicodeMapCodePointOrdinal(
  ON_StringMapOrdinalType map_type,
  ON__UINT32 unicode_code_point
  );

ON_DECL
ON__UINT32 ON_UnicodeMapCodePoint(
  const ON_Locale& locale,
  ON_StringMapType map_type,
  ON__UINT32 unicode_code_point
  );

/*
Parameters:
  sUTF8 - [in]
    pointer to a UTF-8 encoded string.
  element_count - [in]
    number of char elements to parse.
    if element_count < 0, then sUTF8 must be null terminated
  mapping - [in]
    mapping to apply to unicode code points
Returns:
  A SHA-1 value of the sequence of unicode code points. This value is independent
  of platform endian or UTF encoding.
*/
const ON_SHA1_Hash ON_StringContentHash(
  const char* sUTF8,
  int element_count,
  ON_StringMapOrdinalType mapping
);

/*
Parameters:
  sUTF8 - [in]
    pointer to a UTF-8 encoded string.
  element_count - [in]
    number of char elements to parse.
    if element_count < 0, then sUTF8 must be null terminated
  mapping - [in]
    mapping to apply to unicode code points
Returns:
  A SHA-1 value of the sequence of unicode code points. This value is independent
  of platform endian or UTF encoding.
*/
const ON_SHA1_Hash ON_StringContentHash(
  const wchar_t* sWideString,
  int element_count,
  ON_StringMapOrdinalType mapping
);

class ON_CLASS ON_String
{
public:

// Constructors
	ON_String() ON_NOEXCEPT;
	ON_String( const ON_String& );

  // ON_String::EmptyString has length 0.
  // const char* s = ON_String::EmptyString sets s to "".
  static const ON_String EmptyString;

  static const char Backspace;       // Unicode BACKSPACE control U+0008
  static const char Tab;             // Unicode CHARACTER TABULATION control U+0009
  static const char LineFeed;        // Unicode LINE FEED control U+000A
  static const char VerticalTab;     // Unicode LINE TABULATION control U+000B
  static const char FormFeed;        // Unicode FORM FEED control U+000C
  static const char CarriageReturn;  // Unicode CHARACTER TABULATION control U+000D
  static const char Escape;          // Unicode CARRIAGE RETURN control U+001B
  static const char Space;           // Unicode SPACE U+0020
  static const char Slash;           // Unicode SOLIDUS U+002F
  static const char Backslash;       // Unicode REVERSE SOLIDUS U+005C
  static const char Pipe;            // Unicode VERTICAL LINE U+007C
    
private:
  // Use IsEmpty() or IsNotEmpty() when you want a bool 
  // to test for the empty string.
  explicit operator bool() const { return IsNotEmpty(); }
public:

#if defined(ON_HAS_RVALUEREF)
  // Clone constructor
  ON_String( ON_String&& ) ON_NOEXCEPT;

  // Clone Assignment operator
  ON_String& operator=( ON_String&& ) ON_NOEXCEPT;
#endif

	ON_String( const char* );
	ON_String( const char*, int /*length*/ );        // from substring
	ON_String( char, int = 1 /* repeat count */ );   

	ON_String( const unsigned char* );
	ON_String( const unsigned char*, int /*length*/ );        // from substring
	ON_String( unsigned char, int = 1 /* repeat count */ ); 
  
  // construct a UTF-8 string string from a UTF-16 string.
	ON_String( const wchar_t* src );  // src = UTF-16 string
	ON_String( const wchar_t* src, int length ); // from a UTF-16 substring
  ON_String( const ON_wString& src ); // src = UTF-16 string

#if defined(ON_RUNTIME_WIN)
  // Windows support
	bool LoadResourceString( HINSTANCE, UINT); // load from Windows string resource
										                         // 2047 chars max
#endif

  void Create();
  void Destroy(); // releases any memory and initializes to default empty string
  void EmergencyDestroy();

  /*
  Description:
    Enables reference counting.  I limited cases, this is useful 
    for large strings or strings that are frequently passed around.
    Reference counted strings must be carefully managed in
    when multi-threading is used.
  Parameters:
    If EnableReferenceCounting()
    is not called, then the string will not be referanceThe default is to not use
    reference counted strings.
  */
  void EnableReferenceCounting( bool bEnable );

  /*
  Returns:
    True if the string is reference counted.
  */
  bool IsReferenceCounted() const;


  // Attributes & Operations

  /*
  Returns:
    number of nonzero elements in string.
  */
  int Length() const;

  /*
  Returns:
    number of nonzero elements in the string.
  */
  unsigned int UnsignedLength() const;

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static int Length(
    const char* string
    );

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static unsigned int UnsignedLength(
    const char* string
    );

  bool IsEmpty() const; // returns true if length == 0 
  bool IsNotEmpty() const; // returns true if length > 0
  void Empty();   // sets length to zero - if possible, memory is retained

	char& operator[](int);
	char operator[](int) const;
  char GetAt(int) const;
	void SetAt(int, char);
	void SetAt(int, unsigned char);
	
  operator const char*() const;

	// overloaded assignment
	ON_String& operator=(const ON_String&);
	ON_String& operator=(char);
	ON_String& operator=(const char*);
	ON_String& operator=(unsigned char);
	ON_String& operator=(const unsigned char*);
	ON_String& operator=(const wchar_t* src); // src = Wide char string, result is a UTF-8 string
	ON_String& operator=(const ON_wString& src);  // src = Wide char string, result is a UTF-8 string

  // operator+()
  ON_String operator+(const ON_String&) const;
  ON_String operator+(char) const;
  ON_String operator+(unsigned char) const;
  ON_String operator+(const char*) const;
  ON_String operator+(const unsigned char*) const;

  // string concatenation
  void Append( const char*, int ); // append specified number of characters
  void Append( const unsigned char*, int ); // append specified number of characters
	const ON_String& operator+=(const ON_String&);
	const ON_String& operator+=(char);
	const ON_String& operator+=(unsigned char);
	const ON_String& operator+=(const char*);
	const ON_String& operator+=(const unsigned char*);

  ON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
	int Compare( const char* ) const;

  ON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
  int Compare( const unsigned char* ) const;

  ON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
  int CompareNoCase( const char* ) const;

  ON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
  int CompareNoCase( const unsigned char* ) const;

  bool Equal(
    const ON_String& other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;

  bool Equal(
    const char* other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;

  static bool Equal(
    const char* string1,
    const char* string2,
    const class ON_Locale& locale,
    bool bIgnoreCase
    );

  static bool Equal(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    const class ON_Locale& locale,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_string by normalizing (NFC) 
    and using invariant culture ordering.
  Parameters:
    other_string - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  int Compare(
    const ON_String& other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;
 
  int Compare(
    const char* other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;
 
  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    string2 - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  static int Compare(
    const char* string1,
    const char* string2,
    const class ON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of string1[] elements to compare.
      If element_count1 < 0, then string1 must be null terminated.
    string2 - [in]
    element_count2 - [in]
      The number of string2[] elements to compare.
      If element_count2 < 0, then string2 must be null terminated.
    locale - [in]
      Typically ON_Locale::Ordinal, ON_Locale::InvariantCulture, or
      ON_Locale::CurrentCulture.
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */
  static int Compare(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    const class ON_Locale& locale,
    bool bIgnoreCase
    );

  bool EqualOrdinal(
    const ON_String& other_string,
    bool bOrdinalIgnoreCase
    ) const;

  bool EqualOrdinal(
    const char* other_string,
    bool bOrdinalIgnoreCase
    ) const;

  static bool EqualOrdinal(
    const char* string1,
    const char* string2,
    bool bOrdinalIgnoreCase
    );

  static bool EqualOrdinal(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_string unsigned byte by unsigned byte.
  Parameters:
    other_string - [in]
    bIgnoreCase - [in]
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    imposible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */  
  int CompareOrdinal(
     const ON_String& other_string,
     bool bIgnoreCase
     ) const;
 
  int CompareOrdinal(
     const char* other_string,
     bool bIgnoreCase
     ) const;
 
  /*
  Description:
    Compare string1 and string2 unsigned byte by unsigned byte.
  Parameters:
    string1 - [in]
    string2 - [in]
    bIgnoreCase - [in]
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    imposible to ignore case for any other values in a UTF-8 ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const char* string1,
    const char* string2,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare string1 and string2 unsigned byte by unsigned byte.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of elements in string1[] to compare.
      If element_count1 < 1, string1 must be null terminated and every element
      before the null terminator will be compared.
    string2 - [in]
    element_count2 - [in]
      The number of elements in string2[] to compare.
      If element_count2 < 1, string2 must be null terminated and every element
      before the null terminator will be compared.
    bOrdinalIgnoreCase - [in]
      If true, elements with values a-z are compared as if they had values A-Z.
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    imposible to ignore case for any other values in a UTF-8 ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_path as file system paths using
    appropriate tests for the current operating system. 
  Parameters:
    other_path - [in]
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  int ComparePath(
    const char* other_path
    ) const;

  bool EqualPath(
    const char* other_path
    ) const;

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
      null terminated string
    path2 - [in]
      null terminated string
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  static int ComparePath(
    const char* path1,
    const char* path2
    );

  static bool EqualPath(
    const char* path1,
    const char* path2
    );

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
    maximum_element_count1 - [in]
    path2 - [in]
    maximum_element_count2 - [in]
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  static int ComparePath(
    const char* path1,
    int  element_count1,
    const char* path2,
    int element_count2
    );

  static bool EqualPath(
    const char* path1,
    int  element_count1,
    const char* path2,
    int element_count2
    );

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like ON_3dmObjectAttributes.m_name, ON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    other_name - [in]
      null terminated string
  */
  int CompareAttributeName(
    const char* other_name
    ) const;
  bool EqualAttributeName(
    const char* other_name
    ) const;

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like ON_3dmObjectAttributes.m_name, ON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    name1 - [in]
      null terminated string
    name2 - [in]
      null terminated string
  */
  static int CompareAttributeName(
    const char* name1,
    const char* name2
    );
  static bool EqualAttributeName(
    const char* name1,
    const char* name2
    );

  // Description:
  //   Simple case sensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   mathes zero or more occurances of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string mathes the wild card pattern.
	bool WildCardMatch( const char* ) const;
	bool WildCardMatch( const unsigned char* ) const;

  // Description:
  //   Simple case insensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   mathes zero or more occurances of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string mathes the wild card pattern.
	bool WildCardMatchNoCase( const char* ) const;
	bool WildCardMatchNoCase( const unsigned char* ) const;

  /*
  Description:
    Replace all substrings that match token1 with token2
  Parameters:
    token1 - [in]
    token2 - [in]
  Returns:
    Number of times token1 was replaced with token2.
  */
  int Replace( const char* token1, const char* token2 );
  int Replace( const unsigned char* token1, const unsigned char* token2 );
  int Replace(char utf8_single_byte_c1, char utf8_single_byte_c2);
  int Replace(unsigned char utf8_single_byte_c1, unsigned char utf8_single_byte_c2);


	// simple sub-string extraction
	ON_String Mid(
    int, // index of first char
    int  // count
    ) const;
	ON_String Mid(
    int // index of first char
    ) const;
	ON_String Left(
    int // number of chars to keep
    ) const;
	ON_String Right(
    int // number of chars to keep
    ) const;

  /*
  Description:
    Map a single byte UTF-8 element to upper or lower case.
  Parameters:
    c - [in]
      If c is in the range A to Z or a to z, the map specified
      by map_type is applied.  All other values of c are
      unchanged.   
  Remarks:
    MapCharacterOrdinal is not appropriate for general string mapping.
  */
  static char MapCharacterOrdinal(
    ON_StringMapOrdinalType map_type,
    char c
    );

  ON_String MapStringOrdinal(
    ON_StringMapOrdinalType map_type
    ) const;


  /*
  Description:
    Map a wchar_t string.
  Parameters:
    map_type - [in]
    string - [in]
    element_count - [in]
      number of string[] elements to map.
      If element_count < 0, then ON_wString::Length(string) elements are mapped.
    mapped_string - [out]
    mapped_string_capacity - [in]
      number of available elements in mapped_string[].
    map_type - [in]
  Returns:
    Number of mapped_string[] elements that were mapped from string[].
    
    When the number of string[] input elements is >= mapped_string_capacity,
    mapped_string_capacity mapped_string[] elements are set and
    mapped_string_capacity is returned.

    When the return value is < mapped_string_capacity, a null terminator
    is appended after the last mapped element.
  */
  static int MapStringOrdinal(
    ON_StringMapOrdinalType map_type,
    const char* string,
    int element_count,
    char* mapped_string,
    int mapped_string_capacity
    );

  ON_String MapString(
    const class ON_Locale& locale,
    ON_StringMapType map_type
    ) const;

  static ON_String MapString(
    const class ON_Locale& locale,
    ON_StringMapType map_type,
    const char* string,
    int element_count
    );

  /*
  Description:
    Maps element_count elements of string[] to mapped_string[].

  Parameters:
    locale - [in]
      Locale to use when converting case. It is common to pass one of
      the preset locales ON_Locale::Ordinal, ON_Locale::InvariantCulture, 
      or ON_Locale::m_CurrentCulture. 

    map_type - [in]
      selects the mapping

    string - [in]
      input string to map.

    element_count - [in]
      The number of char elements to map from input string[].
      
      If element_count < 1, then string[] must be null terminated and
      ON_wString::Length(string)+1 elements are mapped. 
      The +1 insures the output is null terminated.

    mapped_string - [out]
      The result of the mapping is returned in mapped_string[].

    mapped_string_capacity - [in]
      Number of char elements available in mapped_string[]
      or 0 to calculate the minimum number of elements needed
      for the mapping.

  Returns:
    If mapped_string_capacity > 0, then the number elements set in mapped_string[]
    is returned.  

    If mapped_string_capacity == 0, then the number elements required to perform
    the mapping is returned.

    When there is room, mapped_string[] is null terminated.

    0: Failure.
  */
  static int MapString(
    const class ON_Locale& locale,
    ON_StringMapType map_type,
    const char* string,
    int element_count,
    char* mapped_string,
    int mapped_string_capacity
    );

	// upper/lower/reverse conversion
	/*ON_DEPRECATED */ void MakeUpper();
	/*ON_DEPRECATED */void MakeLower();
	void MakeUpperOrdinal();
	void MakeLowerOrdinal();

  const ON_SHA1_Hash ContentHash(
    ON_StringMapOrdinalType mapping
  ) const;

  ON_String Reverse() const;

  static char* Reverse(
    char* string,
    int element_count
    );

	void MakeReverse();

  void TrimLeft(const char* = nullptr);
  void TrimRight(const char* = nullptr);
  void TrimLeftAndRight(const char* = nullptr);

  // remove occurrences of chRemove
  int Remove(const char utf8_single_byte_c);

	// searching (return starting index, or -1 if not found)
	// look for a single character match
	int Find(
    char utf8_single_byte_c
    ) const;
	int Find(
    unsigned char utf8_single_byte_c
    ) const;
	int ReverseFind(
    char utf8_single_byte_c
    ) const;
	int ReverseFind(
    unsigned char utf8_single_byte_c
    ) const;

	// look for a specific sub-string
  int Find(
    const char*
    ) const;
  int Find(
    const unsigned char*
    ) const;
  int Find(
    const char*,
    int start_index
    ) const;
  int Find(
    const unsigned char*,
    int start_index
  ) const;
  int ReverseFind(
    const char*
    ) const;
  int ReverseFind(
    const unsigned char*
    ) const;

  // It is common to format single numbers into strings
  // and the FromNumber and FromDouble functions are
  // the fastest way to do this and provide consistent results.
  // They return a pointer to their buffer so the can be used
  // as function parameters.
  static const ON_String FromNumber(
    char n
    );
  static const ON_String FromNumber(
    unsigned char n
    );
  static const ON_String FromNumber(
    short n
    );
  static const ON_String FromNumber(
    unsigned short n
    );
  static const ON_String FromNumber(
    int n
    );
  static const ON_String FromNumber(
    unsigned int n
    );
  static const ON_String FromNumber(
    ON__INT64 n
    );
  static const ON_String FromNumber(
    ON__UINT64 n
    );
  static const ON_String FromNumber(
    double d // "%g" format
    );
  static const ON_String ApproximateFromNumber(
    double d // "%f" when possible, otherwise "%g"
    );
  static const ON_String PreciseFromNumber(
    double d // "%.17g"
    );


  /*
  Description:
    Each byte value is converted to 2 hexadecimal digits.
  Parameters:
    bytes - [in]
      list of byte values
    byte_count - [in]
      Number of byte values in bytes[] array.
    bCapitalDigits - [in]
      false: Use 0-9, a - b
      true: Use 0-9, A - F
    bReverse - [in]
      false: 
        The digist in the string will be in the order
        bytes[0], bytes[1], ..., bytes[byte_count-1].
      true: 
        The digist in the string will be in the order
        bytes[byte_count-1], ..., bytes[1], bytes[0].
  */
  static const ON_String HexadecimalFromBytes(
    const ON__UINT8* bytes,
    size_t byte_count,
    bool bCapitalDigits,
    bool bReverse
    ); 

  /*
  Parameters:
    format - [in]
      Format control.  
      Positional paramters of the form %N$x where N >= 1 and x
      is the standard format specification are supported.
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
      arguments for replacable items in the format string.
  Returns:
    True if successful.
    False if the string is too long or the format string is not valid.
  Remarks:
    When using Microsoft's compiler and other compilers that provide similar
    locale support, the locale is the invariant culture locale returned by
    ON_Locale::InvariantCulture::LocalePtr().

    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly reccommended that you never use %S to
    include any string that may possibly contain elements with values > 127.
    The following examples illustrate a way to predictably use UTF-8 and wchar_t
    parameters in buffers of the other element type.

    const char* utf8_string = ...;
    // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
    // The code below will treat utf8_string as a UTF-8 encoded string.
    wchar_t wchar_buffer[...];
    const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
    ON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", ON_wString(utf8_string));

    const wchar_t* wide_string = ...;
    // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",char_string);
    // The code below will include wide_string as a UTF-8 encoded string.
    char char_buffer[...];
    const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
    ON_String::Format(char_buffer, char_buffer_capacity, "%s", ON_String(wide_string));
  */
  bool ON_VARGS_FUNC_CDECL Format(
    const char* format,
    ...
    );

  bool ON_VARGS_FUNC_CDECL Format(
    const unsigned char* format,
    ...
    );

  static const ON_wString ON_VARGS_FUNC_CDECL FormatToString(
    const char* format,
    ...
    );

  bool FormatVargs(
    const char* format, 
    va_list args
    );

  bool FormatVargs(
    const unsigned char* format,
    va_list args
    );

  /*
  Description:
    A platform independent, secure, culture invariant way to format a char string. 
    This function is provide to be used when it is critical that 
    the formatting be platform independent, secure and culture invarient.
  Parameters:
    buffer - [out] 
      not null
    buffer_capacity - [in]
      > 0
      Number of char elements in buffer.
    sFormat - [in]
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
  Returns:
    >= 0: 
      The number of char elements written to buffer[], not including the null terminator.
      A null terminator is always added (buffer[returned value] = 0).
      The last element of buffer[] is always set to zero (buffer[buffer_capacity-1] = 0).
    < 0: failure:
      If buffer is not null and buffer_capacity > 0, then buffer[0] = 0 and buffer[buffer_capacity-1] = 0;
  Remarks:
    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly reccommended that you never use %S to
    include any string that may possibly contain elements with values > 127.  
    The following examples illustrate a way to predictably use UTF-8 and wchar_t 
    parameters in buffers of the other element type.

         const char* utf8_string = ...;
         // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
         wchar_t wchar_buffer[...];
         const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
         ON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", ON_wString(utf8_string));

         const wchar_t* wide_string = ...;
         // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",char_string);
         char char_buffer[...];
         const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
         ON_String::Format(char_buffer, char_buffer_capacity, "%s", ON_String(wide_string));
  */
  static int ON_VARGS_FUNC_CDECL FormatIntoBuffer(
    char* buffer, 
    size_t buffer_capacity,
    const char* format,
    ...
    );

  static int ON_VARGS_FUNC_CDECL FormatIntoBuffer(
    ON_StringBuffer& buffer,
    const char* format,
    ...
    );

  static int FormatVargsIntoBuffer(
    char* buffer,
    size_t buffer_capacity,
    const char* format,
    va_list args
    );

  static int FormatVargsIntoBuffer(
    ON_StringBuffer& buffer,
    const char* format,
    va_list args
    );

  /*
  Returns:
    >= 0: 
      Number of char elements in the formatted string, not including the null terminator.
    < 0:
      Invalid input
  */
  static int FormatVargsOutputCount(
    const char* format,
    va_list args
    );

  /*
  Parameters:
    format - [in]
      null terminated string to scan
    ... - [out]
      pointers to elements to assign.
  Returns:
    >= 0: number of fields successfully converted and assigned.
    <0:  failure
  */
  int ON_VARGS_FUNC_CDECL Scan(
    const char* format,
    ...
    );

  int ON_VARGS_FUNC_CDECL Scan(
    const unsigned char* format,
    ...
    );

  static int ON_VARGS_FUNC_CDECL ScanBuffer(
    const char* buffer,
    const char* format,
    ...
    );

  static int ON_VARGS_FUNC_CDECL ScanBuffer(
    const unsigned char* buffer,
    const unsigned char* format,
    ...
    );

  static int ScanBufferVargs(
    const char* buffer,
    const char* format,
    va_list args
    );

  static int ScanBufferVargs(
    const unsigned char* buffer,
    const unsigned char* format,
    va_list args
    );


  /*
  Parameters:
    buffer - [in]
      decimal number
  Returns:
    not zero:
      pointer to the first character that was not scanned
    nullptr:
      failure
  */
  static const char* ToNumber(
    const char* buffer,
    char value_on_failure,
    char* value
    );
  static const char* ToNumber(
    const char* buffer,
    unsigned char value_on_failure,
    unsigned char* value
    );
  static const char* ToNumber(
    const char* buffer,
    short value_on_failure,
    short* value
    );
  static const char* ToNumber(
    const char* buffer,
    unsigned short value_on_failure,
    unsigned short* value
    );
  static const char* ToNumber(
    const char* buffer,
    int value_on_failure,
    int* value
    );
  static const char* ToNumber(
    const char* buffer,
    unsigned int value_on_failure,
    unsigned int* value
    );
  static const char* ToNumber(
    const char* buffer,
    ON__INT64 value_on_failure,
    ON__INT64* value
    );
  static const char* ToNumber(
    const char* buffer,
    ON__UINT64 value_on_failure,
    ON__UINT64* value
    );
  static const char* ToNumber(
    const char* buffer,
    double value_on_failure,
    double* value
    );

  
  // Low level access to string contents as character array
	char* ReserveArray(size_t); // make sure internal array has at least
                          // the requested capacity.
	void ShrinkArray();     // shrink internal storage to minimum size
  char* SetLength(size_t);    // set length (<=capacity)
  char* Array();
  const char* Array() const;

  /*
  Returns:
    Total number of bytes of memory used by this class.
    (For use in ON_Object::SizeOf() overrides.
  */
  unsigned int SizeOf() const;

  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  /*
  OBSOLETE - use ON_FileSystemPath::SplitPath
  */
  static void SplitPath( 
    const char* path,
    ON_String* drive,
    ON_String* dir,
    ON_String* fname,
    ON_String* ext
    );

public:
	~ON_String();

protected:
  // Implementation
	char* m_s; // pointer to ref counted string array
             // m_s - 12 bytes points at the string's ON_aStringHeader

	// implementation helpers
	struct ON_aStringHeader* Header() const;
	char* CreateArray(int);
  void CopyArray();
  void CopyToArray( const ON_String& );
  void CopyToArray( int, const char* );
  void CopyToArray( int, const unsigned char* );
  void CopyToArray( int, const wchar_t* );
  void AppendToArray( const ON_String& );
  void AppendToArray( int, const char* );
  void AppendToArray( int, const unsigned char* );
};

/*
Returns:
  True if lhs and rhs are identical as arrays of char elements.  
*/
ON_DECL
bool operator==( const ON_String& lhs, const ON_String& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of char elements.  
*/
ON_DECL
bool operator!=(const ON_String& lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator<(const ON_String& lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator>(const ON_String& lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator<=(const ON_String& lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator>=(const ON_String& lhs, const ON_String& rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of char elements.  
*/
ON_DECL
bool operator==( const ON_String& lhs, const char* rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of char elements.  
*/
ON_DECL
bool operator!=(const ON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is less than rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator<(const ON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator>(const ON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator<=(const ON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator>=(const ON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of char elements.  
*/
ON_DECL
bool operator==( const char* lhs, const ON_String& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of char elements.  
*/
ON_DECL
bool operator!=(const char* lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator<(const char* lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator>(const char* lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator<=(const char* lhs, const ON_String& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of unsigned bytes.
*/
ON_DECL
bool operator>=(const char* lhs, const ON_String& rhs);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// ON_wString
//

class ON_CLASS ON_wString
{
public:

  // ON_String::EmptyString has length 0.
  // const char* s = ON_String::EmptyString sets s to L"".
  static const ON_wString EmptyString;

  static const wchar_t Backspace;       // Unicode BACKSPACE control U+0008
  static const wchar_t Tab;             // Unicode CHARACTER TABULATION control U+0009
  static const wchar_t LineFeed;        // Unicode LINE FEED control U+000A
  static const wchar_t VerticalTab;     // Unicode LINE TABULATION control U+000B
  static const wchar_t FormFeed;        // Unicode FORM FEED control U+000C
  static const wchar_t CarriageReturn;  // Unicode CARRIAGE RETURN control U+000D
  static const wchar_t Escape;          // Unicode CARRIAGE RETURN control U+001B
  static const wchar_t Space;           // Unicode SPACE U+0020
  static const wchar_t Slash;           // Unicode SOLIDUS U+002F
  static const wchar_t Backslash;       // Unicode REVERSE SOLIDUS U+005C
  static const wchar_t Pipe;            // Unicode VERTICAL LINE U+007C

#if defined(ON_SIZEOF_WCHAR_T) && ON_SIZEOF_WCHAR_T >= 2
  // Never cast these values as "char"
  // The UTF-8 representation of any Unicode code point with value > 127
  // requires multiple bytes.
  static const wchar_t RadiusSymbol;    // Unicode LATIN CAPITAL LETTER R U+0052
  static const wchar_t DegreeSymbol;    // Unicode DEGREE SIGN U+00B0
  static const wchar_t PlusMinusSymbol; // Unicode PLUS-MINUS SIGN U+00B1
  static const wchar_t DiameterSymbol;  // Unicode LATIN CAPITAL LETTER O WITH STROKE U+00D8
  static const wchar_t RecyclingSymbol; // Unicode UNIVERSAL RECYCLING SYMBOL U+2672 (decimal 9842)
  static const wchar_t ReplacementCharacter; // Unicode REPLACEMENT CHARACTER U+FFFD
  static const wchar_t NextLine;             // Unicode NEXT LINE (NEL) U+0085
  static const wchar_t LineSeparator;        // LINE SEPARATOR U+2028 unambiguous line separator
  static const wchar_t ParagraphSeparator;   // PARAGRAPH SEPARATOR U+2028 unambiguous paragraph separator
  static const wchar_t NoBreakSpace; // NO-BREAK SPACE (NBSP)
  static const wchar_t NarrowNoBreakSpace; // NARROW NO-BREAK SPACE (NNBSP)
  static const wchar_t ZeroWidthSpace; // ZERO WIDTH SPACE (ZWSP)
#endif

private:
  // Use IsEmpty() or IsNotEmpty() when you want a bool 
  // to test for the empty string.
  explicit operator bool() const { return IsNotEmpty(); }
public:

// Constructors
	ON_wString() ON_NOEXCEPT;
	ON_wString( const ON_wString& );

#if defined(ON_HAS_RVALUEREF)
  // Clone constructor
  ON_wString( ON_wString&& ) ON_NOEXCEPT;

  // Clone Assignment operator
  ON_wString& operator=( ON_wString&& ) ON_NOEXCEPT;
#endif

	ON_wString( const ON_String& src ); // src = UTF-8 string

	ON_wString( const char* src ); // src = nul; terminated UTF-8 string
	ON_wString( const char* src, int /*length*/ );  // from UTF-8 substring
	ON_wString( char, int = 1 /* repeat count */ );   

	ON_wString( const unsigned char* src); // src = nul; terminated UTF-8 string
	ON_wString( const unsigned char*src, int /*length*/ );        // from UTF-8 substring
	ON_wString( unsigned char, int = 1 /* repeat count */ ); 
  
	ON_wString( const wchar_t* );
	ON_wString( const wchar_t*, int /*length*/ );        // from substring
	ON_wString( wchar_t, int = 1 /* repeat count */ );   

#if defined(ON_RUNTIME_WIN)
  // Windows support
	bool LoadResourceString(HINSTANCE, UINT); // load from string resource
										                        // 2047 characters max
#endif

  void Create();
  void Destroy(); // releases any memory and initializes to default empty string
  void EmergencyDestroy();

  /*
  Description:
    Enables reference counting.  I limited cases, this is useful 
    for large strings or strings that are frequently passed around.
    Reference counted strings must be carefully managed in
    when multi-threading is used.
  Parameters:
    If EnableReferenceCounting()
    is not called, then the string will not be referanceThe default is to not use
    reference counted strings.
  */
  void EnableReferenceCounting( bool bEnable );

  /*
  Returns:
    True if the string is reference counted.
  */
  bool IsReferenceCounted() const;

  // Attributes & Operations

  /*
  Returns:
    number of nonzero elements in string.
  */
  int Length() const;

  /*
  Returns:
    number of nonzero elements in the string.
  */
  unsigned int UnsignedLength() const;

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static int Length(
    const wchar_t* string
    );

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static unsigned int UnsignedLength(
    const wchar_t* string
    );

  bool IsEmpty() const;
  bool IsNotEmpty() const; // returns true if length > 0
  void Empty();   // sets length to zero - if possible, memory is retained

	wchar_t& operator[](int);
	wchar_t operator[](int) const;
  wchar_t GetAt(int) const;
	void SetAt(int, char);
	void SetAt(int, unsigned char);
	void SetAt(int, wchar_t);

	operator const wchar_t*() const;

	// overloaded assignment
	const ON_wString& operator=(const ON_wString&);
	const ON_wString& operator=(const ON_String& src); // src = UTF-8 string
	const ON_wString& operator=(char);
	const ON_wString& operator=(const char* src); // src = UTF-8 string
	const ON_wString& operator=(unsigned char);
	const ON_wString& operator=(const unsigned char* src); // src = UTF-8 string
  const ON_wString& operator=(wchar_t);
  const ON_wString& operator=(const wchar_t*);

	// string concatenation
  void Append( const char* sUTF8, int ); // append specified number of elements from a UTF-8 string
  void Append( const unsigned char* sUTF8, int ); // append specified number of elements from a UTF-8 string
  void Append( const wchar_t*, int ); // append specified number of elements
	const ON_wString& operator+=(const ON_wString&);
	const ON_wString& operator+=(const ON_String& sUTF8); // append UTF-8 string
	const ON_wString& operator+=(char);
	const ON_wString& operator+=(unsigned char);
	const ON_wString& operator+=(wchar_t);
	const ON_wString& operator+=(const char* sUTF8); // append UTF-8 string
	const ON_wString& operator+=(const unsigned char* sUTF8); // append UTF-8 string
	const ON_wString& operator+=(const wchar_t*);

  // operator+()
  ON_wString operator+(const ON_wString&) const;
  ON_wString operator+(const ON_String& sUTF8) const; // concatinate with a UTF-8 string
  ON_wString operator+(char) const;
  ON_wString operator+(unsigned char) const;
  ON_wString operator+(wchar_t) const;
  ON_wString operator+(const char* sUTF8) const; // concatinate with a UTF-8 string
  ON_wString operator+(const unsigned char* sUTF8) const; // concatinate with a UTF-8 string
  ON_wString operator+(const wchar_t*) const;

  // Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate
	//ON_DEPRECATED // deprecation in progress
	int Compare( const wchar_t* ) const;

  // Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate
	// ON_DEPRECATED // deprecation in progress
	int CompareNoCase( const wchar_t* ) const;

  bool Equal(
    const ON_wString& other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;

  bool Equal(
    const wchar_t* other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;

  static bool Equal(
    const wchar_t* string1,
    const wchar_t* string2,
    const class ON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Determine if string 1 and string2 are equal.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of string1[] elements to test.
      If element_count1 < 0, then string1 must nullptr or be null terminated.
    string2 - [in]
    element_count2 - [in]
      The number of string2[] elements to test.
      If element_count1 < 0, then string2 must nullptr or be null terminated.
    locale - [in]
      Typically ON_Locale::Ordinal, ON_Locale::InvariantCulture, or
      ON_Locale::CurrentCulture.
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */
  static bool Equal(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    const class ON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare this string and other_string by normalizing (NFC) 
    and using invariant culture ordering.
  Parameters:
    other_string - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  int Compare(
    const ON_wString& other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;

  int Compare(
    const wchar_t* other_string,
    const class ON_Locale& locale,
    bool bIgnoreCase
    ) const;
 
  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    string2 - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  static int Compare(
    const wchar_t* string1,
    const wchar_t* string2,
    const class ON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of string1[] elements to compare.
      If element_count1 < 0, then string1 must be null terminated.
    string2 - [in]
    element_count2 - [in]
      The number of string2[] elements to compare.
      If element_count2 < 0, then string2 must be null terminated.
    locale - [in]
      Typically ON_Locale::Ordinal, ON_Locale::InvariantCulture, or
      ON_Locale::CurrentCulture.
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */
  static int Compare(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    const class ON_Locale& locale,
    bool bIgnoreCase
    );
  
  bool EqualOrdinal(
    const ON_wString& other_string,
    bool bOrdinalIgnoreCase
    ) const;
  
  bool EqualOrdinal(
    const wchar_t* other_string,
    bool bOrdinalIgnoreCase
    ) const;

  static bool EqualOrdinal(
    const wchar_t* string1,
    const wchar_t* string2,
    bool bOrdinalIgnoreCase
    );

  static bool EqualOrdinal(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );


  /*
  Description:
    Compare this string and other_string wchar_t element by wchar_t element.
  Parameters:
    other_string - [in]
    bOrdinalIgnoreCase - [in]
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    imposible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */  
  int CompareOrdinal(
     const ON_wString& other_string,
     bool bOrdinalIgnoreCase
     ) const;
 
  int CompareOrdinal(
     const wchar_t* other_string,
     bool bOrdinalIgnoreCase
     ) const;
 
  /*
  Description:
    Compare this string1 and string2 wchar_t element by wchar_t element.
  Parameters:
    string1 - [in]
    string2 - [in]
    bOrdinalIgnoreCase - [in]
      If true, elements with values a-z are compared as if they had values A-Z.
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    imposible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const wchar_t* string1,
    const wchar_t* string2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string1 and string2 wchar_t element by wchar_t element.
  Parameters:
    string1 - [in]
    maximum_element_count1 - [in]
      maximum number of elements to compare
    string2 - [in]
    maximum_element_count2 - [in]
      maximum number of elements to compare
    bOrdinalIgnoreCase - [in]
      If true, elements with values a-z are compared as if they had values A-Z.
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    imposible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_path as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    other_path - [in]
      null terminated string
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  int ComparePath(
    const wchar_t* other_path
    ) const;
  bool EqualPath(
    const wchar_t* other_path
    ) const;

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
      null terminated string
    path2 - [in]
      null terminated string
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  static int ComparePath(
    const wchar_t* path1,
    const wchar_t* path2
    );
  static bool EqualPath(
    const wchar_t* path1,
    const wchar_t* path2
    );

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
    maximum_element_count1 - [in]
    path2 - [in]
    maximum_element_count2 - [in]
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  static int ComparePath(
    const wchar_t* path1,
    int element_count1,
    const wchar_t* path2,
    int element_count2
    );
  static bool EqualPath(
    const wchar_t* path1,
    int element_count1,
    const wchar_t* path2,
    int element_count2
    );

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like ON_3dmObjectAttributes.m_name, ON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    other_name - [in]
      null terminated string
  */
  int CompareAttributeName(
    const wchar_t* other_name
    ) const;
  bool EqualAttributeName(
    const wchar_t* other_name
    ) const;

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like ON_3dmObjectAttributes.m_name, ON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    name1 - [in]
      null terminated string
    name2 - [in]
      null terminated string
  */
  static int CompareAttributeName(
    const wchar_t* name1,
    const wchar_t* name2
    );
  static bool EqualAttributeName(
    const wchar_t* name1,
    const wchar_t* name2
    );


  // Description:
  //   Simple case sensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   mathes zero or more occurances of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string mathes the wild card pattern.
	bool WildCardMatch( const wchar_t* ) const;

  // Description:
  //   Simple case insensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   mathes zero or more occurances of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string mathes the wild card pattern.
	bool WildCardMatchNoCase( const wchar_t* ) const;

  /*
  Description:
    Replace all substrings that match token1 with token2
  Parameters:
    token1 - [in]
    token2 - [in]
  Returns:
    Number of times toke1 was replaced with token2
  */
  int Replace( const wchar_t* token1, const wchar_t* token2 );
  int Replace( wchar_t token1, wchar_t token2 );

  /*
  Description:
    Replaces all characters in the string whose values are
    not '0-9', 'A-Z', or 'a-z' with a percent sign followed
    by a 2 digit hex value.
  */
  void UrlEncode();

  /*
  Description:
    Replaces all %xx where xx a two digit hexadecimal number,
    with a single character. Returns false if the orginal
    string contained 
  */
  bool UrlDecode();

  /*
  Description:
    Replace all white-space characters with the token.
    If token is zero, the string will end up with
    internal 0's
  Parameters:
    token - [in]
    whitespace - [in] if not null, this is a 0 terminated
      string that lists the characters considered to be 
      white space.  If null, then (1,2,...,32,127) is used.
  Returns:
    Number of whitespace characters replaced.
  See Also:
    ON_wString::RemoveWhiteSpace
  */
  int ReplaceWhiteSpace( wchar_t token, const wchar_t* whitespace = 0 );

  /*
  Description:
    Removes all white-space characters with the token.
  Parameters:
    whitespace - [in] if not null, this is a 0 terminated
      string that lists the characters considered to be 
      white space.  If null, then (1,2,...,32,127) is used.
  Returns:
    Number of whitespace characters removed.
  See Also:
    ON_wString::ReplaceWhiteSpace
  */
  int RemoveWhiteSpace( const wchar_t* whitespace = 0 );

  /*
  Parameters: 
    prefix - [in]
    locale - [in]
      When no local is available, pass ON_Locale::Ordinal.
    bIgnoreCase - [in]
      true to ignore case.
  Returns:
    If the string begins with prefix, the returned string has prefix removed. 
    Otherwise the returned string is identical to the string.
  */
  const ON_wString RemovePrefix(
    const wchar_t* prefix,
    const class ON_Locale& locale,
    bool bIgnoreCase
  ) const;

  /*
  Parameters: 
    suffix - [in]
    locale - [in]
      When no local is available, pass ON_Locale::Ordinal.
    bIgnoreCase - [in]
      true to ignore case.
  Returns:
    If the string ends with suffix, the returned string has suffix removed. 
    Otherwise the returned string is identical to the string.
  */
  const ON_wString RemoveSuffix(
    const wchar_t* suffix,
    const class ON_Locale& locale,
    bool bIgnoreCase
  ) const;

  // simple sub-string extraction
	ON_wString Mid(
    int, // index of first char
    int  // count
    ) const;
	ON_wString Mid(
    int // index of first char
    ) const;
	ON_wString Left(
    int // number of chars to keep
    ) const;
	ON_wString Right(
    int // number of chars to keep
    ) const;
  const ON_wString SubString(
    int start_index
  ) const;
  const ON_wString SubString(
    int start_index,
    int count
  ) const;

  /*
  Description:
    Map a single wchar_t element to upper or lower case.
  Parameters:
    c - [in]
      If sizeof(wchar_t) >= 2 and c is not a value used int surrogate pairs,
      the map specified by map_type is applied.  If c is a value used in 
      surrogate pairs, the value is not changed.
  Remarks:
    1) MapCharacterOrdinal is not appropriate for general string mapping
       because it does not correctly handle surrogate pairs.
    2) If sizeof(wchar_t) == sizeof(char), ON_String::MapCharacterOrdinal()
       and ON_wString::MapCharacterOrdinal() are identical.
  */
  static wchar_t MapCharacterOrdinal(
    ON_StringMapOrdinalType map_type,
    wchar_t c
    );

  ON_wString MapStringOrdinal(
    ON_StringMapOrdinalType map_type
    ) const;

  /*
  Description:
    Map a wchar_t string.
  Parameters:
    map_type - [in]
    string - [in]
    element_count - [in]
      number of string[] elements to map.
      If element_count < 0, then ON_wString::Length(string) elements are mapped.
    mapped_string - [out]
    mapped_string_capacity - [in]
      number of available elements in mapped_string[].
      mapped_string_capacity must be >= mapped_element_count where 
      mapped_element_count = (element_count >= 0) element_count ? ON_wString::Length(string).
    map_type - [in]
  Returns:
    Number of mapped_string[] elements that were mapped from string[].

    When the return value is < mapped_string_capacity, a null terminator
    is appended after the last mapped element.
  */
  static int MapStringOrdinal(
    ON_StringMapOrdinalType map_type,
    const wchar_t* string,
    int element_count,
    wchar_t* mapped_string,
    int mapped_string_capacity
    );

  ON_wString MapString(
    const class ON_Locale& locale,
    ON_StringMapType map_type
    ) const;

  static ON_wString MapString(
    const class ON_Locale& locale,
    ON_StringMapType map_type,
    const wchar_t* string,
    int element_count
    );

  /*
  Description:
    Maps element_count elements of string[] to mapped_string[].
  Parameters:
    locale - [in]
      Locale to use when converting case. It is common to pass one of
      the preset locales ON_Locale::Ordinal, ON_Locale::InvariantCulture, 
      or ON_Locale::m_CurrentCulture. 

    map_type - [in]
      selects the mapping

    string - [in]
      input string to map.

    element_count - [in]
      The number of wchar_t elements to map from input string[].
      
      If element_count < 1, then string[] must be null terminated and
      ON_wString::Length(string)+1 elements are mapped. 
      The +1 insures the output is null terminated.

    mapped_string - [out]
      The result of the mapping is returned in mapped_string[].

    mapped_string_capacity - [in]
      Number of wchar_t elements available in mapped_string[]
      or 0 to calculate the minimum number of elements needed
      for the mapping.

  Returns:
    If mapped_string_capacity > 0, then the number elements set in mapped_string[]
    is returned.  

    If mapped_string_capacity == 0, then the number elements required to perform
    the mapping is returned.

    When there is room, mapped_string[] is null terminated.

    0: Failure.
  */
  static int MapString(
    const class ON_Locale& locale,
    ON_StringMapType map_type,
    const wchar_t* string,
    int element_count,
    wchar_t* mapped_string,
    int mapped_string_capacity
    );

  /*
  Returns:
    A platform independed SHA-1 of the string content. Independent of platform endian or platform wide string UTF encoding.
  */
  const ON_SHA1_Hash ContentHash(
    ON_StringMapOrdinalType mapping
  ) const;


	// upper/lower/reverse conversion
	/*ON_DEPRECATED */ void MakeUpper();
	/*ON_DEPRECATED */ void MakeLower();
  
  ON_wString Reverse() const;

  static wchar_t* Reverse(
    wchar_t* string,
    int element_count
    );

  void MakeUpperOrdinal();
	void MakeLowerOrdinal();
	void MakeReverse();

  /*
  Description:
    Removes leading elements from the string.
  Parameters:
    s - [in]
      All leading wchar_t elements with a value that it found in s[] are removed.
      If s is nullptr, All leading wchar_t element with a value between 1 and space are removed.
  */
  void TrimLeft(const wchar_t* s = nullptr);

  /*
  Description:
    Removes trailing elements from the string.
  Parameters:
    s - [in]
      All trailing wchar_t elements with a value that it found in s[] are removed.
      If s is nullptr, All trailing wchar_t elements with a value between 1 and space are removed.
  */
  void TrimRight(const wchar_t* s = nullptr);

  void TrimLeftAndRight(const wchar_t* s = nullptr);

  /*
  Description:
    Remove all occurrences of the input character.
  Parameters:
    c - [in]
      utf8_single_byte_ct must have a value between 0 and 0x7F.
      When w is a 2 byte UTF-16 wchar_t value (like Microsoft's wchar_t), 
      it must be in the range 0 to 0xD7FF or 0xE000 to 0xFFFF.
      When w is a 4 byte UTF-32 wchar_t value (like Apple's wchar_t), 
      it must be in the range 0 to 0xD7FF or 0xE000 to 0x10FFFF.
      Returns:
    Number of characters removed.
  */
  int Remove(
    char utf8_single_byte_c
    );
  int Remove(
    unsigned char utf8_single_byte_c
    );
  int Remove(
    wchar_t w
    );

  /*
  Description:
    Find the first occurrence of a character or substring.
  Parameters:
    utf8_single_byte_c - [in]
      utf8_single_byte_c must have a value between 0 and 0x7F.
    w - [in]
      When w is a 2 byte UTF-16 wchar_t value (like Microsoft's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0xFFFF.
      When w is a 4 byte UTF-32 wchar_t value (like Apple's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0x10FFFF.
      sTUF8 - [in]
      A null terminated UTF-8 string.
    wcharString - [in]
      A null terminated wchar_t string.
  start_index - [in]
      Index where search should begin.
  Returns:
    -1: invalid input or character is not in the string.
    >=0: Index of the first instance of the character.
  */
  int Find(
    char utf8_single_byte_c
    ) const;
  int Find(
    unsigned char utf8_single_byte_c
    ) const;
  int Find(
    wchar_t w
    ) const;
  int Find(
    const char* sUTF8
    ) const;
  int Find(
    const unsigned char* sUTF8
    ) const;
  int Find(
    const wchar_t* wcharString
    ) const;
  int Find(
    char utf8_single_byte_c,
    size_t start_index
    ) const;
  int Find(
    unsigned char utf8_single_byte_c,
    size_t start_index
    ) const;
  int Find(
    wchar_t w,
    size_t start_index
    ) const;
  int Find(
    wchar_t w,
    int start_index
    ) const;
  int Find(
    const char* sUTF8,
    size_t start_index
    ) const;
  int Find(
    const unsigned char* sUTF8,
    size_t start_index
    ) const;
  int Find(
    const wchar_t* wcharString,
    size_t start_index
    ) const;
  int Find(
    const wchar_t* wcharString,
    int start_index
    ) const;
  int FindOneOf(
    const wchar_t* character_set
    ) const;


  /*
  Description:
    Find the last occurrence of a character or substring.
  Parameters:
    utf8_single_byte_c - [in]
      utf8_single_byte_c must have a value between 0 and 0x7F.
    w - [in]
      When w is a 2 byte UTF-16 wchar_t value (like Microsoft's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0xFFFF.
      When w is a 4 byte UTF-32 wchar_t value (like Apple's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0x10FFFF.
      sUTF8 - [in]
    wideString - [in]
  Returns:
    -1: Invalid input or character or substring was not found.
    >=0: Index of the final occurrence of the character or substring.
  */
  int ReverseFind(
    char utf8_single_byte_c
    ) const;
	int ReverseFind(
    unsigned char utf8_single_byte_c
    ) const;
	int ReverseFind(
    wchar_t w
    ) const;
  int ReverseFind(
    const char* sUTF8
    ) const;

  int ReverseFind(
    const wchar_t* wideString
    ) const;


  // It is common to format single numbers into strings
  // and the FromNumber and FromDouble functions are
  // the fastest way to do this and provide consistent results.
  // They return a pointer to their buffer so the can be used
  // as function parameters.
  static const ON_wString FromNumber(
    char n
    );
  static const ON_wString FromNumber(
    unsigned char n
    );
  static const ON_wString FromNumber(
    short n
    );
  static const ON_wString FromNumber(
    unsigned short n
    );
  static const ON_wString FromNumber(
    int n
    );
  static const ON_wString FromNumber(
    unsigned int n
    );
  static const ON_wString FromNumber(
    ON__INT64 n
    );
  static const ON_wString FromNumber(
    ON__UINT64 n
    );
  static const ON_wString FromNumber(
    double d // "%g" format
    );
  static const ON_wString ApproximateFromNumber(
    double d // "%f" when possible, otherwise "%g"
    );
  static const ON_wString PreciseFromNumber(
    double d // "%.17g"
    );

  /*
  Description:
    Convert a list of Unicode code points into a wide string.
  Parameters:
    code_points - [in]
      array of Unicode code points
    code_point_count - [in]
      number of code points.
      -1 indicates code_points[] is terminated by a 0 value.
    error_code_point - [in]
      If error_code_point is a valid Unicode code point,
      then error_code_point will be used in place of invalid values in code_points[].
      Otherwise, conversion will terminate if code_points[] contains an invalid value.
      The values ON_UnicodeCodePoint::ON_ReplacementCharacter and
      ON_UnicodeCodePoint::ON_InvalidCodePoint are commonly used for this parameter.
  Returns:
    A wide string encoding of the Unicode code points.
  Remarks:
    If more control over the conversion process is required, 
    then use ON_ConvertUTF32ToWideChar().
  */
  static const ON_wString FromUnicodeCodePoints(
    const ON__UINT32* code_points,
    int code_point_count,
    ON__UINT32 error_code_point
  );

  /*
  Description:
    Each byte value is converted to 2 hexadecimal digits.
  Parameters:
    bytes - [in]
      list of byte values
    byte_count - [in]
      Number of byte values in bytes[] array.
    bCapitalDigits - [in]
      false: Use 0-9, a - b
      true: Use 0-9, A - F
    bReverse - [in]
      false: 
        The digist in the string will be in the order
        bytes[0], bytes[1], ..., bytes[byte_count-1].
      true: 
        The digist in the string will be in the order
        bytes[byte_count-1], ..., bytes[1], bytes[0].
  */
  static const ON_wString HexadecimalFromBytes(
    const ON__UINT8* bytes,
    size_t byte_count,
    bool bCapitalDigits,
    bool bReverse
    );


  /*
  Parameters:
    format - [in]
      Format control.
      Positional paramters of the form %N$x where N >= 1 and x
      is the standard format specification are supported.
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
      arguments for replacable items in the format string.
  Returns:
    True if successful.
    False if the string is too long or the format string is not valid.
  Remarks:
    When using Microsoft's compiler and other compilers that provide similar
    locale support, the locale is the invariant culture locale returned by
    ON_Locale::InvariantCulture::LocalePtr().

    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly reccommended that you never use %S to
    include any string that may possibly contain elements with values > 127.
    The following examples illustrate a way to predictably use UTF-8 and wchar_t
    parameters in buffers of the other element type.

    const char* utf8_string = ...;
    // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
    // The code below will treat utf8_string as a UTF-8 encoded string.
    wchar_t wchar_buffer[...];
    const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
    ON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", ON_wString(utf8_string));

    const wchar_t* wide_string = ...;
    // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",char_string);
    // The code below will include wide_string as a UTF-8 encoded string.
    char char_buffer[...];
    const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
    ON_String::Format(char_buffer, char_buffer_capacity, "%s", ON_String(wide_string));
  */
  bool ON_VARGS_FUNC_CDECL Format(
    const wchar_t* format,
    ...
    );

  static const ON_wString ON_VARGS_FUNC_CDECL FormatToString(
    const wchar_t* format,
    ...
    );

  bool FormatVargs(
    const wchar_t* format,
    va_list args
    );

  /*
  Description:
    A platform independent, secure, culture invariant way to format a wchar_t string
    with support for positional format parameters.
    This function is provide to be used when it is critical that 
    the formatting be platform independent, secure and culture invarient.
  Parameters:
    buffer - [out] 
      not null
    buffer_capacity - [in]
      > 0
      Number of wchar_t elements in buffer.
    format - [in]
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
  Returns:
    >= 0: 
      The number of wchar_t elements written to buffer[], not including the null terminator.
      A null terminator is always added (buffer[returned value] = 0).
      The last element of buffer[] is always set to zero (buffer[buffer_capacity-1] = 0).
    < 0: failure:
      If buffer is not null and buffer_capacity > 0, then buffer[0] = 0 and buffer[buffer_capacity-1] = 0;
  Remarks:
    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly reccommended that you never use %S to
    include any string that may possibly contain elements with values > 127.  
    The following examples illustrate a way to predictably use UTF-8 and wchar_t 
    parameters in buffers of the other element type.

         const char* utf8_string = ...;
         // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
         wchar_t wchar_buffer[...];
         const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
         ON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", ON_wString(utf8_string));

         const wchar_t* wide_string = ...;
         // UNRELIABLE // ON_wString::Format(buffer,buffer_capacity,"%S",char_string);
         char char_buffer[...];
         const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
         ON_String::Format(char_buffer, char_buffer_capacity, "%s", ON_String(wide_string));
  */
  static int ON_VARGS_FUNC_CDECL FormatIntoBuffer(
    wchar_t* buffer, 
    size_t buffer_capacity,
    const wchar_t* format,
    ...
    );

  static int ON_VARGS_FUNC_CDECL FormatIntoBuffer(
    ON_wStringBuffer& buffer,
    const wchar_t* format,
    ...
    );

  static int FormatVargsIntoBuffer(
    ON_wStringBuffer& buffer,
    const wchar_t* format,
    va_list args
    );

  static int FormatVargsIntoBuffer(
    wchar_t* buffer,
    size_t buffer_capacity,
    const wchar_t* format,
    va_list args
    );

  /*
  Returns:
    >= 0:
     Number of char elements in the formatted string, not including the null terminator.
    < 0:
      Invalid input
  */
  static int FormatVargsOutputCount(
    const wchar_t* format,
    va_list args
    );

  /*
  Parameters:
    format - [in]
      null terminated string to scan
    ... - [out]
      pointers to elements to assign.
  Returns:
    >= 0: number of fields successfully converted and assigned.
    <0:  failure
  */
  int ON_VARGS_FUNC_CDECL Scan(
    const wchar_t* format,
    ...
    ) const;

  static int ON_VARGS_FUNC_CDECL ScanBuffer(
    const wchar_t* buffer,
    const wchar_t* format,
    ...
    );

  static int ScanBufferVargs(
    const wchar_t* buffer,
    const wchar_t* format,
    va_list args
    );

  /*
  Returns:
    not zero:
      pointer to the first character that was not scanned
    nullptr:
      failure
  */
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    char value_on_failure,
    char* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    unsigned char value_on_failure,
    unsigned char* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    short value_on_failure,
    short* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    unsigned short value_on_failure,
    unsigned short* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    int value_on_failure,
    int* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    unsigned int value_on_failure,
    unsigned int* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    ON__INT64 value_on_failure,
    ON__INT64* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    ON__UINT64 value_on_failure,
    ON__UINT64* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    double value_on_failure,
    double* value
    );


	// Low level access to string contents as character array
	wchar_t* ReserveArray(size_t); // make sure internal array has at least
                          // the requested capacity.
	void ShrinkArray();     // shrink internal storage to minimum size
  wchar_t* SetLength(size_t); // set length (<=capacity)
  wchar_t* Array();
  const wchar_t* Array() const;

  /*
  Returns:
    Total number of bytes of memory used by this class.
    (For use in ON_Object::SizeOf() overrides.
  */
  unsigned int SizeOf() const;

  /*
  Returns:
    CRC of the string.
  */
  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  /*
  Returns:
    CRC of the lower case version of the string. Useful
    for case insensitive CRCs and hash codes.
  */
  ON__UINT32 DataCRCLower(ON__UINT32 current_remainder) const;

  /*
  OBSOLETE - Use ON_FileSystemPath::SplitPath
  */
  static void SplitPath( 
    const char* path,
    ON_wString* drive,
    ON_wString* dir,
    ON_wString* fname,
    ON_wString* ext
    );

  static void SplitPath( 
    const wchar_t* path,
    ON_wString* drive,
    ON_wString* dir,
    ON_wString* fname,
    ON_wString* ext
    );

public:
	~ON_wString();

protected:
  // Implementation
	wchar_t* m_s; // pointer to ref counted string array
                // m_s - 12 bytes points at the string's ON_wStringHeader

	// implementation helpers
	struct ON_wStringHeader* Header() const;
	wchar_t* CreateArray(int);
  void CopyArray();
  void CopyToArray( const ON_wString& );
  void CopyToArray( int, const char* );
  void CopyToArray( int, const unsigned char* );
  void CopyToArray( int, const wchar_t* );
  void AppendToArray( const ON_wString& );
  void AppendToArray( int, const char* );
  void AppendToArray( int, const unsigned char* );
  void AppendToArray( int, const wchar_t* );
};

/*
Returns:
  True if lhs and rhs are identical as arrays of wchar_t elements.  
*/
ON_DECL
bool operator==( const ON_wString& lhs, const ON_wString& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of wchar_t elements.  
*/
ON_DECL
bool operator!=(const ON_wString& lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator<(const ON_wString& lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator>(const ON_wString& lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator<=(const ON_wString& lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator>=(const ON_wString& lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of wchar_t elements.  
*/
ON_DECL
bool operator==( const ON_wString& lhs, const wchar_t* rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of wchar_t elements.  
*/
ON_DECL
bool operator!=(const ON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is less than rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator<(const ON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator>(const ON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator<=(const ON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator>=(const ON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of wchar_t elements.  
*/
ON_DECL
bool operator==( const wchar_t* lhs, const ON_wString& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of wchar_t elements.  
*/
ON_DECL
bool operator!=(const wchar_t* lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator<(const wchar_t* lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator>(const wchar_t* lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator<=(const wchar_t* lhs, const ON_wString& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of wchar_t elements.
*/
ON_DECL
bool operator>=(const wchar_t* lhs, const ON_wString& rhs);

/*
Description:
  An ON_NameHash is designed to help search for and compare attribute names
  like the ON_ModelComponent.Name() value.

  If a name is wchar_t encoded as wide_char_name and UTF-8 encoded as utf8_name,
  then ON_NameHash(wide_char_name) == ON_NameHash(utf8_name).

  Set:
    bool bEqualNameHash = ON_NameHash::Create(parent_id1,name1) == ON_NameHash::Create(parent_id2,name2);
    bool bEqualParentId = (parent_id1 == parent_id2)
    bool bEqualAttributeName = ON_String::EqualAttributeName(name1,name2);
  
  If (bEqualParentId && bEqualAttributeName) is true, then bEqualNameHash is true.
  If bEqualParentId is false, then bEqualNameHash is false.
  With probablity 1-epsilon, if bEqualAttributeName is false, then bEqualNameHash is false, 
  where epsilon is an extremely tiny number.
*/
class ON_CLASS ON_NameHash
{
public:
  /*
    Default conststruction creates ON_NameHash::UnsetNameHash.
  */
  ON_NameHash() = default;
  ~ON_NameHash() = default;
  ON_NameHash(const ON_NameHash&) = default;
  ON_NameHash& operator=(const ON_NameHash&) = default;

  static const ON_NameHash EmptyNameHash;
  static const ON_NameHash UnsetNameHash;

  /*
  Returns:
    True if the hash is for a valid non-empty name.
  */
  bool IsValidAndNotEmpty() const;

  /*
  Returns:
    True if the hash is for an empty name.
  */
  bool IsEmptyNameHash() const;

  /*
  Returns:
    True if the hash is for an invalid name.
  */
  bool IsInvalidNameHash() const;

  /*
  Returns:
    True if the hash is not empty and was set by calling CreateFilePathHash().
  */
  bool IsFilePathHash() const;

  /*
  Returns:
    True if hash is set and case is ignored
  */
  bool IgnoreCase() const;

  /*
  Returns:
    ON_NameHash::EmptyNameHash if name is empty.
    ON_NameHash::UnsetNameHash if name is not valid.
  */
  static ON_NameHash Create(
    const ON_wString& name
    );
  static ON_NameHash Create(
    const wchar_t* name
    );
  static ON_NameHash Create(
    size_t length,
    const wchar_t* name
    );
  static ON_NameHash Create(
    const ON_String& utf8_name
    );
  static ON_NameHash Create(
    const char* utf8_name
    );
  static ON_NameHash Create(
    size_t length,
    const char* utf8_name
    );

  static ON_NameHash Create(
    const ON_UUID& parent_id, 
    const ON_wString& name
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    const wchar_t* name
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    size_t length,
    const wchar_t* name
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    const ON_String& name
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    const char* utf8_name
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    size_t length,
    const char* utf8_name
    );

  static ON_NameHash Create(
    const ON_wString& name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const wchar_t* name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    size_t length,
    const wchar_t* name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const ON_String& name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const char* utf8_name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    size_t length,
    const char* utf8_name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id, 
    const ON_wString& name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    const wchar_t* name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    size_t length,
    const wchar_t* name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    const ON_String& utf8_name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    const char* utf8_name,
    bool bIgnoreCase
    );
  static ON_NameHash Create(
    const ON_UUID& parent_id,
    size_t length,
    const char* utf8_name,
    bool bIgnoreCase
    );

  static ON_NameHash CreateIdAndEmptyName(
    const ON_UUID& parent_id
    );

  static ON_NameHash CreateIdAndUnsetName(
    const ON_UUID& parent_id
    );

  static ON_NameHash CreateFilePathHash(
    const class ON_FileReference& file_reference
    );

  static ON_NameHash CreateFilePathHash(
    const wchar_t* file_path
    );

  static ON_NameHash CreateFilePathHash(
    const char* file_path
    );

  static int CompareNameSHA1(
    const ON_NameHash& a,
    const ON_NameHash& b
    );

  static int CompareNameSHA1Ptr(
    const ON_NameHash* a,
    const ON_NameHash* b
    );

  static int CompareParentId(
    const ON_NameHash& a,
    const ON_NameHash& b
    );

  static int CompareParentIdPtr(
    const ON_NameHash* a,
    const ON_NameHash* b
    );

  /*
  Description:
    Compares id, then length, then SHA-1 digest.
  */
  static int Compare(
    const ON_NameHash& a,
    const ON_NameHash& b
    );

  /*
  Description:
    Compares id, then length, then SHA-1 digest.
  */
  static int ComparePtr(
    const ON_NameHash* a,
    const ON_NameHash* b
    );

  // Number of UTf-32 code point values in mapped name.
  // (0 for file path hashes)
  ON__UINT32 MappedNameCodePointCount() const;

  // SHA-1 hash of mapped name
  ON_SHA1_Hash MappedNameSha1Hash() const;

  // 32 bit hash of id and mapped name
  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  // 32 bit hash of id
  ON__UINT32 IdCRC(ON__UINT32 current_remainder) const;

  // 32 bit hash of id and mapped name
  ON__UINT32 NameCRC(ON__UINT32 current_remainder) const;

  ON_UUID ParentId() const;
  
private:

  enum : unsigned int
  {
    flags_length_mask = 0x0FFFFFFF,
    flags_case_sensitive = 0x80000000,
    flags_file_path = 0xFFFFFFFF
  };
  // m_flags = 0xFFFFFFFFU -> Hash is a file path hash
  // m_flags != 0
  // m_flags &0x0FFFFFFF = number of unicode (UTF-32) code points in name
  // m_flags &0x80000000 = 0 to ignore case, !=0 if case sensitive hash
  ON__UINT32 m_flags = 0;

  // m_sha1_hash = SHA-1 hash of ordinal minimum mapped unicode (UTF-32) code points
  // If the name is empty, m_length = 0 and m_sha1_hash = ON_SHA1_Hash::EmptyContentHash.
  // If the name is not valid, m_length = 0 and m_sha1_hash = ON_SHA1_Hash::ZeroDigest.
  ON_SHA1_Hash m_sha1_hash = ON_SHA1_Hash::ZeroDigest; 

  // When names appear in a tree structure, m_parent_id identifies the
  // parent node.
  ON_UUID m_parent_id = ON_nil_uuid;

public:
  /*
  Description:
    Internal_CreateFromDotNet() is public for technical reasons. It is used
    in Rhino Common p-invoke code that provides a .NET interface to the 
    services ON_NameHash provided by the ON_NameHash class.
    This function should be ignored and never called from ordinary C++ code.
    If you choose to ignore the preceding admonition, you will have to read
    the source code for information about how this function works.
  */
  static ON_NameHash Internal_DotNetInterfaceSet(
    const ON_UUID&,
    const ON_SHA1_Hash&,
    const ON__UINT32
  );

  ON__UINT32 Internal_DotNetInterfaceGetFlags() const;
};

/*
Compares id, then length, then SHA-1 digest
*/
ON_DECL
bool operator==(
  const ON_NameHash& a,
  const ON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
ON_DECL
bool operator!=(
  const ON_NameHash& a,
  const ON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
ON_DECL
bool operator<(
  const ON_NameHash& a,
  const ON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
ON_DECL
bool operator<=(
  const ON_NameHash& a,
  const ON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
ON_DECL
bool operator>(
  const ON_NameHash& a,
  const ON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
ON_DECL
bool operator>=(
  const ON_NameHash& a,
  const ON_NameHash& b
  );

class ON_CLASS ON_UnitSystem
{
public:
  // Default construction sets this to ON_UnitSystem::Meters 
  ON_UnitSystem() = default;

  ~ON_UnitSystem() = default;
  ON_UnitSystem(const ON_UnitSystem&) = default;
  ON_UnitSystem& operator=(const ON_UnitSystem&) = default;

public:
  ON_UnitSystem(
    ON::LengthUnitSystem length_unit_system
    );

  ON_UnitSystem& operator=(
    ON::LengthUnitSystem length_unit_system
    );

  /*
  Parameters:
    custom_unit_name - [in]
      length unit name (no spaces)
    meters_per_custom_unit - [in]
      a positive number 
  Example:
      // 1 League = 5556 meters
      const ON_UnitSystem Leagues = ON_UnitSystem::CreateCustomUnitSystem(L"Leagues", 1.0/5556.0);
  */
  static ON_UnitSystem CreateCustomUnitSystem(
    const wchar_t* custom_unit_name,
    double meters_per_custom_unit
  );

public:
  // unit system = ON::LengthUnitSystem::None. Scale to or from any valid unit = 1
  static const ON_UnitSystem None;

  static const ON_UnitSystem Angstroms;
  static const ON_UnitSystem Nanometers;
  static const ON_UnitSystem Microns;

  // unit system = ON::LengthUnitSystem::Millimeters and meters/unit = 0.001 exactly
  static const ON_UnitSystem Millimeters;

  static const ON_UnitSystem Centimeters;
  static const ON_UnitSystem Decimeters;

  // unit system = ON::LengthUnitSystem::Meters and meters/unit = 1
  static const ON_UnitSystem Meters;
  static const ON_UnitSystem Dekameters;
  static const ON_UnitSystem Hectometers;
  static const ON_UnitSystem Kilometers;
  static const ON_UnitSystem Megameters;
  static const ON_UnitSystem Gigameters;
  static const ON_UnitSystem Microinches;
  static const ON_UnitSystem Mils;

  // unit system = ON::LengthUnitSystem::Inches and meters/unit = 0.0254 exactly
  static const ON_UnitSystem Inches;

  // unit system = ON::LengthUnitSystem::Feet and meters/unit = 0.3048  exactly
  static const ON_UnitSystem Feet;

  static const ON_UnitSystem Yards;
  static const ON_UnitSystem Miles;
  static const ON_UnitSystem PrinterPoints;
  static const ON_UnitSystem PrinterPicas;
  static const ON_UnitSystem NauticalMiles;
  static const ON_UnitSystem AstronomicalUnits;
  static const ON_UnitSystem LightYears;
  static const ON_UnitSystem Parsecs;

  // unit system = ON::LengthUnitSystem::Unset and meters/unit = ON_DBL_QNAN
  static const ON_UnitSystem Unset;



public:
  bool operator==(const ON_UnitSystem&) const;
  bool operator!=(const ON_UnitSystem&) const;

  /*
  Returns 
    true if m_unit_system is a valid ON::LengthUnitSystem enum value,
    which may be ON::LengthUnitSystem::None.  If m_unit_system is
    ON::LengthUnitSystem::CustomUnits, then IsValid() returns true
    if m_custom_unit_scale > 0.0 and false otherwise.
    The value of m_custom_unit_name is not tested.
  See Also:
    IsSet()
  */  
  bool IsValid() const;

  bool Read( class ON_BinaryArchive& );
  bool Write( class ON_BinaryArchive& ) const;
  void Dump( class ON_TextLog& ) const;

  /*
  Returns
    true If the unit system is valid and set to something beside
    ON::no_unit_systm;
  */
  bool IsSet() const;

  /*
  Returns
    true If the unit system is ON::LengthUnitSystem::CustomUnits
  */
  bool IsCustomUnitSystem() const;

  void SetUnitSystem(
    ON::LengthUnitSystem us
    );

  /*
  Parameters:
    custom_unit_name - [in]
      length unit name (no spaces)
    meters_per_custom_unit - [in]
      a positive number 
  Example:
      // 1 League = 5556 meters
      ON_UnitSystem Leagues;
      Leagues.SetCustomUnitSystem( L"Leagues", 1.0/5556.0);
      // or
      ON_UnitSystem Leagues = ON_UnitSystem::CreateCustomUnitSystem(L"Leagues", 1.0/5556.0);
  */
  void SetCustomUnitSystem(
    const wchar_t* custom_unit_name,
    double meters_per_custom_unit
    );

  /*
  Description:
    Changes the unit system to custom units and sets the custom unit scale.
  Remarks:
    Avoid using this function. Use SetCustomUnitSystem() or SetUnitSystem()
    instead.
  */
  void SetCustomUnitSystemName(
    const wchar_t* custom_unit_name
    );

  /*
  Description:
    Changes the unit system to custom units and sets the custom unit scale.
  Remarks:
    Avoid using this function. Use SetCustomUnitSystem() or SetUnitSystem()
    instead.
  */
  void SetCustomUnitSystemScale(
    double meters_per_custom_unit
    );

  double MetersPerUnit() const;
  ON::LengthUnitSystem UnitSystem() const;
  const ON_wString& UnitSystemName() const;


private:
  ON::LengthUnitSystem m_unit_system = ON::LengthUnitSystem::Meters;
  unsigned int m_reserved = 0;

  // The m_custom_unit_... settings apply when m_unit_system = ON::LengthUnitSystem::CustomUnits
  double m_meters_per_unit = 1.0;  // 1 meter = m_custom_unit_scale custom units
  ON_wString m_custom_unit_name;   // name of custom units
};
#endif
