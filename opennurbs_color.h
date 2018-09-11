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

#if !defined(OPENNURBS_COLOR_INC_)
#define OPENNURBS_COLOR_INC_

///////////////////////////////////////////////////////////////////////////////
//
// Class ON_Color
// 
class ON_CLASS ON_Color
{
public:
  ON_Color() = default;
  ~ON_Color() = default;
  ON_Color(const ON_Color&) = default;
  ON_Color& operator=(const ON_Color&) = default;

  static const ON_Color UnsetColor;       // 0xFFFFFFFFu
  static const ON_Color Black;            // 0x00000000u
  static const ON_Color White;            // 0x00FFFFFFu on little endan, 0xFFFFFF00u on big endian
  static const ON_Color SaturatedRed;     // 0x000000FFu on little endan, 0xFF000000u on big endian
  static const ON_Color SaturatedGreen;   // 0x0000FF00u on little endan, 0x00FF0000u on big endian
  static const ON_Color SaturatedBlue;    // 0x00FF0000u on little endan, 0x0000FF00u on big endian
  static const ON_Color SaturatedYellow;  // 0x0000FFFFu on little endan, 0xFFFF0000u on big endian
  static const ON_Color SaturatedCyan;    // 0x00FFFF00u on little endan, 0x00FFFF00u on big endian
  static const ON_Color SaturatedMagenta; // 0x00FF00FFu on little endan, 0xFF00FF00u on big endian
  static const ON_Color Gray126;          // R = G = B = 128 (medium)
  static const ON_Color Gray160;          // R = G = B = 160 (medium light)
  static const ON_Color Gray230;          // R = G = B = 230 (light)

  // If you need to use byte indexing to convert RGBA components to and from
  // an unsigned int ON_Color value and want your code to work on both little
  // and big endian computers, then use the RGBA_byte_index enum.
  //
  // unsigned int u;
  // unsigned char* rgba = &y;
  // rbga[ON_Color::kRedByteIndex] = red value 0 to 255.
  // rbga[ON_Color::kGreenByteIndex] = green value 0 to 255.
  // rbga[ON_Color::kBlueByteIndex] = blue value 0 to 255.
  // rbga[ON_Color::kAlphaByteIndex] = alpha value 0 to 255.
  // ON_Color color = u;
  enum RGBA_byte_index : unsigned int
  {
    // same for both little and big endian computers.
    kRedByteIndex = 0,
    kGreenByteIndex = 1,
    kBlueByteIndex = 2,
    kAlphaByteIndex = 3
  };

  // If you need to use shifting to convert RGBA components to and from
  // an unsigned int ON_COlor value and you want your code to work 
  // on both little and big endian computers, use the RGBA_shift enum.
  //
  // unsigned int u = 0;
  // u |= ((((unsigned int)red)   & 0xFFU) << ON_Color::RGBA_shift::kRedShift);
  // u |= ((((unsigned int)green) & 0xFFU) << ON_Color::RGBA_shift::kGreenShift);
  // u |= ((((unsigned int)blue)  & 0xFFU) << ON_Color::RGBA_shift::kBlueShift);
  // u |= ((((unsigned int)alpha) & 0xFFU) << ON_Color::RGBA_shift::kAlphaShift);
  // ON_Color color = u;
  enum RGBA_shift : unsigned int
  {
#if defined(ON_LITTLE_ENDIAN)
    kRedShift = 0,
    kGreenShift = 8,
    kBlueShift = 16,
    kAlphaShift = 24
#elif defined(ON_BIG_ENDIAN)
    kRedShift = 24,
    kGreenShift = 16,
    kBlueShift = 8,
    kAlphaShift = 0
#else
#error unknown endian
#endif
  };

  // Sets A = 0
	ON_Color(
    int red,   // ( 0 to 255 )
    int green, // ( 0 to 255 )
    int blue   // ( 0 to 255 )
    );

	ON_Color(
    int red,   // ( 0 to 255 )
    int green, // ( 0 to 255 )
    int blue,  // ( 0 to 255 )
    int alpha  // ( 0 to 255 )  (0 = opaque, 255 = transparent)
    );

