/*
//
// Copyright (c) 1993-2017 Robert McNeel & Associates. All rights reserved.
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

#if defined(OPENNURBS_FREETYPE_SUPPORT)

// opennurbs uses FreeType to calculate font metric, glyph metric, and glyph outline information.

// FreeType Licensing:
//
//// Retrieved March 22, 2017 
//// https://www.freetype.org/freetype2/docs/index.html
////What is FreeType?
////
////FreeType is a software font engine that is designed to be small, efficient, 
////highly customizable, and portable while capable of producing high-quality 
////output (glyph images). It can be used in graphics libraries, display servers,
////font conversion tools, text image generation tools, and many other products as well.
////
////Note that FreeType is a font service and doesn't provide APIs to perform 
////higher-level features like text layout or graphics processing 
////(e.g., colored text rendering, ‘hollowing’, etc.). However, it greatly 
////simplifies these tasks by providing a simple, easy to use, and uniform
////interface to access the content of font files.
////
////FreeType is released under two open-source licenses: our own BSD-like
////FreeType License and the GNU Public License, Version 2. It can thus
////be used by any kind of projects, be they proprietary or not.
////
////Please note that ‘FreeType’ is also called ‘FreeType 2’, to
////distinguish it from the old, deprecated ‘FreeType 1’ library,
////a predecessor no longer maintained and supported.
////
//// http://git.savannah.gnu.org/cgit/freetype/freetype2.git/tree/docs/FTL.TXT
//// 
////                    The FreeType Project LICENSE
////                    ----------------------------
////
////                            2006-Jan-27
////
////                    Copyright 1996-2002, 2006 by
////          David Turner, Robert Wilhelm, and Werner Lemberg
////
////
////
////Introduction
////============
////
////  The FreeType  Project is distributed in  several archive packages;
////  some of them may contain, in addition to the FreeType font engine,
////  various tools and  contributions which rely on, or  relate to, the
////  FreeType Project.
////
////  This  license applies  to all  files found  in such  packages, and
////  which do not  fall under their own explicit  license.  The license
////  affects  thus  the  FreeType   font  engine,  the  test  programs,
////  documentation and makefiles, at the very least.
////
////  This  license   was  inspired  by  the  BSD,   Artistic,  and  IJG
////  (Independent JPEG  Group) licenses, which  all encourage inclusion
////  and  use of  free  software in  commercial  and freeware  products
////  alike.  As a consequence, its main points are that:
////
////    o We don't promise that this software works. However, we will be
////      interested in any kind of bug reports. (`as is' distribution)
////
////    o You can  use this software for whatever you  want, in parts or
////      full form, without having to pay us. (`royalty-free' usage)
////
////    o You may not pretend that  you wrote this software.  If you use
////      it, or  only parts of it,  in a program,  you must acknowledge
////      somewhere  in  your  documentation  that  you  have  used  the
////      FreeType code. (`credits')
////
////  We  specifically  permit  and  encourage  the  inclusion  of  this
////  software, with  or without modifications,  in commercial products.
////  We  disclaim  all warranties  covering  The  FreeType Project  and
////  assume no liability related to The FreeType Project.
////
////
////  Finally,  many  people  asked  us  for  a  preferred  form  for  a
////  credit/disclaimer to use in compliance with this license.  We thus
////  encourage you to use the following text:
////
////   """
////    Portions of this software are copyright © <year> The FreeType
////    Project (www.freetype.org).  All rights reserved.
////   """
////
////  Please replace <year> with the value from the FreeType version you
////  actually use.
////
////
////Legal Terms
////===========
////
////0. Definitions
////--------------
////
////  Throughout this license,  the terms `package', `FreeType Project',
////  and  `FreeType  archive' refer  to  the  set  of files  originally
////  distributed  by the  authors  (David Turner,  Robert Wilhelm,  and
////  Werner Lemberg) as the `FreeType Project', be they named as alpha,
////  beta or final release.
////
////  `You' refers to  the licensee, or person using  the project, where
////  `using' is a generic term including compiling the project's source
////  code as  well as linking it  to form a  `program' or `executable'.
////  This  program is  referred to  as  `a program  using the  FreeType
////  engine'.
////
////  This  license applies  to all  files distributed  in  the original
////  FreeType  Project,   including  all  source   code,  binaries  and
////  documentation,  unless  otherwise  stated   in  the  file  in  its
////  original, unmodified form as  distributed in the original archive.
////  If you are  unsure whether or not a particular  file is covered by
////  this license, you must contact us to verify this.
////
////  The FreeType  Project is copyright (C) 1996-2000  by David Turner,
////  Robert Wilhelm, and Werner Lemberg.  All rights reserved except as
////  specified below.
////
////1. No Warranty
////--------------
////
////  THE FREETYPE PROJECT  IS PROVIDED `AS IS' WITHOUT  WARRANTY OF ANY
////  KIND, EITHER  EXPRESS OR IMPLIED,  INCLUDING, BUT NOT  LIMITED TO,
////  WARRANTIES  OF  MERCHANTABILITY   AND  FITNESS  FOR  A  PARTICULAR
////  PURPOSE.  IN NO EVENT WILL ANY OF THE AUTHORS OR COPYRIGHT HOLDERS
////  BE LIABLE  FOR ANY DAMAGES CAUSED  BY THE USE OR  THE INABILITY TO
////  USE, OF THE FREETYPE PROJECT.
////
////2. Redistribution
////-----------------
////
////  This  license  grants  a  worldwide, royalty-free,  perpetual  and
////  irrevocable right  and license to use,  execute, perform, compile,
////  display,  copy,   create  derivative  works   of,  distribute  and
////  sublicense the  FreeType Project (in  both source and  object code
////  forms)  and  derivative works  thereof  for  any  purpose; and  to
////  authorize others  to exercise  some or all  of the  rights granted
////  herein, subject to the following conditions:
////
////    o Redistribution of  source code  must retain this  license file
////      (`FTL.TXT') unaltered; any  additions, deletions or changes to
////      the original  files must be clearly  indicated in accompanying
////      documentation.   The  copyright   notices  of  the  unaltered,
////      original  files must  be  preserved in  all  copies of  source
////      files.
////
////    o Redistribution in binary form must provide a  disclaimer  that
////      states  that  the software is based in part of the work of the
////      FreeType Team,  in  the  distribution  documentation.  We also
////      encourage you to put an URL to the FreeType web page  in  your
////      documentation, though this isn't mandatory.
////
////  These conditions  apply to any  software derived from or  based on
////  the FreeType Project,  not just the unmodified files.   If you use
////  our work, you  must acknowledge us.  However, no  fee need be paid
////  to us.
////
////3. Advertising
////--------------
////
////  Neither the  FreeType authors and  contributors nor you  shall use
////  the name of the  other for commercial, advertising, or promotional
////  purposes without specific prior written permission.
////
////  We suggest,  but do not require, that  you use one or  more of the
////  following phrases to refer  to this software in your documentation
////  or advertising  materials: `FreeType Project',  `FreeType Engine',
////  `FreeType library', or `FreeType Distribution'.
////
////  As  you have  not signed  this license,  you are  not  required to
////  accept  it.   However,  as  the FreeType  Project  is  copyrighted
////  material, only  this license, or  another one contracted  with the
////  authors, grants you  the right to use, distribute,  and modify it.
////  Therefore,  by  using,  distributing,  or modifying  the  FreeType
////  Project, you indicate that you understand and accept all the terms
////  of this license.
////
////4. Contacts
////-----------
////
////  There are two mailing lists related to FreeType:
////
////    o freetype@nongnu.org
////
////      Discusses general use and applications of FreeType, as well as
////      future and  wanted additions to the  library and distribution.
////      If  you are looking  for support,  start in  this list  if you
////      haven't found anything to help you in the documentation.
////
////    o freetype-devel@nongnu.org
////
////      Discusses bugs,  as well  as engine internals,  design issues,
////      specific licenses, porting, etc.
////
////  Our home page can be found at
////
////    http://www.freetype.org
////
////--- end of FTL.TXT ---


// There is a compiler option for this file, opennurbs_freetype.cpp, that
// adds ./freetype263/include to the list of "system" include paths.
#include "opennurbs_freetype_include.h"

#if defined(ON_COMPILER_MSC)
#pragma ON_PRAGMA_WARNING_PUSH
/*
Suppress 4263 warnings from dwrite.h
Warning	C4263	..: member function does not override any base class virtual member function ... C:\Program Files (x86)\Windows Kits\8.1\Include\um\DWrite.h ...
Warning	C4263	..: member function does not override any base class virtual member function ... C:\Program Files (x86)\Windows Kits\8.1\Include\um\DWrite_1.h ...
Warning	C4263	..: member function does not override any base class virtual member function ... C:\Program Files (x86)\Windows Kits\8.1\Include\um\dwrite_2.h ...
*/
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4263 4264 )
#endif

#include FT_OUTLINE_H
#include FT_GLYPH_H
#include FT_MODULE_H

#if defined(ON_RUNTIME_WIN)
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")
#endif

#if defined(ON_COMPILER_MSC)
#pragma ON_PRAGMA_WARNING_POP
#endif

class ON_FontFileBuffer
{
public:
  ON_FontFileBuffer() = default;

  ~ON_FontFileBuffer()
  {
    Internal_Destroy();
  }

  ON_FontFileBuffer(const ON_FontFileBuffer& src)
  {
    Internal_CopyFrom(src);
  } 

  ON_FontFileBuffer& operator=(const ON_FontFileBuffer& src)
  {
    Internal_CopyFrom(src);
    return *this;
  }

  void* Buffer() const
  {
    return m_buffer;
  }

  void* AllocateBuffer(size_t sizeof_buffer)
  {
    if (sizeof_buffer != m_sizeof_buffer)
    {
      Internal_Destroy();
      if (sizeof_buffer > 0)
      {
        m_buffer = onmalloc(sizeof_buffer);
        if (nullptr != m_buffer)
        {
          m_sizeof_buffer = sizeof_buffer;
        }
      }
    }
    return m_buffer;
  }

  size_t SizeOfBuffer() const
  {
    return m_sizeof_buffer;
  }

  void TransferTo(
    ON_FontFileBuffer& dest
  )
  {
    if (this != &dest)
    {
      dest.Internal_Destroy();
      dest.m_buffer = m_buffer;
      m_buffer = nullptr;
      dest.m_sizeof_buffer = m_sizeof_buffer;
      m_sizeof_buffer = 0;
    }
  }

private:
  size_t m_sizeof_buffer = 0;
  void* m_buffer = nullptr;

private:
  void Internal_CopyFrom(const ON_FontFileBuffer& src)
  {
    AllocateBuffer(src.m_sizeof_buffer);
    if (nullptr != m_buffer)
    {
      memcpy(m_buffer, src.m_buffer, m_sizeof_buffer);
    }
  }

  void Internal_Destroy()
  {
    if (nullptr != m_buffer)
    {
      onfree(m_buffer);
      m_buffer = nullptr;
    }
    m_sizeof_buffer = 0;
  }
};

class ON_FreeTypeFace
{
public:
  ON_FreeTypeFace() = default;
  ~ON_FreeTypeFace();
  FT_Face m_face = nullptr;
  ON_FontFileBuffer m_font_buffer;
private:
  ON_FreeTypeFace(const ON_FreeTypeFace&) = delete;
  ON_FreeTypeFace& operator=(const ON_FreeTypeFace&) = delete;
};

ON_FreeTypeFace::~ON_FreeTypeFace()
{
  //  FT_New_Memory_Face documentation states:
  //    You must not deallocate the memory before calling @FT_Done_Face.
  //    The memory refered to is m_tt_file_buffer.
  if (nullptr != m_face)
  {
    FT_Done_Face(m_face);
    m_face = nullptr;
  }
  m_font_buffer.AllocateBuffer(0);
}

class ON_FreeType
{
public:
  static FT_Library Library();

  static ON_FreeTypeFace* CreateFace(
    const ON_Font& font
  );

  /*
  Description:
    Finds the glyph id for the specified Unicode code point. When a non-zero
    glyph is is returned, the face->charmap is set to the charmap that was used
    to find the glyph. In some cases this is not a Unicode charmap and unicode_code_point
    was internally coverted to a character code appropriate for the returned charmap.
    In principle, the glyph id for a Unicode code point is independent of the charmap.
  Returns:
     0: failure
    >0: font glyph id
  */
  static unsigned int GlyphId(
    FT_Face face,
    ON__UINT32 unicode_code_point
  );

  static const ON_wString FaceFlagsToString(FT_Long face_flags);

  static const ON_wString StyleFlagsToString(FT_Long style_flags);

  static const ON_wString EncodingTypeToString(
    FT_Encoding charmap_encoding
  );

  static const ON_wString CharmapPlatformEncodingDescription(
    const FT_CharMap cmap
  );

  static bool UseUnicodeAsAppleRomanCharCode(
    FT_Face face
  );

  static bool IsDamagedCharMap(
    FT_CharMap cmap
  );

private:
  static FT_MemoryRec_ m_memory_rec;

  static FT_Library m_library;

private:
#if defined(ON_RUNTIME_WIN)
  static ON_FreeTypeFace* Internal_CreateFaceFromWindowsFont(
    const LOGFONT* logfont
  );
#endif
#if defined (ON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE)
  static ON_FreeTypeFace* Internal_CreateFaceFromAppleFont(NSFont* aFont);
#endif
};

FT_MemoryRec_ ON_FreeType::m_memory_rec;
FT_Library ON_FreeType::m_library = nullptr;

static void* ON_Internal_FT_Alloc_Func(
  FT_Memory  memory,
  long size)
{
  void* ptr = onmalloc(size);
  return ptr;
}

void ON_Internal_FT_Free_Func(
  FT_Memory  memory,
  void* block
)
{
  onfree(block);
}

void* ON_InternalFT_Realloc_Func(
  FT_Memory  memory,
  long cur_size,
  long new_size,
  void* block
)
{
  void* ptr = onrealloc(block, new_size);
  return ptr;
}

