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


#if !defined(OPENNURBS_FONT_INC_)
#define OPENNURBS_FONT_INC_

class ON_CLASS ON_FontMetrics
{
public:
  ON_FontMetrics() = default;
  ~ON_FontMetrics() = default;
  ON_FontMetrics(const ON_FontMetrics&) = default;
  ON_FontMetrics& operator=(const ON_FontMetrics&) = default;


public:
  // All properties are zero.
  static const ON_FontMetrics Unset;

  /*
    ON_FontMetric::DefaultLineFeedRatio*ON_FontMetrics().AspectOfI()
    can be used to cook up a line space value when using the 
    ON_FontMetrics.LineSpace() value defined by the font is
    not desired.
  */
  static const double DefaultLineFeedRatio; // 1.6

  // UNICODE code point of the glyph used to determine HeightOfCapital()
  // when no reaonable value is available from the font definition.
  // Currently this is the 'I' glyph. Opennurbs has used 'I' since 2005.
  // It is possible 'H' would work as well. All other glyphs, in 
  // particular 'M' and 'W', do not work.
  static const ON__UINT32 HeightOfCapitalCodePoint; // 'I'

public:
  /*
  Returns:
    Signed distance from the baseline to highest point on a glyph outline.
  Remarks:
    If every glyph outline in the font has (0,0) on the basline, then Ascent() 
    is the maximum glyph bounding box Y.
  */
  int Ascent() const;

  /*
  Returns:
    Signed distance from the baseline to lowest point on a glyph outline.
  Remarks:
    This value is typically negative because glyphs for letters like 'j'
    typically have a portion of their outline below the baseline.  However,
    some fonts have positive descent.
    If every glyph outline in the font has (0,0) on the basline, then Ascent() 
    is the maximum glyph bounding box Y.
  */
  int Descent() const;

  /*
  Returns:
    The postive distance to move a base line when moving to a new line of text.

  Remarks:
    For almost every font used to render text, LineSpace() > (Ascent() - Descent()).

    This metric is sometimes called "height", but that term is often confused
    with (Ascent() - Descent()). 

    For fonts designed to render horizontal lines of text, LineSpace() is a 
    vertical distance. For fonts desingned to render vertical lines of text,
    LineSpace() is a horizontal distance.  Depending on the context, the 
    direction to move can be up, down, left or right.
  */
  int LineSpace() const;

  /*
  Returns:
    The "units per EM". This is the height and width of the square grid 
    where the font glyphs are designed.  
  Remarks:
    The width of the 'M' glyph in a font can be  different from UPM.
    The height of the 'M' glyph in a font is typically less than UPM.
    In TrueType fonts, UPM is often a power of two and generally 1024 or 2048.
    In OpenType fonts, UPM is often 1000.
    In PostScript fonts, UPM is often 1000.
  */
  int UPM() const;

  /*
  Returns:
    The signed distance from the baseline to the highest point on the 'I' glyph.
  Remarks:
    The primary uses of AscentOfI() are:
    1) 
    Calculate a scale factor to produce text with a user specified "text height".
    2) 
    To calculate insertion location for ON::TextVerticalAlignment::Middle
    and ON::TextVerticalAlignment::Top.

    Since 2005, opennurbs has used
    (user specified text height)/AscentOfI()
    as the scale factor to render glyphs when user interface has provided 
    a "text height" value.  Users are more satisfied with this approach
    than when the scaling is base on line space or maximum font glyph ascent
    values. Experiments have ruled out the use of any other capital latin 
    letter glyph except 'H' for this use.  When a font does not contain an
    'I' glyph, a suitable value is returned that can be used for text height
    scaling and vertical alignment.
  */
  int AscentOfI() const;
  
  /*
  Description:
    Get the scale to apply to normalized glyph boxes and outlines to 
    render the 'I' in the glyph's font at a height of text_height.
  Parameters:
    text_height - [in]
      The desired height of typical capital latin letter glyphs.
  Returns:
    text_height / AscentOfI().
  */
  double GlyphScale(double text_height) const;
  
  /*
  Returns:
    Thickness of strikeout.
  Remarks:
    The signed distance from the baseline to the bottom of the strikeout 
    is StrikeoutPosition() - StrikeoutThickness()/2.
  */
  int StrikeoutThickness() const;

  /*
  Returns:
    Signed distance from baseline to center of strikeout. 
    A positive value indicates the strikeout is above the baseline (common).
  Remarks:
    The signed distance from the baseline to the bottom of the strikeout 
    is StrikeoutPosition() - StrikeoutThickness()/2.
  */
  int StrikeoutPosition() const;


  /*
  Returns:
    Thickness of underscore
  Remarks:
    The signed distance from the baseline to the bottom of the underscore
    is UnderscorePosition() - UnderscoreThickness()/2.
  */
  int UnderscoreThickness() const;
  
  /*
  Returns:
    Signed distance from baseline to center of underscore.
    A negative value indicates the underscore is below the baseline (common).
  Remarks:
    The signed distance from the baseline to the bottom of the underscore
    is UnderscorePosition() - UnderscoreThickness()/2.
  */
  int UnderscorePosition() const; 

  static const ON_FontMetrics Scale(
    const ON_FontMetrics& font_metrics,
    double scale
  );

  void SetHeights(
    int ascent,
    int descent,
    int UPM,
    int line_space
  );

  bool HeightsAreValid() const;

  void SetAscentOfI(
    int ascent_of_I
  );
  
  void SetStrikeout(
    int strikeout_position,
    int strikeout_thickness
  );
  
  void SetUnderscore(
    int underscore_position,
    int underscore_thickness
  );
  
private:
  int m_UPM = 0;           // units per EM
  int m_ascent = 0;        // max over all glyphs in font of (highest outline point - baseline point).y
  int m_descent = 0;       // min over all glyphs in font of (lowest outline point - baseline point).y
  int m_line_space = 0;    // distance between baselines 
  int m_ascent_of_I = 0;   // (highest 'I' outline point - I baseline point).y

  int m_strikeout_thickness = 0;      //
  int m_strikeout_position = 0;  // 

  int m_underscore_thickness = 0;     //
  int m_underscore_position = 0; //

private:
  int m_reserved1 = 0;
  double m_reserved2 = 0.0;
  double m_reserved3 = 0.0;
  ON__UINT_PTR m_reserved5 = 0;
};


/// <summary>
/// An ON_Font is a face in a font family. It corresponds to a Windows LOGFONT,
/// a .NET System.Drawing.Font or a FreeType FT_Face.
/// </summary>
class ON_CLASS ON_Font
{
public:

#pragma region RH_C_SHARED_ENUM [ON_Font::Weight] [Rhino.DocObjects.Font.FontWeight] [nested:byte]
  /// <summary>
  /// Weight enum values
  /// Avoid casting these values to int.
  /// Use ON_Font::WindowsLogfontWeightFromWeight() or
  /// ON_Font::AppleWeightOfFontFromWeight() or
  /// add another converter.
  /// </summary>
  enum class Weight : unsigned char
  {
    /// <summary> Not set. </summary>
    Unset = 0,

    /// <summary> IsLight = true </summary>
    Thin = 1,

    /// <summary> IsLight = true </summary>
    Ultralight = 2,

    //ExtraLight = 2,

    /// <summary> IsLight = true </summary>
    Light = 3,

    /// <summary> Default font weight. IsNormalWeight = true  Also called Regular.</summary>
    Normal = 4,  

    //Regular    = 4,

    /// <summary> IsNormalWeight = true </summary>
    Medium = 5,
    
    /// <summary> IsBold = true </summary>
    Semibold = 6,

    //Demibold   = 6,
    //Demi       = 6,
    //Semi       = 6,
   
    /// <summary> IsBold = true </summary>
    Bold = 7,
    
    /// <summary> IsBold = true </summary>
    Ultrabold = 8,

    //ExtraBold  = 8,

    /// <summary> IsBold = true Also called Black</summary>
    Heavy      = 9

    //Black      = 9,
  };
#pragma endregion

  /*
  Returns:
    -1: weight_a is lighter, weight_b is heavier
    +1: weight_a is heavier, weight_b is lighter
     0: weight_a = weight_b
  */
  static int CompareWeight(
    ON_Font::Weight weight_a,
    ON_Font::Weight weight_b
    );

  /*
  Description:
    In the rare cases when an ON_Font::Weight value must be passed
    as an unsigned int, use ON_Font::FontWeightFromUnsigned() to
    convert the unsigned value to an ON_Font::Weight value.
  Parameters:
    unsigned_font_weight - [in]
  */
  static ON_Font::Weight FontWeightFromUnsigned(
    unsigned int unsigned_font_weight
    );

  /*
  Description:
    The correspondence between Windows LOGFONT lfWeight values and
    ON_Font::Weight enum values is
    ON_Font::Weight::Thin       = 100 LOGFONT lfWeight
    ON_Font::Weight::Ultralight = 200 LOGFONT lfWeight
    ON_Font::Weight::Light      = 300 LOGFONT lfWeight
    ON_Font::Weight::Normal     = 400 LOGFONT lfWeight
    ON_Font::Weight::Medium     = 500 LOGFONT lfWeight
    ON_Font::Weight::Semibold   = 600 LOGFONT lfWeight
    ON_Font::Weight::Bold       = 700 LOGFONT lfWeight
    ON_Font::Weight::Ultrabold  = 800 LOGFONT lfWeight
    ON_Font::Weight::Heavy      = 900 LOGFONT lfWeight
  Returns:
    The Windows LOGFONT lfWeight value that corresponds to the ON_Font::Weight enum value.
  */
  static int WindowsLogfontWeightFromWeight(
    ON_Font::Weight font_weight
    );

