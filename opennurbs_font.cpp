//
// Copyright (c) 1993-2015 Robert McNeel & Associates. All rights reserved.
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

#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_internal_glyph.h"

int ON_FontMetrics::Ascent() const
{
  return m_ascent;
}

int ON_FontMetrics::Descent() const
{
  return m_descent;
}

int ON_FontMetrics::LineSpace() const
{
  return m_line_space;
}

int ON_FontMetrics::UPM() const
{
  return m_UPM;
}

int ON_FontMetrics::AscentOfI() const
{
  return m_ascent_of_I;
}

double ON_FontMetrics::GlyphScale(double text_height) const
{
  // Please consult Dale Lear and the ON_FontGlyph 
  // bounding box, outline, and advance calculation code
  // before making any modifications to this function.
  const double y = (double)AscentOfI();
  return (text_height > 0.0 && y > 0.0) ? (text_height / y) : 1.0;
}

int ON_FontMetrics::StrikeoutThickness() const
{
  return m_strikeout_thickness;
}

int ON_FontMetrics::StrikeoutPosition() const
{
  return m_strikeout_position;
}

int ON_FontMetrics::UnderscoreThickness() const
{
  return m_underscore_thickness;
}

int ON_FontMetrics::UnderscorePosition() const
{
  return m_underscore_position;
}

bool ON_FontMetrics::HeightsAreValid() const
{
  if (m_ascent <= m_descent)
    return false;
  if (m_line_space < m_ascent - m_descent)
    return false;
  if (m_ascent_of_I > m_ascent)
    return false;
  if (m_UPM <= 0)
    return false;
  return true;
}


void ON_FontMetrics::SetHeights(
  int ascent,
  int descent,
  int UPM,
  int line_space
)
{
  if (ON_UNSET_INT_INDEX < descent && descent < ascent && ascent < -ON_UNSET_INT_INDEX)
  {
    m_ascent = ascent;
    m_descent = descent;
  }
  else
  {
    m_ascent = 0;
    m_descent = 0;
  }
  m_UPM = (UPM > 0 && UPM < -ON_UNSET_INT_INDEX) ? UPM : 0;
  m_line_space = (line_space > 0 && line_space < -ON_UNSET_INT_INDEX)? line_space : 0;
}

void ON_FontMetrics::SetAscentOfI(
  int ascent_of_I
)
{
  m_ascent_of_I = (ascent_of_I > 0 && ascent_of_I < -ON_UNSET_INT_INDEX) ?  ascent_of_I : 0;
}
  
void ON_FontMetrics::SetStrikeout(
  int strikeout_position,
  int strikeout_thickness
)
{
  m_strikeout_position = strikeout_position;
  m_strikeout_thickness = strikeout_thickness;
}
  
void ON_FontMetrics::SetUnderscore(
  int underscore_position,
  int underscore_thickness
)
{
  m_underscore_position = underscore_position;
  m_underscore_thickness = underscore_thickness;
}

static int Internal_ScaleInt(double scale, int i)
{
  return (int)((i >= 0) ? ceil(scale*i) : floor(scale*i));
}




const ON_FontMetrics ON_FontMetrics::Scale(
  const ON_FontMetrics& font_metrics,
  double scale
)
{
  ON_FontMetrics scaled_font_metrics = font_metrics;

  if (scale > 0.0 && 1.0 != scale)
  {
    scaled_font_metrics.m_UPM = Internal_ScaleInt(scale, scaled_font_metrics.m_UPM);
    scaled_font_metrics.m_ascent = Internal_ScaleInt(scale, scaled_font_metrics.m_ascent);
    scaled_font_metrics.m_descent = Internal_ScaleInt(scale, scaled_font_metrics.m_descent);
    if (font_metrics.m_line_space == Internal_ScaleInt(ON_FontMetrics::DefaultLineFeedRatio,font_metrics.m_ascent_of_I))
      scaled_font_metrics.m_line_space = Internal_ScaleInt(ON_FontMetrics::DefaultLineFeedRatio,scaled_font_metrics.m_ascent_of_I);
    else
      scaled_font_metrics.m_line_space = Internal_ScaleInt(scale, scaled_font_metrics.m_line_space);
    scaled_font_metrics.m_ascent_of_I = Internal_ScaleInt(scale, scaled_font_metrics.m_ascent_of_I);

    scaled_font_metrics.m_strikeout_thickness = Internal_ScaleInt(scale, scaled_font_metrics.m_strikeout_thickness);
    scaled_font_metrics.m_strikeout_position = Internal_ScaleInt(scale, scaled_font_metrics.m_strikeout_position);

    scaled_font_metrics.m_underscore_thickness = Internal_ScaleInt(scale, scaled_font_metrics.m_underscore_thickness);
    scaled_font_metrics.m_underscore_position = Internal_ScaleInt(scale, scaled_font_metrics.m_underscore_position);
  }

  return scaled_font_metrics;
}

// ON_GlyphMap::GlyphPool needs to be allocated before  ON_ManagedFonts::List. 
// This is so the pool is still around when the ON_ManagedFonts::List
// is being destroyed.
ON_Internal_FontGlyphPool ON_Internal_FontGlyphPool::theGlyphItemPool;
ON_ManagedFonts ON_ManagedFonts::List = ON_ManagedFonts();

ON_ManagedFonts::~ON_ManagedFonts()
{
  ON_SimpleArray<const ON_Font*> managed_fonts(m_managed_fonts_by_serial_number);
  m_managed_fonts.Destroy();
  m_managed_fonts_by_serial_number.Destroy();

  // last created first deleted - it shouldn't really matter.
  for(int i = managed_fonts.Count()-1; i >= 0; i--)
  {
    ON_Font* managed_font = const_cast<ON_Font*>(managed_fonts[i]);

    // The reset needs to happen for all fonts, including ON_Font::Default
    // Otherwise we get unpredictable crashes when closing because the
    // order in which ON_Font::Default, ON_GlyphMap::GlyphPool and ON_ManagedFonts::List
    // is not predictable.
    managed_font->m_font_glyph_cache.reset();


    if (managed_font->m_runtime_serial_number >= 2)
      delete managed_font;
  }
}

class ON_FontGlyphCache
{
public:
  // See ON_Font.FontMetrics() documentation for a discussion
  // of normalized and unnormalized metrics and scales.
  // Both scale values are stored to reduce rounding errors.
  double m_font_unit_to_normalized_scale = 1.0;
  double m_normalized_to_font_unit_scale = 1.0;
  ON_FontMetrics m_font_unit_metrics;
  ON_FontMetrics m_normalized_metrics;

  // Array of glyphs with sizes and display info
  std::shared_ptr<ON_GlyphMap> m_glyphmap;
};

static int CompareManagedFontCharacteristics(
  const void* a,
  const void* b
  )
{
  const ON_Font* a_font = *((const ON_Font*const*)a);
  const ON_Font* b_font = *((const ON_Font*const*)b);
  return ON_Font::CompareFontCharacteristics(*a_font,*b_font);
}

const ON_Font* BinarySearchForManagedFontCharacteristics( const ON_Font& key, const ON_Font*const* base, size_t nel )
{
  if (nel > 0 && nullptr != base )
  {
    size_t i;
    const ON_Font* font;
    int c;

    while ( nel > 0 )
    {
      i = nel/2;
      font = base[i];
      c = ON_Font::CompareFontCharacteristics(key,*font);
      if ( c < 0 )
      {
        nel = i;
      }
      else if ( c > 0 )
      {
        i++;
        base += i;
        nel -= i;
      }
      else
      {
        return font;
      }
    }
  }
  return nullptr;
}

static int CompareManagedFontSerialNumber(
  const void* a,
  const void* b
  )
{
  const unsigned int a_sn = (*((const ON_Font*const*)a))->RuntimeSerialNumber();
  const unsigned int b_sn = (*((const ON_Font*const*)b))->RuntimeSerialNumber();
  if (a_sn < b_sn)
    return -1;
  if (a_sn > b_sn)
    return 1;
  return 0;
}

const ON_Font* BinarySearchForManagedFontSerialNumber( unsigned int key, const ON_Font*const* base, size_t nel )
{
  if (nel > 0 && nullptr != base )
  {
    size_t i;
    const ON_Font* font;
    unsigned int d;

    // The end tests are not necessary, but they
    // seem to provide overall speed improvement
    // for the types of searches that call this
    // function.
    font = base[0];
    d = font->RuntimeSerialNumber();
    if ( key < d )
      return nullptr;
    if ( key == d )
      return font;

    font = base[nel-1];
    d = font->RuntimeSerialNumber();
    if ( key > d )
      return nullptr;
    if ( key == d )
      return font;

    while ( nel > 0 )
    {
      i = nel/2;
      font = base[i];
      d = font->RuntimeSerialNumber();
      if ( key < d )
      {
        nel = i;
      }
      else if ( key > d )
      {
        i++;
        base += i;
        nel -= i;
      }
      else
      {
        return font;
      }
    }
  }
  return nullptr;
}

const ON_Font* ON_ManagedFonts::GetFromSerialNumber(
  unsigned int managed_font_serial_number
  )
{
  if ( managed_font_serial_number == ON_Font::Default.RuntimeSerialNumber() )
    return &ON_Font::Default;

  const ON_Font* const * managed_fonts = m_managed_fonts_by_serial_number.Array();
  const unsigned int font_count = m_managed_fonts_by_serial_number.UnsignedCount();

  if (managed_font_serial_number <= font_count
    && managed_font_serial_number == managed_fonts[managed_font_serial_number - 1]->RuntimeSerialNumber())
  {
    // This test should always find the managed font as long as the current numbering scheme is used.
    return managed_fonts[managed_font_serial_number - 1];
  }

  unsigned int& sorted_count = m_sorted_by_serial_number_count;

  if (font_count > sorted_count + 4)
  {
    ON_qsort((void*)managed_fonts, font_count, sizeof(managed_fonts[0]), CompareManagedFontSerialNumber );
    sorted_count = font_count;
  }
  else
  {
    // most recently added font will be in the unsorted section.
    for (unsigned int i = sorted_count; i < font_count; i++)
    {
      if (managed_font_serial_number == managed_fonts[i]->RuntimeSerialNumber())
        return managed_fonts[i];
    }
  }
  return
    (sorted_count > 0)
    ? BinarySearchForManagedFontSerialNumber(managed_font_serial_number, managed_fonts, sorted_count)
    : nullptr;
}


const ON_Font* ON_ManagedFonts::GetFromFontCharacteristics(
  const ON_Font& font_characteristics,
  bool bCreateIfNotFound
  )
{
  if (0 == m_managed_fonts.UnsignedCount())
  {
    // Put ON_Font::Default as the first entry in this list.
    Internal_AddManagedFont(&ON_Font::Default);
  }

  const ON_Font* const * managed_fonts = m_managed_fonts.Array();
  const unsigned int font_count = m_managed_fonts.UnsignedCount();
  unsigned int& sorted_count = m_sorted_count;

  if (font_count > sorted_count + 4)
  {
    ON_qsort((void*)managed_fonts, font_count, sizeof(managed_fonts[0]), CompareManagedFontCharacteristics );
    sorted_count = font_count;
  }
  else
  {
    // most recently added font will be in the unsorted section.
    for (unsigned int i = sorted_count; i < font_count; i++)
    {
      if (0 == ON_Font::CompareFontCharacteristics( font_characteristics, *managed_fonts[i] ) )
        return managed_fonts[i];
    }
  }

  const ON_Font* managed_font = BinarySearchForManagedFontCharacteristics(font_characteristics,m_managed_fonts.Array(),m_sorted_count);
  if (nullptr == managed_font && bCreateIfNotFound)
  {
    if (font_characteristics.FontDescription().IsEmpty()
      && font_characteristics.AppleFontName().IsEmpty()
      && 0 == font_characteristics.FontFaceName()[0]
      )
    {
      managed_font = &ON_Font::Default;
    }
    else
    {
      // The managed font constructor does not copy user data, m_font_index, m_font_id, m_gonna_change_font_cache
      ON_MemoryAllocationTracking disable_tracking(false);
      managed_font = Internal_AddManagedFont(new ON_Font(2, font_characteristics));
    }
  }

  return managed_font;
}


const ON_Font* ON_ManagedFonts::GetFromAppleFontName(
  const wchar_t* apple_font_name,
  bool bCreateIfNotFound
  )
{
  if (0 == m_managed_fonts.UnsignedCount())
  {
    // Put ON_Font::Default as the first entry in this list.
    Internal_AddManagedFont(&ON_Font::Default);
  }
  
  const ON_Font* const * managed_fonts = m_managed_fonts.Array();
  const unsigned int font_count = m_managed_fonts.UnsignedCount();
  
  for (unsigned int i = 0; i < font_count; i++)
  {
    if (0 == ON_StringCompareOrdinalWideChar(apple_font_name, -1, managed_fonts[i]->m_apple_font_name, -1, true) )
      return managed_fonts[i];
  }
  
  const ON_Font* managed_font = nullptr;
  if (bCreateIfNotFound)
  {
    ON_Font font_characteristics;
    font_characteristics.SetFromAppleFontName(apple_font_name);
    // The managed font constructor does not copy user data, m_font_index, m_font_id, m_gonna_change_font_cache
    managed_font = Internal_AddManagedFont(new ON_Font(2,font_characteristics));
  }
  
  return managed_font;
}


static void Internal_AddManagedFontCleanString(
  const ON_wString& s
)
{
  const ON_wString dirty(s);
  ON_wString& clean = const_cast<ON_wString&>(s);
  clean.Destroy();
  clean = static_cast<const wchar_t*>(dirty);
}

const ON_Font* ON_ManagedFonts::Internal_AddManagedFont(
  const ON_Font* managed_font
  )
{
  // All memory allocated for managed fonts is permanent app workspace memory.
  ON_MemoryAllocationTracking disable_tracking(false);

  /////////////////////
  //
  // Put the cached glyph information here so we only have one set for each font
  //
  ON_FontGlyphCache* font_cache = new ON_FontGlyphCache();
  font_cache->m_glyphmap = std::make_shared<ON_GlyphMap>();
  managed_font->m_font_glyph_cache = std::shared_ptr<ON_FontGlyphCache>(font_cache);
    
  if (true)
  {
    ON_FontMetrics font_unit_metrics;

    ON_ManagedFonts::GetFontMetrics(
      managed_font,
      font_unit_metrics
    );

    const int ascent = font_unit_metrics.Ascent();
    const int descent = font_unit_metrics.Descent();
    if (font_unit_metrics.UPM() <= 0)
    {
      const int line_space = font_unit_metrics.LineSpace();
      if (ascent > descent)
        font_unit_metrics.SetHeights(ascent, descent, ascent - descent, line_space);
    }
    const int UPM = font_unit_metrics.UPM();

    if (UPM > 0 )
    {
      int ascent_of_I = font_unit_metrics.AscentOfI();
      int line_space = font_unit_metrics.LineSpace();

      if (ascent_of_I <= 0)
      {
        // Get 'I' glyph height.
        // Do not use glyph cache or per glyph substuted fonts here.
        // This call is used only to set the value of
        // font_cache->m_unnormalized_metrics.m_height_of_I
        // and that value needs to come from the font.
        ON_TextBox unnormalized_glyph_box = ON_TextBox::Unset;
        if (0 != ON_ManagedFonts::GetGlyphMetrics(managed_font, ON_Font::Constants::MetricsGlyphCodePoint, unnormalized_glyph_box))
        {
          if (unnormalized_glyph_box.IsSet() && unnormalized_glyph_box.m_bbmax.j > 0)
            ascent_of_I = unnormalized_glyph_box.m_bbmax.j;
        }
        
        if (ascent_of_I <= 0)
        {
          if (ascent_of_I <= 0 && line_space > ascent - descent)
          {
            ascent_of_I = (int)ceil(line_space / ON_FontMetrics::DefaultLineFeedRatio);
          }

          if (ascent_of_I <= 0 && &ON_Font::Default != managed_font)
          {
            const ON_FontMetrics default_font_unit_metrics = ON_Font::Default.FontUnitFontMetrics();
            if (default_font_unit_metrics.AscentOfI() > 0 && default_font_unit_metrics.UPM() > 0)
            {
              const double scale = ((double)UPM) / ((double)default_font_unit_metrics.UPM());
              ascent_of_I = (int)ceil(scale*default_font_unit_metrics.AscentOfI());
            }
          }

          if (ascent > 0 && ascent_of_I > ascent)
            ascent_of_I = ascent;

        }
      }

      if (line_space <= 0 && ascent_of_I > 0 )
        line_space = (int)ceil(ON_FontMetrics::DefaultLineFeedRatio*ascent_of_I);

      font_unit_metrics.SetHeights(ascent, descent, UPM, line_space);
      font_unit_metrics.SetAscentOfI(ascent_of_I);

      font_cache->m_font_unit_metrics = font_unit_metrics;


      font_cache->m_normalized_to_font_unit_scale = ((double)UPM) / ((double)ON_Font::Constants::AnnotationFontCellHeight);
      font_cache->m_font_unit_to_normalized_scale = ((double)ON_Font::Constants::AnnotationFontCellHeight) / ((double)UPM);

      font_cache->m_normalized_metrics
        = (font_cache->m_font_unit_to_normalized_scale > 0.0 && 1.0 != font_cache->m_font_unit_to_normalized_scale)
        ? ON_FontMetrics::Scale(font_cache->m_font_unit_metrics, font_cache->m_font_unit_to_normalized_scale)
        : font_cache->m_font_unit_metrics;
    }
  }

  if ( false == font_cache->m_font_unit_metrics.HeightsAreValid() )
  {
    ON_ERROR("Unable to get useful font metrics.");
    // continue and save what we have
  }

  const unsigned int font_count0 = m_managed_fonts.UnsignedCount();
  if (font_count0 > 0)
  {
    if ( font_count0 == m_sorted_by_serial_number_count
         && m_managed_fonts_by_serial_number[font_count0 - 1]->RuntimeSerialNumber() < managed_font->RuntimeSerialNumber()
       )
    {
      m_sorted_by_serial_number_count++;
    }
  }

  m_managed_fonts.Append(managed_font);
  m_managed_fonts_by_serial_number.Append(managed_font);
  
  Internal_AddManagedFontCleanString(managed_font->m_apple_font_name);
  Internal_AddManagedFontCleanString(managed_font->m_font_description);

  return managed_font;
}

