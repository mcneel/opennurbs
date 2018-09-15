/*
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
*/

#if !defined(OPENNURBS_APPLE_NSFONT_INC_)
#define OPENNURBS_APPLE_NSFONT_INC_

//#define ON_NSFONT_GLYPH_SUPPORT_WIP
#if defined(ON_RUNTIME_APPLE) && defined(ON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE) && defined(ON_NSFONT_GLYPH_SUPPORT_WIP)

ON_DECL
bool ON_AppleNSFontGetGlyphMetrics(
  NSFont* appleFont,
  unsigned int font_design_units_per_M,
  unsigned int glyphIndex,
  class ON_TextBox& glyph_metrics
);

ON_DECL
bool ON_AppleNSFontGetGlyphOutline(
  NSFont* appleFont,
  unsigned int font_design_units_per_M,
  unsigned int glyphIndex,
  ON_OutlineFigure::Type figure_type,
  class ON_Outline& outline
);
#endif

#endif