  /*
  Description:
    The correspondence between Apple "weight of font" values and
    ON_Font::Weight enum values is
    ON_Font::Weight::Thin       = 1
    ON_Font::Weight::Ultralight = 2
    ON_Font::Weight::Light      = 3
    ON_Font::Weight::Normal     = 4  
    ON_Font::Weight::Medium     = 5
    ON_Font::Weight::Semibold   = 6
    ON_Font::Weight::Bold       = 7
    ON_Font::Weight::Ultrabold  = 8
    ON_Font::Weight::Heavy      = 9
  Returns:
    The Apple "weight of font" value that corresponds to the ON_Font::Weight enum value.
  */  
  static int AppleWeightOfFontFromWeight(
    ON_Font::Weight font_weight
    );

  /*
  Description:
    The correspondence between Apple "font weight trait" values and
    ON_Font::Weight enum values is
    ON_Font::Weight::Thin       = -0.4    Apple font weight trait
    ON_Font::Weight::Ultralight = -0.2667 Apple font weight trait
    ON_Font::Weight::Light      = -0.1333 Apple font weight trait
    ON_Font::Weight::Normal     =  0.0    Apple font weight trait
    ON_Font::Weight::Medium     =  0.1333 Apple font weight trait
    ON_Font::Weight::Semibold   =  0.2667 Apple font weight trait
    ON_Font::Weight::Bold       =  0.4    Apple font weight trait
    ON_Font::Weight::Ultrabold  =  0.5333 Apple font weight trait
    ON_Font::Weight::Heavy      =  0.6667 Apple font weight trait
  Returns:
    The Apple "NSFontWeightTrait" value that corresponds to the ON_Font::Weight enum value.
  */  
  static double AppleFontWeightTraitFromWeight(
    ON_Font::Weight font_weight
    );

  /*
  Description:
    The correspondence between Windows LOGFONT lfWeight values and
    ON_Font::Weight enum values is

    ON_Font::Weight::Thin       = 100
    ON_Font::Weight::Ultralight = 200
    ON_Font::Weight::Light      = 300
    ON_Font::Weight::Normal     = 400
    ON_Font::Weight::Medium     = 500
    ON_Font::Weight::Semibold   = 600
    ON_Font::Weight::Bold       = 700
    ON_Font::Weight::Ultrabold  = 800
    ON_Font::Weight::Heavy      = 900
  Returns:
    The best ON_Font::Weight enum value for the Windows LOGFONT weight.
  */  

  static ON_Font::Weight WeightFromWindowsLogfontWeight(
    int windows_logfont_weight
    );

  /*
  Description:
    The correspondence between Apple "weight of font" values and
    ON_Font::Weight enum values is
    ON_Font::Weight::Thin       = 1
    ON_Font::Weight::Ultralight = 2
    ON_Font::Weight::Light      = 3
    ON_Font::Weight::Normal     = 4  
    ON_Font::Weight::Medium     = 5
    ON_Font::Weight::Semibold   = 6
    ON_Font::Weight::Bold       = 7
    ON_Font::Weight::Ultrabold  = 8
    ON_Font::Weight::Heavy      = 9
  Returns:
    The best ON_Font::Weight enum value for the Apple weight of font.
  */  
  static ON_Font::Weight WeightFromAppleWeightOfFont(
    int apple_weight_of_font
    );

  /*
  Parameters:
    apple_font_weight_trait - [in]
      Apple NSFontWeightTrait
      The valid value range is from -1.0 to 1.0. The value of 0.0 corresponds to the regular or medium font weight.
  */
  static ON_Font::Weight WeightFromAppleFontWeightTrait(
    double apple_font_weight_trait
    );

#pragma region RH_C_SHARED_ENUM [ON_Font::Stretch] [Rhino.DocObjects.Font.FontStretch] [nested:byte]
  /// <summary>
  /// Horizontal expansion or contraction of font
  /// </summary>
  enum class Stretch : unsigned char
  {
    /// <summary> Not set. </summary>
    Unset = 0,
    /// <summary> </summary>
    Ultracondensed = 1,
    /// <summary> </summary>
    Extracondensed = 2,
    /// <summary> </summary>
    Condensed = 3,
    /// <summary> </summary>
    Semicondensed = 4,

    /// <summary> Default font stretch. </summary>
    Medium = 5,

    //Normal         = 5,

    /// <summary> </summary>
    Semiexpanded = 6,
    /// <summary> </summary>
    Expanded = 7,
    /// <summary> </summary>
    Extraexpanded = 8,
    /// <summary> </summary>
    Ultraexpanded = 9
  };
#pragma endregion

  /*
  Description:
    In the rare cases when an ON_Font::Stretch value must be passed
    as an unsigned int, use ON_Font::FontStretchFromUnsigned() to
    convert the unsigned value to an ON_Font::Stretch value.
  Parameters:
    unsigned_font_stretch - [in]
  */
  static ON_Font::Stretch FontStretchFromUnsigned(
    unsigned int unsigned_font_stretch
    );


#pragma region RH_C_SHARED_ENUM [ON_Font::Style] [Rhino.DocObjects.Font.FontStyle] [nested:byte]
  /// <summary>
  /// Vertical angle of font
  /// Upright, Italic, or Oblique
  /// </summary>
  enum class Style : unsigned char
  {
    /// <summary> Not set. </summary>
    Unset = 0,

    /// <summary> Default font style. </summary>
    Upright = 1,

    //Normal = 1,
    //Roman = 1,

    /// <summary> </summary>
    Italic = 2,

    /// <summary> </summary>
    Oblique = 3
  };
#pragma endregion

  /*
  Description:
    In the rare cases when an ON_Font::Style value must be passed
    as an unsigned int, use ON_Font::FontStyleFromUnsigned() to
    convert the unsigned value to an ON_Font::Style value.
  Parameters:
    unsigned_font_style - [in]
  */
  static ON_Font::Style FontStyleFromUnsigned(
    unsigned int unsigned_font_style
    );

public:
  static const ON_Font Default;
  static bool IsValidFaceName(
    const wchar_t* face_name
    );
    
private:
  // This private constructor is used to construct ON_Font::Default and managed fonts.
  // Never make this constructor protected or public.
  ON_Font(
    unsigned char managed_status, // 0 = no, 1 = ON_Font::Default, 2 = managed
    const ON_Font& src
    );

private:
  // Use ON_Font( const ON_Font& ) or ON_Font::operator= if you need to make a copy.
  // Never make CopyHelper protected or public.
  void CopyHelper(
    const ON_Font&
    );

public:
  /*
  Description:
    Get a font managed by the application from the font characteristics.
    Never delete a font returned by GetManagedFont().
  Parameters:
    face_name - [in]
    font_weight - [in]
      default = ON_Font::Default.FontWeight()
    font_style - [in]
      default = ON_Font::Default.FontStyle()
    font_stretch - [in]
      default = ON_Font::Default.FontStretch()
    bUnderlined - [in]
      default = ON_Font::Default.Underlined() = false
    bStrikethrough - [in]
      default = ON_Font::Default.Strikethrough() = false
    linefeed_ratio - [in]
      default = ON_Font::Default.LinefeedRatio()
    windows_charset - [in]
      default = ON_Font::WindowsCharSet::DefaultCharSet
  */
  static const ON_Font* GetManagedFont(
    const wchar_t* face_name
    );

  static const ON_Font* GetManagedFont(
    double point_size,
    const wchar_t* face_name
    );

  static const ON_Font* GetManagedFont(
    const wchar_t* face_name,
    bool bBold
    );

  static const ON_Font* GetManagedFont(
    double point_size,
    const wchar_t* face_name,
    bool bBold
    );

  static const ON_Font* GetManagedFont(
    const wchar_t* face_name,
    bool bBold,
    bool bItalic
    );

  static const ON_Font* GetManagedFont(
    double point_size,
    const wchar_t* face_name,
    bool bBold,
    bool bItalic
    );

  static const ON_Font* GetManagedFont(
    const wchar_t* face_name, 
    ON_Font::Weight font_weight,
    ON_Font::Style font_style
    );

  static const ON_Font* GetManagedFont(
    double point_size,
    const wchar_t* face_name, 
    ON_Font::Weight font_weight,
    ON_Font::Style font_style
    );

  static const ON_Font* GetManagedFont(
    const wchar_t* face_name, 
    ON_Font::Weight font_weight,
    ON_Font::Style font_style,
    ON_Font::Stretch font_stretch,
    bool bUnderlined, 
    bool bStrikethrough,
    double linefeed_ratio,
    unsigned int logfont_charset
    );

  static const ON_Font* GetManagedFont(
    double point_size,
    const wchar_t* face_name, 
    ON_Font::Weight font_weight,
    ON_Font::Style font_style,
    ON_Font::Stretch font_stretch,
    bool bUnderlined, 
    bool bStrikethrough,
    double linefeed_ratio,
    unsigned int logfont_charset
    );

  static const ON_Font* GetManagedFontFromFontDescription(
    const wchar_t* font_description
    );

#if defined(ON_OS_WINDOWS_GDI)
  /*
  Description:
    Get a managed font from a LOGFONT
  Parameters:
    map_mode - [in]
      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.
    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.
    logfont - [in]
      These logfont properties are used to find the managed font.
        lfHeight (when dc is not zero)
        lfWeight;
        lfItalic;
        lfUnderline;
        lfStrikeOut;
        lfCharSet;
        lfFaceName[LF_FACESIZE];
        All other LOGFONT properties is ignored.
  See Also:
    ON_Font::GetManagedFontFromWindowsLogfontComplete
  */
  static const ON_Font* GetManagedFontFromWindowsLogfont(
    int map_mode,
    HDC hdc,
    const LOGFONT& logfont
    );


#endif