FT_Library ON_FreeType::Library()
{
  if (nullptr == ON_FreeType::m_library)
  {
#if 1
    // The only reason a custom memory allocator is used
    // is so memory allocated by freetype is not flagged
    // as a leak because it is used in the cached fonts
    // ON_Font::ManagedFont(). These are created as needed
    // one per face and never deleted.
    memset(&ON_FreeType::m_memory_rec, 0, sizeof(ON_FreeType::m_memory_rec));
    ON_FreeType::m_memory_rec.user = nullptr;
    ON_FreeType::m_memory_rec.alloc = ON_Internal_FT_Alloc_Func;
    ON_FreeType::m_memory_rec.free = ON_Internal_FT_Free_Func;
    ON_FreeType::m_memory_rec.realloc = ON_InternalFT_Realloc_Func;

    FT_Library library = nullptr;
    int rc = FT_New_Library(&ON_FreeType::m_memory_rec, &library);
    if ( 0 == rc && nullptr != library )
      FT_Add_Default_Modules( library );
#else
    // Works fin except freetype cached information is flagged as a memory leak
    // int rc = FT_Init_FreeType(&library);
#endif

    if (0 != rc || nullptr == library)
    {
      ON_ERROR("FreeType FT_Init_FreeType() failed.");
    }
    else
    {
      ON_FreeType::m_library = library;
    }
  }
  return ON_FreeType::m_library;
}

bool ON_FreeType::IsDamagedCharMap(
  FT_CharMap cmap
)
{
  if (nullptr == cmap || nullptr == cmap->face)
    return true;
  
  bool rc = false;
  
  switch (cmap->encoding)
  {
  case FT_ENCODING_APPLE_ROMAN:
    rc = ON_FreeType::UseUnicodeAsAppleRomanCharCode(cmap->face);
    break;
  }

  return rc;
}

#define ON__FLAG_TO_STR(flag,i,v,s) do {if ( flag == (i & flag) ) {if (s.IsNotEmpty()) s += L", "; s += v; }} while(false)

const ON_wString ON_FreeType::FaceFlagsToString(FT_Long face_flags)
{
  ON_wString s;
  ON__FLAG_TO_STR(FT_FACE_FLAG_SCALABLE, face_flags, L"SCALABLE", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_FIXED_SIZES, face_flags, L"FIXED_SIZES", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_FIXED_WIDTH, face_flags, L"FIXED_WIDTH", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_SFNT, face_flags, L"SFNT", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_HORIZONTAL, face_flags, L"HORIZONTAL", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_VERTICAL, face_flags, L"VERTICAL", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_KERNING, face_flags, L"KERNING", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_FAST_GLYPHS, face_flags, L"FAST_GLYPHS", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_MULTIPLE_MASTERS, face_flags, L"MULTIPLE_MASTERS", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_GLYPH_NAMES, face_flags, L"GLYPH_NAMES", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_EXTERNAL_STREAM, face_flags, L"EXTERNAL_STREAM", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_HINTER, face_flags, L"FLAG_HINTER", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_CID_KEYED, face_flags, L"CID_KEYED", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_TRICKY, face_flags, L"FLAG_TRICKY", s);
  ON__FLAG_TO_STR(FT_FACE_FLAG_COLOR, face_flags, L"FLAG_COLOR", s);
  return s;
}

const ON_wString ON_FreeType::StyleFlagsToString(FT_Long style_flags)
{
  ON_wString s;
  ON__FLAG_TO_STR(FT_STYLE_FLAG_BOLD, style_flags, L"BOLD", s);
  ON__FLAG_TO_STR(FT_STYLE_FLAG_ITALIC, style_flags, L"ITALIC", s);
  return s;
}


bool ON_FreeType::UseUnicodeAsAppleRomanCharCode(FT_Face face)
{

  // It's not clear if the bug is in these font files or Windows or Freetype or
  // the way we are selecting maps, but the fonts listed below need this hack to
  // find the correct glyph and other fonts don't.
  //
  // The mighty internet contains mentions of other people having trouble with these fonts as well.
  //
  // It appears that for these fonts, passing a Unicode code point value
  // to the cmap[] idenfied as "FT_ENCODING_APPLE_ROMAN" will get the correct
  // glyph id. 
  //
  // I have verified that the way opennurbs handles FT_ENCODING_APPLE_ROMAN charmaps
  // and the funciton ON_MapUnicodeToAppleRoman() works correctly with all the
  // TTF fonts shipped with Windows 10 pro.
  //
  if (
    nullptr != face
    && 1 == face->num_faces
    && 3 == face->num_charmaps
    && nullptr != face->charmaps[0]
    && nullptr != face->charmaps[1]
    && nullptr != face->charmaps[2]
    && 0 == face->charmaps[0]->platform_id && 0 == face->charmaps[0]->encoding_id
    && 1 == face->charmaps[1]->platform_id && 0 == face->charmaps[1]->encoding_id
    && 3 == face->charmaps[2]->platform_id && 0 == face->charmaps[2]->encoding_id
    )
  {
    // May 2017 Dale Lear:
    //
    // Fonts we've found where FT_ENCODING_APPLE_ROMAN charmap does not work correctly
    // with Apple Roman char codes.
    //
    // Linguist's Software CityBlueprint 2.0 generated with Altsys Fontographer 4.1 9/17/96
    // Linguist's Software CountryBlueprint 2.0 generated with Altsys Fontographer 4.1 9/17/96
    // Linguist's Software Romantic 2.0 generated with Altsys Fontographer 4.1 9/17/96
    // Linguist's Software SansSerif 2.0 generated with Altsys Fontographer 4.1 9/17/96
    // Linguist's Software Technic 2.0 generated with Altsys Fontographer 4.1 9/17/96
    // 
    // So far, all styles with these face names have the buggy cmap[].
    if (ON_String::EqualOrdinal(face->family_name, "CityBlueprint", false))
      return true;
    if (ON_String::EqualOrdinal(face->family_name, "CountryBlueprint", false))
      return true;
    if (ON_String::EqualOrdinal(face->family_name, "Romantic", false))
      return true;
    if (ON_String::EqualOrdinal(face->family_name, "SansSerif", false))
      return true;
    if (ON_String::EqualOrdinal(face->family_name, "Technic", false))
      return true;
  }
  return false;
}

const ON_wString ON_FreeType::EncodingTypeToString( FT_Encoding charmap_encoding )
{
  ON_wString e;
  
  switch (charmap_encoding)
  {
  case FT_ENCODING_NONE:           e = L"FT_ENCODING_NONE"; break;
  case FT_ENCODING_UNICODE:        e = L"FT_ENCODING_UNICODE"; break;
  case FT_ENCODING_MS_SYMBOL:      e = L"FT_ENCODING_MS_SYMBOL"; break;
  case FT_ENCODING_ADOBE_LATIN_1:  e = L"FT_ENCODING_ADOBE_LATIN_1"; break;
  case FT_ENCODING_OLD_LATIN_2:    e = L"FT_ENCODING_OLD_LATIN_2"; break;
  case FT_ENCODING_SJIS:           e = L"FT_ENCODING_SJIS"; break;
  case FT_ENCODING_GB2312:         e = L"FT_ENCODING_GB2312"; break;
  case FT_ENCODING_BIG5:           e = L"FT_ENCODING_BIG5"; break;
  case FT_ENCODING_WANSUNG:        e = L"FT_ENCODING_WANSUNG"; break;
  case FT_ENCODING_JOHAB:          e = L"FT_ENCODING_JOHAB"; break;
  case FT_ENCODING_ADOBE_STANDARD: e = L"FT_ENCODING_ADOBE_STANDARD"; break;
  case FT_ENCODING_ADOBE_EXPERT:   e = L"FT_ENCODING_ADOBE_EXPERT"; break;
  case FT_ENCODING_ADOBE_CUSTOM:   e = L"FT_ENCODING_ADOBE_CUSTOM"; break;
  case FT_ENCODING_APPLE_ROMAN:    e = L"FT_ENCODING_APPLE_ROMAN"; break;
  default: 
    e = ON_wString::FormatToString(
      L"((FT_Encoding)%u)",
      static_cast<unsigned int>(charmap_encoding)
    );
    break;
  }
  
  return e;
}

const ON_wString ON_FreeType::CharmapPlatformEncodingDescription( const FT_CharMap cmap )
{
  // Reference
  // https://www.microsoft.com/typography/otspec/name.htm#enc3

  ON_wString platform;
  ON_wString encoding;
  switch (cmap->platform_id)
  {
  case 0: 
    platform = L"Unicode"; 
    switch (cmap->encoding_id)
    {
    case 0: encoding = L"Unicode 1.0 semantics [deprecated]"; break;
    case 1: encoding = L"Unicode 1.1 semantics [deprecated]"; break;
    case 2: encoding = L"ISO/IEC 10646 semantics [deprecated]"; break;
    case 3: encoding = L"Unicode 2.0+ semantics BMP only"; break;
    case 4: encoding = L"Unicode 2.0+ semantics"; break;
    case 5: encoding = L"Unicode Variation Sequences"; break;
    case 6: encoding = L"Unicode full repertoire"; break;
    }
    break;

  case 1: 
    platform = L"Apple Script Manager";
    switch (cmap->encoding_id)
    {
    case  0: encoding = L"Roman"; break;
    case  1: encoding = L"Japanese"; break;
    case  2: encoding = L"Chinese (Traditional)"; break;
    case  3: encoding = L"Korean"; break;
    case  4: encoding = L"Arabic"; break;
    case  5: encoding = L"Hebrew"; break;
    case  6: encoding = L"Greek"; break;
    case  7: encoding = L"Russian"; break;
    case  8: encoding = L"RSymbol"; break;
    case  9: encoding = L"Devanagari"; break;
    case 10: encoding = L"Gurmukhi"; break;
    case 11: encoding = L"Gujarati"; break;
    case 12: encoding = L"Oriya"; break;
    case 13: encoding = L"Bengali"; break;
    case 14: encoding = L"Tamil"; break;
    case 15: encoding = L"Telugu"; break;
    case 16: encoding = L"Kannada"; break;
    case 17: encoding = L"Malayalam"; break;
    case 18: encoding = L"Sinhalese"; break;
    case 19: encoding = L"Burmese"; break;
    case 20: encoding = L"Khmer"; break;
    case 21: encoding = L"Thai"; break;
    case 22: encoding = L"Laotian"; break;
    case 23: encoding = L"Georgian"; break;
    case 24: encoding = L"Armenian"; break;
    case 25: encoding = L"Chinese (Simplified)"; break;
    case 26: encoding = L"Tibetan"; break;
    case 27: encoding = L"Mongolian"; break;
    case 28: encoding = L"Geez"; break;
    case 29: encoding = L"Slavic"; break;
    case 30: encoding = L"Vietnamese"; break;
    case 31: encoding = L"Sindhi"; break;
    case 32: encoding = L"Uninterpreted"; break;
    }
    break;

  case 2:
    platform = L"ISO [deprecated]";
    switch (cmap->encoding_id)
    {
    case 0: encoding = L"7-bit ASCII"; break;
    case 1: encoding = L"ISO 10646"; break;
    case 2: encoding = L"ISO 8859-1"; break;
    }
    break;

  case 3:
    platform = L"Windows"; 
    switch (cmap->encoding_id)
    {
    case  0: encoding = L"Symbol"; break;
    case  1: encoding = L"Unicode BMP UCS-2"; break;
    case  2: encoding = L"ShiftJIS"; break;
    case  3: encoding = L"PRC"; break;
    case  4: encoding = L"Big5"; break;
    case  5: encoding = L"Wansung"; break;
    case  6: encoding = L"Johab"; break;
    case 10: encoding = L"Unicode UCS-4"; break;
    }
    break;
  }


  const ON_wString e = ON_FreeType::EncodingTypeToString(cmap->encoding);

  ON_wString s = ON_wString::FormatToString(
    L"%ls %d-%d",
    static_cast<const wchar_t*>(e),
    cmap->platform_id,
    cmap->encoding_id
  );

  if (platform.IsNotEmpty())
  {
    if (encoding.IsEmpty())
      encoding = L"unknown";
    s += ON_wString::FormatToString(
      L" (%ls %ls)",
      static_cast<const wchar_t*>(platform),
      static_cast<const wchar_t*>(encoding)
    );
  }

  return s;
}

