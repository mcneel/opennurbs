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
#if !defined(OPENNURBS_INTERNAL_GLYPH_INC_)
#define OPENNURBS_INTERNAL_GLYPH_INC_

class ON_Internal_FontGlyphPool : private ON_FixedSizePool
{
private:
  friend class ON_FontGlyph;
  friend class ON_GlyphMap;
  ON_Internal_FontGlyphPool();
  ~ON_Internal_FontGlyphPool() = default;
  ON_Internal_FontGlyphPool(const ON_Internal_FontGlyphPool&) = delete;
  ON_Internal_FontGlyphPool operator=(const ON_Internal_FontGlyphPool&) = delete;
  static ON_Internal_FontGlyphPool theGlyphItemPool;
};

class ON_ManagedFonts
{
public:
  // List is the only instance of this class.
  static ON_ManagedFonts List;

  const ON_Font* GetFromFontCharacteristics(
    const ON_Font& font_characteristics,
    bool bCreateIfNotFound
    );

  const ON_Font* GetFromAppleFontName(
    const wchar_t* apple_font_name,
    bool bCreateIfNotFound
    );
  
  const ON_Font* GetFromSerialNumber(
    unsigned int managed_font_runtime_serial_number
    );

  unsigned int GetList(
    ON_SimpleArray< const ON_Font* >& managed_fonts
    );

  /*
  Returns:
    0: failure
    >0: success font glyph id
  */
  static ON__UINT_PTR GetGlyphMetrics(
    const class ON_Font* font, 
    ON__UINT32 unicode_code_point,
    class ON_TextBox& font_unit_glyph_box
    );

  static void GetFontMetrics(
    const ON_Font* font,
    ON_FontMetrics& font_unit_font_metrics
    );

private:
  ON_ManagedFonts() = default;
  ~ON_ManagedFonts();

  const ON_Font* Internal_AddManagedFont(
    const ON_Font* managed_font
    );

private:
  ON_SimpleArray<const ON_Font*> m_managed_fonts;
  ON_SimpleArray<const ON_Font*> m_managed_fonts_by_serial_number;
  unsigned int m_sorted_count = 0;
  unsigned int m_sorted_by_serial_number_count = 0;
};

class ON_CLASS ON_GlyphMap
{
public:
  ON_GlyphMap();
  ~ON_GlyphMap() = default;

public:
  const class ON_FontGlyph* FindGlyph(
    const ON__UINT32 unicode_code_point
  ) const;
  
  // returns pointer to the persistent glyph item
  const ON_FontGlyph* InsertGlyph(
    const ON_FontGlyph& glyph
  );

  unsigned int GlyphCount() const;

private:
  friend class ON_Font;
  friend class ON_FontGlyph;
  unsigned int m_glyph_count = 0;
  unsigned int m_reserved = 0;
  ON_SimpleArray< const class ON_FontGlyph* > m_glyphs;
};

#endif