  static const ON_Font* GetManagedFontFromAppleFontName(
    const wchar_t* apple_font_name
    );

  /*
  Returns:
    The managed font for this font. 
  Remarks:
    If this->IsManagedFont() is true, then "this" is returned.
  */
  const ON_Font* ManagedFont() const;

  /*
  Description:
    Returns the glpyh informationh for used to render a specific code point
  Parameters:
    unicode_code_point 
      UNICODE code point value
  Returns:
    Glyph rendering information.

  Remarks:
    Typically the returned glpyh uses is a single glpyh in this->ManagedFont().
    In this case, glyph->SubstitueCount() is 0.

    In some cases one or more glyphs from one or more substitute fonts are required
    to render the code point.  In this case, glyph->SubstitueCount() is 0.

  Example:
    ON_Font* font = ...;
    unsigned int code_point = ...;
    const ON_FontGlyph* g = font->CodePointGlyph(code_point);
    if (nullptr != g )
    {
      if ( g->SubstituteCount() > 0 )
      {
        // complicate case - one of more substitutes must be rendered to render g
        for ( const ON_FontGlyph* gsub = g.NextSubstitute(); nullptr != gsub; gsub = gsub->NextSubstitute() )
        {
          ...
        }
      }
      else
      {
        // simple case - this computer can directly render g
        ...
      }
    }
  */
  const class ON_FontGlyph* CodePointGlyph(
    ON__UINT32 unicode_code_point
  ) const;

private:
  friend class ON_FontGlyph;
  const class ON_FontGlyph* Internal_ManagedCodePointGlyph(
    ON__UINT32 unicode_code_point,
    bool bCreateIfMissing,
    bool bFindSubstitutes
  ) const;

public:

  /*
  Description:
    When reading version 5 3dm achives, the font description can be
    a generic font description or an Apple font name. This function
    rejects certain descriptions like "Default" and "Arial" for
    use as Apple font names.
  */
  static bool IsNotAppleFontName(
    const wchar_t* font_description
    );

  static const ON_Font* GetManagedFont(
    const ON_Font& font_characteristics,
    bool bCreateIfNotFound
    );

  static const ON_Font* GetManagedFontFromSerialNumber(
    unsigned int managed_font_runtime_serial_number
    );

  static unsigned int GetManagedFontList(
    ON_SimpleArray< const ON_Font* >& managed_fonts
    );

  /*
  Returns:
    True if this font is a managed font returned by one of the
    static ON_Font::GetManagedFont(...) functions.
  Remarks:
    ON_Font::Default is managed.
  */
  bool IsManagedFont() const;

public:

  /*
  Description:
    If opennurbs is built with FreeType support then
    FT_Face freetype_face = (FT_Face)ON_Font::FreeTypeFace(font)
    will return a FreeType face that can be used to render the font.
  Parameters:
    font - [in]
  Returns:
    A value that can be cast as a FreeType FT_Face.
  Example
    const ON_Font* font = ...;
    FT_Face freetype_face = (FT_Face)ON_Font::FreeTypeFace(font);
  Remarks:
    Many fonts do not have a glyph for a every UNICODE codepoint and font
    substitution is required. If you want to get the freetype face
    used for a specfic UNICODE codepoint, call ON_Font::CodepointFreeTypeFace().
  */
  static ON__UINT_PTR FreeTypeFace(
    const ON_Font* font
  );
 
private:
  /*
  Description:
    Helper function used by destructor to deallocate memory used
    by FreeType face
  */
  static void DestroyFreeTypeFace(
    const ON_Font* font
  );

public:
  ON_Font();
  ~ON_Font() = default;
  ON_Font(const ON_Font& src);
  ON_Font& operator=(const ON_Font& src);

public:
  /*
  Description:
    Create a font with a specified facename and properties.
  Parameters:
    face_name - [in]
      nullptr is treated as ON_Font::Default.FaceName().
    bBold - [in]
      True for a bold version of the font.
    bItalic - [in]
      True for an italic version of the font.
  Returns:
    True if the font characteristics were valid and set on the font.
 */
  bool SetFontCharacteristics( 
    const wchar_t* face_name,
    bool bBold,
    bool bItalic,
    bool bUnderlined,
    bool bStrikethrough
    );

  /*
  Description:
    Create a font with a specified facename and properties.
  Parameters:
    point_size - [in]
      If point_size > 0.0, then it specifies which size of font definition
      should be used. Otherwise the font size used for annotation text
      is used.
      For high quality fonts it is generally the case that
      different point sizes of the same font face have
      subtle differences in glyph design and are not
      simply scaled versions of a base glyph.
    face_name - [in]
      nullptr is treated as ON_Font::Default.FaceName().
    bBold - [in]
      True for a bold version of the font.
    bItalic - [in]
      True for an italic version of the font.
  Returns:
    True if the font characteristics were valid and set on the font.
 */
  bool SetFontCharacteristics( 
    double point_size,
    const wchar_t* face_name,
    bool bBold,
    bool bItalic,
    bool bUnderlined,
    bool bStrikethrough
    );

  /*
  Description:
    Set the font's face name and characteristics.
  Parameters:
    face_name - [in]
      nullptr is not permitted.
      Pass ON_Font::Default.FaceName() if you don't have a face name.
  Returns:
    True if the font characteristics were valid and set on the font.
 */  
  bool SetFontCharacteristics( 
    const wchar_t* face_name,
    ON_Font::Weight font_weight,
    ON_Font::Style font_style,
    ON_Font::Stretch font_stretch,
    bool bUnderlined,
    bool bStrikethrough
    );

  bool SetFontCharacteristics( 
    double point_size,
    const wchar_t* face_name,
    ON_Font::Weight font_weight,
    ON_Font::Style font_style,
    ON_Font::Stretch font_stretch,
    bool bUnderlined,
    bool bStrikethrough
    );

  bool SetFontCharacteristics( 
    const wchar_t* face_name,
    ON_Font::Weight font_weight,
    ON_Font::Style font_style,
    ON_Font::Stretch font_stretch,
    bool bUnderlined,
    bool bStrikethrough,
    double linefeed_ratio,
    unsigned int logfont_charset
    );

  bool SetFontCharacteristics( 
    double point_size,
    const wchar_t* face_name,
    ON_Font::Weight font_weight,
    ON_Font::Style font_style,
    ON_Font::Stretch font_stretch,
    bool bUnderlined,
    bool bStrikethrough,
    double linefeed_ratio,
    unsigned int logfont_charset
    );


  /*
  Description:
    The font properties weight, style, stretch, underlined, 
    and strikethrough are encoded in the returned value.
  Remarks:
    This is a legacy value used in 3dm archive reading/writing 
    and some sorting operations.
  */
  unsigned int FontCharacteristicsAsUnsigned() const;

private:
  /*
  Description:
    All font characterisics defined by the input parameters are encoded 
    in the returned value.
  Remarks:
    Used in 3dm archive reading/writing.
  */
  static unsigned int Internal_FontCharacteristicsAsUnsigned(
    ON_Font::Weight font_weight,
    ON_Font::Style font_style,
    ON_Font::Stretch font_stretch,
    bool bUnderlined,
    bool bStrikethrough
    );
    
  /*
  Description:
    All font characterisics except facename (weight, style, stretch, 
    underlined, strikethrough, charset) are encoded in the returned
    value.
  Parameters:
    font_characteristics_as_unsigned - [in]
    Value returned from ON_Font.FontCharacteristicsAsUnsigned()
  Returns:
    True if the characterstics were set.
  Remarks:
    Used in 3dm archive reading/writing.
  */
  bool Internal_SetFontCharacteristicsFromUnsigned(
    unsigned int font_characteristics_as_unsigned
    );

public:
  /*
  Description:
    Returns a 32-bit crc of the font weight, style, stretch, underline, strikethrough, 
    and facename characteristics.

  Parameters:
    bIgnoreFaceNameOrdinalCase - [in]
      If true, ON_wString::MapStringOrdinal() is applied to the face name
      and the returned CRC is ordinal case independent.
  */
  ON__UINT32 CRC32(
    bool bIgnoreFaceNameOrdinalCase
    ) const;

#if defined(ON_OS_WINDOWS_GDI)

  /*
  Description:
    Get the scale factors for converting heights beween 
    Windows device coordinates and Windows logical coordinates.

  Parameters:
    hdc - [in]
      Windows device context.
      The device context is used to get the conversion between device 
      and logical pixel heights. The Windows GDI functions
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.

    device_to_logical_scale - [out]
      logical_height = device_to_logical_scale*device_height

    logical_to_device_scale - [out]
      device_height = logical_to_device_scale*logical_height
    
  Returns:
    True if successful.
    False otherwise. In the returned scale factors are set to 1.0.
  */
  static bool GetWindowsDeviceToLogicalHeightScales(
    HDC hdc,
    double* device_to_logical_scale,
    double* logical_to_device_scale
  );