bool ON_FontGlyph::TestFaceCharMaps(
  ON_TextLog* text_log
) const
{
  // In order for false to be returned, charmaps[] have to exist and 
  // an explicit error has to be detected. Otherwise, true is returned.
  const ON_Font* font = Font();
  if (nullptr == font)
  {
    if (text_log)
      text_log->Print("Font() = nullptr. Nothing to test.\n");
    return true; // nothing to test.
  }

  if (false == font->IsManagedFont())
  {
    // this "should" never happen.
    if (text_log)
      text_log->Print("Font().IsManagedFont() = false. Nothing to test.\n");
    return true; // nothing to test.
  }

  const ON__UINT32 unicode_code_point = CodePoint();
  if (false == ON_IsValidUnicodeCodePoint(unicode_code_point))
  {
    if (text_log)
      text_log->Print("CodePoint() is not valid. Nothing to test.\n");
    return true; // nothing to test.
  }

  FT_Face face = reinterpret_cast<FT_Face>(ON_Font::FreeTypeFace(font));
  if (nullptr == face)
  {
    if (text_log)
      text_log->Print("Face is nullptr. Nothing to test.\n");
    return true; // nothing to test.
  }

  const unsigned int glyph_id = static_cast<unsigned int>(FontGlyphId());
  if ( 0 == glyph_id )
  {
    if (text_log)
      text_log->Print("FontGlyphId is 0. Nothing to test.\n");
    return true; // nothing to test.
  }

  // Save current face charmap state
  FT_CharMap charmap0 = face->charmap;

  bool rc = true;
  for (int charmap_index = 0; charmap_index < face->num_charmaps; charmap_index++)
  {
    FT_CharMap charmap = face->charmaps[charmap_index];
    if (nullptr == charmap)
      continue;


    bool bHaveCharMap = false;
    bool bHaveCharCode = false;
    bool bBuggyMap = false;
    bool bUnicode = false;
    unsigned int char_code = 0xFFFFFFFF;
    unsigned int gid = 0;

    const ON_wString e = ON_FreeType::CharmapPlatformEncodingDescription(charmap);

    switch (charmap->encoding)
    {
    case FT_ENCODING_UNICODE:
      char_code = unicode_code_point;
      bHaveCharCode = true;
      bUnicode = true;
      break;

    case FT_ENCODING_APPLE_ROMAN:
      bBuggyMap = ON_FreeType::UseUnicodeAsAppleRomanCharCode(face);
      if ( bBuggyMap )
      {
        bHaveCharCode = unicode_code_point <= 0xFF;
        if (bHaveCharCode)
        {
          bUnicode = true;
          char_code = unicode_code_point;
        }
      }
      else
      {
        // Microsoft code page 10000 = Apple Roman encoding
        char_code = ON_MapUnicodeToMSSBCP(10000, unicode_code_point);
        bHaveCharCode = (char_code <= 0xFF);
      }
      break;

    case FT_ENCODING_MS_SYMBOL:
      bHaveCharCode = true;
      char_code = unicode_code_point;
      break;

    default:
      break;
    }

    if (bHaveCharCode)
    {
      bHaveCharMap 
        = FT_Err_Ok == FT_Set_Charmap(face, charmap)
        && charmap == face->charmap;
      if (bHaveCharMap)
      {
        gid = FT_Get_Char_Index(face, char_code);
        if (glyph_id != gid)
          rc = false;
      }
      else
        rc = false;
    }

    if (text_log)
    {
      const wchar_t* damaged = ( bBuggyMap || ON_FreeType::IsDamagedCharMap(charmap) )? L"DAMAGED " : L"";
      ON_wString s = ON_wString::FormatToString(
        L"%lscmap[%d] %ls",
        damaged,
        charmap_index,
        static_cast<const wchar_t*>(e)
      );

      s += ON_wString::FormatToString(L" U+%04X", unicode_code_point);
      if (false == bHaveCharCode)
        s += L" (no char code)";
      if (false == bUnicode || bBuggyMap || char_code != unicode_code_point)
        s += ON_wString::FormatToString(L" -> char code 0x%X (%u)", char_code, char_code);

      if (false == bHaveCharMap)
      {
        s += L" ERROR(cannot use cmap)";
      }
      else if (0 == gid)
      {
        s += ON_wString::FormatToString(
          L" no glpyh",
          char_code
        );
      }
      else
      {
        s += ON_wString::FormatToString(
          L" -> glyph id %u",
          gid
        );
        if (glyph_id != gid)
        {
          s += ON_wString::FormatToString(L"ERROR(expected glyph id %u)",glyph_id);
        }                
      }
      text_log->Print(L"%ls\n", static_cast<const wchar_t*>(s));
    }
  }

  // restore face charmap state
  FT_Set_Charmap(face, charmap0);
  
  return rc;
}

unsigned int ON_FreeType::GlyphId(
    FT_Face face,
    ON__UINT32 unicode_code_point
  )
{
  if (nullptr == face)
    return 0;

  const FT_CharMap charmap0 = face->charmap;
  if (nullptr == charmap0 || FT_ENCODING_UNICODE != charmap0->encoding)
  {
    if (FT_Err_Ok != FT_Select_Charmap(face, FT_ENCODING_UNICODE))
      FT_Set_Charmap(face, charmap0);
  }

  FT_CharMap charmap1 = face->charmap;
  if (nullptr != charmap1 && FT_ENCODING_UNICODE == charmap1->encoding)
  {
    unsigned int glyph_id = FT_Get_Char_Index(face, unicode_code_point);
    if (0 != glyph_id)
    {
      // commonly happens for well designed modern fonts.
      return glyph_id;
    }
  }
  else
  {
    charmap1 = nullptr;
  }

  // May 2017 Dale Lear
  //
  // In some fonts, there are multiple FT_ENCODING_UNICODE charmaps
  // and they can map different unicode code points. These typically are
  // Windows UCS-2 and Windows UCS-4 cmaps. UCS-2 and UCS-4 values subsets of Unicode.
  //
  // In fonts like CityBlueprint and CountryBlueprint (which many customers use), there 
  // is no FT_ENCODING_UNICODE charmap but there is a viable FT_ENCODING_APPLE_ROMAN charmap.
  //
  // As we discover fonts that customers use, we will add support for their charmaps.
  //
  // TrueType platform_id and encoding_id. The encoding id is platform specific.
  // platform_id-encoding_id 
  //
  //  0-* "Apple *code" - encoding id varies 
  //
  //  1-0 Apple Roman (256 codes - see ON_MapAppleRomanToUnicode())
  //  1-* Apple (encoding id = script manager)
  //
  //  2-* ISO (encoding id = ISO encoding)
  //
  //  3-0 Windows Symbol
  //  3-1 Wnidows UCS-2 (subset of Unicode)
  //  3-2 Windows ShiftJIS
  //  3-3 Windows Big5
  //  3-4 WIndows PRC
  //  3-5 Windows Wansung
  //  3-6 Windows Johab
  //  3-10 Wnidows UCS-4 (subset of Unicode)
  //
  //  4-* Custom
  //
  // http://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=iws-chapter08
  // 
  const FT_Encoding encoding_pass[]
  {
    FT_ENCODING_UNICODE,
    FT_ENCODING_APPLE_ROMAN,
    FT_ENCODING_NONE // FT_ENCODING_NONE must terminate the array
  };

  const size_t pass_count = sizeof(encoding_pass) / sizeof(encoding_pass[0]);

  for (size_t pass = 0; pass < pass_count; pass++)
  {
    if (pass+1 >= pass_count && unicode_code_point >= 0x80)
    {
      // As a last gasp pass, when the unicode_code_point < 0x80, we try every charmap
      // because using the "ASCII encoding" for those code points was farily common
      // in old fonts and the charmap identifion may be incorrect.
      break;
    }

    // Often, ft_face has multiple FT_ENCODING_UNICODE charmaps and
    // sometimes FT_Select_Charmap() sometimes selects the wrong one
    // when we want a glyph for a specific unicode code point.
    const FT_Encoding e = encoding_pass[pass];
    for ( int charmap_index = 0; charmap_index < face->num_charmaps; charmap_index++)
    {
      const FT_CharMap charmap = face->charmaps[charmap_index];
      if (nullptr == charmap)
        continue;

      if (charmap1 == charmap)
        continue; // charmap1 was already tested.

      if (FT_ENCODING_NONE != e)
      {
        if ( e != charmap->encoding)
         continue; // wrong encoding for this pass
      }
      else
      {
        if (FT_ENCODING_UNICODE == charmap->encoding)
          continue; // already tested
      }

      if (FT_Err_Ok != FT_Set_Charmap(face, charmap))
        continue;
      if (charmap != face->charmap)
        continue;

      unsigned int charcode = 0xFFFFFFFF;
      switch (e)
      {
      case FT_ENCODING_APPLE_ROMAN:
        // Required to get CityBlueprint and CountryBlueprint fonts to work correctly.
        if (ON_FreeType::UseUnicodeAsAppleRomanCharCode(face))
        {
          // Buggy font.
          // The FT_ENCODING_APPLE_ROMAN encoding in these fonts is really a unicode encoding.
          charcode = unicode_code_point;
        }
        else
        {
          // Microsoft code page 10000 = Apple Roman
          // Single byte encoding
          charcode = ON_MapUnicodeToMSSBCP(10000, unicode_code_point);
          if (charcode > 0xFF)
            continue; // no mapping from Unicode to Apple Roman
        }
        break;

      case FT_ENCODING_UNICODE:
        charcode = unicode_code_point;
        break;

      default:
        // This is risky but might work when unicode_code_point < 0x80
        charcode = unicode_code_point;
        break;
      }

      // see if the glyph is in this char map
      unsigned int glyph_id = FT_Get_Char_Index(face, charcode);
      if (0 == glyph_id)
        continue;

      return glyph_id;
    }
  }

  // No glpyh for this unicode code point is available.
  FT_Set_Charmap(face, charmap0);

  return 0;
}




#if defined(ON_RUNTIME_WIN)

#if 0
class ON_WindowsFontFamily
{
public:
  ON_WindowsFontFamily();
  ~ON_WindowsFontFamily() = default;
  ON_WindowsFontFamily(const ON_WindowsFontFamily&) = default;
  ON_WindowsFontFamily& operator=(const ON_WindowsFontFamily&) = default;

  bool IsTrueTypeFontType() const;
  bool IsRasterFontType() const;
  bool IsVectorFontType() const;
  bool IsDeviceFontType() const;

public:
  LOGFONT m_logfont;
  NEWTEXTMETRIC m_text_metric;

  // m_font_type can be a bitwise or of the values
  // DEVICE_FONTTYPE
  // RASTER_FONTTYPE
  // TRUETYPE_FONTTYPE
  DWORD m_font_type = 0; // value from ::EnumFontFamilies

  // Note:
  // Sone font files contain multiple font faces.
  ON_wString m_font_file_path;
  unsigned int m_font_file_face_index = 0;
};

bool ON_WindowsFontFamily::IsTrueTypeFontType() const
{
  // If the TRUETYPE_FONTTYPE bit is set, the font is a TrueType font. 
  // An application can also check bits 1 and 2 in the tmPitchAndFamily 
  // member of the NEWTEXTMETRIC structure to identify a TrueType font.
  // If bit 1 is 0 and bit 2 is 1, the font is a TrueType font.
  return (0 != (m_font_type & TRUETYPE_FONTTYPE)) ? true : false;
}

bool ON_WindowsFontFamily::IsRasterFontType() const
{
  // If the RASTER_FONTTYPE bit is set, the font is a raster font. 
  return (0 != (m_font_type & RASTER_FONTTYPE)) ? true : false;
}

bool ON_WindowsFontFamily::IsVectorFontType() const
{
  // If neither the RASTER_FONTTYPE bit nor the TRUETYPE_FONTTYPE bit is set, the font is a vector font.
  // Vector fonts are categorized as OEM_CHARSET instead of ANSI_CHARSET. 
  // Some applications identify vector fonts by using this information, 
  // checking the tmCharSet member of the NEWTEXTMETRIC structure. 
  // This categorization usually prevents the font mapper from choosing
  // vector fonts unless they are specifically requested. 
  // (Most applications no longer use vector fonts because their
  // strokes are single lines and they take longer to draw than 
  // TrueType fonts, which offer many of the same scaling and 
  // rotation features that required vector fonts.)
  return (0 == (m_font_type & (RASTER_FONTTYPE|TRUETYPE_FONTTYPE))) ? true : false;
}

bool ON_WindowsFontFamily::IsDeviceFontType() const
{
  // The "device" mentioned below depends on the HDC passed to EnumFontFamilies().
  //
  // A third mask, DEVICE_FONTTYPE, is set when a device (for example, a laser printer)
  // supports downloading TrueType fonts; it is zero if the device is a display adapter,
  // dot-matrix printer, or other raster device. 
  //
  // An application can also use the DEVICE_FONTTYPE mask to distinguish GDI-supplied
  // raster fonts from device-supplied fonts.
  //
  // The system can simulate bold, italic, underline, and strikeout attributes for 
  // GDI-supplied raster fonts, but not for device-supplied fonts.
  return (0 == (m_font_type & DEVICE_FONTTYPE)) ? true : false;
}

ON_WindowsFontFamily::ON_WindowsFontFamily()
{
  memset(&m_logfont, 0, sizeof(m_logfont));
  memset(&m_text_metric, 0, sizeof(m_text_metric));
}

static int CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID a) 
{ 
  ON_WindowsFontFamily& f = ((ON_ClassArray<ON_WindowsFontFamily>*)a)->AppendNew();
  if (nullptr != lplf)
    f.m_logfont = *lplf;
  if (nullptr != lpntm)
    f.m_text_metric = *lpntm;
  f.m_font_type = FontType;
  return 1; // 0 = stop enumeration, nonzero = continue enumeration
}

class ON_FontRegistryNameAndFilePath
{
public:
  ON_FontRegistryNameAndFilePath() = default;
  ~ON_FontRegistryNameAndFilePath() = default;
  ON_FontRegistryNameAndFilePath(const ON_FontRegistryNameAndFilePath&) = default;
  ON_FontRegistryNameAndFilePath& operator=(const ON_FontRegistryNameAndFilePath&) = default;

public:
  // Note: A single file can contain multiplle faces. For example
  // m_registry_name = "Sitka Small Bold Italic & Sitka Text Bold Italic & Sitka Subheading Bold Italic & Sitka Heading Bold Italic & Sitka Display Bold Italic & Sitka Banner Bold Italic (TrueType)"
  // m_font_file_path = "C:\\Windows\\Fonts\\SitkaZ.ttc"
  // Contains 6 faces.
  ON_wString m_registry_name;
  ON_wString m_font_file_path;
  ON_wString m_font_face_name;
  unsigned int m_font_face_index = 0;
};

static unsigned int Internal_GetWindowsFontFamilyList(
  HDC hdc,
  ON_ClassArray<ON_WindowsFontFamily>& font_families
)
{
  // Enumerate Windows fonts.
  bool bCallDeleteDC = false;
  if (nullptr == hdc)
  {
    hdc = ::CreateCompatibleDC(nullptr); // "screen" device context
    bCallDeleteDC = true;
  }
  LPCTSTR lpszFamily = nullptr;
  ::EnumFontFamilies(
    hdc,
    lpszFamily,
    (FONTENUMPROC)EnumFamCallBack,
    (LPARAM)((UINT_PTR)(&font_families))
  );
  if ( bCallDeleteDC )
    ::DeleteDC(hdc);

  return font_families.UnsignedCount();
}