unsigned int ON_ManagedFonts::GetList(
  ON_SimpleArray< const ON_Font* >& managed_fonts
  )
{
  const unsigned int font_count = m_managed_fonts_by_serial_number.UnsignedCount();
  if (m_sorted_by_serial_number_count < font_count)
  {
    ON_qsort((void*)m_managed_fonts_by_serial_number.Array(), font_count, sizeof(managed_fonts[0]), CompareManagedFontSerialNumber);
    m_sorted_by_serial_number_count = font_count;
  }
  managed_fonts = m_managed_fonts_by_serial_number;
  return managed_fonts.UnsignedCount();
}

#define ON_MANAGED_FONT_CHECK(falure_return_value) {if (0 != m_managed_font) (ON_ERROR("Cannot modify managed fonts."); return (falure_return_value);}}
//
// END list of managed ON_Fonts
//
//////////////////////////////////////////////////////////////////////////

bool ON_Font::ModificationPermitted(
    const char* function_name,
    const char* file_name,
    int line_number
  ) const
{
  if ( IsManagedFont() )
  {
    // ON_Font::Default and managed fonts can never be modified
    if ( this == &ON_Font::Default )
      ON_ErrorEx(file_name, line_number, function_name, "ON_Font::Default cannot be modified.");
    else
      ON_ErrorEx(file_name, line_number, function_name, "Managed fonts cannot be modified.");
    return false;
  }

  // Modificaton of this font means the managed information it references
  // will not be valid.  A reference to the correct cached information
  // will be generated when it is actually needed.
  m_font_glyph_cache.reset();
  return true;
}

#define ON_FONT_MODIFICATION_PERMITTED this->ModificationPermitted(OPENNURBS__FUNCTION__,__FILE__,__LINE__)

//// V6 files 4F0F51FB-35D0-4865-9998-6D2C6A99721D is the class id for ON_TextStyle
////ON_OBJECT_IMPLEMENT( ON_Font, ON_Object, "4F0F51FB-35D0-4865-9998-6D2C6A99721D" );
//ON_OBJECT_IMPLEMENT( ON_Font, ON_Object, "5F7476D1-798A-4953-B359-4A37699CD6F4" );

const ON_Font* ON_Font::ManagedFont() const
{
  return 
    IsManagedFont()
    ? this
    : ON_Font::GetManagedFont(*this, true);
}

const ON_FontGlyph* ON_Font::CodePointGlyph(
  ON__UINT32 unicode_codepoint
) const
{
  const ON_Font* managed_font = ManagedFont();
  if (nullptr == managed_font)
    return nullptr;

  bool bCreateIfMissing = true;
  bool bFindSubstitutes = true;
  return managed_font->Internal_ManagedCodePointGlyph(unicode_codepoint,bCreateIfMissing,bFindSubstitutes);
}

const ON_Font* ON_Font::GetManagedFont(
  const ON_Font& font_characteristics,
  bool bCreateIfNotFound
  )
{
  if ( font_characteristics.IsManagedFont() )
  {
    // No need to look it up and return itself.
    return &font_characteristics;
  }

#if defined (ON_RUNTIME_APPLE)
  const ON_Font* font = ON_ManagedFonts::List.GetFromAppleFontName(font_characteristics.m_apple_font_name,bCreateIfNotFound);
  if (nullptr != font)
    return font;
#endif
  
  return ON_ManagedFonts::List.GetFromFontCharacteristics(font_characteristics,bCreateIfNotFound);
}

const ON_Font* ON_Font::GetManagedFontFromSerialNumber(
  unsigned int managed_font_runtime_serial_number
  )
{
  return ON_ManagedFonts::List.GetFromSerialNumber(managed_font_runtime_serial_number);
}

unsigned int ON_Font::GetManagedFontList(
  ON_SimpleArray< const ON_Font* >& managed_fonts
  )
{
  return ON_ManagedFonts::List.GetList(managed_fonts);
}

bool ON_Font::IsManagedFont() const
{
  return ( 0 != m_runtime_serial_number );
}

const ON_Font* ON_Font::GetManagedFont(
  const wchar_t* face_name
)
{
  return ON_Font::GetManagedFont(0.0, face_name);
}

const ON_Font* ON_Font::GetManagedFont(
  double point_size,
  const wchar_t* face_name
  )
{
  return ON_Font::GetManagedFont(
    point_size,
    face_name,
    ON_Font::Default.m_font_weight,
    ON_Font::Default.m_font_style
    );
}

const ON_Font* ON_Font::GetManagedFont(
  const wchar_t* face_name,
  bool bBold
)
{
  return ON_Font::GetManagedFont(0.0, face_name, bBold);
}

const ON_Font* ON_Font::GetManagedFont(
  double point_size,
  const wchar_t* face_name,
  bool bBold
  )
{
  const bool bItalic = false;
  return ON_Font::GetManagedFont( point_size, face_name, bBold, bItalic );
}

const ON_Font* ON_Font::GetManagedFont(
  const wchar_t* face_name,
  bool bBold,
  bool bItalic
)
{
  return ON_Font::GetManagedFont(0.0, face_name, bBold, bItalic);
}

const ON_Font* ON_Font::GetManagedFont(
  double point_size,
  const wchar_t* face_name,
  bool bBold,
  bool bItalic
  )
{
  return ON_Font::GetManagedFont(
    point_size,
    face_name,
    bBold ? ON_Font::Weight::Bold : ON_Font::Default.FontWeight(),
    bItalic ? ON_Font::Style::Italic : ON_Font::Default.FontStyle()
    );
}

const ON_Font* ON_Font::GetManagedFont(
  const wchar_t* face_name,
  ON_Font::Weight font_weight,
  ON_Font::Style font_style
)
{
  return ON_Font::GetManagedFont(0.0, face_name, font_weight, font_style);
}

const ON_Font* ON_Font::GetManagedFont(
  double point_size,
  const wchar_t* face_name,
  ON_Font::Weight font_weight,
  ON_Font::Style font_style
  )
{
  unsigned int logfont_charset
    = (nullptr != face_name && 0 != face_name[0])
    ? static_cast<unsigned int>(ON_Font::WindowsLogfontCharSetFromFaceName(face_name))
    : static_cast<unsigned int>(ON_Font::WindowsConstants::logfont_default_charset);

  return ON_Font::GetManagedFont(
    point_size,
    face_name, 
    font_weight,
    font_style,
    ON_Font::Default.m_font_stretch,
    ON_Font::Default.m_font_bUnderlined,
    ON_Font::Default.m_font_bStrikethrough,
    ON_FontMetrics::DefaultLineFeedRatio,
    logfont_charset
    );
}

const ON_Font* ON_Font::GetManagedFont(
  const wchar_t* face_name,
  ON_Font::Weight font_weight,
  ON_Font::Style font_style,
  ON_Font::Stretch font_stretch,
  bool bUnderlined,
  bool bStrikethrough,
  double linefeed_ratio,
  unsigned int logfont_charset
)
{
  return ON_Font::GetManagedFont(
    0.0, // point_size
    face_name,
    font_weight,
    font_style,
    font_stretch,
    bUnderlined,
    bStrikethrough,
    linefeed_ratio,
    logfont_charset
  );
}

const ON_Font* ON_Font::GetManagedFont(
  double point_size,
  const wchar_t* face_name, 
  ON_Font::Weight font_weight,
  ON_Font::Style font_style,
  ON_Font::Stretch font_stretch,
  bool bUnderlined, 
  bool bStrikethrough,
  double linefeed_ratio,
  unsigned int logfont_charset
  )
{
  ON_Font font_characteristics;
  if ( false == font_characteristics.SetFontCharacteristics(
    point_size,
    face_name, 
    font_weight,
    font_style,
    font_stretch,
    bUnderlined, 
    bStrikethrough,
    linefeed_ratio,
    logfont_charset
    ))
    return nullptr;
  return font_characteristics.ManagedFont();
}

#if defined(ON_OS_WINDOWS_GDI)

const ON_Font* ON_Font::GetManagedFontFromWindowsLogfont(
  int map_mode,
  HDC hdc,
  const LOGFONT& logfont
  )
{
  ON_Font font_characteristics;
  if (false == font_characteristics.SetFromWindowsLogFont(map_mode, hdc,logfont))
    return nullptr;
  return font_characteristics.ManagedFont();
}

#endif

const ON_Font* ON_Font::GetManagedFontFromFontDescription(
  const wchar_t* font_description
  )
{
  ON_Font font_characteristics;
  if ( false == font_characteristics.SetFromFontDescription(font_description) )
    return nullptr;
  return font_characteristics.ManagedFont();
}

bool ON_Font::IsNotAppleFontName(
  const wchar_t* font_description
  )
{
  if (nullptr == font_description || 0 == font_description[0])
    return true;
  if ( ON_wString::EqualOrdinal(L"Default",font_description,true) )
    return true;
  // In RH-35535 Marlin reports that Arial is shipped with OS X.
  //if ( ON_wString::EqualOrdinal(L"Arial",font_description,true) )
  //  return true;
  return false;
}

const ON_Font* ON_Font::GetManagedFontFromAppleFontName(
  const wchar_t* apple_font_name
  )
{
  ON_Font font_characteristics;
  if ( false == font_characteristics.SetFromAppleFontName(apple_font_name) )
    return nullptr;
  return font_characteristics.ManagedFont();
}




int ON_Font::WindowsLogfontWeightFromWeight(
  ON_Font::Weight font_weight
  )
{
  int logfont_weight = (int)(100U*static_cast<unsigned int>(font_weight));
  if ( logfont_weight < 50 )
    logfont_weight = 400;
  if ( logfont_weight < 150 )
    logfont_weight = 100;
  else if ( logfont_weight >= 850 )
    logfont_weight = 900;
  else if (0 != logfont_weight % 100)
  {
    int delta = logfont_weight %100;
    if (delta < 50)
      logfont_weight -= delta;
    else
      logfont_weight += (100-delta);
  }
  return logfont_weight;
}

int ON_Font::AppleWeightOfFontFromWeight(
  ON_Font::Weight font_weight
  )
{
  return ON_Font::WindowsLogfontWeightFromWeight(font_weight)/100;
}

double ON_Font::AppleFontWeightTraitFromWeight(
  ON_Font::Weight font_weight
)
{
  // These values are selected to optimize conversion of font weights between Windows and Apple platforms.
  // https://mcneel.myjetbrains.com/youtrack/issue/RH-37075


  const double default_apple_font_weight_trait = 0.0;

  double w = ((double)((int)static_cast<unsigned char>(font_weight)) - 400.0) / 750.0;
  if (w < -1.0)
    w = -1.0;
  else if (w > 1.0)
    w = 1.0;
  if (!(-1.0 <= w && w < 1.0))
    w = default_apple_font_weight_trait;
  
  double apple_font_weight_trait;
  switch (font_weight)
  {
  case ON_Font::Weight::Unset:
    apple_font_weight_trait = default_apple_font_weight_trait;
    break;
  case ON_Font::Weight::Thin:
    apple_font_weight_trait = -0.4;
    break;
  case ON_Font::Weight::Ultralight:
    apple_font_weight_trait = w;
    break;
  case ON_Font::Weight::Light:
    apple_font_weight_trait = w;
    break;
  case ON_Font::Weight::Normal:
    apple_font_weight_trait = 0.0;
    break;
  case ON_Font::Weight::Medium:
    apple_font_weight_trait = w;
    break;
  case ON_Font::Weight::Semibold:
    apple_font_weight_trait = w;
    break;
  case ON_Font::Weight::Bold:
    apple_font_weight_trait = 0.4;
    break;
  case ON_Font::Weight::Ultrabold:
    apple_font_weight_trait = w;
    break;
  case ON_Font::Weight::Heavy:
    apple_font_weight_trait = w;
    break;
  default:
    apple_font_weight_trait = default_apple_font_weight_trait;
    break;
  }

  // The valid value range is from -1.0 to 1.0. The value of 0.0 corresponds to the regular or medium font weight.
  return 
    (-1.0 <= apple_font_weight_trait && apple_font_weight_trait <= 1.0) 
    ? apple_font_weight_trait 
    : default_apple_font_weight_trait;
}

ON_Font::Weight ON_Font::WeightFromWindowsLogfontWeight(
  int windows_logfont_weight
  )
{
  if ( windows_logfont_weight <= 0 || windows_logfont_weight > 1000 )
    return ON_Font::Weight::Normal;

  if ( windows_logfont_weight < 150 )
    return ON_Font::Weight::Thin;

  if ( windows_logfont_weight >= 850 )
    return ON_Font::Weight::Heavy;

  const ON_Font::Weight weights[] = 
  {
    ON_Font::Weight::Thin, // = 1
    ON_Font::Weight::Ultralight, // = 2
    ON_Font::Weight::Light, // = 3
    ON_Font::Weight::Normal, // = 4
    ON_Font::Weight::Medium, // = 5
    ON_Font::Weight::Semibold, // = 6
    ON_Font::Weight::Bold, // = 7
    ON_Font::Weight::Ultrabold, // = 8
    ON_Font::Weight::Heavy, // = 9
  };

  const size_t weight_count = sizeof(weights) / sizeof(weights[0]);
  ON_Font::Weight font_weight = ON_Font::Default.m_font_weight;
  int delta = std::abs(static_cast<int>(ON_Font::WindowsLogfontWeightFromWeight(font_weight)) - windows_logfont_weight);

  for (size_t i = 0; 0 != delta && i < weight_count; i++)
  {
    // look for a closer match
    int d = std::abs(static_cast<int>(ON_Font::WindowsLogfontWeightFromWeight(weights[i])) - windows_logfont_weight);
    if (d < delta)
    {
      font_weight = weights[i];
      delta = d;
    }
  }

  return font_weight;
}

ON_Font::Weight ON_Font::WeightFromAppleWeightOfFont(
  int apple_weight_of_font
  )
{
  return ON_Font::WeightFromWindowsLogfontWeight(apple_weight_of_font*100);
}

ON_Font::Weight ON_Font::WeightFromAppleFontWeightTrait(
  double apple_font_weight_trait
)
{
  if (false == ON_IsValid(apple_font_weight_trait))
    return ON_Font::Weight::Unset;

  const double x = (-1.0 <= apple_font_weight_trait && apple_font_weight_trait <= 1.0) ? apple_font_weight_trait : 0.0;
  int windows_logfont_weight = (int)(400.0 + 750.0*x);
  if (windows_logfont_weight < 1)
    windows_logfont_weight = 1;
  else if (windows_logfont_weight > 1000)
    windows_logfont_weight = 1000;
  return ON_Font::WeightFromWindowsLogfontWeight(windows_logfont_weight);
}

void ON_Font::CopyHelper(const ON_Font& src)
{
  m_font_weight = src.m_font_weight;
  m_windows_logfont_weight = src.m_windows_logfont_weight;
  m_apple_font_weight_trait = src.m_apple_font_weight_trait;

  m_font_style = src.m_font_style;
  m_font_stretch = src.m_font_stretch;
  m_font_bUnderlined = src.m_font_bUnderlined;
  m_font_bStrikethrough = src.m_font_bStrikethrough;
  m_logfont_charset = src.m_logfont_charset;

  memset(m_face_name, 0, sizeof(m_face_name));
  for (int i = 0; i < ON_Font::face_name_capacity && 0 != src.m_face_name[i]; i++)
  {
    m_face_name[i] = src.m_face_name[i];
  }

  m_font_description = src.m_font_description;

  m_apple_font_name = src.m_apple_font_name;
  if (0 == m_runtime_serial_number)
  {
    // destination font is not managed
    m_font_glyph_cache = src.m_font_glyph_cache;
  }

  m_point_size = src.m_point_size;

  m_font_characteristics_hash = ON_SHA1_Hash::ZeroDigest;
}

ON_Font::ON_Font()
{
  memset(m_face_name, 0, sizeof(m_face_name));
}

ON_Font::ON_Font(
  unsigned char managed_status,
  const ON_Font& src
  )
  : m_runtime_serial_number((1 == managed_status || 2 == managed_status) ? (++ON_Font::__runtime_serial_number_generator) : 0)
{
  CopyHelper(src);
}

ON_Font::ON_Font(const ON_Font& src)
  : m_runtime_serial_number(0)
{
  memset(m_face_name, 0, sizeof(m_face_name));
  CopyHelper(src);
}

ON_Font& ON_Font::operator=(const ON_Font& src)
{
  if (this != &src)
  {
    if (IsManagedFont() )
    {
      // managed fonts can never be modified
      if ( false == ON_Font::EqualFontCharacteristics(*this, src) )
      {
        ON_ERROR("Attempt to modify a managed font");
      }
    }
    else
    {
      CopyHelper(src);
    }
  }
  return *this;
}


bool ON_Font::SetFontCharacteristics(
  const wchar_t* face_name,
  bool bBold,
  bool bItalic,
  bool bUnderlined,
  bool bStrikethrough
  )
{
  return SetFontCharacteristics(
    0.0,
    face_name,
    bBold,
    bItalic,
    bUnderlined,
    bStrikethrough
  );
}

bool ON_Font::SetFontCharacteristics(
  double point_size,
  const wchar_t * face_name,
  bool bBold,
  bool bItalic, 
  bool bUnderlined,
  bool bStrikethrough
)
{
  if (nullptr == face_name || 0 == face_name[0] )
    face_name = ON_Font::Default.m_face_name;
  return SetFontCharacteristics(
    point_size,
    face_name,
    (bBold ? ON_Font::Weight::Bold :  ON_Font::Weight::Normal),
    (bItalic ? ON_Font::Style::Italic : ON_Font::Style::Upright),
    ON_Font::Default.m_font_stretch,
    bUnderlined,
    bStrikethrough,
    ON_FontMetrics::DefaultLineFeedRatio,
    ON_Font::WindowsLogfontCharSetFromFaceName(face_name)
    );
}

bool ON_Font::IsValidFaceName(
  const wchar_t* face_name
  )
{
  if ( nullptr == face_name || 0 == face_name[0] || ON_wString::Space == face_name[0])
    return false;
  
  int i = 0;
  while (i < ON_Font::face_name_capacity && 0 != face_name[i])
  {
    if (face_name[i] < ON_wString::Space )
      return false;
    switch (face_name[i])
    {
    case ';':
    case '"':
    case '\'':
    case '`':
    case '=':
    case '#':
      // lots more
      return false;
    //case '@': - There are valid fonts like @Gulim with '@' in the name.
    default:
      break;
    }
    
    i++;
  }

  if (0 != face_name[i])
    return false;

  return true;
}

ON_Font::Weight ON_Font::FontWeightFromUnsigned(
  unsigned int unsigned_font_weight
  )
{
  switch (unsigned_font_weight)
  {
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Unset);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Thin);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Ultralight);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Light);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Normal);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Medium);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Semibold);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Bold);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Ultrabold);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Weight::Heavy);
  default:
    break;
  }
  ON_ERROR("unsigned_font_weight is not valid");
  return ON_Font::Weight::Unset;
}