  /*
  Description:
    Convert a character height in points to a Windows LOGFONT lfHeight value (negative number).

    The mapping mode determines the length unit system for the returned value.
    
    
    The Windows convention is to use negative lfHeight values to specify
    font character heights and postive height values to specify font cell heights.

    font cell height = font acsent + font descent.

    font character height = Cell height - internal leading.

  Parameters:
    map_mode - [in]
      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.

    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.

    point_size - [in]
      Font character height in points (1 point = 1/72 inch = 25.4/72 millimeters). 
      In terms of font metrics, character height = ascent + descent - internal leading.

  Returns:
    LOGFONT lfHeight value. 

    This value is always negative.

    The absolute value of the returned value
    = character height
    = ascent + descent - internal leading
    For many common fonts, the "character height" is close to the distance
    from the bottom of a lower case g to the top of an upper case M. 
    The internal leading is space reseved for diacritical marks like the
    ring above the A in the UNICODE "LATIN LETTER A WITH RING" U+00C5 glyph.
    Character height is also known as the "em height". 
    Note that the "em height" is typically larger than the height of the
    letter M because "em height" inlcude descent.
  */
  static int WindowsLogfontCharacterHeightFromPointSize(
    int map_mode,
    HDC hdc,
    double point_size
  );

  /*
  Parameters:
    map_mode - [in]
      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.

    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.

    logfont_character_height - [in]
      This value must be a Windows LOGFONT character height in units
      determine from map_mode and hdc.  If you have a LOGFONT with postive
      lfHeight value, you must get the fonts TEXTMETRICS and subbr
    
  Returns:
    Character height in points (1 point = 1/72 inch).

   font character height = font ascent + font descent - font internal leading.

  Remarks:
    See ON_Font::PointSize() for information about point units,
    font character height, and font cell height.
  */
  static double PointSizeFromWindowsLogfontCharacterHeight(
    int map_mode,
    HDC hdc,
    int logfont_character_height
  );

  /*
  Description:
    Get a Windows LOGFONT character height 
    = -(TEXTMETRIC.tmAscent + TEXTMETRIC.tmDescent - TEXTMETRIC.tmLeading ) 
    as a negative integer.

  Parameters:
    map_mode - [in]
      The best results are obtained when map_mode = MM_TEXT and the hdc is
      correctly set for the context where the font is being rendered. Otherwise
      the loss of precision when length units system conversion scale factors 
      are applied and results are stored in int LOGFONT and TEXTMETRIC fields
      lead to discrepancies.

      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.

    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.

    logfont - [in]
      If logfont.lfHeight <= 0, then logfont.lfHeight is returned.
      If logfont.lfHeight > 0, then logfont face name, map_mode and hdc are 
      used to calculate the font's TEXTMETRICS tmInternalLeading value
      -((tm.tmAscent + tm.tmDescent) - tmInternalLeading) is returned.

  Returns:
    0: failure
    <0: Windows LOGFONT character height in units specified by map_mode and hdc.
  */
  static int WindowsLogfontCharacterHeight(
    int map_mode,
    HDC hdc,
    const LOGFONT& logfont
  );

  /*
  Description:
    Get a Windows LOGFONT cell height 
    = (TEXTMETRIC.tmAscent + TEXTMETRIC.tmDescent)
    as a positive integer.

  Parameters:
    map_mode - [in]
      The best results are obtained when map_mode = MM_TEXT and the hdc is
      correctly set for the context where the font is being rendered. Otherwise
      the loss of precision when length units system conversion scale factors 
      are applied and results are stored in int LOGFONT and TEXTMETRIC fields
      lead to discrepancies.

      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.

    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.

    logfont - [in]
      If logfont.lfHeight >= 0, then logfont.lfHeight is returned.
      If logfont.lfHeight < 0, then logfont face name, map_mode and hdc are 
      used to calculate the font's TEXTMETRIC and
      (tm.tmAscent + tm.tmDescent) is returned.

  Returns:
    0: failure
    >0: Windows LOGFONT cell height in units specified by map_mode and hdc.
  */
  static int WindowsLogfontCellHeight(
    int map_mode,
    HDC hdc,
    const LOGFONT& logfont
  );


  /*
  Description:
    Get a Windows text metrics.

  Parameters:
    map_mode - [in]
      The best results are obtained when map_mode = MM_TEXT and the hdc is
      correctly set for the context where the font is being rendered. Otherwise
      the loss of precision when length units system conversion scale factors 
      are applied and results are stored in int LOGFONT and TEXTMETRIC fields
      lead to discrepancies.

      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.

    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.

    logfont - [in]
      If logfont.lfHeight >= 0, then logfont.lfHeight is returned.
      If logfont.lfHeight < 0, then logfont face name, map_mode and hdc are 
      used to calculate the font's TEXTMETRIC and
      (tm.tmAscent + tm.tmDescent) is returned.

    textmetric - [out]

  Returns:
    0: failure
    >0: Windows LOGFONT cell height in units specified by map_mode and hdc.
  */
  static bool GetWindowsTextMetrics(
    int map_mode,
    HDC hdc,
    const LOGFONT& logfont,
    TEXTMETRIC& textmetric
  );

private:
  static HDC Internal_CreateWindowsLogfontDeviceContext();
  static void Internal_DeleteWindowsLogfontDeviceContext(
    HDC hdc
  );

public:  
  /*
  Description:
    Set ON_Font properties from a subset of the LOGFONT properties.
  Parameters:
    map_mode - [in]
      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.
    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.
    logfont - [in]
      These logfont properties are used to set the ON_Font.
        lfHeight (when dc is not zero),
        lfWeight
        lfItalic
        lfUnderline
        lfStrikeOut
        lfCharSet
        lfFaceName[LF_FACESIZE];
        All other LOGFONT properties are ignored.
  See Also:
    ON_Font::SetFromWindowsLogFontPartialComplete
  */
  bool SetFromWindowsLogFont(
    int map_mode,
    HDC hdc,
    const LOGFONT& logfont
    );

  /*
  Parameters:
    map_mode - [in]
      If map_mode is 0, then ::GetMapMode(hdc) is called to get the mapping mode.
      Otherwised, map_mode must identify a Windows mapping mode
      (MM_TEXT, MM_LOMETRIC, MM_HIMETRIC, MM_LOENGLISH, MM_HIENGLISH, M_TWIPS).
      If map_mode = MM_TEXT (1), then hdc is used as described in the hdc parameter.
    hdc - [in]
      Windows device context.
      If map_mode is set and not MM_TEXT, then hdc is ignored.
      Otherwise the device context is used to get the mapping mode ( GetMapMode(hdc) ). 
      If the mapping mode is MM_TEXT, then the additional device context values
      GetDeviceCaps(hdc, LOGPIXELSY) and conversion between device and logical pixel heights
      DPtoLP(hdc,...) and LPtoDP(hdc,...) are used.
  Returns:
    A Windows LOGFONT with propeties copied from this ON_Font.
    If WindowsLogFontIsComplete() is true, then all LOGFONT properties
    are copied from the ON_Font. 
    If WindowsLogFontIsComplete() is false, then the LOGFONT lfHeight,
    lfWidth, lfEscapement, lfOrientation, lfClipPrecision, lfQuality,
    lfPitchAndFamily, and lfOutPrecision properties are set to ON_Font
    default values.
  */
  const LOGFONT WindowsLogFont(
    int map_mode,
    HDC hdc
    ) const;

  const MAT2 WindowsFontMat2() const;

#endif

#if defined (ON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE)
  bool SetFromAppleFont (NSFont* apple_font);
  NSFont* AppleFont() const;
#endif

  bool SetFromAppleFontName(
    const wchar_t* apple_font_name
    );
  
  const ON_wString& AppleFontName() const;
  
  const wchar_t* AppleFontNameAsPointer() const;

  bool SetFromFontDescription(
    const wchar_t* font_description
    );

  bool SetFromFontDescription(
    const wchar_t* font_description,
    const wchar_t* apple_font_name
    );

  /*
  Description:
    Tests an object to see if its data members are correctly
    initialized.
  Parameters:
    text_log - [in] if the object is not valid and text_log
        is not nullptr, then a brief englis description of the
        reason the object is not valid is appened to the log.
        The information appended to text_log is suitable for 
        low-level debugging purposes by programmers and is 
        not intended to be useful as a high level user 
        interface tool.
  Returns:
    @untitled table
    true     object is valid
    false    object is invalid, uninitialized, etc.
  */
  bool IsValid( ON_TextLog* text_log = nullptr ) const;

  void Dump( ON_TextLog& ) const; // for debugging


  void DumpFreeType(
    ON_TextLog& text_log
  ) const;

  static void DumpFreeTypeFace(
    ON__UINT_PTR free_type_face_ptr,
    ON_TextLog& text_log
  );

#if defined(ON_OS_WINDOWS_GDI)
  static void DumpLogfont(
    const LOGFONT* logfont,
    ON_TextLog& text_log
  );
#endif

  // serialize definition to binary archive
  bool Write( ON_BinaryArchive& ) const;

  // restore definition from binary archive
  bool Read( ON_BinaryArchive& );

  // V6 separated the V5 ON_Font into ON_TextStyle and ON_Font.
  bool WriteV5(
    int V5_font_index,
    ON_UUID V5_font_id,
    ON_BinaryArchive& 
    ) const;

  // V6 separated the V5 ON_Font into ON_TextStyle and ON_Font.
  bool ReadV5( 
    ON_BinaryArchive&, 
    int* V5_font_index,
    ON_UUID* V5_font_id
    );