const wchar_t* Internal_GetNextFontFaceNameFromRegistryData(
  const wchar_t* s,
  ON_wString& font_face_name
)
{
  if (0 == s)
    return nullptr;
  const wchar_t* s1 = s;
  while (0 != *s1 && '&' != *s1 && '(' != *s1)
  {
    s1++;
  }
  if (s1 <= s)
    return nullptr;
  const size_t length = s1 - s;
  if (length <= 0 || length > 0xFFFFFFF)
    return nullptr;
  ON_wString buffer(s, (int)length);
  buffer.TrimLeftAndRight();
  if (buffer.IsEmpty())
    return nullptr;
  font_face_name = buffer;
  if ('&' == *s1)
  {
    s1++;
  }
  else if ('(' == *s1)
  {
    s1++;
    for ( int paren_level = 1; 0 != *s1 && paren_level > 0; s1++ )
    {
      if ('(' == *s1)
        paren_level++;
      else if (')' == *s1)
        paren_level--;
    }
  }
  return s1;
}

static unsigned int Internal_GetWindowsRegistryFontist(
  ON_ClassArray<ON_FontRegistryNameAndFilePath>& registry_font_list
)
{
  // Enumerates the font file paths listed in the registry.
  const wchar_t* regkey_names[] =
  {
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
    //L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts",
    nullptr
  };

  // Enumerates the font file paths listed in the registry.

  /*
PItemIDList PIDL;

SHGetSpecialFolderLocation(Application.Handle, CSIDL_FONTS, PIDL);

In this case, the 1st parameter is you application main handle, the
second is a constant defined in the Windows headers telling it what path
you're looking for, and the last is a pointer that will carry the return
value.  Here's a code snippet in Delphi from my application.  Note that
you have to free some memory when you're done:

  Path := StrAlloc(MAX_PATH);
  SHGetSpecialFolderLocation(Application.Handle, CSIDL_FONTS, PIDL);
  if SHGetPathFromIDList(PIDL, Path) then
    CurrWindowsFontPath := Path;
  SHGetMalloc(AMalloc);
  AMalloc.Free(PIDL);
  StrDispose(Path);

  // Make sure the path has a trailing directory separator
  CurrWindowsFontPath :=
IncludeTrailingPathDelimiter(CurrWindowsFontPath);

  */
  const wchar_t* font_directories[] =
  {
    // System fonts
    L"C:\\Windows\\Fonts\\",

    // The empty string is needed to find fonts like
    // registry value name: "MecSoft_Font-1 (TrueType)"
    // registry string: "C:\\Program Files\\Rhino WIP\\System\\MecSoft_Font-1.ttf"
    L"",
    nullptr,
    nullptr,
    nullptr,
  };

  for (int key_index = 0; nullptr != regkey_names[key_index]; key_index++)
  {
    ON_RegKey regkey;
    if (false == regkey.OpenRead(regkey_names[key_index]))
      continue;
    ON_ClassArray<ON_wString> value_names;
    if (false == regkey.GetValueNames(value_names))
      continue;
    for (int value_index = 0; value_index < value_names.Count(); value_index++)
    {
      const wchar_t* value_name = static_cast<const wchar_t*>(value_names[value_index]);
      if (nullptr == value_name || 0 == value_name[0])
        continue;
      ON_wString font_file_name;
      if (false == regkey.QueryValue(value_name, font_file_name))
        continue;
      if (font_file_name.IsEmpty())
        continue;
      bool bFontFileFound = false;
      for (int font_dir_index = 0; nullptr != font_directories[font_dir_index]; font_dir_index++)
      {
        ON_wString font_file_path = font_directories[font_dir_index];
        font_file_path += font_file_name;
        if (false == ON_FileSystem::IsFile(font_file_path))
          continue;
        bFontFileFound = true;
        // Note: A single file can contain multiple font faces
        ON_FontRegistryNameAndFilePath sp;
        sp.m_registry_name = value_names[value_index];
        sp.m_font_file_path = font_file_path;
        sp.m_font_face_index = 0;
        for (
          const wchar_t* s = static_cast<const wchar_t*>(sp.m_registry_name);
          nullptr != (s = Internal_GetNextFontFaceNameFromRegistryData(s, sp.m_font_face_name));
          sp.m_font_face_index++
          )
        {
          if (sp.m_font_face_name.IsNotEmpty())
          {
            registry_font_list.Append(sp);
            sp.m_font_face_name = ON_wString::EmptyString;
          }
        }
        break;
      }
      if (false == bFontFileFound)
      {
        ON_ERROR("Unable to locate Windows font file.");
      }
    }
  }

  return registry_font_list.UnsignedCount();
}

#endif
class CDirectWriteStackCleaner
{
public:
  ~CDirectWriteStackCleaner()
  {
    if (m_font_file_stream)
      m_font_file_stream->Release();
    if (m_font_file_loader)
      m_font_file_loader->Release();
    if (m_font_file)
      m_font_file->Release();
    if (m_font_face)
      m_font_face->Release();
    if (m_font)
      m_font->Release();
    if (m_font_family)
      m_font_family->Release();
    if (m_font_collection)
      m_font_collection->Release();
    if (m_factory)
      m_factory->Release();
  }
public:
  IDWriteFactory* m_factory = nullptr;
  IDWriteFontCollection* m_font_collection = nullptr;
  IDWriteFontFamily* m_font_family = nullptr;
  IDWriteFont* m_font = nullptr;
  IDWriteFontFace* m_font_face = nullptr;
  IDWriteFontFile* m_font_file = nullptr;
  IDWriteFontFileLoader* m_font_file_loader = nullptr;
  IDWriteFontFileStream* m_font_file_stream = nullptr;
};

static DWRITE_FONT_WEIGHT Internal_DWFontWeightFromLogfont(
  const LOGFONT& logfont
)
{
  const DWRITE_FONT_WEIGHT a[]
    = {
    DWRITE_FONT_WEIGHT_THIN, // = 100,
    DWRITE_FONT_WEIGHT_EXTRA_LIGHT, // = 200,
    DWRITE_FONT_WEIGHT_ULTRA_LIGHT, // = 200,
    DWRITE_FONT_WEIGHT_LIGHT, // = 300,
    DWRITE_FONT_WEIGHT_SEMI_LIGHT, // = 350,
    DWRITE_FONT_WEIGHT_NORMAL, // = 400,
    DWRITE_FONT_WEIGHT_REGULAR, // = 400,
    DWRITE_FONT_WEIGHT_MEDIUM, // = 500,
    DWRITE_FONT_WEIGHT_DEMI_BOLD, // = 600,
    DWRITE_FONT_WEIGHT_SEMI_BOLD, // = 600,
    DWRITE_FONT_WEIGHT_BOLD, // = 700,
    DWRITE_FONT_WEIGHT_EXTRA_BOLD, // = 800,
    DWRITE_FONT_WEIGHT_ULTRA_BOLD, // = 800,
    DWRITE_FONT_WEIGHT_BLACK, // = 900,
    DWRITE_FONT_WEIGHT_HEAVY, // = 900,
    DWRITE_FONT_WEIGHT_EXTRA_BLACK, // = 950,
    DWRITE_FONT_WEIGHT_ULTRA_BLACK // = 950
  };

  const size_t a_count = sizeof(a) / sizeof(a[0]);

  if (logfont.lfWeight <= static_cast<int>(a[0]))
    return a[0];

  if (logfont.lfWeight >= static_cast<int>(a[a_count-1]))
    return a[a_count-1];

  for (size_t i = 1; i < a_count; i++)
  {
    const int w1 = static_cast<int>(a[i]);
    if ( logfont.lfWeight > w1 )
      continue;
    const int w0 = static_cast<int>(a[i - 1]);
    if (2 * logfont.lfWeight <= (w0 + w1))
      return (a[i-1] );
    return (a[i] );
  }

  return DWRITE_FONT_WEIGHT_REGULAR;
}

static bool Internal_CreateFontBufferFromDirectWrite(
  const LOGFONT& logfont,
  ON_FontFileBuffer& font_buffer
)
{
  font_buffer.AllocateBuffer(0);

  CDirectWriteStackCleaner dwrite;
  UINT64 filesize = 0;
  //create the factory - this should probably be cached
  HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwrite.m_factory));
  if (FAILED(hr))
    return false;
  if (nullptr == dwrite.m_factory)
    return false;

  //fonts on this computer
  hr = dwrite.m_factory->GetSystemFontCollection(&dwrite.m_font_collection, true);
  if (FAILED(hr))
    return false;
  if (nullptr == dwrite.m_font_collection)
    return false;

  UINT32 index = 0;
  int bExists = 0;
  hr = dwrite.m_font_collection->FindFamilyName(logfont.lfFaceName, &index, &bExists);
  if (FAILED(hr))
    return false;
  if (bExists == false)
    return false;

  hr = dwrite.m_font_collection->GetFontFamily(index, &dwrite.m_font_family);
  if (FAILED(hr))
    return false;
  if (nullptr == dwrite.m_font_family)
    return false;

  const DWRITE_FONT_WEIGHT weight = Internal_DWFontWeightFromLogfont(logfont);
  const DWRITE_FONT_STYLE style 
    = (0 != logfont.lfItalic)
    ? DWRITE_FONT_STYLE_ITALIC
    : DWRITE_FONT_STYLE_NORMAL;
  hr = dwrite.m_font_family->GetFirstMatchingFont(weight, DWRITE_FONT_STRETCH_NORMAL, style, &dwrite.m_font);
  if (FAILED(hr))
    return false;
  if (nullptr == dwrite.m_font)
    return false;

  hr = dwrite.m_font->CreateFontFace(&dwrite.m_font_face);
  if (FAILED(hr))
    return false;
  if (nullptr == dwrite.m_font_face)
    return false;

  // The docs state that this function should be called twice; first to obtain
  // the number of files. In the context of FreeType, I'm not sure what to do
  // with multiple files.
  UINT32 numfiles = 1;
  hr = dwrite.m_font_face->GetFiles(&numfiles, &dwrite.m_font_file);
  if (FAILED(hr))
    return false;
  if (nullptr == dwrite.m_font_file)
    return false;

  const void* reference_key = nullptr;
  UINT32 reference_key_size = 0;
  hr = dwrite.m_font_file->GetReferenceKey(&reference_key, &reference_key_size);
  if (FAILED(hr))
    return false;

  hr = dwrite.m_font_file->GetLoader(&dwrite.m_font_file_loader);
  if (FAILED(hr))
    return false;
  if (nullptr == dwrite.m_font_file_loader)
    return false;

  hr = dwrite.m_font_file_loader->CreateStreamFromKey(reference_key, reference_key_size, &dwrite.m_font_file_stream);
  if (FAILED(hr))
    return false;

  hr = dwrite.m_font_file_stream->GetFileSize(&filesize);
  if (FAILED(hr))
    return false;

  const void *fragstart = nullptr;
  void *fragcontext = nullptr;
  hr = dwrite.m_font_file_stream->ReadFileFragment(&fragstart, 0, filesize, &fragcontext);
  if (FAILED(hr))
    return false;

  if (filesize > 0)
  {
    void* buffer = font_buffer.AllocateBuffer((size_t)filesize);
    if ( nullptr != buffer )
      memcpy(buffer, fragstart, font_buffer.SizeOfBuffer());
  }
  dwrite.m_font_file_stream->ReleaseFileFragment(fragcontext);

  return (font_buffer.SizeOfBuffer() > 0);
}

static bool Internal_CreateFontBufferFromGDI(
  const LOGFONT& logfont,
  ON_FontFileBuffer& font_buffer
)
{
  font_buffer.AllocateBuffer(0);

  HDC hdc = nullptr;
  HFONT hfont = nullptr;
  HGDIOBJ hfont_original = nullptr;
  bool rc = false;
  for (;;)
  {
    hfont = ::CreateFontIndirect(&logfont);
    if (0 == hfont)
    {
      ON_ERROR("CreateFontIndirect failed.");
      break;
    }

    hdc = ::CreateCompatibleDC(nullptr);
    if (nullptr == hdc)
    {
      ON_ERROR("CreateCompatibleDC(nullptr) failed.");
      break;
    }

    hfont_original = ::SelectObject(hdc, hfont);
    if (nullptr == hfont_original)
    {
      ON_ERROR("SelectObject(hdc, hfont) failed.");
      break;
    }

    //const DWORD dwTable_TrueTypeCollection = 0x66637474;
    DWORD  dwTable = 0;
    DWORD  dwOffset = 0;
    const DWORD buffer_capacity = ::GetFontData(hdc, dwTable, dwOffset, nullptr, 0);
    if (buffer_capacity <= 0)
    {
      ON_ERROR("GetFontData(...,nullptr,0) failed.");
      break;
    }

    void* buffer = font_buffer.AllocateBuffer(buffer_capacity);
    if (nullptr == buffer)
    {
      ON_ERROR("onmalloc(buffer_capacity) failed.");
      break;
    }    
    memset(buffer, 0, buffer_capacity);

    const DWORD buffer_size = ::GetFontData(hdc, dwTable, dwOffset, buffer, buffer_capacity);
    if ( buffer_size != buffer_capacity )
    {
      ON_ERROR("GetFontData(...,nullptr,0) failed.");
      break;
    }

    rc = true;
    break;
  }

  if (false == rc)
  {
    font_buffer.AllocateBuffer(0);
  }

  if (nullptr != hdc)
  {
    if (nullptr != hfont_original)
      ::SelectObject(hdc, hfont_original);
    ::DeleteDC(hdc);
  }

  if (nullptr != hfont)
  {
    ::DeleteObject(hfont);
  }

  return (font_buffer.SizeOfBuffer() > 0);
}