int ON_Font::CompareWeight(
  ON_Font::Weight weight_a,
  ON_Font::Weight weight_b
  )
{
  unsigned int a = static_cast<unsigned int>(weight_a);
  unsigned int b = static_cast<unsigned int>(weight_b);
  if ( a < b )
    return -1;
  if ( a < b )
    return 1;
  return 0;
}

ON_Font::Style ON_Font::FontStyleFromUnsigned(
  unsigned int unsigned_font_style
  )
{
  switch (unsigned_font_style)
  {
  ON_ENUM_FROM_UNSIGNED_CASE( ON_Font::Style::Upright);
  ON_ENUM_FROM_UNSIGNED_CASE( ON_Font::Style::Italic);
  ON_ENUM_FROM_UNSIGNED_CASE( ON_Font::Style::Oblique);
  default:
    break;
  }
  ON_ERROR("unsigned_font_style is not valid");
  return ON_Font::Style::Upright;
}

ON_Font::Stretch ON_Font::FontStretchFromUnsigned(
  unsigned int unsigned_font_stretch
  )
{
  switch (unsigned_font_stretch)
  {
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Unset);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Ultracondensed);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Extracondensed);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Condensed);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Semicondensed);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Medium);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Semiexpanded);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Expanded);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Extraexpanded);
  ON_ENUM_FROM_UNSIGNED_CASE(ON_Font::Stretch::Ultraexpanded);
  default:
    break;
  }
  ON_ERROR("unsigned_font_stretch is not valid");
  return ON_Font::Stretch::Unset;
}

unsigned int ON_Font::FontCharacteristicsAsUnsigned() const
{
  return ON_Font::Internal_FontCharacteristicsAsUnsigned(
    FontWeight(),
    FontStyle(),
    FontStretch(),
    m_font_bUnderlined,
    m_font_bStrikethrough
    );
}

unsigned int ON_Font::Internal_FontCharacteristicsAsUnsigned(
  ON_Font::Weight font_weight,
  ON_Font::Style font_style,
  ON_Font::Stretch font_stretch,
  bool bUnderlined,
  bool bStrikethrough
  )
{
  unsigned int a[][2]
    = {
      { 2U, 1U }, // insures 0 is not a valid FontCharacteristicsAsUnsigned() value.
      { 10U, static_cast<unsigned int>(font_weight) },
      { 4U, static_cast<unsigned int>(font_style) },
      { 10U, static_cast<unsigned int>(font_stretch) },
      // The static_cast<unsigned int> in b ? 1U : 0U is to work around a CLang compiler bug.
      { 2U, static_cast<unsigned int>(bUnderlined ? 1U : 0U) },
      { 2U, static_cast<unsigned int>(bStrikethrough ? 1U : 0U) }
      // insert new information below this line.
  };

  const int count = (int)(sizeof(a) / sizeof(a[0]));
  int i = count-1;
  unsigned int u = a[i][1] % a[i][0];

  for (i--; i >= 0; i--)
  {
    u = (u * a[i][0]) + (a[i][1] % a[i][0]);
  }

  return u;
}
    

bool ON_Font::Internal_SetFontCharacteristicsFromUnsigned(
  unsigned int font_characteristics_as_unsigned
  )
{
  unsigned int u = font_characteristics_as_unsigned;
  const unsigned int u_one = u % 2;
  u /= 2;
  const unsigned int u_font_weight = u % 10;
  u /= 10;
  const unsigned int u_font_style = u % 4;
  u /= 4;
  const unsigned int u_font_stretch = u % 10;
  u /= 10;
  const unsigned int u_bUnderlined = u % 2;
  u /= 2;
  const unsigned int u_bStrikethrough = u % 2;
  u /= 2;
  // extract new information below this line

  ON_Font::Weight font_weight = (1U == u_one && u_font_weight > 0) ? ON_Font::FontWeightFromUnsigned(u_font_weight) : ON_Font::Default.FontWeight();
  ON_Font::Style font_style = (1U == u_one) ? ON_Font::FontStyleFromUnsigned(u_font_style) : ON_Font::Default.FontStyle();
  ON_Font::Stretch font_stretch = (1U == u_one) ? ON_Font::FontStretchFromUnsigned(u_font_stretch) : ON_Font::Default.FontStretch();
  bool bUnderlined = (1U == u_one) ? (1 == u_bUnderlined) : ON_Font::Default.IsUnderlined();
  bool bStrikethrough = (1U == u_one) ? (1 == u_bStrikethrough) : ON_Font::Default.IsStrikethrough();
  
  return SetFontCharacteristics(
    m_face_name,
    font_weight,
    font_style,
    font_stretch,
    bUnderlined,
    bStrikethrough
    );
}

unsigned int ON_Font::CRC32(
   bool bIgnoreFaceNameOrdinalCase
  ) const
{
  unsigned int u = FontCharacteristicsAsUnsigned();
  wchar_t mapped_face_name[ON_Font::face_name_capacity+1];
  int element_count = 0;
  while (element_count < ON_Font::face_name_capacity && 0 != m_face_name[element_count] )
    element_count++;
  const wchar_t* face_name = m_face_name;
  if (bIgnoreFaceNameOrdinalCase)
  {
    ON_wString::MapStringOrdinal(
      ON_StringMapOrdinalType::MinimumOrdinal,
      m_face_name,
      element_count,
      mapped_face_name,
      ON_Font::face_name_capacity
      );
    face_name = mapped_face_name;
  }
  ON__UINT32 hash = ON_CRC32(0, sizeof(u), &u );
  hash = ON_CRC32(hash,element_count*sizeof(face_name[0]),face_name);

#if defined(ON_RUNTIME_WIN) 
  if ( m_point_size > 0.0 )
    hash = ON_CRC32(hash, sizeof(m_point_size), &m_point_size);
#endif

  return hash;
}

bool ON_Font::SetFontCharacteristics(
  const wchar_t* face_name,
  ON_Font::Weight font_weight,
  ON_Font::Style font_style,
  ON_Font::Stretch font_stretch,
  bool bUnderlined,
  bool bStrikethrough
)
{
  return SetFontCharacteristics(
    0.0,
    face_name,
    font_weight,
    font_style,
    font_stretch,
    bUnderlined,
    bStrikethrough
  );
}

bool ON_Font::SetFontCharacteristics(
  double point_size,
  const wchar_t* face_name,
  ON_Font::Weight font_weight,
  ON_Font::Style font_style,
  ON_Font::Stretch font_stretch,
  bool bUnderlined,
  bool bStrikethrough
  )
{
  const unsigned char logfont_charset = ON_Font::WindowsLogfontCharSetFromFaceName(face_name);

  double linefeed_ratio = ON_FontMetrics::DefaultLineFeedRatio;

  return SetFontCharacteristics(
    point_size,
    face_name,
    font_weight,
    font_style,
    font_stretch,
    bUnderlined,
    bStrikethrough,
    linefeed_ratio,
    logfont_charset
    );
}

bool ON_Font::SetFontCharacteristics(
  const wchar_t* face_name,
  ON_Font::Weight font_weight,
  ON_Font::Style font_style,
  ON_Font::Stretch font_stretch,
  bool bUnderlined,
  bool bStrikethrough,
  double linefeed_ratio,
  unsigned int logfont_charset
)
{
  return SetFontCharacteristics(
    0.0,
    face_name,
    font_weight,
    font_style,
    font_stretch,
    bUnderlined,
    bStrikethrough,
    linefeed_ratio,
    logfont_charset
  );
}

bool ON_Font::SetFontCharacteristics(
  double point_size,
  const wchar_t* face_name,
  ON_Font::Weight font_weight,
  ON_Font::Style font_style,
  ON_Font::Stretch font_stretch,
  bool bUnderlined,
  bool bStrikethrough,
  double linefeed_ratio,
  unsigned int logfont_charset
  )
{
  if (this == &ON_Font::Default)
    return false;

  if (false == ON_Font::IsValidFaceName(face_name))
    return false;

  if (logfont_charset >= 256)
    return false;

  ON_Font new_characteristics;
  
  for (int i = 0; i < ON_Font::face_name_capacity && 0 != face_name[i]; i++)
    new_characteristics.m_face_name[i] = face_name[i];

  new_characteristics.m_font_weight = ON_Font::FontWeightFromUnsigned(static_cast<unsigned char>(font_weight));
  new_characteristics.m_point_size 
    = (point_size > 0.0 && point_size < 2147483640.0)
    ? point_size
    : 0.0;
  new_characteristics.m_windows_logfont_weight = ON_Font::WindowsLogfontWeightFromWeight(new_characteristics.m_font_weight);
  new_characteristics.m_apple_font_weight_trait = ON_Font::AppleFontWeightTraitFromWeight(new_characteristics.m_font_weight);
  new_characteristics.m_font_style = font_style;
  new_characteristics.m_font_stretch = font_stretch;
  new_characteristics.m_font_bUnderlined = bUnderlined ? true : false;
  new_characteristics.m_font_bStrikethrough = bStrikethrough;

  if (ON_Font::logfont_symbol_charset == logfont_charset)
  {
    // verify this is correct.
    logfont_charset = ON_Font::WindowsLogfontCharSetFromFaceName(new_characteristics.m_face_name);
  }

  new_characteristics.m_logfont_charset = (unsigned char)logfont_charset;

  if ( 
    // 3 fast checks to avoid time consuming hash calculation
    0 != memcmp(m_face_name,new_characteristics.m_face_name,sizeof(m_face_name))
    || m_font_weight != new_characteristics.m_font_weight
    || m_font_style != new_characteristics.m_font_style
    || FontCharacteristicsHash() != new_characteristics.FontCharacteristicsHash() 
    )
  {
    if ( false == ON_FONT_MODIFICATION_PERMITTED )
      return false;
    CopyHelper(new_characteristics);
    if (0 == m_runtime_serial_number)
    {
      // destination font is not managed
      m_font_glyph_cache = nullptr;
    }
    Internal_SetFontDescription();
  }

  return true;
}

#if defined (ON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE)
bool ON_Font::SetFromAppleFont (NSFont* apple_font)
{
  const char* sAppleFontName = apple_font.fontName.UTF8String;
  const ON_wString apple_font_name = sAppleFontName;
  bool rc = SetFromAppleFontName(apple_font_name);
  if (rc)
  {
    const ON_wString saved_apple_font_name = m_apple_font_name;
    const ON_wString saved_font_description = m_font_description;

    // Set face name -- used if this font needs sustution on another computer
    // https://mcneel.myjetbrains.com/youtrack/issue/RH-37074
    const char* sAppleFontFamilyName = apple_font.familyName.UTF8String;
    const ON_wString face_name = sAppleFontFamilyName; // UTF-8 to wchar_t conversion
    SetFontFaceName(face_name);

    // Set weight - used if this font needs sustution on another computer
    // https://mcneel.myjetbrains.com/youtrack/issue/RH-37075
    NSFontDescriptor* fd = apple_font.fontDescriptor;
    NSDictionary* traits = [fd objectForKey: NSFontTraitsAttribute];
    NSNumber* weightValue = [traits objectForKey: NSFontWeightTrait];
    if (weightValue) 
    {
      const double apple_font_weight_trait = weightValue.doubleValue;
      SetAppleFontWeightTrait(apple_font_weight_trait);
    }
      
    // Set style - used if this font needs sustution on another computer
    const ON_Font::Style font_style
      = ( 0 != (fd.symbolicTraits & NSFontItalicTrait) )
      ? ON_Font::Style::Italic
      : ON_Font::Style::Upright;
    SetFontStyle(font_style);
      
    // Restore m_apple_font_name and m_font_description to the values set in SetFromAppleFontName().
    // These values may get modified by the SetFontFaceName() or SetFontStyle() calls.
    m_apple_font_name = saved_apple_font_name;
    m_font_description = saved_font_description;
  }
  return rc;
}


NSFont* ON_Font::AppleFont() const
{
  NSFont* userFont = nullptr;
  double pointSize = (double)FontMetrics().LineSpace() / 96.0 * 72.0;

  ON_String font_name = m_apple_font_name;    // convert to UTF8
  NSString* fullFontName = [NSString stringWithUTF8String: font_name];    // first try full font name
  userFont = [NSFont fontWithName: fullFontName size: pointSize];
  if (userFont)
    return userFont;
  
  // Try getting a NSFont by using NSFontManager
  NSFontTraitMask traits = 0;
  if (IsItalic())
    traits |= NSItalicFontMask;
  if (FontStretch() <= ON_Font::Stretch::Condensed)
    traits |= NSCondensedFontMask;
  if (FontStretch() >= ON_Font::Stretch::Expanded)
    traits |= NSExpandedFontMask;
  
  //  AppleWeightOfFontFromWeight returns a value from 0 to 9.  The fontWithFamily:traits:weight:size: method needs weights in the range 0 to 15.
  int onWeight = ON_Font::AppleWeightOfFontFromWeight (FontWeight());
  int weight = fmax(fmin(onWeight * 1.5, 15.0), 0.0);
  
  ON_String family_name = FontFaceName();    // convert to UTF8
  NSString* fontFamilyName = [NSString stringWithUTF8String: family_name];   // font family name
  
  userFont = [[NSFontManager sharedFontManager] fontWithFamily: fontFamilyName traits: traits weight: weight size: pointSize];
  if (userFont)
    return userFont;
  
  // Try using just FontFaceName()
  userFont = [NSFont fontWithName: fontFamilyName size: pointSize];
  if (userFont)
    return userFont;
  
  // Cannot find an equivalent font.  Just use a system font.
  userFont = [NSFont userFontOfSize: pointSize];
  if (userFont)
    return userFont;
  
  userFont = [NSFont systemFontOfSize: pointSize];
  return userFont;
}

#endif


bool ON_Font::SetFromAppleFontName(
  const wchar_t* apple_font_name
  )
{
  ON_wString local_apple_font_name(apple_font_name);
  local_apple_font_name.TrimLeftAndRight();
  apple_font_name = static_cast<const wchar_t*>(local_apple_font_name);
  
  const bool rc = SetFromFontDescription(apple_font_name,apple_font_name);

  // unconditionally set m_font_description and m_apple_font_name
  m_font_description = local_apple_font_name;
  m_apple_font_name = local_apple_font_name;

  return rc;
}


const wchar_t* ON_Font::AppleFontNameAsPointer() const
{
  return m_apple_font_name;
}



const ON_wString& ON_Font::AppleFontName() const
{
  return m_apple_font_name;
}


static bool IsAtoZ(
  const wchar_t* s
  )
{
  return (nullptr != s && ((s[0] >= 'A' && s[0] <= 'Z') || (s[0] >= 'a' && s[0] <= 'z')));
}

static const unsigned int ParseToken(
  const wchar_t*& s,
  size_t count,
  const wchar_t*const* token,
  const unsigned int* token_rc,
  unsigned int token_not_found_rc
  )
{
  if (IsAtoZ(s))
  {
    for (size_t i = 0; i < count; i++)
    {
      const int len = (int)ON_wString::Length(token[i]);
      if (ON_wString::EqualOrdinal(token[i], len, s, len, true))
      {
        s += len;
        return token_rc[i];
      }
    }
  }
  return token_not_found_rc;
}

static bool SkipSeparator(bool bSkipSpace, const wchar_t*& s)
{
  if (nullptr == s)
    return false;
  bool rc = false;
  switch (s[0])
  {
  case '-':
  case '_':
  case ',':
  case ';':
  case '.':
    rc = true;
    break;
  default:
    if (bSkipSpace && ON_wString::Space == s[0])
      rc = true;
    break;
  }
  if (rc)
    s++;
  return rc;
}

static bool ParseStretch(
  const wchar_t*& s,
  ON_Font::Stretch& font_stretch
  )
{
  if (false == IsAtoZ(s))
    return false;

  const wchar_t* s1 = s;

  const wchar_t* prefix_token[] = { L"SEMI", L"DEMI", L"EXTRA", L"ULTRA" };
  const size_t prefix_count = sizeof(prefix_token) / sizeof(prefix_token[0]);
  unsigned int prefix_rc[prefix_count] = { 1, 1, 2, 3 };
  const unsigned int prefix_id = ParseToken(s1, prefix_count, prefix_token, prefix_rc, 0);
  
  if ( 0 != prefix_id )
    SkipSeparator(true,s1);

  const unsigned int medium_rc = (0 != prefix_id) ? 0 : 2;
  const wchar_t* name_token[] = { L"CONDENSED", L"MEDIUM", L"NORMAL", L"EXPANDED",  };
  const size_t name_count = sizeof(name_token) / sizeof(name_token[0]);
  unsigned int name_rc[name_count] = { 1,medium_rc,medium_rc,3 };
  const unsigned int name_id = ParseToken(s1, name_count, name_token, name_rc, 0);
  if (0 == name_id)
    return false;


  bool rc = false;
  switch (name_id)
  {
  case 1: // Condensed
    switch (prefix_id)
    {
    case 0: // no prefix
      font_stretch = ON_Font::Stretch::Condensed;
      rc = true;
      break;
    case 1: // semi
      font_stretch = ON_Font::Stretch::Semicondensed;
      rc = true;
      break;
    case 2: // extra
      font_stretch = ON_Font::Stretch::Extracondensed;
      rc = true;
      break;
    case 3: // ultra
      font_stretch = ON_Font::Stretch::Ultracondensed;
      rc = true;
      break;
    }
    break;
  case 2: // Medium
    switch (prefix_id)
    {
    case 0: // no prefix
      font_stretch = ON_Font::Stretch::Medium;
      rc = true;
      break;
    }
    break;
  case 3: // Expanded
    switch (prefix_id)
    {
    case 0: // no prefix
      font_stretch = ON_Font::Stretch::Expanded;
      rc = true;
      break;
    case 1: // semi
      font_stretch = ON_Font::Stretch::Semiexpanded;
      rc = true;
      break;
    case 2: // extra
      font_stretch = ON_Font::Stretch::Extraexpanded;
      rc = true;
      break;
    case 3: // ultra
      font_stretch = ON_Font::Stretch::Ultraexpanded;
      rc = true;
      break;
    }
    break;
  }

  if (rc)
  {
    s = s1;
    return true;
  }

  return false;
}