  /*
  Returns:
        0: This is not a managed font.
        1: This is the managed font ON_Font::Default.
     >= 2: This is a managed font other than ON_Font::Default.
  Remark:
    For managed fonts other than ON_Font::Default, the value of RuntimeSerialNumber()
    typically varies between instances of the same application. 
    Different platforms and application versions may use different font faces for ON_Font::Default.
    If an ON_Font is a managed font, then RuntimeSerialNumber() and ManagedFontSerialNumber()
    are identical. If an ON_Font is not a managed font, then RuntimeSerialNumber() is zero.
  */
  unsigned int RuntimeSerialNumber() const;

  /*
  Description:
    Two ON_Font classes reference the same platform font and create identical glyphs 
    if and only if the have the same ManagedFontSerialNumber().
  Returns:
        0: This font is unset.
     >= 1: Serial number of the managed font with the same characteristics.
  Remark:
    For managed fonts other than ON_Font::Default, the value of ManagedFontSerialNumber()
    typically varies between instances of the same application. 
    Different platforms and application versions may use different font faces 
    for ON_Font::Default.
    If an ON_Font is a managed font, then RuntimeSerialNumber() and ManagedFontSerialNumber()
    are identical. If an ON_Font is not a managed font, then RuntimeSerialNumber() is zero.
  */
  unsigned int ManagedFontSerialNumber() const;

  //////////////////////////////////////////////////////////////////////
  //
  // Interface
  
  enum WindowsConstants : unsigned char
  { 
    // Values used to set Windows LOGFONT fields.
#if defined(ON_OS_WINDOWS_GDI)
    logfont_ansi_charset = ANSI_CHARSET,
    logfont_default_charset = DEFAULT_CHARSET,                // LOGFONT.lfCharSet
    logfont_symbol_charset = SYMBOL_CHARSET,                  // LOGFONT.lfCharSet
    logfont_out_precis = OUT_TT_ONLY_PRECIS,                  // LOGFONT.lfOutPrecision

    // 2017-07-27, Brian Gillespie
    // Changed ON_Font::WindowsConstants::logfont_quality from ANTIALIASED_QUALITY to DEFAULT_QUALITY.
    // This makes it so that ON_Font conversion to LOGFONT results in a good-looking font when rendered by MFC.
    // With lfQuality set to ANTIALIASED_QUALITY, the font looks crummy - probably because all the rest of the fonts
    // are being rendered today with CLEARTYPE_QUALITY. Letting Windows decide what to do is probably better.
    logfont_quality = DEFAULT_QUALITY,                        // LOGFONT.lfQuality

    logfont_pitch_and_family = (DEFAULT_PITCH | FF_DONTCARE), // LOGFONT.lfPitchAndFamily
#else
    // The values below are identical to the ones above and
    // are used to insure code compiles for Apple and other
    // platforms.  
    logfont_ansi_charset = 0,
    logfont_default_charset = 1,
    logfont_symbol_charset  = 2,
    logfont_out_precis = 7,
    logfont_quality = 4,
    logfont_pitch_and_family = 0
#endif
  }; 

  /*
  Parameters:
    face_name - [in]    
  Returns:
    If the code is running on Windows:
      The appropriate value of LOGFONT.lfCharSet for the input facename.
    If the code is not running on Windows:
      ON_Font::WindowsConstants::logfont_default_charset.
  */
  static unsigned char WindowsLogfontCharSetFromFaceName(
    const wchar_t* face_name 
    );
  
  // miscellaneous constants use to initialize Windows LOGFONT fields
  enum Constants: int
  { 
    // 1995 - 2015:
    // Windows fonts have variations in glyph size, design and kerning
    // for different point sizes. Text in Rhino is generally
    // placed around geometry and the relative spatial
    // relationships between the text and the geometry must 
    // remain constant on all devices and at all "zoom" levels.
    // We have to choose a point size and then apply appropriate
    // scaling during display, printing, and in other rendering
    // calculations.  After many experiments and 20 years of commercial use, 
    // (1995-2015) we have found 256 works best.
    // This value is used on all platforms because the calculations
    // it is used in occur on all platforms.  These calculations must return 
    // consistent results so models exchanged between platforms maintain
    // spatial relationships between text and geometry.
    //
    // 2017:
    // (switching to freetype)
    // The value ON_Font::Constants::AnnotationFontCellHeight is used to define
    // "opennurbs normalized font coordinates".  The scale
    // ((double)ON_Font::Constants::AnnotationFontCellHeight)/(font definition grid height)
    // is used to convert bounding information and outlines from a native
    // font definition to opennurbs normalized font coordinates.
    // Many TrueType fonts have font definition grid height = 2048.
    // Many PostScript fonts have font definition grid height = 1000.
    AnnotationFontCellHeight = 256,  // LOGFONT.lfHeight value

    // ON_Font::Constants::metric_char is the unicode code point value
    // for the glpyh used to calculate critical glyph metrics.
    // It must be an 'I' or 'H', but we have not tested 'H'.
    // There are problems with any other upper case latin letter in common fonts.
    // In particular, the standard 'M' does not work.
    // We have used 'I' for 22 years (1995 - 2017).
    // This value is used on all platforms because the calculations
    // it is used in occur on all platforms. These calculations must return 
    // consistent results so models exchanged between platforms maintain
    // spatial relationships between text and geometry.
    MetricsGlyphCodePoint = 'I'
  }; 

  /*
  Description:
    Get a text description of the font.
  Parameters:
    font_description - [out]
  Returns:
    A pointer to the font description string stored in the font_description parameter.
  */
  const ON_wString& FontDescription() const;

  /*
  Description:
    Get a text description of the font.
  Parameters:
    font_description - [out]
  Returns:
    A pointer to the font description string stored in the font_description parameter.
  */
  const wchar_t* FontDescriptionAsPointer() const;
  
  ON_DEPRECATED_MSG("Use ON_FontMetrics::DefaultLineFeedRatio")
  double LinefeedRatio() const;

  /*
  Returns:
    Normalized font metrics.

  Remarks:
    Font metric "normalized" units are comparable between different fonts.
    Normalized font metrics exist so that code that positions glyphs from 
    multiple fonts does not have to take the unit system and resolution used
    in the design of each font.
    In opennurbs, much of this code that positions glyphs is located in ON_Annotation,
    ON_TextContent, and ON_TextRun member functions and is used when rendering 
    annotation objects.

    Fonts can be designed and defined at different resolutions and
    relative scaling is necessary when text contains glyphs from 
    fonts desinged at different grid resolutions.  For example, 
    TrueType font grid with and height is often 1024x1024 or 
    2048x2014, OpenType grids are often 1000x1000, and PostScript 
    grids are often 1000x1000. Opennurbs "font units" are the units
    the font was designed in.

    Long ago, opennurbs and Rhino used only Windows TrueType fonts
    and ran only in Microsoft Windows.  During this era,
    the "normalized units" were for a Windows LOGFONT created
    with lfHeight = ON_Font::Constants::AnnotationFontCellHeight.
  
    Currently opennurbs and Rhino work on Microsoft Windows and Apple
    platforms and use FreeType to access font information. When a font 
    is not "tricky", the "font design" units are the the units FreeType
    uses when a font is loaded with FT_LOAD_NO_SCALE.
  
    When working with fonts and glyhphs in opennurbs and Rhino, 
    SDK users almost always want to use normalized font and glyph metrics.
  */
  const ON_FontMetrics& FontMetrics() const;

  /*
  Description:
    This function is for expert users doing something complicated.
  Returns:
    Font metrics read directly from the font definition with no or minimal
    scaling.
  Remarks:
    See ON_Font.FontMetrics() documentation for important information
    about the differnce bewteen normalized and font unit metrics.
  */
  const ON_FontMetrics& FontUnitFontMetrics() const;

  /*
  Returns:
    scale to apply when converting from a FT_LOAD_NO_SCALE FreeType
    glyph metric or outline to normalized opennurbs font coordinates.    
  */
  double FontUnitToNormalizedScale() const;
  
  /*
  Returns:
    scale to apply when converting from a FT_LOAD_NO_SCALE FreeType
    glyph metric or outline to normalized opennurbs font coordinates.
  */
  double NormalizedToFontUnitScale() const;

  /*
  Returns:
    Font character height in points (1 point = 1/72 inch).

    See the remarks for a defintion of "character height".

  Remarks:
    A "point" is a length unit system.
    1 point = 1/72 inch = 25.4/72 millimeters. 

    Typically, fonts are designed for maximum clarity when the rendered 
    character height is close to PointSize().

    font cell height = font ascent + font descent.

    font character height = font cell height - font internal leading.

    For fonts designed for languages that use latin letters, it is common for 
    the character height to be equal to or a little larger than the distance 
    from the bottom of a lower case g to the top of an upper case M. 
    The character height is also called the "em hieght".

    Font internal leading is the space above typical capital latin letters
    that is reseved for diacritical marks like the ring above the A in 
    the UNICODE "LATIN LETTER A WITH RING" U+00C5 glyph (Angstrom symbol).
  */
  double PointSize() const;

  /*
  Parameters:
    point_size - [in]
      font character height in point units.

  Remarks:
    See the remarks section ON_Font::PointSize() for more information
    about point units and character height.
  */
  bool SetPointSize(
    double point_size
  );

  bool SetFontFaceName( 
    const wchar_t* face_name
    );
  const wchar_t* FontFaceName() const;

  ON_Font::Weight FontWeight() const;

  int WindowsLogfontWeight() const;
  int AppleWeightOfFont() const;
  double AppleFontWeightTrait() const;

  bool SetFontWeight(
    ON_Font::Weight font_weight
    );

  bool SetWindowsLogfontWeight(
    int windows_logfont_weight
  );

  bool SetAppleWeightOfFont(
    int apple_weight_of_font
  );

