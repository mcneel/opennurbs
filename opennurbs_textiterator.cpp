
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

#include "opennurbs_textiterator.h"


static bool IsAtoZ(ON__UINT32 ch)
{
  return ( ch < 0xD800 && iswalpha((wchar_t)ch)) ? true : false;
}

//static bool IsHexDigit(ON__UINT32 ch)
//{
//  if ( '0' <= ch && ch <= '9')
//    return true;
//  if ( 'A' <= ch && ch <= 'F')
//    return true;
//  if ( 'a' <= ch && ch <= 'f')
//    return true;
//  return false;
//}

static bool IsDigit(ON__UINT32 ch)
{
  return ( ch < 0xD800 && iswdigit((wchar_t)ch)) ? true : false;
}

// Rtf tagnames
static const wchar_t* tagRtf                 = L"rtf";
static const wchar_t* tagFontTable           = L"fonttbl";
static const wchar_t* tagDefaultFont         = L"deff";
static const wchar_t* tagFont                = L"f";
static const wchar_t* tagFontSize            = L"fs";
static const wchar_t* tagCharSet             = L"fcharset";
static const wchar_t* tagCodePage            = L"cpg";

static const wchar_t* tagNewline             = L"line";
static const wchar_t* tagParagraph           = L"par";
static const wchar_t* tagParagraphDefaults   = L"pard";
static const wchar_t* tagSection             = L"sect";
static const wchar_t* tagTabulator           = L"tab";
                                    
static const wchar_t* tagBold                = L"b";
static const wchar_t* tagItalic              = L"i";
static const wchar_t* tagUnderLine           = L"ul";
static const wchar_t* tagUnderLineNone       = L"ulnone";
static const wchar_t* tagStrikeThrough       = L"strike";
                                    
static const wchar_t* tagSuperscript         = L"super";
static const wchar_t* tagSubscript           = L"sub";
static const wchar_t* tagNoSuperSub          = L"nosupersub";
                                    
//static const wchar_t* tagAlignLeft           = L"ql";
//static const wchar_t* tagAlignCenter         = L"qc";
//static const wchar_t* tagAlignRight          = L"qr";
//static const wchar_t* tagAlignJustify        = L"qj";
                                    
static const wchar_t* tagColorTable          = L"colortbl";
static const wchar_t* tagColorRed            = L"red";
static const wchar_t* tagColorGreen          = L"green";
static const wchar_t* tagColorBlue           = L"blue";
static const wchar_t* tagColorForeground     = L"cf";
static const wchar_t* tagColorBackground     = L"cb";
//static const wchar_t* tagColorBackgroundWord = L"chcbpat";
//static const wchar_t* tagColorHighlight      = L"highlight";


//static const wchar_t* tagExColorTable        = L"expandedcolortbl";
//static const wchar_t* tagTextHeight          = L"h";      // not rtf
static const wchar_t* tagStackFraction       = L"stackheight"; // not rtf - scale factor for text height in stacked text
static const wchar_t* tagStackText           = L"stack";  // not rtf - begin stacked fraction
static const wchar_t* tagStackEnd            = L"stnone"; // not rtf - end stacked fraction
static const wchar_t* tagField               = L"field";

static const wchar_t* tagUniCpCount          = L"uc";     // #bytes used following \uN for codepage code of equivalenet char

static const wchar_t* tagUniCharDec          = L"u";      // UNOCODE UTF-16 encoded value as a signed short (0x8...) will be -....
                                                          // NOTE WELL: When a single UNICODE code point requires a UTF-16
                                                          // surrogate pair encoding, there will be TWO \uXXXX? values for that code point.
                                                          // For example, the single UNICODE code point 
                                                          // ON_UnicodeCodePoint::Wastebasket U+1F5D1 (decimal 128465)
                                                          // The UTF-16 surrogate pair for U+1F5D1 is (0xD83D, 0xDDD1) and this 
                                                          // value in RTF looks like ...{\ltrch \u-10179?\u-8751?}...
                                                          // -10179 as a signed 2 byte short has the same bits as unsigned short 0xD83D.
                                                          // -8751 as a signed 2 byte short has the same bits as unsigned short 0xDDD1.
                                                          // Many "emoji glyphs" UNOCODE code points require UTF-16 surrogate pair encodings.

static const wchar_t* tagUniTwoDest          = L"upr";    // two embedded unicode destinations

static const wchar_t* tagUniDest             = L"ud";     // unicode destination

// Characters typed with Alt+0123
// \lquote \rquote  \ldblquote  \rdblquote  \bullet  \endash  \emdash
static const wchar_t* taglquote              = L"lquote";        // left quote
static const wchar_t* tagrquote              = L"rquote";        // right quote
static const wchar_t* tagldblquote           = L"ldblquote";     // left double quote
static const wchar_t* tagrdblquote           = L"rdblquote";     // right double quote
static const wchar_t* tagbullet              = L"bullet";        // bullet
static const wchar_t* tagendash              = L"endash";        // endash
static const wchar_t* tagemdash              = L"emdash";        // emdash


#pragma region TextIterator

ON_TextIterator::ON_TextIterator(const ON_wString& str)
  : m_text(str.Array())
  , m_length(str.Length())
{
  Step(); //  Make first character in the string current
}

ON_TextIterator::ON_TextIterator(const wchar_t* str, size_t length)
  : m_text(str)
  , m_length(length)
{
  Step(); //  Make first character in the string current
}

bool ON_TextIterator::Step()
{
  // Get the next UNICODE code point encoded in m_text beginning at m_text[m_next_text_ci];
  // Save this code point in m_cur_codepoint.
  if(m_next_text_ci < m_length)
  {
    m_ue.m_error_status = 0;
    ON__UINT32 codepoint = 0;
    
    // Works on Windows, Apple, and any other platform.
    const int ci = ON_DecodeWideChar((m_text+m_next_text_ci), (int)(m_length-m_next_text_ci), &m_ue, &codepoint);

    if(ci > 0)
    {
      m_prev_text_ci = m_cur_text_ci;
      m_cur_text_ci = m_next_text_ci;
      m_next_text_ci = m_next_text_ci+ci;
      m_prev_codepoint = m_cur_codepoint;
      m_cur_codepoint = codepoint;
      return true;
    }
  }

  m_prev_codepoint = m_cur_codepoint;
  m_cur_codepoint = 0;

  return false;
}

bool ON_TextIterator::PeekCodePoint(ON__UINT32& unicode_code_point) const
{
  unicode_code_point = m_cur_codepoint;
  return (0 != unicode_code_point);
}

bool ON_TextIterator::ReadCodePoint(ON__UINT32& unicode_code_point)
{
  unicode_code_point = m_cur_codepoint;
  Step();
  return (0 != unicode_code_point);
}

bool ON_TextIterator::Back()
{
  if(m_prev_text_ci == m_cur_text_ci)
  {
    // fancy backup in wchar_t string
  }
  m_next_text_ci = m_cur_text_ci;
  m_cur_text_ci = m_prev_text_ci;
  m_cur_codepoint = m_prev_codepoint;
  return true;
}

static bool Internal_IsHexDigit(ON__UINT32 x)
{
  return ((x >= '0' && x <= '9') || (x >= 'A' && x <= 'F') || (x >= 'a' && x <= 'f'));
}

static ON__UINT32 Internal_IsHexValue(ON__UINT32 x)
{
  if (x >= '0' && x <= '9')
    return (x - '0');
  if (x >= 'A' && x <= 'F')
    return x - 'A' + 10;
  if (x >= 'a' && x <= 'f')
    return x - 'a' + 10;
  return 0xFFFFFFFF;
}

bool ON_TextIterator::AtBackslashTic() const
{
  return (
    ON_UnicodeCodePoint::ON_Backslash == m_cur_codepoint
    && m_next_text_ci < m_length
    && '\'' == m_text[m_next_text_ci]
    );
}

bool ON_TextIterator::ReadCharValue(
  unsigned char& c
)
{
  for(;;)
  {
    ON__UINT32 buffer[4];
    if (false == ReadCodePoint(buffer[0]))
      break;
    if (ON_UnicodeCodePoint::ON_Backslash != buffer[0])
      break;
    if (false == ReadCodePoint(buffer[1]))
      break;
    if ('\'' != buffer[1])
      break;
    if (false == ReadCodePoint(buffer[2]))
      break;
    if (false == Internal_IsHexDigit(buffer[2]))
      break;
    if (false == ReadCodePoint(buffer[3]))
      break;
    if (false == Internal_IsHexDigit(buffer[3]))
      break;
    ON__UINT32 x = 16 * Internal_IsHexValue(buffer[2]) + Internal_IsHexValue(buffer[3]);
    if (x > 255)
      break;
    c = (unsigned char)x;
    return true;
  }
  c = 0;
  return false;
}

ON__UINT32 ON_RtfParser::Internal_ParseMBCSString( const ON__UINT32 windows_code_page )
{
  ON__UINT32 count = 0;
  bool bParsed = false;
  ON_String mbcs;
  mbcs.ReserveArray(64);
  while (m_ti.AtBackslashTic())
  {
    count++;
    unsigned char c;
    bParsed = m_ti.ReadCharValue(c);
    if (false == bParsed)
      break;
    mbcs.Append((const char*)(&c), 1);
  }

  const char* sMBCS = static_cast<const char*>(mbcs);
  int sMBCS_count = mbcs.Length();
  wchar_t* sWideChar = nullptr;
  int sWideChar_capacity = 0;
  unsigned int error_status = 0;

  if (nullptr != sMBCS && 0 != sMBCS[0] && sMBCS_count > 0)
  {
    const int sWideChar_count0 = ON_ConvertMSMBCPToWideChar(
      windows_code_page,
      sMBCS,
      sMBCS_count,
      sWideChar,
      sWideChar_capacity,
      &error_status
    );

    if (sWideChar_count0 > 0)
    {
      sWideChar_capacity = sWideChar_count0 + 2;
      sWideChar = new wchar_t[sWideChar_capacity];
      if (nullptr != sWideChar)
      {
        memset(sWideChar, 0, sWideChar_capacity * sizeof(sWideChar[0]));
      }
      error_status = 0;
      const int sWideChar_count1 = ON_ConvertMSMBCPToWideChar(
        windows_code_page,
        sMBCS,
        sMBCS_count,
        sWideChar,
        sWideChar_capacity-1,
        &error_status
      );

      if (sWideChar_count1 > 0 && 0 != sWideChar[0] && 0 == sWideChar[sWideChar_capacity - 1])
      {
        int delta_i = sWideChar_count1;
        for (int i = 0; i < sWideChar_count1; i += delta_i)
        {
          struct ON_UnicodeErrorParameters e = ON_UnicodeErrorParameters::MaskErrors;
          ON__UINT32 unicode_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter;
          delta_i = ON_DecodeWideChar(sWideChar + i, sWideChar_count1 - i, &e, &unicode_code_point);
          if (delta_i <= 0)
          {
            bParsed = false;
            break;
          }
          m_builder.m_current_codepoints.Append(unicode_code_point);
        }
      }
      delete[] sWideChar;
      sWideChar = nullptr;
    }
  }

  if (false == bParsed)
    m_builder.m_current_codepoints.Append(ON_UnicodeCodePoint::ON_ReplacementCharacter);

  return count;
}

#pragma endregion TextIterator

#pragma region TextBuilder

ON_TextBuilder::~ON_TextBuilder()
{}

ON_TextBuilder::ON_TextBuilder()
  : m_current_codepoints(16)
{
  m_current_UTF16_buffer_count = 0;
  m_current_UTF16_buffer[0] = 0;
  m_current_UTF16_buffer[1] = 0;
}

void ON_TextBuilder::InitBuilder(const ON_Font* default_font)
{
  m_in_run = 0;
  m_level = 0;
  m_font_table_level = -1;
  m_ansi_equiv_chars.Empty();
  m_current_codepoints.Empty();
}

void ON_TextBuilder::GroupBegin()
{
  m_level++;
  if (m_ansi_equiv_chars.Count() > 0)
    m_ansi_equiv_chars.Append(*m_ansi_equiv_chars.Last());
  else
    m_ansi_equiv_chars.Append(1);
}

void ON_TextBuilder::GroupEnd()
{
  if(m_font_table_level >= m_level)
    m_font_table_level = -1;
  m_level--;
  if (m_ansi_equiv_chars.Count() > 0)
    m_ansi_equiv_chars.Remove();
}

void ON_TextBuilder::FlushText(size_t count, ON__UINT32* cp)
{
}

void ON_TextBuilder::BeginFontTable()
{
  m_font_table_level = m_level;
}

void ON_TextBuilder::BeginHeader()
{
}

// Sets m_default_font_index when \deffn is read.
void ON_TextBuilder::DefaultFont(const wchar_t* value)
{
  wchar_t* sp = const_cast< wchar_t* >(value);
  ON__INT32 nval = -1;
  nval = (ON__INT32)::wcstol(value, &sp, 10);
  if(nval >= 0 && sp != value)
    m_default_font_index = nval;
}

