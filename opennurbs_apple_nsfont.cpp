//
// Copyright (c) 1993-2018 Robert McNeel & Associates. All rights reserved.
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

#if defined(ON_RUNTIME_APPLE) && defined(ON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE)

#include "opennurbs_internal_glyph.h"
#include "opennurbs_apple_nsfont.h"

void ON_AppleNSFontGetFontMetrics(
  const ON_Font* font,
  ON_FontMetrics& font_metrics
  )
{
  for (;;)
  {
    if (nullptr == font)
      break;
    
    font = font->ManagedFont();
    if (nullptr == font)
      break;
    
    NSFont* appleFont = font->AppleFont();
    if (nullptr == appleFont)
      break;
    
    // No way to get actual font design units per M from NSFont API?
    // 2048 is a common value and the default.
    unsigned int font_design_units_per_M = 2048;
#if defined(OPENNURBS_FREETYPE_SUPPORT)
    const unsigned int freetypeUPM = ON_FreeTypeGetFontUnitsPerM(font);
    if (freetypeUPM > 0)
      font_design_units_per_M = freetypeUPM;
#endif
    
    const double point_size = appleFont.pointSize;
    const double UPM = font_design_units_per_M;
    
    // This scaling approach recovers correct values in font design units
    // when compared with values from DWrite and FreeType.
    const double pointSizeToUPM = (point_size > 0.0) ? (UPM/point_size) : 1.0;
    
    const double leading = pointSizeToUPM*((double)appleFont.leading);
    const double ascent = pointSizeToUPM*((double)appleFont.ascender);
    const double descent = pointSizeToUPM*((double)appleFont.descender);
    
    const double line_space1 = ascent - descent + leading;
    const double line_space2 = pointSizeToUPM*((double)(appleFont.ascender-appleFont.descender+appleFont.leading));
    const double line_space = (line_space2>=line_space1) ? line_space2 : line_space1;
    
    const double ascent_of_x = pointSizeToUPM*((double)appleFont.xHeight);
    double ascent_of_capital = pointSizeToUPM*((double)appleFont.capHeight);
    
    const double underscore_position = pointSizeToUPM*((double)appleFont.underlinePosition);
    const double underscore_thickness = pointSizeToUPM*((double)appleFont.underlineThickness);
    
    font_metrics = ON_FontMetrics::Unset;
    font_metrics.SetHeights(
      ascent,
      descent,
      UPM,
      line_space
      );
    if (false == font_metrics.AscentDescentAndUPMAreValid())
      break;
    
    font_metrics.SetAscentOfx(ascent_of_x);
    font_metrics.SetAscentOfCapital(ascent_of_capital);
    font_metrics.SetUnderscore(underscore_position,underscore_thickness);
    
    // Have to fake strikeout settings - not in NSFont API?
    //int strikeout_position = ...;
    //int strikeout_thickness = ...;
    double h = (ascent_of_capital > 0) ? ascent_of_capital : ascent;
    if (h > 0.0)
    {
    font_metrics.SetStrikeout(
      (0.5*h),
      (0.5*underscore_thickness)
      );
    }
    
    if (font_metrics.AscentDescentAndUPMAreValid())
      return;
    break;
  }
  
  font_metrics = ON_FontMetrics::Unset;
  return;
}

#if defined(ON_NSFONT_GLYPH_SUPPORT_WIP)

bool ON_AppleNSFontGetGlyphMetrics(
  NSFont* appleFont,
  unsigned int font_design_units_per_M,
  unsigned int glyphIndex,
  class ON_TextBox& glyph_metrics
)
{
  // TODO - make this work on MacOS
  glyph_metrics = ON_TextBox::Unset;
  return false;
}

bool ON_AppleNSFontGetGlyphOutline(
  NSFont* appleFont,
  unsigned int font_design_units_per_M,
  unsigned int glyphIndex,
  ON_OutlineFigure::Type figure_type,
  class ON_Outline& outline
)
{
  // TODO - make this work on MacOS
  outline = ON_Outline::Unset;
  return false;
}

ON__UINT_PTR ON_AppleNSFontGetGlyphMetrics(
  const ON_FontGlyph* glyph,
  unsigned int font_design_units_per_M,
  ON_TextBox& glyph_metrics
)
{
  glyph_metrics = ON_TextBox::Unset;

  if (nullptr == glyph)
    return 0;

  if (false == glyph->CodePointIsSet())
    return 0;

  const ON_Font* font = glyph->Font();
  if (nullptr == font)
    return 0;

  NSFont* appleFont = font->AppleFont();
  if (nullptr == appleFont)
    return 0;

  ON__UINT_PTR glpyh_id = 0;

  bool rc = false;

  // NEVER TESTED
  //NSString* baseString = [NSString stringWithFormat:@"%C", glpyh->CodePoint()];
  //NSGlyphInfo* glyphInfo = [NSGlyphInfo glyphInfoWithGlyph:glyph forFont:appleFont baseString: baseString];
  //if (nullptr == glyphInfo)
  //  return 0;

  //glyph_id = (ON__UINT_PTR)glyphInfo.glyphID();

  //const bool rc
  //  = (0 != glpyh_id)
  //  ? ON_AppleNSFontGetGlyphMetrics(appleFont, (unsigned int)glpyh_id, glyph_metrics)
  //  : false;

  return rc ? glpyh_id : 0;
}

bool ON_AppleNSFontGetGlyphOutline(
  const ON_FontGlyph* glyph,
  unsigned int font_design_units_per_M,
  ON_OutlineFigure::Type figure_type,
  class ON_Outline& outline
)
{
  outline = ON_Outline::Unset;

  if (nullptr == glyph)
    return false;

  if (false == glyph->CodePointIsSet())
    return false;

  ON__UINT64 glpyh_id = (ON__UINT64)glyph->FontGlyphId();
  if (glpyh_id > 0xFFFFFFFF)
    return false;

  const unsigned int glyphIndex = (unsigned int)glpyh_id;

  const ON_Font* font = glyph->Font();
  if (nullptr == font)
    return false;

  if (ON_OutlineFigure::Type::Unset == figure_type)
  {
    ON_OutlineFigure::Type font_figure_type = font->OutlineFigureType();
    if (ON_OutlineFigure::Type::Unset != font_figure_type)
    {
      figure_type = font_figure_type;
    }
  }

  NSFont* appleFont = font->AppleFont();
  if (nullptr == appleFont)
    return false;

  const bool rc = ON_AppleNSFontGetGlyphOutline(
    appleFont,
    font_design_units_per_M,
    glyphIndex,
    figure_type,
    outline
  );

  return rc;
}

#endif
#endif
