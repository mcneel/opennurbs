/*
// Copyright (c) 1993-2017 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//				
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
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

#include "opennurbs_internal_glyph.h"

void ON_FontGlyph::Internal_CopyFrom(const ON_FontGlyph& src)
{
  // Do not call any other ON_FontGlyph functions or
  // you risk infinite recursion when managed glyphs are being manufactured.
  m_normalized_glyph_bbox = src.m_normalized_glyph_bbox;
  m_font_unit_glyph_bbox = src.m_font_unit_glyph_bbox;
  m_code_point = src.m_code_point;
  m_managed_font = src.m_managed_font;
  m_substitute = src.m_substitute;
  // Do not copy m_is_managed.
  m_font_glyph_id = src.m_font_glyph_id;
}

ON_FontGlyph::ON_FontGlyph(const ON_FontGlyph& src)
{
  // Do not copy m_is_managed.
  Internal_CopyFrom(src);
}

ON_FontGlyph& ON_FontGlyph::operator=(const ON_FontGlyph& src)
{
  if (this != &src)
  {
    // Do not copy m_is_managed.
    Internal_CopyFrom(src);
  }
  return *this;
}

void ON_FontGlyph::Internal_SetFontGlyphId(ON__UINT_PTR font_glyph_id)
{
  m_font_glyph_id = font_glyph_id;
}

ON_FontGlyph::ON_FontGlyph(
  const ON_Font* font,
  ON__UINT32 code_point
)
{
  SetCodePoint(font, code_point);
}

const ON_Font* ON_FontGlyph::Font() const
{
  return m_managed_font;
}
  
int ON_FontGlyph::CompareCodePointAndFont(
  ON_FontGlyph& lhs,
  ON_FontGlyph& rhs
)
{
  if (lhs.m_managed_font != rhs.m_managed_font)
  {
    if (nullptr == lhs.m_managed_font)
      return -1;
    if (nullptr == rhs.m_managed_font)
      return 1;
    int rc = ON_Font::CompareFontCharacteristics(*lhs.m_managed_font, *rhs.m_managed_font);
    if (0 != rc)
      return rc;
    const unsigned int lhs_sn = lhs.m_managed_font->RuntimeSerialNumber();
    const unsigned int rhs_sn = rhs.m_managed_font->RuntimeSerialNumber();
    if (lhs_sn < rhs_sn)
      return -1;
    if (lhs_sn > rhs_sn)
      return 1;
  }

  if (lhs.m_code_point < rhs.m_code_point)
    return -1;

  if (lhs.m_code_point > rhs.m_code_point)
    return 1;

  return 0;
}

int ON_FontGlyph::GetGlyphList
(
  size_t code_point_count,
  ON__UINT32* code_points,
  const ON_Font* font,
  ON__UINT32 unicode_CRLF_code_point,
  ON_SimpleArray<const ON_FontGlyph*>& glyph_list,
  ON_TextBox& text_box
)
{
  glyph_list.SetCount(0);
  text_box = ON_TextBox::Unset;

  if (nullptr == code_points)
    return 0;

  while (code_point_count > 0)
  {
    const size_t i = code_point_count - 1;
    if (0 != code_points[i] && ON_IsValidUnicodeCodePoint(code_points[i]) && false == ON_FontGlyph::IsEndOfLineCodePoint(code_points[i]))
      break;
    code_point_count--;
  }

  while (code_point_count > 0)
  {
    // do not trim leading end of line code points.
    if (0 != code_points[0] && ON_IsValidUnicodeCodePoint(code_points[0]))
      break;
    code_points++;
    code_point_count--;
  }

  if (code_point_count <= 0)
    return 0;

  const int cp_count = (int)code_point_count;

  const ON_Font* managed_font
    = (nullptr != font)
    ? font->ManagedFont()
    : nullptr;

  if (nullptr == managed_font)
    managed_font = &ON_Font::Default;
  
  const bool bCondenseCRLF = ON_IsValidUnicodeCodePoint(unicode_CRLF_code_point) ? true : false;

  glyph_list.Reserve(cp_count + 1);
  glyph_list.SetCount(cp_count + 1);
  ON_FontGlyph const ** glyphs = glyph_list.Array();
  
  //////////////////////////////////////////////////////////
  //
  // Fill in glyphs[] with all the information needed to render the string.
  //
  ON__UINT32 cp1 = code_points[0];
  int glyph_count = 0;
  for (int i = 0; i < cp_count; i++)
  {
    glyphs[glyph_count] = nullptr;

    ON__UINT32 cp0 = cp1;
    cp1 = (i + 1 < cp_count) ? code_points[i + 1] : 0;
    if (0 == cp0)
      continue;
    if (false == ON_IsValidUnicodeCodePoint(cp0))
      continue;
    
    if (ON_FontGlyph::IsEndOfLineCodePoint(cp0))
    {
      if ( bCondenseCRLF && ON_FontGlyph::IsCarriageReturnAndLineFeed(cp0,cp1) )
      {
        cp0 = unicode_CRLF_code_point;
        i++; // skip the LF in (CR,LF) pair or the CR in a (LF,CR) pair.
        cp1 = (i + 1 < cp_count) ? code_points[i + 1] : 0;
      }
    }

    // Get all the glyph metric and freetype information required to render the
    // glyph for this codepoint in the correct location.
    ON_FontGlyph g;
    if (false == g.SetCodePoint(managed_font, cp0))
      continue;

    const ON_FontGlyph* managed_glyph = g.ManagedGlyph();
    if (nullptr == managed_glyph)
      continue;
      
    glyphs[glyph_count++] = managed_glyph;
  }

  while (glyph_count > 0 && glyphs[glyph_count - 1]->IsEndOfLineCodePoint() )
    glyph_count--;
  if (glyph_count <= 0)
  {
    glyph_list.SetCount(0);
    return 0;
  }
  glyph_list.SetCount(glyph_count);

  //////////////////////////////////////////////////////////
  //
  // Get the extents of the rendered text
  //
  int line_count = 0;
  const int line_height = managed_font->FontMetrics().LineSpace(); // >= 0

  int start_index = 0;
  for (int i = 0; i < glyph_count; i++)
  {
    const bool bEOL 
      = glyphs[i]->IsEndOfLineCodePoint() 
      || (bCondenseCRLF && unicode_CRLF_code_point == glyphs[i]->CodePoint());

    if (false == bEOL && i + 1 < glyph_count)
    {
      continue; // not at the end of a line
    }

    const int end_index = bEOL ? i-1 : i;
    const bool bEmptyLine 
      = glyphs[end_index]->IsEndOfLineCodePoint()
      || (bCondenseCRLF && unicode_CRLF_code_point == glyphs[end_index]->CodePoint());
    if (false == bEmptyLine)
    {
      // get bounding box of line
      ON_TextBox line_box;
      line_box.m_advance = ON_2dex::Zero;
      for (int gdex = start_index; gdex <= end_index; gdex++)
      {
        const ON_FontGlyph* gi = glyphs[gdex];
        ON_TextBox glyph_box = gi->GlyphBox();
        if (false == glyph_box.IsSet())
          continue;
        const ON_2dex glyph_delta = line_box.m_advance;
        line_box.m_advance = ON_2dex::Zero;
        if (line_box.IsSet())
          line_box = ON_TextBox::Union(line_box, ON_TextBox::Translate(glyph_box,glyph_delta) );
        else
          line_box = glyph_box;
        line_box.m_advance = glyph_delta;
        line_box.m_advance.i += glyph_box.m_advance.i;
      }

      if (line_box.IsSet())
      {
        // grow text_box
        // At this location in the code, line_count = 0 on first line, 1 on second line, ...
        const ON_2dex line_delta = { 0, -(line_count*line_height) };
        const ON_2dex line_advance = line_box.m_advance;
        line_box.m_advance = ON_2dex::Zero;
        text_box.m_advance = ON_2dex::Zero;
        if (text_box.IsSet())
          text_box = ON_TextBox::Union( text_box, ON_TextBox::Translate(line_box,line_delta) );
        else
          text_box = line_box;
        text_box.m_advance.i = line_advance.i;
        text_box.m_advance.j = -line_delta.j; // line_delta.j < 0 and m_advance.j is always >= 0.
      }
    }

    line_count++;

    if (false == bCondenseCRLF
      && i + 1 < glyph_count
      && ON_FontGlyph::IsCarriageReturnAndLineFeed(glyphs[i]->CodePoint(), glyphs[i + 1]->CodePoint())
      )
    {
      // false == bCondenseCRLF means the caller wants to preserve (CR,LF) and (LF,CR) 
      // as two distinct code points/ in the output glyph_list[].
      // We currently have (glyphs[i],glyphs[i + 1]) as a (CR,LF) or (LF,CR) pair.
      // Skip the second code point in the pair so line_count value will be set correctly.
      i++;
    }

    start_index = i + 1;
  }

  return line_count;
}

int ON_FontGlyph::GetGlyphList
(
  const wchar_t* text,
  const ON_Font* font,
  ON__UINT32 unicode_CRLF_code_point,
  ON_SimpleArray<const ON_FontGlyph*>& glyph_list,
  ON_TextBox& text_box
)
{
  glyph_list.SetCount(0);
  text_box = ON_TextBox::Unset;

  if (nullptr == text || 0 == text[0])
    return 0;

  const int textlength = ON_wString::Length(text);
  if (textlength < 1)
    return 0;

  ON_SimpleArray< ON__UINT32 > code_points(textlength + 1);
  code_points.SetCount(textlength + 1);

  const int cp_count = ON_ConvertWideCharToUTF32(
    0, // bTestByteOrder = false
    text,
    textlength,
    code_points.Array(),
    textlength,
    nullptr,    // error status - ingnored
    0xFFFFFFFF, // mask as many errors as possible
    ON_UnicodeCodePoint::ON_ReplacementCharacter,  // unicode error mark when string is incorrectly encoded
    nullptr     // pointer to end of parsed text is ignored
  );

  return GetGlyphList(cp_count, code_points.Array(), font, unicode_CRLF_code_point, glyph_list, text_box);
}

int ON_FontGlyph::GetGlyphListBoundingBox
(
  const wchar_t* text,
  const ON_Font* font,
  ON_TextBox& text_box
)
{
  const ON__UINT32 unicode_CRLF_code_point = ON_UnicodeCodePoint::ON_LineSeparator;
  ON_SimpleArray<const ON_FontGlyph*> glyph_list;
  return ON_FontGlyph::GetGlyphList(
    text,
    font,
    unicode_CRLF_code_point,
    glyph_list,
    text_box
  );
}

int ON_FontGlyph::GetGlyphListBoundingBox
(
  size_t code_point_count,
  ON__UINT32* code_points,
  const ON_Font* font,
  ON_TextBox& text_box
)
{
  const ON__UINT32 unicode_CRLF_code_point = ON_UnicodeCodePoint::ON_LineSeparator;
  ON_SimpleArray<const ON_FontGlyph*> glyph_list;
  return ON_FontGlyph::GetGlyphList(
    code_point_count,
    code_points,
    font,
    unicode_CRLF_code_point,
    glyph_list,
    text_box
  );
}

const ON__UINT32 ON_FontGlyph::CodePoint() const
{
  return m_code_point;
}

const ON__UINT_PTR ON_FontGlyph::FontGlyphId() const
{
  const ON_FontGlyph* managed_glyph = ManagedGlyph();
  return
    nullptr == managed_glyph
    ? 0
    : managed_glyph->m_font_glyph_id;
}

const ON__UINT_PTR ON_FontGlyph::FreeTypeFace() const
{
  return
    (nullptr == m_managed_font)
    ? 0
    : ON_Font::FreeTypeFace(m_managed_font);
}


bool ON_FontGlyph::IsEndOfLineCodePoint() const
{
  return ON_FontGlyph::IsEndOfLineCodePoint(m_code_point);
}

bool ON_FontGlyph::IsEndOfLineCodePoint(
  ON__UINT32 unicode_code_point
)
{
  switch (unicode_code_point)
  {
  case ON_UnicodeCodePoint::ON_LineFeed:
  case ON_UnicodeCodePoint::ON_VerticalTab:
  case ON_UnicodeCodePoint::ON_FormFeed:
  case ON_UnicodeCodePoint::ON_CarriageReturn:
  case ON_UnicodeCodePoint::ON_NextLine:
  case ON_UnicodeCodePoint::ON_LineSeparator:
  case ON_UnicodeCodePoint::ON_ParagraphSeparator:
    return true;
  default:
    break;
  }
  return false;
}

bool ON_FontGlyph::IsCarriageReturnAndLineFeed(
  ON__UINT32 cp0,
  ON__UINT32 cp1
)
{
  if (ON_UnicodeCodePoint::ON_CarriageReturn == cp0)
  {
    return (ON_UnicodeCodePoint::ON_LineFeed == cp1);
  }
 
  if (ON_UnicodeCodePoint::ON_LineFeed == cp0)
  {
    return (ON_UnicodeCodePoint::ON_CarriageReturn == cp1);
  }

  return false;
}

const ON_TextBox& ON_FontGlyph::GlyphBox() const
{
  return m_normalized_glyph_bbox;
}

const ON_TextBox& ON_FontGlyph::FontUnitGlyphBox() const
{
  return m_font_unit_glyph_bbox;
}

bool ON_FontGlyph::SetCodePoint(
  const ON_Font* font,
  ON__UINT32 code_point
)
{
  *this = ON_FontGlyph::Unset;
  const bool bValidCodePoint = (0 != ON_IsValidUnicodeCodePoint(code_point)) ? true : false;
  if ( bValidCodePoint)
    m_code_point = code_point;
  m_managed_font = (nullptr != font) ? font->ManagedFont() : nullptr;
  return (bValidCodePoint && nullptr != m_managed_font);
}

const ON_FontGlyph* ON_FontGlyph::RenderGlyph(
  bool bUseReplacementCharacter
) const
{
  if (CodePointIsSet())
  {
    for (int pass = 0; pass < (bUseReplacementCharacter ? 2 : 1); pass++)
    {
      const ON_FontGlyph* glyph
        = (0 == pass)
        ? this
        : ON_FontGlyph(this->m_managed_font, ON_UnicodeCodePoint::ON_ReplacementCharacter).ManagedGlyph();

      if (nullptr == glyph)
        continue;

      const ON_FontGlyph* managed_glyph = glyph->ManagedGlyph();
      if (nullptr == managed_glyph)
        continue;
      if (nullptr != managed_glyph->m_substitute)
        return managed_glyph->m_substitute;
      if (0 == glyph->m_font_glyph_id && bUseReplacementCharacter)
        continue;
      return glyph;
    }
  }
  return nullptr;
}


const ON_FontGlyph* ON_FontGlyph::SubstituteGlyph() const
{
  return m_substitute;
}

bool ON_FontGlyph::CodePointIsSet() const
{
  return (
    ON_IsValidUnicodeCodePoint(m_code_point)
    && nullptr != m_managed_font
    && m_managed_font->IsManagedFont()
    );
}

const ON_FontGlyph* ON_FontGlyph::ManagedGlyph() const
{
  if (IsManaged())
    return this;
  if (false == CodePointIsSet())
    return nullptr;
  return Font()->CodePointGlyph(CodePoint());
}

bool ON_FontGlyph::IsManaged() const
{
  return (m_is_managed ? true : false);
}

void ON_FontGlyph::Dump(
  bool bIncludeCharMaps,
  ON_TextLog& text_log
) const
{
  ON_wString s;
  const ON_FontGlyph* g = this;
  bool bPrintMaps = false;
  for (int pass = 0; pass < 2; pass++)
  {
    if (nullptr == g)
      break;

    if (pass > 0)
    {
      s += L" -> substitute: ";
    }

    if (g->CodePointIsSet())
    {
      const unsigned int code_point = g->CodePoint();
      const unsigned int glyph_id = (unsigned int)g->FontGlyphId();
      wchar_t w[8] = { 0 };
      ON_EncodeWideChar(code_point, 7, w);
      const ON_Font* font = g->Font();
      const ON_wString font_description = (font) ? font->FontDescription() : ON_wString::EmptyString;
      unsigned int font_sn = (font) ? font->RuntimeSerialNumber() : 0;
      s += ON_wString::FormatToString(
        L"[%ls] U+%04X",
        w,
        code_point,
        font_sn,
        static_cast<const wchar_t*>(font_description)
      );

      if (nullptr != font)
      {
        s += ON_wString::FormatToString(
          L" %ls <%u>",
          static_cast<const wchar_t*>(font_description),
          font_sn
        );
      }
      else
      {
        s += L" (no font)";
      }

      if (glyph_id > 0)
      {
        s += ON_wString::FormatToString(L" glyph id = %u", glyph_id);
        bPrintMaps = bIncludeCharMaps;
      }
      else
      {
        s += L" (no glyph)";
      }

      const ON_TextBox gbox = g->GlyphBox();
      const bool bGlyphBoxIsSet = gbox.IsSet();
      const bool bManagedGlyph = (g->IsManaged());
      if (bManagedGlyph)
      {
        if (false == bGlyphBoxIsSet)
          s += L" (unset box)";
      }
      else
      {
        s += (bGlyphBoxIsSet ? L" (unmanaged)" : L" (unmanaged, unset box)");
      }
    }
    else
    {
      s =+ L"ON_FontGlyph::Unset";
    }
    const ON_FontGlyph* sub_g = g->SubstituteGlyph();
    if (nullptr == sub_g)
      break;
    bPrintMaps = false;
    g = sub_g;
  }

  if (s.IsEmpty())
    s = L"ON_FontGlyph->this = nullptr";

  text_log.PrintString(s);
  text_log.PrintNewLine();
  if ( bPrintMaps && nullptr != g )
  {
    text_log.PushIndent();
    g->TestFaceCharMaps(&text_log);
    text_log.PopIndent();
  }
}


bool ON_TextBox::IsSet() const
{
  // m_advance values may be INT_MIN = ON_UNSET_INT_INDEX-1 for line feeds - more investigation required.
  return(
    ON_UNSET_INT_INDEX < m_bbmin.i && m_bbmin.i <= m_bbmax.i
    && ON_UNSET_INT_INDEX < m_bbmax.j && m_bbmin.j <= m_bbmax.j
    && ON_UNSET_INT_INDEX != m_max_basepoint.i
    && ON_UNSET_INT_INDEX != m_max_basepoint.j
    && ON_UNSET_INT_INDEX != m_advance.i
    && ON_UNSET_INT_INDEX != m_advance.j
    );
}

ON_TextBox::ON_TextBox(
  ON_2dPoint bbmin,
  ON_2dPoint bbmax
)
{
  if (bbmin.IsValid())
  {
    m_bbmin.i = (int)floor(bbmin.x);
    m_bbmin.j = (int)floor(bbmin.y);
  }
  if (bbmax.IsValid())
  {
    m_bbmax.i = (int)floor(bbmax.x);
    m_bbmax.j = (int)floor(bbmax.y);
  }  
}


const ON_TextBox ON_TextBox::Scale(
  const ON_TextBox& text_box,
  double scale
)
{
  ON_TextBox scaled_box = text_box;
  if ( scale > 0.0 && fabs(scale-1.0) > ON_ZERO_TOLERANCE )
  {
    if ( ON_UNSET_INT_INDEX != text_box.m_bbmin.i)
      scaled_box.m_bbmin.i = (int)floor(scale*text_box.m_bbmin.i);
    if ( ON_UNSET_INT_INDEX != text_box.m_bbmin.j)
      scaled_box.m_bbmin.j = (int)floor(scale*text_box.m_bbmin.j);
    if ( ON_UNSET_INT_INDEX != text_box.m_bbmax.i)
      scaled_box.m_bbmax.i = (int)ceil(scale*text_box.m_bbmax.i);
    if ( ON_UNSET_INT_INDEX != text_box.m_bbmax.j)
      scaled_box.m_bbmax.j = (int)ceil(scale*text_box.m_bbmax.j);

    if ( ON_UNSET_INT_INDEX != text_box.m_max_basepoint.i)
      scaled_box.m_max_basepoint.i = (int)ceil(scale*text_box.m_max_basepoint.i); // ceil is correct m_max_basepoint.i increases to the left
    if ( ON_UNSET_INT_INDEX != text_box.m_max_basepoint.j)
      scaled_box.m_max_basepoint.j = (int)floor(scale*text_box.m_max_basepoint.j); // floor is correct m_max_basepoint.j decreases with each line
    
    // currently rounding to nearest int
    if ( ON_UNSET_INT_INDEX != text_box.m_advance.i)
      scaled_box.m_advance.i = (int)floor(scale*text_box.m_advance.i + 0.5);
    if ( ON_UNSET_INT_INDEX != text_box.m_advance.j)
      scaled_box.m_advance.j = (int)floor(scale*text_box.m_advance.j + 0.5);
  }

  return scaled_box;
}

static int Internal_TextBoxMinMax(
  bool bMaximize,
  int lhs,
  int rhs
)
{
  int i = lhs;
  if (ON_UNSET_INT_INDEX == i)
    i = rhs;
  else if (ON_UNSET_INT_INDEX != rhs)
  {
    if (bMaximize)
    {
      if (i < rhs)
        i = rhs;
    }
    else
    {
      if (i > rhs)
        i = rhs;
    }
  }
  return i;
}

const ON_TextBox ON_TextBox::Union(
  const ON_TextBox& lhs,
  const ON_TextBox& rhs
)
{
  ON_TextBox u;

  u.m_bbmin.i = Internal_TextBoxMinMax(false, lhs.m_bbmin.i, rhs.m_bbmin.i);
  u.m_bbmin.j = Internal_TextBoxMinMax(false, lhs.m_bbmin.j, rhs.m_bbmin.j);

  u.m_bbmax.i = Internal_TextBoxMinMax(true, lhs.m_bbmax.i, rhs.m_bbmax.i);
  u.m_bbmax.j = Internal_TextBoxMinMax(true, lhs.m_bbmax.j, rhs.m_bbmax.j);

  u.m_max_basepoint.i = Internal_TextBoxMinMax(true,  lhs.m_max_basepoint.i, rhs.m_max_basepoint.i);
  u.m_max_basepoint.j = Internal_TextBoxMinMax(false, lhs.m_max_basepoint.j, rhs.m_max_basepoint.j);

  u.m_advance.i = 0;
  u.m_advance.j = 0;

  return u;
}

const ON_TextBox ON_TextBox::Translate(
  const ON_TextBox& text_box,
  const ON_2dVector& delta
)
{
  // When delta components are not integer valued, 
  // the size of the box often increases by 1.
  ON_TextBox t = text_box;

  // Translate modifies point locations m_bbmin, m_bbmax, m_max_basepoint.
  // Translate does not modify the m_advance vector direction.

  if (ON_IsValid(delta.x) && 0.0 != delta.x)
  {
    if (ON_UNSET_INT_INDEX != t.m_bbmin.i)
      t.m_bbmin.i = (int)floor(t.m_bbmin.i + delta.x);
    if (ON_UNSET_INT_INDEX != t.m_bbmax.i)
      t.m_bbmax.i = (int)ceil(t.m_bbmax.i + delta.x);
    if (ON_UNSET_INT_INDEX != t.m_max_basepoint.i)
      t.m_max_basepoint.i = (int)ceil(t.m_max_basepoint.i + delta.x);
  }

  if (ON_IsValid(delta.y) && 0.0 != delta.y)
  {
    if (ON_UNSET_INT_INDEX != t.m_bbmin.j)
      t.m_bbmin.j = (int)floor(t.m_bbmin.j + delta.y);
    if (ON_UNSET_INT_INDEX != t.m_bbmax.j)
      t.m_bbmax.j = (int)ceil(t.m_bbmax.j + delta.y);
    if (ON_UNSET_INT_INDEX != t.m_max_basepoint.j)
      t.m_max_basepoint.j = (int)floor(t.m_max_basepoint.j + delta.y);
  }


  return t;
}

const ON_TextBox ON_TextBox::Translate(
  const ON_TextBox& text_box,
  const ON_2dex& delta
)
{
  ON_TextBox t = text_box;

  // Translate modifies point locations m_bbmin, m_bbmax, m_max_basepoint.
  // Translate does not modify the m_advance vector direction.

  if (ON_UNSET_INT_INDEX != delta.i && 0 != delta.i)
  {
    if (ON_UNSET_INT_INDEX != t.m_bbmin.i)
      t.m_bbmin.i += delta.i;
    if (ON_UNSET_INT_INDEX != t.m_bbmax.i)
      t.m_bbmax.i += delta.i;
    if (ON_UNSET_INT_INDEX != t.m_max_basepoint.i)
      t.m_max_basepoint.i += delta.i;
  }

  if (ON_UNSET_INT_INDEX != delta.j && 0 != delta.j)
  {
    if (ON_UNSET_INT_INDEX != t.m_bbmin.j)
      t.m_bbmin.j += delta.j;
    if (ON_UNSET_INT_INDEX != t.m_bbmax.j)
      t.m_bbmax.j += delta.j;
    if (ON_UNSET_INT_INDEX != t.m_max_basepoint.j)
      t.m_max_basepoint.j += delta.j;
  }

  // Translate does not modify the m_advance vector.

  return t;
}


//-----------------------------------------------------------------

ON_Internal_FontGlyphPool::ON_Internal_FontGlyphPool()
{
  ON_FixedSizePool::Create(sizeof(ON_FontGlyph), 0, 0);
}

ON_FontGlyph* ON_FontGlyph::Internal_AllocateManagedGlyph(
  const ON_FontGlyph& src
  )
{
  if ( false == src.CodePointIsSet() || src.IsManaged() )
  {
    ON_ERROR("invalid glyph parameter.");
    return nullptr;
  }

  // managed glyphs are app resources, allocated once per instance and never freed.
  ON_MemoryAllocationTracking disable_tracking(false); 

  ON_FontGlyph* managed_glyph = (ON_FontGlyph*)ON_Internal_FontGlyphPool::theGlyphItemPool.AllocateElement();
  if (nullptr != managed_glyph)
  {
    managed_glyph = new (managed_glyph)ON_FontGlyph();
    *managed_glyph = src;
    managed_glyph->m_is_managed = 1;
  }
  return managed_glyph;
}

ON_GlyphMap::ON_GlyphMap()
{}

static int Internal_CompareGlyphItemCodepoint(const void* lhs, const void* rhs)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return -1;
  if (nullptr == rhs)
    return 1;
  const ON_FontGlyph* a = *((const ON_FontGlyph *const *)lhs);
  const ON_FontGlyph* b = *((const ON_FontGlyph *const *)rhs);
  if (a == b)
    return 0;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;

  const unsigned int a_code_point = a->CodePoint();
  const unsigned int b_code_point = b->CodePoint();
  if (a_code_point < b_code_point)
    return -1;
  if (a_code_point > b_code_point)
    return 1;

  return 0;
}

const ON_FontGlyph* ON_GlyphMap::FindGlyph(const ON__UINT32 unicode_codepoint) const
{
  if ( false == ON_IsValidUnicodeCodePoint(unicode_codepoint) )
    return nullptr; // invalid codepoint
  const unsigned count = m_glyphs.UnsignedCount();
  if (unicode_codepoint < 256)
  {
    // codepoints up to 255 are saved by m_glyphs[] index.
    return
      (unicode_codepoint < count)
      ? m_glyphs[unicode_codepoint]
      : nullptr;
  }

  if (count > 256)
  {
    // binary search for codepoint values >= 256
    if (unicode_codepoint >= m_glyphs[256]->m_code_point && unicode_codepoint <= m_glyphs[count-1]->m_code_point)
    {
      ON_FontGlyph cp;
      cp.m_code_point = unicode_codepoint;
      const ON_FontGlyph* p = &cp;
      const ON_FontGlyph *const * a = m_glyphs.Array() + 256;
      const ON_FontGlyph *const * b = (const ON_FontGlyph *const *)bsearch(&p, a, count - 256, sizeof(*a), Internal_CompareGlyphItemCodepoint);
      const ON_FontGlyph* gi
        = (nullptr != b)
        ? *b
        : nullptr;

      return gi;
    }
  }

  return nullptr;
}

const ON_FontGlyph* ON_GlyphMap::InsertGlyph(const ON_FontGlyph& glyph )
{
  // managed glyphs are app resources - 1 per glpyh as needed and never freed.
  ON_MemoryAllocationTracking disable_tracking(false);

  if ( glyph.IsManaged() )
  {
    ON_ERROR("invalid glyph parameter");
    return nullptr;
  }

  if (false == glyph.CodePointIsSet())
  {
    ON_ERROR("glyph.CodePointIsSet() is false.");
    return nullptr;
  }

  const int base_count = 256;

  if (0 == m_glyphs.Count())
  {
    // codepoints 0 to base_count-1 are in m_glyphs[] by code point value.
    m_glyphs.Reserve(base_count);
    m_glyphs.SetCount(base_count);
    m_glyphs.Zero();
  }

  const int count = m_glyphs.Count();
  
  const ON__UINT32 code_point = glyph.m_code_point;
  int gdex;
  if (code_point < base_count)
  {
    gdex = (int)code_point;
    // codepoints up to 255 are saved by m_glyphs[] index.
    const ON_FontGlyph* gi = m_glyphs[gdex];
    if (nullptr != m_glyphs[gdex])
    {
      ON_ERROR("code point is already cached.");
      return gi; // codepoint already in the map
    }
  }
  else
  {
    for (gdex = base_count; gdex < count; gdex++)
    {
      const ON_FontGlyph* gi = m_glyphs[gdex];
      if (nullptr == gi)
        continue;
      if (gi->m_code_point == code_point)
      {
        ON_ERROR("code point is already cached.");
        return gi;  // codepoint already in the map
      }
      if (gi->m_code_point > code_point)
        break;
    }
  }

  ON_FontGlyph* managed_glyph = ON_FontGlyph::Internal_AllocateManagedGlyph(glyph);
  if (nullptr == managed_glyph)
  {
    ON_ERROR("theGlyphItemPool.AllocateUnsetGlyph() returned nullptr.");
    return nullptr;
  }

  if (gdex < base_count)
    m_glyphs[gdex] = managed_glyph;
  else if ( gdex < count )
    m_glyphs.Insert(gdex, managed_glyph);
  else
    m_glyphs.Append(managed_glyph);

  return managed_glyph;
}

unsigned int ON_GlyphMap::GlyphCount() const
{
  return m_glyph_count;
}

ON_FontGlyphOutlinePoint::ContourPointType ON_FontGlyphOutlinePoint::ContourPointTypeFromUnsigned(
  unsigned contour_point_type_as_unsigned
)
{
  switch (contour_point_type_as_unsigned)
  {
  ON_ENUM_FROM_UNSIGNED_CASE(ON_FontGlyphOutlinePoint::ContourPointType::Unset);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_FontGlyphOutlinePoint::ContourPointType::MoveTo);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_FontGlyphOutlinePoint::ContourPointType::LineTo);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_FontGlyphOutlinePoint::ContourPointType::QuadraticBezierPoint);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_FontGlyphOutlinePoint::ContourPointType::CubicBezierPoint);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_FontGlyphOutlinePoint::ContourPointType::LineToCloseContour);
  }

  ON_ERROR("Invalid contour_point_type_as_unsigned parameter.");
  return ON_FontGlyphOutlinePoint::ContourPointType::Unset;
}