// Process a rtf \fn tag to set the current font to the nth font in the rtf font table
void ON_TextBuilder::FontTag(const wchar_t* value)
{
  wchar_t* sp = const_cast< wchar_t* >(value);
  ON__INT32 nval = -1;
  nval = (ON__INT32)::wcstol(value, &sp, 10);
  if(nval >= 0 && sp != value)
  {
    if(m_font_table_level >= 0 && 
       m_level > m_font_table_level)
    {
      // Defining a font in the font table
      // n in \fn is the rtf index of the font currently being read from the table
      // store a mapping from \fn to font
      // when the font for this font table entry is made
      m_font_index = nval;
      // next will be a face name that will be read and put in a fontdef run
    }
    //else
    //{
    //  // Not defining the font table. Rather, setting a font current
    //  // Set current font to font corresponding to font_index
    //}
  }
  return;
}

const ON_wString ON_TextBuilder::FaceNameFromMap(int nval)
{
  int count = m_facename_map.Count();
  for (int mi = 0; mi < count; mi++)
  {
    if (m_facename_map[mi].m_rtf_font_index == nval)
      return m_facename_map[mi].m_facename;
  }
  return ON_wString::EmptyString;
}

unsigned int ON_TextBuilder::CodePageFromMap(int nval)
{
  int count = m_facename_map.Count();
  for (int mi = 0; mi < count; mi++)
  {
    if (m_facename_map[mi].m_rtf_font_index == nval)
      return m_facename_map[mi].m_codepage;
  }
  return 1252;
}

unsigned int ON_TextBuilder::CharSetFromMap(int nval)
{
  int count = m_facename_map.Count();
  for (int mi = 0; mi < count; mi++)
  {
    if (m_facename_map[mi].m_rtf_font_index == nval)
      return m_facename_map[mi].m_charset;
  }
  return 0;
}

void ON_TextBuilder::FontSize(const wchar_t* value)
{
}

void ON_TextBuilder::CharSet(const wchar_t* value)  // \fcharsetN
{
  wchar_t* sp = const_cast< wchar_t* >(value);
  unsigned int charset = (ON__INT32)::wcstol(value, &sp, 10);
  if (charset >= 0 && sp != value)
  {
    if (m_font_table_level >= 0 && m_level > m_font_table_level)
    {
      // This is a charset specification in a font definition in the font table
      // the value is convertable to a codepage to use for interpreting the text chars
      // using this font into unicode points
      m_current_props.SetCharSet(charset, true);
    }
  }
}

void ON_TextBuilder::CodePage(const wchar_t* value)  // \cpgN
{
  wchar_t* sp = const_cast< wchar_t* >(value);
  unsigned int codepage = (ON__INT32)::wcstol(value, &sp, 10);
  if (codepage >= 0 && sp != value)
  {
    if (m_font_table_level >= 0 && m_level > m_font_table_level)
    {
      // This is a codepage specification in a font definition in the font table
      m_current_props.SetCodePage(codepage);
    }
  }
}

void ON_TextBuilder::Newline()
{
}

void ON_TextBuilder::Paragraph()
{
}

void ON_TextBuilder::ParagraphDefaults()
{
}

void ON_TextBuilder::Section()
{
}

void ON_TextBuilder::Tab()
{
}

void ON_TextBuilder::Bold(const wchar_t* value)
{
}

void ON_TextBuilder::Italic(const wchar_t* value)
{
}

void ON_TextBuilder::UnderlineOn()
{
}

void ON_TextBuilder::UnderlineOff()
{
}

void ON_TextBuilder::Strikethrough(const wchar_t* value)
{
}

void ON_TextBuilder::Superscript()
{
}

void ON_TextBuilder::Subscript()
{
}

void ON_TextBuilder::NoSuperSub()
{
}

void ON_TextBuilder::BeginColorTable()
{
}

void ON_TextBuilder::ColorRed(const wchar_t* value)
{
}

void ON_TextBuilder::ColorGreen(const wchar_t* value)
{
}

void ON_TextBuilder::ColorBlue(const wchar_t* value)
{
}

void ON_TextBuilder::ColorForeground(const wchar_t* value)
{
}

void ON_TextBuilder::ColorBackground(const wchar_t* value)
{
}

void ON_TextBuilder::SetStackScale(const wchar_t* value)
{
}

void ON_TextBuilder::StackFraction(const wchar_t* value)
{
}

void ON_TextBuilder::StackEnd()
{
}

void ON_TextBuilder::TextField(const wchar_t* name)
{
}

ON__UINT32* ON_TextBuilder::RunCodePoints(const ON_TextRun& run)
{
  return run.m_codepoints;
}

void ON_TextBuilder::UniCpCount(const wchar_t* value)
{
  // #bytes used following \uN for codepage code of equivalenet char
  short count = -1;
  if (nullptr != ON_wString::ToNumber(value, -1, &count))
  {
    if (m_ansi_equiv_chars.Count() > 0)
      *m_ansi_equiv_chars.Last() = count;
  }
}

void ON_TextBuilder::UniDecimal(const wchar_t* value)
{
  // UTF-16 encoding value as a decimal, possibly signed negative
  // unicode char in decimal followed by ascii char(s) (to skip if unicode can't be digested) :
  // \u1472? means char at unicode point 1472, or '?' as a fallback ascii char to substitute.
  // the decimal value may be written as a negative number: \u-2671? and should be cast to unsigned.
  // The number of fallback chars to skip after the decimal number is set by the N in \ucN
  // and is stored here in m_ansi_equiv_chars.Last() (default 1)
  short d = INT16_MIN;
  ON_wString::ToNumber(value, INT16_MIN, &d);
  if (INT16_MIN != d)
  {
    bool bError = false;
    const ON__UINT32 error_cp = ON_UnicodeCodePoint::ON_ReplacementCharacter;

    const ON__UINT16 waiting_mark = ON_TextBuilder::m_UFT16_waiting_mark;
    const ON__UINT16 unused_mark  = ON_TextBuilder::m_UFT16_unused_mark;

    const bool bHaveFirstSurrogate
      = (1 == m_current_UTF16_buffer_count
        && m_current_UTF16_buffer[0] >= 0xD800
        && m_current_UTF16_buffer[0] < 0xDC00
        && waiting_mark == m_current_UTF16_buffer[1]);

    ON__UINT32 cp = 0;
    ON__UINT16 u = (unsigned short)d;
    // Mar 3, 2017 Dale Lear
    //   Attempt to support handling of UTF-16 surrogate pairs without
    //   rewriting the RTF parser.
    // For example, the single UNICODE code point ON_UnicodeCodePoint::Wastebasket U+1F5D1 (decimal 128465)
    // is in the RTF string as ...{\ltrch \u-10179?\u-8751?}...
    // The UTF-16 surrogate pair is (0xD83D, 0xDDD1) and this value decodes to the single UNICODE code point U+1F5D1
    //   \u-10179? -> unsigned short 0xD83D
    //   \u-8751?  -> unsigned short 0xDDD1
    if (u >= 0xD800 && u < 0xDC00)
    {
      if (bHaveFirstSurrogate)
      {
        // never got the second value for the pair
        m_current_codepoints.Append(error_cp);
      }

      if (0 == m_current_UTF16_buffer_count)
      {
        m_current_UTF16_buffer_count = 1;
        m_current_UTF16_buffer[0] = u;
        m_current_UTF16_buffer[1] = waiting_mark;
        return; // we need the second surrogate pair value before we can decode
      }

      bError = true;
    }
    else if (u >=  0xDC00 && u < 0xE000)
    {
      if (bHaveFirstSurrogate)
      {
        m_current_UTF16_buffer_count = 2;
        m_current_UTF16_buffer[1] = u;
      }
      else
      {
        // We should have gotten the second value for the pair
        bError = true;
      }
    }
    else
    {
      if (bHaveFirstSurrogate)
      {
        // never got the second value for the pair
        m_current_codepoints.Append(error_cp);
      }
      m_current_UTF16_buffer_count = 1;
      m_current_UTF16_buffer[0] = u;
      m_current_UTF16_buffer[1] = unused_mark;
    }

    if (false == bError)
    {
      ON_UnicodeErrorParameters e;
      e.m_error_code_point = error_cp;
      e.m_error_mask = 16;
      e.m_error_status = 0;
      const int rc = ON_DecodeUTF16(m_current_UTF16_buffer, m_current_UTF16_buffer_count, &e, &cp);
      bError = (m_current_UTF16_buffer_count != rc || false == ON_IsValidUnicodeCodePoint(cp));
    }

    if (bError)
      cp = error_cp;
    m_current_codepoints.Append(cp);

    m_current_UTF16_buffer_count = 0;
    m_current_UTF16_buffer[0] = unused_mark;
    m_current_UTF16_buffer[1] = unused_mark;
  }
}

void ON_TextBuilder::UniEmbeddedDest(const wchar_t* value)
{
  // two embedded unicode destinations - footnote, etc in rtf document
}

void ON_TextBuilder::UniDest(const wchar_t* value)
{
  // unicode destination
}

void ON_TextBuilder::LQuote()
{
  m_current_codepoints.Append(0x2018);
}

void ON_TextBuilder::RQuote()
{
  m_current_codepoints.Append(0x2019);
}

void ON_TextBuilder::LDblQuote()
{
  m_current_codepoints.Append(0x201c);
}

void ON_TextBuilder::RDblQuote()
{
  m_current_codepoints.Append(0x201d);
}

void ON_TextBuilder::Bullet()
{
  m_current_codepoints.Append(0x2022);
}

void ON_TextBuilder::EnDash()
{
  m_current_codepoints.Append(0x2013);
}

void ON_TextBuilder::EmDash()
{
  m_current_codepoints.Append(0x2014);
}

bool ON_TextBuilder::AppendCodePoint(ON__UINT32 codept)
{
  m_current_codepoints.Append(codept);
  return true;
}

#pragma endregion  // TextBuilder



#pragma region TextRunBuilder

ON_TextRunBuilder::~ON_TextRunBuilder()
{}

ON_TextRunBuilder::ON_TextRunBuilder(
  ON_TextContent& text,
  ON_TextRunArray& runs,
  const ON_DimStyle* dimstyle,
  double height,
  ON_Color color)
  : m_runs(runs)
  , m_text(text)
{
  m_current_UTF16_buffer_count = 0;
  m_current_UTF16_buffer[0] = 0;
  m_current_UTF16_buffer[1] = 0;

  if (nullptr == dimstyle)
    dimstyle = &ON_DimStyle::Default;
  const ON_Font& style_font = dimstyle->Font();
  double stackscale = dimstyle->StackHeightScale();
  ON_DimStyle::stack_format stackformat = dimstyle->StackFractionFormat();
  bool bold = dimstyle->Font().IsBold();
  bool italic = dimstyle->Font().IsItalic();
  bool underlined = dimstyle->Font().IsUnderlined();
  bool strikethrough = dimstyle->Font().IsStrikethrough();

  m_current_font = &style_font;
  m_current_props.SetColor(color);
  m_current_props.SetHeight(height);
  m_current_props.SetStackScale(stackscale);
  m_current_props.SetStackFormat(stackformat);
  m_current_props.SetBold(bold);
  m_current_props.SetItalic(italic);
  m_current_props.SetUnderlined(underlined);
  m_current_props.SetStrikethrough(strikethrough);

  m_current_run.Init(m_current_font, m_current_props.Height(), m_current_props.StackScale(), m_current_props.Color(),
    m_current_props.IsBold(), m_current_props.IsItalic(), m_current_props.IsUnderlined(), m_current_props.IsStrikethrough());
}

void ON_TextRunBuilder::InitBuilder(const ON_Font* default_font)
{
  if (nullptr == default_font)
    default_font = &ON_Font::Default;
  if (nullptr == default_font)
    return;
  m_current_font = default_font;  // copy of default_font

  m_in_run = 0;
  m_level = 0;
  m_font_table_level = -1;
  m_runs = ON_TextRunArray::EmptyArray;
  m_current_run.Init(m_current_font, m_current_props.Height(), m_current_props.StackScale(), m_current_props.Color(),
    m_current_props.IsBold(), m_current_props.IsItalic(), m_current_props.IsUnderlined(), m_current_props.IsStrikethrough());
  m_ansi_equiv_chars.Empty();
  // Array for accumulating text codepoints
  m_current_codepoints.Empty();
}

void ON_TextRunBuilder::AppendCurrentRun()
{
  //if(m_current_run.IsValid())
  {
    ON_TextRun* run = ON_TextRun::GetManagedTextRun(m_current_run);
    if (0 != run)
    {
      m_runs.AppendRun(run);
    }
  }
}


#ifndef ON_TEXT_BRACKET_FRACTION
static bool FindFrac(const wchar_t* wstr, int start, int spos, int& topstart, int& bottomend)
{
  if (spos < start + 1)
    return false;
  int len = (int)wcslen(wstr);
  if (spos > len - 2)
    return false;
  topstart = spos;
  bottomend = spos;
  for (int i = spos; i > start && isdigit(wstr[i - 1]); i--)
  {
    topstart = i - 1;
  }
  bottomend = spos;
  for (int i = spos; i < len - 1 && isdigit(wstr[i + 1]); i++)
  {
    bottomend = i + 1;
  }
  if (topstart == spos || bottomend == spos)
    return false;

  if (topstart > start + 2 && isdigit(wstr[topstart - 2]))
  {//drop leading space
  }
  if (bottomend < len - 2 && isdigit(wstr[bottomend + 2]))
  {// drop trailing space
  }
  return true;
}