static bool ParseWeight(
  const wchar_t*& s,
  ON_Font::Weight& font_weight
  )
{
  if (false == IsAtoZ(s))
    return false;

  const wchar_t* s1 = s;

  const wchar_t* prefix_token[] = { L"SEMI", L"DEMI", L"EXTRA", L"ULTRA" };
  const size_t prefix_count = sizeof(prefix_token) / sizeof(prefix_token[0]);
  unsigned int prefix_rc[prefix_count] = { 1, 1, 2, 2 };
  const unsigned int prefix_id = ParseToken(s1, prefix_count, prefix_token, prefix_rc, 0);
  
  if ( prefix_id > 0 )
    SkipSeparator(true,s1);

  const unsigned int medium_rc = (0 != prefix_id) ? 0 : 3;
  const unsigned int normal_rc = (0 != prefix_id) ? 0 : 4;

  const wchar_t* name_token[] = { L"THIN", L"LIGHT", L"NORMAL", L"REGULAR", L"MEDIUM", L"BOLD", L"HEAVY", L"BLACK" };
  const size_t name_count = sizeof(name_token) / sizeof(name_token[0]);
  unsigned int name_rc[name_count] = { 1, 2, normal_rc, normal_rc, medium_rc, 5, 6, 6 };
  const unsigned int name_id = ParseToken(s1, name_count, name_token, name_rc, 0);
  if (0 == name_id)
     return false;

  bool rc = false;
  switch (name_id)
  {
  case 1: // Thin
    font_weight = ON_Font::Weight::Thin;
    rc = true;
    break;

  case 2: // Light
    switch (prefix_id)
    {
    case 0:
      font_weight = ON_Font::Weight::Light;
      rc = true;
    case 1: // Semi
      font_weight = ON_Font::Weight::Light;
      rc = true;
    case 2: // Ultra
      font_weight = ON_Font::Weight::Ultralight;
      rc = true;
      break;
    }
    break;

  case 3: // Normal
    font_weight = ON_Font::Weight::Normal;
    rc = true;
    break;

  case 4: // Medium
    font_weight = ON_Font::Weight::Medium;
    rc = true;
    break;

  case 5: // Bold
    switch (prefix_id)
    {
    case 0:
      font_weight = ON_Font::Weight::Bold;
      rc = true;
    case 1: // Semi
      font_weight = ON_Font::Weight::Semibold;
      rc = true;
    case 2: // Ultra
      font_weight = ON_Font::Weight::Ultrabold;
      rc = true;
      break;
    }
    break;

  case 6: // Heavy
    font_weight = ON_Font::Weight::Heavy;
    rc = true;
    break;
  }

  if (rc)
  {
    s = s1;
    return true;
  }

  return false;
}

static bool ParseStyle(
  const wchar_t*& s,
  ON_Font::Style& font_style
  )
{
  if (false == IsAtoZ(s))
    return false;

  const wchar_t* s1 = s;

  const wchar_t* name_token[] = { L"UPRIGHT", L"ROMAN", L"ITALIC", L"OBLIQUE" };
  const size_t name_count = sizeof(name_token) / sizeof(name_token[0]);
  unsigned int name_rc[name_count] = { 1,1,2,3 };
  const unsigned int name_id = ParseToken(s1, name_count, name_token, name_rc, 0);
  if (0 == name_id)
     return false;

  bool rc = false;
  switch (name_id)
  {
  case 1: // Upright
    font_style = ON_Font::Style::Upright;
    rc = true;
    break;

  case 2: // Italic
    font_style = ON_Font::Style::Italic;
    rc = true;
    break;

  case 3: // Oblique
    font_style = ON_Font::Style::Oblique;
    rc = true;
    break;
  }

  if (rc)
  {
    s = s1;
    return true;
  }

  return false;
}

bool ON_Font::SetFromFontDescription(
  const wchar_t* font_description
)
{
  const wchar_t* apple_font_name = nullptr;
  return SetFromFontDescription(
    font_description,
    apple_font_name
  );
}

bool ON_Font::SetFromFontDescription(
  const wchar_t* font_description,
  const wchar_t* apple_font_name
  )
{
  ON_wString local_font_description(font_description);
  local_font_description.TrimLeftAndRight();
  font_description = static_cast<const wchar_t*>(local_font_description);

  ON_wString local_apple_font_name(apple_font_name);
  local_apple_font_name.TrimLeftAndRight();
  apple_font_name = static_cast<const wchar_t*>(local_apple_font_name);

  if (nullptr == font_description || font_description[0] <= ON_wString::Space)
  {
    font_description = apple_font_name;
    local_font_description = local_apple_font_name;
    if (nullptr == font_description || font_description[0] <= ON_wString::Space)
      return false;
  }

  // As names are discovered that do not work in the code below,
  // add a special case here.  These are typically fonts that have
  // words like Upright, Italic, Oblique, Regular, Semi, Demi, Extra, Ultra, Medium
  // Black, Heavy, ... as part of the face name and those words must not
  // be parsed as possible weight, style or stretch characteristics applied
  // to a root face name.
  const wchar_t* special_cases[] = {
    // Apple's "Times New Roman" and "Times New Roman Bold Italic" is an example of why
    // special cases are required.  The face name is "Times New Roman" and "Roman" no
    // longer indicates an upright style.
    // However, Apple's "Avenir Roman" and "Avenir Oblique" fonts provide an example
    // where "Roman" is style and "Avenir" is a face name.  The default parsing
    // handles Avenir and any other font names that are using Roman as a style.
    L"Times New Roman", 

    // Put new special case names above this nullptr which terminates the special_cases[] list.
    nullptr
  };

  // NOTE WELL: 
  //  It is important that local_face_name be created from a pointer
  //  so that it's string buffer is not shared with other ON_wStrings.
  //  wchar_t values in local_face_name are modifed via const_cast<> below.
  ON_wString local_face_name = static_cast<const wchar_t*>(font_description);
  const wchar_t* characteristics = nullptr;
  int face_name_length = local_face_name.Length();
  for (int i = 0; nullptr != special_cases[i]; i++)
  {
    const int special_case_length = ON_wString::Length(special_cases[i]);
    if (special_case_length > face_name_length)
      continue;
    
    if (special_case_length < face_name_length && local_face_name[special_case_length] > ON_wString::Space )
      continue;
    
    if (false == ON_wString::EqualOrdinal(special_cases[i], special_case_length, font_description, special_case_length, true))
      continue;

    characteristics = static_cast<const wchar_t*>(local_face_name) + special_case_length;
    break;
  }

  if (nullptr == characteristics)
    characteristics = static_cast<const wchar_t*>(local_face_name) + 1;

  const wchar_t x = (wchar_t)1;

  ON_Font::Weight font_weight = ON_Font::Default.m_font_weight;
  for (wchar_t* s0 = const_cast<wchar_t*>(characteristics); 0 != *s0; s0++)
  {
    const wchar_t* s1 = s0;
    if (ParseWeight(s1, font_weight))
    {
      while ( s0 < s1 )
        *s0++ = x; // NOTE - modifies local_face_name content
      break;
    }
  }

  ON_Font::Style font_style = ON_Font::Default.m_font_style;
  for (wchar_t* s0 = const_cast<wchar_t*>(characteristics); 0 != *s0; s0++)
  {
    const wchar_t* s1 = s0;
    if (ParseStyle(s1, font_style))
    {
      while ( s0 < s1 )
        *s0++ = x; // NOTE - modifies local_face_name content
      break;
    }
  }

  ON_Font::Stretch font_stretch = ON_Font::Default.m_font_stretch;
  for (wchar_t* s0 = const_cast<wchar_t*>(characteristics); 0 != *s0; s0++)
  {
    const wchar_t* s1 = s0;
    if (ParseStretch(s1, font_stretch))
    {
      while ( s0 < s1 )
        *s0++ = x; // NOTE - modifies local_face_name content
      break;
    }
  }

  face_name_length = 0;
  bool bCopyFontDescription = false;
  for (const wchar_t* s = static_cast<const wchar_t*>(local_face_name); 0 != *s; s++)
  {
    if (*s >= ON_wString::Space)
      face_name_length++;
    else
    {
      while (*s > 0 && *s <= ON_wString::Space)
        s++;
      if (0 == *s)
      {
        // all text after the face name was converted to a characteristic
        // Using a copy preserves word order so input descriptions like
        // "Avenir Next Condensed Heavy" don't gert reordered to "Avenir Next Heavy Condensed"
        bCopyFontDescription = true;
      }
      break;
    }
  }
  local_face_name.SetLength(face_name_length);
  local_face_name.TrimLeftAndRight();
  const wchar_t* face_name = static_cast<const wchar_t*>(local_face_name);

  bool rc = SetFontCharacteristics(
    face_name,
    font_weight,
    font_style,
    font_stretch,
    false,
    false,
    ON_FontMetrics::DefaultLineFeedRatio,
    ON_Font::Default.m_logfont_charset
    );

  if (rc)
  {
    if (bCopyFontDescription)
      m_font_description = local_font_description;
    m_apple_font_name = local_apple_font_name;
  }
  

  return rc;
}



//////////////////////////////////////////////////////////////////////
//
// ON_Object overrides

bool ON_Font::IsValid(ON_TextLog* text_log) const
{
  return (0 == m_face_name[ON_Font::face_name_capacity] && ON_Font::IsValidFaceName(m_face_name));
}

void ON_Font::Dump(ON_TextLog& dump) const
{
  const bool bTextHash = dump.IsTextHash();;

  if (bTextHash)
  {
    dump.Print("Font face name = ...\n");
    dump.PushIndent();
    dump.Print(
      "The font face name and other properties depend on the platform \n"
      "or the fonts installed on a particular computer. Information like \n"
      "this is omitted from dumps used for SHA-1 hash caluculations so \n"
      "hash values from different platforms and computers can be compared. \n"
    );
  }
  else
  {
    dump.Print("Font face name = \"%ls\"\n", FontFaceName());
    dump.PushIndent();
    if (FontDescription().IsNotEmpty())
      dump.Print("Descripton = %ls\n", FontDescriptionAsPointer());
    if (AppleFontName().IsNotEmpty())
      dump.Print("Apple font name = %ls\n", AppleFontNameAsPointer());
  }

  ON_wString s;

  if (m_point_size > 0.0)
  {
    dump.Print("PointSize = %g\n", m_point_size);
  }
  else
  {
    dump.Print("PointSize = ON_Font::Constants::AnnotationFontCellHeight (%d)\n", ON_Font::Constants::AnnotationFontCellHeight);
  }

  switch (this->FontWeight())
  {
    case ON_Font::Weight::Unset: s = "Unset"; break;

    case ON_Font::Weight::Thin: s = "Light-Thin"; break;
    case ON_Font::Weight::Ultralight: s = "Light-Ultralight"; break;
    case ON_Font::Weight::Light: s = "Light"; break;

    case ON_Font::Weight::Normal: s = "Normal"; break;
    case ON_Font::Weight::Medium: s = "Normal-Medium"; break;

    case ON_Font::Weight::Semibold: s = "Bold-Semibold"; break;
    case ON_Font::Weight::Bold: s = "Bold"; break;
    case ON_Font::Weight::Ultrabold: s = "Bold-Ultrabold"; break;
    case ON_Font::Weight::Heavy: s = "Bold-Heavy"; break;

    default: s = ON_wString::FormatToString(L"%u", static_cast<unsigned int>(this->FontWeight())); break;
  }
  dump.Print("Weight = %ls\n", static_cast<const wchar_t*>(s));

  switch (this->FontStyle())
  {
    case ON_Font::Style::Unset: s = "Unset"; break;
    case ON_Font::Style::Upright: s = "Upright"; break;
    case ON_Font::Style::Italic: s = "Italic"; break;
    case ON_Font::Style::Oblique: s = "Oblique"; break;
    default: s = ON_wString::FormatToString(L"%u", static_cast<unsigned int>(this->FontStyle())); break;
  }
  dump.Print("Style = %ls\n", static_cast<const wchar_t*>(s));

  switch (this->FontStretch())
  {
    case ON_Font::Stretch::Unset: s = "Unset"; break;
    case ON_Font::Stretch::Ultracondensed: s = "Ultracondensed"; break;
    case ON_Font::Stretch::Extracondensed: s = "Extracondensed"; break;
    case ON_Font::Stretch::Condensed: s = "Condensed"; break;
    case ON_Font::Stretch::Semicondensed: s = "Semicondensed"; break;
    case ON_Font::Stretch::Medium: s = "Medium"; break;
    case ON_Font::Stretch::Semiexpanded: s = "Semiexpanded"; break;
    case ON_Font::Stretch::Expanded: s = "Expanded"; break;
    case ON_Font::Stretch::Extraexpanded: s = "Extraexpanded"; break;
    case ON_Font::Stretch::Ultraexpanded: s = "Ultraexpanded"; break;
    default: s = ON_wString::FormatToString(L"%u", static_cast<unsigned int>(this->FontStretch())); break;
  }
  dump.Print("Stretch = %ls\n", static_cast<const wchar_t*>(s));

  dump.Print("Underlined = %ls\n", this->IsUnderlined() ? "true" : "false");

  dump.Print("Strikethrough = %ls\n", this->IsStrikethrough() ? "true" : "false");

  if (false == bTextHash)
  {
    const ON_wString characteristics_hash = FontCharacteristicsHash().ToString(true);
    dump.Print(L"Font characteristics SHA-1 hash = %ls\n", static_cast<const wchar_t*>(characteristics_hash));

    unsigned int runtime_sn = RuntimeSerialNumber();
    if (runtime_sn >= 1)
    {
      if (this == &ON_Font::Default)
        dump.Print("Managed font <%u> (ON_Font::Default)\n", runtime_sn);
      else
        dump.Print("Managed font <%u>\n", runtime_sn);
    }

#if defined(ON_OS_WINDOWS_GDI)
    // LOGFONT details
    dump.Print("LOGFONT\n");
    const LOGFONT logfont = this->WindowsLogFont(0, nullptr);
    ON_Font::DumpLogfont(&logfont, dump);
#endif

    // Free Type font details
    DumpFreeType(dump);
  }
  dump.PopIndent();
}

#if defined(ON_OS_WINDOWS_GDI)
void ON_Font::DumpLogfont(
  const LOGFONT* logfont,
  ON_TextLog& text_log
)
{
  if (nullptr == logfont)
  {
    text_log.Print("LOGFONT = nullptr\n");
    return;
  }

  text_log.Print("LOGFONT\n");
  text_log.PushIndent();
  ON_wString s = logfont->lfFaceName;

  text_log.Print("lfFaceName = %ls\n",static_cast<const wchar_t*>(s));
  text_log.Print("Height = %d\n", logfont->lfHeight);
  text_log.Print("Width = %d\n", logfont->lfWidth);
  text_log.Print("Escapement = %d.%d degrees\n", (logfont->lfEscapement)/10, (logfont->lfEscapement)%10);
  text_log.Print("Orientation = %d.%d degrees\n",  (logfont->lfOrientation)/10, (logfont->lfOrientation)%10);

  switch (logfont->lfWeight)
  {
  case FW_DONTCARE: s = "FW_DONTCARE"; break;
  case FW_THIN: s = "FW_THIN"; break;
  case FW_EXTRALIGHT: s = "FW_EXTRALIGHT = FW_ULTRALIGHT"; break;
  case FW_LIGHT: s = "FW_LIGHT"; break;
  case FW_NORMAL: s = "FW_NORMAL = FW_REGULAR"; break;
  case FW_MEDIUM: s = "FW_MEDIUM"; break;
  case FW_SEMIBOLD: s = "FW_SEMIBOLD = FW_DEMIBOLD"; break;
  case FW_BOLD: s = "FW_BOLD"; break;
  case FW_EXTRABOLD: s = "FW_EXTRABOLD = FW_ULTRABOLD"; break;
  case FW_HEAVY: s = "FW_HEAVY = FW_BLACK"; break;
  default: s = ON_wString::EmptyString; break;
  }
  if (s.IsNotEmpty())
    s += " = ";
  s += ON_wString::FormatToString(L"%d", logfont->lfWeight);
  text_log.Print("Weight = %ls\n", static_cast<const wchar_t*>(s));


  text_log.Print("Italic = %ls\n", logfont->lfItalic ? L"true" : L"false");
  text_log.Print("Underline = %ls\n", logfont->lfUnderline ? L"true" : L"false");
  text_log.Print("StrikeOut = %ls\n", logfont->lfStrikeOut ? L"true" : L"false");
  switch (logfont->lfCharSet)
  {
  case ANSI_CHARSET: s = "ANSI_CHARSET"; break;
  case DEFAULT_CHARSET: s = "DEFAULT_CHARSET"; break;
  case SYMBOL_CHARSET: s = "SYMBOL_CHARSET"; break;
  case SHIFTJIS_CHARSET: s = "SHIFTJIS_CHARSET"; break;
  case HANGEUL_CHARSET: s = "HANGEUL_CHARSET"; break;
  case GB2312_CHARSET: s = "GB2312_CHARSET"; break;
  case CHINESEBIG5_CHARSET: s = "CHINESEBIG5_CHARSET"; break;
  case OEM_CHARSET: s = "OEM_CHARSET"; break;
#if(WINVER >= 0x0400)
  case JOHAB_CHARSET: s = "JOHAB_CHARSET"; break;
  case HEBREW_CHARSET: s = "HEBREW_CHARSET"; break;
  case ARABIC_CHARSET: s = "ARABIC_CHARSET"; break;
  case GREEK_CHARSET: s = "GREEK_CHARSET"; break;
  case TURKISH_CHARSET: s = "TURKISH_CHARSET"; break;
  case VIETNAMESE_CHARSET: s = "VIETNAMESE_CHARSET"; break;
  case THAI_CHARSET: s = "THAI_CHARSET"; break;
  case EASTEUROPE_CHARSET: s = "EASTEUROPE_CHARSET"; break;
  case RUSSIAN_CHARSET: s = "RUSSIAN_CHARSET"; break;
  case MAC_CHARSET: s = "MAC_CHARSET"; break;
  case BALTIC_CHARSET: s = "BALTIC_CHARSET"; break;
#endif
  default: s = ON_wString::EmptyString; break;
  }
  if (s.IsNotEmpty())
    s += L" = ";
  s += ON_wString::FormatToString(L"%d", (unsigned int)logfont->lfCharSet);
  text_log.Print("CharSet = %ls\n", static_cast<const wchar_t*>(s));

  switch (logfont->lfOutPrecision)
  {
  case OUT_DEFAULT_PRECIS: s = "OUT_DEFAULT_PRECIS"; break;
  case OUT_STRING_PRECIS: s = "OUT_STRING_PRECIS"; break;
  case OUT_CHARACTER_PRECIS: s = "OUT_CHARACTER_PRECIS"; break;
  case OUT_STROKE_PRECIS: s = "OUT_STROKE_PRECIS"; break;
  case OUT_TT_PRECIS: s = "OUT_TT_PRECIS"; break;
  case OUT_DEVICE_PRECIS: s = "OUT_DEVICE_PRECIS"; break;
  case OUT_RASTER_PRECIS: s = "OUT_RASTER_PRECIS"; break;
  case OUT_TT_ONLY_PRECIS: s = "OUT_TT_ONLY_PRECIS"; break;
  case OUT_OUTLINE_PRECIS: s = "OUT_OUTLINE_PRECIS"; break;
  case OUT_SCREEN_OUTLINE_PRECIS: s = "OUT_SCREEN_OUTLINE_PRECIS"; break;
  case OUT_PS_ONLY_PRECIS: s = "OUT_PS_ONLY_PRECIS"; break;
  };
  if (s.IsNotEmpty())
    s += L" = ";
  s += ON_wString::FormatToString(L"%d", (unsigned int)logfont->lfOutPrecision);
  text_log.Print("OutPrecision = %ls\n", static_cast<const wchar_t*>(s));

  text_log.Print("ClipPrecision = 0x%02x\n", logfont->lfOutPrecision);
  text_log.Print("Quality = 0x%02x\n", logfont->lfQuality);

  const unsigned int pitch = (logfont->lfPitchAndFamily & 0x03);
  const unsigned int family = (logfont->lfPitchAndFamily & 0xFC);
  switch (pitch)
  {
  case DEFAULT_PITCH: s = "DEFAULT_PITCH"; break;
  case FIXED_PITCH: s = "FIXED_PITCH"; break;
  case VARIABLE_PITCH: s = "VARIABLE_PITCH"; break;
  default: s = ON_wString::FormatToString(L"0x%02x", pitch);
  };
  s += " | ";
  switch (family)
  {
  case FF_DONTCARE: s += "FF_DONTCARE"; break;
  case FF_ROMAN: s += "FF_ROMAN"; break;
  case FF_SWISS: s += "FF_SWISS"; break;
  case FF_MODERN: s += "FF_MODERN"; break;
  case FF_SCRIPT: s += "FF_SCRIPT"; break;
  case FF_DECORATIVE: s += "FF_DECORATIVE"; break;
  default: s += ON_wString::FormatToString(L"0x%02x", family);
  };
  if (s.IsNotEmpty())
    s += L" = ";
  s += ON_wString::FormatToString(L"0x%02x", (unsigned int)logfont->lfPitchAndFamily);
  text_log.Print("PitchAndFamily = %ls\n", static_cast<const wchar_t*>(s));
  text_log.PopIndent();
}
#endif