  bool SetAppleFontWeightTrait(
    double apple_font_weight_trait
  );

private:
  bool Internal_SetFontWeightTrio(
    ON_Font::Weight font_weight,
    int windows_logfont_weight,
    double apple_font_weight_trait,
    bool bUpdateFontDescription
    );

public:
  /*
  Description:
    User interfaces that want to behave as if there are 3 font weights,
    light < normal < < bold, can use the functions
    ON_Font.IsLight(),
    ON_Font.IsNormalWeight(),
    ON_Font.IsBold(),
    to query font weight ranges.
  Returns:
    True if FontWeight() is lighter than ON_Font::Weight::Normal
  */
  bool IsLight() const;

  /*
  Description:
    User interfaces that want to behave as if there are 3 font weights,
    light < normal < < bold, can use the functions
    ON_Font.IsLight(),
    ON_Font.IsNormalWeight(),
    ON_Font.IsBold(),
    to query font weight ranges.
  Returns:
    True if FontWeight() is ON_Font::Normal or ON_Font::Weight::Medium
  */
  bool IsNormalWeight() const;

  /*
  Description:
    User interfaces that want to behave as if there are 3 font weights,
    light < normal < < bold, can use the functions
    ON_Font.IsLight(),
    ON_Font.IsNormalWeight(),
    ON_Font.IsBold(),
    to query font weight ranges.
  Returns:
    True if heavier than ON_Font::Weight::Medium.
  */
  bool IsBold() const;
  
  ON_Font::Style FontStyle() const;

  bool SetFontStyle(
    ON_Font::Style font_style
    );

  /*
  Returns:
    true if FontStyle() is ON_Font::Style::Italic.
    false if FontStyle() is ON_Font::Style::Upright or .ON_Font::Style::Oblique.
  */
  bool IsItalic() const;

  /*
  Returns:
    true if FontStyle() is ON_Font::Style::Upright.
    false if FontStyle() is ON_Font::Style::Italic or .ON_Font::Style::Oblique.
  */
  bool IsUpright() const;

  /*
  Returns:
    true if FontStyle() is ON_Font::Style::Oblique.
    false if FontStyle() is ON_Font::Style::Upright or .ON_Font::Style::Italic.
  */
  bool IsOblique();

  
  ON_Font::Stretch FontStretch() const;

  bool SetFontStretch(
    ON_Font::Stretch font_stretch
    );

  bool IsUnderlined() const;
  bool SetUnderlined(
    bool bUnderlined 
    );

  bool IsStrikethrough() const;
  bool SetStrikethrough( 
    bool bStrikethrough
    );

  unsigned char LogfontCharSet() const;
  
  bool SetLogfontCharSet(
    unsigned char logfont_charset
    );

  ON_DEPRECATED_MSG("Use FontMetrics().AscentOfI()")
  int HeightOfI() const;

  ON_DEPRECATED_MSG("Use FontMetrics().LineSpace()")
  int HeightOfLinefeed() const;
  
  ON_DEPRECATED_MSG("Use FontMetrics().GlyphScale()")
  double HeightScale(double text_height) const;
  
  ON_DEPRECATED_MSG("Use FontMetrics().StrikeoutThickness()")
  int GetStrikeoutSize() const;

  ON_DEPRECATED_MSG("Use FontMetrics().StrikeoutPosition()")
  int GetStrikeoutPosition() const;

  ON_DEPRECATED_MSG("Use FontMetrics().UnderscoreThickness()")
  int GetUnderscoreSize() const;


  ON_DEPRECATED_MSG("Use FontMetrics().UnderscorePosition()")
  int GetUnderscorePosition() const;

  /*
  Returns:
    A SHA-1 hash of all font characteristics, including platform specific settings. 
    Two fonts have identical font characteristics, if and only if they have identical 
    FontCharacteristicsHash() values.

  Example:
        ON_Font f1 = ...
        ON_Font f2 = ...
        if ( f1.FontCharacteristicsHash() == f2.FontCharacteristicsHash() )
        {
          // f1 and f2 have identical font characteristics
        }
        else
        {
          // f1 and f2 have different font characteristics
        }
  */
  const class ON_SHA1_Hash& FontCharacteristicsHash() const;

private:

public:

  /*
  Description:
    Compares the font weight, style, stretch, underline, strikethrough, linefeed_ratio
    and facename characteristics. 
  Returns:
    -1: a characteristics < b characteristics
      0: a characteristics = b characteristics
    +1: a characteristics > b characteristics
  Remarks:
    This is a legacy function and to preserve past behavior, some platform specific 
    characteristics are not checked. 
    Use FontCharacteristicsHash() when every characteristic needs to be compared.
  */
  static int CompareFontCharacteristics(
    const ON_Font& a,
    const ON_Font& b
    );

  /*
  Description:
    Compares the font weight, style, stretch, underline, strikethrough, linefeed_ratio
    and facename characteristics.
  Returns:
    0 == ON_Font::CompareFontCharacteristics(a,b).
  Remarks:
    This is a legacy function and to preserve past behavior, some platform specific 
    characteristics are not checked. 
    Use FontCharacteristicsHash() when every characteristic needs to be compared.
  */
  static bool EqualFontCharacteristics(
    const ON_Font& a,
    const ON_Font& b
    );

private:
  friend class ON_ManagedFonts;

  //////////////////////////////////////////////////////////////////////////////////
  //
  // The "font glpyh definition" parameters completely determine the appearance
  // of font glyphs.
  //
  // If all "font glpyh definition" parameters have identical values, 
  // text rendered using those fonts will look identical.
  //
  // If two fonts have a "font glpyh definition" parameter with different values,
  // text rendered using those fonts will not look identical.
  //
  // BEGIN "font glpyh definition" parameters:
  //

  // The font ON_Font::Default has m_runtime_serial_number = 1.
  // Managed fonts have m_runtime_serial_number >= 1.
  // Unmanaged fonts have m_runtime_serial_number = 0;
  static unsigned int __runtime_serial_number_generator;
  const unsigned int m_runtime_serial_number = 0;
  
  int m_windows_logfont_weight = 400; // 100 <= m_windows_logfont_weight <= 1000
  double m_point_size = 0.0;
  double m_apple_font_weight_trait = 0.0; // = Apple NSFontWeightTrait value -1.0 <= m_apple_font_weight < 1.0, 0.0 = "normal"
  ON_Font::Weight  m_font_weight = ON_Font::Weight::Normal;

  ON_Font::Style   m_font_style = ON_Font::Style::Upright;    // m_font_style corresponds to Windows LOGFONT.lfItalic field
  ON_Font::Stretch m_font_stretch = ON_Font::Stretch::Medium;
  bool             m_font_bUnderlined = false;                // Same as Windows LOGFONT.lfUnderlined
  bool             m_font_bStrikethrough = false;             // Same as Windows LOGFONT.lfStrikeOut
  unsigned char    m_logfont_charset = ON_Font::WindowsConstants::logfont_default_charset;

private:
  unsigned char    m_reserved_char1 = 0;

private:
  // If m_bCompleteLOGFONT is true, then the m_LOGFONT_* values are set.
  // If m_bCompleteLOGFONT is false, then the m_LOGFONT_* values are undefined and must be ignored.
  /*
  unsigned char m_bCompleteLOGFONT = 0;
  unsigned char m_LOGFONT_lfClipPrecision = 0;
  unsigned char m_LOGFONT_lfQuality = 0;
  unsigned char m_LOGFONT_lfPitchAndFamily = 0;
  unsigned char m_LOGFONT_lfOutPrecision = 0;
  int m_LOGFONT_lfHeight = 0;
  int m_LOGFONT_lfWidth = 0;
  int m_LOGFONT_lfEscapement = 0;
  int m_LOGFONT_lfOrientation = 0;
  */

private:
  enum : int
  {
    face_name_capacity = 32
  };
  // https://en.wikipedia.org/wiki/List_of_typefaces_included_with_Microsoft_Windows
  wchar_t          m_face_name[ON_Font::face_name_capacity+2];  // same as Windows LOGFONT.lfFaceName 

  // If m_linefeed_ratio needs to become a variable or variables are required for tracking or stretch,
  // these fields will be used.
  double m_reserved_double_0 = 0.0;
  double m_reserved_double_1 = 0.0;
  //
  // END "font glpyh definition" parameters:
  //
  //////////////////////////////////////////////////////////////////////////////////

private:
  // https://support.apple.com/en-us/HT201375
  // https://en.wikipedia.org/wiki/List_of_typefaces_included_with_OS_X
  ON_wString m_apple_font_name;

private:
  ON_wString m_font_description;

private:
  // A sha1 hash of all font characteristics.
  // This value is set using lazy evaluation.
  // A zero digest indicates it is not set.
  mutable ON_SHA1_Hash m_font_characteristics_hash;

private:
  bool ModificationPermitted(
    const char* function_name,
    const char* file_name,
    int line_number
    ) const;

private:
  void Internal_SetFontDescription();


private:
  //////////////////////////////////////////////////////////////////////////////////
  //
  // BEGIN global font glyph cache interface
  //
  // There is a single font glyph cache for each managed font.
  // Fonts that are not managed use a glyph cache from a managed font.
  // This make functions like ON_Font.FindGlyph() efficient and reliable.
  //
  void DestroyFontGlyphCache();
  class ON_FontGlyphCache* FontGlyphCache(
    bool bCreateIfMissing
    ) const;
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: '...std::shared_ptr<class ON_FontGlyphCache>...' 
  //         needs to have dll-interface to be used by clients of class 'ON_Font'
  // m_font_glyph_cache is private and all code that manages m_font_glyph_cache is explicitly implemented in the DLL.
private:
  mutable std::shared_ptr<class ON_FontGlyphCache> m_font_glyph_cache;
#pragma ON_PRAGMA_WARNING_POP
  //
  // END global font cache interface
  //
  //////////////////////////////////////////////////////////////////////////////////

private:
  // Only managed fonts have a non-null m_free_type_face face.
  mutable class ON_FreeTypeFace* m_free_type_face = nullptr;

private:
  ON__UINT_PTR m_reserved_ptr = 0;

public:
  // Returns free type glyph index (or nonzero equivalent) if glyph is defined fo the glyph.CodePoint() in glyph.Font()
  // and glyph_box is set.
  typedef ON__UINT_PTR (*ON_GetGlyphMetricsFuncType)(
    const class ON_Font* font, 
    ON__UINT32 unicode_code_point,
    class ON_TextBox& font_unit_glyph_box
    );