typedef struct tagFract
{
  int start;
  int slash;
  int end;
} Fraction;

static void FindFractions(ON_TextRun* run, ON_SimpleArray<Fraction>& fractlist)
{
  if (nullptr == run)
    return;
  size_t cpcount = ON_TextRun::CodepointCount(run->UnicodeString());
  if (0 != cpcount)
  {
    ON_wString str;
    ON_TextContext::ConvertCodepointsToString((int)cpcount, run->UnicodeString(), str);
    if (!str.IsEmpty())
    {
      const wchar_t* wstr = str.Array();
      int len = str.Length();
      int start = 0;
      int topstart = -1;
      int bottomend = -1;
      int i = 0;
      while (i < len && 0 != wstr[i])
      {
        if (wstr[i] == L'/')
        {
          if (FindFrac(wstr, start, i, topstart, bottomend))
          {
            Fraction& fract = fractlist.AppendNew();
            fract.start = topstart;
            fract.end = bottomend;
            fract.slash = i;
            i = bottomend;
            start = bottomend + 1;
          }
        }
        i++;
      }
    }
  }
}
#endif

void ON_TextRunBuilder::FinishCurrentRun()
{
  //int cplen = m_current_run.m_codepoint_idx.j - m_current_run.m_codepoint_idx.i;
  if (m_current_run.Type() == ON_TextRun::RunType::kText ||
    m_current_run.Type() == ON_TextRun::RunType::kField ||
    m_current_run.Type() == ON_TextRun::RunType::kNewline ||
    m_current_run.Type() == ON_TextRun::RunType::kParagraph)
  {
    // Finish off the text run - 
    // Text string is already stored in m_codepoints
    // Find or make a managed font like the current one
    // and store that pointer on the run
    if (nullptr == m_current_font)
      m_current_font = &ON_Font::Default;
    const ON_Font* pManagedFont = m_current_font->ManagedFont();

    if (nullptr != pManagedFont)
    {
      m_current_run.SetFont(pManagedFont);
      m_current_run.SetColor(m_current_props.Color());
      m_current_run.SetTextHeight(m_current_props.Height());
      m_current_run.SetStackFractionHeight(m_current_props.StackScale());
    }
#ifndef ON_TEXT_BRACKET_FRACTION
    ON_SimpleArray<Fraction> fractlist;
    int fcount = 0;
    if (ON_DimStyle::stack_format::None != m_current_props.StackFormat())
    {
      FindFractions(&m_current_run, fractlist);
      fcount = fractlist.Count();
    }
    if (0 < fcount)
    {
      int cpos = 0;
      int cpcount = (int)ON_TextRun::CodepointCount(m_current_run.m_codepoints);
      ON__UINT32* cpts = (ON__UINT32*)onmalloc((1 + cpcount) * sizeof(ON__UINT32));
      memcpy(cpts, m_current_run.m_codepoints, (1 + cpcount) * sizeof(ON__UINT32));
      for (int i = 0; i < fcount; i++)
      {
        Fraction& f = fractlist[i];
        if (f.start >= cpos)
        {
          int k = f.start;
          // If the last char before the fraction is a space
          // and the char before that is a digit, drop the space
          if (f.start > 1 && isdigit(cpts[f.start - 1]))
            k = f.start - 1;
          m_current_run.SetUnicodeString(k - cpos, cpts + cpos);
          AppendCurrentRun();

          cpos = f.start;
          m_current_run.SetUnicodeString(f.end - f.start + 1, cpts + cpos);
          if (m_current_props.StackFormat() != ON_DimStyle::stack_format::None)
            m_current_run.SetStacked(ON_TextRun::Stacked::kStacked);
          else
            m_current_run.SetStacked(ON_TextRun::Stacked::kNone);
          AppendCurrentRun();
          m_current_run.SetStacked(ON_TextRun::Stacked::kNone);
          cpos = f.end + 1;
        }
      }
      if (cpos < cpcount)
      {
        m_current_run.SetUnicodeString(cpcount - cpos, cpts + cpos);
        AppendCurrentRun();
      }
    }
    else

#endif // 0
      AppendCurrentRun();
  }
  else if (m_current_run.Type() == ON_TextRun::RunType::kFontdef)
  {
    // String is a font facename. Make a font with that facename
    // and a font definition run 
    size_t cpcount = ON_TextRun::CodepointCount(RunCodePoints(m_current_run));
    if (0 != cpcount)
    {
      ON_wString str;
      ON_TextContext::ConvertCodepointsToString((int)cpcount, RunCodePoints(m_current_run), str);
      if (!str.IsEmpty())
      {
        str.Remove(L';');  // facename delimiter from rtf
        ON_FaceNameKey& fn_key = m_facename_map.AppendNew();
        fn_key.m_rtf_font_index = m_font_index;
        fn_key.m_facename = str;
        fn_key.m_codepage = m_current_props.CodePage();
        fn_key.m_charset = m_current_props.CharSet();
      }
    }
  }
}

void ON_TextRunBuilder::GroupBegin()
{
  int cp32_count = m_current_codepoints.Count();
  if (cp32_count > 0)
  {
    FlushText(cp32_count, m_current_codepoints.Array());
    m_current_codepoints.Empty();
  }
  FinishCurrentRun();

  m_level++;
  // m_current_font starts out as the value from the text object
  m_font_stack.Append(m_current_font);    // push prev current font
  m_prop_stack.Append(m_current_props);   // push prev current height and color

  m_current_run.Init(m_current_font, m_current_props.Height(), m_current_props.StackScale(), m_current_props.Color(),
    m_current_props.IsBold(), m_current_props.IsItalic(), m_current_props.IsUnderlined(), m_current_props.IsStrikethrough());
  if (m_ansi_equiv_chars.Count() > 0)
    m_ansi_equiv_chars.Append(*m_ansi_equiv_chars.Last());
  else
    m_ansi_equiv_chars.Append(1);
}


void ON_TextRunBuilder::GroupEnd()
{
  int cp32_count = m_current_codepoints.Count();
  if (cp32_count > 0)
  {
    FlushText(cp32_count, m_current_codepoints.Array());
    m_current_codepoints.Empty();
  }
  FinishCurrentRun();
  // Pop font and set up for next run
  if (m_font_stack.Count() > 0 && m_prop_stack.Count() > 0)
  {
    m_current_font = *m_font_stack.Last();  // pop
    m_font_stack.Remove();
    m_current_props = *m_prop_stack.Last(); // pop
    m_prop_stack.Remove();
  }
  m_current_run.Init(m_current_font, m_current_props.Height(), m_current_props.StackScale(), m_current_props.Color(),
    m_current_props.IsBold(), m_current_props.IsItalic(), m_current_props.IsUnderlined(), m_current_props.IsStrikethrough());

  if (m_font_table_level >= m_level)
    m_font_table_level = -1;
  m_level--;
  if (m_ansi_equiv_chars.Count() > 0)
    m_ansi_equiv_chars.Remove();
}

void ON_TextRunBuilder::FlushText(size_t count, ON__UINT32* cp)
{
  if (count < 1 || 0 == cp || 0 == cp[0])
    return;

  if (m_font_table_level > -1 && m_level >= m_font_table_level)
  {
    // String is a font facename. Make a font with that facename
    // and a font definition run 
    m_current_run.SetUnicodeString(count, cp);
    m_current_run.SetType(ON_TextRun::RunType::kFontdef);


    ON_wString str;
    ON_TextContext::ConvertCodepointsToString((int)count, cp, str);
    if (!str.IsEmpty())
    {
      str.Remove(L';');  // facename delimiter from rtf
      m_current_run.SetType(ON_TextRun::RunType::kFontdef);
      const ON_Font* pManagedFont = ON_Font::GetManagedFont(str);
      if (nullptr != pManagedFont)
        m_current_font = pManagedFont;
    }
  }
  else
  {
    // String is for a text run
    m_current_run.SetUnicodeString(count, cp);
    m_current_run.SetType(ON_TextRun::RunType::kText);
  }

#if defined(ON_DEBUG) && defined(ON_COMPILER_MSC)
  if (m_current_run.IsStacked() == ON_TextRun::Stacked::kStacked)
    count = count; // good place for a breakpoint
#endif
}

void ON_TextRunBuilder::BeginFontTable()
{
  m_font_table_level = m_level;
}

void ON_TextRunBuilder::BeginHeader()
{
  m_current_run.SetType(ON_TextRun::RunType::kHeader);
}

// Sets m_default_font_index when \deffn is read.
void ON_TextRunBuilder::DefaultFont(const wchar_t* value)
{
  wchar_t* sp = const_cast< wchar_t* >(value);
  ON__INT32 nval = (ON__INT32)::wcstol(value, &sp, 10);
  if (nval >= 0 && sp != value)
    m_default_font_index = nval;
}

// Process a rtf \fn tag to set the current font to the nth font in the rtf font table
void ON_TextRunBuilder::FontTag(const wchar_t* value)
{
  wchar_t* sp = const_cast< wchar_t* >(value);
  ON__INT32 nval = (ON__INT32)::wcstol(value, &sp, 10);
  if (nval >= 0 && sp != value)
  {
    if (m_font_table_level >= 0 &&
      m_level > m_font_table_level)
    {
      // Defining a font in the font table
      // n in \fn is the rtf index of the font currently being read from the table
      // store a mapping from \fn to font
      // when the font for this font table entry is made
      m_font_index = nval;
      // next will be a face name that will be read and put in a fontdef run
    }
    else
    {
      // Not defining the font table. Rather, setting a font current
      // Set current font to font corresponding to font_index
      //const ON_Font* pManagedFont = FindFontInMap(nval);

      const ON_Font* pManagedFont = (nullptr == m_current_font)
        ? &ON_Font::Default : m_current_font->ManagedFont();

      ON_wString facename = FaceNameFromMap(nval);
      if (facename.IsEmpty())
        facename = (nullptr == pManagedFont)
        ? ON_Font::Default.FontFaceName() : pManagedFont->FontFaceName();
      ON_Font::Weight weight = m_current_props.IsBold()
        ? ON_Font::Weight::Bold : ON_Font::Weight::Normal;
      ON_Font::Style style = m_current_props.IsItalic()
        ? ON_Font::Style::Italic : ON_Font::Style::Upright;
      ON_Font::Stretch stretch = ON_Font::Stretch::Medium;
      bool underlined = m_current_props.IsUnderlined();
      bool strikethrough = m_current_props.IsStrikethrough();

      ON_Font font;
      font.SetFontCharacteristics(facename, weight, style, stretch, underlined, strikethrough);

      if (nullptr != pManagedFont)
      {
        if (0 != ON_Font::CompareFontCharacteristics(*pManagedFont, font))
        {
          const ON_Font* newfont = ON_Font::GetManagedFont(font, true);
          pManagedFont = newfont;
        }
        m_current_font = pManagedFont;
        m_current_props.SetBold(pManagedFont->IsBold());
        m_current_props.SetItalic(pManagedFont->IsItalic());
        m_current_props.SetUnderlined(pManagedFont->IsUnderlined());
        m_current_props.SetStrikethrough(pManagedFont->IsStrikethrough());
      }

      unsigned int charset = CharSetFromMap(nval);
      m_current_props.SetCharSet(charset, true);
    }
  }
  return;  // error - need font index      
}

void ON_TextRunBuilder::FontSize(const wchar_t* value)
{
  // Even though there is supposed to be a way
  // to decode control font height to 3d text height,
  // This always uses the TextHeight from the ON_TextContent object
  m_current_run.SetTextHeight(m_current_props.Height());


  //wchar_t* eptr = const_cast< wchar_t* >(value);
  //ON__UINT32 fs = wcstol(value, &eptr, 10);
  //double d = 1.0;
  //if(eptr != value && fs > 0)
  //  d = MapControlFontSizeTo3dFontSize(fs);

  //m_current_run.SetHeight(d);
  //m_current_props.m_height = d;
}

void ON_TextRunBuilder::Newline()
{
  m_current_run.SetType(ON_TextRun::RunType::kNewline);
}

void ON_TextRunBuilder::Paragraph()
{
  m_current_run.SetType(ON_TextRun::RunType::kParagraph);
}

void ON_TextRunBuilder::ParagraphDefaults()
{
}

void ON_TextRunBuilder::Section()
{
}

void ON_TextRunBuilder::Tab()
{
  for(int i = 0; i < 8; i++) AppendCodePoint((ON__UINT32)' ');
  m_in_run = true;
}

