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

#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

int ON_IsValidUnicodeCodePoint(ON__UINT32 u)
{
  return (u < 0xD800 || (u >= 0xE000 && u <= 0x10FFFF));
}

int ON_IsValidUTF32Value(
  ON__UINT32 c
  )
{
  return (c < 0xD800 || (c >= 0xE000 && c <= 0x10FFFF));
}

int ON_IsValidSingleElementUTF16Value(ON__UINT32 c)
{
  return ((c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF));
}

int ON_IsValidUTF16Singleton(ON__UINT32 c)
{
  return ((c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF));
}

enum ON_UnicodeEncoding ON_UnicodeNativeCPU_UTF16()
{
  return (ON::endian::little_endian== ON::Endian()) ? ON_UTF_16LE : ON_UTF_16BE;
}

enum ON_UnicodeEncoding ON_UnicodeNativeCPU_UTF32()
{
  return (ON::endian::little_endian== ON::Endian()) ? ON_UTF_32LE : ON_UTF_32BE;
}

int ON_IsValidSingleByteUTF8CharValue(
  char c
  )
{
  return (c >= 0 && c <= 0x7F);
}

int ON_IsValidUTF8SingletonChar(
  char c
  )
{
  return (c >= 0 && c <= 0x7F);
}

int ON_IsValidSingleElementUTF8Value(
  ON__UINT32 c
  )
{
  return (c <= 0x7F);
}

int ON_IsValidUTF8Singleton(
  ON__UINT32 c
  )
{
  return (c <= 0x7FU);
}

int ON_IsValidUTF16SurrogatePair(
  unsigned int w1,
  unsigned int w2
  )
{
  return ( w1 >= 0xD800U && w1 < 0xDC00 && w2 >= 0xDC00 && w2 < 0xE000 );
}


int ON_IsValidSingleElementWideCharValue(
  wchar_t w
  )
{
#pragma ON_PRAGMA_WARNING_PUSH
// warning C4127: conditional expression is constant
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4127 )
  if (1 == sizeof(w))
    return ON_IsValidSingleElementUTF8Value((ON__UINT32)w);
  if (2 == sizeof(w))
    return ON_IsValidSingleElementUTF16Value((ON__UINT32)w);
  return ON_IsValidUTF32Value((ON__UINT32)w);
#pragma ON_PRAGMA_WARNING_POP
}

enum ON_UnicodeEncoding ON_IsUTFByteOrderMark(
  const void* buffer,
  size_t sizeof_buffer
  )
{  
  if ( 0 != buffer && sizeof_buffer >= 2 )
  {
    const unsigned char* b = static_cast<const unsigned char*>(buffer);

    if ( 0 == b[0] )
    {
      if ( sizeof_buffer >= 4 && 0 == b[1] && 0xFE == b[2] && 0xFF == b[3] )
        return ON_UTF_32BE;
    }
    else if ( 0xEF == b[0] )
    {
      if ( sizeof_buffer >= 3 && 0xBB == b[1] && 0xBF == b[2] )
        return ON_UTF_8;
    }
    else if ( 0xFE == b[0] )
    {
      if ( 0xFF == b[1] )
        return ON_UTF_16BE;
    }
    else if ( 0xFF == b[0] && 0xFE == b[1] )
    {
      return ( sizeof_buffer >= 4 && 0 == b[2] && 0 == b[3] )
        ? ON_UTF_32LE
        : ON_UTF_16LE;
    }

  }

  return ON_UTF_unset;
}

unsigned int ON_UTFSizeofByteOrderMark(
  ON_UnicodeEncoding e
  )
{
  unsigned int sizeof_bom;
  switch (e)
  {
  case ON_UTF_8:
    sizeof_bom = 3;
    break;

  case ON_UTF_16:
  case ON_UTF_16BE:
  case ON_UTF_16LE:
    sizeof_bom = 2;
    break;

  case ON_UTF_32:
  case ON_UTF_32BE:
  case ON_UTF_32LE:
    sizeof_bom = 4;
    break;

  default:
    sizeof_bom = 0;
    break;
  }

  return sizeof_bom;
}

static int ON_IsUTF8ByteOrderMark(
  const char* sUTF8,
  int sUTF8_count
  )
{
  if ( 0 == sUTF8 )
    return 0;
  if ( -1 != sUTF8_count || sUTF8_count < 3 )
    return 0;
  return (0xEF == (unsigned char)(sUTF8[0]) && 0xBB == (unsigned char)(sUTF8[1]) && 0xBF == (unsigned char)(sUTF8[2]));
}

int ON_EncodeUTF8( ON__UINT32 u, char sUTF8[6] )
{
  ON__UINT32 c;

  if ( u <= 0x7F )
  {
    // 1 byte UTF8 encoding: 0xxxxxxx (7 bits of u)
    sUTF8[0] = (char)u;
    return 1;
  }

  if ( u <= 0x7FF )
  {
    // 2 byte UTF8 encoding: 110xxxxx, 10xxxxxx (11 bits of u)
    c = (u / 0x40);  // c  = 000xxxxx
    c |= 0xC0;                      //   |= 11000000
    sUTF8[0] = (char)c;
    c = (u & 0x3F);
    c |= 0x80;
    sUTF8[1] = (char)c;
    return 2;
  }

  if ( u <= 0xFFFF )
  {
    // 3 byte UTF8 encoding: 1110xxxx, 10xxxxxx, 10xxxxxx (16 bits of u)
    c = (u / 0x1000); // c  = 0000xxxx
    c |= 0xE0;                       //   |= 11100000
    sUTF8[0] = (char)c;
    c = ((u & 0xFFF) / 0x40);
    c |= 0x80;
    sUTF8[1] = (char)c;
    c = u & 0x3F;
    c |= 0x80;
    sUTF8[2] = (char)c;
    return 3;
  }

  if ( u <= 0x1FFFFF )
  {
    // (maximum valid unicode codepoint is 0x10FFFF)
    // 4 byte UTF8 encoding: 11110xxx, 10xxxxxx, 10xxxxxx, 10xxxxxx (21 bits of u)
    // Note: 0x10FFFF is the maximum valid unicode code point.
    // For u > 0x10FFFF and u <= 0x1FFFFF, this calculation encodes the low 21 bits of u.
    c = (u / 0x40000);  // c  = 00000xxx
    c |= 0xF0;                         //   |= 11110000
    sUTF8[0] = (char)c;
    c = ((u & 0x3FFFF)/0x1000);
    c |= 0x80;
    sUTF8[1] = (char)c;
    c = ((u & 0xFFF) / 0x40);
    c |= 0x80;
    sUTF8[2] = (char)c;
    c = u & 0x3F;
    c |= 0x80;
    sUTF8[3] = (char)c;
    return 4;
  }

  if ( u <= 0x3FFFFFF )
  {
    // 5 byte encoding: 111110xx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx (26 bits of u)
    // Note: 0x10FFFF is the maximum valid unicode code point.
    c = (u / 0x1000000); // c  = 000000xx
    c |= 0xF8;                         //   |= 11111000
    sUTF8[0] = (char)c;
    c = ((u & 0xFFFFFF)/0x40000);
    c |= 0x80;
    sUTF8[1] = (char)c;
    c = ((u & 0x3FFFF)/0x1000);
    c |= 0x80;
    sUTF8[2] = (char)c;
    c = ((u & 0xFFF) / 0x40);
    c |= 0x80;
    sUTF8[3] = (char)c;
    c = u & 0x3F;
    c |= 0x80;
    sUTF8[4] = (char)c;
    return 5;
  }

  if ( u <= 0x7FFFFFFF )
  {
    // 6 byte encoding: 1111110x, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx (31 bits of u)
    // Note: 0x10FFFF is the maximum valid unicode code point.
    c = (u / 0x40000000); // c  = 00000000x
    c |= 0xFC;                           //   |= 11111100
    sUTF8[0] = (char)c;
    c = ((u & 0x3FFFFFFF)/0x1000000);
    c |= 0x80;
    sUTF8[1] = (char)c;
    c = ((u & 0xFFFFFF)/0x40000);
    c |= 0x80;
    sUTF8[2] = (char)c;
    c = ((u & 0x3FFFF)/0x1000);
    c |= 0x80;
    sUTF8[3] = (char)c;
    c = ((u & 0xFFF) / 0x40);
    c |= 0x80;
    sUTF8[4] = (char)c;
    c = u & 0x3F;
    c |= 0x80;
    sUTF8[5] = (char)c;
    return 6;
  }

  return 0;
}

