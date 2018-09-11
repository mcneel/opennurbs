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

#if !defined(OPENNURBS_UNICODE_INC_)
#define OPENNURBS_UNICODE_INC_

ON_BEGIN_EXTERNC

enum ON_UnicodeEncoding
{
  ON_UTF_unset=0, // 
  ON_not_UTF,     // not a UTF encoding
  ON_UTF_8,       // UTF-8 big endian byte order
  ON_UTF_16,      // UTF-16 in native CPU byte order
  ON_UTF_16BE,    // UTF-16 big endian byte order
  ON_UTF_16LE,    // UTF-16 little endian byte order
  ON_UTF_32,      // UTF-32 in native CPU byte order
  ON_UTF_32BE,    // UTF-32 big endian byte order
  ON_UTF_32LE     // UTF-32 little endian CPU byte order
};

enum ON_UnicodeCodePoint
{
  // UTF-8 encodings:
  //   The UTF-8 encoding for codepoint values from 0 to 127 is a single single byte (char).  
  //   The UTF-8 encoding for codepoint values >= 128 require multiple bytes.
  // UTF-16 encodings:
  //   The UTF-16 encoding of every codepoint in this enum except Wastebasket is a single word (unsigned short).

  ON_NullCodePoint = 0x00, // nullptr control U+0000 (decimal 0)
  ON_Backspace = 0x08, // BACKSPACE control U+0008  (decimal 8)
  ON_Tab = 0x09, // CHARACTER TABULATION control U+0009  (decimal 9)
  ON_LineFeed = 0x0A, // LINE FEED control U+000A  (decimal 10)
  ON_VerticalTab = 0x0B, // LINE TABULATION control U+000B  (decimal 11)
  ON_FormFeed = 0x0C, // FORM FEED control U+000C  (decimal 12)
  ON_CarriageReturn = 0x0D, // CARRIAGE RETURN control U+000D  (decimal 13)
  ON_Escape = 0x1B, // CARRIAGE RETURN control U+001B  (decimal 27)
  ON_Space = 0x20, // SPACE U+0020  (decimal 32)
  ON_Slash = 0x2F, // SOLIDUS U+002F  (decimal 47)
  ON_Backslash = 0x5C, // REVERSE SOLIDUS U+005C (decimal 92)ere
  ON_Pipe = 0x7C, // VERTICAL LINE U+007C (decimal 124)

  //
  // NOTE: UTF-8 encodings of the codepoint values below this comment require multiple bytes.
  //
  ON_NextLine = 0x0085, // NEXT LINE (NEL) U+0085
  ON_NoBreakSpace = 0x00A0, // NO-BREAK SPACE (NBSP)
  ON_NarrowNoBreakSpace = 0x202F, // NARROW NO-BREAK SPACE (NNBSP)
  ON_ZeroWidthSpace = 0x200B, // ZERO WIDTH SPACE (ZWSP)

  //////////////////////////////////////////////////////////////
  //
  // Annotation symbols
  //
  ON_RadiusSymbol = 0x0052, // LATIN CAPITAL LETTER R U+0052 (decimal 82)
  ON_DegreeSymbol = 0x00B0, // DEGREE SIGN U+00B0 (decimal 176)
  ON_PlusMinusSymbol = 0x00B1, // PLUS-MINUS SIGN U+00B1 (decimal 177)
  ON_DiameterSymbol = 0x00D8, // LATIN CAPITAL LETTER O WITH STROKE U+00D8 (decimal 216)

  //////////////////////////////////////////////////////////////
  //
  // Unambiguous format control code points
  //
  ON_LineSeparator = 0x2028,      // LINE SEPARATOR U+2028 unambiguous line separator
  ON_ParagraphSeparator = 0x2029, // PARAGRAPH SEPARATOR U+2028 unambiguous paragraph separator

  //////////////////////////////////////////////////////////////
  //
  // Greek, Cyrillic and CJK glyph code points used for testing purposes.
  //
  ON_GreekAlpha = 0x03B1, // GREEK SMALL LETTER ALPHA
  ON_CyrillicCapitalYu = 0x042E, // CYRILLIC CAPITAL LETTER YU
  ON_SimplifiedChineseTree = 0x6881,
  ON_TraditionalChineseTree = 0x6A39,
  ON_JapaneseRhinoceros = 0x7280,
  ON_JapaneseTree = 0x6728,
  ON_KoreanHan = 0xD55C,
  ON_KoreanJeong = 0xC815,

  //////////////////////////////////////////////////////////////
  //
  // Currency symbols
  //
  ON_DollarSign   = 0x0024, // DOLLAR SIGN U+0024
  ON_CentSign     = 0x00A2, // CENT SIGN U+00A2
  ON_PoundSign    = 0x00A3, // POUND SIGN U+00A3
  ON_CurrencySign = 0x00A4, // CURRENCY SIGN U+00A4
  ON_YenSign      = 0x00A5, // YEN SIGN U+00A5 (Chinese yuan, Japanese yen)
  ON_EuroSign     = 0x20AC, // EURO SIGN U+20AC
  ON_PesoSign     = 0x20B1, // PESO SIGN U+20B1
  ON_RubleSign    = 0x20BD, // RUBLE SIGN U+20BD

  //////////////////////////////////////////////////////////////
  //
  // RECYCLING SYMBOL is useful for testing symbol font substitution
  //
  ON_RecyclingSymbol = 0x2672, // UNIVERSAL RECYCLING SYMBOL U+2672 (decimal 9842)
  ON_BlackRecyclingSymbol = 0x267B, // BLACK UNIVERSAL RECYCLING SYMBOL U+267B (decimal 9851)

  //////////////////////////////////////////////////////////////
  //
  // REPLACEMENT CHARACTER is the conventional glpyh used
  // to mark locations where UTF encodings contain invalid
  // information.
  //
  ON_ReplacementCharacter = 0xFFFD, // REPLACEMENT CHARACTER U+FFFD (decimal 65533)

  //////////////////////////////////////////////////////////////
  //
  // WASTEBASKET (Good value for testing UTF-16 surrogte pair handling)
  // 
  // wchar_t sWastebasket[] = {0xD83D,0xDDD1,0}; // correct on Windows. (Windows wchar_t strings are UTF-16 encoded).
  // wchar_t sWastebasket[] = {0x1F5D1,0}; // correct on OS X (OS X wchar_t strings are UTF-32 encoded).
  //
  // WASTEBASKET UTF-8 encodeing = (0xF0, 0x9F, 0x97, 0x91)
  // WASTEBASKET UTF-16 encodeing = ( 0xD83D, 0xDDD1 ) (surrogate pair)
  ON_Wastebasket = 0x1F5D1, // WASTEBASKET U+1F5D1 (decimal 128465)

  //////////////////////////////////////////////////////////////
  //
  // Valid codepoint values are <= 0x10FFFF 
  // See ON_IsValidUnicodeCodepoint() for additional restrictions.
  //
  ON_InvalidCodePoint = 0x110000
};

/*
Returns:
  ON_UTF_16BE 
    The byte order on where the function was run is big endian.
  ON_UTF_16L
    The byte order on where the function was run is little endian.
*/
ON_DECL
enum ON_UnicodeEncoding ON_UnicodeNativeCPU_UTF16();

/*
Returns:
  ON_UTF_32BE 
    The byte order on where the function was run is big endian.
  ON_UTF_32LE
    The byte order on where the function was run is little endian.
*/
ON_DECL
enum ON_UnicodeEncoding ON_UnicodeNativeCPU_UTF32();

/*
Description:
  Determine if the buffer has the values of a UTF BOM (byte order mark)
Parameters:
  buffer - [in]
    buffer to test
  sizeof_buffer - [in]
    number of bytes that can be examined in the buffer
Returns:
  ON_UTF_unset (0)
    buffer is not a UTF BOM
  ON_UTF_8
    sizeof_buffer >= 3 and the values fo the first three bytes
    are 0xEF, 0xBB, 0xBF.
  ON_UTF_16BE
    sizeof_buffer >= 2 and the values of the first two bytes
    are 0xFE, 0xFF and, if sizeof_buffer >= 4, the value of
    one of the thrid or forth byte is not zero.
  ON_UTF_16LE
    sizeof_buffer >= 2 and the values of the first two bytes
    are 0xFE, 0xFF
  ON_UTF_32BE
    sizeof_buffer >= 4 and the values of the first four bytes
    are 0x00, 0x00, 0xFE, 0xFF.
  ON_UTF_32LE
    sizeof_buffer >= 4 and the values of the first four bytes
    are 0xFF, 0xFE, 0x00, 0x00.
*/
ON_DECL
enum ON_UnicodeEncoding ON_IsUTFByteOrderMark(
  const void* buffer,
  size_t sizeof_buffer
  );