void ON_TextRunBuilder::Bold(const wchar_t* value)
{
  if (nullptr == m_current_font)
    m_current_font = &ON_Font::Default;
  bool bold = m_current_font->IsBold();
  bool on = true;
  if (nullptr != value)
  {
    if ('1' == value[0] || 0 == value[0])
      on = true;
    else if ('0' == value[0])
      on = false;
  }
  if (on != bold)
  {
    ON_Font font(*m_current_font);
    font.SetFontWeight(on ? ON_Font::Weight::Bold : ON_Font::Weight::Normal);
    m_current_font = font.ManagedFont();
  }
  m_current_props.SetBold(on);
}

void ON_TextRunBuilder::Italic(const wchar_t* value)
{
  if (nullptr == m_current_font)
    m_current_font = &ON_Font::Default;

  bool italic = m_current_font->IsItalic();
  bool on = true;
  if (nullptr != value)
  {
    if ('1' == value[0] || 0 == value[0])
      on = true;
    else if ('0' == value[0])
      on = false;
  }
  if (on != italic)
  {
    ON_Font font(*m_current_font);
    if (on)
      font.SetFontStyle(ON_Font::Style::Italic);
    else
      font.SetFontStyle(ON_Font::Style::Upright);
    m_current_font = font.ManagedFont();
  }
  m_current_props.SetItalic(on);
}

void ON_TextRunBuilder::UnderlineOn()
{
  if (nullptr == m_current_font)
    m_current_font = &ON_Font::Default;
  ON_Font font(*m_current_font);
  font.SetUnderlined(true);
  m_current_font = font.ManagedFont();
  m_current_props.SetUnderlined(true);
}

void ON_TextRunBuilder::UnderlineOff()
{
  if (nullptr == m_current_font)
    m_current_font = &ON_Font::Default;
  ON_Font font(*m_current_font);
  font.SetUnderlined(false);
  m_current_font = font.ManagedFont();
  m_current_props.SetUnderlined(false);
}

void ON_TextRunBuilder::Strikethrough(const wchar_t* value)
{
  bool strike = m_current_font->IsStrikethrough();
  bool on = true;
  if (nullptr != value)
  {
    if ('1' == value[0] || 0 == value[0])
      on = true;
    else if ('0' == value[0])
      on = false;
  }
  if (on != strike)
  {
    ON_Font font(*m_current_font);
    font.SetStrikethrough(true);
    m_current_font = font.ManagedFont();
  }
  m_current_props.SetStrikethrough(on);
}

void ON_TextRunBuilder::Superscript()
{
}

void ON_TextRunBuilder::Subscript()
{
}

void ON_TextRunBuilder::NoSuperSub()
{
}

void ON_TextRunBuilder::BeginColorTable()
{
}

void ON_TextRunBuilder::ColorRed(const wchar_t* value)
{
}

void ON_TextRunBuilder::ColorGreen(const wchar_t* value)
{
}

void ON_TextRunBuilder::ColorBlue(const wchar_t* value)
{
}

void ON_TextRunBuilder::ColorForeground(const wchar_t* value)
{
}

void ON_TextRunBuilder::ColorBackground(const wchar_t* value)
{
}

void ON_TextRunBuilder::SetStackScale(const wchar_t* value)
{
  double stackscale = ON_TextRun::DefaultStackFractionHeight();
  ON_wString::ToNumber(value, stackscale, &stackscale);
  m_current_run.SetStackFractionHeight(stackscale);
  m_current_props.SetStackScale(stackscale);
}

void ON_TextRunBuilder::StackFraction(const wchar_t* value)
{
  m_current_run.SetType(ON_TextRun::RunType::kText);
  m_current_run.SetStacked(ON_TextRun::Stacked::kStacked);
  if (nullptr != m_current_run.m_stacked_text)
    delete m_current_run.m_stacked_text;
  m_current_run.m_stacked_text = new ON_StackedText;
  if (nullptr != value)
    m_current_run.m_stacked_text->m_separator = (wchar_t)wcstol(value, 0, 10);
  else
    m_current_run.m_stacked_text->m_separator = L'/';
}

void ON_TextRunBuilder::StackEnd()
{
  m_current_run.SetStackedOff();
}

void ON_TextRunBuilder::TextField(const wchar_t* name)
{
  m_current_run.SetType(ON_TextRun::RunType::kField);
}

void ON_TextRunBuilder::UniEmbeddedDest(const wchar_t* value)
{
  // two embedded unicode destinations - footnote, etc in rtf document
}

void ON_TextRunBuilder::UniDest(const wchar_t* value)
{
  // unicode destination
}

#pragma endregion  // TextRunBuilder



#pragma region RtfStringBuilder

ON_RtfStringBuilder::~ON_RtfStringBuilder()
{}

ON_RtfStringBuilder::ON_RtfStringBuilder(
  const ON_DimStyle* dimstyle,
  double height,
  ON_Color color)
{
  InitStringBuilder(dimstyle);
}


void ON_RtfStringBuilder::InitStringBuilder(const ON_DimStyle* dimstyle)
{
  if (nullptr == dimstyle)
    dimstyle = &ON_DimStyle::Default;
  const ON_Font& style_font = dimstyle->Font();
  ON_FaceNameKey& fnkey = m_facename_map.AppendNew();
  fnkey.m_facename = style_font.FontFaceName();
  fnkey.m_rtf_font_index = 0;

  bool bold = dimstyle->Font().IsBold();
  bool italic = dimstyle->Font().IsItalic();
  bool underlined = dimstyle->Font().IsUnderlined();
  bool strikethrough = dimstyle->Font().IsStrikethrough();

  m_run_stack.Empty();

  m_current_run.SetFontIndex(fnkey.m_rtf_font_index);
  m_current_run.SetBold(bold);
  m_current_run.SetItalic(italic);
  m_current_run.SetUnderlined(underlined);
  m_current_run.SetStrikeThrough(strikethrough);

  m_in_run = 0;
  m_level = 0;
  m_font_table_level = -1;
  m_ansi_equiv_chars.Empty();
  m_current_codepoints.Empty();
}

void ON_RtfStringBuilder::PushRun(TextRun& run)
{
  m_run_stack.Append(run);
}
ON_RtfStringBuilder::TextRun ON_RtfStringBuilder::PopRun()
{
  if (m_run_stack.Count() > 0)
  {
    TextRun run = *m_run_stack.Last();
    m_run_stack.Remove();
    return run;
  }
  return m_current_run;
}

bool ON_RtfStringBuilder::InFontTable()
{
  return m_in_font_table;
}
void ON_RtfStringBuilder::SetInFontTable(bool b)
{
  m_in_font_table = b;
}
bool ON_RtfStringBuilder::InColorTable()
{
  return m_in_color_table;
}
void ON_RtfStringBuilder::SetInColorTable(bool b)
{
  m_in_color_table = b;
}

void ON_RtfStringBuilder::SetSkipColorTbl(bool b)
{
  m_skip_color_tbl = b;
}
void ON_RtfStringBuilder::SetSkipBold(bool b)
{
  m_skip_bold = b;
}
void ON_RtfStringBuilder::SetSkipItalic(bool b)
{
  m_skip_italic = b;
}
void ON_RtfStringBuilder::SetSkipUnderline(bool b)
{
  m_skip_underline = b;
}
void ON_RtfStringBuilder::SetSkipFacename(bool b)
{
  m_skip_facename = b;
}

bool ON_RtfStringBuilder::SkipColorTbl()
{
  return m_skip_color_tbl;
}
bool ON_RtfStringBuilder::SkipBold()
{
  return m_skip_bold;
}
bool ON_RtfStringBuilder::SkipItalic()
{
  return m_skip_italic;
}
bool ON_RtfStringBuilder::SkipUnderline()
{
  return m_skip_underline;
}
bool ON_RtfStringBuilder::SkipFacename()
{
  return m_skip_facename;
}

void ON_RtfStringBuilder::SetMakeBold(bool b)
{
  if(b)
    m_skip_bold = true;
  m_make_bold = b;
}
void ON_RtfStringBuilder::SetMakeItalic(bool b)
{
  if(b)
    m_skip_italic = true;
  m_make_italic = b;
}
void ON_RtfStringBuilder::SetMakeUnderline(bool b)
{
  if (b)
    m_skip_underline = true;
  m_make_underline = b;
}
void ON_RtfStringBuilder::SetMakeFacename(bool b)
{
  if (b)
    m_skip_facename = true;
  m_make_facename = b;
}

bool ON_RtfStringBuilder::MakeBold()
{
  return m_make_bold;
}
bool ON_RtfStringBuilder::MakeItalic()
{
  return m_make_italic;
}
bool ON_RtfStringBuilder::MakeUnderline()
{
  return m_make_underline;
}

bool ON_RtfStringBuilder::MakeFacename()
{
  return m_make_facename;
}

void ON_RtfStringBuilder::SetDefaultFacename(const wchar_t* facename)
{
  m_default_facename = (nullptr == facename) ? L"" : facename;
}

void ON_RtfStringBuilder::SetOverrideFacename(const wchar_t* facename)
{
  m_override_facename = (nullptr == facename) ? L"" : facename;
}

bool ON_RtfStringBuilder::SkippingFacename()
{
  return (m_skip_facename && !m_make_facename);
}

bool ON_RtfStringBuilder::SettingFacename()
{
  return (m_make_facename && !m_default_facename.IsEmpty());
}



void ON_RtfStringBuilder::GroupBegin()
{
  // not skipping the color table or we're not in it
  if (!SkipColorTbl() || m_current_run.Type() != ON_TextRun::RunType::kColortbl)
    m_string_out += m_current_run.TextString();

  m_current_run.EmptyText();
  m_current_run.SetTerminated(true);

  PushRun(m_current_run);
  if (m_ansi_equiv_chars.Count() > 0)
    m_ansi_equiv_chars.Append(*m_ansi_equiv_chars.Last());
  else
    m_ansi_equiv_chars.Append(1);
  
  m_current_run.AddControl(L"{");
  m_have_rtf = true;
  m_level++;
}

void ON_RtfStringBuilder::GroupEnd()
{
  if (m_font_table_level > m_level)
    m_font_table_level = 0;
  if (m_current_run.Type() != ON_TextRun::RunType::kColortbl)
  {
    if (m_level >= 0)
    {
      m_current_run.AddControl(L"}");
      m_level--;
      if (m_current_run.Type() == ON_TextRun::RunType::kFonttbl)
      {
        SetInFontTable(false);
        if (SkippingFacename())
        {
          m_current_run.AddControl(L"{\\f0 ");
          m_level++;
        }
        else if (SettingFacename())
        {
          // Add \\fn ofter font table run
          ON_wString temp;
          temp.Format(L"{\\f%d ", m_font_index);
          m_current_run.AddControl(temp.Array());
          m_level++;
        }
        m_font_table_level = -1;
      }
      m_string_out = m_string_out + m_current_run.TextString();
      m_current_run.EmptyText();
    }
  }

  if (m_current_run.Type() == ON_TextRun::RunType::kColortbl)
    SetInColorTable(false);

  m_current_run = PopRun();

  if (m_ansi_equiv_chars.Count() > 0)
    m_ansi_equiv_chars.Remove();
}

void ON_RtfStringBuilder::BeginFontTable()
{
  m_font_table_level = m_level;
  m_current_run.SetType(ON_TextRun::RunType::kFonttbl);

  if (SkippingFacename())
    return;

  ON_wString temp;
  temp.Format(L"\\fonttbl{\\f0 %s;}", m_default_facename.Array());
  m_current_run.AddText(temp.Array());
  SetInFontTable(true);
  if (SettingFacename())
  {
    temp.Format(L"{\\f1 %s;}", m_override_facename.Array());
    m_current_run.AddText(temp.Array());
  }
}

void ON_RtfStringBuilder::BeginHeader()
{
  m_current_run.SetType(ON_TextRun::RunType::kHeader);
  m_current_run.AddControl(L"\\rtf1");
  if (MakeBold())
    m_current_run.AddControl(L"\\b");
  if (MakeItalic())
    m_current_run.AddControl(L"\\i");
  if (MakeUnderline())
    m_current_run.AddControl(L"\\ul");
}

// Sets m_default_font_index when \deffn is read.
void ON_RtfStringBuilder::DefaultFont(const wchar_t* value)
{
  ON__INT32 nval = -1;
  if (m_skip_facename || (m_make_facename && !m_default_facename.IsEmpty()))
    m_default_font_index = 0;
  else
  {
    wchar_t* sp = const_cast<wchar_t*>(value);
    nval = (ON__INT32)::wcstol(value, &sp, 10);
    if (nval >= 0 && sp != value)
      m_default_font_index = nval;
  }
  ON_wString temp;

  temp.Format(L"\\deff%d", m_default_font_index);
  m_current_run.AddControl(temp.Array());
}

const ON_wString ON_RtfStringBuilder::OutputString()
{
  m_string_out.Replace(L"{}", L"");
  return m_string_out;
}