  typedef void (*ON_GetFontMetricsFuncType)(
    const ON_Font* font,
    ON_FontMetrics& font_unit_font_metrics
    );

  static void SetCustomMeasurementFunctions(
    ON_GetGlyphMetricsFuncType measureGlyphFunc,
    ON_GetFontMetricsFuncType metricsFunction
  );

private:
  static ON_GetGlyphMetricsFuncType Internal_CustomGetGlyphMetricsFunc;
  static ON_GetFontMetricsFuncType Internal_CustomGetFontMetricsFunc;

public:
  static void GetRunBounds(
    const ON_Font& font,
    const wchar_t* text,
    double fontSizePixels,
    ON::TextHorizontalAlignment horizontalAlignment,
    ON::TextVerticalAlignment verticalAlignment,
    ON_2dPoint& boundsMin,
    ON_2dPoint& boundsMax, 
    int& lineCount
  );
};

class ON_CLASS ON_TextBox
{
public:
  ON_TextBox() = default;
  ~ON_TextBox() = default;
  ON_TextBox(const ON_TextBox&) = default;
  ON_TextBox& operator=(const ON_TextBox&) = default;

  ON_TextBox(
    ON_2dPoint bbmin,
    ON_2dPoint bbmax
  );

  /*
  Returns:
    true if bounding box is set.
  */
  bool IsSet() const;

  static const ON_TextBox Scale(
    const ON_TextBox& text_box,
    double scale
  );

  /*
  Returns:
    A text box with m_bbmin, m_bbmax, m_max_basepoint are translated by delta.
    m_advance is not changed.
  */
  static const ON_TextBox Translate(
    const ON_TextBox& text_box,
    const ON_2dVector& delta
  );

  static const ON_TextBox Translate(
    const ON_TextBox& text_box,
    const ON_2dex& delta
  );

  /*
  Parameters:
    lhs - [in]
      lhs.m_advance is ignored
    rhs - [in]
      rhs.m_advance is ignored
  Returns:
    Returned m_bbmin, m_bbmax, m_max_basepoint are the union of the lhs and rhs bounding box.
    Returned m_advance = (0,0)
  */
  static const ON_TextBox Union(
    const ON_TextBox& lhs,
    const ON_TextBox& rhs
  );

public:
  static const ON_TextBox Unset;

public:
  // Default units are with respect to a LOGFONT height = ON_Font::Constants::AnnotationFontCellHeight.
  // and (0,0) is the font glyph base point.
  //
  // Application of a Scale or Translate will change distance units and base point.
  //

  // Tight bounding box of the rendered glyphs.
  ON_2dex m_bbmin = ON_2dex::Unset;
  ON_2dex m_bbmax = ON_2dex::Unset;

  // m_max_basepoint.i = maximum horizontal delta in any line. Increases to the right, decreases to the left.
  // m_max_basepoint.i = vertical delta to basline of bottom line. Increases upward, decreases downward.
  ON_2dex m_max_basepoint = ON_2dex::Zero;

  // m_advance is a vector that specifies where the basepoint should be moved
  // to after the text is rendered. m_advance.i and m_advance.j are is always >= 0.  
  // When glyphs are rendered right to left (Arabic and Hebrew being examples)
  // or bottom to top, the rendering code must apply the correct sign. One reason
  // is that Arabic and Hebrew text can be mixed with latin and Cyrillic text
  // and text rendering is much more complicated than a signed advance can handle.
  // Another is that the sign of y associated with "up" is sometimes positive and sometimes negative.
  // ON_TextBox::Translate does not modify the vector m_advance. 
  // ON_TextBox::Union ignored input advance values and returns a box with advance = (0,0).
  // 0 <= m_advance.i will be <= m_max_basepoint.i.
  ON_2dex m_advance  = ON_2dex::Zero;
};

class ON_CLASS ON_FontGlyphOutlinePoint
{
public:
  ON_FontGlyphOutlinePoint() = default;
  ~ON_FontGlyphOutlinePoint() = default;
  ON_FontGlyphOutlinePoint(const ON_FontGlyphOutlinePoint&) = default;
  ON_FontGlyphOutlinePoint& operator= (const ON_FontGlyphOutlinePoint&) = default;
public:
  enum class ContourPointType : ON__UINT8
  {
    Unset = 0,
    MoveTo = 1,
    LineTo = 2,
    
    // quadratic bezier (degree=2, order=3) control point.
    QuadraticBezierPoint = 3,

    // cubic bezier (degree=3, order=4) control point.
    CubicBezierPoint = 4,

    // a line segment added to close an open contour.
    // This is common. It does not indicate the glyph is a single stroke glyph.
    LineToCloseContour = 5
  };

  static ON_FontGlyphOutlinePoint::ContourPointType ContourPointTypeFromUnsigned(unsigned contour_point_type_as_unsigned);

  static const ON_FontGlyphOutlinePoint Unset;

public:
  ON_FontGlyphOutlinePoint::ContourPointType m_point_type = ContourPointType::Unset; 
  ON__UINT8 m_bToPoint = 0; // 1 if the point is a move to, line to, or the start or end of a bezier segment.
                            // 0 otherwise
  ON__UINT16 m_contour_index = 0; // 0 = unset. The first contour has m_contour_index = 1.
  ON_2iPoint m_point = ON_2iPoint::Unset;
};


/*
  The best way to get a useful ON_FontGlyph is to call
  ON_Font.CodePointGlyph(unicode_code_point)
*/
class ON_CLASS ON_FontGlyph 
{
public:
  /*
    The best way to get a useful ON_FontGlyph is to call
    ON_Font.CodePointGlyph(unicode_code_point)
  */
  ON_FontGlyph() = default;
  ~ON_FontGlyph() = default;
  ON_FontGlyph(const ON_FontGlyph& src);
  ON_FontGlyph& operator=(const ON_FontGlyph& src);


  /*
    If the font and code point are valid, constructs an unmanaged 
    glyph with the specified font and code point.
    The glyph box is not set.
  */
  ON_FontGlyph(
    const ON_Font* font,
    ON__UINT32 code_point
  );

public:
  static const ON_FontGlyph Unset;

  const ON_Font* Font() const;
  
  const ON__UINT32 CodePoint() const;

  bool IsEndOfLineCodePoint() const;

  static bool IsEndOfLineCodePoint(
    ON__UINT32 unicode_code_point
  );

  static bool IsCarriageReturnAndLineFeed(
    ON__UINT32 unicode_code_point,
    ON__UINT32 next_unicode_code_point
  );

  /*
  Returns:
    Glyph box in opennurbs normalized font coordinates.
  */
  const ON_TextBox& GlyphBox() const;

  /*
  Returns:
    Font unit glyph box. 
  Remarks:
    Must be used with ON_Font::FontUnitFontMetrics() and a single font to obtain useful results.
    You are probably better of using normalized font coordinates in a ON_FontGlyph.GlyphBox().
  */
  const ON_TextBox& FontUnitGlyphBox() const;

  static int CompareCodePointAndFont(
    ON_FontGlyph& lhs,
    ON_FontGlyph& rhs
  );

  /*
  Parameters:
    text - [in]
      Null terminated wchar_t string.
    font - [in]
      The font used to render the glyphs.
    unicode_CRLF_code_point - [in]
      If unicode_CRLF_code_point is a valid unicode code point,
      then consecutive carriage return line feed pairs are converted
      to a single glyph with code point = unicode_CRLF_code_point.

      ON_UnicodeCodePoint::ON_LineSeparator is a good choice when you want to  
      condense carriage return line feed pairs to a single unambiguous code point.

      ON_UnicodeCodePoint::ON_InvalidCodePoint is a good choice when you want to
      preserve carriage return line feed pairs as two separate glyphs.

    glyph_list - [out]
      Note that glyph_list.Count() is often different than the 
      length of the text string or the number of unicode codepoints
      in the decoded text. 
      Adjacent carriage return and line feed codepoints are 
      converted to single a hard end of line.
      All trailing end of line code points are removed from text.
      Invalid unicode encoding sequences are replaced with
      ON_UnicodeCodePoint::ReplacementCharacter glyphs.

    text_box - [out]
      tight bounding boxt of text extents.
      text_box.m_advance.i = maximum of all line horizontal advance values..
      text_box.m_advance.j = vertical advance to baseline of last line
      If if the font height
      is ON_Font::Constants::AnnotationFontCellHeight. If you will render the font
      at a different height from ON_Font::Constants::AnnotationFontCellHeight, then
      use ON_TextBox::Scale as follows:
      ON_TextBox scaled_box 
        = ON_TextBox::Scale(
            text_box,
            (font render height)/((double)ON_Font::Constants::AnnotationFontCellHeight)
        );
  Return:
    number of lines of text or 0 if input is not valid or text is empty.
  */
  static int GetGlyphList
  (
    const wchar_t* text,
    const ON_Font* font,
    ON__UINT32 unicode_CRLF_code_point,
    ON_SimpleArray<const ON_FontGlyph*>& glyph_list,
    ON_TextBox& text_box
  );