static int ON_DecodeUTF8Helper(
    const char* sUTF8,
    int sUTF8_count,
    ON__UINT32* value,
    unsigned int* error_status
    )
{
#define INPUT_BUFFER_TOO_SHORT 16
#define INVALID_CONTINUATION_VALUE 16
#define OVERLONG_ENCODING 8

  ON__UINT32 u;
  char c;

  c = sUTF8[0];
   
  if ( 0 == (0x80 & c) )
  {
    // 1 byte ASCII encoding: 0xxxxxxx
    *value = c;
    return 1;
  }

  if ( 0xC0 == ( 0xE0 & c) )
  {
    // 2 byte character encoding: 10xxxxxx, 10xxxxxx
    if ( sUTF8_count < 2 )
    {
      *error_status |= INPUT_BUFFER_TOO_SHORT; // input buffer too short
      return 0;
    }
    u = (0x1F & c);
    c = sUTF8[1];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    if ( u <= 0x7F )
    {
      *error_status |= OVERLONG_ENCODING; // overlong 2 byte character encoding
    }
    *value = u;
    return 2;
  }

  if ( 0xE0 == ( 0xF0 & c) )
  {
    // 3 byte character encoding: 110xxxxx, 10xxxxxx, 10xxxxxx
    if ( sUTF8_count < 3 )
    {
      *error_status |= INPUT_BUFFER_TOO_SHORT; // input buffer too short
      return 0;
    }
    u = (0x0F & c);
    c = sUTF8[1];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[2];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    if ( u <= 0x7FF )
    {
      *error_status |= OVERLONG_ENCODING; // overlong 3 byte character encoding
    }
    *value = u;
    return 3;
  }

  if ( 0xF0 == ( 0xF8 & c) )
  {
    // 4 byte character encoding: 11110xxx, 10xxxxxx, 10xxxxxx, 10xxxxxx
    if ( sUTF8_count < 4 )
    {
      *error_status |= INPUT_BUFFER_TOO_SHORT; // input buffer too short
      return 0;
    }

    u = (0x07 & c);
    c = sUTF8[1];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[2];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[3];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    if ( u <= 0xFFFF )
    {
      *error_status |= OVERLONG_ENCODING; // overlong 4 byte character encoding
    }
    *value = u;
    return 4;
  }
  
  if ( 0xF8 == ( 0xFC & c) )
  {
    // 5 byte character encoding: 111110xx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx
    if ( sUTF8_count < 5 )
    {
      *error_status |= INPUT_BUFFER_TOO_SHORT; // input buffer too short
      return 0;
    }

    u = (0x03 & c);
    c = sUTF8[1];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[2];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[3];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[4];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    if ( u <= 0x1FFFFF )
    {
      *error_status |= OVERLONG_ENCODING; // overlong 5 byte character encoding
    }
    *value = u;
    return 5;
  }

  if ( 0xFC == ( 0xFE & c) )
  {
    // 6 byte character encoding: 110xxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx
    if ( sUTF8_count < 6 )
    {
      *error_status |= INPUT_BUFFER_TOO_SHORT; // input buffer too short
      return 0;
    }

    u = (0x01 & c);
    c = sUTF8[1];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[2];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[3];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[4];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    c = sUTF8[5];
    if (  0x80 != ( 0xC0 & c) )
    {
      *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 continuation value
      return 0;
    }
    u *= 64;
    u |= (0x3F & c);
    if ( u <= 0x3FFFFFF )
    {
      *error_status |= OVERLONG_ENCODING; // overlong 6 byte character encoding
    }
    *value = u;
    return 6;
  }

  *error_status |= INVALID_CONTINUATION_VALUE; // invalid UTF=8 start value
  return 0;

#undef INPUT_BUFFER_TOO_SHORT
#undef INVALID_CONTINUATION_VALUE
#undef OVERLONG_ENCODING
}

int ON_DecodeUTF8(
    const char* sUTF8,
    int sUTF8_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    )
{
  ON__UINT32 u0, u1;
  int i0, i1;
  unsigned int error_status;
  ON__UINT16 sUTF16[2];
  char c;

  ON_UnicodeErrorParameters local_e = ON_UnicodeErrorParameters::MaskErrors;
  if (nullptr == e)
    e = &local_e;

  if (  0 == sUTF8 || sUTF8_count <= 0 || 0 == unicode_code_point )
  {
    if ( e )
      e->m_error_status |= 1;
    return 0;
  }

  // special cases for most common unicode values
  // If any error conditions exist, then ON_DecodeUTF8Helper()
  // is used.
  if ( 0 == (0x80 & sUTF8[0]) )
  {
    *unicode_code_point = sUTF8[0];
    return 1;
  }
  
  c = sUTF8[0];
  if ( 0xC0 == ( 0xE0 & c) && sUTF8_count >= 2 )
  {
    // 2 byte character encoding: 10xxxxxx, 10xxxxxx
    u0 = (0x1F & c);
    c = sUTF8[1];
    if (  0x80 == ( 0xC0 & c) )
    {
      u0 *= 64;
      u0 |= (0x3F & c);
      if ( u0 > 0x7F )
      {
        *unicode_code_point = u0;
        return 2;
      }
    }
  }
  else if ( 0xE0 == ( 0xF0 & c) && sUTF8_count >= 3 )
  {
    // 3 byte character encoding: 110xxxxx, 10xxxxxx, 10xxxxxx
    u0 = (0x0F & c);
    c = sUTF8[1];
    if (  0x80 == ( 0xC0 & c) )
    {
      u0 *= 64;
      u0 |= (0x3F & c);
      c = sUTF8[2];
      if (  0x80 == ( 0xC0 & c) )
      {
        u0 *= 64;
        u0 |= (0x3F & c);
        if ( u0 >= 0x0800 && (u0 <= 0xD800 || u0 >= 0xE000) )
        {
          *unicode_code_point = u0;
          return 3;
        }
      }
    }
  }
  else if ( 0xF0 == ( 0xF8 & c) && sUTF8_count >= 4 )
  {
    // 4 byte character encoding: 11110xxx, 10xxxxxx, 10xxxxxx, 10xxxxxx
    u0 = (0x07 & c);
    c = sUTF8[1];
    if (  0x80 == ( 0xC0 & c) )
    {
      u0 *= 64;
      u0 |= (0x3F & c);
      c = sUTF8[2];
      if (  0x80 == ( 0xC0 & c) )
      {
        u0 *= 64;
        u0 |= (0x3F & c);
        c = sUTF8[3];
        if (  0x80 == ( 0xC0 & c) )
        {
          u0 *= 64;
          u0 |= (0x3F & c);
          if ( u0 >= 0x010000 && u0 <= 0x10FFFF )
          {
            *unicode_code_point = u0;
            return 4;
          }
        }
      }
    }
  }


  error_status = 0;
  u0 = 0xFFFFFFFF;
  i0 = ON_DecodeUTF8Helper(sUTF8,sUTF8_count,&u0,&error_status);
  if ( i0 > 0 && 0 == error_status && (u0 < 0xD800 || (u0 >= 0xE000 && u0 <= 0x10FFFF) ) )
  {
    // valid UTF-8 multibyte encoding parsed
    *unicode_code_point = u0;
    return i0;
  }

  // handle errors
  if ( 0 == e )
  {
    // no errors are masked.
    return 0;
  }

  // report error condition
  e->m_error_status |= error_status;

  if ( error_status != (error_status & e->m_error_mask) )
  {
    // this error is not masked
    return 0;
  }

  if ( i0 <= 0 )
  {
    i0 = 1;
    if ( ON_IsValidUnicodeCodePoint(e->m_error_code_point) )
    {
      // skip to next UTF-8 start elemement
      for ( /*empty for initializer*/; i0 < sUTF8_count; i0++ )
      {
        // Search for the next element of sUTF8[] that is the 
        // start of a UTF-8 encoding sequence.
        c = sUTF8[i0];
        if (    0 == (0x80 & c)     // ASCII 0 - 127
              || 0xC0 == ( 0xE0 & c) // 2 byte encoding first character
              || 0xE0 == ( 0xF0 & c) // 3 byte encoding first character
              || 0xF0 == ( 0xF8 & c) // 4 byte encoding first character
              || 0xF8 == ( 0xFC & c) // 5 byte encoding first character
              || 0xFC == ( 0xFE & c) // 6 byte encoding first character
            )
        {
          // resume parsing at this character
          break;
        }
      }
      *unicode_code_point = e->m_error_code_point;
    }
    return i0;
  }

  if ( ON_IsValidUnicodeCodePoint(u0) && 8 == error_status )
  {
    // overlong UTF-8 multibyte encoding of valid unicode code point
    *unicode_code_point = u0;
    return i0;
  }
  
  if ( i0 < sUTF8_count 
       && u0 >= 0xD800 && u0 <= 0xDBFF 
       && (0 == error_status || 8 == error_status)
       && 0 != (4 & e->m_error_mask) 
     )
  {
    // See if a UFT-16 surrogate pair was incorrectly encoded 
    // as two consecutive UTF-8 sequences.
    u1 = 0xFFFFFFFF;
    i1 = ON_DecodeUTF8Helper(sUTF8+i0,sUTF8_count-i0,&u1,&error_status);
    if ( i1 > 0 && (0 == error_status || 8 == error_status) )
    {
      error_status = 0;
      sUTF16[0] = (ON__UINT16)u0;
      sUTF16[1] = (ON__UINT16)u1;
      u0 = 0xFFFFFFFF;
      if ( 2 == ON_ConvertUTF16ToUTF32(false,sUTF16,2,&u0,1,&error_status,0,0,0) 
           && 0 == error_status 
           && ON_IsValidUnicodeCodePoint(u0)
         )
      {
        *unicode_code_point = u0;
        e->m_error_status |= 4;
        return i0+i1;
      }
    }
  }

  if ( ON_IsValidUnicodeCodePoint(e->m_error_code_point) )
  {
    *unicode_code_point = e->m_error_code_point;
    return i0;
  }

  return 0;
}