bool ON_Font::Write(
  ON_BinaryArchive& file // serialize definition to binary archive
  ) const
{

  if (file.Archive3dmVersion() < 60 
    || file.ArchiveOpenNURBSVersion() < ON_TextStyle::binary_archive_opennurbs_version
    )
  {
    ON_WARNING("This font should probably be an ON_TextStyle.");
    return WriteV5(
      RuntimeSerialNumber(),
      ON_nil_uuid,
      file
      );
  }


  if (!file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,3))
    return false;

  bool rc = false;
  for (;;)
  {
    // version 1.0
    unsigned int fc = FontCharacteristicsAsUnsigned();
    if (!file.WriteInt(fc))
      break;
    int face_name_length = 0;
    while (face_name_length < ON_Font::face_name_capacity)
    {
      if ( 0 == m_face_name[face_name_length] )
        break;
      face_name_length++;
    }
    if ( !file.WriteWideString(m_face_name,face_name_length) )
      break;
    if (!file.WriteString(m_apple_font_name))
      break;

    // version 1.1 added font_description August 2016
    if (!file.WriteString(m_font_description))
      break;

    // version 1.2 added m_windows_logfont_weight and m_apple_font_weight_trait
    if (!file.WriteInt(m_windows_logfont_weight))
      break;

    if (!file.WriteDouble(m_apple_font_weight_trait))
      break;

    // version 1.3 added additional m_point_size and m_LOGFONT_* values.
    if (!file.WriteDouble(m_point_size))
      break;

    const bool bOBSOLETEBool = false;
    if (!file.WriteBool(bOBSOLETEBool))
      break;

    rc = true;
    break;
  }

  if (!file.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool ON_Font::WriteV5(
  int V5_font_index,
  ON_UUID V5_font_id,
  ON_BinaryArchive& file // serialize definition to binary archive
  ) const
{
  bool rc = file.Write3dmChunkVersion(1, 2);
  while(rc)
  {
    rc = file.Write3dmReferencedComponentIndex(ON_ModelComponent::Type::TextStyle,V5_font_index);
    if(!rc) 
      break;

    // Mac Rhino 5 uses the V5 "m_font_description" field to store Apple font names because
    // there was no other appropriate place to save the information in a V5 file format.

    const ON_wString font_description
      = (m_font_description.IsEmpty() || (ON::RuntimeEnvironment::Apple == file.ArchiveRuntimeEnvironment() && m_apple_font_name.IsNotEmpty()) )
      ? m_apple_font_name
      : m_font_description;
    rc = file.WriteString(font_description);
    if(!rc) 
      break;

    {
      // 18 October 2002 Dale Lear:
      //   Lowell, wcha:r_t has different sizes on different OSs.
      //   When writing a wchar_t string, you should use one
      //   of the WriteString functions.  This function must continue
      //   to use WriteShort(64,...) so old files will remain valid.
      unsigned short sh[64];
      memset(sh, 0, sizeof(sh));
      int i;
      for(i = 0; i < 64 && i < ON_Font::face_name_capacity; i++)
        sh[i] = m_face_name[i];
      rc = file.WriteShort(64, sh);
      if(!rc) break;
    }

    // 1.1 additions
    int windows_logfont_weight = ON_Font::WindowsLogfontWeightFromWeight(m_font_weight);
    if (file.Archive3dmVersion() < 60)
    {
      // V5 and earlier files had 4 permitted weights
      //  Light  = 300
      //  Normal = 400
      //  Medium = 500
      //  Bold   = 700
      if ( windows_logfont_weight <= 0 )
        windows_logfont_weight = 400;
      if ( windows_logfont_weight < 350 )
        windows_logfont_weight = 300;
      else if ( windows_logfont_weight < 450 )
        windows_logfont_weight = 400;
      else if ( windows_logfont_weight < 600 )
        windows_logfont_weight = 500;
      else if ( windows_logfont_weight <= 1000 )
        windows_logfont_weight = 700;
      else
        windows_logfont_weight = 400;
    }
    
    rc = file.WriteInt(static_cast<int>(windows_logfont_weight));
    if(!rc) break;
    rc = file.WriteInt(ON_Font::Style::Italic == m_font_style);
    if(!rc) break;
    rc = file.WriteDouble(ON_FontMetrics::DefaultLineFeedRatio);
    if(!rc) break;

    // 1.2 addition
    rc = file.WriteUuid(V5_font_id);
    if(!rc) break;

    break;
  }

  return rc;
}

bool ON_Font::Read(
  ON_BinaryArchive& file // restore definition from binary archive
  )
{
  // On September 16, 2015 the "V5" ON_Font was split into
  //   ON_TextStyle (a document object) and
  //   ON_Font (a current runtime resource)
  *this = ON_Font::Default;

  ON__UINT32 typecode = 0;
  ON__INT64 big_value = 0;
  if (file.Archive3dmVersion() < 60
    || file.ArchiveOpenNURBSVersion() < ON_TextStyle::binary_archive_opennurbs_version
    || (file.PeekAt3dmBigChunkType(&typecode,&big_value) && 1 == typecode)
    )
  {
    ON_WARNING("Should probably be reading an ON_TextStyle");
    int font_index = -1;
    ON_UUID font_id = ON_nil_uuid;
    return ReadV5(
      file,
      &font_index,
      &font_id
      );
  }

  int major_verision = 0;
  int minor_verision = 0;
  if (!file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_verision,&minor_verision))
    return false;

  bool rc = false;
  for (;;)
  {
    if ( 1 != major_verision )
      break;
    unsigned int fc = 0;
    if (!file.ReadInt(&fc))
      break;

    ON_wString face_name;
    if ( !file.ReadWideString(face_name) )
      break;
    SetFontFaceName(static_cast<const wchar_t*>(face_name));
    Internal_SetFontCharacteristicsFromUnsigned(fc);

    if (!file.ReadString(m_apple_font_name))
      break;

    if (ON::RuntimeEnvironment::Windows == file.ArchiveRuntimeEnvironment())
    {
      // Dale Lear - August 16, 2016.
      // The value of m_apple_font_name is damaged in many archives written by Windows Rhino.
      // It has values like "Font 01". So, I'm going to clean it
      // during read and we will get it right going forward.
      // This value is not saved in V5 files.
      const unsigned int broken_apple_font_name_version = ON_VersionNumberConstruct(6, 0, 2016, 8, 18, 0);
      const unsigned int archive_version = file.Archive3dmVersion();
      const unsigned int archive_opennurbs_version = file.ArchiveOpenNURBSVersion();
      if (
        archive_version < 60
        || (60 == archive_version && archive_opennurbs_version <= broken_apple_font_name_version)
        )
      {
        m_apple_font_name = ON_wString::EmptyString;
      }
    }

    if (minor_verision <= 0)
    {
      Internal_SetFontDescription();
      rc = true;
      break;
    }

    // version 1.1 added m_font_description August 2016
    if ( !file.ReadString(m_font_description) )
      break;

    if (minor_verision <= 1)
    {
      // m_windows_logfont_weight and  m_apple_font_weight_trait
      // are set above in the call to SetFontCharacteristicsFromUnsigned().
      rc = true;
      break;
    }

    // version 1.2 added m_windows_logfont_weight and m_apple_font_weight_trait
    if (!file.ReadInt(&m_windows_logfont_weight))
      break;

    if (!file.ReadDouble(&m_apple_font_weight_trait))
      break;

    if (minor_verision <= 2)
    {
      rc = true;
      break;
    }

    // version 1.3 added additional m_point_size and m_LOGFONT_* values.
    if (!file.ReadDouble(&m_point_size))
      break;

    bool bOBSOLETE_Bool = false;
    if (!file.ReadBool(&bOBSOLETE_Bool))
      break;
    if (bOBSOLETE_Bool)
    {
      unsigned char obsolete_c = 0;
      if (!file.ReadChar(&obsolete_c))
        break;
      if (!file.ReadChar(&obsolete_c))
        break;
      if (!file.ReadChar(&obsolete_c))
        break;
      if (!file.ReadChar(&obsolete_c))
        break;
      int obsolete_i = 0;
      if (!file.ReadInt(&obsolete_i))
        break;
      if (!file.ReadInt(&obsolete_i))
        break;
      if (!file.ReadInt(&obsolete_i))
        break;
      if (!file.ReadInt(&obsolete_i))
        break;
    }

    rc = true;
    break;
  }

  if (!file.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool ON_Font::ReadV5(
  ON_BinaryArchive& file, // restore definition from binary archive
  int* V5_font_index,
  ON_UUID* V5_font_id
  )
{
  *this = ON_Font::Default;
  if ( nullptr != V5_font_index )
    *V5_font_index = -1;
  if ( nullptr != V5_font_id )
    *V5_font_id = ON_nil_uuid;
  
  int major_version = 0;
  int minor_version = 0;
  if (!file.Read3dmChunkVersion(&major_version, &minor_version))
    return false;

  bool rc = false;
  ON_wString apple_font_name;
  for (;;)
  {
    if ( 1 != major_version )
      break;
    int i;
    if (!file.ReadInt(&i))
      break;
    if ( nullptr != V5_font_index )
      *V5_font_index = i;

    ON_wString corrupt_information_font_description;
    if (!file.ReadString(corrupt_information_font_description))
      break;

    if (ON::RuntimeEnvironment::Apple == file.ArchiveRuntimeEnvironment()
      && false == ON_Font::IsNotAppleFontName(corrupt_information_font_description)
      )
    {
      // Files written by Mac Rhino 5 for Mac have the Apple font name stored in this string.
      apple_font_name = corrupt_information_font_description;
    }
 
    // 18 October 2002 Dale Lear:
    //   Lowell, wchar_t has different sizes on different OSs.
    //   When writing a wchar_t string, you should use one
    //   of the WriteString functions.  This function must continue
    //   to use ReadShort(64,...) so old files will remain valid.
    unsigned short sh[64];
    if (!file.ReadShort(64, sh))
      break;

    wchar_t facename[65];
    for(i = 0; i < 64; i++)
      facename[i] = sh[i];
    facename[64] = 0;

    SetFontFaceName(facename);
    m_logfont_charset = ON_Font::WindowsLogfontCharSetFromFaceName(m_face_name);
    if (minor_version >= 1)
    {
      int logfont_weight = 0;
      if (!file.ReadInt(&logfont_weight))
        break;
      if (logfont_weight >= 100 && logfont_weight <= 1000)
      {
        Internal_SetFontWeightTrio(
          ON_Font::WeightFromWindowsLogfontWeight(logfont_weight),
          logfont_weight,
          ON_UNSET_VALUE,
          false
        );
      }

      int bItalic = 0;
      if (!file.ReadInt(&bItalic))
        break;
      if ( 0 != bItalic )
        m_font_style = ON_Font::Style::Italic;

      double obsolete_linefeed_ratio = 1.6;
      if (!file.ReadDouble(&obsolete_linefeed_ratio))
        break;

      if (minor_version >= 2)
      {
        ON_UUID uuid = ON_nil_uuid;
        if (!file.ReadUuid(uuid))
          break;
        if ( nullptr != V5_font_id )
          *V5_font_id = uuid;
      }
    }

    rc = true;
    break;
  }

  if (apple_font_name.IsNotEmpty())
  {
    m_font_description = apple_font_name;
  }
  else if (m_face_name[0] > ON_wString::Space)
  {
    Internal_SetFontDescription();
  }
  else
  {
    m_font_description = ON_wString::EmptyString;
  }

  m_apple_font_name = apple_font_name;

  return rc;
}

unsigned int ON_Font::RuntimeSerialNumber() const
{
  return m_runtime_serial_number;
}

unsigned int ON_Font::ManagedFontSerialNumber() const
{
  if (0 != m_runtime_serial_number)
    return m_runtime_serial_number;
  const ON_Font* mananged_font = this->ManagedFont();
  return (nullptr == mananged_font) ? 0 : mananged_font->RuntimeSerialNumber();
}

#if defined(ON_OS_WINDOWS_GDI)
static
int CALLBACK ON__IsSymbolCharSetFontFaceNameHelper(ENUMLOGFONTEX* lf, NEWTEXTMETRICEX* tm, DWORD font_type, LPARAM)
{
  // If the fontname in the logfont structure has
  // a corresponding symbol font on the system, 
  // set the  lfCharSet member to SYMBOL_CHARSET, 
  // otherwise DEFAULT_CHARSET
  // The input logfont structure may be modified.
  return 7;
}
#endif

unsigned char ON_Font::WindowsLogfontCharSetFromFaceName(
  const wchar_t* face_name
  )
{
  unsigned char logfont_charset = ON_Font::WindowsConstants::logfont_default_charset;

#if defined(ON_OS_WINDOWS_GDI)
  if( nullptr != face_name && face_name[0] > ON_wString::Space )
  {
    HDC hdc = ::GetDC(nullptr);
    if(hdc)
    {
      // See if there is a font with this facename that has the symbol charset 
      LOGFONT logfont;
      memset(&logfont, 0, sizeof(logfont));
      for(int i = 0; i < LF_FACESIZE && face_name[i]; i++)
        logfont.lfFaceName[i] = face_name[i];

      // Dale lear - set logfont.lfOutPrecision May 2017
      logfont.lfOutPrecision = ON_Font::WindowsConstants::logfont_out_precis;

      // Is it a SYMBOL_CHARSET font?
      logfont.lfCharSet = ON_Font::WindowsConstants::logfont_symbol_charset;
      if (7 == ::EnumFontFamiliesEx(hdc, &logfont, (FONTENUMPROC)ON__IsSymbolCharSetFontFaceNameHelper, 0, 0))
      {
        // Yes, this facename is a "symbol font"
        logfont_charset = ON_Font::WindowsConstants::logfont_symbol_charset;
      }
      ::ReleaseDC(nullptr, hdc);
    }
  }
#endif

  return logfont_charset;
}

const ON_wString& ON_Font::FontDescription() const
{
  return m_font_description;
}

const wchar_t* ON_Font::FontDescriptionAsPointer() const
{
  return static_cast<const wchar_t*>(m_font_description);
}

void ON_Font::Internal_SetFontDescription()
{
  ON_wString local_font_description = m_face_name;
  m_font_description = ON_wString::EmptyString;

#if defined (ON_RUNTIME_APPLE)
  if (m_apple_font_name.Length() > 0)
  {
    local_font_description = m_apple_font_name;
    m_font_description = local_font_description;
    return;    
  }
#endif
  
  const double point_size = PointSize();
  if (point_size > 0.0)
  {
    local_font_description += ON_wString::FormatToString(L" %g point",point_size);
  }

  switch (FontWeight())
  {
  case ON_Font::Weight::Unset:
    //local_font_description += L" Unsetweight";
    break;
  case ON_Font::Weight::Thin:
    local_font_description += L" Thin";
    break;
  case ON_Font::Weight::Ultralight:
    local_font_description += L" Ultralight";
    break;
  case ON_Font::Weight::Light:
    local_font_description += L" Light";
    break;
  case ON_Font::Weight::Normal:
    //local_font_description += L" Normal";
    break;
  case ON_Font::Weight::Medium:
    local_font_description += L" Medium";
    break;
  case ON_Font::Weight::Semibold:
    local_font_description += L" Semibold";
    break;
  case ON_Font::Weight::Bold:
    local_font_description += L" Bold";
    break;
  case ON_Font::Weight::Ultrabold:
    local_font_description += L" Ultrabold";
    break;
  case ON_Font::Weight::Heavy:
    local_font_description += L" Heavy";
    break;
  default:
    break;
  }

  switch (FontStyle())
  {
  case ON_Font::Style::Upright:
    //local_font_description += L" Upright";
    break;
  case ON_Font::Style::Italic:
    local_font_description += L" Italic";
    break;
  case ON_Font::Style::Oblique:
    local_font_description += L" Oblique";
    break;
  default:
    break;
  }

  switch (m_font_stretch)
  {
  case ON_Font::Stretch::Unset:
    //local_font_description += L" Unsetstretch";
    break;
  case ON_Font::Stretch::Ultracondensed:
    local_font_description += L" Ultracondensed";
    break;
  case ON_Font::Stretch::Extracondensed:
    local_font_description += L" Extracondensed";
    break;
  case ON_Font::Stretch::Condensed:
    local_font_description += L" Condensed";
    break;
  case ON_Font::Stretch::Semicondensed:
    local_font_description += L" Semicondensed";
    break;
  case ON_Font::Stretch::Medium:
    //local_font_description += L" Medium";
    break;
  case ON_Font::Stretch::Semiexpanded:
    local_font_description += L" Semiexpanded";
    break;
  case ON_Font::Stretch::Expanded:
    local_font_description += L" Expanded";
    break;
  case ON_Font::Stretch::Extraexpanded:
    local_font_description += L" Extraexpanded";
    break;
  case ON_Font::Stretch::Ultraexpanded:
    local_font_description += L" Ultraexpanded";
    break;
  default:
    break;
  };

  if(IsUnderlined())
    local_font_description += L" Underlined";

  if(IsStrikethrough())
    local_font_description += L" Strikethrough";

  local_font_description.TrimLeft();

  m_font_description = local_font_description;
}

double ON_Font::LinefeedRatio() const
{
  return ON_FontMetrics::DefaultLineFeedRatio;
}

bool ON_Font::SetFontFaceName(
  const wchar_t* face_name
  )
{
  if (false == ON_Font::IsValidFaceName(face_name))
    return false;

  if ( ON_wString::EqualOrdinal(face_name,m_face_name,false) )
    return true;

  if ( false == ON_FONT_MODIFICATION_PERMITTED )
    return false;

  memset(m_face_name, 0, sizeof(m_face_name));
  for (int i = 0; i < ON_Font::face_name_capacity && 0 != face_name[i]; i++)
    m_face_name[i] = face_name[i];

  if (0 == m_logfont_charset || ON_Font::logfont_symbol_charset == m_logfont_charset )
    m_logfont_charset = ON_Font::WindowsLogfontCharSetFromFaceName(m_face_name);

  Internal_SetFontDescription();

  return true;
}

const wchar_t* ON_Font::FontFaceName() const
{
  return static_cast<const wchar_t*>(m_face_name);
}

ON_Font::Weight ON_Font::FontWeight() const
{
  return m_font_weight;
}

bool ON_Font::IsLight() const
{
  const int font_weight = ON_Font::WindowsLogfontWeightFromWeight(m_font_weight);
  const int normal_weight = ON_Font::WindowsLogfontWeightFromWeight(ON_Font::Weight::Normal);
  return (font_weight < normal_weight && m_font_weight != ON_Font::Weight::Unset);
}

bool ON_Font::IsNormalWeight() const
{
  return (ON_Font::Weight::Normal == m_font_weight || ON_Font::Weight::Medium == m_font_weight);
}

bool ON_Font::IsBold() const
{
  const int font_weight = ON_Font::WindowsLogfontWeightFromWeight(m_font_weight);
  const int bold_threshhold_weight = ON_Font::WindowsLogfontWeightFromWeight(ON_Font::Weight::Semibold);
  return (font_weight >= bold_threshhold_weight && m_font_weight != ON_Font::Weight::Unset);
}

bool ON_Font::IsItalic() const
{
  return (ON_Font::Style::Italic == m_font_style);
}

bool ON_Font::IsUpright() const
{
  return (ON_Font::Style::Upright == m_font_style);
}

bool ON_Font::IsOblique()
{
  return (ON_Font::Style::Oblique == m_font_style);
}


bool ON_Font::Internal_SetFontWeightTrio(
  ON_Font::Weight font_weight,
  int windows_logfont_weight,
  double apple_font_weight_trait,
  bool bUpdateFontDescription
)
{
  font_weight = ON_Font::FontWeightFromUnsigned((unsigned int)static_cast<unsigned char>(font_weight));
  if (ON_Font::Weight::Unset == font_weight)
    return false;

  if (false == (1 <= windows_logfont_weight && windows_logfont_weight <= 1000))
    windows_logfont_weight = ON_Font::WindowsLogfontWeightFromWeight(font_weight);

  if (false == (-1.0 <= apple_font_weight_trait && apple_font_weight_trait <= 1.0) )
    apple_font_weight_trait = ON_Font::AppleFontWeightTraitFromWeight(font_weight);
  
  if ( 
    font_weight != m_font_weight
    || false == (apple_font_weight_trait == m_apple_font_weight_trait) // handles nans correctly
    || windows_logfont_weight != m_windows_logfont_weight
    )
  {
    if (false == ON_FONT_MODIFICATION_PERMITTED)
      return false;
    
    if (font_weight != m_font_weight)
      m_font_weight = font_weight;

    if (false == (apple_font_weight_trait == m_apple_font_weight_trait))
      m_apple_font_weight_trait = apple_font_weight_trait;

    if (windows_logfont_weight != m_windows_logfont_weight)
      m_windows_logfont_weight = windows_logfont_weight;

    if ( bUpdateFontDescription )
      Internal_SetFontDescription();
  }

  return true;
}


bool ON_Font::SetFontWeight(ON_Font::Weight font_weight)
{
  return Internal_SetFontWeightTrio(
    font_weight, 
    -1, 
    ON_UNSET_VALUE,
    font_weight != m_font_weight
  );
}


bool ON_Font::SetWindowsLogfontWeight(
  int windows_logfont_weight
)
{
  const ON_Font::Weight font_weight = ON_Font::WeightFromWindowsLogfontWeight(windows_logfont_weight);
  return Internal_SetFontWeightTrio(
    font_weight, 
    windows_logfont_weight, 
    ON_UNSET_VALUE,
    windows_logfont_weight != m_windows_logfont_weight
  );
}

int ON_Font::WindowsLogfontWeight() const
{
  return
    (100 <= m_windows_logfont_weight && m_windows_logfont_weight <= 1000)
    ? m_windows_logfont_weight
    : ON_Font::WindowsLogfontWeightFromWeight(FontWeight());
}

bool ON_Font::SetAppleWeightOfFont(
  int apple_weight_of_font
)
{
  const bool bUpdateFontDescription = m_apple_font_name.IsEmpty() || m_apple_font_name != m_font_description;
  const ON_Font::Weight font_weight = ON_Font::WeightFromAppleWeightOfFont(apple_weight_of_font);
  double apple_font_weight_trait = ON_Font::AppleFontWeightTraitFromWeight(font_weight);
  if (0 <= apple_weight_of_font && apple_weight_of_font <= 9)
  {
    apple_font_weight_trait = (1.0 - apple_weight_of_font) / 7.5;
    if (apple_font_weight_trait < -1.0)
      apple_font_weight_trait = -1.0;
    else if (apple_font_weight_trait > 1.0)
      apple_font_weight_trait = 1.0;    
  }
  return Internal_SetFontWeightTrio(
    font_weight, 
    -1, 
    apple_font_weight_trait,
    bUpdateFontDescription
  );
}

int ON_Font::AppleWeightOfFont() const
{
  return WindowsLogfontWeight() / 100;
}

bool ON_Font::SetAppleFontWeightTrait(
  double apple_font_weight_trait
)
{
  if (false == ON_IsValid(apple_font_weight_trait))
    return false; // nan or unset value

  const ON_Font::Weight font_weight = ON_Font::WeightFromAppleFontWeightTrait(apple_font_weight_trait);
  const bool bUpdateFontDescription = m_apple_font_name.IsEmpty() || m_apple_font_name != m_font_description;

  return Internal_SetFontWeightTrio(
    font_weight, 
    -1, 
    apple_font_weight_trait,
    bUpdateFontDescription
  );
}

double ON_Font::AppleFontWeightTrait() const
{
  return 
    (m_apple_font_weight_trait >= -1.0 && m_apple_font_weight_trait <= 1.0)
    ? m_apple_font_weight_trait
    : ON_Font::AppleFontWeightTraitFromWeight(this->FontWeight());
}

double ON_Font::PointSize() const
{
  return m_point_size;
}

bool ON_Font::SetPointSize(
  double point_size
)
{
  double x = (point_size > 0.0) ? point_size : 0.0;
  if (!(m_point_size == x))
  {
    if ( false == ON_FONT_MODIFICATION_PERMITTED )
      return false;
    m_point_size = x;
    Internal_SetFontDescription();
  }
  return true;
}

ON_Font::Style ON_Font::FontStyle() const
{
  return m_font_style;
}

bool ON_Font::SetFontStyle(
  ON_Font::Style font_style
  )
{
  if (m_font_style != font_style)
  {
    if ( false == ON_FONT_MODIFICATION_PERMITTED )
      return false;
    m_font_style = font_style;
    Internal_SetFontDescription();
  }
  return true;
}

ON_Font::Stretch ON_Font::FontStretch() const
{
  return m_font_stretch;
}

bool ON_Font::SetFontStretch(
  ON_Font::Stretch font_stretch
  )
{
  if (m_font_stretch != font_stretch)
  {
    if ( false == ON_FONT_MODIFICATION_PERMITTED )
      return false;
    m_font_stretch = font_stretch;
    Internal_SetFontDescription();
  }
  return true;
}
  
bool ON_Font::IsUnderlined() const
{
  return m_font_bUnderlined;
}

bool ON_Font::SetUnderlined(bool bUnderlined)
{
  if( m_font_bUnderlined != (bUnderlined?true:false))
  {
    if ( false == ON_FONT_MODIFICATION_PERMITTED )
      return false;
    m_font_bUnderlined = bUnderlined;
  }
  return true;
}

bool ON_Font::IsStrikethrough() const
{
  return m_font_bStrikethrough;
}

bool ON_Font::SetStrikethrough(bool bStrikethrough)
{
  if(m_font_bStrikethrough != (bStrikethrough?true:false))
  {
    if ( false == ON_FONT_MODIFICATION_PERMITTED )
      return false;
    m_font_bStrikethrough = bStrikethrough;
  }
  return true;
}

unsigned char ON_Font::LogfontCharSet() const
{
  return m_logfont_charset;
}

bool ON_Font::SetLogfontCharSet(unsigned char logfont_charset)
{
  if(logfont_charset != m_logfont_charset)
  {
    if ( false == ON_FONT_MODIFICATION_PERMITTED )
      return false;
    m_logfont_charset = logfont_charset;
    Internal_SetFontDescription();
  }
  return true;
}

const ON_FontMetrics& ON_Font::FontMetrics() const
{
  class ON_FontGlyphCache* font_cache = FontGlyphCache(true);
  if (nullptr != font_cache)
    return font_cache->m_normalized_metrics;
  return ON_FontMetrics::Unset;
}

const ON_FontMetrics& ON_Font::FontUnitFontMetrics() const
{
  class ON_FontGlyphCache* font_cache = FontGlyphCache(true);
  if (nullptr != font_cache)
    return font_cache->m_font_unit_metrics;
  return ON_FontMetrics::Unset;
}

double ON_Font::FontUnitToNormalizedScale() const
{
  class ON_FontGlyphCache* font_cache = FontGlyphCache(true);
  if (nullptr != font_cache)
    return font_cache->m_font_unit_to_normalized_scale;
  return 1.0;
}
  
double ON_Font::NormalizedToFontUnitScale() const
{
  class ON_FontGlyphCache* font_cache = FontGlyphCache(true);
  if (nullptr != font_cache)
    return font_cache->m_normalized_to_font_unit_scale;
  return 1.0;
}


int ON_Font::HeightOfI() const
{
  return FontMetrics().AscentOfI();
}

int ON_Font::HeightOfLinefeed() const
{
  return FontMetrics().LineSpace();
}

double ON_Font::HeightScale(double text_height) const
{
  return FontMetrics().GlyphScale(text_height);
}

int ON_Font::GetStrikeoutSize() const
{
  return FontMetrics().StrikeoutThickness();
}

int ON_Font::GetStrikeoutPosition() const
{
  return FontMetrics().StrikeoutPosition();
}

int ON_Font::GetUnderscoreSize() const
{
  return FontMetrics().UnderscoreThickness();
}

int ON_Font::GetUnderscorePosition() const
{
  return FontMetrics().UnderscorePosition();
}

const class ON_SHA1_Hash& ON_Font::FontCharacteristicsHash() const
{
  if (m_font_characteristics_hash.IsZeroDigest())
  {
    ON_SHA1 sha1;

    sha1.AccumulateUnsigned64(sizeof(*this));

    sha1.AccumulateString(m_face_name, sizeof(m_face_name)/sizeof(m_face_name[0]), ON_StringMapOrdinalType::Identity);
    sha1.AccumulateUnsigned8(static_cast<ON__UINT8>(m_font_weight));
    sha1.AccumulateUnsigned8(static_cast<ON__UINT8>(m_font_stretch));
    sha1.AccumulateBool(m_font_bUnderlined);
    sha1.AccumulateBool(m_font_bStrikethrough);
    sha1.AccumulateUnsigned8(static_cast<ON__UINT8>(m_logfont_charset));
    ON_SHA1_Hash string_hash;

    if (m_font_description.IsNotEmpty())
    {
      string_hash = ON_SHA1_Hash::StringHash(this->m_font_description);
      sha1.AccumulateSubHash(string_hash);
    }

    if (m_point_size > 0)
    {
      sha1.AccumulateDouble(m_point_size);
    }

#if defined(ON_RUNTIME_WIN)  
    sha1.AccumulateInteger32(m_windows_logfont_weight);
#endif

#if defined(ON_RUNTIME_APPLE)     
    sha1.AccumulateDouble(m_apple_font_weight_trait);
    if (m_apple_font_name.IsNotEmpty())
    {
      string_hash = ON_SHA1_Hash::StringHash(m_apple_font_name);
      sha1.AccumulateSubHash(string_hash);
    }
#endif

    m_font_characteristics_hash = sha1.Hash();
  }

  return m_font_characteristics_hash;
}


int ON_Font::CompareFontCharacteristics(
  const ON_Font& a,
  const ON_Font& b
  ) 
{
  int i = ON_wString::CompareOrdinal(a.m_face_name, ON_Font::face_name_capacity, b.m_face_name, ON_Font::face_name_capacity, true);
  int j = 0;

  for (;;)
  {
    if ( i != j )
      break;

    i = ON_Font::WindowsLogfontWeightFromWeight(a.m_font_weight);
    j = ON_Font::WindowsLogfontWeightFromWeight(b.m_font_weight);
    if ( i != j )
      break;

    i = static_cast<int>(a.m_font_style);
    j = static_cast<int>(b.m_font_style);
    if ( i != j )
      break;

    i = static_cast<int>(a.m_font_stretch);
    j = static_cast<int>(b.m_font_stretch);
    if ( i != j )
      break;

    i = a.m_font_bUnderlined ? 1 : 0;
    j = b.m_font_bUnderlined ? 1 : 0;
    if ( i != j )
      break;

    i = a.m_font_bStrikethrough ? 1 : 0;
    j = b.m_font_bStrikethrough ? 1 : 0;
    if ( i != j )
      break;

    j = 0;
    if (a.m_logfont_charset < b.m_logfont_charset)
      i = -1;
    else if (a.m_logfont_charset > b.m_logfont_charset)
      i = 1;
    else
    {
#if defined(ON_RUNTIME_APPLE)     
      i = ON_wString::CompareOrdinal(a.m_apple_font_name, b.m_apple_font_name, false);
      if (0 == i)
      {
        if (a.m_apple_font_weight_trait < b.m_apple_font_weight_trait)
          i = -1;
        else if (a.m_apple_font_weight_trait > b.m_apple_font_weight_trait)
          i = 1;
      }
#else
      i = 0;
#endif
    }

    if (i == j)
    {
      j = 0;
      if (a.m_point_size < b.m_point_size)
        i = -1;
      else if (a.m_point_size > b.m_point_size)
        i = 1;
      else
        i = 0;
    }
 
    break;
  }

  if (i < j)
    return -1;
  if (i > j)
    return 1;

  return 0;
}

bool ON_Font::EqualFontCharacteristics(
  const ON_Font& a,
  const ON_Font& b
  )
{
  return (0 == ON_Font::CompareFontCharacteristics(a,b));
}

#if defined(ON_RUNTIME_WIN)

static int ON_InternalVerticalPixelScale(
  HDC hdc,
  bool bDCtoLP,
  int y0
)
{
  int y1 = -1;

  for (;;)
  {
    POINT origin{ 0,0 };
    POINT pt{ 0, y0 };
    if (bDCtoLP)
    {
      if (0 == ::DPtoLP(hdc, &pt, 1))
        break;
      if (0 == ::DPtoLP(hdc, &origin, 1))
        break;
    }
    else
    {
      if (0 == ::LPtoDP(hdc, &pt, 1))
        break;
      if (0 == ::LPtoDP(hdc, &origin, 1))
        break;
    }
    y1 = std::abs(pt.y - origin.y);
    break;
  }

  return y1;
}


bool ON_Font::GetWindowsDeviceToLogicalHeightScales(
  HDC hdc,
  double* device_to_logical_scale,
  double* logical_to_device_scale
)
{
  if (nullptr != device_to_logical_scale)
    *device_to_logical_scale = 1.0;
  if (nullptr != logical_to_device_scale)
    *logical_to_device_scale = 1.0;

  int device_coordinate = 0;
  int logical_coordinate = 0;

  // see which is larger, device or logical
  const int thousand = 1000;
  const int device_to_logical_1000x = ON_InternalVerticalPixelScale(hdc, true, thousand);
  const int logical_to_device_1000x = ON_InternalVerticalPixelScale(hdc, false, thousand);
  if (device_to_logical_1000x <= 0 && logical_to_device_1000x <= 0)
  {
    ON_ERROR("DPtoLP(hdc, ...) and LPtoDP(hdc, ...) failed.");
    return false;
  }

  if (thousand == device_to_logical_1000x && thousand == logical_to_device_1000x)
  {
    // vertical logical pixels are the same size as device pixels
    device_coordinate = 1;
    logical_coordinate = 1;
  }
  else
  {
    // bDCtoLP = true if logical pixels are smaller than device pixels
    const bool bDCtoLP = (device_to_logical_1000x >= logical_to_device_1000x);
    const int s1000 = bDCtoLP ? device_to_logical_1000x : logical_to_device_1000x;
    int y0 = 0;
    int y1 = 0;
    const int s1024 = ON_InternalVerticalPixelScale(hdc, bDCtoLP, 1024);
    for (y0 = 1; y0 <= 100; y0++)
    {
      y1 = ON_InternalVerticalPixelScale(hdc, bDCtoLP, y0);
      if (y0*s1000 == y1*thousand || y0*s1024 == y1*1024)
      {
        break;
      }
      y1 = 0;
    }
    if (0 == y1)
    {
      y0 = thousand;
      y1 = s1000;
    }
    if (bDCtoLP)
    {
      device_coordinate = y0;
      logical_coordinate = y1;
    }
    else
    {
      device_coordinate = y1;
      logical_coordinate = y0;
    }
  }

  if (nullptr != device_to_logical_scale)
  {
    *device_to_logical_scale = ((double)logical_coordinate) / ((double)device_coordinate);
  }

  if (nullptr != logical_to_device_scale)
  {
    *logical_to_device_scale = ((double)device_coordinate) / ((double)logical_coordinate);
  }

  return true;
}

HDC ON_Font::Internal_CreateWindowsLogfontDeviceContext()
{
  return ::CreateCompatibleDC(nullptr);
}

void ON_Font::Internal_DeleteWindowsLogfontDeviceContext(
  HDC hdc
)
{
  if ( nullptr != hdc )
    ::DeleteDC(hdc);
}

bool ON_Font::GetWindowsTextMetrics(
  int map_mode,
  HDC hdc,
  const LOGFONT& logfont,
  TEXTMETRIC& textmetric
)
{
  bool rc = false;

  TEXTMETRIC tm;
  memset(&tm, 0, sizeof(tm));
  textmetric = tm;

  // need to get TEXTMETRICS tmInternalLeading
  const bool bValidMapModeParameter = (map_mode >= MM_MIN && map_mode <= MM_MAX);
  const bool bNeedDeviceContext = (false == bValidMapModeParameter || MM_TEXT == map_mode);
  HDC screen_dc = nullptr;

  const int savedDC
    = (nullptr != hdc)
    ? ::SaveDC(hdc)
    : 0;

  HFONT hfont = 0;

  for (;;)
  {
    if (nullptr == hdc)
    {
      screen_dc = ON_Font::Internal_CreateWindowsLogfontDeviceContext();
      hdc = screen_dc;
      if (nullptr == hdc)
        break;
    }

    const int hdc_mm = ::GetMapMode(hdc);

    const int mm
      = bValidMapModeParameter
      ? map_mode
      : ((nullptr != hdc) ? hdc_mm : 0);

    if (mm < MM_MIN || mm > MM_MAX)
    {
      ON_ERROR("GetMapMode(hdc) failed.");
      break;
    }

    if (hdc_mm != mm)
    {
      ::SetMapMode(hdc, mm);
    }

    hfont = ::CreateFontIndirect(&logfont);
    if (0 == hfont)
      break;

    ::SelectObject(hdc, hfont);

    if (0 == ::GetTextMetrics(hdc, &tm))
    {
      ON_ERROR("GetTextMetrics(hdc,...) failed.");
      break;
    }

    textmetric = tm;

    rc = true;
    break;
  }  

  if (nullptr != screen_dc)
    ON_Font::Internal_DeleteWindowsLogfontDeviceContext(screen_dc);
  else if ( nullptr != hdc)
    ::RestoreDC(hdc, savedDC);

  if (0 != hfont)
    ::DeleteObject(hfont);

  return rc;
}

int ON_Font::WindowsLogfontCharacterHeight(
  int map_mode,
  HDC hdc,
  const LOGFONT& logfont
)
{
  if (logfont.lfHeight <= 0)
    return logfont.lfHeight;

  int logfont_lfHeight = 0;

  for (;;)
  {
    TEXTMETRIC tm;
    memset(&tm, 0, sizeof(tm));
    bool bHaveTextMetric = ON_Font::GetWindowsTextMetrics(
      map_mode,
      hdc,
      logfont,
      tm
    );

    if (false == bHaveTextMetric)
    {
      ON_ERROR("ON_Font::GetWindowsTextMetrics...) failed.");
      break;
    }

    const int cellHeight = tm.tmAscent + tm.tmDescent;
    const int characterHeight = cellHeight - tm.tmInternalLeading;
    if (cellHeight <= 0 || characterHeight <= 0)
    {
      ON_ERROR("tm.tmInternalLeading >= cellHeight.");
      break;
    }



    // logfont_lfHeight is negative to indicate it is character height.
    // (positive values are cell height)
    logfont_lfHeight = -characterHeight;
    break;
  }

  return logfont_lfHeight;
}

int ON_Font::WindowsLogfontCellHeight(
  int map_mode,
  HDC hdc,
  const LOGFONT& logfont
)
{
  if (logfont.lfHeight >= 0)
    return logfont.lfHeight;

  int logfont_lfHeight = 0;

  for (;;)
  {
    TEXTMETRIC tm;
    memset(&tm, 0, sizeof(tm));
    bool bHaveTextMetric = ON_Font::GetWindowsTextMetrics(
      map_mode,
      hdc,
      logfont,
      tm
    );

    if (false == bHaveTextMetric)
    {
      ON_ERROR("ON_Font::GetWindowsTextMetrics...) failed.");
      break;
    }

    const int cellHeight = tm.tmAscent + tm.tmDescent;
    if (cellHeight <= 0)
    {
      ON_ERROR("tm.tmAscent + tm.tmDescent < 0.");
      break;
    }

    // logfont_lfHeight is positive to indicate it is cell height.
    // (negative values are character height)
    logfont_lfHeight = cellHeight;
    break;
  }

  return logfont_lfHeight;
}


int ON_Font::WindowsLogfontCharacterHeightFromPointSize(
  int map_mode,
  HDC hdc,
  double point_size
)
{
  int logfont_lfHeight = 0;

  const bool bValidMapModeParameter = (map_mode >= MM_MIN && map_mode <= MM_MAX);
  const bool bNeedDeviceContext = (false == bValidMapModeParameter || MM_TEXT == map_mode);
  HDC screen_dc = nullptr;

  for (;;)
  {
    if (0 == point_size)
      break;

    if (!(point_size > 0.0 && point_size < 21474836.47))
    {
      ON_ERROR("Invalid point_size parameter.");
      break;
    }

    if (bNeedDeviceContext && nullptr == hdc)
    {
      screen_dc = ON_Font::Internal_CreateWindowsLogfontDeviceContext();
      hdc = screen_dc;
      if (nullptr == hdc)
        break;
    }

    const int mm
      = bValidMapModeParameter
      ? map_mode
      : ((nullptr != hdc) ? ::GetMapMode(hdc) : 0);

    if (mm < MM_MIN || mm > MM_MAX)
    {
      ON_ERROR("GetMapMode(hdc) failed.");
      break;
    }

    double points_to_lfHeight = 0.0;
    switch (mm)
    {
    case MM_TEXT:
      // Each logical unit is mapped to one device pixel. Positive x is to the right; positive y is down.
      if(nullptr != hdc)
      {
        const int device_pixels_per_logical_inch = ::GetDeviceCaps(hdc, LOGPIXELSY);
        if (device_pixels_per_logical_inch <= 0)
        {
          ON_ERROR("GetDeviceCaps(hdc, LOGPIXELSY) failed.");
          break;
        }

        double device_to_logical_scale = 1.0;
        if (false == ON_Font::GetWindowsDeviceToLogicalHeightScales(hdc, &device_to_logical_scale, nullptr) )
        {
          ON_ERROR("ON_Font::GetWindowsDeviceToLogicalHeightScales(hdc, ...) failed.");
          break;
        }

        // MM_TEXT points_to_lfHeight units 
        // = ((device pixel height)/(logical inch)) * (logical pixel height) / ( (points/inch)*(device pixel height) )
        // = ( (logical pixel height) / points ) * (inch / (logical inch))
        points_to_lfHeight = ((static_cast<double>(device_pixels_per_logical_inch)*device_to_logical_scale)) / 72.0;
      }
      break;
    case MM_LOMETRIC:
      // Each logical unit is mapped to 0.1 millimeter. Positive x is to the right; positive y is up.
      // (254 tenths of a mm/inch)/(72 points/inch)
      points_to_lfHeight = 254.0/72.0;
      break;
    case MM_HIMETRIC:
      // Each logical unit is mapped to 0.01 millimeter. Positive x is to the right; positive y is up.
      // (2540 hundredths of a mm/inch)/(72 points/inch)
      points_to_lfHeight = 2540.0/72.0;
      break;
    case MM_LOENGLISH:
      // Each logical unit is mapped to 0.01 inch. Positive x is to the right; positive y is up.
      // (100 hundredths of a inch/inch)/(72 points/inch)
      points_to_lfHeight = 1.0/0.72;
      break;
    case MM_HIENGLISH:
      // Each logical unit is mapped to 0.001 inch. Positive x is to the right; positive y is up.
      // (1000 thousandths of a inch/inch)/(72 points/inch)
      points_to_lfHeight = 1.0/0.072;
      break;
    case MM_TWIPS:
      // Each logical unit is mapped to one twentieth of a printer's point (1/1440 inch, also called a "twip"). Positive x is to the right; positive y is up.
      // 20 twips / point
      points_to_lfHeight = 20.0;
      break;
    case MM_ISOTROPIC:
      // Logical units are mapped to arbitrary units with equally scaled axes; 
      // that is, one unit along the x-axis is equal to one unit along the y-axis. 
      // Use the SetWindowExtEx and SetViewportExtEx functions to specify the units
      // and the orientation of the axes. Graphics device interface makes adjustments
      // as necessary to ensure the x and y units remain the same size. 
      // (When the windows extent is set, the viewport will be adjusted to keep
      // the units isotropic).
      ON_ERROR("GetMapMode(hdc) returned MM_ISOTROPIC mapping mode.");
      break;
    case MM_ANISOTROPIC:
      // Logical units are mapped to arbitrary units with arbitrarily scaled axes. 
      // Use the SetWindowExtEx and SetViewportExtEx functions to specify the units, 
      // orientation, and scaling required.
      ON_ERROR("GetMapMode(hdc) returned MM_ANISOTROPIC mapping mode.");
      break;
    default:
      ON_ERROR("GetMapMode(hdc) returned undocumented mapping mode.");
    }

    if (0 != points_to_lfHeight)
    {
      logfont_lfHeight = -static_cast<int>(fabs(points_to_lfHeight*point_size + 0.5));
      if (0 == logfont_lfHeight)
        logfont_lfHeight = -1;
    }
    break;
  }

  if (0 != screen_dc)
  {
    ON_Font::Internal_DeleteWindowsLogfontDeviceContext(screen_dc);
  }

  return logfont_lfHeight;
}

double ON_Font::PointSizeFromWindowsLogfontCharacterHeight(
  int map_mode,
  HDC hdc,
  int logfont_character_height
)
{
  double point_size = 0.0;

  const bool bValidMapModeParameter = (map_mode >= MM_MIN && map_mode <= MM_MAX);

  const bool bNeedDeviceContext = (false == bValidMapModeParameter || MM_TEXT == map_mode);

  HDC screen_dc = 0;

  // Windows LOGFONT.lfHeight is negative when
  // the value is a character height. This
  // function allow the user to pass in a positive value.
  const int logfont_lfHeight
    = (logfont_character_height <= 0)
    ? logfont_character_height
    : -logfont_character_height;

  for (;;)
  {
    if (logfont_lfHeight >= 0)
      break;

    if (bNeedDeviceContext && nullptr == hdc )
    {
      screen_dc = ON_Font::Internal_CreateWindowsLogfontDeviceContext();
      hdc = screen_dc;
      if (nullptr == hdc)
        break;
    }

    const int mm
      = bValidMapModeParameter
      ? map_mode
      : ((nullptr != hdc) ? ::GetMapMode(hdc) : 0);

    if (mm < MM_MIN || mm > MM_MAX)
    {
      ON_ERROR("GetMapMode(hdc) failed.");
      break;
    }

    double lfHeight_to_points = 0.0;
    switch (mm)
    {
    case MM_TEXT:
      // Each logical unit is mapped to one device pixel. Positive x is to the right; positive y is down.
      if(nullptr != hdc)
      {
        const int device_pixels_per_logical_inch = ::GetDeviceCaps(hdc, LOGPIXELSY);
        if (device_pixels_per_logical_inch <= 0)
        {
          ON_ERROR("GetDeviceCaps(hdc, LOGPIXELSY) failed.");
          break;
        }

        double logical_to_device_scale = 1.0;
        if (false == ON_Font::GetWindowsDeviceToLogicalHeightScales(hdc, nullptr, &logical_to_device_scale))
        {
          ON_ERROR("ON_Font::GetWindowsDeviceToLogicalHeightScales(hdc, ...) failed.");
          break;
        }

        // MM_TEXT lfHeight_to_points units 
        // = ( (points/inch) * (device pixel height) ) / ( ((device pixel height)/(logical inch)) * (logical pixel height)   )
        // = ( points / (logical pixel height) ) * ( (logical inch)/inch )
        lfHeight_to_points = (72.0*logical_to_device_scale) / static_cast<double>(device_pixels_per_logical_inch);
      }
      break;
    case MM_LOMETRIC:
      // Each logical unit is mapped to 0.1 millimeter. Positive x is to the right; positive y is up.
      // 72 points/inch / (254 tenths of a mm/inch)
      lfHeight_to_points = 72.0 / 254.0;
      break;
    case MM_HIMETRIC:
      // Each logical unit is mapped to 0.01 millimeter. Positive x is to the right; positive y is up.
      // 72 points/inch / (2540 hundredths of a mm/inch)
      lfHeight_to_points = 72.0 / 2540.0;
      break;
    case MM_LOENGLISH:
      // Each logical unit is mapped to 0.01 inch. Positive x is to the right; positive y is up.
      // 72 points/inch / (100 hundredths of a inch/inch)
      lfHeight_to_points = 0.72;
      break;
    case MM_HIENGLISH:
      // Each logical unit is mapped to 0.001 inch. Positive x is to the right; positive y is up.
      // 72 points/inch / (1000 thousandths of a inch/inch)
      lfHeight_to_points = 0.072;
      break;
    case MM_TWIPS:
      // Each logical unit is mapped to one twentieth of a printer's point (1/1440 inch, also called a "twip"). Positive x is to the right; positive y is up.
      // 1 point / 20 twips
      lfHeight_to_points = 1.0/20.0;
      break;
    case MM_ISOTROPIC:
      // Logical units are mapped to arbitrary units with equally scaled axes; 
      // that is, one unit along the x-axis is equal to one unit along the y-axis. 
      // Use the SetWindowExtEx and SetViewportExtEx functions to specify the units
      // and the orientation of the axes. Graphics device interface makes adjustments
      // as necessary to ensure the x and y units remain the same size. 
      // (When the windows extent is set, the viewport will be adjusted to keep
      // the units isotropic).
      ON_ERROR("GetMapMode(hdc) returned MM_ISOTROPIC mapping mode.");
      break;
    case MM_ANISOTROPIC:
      // Logical units are mapped to arbitrary units with arbitrarily scaled axes. 
      // Use the SetWindowExtEx and SetViewportExtEx functions to specify the units, 
      // orientation, and scaling required.
      ON_ERROR("GetMapMode(hdc) returned MM_ANISOTROPIC mapping mode.");
      break;
    default:
      ON_ERROR("GetMapMode(hdc) returned undocumented mapping mode.");
    }

    point_size = fabs(lfHeight_to_points*static_cast<double>(logfont_lfHeight));

    for (;;)
    {
      // Because LOGFONT lfHeight is an integer and point_size is typically
      // an integer and sometimes a multiple of 0.5, the following rouding
      // is performed so point_size will round trip.
      double i0 = floor(point_size);
      if (i0 == point_size)
        break;

      if (lfHeight_to_points < 0.5 && 2.0*point_size == floor(2.0*point_size))
        break;

      double i1 = ceil(point_size);
      if (i1 >= 1.0)
      {
        if (i0 >= 1.0 && fabs(i0 - point_size) < fabs(i1 - point_size) && fabs(i0 - point_size) < lfHeight_to_points)
        {
          if (logfont_lfHeight == ON_Font::WindowsLogfontCharacterHeightFromPointSize(map_mode, hdc, i0))
          {
            point_size = i0;
            break;
          }
        }
        else if (fabs(i1 - point_size) < fabs(i0 - point_size) && fabs(i1 - point_size) < lfHeight_to_points)
        {
          if (logfont_lfHeight == ON_Font::WindowsLogfontCharacterHeightFromPointSize(map_mode, hdc, i1))
          {
            point_size = i1;
            break;
          }
        }

        if (lfHeight_to_points < 0.25 && fabs(point_size - 0.5*(i0 + i1)) < lfHeight_to_points)
        {
          if (logfont_lfHeight == ON_Font::WindowsLogfontCharacterHeightFromPointSize(map_mode, hdc, 0.5*(i0 + i1)))
          {
            point_size = 0.5*(i0 + i1);
            break;
          }
        }
      }
      
      break;
    }

    break;
  }

  if (0 != screen_dc)
  {
    ON_Font::Internal_DeleteWindowsLogfontDeviceContext(screen_dc);
  }

  return point_size;
}

bool ON_Font::SetFromWindowsLogFont(
  int map_mode,
  HDC hdc,
  const LOGFONT& logfont
)
{
  const ON_Font::Weight font_weight = ON_Font::WeightFromWindowsLogfontWeight(logfont.lfWeight);
  const ON_Font::Style font_style = (0 != logfont.lfItalic) ? ON_Font::Style::Italic : ON_Font::Default.m_font_style;
  const ON_Font::Stretch font_stretch = ON_Font::Default.m_font_stretch;
  const bool bUnderlined = (0 != logfont.lfUnderline) ? true : false;
  const bool bStrikethrough = (0 != logfont.lfStrikeOut) ? true : false;
  const double linefeed_ratio = ON_FontMetrics::DefaultLineFeedRatio;

  // If logfont.lfHeight > 0, +lfHeight = cell height.
  // If logfont.lfHeight < 0, -lfHeight = character height = cell height - TEXTMETRIC.tmInternalLeading.
  // PointSize() calculations require a character height.
  const int logfont_lfHeight
    = (0 != logfont.lfHeight)
    ? ON_Font::WindowsLogfontCharacterHeight(map_mode, hdc, logfont)
    : 0;

  const double point_size
    = (logfont_lfHeight < 0)
    ? ON_Font::PointSizeFromWindowsLogfontCharacterHeight(map_mode, hdc, logfont_lfHeight)
    : 0.0;

  unsigned char logfont_charset = ON_Font::WindowsLogfontCharSetFromFaceName(logfont.lfFaceName);
  if (    ON_Font::WindowsConstants::logfont_symbol_charset != logfont_charset
       && ON_Font::WindowsConstants::logfont_symbol_charset != logfont.lfCharSet
     )
  {
    // Use the input value of logfont.lfCharSet.
    //
    // The problem:
    //
    // Many callers memset logfont to zero and don't correctly and consciously
    // set logfont.lfCharSet. The value 0 = ANSI_CHARSET is a valid value for
    // logfont.lfCharSet. 
    //
    // At this point, we cannot tell if 0 == logfont.lfCharSet occurs because
    // the caller didn't set it or because the caller really wants ANSI_CHARSET
    // for this particular font. 
    //
    // If the caller failed to correctly and consciously set logfont.lfCharSet
    // and the font is being used to render text for a locale other than
    // US English, DEFAULT_CHARSET (=1) would be a much better choice.
    //
    logfont_charset = logfont.lfCharSet;
  }

  const bool rc = SetFontCharacteristics(
    point_size,
    logfont.lfFaceName,
    font_weight,
    font_style,
    font_stretch,
    bUnderlined,
    bStrikethrough,
    linefeed_ratio,
    logfont_charset
    );

  if (rc)
  {
    return Internal_SetFontWeightTrio(
      font_weight, 
      logfont.lfWeight, 
      ON_UNSET_VALUE,
      false
    );
  }

  return rc;
}

const LOGFONT ON_Font::WindowsLogFont(
  int map_mode,
  HDC hdc
  ) const
{
  LOGFONT logfont;
  memset(&logfont, 0, sizeof(LOGFONT));

  logfont.lfHeight 
    = (m_point_size > 0.0)
    ? ON_Font::WindowsLogfontCharacterHeightFromPointSize(map_mode,hdc,m_point_size)
    : ON_Font::Constants::AnnotationFontCellHeight;

  logfont.lfWidth = 0;
  logfont.lfEscapement = 0;
  logfont.lfOrientation = 0;
  logfont.lfWeight = ON_Font::WindowsLogfontWeightFromWeight(m_font_weight);
  logfont.lfItalic = (ON_Font::Style::Italic == m_font_style) ? 1 : 0;
  logfont.lfUnderline = m_font_bUnderlined ? 1 : 0;
  logfont.lfStrikeOut = m_font_bStrikethrough ? 1 : 0;
  logfont.lfCharSet = m_logfont_charset;
  logfont.lfOutPrecision = ON_Font::WindowsConstants::logfont_out_precis;
  logfont.lfClipPrecision = 0;
  logfont.lfQuality = ON_Font::WindowsConstants::logfont_quality;
  logfont.lfPitchAndFamily = ON_Font::WindowsConstants::logfont_pitch_and_family;

  for(int i = 0; i <= ON_Font::face_name_capacity && i < LF_FACESIZE; i++)
    logfont.lfFaceName[i] = m_face_name[i];

  return logfont;
}

const MAT2 ON_Font::WindowsFontMat2() const
{
  MAT2 mat2;
  memset(&mat2, 0, sizeof(mat2));

  mat2.eM11.fract = 0;
  mat2.eM11.value = 1;

  mat2.eM12.fract = 0;
  mat2.eM12.value = 0;

  mat2.eM21.fract = 0;
  mat2.eM21.value = 0;

  mat2.eM22.fract = 0;
  mat2.eM22.value = 1;

  double eM11 = 1.0;

  switch (FontStretch())
  {
  case ON_Font::Stretch::Ultracondensed:
    eM11 = 0.2;
    break;
  case ON_Font::Stretch::Extracondensed:
    eM11 = 0.3;
    break;
  case ON_Font::Stretch::Condensed:
    eM11 = 0.5;
    break;
  case ON_Font::Stretch::Semicondensed:
    eM11 = 0.75;
    break;
  case ON_Font::Stretch::Medium:
    eM11 = 1.0;
    break;
  case ON_Font::Stretch::Semiexpanded:
    eM11 = 1.25;
    break;
  case ON_Font::Stretch::Expanded:
    eM11 = 1.5;
    break;
  case ON_Font::Stretch::Extraexpanded:
    eM11 = 2.0;
    break;
  case ON_Font::Stretch::Ultraexpanded:
    eM11 = 3.0;
    break;
  }

  if (eM11 > 0.0 && 1.0 != eM11)
  {
    FIXED fx = { 1, 0 };
    double ffrac = fmod(eM11, 1.0);
    if (ffrac != ffrac)
      ffrac = 0.0;
    fx.fract = static_cast<unsigned short>(floor(65535.0 * ffrac));
    fx.value = static_cast<short>(eM11 - ffrac);
    mat2.eM11 = fx;
  }

  return mat2;
}


#endif

void ON_Font::DestroyFontGlyphCache()
{
  m_font_glyph_cache.reset();
}

class ON_FontGlyphCache* ON_Font::FontGlyphCache(
  bool bCreateIfMissing
  ) const
{  
  ON_FontGlyphCache* font_cache = m_font_glyph_cache.get();
  if(nullptr == font_cache)
  {
    // get the static ON_Font that matches this one
    const ON_Font* managed_font = this->ManagedFont();
    if(nullptr != managed_font)
    {
      // The first call to ON_Font::GetManagedFont() adds
      // &ON_Font::Default and calculates its font metric
      // and glyph information. The this != managed_font
      // text below prevents a silly copy in this case
      // and in other unanticipated cases.
      if ( this != managed_font)
        m_font_glyph_cache = managed_font->m_font_glyph_cache;
      font_cache = m_font_glyph_cache.get();
    }
  }
  return font_cache;
}

// static
void ON_Font::GetRunBounds(
  const ON_Font& font,
  const wchar_t* text,
  double font_height_pixels,
  ON::TextHorizontalAlignment horizontal_alignment,
  ON::TextVerticalAlignment vertical_alignment,
  ON_2dPoint& bounds_min,
  ON_2dPoint& bounds_max, 
  int& line_count
)
{
  line_count = 0;
  bounds_min = ON_2dPoint::Origin;
  bounds_max = ON_2dPoint::Origin;

  ON_wString run = text;
  int textlength = run.Length();
  if(textlength < 1)
    return;

  const ON_Font* managed_font = font.ManagedFont();
  if (nullptr == managed_font)
    managed_font = &ON_Font::Default;

  const ON_FontMetrics& fm = managed_font->FontMetrics();

  const int height_of_I = fm.AscentOfI();
  const int height_of_LF = fm.LineSpace();

  ON_TextBox text_box;
  line_count = ON_FontGlyph::GetGlyphListBoundingBox(text, managed_font, text_box);
  if ( line_count < 0 || false == text_box.IsSet())
    return;

  bounds_min.x = text_box.m_bbmin.i;
  bounds_min.y = text_box.m_bbmin.j;

  bounds_max.x = text_box.m_bbmax.i;
  bounds_max.y = text_box.m_bbmax.j;

  // in text_box coordinate system (0.0) = left most baseline point of first line.
  const ON_2dPoint first_line_basepoint = ON_2dPoint::Origin;
  const ON_2dPoint last_line_basepoint(first_line_basepoint.x, -(line_count - 1)*height_of_LF);

  ON_2dVector offset = ON_2dVector::ZeroVector;

  switch (horizontal_alignment)
  {
  case ON::TextHorizontalAlignment::Left:
    offset.x = -first_line_basepoint.x;
    break;

  case ON::TextHorizontalAlignment::Center:
    // Text command uses m_max_basepoint (not bbmax)
    offset.x = -first_line_basepoint.x - 0.5*text_box.m_max_basepoint.i;
    break;

  case ON::TextHorizontalAlignment::Right:
    // Text command uses m_max_basepoint (not bbmax)
    offset.x = -first_line_basepoint.x - text_box.m_max_basepoint.i;
    break;
  default:
    break;
  }

  switch (vertical_alignment)
  {
  case ON::TextVerticalAlignment::Top:
    // (*,0) is at the top of an "I" on the first line.
    offset.y = -first_line_basepoint.y - height_of_I;
    break;
  case ON::TextVerticalAlignment::MiddleOfTop:
    // (*,0) is at the middle of an "I" on the first line.
    offset.y = -first_line_basepoint.y - 0.5*height_of_I;
    break;
  case ON::TextVerticalAlignment::BottomOfTop:
    // (*,0) is at the baseline of the first line.
    offset.y = -first_line_basepoint.y;
    break;

  case ON::TextVerticalAlignment::Middle:
    {
      // (*,0) is the vertical middle of lines (not vertical middle of glyph bounding box)
      double h = (line_count - 1)*height_of_LF + height_of_I;
      offset.y = -last_line_basepoint.y + 0.5*h;
    }
    break;

  case ON::TextVerticalAlignment::MiddleOfBottom:
    // (*,0) is at the middle of an "I" on the last line.
    offset.y = -last_line_basepoint.y  - 0.5*height_of_I;
    break;
  case ON::TextVerticalAlignment::Bottom:
    // (*,0) is at the basline of the last line.
    offset.y = -last_line_basepoint.y;
    break;
  case ON::TextVerticalAlignment::BottomOfBoundingBox:
    // (*,0) is on the bottom the the glyphs' bounding box
    offset.y = -bounds_min.y;
    break;
  default:
    break;
  }

  bounds_min += offset;
  bounds_max += offset;

  // scale is applied last.
  const double scale = font_height_pixels / (double)ON_Font::Constants::AnnotationFontCellHeight;
  if (scale > 0.0)
  {
    bounds_min.x = scale * text_box.m_bbmin.i;
    bounds_max.x = scale * text_box.m_bbmax.i;
    bounds_min.y = scale * text_box.m_bbmin.j;
    bounds_max.y = scale * text_box.m_bbmax.j;
  }
}

const ON_FontGlyph* ON_Font::Internal_ManagedCodePointGlyph(
  ON__UINT32 unicode_code_point,
  bool bCreateIfMissing,
  bool bFindSubstitutes
) const
{
  if ( false == ON_IsValidUnicodeCodePoint(unicode_code_point) )
  {
    // valid UNICODE codepoint values are <= 0x10FFFF
    ON_ERROR("invalid codepoint parameter.");
    return nullptr;
  }

  if (false == IsManagedFont())
  {
    ON_ERROR("this->IsManagedFont() must be true.");
    return nullptr; 
  }
  
  // First see if we already have the glyph in the cache
  ON_FontGlyphCache* font_cache = FontGlyphCache(true);
  ON_GlyphMap* glyph_map
    = (nullptr == font_cache)
    ? nullptr
    : font_cache->m_glyphmap.get();
  if (nullptr == glyph_map)
  {
    ON_ERROR("Unable to create ON_FontGlyphCache.");
    return nullptr;
  }

  const ON_FontGlyph* managed_glyph = glyph_map->FindGlyph(unicode_code_point);
  if (nullptr != managed_glyph)
    return managed_glyph;

  if (false == bCreateIfMissing)
    return nullptr;

  ON_FontGlyph glyph;
  glyph.SetCodePoint(this, unicode_code_point);

  // Call external function to get glyph details
  ON_TextBox font_unit_glyph_box = ON_TextBox::Unset;
  ON__UINT_PTR font_glyph_id = ON_ManagedFonts::GetGlyphMetrics(this, unicode_code_point, font_unit_glyph_box);
  if (0 != font_glyph_id)
  {
    if (font_unit_glyph_box.IsSet())
    {
      glyph.m_font_unit_glyph_bbox = font_unit_glyph_box;
      // Save a normalized box as well.
      const double normalize_scale = this->FontUnitToNormalizedScale();
      ON_TextBox normalized_glyph_box = font_unit_glyph_box;
      if (normalize_scale > 0.0)
        normalized_glyph_box = ON_TextBox::Scale(font_unit_glyph_box, normalize_scale);
      glyph.m_normalized_glyph_bbox = normalized_glyph_box;
    }
    glyph.Internal_SetFontGlyphId(font_glyph_id);
    bFindSubstitutes = false;
  }


  return glyph_map->InsertGlyph(glyph);
}

#if defined(ON_RUNTIME_WIN)


static bool ON_Internal_WindowsGetGlyphMetricsEx(
  const ON_Font* font,
  int logfont_height,
  ON__UINT32 unicode_code_point,
  class ON_TextBox& font_unit_glyph_box
)
{
  font_unit_glyph_box = ON_TextBox::Unset;

  if (logfont_height <= 0)
    logfont_height = ON_Font::Constants::AnnotationFontCellHeight;

  if (false == ON_IsValidUnicodeCodePoint(unicode_code_point) )
    return false;
  if (nullptr == font)
    return false;
  font = font->ManagedFont();
  if (nullptr == font)
    return false;

  wchar_t w[8] = { 0 };
  const size_t w_capacity = (sizeof(w) / sizeof(w[0])) - 1;
  const int w_count = ON_EncodeWideChar(unicode_code_point, w_capacity, w);
  if ( 0 == w[0] || w_count <= 0 || w_count > (int)w_capacity )
    return false;
  
  HDC hdc = nullptr;
  HFONT hfont = nullptr;
  HGDIOBJ hfont0 = nullptr;

  for (;;)
  {
    hdc = ::CreateCompatibleDC(0);
    if (nullptr == hdc)
      break;

    LOGFONT lf = font->WindowsLogFont(0,nullptr);
    lf.lfHeight = logfont_height;
    hfont = ::CreateFontIndirect(&lf);
    if (nullptr == hfont)
      break;
    hfont0 = ::SelectObject(hdc, hfont);

    ON__UINT16 glyphindex[sizeof(w)/sizeof(w[0])] = { 0 };
    const DWORD gicount = ::GetGlyphIndices(hdc, w, w_count, glyphindex, GGI_MARK_NONEXISTING_GLYPHS);
    if (GDI_ERROR == gicount)
      break;
    if (0xffff == glyphindex[0] || 0xffff == glyphindex[1])
    {
      // March 2017 Dale Lear
      // https://mcneel.myjetbrains.com/youtrack/issue/RH-38377
      //
      // The GGI_MARK_NONEXISTING_GLYPHS flag causes GetGlyphIndices()
      // to set glyphindex[] values to 0xFFFF for missing glyphs.
      //
      // GetGlyphIndices() is not capable of getting this glyph in the hdc font.
      // This often happes for surrogate pair encodings, even when the 
      // glyph does exist in the font.
      //
      // GetGlyphIndices cannot find glyph in the font
      // Often a surrogate pair and we probably need to be using 
      // Windows newer Uniscribe API instead of the tired old
      // GetGlyphIndices + GetGlyphOutlineW stuff.
      break; 
    }

    GLYPHMETRICS glm;
    memset(&glm, 0, sizeof(glm));
    MAT2 mat2 = font->WindowsFontMat2();
    const DWORD cb_size = ::GetGlyphOutlineW(hdc, glyphindex[0], GGO_NATIVE | GGO_GLYPH_INDEX, &glm, 0, nullptr, &mat2);
    if (GDI_ERROR == cb_size)
      break;

    ////bool ul = false, so = false;
    ////int ulsize = 0, ulpos = 0, sosize = 0, sopos = 0;
    ////HGDIOBJ hfont = GetCurrentObject(hdc, OBJ_FONT);
    ////LOGFONT lf = { 0 };
    ////if (sizeof(lf) == GetObject(hfont, sizeof(lf), &lf))
    ////{
    ////  ul = lf.lfUnderline ? true : false;
    ////  so = lf.lfStrikeOut ? true : false;
    ////}

    ////if (ul || so)
    ////{
    ////  OUTLINETEXTMETRIC otm = { 0 };
    ////  if (0 != ::GetOutlineTextMetrics(hdc, sizeof(OUTLINETEXTMETRIC), &otm))
    ////  {
    ////    sosize = otm.otmsStrikeoutSize;
    ////    sopos = otm.otmsStrikeoutPosition;
    ////    ulsize = otm.otmsUnderscoreSize;
    ////    ulpos = otm.otmsUnderscorePosition;
    ////  }
    ////}

    font_unit_glyph_box.m_advance.i = glm.gmCellIncX;
    font_unit_glyph_box.m_advance.j = glm.gmCellIncY;
    if (cb_size == 0)
    {
      // Non-printing char - nothing to draw
      font_unit_glyph_box.m_bbmin.i = 0;
      font_unit_glyph_box.m_bbmin.j = 0;
      font_unit_glyph_box.m_bbmax.i = font_unit_glyph_box.m_advance.i;
      font_unit_glyph_box.m_bbmax.j = font_unit_glyph_box.m_advance.j;
    }
    else
    {
      font_unit_glyph_box.m_bbmin.i = glm.gmptGlyphOrigin.x;
      font_unit_glyph_box.m_bbmin.j = glm.gmptGlyphOrigin.y - (int)glm.gmBlackBoxY;
      font_unit_glyph_box.m_bbmax.i = (int)glm.gmBlackBoxX + glm.gmptGlyphOrigin.x;
      font_unit_glyph_box.m_bbmax.j = glm.gmptGlyphOrigin.y;
    }

    break;
  }

  if (nullptr != hdc)
  {
    if (nullptr != hfont)
    {
      ::SelectObject(hdc, hfont0);
      ::DeleteObject(hfont);
    }
    ::DeleteDC(hdc);
  }

  return font_unit_glyph_box.IsSet();
}

bool ON_WindowsGetGlyphMetrics(
  const ON_Font* font,
  ON__UINT32 unicode_code_point,
  class ON_TextBox& font_unit_glyph_box
)
{
  return ON_Internal_WindowsGetGlyphMetricsEx(font, 0, unicode_code_point, font_unit_glyph_box);
}

#endif

ON_Font::ON_GetGlyphMetricsFuncType ON_Font::Internal_CustomGetGlyphMetricsFunc = nullptr;
ON_Font::ON_GetFontMetricsFuncType ON_Font::Internal_CustomGetFontMetricsFunc = nullptr;

void ON_Font::SetCustomMeasurementFunctions(
  ON_GetGlyphMetricsFuncType measureGlyphFunc,
  ON_GetFontMetricsFuncType metricsFunc
)
{
  ON_Font::Internal_CustomGetGlyphMetricsFunc = measureGlyphFunc;
  ON_Font::Internal_CustomGetFontMetricsFunc = metricsFunc;
}

ON__UINT_PTR ON_ManagedFonts::GetGlyphMetrics(
  const class ON_Font* font,
  ON__UINT32 unicode_code_point,
  class ON_TextBox& font_unit_glyph_box
)
{
  return
    (nullptr != ON_Font::Internal_CustomGetGlyphMetricsFunc)
    ? ON_Font::Internal_CustomGetGlyphMetricsFunc(font, unicode_code_point, font_unit_glyph_box)
    : ON_FreeTypeGetGlyphMetrics(font, unicode_code_point, font_unit_glyph_box);
}

void ON_ManagedFonts::GetFontMetrics(
  const ON_Font* font,
  ON_FontMetrics& font_unit_font_metrics
)
{
  return
    (nullptr != ON_Font::Internal_CustomGetFontMetricsFunc)
    ? ON_Font::Internal_CustomGetFontMetricsFunc(font, font_unit_font_metrics)
    : ON_FreeTypeGetFontMetrics(font, font_unit_font_metrics);
}