  /*
  Parameters:
    colorref - [in]
      Windows COLORREF in little endian RGBA order.
  */
	ON_Color(
    unsigned int colorref
    );

	// Conversion to Windows COLORREF in little endian RGBA order.
  operator unsigned int() const;	

  /*
  Description:
    Call this function when the color is needed in a 
    Windows COLORREF format with alpha = 0;
  Returns
    A Windows COLOREF with alpha = 0.
  */
  unsigned int WindowsRGB() const;

  // < 0 if this < arg, 0 ir this==arg, > 0 if this > arg
  int Compare( const ON_Color& ) const; 

	int Red()   const; // ( 0 to 255 )
	int Green() const; // ( 0 to 255 )
	int Blue()  const; // ( 0 to 255 )
  int Alpha() const; // ( 0 to 255 ) (0 = opaque, 255 = transparent)

	double FractionRed()   const; // ( 0.0 to 1.0 )
	double FractionGreen() const; // ( 0.0 to 1.0 )
	double FractionBlue()  const; // ( 0.0 to 1.0 )
	double FractionAlpha() const; // ( 0.0 to 1.0 ) (0.0 = opaque, 1.0 = transparent)

  void SetRGB(
    int red,   // red in range 0 to 255
    int green, // green in range 0 to 255
    int blue   // blue in range 0 to 255
    );

  void SetFractionalRGB(
    double red,   // red in range 0.0 to 1.0
    double green, // green in range 0.0 to 1.0
    double blue   // blue in range 0.0 to 1.0
    );

  void SetAlpha(
    int alpha // alpha in range 0 to 255 (0 = opaque, 255 = transparent)
    );

  void SetFractionalAlpha(
    double alpha // alpha in range 0.0 to 1.0 (0.0 = opaque, 1.0 = transparent)
    );

  void SetRGBA(
    int red,   // red in range 0 to 255
    int green, // green in range 0 to 255
    int blue,  // blue in range 0 to 255
    int alpha  // alpha in range 0 to 255 (0 = opaque, 255 = transparent)
    );

  // input args
  void SetFractionalRGBA(
    double red,   // red in range 0.0 to 1.0
    double green, // green in range 0.0 to 1.0
    double blue,  // blue in range 0.0 to 1.0
    double alpha  // alpha in range 0.0 to 1.0 (0.0 = opaque, 1.0 = transparent)
    );

  // Hue() returns an angle in the range 0 to 2*pi 
  //
  //           0 = red, pi/3 = yellow, 2*pi/3 = green, 
  //           pi = cyan, 4*pi/3 = blue,5*pi/3 = magenta,
  //           2*pi = red
  double Hue() const;

  // Returns 0.0 (gray) to 1.0 (saturated)
  double Saturation() const;

  // Returns 0.0 (black) to 1.0 (white)
  double Value() const;

  void SetHSV( 
         double h, // hue in radians 0 to 2*pi
         double s, // satuation 0.0 = gray, 1.0 = saturated
         double v // value     
         );

private:
  union {
    // On little endian (Intel) computers, m_color has the same byte order
    // as Windows COLORREF values.
    // On little endian computers, m_color = 0xaabbggrr as an unsigned int value.
    // On big endian computers, m_color = 0xrrggbbaa as an unsigned int value
    //  rr = red component 0-255
    //  gg = grean component 0-255
    //  bb = blue component 0-255
    //  aa = alpha 0-255. 0 means opaque, 255 means transparent.
    unsigned int m_color = 0;

    // m_colorComponent is a 4 unsigned byte array in RGBA order
    // red component = m_RGBA[ON_Color::RGBA_byte::kRed]
    // grean component = m_RGBA[ON_Color::RGBA_byte::kGreen]
    // blue component = m_RGBA[ON_Color::RGBA_byte::kBlue]
    // alpha component = m_RGBA[ON_Color::RGBA_byte::kAlpha]
    unsigned char m_RGBA[4];
  };
};

#endif