int ON_EncodeUTF16( ON__UINT32 unicode_code_point, ON__UINT16 sUTF16[2] )
{
  // put the most comman case first
  if ( unicode_code_point < 0xD800 )
  {
    // code point values U+0000 ... U+D7FF
    // = UTF-16 values
    sUTF16[0] = (ON__UINT16)unicode_code_point;
    return 1;
  }

  if ( unicode_code_point < 0xE000 )
  {
    // 0xD800 ... 0xDFFF are invalid unicode code point values
    return 0;
  }

  if ( unicode_code_point <= 0xFFFF )
  {
    // code point values U+E000 ... U+FFFF
    // = UTF-16 values
    sUTF16[0] = (ON__UINT16)unicode_code_point;
    return 1;
  }

  if ( unicode_code_point <= 0x10FFFF )
  {
    // code point values U+10000 ... U+10FFFF
    // = surrogate pair UTF-16 values
    unicode_code_point -= 0x10000;
    sUTF16[0] = (ON__UINT16)(0xD800 + (unicode_code_point / 0x400)); // high surrogate value (0xD800 ... 0xDBFF)
    sUTF16[1] = (ON__UINT16)(0xDC00 + (unicode_code_point & 0x3FF)); // low surrogate value (0xDC00 ... 0xDFFF)
    return 2;
  }

  // 0x110000 ... 0xFFFFFFFF are invalid unicode code point values
  return 0;
}

int ON_DecodeUTF16(
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    )
{
  ON__UINT32 uhi, ulo;

  ON_UnicodeErrorParameters local_e = ON_UnicodeErrorParameters::MaskErrors;
  if (nullptr == e)
    e = &local_e;

  if ( 0 == sUTF16 || sUTF16_count <= 0 || 0 == unicode_code_point )
  {
    if ( e )
      e->m_error_status |= 1;
    return 0;
  }

  // special case for most common UTF-16 single element values
  if ( ( sUTF16[0] < 0xD800 ) || ( sUTF16[0] >= 0xE000 ) )
  {
    *unicode_code_point = sUTF16[0];
    return 1;
  }

  if ( sUTF16_count >= 2 && sUTF16[0] < 0xDC00 && sUTF16[1] >=  0xDC00 && sUTF16[1] < 0xE000 )
  {
    // UTF-16 surrogate pair
    uhi = sUTF16[0];
    ulo = sUTF16[1];
    *unicode_code_point = (uhi-0xD800)*0x400 + (ulo-0xDC00) + 0x10000;
    return 2;
  }

  // handle errors
  if ( 0 == e )
  {
    // no errors are masked.
    return 0;
  }

  // report error condition
  e->m_error_status |= 16;

  if ( 16 != (16 & e->m_error_mask) || !ON_IsValidUnicodeCodePoint(e->m_error_code_point) )
  {
    // this error is not masked
    return 0;
  }

  // Search for the next element of sUTF16[] that is a 
  // valid UTF-16 encoding sequence.
  int i;
  for ( i = 1; i < sUTF16_count; i++ )
  {
    if ( ( sUTF16[i] < 0xD800 ) || ( sUTF16[i] >= 0xE000 ) )
    {
      // valid single UTF-16 code unit
      break;
    }
    if ( i+1 < sUTF16_count 
         && sUTF16[i] >= 0xD800 && sUTF16[i] < 0xDC00 
         && sUTF16[i+1] >= 0xDC00 && sUTF16[i+1] < 0xE000
       )
    {
      // valid UTF-16 surrogate pair
      break;
    }
  }

  *unicode_code_point = e->m_error_code_point;

  return i;
}

int ON_DecodeUTF16LE(
  const ON__UINT16* sUTF16,
  int sUTF16_count,
  struct ON_UnicodeErrorParameters* e,
  ON__UINT32* unicode_code_point
  )
{
#if defined(ON_LITTLE_ENDIAN)
  return ON_DecodeUTF16(sUTF16,sUTF16_count,e,unicode_code_point);
#else
  return ON_DecodeSwapByteUTF16(sUTF16,sUTF16_count,e,unicode_code_point);
#endif
}

int ON_DecodeUTF16BE(
  const ON__UINT16* sUTF16,
  int sUTF16_count,
  struct ON_UnicodeErrorParameters* e,
  ON__UINT32* unicode_code_point
  )
{
#if defined(ON_BIG_ENDIAN)
  return ON_DecodeUTF16(sUTF16,sUTF16_count,e,unicode_code_point);
#else
  return ON_DecodeSwapByteUTF16(sUTF16,sUTF16_count,e,unicode_code_point);
#endif
}

int ON_DecodeUTF32LE(
  const ON__UINT32* sUTF32,
  int sUTF32_count,
  struct ON_UnicodeErrorParameters* e,
  ON__UINT32* unicode_code_point
  )
{
#if defined(ON_LITTLE_ENDIAN)
  return ON_DecodeUTF32(sUTF32,sUTF32_count,e,unicode_code_point);
#else
  return ON_DecodeSwapByteUTF32(sUTF32,sUTF32_count,e,unicode_code_point);
#endif
}

int ON_DecodeUTF32BE(
  const ON__UINT32* sUTF32,
  int sUTF32_count,
  struct ON_UnicodeErrorParameters* e,
  ON__UINT32* unicode_code_point
  )
{
#if defined(ON_BIG_ENDIAN)
  return ON_DecodeUTF32(sUTF32,sUTF32_count,e,unicode_code_point);
#else
  return ON_DecodeSwapByteUTF32(sUTF32,sUTF32_count,e,unicode_code_point);
#endif
}


int ON_EncodeWideChar(
  ON__UINT32 code_point,
  size_t w_capacity,
  wchar_t* w
  )
{
  int rc = 0;
  if (nullptr != w && w_capacity > 0)
  {
    if (ON_IsValidUnicodeCodePoint(code_point))
    {
#if 1 == ON_SIZEOF_WCHAR_T
      char sUTF8[6];
      rc = ON_EncodeUTF8(code_point, sUTF8);
      if (rc > (int)w_capacity)
        rc = 0;
      switch (rc)
      {
      case 1:
        w[0] = (wchar_t)sUTF8[0];
        break;
      case 2:
        w[0] = (wchar_t)sUTF8[0];
        w[1] = (wchar_t)sUTF8[1];
        break;
      case 3:
        w[0] = (wchar_t)sUTF8[0];
        w[1] = (wchar_t)sUTF8[1];
        w[2] = (wchar_t)sUTF8[2];
        break;
      case 4:
        w[0] = (wchar_t)sUTF8[0];
        w[1] = (wchar_t)sUTF8[1];
        w[2] = (wchar_t)sUTF8[2];
        w[3] = (wchar_t)sUTF8[3];
        break;
      default:
        rc = 0; break;
      }
#elif 2 == ON_SIZEOF_WCHAR_T
      ON__UINT16 sUTF16[2];
      rc = ON_EncodeUTF16(code_point, sUTF16);
      if (rc > (int)w_capacity)
        rc = 0;
      switch (rc)
      {
      case 1:
        w[0] = (wchar_t)sUTF16[0];
        break;
      case 2:
        w[0] = (wchar_t)sUTF16[0];
        w[1] = (wchar_t)sUTF16[1];
        break;
      default:
        rc = 0; break;
      }
#elif 4 == ON_SIZEOF_WCHAR_T
      if (w_capacity > 0)
      {
        w[0] = (wchar_t)code_point;
        rc = 1;
      }
#endif
    }
    if (rc >= 0 && rc < (int)w_capacity)
      w[rc] = 0;
  }
  return rc;
}