/*
Parameters:
  e - [in]
Returns:
  Number of bytes in byte order mark for the specified encoding.
*/
ON_DECL
unsigned int ON_UTFSizeofByteOrderMark(
  enum ON_UnicodeEncoding e
  );

/*
Description:
  Test a value to determine if it is a valid unicode code point value.
Parameters:
  u - [in] value to test
Returns:
  true: u is a valid unicode code point
  false: u is not a valid unicode code point
Remarks:
  Valid unicode code point values u satisfy
  (0 <= u && u <= 0xD7FF) || (0xE000 <= u && u <= 0x10FFFF)
*/
ON_DECL
int ON_IsValidUnicodeCodePoint(
  ON__UINT32 u
  );

/*
Description:
  Test a value to determine if it is a valid unicode code point value.
Parameters:
  w - [in] value to test
Returns:
  true: 
    w is a valid single wchar_t value
  false: 
    w is not a valid single wchar_t value. 
    It may be a completely invalid value for a string.
    When sizeof(w) < 4, is may also b a value used in a 
    multiple element encoding like a surrogate pair value.
Remarks:
  Valid 1 byte wchar_t UTF-8 unicode code points are
    (0 <= w && w <= 0x7F)
  Valid 2 byte wchar_t UTF-16 unicode code points are
    (0 <= w && w <= 0xD7FF) || (0xE000 <= w && w <= 0xFFFF)
  Valid 4 bytes wchar_t UTF-32 unicode code points are
    (0 <= u && u <= 0xD7FF) || (0xE000 <= u && u <= 0x10FFFF)
*/
ON_DECL
int ON_IsValidSingleElementWideCharValue(
  wchar_t w
  );

ON_DECL
int ON_IsValidUTF16SurrogatePair(
  unsigned int w1,
  unsigned int w2
  );

/*
Description:
  Test a value to determine if it is a valid UTF-32 value.
Parameters:
  c - [in] value to test
Returns:
  true: c is a valid UTF-32 value
  false: c is not a valid UTF-32 value
Remarks:
  Valid single element UTF-32 values are
  (0 <= u && u <= 0xD7FF) || (0xE000 <= u && u <= 0x10FFFF)
*/
ON_DECL
int ON_IsValidUTF32Value(
  ON__UINT32 c
  );


/*
Description:
    Test a value to determine if it is a valid single element UTF-16 value.
Parameters:
    c - [in] value to test
Returns:
  true: c is a valid single wchar_t unicode code point
  false: c is not a valid unicode code point
  w is not a valid single element UTF-16 value.
  It may be a completely invalid value or it
  may be a value used in a surrogate pair.
Remarks:
  Valid single element UTF-16 values are
  (0 <= c && c <= 0xD7FF) || (0xE000 <= c && c <= 0xFFFF)
*/
ON_DECL
int ON_IsValidUTF16Singleton(
  ON__UINT32 c
  );

// ON_DEPRECATED
ON_DECL
int ON_IsValidSingleElementUTF16Value(
ON__UINT32 c
);



/*
Description:
  Test a value to determine if it is a valid single byte UTF-8 value.
Parameters:
  c - [in] value to test
Returns:
  true: c is a valid single byte UTF-8 value
  false: c is not a valid single byte UTF-8 value
Remarks:
  Valid single byte UTF-8 values are (0 <= w && w <= 0x7F)
*/
ON_DECL
int ON_IsValidUTF8SingletonChar(
  char c
  );
// ON_DEPRECATED
ON_DECL
int ON_IsValidSingleByteUTF8CharValue(
  char c
  );


/*
Description:
  Test a value to determine if it is a valid single byte UTF-8 value.
Parameters:
  c - [in] value to test
Returns:
true: c is a valid single byte UTF-8 value
false: c is not a valid single byte UTF-8 value
Remarks:
  Valid single byte UTF-8 values are (0 <= c && c <= 0x7F)
*/
ON_DECL
int ON_IsValidUTF8Singleton(
  ON__UINT32 c
  );

// ON_DEPRECATED
ON_DECL
int ON_IsValidSingleElementUTF8Value(
  ON__UINT32 c
  );


struct ON_CLASS ON_UnicodeErrorParameters
{
#if defined(ON_CPLUSPLUS)
  // 
  static const ON_UnicodeErrorParameters MaskErrors; // m_error_status = 0, m_error_mask = 0xFFFFFFFF, m_error_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter
  static const ON_UnicodeErrorParameters FailOnErrors; // m_error_status = 0, m_error_mask = 0, m_error_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter
#endif

  /*
  If an error occurs, then bits of error_status are
  set to indicate what type of error occured.

  Error types:
    1: The input parameters were invalid. 
       This error cannot be masked.

    2: The ouput buffer was not large enough to hold the converted
       string. As much conversion as possible is performed in this
       case and the error cannot be masked.

    4: When parsing a UTF-8 or UTF-32 string, the values of two
       consecutive encoding sequences formed a valid UTF-16 
       surrogate pair. 
          
       This error is masked if 0 != (4 & m_error_mask).
       If the error is masked, then the surrogate pair is
       decoded, the value of the resulting unicode code point
       is used, and parsing continues.

    8: An overlong UTF-8 encoding sequence was encountered and 
       the value of the overlong sUTF-8 equence was a valid
       unicode code point. 
          
       This error is masked if 0 != (8 & m_error_mask).
       If the error is masked, then the unicode code point
       is used and parsing continues.

   16: An illegal UTF-8, UTF-16 or UTF-32 sequence occured, 
      or an unsupported or invalid Windows code page value,      
      or an invalid unicode code point value resulted from 
      decoding a UTF-8 sequence. 

       This error is masked if 0 != (16 & m_error_mask).
       If the error is masked and the value of m_error_code_point is
       a valid unicode code point, then m_error_code_point is used
       and parsing continues.
  */
  unsigned int m_error_status;

  /*
  If 0 != (error_mask & 4), then type 4 errors are masked.
  If 0 != (error_mask & 8), then type 8 errors are masked.
  If 0 != (error_mask & 16) and m_error_code_point is a valid unicode
  code point value, then type 16 errors are masked.
  */
  unsigned int m_error_mask;

  /*
  Unicode code point value to use in when masking type 16 errors.
  If 0 == (error_mask & 16), then this parameter is ignored.
  ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
  is a popular choice for the m_error_code_point value.
  */
  ON__UINT32 m_error_code_point;
};

