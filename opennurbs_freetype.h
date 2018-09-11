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

#if !defined(OPENNURBS_FREETYPE_INC_)
#define OPENNURBS_FREETYPE_INC_

#if defined(ON_COMPILER_MSC) 
//&& defined(ON_64BIT_RUNTIME)
// freetype is not delivered in a 32-bit version.
// To disable freetype support, comment out the following define.
// To enable freetype support, define OPENNURBS_FREETYPE_SUPPORT

#define OPENNURBS_FREETYPE_SUPPORT

#elif defined(ON_RUNTIME_APPLE)
// To disable freetype support, comment out the following define.
// To enable freetype support, define OPENNURBS_FREETYPE_SUPPORT

#define OPENNURBS_FREETYPE_SUPPORT

#endif

/*
Parameters:
  font_unit_font_metrics - [in]
    metrics in font units (freetype face loaded with FT_LOAD_NO_SCALE) unless
    it is a "tricky" font.
*/
ON_DECL
void ON_FreeTypeGetFontMetrics(
  const ON_Font* font,
  ON_FontMetrics& font_unit_font_metrics
  );

/*
Parameters:
  font_unit_glyph_box - [in]
    box in font units (freetype face loaded with FT_LOAD_NO_SCALE) unless
    it is a "tricky" font.
Returns:
  0 if box was not set.
  >0: font glyph id (or other non-zero value) when box is set
*/
ON_DECL
ON__UINT_PTR ON_FreeTypeGetGlyphMetrics(
  const ON_Font* font,
  ON__UINT32 unicode_code_point,
  class ON_TextBox& font_unit_glyph_box
);

ON_DECL
bool ON_FreeTypeGetGlyphOutline(
  const ON_FontGlyph* glyph,
  bool bSingleStrokeFont,
  double text_height,
  ON_ClassArray< ON_SimpleArray< ON_Curve* > >& contours,
  ON_BoundingBox* glyph_bbox,
  ON_3dVector* glyph_advance
);

/*
Description:
  A wrapper for calculating parameters and calling FreeType library 
  functions FT_Set_Char_Size() FT_Load_Glyph().
Parameters:
  ft_face - [in]
    A pointer to and FT_Face. One way to get this value is to call ON_Font::FreeTypeFace()
  font_glyph_id - [in]
    font glyph id
Returns:
  True if glyph is available and loaded.
*/
ON_DECL
bool ON_FreeTypeLoadGlyph(
  ON__UINT_PTR ft_face,
  ON__UINT_PTR font_glyph_id,
  bool bLoadRenderBitmap
);


#endif