int ON_DecodeWideChar(
  const wchar_t* sWideChar,
  int sWideChar_count,
  struct ON_UnicodeErrorParameters* e,
  ON__UINT32* unicode_code_point
  )
{
  const ON_UnicodeEncoding widechar_encoding = ON_WCHAR_T_ENCODING;
  int rc;

  switch (widechar_encoding)
  {
#if 1 == ON_SIZEOF_WCHAR_T
  case ON_UTF_8:
    rc = ON_DecodeUTF8((const char*)sWideChar,sWideChar_count,e,unicode_code_point);
    break;

#elif 2 == ON_SIZEOF_WCHAR_T
  case ON_UTF_16:
    return ON_DecodeUTF16((const ON__UINT16*)sWideChar,sWideChar_count,e,unicode_code_point);
    break;

  case ON_UTF_16BE:
    rc = ON_DecodeUTF16BE((const ON__UINT16*)sWideChar,sWideChar_count,e,unicode_code_point);
    break;

  case ON_UTF_16LE:
    rc = ON_DecodeUTF16LE((const ON__UINT16*)sWideChar,sWideChar_count,e,unicode_code_point);
    break;

#elif 4 == ON_SIZEOF_WCHAR_T
  case ON_UTF_32:
    rc = ON_DecodeUTF32((const ON__UINT32*)sWideChar,sWideChar_count,e,unicode_code_point);
    break;

  case ON_UTF_32BE:
    rc = ON_DecodeUTF32BE((const ON__UINT32*)sWideChar,sWideChar_count,e,unicode_code_point);
    break;

  case ON_UTF_32LE:
    rc = ON_DecodeUTF32LE((const ON__UINT32*)sWideChar,sWideChar_count,e,unicode_code_point);
    break;
#endif

  default:
    rc = 0;
    if ( e )
      e->m_error_status |= 1;
    break;
  }

  return rc;
}