// Process a rtf \fn tag to set the current font to the nth font in the rtf font table
// or to store the definition for the nth font in the font table
void ON_RtfStringBuilder::FontTag(const wchar_t* value)
{
  if (m_font_table_level >= 0 && m_level > m_font_table_level)  // if(InFontTable())...
    m_current_run.SetType(ON_TextRun::RunType::kFontdef);

  if (SkippingFacename())
    return;

  wchar_t* sp = const_cast< wchar_t* >(value);
  ON__INT32 nval = (ON__INT32)::wcstol(value, &sp, 10);
  if (nval >= 0 && sp != value) // got a number
  {
    if (m_font_table_level >= 0 && m_level > m_font_table_level)  // if(InFontTable())...
    {
      if (!SettingFacename())
      {
        // Defining a font in the font table
        // n in \fn is the rtf index of the font currently being read from the table
        // store a mapping from \fn to font
        // when the font for this font table entry is made
        m_font_index = nval;
        // next will be a face name that will be read and put in a fontdef run
        ON_wString temp;
        temp.Format(L"\\f%d", nval);
        m_current_run.AddControl(temp.Array());
      }
    }
    else
    {
      // Not defining the font table. Rather, setting a font current
      // Set current font to font corresponding to font_index
      
      {
        if (SkippingFacename() || SettingFacename())
          m_current_run.AddControl(L"\\f1");
        else if (m_current_run.FontIndex() != nval)
        {
          ON_wString temp;
          temp.Format(L"\\f%d", nval);
          m_current_run.AddControl(temp.Array());
          m_current_run.SetFontIndex(nval);
        }
      }
    }
  }
  return;      
}

void ON_RtfStringBuilder::FontSize(const wchar_t* value)
{
  // We ignore font height
  wchar_t* sp = const_cast< wchar_t* >(value);
  ON__INT32 nval = (ON__INT32)::wcstol(value, &sp, 10);
  if (nval >= 0 && sp != value) // got a number
  {
    ON_wString temp;
    temp.Format(L"\\fs%d", nval);
    m_current_run.AddControl(temp.Array());
  }
}

void ON_RtfStringBuilder::Newline()
{
  m_current_run.AddControl(L"\\par");
}

void ON_RtfStringBuilder::Paragraph()
{
  m_current_run.AddControl(L"\\par");
}

void ON_RtfStringBuilder::ParagraphDefaults()
{
  m_current_run.AddControl(L"\\pard");
}

void ON_RtfStringBuilder::Section()
{
}

void ON_RtfStringBuilder::Tab()
{
  for (int i = 0; i < 8; i++) AppendCodePoint((ON__UINT32)' ');
  m_in_run = true;
}

void ON_RtfStringBuilder::Bold(const wchar_t* value)
{
  if (!SkipBold())
  {
    bool bold = m_current_run.IsBold();
    bool on = true;
    if (nullptr != value)
    {
      if ('1' == value[0] || 0 == value[0])
        on = true;
      else if ('0' == value[0])
        on = false;
    }
    if (on != bold)
    {
      ON_wString temp;
      if ('0' == value[0])
        temp.Format(L"\\b0");
      else
        temp.Format(L"\\b");

      m_current_run.AddControl(temp.Array());
      m_current_run.SetBold(on);
    }
  }
}

void ON_RtfStringBuilder::Italic(const wchar_t* value)
{
  if (!SkipItalic())
  {
    bool italic = m_current_run.IsItalic();
    bool on = true;
    if (nullptr != value)
    {
      if ('1' == value[0] || 0 == value[0])
        on = true;
      else if ('0' == value[0])
        on = false;
    }
    if (on != italic)
    {
      ON_wString temp;
      if ('0' == value[0])
        temp.Format(L"\\i0");
      else
        temp.Format(L"\\i");

      m_current_run.AddControl(temp.Array());
      m_current_run.SetItalic(on);
    }
  }
}

void ON_RtfStringBuilder::UnderlineOn()
{
  if (!SkipUnderline())
  {
    bool under = m_current_run.IsUnderlined();
    bool on = true;
    if (on != under)
    {
      m_current_run.AddControl(L"\\ul");
      m_current_run.SetUnderlined(true);
    }
  }
}

void ON_RtfStringBuilder::UnderlineOff()
{
  if (!SkipUnderline())
  {
    bool under = m_current_run.IsUnderlined();
    bool on = false;
    if (on != under)
    {
      m_current_run.AddControl(L"\\ul0");
      m_current_run.SetUnderlined(true);
    }
  }
}

void ON_RtfStringBuilder::Strikethrough(const wchar_t* value)
{
}

void ON_RtfStringBuilder::Superscript()
{
}

void ON_RtfStringBuilder::Subscript()
{
}

void ON_RtfStringBuilder::NoSuperSub()
{
}

void ON_RtfStringBuilder::BeginColorTable()
{
  m_current_run.SetType(ON_TextRun::RunType::kColortbl);
  m_current_run.AddControl(L"\\colortbl");
  SetInColorTable(true);
}

void ON_RtfStringBuilder::ColorRed(const wchar_t* value)
{
}

void ON_RtfStringBuilder::ColorGreen(const wchar_t* value)
{
}

void ON_RtfStringBuilder::ColorBlue(const wchar_t* value)
{
}

void ON_RtfStringBuilder::ColorForeground(const wchar_t* value)
{
}

void ON_RtfStringBuilder::ColorBackground(const wchar_t* value)
{
}

void ON_RtfStringBuilder::TextField(const wchar_t* name)
{
  m_current_run.SetType(ON_TextRun::RunType::kField);
}

void ON_RtfStringBuilder::UniEmbeddedDest(const wchar_t* value)
{
  // two embedded unicode destinations - footnote, etc in rtf document
}

void ON_RtfStringBuilder::UniDecimal(const wchar_t* value)
{
  m_current_run.AddText(L"\\u");
  m_current_run.AddText(value);
  m_current_run.AddText(L"?");
}

void ON_RtfStringBuilder::UniDest(const wchar_t* value)
{
  // unicode destination
}

bool ON_RtfStringBuilder::AppendCodePoint(ON__UINT32 codept)
{
  if ((SettingFacename() || SkippingFacename()) && m_current_run.Type() == ON_TextRun::RunType::kFontdef)
    return true;

  if (!m_have_rtf)
  {
    if (MakeBold())
      m_current_run.AddControl(L"\\b");
    if (MakeItalic())
      m_current_run.AddControl(L"\\i");
    if (MakeUnderline())
      m_current_run.AddControl(L"\\ul");
    m_have_rtf = true;
  }

  ON_wString str;
  ON_TextContext::ConvertCodepointsToString(1, &codept, str);
  m_current_run.AddText(str.Array());

  m_current_codepoints.Append(codept);
  return true;
}



#pragma endregion  // RtfStringBuilder



#ifdef RTFFIRSTCHAR

#pragma region RtfFirstChar

ON_RtfFirstChar::~ON_RtfFirstChar()
{}

ON_RtfFirstChar::ON_RtfFirstChar(
  const ON_DimStyle* dimstyle,
  double height,
  ON_Color color)
{
  InitStringBuilder(dimstyle);
}


void ON_RtfFirstChar::InitStringBuilder(const ON_DimStyle* dimstyle)
{
  if (nullptr == dimstyle)
    dimstyle = &ON_DimStyle::Default;
  const ON_Font& style_font = dimstyle->Font();
  ON_FaceNameKey& fnkey = m_facename_map.AppendNew();
  fnkey.m_facename = style_font.FontFaceName();
  fnkey.m_rtf_font_index = -1;

  bool bold = dimstyle->Font().IsBold();
  bool italic = dimstyle->Font().IsItalic();
  bool underlined = dimstyle->Font().IsUnderlined();
  bool strikethrough = dimstyle->Font().IsStrikethrough();

  m_run_stack.Empty();

  m_current_run.SetFontIndex(fnkey.m_rtf_font_index);
  m_current_run.SetBold(bold);
  m_current_run.SetItalic(italic);
  m_current_run.SetUnderlined(underlined);
  m_current_run.SetStrikeThrough(strikethrough);

  m_in_run = 0;
  m_level = 0;
  m_font_table_level = -1;
}

void ON_RtfFirstChar::PushRun(TextRun& run)
{
  m_run_stack.Append(run);
}
ON_RtfFirstChar::TextRun ON_RtfFirstChar::PopRun()
{
  if (m_run_stack.Count() > 0)
  {
    TextRun run = *m_run_stack.Last();
    m_run_stack.Remove();
    return run;
  }
  return m_current_run;
}

bool ON_RtfFirstChar::InFontTable()
{
  return m_in_font_table;
}
void ON_RtfFirstChar::SetInFontTable(bool b)
{
  m_in_font_table = b;
}
bool ON_RtfFirstChar::InColorTable()
{
  return m_in_color_table;
}
void ON_RtfFirstChar::SetInColorTable(bool b)
{
  m_in_color_table = b;
}


void ON_RtfFirstChar::GroupBegin()
{
  PushRun(m_current_run);
  m_have_rtf = true;
  m_level++;
}

void ON_RtfFirstChar::GroupEnd()
{
  if (m_current_run.Type() == ON_TextRun::RunType::kFonttbl)
    SetInFontTable(false);

  if (m_current_run.Type() == ON_TextRun::RunType::kColortbl)
    SetInColorTable(false);

  if (m_current_run.Type() == ON_TextRun::RunType::kFontdef)
  {
    // String is a font facename. Make a font with that facename
    // and a font definition run 
    ON_wString str;
    str = m_current_run.Text();
    if (!str.IsEmpty())
    {
      str.Remove(L';');  // facename delimiter from rtf
      ON_FaceNameKey& fn_key = m_facename_map.AppendNew();
      fn_key.m_rtf_font_index = m_font_index;
      fn_key.m_facename = str;
      fn_key.m_charset = m_current_props.CharSet();
      fn_key.m_codepage = m_current_props.CodePage();
    }
  }

  m_current_run = PopRun();

  if (m_font_table_level >= m_level)
    m_font_table_level = -1;
  m_level--;
}

void ON_RtfFirstChar::BeginFontTable()
{
  m_font_table_level = m_level;
  m_current_run.SetType(ON_TextRun::RunType::kFonttbl);
  SetInFontTable(true);
}

void ON_RtfFirstChar::BeginHeader()
{
  m_current_run.SetType(ON_TextRun::RunType::kHeader);
}

void ON_RtfFirstChar::BeginColorTable()
{
  m_current_run.SetType(ON_TextRun::RunType::kColortbl);
  SetInColorTable(true);
}

void ON_RtfFirstChar::TextField(const wchar_t* name)
{
  m_current_run.SetType(ON_TextRun::RunType::kField);
}

bool ON_RtfFirstChar::AppendCodePoint(ON__UINT32 codept)
{
  if (!m_have_rtf)
    m_have_rtf = true;
  // First char not in a table or tag - return false to stop parsing
  if (!InColorTable() && !InFontTable())
    return false;

  ON_wString str;
  ON_TextContext::ConvertCodepointsToString(1, &codept, str);
  m_current_run.AddText(str.Array());

  m_current_codepoints.Append(codept);

  return true;
}

void ON_RtfFirstChar::FontTag(const wchar_t* value)
{
  if (m_font_table_level >= 0 && m_level > m_font_table_level)  // if(InFontTable())...
    m_current_run.SetType(ON_TextRun::RunType::kFontdef);

  wchar_t* sp = const_cast< wchar_t* >(value);
  ON__INT32 nval = (ON__INT32)::wcstol(value, &sp, 10);
  if (nval >= 0 && sp != value) // got a number
  {
    if (m_font_table_level >= 0 && m_level > m_font_table_level)  // if(InFontTable())...
    {
      // Defining a font in the font table
      // n in \fn is the rtf index of the font currently being read from the table
      // store a mapping from \fn to font
      // when the font for this font table entry is made
      m_font_index = nval;
    }
    else
    {
      // Not defining the font table. Rather, setting a font current
      // Set current font to font corresponding to font_index
      if (m_current_run.FontIndex() != nval)
        m_current_run.SetFontIndex(nval);
    }
  }
  return;
}

void ON_RtfFirstChar::Bold(const wchar_t* value)
{
  bool bold = m_current_run.IsBold();
  bool on = true;
  if (nullptr != value)
  {
    if ('1' == value[0] || 0 == value[0])
      on = true;
    else if ('0' == value[0])
      on = false;
  }
  if (on != bold)
  {
    m_current_run.SetBold(on);
  }
}

void ON_RtfFirstChar::Italic(const wchar_t* value)
{
  bool italic = m_current_run.IsItalic();
  bool on = true;
  if (nullptr != value)
  {
    if ('1' == value[0] || 0 == value[0])
      on = true;
    else if ('0' == value[0])
      on = false;
  }
  if (on != italic)
    m_current_run.SetItalic(on);
}

void ON_RtfFirstChar::UnderlineOn()
{
    bool under = m_current_run.IsUnderlined();
    bool on = true;
    if (on != under)
      m_current_run.SetUnderlined(true);
}

void ON_RtfFirstChar::UnderlineOff()
{
  bool under = m_current_run.IsUnderlined();
  bool on = false;
  if (on != under)
    m_current_run.SetUnderlined(false);
}

void ON_RtfFirstChar::Strikethrough(const wchar_t* value)
{
}




#pragma endregion  // RtfFirstChar

#endif // RTFFIRSTCHAR