ON_FreeTypeFace* ON_FreeType::Internal_CreateFaceFromWindowsFont(
  const LOGFONT* logfont
)
{
  if (nullptr == logfont)
    return nullptr;

  FT_Library freetype_library = ON_FreeType::Library();
  if (nullptr == freetype_library)
    return nullptr;
  for (int pass = 1; pass <= 2; pass++)
  {
    ON_FontFileBuffer font_buffer;

    // May 2015 Dale Lear
    //  The DirectWrite approach yields better results in some cases.
    //  For example, the Yu Gothic in Windows 10
    //    The font_buffer created by DirectWrite results in an FT_Face
    //    with 3 charmaps (2 Uniocde) and the font_buffer created by 
    //    GDI has 0 charmaps which means getting a glyph from a code point
    //    is not possible.
    const bool bHaveBuffer
      = (1 == pass)
      ? Internal_CreateFontBufferFromDirectWrite(*logfont, font_buffer)
      : Internal_CreateFontBufferFromGDI(*logfont, font_buffer);

    if (false == bHaveBuffer)
      continue;
    if (font_buffer.SizeOfBuffer() <= 0)
      continue;
    if (nullptr == font_buffer.Buffer())
      continue;
    
    int font_face_index = 0;
    FT_Face face = nullptr;
    FT_Error rc = FT_New_Memory_Face(
      freetype_library,
      reinterpret_cast<const FT_Byte*>(font_buffer.Buffer()),
      (FT_Long)font_buffer.SizeOfBuffer(),
      font_face_index,
      &face
    );

    if (nullptr == face)
      continue;

    if (FT_Err_Ok != rc)
    {
      FT_Done_Face(face);
      continue;
    }

    ON_FreeTypeFace* f = new ON_FreeTypeFace();

    f->m_face = face;
    font_buffer.TransferTo(f->m_font_buffer);

    return f;
  }

  return nullptr;
}

#endif


#if defined (ON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE)

ON_FreeTypeFace* ON_FreeType::Internal_CreateFaceFromAppleFont (NSFont* font)
{
  if (font == nullptr)
    return nullptr;
  
  // determine file path for NSFont
  CTFontRef fontRef = (__bridge CTFontRef) font;
  CFURLRef fontURLRef = (CFURLRef) CTFontCopyAttribute (fontRef, kCTFontURLAttribute);
  
  NSURL* fontURL = (NSURL*) CFBridgingRelease (fontURLRef);
  const char* path = fontURL.path.UTF8String;
  
  // Search all the faces in this font file for a face that matches the NSFont family and style
  FT_Face ftFace;
  FT_Error err = FT_New_Face (ON_FreeType::Library(), path, 0, &ftFace);    // get first face
  if (err)
    return nullptr;     // that didn't work
  
  const int numFaces = (int)ftFace->num_faces; // determine number of faces in the font file

  ON_FreeTypeFace* rc = new ON_FreeTypeFace();
  if (numFaces <= 1)
  {
    rc->m_face = ftFace;
    return rc;       // only one face, so this must be the right one
  }
  
  int faceIndex = 0;
  for (;;)
  {
    NSString* ftFamilyName = [NSString stringWithUTF8String: ftFace->family_name];
    NSString* ftStyleName = [NSString stringWithUTF8String: ftFace->style_name];
    NSString* nsStyleName = [font.fontDescriptor objectForKey: NSFontFaceAttribute];
    if ([font.familyName isEqualToString: ftFamilyName] && [nsStyleName isEqualToString: ftStyleName])
    {
      rc->m_face = ftFace;
      return rc;
    }
    
    // No match.  Step to next face.
    FT_Done_Face (ftFace);
    
    FT_Error err = FT_New_Face (ON_FreeType::Library(), path, ++faceIndex, &ftFace);
    if (ftFace == nullptr || err || faceIndex >= numFaces) {
      // Ran out of faces to inspect or FT_New_Face returned an error.
      FT_Done_Face (ftFace);
      break;
    }
  }
  
  // When no match found, use first face in font file as the default face.
  FT_New_Face (ON_FreeType::Library(), path, 0, &ftFace);    // get first face

  rc->m_face = ftFace;
  return rc;
}

#endif    // ON_RUNTIME_APPLE


ON_FreeTypeFace* ON_FreeType::CreateFace(
  const ON_Font& font
)
{
  ON_FreeTypeFace* f = nullptr;

  if (false == font.IsManagedFont())
  {
    // Managed fonts have valid settings.
    return nullptr;
  }

#if defined(ON_RUNTIME_WIN)
  LOGFONT logfont = font.WindowsLogFont(0,nullptr);
  f = ON_FreeType::Internal_CreateFaceFromWindowsFont(&logfont);

#elif defined (ON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE)
  NSFont* nsFont = font.AppleFont();
  f = ON_FreeType::Internal_CreateFaceFromAppleFont(nsFont);
#endif

  // Create empty holder so this function doesn't perform the same calculations
  // over and over when a freetype face can not be loaded.
  if (nullptr == f)
    f = new ON_FreeTypeFace();

  return f;
}

ON__UINT_PTR ON_Font::FreeTypeFace(
  const ON_Font* font
)
{
  if (nullptr == font)
    return 0;
  const ON_Font* managed_font = font->ManagedFont();
  if (nullptr == managed_font)
    return 0;

  if (0 == managed_font->m_free_type_face)
  {
    managed_font->m_free_type_face = ON_FreeType::CreateFace(*managed_font);
  }

  FT_Face ft_face
    = (nullptr != managed_font->m_free_type_face)
    ? managed_font->m_free_type_face->m_face
    : nullptr;

  return (ON__UINT_PTR)ft_face;
}

void ON_Font::DestroyFreeTypeFace(
  const ON_Font* font
)
{
  if (nullptr != font && nullptr != font->m_free_type_face)
  {
    if (font->IsManagedFont())
      delete font->m_free_type_face;
    font->m_free_type_face = nullptr;
  }
}