int ON_DecodeSwapByteUTF16(
    const ON__UINT16* sUTF16,
    int sUTF16_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    )
{
  int i;
  ON__UINT32 uhi, ulo;
  ON__UINT16 w0, w1;
  const ON__UINT8* p;
  ON__UINT8* p0;
  ON__UINT8* p1;

  ON_UnicodeErrorParameters local_e = ON_UnicodeErrorParameters::MaskErrors;
  if (nullptr == e)
    e = &local_e;

  if ( 0 == sUTF16 || sUTF16_count <= 0 || 0 == unicode_code_point )
  {
    if ( e )
      e->m_error_status |= 1;
    return 0;
  }

  // special case for most common UTF-16 single element values
  // w0 = byte swapped sUTF16[0]
  p = (const ON__UINT8*)sUTF16;
  p0 = (ON__UINT8*)&w0;
  p0[1] = p[0];
  p0[0] = p[1];
  if ( ( w0 < 0xD800 ) || (w0 >= 0xE000 ) )
  {
    *unicode_code_point = w0;
    return 1;
  }

  if ( sUTF16_count >= 2 && w0 < 0xDC00 )
  {
    // w1 = byte swapped sUTF16[1]
    p1 = (ON__UINT8*)&w1;
    p1[1] = p[2];
    p1[0] = p[3];
    if ( w1 >=  0xDC00 && w1 < 0xE000 )
    {
      // UTF-16 surrogate pair
      uhi = w0;
      ulo = w1;
      *unicode_code_point = (uhi-0xD800)*0x400 + (ulo-0xDC00) + 0x10000;
      return 2;
    }
  }

  // handle errors
  if ( 0 == e )
  {
    // no errors are masked.
    return 0;
  }

  // report error condition
  e->m_error_status |= 16;

  if ( 16 != (16 & e->m_error_mask) || !ON_IsValidUnicodeCodePoint(e->m_error_code_point) )
  {
    // this error is not masked
    return 0;
  }

  // Search for the next element of sUTF16[] that is a 
  // valid UTF-16 encoding sequence.
  p1 = (ON__UINT8*)&w1;
  p += sizeof(sUTF16[0]);
  for ( i = 1; i < sUTF16_count; i++, p += sizeof(sUTF16[0]) )
  {
    // w0 = byte swapped sUTF16[i]
    p0[1] = p[0];
    p0[0] = p[1];
    if ( ( w0 < 0xD800 ) || ( w0 >= 0xE000 ) )
    {
      // valid single UTF-16 code unit
      break;
    }
    if ( i+1 < sUTF16_count && w0 >= 0xD800 && w0 < 0xDC00 )
    {
      // w1 = byte swapped sUTF16[i+1]
      p1[1] = p[sizeof(sUTF16[0])];
      p1[0] = p[sizeof(sUTF16[0])+1];
      if ( w1 >= 0xDC00 && w1 < 0xE000 )
      {
        // valid UTF-16 surrogate pair
        break;
      }
    }
  }

  *unicode_code_point = e->m_error_code_point;

  return i;
}

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
    )
{
  int i, j, k, output_count;
  ON__UINT32 u;
  char s[6];
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sInputUTF8_count && 0 != sInputUTF8 )
  {
    for ( sInputUTF8_count = 0; 0 != sInputUTF8[sInputUTF8_count]; sInputUTF8_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sInputUTF8 || sInputUTF8_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextInputUTF8 )
      *sNextInputUTF8 = sInputUTF8;
    return 0;
  }

  if ( 0 == sOutputUTF8_count )
  {
    sOutputUTF8 = 0;
    sOutputUTF8_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sOutputUTF8 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextInputUTF8 )
      *sNextInputUTF8 = sInputUTF8;
    return 0;
  }

  if ( bTestByteOrder && ON_IsUTF8ByteOrderMark(sInputUTF8,sInputUTF8_count) )
  {
    // skip UTF-8 byte order element
    sInputUTF8_count -= 3;
    sInputUTF8 += 3;
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  for ( i = 0; i < sInputUTF8_count; i += j )
  {
    j = ON_DecodeUTF8(sInputUTF8+i,sInputUTF8_count-i,&e,&u);
    if ( j <= 0 )
      break;
    k = ON_EncodeUTF8(u,s);
    if ( 0 != sOutputUTF8 )
    {
      if ( output_count + k > sOutputUTF8_count )
      {
        e.m_error_status |= 2;
        break;
      }
      memcpy(sOutputUTF8+output_count,s,k*sizeof(sOutputUTF8[0]));
    }
    output_count += k;
  }

  if ( 0 != sOutputUTF8 && output_count < sOutputUTF8_count)
    sOutputUTF8[output_count] = 0;
  if ( sNextInputUTF8 )
    *sNextInputUTF8 = sInputUTF8+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}


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
    )
{
  int i, j, k, output_count;
  ON__UINT32 u;
  ON__UINT16 w[2];
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sUTF8_count && 0 != sUTF8 )
  {
    for ( sUTF8_count = 0; 0 != sUTF8[sUTF8_count]; sUTF8_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sUTF8 || sUTF8_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF8 )
      *sNextUTF8 = sUTF8;
    return 0;
  }

  if ( bTestByteOrder && ON_IsUTF8ByteOrderMark(sUTF8,sUTF8_count) )
  {
    // skip UTF-8 byte order element
    sUTF8_count -= 3;
    sUTF8 += 3;
  }

  if ( 0 == sUTF16_count )
  {
    sUTF16 = 0;
    sUTF16_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sUTF16 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF8 )
      *sNextUTF8 = sUTF8;
    return 0;
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  for ( i = 0; i < sUTF8_count; i += j )
  {
    j = ON_DecodeUTF8(sUTF8+i,sUTF8_count-i,&e,&u);
    if ( j <= 0 )
      break;
    k = ON_EncodeUTF16(u,w);
    if ( 0 != sUTF16 )
    {
      if ( output_count + k > sUTF16_count )
      {
        e.m_error_status |= 2;
        break;
      }
      sUTF16[output_count] = w[0];
      if ( 2 == k )
        sUTF16[output_count+1] = w[1];
    }
    output_count += k;
  }

  if ( 0 != sUTF16 && output_count < sUTF16_count)
    sUTF16[output_count] = 0;
  if ( sNextUTF8 )
    *sNextUTF8 = sUTF8+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}

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
    )
{
  int i, j, output_count;
  ON__UINT32 u;
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sUTF8_count && 0 != sUTF8 )
  {
    for ( sUTF8_count = 0; 0 != sUTF8[sUTF8_count]; sUTF8_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sUTF8 || sUTF8_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF8 )
      *sNextUTF8 = sUTF8;
    return 0;
  }

  if ( bTestByteOrder && ON_IsUTF8ByteOrderMark(sUTF8,sUTF8_count) )
  {
    // skip UTF-8 byte order element
    sUTF8_count -= 3;
    sUTF8 += 3;
  }

  if ( 0 == sUTF32_count )
  {
    sUTF32 = 0;
    sUTF32_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sUTF32 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF8 )
      *sNextUTF8 = sUTF8;
    return 0;
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  for ( i = 0; i < sUTF8_count; i += j )
  {
    j = ON_DecodeUTF8(sUTF8+i,sUTF8_count-i,&e,&u);
    if ( j <= 0 )
      break;
    if ( 0 != sUTF32 )
    {
      if ( output_count >= sUTF32_count )
      {
        e.m_error_status |= 2;
        break;
      }
      sUTF32[output_count] = u;
    }
    output_count++;
  }

  if ( 0 != sUTF32 && output_count < sUTF32_count)
    sUTF32[output_count] = 0;
  if ( sNextUTF8 )
    *sNextUTF8 = sUTF8+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}

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
    )
{
  int i, j, k, output_count, bSwapBytes;
  ON__UINT32 u;
  char s[6];
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sUTF16_count && 0 != sUTF16 )
  {
    for ( sUTF16_count = 0; 0 != sUTF16[sUTF16_count]; sUTF16_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sUTF16 || sUTF16_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF16 )
      *sNextUTF16 = sUTF16;
    return 0;
  }

  if ( 0 == sUTF8_count )
  {
    sUTF8 = 0;
    sUTF8_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sUTF8 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF16 )
      *sNextUTF16 = sUTF16;
    return 0;
  }

  bSwapBytes = false;
  if ( bTestByteOrder && sUTF16_count > 0 )
  {
    if ( 0xFEFF == sUTF16[0] )
    {
      // skip BOM
      sUTF16_count--;
      sUTF16++;
    }
    else if ( 0xFFFE == sUTF16[0] )
    {
      // skip BOM and swap bytes in rest of sUTF16
      bSwapBytes = true;
      sUTF16_count--;
      sUTF16++;
    }
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  if ( bSwapBytes )
  {
    for ( i = 0; i < sUTF16_count; i += j )
    {
      j = ON_DecodeSwapByteUTF16(sUTF16+i,sUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      k = ON_EncodeUTF8(u,s);
      if ( 0 != sUTF8 )
      {
        if ( output_count + k > sUTF8_count )
        {
          e.m_error_status |= 2;
          break;
        }
        memcpy(sUTF8+output_count,s,k*sizeof(sUTF8[0]));
      }
      output_count += k;
    }
  }
  else
  {
    for ( i = 0; i < sUTF16_count; i += j )
    {
      j = ON_DecodeUTF16(sUTF16+i,sUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      k = ON_EncodeUTF8(u,s);
      if ( 0 != sUTF8 )
      {
        if ( output_count + k > sUTF8_count )
        {
          e.m_error_status |= 2;
          break;
        }
        memcpy(sUTF8+output_count,s,k*sizeof(sUTF8[0]));
      }
      output_count += k;
    }
  }
  if ( 0 != sUTF8 && output_count < sUTF8_count)
    sUTF8[output_count] = 0;
  if ( sNextUTF16 )
    *sNextUTF16 = sUTF16+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}

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
    )
{
  int i, j, k, output_count, bSwapBytes;
  ON__UINT32 u;
  ON__UINT16 s[2];
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sInputUTF16_count && 0 != sInputUTF16 )
  {
    for ( sInputUTF16_count = 0; 0 != sInputUTF16[sInputUTF16_count]; sInputUTF16_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sInputUTF16 || sInputUTF16_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextInputUTF16 )
      *sNextInputUTF16 = sInputUTF16;
    return 0;
  }

  if ( 0 == sOutputUTF16_count )
  {
    sOutputUTF16 = 0;
    sOutputUTF16_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sOutputUTF16 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextInputUTF16 )
      *sNextInputUTF16 = sInputUTF16;
    return 0;
  }

  bSwapBytes = false;
  if ( bTestByteOrder && sInputUTF16_count > 0 )
  {
    if ( 0xFEFF == sInputUTF16[0] )
    {
      // skip BOM
      sInputUTF16_count--;
      sInputUTF16++;
    }
    else if ( 0xFFFE == sInputUTF16[0] )
    {
      // skip BOM and swap bytes in rest of sInputUTF16
      bSwapBytes = true;
      sInputUTF16_count--;
      sInputUTF16++;
    }
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  if ( bSwapBytes )
  {
    for ( i = 0; i < sInputUTF16_count; i += j )
    {
      j = ON_DecodeSwapByteUTF16(sInputUTF16+i,sInputUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      k = ON_EncodeUTF16(u,s);
      if ( 0 != sOutputUTF16 )
      {
        if ( output_count + k > sOutputUTF16_count )
        {
          e.m_error_status |= 2;
          break;
        }
        memcpy(sOutputUTF16+output_count,s,k*sizeof(sOutputUTF16[0]));
      }
      output_count += k;
    }
  }
  else
  {
    for ( i = 0; i < sInputUTF16_count; i += j )
    {
      j = ON_DecodeUTF16(sInputUTF16+i,sInputUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      k = ON_EncodeUTF16(u,s);
      if ( 0 != sOutputUTF16 )
      {
        if ( output_count + k > sOutputUTF16_count )
        {
          e.m_error_status |= 2;
          break;
        }
        memcpy(sOutputUTF16+output_count,s,k*sizeof(sOutputUTF16[0]));
      }
      output_count += k;
    }
  }
  if ( 0 != sOutputUTF16 && output_count < sOutputUTF16_count)
    sOutputUTF16[output_count] = 0;
  if ( sNextInputUTF16 )
    *sNextInputUTF16 = sInputUTF16+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}

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
    )
{
  int i, j, output_count, bSwapBytes;
  ON__UINT32 u;
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sUTF16_count && 0 != sUTF16 )
  {
    for ( sUTF16_count = 0; 0 != sUTF16[sUTF16_count]; sUTF16_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sUTF16 || sUTF16_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF16 )
      *sNextUTF16 = sUTF16;
    return 0;
  }

  if ( 0 == sUTF32_count )
  {
    sUTF32 = 0;
    sUTF32_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sUTF32 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF16 )
      *sNextUTF16 = sUTF16;
    return 0;
  }

  bSwapBytes = false;
  if ( bTestByteOrder && sUTF16_count > 0 )
  {
    if ( 0xFEFF == sUTF16[0] )
    {
      // skip BOM
      sUTF16_count--;
      sUTF16++;
    }
    else if ( 0xFFFE == sUTF16[0] )
    {
      // skip BOM and swap bytes in rest of sUTF16
      bSwapBytes = true;
      sUTF16_count--;
      sUTF16++;
    }
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  if ( bSwapBytes )
  {
    for ( i = 0; i < sUTF16_count; i += j )
    {
      j = ON_DecodeSwapByteUTF16(sUTF16+i,sUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      if ( 0 != sUTF32 )
      {
        if ( output_count >= sUTF32_count )
        {
          e.m_error_status |= 2;
          break;
        }
        sUTF32[output_count] = u;
      }
      output_count++;
    }
  }
  else
  {
    for ( i = 0; i < sUTF16_count; i += j )
    {
      j = ON_DecodeUTF16(sUTF16+i,sUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      if ( 0 != sUTF32 )
      {
        if ( output_count >= sUTF32_count )
        {
          e.m_error_status |= 2;
          break;
        }
        sUTF32[output_count] = u;
      }
      output_count++;
    }
  }

  if ( 0 != sUTF32 && output_count < sUTF32_count)
    sUTF32[output_count] = 0;
  if ( sNextUTF16 )
    *sNextUTF16 = sUTF16+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}

static ON__UINT32 SwapBytes32(ON__UINT32 u)
{
  ON__UINT8 b;
  ON__UINT8* p = (ON__UINT8*)&u;
  b = p[0]; p[0] = p[3]; p[3] = b;
  b = p[1]; p[1] = p[2]; p[2] = b;
  return u;
}

int ON_DecodeUTF32(
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    )
{
  ON__UINT32 uhi, ulo;

  ON_UnicodeErrorParameters local_e = ON_UnicodeErrorParameters::MaskErrors;
  if (nullptr == e)
    e = &local_e;

  if ( 0 == sUTF32 || sUTF32_count <= 0 || 0 == unicode_code_point )
  {
    e->m_error_status |= 1;
    return 0;
  }

  // special case for most common UTF-16 single element values
  if ( ( sUTF32[0] < 0xD800 ) || ( sUTF32[0] >= 0xE000 && sUTF32[0] <= 0x10FFFF) )
  {
    // valid UTF-32 encoding.
    *unicode_code_point = sUTF32[0];
    return 1;
  }

  // handle errors
  if ( 0 == e )
    return 0;

  if ( sUTF32_count >= 2 && sUTF32[0] < 0xDC00 && sUTF32[1] >=  0xDC00 && sUTF32[1] < 0xE000 )
  {
    // UTF-16 surrogate pair appears in UTF-32 array
    e->m_error_status |= 4;
    if ( 0 == (4 & e->m_error_mask) )
      return 0; // this error is not masked.

    uhi = sUTF32[0];
    ulo = sUTF32[1];
    *unicode_code_point = (uhi-0xD800)*0x400 + (ulo-0xDC00) + 0x10000;

    return 2; // error masked and reasonable value returned.
  }

  // bogus value
  e->m_error_status |= 16;
  if ( 16 != (16 & e->m_error_mask) || !ON_IsValidUnicodeCodePoint(e->m_error_code_point) )
  {
    // this error is not masked
    return 0;
  }

  *unicode_code_point = e->m_error_code_point;
  return 1; // error masked and e->m_error_code_point returnred.
}

int ON_DecodeSwapByteUTF32(
    const ON__UINT32* sUTF32,
    int sUTF32_count,
    struct ON_UnicodeErrorParameters* e,
    ON__UINT32* unicode_code_point
    )
{
  ON__UINT32 sUTF32swap[2];

  ON_UnicodeErrorParameters local_e = ON_UnicodeErrorParameters::MaskErrors;
  if (nullptr == e)
    e = &local_e;

  if ( 0 != sUTF32 && sUTF32_count > 0 )
  {
    sUTF32swap[0] = SwapBytes32(sUTF32[0]);
    if ( sUTF32_count > 1 )
    {
      // Get up to 2 elements to pass to the unswapped
      // decoder so that masked errors are uniformly
      // handled.
      sUTF32swap[1] = SwapBytes32(sUTF32[1]);
      sUTF32_count = 2;
    }
    sUTF32 = sUTF32swap;
  }

  return ON_DecodeUTF32(sUTF32,sUTF32_count,e,unicode_code_point);
}

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
    )
{
  int i, k, output_count, bSwapBytes;
  ON__UINT32 u;
  char s[6];
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sUTF32_count && 0 != sUTF32 )
  {
    for ( sUTF32_count = 0; 0 != sUTF32[sUTF32_count]; sUTF32_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sUTF32 || sUTF32_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF32 )
      *sNextUTF32 = sUTF32;
    return 0;
  }

  if ( 0 == sUTF8_count )
  {
    sUTF8 = 0;
    sUTF8_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sUTF8 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF32 )
      *sNextUTF32 = sUTF32;
    return 0;
  }

  bSwapBytes = false;
  if ( bTestByteOrder && sUTF32_count > 0 )
  {
    if ( 0x0000FEFF == sUTF32[0] )
    {
      // skip BOM
      sUTF32_count--;
      sUTF32++;
    }
    else if ( 0xFFFE0000 == sUTF32[0] )
    {
      // skip BOM and swap bytes in rest of sUTF32
      bSwapBytes = true;
      sUTF32_count--;
      sUTF32++;
    }
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  for ( i = 0; i < sUTF32_count; i++ )
  {
    u = bSwapBytes ? SwapBytes32(sUTF32[i]) : sUTF32[i];
    if ( !ON_IsValidUnicodeCodePoint(u) )
    {
      e.m_error_status |= 16;
      if ( 16 != (16 & e.m_error_mask) )
        break;
      if ( !ON_IsValidUnicodeCodePoint(e.m_error_code_point) )
        break;
      u = e.m_error_code_point;
    }
    k = ON_EncodeUTF8(u,s);
    if ( 0 != sUTF8 )
    {
      if ( output_count + k > sUTF8_count )
      {
        e.m_error_status |= 2;
        break;
      }
      memcpy(sUTF8+output_count,s,k*sizeof(sUTF8[0]));
    }
    output_count += k;
  }

  if ( 0 != sUTF8 && output_count < sUTF8_count)
    sUTF8[output_count] = 0;
  if ( sNextUTF32 )
    *sNextUTF32 = sUTF32+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}

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
    )
{
  int i, k, output_count, bSwapBytes;
  ON__UINT32 u;
  ON__UINT16 w[2];
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sUTF32_count && 0 != sUTF32 )
  {
    for ( sUTF32_count = 0; 0 != sUTF32[sUTF32_count]; sUTF32_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sUTF32 || sUTF32_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF32 )
      *sNextUTF32 = sUTF32;
    return 0;
  }

  if ( 0 == sUTF16_count )
  {
    sUTF16 = 0;
    sUTF16_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sUTF16 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF32 )
      *sNextUTF32 = sUTF32;
    return 0;
  }

  bSwapBytes = false;
  if ( bTestByteOrder && sUTF32_count > 0 )
  {
    if ( 0x0000FEFF == sUTF32[0] )
    {
      // skip BOM
      sUTF32_count--;
      sUTF32++;
    }
    else if ( 0xFFFE0000 == sUTF32[0] )
    {
      // skip BOM and swap bytes in rest of sUTF32
      bSwapBytes = true;
      sUTF32_count--;
      sUTF32++;
    }
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  for ( i = 0; i < sUTF32_count; i++ )
  {
    u = bSwapBytes ? SwapBytes32(sUTF32[i]) : sUTF32[i];
    if ( !ON_IsValidUnicodeCodePoint(u) )
    {
      e.m_error_status |= 16;
      if ( 16 != (16 & e.m_error_mask) )
        break;
      if ( !ON_IsValidUnicodeCodePoint(e.m_error_code_point) )
        break;
      u = e.m_error_code_point;
    }
    k = ON_EncodeUTF16(u,w);
    if ( 0 != sUTF16 )
    {
      if ( output_count + k > sUTF16_count )
      {
        e.m_error_status |= 2;
        break;
      }
      sUTF16[output_count] = w[0];
      if ( 2 == k )
        sUTF16[output_count+1] = w[1];
    }
    output_count += k;
  }

  if ( 0 != sUTF16 && output_count < sUTF16_count)
    sUTF16[output_count] = 0;
  if ( sNextUTF32 )
    *sNextUTF32 = sUTF32+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}