ON_RtfParser::ON_RtfParser(ON_TextIterator& iter, ON_TextBuilder& builder)
  : m_ti(iter)
  , m_builder(builder)
  , m_p_level(0)
  , m_in_real_rtf(false)
{
}


ON_Color ParseColor(const wchar_t* value)
{
  ON_Color color;
  return color;
}

bool ON_RtfParser::ProcessTag(const wchar_t* name, const wchar_t* value, bool optional)
{
  ON_wString tagname(name);
  if( tagname.IsEmpty() )
    return false;

  const bool bOrdinalIgnoreCase = true;

  if (0 == tagname.CompareOrdinal(tagRtf, bOrdinalIgnoreCase))
  {
    m_in_real_rtf = true;
    m_builder.BeginHeader();
  }
  else if(0 == tagname.CompareOrdinal(tagFontTable,bOrdinalIgnoreCase))
    m_builder.BeginFontTable();
  else if(0 == tagname.CompareOrdinal(tagDefaultFont,bOrdinalIgnoreCase)      && 0 != value && 0 != value[0])
    m_builder.DefaultFont(value);
  else if(0 == tagname.CompareOrdinal(tagFont,bOrdinalIgnoreCase)             && 0 != value && 0 != value[0])
    m_builder.FontTag(value);
  else if(0 == tagname.CompareOrdinal(tagFontSize,bOrdinalIgnoreCase)         && 0 != value && 0 != value[0])
    m_builder.FontSize(value);
  else if (0 == tagname.CompareOrdinal(tagCharSet, bOrdinalIgnoreCase)        && 0 != value && 0 != value[0])
    m_builder.CharSet(value);
  else if (0 == tagname.CompareOrdinal(tagCodePage, bOrdinalIgnoreCase)       && 0 != value && 0 != value[0])
    m_builder.CodePage(value);

  else if(0 == tagname.CompareOrdinal(tagNewline,bOrdinalIgnoreCase))
    m_builder.Newline();
  else if(0 == tagname.CompareOrdinal(tagParagraph,bOrdinalIgnoreCase))
   m_builder.Paragraph();
  else if(0 == tagname.CompareOrdinal(tagParagraphDefaults,bOrdinalIgnoreCase))
    m_builder.ParagraphDefaults();
  else if(0 == tagname.CompareOrdinal(tagSection,bOrdinalIgnoreCase))
    m_builder.Section();
  else if(0 == tagname.CompareOrdinal(tagTabulator,bOrdinalIgnoreCase))
    m_builder.Tab();

  else if(0 == tagname.CompareOrdinal(tagBold,bOrdinalIgnoreCase))
    m_builder.Bold(value);
  else if(0 == tagname.CompareOrdinal(tagItalic,bOrdinalIgnoreCase))
    m_builder.Italic(value);
  else if (0 == tagname.CompareOrdinal(tagUnderLine, bOrdinalIgnoreCase))
  {
    if((*value) == L'0')
      m_builder.UnderlineOff();
    else
      m_builder.UnderlineOn();
  }
  else if (0 == tagname.CompareOrdinal(tagUnderLineNone, bOrdinalIgnoreCase))
    m_builder.UnderlineOff();
  else if(0 == tagname.CompareOrdinal(tagStrikeThrough,bOrdinalIgnoreCase))
    m_builder.Strikethrough(value);

  else if(0 == tagname.CompareOrdinal(tagSuperscript,bOrdinalIgnoreCase))
    m_builder.Superscript();
  else if(0 == tagname.CompareOrdinal(tagSubscript,bOrdinalIgnoreCase))
    m_builder.Subscript();
  else if(0 == tagname.CompareOrdinal(tagNoSuperSub,bOrdinalIgnoreCase))
    m_builder.NoSuperSub();
                                                                                            
  else if(0 == tagname.CompareOrdinal(tagColorTable,bOrdinalIgnoreCase))
    m_builder.BeginColorTable();
  else if(0 == tagname.CompareOrdinal(tagColorRed,bOrdinalIgnoreCase)         && 0 != value && 0 != value[0])
    m_builder.ColorRed(value);
  else if(0 == tagname.CompareOrdinal(tagColorGreen,bOrdinalIgnoreCase)       && 0 != value && 0 != value[0])
    m_builder.ColorGreen(value);
  else if(0 == tagname.CompareOrdinal(tagColorBlue,bOrdinalIgnoreCase)        && 0 != value && 0 != value[0])
    m_builder.ColorBlue(value);
  else if(0 == tagname.CompareOrdinal(tagColorForeground,bOrdinalIgnoreCase)  && 0 != value && 0 != value[0])
    m_builder.ColorForeground(value);
  else if(0 == tagname.CompareOrdinal(tagColorBackground,bOrdinalIgnoreCase)  && 0 != value && 0 != value[0])
    m_builder.ColorBackground(value);

  else if (0 == tagname.CompareOrdinal(tagStackFraction, bOrdinalIgnoreCase))
    m_builder.SetStackScale(value);
  else if (0 == tagname.CompareOrdinal(tagStackText, bOrdinalIgnoreCase))
    m_builder.StackFraction(value);
  else if (0 == tagname.CompareOrdinal(tagStackEnd, bOrdinalIgnoreCase))
    m_builder.StackEnd();

  else if (0 == ON_wString::CompareOrdinal(name, tagField, bOrdinalIgnoreCase))
    m_builder.TextField(value);

  else if (0 == ON_wString::CompareOrdinal(name, tagUniCpCount, bOrdinalIgnoreCase))
    m_builder.UniCpCount(value);
  else if (0 == ON_wString::CompareOrdinal(name, tagUniCharDec, bOrdinalIgnoreCase))
    m_builder.UniDecimal(value);
  else if (0 == ON_wString::CompareOrdinal(name, tagUniTwoDest, bOrdinalIgnoreCase))
    m_builder.UniEmbeddedDest(value);
  else if (0 == ON_wString::CompareOrdinal(name, tagUniDest, bOrdinalIgnoreCase))
    m_builder.UniDest(value);

  else if (0 == ON_wString::CompareOrdinal(name, taglquote, bOrdinalIgnoreCase))
    m_builder.LQuote();
  else if (0 == ON_wString::CompareOrdinal(name, tagrquote, bOrdinalIgnoreCase))
    m_builder.RQuote();
  else if (0 == ON_wString::CompareOrdinal(name, tagldblquote, bOrdinalIgnoreCase))
    m_builder.LDblQuote();
  else if (0 == ON_wString::CompareOrdinal(name, tagrdblquote, bOrdinalIgnoreCase))
    m_builder.RDblQuote();
  else if (0 == ON_wString::CompareOrdinal(name, tagbullet, bOrdinalIgnoreCase))
    m_builder.Bullet();
  else if (0 == ON_wString::CompareOrdinal(name, tagendash, bOrdinalIgnoreCase))
    m_builder.EnDash();
  else if (0 == ON_wString::CompareOrdinal(name, tagemdash, bOrdinalIgnoreCase))
    m_builder.EmDash();

  
    
    
    
    
    
    

  else if (!optional)
    return false;

  return true;
}

// Called after '\\' is read
// tags can contain up to 32 chars in the name optionally followed by a number
bool ON_RtfParser::ReadTag(bool optional_tag)
{
  wchar_t name[64];  // These are limited to 32 chars in the rtf spec
  wchar_t value[64];
  memset(name, 0, 64*sizeof(wchar_t));
  memset(value, 0, 64*sizeof(wchar_t));
  int namecnt = 0, valcnt = 0;
  m_ti.Back();
  bool in_name = true;
  bool end_of_tag = false;
  ON__UINT32 cp = 0;

  while(!end_of_tag)
  {
    if( false == m_ti.ReadCodePoint(cp) )
    {
      end_of_tag = true;
      break;
    }
    
    if(in_name && IsAtoZ(cp) && namecnt < 63)  // Tag name is all alpha chars
    {
      name[namecnt++] = (wchar_t)cp;
    }
    else if((IsDigit(cp) || '.' == cp || ('-' == cp && valcnt == 0)) && valcnt < 63)  // digits following name is tag parameter
    {
      in_name = false;
      value[valcnt++] = (wchar_t)cp;
    }
    else   // tag name terminated by any non-alphabetic char, tag parameter argument terminated by non-digit char
    {
      end_of_tag = true;
      ProcessTag(name, value, optional_tag);
      // MAC HACK - mac rtf stirngs include {\\*\\expandedcolortbl;;} which has an extra ';' to throw away
      if (optional_tag && (0 == ON_wString::CompareOrdinal(name, L"expandedcolortbl", false)))
      {
        while ('}' != cp)
        {
          if (false == m_ti.ReadCodePoint(cp))
            break;
        }
      }
      // Terminating chars other than these are eaten here
      if('\\' == cp || '{' == cp || '}' == cp)
        m_ti.Back();
    }
  }
  return true;
}


// input: cp_array is an array of codepoints
// Encodes to UTF16 to make string for text in the run
bool ON_RtfParser::FlushCurText(ON_SimpleArray< ON__UINT32 >& cp_array)
{
  // Calling this with an empty array does nothing
  int cp32_count = cp_array.Count();
  if(cp32_count <= 0)
    return false;

  m_builder.FlushText(cp32_count, cp_array.Array());
  cp_array.Empty();
  return true;
}

bool ON_RtfParser::Parse()
{
  bool rc = true;
  bool end_of_string = false;
  bool optional_tag = false;
  ON__UINT32 rtf_code_point;

  while(!end_of_string)
  {
    if (m_ti.AtBackslashTic())
    {
      // parse the entire contiguous MBCS string up to <OTHER>
      // \`XX\`XX...\`XX<OTHER>
      // <OTHER> != \`, XX = hex digit 0 to FF.
      // This string is a Windows MBCS string with code page identified by
      // 
      const ON__UINT32 char_count = Internal_ParseMBCSString( m_builder.m_current_props.CodePage() );
      if (char_count > 0)
        continue;
    }

    if(false == m_ti.ReadCodePoint(rtf_code_point))
      break;

    // Found a single byte character
    switch (rtf_code_point)
    {
    case ON_UnicodeCodePoint::ON_Backslash:
      {
        if ( m_ti.ReadCodePoint(rtf_code_point) )
        {
          // character following '\\'
          // Its a tag of some kind, or a literal char
          // These are special characters that follow '\\'
          switch (rtf_code_point)
          {
            // Literal escaped  '\' or '{' or '}'
            // Append to current text string
          case ON_UnicodeCodePoint::ON_Backslash:
          case L'{':
          case L'}':
            m_builder.m_current_codepoints.Append(rtf_code_point);
            break;

            // Paragraph tag when following '\\'
          case ON_UnicodeCodePoint::ON_LineFeed:
          case ON_UnicodeCodePoint::ON_CarriageReturn:
          case ON_UnicodeCodePoint::ON_LineSeparator:
          case ON_UnicodeCodePoint::ON_ParagraphSeparator:
            m_builder.m_current_codepoints.Append(rtf_code_point);
            //      TODO    ProcessTag(new RtfTag(tagParagraph));
            break;

          case '\'':
            // This case sould never occur - it is handled by 
            // Internal_ParseMBCSString at the beginning of this while statement.
            ON_ERROR("Bug in RTF parsing code.");
            break;

            // Symbol tags...
          case '~':  // non-break space
            if (!m_builder.AppendCodePoint((ON__UINT32)' '))
              return true;
            break;
          case '-':  // optional hyphen
            break;
          case '_':  // non-break hyphen
            if (!m_builder.AppendCodePoint((ON__UINT32)'-'))
              return true;
            break;
          case '|':
          case ':':
            break;
          case '*':
            optional_tag = true;
            break;
          case 'n':
            if (!m_in_real_rtf)
            {
              FlushCurText(m_builder.m_current_codepoints);
              m_builder.GroupEnd();
              ProcessTag(L"par", nullptr, false);
              m_builder.GroupBegin();
              break;
            }

          default:
            // Tag names are always low ascii alphabetic
            ReadTag(optional_tag);
            //m_builder.m_current_codepoints.Empty();
            optional_tag = false;
          }
        }
      }
      break;

      // Character not immediately following '\\'
      // Skip newline characters if this is a real rtf string - just white space
      // If this is one of our recomposed plain text strings, \n means newline
    case ON_UnicodeCodePoint::ON_LineFeed:
    case ON_UnicodeCodePoint::ON_LineSeparator:
      if (!m_in_real_rtf)
      {
        FlushCurText(m_builder.m_current_codepoints);
        m_builder.GroupEnd();
        ProcessTag(L"par", nullptr, false);
        m_builder.GroupBegin();
      }
      break;

    case ON_UnicodeCodePoint::ON_NoBreakSpace:
    case ON_UnicodeCodePoint::ON_NarrowNoBreakSpace:
    case ON_UnicodeCodePoint::ON_ZeroWidthSpace:
      if (!m_builder.AppendCodePoint((ON__UINT32)' '))
        return true;
      break;

    case ON_UnicodeCodePoint::ON_CarriageReturn:
      break;

    case ON_UnicodeCodePoint::ON_Tab:
      // '\tab' tag
      //      ProcessTag(new RtfTag(tagTabulator));
      break;

      // Start group
    case '{':
      //FlushCurText(m_builder.m_current_codepoints);
      m_builder.GroupBegin();
      m_p_level++;
      break;

    case '}':
      //FlushCurText(m_builder.m_current_codepoints);
      m_builder.GroupEnd();
      if (m_p_level > 0)
      {
        m_p_level--;
      }
      else
      {
        // {} underflow
      }
      break;

    default:
      // Normal single byte text character not in a tag name
      // Could be argument to a tag, like a font name in fonttbl
      // AppendCodePoint() returns false to stop parsing
      if (!m_builder.AppendCodePoint(rtf_code_point))
        return true;
      break;
    }
  }  // end of big loop for whole string

  FlushCurText(m_builder.m_current_codepoints);
  m_builder.GroupEnd();

  return rc;
}