/*
Description:
  Decode a UTF-32 little endian byte order string to get a single unicode code point.
Parameters:
  sUTF32 - [in]
    UTF-32 little byte order string to convert.

  sUTF32_count - [in]
    number of ON__UINT32 elements in sUTF32[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF32 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF32[0] was decoded was a valid 
     UTF-32 value. See e for masked errors.
  2:
      sUTF32[0],sUTF32[1] had values of a valid UTF-16 surrogate pair
      and e indicated to mask this error.  The UTF-16 code point
      value was returned and e was set to indicate the error occured.
*/
ON_DECL
int ON_DecodeUTF32LE(
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Decode a UTF-32 big endian byte order string to get a single unicode code point.
Parameters:
  sUTF32 - [in]
    UTF-32 big byte order string to convert.

  sUTF32_count - [in]
    number of ON__UINT32 elements in sUTF32[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF32 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF32[0] was decoded was a valid 
     UTF-32 value. See e for masked errors.
  2:
      sUTF32[0],sUTF32[1] had values of a valid UTF-16 surrogate pair
      and e indicated to mask this error.  The UTF-16 code point
      value was returned and e was set to indicate the error occured.
*/
ON_DECL
int ON_DecodeUTF32BE(
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );


/*
Description:
  Decode a UTF-32 native byte order string to get a single unicode code point.
Parameters:
  sUTF32 - [in]
    UTF-32 native byte order string to convert.

  sUTF32_count - [in]
    number of ON__UINT32 elements in sUTF32[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF32 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF32[0] was decoded was a valid 
     UTF-32 value. See e for masked errors.
  2:
      sUTF32[0],sUTF32[1] had values of a valid UTF-16 surrogate pair
      and e indicated to mask this error.  The UTF-16 code point
      value was returned and e was set to indicate the error occured.
*/
ON_DECL
int ON_DecodeUTF32(
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Decode a UTF-32 string whose elements have byte order
  opposite the native CPU's to get a single unicode code point.
Parameters:
  sUTF32 - [in]
    UTF-32 string to convert with byte order opposite the
    CPU's native byte order.

  sUTF32_count - [in]
    number of ON__UINT32 elements in sUTF32[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF32 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF32[0] was decoded was a valid 
     UTF-32 value. See e for masked errors.
  2:
      sUTF32[0],sUTF32[1] had values of a valid UTF-16 surrogate pair
      and e indicated to mask this error.  The UTF-16 code point
      value was returned and e was set to indicate the error occured.
*/
ON_DECL
int ON_DecodeSwapByteUTF32(
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Convert valid unicode code point values to its UTF-8 form and use the 
  same encoding calculation for other integers with values <= 0x7FFFFFFF.
  When strict UTF-8 encoding is desired, the caller is responsible for 
  insuring the value of u is a valid uncode codepoint.
Parameters:
  u - [in]
    Interger in the CPU's native byte order in the interval [0,2147483647].
  sUTF8 - [out]
    sUTF8 is a buffer of 6 char elements and the UTF-8 form
    is returned in sUTF8[]. The returned value specifies how 
    many elements of sUTF8[] are set.
Returns:
  0: u is too large (>=2^31) to be encode.
     No changes are made to the sUTF8[] values.
  1: the UTF-8 form of u is 1 byte returned in sUTF8[0].
  2: the UTF-8 form of u is 2 byts returned in sUTF8[0],sUTF8[1].
  3: the UTF-8 form of u is 3 bytes returned in sUTF8[0],sUTF8[1],sUTF8[2].
  4: the UTF-8 form of u is 4 bytes returned in sUTF8[0],sUTF8[1],sUTF8[2],sUTF8[3].
     Note: The maximum valid unicode codepoint is 0x10FFFF.  Values of u > 0x10FFFF
     and u <= 0x1FFFFF are encoded to 4 bytes using the same algorithm.
  5: the Universal Character Set form of u 
     is 5 bytes returned in sUTF8[0],sUTF8[1],sUTF8[2],sUTF8[3],sUTF8[4].
  6: the Universal Character Set form of u
     is 6 bytes returned in sUTF8[0],sUTF8[1],sUTF8[2],sUTF8[3],sUTF8[4],sUTF8[5].
  For return values requiring less than 6 bytes, no changes
  are made to the unused bytes in sUTF8[].
Remarks:
  Any integer in the range 0 to 2^31 - 1 can be encoded.
  When a unicode string is being encoded take steps to ensure that
  u is a valid unicode code point value.  
  The function ON_IsValidUnicodeCodePoint() can be used to determine
  if u is a valid unicode code point value.
*/
ON_DECL
int ON_EncodeUTF8( ON__UINT32 u, char sUTF8[6] );

/*
Description:
  Decode a UTF-8 encode string to get a single unicode code point.
Parameters:
  sUTF8 - [in]
    UTF-8 string to convert.

  sUTF8_count - [in]
    number of char elements in sUTF8[].

   e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value.
Returns:
  Number of elements of sUTF8 that were parsed.
  0 indicates failure.
*/
ON_DECL
int ON_DecodeUTF8(
    const char* sUTF8,
    int sUTF8_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Convert a 4 byte unicode code point value to its UTF-16 form.
Parameters:
  unicode_code_point - [in]
    4 byte unicode code point value in the CPU's native byte order.
    Valid values are in the interval [0,0xD7FF] or the 
    interval [0xE000,0x10FFFF].
  sUTF16 - [out]
    sUTF16 is buffer of 2 ON__UINT16 elements. If the UTF-16 form
    is a single value, it is returned in sUTF16[0]. If the UTF-16
    is a surrogate pair, the first code unit (high surrogate) 
    is returned sUTF16[0] and the second unit (low surrogate) is
    returned in sUTF16[1].  The returned values are in
    the CPU's native byte order.
Returns:
  0: u is not a valid Unicode code point. No changes are
     made to the sUTF16[] values.
  1: u is a valid Unicode code point with a UTF-16 form 
     consisting of the single value returned in sUTF16[0].
  2: u is a valid Unicode code point with a UTF-16 form 
     consisting of a surrogate pair returned in sUTF16[0] and sUTF16[1].
*/
ON_DECL
int ON_EncodeUTF16( ON__UINT32 unicode_code_point, ON__UINT16 sUTF16[2] );

/*
Description:
  Decode a UTF-16 little endian byte order string to get a single unicode code point.
Parameters:
  sUTF16 - [in]
    UTF-16 little endian byte order string to convert.

  sUTF16_count - [in]
    number of ON__UINT16 elements in sUTF16[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF16 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF16[0] was decoded as a valid 
     UTF-16 singleton. See e for masked errors.
  2: 
     If no error occured, then sUTF16[0],sUTF16[1] was decoded 
     as a valid UTF-16 surrogate pair.
     See e for masked errors.
  n >= 3:
      sUTF16[0],..,sUTF16[n-1] did not forma valid UTF-16 encoding
      and were parsed as reasonably as possible.
      See e for masked errors.
*/
ON_DECL
int ON_DecodeUTF16LE(
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Decode a UTF-16 big endian byte order string to get a single unicode code point.
Parameters:
  sUTF16 - [in]
    UTF-16 big endian byte order string to convert.

  sUTF16_count - [in]
    number of ON__UINT16 elements in sUTF16[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF16 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF16[0] was decoded as a valid 
     UTF-16 singleton. See e for masked errors.
  2: 
     If no error occured, then sUTF16[0],sUTF16[1] was decoded 
     as a valid UTF-16 surrogate pair.
     See e for masked errors.
  n >= 3:
      sUTF16[0],..,sUTF16[n-1] did not forma valid UTF-16 encoding
      and were parsed as reasonably as possible.
      See e for masked errors.
*/
ON_DECL
int ON_DecodeUTF16BE(
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Decode a UTF-16 string in native byte order to get a single unicode code point.
Parameters:
  sUTF16 - [in]
    UTF-16 string in native byte order to convert.

  sUTF16_count - [in]
    number of ON__UINT16 elements in sUTF16[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF16 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF16[0] was decoded as a valid 
     UTF-16 singleton. See e for masked errors.
  2: 
     If no error occured, then sUTF16[0],sUTF16[1] was decoded 
     as a valid UTF-16 surrogate pair.
     See e for masked errors.
  n >= 3:
      sUTF16[0],..,sUTF16[n-1] did not forma valid UTF-16 encoding
      and were parsed as reasonably as possible.
      See e for masked errors.
*/
ON_DECL
int ON_DecodeUTF16(
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Decode a UTF-16 encode string whose elements have byte order
  opposite native byte order  to get a single unicode code point.
Parameters:
  sUTF16 - [in]
    UTF-16 string to convert with byte order opposite the
    CPU's native byte order.

  sUTF16_count - [in]
    number of ON__UINT16 elements in sUTF16[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF16 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sUTF16[0] was decoded as a valid 
     UTF-16 singleton. See e for masked errors.
  2: 
     If no error occured, then sUTF16[0],sUTF16[1] was decoded 
     as a valid UTF-16 surrogate pair.
     See e for masked errors.
  n >= 3:
      sUTF16[0],..,sUTF16[n-1] did not forma valid UTF-16 encoding
      and were parsed as reasonably as possible.
      See e for masked errors.
*/
ON_DECL
int ON_DecodeSwapByteUTF16(
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );

/*
Description:
  Decode a UTF-16 encode string whose elements have byte order
  opposite the native CPU's to get a single unicode code point.
Parameters:
  sWideChar - [in]
    wchar_t string to convert.

  sWideChar_count - [in]
    number of wchar_t elements in sWideChar[].

  e - [in/out] 
    If e is null, errors are not masked and parsing is performed
    to the point where the first error occurs.
    If e is not null, all errors are reported by setting the appropriate
    e->m_error_status bits and errors are handled as described in the
    definition of the ON_UnicodeErrorParameters struct.

    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
    or
    ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::FailOnErrors;
    are good ways to initialize the input values.

  unicode_code_point - [out]
    The unicode_code_point pointer must not be null.
    If a nonzero value is returned, then *unicode_code_point is
    a valid unicode code point value in the CPU's native byte order.
Returns:
  Number of elements of sUTF16 that were parsed.
  0: 
     Nothing was decoded. The input value of *unicode_code_point
     is not changed.  See e->m_error_status.
  1: 
     If no error occured, then sWideChar[0] was decoded as a valid 
     wchar_t singleton. See e for masked errors.
  n>=2: 
     If no error occured, then sWideChar[0],..,sWideChar[n-1] was decoded 
     as a valid wchar_t multi-element encoding.
     Typically, UTF-16 surrogate pair or UTF-8 multi-byte sequence.
     See e for masked errors.
*/
ON_DECL
int ON_DecodeWideChar(
    const wchar_t* sWideChar,
    int sWideChar_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    );


/*
Description:
  Convert an RFT charset value to a Windows code page.
  This conversion is part of the process to get a UNICODE encoding of strings in RTF files.

Parameters:
  rtf_charset - [in]
    The RTF charset specifed by /fcharsetN in the RTF font table.
  default_code_page - [out]
    Value to return if none is associated with the input rtf_charset value.

Returns:
  code page

Example:

  The RTF:
      ...
      {\fonttbl
      ...
      {\f2\fcharset129 Malgun Gothic;}
      ...
      }
      ...
      {\f2 {\ltrch \'c7\'d1\'b1\'db...}
      ...

  Uses RTF charset 129 which maps to Windows code page 949. This means
  {0xC7,0xD1,0xB1,0xBD, ... } needs to be parsed as a code page 949 multibyte encoding.
  The function ON_MapWindowsCodePage949ToUnicode() can be used to convert
  Windows code page 949 values to UNICODE code point values.

    code page 949 0xC7D1 -> U+D55C #HANGUL SYLLABLE HIEUH A NIEUN
    code page 949 0xB1BD -> U+AD75 #HANGUL SYLLABLE KIYEOK U RIEULKIYEOK

  NOTE WELL: 
  The Windows code page 949 encoding uses both single and double byte encodings.
  When the initial byte has a value < 0x80, it is a single byte encoding.
  When the initial byte has a value > 0x80, it is a double byte encoding.

Remarks:
  Conversions are based on the Rich Text Format (RTF) Specification Version 1.9.1
*/
ON_DECL
ON__UINT32 ON_MapRTFcharsetToWindowsCodePage(
  ON__UINT32 rtf_charset,
  ON__UINT32 default_code_page
);

/*
Description:
  Get a pointer to an array of 128 UNICODE code point values that are the best fit
  for Microsoft single byte code page encodings of 0x80 to 0xFF inclusive.

Parameters:
  code_page - [in]
    A Microsoft single byte code page value. (1252, 10000, etc)
Returns:
  If code_page identifies a supported single byte code page, then an array 
  of 128 UNICODE code points sorted by single byte encoding is returned.
  If a the single byte encoding is not defined, the corresponding element
  Otherwise nullptr is returned.

Example:

      const ON__UINT32 code_page = ...;
      ON__UINT32 cp_encoding = ...;
      const ON__UINT32* cp_to_unicode = ON_MSSBCP_0x80_0xFF_Unicode(code_page);
      ON__UINT32 unicode_code_point
      = (nullptr != cp_to_unicode && cp_encoding >= 0x80 && cp_encoding <= 0xFF)
      ? cp_to_unicode[cp_encoding - 0x80]
      : ON_UnicodeCodePoint::ON_ReplacementCharacter;

*/
ON_DECL
const ON__UINT32* ON_MSSBCP_0x80_0xFF_Unicode(
  ON__UINT32 code_page
  );

/*
Description:
  Convert a Microsoft single byte code page value to a UNICODE code point.
  Values 0x20 to 0x7E are the same as the ASCII encoding.

Parameters:
  code_page - [in]
    A Microsoft single byte code page value. (1252, 10000, etc)
  code_page_single_byte_encoding - [in]
    A single byte encoding of the desired glpyh.

Returns:
  If cod page and code_page_single_byte_encoding are valid, then
  the best fit unicode code point is returned.
  Otherwise ON_UnicodeCodePoint::ON_ReplacementCharacter ( 0xFFFD ) is returned.
*/
ON_DECL
ON__UINT32 ON_MapMSSBCPToUnicode(
  ON__UINT32 code_page,
  ON__UINT32 code_page_single_byte_encoding
);

/*
Description:
  Convert a Unicode code point to a Microsoft code page 1252 character value.
  Windows code page 1252 is a single byte encoding.
  Values 0x20 to 0x7E are the same as the ASCII encoding.

  This function is used to find fonts where glpyhs are identified by code page 1252 values.

Parameters:
  code_page - [in]
    A Microsoft single byte code page value. (1252, 10000, etc)
  unicode_code_point - [in]
    UNICODE code point

Returns:
  If unicode_code_point has a corresponding single byte encoding on the specified code page,
  then the single byte encoding is returned.
  Otherwise 0xFFFFFFFF is returned.
*/
ON_DECL
ON__UINT32 ON_MapUnicodeToMSSBCP(
  ON__UINT32 code_page,
  ON__UINT32 unicode_code_point
);

/*
Description:
  Convert unicode code point values to its wide char form.
Parameters:
  code_point - [in]
    Unicode code point in the CPU's native byte order.
  w_capacity - [in]
    If the platform wchar_t encoding is UTF-32, then w_capacity >= 1 is sufficient.
    If the platform wchar_t encoding is UTF-16, then w_capacity >= 2 is sufficient.
    If the platform wchar_t encoding is UTF-8, then w_capacity >= 6 is sufficient.
  w - [out]
    w is a buffer of w_capacity wchar_t elements and the wide char
    encoding of code_point is returned in w[].
    The returned value specifies how  many elements of w[] are set.
    When w_capacity > the returned value, the encoding is null terminated.
Returns:
  0: Invalid input (code_point is not a valid Unicode code point or
     w is nullptr).
     No changes are made to the w[] values.
  1: the wchar_t encoding of code_point is 1 wchar_t element returned in w[0].
  2: the wchar_t encoding form of code_point is 2 wchar_t element returned in w[0],w[1].
  3: the UTF-8 wchar_t encoding form of code_point is 3 wchar_t element returned in w[0],w[1],w[2].
  4: the UTF-8 wchar_t encoding form of code_point is 4 wchar_t element returned in w[0],w[1],w[2],w[3].
  5: the UTF-8 wchar_t encoding form of code_point is 5 wchar_t element returned in w[0],w[1],w[2],w[3],w[4].
  6: the UTF-8 wchar_t encoding form of code_point is 6 wchar_t element returned in w[0],w[1],w[2],w[3],w[4],w[5].
*/
ON_DECL
int ON_EncodeWideChar( 
  ON__UINT32 code_point, 
  size_t w_capacity,
  wchar_t* w
  );

/*
Description:
  Convert a unicode string from a UTF-8 encoded char array
  into a UTF-8 encoded char array.  This function can be
  used to clean UTF-8 strings that have a leading
  byte-order-mark (BOM) or contain encoding errors.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sInputUTF8 - [in]
    UTF-8 encoded string to convert.

  sInputUTF8_count - [in]
    If sInputUTF8_count >= 0, then it specifies the number of
    char elements in sInputUTF8[] to convert.

    If sInputUTF8_count == -1, then sInputUTF8 must be a null
    terminated string and all the elements up to the first 
    null element are converted.

  sOutputUTF8 - [out]
    If sOutputUTF8 is not null and sOutputUTF8_count > 0, then 
    the output UTF-8 encoded string is returned in this buffer. 
    If there is room for the null terminator, the converted string
    will be null terminated. The null terminator is never included
    in the count returned by this function. No byte order mark is
    prepended.

  sOutputUTF8_count - [in]
    If sOutputUTF8_count > 0, then it specifies the number of available
    char elements in the sOutputUTF8[] buffer.
    
    If sOutputUTF8_count == 0, then the sOutputUTF8 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextInputUTF8 - [out]
    If sNextInputUTF8 is not null, then *sNextInputUTF8 points to
    the first element in the input sInputUTF8[] buffer that was not 
    converted. 

    If an error occurs and is not masked, then *sNextInputUTF8 points
    to the element of sInputUTF8[] where the conversion failed.
    If no errors occur or all errors are masked, then 
    *sNextInputUTF8 points to sInputUTF8 + sInputUTF8_count.

Returns:
  If sOutputUTF8_count > 0, the return value is the number of char
  elements written to sOutputUTF8[].  When the return value < sOutputUTF8_count,
  a null terminator is written to sOutputUTF8[return value].

  If sOutputUTF8_count == 0, the return value is the minimum number of
  char elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF8ToUTF8(
    int bTestByteOrder,
    const char* sInputUTF8,
    int sInputUTF8_count,
    char* sOutputUTF8,
    int sOutputUTF8_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const char** sNextInputUTF8
    );

/*
Description:
  Convert a unicode string from a UTF-8 encoded char array
  into a UTF-16 encoded ON__UINT16 array.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF8 - [in]
    UTF-8 string to convert.

  sUTF8_count - [in]
    If sUTF8_count >= 0, then it specifies the number of
    char elements in sUTF8[] to convert.

    If sUTF8_count == -1, then sUTF8 must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sUTF16 - [out]
    If sUTF16 is not null and sUTF16_count > 0, then the UTF-16
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF16_count - [in]
    If sUTF16_count > 0, then it specifies the number of available
    ON__UINT16 elements in the sUTF16[] buffer.
    
    If sUTF16_count == 0, then the sUTF16 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF8 - [out]
    If sNextUTF8 is not null, then *sNextUTF8 points to the first
    element in the input sUTF8[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF8 points to
    the element of sUTF8[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF8 points to
    sUTF8 + sUTF8_count.

Returns:
  If sUTF16_count > 0, the return value is the number of ON__UINT16
  elements written to sUTF16[].  When the return value < sUTF16_count,
  a null terminator is written to sUTF16[return value].

  If sUTF16_count == 0, the return value is the minimum number of
  ON__UINT16 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF8ToUTF16(
    int bTestByteOrder,
    const char* sUTF8,
    int sUTF8_count,
    ON__UINT16* sUTF16,
    int sUTF16_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const char** sNextUTF8
    );

/*
Description:
  Convert a unicode string from a UTF-8 encoded char array
  into a UTF-32 encoded ON__UINT32 array.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF8 - [in]
    UTF-8 string to convert.

  sUTF8_count - [in]
    If sUTF8_count >= 0, then it specifies the number of
    char elements in sUTF8[] to convert.

    If sUTF8_count == -1, then sUTF8 must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sUTF32 - [out]
    If sUTF32 is not null and sUTF32_count > 0, then the UTF-32
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF32_count - [in]
    If sUTF32_count > 0, then it specifies the number of available
    ON__UINT32 elements in the sUTF32[] buffer.
    
    If sUTF32_count == 0, then the sUTF32 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF8 - [out]
    If sNextUTF8 is not null, then *sNextUTF8 points to the first
    element in the input sUTF8[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF8 points to
    the element of sUTF8[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF8 points to
    sUTF8 + sUTF8_count.

Returns:
  If sUTF32_count > 0, the return value is the number of ON__UINT32
  elements written to sUTF32[].  When the return value < sUTF32_count,
  a null terminator is written to sUTF32[return value].

  If sUTF32_count == 0, the return value is the minimum number of
  ON__UINT32 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF8ToUTF32(
    int bTestByteOrder,
    const char* sUTF8,
    int sUTF8_count,
    ON__UINT32* sUTF32,
    int sUTF32_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const char** sNextUTF8
    );

/*
Description:
  Convert a unicode string from a UTF-16 encoded ON__UINT16 array
  into a UTF-8 encoded char array.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF16 - [in]
    UTF-16 string to convert.  
    
    If bTestByteOrder is true and the first element of sUTF16[]
    is 0xFEFF, then this element is skipped and it is assumed 
    that sUTF16[] is in the CPU's native byte order.
    
    If bTestByteOrder is true and the first element of sUTF16[]
    is 0xFFFE, then this element is skipped and it is assumed 
    that sUTF16[] is not in the CPU's native byte order and bytes
    are swapped before characters are converted.

    If bTestByteOrder is false or the first character of sUTF16[]
    is neither 0xFEFF nor 0xFFFE, then the sUTF16 string must match
    the CPU's byte order.

  sUTF16_count - [in]
    If sUTF16_count >= 0, then it specifies the number of
    ON__UINT16 elements in sUTF16[] to convert.

    If sUTF16_count == -1, then sUTF16 must be a null terminated
    string and all the elements up to the first null element are
    converted.
    
  sUTF8 - [out]
    If sUTF8 is not null and sUTF8_count > 0, then the UTF-8
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF8_count - [in]
    If sUTF8_count > 0, then it specifies the number of available
    char elements in the sUTF8[] buffer.
    
    If sUTF8_count == 0, then the sUTF8 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF16 - [out]
    If sNextUTF16 is not null, then *sNextUTF16 points to the first
    element in the input sUTF16[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF16 points to
    the element of sUTF16[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF16 points to
    sUTF16 + sUTF16_count.

Returns:
  If sUTF8_count > 0, the return value is the number of char
  elements written to sUTF8[].  When the return value < sUTF8_count,
  a null terminator is written to sUTF8[return value].

  If sUTF8_count == 0, the return value is the minimum number of
  char elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF16ToUTF8(
    int bTestByteOrder,
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    char* sUTF8,
    int sUTF8_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT16** sNextUTF16
    );

/*
Description:
  Convert a unicode string from a UTF-16 encoded ON__UINT16 array
  into a UTF-16 encoded ON__UINT16 array. This is not simply
  a copy in the case when the input has a byte order mark (BOM),
  different byte ordering or contains errors.  This function can
  be used to validate UTF-16 encoded strings.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sInputUTF16 - [in]
    UTF-16 encoded string to convert.

  sInputUTF16_count - [in]
    If sInputUTF16_count >= 0, then it specifies the number of
    ON__UINT16 elements in sInputUTF16[] to convert.

    If sInputUTF16_count == -1, then sInputUTF16 must be a null
    terminated array and all the elements up to the first 
    null element are converted.

  sOutputUTF16 - [out]
    If sOutputUTF16 is not null and sOutputUTF16_count > 0, then 
    the output UTF-16 encoded string is returned in this buffer. 
    If there is room for the null terminator, the converted string
    will be null terminated. The null terminator is never included
    in the count returned by this function. No byte order mark is
    prepended.

  sOutputUTF16_count - [in]
    If sOutputUTF16_count > 0, then it specifies the number of available
    ON__UINT16 elements in the sOutputUTF16[] buffer.
    
    If sOutputUTF16_count == 0, then the sOutputUTF16 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextInputUTF16 - [out]
    If sNextInputUTF16 is not null, then *sNextInputUTF16 points to
    the first element in the input sInputUTF16[] buffer that was not 
    converted. 

    If an error occurs and is not masked, then *sNextInputUTF16 points
    to the element of sInputUTF16[] where the conversion failed.
    If no errors occur or all errors are masked, then 
    *sNextInputUTF16 points to sInputUTF16 + sInputUTF16_count.

Returns:
  If sOutputUTF16_count > 0, the return value is the number of ON__UINT16
  elements written to sOutputUTF16[].  When the return value < sOutputUTF16_count,
  a null terminator is written to sOutputUTF16[return value].

  If sOutputUTF16_count == 0, the return value is the minimum number of
  ON__UINT16 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF16ToUTF16(
    int bTestByteOrder,
    const ON__UINT16* sInputUTF16,
    int sInputUTF16_count,
    ON__UINT16* sOutputUTF16,
    int sOutputUTF16_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT16** sNextInputUTF16
    );

/*
Description:
  Convert a unicode string from a UTF-16 encoded ON__UINT16 array
  into a UTF-32 encoded ON__UINT32 array.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF16 - [in]
    UTF-16 string to convert.  
    
    If bTestByteOrder is true and the first element of sUTF16[]
    is 0xFEFF, then this element is skipped and it is assumed 
    that sUTF16[] is in the CPU's native byte order.
    
    If bTestByteOrder is true and the first element of sUTF16[]
    is 0xFFFE, then this element is skipped and it is assumed 
    that sUTF16[] is not in the CPU's native byte order and bytes
    are swapped before characters are converted.

    If bTestByteOrder is false or the first character of sUTF16[]
    is neither 0xFEFF nor 0xFFFE, then the sUTF16 string must match
    the CPU's byte order.

  sUTF16_count - [in]
    If sUTF16_count >= 0, then it specifies the number of
    ON__UINT16 elements in sUTF16[] to convert.

    If sUTF16_count == -1, then sUTF16 must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sUTF32 - [out]
    If sUTF32 is not null and sUTF32_count > 0, then the UTF-32
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF32_count - [in]
    If sUTF32_count > 0, then it specifies the number of available
    ON__UINT32 elements in the sUTF32[] buffer.
    
    If sUTF32_count == 0, then the sUTF32 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF16 - [out]
    If sNextUTF16 is not null, then *sNextUTF16 points to the first
    element in the input sUTF16[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF16 points to
    the element of sUTF16[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF16 points to
    sUTF16 + sUTF16_count.

Returns:
  If sUTF32_count > 0, the return value is the number of ON__UINT32
  elements written to sUTF32[].  When the return value < sUTF32_count,
  a null terminator is written to sUTF32[return value].

  If sUTF32_count == 0, the return value is the minimum number of
  ON__UINT32 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF16ToUTF32(
    int bTestByteOrder,
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    unsigned int* sUTF32,
    int sUTF32_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT16** sNextUTF16
    );

/*
Description:
  Convert a unicode string from a UTF-32 encoded ON__UINT32 array
  into a UTF-8 encoded char array.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF32 - [in]
    UTF-32 string to convert.  
    
    If bTestByteOrder is true and the first element of sUTF32[]
    is 0x0000FEFF, then this element is skipped and it is assumed 
    that sUTF32[] is in the CPU's native byte order.
    
    If bTestByteOrder is true and the first element of sUTF32[]
    is 0xFFFE0000, then this element is skipped and it is assumed 
    that sUTF32[] is not in the CPU's native byte order and bytes
    are swapped before characters are converted.

    If bTestByteOrder is false or the first character of sUTF32[]
    is neither 0x0000FEFF nor 0xFFFE0000, then the sUTF32 string 
    must match the CPU's byte order.

  sUTF32_count - [in]
    If sUTF32_count >= 0, then it specifies the number of
    ON__UINT32 elements in sUTF32[] to convert.

    If sUTF32_count == -1, then sUTF32 must be a null terminated
    string and all the elements up to the first null element are
    converted.
    
  sUTF8 - [out]
    If sUTF8 is not null and sUTF8_count > 0, then the UTF-8
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF8_count - [in]
    If sUTF8_count > 0, then it specifies the number of available
    char elements in the sUTF8[] buffer.
    
    If sUTF8_count == 0, then the sUTF8 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF32 - [out]
    If sNextUTF32 is not null, then *sNextUTF32 points to the first
    element in the input sUTF32[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF32 points to
    the element of sUTF32[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF32 points to
    sUTF32 + sUTF32_count.

Returns:
  If sUTF8_count > 0, the return value is the number of char
  elements written to sUTF8[].  When the return value < sUTF8_count,
  a null terminator is written to sUTF8[return value].

  If sUTF8_count == 0, the return value is the minimum number of
  char elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF32ToUTF8(
    int bTestByteOrder,
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    char* sUTF8,
    int sUTF8_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT32** sNextUTF32
    );

/*
Description:
  Convert a unicode string from a UTF-32 encoded ON__UINT32 array
  into a UTF-16 encoded ON__UINT16 array.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF32 - [in]
    UTF-32 string to convert.  
    
    If bTestByteOrder is true and the first element of sUTF32[]
    is 0x0000FEFF, then this element is skipped and it is assumed 
    that sUTF32[] is in the CPU's native byte order.
    
    If bTestByteOrder is true and the first element of sUTF32[]
    is 0xFFFE0000, then this element is skipped and it is assumed 
    that sUTF32[] is not in the CPU's native byte order and bytes
    are swapped before characters are converted.

    If bTestByteOrder is false or the first character of sUTF32[]
    is neither 0x0000FEFF nor 0xFFFE0000, then the sUTF32 string 
    must match the CPU's byte order.

  sUTF32_count - [in]
    If sUTF32_count >= 0, then it specifies the number of
    ON__UINT32 elements in sUTF32[] to convert.

    If sUTF32_count == -1, then sUTF32 must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sUTF16 - [out]
    If sUTF16 is not null and sUTF16_count > 0, then the UTF-16
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF16_count - [in]
    If sUTF16_count > 0, then it specifies the number of available
    ON__UINT16 elements in the sUTF16[] buffer.
    
    If sUTF16_count == 0, then the sUTF16 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUnicode - [out]
    If sNextUnicode is not null, then *sNextUnicode points to the first
    byte in the input sNextUnicode[] buffer that was not converted. 

    If an error occurs and is not masked, then this unsigned int
    will be an illegal unicode code point value.

    If an error does not occur, then (*sNextUnicode - sUnicode) 
    is the number of values converted.

Returns:
  If sUTF16_count > 0, the return value is the number of ON__UINT16
  elements written to sUTF16[].  When the return value < sUTF16_count,
  a null terminator is written to sUTF16[return value].

  If sUTF16_count == 0, the return value is the minimum number of
  ON__UINT16 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF32ToUTF16(
    int bTestByteOrder,
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    ON__UINT16* sUTF16,
    int sUTF16_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT32** sNextUTF32
    );

/*
Description:
  Convert a unicode string from a UTF-32 encoded ON__UINT32 array
  into a UTF-32 encoded ON__UINT32 array.  This is not simply
  a copy in the case when the input has a byte order mark (BOM),
  different byte ordering or contains errors.  This function can
  be used to validate UTF-32 encoded strings.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sInputUTF32 - [in]
    UTF-32 string to convert.  
    
    If bTestByteOrder is true and the first element of sInputUTF32[]
    is 0x0000FEFF, then this element is skipped and it is assumed 
    that sInputUTF32[] is in the CPU's native byte order.
    
    If bTestByteOrder is true and the first element of sInputUTF32[]
    is 0xFFFE0000, then this element is skipped and it is assumed 
    that sInputUTF32[] is not in the CPU's native byte order and bytes
    are swapped before characters are converted.

    If bTestByteOrder is false or the first character of sUTF32[]
    is neither 0x0000FEFF nor 0xFFFE0000, then the sUTF32 string 
    must match the CPU's byte order.

  sInputUTF32_count - [in]
    If sInputUTF32_count >= 0, then it specifies the number of
    ON__UINT32 elements in sInputUTF32[] to convert.

    If sInputUTF32_count == -1, then sInputUTF32 must be a null 
    terminated string and all the elements up to the first null
    element are converted.

  sOutputUTF32 - [out]
    If sOutputUTF32 is not null and sOutputUTF32_count > 0, then
    the UTF-32 encoded string is returned in this buffer. If there
    is room for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sOutputUTF32_count - [in]
    If sOutputUTF32_count > 0, then it specifies the number of available
    ON__UINT32 elements in the sOutputUTF32[] buffer.
    
    If sOutputUTF32_count == 0, then the sOutputUTF32 parameter
    is ignored.  This is useful when you want to validate a UTF-32
    formatted string.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextInputUTF32 - [out]
    If sNextInputUTF32 is not null, then *sNextInputUTF32 points to
    the first element in the input sInputUTF32[] buffer that was not
    converted. 

    If an error occurs and is not masked, then this unsigned int
    will be an illegal unicode code point value.

    If an error does not occur, then (*sNextInputUTF32 - sInputUTF32) 
    is the number of values converted.

Returns:
  If sOutputUTF32_count > 0, the return value is the number of ON__UINT32
  elements written to sOutputUTF32[].  
  When the return value < sOutputUTF32_count,
  a null terminator is written to sOutputUTF32[return value].

  If sOutputUTF32_count == 0, the return value is the minimum number of
  ON__UINT32 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF32ToUTF32(
    int bTestByteOrder,
    const ON__UINT32* sInputUTF32,
    int sInputUTF32_count,
    ON__UINT32* sOuputUTF32,
    int sOutputUTF32_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT32** sNextInputUTF32
    );

/*
Description:
  Convert a wchar_t string using the native platform's most common
  encoding into a unicode string encoded as a UTF-8 char array.

  If 1 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-8 encoded string.

  If 2 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-16 encoded string. This is the case with current versions
  of Microsoft Windows.

  If 4 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-32 encoded string. This is the case with current versions
  of Apple OSX.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sWideChar - [in]
    wchar_t input string to convert.  
    
  sWideChar_count - [in]
    If sWideChar_count >= 0, then it specifies the number of
    wchar_t elements in sWideChar[] to convert.

    If sWideChar_count == -1, then sWideChar must be a null terminated
    array and all the elements up to the first null element are
    converted.
    
  sUTF8 - [out]
    If sUTF8 is not null and sUTF8_count > 0, then the UTF-8
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF8_count - [in]
    If sUTF8_count > 0, then it specifies the number of available
    char elements in the sUTF8[] buffer.
    
    If sUTF8_count == 0, then the sUTF8 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextWideChar - [out]
    If sNextWideChar is not null, then *sNextWideChar points to the first
    element in the input sWideChar[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextWideChar points to
    the element of sWideChar[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextWideChar points to
    sWideChar + sWideChar_count.

  If sUTF8_count > 0, the return value is the number of char
  elements written to sUTF8[].  When the return value < sUTF8_count,
  a null terminator is written to sUTF8[return value].

  If sUTF8_count == 0, the return value is the minimum number of
  char elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertWideCharToUTF8(
    int bTestByteOrder,
    const wchar_t* sWideChar,
    int sWideChar_count,
    char* sUTF8,
    int sUTF8_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const wchar_t** sNextWideChar
    );

/*
Description:
  Convert a wchar_t string using the native platform's most common
  encoding into a unicode string encoded as a UTF-16 ON__UINT16 array.

  If 1 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-8 encoded string.

  If 2 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-16 encoded string. This is the case with current versions
  of Microsoft Windows.

  If 4 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-32 encoded string. This is the case with current versions
  of Apple OS X.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sWideChar - [in]
    wchar_t input string to convert.  
    
  sWideChar_count - [in]
    If sWideChar_count >= 0, then it specifies the number of
    wchar_t elements in sWideChar[] to convert.

    If sWideChar_count == -1, then sWideChar must be a null terminated
    array and all the elements up to the first null element are
    converted.
    
  sUTF16 - [out]
    If sUTF16 is not null and sUTF16_count > 0, then the UTF-16
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF16_count - [in]
    If sUTF16_count > 0, then it specifies the number of available
    ON__UINT16 elements in the sUTF16[] buffer.
    
    If sUTF16_count == 0, then the sUTF16 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextWideChar - [out]
    If sNextWideChar is not null, then *sNextWideChar points to the first
    element in the input sWideChar[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextWideChar points to
    the element of sWideChar[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextWideChar points to
    sWideChar + sWideChar_count.

  If sUTF16_count > 0, the return value is the number of ON__UINT16
  elements written to sUTF16[].  When the return value < sUTF16_count,
  a null terminator is written to sUTF16[return value].

  If sUTF16_count == 0, the return value is the minimum number of
  ON__UINT16 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertWideCharToUTF16(
    int bTestByteOrder,
    const wchar_t* sWideChar,
    int sWideChar_count,
    ON__UINT16* sUTF16,
    int sUTF16_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const wchar_t** sNextWideChar
    );


/*
Description:
  Convert a wchar_t string using the native platform's most common
  encoding into a unicode string encoded as a UTF-32 char array.

  If 1 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-8 encoded string.

  If 2 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-16 encoded string. This is the case with current versions
  of Microsoft Windows.

  If 4 = sizeof(wchar_t), then the wchar_t array is assumed to be
  a UTF-32 encoded string. This is the case with current versions
  of Apple OSX.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sWideChar - [in]
    wchar_t string to convert.  

  sWideChar_count - [in]
    If sWideChar_count >= 0, then it specifies the number of
    wchar_t elements in sWideChar[] to convert.

    If sWideChar_count == -1, then sWideChar must be a null terminated
    string and all the elements up to the first null element are
    converted.
    
  sUTF32 - [out]
    If sUTF32 is not null and sUTF32_count > 0, then the UTF-32
    encoded string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sUTF32_count - [in]
    If sUTF32_count > 0, then it specifies the number of available
    ON__UINT32 elements in the sUTF32[] buffer.
    
    If sUTF32_count == 0, then the sUTF32 parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextWideChar - [out]
    If sNextWideChar is not null, then *sNextWideChar points to the first
    element in the input sWideChar[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextWideChar points to
    the element of sWideChar[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextWideChar points to
    sWideChar + sWideChar_count.

  If sUTF32_count > 0, the return value is the number of ON__UINT32
  elements written to sUTF32[].  When the return value < sUTF32_count,
  a null terminator is written to sUTF32[return value].

  If sUTF32_count == 0, the return value is the minimum number of
  ON__UINT32 elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertWideCharToUTF32(
    int bTestByteOrder,
    const wchar_t* sWideChar,
    int sWideChar_count,
    ON__UINT32* sUTF32,
    int sUTF32_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const wchar_t** sNextWideChar
    );


/*
Description:
  Convert a UTF-8 encoded char string to wchar_t string using
  the native platform's most common encoding.

  If 1 = sizeof(wchar_t), then UTF-8 encoding is used for the
  output string.

  If 2 = sizeof(wchar_t), then UTF-16 encoding is used for the
  output string. This is the case with current versions of
  Microsoft Windows.

  If 4 = sizeof(wchar_t), then UTF-32 encoding is used for the
  output string. This is the case with current versions of
  Apple OSX.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF8 - [in]
    UTF-8 string to convert.

  sUTF8_count - [in]
    If sUTF8_count >= 0, then it specifies the number of
    char elements in sUTF8[] to convert.

    If sUTF8_count == -1, then sUTF8 must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sWideChar - [out]
    If sWideChar is not null and sWideChar_count > 0, then the
    output string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sWideChar_count - [in]
    If sWideChar_count > 0, then it specifies the number of available
    wchar_t elements in the sWideChar[] buffer.
    
    If sWideChar_count == 0, then the sWideChar parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF8 - [out]
    If sNextUTF8 is not null, then *sNextUTF8 points to the first
    element in the input sUTF8[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF8 points to
    the element of sUTF8[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF8 points to
    sUTF8 + sUTF8_count.

Returns:
  If sWideChar_count > 0, the return value is the number of wchar_t
  elements written to sWideChar[].  When the return value < sWideChar_count,
  a null terminator is written to sWideChar[return value].

  If sWideChar_count == 0, the return value is the minimum number of
  wchar_t elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF8ToWideChar(
    int bTestByteOrder,
    const char* sUTF8,
    int sUTF8_count,
    wchar_t* sWideChar,
    int sWideChar_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const char** sNextUTF8
    );

/*
Description:
  Convert a UTF-16 encoded string to wchar_t string using
  the native platform's most common encoding.

  If 1 = sizeof(wchar_t), then UTF-8 encoding is used for the
  output string.

  If 2 = sizeof(wchar_t), then UTF-16 encoding is used for the
  output string. This is the case with current versions of
  Microsoft Windows.

  If 4 = sizeof(wchar_t), then UTF-32 encoding is used for the
  output string. This is the case with current versions of
  Apple OSX.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF16 - [in]
    UTF-16 string to convert.

  sUTF16_count - [in]
    If sUTF16_count >= 0, then it specifies the number of
    ON__UINT16 elements in sUTF16[] to convert.

    If sUTF16_count == -1, then sUTF16 must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sWideChar - [out]
    If sWideChar is not null and sWideChar_count > 0, then the
    output string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sWideChar_count - [in]
    If sWideChar_count > 0, then it specifies the number of available
    wchar_t elements in the sWideChar[] buffer.
    
    If sWideChar_count == 0, then the sWideChar parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF16 - [out]
    If sNextUTF16 is not null, then *sNextUTF16 points to the first
    element in the input sUTF16[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF8 points to
    the element of sUTF16[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF16 points to
    sUTF16 + sUTF16_count.

Returns:
  If sWideChar_count > 0, the return value is the number of wchar_t
  elements written to sWideChar[].  When the return value < sWideChar_count,
  a null terminator is written to sWideChar[return value].

  If sWideChar_count == 0, the return value is the minimum number of
  wchar_t elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertUTF16ToWideChar(
    int bTestByteOrder,
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    wchar_t* sWideChar,
    int sWideChar_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT16** sNextUTF16
    );

/*
Description:
  Convert a UTF-32 encoded string to wchar_t string using
  the native platform's most common encoding.

  If 1 = sizeof(wchar_t), then UTF-8 encoding is used for the
  output string.

  If 2 = sizeof(wchar_t), then UTF-16 encoding is used for the
  output string. This is the case with current versions of
  Microsoft Windows.

  If 4 = sizeof(wchar_t), then UTF-32 encoding is used for the
  output string. This is the case with current versions of
  Apple OSX.

Parameters:
  bTestByteOrder - [in]
    If bTestByteOrder is true and the the input buffer is a 
    byte order mark (BOM), then the BOM is skipped.  It the value
    of the BOM is byte swapped, then subsequent input elements are
    byte swapped before being decoded. Specifically:
    - If the size of an input buffer element is 1 byte and the
      values of the first three input elements are a UTF-8 BOM 
      (0xEF, 0xBB, 0xBF), then the first three input elements are
      ignored and decoding begins at the forth input element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a UTF-16 BOM (0xFEFF), then the first
      element is ignored and decoding begins with the second element.
    - If the size of an input buffer element is 2 bytes and the value
      of the first element is a byte swapped UTF-16 BOM (0xFFFE), 
      then the first element is ignored, decoding begins with the
      second element, and input element bytes are swapped before 
      being decoded.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is a UTF-32 BOM (0x0000FEFF), then the 
      first element is ignored and decoding begins with the second
      element.
    - If the size of an input buffer element is 4 bytes and the value
      of the first element is bytes swapped UTF-32 BOM (0xFFFE0000),
      then the first element is ignored, decoding begins with the 
      second element, and input element bytes are swapped before 
      being decoded.
    - In all other cases the first element of the input buffer is
      decoded and no byte swapping is performed.

  sUTF32 - [in]
    UTF-32 string to convert.

  sUTF32_count - [in]
    If sUTF32_count >= 0, then it specifies the number of
    ON__UINT32 elements in sUTF32[] to convert.

    If sUTF32_count == -1, then sUTF32 must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sWideChar - [out]
    If sWideChar is not null and sWideChar_count > 0, then the
    output string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sWideChar_count - [in]
    If sWideChar_count > 0, then it specifies the number of available
    wchar_t elements in the sWideChar[] buffer.
    
    If sWideChar_count == 0, then the sWideChar parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
       4: When parsing a UTF-8 or UTF-32 string, the values of two
          consecutive encoding sequences formed a valid UTF-16 
          surrogate pair.           
          This error is masked if 0 != (4 & m_error_mask).
          If the error is masked, then the surrogate pair is
          decoded, the value of the resulting unicode code point
          is used, and parsing continues.
       8: An overlong UTF-8 encoding sequence was encountered and 
          the value of the overlong sUTF-8 equence was a valid
          unicode code point.           
          This error is masked if 0 != (8 & m_error_mask).
          If the error is masked, then the unicode code point
          is used and parsing continues.
      16: An illegal UTF-8, UTF-16 or UTF-32 encoding sequence occured
          or an invalid unicode code point value resulted from decoding
          a UTF-8 sequence. 
          This error is masked if 0 != (16 & m_error_mask).
          If the error is masked and the value of error_code_point is
          a valid unicode code point, then error_code_point is encoded
          in the output string and parsing continues.

  error_mask - [in]
    If 0 != (error_mask & 4), then type 4 errors are masked.
    If 0 != (error_mask & 8), then type 8 errors are masked.
    If 0 != (error_mask & 16) and error_code_point is a valid unicode
    code point value, then type 16 errors are masked.

  error_code_point - [in]
    Unicode code point value to use in when masking type 16 errors.
    If 0 == (error_mask & 16), then this parameter is ignored.
    ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) 
    is a popular choice for the error_code_point value.

  sNextUTF32 - [out]
    If sNextUTF32 is not null, then *sNextUTF32 points to the first
    element in the input sUTF32[] buffer that was not converted. 

    If an error occurs and is not masked, then *sNextUTF8 points to
    the element of sUTF32[] where the conversion failed.  If no errors
    occur or all errors are masked, then *sNextUTF32 points to
    sUTF32 + sUTF32_count.

Returns:
  If sWideChar_count > 0, the return value is the number of wchar_t
  elements written to sWideChar[].  When the return value < sWideChar_count,
  a null terminator is written to sWideChar[return value].

  If sWideChar_count == 0, the return value is the minimum number of
  wchar_t elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
See Also:
  ON_wString::FromUnicodeCodePoints()
*/
ON_DECL
int ON_ConvertUTF32ToWideChar(
    int bTestByteOrder,
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    wchar_t* sWideChar,
    int sWideChar_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT32** sNextUTF32
    );

/*
Description:
  Convert a string from a Microsoft multibyte code page encoding 
  to a wide string using the native platform's wchar_t encoding. 
  This function is designed to be used to parse portions of 
  rich text RTF in ON_TextContent and user interface code. 
  Opennnurbs assumes all other char strings are UTF-8 encoded.  

  If 1 = sizeof(wchar_t), then UTF-8 encoding is used for the
  output string.

  If 2 = sizeof(wchar_t), then UTF-16 encoding is used for the
  output string. This is the case with current versions of
  Microsoft Windows.

  If 4 = sizeof(wchar_t), then UTF-32 encoding is used for the
  output string. This is the case with current versions of
  Apple OSX.

Parameters:
  windows_code_page - [in]
    THe windows code page specifices the encoding of the sMBCS string.

  sMBCS - [in]
    Windows multibyte string with encoding identified by windows_code_page.

  sMBCS_count - [in]
    If sMBCS_count >= 0, then it specifies the number of
    char elements in sMBCS[] to convert.

    If sMBCS_count == -1, then sMBCS must be a null terminated
    string and all the elements up to the first null element are
    converted.

  sWideChar - [out]
    If sWideChar is not null and sWideChar_count > 0, then the
    output string is returned in this buffer. If there is room
    for the null terminator, the converted string will be null
    terminated. The null terminator is never included in the count 
    of returned by this function. The converted string is in the 
    CPU's native byte order. No byte order mark is prepended.

  sWideChar_capacity - [in]
    If sWideChar_capacity > 0, then it specifies the number of available
    wchar_t elements in the sWideChar[] buffer.
    
    If sWideChar_count == 0, then the sWideChar parameter is ignored.

  error_status - [out]
    If error_status is not null, then bits of *error_status are
    set to indicate the success or failure of the conversion.  
    When the error_mask parameter is used to used to mask some
    conversion errors, multiple bits may be set.
       0: Successful conversion with no errors.
       1: The input parameters were invalid. 
          This error cannot be masked.
       2: The ouput buffer was not large enough to hold the converted
          string. As much conversion as possible is performed in this
          case and the error cannot be masked.
      16: An illegal encoding sequence occurred.
          The illegal sequence is replaced with
          a single ON_wString::ReplacementCharacter in the output string
          and parsing continues.

Returns:
  If sWideChar_capacity > 0, the return value is the number of wchar_t
  elements written to sWideChar[].  When the return value < sWideChar_count,
  a null terminator is written to sWideChar[return value].

  If sWideChar_count == 0, the return value is the minimum number of
  wchar_t elements that are needed to hold the converted string.
  The return value does not include room for a null terminator.  
  Increment the return value by one if you want to have an element
  to use for a null terminator.
*/
ON_DECL
int ON_ConvertMSMBCPToWideChar(
    ON__UINT32 windows_code_page,
    const char* sMBCS,
    int sMBCS_count,
    wchar_t* sWideChar,
    int sWideChar_capacity,
    unsigned int* error_status
    );


ON_END_EXTERNC

#if defined(ON_CPLUSPLUS)
ON_DECL
ON__UINT32 ON_Test_MSSBCP(
  const ON__UINT32 code_page,
  const ON__UINT32 char_encoding,
  bool bWindowsAPITest,
  ON_TextLog& text_log
);

ON_DECL
bool ON_Test_MSSBCP(
  const ON__UINT32 code_page,
  bool bWindowsAPITest,
  ON_TextLog& text_log
);

ON_DECL
bool ON_Test_MSSBCP(
  bool bWindowsAPITest,
  ON_TextLog& text_log
);

ON_DECL
bool ON_Test_PrintPlatformMSSBCPToUnicodeTable(
  const ON__UINT32 code_page,
  ON__UINT32 char_encoding0,
  ON__UINT32 char_encoding1,
  ON_TextLog& text_log
);

#endif
#endif