int ON_ConvertUTF32ToUTF32(
    int bTestByteOrder,
    const ON__UINT32* sUTF16,
    int sUTF16_count,
    unsigned int* sUTF32,
    int sUTF32_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const ON__UINT32** sNextUTF16
    )
{
  int i, j, output_count, bSwapBytes;
  ON__UINT32 u;
  struct ON_UnicodeErrorParameters e;

  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sUTF16_count && 0 != sUTF16 )
  {
    for ( sUTF16_count = 0; 0 != sUTF16[sUTF16_count]; sUTF16_count++)
    {
      // empty for body
    }
  }

  if ( 0 == sUTF16 || sUTF16_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF16 )
      *sNextUTF16 = sUTF16;
    return 0;
  }

  if ( 0 == sUTF32_count )
  {
    sUTF32 = 0;
    sUTF32_count = 2147483647; // maximum value of a 32-bit signed int
  }
  else if ( 0 == sUTF32 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    if ( sNextUTF16 )
      *sNextUTF16 = sUTF16;
    return 0;
  }

  bSwapBytes = false;
  if ( bTestByteOrder && sUTF16_count > 0 )
  {
    if ( 0x0000FEFF == sUTF16[0] )
    {
      // skip BOM
      sUTF16_count--;
      sUTF16++;
    }
    else if ( 0xFFFE0000 == sUTF16[0])
    {
      // skip BOM and swap bytes in rest of sUTF16
      bSwapBytes = true;
      sUTF16_count--;
      sUTF16++;
    }
  }

  e.m_error_status = 0;
  e.m_error_mask = error_mask;
  e.m_error_code_point = error_code_point;

  output_count = 0;

  if ( bSwapBytes )
  {
    for ( i = 0; i < sUTF16_count; i += j )
    {
      j = ON_DecodeSwapByteUTF32(sUTF16+i,sUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      if ( 0 != sUTF32 )
      {
        if ( output_count >= sUTF32_count )
        {
          e.m_error_status |= 2;
          break;
        }
        sUTF32[output_count] = u;
      }
      output_count++;
    }
  }
  else
  {
    for ( i = 0; i < sUTF16_count; i += j )
    {
      j = ON_DecodeUTF32(sUTF16+i,sUTF16_count-i,&e,&u);
      if ( j <= 0 )
        break;
      if ( 0 != sUTF32 )
      {
        if ( output_count >= sUTF32_count )
        {
          e.m_error_status |= 2;
          break;
        }
        sUTF32[output_count] = u;
      }
      output_count++;
    }
  }

  if ( 0 != sUTF32 && output_count < sUTF32_count)
    sUTF32[output_count] = 0;
  if ( sNextUTF16 )
    *sNextUTF16 = sUTF16+i;
  if ( error_status )
    *error_status = e.m_error_status;
  
  return output_count;
}

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
    )
{
  int rc;

  switch(sizeof(sWideChar[0]))
  {
  case sizeof(char):
    // assume wchar_t strings are UTF-8 encoded
    rc = ON_ConvertUTF8ToUTF8(
            bTestByteOrder,
            (const char*)sWideChar,sWideChar_count,
            sUTF8,sUTF8_count,
            error_status,error_mask,error_code_point,
            (const char**)sNextWideChar
            );
    break;

  case sizeof(ON__UINT16):
    // assume wchar_t strings are UTF-16 encoded
    rc = ON_ConvertUTF16ToUTF8(
            bTestByteOrder,
            (const ON__UINT16*)sWideChar,sWideChar_count,
            sUTF8,sUTF8_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT16**)sNextWideChar
            );
    break;

  case sizeof(ON__UINT32):
    // assume wchar_t strings are UTF-32 encoded
    rc = ON_ConvertUTF32ToUTF8(
            bTestByteOrder,
            (const ON__UINT32*)sWideChar,sWideChar_count,
            sUTF8,sUTF8_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT32**)sNextWideChar
            );
    break;

  default:
    rc = 0;
  }

  return rc;
}