// This has to make a string that the text control can digest
// and that's short and simple as possible so it can be put in the file
unsigned int RtfComposer::GetFacenameKey(const wchar_t* facename, ON_SimpleArray< wchar_t[34] >& fonttable)
{
  int count = fonttable.Count();
  for(int i = 0; i < count; i++)
  {
    if(0 == ON_wString::CompareOrdinal(facename, 34, fonttable[i], 34, true))
      return i;
  }
  wcsncpy( fonttable.AppendNew(), facename, 33);
  return count;
}
unsigned int RtfComposer::GetColorKey(ON_Color color, ON_SimpleArray< unsigned int >& colortable)
{
  int count = colortable.Count();
  for(int i = 0; i < count; i++)
  {
    if(color == colortable[i])
      return i;
  }
  colortable.Append((unsigned int)color);
  return count;
}
bool RtfComposer::FormatTextHeight(double height, ON_wString& str)
{
  str.Format(L"\\*h%lf", height);
  return true;
}

bool IsSpecial(const wchar_t ch)
{
  if (L'\\' == ch) return true;
  if (L'}' == ch) return true;
  if (L'{' == ch) return true;
  return false;
}

#if 1

static bool GetRunText(ON_TextRun* run, ON_wString& text_out)
{
  if (nullptr == run)
    return false;
  // Have to convert codepoints directly instead of
  // calling DisplayString() because of field evaluation
  // and because RTF uses a "signed UTF-16" encoding and converting the run
  // into RTF has to work on Apple platforms where wchar_t strings 
  // are UTF-32 encoded strings. (Ask Dale Lear if you have questions).
  const ON__UINT32* run_code_points = run->UnicodeString(); // null terminated
  if (nullptr != run_code_points && 0 != run_code_points[0])
  {
    for (int ci = 0; 0 != run_code_points[ci]; ci++)
    {
      const ON__UINT32 code_point = run_code_points[ci];

      // works on Windows and Apple
      ON__UINT16 utf16[2] = { 0 };
      const int utf16_count = ON_EncodeUTF16(code_point, utf16);
      if (utf16_count < 0 || utf16_count > 2 || 0 == utf16[0])
        continue;

      if (code_point > 0x80 || 1 != utf16_count || code_point != (ON__UINT32)utf16[0])
      {
        // When we write RTF, we do not specify what encodding is used for values in the range 0x80 - 0xFF.
        // 
        // The ON_wString temp should to have UTF-16 encoding on Windows platforms
        // and UTF-32 encoding on Apple platforms.
        //
        // There are 27 "tricky values" in this range 0x80 - 0xFF where Windows-1252 maps the value to a glyph and 
        // and UNICODE maps the value to a control that typically has no printable glyph.
        // These "tricky values" are all in the range 0x80 ... 0x9F.
        // An example is the Euro sign (Windows-1252 0x80 = Euro sign, UNICODE U+0080 = xxx control,
        // UNOCODE U+20AC = Euro sign).
        //
        // The RTF we get from Windows controls, like the "Text" command dialog box,
        // typically specifies it is using Windows-1252 and encodes the Euro sign as \`80.
        // So, if we have one of these "euro like" values, we will explicitly write it as a UNICODE value
        // to avoid the possiblity of something defaulting to using Windows-1252.
        // https://mcneel.myjetbrains.com/youtrack/issue/RH-38205
        //
        // See ON_DecodeWindowsCodePage1252Value() for more details.
        //
        // UNOCODE code points that require UTF-16 surrogate pair encodings have
        // two RTF values TWO \uN?\uN? values. 
        // For example, UNOCODE code point U+1F5D1 has UTF-16 encodeing (0xD83D, 0xDDD1)
        // and the RTF looks like ...{\ltrch \u-10179?\u-8751?}.
        for (int utf16i = 0; utf16i < utf16_count; utf16i++)
        {
          ON_wString n;
          const ON__INT16 signed_rtf_utf16_value = (ON__INT16)utf16[utf16i]; // will be negative when utf16[utf16i] >= 0x8000;
          const int signed_string_format_param = (int)signed_rtf_utf16_value;
          n.Format(L"\\u%d?", signed_string_format_param);
          text_out += n;
        }
      }
      else
      {
        // code_point < 0x80 (ASCII value range) and casting to wchar_t will work
        // on any platform;
        wchar_t ascii_value = (wchar_t)code_point;
        if (IsSpecial(ascii_value))
        {
          text_out += L'\\';
        }
        text_out += ascii_value;
      }
    }
  }
  return true;
}
#endif



bool RtfComposer::Compose(const ON_TextContent* text, const ON_DimStyle* dimstyle, ON_wString& rtf)
{
  dimstyle = &ON_DimStyle::DimStyleOrDefault(dimstyle);

  if (0 == text)
    return false;

  if (!RtfComposer::RecomposeRTF())
  {
    rtf = text->RtfText();
    return true;
  }

  ON_TextRunArray* runs = text->TextRuns(true);
  if (nullptr == runs)
    return false;

  const ON_Font& style_font = dimstyle->Font();
  const wchar_t* style_facename = style_font.FontFaceName();
  if (nullptr == style_facename)
    style_facename = ON_Font::Default.FontFaceName();

  bool style_bold = style_font.IsBold();
  bool style_italic = (ON_Font::Style::Italic == style_font.FontStyle());
  bool style_underline = style_font.IsUnderlined();
  bool style_strikeout = style_font.IsStrikethrough();

  bool chg_bold = false;
  bool chg_italic = false;
  bool chg_underline = false;
  bool chg_strikeout = false;
  bool chg_facename = false;
  
  // First facename is from the ON_TextContent
  // Any after that are from runs
  ON_SimpleArray< wchar_t[34] > fonttable;
  
  // Creates a fonttable entry the first time
  int deffont_key = GetFacenameKey(style_facename, fonttable);

  int runcount = runs->Count();
  int nlcount = 0;
  
  // See if this is multi-line text
  bool multiline = false;
  for (int ri = 0; ri < runcount; ri++)
  {
    ON_TextRun* run = (*runs)[ri];
    if (0 != run)
    {
      if (ON_TextRun::RunType::kText == run->Type() && 0 < nlcount)
        multiline = true;
      else if (ON_TextRun::RunType::kNewline == run->Type() ||
        ON_TextRun::RunType::kParagraph == run->Type())
        nlcount++;
    }
  }
  
  bool make_rtf = false;
  ON_SimpleArray< ON_TextRun* > runholders;
  for (int ri = 0; ri < runcount; ri++)
  {
    ON_TextRun* run = (*runs)[ri];
    if (0 != run)
    {
      if (
        ON_TextRun::RunType::kText == run->Type() ||
        ON_TextRun::RunType::kField == run->Type()
        )
      {
        const ON_Font* run_font = run->Font();
        if (nullptr != run_font)
        {
          runholders.AppendNew() = run;
          int facename_key = GetFacenameKey(run_font->FontFaceName(), fonttable);
          if(!chg_bold)
            chg_bold = run_font->IsBold() != style_bold;
          if(!chg_italic)
            chg_italic = run_font->IsItalic() != style_italic;
          if(!chg_underline)
            chg_underline = run_font->IsUnderlined() != style_underline;
          if(!chg_strikeout)
            chg_strikeout = run_font->IsStrikethrough() != style_strikeout;
          if(!chg_facename)
            chg_facename = facename_key != deffont_key;
        }
      }
      else if (
        ON_TextRun::RunType::kParagraph == run->Type() ||
        ON_TextRun::RunType::kNewline == run->Type()
        )
      {
        runholders.AppendNew() = run;
      }
    }

  } // end of getting runinfo

  if (chg_bold || chg_italic || chg_underline || chg_strikeout || chg_facename)
    make_rtf = true;

  ON_wString run_strings;
  ON_wString temp;
  runcount = runholders.Count();
  for (int ri = 0; ri < runcount; ri++)
  {
    ON_TextRun* run = runholders[ri];
    if (nullptr == run)
      continue;

    if (
      ON_TextRun::RunType::kText == run->Type() ||
      ON_TextRun::RunType::kField == run->Type()
      )
    {
      if (make_rtf || (run->IsStacked() == ON_TextRun::Stacked::kStacked && run->m_stacked_text != 0))
      {
        const ON_Font* run_font = run->Font();
        if (nullptr == run_font)
          continue;

        // add properties for this string
        run_strings += L"{";
        bool addspace = false;
        int run_font_key = GetFacenameKey(run_font->FontFaceName(), fonttable);
        if (run_font_key != deffont_key)
        {
          temp.Format(L"\\f%d", run_font_key);
          run_strings += temp;
          addspace = true;
        }
        //if (run_font->IsBold() != style_bold)
        {
          run_strings += run_font->IsBold() ? L"\\b" : L"\\b0";
          addspace = true;
        }
        if (run_font->IsItalic())
        {
          run_strings += L"\\i";
          addspace = true;
        }
        if (run_font->IsUnderlined())
        {
          run_strings += L"\\ul";
          addspace = true;
        }
        if (addspace)
          run_strings += L" ";

        if (run->IsStacked() == ON_TextRun::Stacked::kStacked && run->m_stacked_text != 0)
        {
          run_strings += L"[[";
          GetRunText(run->m_stacked_text->m_top_run, run_strings);
          run_strings += L"/";
          GetRunText(run->m_stacked_text->m_bottom_run, run_strings);
          run_strings += L"]]}";
        }
        else if (ON_TextRun::RunType::kField == run->Type())
        {
          run_strings += L"%<";
          GetRunText(run, run_strings);
          run_strings += L">%";
        }
        else
        {
          GetRunText(run, run_strings);
        }
        run_strings += L"}";
      }
      else
      {
        GetRunText(run, run_strings);
      }
    }
    else if (ri < runcount-1 && multiline && (ON_TextRun::RunType::kNewline == run->Type() || ON_TextRun::RunType::kParagraph == run->Type()))
    {
      run_strings += L"{\\par}";
    }
  }  // end of collecting run text

  int nfont = fonttable.Count();
  // Any time the font for the annotation's style has 
  // bold, italic or underline set, we have to write rtf 
  // even if there are no changes in the string itself
  if (run_strings.Length() > 0 && make_rtf)
  {
    // deff0 means use font0 for the default font throughout the string.
    // If we include font0 in the font table, when we send
    // the string back to the RTF control, the font listed as
    // font0 will be used instead of the default font for the 
    // style.
    // So the default font is listed as 0 and there is no
    // entry in the table for font0.

    rtf.Format(L"{\\rtf1");

    if (0 < nfont)
    {
      ON_wString fonttable_string;
      temp.Format(L"\\deff%d", deffont_key);
      rtf += temp;
      fonttable_string = L"{\\fonttbl";
      for (int fi = 0; fi < nfont; fi++)
      {
        // 
        temp.Format(L"{\\f%d %s;}", fi, fonttable[fi]);
        fonttable_string += temp;
      }
      fonttable_string += "}";
      rtf += fonttable_string;
    }
    temp.Format(L"{\\f%d\\b", deffont_key);
    rtf += temp;

    rtf += run_strings;

    rtf += L"\\par}}";
  }
  else
  {
    rtf = run_strings;
  }
  return true;
}