void ON_FreeTypeGetFontMetrics(
  const ON_Font* font,
  ON_FontMetrics& font_unit_font_metrics
)
{
  font_unit_font_metrics = ON_FontMetrics::Unset;

  if (nullptr == font)
    return;

  FT_Face ft_face = (FT_Face)ON_Font::FreeTypeFace(font);
  if (nullptr == ft_face)
    return;

  ON_TextBox Ibox = ON_TextBox::Unset;
  const int ascent_of_I
    = (0 != ON_FreeTypeGetGlyphMetrics(font, 'I', Ibox) && Ibox.IsSet() && Ibox.m_bbmax.j > 0)
    ? Ibox.m_bbmax.j
    : 0;

  font_unit_font_metrics.SetAscentOfI(ascent_of_I);

  font_unit_font_metrics.SetHeights(
    ft_face->ascender,
    ft_face->descender,
    ft_face->units_per_EM,
    ft_face->height
  );

  font_unit_font_metrics.SetUnderscore(
    ft_face->underline_position,
    ft_face->underline_thickness
  );

  int h = (ascent_of_I > 0) ? ascent_of_I : ft_face->ascender;

  font_unit_font_metrics.SetStrikeout(
    (int)ceil(0.5*h),
    (int)ceil(0.5*ft_face->underline_thickness)
  );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class ON_FreeTypeOutlineAccumlator
{
public:
  ON_FreeTypeOutlineAccumlator() = default;
  ~ON_FreeTypeOutlineAccumlator() = default;
  ON_FreeTypeOutlineAccumlator(const ON_FreeTypeOutlineAccumlator&) = default;
  ON_FreeTypeOutlineAccumlator& operator=(const ON_FreeTypeOutlineAccumlator&) = default;

public:
  bool AccumulateOutlineBoundingBox(
    const ON_FontGlyph* glyph
  )
  {
    m_bAccumulateBoundingBox = true;
    return Internal_Accumulate(glyph);
  }

  bool AccumulateOutlineBoundingBox(
    FT_Face ft_face,
    FT_UInt ft_glyph_index,
    ON_TextBox& glyph_outline_bbox
  )
  {
    glyph_outline_bbox = ON_TextBox::Unset;
    m_bAccumulateBoundingBox = true;
    bool rc = Internal_Accumulate(
      ft_face, 
      ft_glyph_index
    );
    if (rc)
    {
      glyph_outline_bbox.m_bbmin.i = m_glyph_outline_bbmin.x;
      glyph_outline_bbox.m_bbmin.j = m_glyph_outline_bbmin.y;
      glyph_outline_bbox.m_bbmax.i = m_glyph_outline_bbmax.x;
      glyph_outline_bbox.m_bbmax.j = m_glyph_outline_bbmax.y;
    }
    return rc;
  }

  bool AccumulateOutlineContours(
    const ON_FontGlyph* glyph,
    bool bSingleStrokeFont,
    double text_height,
    ON_ClassArray< ON_SimpleArray< ON_Curve* > >& contours,
    ON_BoundingBox* glyph_bbox,
    ON_3dVector* glyph_advance
  );


  // "to" points are ends of segments.
  ON_2iPoint m_to_points_bbmin = ON_2iPoint::Unset;
  ON_2iPoint m_to_points_bbmax = ON_2iPoint::Unset;

  // "control" points are interior quadratic and cubic bezier control points.
  // They are not necessarily inside thte glyph's bounding box.
  ON_2iPoint m_control_points_bbmin = ON_2iPoint::Unset;
  ON_2iPoint m_control_points_bbmax = ON_2iPoint::Unset;

  ON_2iPoint m_glyph_outline_bbmin = ON_2iPoint::Unset;
  ON_2iPoint m_glyph_outline_bbmax = ON_2iPoint::Unset;

  ON_SimpleArray<ON_FontGlyphOutlinePoint> m_points;
private:

  int Internal_GetOutlineCurves(
    int point_index0,
    int point_index1,
    double curve_scale,
    ON_SimpleArray< ON_Curve* >& curves
  ) const;

  ON_FontGlyphOutlinePoint m_current_point = ON_FontGlyphOutlinePoint::Unset;

  void Internal_AccumulatePoint(
    const FT_Vector* v,
    ON_FontGlyphOutlinePoint::ContourPointType point_type,
    bool bIsToPoint
  );

  void UnsetCurrentPoint()
  {
    m_current_point = ON_FontGlyphOutlinePoint::Unset;
  }
  
private:
  bool Internal_Accumulate(const ON_FontGlyph* glyph);
  bool Internal_Accumulate(FT_Face ft_face, FT_UInt font_glyph_id);
  
  static int Internal_SegmentNurbsOrder(
    int count,
    const ON_FontGlyphOutlinePoint* points
  );

  static bool Internal_GetBezierBoundingBox(
    int order,
    const ON_FontGlyphOutlinePoint* point,
    ON_2iPoint& bbmin, 
    ON_2iPoint& bbmax
  );

  static void Internal_AddPointToBBox(
    const ON_2iPoint& point,
    ON_2iPoint& bbmin, 
    ON_2iPoint& bbmax
  );

private:
  bool m_bAccumulateBoundingBox = false;
  bool m_bAccumulatePoints = true;

  // m_point_count is incremented as points are accumulated.
  // In some cases, the information in FT_Outline does not
  // match the what is accumulated.
  ON__UINT32 m_point_count = 0;

  // m_contour_count is incremented by Internal_FreeTypeOutlineMoveToFunc()
  // When glyph outlines are damaged, the information in FT_Outline does not
  // match the what is accumulated.
  ON__UINT32 m_contour_count = 0;

  ON__UINT32 m_font_units_per_EM = 0;   // font design grid size
  ON__UINT32 m_font_height_of_font = 0; // typically ON_Font::Constants::AnnotationFontCellHeight
  ON__UINT32 m_font_height_of_I = 0;    // in normal font height

private:
  static int Internal_FreeTypeOutlineMoveToFunc(
    const FT_Vector* to,
    void* user
  );
  static int Internal_FreeTypeOutlineLineToFunc(
    const FT_Vector* to,
    void* user
  );
  static int Internal_FreeTypeOutlineConicToFunc(
    const FT_Vector* control,
    const FT_Vector* to,
    void* user
  );
  static int Internal_FreeTypeOutlineCubicToFunc(
    const FT_Vector* control1,
    const FT_Vector* control2,
    const FT_Vector* to,
    void* user
  );
  static int Internal_FreeTypeOutlineLineToCloseContourFunc(
    const FT_Vector* to,
    void* user
  );
};

bool ON_FreeTypeOutlineAccumlator::Internal_Accumulate(
  const ON_FontGlyph* glyph
)
{
  if (nullptr == glyph || false == glyph->CodePointIsSet())
    return false;

  return Internal_Accumulate(
    (FT_Face)glyph->FreeTypeFace(),
    (FT_UInt)glyph->FontGlyphId()
  );
}

void ON_FreeTypeOutlineAccumlator::Internal_AccumulatePoint(
  const FT_Vector* v,
  ON_FontGlyphOutlinePoint::ContourPointType point_type,
  bool bIsToPoint
)
{
  if (nullptr == v
    || ON_FontGlyphOutlinePoint::ContourPointType::Unset == point_type
    )
  {
    UnsetCurrentPoint();
  }
  else
  {
    m_point_count++;

    m_current_point.m_point_type = point_type;
    m_current_point.m_bToPoint = bIsToPoint ? 1 : 0;
    m_current_point.m_contour_index = (ON__UINT16)m_contour_count;
    m_current_point.m_point.x = (ON__INT32)v->x;
    m_current_point.m_point.y = (ON__INT32)v->y;
    if ( m_bAccumulatePoints )
      m_points.Append(m_current_point);
    if (m_bAccumulateBoundingBox)
    {
      if ( bIsToPoint )
        ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(m_current_point.m_point, m_to_points_bbmin, m_to_points_bbmax);
      else
        ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(m_current_point.m_point, m_control_points_bbmin, m_control_points_bbmax);
    }
  }
}


void ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(const ON_2iPoint& point, ON_2iPoint& bbmin, ON_2iPoint& bbmax)
{
  if (ON_UNSET_INT_INDEX == bbmin.x)
  {
    bbmin = point;
    bbmax = point;
  }
  else
  {
    if (ON_UNSET_INT_INDEX != point.x)
    {
      if (point.x < bbmin.x)
        bbmin.x = point.x;
      else if (point.x > bbmax.x)
        bbmax.x = point.x;
    }
    if (ON_UNSET_INT_INDEX != point.y)
    {
      if (point.y < bbmin.y)
        bbmin.y = point.y;
      else if (point.y > bbmax.y)
        bbmax.y = point.y;
    }
  }
}

bool ON_FreeTypeOutlineAccumlator::Internal_GetBezierBoundingBox(
  int order,
  const ON_FontGlyphOutlinePoint* point,
  ON_2iPoint& bez_bbmin,
  ON_2iPoint& bez_bbmax
)
{
  bez_bbmin = ON_2iPoint::Unset;
  if (order < 3 || order > 4 || nullptr != point )
    return false;

  const ON_FontGlyphOutlinePoint::ContourPointType point_type
    = (3 == order)
    ? ON_FontGlyphOutlinePoint::ContourPointType::QuadraticBezierPoint
    : ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint;

  if (false == point[0].m_bToPoint || false == point[0].m_point.IsSet())
    return false;

  if (point_type != point[1].m_point_type || point[1].m_bToPoint || false == point[1].m_point.IsSet())
    return false;

  if (4 == order)
  {
    if (point_type != point[2].m_point_type || point[2].m_bToPoint || false == point[2].m_point.IsSet())
      return false;
  }

  if (point_type != point[order-1].m_point_type || point[order-1].m_bToPoint || false == point[order-1].m_point.IsSet())
    return false;
  
  ON_BezierCurve bez(3, false, order);
  for (int i = 0; i < order; i++)
  {
    if (false == point[i].m_point.IsSet())
      return false;
    if (i > 0 && point_type != point[i].m_point_type)
      return false;
    if (i > 0 && i + 1 < order)
    {
      if (point[i].m_bToPoint)
        return false;
    }
    else
    {
      if (false == point[i].m_bToPoint)
        return false;
    }
    ON_3dPoint cv((double)point[i].m_point.x, (double)point[i].m_point.y, 0.0);
    bez.SetCV(i, cv);
  }
  const ON_BoundingBox bez_bbox = bez.BoundingBox();
  if (false == bez_bbox.IsNotEmpty())
    return false;

  bez_bbmin.x = (int)floor(bez_bbox.m_min.x);
  bez_bbmin.y = (int)floor(bez_bbox.m_min.y);
  bez_bbmax.x = (int)ceil(bez_bbox.m_max.x);
  bez_bbmax.y = (int)ceil(bez_bbox.m_max.y);
  return true;
}


bool ON_FreeTypeOutlineAccumlator::Internal_Accumulate(FT_Face ft_face, FT_UInt font_glyph_id)
{
  if (nullptr == ft_face)
    return false;

  if (0 == font_glyph_id)
    return false;

  if (m_bAccumulateBoundingBox)
    m_bAccumulatePoints = true;

  if (false == m_bAccumulatePoints)
    return false;

  if ( false == ON_FreeTypeLoadGlyph((ON__UINT_PTR)ft_face, font_glyph_id, false) )
    return false;
  
  FT_Glyph ft_glyph = nullptr; 
  if (FT_Err_Ok != FT_Get_Glyph(ft_face->glyph, &ft_glyph))
    return false;
  if (nullptr == ft_glyph)
    return false;

  if (FT_GLYPH_FORMAT_OUTLINE != ft_glyph->format)
    return false;

  const FT_OutlineGlyph ft_outline_glyph = (FT_OutlineGlyph)ft_glyph;

  FT_Outline ft_outline = ft_outline_glyph->outline;

  if (ft_outline.n_points <= 0 || nullptr == ft_outline.points)
  {
    FT_Done_Glyph(ft_glyph);
    return false;
  }

  m_current_point = ON_FontGlyphOutlinePoint::Unset;
  m_point_count = 0;
  m_contour_count = 0;
  m_points.SetCount(0);

  FT_Outline_Funcs ft_outline_funcs;
  memset(&ft_outline_funcs, 0, sizeof(ft_outline_funcs));
  ft_outline_funcs.move_to = ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineMoveToFunc;
  ft_outline_funcs.line_to = ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineLineToFunc;
  ft_outline_funcs.conic_to = ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineConicToFunc;
  ft_outline_funcs.cubic_to = ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineCubicToFunc;
  ft_outline_funcs.line_to_close_contour = ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineLineToCloseContourFunc;
  
  FT_Outline_Decompose( &ft_outline, &ft_outline_funcs, (void*)this );

  bool rc = (m_point_count > 0);

  while (rc && m_bAccumulateBoundingBox)
  {
    if (false == m_to_points_bbmin.IsSet() )
      break;
    if (false == m_to_points_bbmax.IsSet() )
      break;

    ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(m_to_points_bbmin, m_glyph_outline_bbmin, m_glyph_outline_bbmax);
    ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(m_to_points_bbmax, m_glyph_outline_bbmin, m_glyph_outline_bbmax);

    if (false == m_control_points_bbmin.IsSet())
      break;
    if (false == m_control_points_bbmax.IsSet())
      break;
    if (
      m_control_points_bbmin.x >= m_glyph_outline_bbmin.x
      && m_control_points_bbmax.x <= m_glyph_outline_bbmax.x
      && m_control_points_bbmin.y >= m_glyph_outline_bbmin.y
      && m_control_points_bbmax.y <= m_glyph_outline_bbmax.y
      )
      break;

    // look for bezier segments that are outside of current glyph box
    const int point_count = m_points.Count();
    if (point_count < 3)
      break;
    const ON_FontGlyphOutlinePoint* points = m_points.Array();

    int i1 = point_count;
    for (int i = 0; i + 1 < point_count; i = i1)
    {
      const int order = ON_FreeTypeOutlineAccumlator::Internal_SegmentNurbsOrder(point_count - i,points + i);
      if (order <= 2 )
      {
        i1 = i + 1;
        continue;
      }
      i1 = i + order - 1;
      ON_2iPoint cv_bbmin = points[i+1].m_point;
      ON_2iPoint cv_bbmax = points[i+1].m_point;
      if (4 == order)
      {
        ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(points[i + 1].m_point,cv_bbmin, cv_bbmax);
      }
      if (
        cv_bbmin.x >= m_glyph_outline_bbmin.x
        && cv_bbmax.x <= m_glyph_outline_bbmax.x
        && cv_bbmin.y >= m_glyph_outline_bbmin.y
        && cv_bbmax.y <= m_glyph_outline_bbmax.y
        )
      {
        // bezier segment is contained in current m_glyph_outline_bb* box.
        continue;
      }

      // Get tight bezier bounding box
      ON_2iPoint bez_bbmin = ON_2iPoint::Unset;
      ON_2iPoint bez_bbmax = ON_2iPoint::Unset;
      if( ON_FreeTypeOutlineAccumlator::Internal_GetBezierBoundingBox(order,points+i,bez_bbmin,bez_bbmax) )
      {
        ON_FreeTypeOutlineAccumlator::ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(bez_bbmin, m_glyph_outline_bbmin, m_glyph_outline_bbmax);
        ON_FreeTypeOutlineAccumlator::Internal_AddPointToBBox(bez_bbmax, m_glyph_outline_bbmin, m_glyph_outline_bbmax);
      }
    }
    break;
  }

  // Frees point memory
  FT_Done_Glyph(ft_glyph);

  return rc;
}

bool ON_FreeTypeOutlineAccumlator::AccumulateOutlineContours(
  const ON_FontGlyph* glyph,
  bool bSingleStrokeFont,
  double text_height,
  ON_ClassArray< ON_SimpleArray< ON_Curve* > >& contours,
  ON_BoundingBox* glyph_bbox,
  ON_3dVector* glyph_advance
)
{
  ON_BoundingBox local_glyph_bbox = ON_BoundingBox::UnsetBoundingBox;
  ON_3dVector local_glyph_advance = ON_3dVector::ZeroVector;
  if (nullptr == glyph_bbox)
    glyph_bbox = &local_glyph_bbox;
  else
    *glyph_bbox = local_glyph_bbox;
  if (nullptr == glyph_advance)
    glyph_advance = &local_glyph_advance;
  else
    *glyph_advance = local_glyph_advance;

  if (nullptr == glyph || false == glyph->CodePointIsSet())
    return false;

  double scale = 1.0; // scale applid to glyph outline in font units
  const bool bNoScale = (ON_UNSET_VALUE == text_height);
  
  if (false == bNoScale)
  {
    const ON_Font* font = glyph->Font();
    if (nullptr != font)
    {
      scale = glyph->Font()->FontUnitToNormalizedScale();
      if (text_height > 0.0 && ON_IsValid(text_height))
        scale *= font->FontMetrics().GlyphScale(text_height);
      if (fabs(1.0 - scale) <= ON_ZERO_TOLERANCE)
        scale = 1.0;
    }
  }

  if (nullptr != glyph)
  {
    const ON_TextBox glyph_box = glyph->FontUnitGlyphBox();
    if (glyph_box.IsSet())
    {
      glyph_bbox->m_min.Set(scale*glyph_box.m_bbmin.i, scale*glyph_box.m_bbmin.j, 0.0);
      glyph_bbox->m_max.Set(scale*glyph_box.m_bbmax.i, scale*glyph_box.m_bbmax.j, 0.0);
    }
    if ( glyph_box.m_advance.i >= 0 && glyph_box.m_advance.j >= 0 )
    {
      glyph_advance->Set(scale*glyph_box.m_advance.i, scale*glyph_box.m_advance.j, 0.0);
    }
  }

  m_bAccumulateBoundingBox = false;
  m_bAccumulatePoints = true;

  if (false == Internal_Accumulate(glyph))
    return false;

  const int point_count = m_points.Count();
  if (0 == point_count)
    return false;

  int i1 = point_count;
  for (int i0 = 0; i0 < point_count; i0 = i1)
  {
    const ON_FontGlyphOutlinePoint p0 = m_points[i0];
    for (i1 = i0 + 1; i1 < point_count; i1++)
    {
      if (p0.m_contour_index != m_points[i1].m_contour_index)
        break;
    }

    int ii1 = i1;
    if (bSingleStrokeFont
      && ii1 > i0 + 1
      && ON_FontGlyphOutlinePoint::ContourPointType::LineToCloseContour == m_points[ii1-1].m_point_type
      )
    {
      // omit final line segment added by FT_Outline_Decompose(...)  to close this contour.
      ii1--;
    }

    ON_SimpleArray< ON_Curve* > curves;
    Internal_GetOutlineCurves( i0, ii1, scale, curves);
    const int curve_count = curves.Count();
    if (curve_count > 0)
    {
      if (false == bSingleStrokeFont)
      {
        const ON_3dPoint P0 = curves[0]->PointAtStart();
        const ON_3dPoint P1 = curves[curve_count-1]->PointAtEnd();
        double d = P0.DistanceTo(P1);
        if (d > ON_ZERO_TOLERANCE)
        {
          curves.Reserve(2 * curve_count);
          for (int k = curve_count - 1; k >= 0; k--)
          {
            ON_Curve* rev = curves[k]->DuplicateCurve();
            if (nullptr == rev )
            {
              for (int n = curve_count; n < k; n++)
              {
                delete curves[n];
              }
              curves.SetCount(curve_count);
              break;
            }
            rev->Reverse();
            curves.Append(rev);
          }
        }
      }
      contours.Append(curves);
    }
  }

  return true;
}



int ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineMoveToFunc( 
  const FT_Vector* to, 
  void* user 
)
{
  ON_FreeTypeOutlineAccumlator* a = (ON_FreeTypeOutlineAccumlator*)user;
  if (nullptr == a)
    return FT_Err_Invalid_Argument;

  a->m_contour_count++;

  if (nullptr != to)
  {
    a->Internal_AccumulatePoint(to,ON_FontGlyphOutlinePoint::ContourPointType::MoveTo,true);
  }
  else
  {
    a->UnsetCurrentPoint();
  }

  return FT_Err_Ok;
}

int ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineLineToFunc(
  const FT_Vector* to, 
  void* user 
)
{
  ON_FreeTypeOutlineAccumlator* a = (ON_FreeTypeOutlineAccumlator*)user;
  if (nullptr == a)
    return FT_Err_Invalid_Argument;

  if (
    nullptr != to 
    && ON_FontGlyphOutlinePoint::ContourPointType::Unset != a->m_current_point.m_point_type
    )
  {
    a->Internal_AccumulatePoint(to,ON_FontGlyphOutlinePoint::ContourPointType::LineTo,true);
  }
  else
    a->UnsetCurrentPoint();

  return FT_Err_Ok;
}

int ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineLineToCloseContourFunc(
  const FT_Vector* to, 
  void* user 
)
{
  ON_FreeTypeOutlineAccumlator* a = (ON_FreeTypeOutlineAccumlator*)user;
  if (nullptr == a)
    return FT_Err_Invalid_Argument;

  if (
    nullptr != to 
    && ON_FontGlyphOutlinePoint::ContourPointType::Unset != a->m_current_point.m_point_type
    )
  {
    a->Internal_AccumulatePoint(to,ON_FontGlyphOutlinePoint::ContourPointType::LineToCloseContour,true);
  }
  else
    a->UnsetCurrentPoint();

  return FT_Err_Ok;
}


int ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineConicToFunc(
  const FT_Vector* control,
  const FT_Vector* to, 
  void* user 
)
{
  ON_FreeTypeOutlineAccumlator* a = (ON_FreeTypeOutlineAccumlator*)user;
  if (nullptr == a)
    return 1;

  if (
    nullptr != control 
    && nullptr != to 
    && ON_FontGlyphOutlinePoint::ContourPointType::Unset != a->m_current_point.m_point_type
    )
  {
    a->Internal_AccumulatePoint(control,ON_FontGlyphOutlinePoint::ContourPointType::QuadraticBezierPoint,false);
    a->Internal_AccumulatePoint(to,ON_FontGlyphOutlinePoint::ContourPointType::QuadraticBezierPoint,true);
  }
  else
    a->UnsetCurrentPoint();

  return FT_Err_Ok;
}

int ON_FreeTypeOutlineAccumlator::Internal_FreeTypeOutlineCubicToFunc(
  const FT_Vector* control1,
  const FT_Vector* control2,
  const FT_Vector* to, 
  void* user 
)
{
  ON_FreeTypeOutlineAccumlator* a = (ON_FreeTypeOutlineAccumlator*)user;
  if (nullptr == a)
    return FT_Err_Invalid_Argument;

  if (
    nullptr != control1 
    && nullptr != control2
    && nullptr != to
    && ON_FontGlyphOutlinePoint::ContourPointType::Unset != a->m_current_point.m_point_type
    )
  {
    a->Internal_AccumulatePoint(control1,ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint,false);
    a->Internal_AccumulatePoint(control2,ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint,false);
    a->Internal_AccumulatePoint(to,ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint,true);
  }
  else
    a->UnsetCurrentPoint();

  return FT_Err_Ok;
}

int ON_FreeTypeOutlineAccumlator::Internal_SegmentNurbsOrder(
  int count,
  const ON_FontGlyphOutlinePoint* points
)
{
  if (count < 2 || 0 == points[0].m_bToPoint )
    return 0;

  switch (points[1].m_point_type)
  {
  case ON_FontGlyphOutlinePoint::ContourPointType::LineTo:
  case ON_FontGlyphOutlinePoint::ContourPointType::LineToCloseContour:
    if ( 0 != points[1].m_bToPoint )
      return 2;
    break;

  case ON_FontGlyphOutlinePoint::ContourPointType::QuadraticBezierPoint:
    if (count < 3)
      return 0;
    if (
      ON_FontGlyphOutlinePoint::ContourPointType::QuadraticBezierPoint == points[2].m_point_type
      && 0 != points[2].m_bToPoint 
      )
      return 3;
    break;

  case ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint:
    if (count < 4)
      return 0;
    if (
      ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint == points[2].m_point_type 
      && ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint == points[3].m_point_type
      && 0 != points[3].m_bToPoint 
      )
      return 4;
    break;
  }

  return 0;
}

int ON_FreeTypeOutlineAccumlator::Internal_GetOutlineCurves(
  int point_index0,
  int point_index1,
  double curve_scale,
  ON_SimpleArray< ON_Curve* >& curves
) const
{
  if (point_index0 < 0 || point_index1 > m_points.Count())
    return 0;

  int count = point_index1 - point_index0;
  if (count <= 1)
    return 0;

  const ON_FontGlyphOutlinePoint* points = m_points.Array() + point_index0;
  
  const int curve_count0 = curves.Count();

  const double scale
    = (curve_scale > 0.0 && ON_IsValid(curve_scale))
    ? curve_scale
    : 1.0;

  int i1 = count;
  for (int i = 0; i+1 < count; i = i1)
  {
    const int order = ON_FreeTypeOutlineAccumlator::Internal_SegmentNurbsOrder(count - i,points + i);
    if (0 == order)
    {
      i1 = i + 1;
      continue;
    }
    int cv_count = order;
    const int degree = order - 1;
    for (i1 = i + degree; i1+1 < count; i1 += degree)
    {
      if (order != ON_FreeTypeOutlineAccumlator::Internal_SegmentNurbsOrder(count - i1, points + i1))
        break;
      cv_count += degree;
    }

    ON_NurbsCurve* curve = new ON_NurbsCurve(3, false, order, cv_count);
    ON_3dPoint cv = ON_3dPoint::Origin;
    for (int j = 0; j < cv_count; j++)
    {
      cv.x = scale*(double)points[i+j].m_point.x;
      cv.y = scale*(double)points[i+j].m_point.y;
      curve->SetCV(j, ON::point_style::not_rational,&cv.x);
    }
    const int knot_count = cv_count + order - 2;
    const int knot_mult = order - 1;
    double t = 0.0;
    for (int j = 0; j < knot_count; /*empty iterator*/)
    {
      const int j1 = j + knot_mult;
      while(j < j1 && j < knot_count)
        curve->m_knot[j++] = t;
      t += 1.0;
    }


    curves.Append(curve);
  }

  return (curves.Count() - curve_count0);
}


bool ON_FreeTypeLoadGlyph(
  ON__UINT_PTR ft_face_ptr,
  ON__UINT_PTR ft_glyph_id_ptr,
  bool bLoadRenderBitmap
)
{
  FT_Face ft_face = (FT_Face)ft_face_ptr;
  if (nullptr == ft_face)
    return false;

  const FT_UInt font_glyph_id = (FT_UInt)ft_glyph_id_ptr;
  if (0 == font_glyph_id)
    return false;

#if defined(ON_RUNTIME_WIN)
  const int pass0 = 1;
#else
  const int pass0 = 1;
#endif
  for (int pass = bLoadRenderBitmap ? 1 : pass0; pass < 2; pass++)
  {
    FT_Int32 ft_face_load_no_scale_flag 
      = (0 == pass)
      ? 0
      : FT_LOAD_NO_SCALE;


    if (0 == pass)
    {
      const FT_UInt ft_font_height
        = (0 == pass)
        ? ((FT_UInt)ON_Font::Constants::AnnotationFontCellHeight)
        : ((FT_UInt)ft_face->units_per_EM);
      /*
        Avoid use of FT_LOAD_NO_SCALE
        https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_LOAD_NO_SCALE
          If the font is ‘tricky’ (see FT_FACE_FLAG_TRICKY for more), using FT_LOAD_NO_SCALE 
          usually yields meaningless outlines because the subglyphs must be scaled and 
          positioned with hinting instructions. This can be solved by loading the font 
          without FT_LOAD_NO_SCALE and setting the character size to ‘font->units_per_EM’.
      */
#if defined(ON_RUNTIME_WIN)
      // Unit Systems:
      //  "ppt" = printers points = 1/72 inch
      // "lfu" = Windows logical font units = LOGFONT.lfHeight units
      // "linch" = "logical" inch units used by ::GetDeviceCaps(hdc, LOGPIXELS*)
      // "pixels" = "pixel" units used by ::GetDeviceCaps(hdc, LOGPIXELS*)
      HDC hdc = ::CreateCompatibleDC(0); // "screen" device context
      const int horiz_pixels_per_inch = ::GetDeviceCaps(hdc, LOGPIXELSX); // units = horiz pixels/"logical inch"
      const int vert_pixels_per_inch = ::GetDeviceCaps(hdc, LOGPIXELSY); // units = vertical pixels/"logical inch"
      ::DeleteDC(hdc);
      const double logical_font_height = ((double)ON_Font::Constants::AnnotationFontCellHeight); // units = lfu
      const double ppts_per_inch = 72.0; // printer points / inch
      // If "logical font units"/("pixels"/"logical inch") = "real" inch = 2.54 cm
      // then vpptr and hppts are in printers points.
      const double vppts = ppts_per_inch * logical_font_height / ((double)vert_pixels_per_inch);
      const double hppts = ppts_per_inch * logical_font_height / ((double)horiz_pixels_per_inch);

      const FT_F26Dot6 ft_char_width = (int)ceil(hppts*64.0);
      const FT_F26Dot6 ft_char_height = (int)ceil(vppts*64.0);
#else
      const FT_F26Dot6 ft_char_width = 0;
      const FT_F26Dot6 ft_char_height = 0;
#endif
      if (0 == ft_char_width || 0 == ft_char_height)
        continue;
      if (FT_Err_Ok != FT_Set_Char_Size(
        ft_face,
        ft_char_width,
        ft_char_height,
        ft_font_height,
        ft_font_height
      ))
      {
        continue;
      }
    }

    const FT_Int32 ft_load_outline_flags
      = FT_LOAD_NO_BITMAP
      | FT_LOAD_NO_HINTING
      | FT_LOAD_NO_AUTOHINT
      | ft_face_load_no_scale_flag
      | FT_LOAD_LINEAR_DESIGN
      | FT_LOAD_IGNORE_TRANSFORM
      ;

    const FT_Int32 ft_load_flags
      = (bLoadRenderBitmap)
      ? FT_LOAD_RENDER
      : ft_load_outline_flags;

    if (FT_Err_Ok != FT_Load_Glyph(ft_face, font_glyph_id, ft_load_flags))
      continue;

    return true;
  }

  return false;
}

// Returns font glyph id or 0
ON__UINT_PTR ON_FreeTypeGetGlyphMetrics(
  const ON_Font* font,
  ON__UINT32 unicode_code_point,
  class ON_TextBox& font_unit_glyph_box
)  
{
  font_unit_glyph_box = ON_TextBox::Unset;

  if (false == ON_IsValidUnicodeCodePoint(unicode_code_point))
    return 0;

  const ON__UINT_PTR ft_face_as_uint = ON_Font::FreeTypeFace(font);
  if (0 == ft_face_as_uint)
    return 0;

  FT_Face face = (FT_Face)ft_face_as_uint;
    
  const unsigned int glyph_id = ON_FreeType::GlyphId(face, unicode_code_point);
  if (0 == glyph_id)
    return 0;
  
  const bool bLoadRenderBitmap = false;
  // bLoadRenderBitmap = false means we load using FT_LOAD_NO_SCALE
  // This won't work for "tricky" font faces that render glyphs using composites.
  if (false == ON_FreeTypeLoadGlyph(ft_face_as_uint, glyph_id, bLoadRenderBitmap))
    return 0;

  if ( nullptr == face->glyph)
    return 0;
  
  // Because ft_load_flags includes FT_LOAD_NO_SCALE, the
  // face->glyph->metrics units are expressed in font units.
  ON_TextBox ft_glyph_box;
  ft_glyph_box.m_bbmin.i = (int)(face->glyph->metrics.horiBearingX);
  ft_glyph_box.m_bbmin.j = (int)(face->glyph->metrics.horiBearingY - face->glyph->metrics.height);
  ft_glyph_box.m_bbmax.i = (int)(face->glyph->metrics.horiBearingX + face->glyph->metrics.width);
  ft_glyph_box.m_bbmax.j = (int)(face->glyph->metrics.horiBearingY);
  ft_glyph_box.m_advance.i = (int)(face->glyph->metrics.horiAdvance);
  ft_glyph_box.m_advance.j = (int)(face->glyph->metrics.vertAdvance); // positive values mean downwards advance

  // NOPE - read freetype docs an pay close attention to FT_LOAD_NO_SCALE behavior.
  //ft_glyph_box.m_advance.i = (int)(face->glyph->advance.x);
  //ft_glyph_box.m_advance.j = (int)(face->glyph->advance.y);
  //ft_glyph_box.m_advance.i = (int)(face->glyph->linearHoriAdvance);
  //ft_glyph_box.m_advance.j = (int)(face->glyph->linearVertAdvance);

  ON_TextBox ft_glyph_outline_box;
  ON_FreeTypeOutlineAccumlator a;
  if ( a.AccumulateOutlineBoundingBox(face, glyph_id, ft_glyph_outline_box) 
    && ft_glyph_outline_box.IsSet()
    )
  {
    // AccumulateBoundingBox to returns false for code points like CR, LF, TAB, ...
    // Whenever possible we get the answer directly from the  glyph outline. 
    // This box gets cached on ON_FontGlyph's so it is calculated
    // once per glyph.
    ft_glyph_box.m_bbmin = ft_glyph_outline_box.m_bbmin;
    ft_glyph_box.m_bbmax = ft_glyph_outline_box.m_bbmax;
  }

  font_unit_glyph_box = ft_glyph_box;

  return glyph_id;
  // face glyph_id
}

bool ON_FreeTypeGetGlyphOutline(
  const ON_FontGlyph* glyph,
  bool bSingleStrokeFont,
  double text_height,
  ON_ClassArray< ON_SimpleArray< ON_Curve* > >& contours,
  ON_BoundingBox* glyph_bbox,
  ON_3dVector* glyph_advance
)
{
  ON_FreeTypeOutlineAccumlator a;
  return a.AccumulateOutlineContours(
    glyph,
    bSingleStrokeFont,
    text_height,
    contours,
    glyph_bbox,
    glyph_advance
    );  
}

void ON_Font::DumpFreeTypeFace(
  ON__UINT_PTR free_type_face_ptr,
  ON_TextLog& text_log
)
{
  FT_Face face = (FT_Face)free_type_face_ptr;
  if (nullptr == face)
  {
    text_log.Print("FT_Face nullptr\n");
    return;
  }

  text_log.Print(
    "FT_Face: face[%d] (%d faces in font definition)\n",
    face->face_index,
    face->num_faces
  );

  text_log.PushIndent();
  ON_wString s;

  s = face->family_name;
  text_log.Print("Family name = %ls\n", static_cast<const wchar_t*>(s));
  
  s = face->style_name;
  text_log.Print("Style name = %ls\n", static_cast<const wchar_t*>(s));
  
  FT_Long style_mask = 0xFFFF;
  s = ON_FreeType::StyleFlagsToString(face->style_flags);
  if ( 0 != (style_mask&face->style_flags) || s.IsNotEmpty() )
     text_log.Print( "Style flags = 0x%04x %ls\n", (style_mask&face->style_flags), static_cast<const wchar_t*>(s) );

  s = ON_FreeType::FaceFlagsToString(face->face_flags);
  if ( 0 != face->face_flags || s.IsNotEmpty() )
     text_log.Print( "Face flags = 0x%x %ls\n", (face->face_flags), static_cast<const wchar_t*>(s) );

  text_log.Print("%d glyphs\n", face->num_glyphs);
  text_log.Print("%d charmaps\n", face->num_charmaps);

  text_log.PushIndent();
  if (nullptr != face->charmaps)
  {
    for (int i = 0; i < face->num_charmaps; i++)
    {
      FT_CharMap cmap = face->charmaps[i];
      const wchar_t* damaged
        = ON_FreeType::IsDamagedCharMap(cmap)
        ? L"DAMAGED "
        : L"";
      s = ON_wString::FormatToString( L"%lscmap[%d]", damaged, i );
      if (nullptr == cmap)
        s += L"nullptr";
      else
      {
        const ON_wString s1 = ON_FreeType::CharmapPlatformEncodingDescription(cmap);
        s += ON_wString::FormatToString(L" %ls", static_cast<const wchar_t*>(s1));
      }
      text_log.Print("%ls\n", static_cast<const wchar_t*>(s));
    }
  }
  text_log.PopIndent();
  
  text_log.PopIndent();

  return;
}


#else

bool ON_FontGlyph::TestFaceCharMaps(
  ON_TextLog* text_log
) const
{
  return true;
}

void ON_FreeTypeGetFontMetrics(
  const ON_Font* font,
  ON_FontMetrics& font_unit_font_metrics
)
{
  font_unit_font_metrics = ON_FontMetrics::Unset;
  return;
}

ON__UINT_PTR ON_FreeTypeGetGlyphMetrics(
  const ON_Font* font,
  ON__UINT32 unicode_code_point,
  class ON_TextBox& font_unit_glyph_box
)  
{
  font_unit_glyph_box = ON_TextBox::Unset;
  return 0;
}

ON__UINT_PTR ON_Font::FreeTypeFace(
  const ON_Font* font
)
{
  return 0;
}

void ON_Font::DestroyFreeTypeFace(
  const ON_Font* font
)
{}


bool ON_FreeTypeGetGlyphOutline(
  const ON_FontGlyph* glyph,
  bool bSingleStrokeFont,
  double text_height,
  ON_ClassArray< ON_SimpleArray< ON_Curve* > >& contours,
  ON_BoundingBox* glyph_bbox,
  ON_3dVector* glyph_advance
)
{
  return false;
}

bool ON_FreeTypeLoadGlyph(
  ON__UINT_PTR ft_face_ptr,
  ON__UINT_PTR font_glyph_index_ptr,
  bool bLoadRenderBitmap
)
{
  return false;
}

void ON_Font::DumpFreeTypeFace(
  ON__UINT_PTR free_type_face_ptr,
  ON_TextLog& text_log
)
{
  return;
}


#endif

bool ON_FontGlyph::GetGlyphContours(
  bool bSingleStrokeFont,
  double text_height,
  ON_ClassArray< ON_SimpleArray< ON_Curve* > >& glyph_contours,
  ON_BoundingBox* glyph_bbox,
  ON_3dVector* glyph_advance
) const
{
  return ON_FreeTypeGetGlyphOutline(
    this,
    bSingleStrokeFont,
    text_height,
    glyph_contours,
    glyph_bbox,
    glyph_advance
  );
}

bool ON_Annotation::GetTextGlyphContours(
  const ON_Viewport* vp,
  const ON_DimStyle* dimstyle,
  bool bApplyDimStyleDimScale,
  bool bSingleStrokeFont,
  ON_ClassArray< ON_ClassArray< ON_SimpleArray< ON_Curve* > > >& text_contours
) const
{
  const ON_TextContent* text_content = Text();
  if (nullptr == text_content)
    return false;

  double text_scale = 0.0;
  if (bApplyDimStyleDimScale && nullptr != dimstyle)
  {
    text_scale = dimstyle->DimScale();
  }
  if (false == (text_scale > 0.0 && ON_IsValid(text_scale)))
    text_scale = 1.0;


  ON_Xform text_xform = ON_Xform::IdentityTransformation;
  if (false == this->GetTextXform(vp, dimstyle, text_scale, text_xform))
    text_xform = ON_Xform::IdentityTransformation;

  const ON_Font* text_font = (nullptr != dimstyle) ? &dimstyle->Font() : nullptr;
  
  return text_content->GetGlyphContours(text_font, bSingleStrokeFont, text_xform, text_contours);
}

bool ON_TextContent::GetGlyphContours(
  const ON_Font* text_font,
  bool bSingleStrokeFont,
  double text_height,
  ON_ClassArray< ON_ClassArray< ON_SimpleArray< ON_Curve* > > >& text_contours
) const
{
  ON_Xform text_xform = ON_Xform::IdentityTransformation;

  double scale = 1.0;
  if (text_height > 0.0 && ON_IsValid(text_height) )
  {
    if (nullptr == text_font)
      text_font = &ON_Font::Default;
    scale = text_font->FontMetrics().GlyphScale(text_height);
    if (scale > 0.0)
      text_xform = ON_Xform::DiagonalTransformation(scale);
  }

  return this->GetGlyphContours(
    text_font,
    bSingleStrokeFont,
    text_xform,
    text_contours
  );
}


static const ON_FontGlyph* Internal_GetGlyphContours_SmallCapsGlyph(
  const ON_FontGlyph* glyph
)
{
  if (nullptr == glyph || false == glyph->CodePointIsSet() )
    return nullptr;
  const ON_FontGlyph* small_caps_glyph = nullptr;
  const ON__UINT32 code_point = glyph->CodePoint();
  const ON__UINT32 upper_ordinal_code_point = ON_UnicodeMapCodePointOrdinal(ON_StringMapOrdinalType::UpperOrdinal, code_point);
  if (
    upper_ordinal_code_point != code_point
    && upper_ordinal_code_point >= 'A'
    && ON_IsValidUnicodeCodePoint(upper_ordinal_code_point)
    )
  {
    small_caps_glyph = glyph->Font()->CodePointGlyph(upper_ordinal_code_point);
    if (nullptr != small_caps_glyph)
    {
      if (glyph->Font() != small_caps_glyph->Font() || small_caps_glyph != small_caps_glyph->RenderGlyph(false))
      {
        // do not permit font or glyph substitution when "small caps" are used.
        small_caps_glyph = nullptr;
      }
    }
  }
  return small_caps_glyph;
}

bool ON_FontGlyph::GetStringContours(
  const wchar_t* text_string,
  const ON_Font* font,
  bool bSingleStrokeFont,
  double text_height,
  double small_caps_scale,
  ON_ClassArray< ON_ClassArray< ON_SimpleArray< ON_Curve* > > >& string_contours
)
{
  // Dale Lear: https://mcneel.myjetbrains.com/youtrack/issue/RH-38183
  // Font substitution has to be used to get outlines for all code points.
  // I rewrote this entire function to support use of multiple fonts in a single string
  // to fix RH-38183.
  const bool bUseReplacementCharacter = true;

  if (nullptr == text_string || 0 == text_string[0])
    return false;
  
  const ON_Font* primary_font = (nullptr != font) ? font->ManagedFont() : ON_Font::Default.ManagedFont();
  if (nullptr == primary_font)
    return false;

  const ON_FontMetrics primary_fm = primary_font->FontMetrics();

  double scale = (text_height > ON_ZERO_TOLERANCE && text_height < 1.e38)
    ? primary_fm.GlyphScale(text_height)
    : 0.0;
  if (false == (scale > ON_ZERO_TOLERANCE && ON_IsValid(scale)) )
  {
    text_height = 0.0;
    scale = 1.0;
  }
  const double height_of_LF = scale*primary_fm.LineSpace();

  if (false == (text_height > ON_ZERO_TOLERANCE && text_height < 1.e38))
    text_height = 0.0;

  const double small_caps_text_height
    = (small_caps_scale > ON_ZERO_TOLERANCE && small_caps_scale < 1.0)
    ? small_caps_scale*text_height
    : text_height;

  ON_SimpleArray< const ON_FontGlyph* > glyph_list;
  ON_TextBox text_box;
  if (ON_FontGlyph::GetGlyphList(
    text_string,
    primary_font,
    ON_UnicodeCodePoint::ON_LineSeparator,
    glyph_list,
    text_box) <= 0)
  {
    return false;
  }
  
  double line_advance = 0.0;
  ON_3dPoint glyph_base_point = ON_3dPoint::Origin;

  unsigned int glyph_count = glyph_list.UnsignedCount();
  for ( unsigned int gdex = 0; gdex < glyph_count; gdex++ )
  {
    const ON_FontGlyph* glyph = glyph_list[gdex];
    if (nullptr == glyph)
      continue;
    if (glyph->IsEndOfLineCodePoint())
    {
      line_advance += height_of_LF;
      glyph_base_point.x = 0;
      glyph_base_point.y = line_advance;
      continue;
    }

    glyph = glyph->RenderGlyph(bUseReplacementCharacter);
    if (nullptr == glyph)
      continue;

    double glyph_text_height = text_height;

    const ON_FontGlyph* small_caps_glyph = 
      (small_caps_text_height > 0.0 &&  small_caps_text_height < text_height)
      ? Internal_GetGlyphContours_SmallCapsGlyph(glyph)
      : glyph;
    if (nullptr != small_caps_glyph)
    {
      glyph_text_height = small_caps_text_height;
      glyph = small_caps_glyph;
    }

    ON_BoundingBox glyph_contours_bbox = ON_BoundingBox::UnsetBoundingBox;
    ON_3dVector glyph_contours_advance = ON_3dVector::ZeroVector;
    ON_ClassArray< ON_SimpleArray< ON_Curve* > >& glyph_contours = string_contours.AppendNew();
    glyph->GetGlyphContours(bSingleStrokeFont, glyph_text_height, glyph_contours, &glyph_contours_bbox, &glyph_contours_advance);

    const ON_3dVector translate = glyph_base_point;
    glyph_base_point.x += glyph_contours_advance.x;

    const int contour_count = glyph_contours.Count();

    for (int li = 0; li < contour_count; li++)  // contours per glyph
    {
      const int curve_count = glyph_contours[li].Count();
      for (int ci = 0; ci < curve_count; ci++)
      {
        if (nullptr != glyph_contours[li][ci])
          glyph_contours[li][ci]->Translate(translate);
      }
    }
  }

  return true;
}

bool ON_TextRun::GetGlyphContours(
  const ON_Font* text_font,
  bool bSingleStrokeFont,
  const ON_Xform& text_xform,
  ON_ClassArray< ON_ClassArray< ON_SimpleArray< ON_Curve* > > >& run_contours
) const
{
  const ON_TextRun& run = *this;

  const ON_Font* run_font = run.Font();
  if (nullptr == run_font)
  {
    run_font = text_font;
    if (nullptr == run_font)
      run_font = &ON_Font::Default;
  }

  ON_Xform run_xf(text_xform);

  if (0.0 != run.m_offset.x || 0.0 != run.m_offset.y)
  {
    const ON_Xform run_offset(ON_Xform::TranslationTransformation(run.m_offset.x, run.m_offset.y, 0.0));
    run_xf = text_xform * run_offset;
  }

  double run_height = run.TextHeight();  // Specified height of text in Model units
  double I_height = run_font->FontMetrics().AscentOfI();
  double font_scale = run_height / I_height; // converts Font units to Model units, including text height
  ON_Xform scale_xf(ON_Xform::DiagonalTransformation(font_scale));
  run_xf = run_xf * scale_xf;

  if (run.IsStacked() == ON_TextRun::Stacked::kStacked && nullptr != run.m_stacked_text)
  {
    const ON_TextRun* stacked_runs[2] =
    {
      run.m_stacked_text->m_top_run,
      run.m_stacked_text->m_bottom_run,
    };
    bool rc = false;
    for (int i = 0; i < 2; i++)
    {
      if (nullptr == stacked_runs[i])
        continue;
      if (stacked_runs[i]->GetGlyphContours(
        run_font,
        bSingleStrokeFont,
        text_xform,
        run_contours
      ))
        rc = true;
    }

    //if (L'/' == run.m_stacked_text->m_separator)
    //{
    //  double h = 0.5 * I_height;
    //  double hs = (double)font->GetUnderscoreSize();
    //  double l = run.m_advance.x / font_scale;
    //  DrawFracLine(*this, run_xf, 0.0, h, hs, l, color);
    //}
    return rc;
  }


  // run->UnicodeString() returns the raw string which may have unevaluated fields.
  // run->DisplayString() returns the evaluated results of fields.
  const int run_contours_count0 = run_contours.Count();
  bool rc = ON_FontGlyph::GetStringContours(
    run.DisplayString(),
    run_font,
    bSingleStrokeFont,
    0.0, // text_height = 0.0 means get glyphs in openurbs normalized font size
    0.0, // small_caps_scale,
    run_contours
  );

  const int run_contours_count1 = run_contours.Count();
  for (int gi = run_contours_count0; gi < run_contours_count1; gi++)
  {
    ON_ClassArray< ON_SimpleArray< ON_Curve* > >& countours = run_contours[gi];
    const int countour_count = countours.Count();
    for (int li = 0; li < countour_count; li++)
    {
      ON_SimpleArray< ON_Curve* >& curves = countours[li];
      const int curve_count = curves.Count();
      for (int ci = 0; ci < curve_count; ci++)
      {
        ON_Curve* curve = curves[ci];
        if (curve)
          curve->Transform(run_xf);
      }
    }
  }

  return rc;
}

bool ON_TextContent::GetGlyphContours(
  const ON_Font* text_font,
  bool bSingleStrokeFont,
  const ON_Xform& text_xform,
  ON_ClassArray< ON_ClassArray< ON_SimpleArray< ON_Curve* > > >& text_contours
) const
{
  if (nullptr == text_font)
    text_font = &ON_Font::Default;

  const ON_Xform xf = text_xform;

  const ON_TextRunArray* runs = TextRuns(false);
  if (nullptr != runs)
  {
    const int runcount = runs->Count();
    for (int ri = 0; ri < runcount; ri++)
    {
      const ON_TextRun* run = (*runs)[ri];
      if (nullptr == run)
        continue;
      if (ON_TextRun::RunType::kText != run->Type() && ON_TextRun::RunType::kField != run->Type())
        continue;

      const ON_Font* run_font = run->Font();
      if (nullptr == run_font)
        run_font = text_font;

      run->GetGlyphContours(run_font, bSingleStrokeFont, xf, text_contours);
    }
  }

  return false;
}

void ON_Font::DumpFreeType(
  ON_TextLog& text_log
) const
{
  const ON_Font* managed_font = this->ManagedFont();
  if (nullptr == managed_font)
    return;
#if defined(OPENNURBS_FREETYPE_SUPPORT)
  ON_Font::DumpFreeTypeFace((ON__UINT_PTR)(managed_font->m_free_type_face->m_face), text_log);
#endif
}