  static int GetGlyphList
  (
    size_t code_point_count,
    ON__UINT32* code_points,
    const ON_Font* font,
    ON__UINT32 unicode_CRLF_code_point,
    ON_SimpleArray<const ON_FontGlyph*>& glyph_list,
    ON_TextBox& text_box
  );

  /*
  Parameters:
    font - [in]
      The font used to render the glyphs.
    text_box - [out]
      tight bounding boxt of text extents.
      text_box.m_advance.i = maximum of all line horizontal advance values..
      text_box.m_advance.j = vertical advance to baseline of last line
      If if the font height
      is ON_Font::Constants::AnnotationFontCellHeight. If you will render the font
      at a different height from ON_Font::Constants::AnnotationFontCellHeight, then
      use ON_TextBox::Scale as follows:
      ON_TextBox scaled_box 
        = ON_TextBox::Scale(
            text_box,
            (font render height)/((double)ON_Font::Constants::AnnotationFontCellHeight)
        );
  Return:
    number of lines of text or 0 if input is not valid or text is empty.
  */
  static int GetGlyphListBoundingBox
  (
    const wchar_t* text,
    const ON_Font* font,
    ON_TextBox& text_box
  );

  static int GetGlyphListBoundingBox
  (
    size_t code_point_count,
    ON__UINT32* code_points,
    const ON_Font* font,
    ON_TextBox& text_box
  );

  /*
  Description:
    Sets the font and code point and unsets every other property including the
    glyph box and substitute information.
  Parameters:
    font - [in]
    code_point - [in]
  */
  bool SetCodePoint(
    const ON_Font* font,
    ON__UINT32 code_point
  );

  /*
  Returns:
    True if the unicode code point and font are set
  */
  bool CodePointIsSet() const;

  /*
  Returns: 
    true if this is a managed instance.
    Managed instances persist for the lifetime of the application
    and the pointer can be safely saved and referenced at any time.
  */
  bool IsManaged() const;

  /*
  Returns:
    If this->CodePointIsSet() is true, then a persistent pointer
    to a managed glyph with the same code point and font is returned.
    Otherwise nullptr is returned.
  */
  const ON_FontGlyph* ManagedGlyph() const;

  /*
  Parameters:
    bUseReplacementCharacter - [in]
      When this->CodePointIsSet() is true, 
      and bUseReplacementCharacter is true,
      and no reasonable glyph definition exists,
      and no substitued  is available, 
      then the replacement character glyph for UNICODE code point
      ON_UnicodeCodePoint::ON_ReplacementCharacter (U+FFFD) will be returned.

  Returns:
    A managed glyph that can be used to render "this".
    If this->CodePointIsSet() is false, nullptr is returned.
    If this->CodePointIsSet() is true, the returned glyph may
    have a different font and code point when the current 
    computer requires font or glyph substitution to draw
    the glyph.  When the current platform cannot render this,
    nullptr or the replacement glyph is returned depending on
    the value of bUseReplacementCharacter.

  See Also:
    ON_FontGlyph.SubstituteGlyph().
  */
  const ON_FontGlyph* RenderGlyph(
    bool bUseReplacementCharacter
    ) const;

  /*
  Returns:
    If this is a managed glyph or a copy of a managed glyph, 
    and a substitute font or code point is used to render the glyph, 
    then the substitue is returned.
    In all other cases, nullptr is returned.
  See Also:
    ON_FontGlyph.RenderGlyph().
  */
  const ON_FontGlyph* SubstituteGlyph() const;

  /*
  Parameters:
    bIncludeCharMaps - [in]
      If true, then char information is printed.
  */
  void Dump(
    bool bIncludeCharMaps,
    ON_TextLog& text_log
  ) const;

  /*
  Description:
    This is a debugging tool to test the code that starts with a font and 
    Unicode code point and and finds a glyph in the font definition for 
    that code point.
  Parameters:
    text_log - [in]
      If text_log is not nullptr, then diagnostic messages are sent to this log.
  Returns:
    True: 
      No errors were found. Every available charmap either returned the same glyph id
      that FontGlyphId() function returns or had no glyph id for this code point.
    False:
      Inconsistent results were returned from different charmaps.
  Remarks:
    If a font or charmap is known to contain a bug and that bug is
    handled by opennurbs, then true is returned and a message is printed
    to the log.
  */
  bool TestFaceCharMaps(
    ON_TextLog* text_log
  ) const;

public:

  const ON__UINT_PTR FreeTypeFace() const;

  /*
  Returns:
    Font glyph id.
  Remarks:
    The glyph id depends on the font and is assigned by the font designer.
    In particular the font glyph id for the same Unicode code point
    often varies from font to font. In a font, it is often the case that
    multiple Unicode code points map to the same glyph. For example,
    space an non-breaking space typically map to the same font glyph id.
  */
  const ON__UINT_PTR FontGlyphId() const;

  /*
  Description:
    Get glyph contours as NURBS curves.
  Parameters:
    bSingleStrokeFont - [in]
      If true, open contours will not be closed by adding a line segment.
    text_height - [in]
      If > 0, ouptut curves, bounding box, and advance vector are scaled 
      so that a capital latin letter I would have a height of text_height.
      Otherwise, no scaling is applied to the output curves, bounding box,
      and advance vector.
      Pass 0.0 or in this->Font()->HeightOfI() to get the contours to be in opennurbs
      normalized font coordinates.
      Pass ON_UNSET_VALUE to get the contours to be in native font definition units.
      All other values < 0 are treated as 0.0.
    glyph_contours - [out]
    glyph_bbox - [out]
      glyph bounding box.
    glyph_advance - [out]
      glyph_advance->x = horizontal advance to apply when rendering glyphs horizontally.
      A positive horizontal advance indicates advance to the right.
      glyph_advance->y = vertical advance to apply when rendering glyphs vertically.
      A positive vertical advance indicates advance downwards.
  */
  bool GetGlyphContours(
    bool bSingleStrokeFont,
    double text_height,
    ON_ClassArray< ON_SimpleArray< ON_Curve* > >& glyph_contours,
    ON_BoundingBox* glyph_bbox,
    ON_3dVector* glyph_advance
  ) const;

  static bool GetStringContours(
    const wchar_t* text_string,
    const ON_Font* font,
    bool bSingleStrokeFont,
    double text_height,
    double small_caps_scale,
    ON_ClassArray< ON_ClassArray< ON_SimpleArray< ON_Curve* > > >& string_contours
  );


private:
  friend class ON_GlyphMap;
  friend class ON_Font;
  
  // NOTE WELL: 
  //   The offset of m_codepoint in ON_FontGlyph must be >= 8 bytes.
  //   so the ON_FixeSizePool that manages memory for the glyph cache
  //   can efficiently iteratate all active managed glyphs.
  //
  ON_TextBox m_font_unit_glyph_bbox; // values in the native font definition units (freetype FT_LOAD_NO_SCALE units)
  ON_TextBox m_normalized_glyph_bbox; // bounding box in opennurbs normalized font coordinates

  // This box is for the platform native glyph. It can be different than m_glyph_box.
  // Example:
  //  Start with a Windows LOGFONT with face = Arial, height = ON_Font::Constants::AnnotationFontCellHeight (256)
  //  Native Windows height of Arial I = 165, height of LF = ...
  //  FreeType made from the same LOGFONT on the same has height of Arial I = 184, height of LF = ...

  // When font does not contain a glyph to render  a specified unicode codepoint,
  // then one or more glyphs from one or more subsitution fonts are used to
  // render the codepoint. In this case, m_substitutes points to a linked
  // list of substitute used to render the glyph.
  //
  ON__UINT32 m_code_point = ON_UnicodeCodePoint::ON_InvalidCodePoint;
 
  ON__UINT8 m_is_managed = 0; // 1 = managed glyph
  ON__UINT8 m_reserved1 = 0;
  ON__UINT16 m_reserved2 = 0;
  ON__UINT_PTR m_font_glyph_id = 0;
  const class ON_Font* m_managed_font = nullptr;
  const class ON_FontGlyph* m_substitute = nullptr;


private:
  void Internal_SetFontGlyphId(ON__UINT_PTR font_glyph_id);
  void Internal_CopyFrom(const ON_FontGlyph& src);
  static ON_FontGlyph* Internal_AllocateManagedGlyph(const ON_FontGlyph& src);
  bool Internal_GetPlatformSubstitute(
    ON_FontGlyph& substitue
  ) const;
};


#if defined(ON_RUNTIME_WIN)

/*
Remarks:
  Windows GDI functions used by ON_WindowsMeasureGlyph fail when the
  UTF-16 encoding of unicode_code_point requires a surrogate pair.
*/
ON_DECL
bool ON_WindowsGetGlyphMetrics(
  const ON_Font* font,
  ON__UINT32 unicode_code_point,
  class ON_TextBox& font_unit_glyph_box
);



#endif


#if defined(ON_DLL_TEMPLATE)
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_Font*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<const ON_Font*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_ClassArray<ON_Font>;
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_FontGlyph*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<const ON_FontGlyph*>;
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_FontGlyph>;
#endif

#endif