//static
void RtfComposer::ComposeRun(
  const ON_TextRun* run,
  const ON_DimStyle* dimstyle,
  ON_SimpleArray< wchar_t[34] >& fonttable,
  bool multiline,
  int& changecount,
  int& changefont,
  int& changecolor,
  bool& bold,
  bool& italic,
  bool& underlined,
  bool& strikeout,
  ON_wString& strings_out)
{
  bool chg_bold = false;
  bool chg_italic = false;
  bool chg_underlined = false;
  bool chg_strikeout = false;
  bool chg_subscript = false;
  bool chg_superscript = false;
  bool chg_color = false;
  bool chg_height = false;
  bool chg_facename = false;
  unsigned int facenamekey = 0;

  if (nullptr == dimstyle)
    dimstyle = &ON_DimStyle::Default;

  const ON_Font& style_font = dimstyle->Font();

  const ON_Font* run_font = run->Font();
  if (nullptr == run_font)
    run_font = &ON_Font::Default;

  const wchar_t* style_facename = style_font.FontFaceName();
  const wchar_t* run_facename = run_font->FontFaceName();

  if (0 != ON_wString::CompareOrdinal(style_facename, 34, run_facename, 34, true))
  {
    chg_facename = true;
    facenamekey = GetFacenameKey(run_facename, fonttable);
  }
  if (run_font->IsBold() != (bold ? true : false))
  {
    chg_bold = true;
  }
  if ((ON_Font::Style::Italic == run_font->FontStyle()) != italic)
  {
    chg_italic = true;
  }
  if (run_font->IsUnderlined() != underlined)
  {
    chg_underlined = true;
  }
  if (run_font->IsStrikethrough() != strikeout)
  {
    chg_strikeout = true;
  }

  // Any change here means we need a real rtf string, not just plain text
  if (chg_bold || chg_italic || chg_underlined || chg_strikeout || chg_subscript || chg_superscript || chg_color || chg_height || chg_facename || multiline)
  {
    ON_wString temp;
    changecount++;
    if (chg_facename)
    {
      temp.Format(L"{\\f%d", facenamekey);
      strings_out += temp;
    }
    else
      strings_out += L"{";
    //if (run->FlowDirection() == ON_TextRun::RunDirection::kLtR)
    //  strings_out += L"{\\ltrch";
    //else
    //  strings_out += L"{\\rtlch";


    if (chg_bold)
    {
      if (run_font->IsBold())
        strings_out += L"\\b";
      else
        strings_out += L"\\b0";
    }
    if (chg_italic)
    {
      if (ON_Font::Style::Italic == run_font->FontStyle())
        strings_out += L"\\i";
      else
        strings_out += L"\\i0";
    }
    if (chg_underlined)
    {
      if (run_font->IsUnderlined())
        strings_out += L"\\ul";
      else
        strings_out += L"\\ul0";
    }
    wchar_t last = strings_out[strings_out.Length() - 1];
    if (last != L';' && last != L'{')
      strings_out += L" ";

    // Have to convert codepoints directly instead of
    // calling DisplayString() because of field evaluation
    // and because RTF uses a "signed UTF-16" encoding and converting the run
    // into RTF has to work on Apple platforms where wchar_t strings 
    // are UTF-32 encoded strings. (Ask Dale Lear if you have questions).
    const ON__UINT32* run_code_points = run->UnicodeString(); // null terminated
    if (nullptr != run_code_points && 0 != run_code_points[0])
    {
      for (int ci = 0; 0 != run_code_points[ci]; ci++)
      {
        const ON__UINT32 code_point = run_code_points[ci];

        // works on Windows and Apple
        ON__UINT16 utf16[2] = { 0 };
        const int utf16_count = ON_EncodeUTF16(code_point, utf16);
        if (utf16_count < 0 || utf16_count > 2 || 0 == utf16[0])
          continue;

        if (code_point > 0x80 || 1 != utf16_count || code_point != (ON__UINT32)utf16[0])
        {
          // When we write RTF, we do not specify what encodding is used for values in the range 0x80 - 0xFF.
          // 
          // The ON_wString temp should to have UTF-16 encoding on Windows platforms
          // and UTF-32 encoding on Apple platforms.
          //
          // There are 27 "tricky values" in this range 0x80 - 0xFF where Windows-1252 maps the value to a glyph and 
          // and UNICODE maps the value to a control that typically has no printable glyph.
          // These "tricky values" are all in the range 0x80 ... 0x9F.
          // An example is the Euro sign (Windows-1252 0x80 = Euro sign, UNICODE U+0080 = xxx control,
          // UNOCODE U+20AC = Euro sign).
          //
          // The RTF we get from Windows controls, like the "Text" command dialog box,
          // typically specifies it is using Windows-1252 and encodes the Euro sign as \`80.
          // So, if we have one of these "euro like" values, we will explicitly write it as a UNICODE value
          // to avoid the possiblity of something defaulting to using Windows-1252.
          // https://mcneel.myjetbrains.com/youtrack/issue/RH-38205
          //
          // See ON_DecodeWindowsCodePage1252Value() for more details.
          //
          // UNOCODE code points that require UTF-16 surrogate pair encodings have
          // two RTF values TWO \uN?\uN? values. 
          // For example, UNOCODE code point U+1F5D1 has UTF-16 encodeing (0xD83D, 0xDDD1)
          // and the RTF looks like ...{\ltrch \u-10179?\u-8751?}.
          for (int utf16i = 0; utf16i < utf16_count; utf16i++)
          {
            ON_wString n;
            const ON__INT16 signed_rtf_utf16_value = (ON__INT16)utf16[utf16i]; // will be negative when utf16[utf16i] >= 0x8000;
            const int signed_string_format_param = (int)signed_rtf_utf16_value;
            n.Format(L"\\u%d?", signed_string_format_param);
            strings_out += n;
          }
        }
        else
        {
          // code_point < 0x80 (ASCII value range) and casting to wchar_t will work
          // on any platform;
          wchar_t ascii_value = (wchar_t)code_point;
          if (IsSpecial(ascii_value))
          {
            strings_out += L'\\';
          }
          strings_out += ascii_value;
        }
      }
    }
    strings_out += L"}";
    //if (chg_facename)
    //  strings_out += L"}";
  }
  else
  {
    ON_wString temp;
    size_t cplen = ON_TextRun::CodepointCount(run->UnicodeString());
    ON_TextContext::ConvertCodepointsToString((int)cplen, run->UnicodeString(), temp);
    if (!temp.IsEmpty())
    {
      int len = temp.Length();
      for (int si = 0; si < len; si++)
      {
        if (temp[si] > 0xFE)
        {
          // convert to rtf unicode string
          ON_wString n;
          n.Format(L"\\u%d?", (short)temp[si]);
          strings_out += n;
        }
        else
        {
          strings_out += temp[si];
          if (temp[si] == '\\')
            strings_out += '\\';
        }
      }
    }
  }
}

bool RtfComposer::m_bComposeRTF = true;
bool RtfComposer::RecomposeRTF()
{
  return RtfComposer::m_bComposeRTF;
}
void RtfComposer::SetRecomposeRTF(bool b)
{
  RtfComposer::m_bComposeRTF = b;
}




bool RtfComposer::ComposeA(const ON_TextContent* text, const ON_DimStyle* dimstyle, ON_wString& rtf)
{
  dimstyle = &ON_DimStyle::DimStyleOrDefault(dimstyle);

  if (0 == text)
    return false;

  if (!RtfComposer::RecomposeRTF())
  {
    rtf = text->RtfText();
    return true;
  }

  ON_TextRunArray* runs = text->TextRuns(true);
  if (nullptr == runs)
    return false;

  const ON_Font& style_font = dimstyle->Font();
  const wchar_t* style_facename = style_font.FontFaceName();
  if (nullptr == style_facename)
    style_facename = ON_Font::Default.FontFaceName();

  bool bold = style_font.IsBold();
  bool italic = (ON_Font::Style::Italic == style_font.FontStyle());
  bool underlined = style_font.IsUnderlined();
  bool strikeout = style_font.IsStrikethrough();

  // First color and first facename are from the ON_TextContent
  // Any after that are from runs
  ON_SimpleArray< unsigned int > colortable;
  ON_SimpleArray< wchar_t[34] > fonttable;
  int changecount = 0;  // count all changes except font and color (because they are in tables in rtf)
  int changecolor = 0;  // count changes in color
  int changefont = 0;   // count changes in font

  int deffont_index = GetFacenameKey(style_facename, fonttable);

  ON_ClassArray< ON_String > lines;
  ON_wString fonttbl_string;
  ON_wString colortbl;
  ON_wString strings;
  ON_wString temp;

  int runcount = runs->Count();
  int nlcount = 0;
  bool multiline = false;
  for (int ri = 0; ri < runcount; ri++)
  {
    ON_TextRun* run = (*runs)[ri];
    if (0 != run)
    {
      if (ON_TextRun::RunType::kText == run->Type() && 0 < nlcount)
        multiline = true;
      else if (ON_TextRun::RunType::kNewline == run->Type() ||
        ON_TextRun::RunType::kParagraph == run->Type())
        nlcount++;
    }
  }

  for (int ri = 0; ri < runcount; ri++)
  {
    ON_TextRun* run = (*runs)[ri];
    if (0 != run)
    {
      if (ON_TextRun::RunType::kText == run->Type() ||
        ON_TextRun::RunType::kField == run->Type())
      {
        const ON_Font* run_font = run->Font();
        if (nullptr != run_font)
        {
          if (run->IsStacked() == ON_TextRun::Stacked::kStacked && run->m_stacked_text != 0)
          {
            // See if this run is all digits and delimiter
            temp.Empty();
            size_t cplen = ON_TextRun::CodepointCount(run->UnicodeString());
            ON_TextContext::ConvertCodepointsToString((int)cplen, run->UnicodeString(), temp);

#ifndef ON_TEXT_BRACKET_FRACTION  // Stacked fraction brackets
            bool alldigits = true;
            if (!temp.IsEmpty())
            {
              int len = temp.Length();
              for (int si = 0; si < len; si++)
              {
                if (!isdigit(temp[si]) && L'/' != temp[si])
                  alldigits = false;
              }
            }
            if (alldigits)
            {
              // If this stacked run has only digits and a separator in the text,
              // and the character before it is a digit, add a space to separate this 
              // from the previous run.  The space is thrown away when parsing.
              const wchar_t* s = strings.Array();
              int l = strings.Length();
              if (l > 0 && (isdigit(s[l - 1]) || (L'}' == s[l - 1] && isdigit(s[l - 2]))))
                strings += L" ";
              ComposeRun(run, dimstyle, fonttable, multiline, changecount, changefont, changecolor, bold, italic, underlined, strikeout, strings);
            }
            else  // If it's not all digits, add [[ ]]
#endif
            {
              const wchar_t* run_facename = run_font->FontFaceName();
              int facenamekey = GetFacenameKey(run_facename, fonttable);
              temp.Format(L"{\\f%d [[", facenamekey);
              strings += temp;

              //strings += L"[[";
              ComposeRun(run->m_stacked_text->m_top_run, dimstyle, fonttable, false, changecount, changefont, changecolor, bold, italic, underlined, strikeout, strings);
              strings += L"/";
              ComposeRun(run->m_stacked_text->m_bottom_run, dimstyle, fonttable, false, changecount, changefont, changecolor, bold, italic, underlined, strikeout, strings);
              strings += L"]]}";
            }
          }
          else
            if (ON_TextRun::RunType::kField == run->Type())
            {
              strings += L"%<";
              ComposeRun(run, dimstyle, fonttable, multiline, changecount, changefont, changecolor, bold, italic, underlined, strikeout, strings);
              strings += L">%";
            }
            else
              ComposeRun(run, dimstyle, fonttable, multiline, changecount, changefont, changecolor, bold, italic, underlined, strikeout, strings);
        }
      }
      else if (multiline && (ON_TextRun::RunType::kNewline == run->Type() || ON_TextRun::RunType::kParagraph == run->Type()))
      {
        strings += L"{\\par}";
      }
    }
  }

  int nfont = fonttable.Count();
  int ncolor = colortable.Count();
  // Any time the font for the annotation's style has 
  // bold, italic or underline set, we have to write rtf 
  // even if there are no changes in the string itself
  if (strings.Length() > 0 &&

    (style_font.IsBold() ||
      // Currently not allowing italic or underlined in styles 
      // because the WPF text control doesn't deal with them well
      //ON_Font::Style::Italic == style_font.FontStyle() ||
      //style_font.IsUnderlined() ||

      0 < changecount ||
      1 < nfont || 0 < ncolor))
  {
    // deff0 means use font0 for the default font throughout the string.
    // If we include font0 in the font table, when we send
    // the string back to the RTF control, the font listed as
    // font0 will be used instead of the default font for the 
    // style.
    // So the default font is listed as 0 and there is no
    // entry in the table for font0.

    rtf.Format(L"{\\rtf1");

    if (1 < nfont)
    {
      temp.Format(L"\\deff%d", deffont_index);
      rtf += temp;
      fonttbl_string = L"{\\fonttbl";
      for (int fi = 0; fi < nfont; fi++)
      {
        // 
        temp.Format(L"{\\f%d %s;}", fi, fonttable[fi]);
        fonttbl_string += temp;
      }
      fonttbl_string += "}";
      rtf += fonttbl_string.Array();
    }
    if (0 < ncolor)
    {
      colortbl = L"{\\colortbl";
      for (int ci = 0; ci < ncolor; ci++)
      {
        ON_Color color = colortable[ci];
        temp.Format(L"\\red%d,\\green%d\\blue%d;", ci, color.Red(), color.Green(), color.Blue());
        colortbl += temp;
      }
      colortbl += "}";
      rtf += colortbl.Array();
    }

    if (style_font.IsBold())
      rtf += L"{\\b";
    else
      rtf += L"{\\b0";


    if (L'{' != strings[0]/* && ((1 == nfont && 0 == ncolor) || style_font.IsBold())*/)
      rtf += L" ";

    rtf += strings;

    //if (style_font.IsBold())
    //  rtf += L"}";

    rtf += L"}}";
  }
  else
  {
    rtf = strings;
  }
  return true;
}