int ON_ConvertWideCharToUTF16(
    int bTestByteOrder,
    const wchar_t* sWideChar,
    int sWideChar_count,
    char* sUTF16,
    int sUTF16_count,
    unsigned int* error_status,
    unsigned int error_mask,
    ON__UINT32 error_code_point,
    const wchar_t** sNextWideChar
    )
{
  int rc;

  switch(sizeof(sWideChar[0]))
  {
  case sizeof(char):
    // assume wchar_t strings are UTF-8 encoded
    rc = ON_ConvertUTF8ToUTF16(
            bTestByteOrder,
            (const char*)sWideChar,sWideChar_count,
            (ON__UINT16*)sUTF16,sUTF16_count,
            error_status,error_mask,error_code_point,
            (const char**)sNextWideChar
            );
    break;

  case sizeof(ON__UINT16):
    // assume wchar_t strings are UTF-16 encoded
    rc = ON_ConvertUTF16ToUTF16(
            bTestByteOrder,
            (const ON__UINT16*)sWideChar,sWideChar_count,
            (ON__UINT16*)sUTF16,sUTF16_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT16**)sNextWideChar
            );
    break;

  case sizeof(ON__UINT32):
    // assume wchar_t strings are UTF-32 encoded
    rc = ON_ConvertUTF32ToUTF16(
            bTestByteOrder,
            (const ON__UINT32*)sWideChar,sWideChar_count,
            (ON__UINT16*)sUTF16,sUTF16_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT32**)sNextWideChar
            );
    break;

  default:
    rc = 0;
  }

  return rc;
}


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
    )
{
  int rc;

  switch(sizeof(sWideChar[0]))
  {
  case sizeof(char):
    // assume wchar_t strings are UTF-8 encoded
    rc = ON_ConvertUTF8ToUTF32(
            bTestByteOrder,
            (const char*)sWideChar,sWideChar_count,
            sUTF32,sUTF32_count,
            error_status,error_mask,error_code_point,
            (const char**)sNextWideChar
            );
    break;

  case sizeof(ON__UINT16):
    // assume wchar_t strings are UTF-16 encoded
    rc = ON_ConvertUTF16ToUTF32(
            bTestByteOrder,
            (const ON__UINT16*)sWideChar,sWideChar_count,
            sUTF32,sUTF32_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT16**)sNextWideChar
            );
    break;

  case sizeof(ON__UINT32):
    // assume wchar_t strings are UTF-32 encoded
    rc = ON_ConvertUTF32ToUTF32(
            bTestByteOrder,
            (const ON__UINT32*)sWideChar,sWideChar_count,
            sUTF32,sUTF32_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT32**)sNextWideChar
            );
     break;

  default:
    rc = 0;
  }

  return rc;
}


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
    )
{
  int rc;

  switch(sizeof(sWideChar[0]))
  {
  case sizeof(char):
    // assume wchar_t strings are UTF-8 encoded
    rc = ON_ConvertUTF8ToUTF8(
            bTestByteOrder,
            sUTF8,sUTF8_count,
            (char*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            sNextUTF8
            );
    break;

  case sizeof(ON__UINT16):
    // assume wchar_t strings are UTF-16 encoded
    rc = ON_ConvertUTF8ToUTF16(
            bTestByteOrder,
            sUTF8,sUTF8_count,
            (ON__UINT16*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            sNextUTF8
            );
    break;

  case sizeof(ON__UINT32):
    // assume wchar_t strings are UTF-32 encoded
    rc = ON_ConvertUTF8ToUTF32(
            bTestByteOrder,
            sUTF8,sUTF8_count,
            (ON__UINT32*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            sNextUTF8
            );
    break;

  default:
    if (error_status)
      *error_status = 1;
    if (sNextUTF8)
      *sNextUTF8 = sUTF8;
    rc = 0;
  }

  return rc;
}


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
    )
{
  int rc;

  switch(sizeof(sWideChar[0]))
  {
  case sizeof(char):
    // assume wchar_t strings are UTF-8 encoded
    rc = ON_ConvertUTF16ToUTF8(
            bTestByteOrder,
            (const ON__UINT16*)sUTF16,sUTF16_count,
            (char*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT16**)sNextUTF16
            );
    break;

  case sizeof(ON__UINT16):
    // assume wchar_t strings are UTF-16 encoded
    rc = ON_ConvertUTF16ToUTF16(
            bTestByteOrder,
            (const ON__UINT16*)sUTF16,sUTF16_count,
            (ON__UINT16*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT16**)sNextUTF16
            );
    break;

  case sizeof(ON__UINT32):
    // assume wchar_t strings are UTF-32 encoded
    rc = ON_ConvertUTF16ToUTF32(
            bTestByteOrder,
            (const ON__UINT16*)sUTF16,sUTF16_count,
            (ON__UINT32*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT16**)sNextUTF16
            );
    break;

  default:
    if (error_status)
      *error_status = 1;
    if (sNextUTF16)
      *sNextUTF16 = sUTF16;
    rc = 0;
  }

  return rc;
}

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
    )
{
  int rc;

  switch(sizeof(sWideChar[0]))
  {
  case sizeof(char):
    // assume wchar_t strings are UTF-8 encoded
    rc = ON_ConvertUTF32ToUTF8(
            bTestByteOrder,
            (const ON__UINT32*)sUTF32,sUTF32_count,
            (char*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT32**)sNextUTF32
            );
    break;

  case sizeof(ON__UINT16):
    // assume wchar_t strings are UTF-16 encoded
    rc = ON_ConvertUTF32ToUTF16(
            bTestByteOrder,
            (const ON__UINT32*)sUTF32,sUTF32_count,
            (ON__UINT16*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT32**)sNextUTF32
            );
    break;

  case sizeof(ON__UINT32):
    // assume wchar_t strings are UTF-32 encoded
    rc = ON_ConvertUTF32ToUTF32(
            bTestByteOrder,
            (const ON__UINT32*)sUTF32,sUTF32_count,
            (ON__UINT32*)sWideChar,sWideChar_count,
            error_status,error_mask,error_code_point,
            (const ON__UINT32**)sNextUTF32
            );
    break;

  default:
    if (error_status)
      *error_status = 1;
    if (sNextUTF32)
      *sNextUTF32 = sUTF32;
    rc = 0;
  }

  return rc;
}

const ON_wString ON_wString::FromUnicodeCodePoints(
  const ON__UINT32* code_points,
  int code_point_count,
  ON__UINT32 error_code_point
)
{
  const bool bErrorCodePointIsValid = ON_IsValidUnicodeCodePoint(error_code_point);
  
  if (nullptr == code_points)
    return ON_wString::EmptyString;

  if (-1 == code_point_count)
  {
    code_point_count = 0;
    while (0 != code_points[code_point_count])
    {
      if (
        false == bErrorCodePointIsValid
        && false == ON_IsValidUnicodeCodePoint(code_points[code_point_count])
        )
      {
        break;
      }
      code_point_count++;
    }
  }

  if ( code_point_count <= 0 )
    return ON_wString::EmptyString;

  const int bTestByteOrder = false;
  unsigned int error_status = 0;
  const unsigned int error_mask = bErrorCodePointIsValid ? 0xFFFFFFFF : 0;
  int wchar_count = ON_ConvertUTF32ToWideChar(
    bTestByteOrder,
    code_points,
    code_point_count,
    nullptr,
    0,
    &error_status,
    error_mask,
    error_code_point,
    nullptr
  );

  if (wchar_count <= 0)
    return ON_wString::EmptyString;

  ON_wString s;
  const int s_capacity = (wchar_count + 1);
  wchar_t* a = s.ReserveArray((size_t)s_capacity);
  error_status = 0;
  wchar_count = ON_ConvertUTF32ToWideChar(
    bTestByteOrder,
    code_points,
    code_point_count,
    a,
    s_capacity,
    &error_status,
    error_mask,
    error_code_point,
    nullptr
  );
  
  if (wchar_count <= 0)
    return ON_wString::EmptyString;

  s.SetLength(wchar_count);
  return s;
}


////int ON_ConvertWindowsCodePageValueToWideChar(
////  int windows_code_page,
////  ON__UINT32 code_page_character_value,
////  size_t w_capacity,
////  wchar_t* w
////)
////{
////  ON__UINT32 unicode_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter;
////  ON_UnicodeErrorParameters e;
////  memset(&e, 0, sizeof(e));
////  e.m_error_mask = 0xFF;
////  e.m_error_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter;
////  ON_DecodeWindowsCodePageValue( windows_code_page, code_page_character_value,  &e,  &unicode_code_point);
////  return ON_EncodeWideChar(unicode_code_point, w_capacity, w);
////}

ON__UINT32 ON_MapRTFcharsetToWindowsCodePage(
  ON__UINT32 rtf_charset,
  ON__UINT32 default_code_page
)
{
  //  From the Microsoft version of the RTF ver 1.9 spec available on MSDN
  //
  //  \fcharsetN:  	Specifies the character set of a font in the font table.If this appears, it implies that bytes in runs 
  //    tagged with the associated \fN are character codes in the codepage corresponding to the charset N.
  //    Use this codepage to convert the codes to Unicode using a function like the Windows MultiByteToWideChar().
  //    See also the \cpgN control word, which, if it appears, supersedes the codepage given by \fcharsetN.Values for N are defined, 
  //    for example, in the Windows header file wingdi.h(e.g., see ANSI_CHARSET) and are repeated here together with the corresponding 
  //    Windows or Mac codepages for convenience:charset	codepage	Windows / Mac name

  //  A font may have a different character set from the character set of the document. For example, the Symbol font has the 
  //    same characters in the same code positions both on the Macintosh and in Windows. Typically, RTF fonts use the code page 
  //    corresponding to the \fcharsetN control word in their \fonttbl description. If the charset doesn’t exist, the codepage 
  //    may be given by the \cpgN control word, for which the code page is N. If the \cpgN does appear, it supersedes the code 
  //    page corresponding to the \fcharsetN. 
  //    For such cases, codepage conversions can be avoided altogether by using the Unicode \uN notation for characters. 
  //    In addition, file names (used in field instructions and in embedded fonts) may not necessarily be the same as the character 
  //    set of the document; the \cpgN control word can change the character set for these file names as well. 
  //  

  ON__UINT32 cp;
  switch (rtf_charset)
  {
  case 0:   cp = 1252; break;         // ANSI
  case 1:   cp = 0; break;            // Default
  case 2:   cp = 42; break;           // Symbol
  case 77:  cp = 10000; break;        // Mac Roman
  case 78:  cp = 10001; break;        // Mac Shift Jis
  case 79:  cp = 10003; break;        // Mac Hangul
  case 80:  cp = 10008; break;        // Mac GB2312
  case 81:  cp = 10002; break;        // Mac Big5
  case 82:  cp = default_code_page; break; // Mac Johab (old)
  case 83:  cp = 10005; break;        // Mac Hebrew
  case 84:  cp = 10004; break;        // Mac Arabic
  case 85:  cp = 10006; break;        // Mac Greek
  case 86:  cp = 10081; break;        // Mac Turkish
  case 87:  cp = 10021; break;        // Mac Thai
  case 88:  cp = 10029; break;        // Mac East Europe
  case 89:  cp = 10007; break;        // Mac Russian
  case 128: cp = 932; break;          // Shift JIS
  case 129: cp = 949; break;          // Hangul (Korean)
  case 130: cp = 1361; break;         // Johab
  case 134: cp = 936; break;          // GB2312
  case 136: cp = 950; break;          // Big5
  case 161: cp = 1253; break;         // Greek
  case 162: cp = 1254; break;         // Turkish
  case 163: cp = 1258; break;         // Vietnamese
  case 177: cp = 1255; break;         // Hebrew
  case 178: cp = 1256; break;         // Arabic 
  case 179: cp = default_code_page; break; // Arabic Traditional (old)
  case 180: cp = default_code_page; break; // Arabic user (old)
  case 181: cp = default_code_page; break; // Hebrew user (old)
  case 186: cp = 1257; break;         // Baltic
  case 204: cp = 1251; break;         // Russian
  case 222: cp = 874; break;          // Thai
  case 238: cp = 1250; break;         // Eastern European
  case 254: cp = 437; break;          // PC 437
  case 255: cp = 850; break;          // OEM
  default: cp = default_code_page; break;
  }
  return cp;
}

static int ON_Internal_ConvertMSSBCPToWideChar(
  const ON__UINT32* sb_code_page_0x80_to_0xFF_to_unicode,
  const char* sMBCS,
  int sMBCS_count,
  wchar_t* sWideChar,
  int sWideChar_capacity,
  unsigned int* error_status
)
{
  wchar_t* sWideCharMax
    = (sWideChar_capacity > 0 && nullptr != sWideChar)
    ? sWideChar + sWideChar_capacity
    : nullptr;
  if (nullptr == sWideCharMax)
  {
    sWideChar = nullptr;
    sWideChar_capacity = 0;
  }
  else
  {
    sWideChar[0] = 0;
  }
  if (nullptr != error_status)
    *error_status = 0;

  unsigned int e = 0;
  if (nullptr == sMBCS || sMBCS_count < 0)
    sMBCS_count = 0;
  wchar_t* s = sWideChar;
  wchar_t w_buffer[8];
  int rc = 0;

  for (int i = 0; i < sMBCS_count; i++)
  {
    const ON__UINT32 c = (unsigned char)sMBCS[i];
    ON__UINT32 unicode_code_point;
    if (c < 0x80)
      unicode_code_point = c;
    else 
    {
      if (c <= 0xFF && nullptr != sb_code_page_0x80_to_0xFF_to_unicode )
      {
        unicode_code_point = sb_code_page_0x80_to_0xFF_to_unicode[c - 0x80];
        if (0 == ON_IsValidUnicodeCodePoint(unicode_code_point))
          unicode_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter;
      }
      else
        unicode_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter;
      if ( ON_UnicodeCodePoint::ON_ReplacementCharacter == unicode_code_point )
        e |= 16;
    }
    const int w_count = ON_EncodeWideChar(unicode_code_point, sizeof(w_buffer)/sizeof(w_buffer[0]), w_buffer);
    if (w_count <= 0)
    {
      e |= 16;
      continue;
    }
    rc += w_count;
    if (s == nullptr)
      continue;
    wchar_t* s1 = s + w_count;
    if (s1 > sWideCharMax)
    {
      e |= 2;
      continue;
    }
    const wchar_t* w = w_buffer;
    while (s < s1)
      *s++ = *w++;
  }
  
  while (s < sWideCharMax)
  {
    *s++ = 0;
  }

  if (nullptr != error_status)
    *error_status = e;

  return rc;
}

int ON_ConvertMSMBCPToWideChar(
    ON__UINT32 windows_code_page,
    const char* sMBCS,
    int sMBCS_count,
    wchar_t* sWideChar,
    int sWideChar_capacity,
    unsigned int* error_status
    )
{
  if ( 0 != error_status )
    *error_status = 0;

  if ( -1 == sMBCS_count && nullptr != sMBCS )
  {
    for ( sMBCS_count = 0; 0 != sMBCS[sMBCS_count]; sMBCS_count++)
    {
      // empty for body
    }
  }

  if ( nullptr == sMBCS || sMBCS_count < 0 )
  {
    if ( 0 != error_status )
      *error_status |= 1;
    return 0;
  }

  if ( 0 == sMBCS_count )
  {
    return 0;
  }

  if (sWideChar_capacity <= 0)
  {
    sWideChar_capacity = 0;
    sWideChar = nullptr;
  }
  else if (nullptr == sWideChar)
  {
    sWideChar_capacity = 0;
  }
  else
  {
    sWideChar[0] = 0;
  }

  const char* c = sMBCS;
  const char* c1 = c + sMBCS_count;
  wchar_t* w = sWideChar;
  wchar_t* w1 = w + sWideChar_capacity;
  while (c < c1 && *c >= 0 && *c <= 127)
  {
    if (nullptr != w)
    {
      if (w >= w1)
        break;
      *w++ = (wchar_t)*c;
    }
    c++;
  }
  if (c == c1)
  {
    if (w < w1)
      *w = 0;
    return sMBCS_count;
  }

  const ON__UINT32* sb_code_page_0x80_to_0xFF_to_unicode = ON_MSSBCP_0x80_0xFF_Unicode(windows_code_page);
  if (nullptr != sb_code_page_0x80_to_0xFF_to_unicode)
  {
    // fast platform independent single byte code page conversion built into opennurbs
    return ON_Internal_ConvertMSSBCPToWideChar(
      sb_code_page_0x80_to_0xFF_to_unicode,
      sMBCS,
      sMBCS_count,
      sWideChar,
      sWideChar_capacity,
      error_status
    );
  }

#if defined(ON_RUNTIME_WIN)
  // Starting with Windows Vista, the function does not drop illegal code points when dwFlags=0.
  // It replaces illegal sequences with U+FFFD (encoded as appropriate for the specified codepage).
  DWORD dwFlags = 0; 
  int sWideChar_count = ::MultiByteToWideChar(windows_code_page, dwFlags, sMBCS, sMBCS_count, sWideChar, sWideChar_capacity);
  if (sWideChar_count < 0)
    sWideChar_count = 0;
  if (nullptr == sWideChar)
    return sWideChar_count;

  for (int i = 0; i < sWideChar_count; i++)
  {
    if (0 == sWideChar[i])
    {
      sWideChar_count = i;
      break;
    }
    if ( ON_wString::ReplacementCharacter == sWideChar[i] )
    {
      if ( nullptr != error_status)
        *error_status |= 16;
    }
  }
  if (sWideChar_count < sWideChar_capacity)
    sWideChar[sWideChar_count] = 0;
  return sWideChar_count;
#else
  // Add support for Mac if needed.
  // Shift JIS, Hangol, and Big 5 are likely candidates. 
  // These are encodings with either 1 or 2 bytes per glyph.
  if (949 == windows_code_page)
  {

  }
  return 0;
#endif

}
