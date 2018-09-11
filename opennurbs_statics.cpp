#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#if defined(ON_COMPILER_MSC)
// Force this module to be inited first so the important globals
// are initialized before there is any possibility they are used.
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4073 )
#pragma init_seg(lib)
#pragma ON_PRAGMA_WARNING_POP
#endif

static unsigned int ON_LibraryStatusInit()
{
  return 0;
}

unsigned int ON::m_opennurbs_library_status = ON_LibraryStatusInit();

unsigned int ON_MemoryAllocationTracking::m_g_stack_depth = 0;
int ON_MemoryAllocationTracking::m_g_crt_dbg_flag0 = 0;

ON_MemoryAllocationTracking::ON_MemoryAllocationTracking(bool bEnableAllocationTracking)
#if defined(ON_DEBUG_MEMORY_MSC_WIN)
  : m_this_statck_depth(++ON_MemoryAllocationTracking::m_g_stack_depth)
  , m_this_crt_dbg_flag0(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
  : m_this_statck_depth(0)
  , m_this_crt_dbg_flag0(0)
#endif
{
#if defined(ON_DEBUG_MEMORY_MSC_WIN)
  // Dale Lear - Dec 2017
  //   Not thread safe but good enough for now and won't crash or assert.
  //   Relax, it's just a debugging tool based on ancient Microsoft debug heap tools.
  if (1 == m_this_statck_depth)
    ON_MemoryAllocationTracking::m_g_crt_dbg_flag0 = m_this_crt_dbg_flag0;
  if (bEnableAllocationTracking)
  {
    if ( 0 == (m_this_crt_dbg_flag0 & _CRTDBG_ALLOC_MEM_DF))
      _CrtSetDbgFlag(m_this_crt_dbg_flag0 | _CRTDBG_ALLOC_MEM_DF);
  }
  else if ( 0 != m_this_crt_dbg_flag0 )
  {
    _CrtSetDbgFlag(0);
  }
#endif
}

ON_MemoryAllocationTracking::~ON_MemoryAllocationTracking()
{
#if defined(ON_DEBUG_MEMORY_MSC_WIN)
  // Dale Lear - Dec 2017
  //   Not thread safe but good enough for now and won't crash or assert.
  //   Relax, it's just a debugging tool based on ancient Microsoft debug heap tools.
  if (ON_MemoryAllocationTracking::m_g_stack_depth > 0)
  {
    const bool bOutOfOrder = (m_this_statck_depth != ON_MemoryAllocationTracking::m_g_stack_depth);

    const int crt_dbg_flag0
      = bOutOfOrder
      ? ON_MemoryAllocationTracking::m_this_crt_dbg_flag0
      : m_this_crt_dbg_flag0;

    const int crt_dbg_flag1 = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    if ( crt_dbg_flag1 != crt_dbg_flag0 )
    {
      _CrtSetDbgFlag(crt_dbg_flag0);
    }
    ON_MemoryAllocationTracking::m_g_stack_depth--;
  }
#endif
}

ON_ClassId* ON_ClassId::m_p0 = 0; // static pointer to first id in list
ON_ClassId* ON_ClassId::m_p1 = 0; // static pointer to last id in list
int ON_ClassId::m_mark0 = 0;

// {1311ADCB-D89E-4051-A3F0-F64441FB8EC6}
const ON_UUID ON_StandardDisplayModeId::Wireframe =
{ 0x1311ADCB,0xD89E,0x4051,{ 0xA3,0xF0,0xF6,0x44,0x41,0xFB,0x8E,0xC6 } };

// {8BC8DEBE-C83B-4c47-B13C-9DB074510CAC}
const ON_UUID ON_StandardDisplayModeId::Shaded =
{ 0x8BC8DEBE,0xC83B,0x4c47,{ 0xB1,0x3C,0x9D,0xB0,0x74,0x51,0x0C,0xAC } };

// {CAE60BAE-2D51-4299-ABF7-A339FCA86F3B}
const ON_UUID ON_StandardDisplayModeId::Rendered =
{ 0xCAE60BAE,0x2D51,0x4299,{ 0xAB,0xF7,0xA3,0x39,0xFC,0xA8,0x6F,0x3B } };

// {FF608B97-81D3-4186-831C-41F7DC140881}
const ON_UUID ON_StandardDisplayModeId::Ghosted =
{ 0xFF608B97,0x81D3,0x4186,{ 0x83,0x1C,0x41,0xF7,0xDC,0x14,0x08,0x81 } };

// {B5C19D5D-0AEC-4ff7-A10E-E052E660263A}
const ON_UUID ON_StandardDisplayModeId::XrayShade =
{ 0xB5C19D5D,0x0AEC,0x4ff7,{ 0xA1,0x0E,0xE0,0x52,0xE6,0x60,0x26,0x3A } };

// {A5545314-9D87-428d-95AE-91052EEAD0FA}
const ON_UUID ON_StandardDisplayModeId::RenderedShadows =
{ 0xA5545314,0x9D87,0x428d,{ 0x95,0xAE,0x91,0x05,0x2E,0xEA,0xD0,0xFA } };

// {63612C72-778F-4afd-B81B-17426FDFE8A6}
const ON_UUID ON_StandardDisplayModeId::Technical =
{ 0x63612C72,0x778F,0x4afd,{ 0xB8,0x1B,0x17,0x42,0x6F,0xDF,0xE8,0xA6 } };

// {B46AB226-05A0-4568-B454-4B1AB721C675}
const ON_UUID ON_StandardDisplayModeId::Artistic =
{ 0xB46AB226,0x05A0,0x4568,{ 0xB4,0x54,0x4B,0x1A,0xB7,0x21,0xC6,0x75 } };

// {F4616FA5-A831-4620-A97E-9B807D5EC376}
const ON_UUID ON_StandardDisplayModeId::Pen =
{ 0xF4616FA5,0xA831,0x4620,{ 0xA9,0x7E,0x9B,0x80,0x7D,0x5E,0xC3,0x76 } };

// {C32B72C3-41BD-4ADC-82A8-B7AEF4456A37}
const ON_UUID ON_StandardDisplayModeId::AmbientOcclusion = 
{ 0xc32b72c3, 0x41bd, 0x4adc, { 0x82, 0xa8, 0xb7, 0xae, 0xf4, 0x45, 0x6a, 0x37 } };

// {69E0C7A5-1C6A-46C8-B98B-8779686CD181}
const ON_UUID ON_StandardDisplayModeId::Raytraced = 
{ 0x69e0c7a5, 0x1c6a, 0x46c8, { 0xb9, 0x8b, 0x87, 0x79, 0x68, 0x6c, 0xd1, 0x81 } };


const ON_UUID ON_nil_uuid = { 0,0,0,{ 0,0,0,0,0,0,0,0 } };
const ON_UUID ON_max_uuid = { 0xFFFFFFFF,0xFFFF,0xFFFF,{ 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } };

const ON_UUID ON_rhino2_id = { 0x16d0eca6, 0x359, 0x4e4c,{ 0x9f, 0xe, 0xf2, 0x69, 0xfd, 0x47, 0x6c, 0xc4 } };

const ON_UUID ON_rhino3_id = { 0xA7BBFF3C, 0xFF19, 0x4883,{ 0x85, 0x8D, 0xB1, 0xE7, 0xDB, 0x4F, 0x1A, 0x7E } };

// {E2143A46-BB01-4b0c-AC4D-C34B5652FAE0}
const ON_UUID ON_rhino4_id = { 0xe2143a46, 0xbb01, 0x4b0c,{ 0xac, 0x4d, 0xc3, 0x4b, 0x56, 0x52, 0xfa, 0xe0 } };

// {60515F84-8F7F-41da-801D-1C87E32F88F5}
const ON_UUID ON_rhino5_id = { 0x60515f84, 0x8f7f, 0x41da,{ 0x80, 0x1d, 0x1c, 0x87, 0xe3, 0x2f, 0x88, 0xf5 } };

// {06BB1079-5A56-47A1-AD6D-0B45183D894B}
const ON_UUID ON_rhino6_id = { 0x6bb1079, 0x5a56, 0x47a1,{ 0xad, 0x6d, 0xb, 0x45, 0x18, 0x3d, 0x89, 0x4b } };

// ON_rhino_id is always set to the value for the current version
// of Rhino.  ON_rhino_id is the id that should be used as the
// userdata application id for userdata class definitions that are
// in the core Rhino executable.
const ON_UUID ON_rhino_id = ON_rhino6_id;

// Used to identifiy userdata read from V2 files
// which were written before userdata had application ids.
// {132F2340-DB90-494e-BF02-C36F0EA3197C}
const ON_UUID ON_v2_userdata_id = { 0x132f2340, 0xdb90, 0x494e,{ 0xbf, 0x2, 0xc3, 0x6f, 0xe, 0xa3, 0x19, 0x7c } };

// Used to identifiy userdata read from V3 files
// which were written before userdata had application ids.
// {4307B91D-6A9D-478e-B0A2-7C577997C663}
const ON_UUID ON_v3_userdata_id = { 0x4307b91d, 0x6a9d, 0x478e,{ 0xb0, 0xa2, 0x7c, 0x57, 0x79, 0x97, 0xc6, 0x63 } };

// Used to identifiy userdata read from V4 files
// which were written before opennurbs 200609190
// required application ids.
// {F73F2953-A244-44c2-B7C2-7E27390D1196}
const ON_UUID ON_v4_userdata_id = { 0xf73f2953, 0xa244, 0x44c2,{ 0xb7, 0xc2, 0x7e, 0x27, 0x39, 0xd, 0x11, 0x96 } };

// {17B3ECDA-17BA-4e45-9E67-A2B8D9BE520D}
const ON_UUID ON_opennurbs4_id = { 0x17b3ecda, 0x17ba, 0x4e45,{ 0x9e, 0x67, 0xa2, 0xb8, 0xd9, 0xbe, 0x52, 0xd } };

// {C8CDA597-D957-4625-A4B3-A0B510FC30D4}
const ON_UUID ON_opennurbs5_id = { 0xc8cda597, 0xd957, 0x4625,{ 0xa4, 0xb3, 0xa0, 0xb5, 0x10, 0xfc, 0x30, 0xd4 } };

// {7B0B585D-7A31-45D0-925E-BDD7DDF3E4E3}
const ON_UUID ON_opennurbs6_id = { 0x7b0b585d, 0x7a31, 0x45d0,{ 0x92, 0x5e, 0xbd, 0xd7, 0xdd, 0xf3, 0xe4, 0xe3 } };


// ON_opennurbs_id is always set to the value for the current version
// of opennurbs.  ON_opennurbs_id is the id that should be used as
// the userdata application id for userdata classes definitions that
// are in the opennurbs library.
const ON_UUID ON_opennurbs_id = ON_opennurbs6_id;

const ON_UuidPairList ON_UuidPairList::EmptyList;

const ON_COMPONENT_INDEX ON_COMPONENT_INDEX::UnsetComponentIndex;

// All opennurbs static members are initialized here so that initialization happens in a predictable order.
/*
IEEE 754
Storage
size      sign     exponent         fraction
float    4 bytes   bit 31    8 bits (30-23)  23 bits (22-0)
double   8 bytes   bit 63   11 bits (62-52)  52 bits (51-0)
sign bit = 1 indicates negative
sign bit = 0 indicates positive
float  absolute value = 2^(e-127)  * 1+(f/2^23)
e = value of the 8 bit number in the exponent field
f = value of the 23 bit number in the fraction field
double absolute value = 2^(e-1023) * 1+(f/2^51)
e = value of the 11 bit number in the exponent field
f = value of the 51 bit number in the fraction field
Exceptions:
If all exponent bits are all 0 (e = 0) and the fraction bits
are all zero, then the value of the number is zero.
If all exponent bits are all 0 (e = 0) and at least one fraction
bits is not zero, then the representaion is "denormalized".
In this case, the float absolute value is 0.f*2^-126 and the
double absolute value is 0.f*2^-1022.
If all exponent bits are 1 (float e = 11111111binary = 255decimal
or double e = 11111111111 binary = 2047 decimal) and the fraction
bits are all zero, the number is infinity.  The sign bit
determines the sign of infinity.

If all exponent bits are 1 and at least one fraction bit is
not zero, the number is a "NaN" (not a number).  If the most
significant fraction bit is 1, the number is a quiet NaN or
"QNaN".  If the most significan fraction bit is 0, the number
is a signalling NaN or "SNaN".

Some authors (SH)   QNaNs are used to indicate
indeterminant operations, like sqrt(-1.0).  SNaNs are used
to indicate invalid operations.
SH - http://steve.hollasch.net/cgindex/coding/ieeefloat.html
Intel -
*/
static double ON__dblinithelper(int i)
{
  // called twice - performance is not important
  union
  {
    double x;
    unsigned char b[8];
  } u;
  unsigned int i7, i6;

  // different bytes on
  u.x = 2.0; // sign = 0; fraction = 0; exponent = 100 0000 0000 binary

  if (0x40 == u.b[7] && 0 == u.b[0]
    && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
    && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6]
    )
  {
    // little endian doubles
    i7 = 7; i6 = 6;
  }
  else if (0x40 == u.b[0] && 0 == u.b[7]
    && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
    && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6]
    )
  {
    // big endian doubles
    i7 = 0; i6 = 1;
  }
  else
  {
    // this sitation is not handled by this algorithm
    // and that is a bug in the algorithm.
    ON_ERROR("CPU has unexpected bit pattern in double 2.0.");
    i7 = 0;
    i6 = 0;
    i = 99;
  }

  if (1 == i) // positive quiet NaN
  {
    // all exponent bits = 1
    // fraction bits = 100...0
    u.b[i7] = 0x7F; // 0111 1111
    u.b[i6] = 0xF8; // 1111 1000
    u.b[5] = 0;    // 0...
    u.b[4] = 0;
    u.b[3] = 0;
    u.b[2] = 0;
    u.b[7 - i6] = 0;
    u.b[7 - i7] = 0;
  }
  else if (2 == i) // positive infinity
  {
    // all exponent bits = 1
    // all fraction bits = 0
    u.b[i7] = 0x7F; // 0111 1111
    u.b[i6] = 0xF0; // 1111 0000
    u.b[5] = 0;    // 0...
    u.b[4] = 0;
    u.b[3] = 0;
    u.b[2] = 0;
    u.b[7 - i6] = 0;
    u.b[7 - i7] = 0;
  }
  else
  {
    // invalid input 
    u.b[0] = 0xFF;
    u.b[1] = 0xFF;
    u.b[2] = 0xFF;
    u.b[3] = 0xFF;
    u.b[4] = 0xFF;
    u.b[5] = 0xFF;
    u.b[6] = 0xFF;
    u.b[7] = 0xFF;
  }

  return u.x;
}

static float ON__fltinithelper(int i)
{
  // called twice - performance is not important
  union
  {
    float x;
    unsigned char b[4];
  } u;
  unsigned int i3, i2;

  // different bytes on
  u.x = 2.0f; // sign = 0; mantissa = 0; exponent = 1000 0000
  if (0x40 == u.b[3] && 0 == u.b[0] && 0 == u.b[1] && 0 == u.b[2])
  {
    // little endian doubles
    i3 = 3; i2 = 2;
  }
  else if (0x40 == u.b[0] && 0 == u.b[3] && 0 == u.b[1] && 0 == u.b[2])
  {
    // big endian doubles
    i3 = 0; i2 = 1;
  }
  else
  {
    // this sitation is not handled by this algorithm
    // and that is a bug in the algorithm.
    ON_ERROR("CPU has unexpected bit pattern in float 2.0f.");
    i3 = 0;
    i2 = 0;
    i = 99;
  }

  if (1 == i) // positive quiet NaN
  {
    // all exponent bits = 1
    // fraction bits = 100...0
    u.b[i3] = 0x7F; // 0111 1111
    u.b[i2] = 0xC0; // 1100 0000
    u.b[3 - i2] = 0;    // 0...
    u.b[3 - i3] = 0;
  }
  else if (2 == i) // positive infinity
  {
    // all exponent bits = 1
    // all fraction bits = 0
    u.b[i3] = 0x7F; // 0111 1111
    u.b[i2] = 0x80; // 1000 0000
    u.b[3 - i2] = 0;    // 0...
    u.b[3 - i3] = 0;
  }
  else
  {
    // invalid input 
    u.b[0] = 0xFF;
    u.b[1] = 0xFF;
    u.b[2] = 0xFF;
    u.b[3] = 0xFF;
  }

  return u.x;
}

const double ON_DBL_QNAN = ON__dblinithelper(1);
const double ON_DBL_PINF = ON__dblinithelper(2);
const double ON_DBL_NINF = -ON__dblinithelper(2);

const float  ON_FLT_QNAN = ON__fltinithelper(1);
const float  ON_FLT_PINF = ON__fltinithelper(2);
const float  ON_FLT_NINF = -ON__fltinithelper(2);

const ON_2dex ON_2dex::Unset(ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX);
const ON_2dex ON_2dex::Zero(0, 0);
const ON_3dex ON_3dex::Unset(ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX);
const ON_3dex ON_3dex::Zero(0, 0, 0);
const ON_4dex ON_4dex::Unset(ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX);
const ON_4dex ON_4dex::Zero(0, 0, 0, 0);

const ON_2udex ON_2udex::Unset(ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX);
const ON_2udex ON_2udex::Zero(0, 0);
const ON_3udex ON_3udex::Unset(ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX);
const ON_3udex ON_3udex::Zero(0, 0, 0);
const ON_4udex ON_4udex::Unset(ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX, ON_UNSET_UINT_INDEX);
const ON_4udex ON_4udex::Zero(0, 0, 0, 0);

const ON_Interval ON_Interval::EmptyInterval(ON_UNSET_VALUE,ON_UNSET_VALUE);
const ON_Interval ON_Interval::ZeroToOne(0.0,1.0);
const ON_Interval ON_Interval::ZeroToTwoPi(0.0,2.0*ON_PI);
const ON_Interval ON_Interval::Nan(ON_DBL_QNAN,ON_DBL_QNAN);

const ON_TextBox ON_TextBox::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_TextBox);

const ON_CheckSum ON_CheckSum::UnsetCheckSum;

const ONX_ErrorCounter ONX_ErrorCounter::Zero ON_CLANG_CONSTRUCTOR_BUG_INIT(ONX_ErrorCounter);

static ON_MD5_Hash ON_MD5_Hash_EmptyContentHash()
{
  ON_MD5 md5;
  return md5.Hash();
}
const ON_MD5_Hash ON_MD5_Hash::EmptyContentHash = ON_MD5_Hash_EmptyContentHash();
const ON_MD5_Hash ON_MD5_Hash::ZeroDigest ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_MD5_Hash);

static ON_SHA1_Hash ON_SHA1_Hash_EmptyContentHash()
{
  ON_SHA1 sha1;
  return sha1.Hash();
}
const ON_SHA1_Hash ON_SHA1_Hash::EmptyContentHash = ON_SHA1_Hash_EmptyContentHash();
const ON_SHA1_Hash ON_SHA1_Hash::ZeroDigest ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SHA1_Hash);


const ONX_ModelTest ONX_ModelTest::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ONX_ModelTest);

// Works with Microsoft's CL, fails for Apple's CLang
////   const struct ON_UnicodeErrorParameters ON_UnicodeErrorParameters::MaskErrors   = { 0, 0xFFFFFFFF, ON_UnicodeCodePoint::ON_ReplacementCharacter };
////   const struct ON_UnicodeErrorParameters ON_UnicodeErrorParameters::FailOnErrors = { 0, 0,          ON_UnicodeCodePoint::ON_ReplacementCharacter };
// Works with Microsoft's CL and less capable compilers
static struct ON_UnicodeErrorParameters ON_Internal_UnicodeErrorParameters_Default(unsigned int error_mask)
{
  struct ON_UnicodeErrorParameters uep;
  memset(&uep, 0, sizeof(uep));
  uep.m_error_status = 0;
  uep.m_error_mask = error_mask;
  uep.m_error_code_point = ON_UnicodeCodePoint::ON_ReplacementCharacter;
  return uep;
}
const struct ON_UnicodeErrorParameters ON_UnicodeErrorParameters::MaskErrors = ON_Internal_UnicodeErrorParameters_Default(0xFFFFFFFF);
const struct ON_UnicodeErrorParameters ON_UnicodeErrorParameters::FailOnErrors = ON_Internal_UnicodeErrorParameters_Default(0);

// ON_wString is UTF-8 encoded
const char ON_String::Backspace = (char)ON_UnicodeCodePoint::ON_Backspace;
const char ON_String::Tab = (char)ON_UnicodeCodePoint::ON_Tab;
const char ON_String::LineFeed = (char)ON_UnicodeCodePoint::ON_LineFeed;
const char ON_String::VerticalTab = (char)ON_UnicodeCodePoint::ON_VerticalTab;
const char ON_String::FormFeed = (char)ON_UnicodeCodePoint::ON_FormFeed;
const char ON_String::CarriageReturn = (char)ON_UnicodeCodePoint::ON_CarriageReturn;
const char ON_String::Escape = (char)ON_UnicodeCodePoint::ON_Escape;
const char ON_String::Space = (char)ON_UnicodeCodePoint::ON_Space;
const char ON_String::Slash = (char)ON_UnicodeCodePoint::ON_Slash;
const char ON_String::Backslash = (char)ON_UnicodeCodePoint::ON_Backslash;
const char ON_String::Pipe = (char)ON_UnicodeCodePoint::ON_Pipe;

// ON_wString is UTF-16 encoded when sizeof(wchar_t) = 2
// ON_wString is UTF-32 encoded when sizeof(wchar_t) = 4
// Never cast these values as "char"
// The UTF-8 representation of any Unicode code point with value > 127
// requires multiple bytes.
const wchar_t ON_wString::Backspace = (wchar_t)ON_UnicodeCodePoint::ON_Backspace;
const wchar_t ON_wString::Tab = (wchar_t)ON_UnicodeCodePoint::ON_Tab;
const wchar_t ON_wString::LineFeed = (wchar_t)ON_UnicodeCodePoint::ON_LineFeed;
const wchar_t ON_wString::VerticalTab = (wchar_t)ON_UnicodeCodePoint::ON_VerticalTab;
const wchar_t ON_wString::FormFeed = (wchar_t)ON_UnicodeCodePoint::ON_FormFeed;
const wchar_t ON_wString::CarriageReturn = (wchar_t)ON_UnicodeCodePoint::ON_CarriageReturn;
const wchar_t ON_wString::Escape = (wchar_t)ON_UnicodeCodePoint::ON_Escape;
const wchar_t ON_wString::Space = (wchar_t)ON_UnicodeCodePoint::ON_Space;
const wchar_t ON_wString::Slash = (wchar_t)ON_UnicodeCodePoint::ON_Slash;
const wchar_t ON_wString::Backslash = (wchar_t)ON_UnicodeCodePoint::ON_Backslash;
const wchar_t ON_wString::Pipe = (wchar_t)ON_UnicodeCodePoint::ON_Pipe;

#if defined(ON_SIZEOF_WCHAR_T) && ON_SIZEOF_WCHAR_T >= 2
// ON_wString is UTF-16 encoded when sizeof(wchar_t) = 2
// ON_wString is UTF-32 encoded when sizeof(wchar_t) = 4
const wchar_t ON_wString::DegreeSymbol = (wchar_t)ON_UnicodeCodePoint::ON_DegreeSymbol;
const wchar_t ON_wString::RadiusSymbol = (wchar_t)ON_UnicodeCodePoint::ON_RadiusSymbol;
const wchar_t ON_wString::DiameterSymbol = (wchar_t)ON_UnicodeCodePoint::ON_DiameterSymbol;
const wchar_t ON_wString::PlusMinusSymbol = (wchar_t)ON_UnicodeCodePoint::ON_PlusMinusSymbol;
const wchar_t ON_wString::RecyclingSymbol = (wchar_t)ON_UnicodeCodePoint::ON_RecyclingSymbol;
const wchar_t ON_wString::ReplacementCharacter = (wchar_t)ON_UnicodeCodePoint::ON_ReplacementCharacter;
const wchar_t ON_wString::NextLine = (wchar_t)ON_UnicodeCodePoint::ON_NextLine;
const wchar_t ON_wString::LineSeparator = (wchar_t)ON_UnicodeCodePoint::ON_LineSeparator;
const wchar_t ON_wString::ParagraphSeparator = (wchar_t)ON_UnicodeCodePoint::ON_ParagraphSeparator;
const wchar_t ON_wString::NoBreakSpace = (wchar_t)ON_UnicodeCodePoint::ON_NoBreakSpace;
const wchar_t ON_wString::NarrowNoBreakSpace = (wchar_t)ON_UnicodeCodePoint::ON_NarrowNoBreakSpace;
const wchar_t ON_wString::ZeroWidthSpace = (wchar_t)ON_UnicodeCodePoint::ON_ZeroWidthSpace;
#endif

const ON_String ON_String::EmptyString;
const ON_wString ON_wString::EmptyString;
const ON_NameHash ON_NameHash::UnsetNameHash ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_NameHash);
const ON_NameHash ON_NameHash::EmptyNameHash = ON_NameHash::CreateIdAndEmptyName(ON_nil_uuid);
const ON_wString ON_ModelComponent::ReferencePrefixDelimiter(L" : ");
const ON_wString ON_ModelComponent::ReferencePrefixSeparator(L">");
const ON_wString ON_ModelComponent::NamePathSeparator(L"::");

const char ON_FileSystemPath::DirectorySeparatorAsChar =
#if defined(ON_RUNTIME_WIN)
// Windows
ON_String::Backslash
#else
// Apple, Android, Unix, ...
ON_String::Slash
#endif
;
const wchar_t ON_FileSystemPath::DirectorySeparator = (wchar_t)ON_FileSystemPath::DirectorySeparatorAsChar;

const char ON_FileSystemPath::AlternateDirectorySeparatorAsChar =
#if defined(ON_RUNTIME_WIN)
// Windows
ON_String::Slash
#else
// Apple, Android, Unix, ...
ON_String::Backslash
#endif
;
const wchar_t ON_FileSystemPath::AlternateDirectorySeparator = (wchar_t)ON_FileSystemPath::AlternateDirectorySeparatorAsChar;



const ON_ContentHash ON_ContentHash::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_ContentHash);
const ON_FileReference ON_FileReference::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_FileReference);

const ON_3dmRevisionHistory ON_3dmRevisionHistory::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmRevisionHistory);
const ON_3dmProperties ON_3dmProperties::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmProperties);



static ON_Xform ON_Xform_Init(double x, bool bDiagonal)
{
  ON_Xform xform;
  memset(&xform, 0, sizeof(xform));
  if (bDiagonal)
  {
    xform.m_xform[0][0] = x;
    xform.m_xform[1][1] = x;
    xform.m_xform[2][2] = x;
    xform.m_xform[3][3] = 1.0;
  }
  else
  {
    double* p = &xform.m_xform[0][0];
    double* p1 = p + 16;
    while (p < p1)
      *p++ = x;
  }
  return xform;
}

const ON_Xform ON_Xform::IdentityTransformation = ON_Xform_Init(1.0, true);
const ON_Xform ON_Xform::ZeroTransformation = ON_Xform_Init(0.0, true);

const ON_Xform ON_Xform::Zero4x4 = ON_Xform_Init(0.0, false);
const ON_Xform ON_Xform::Unset = ON_Xform_Init(ON_UNSET_VALUE, false);
const ON_Xform ON_Xform::Nan = ON_Xform_Init(ON_DBL_QNAN, false);


const ON_2dPoint ON_2dPoint::Origin(0.0, 0.0);
const ON_2dPoint ON_2dPoint::UnsetPoint(ON_UNSET_VALUE, ON_UNSET_VALUE);
const ON_2dPoint ON_2dPoint::NanPoint(ON_DBL_QNAN, ON_DBL_QNAN);


const ON_3dPoint ON_3dPoint::Origin(0.0, 0.0, 0.0);
const ON_3dPoint ON_3dPoint::UnsetPoint(ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE);
const ON_3dPoint ON_3dPoint::NanPoint(ON_DBL_QNAN, ON_DBL_QNAN, ON_DBL_QNAN);

const ON_4dPoint ON_4dPoint::Zero(0.0, 0.0, 0.0, 0.0);
const ON_4dPoint ON_4dPoint::Nan(ON_DBL_QNAN, ON_DBL_QNAN, ON_DBL_QNAN, ON_DBL_QNAN);


const ON_Triangle ON_Triangle::ZeroTriangle(ON_3dPoint::Origin, ON_3dPoint::Origin, ON_3dPoint::Origin);
const ON_Triangle ON_Triangle::UnsetTriangle(ON_3dPoint::UnsetPoint, ON_3dPoint::UnsetPoint, ON_3dPoint::UnsetPoint);
const ON_Triangle ON_Triangle::NanTriangle(ON_3dPoint::NanPoint, ON_3dPoint::NanPoint, ON_3dPoint::NanPoint);

const ON_2dVector ON_2dVector::ZeroVector(0.0, 0.0);
const ON_2dVector ON_2dVector::XAxis(1.0, 0.0);
const ON_2dVector ON_2dVector::YAxis(0.0, 1.0);
const ON_2dVector ON_2dVector::UnsetVector(ON_UNSET_VALUE, ON_UNSET_VALUE);
const ON_2dVector ON_2dVector::NanVector(ON_DBL_QNAN, ON_DBL_QNAN);

const ON_3dVector ON_3dVector::ZeroVector(0.0, 0.0, 0.0);
const ON_3dVector ON_3dVector::XAxis(1.0, 0.0, 0.0);
const ON_3dVector ON_3dVector::YAxis(0.0, 1.0, 0.0);
const ON_3dVector ON_3dVector::ZAxis(0.0, 0.0, 1.0);
const ON_3dVector ON_3dVector::UnsetVector(ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE);
const ON_3dVector ON_3dVector::NanVector(ON_DBL_QNAN, ON_DBL_QNAN, ON_DBL_QNAN);

const ON_2fPoint ON_2fPoint::Origin(0.0f, 0.0f);
const ON_2fPoint ON_2fPoint::NanPoint(ON_FLT_QNAN, ON_FLT_QNAN);

const ON_3fPoint ON_3fPoint::Origin(0.0f, 0.0f, 0.0f);
const ON_3fPoint ON_3fPoint::NanPoint(ON_FLT_QNAN, ON_FLT_QNAN, ON_FLT_QNAN);

const ON_4fPoint ON_4fPoint::Zero(0.0f, 0.0f, 0.0f, 0.0f);
const ON_4fPoint ON_4fPoint::Nan(ON_FLT_QNAN, ON_FLT_QNAN, ON_FLT_QNAN, ON_FLT_QNAN);

const ON_2fVector ON_2fVector::ZeroVector(0.0f, 0.0f);
const ON_2fVector ON_2fVector::XAxis(1.0f, 0.0f);
const ON_2fVector ON_2fVector::YAxis(0.0f, 1.0f);

const ON_3fVector ON_3fVector::ZeroVector(0.0f, 0.0f, 0.0f);
const ON_3fVector ON_3fVector::XAxis(1.0f, 0.0f, 0.0f);
const ON_3fVector ON_3fVector::YAxis(0.0f, 1.0f, 0.0f);
const ON_3fVector ON_3fVector::ZAxis(0.0f, 0.0f, 1.0f);

static ON_BoundingBox BoundingBoxInit(double x)
{
  ON_BoundingBox bbox;
  bbox.m_min.x = x;
  bbox.m_min.y = x;
  bbox.m_min.z = x;
  bbox.m_max.x = x;
  bbox.m_max.y = x;
  bbox.m_max.z = x;
  return bbox;
}
const ON_BoundingBox ON_BoundingBox::EmptyBoundingBox;
const ON_BoundingBox ON_BoundingBox::UnsetBoundingBox = BoundingBoxInit(ON_UNSET_VALUE);
const ON_BoundingBox ON_BoundingBox::NanBoundingBox = BoundingBoxInit(ON_DBL_QNAN);

const ON_UnitSystem ON_UnitSystem::None(ON::LengthUnitSystem::None);
const ON_UnitSystem ON_UnitSystem::Angstroms(ON::LengthUnitSystem::Angstroms);
const ON_UnitSystem ON_UnitSystem::Nanometers(ON::LengthUnitSystem::Nanometers);
const ON_UnitSystem ON_UnitSystem::Microns(ON::LengthUnitSystem::Microns);
const ON_UnitSystem ON_UnitSystem::Millimeters(ON::LengthUnitSystem::Millimeters);
const ON_UnitSystem ON_UnitSystem::Centimeters(ON::LengthUnitSystem::Centimeters);
const ON_UnitSystem ON_UnitSystem::Decimeters(ON::LengthUnitSystem::Decimeters);
const ON_UnitSystem ON_UnitSystem::Meters(ON::LengthUnitSystem::Meters);
const ON_UnitSystem ON_UnitSystem::Dekameters(ON::LengthUnitSystem::Dekameters);
const ON_UnitSystem ON_UnitSystem::Hectometers(ON::LengthUnitSystem::Hectometers);
const ON_UnitSystem ON_UnitSystem::Kilometers(ON::LengthUnitSystem::Kilometers);
const ON_UnitSystem ON_UnitSystem::Megameters(ON::LengthUnitSystem::Megameters);
const ON_UnitSystem ON_UnitSystem::Gigameters(ON::LengthUnitSystem::Gigameters);
const ON_UnitSystem ON_UnitSystem::Microinches(ON::LengthUnitSystem::Microinches);
const ON_UnitSystem ON_UnitSystem::Mils(ON::LengthUnitSystem::Mils);
const ON_UnitSystem ON_UnitSystem::Inches(ON::LengthUnitSystem::Inches);
const ON_UnitSystem ON_UnitSystem::Feet(ON::LengthUnitSystem::Feet);
const ON_UnitSystem ON_UnitSystem::Yards(ON::LengthUnitSystem::Yards);
const ON_UnitSystem ON_UnitSystem::Miles(ON::LengthUnitSystem::Miles);
const ON_UnitSystem ON_UnitSystem::PrinterPoints(ON::LengthUnitSystem::PrinterPoints);
const ON_UnitSystem ON_UnitSystem::PrinterPicas(ON::LengthUnitSystem::PrinterPicas);
const ON_UnitSystem ON_UnitSystem::NauticalMiles(ON::LengthUnitSystem::NauticalMiles);
const ON_UnitSystem ON_UnitSystem::AstronomicalUnits(ON::LengthUnitSystem::AstronomicalUnits);
const ON_UnitSystem ON_UnitSystem::LightYears(ON::LengthUnitSystem::LightYears);
const ON_UnitSystem ON_UnitSystem::Parsecs(ON::LengthUnitSystem::Parsecs);
const ON_UnitSystem ON_UnitSystem::Unset(ON::LengthUnitSystem::Unset);

const ON_LengthUnitName ON_LengthUnitName::None ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_LengthUnitName);
const ON_AngleUnitName ON_AngleUnitName::None ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_AngleUnitName);

const ON_LengthValue ON_LengthValue::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_LengthValue);
const ON_LengthValue ON_LengthValue::Zero = ON_LengthValue::Create(0.0, ON::LengthUnitSystem::None, 0, ON_LengthValue::StringFormat::CleanDecimal);

const ON_AngleValue ON_AngleValue::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_AngleValue);
const ON_AngleValue ON_AngleValue::Zero = ON_AngleValue::Create(0.0, ON::AngleUnitSystem::None, 0, ON_AngleValue::StringFormat::CleanDecimal );

const ON_ScaleValue ON_ScaleValue::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_ScaleValue);
const ON_ScaleValue ON_ScaleValue::OneToOne = ON_ScaleValue::Create(
  ON_LengthValue::Create(1.0, ON::LengthUnitSystem::None, 0, ON_LengthValue::StringFormat::CleanDecimal),
  ON_LengthValue::Create(1.0, ON::LengthUnitSystem::None, 0, ON_LengthValue::StringFormat::CleanDecimal),
  ON_ScaleValue::ScaleStringFormat::RatioFormat
);

ON_TextLog ON_TextLog::Null( *((ON_wString*)((ON__INT_PTR)1)) );


// Discuss any changes of these values with Dale Lear
//
// For 32 bit float based OpenGL drivers, the value of
// the ON_Viewport::DefaultMinNearOverFar constant must 
// be <0.01 and >= 0.0001.  
// If you change this value, you need to retest RR 8902 on OpenGL
// drivers that (internally) use float precision transformations.
// Some OpenGL drivers, like the Microsoft software emulation
// driver for XP crash in some cases when near/far > 1e8.
//
// ON_Viewport::DefaultMinNearOverFar = 0.001    // used in Rhino 3.0 beta testing until 11 Sep 2002
// ON_Viewport::DefaultMinNearOverFar = 0.01     // used for Rhino 3.0 CD1 and CD2
// ON_Viewport::DefaultMinNearOverFar = 0.000001 // used for Rhino 3.0 CD3
// ON_Viewport::DefaultMinNearOverFar = 0.0001   // used for Rhino 4.0 Fixes RR 8902
//
// Discuss any changes of these values with Dale Lear
const double ON_Viewport::DefaultNearDist = 0.005;
const double ON_Viewport::DefaultFarDist = 1000.0;
const double ON_Viewport::DefaultMinNearDist = 0.0001;
const double ON_Viewport::DefaultMinNearOverFar = 0.0001;

const ON_3dVector ON_Viewport::Default3dCameraDirection(-0.43301270189221932338186158537647,0.75,-0.5);
const ON_3dPoint ON_Viewport::DefaultCameraLocation(0.0, 0.0, 100.0);
const ON_Viewport ON_Viewport::DefaultTopViewYUp ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Viewport);
static ON_Viewport Internal_ON_Viewport_DefaultPerspectiveView_Init()
{
  ON_Viewport vp;
  vp.SetCameraDirection(ON_Viewport::Default3dCameraDirection);
  vp.SetCameraUp(ON_3dVector::ZAxis);
  vp.SetCameraLocation(ON_Viewport::DefaultCameraLocation.DistanceTo(ON_3dPoint::Origin)*vp.CameraZ());

  return vp;
}
const ON_Viewport ON_Viewport::DefaultPerspectiveViewZUp = Internal_ON_Viewport_DefaultPerspectiveView_Init();


const ON_3dmIOSettings ON_3dmIOSettings::Default ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmIOSettings);
const ON_3dmConstructionPlaneGridDefaults ON_3dmConstructionPlaneGridDefaults::Default ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmConstructionPlaneGridDefaults);

ON_EarthAnchorPoint Internal_EarthAnchorPointConstant(int which)
{
  ON_EarthAnchorPoint eap;
  switch (which)
  {
  case 1: // Seattle Space Needle
    {
      const double latitude_degrees = 47.620397; // Cal Topo Maps (downloaded June 24, 2016)
      const double longitude_degrees = -122.349179; // Cal Topo Maps (downloaded June 24, 2016)
      const double ground_elevation_feet = 207.0; // feet c. Geonames - Cal Topo Maps (downloaded June 24, 2016)
      const double observation_deck_height_feet = 520.0; // feet above the ground  http://www.spaceneedle.com/fun-facts/
      eap.SetEarthLocation(
        ON::EarthCoordinateSystem::MeanSeaLevel, 
        ON::LengthUnitSystem::Feet,
        latitude_degrees,
        longitude_degrees,
        ground_elevation_feet + observation_deck_height_feet
      );
    }
    break;
  }
  return eap;
}
const ON_EarthAnchorPoint ON_EarthAnchorPoint::Unset = Internal_EarthAnchorPointConstant(0);
const ON_EarthAnchorPoint ON_EarthAnchorPoint::SeattleSpaceNeedle = Internal_EarthAnchorPointConstant(1);

const ON_3dmAnnotationSettings ON_3dmAnnotationSettings::Default ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmAnnotationSettings);

const ON_3dmSettings ON_3dmSettings::Default ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmSettings);

const ON_3dmAnnotationContext ON_3dmAnnotationContext::Default ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmAnnotationContext);

const ON_3dmArchiveTableStatus ON_3dmArchiveTableStatus::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmArchiveTableStatus);

const wchar_t* ON_TextDot::DefaultFontFace = L"Arial";
const int ON_TextDot::DefaultHeightInPoints = 14;
const int ON_TextDot::MinimumHeightInPoints = 3;
const ON_TextDot ON_TextDot::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_TextDot);

const ON_Locale ON_Locale::Ordinal = ON_Locale::FromWindowsLCID(ON_Locale::OrdinalLCID);
const ON_Locale ON_Locale::InvariantCulture = ON_Locale::FromWindowsLCID(ON_Locale::InvariantCultureLCID);

// DLL_Main() andc other appropriate places in Rhino application
// initialization call ON_Locale::SetCurrentCulture() to initialize
// ON_Locale::m_CurrentCulture.
ON_Locale ON_Locale::m_CurrentCulture = ON_Locale::Ordinal;
const ON_Locale& ON_Locale::CurrentCulture = ON_Locale::m_CurrentCulture;

static ON_ClippingRegionPoints ON_ClippingRegionPoints_EmptyInit()
{
  ON_ClippingRegionPoints empty_clip_points;
  memset(&empty_clip_points, 0, sizeof(empty_clip_points));
  return empty_clip_points;
}

const ON_ClippingRegionPoints ON_ClippingRegionPoints::Empty = ON_ClippingRegionPoints_EmptyInit();

static ON_PickPoint ON_PickPoint_UnsetInit()
{
  ON_PickPoint unset_pick_point;
  double* p = unset_pick_point.m_t;
  double* p1 = p + sizeof(unset_pick_point.m_t) / sizeof(unset_pick_point.m_t[0]);
  while (p < p1)
    *p++ = ON_UNSET_VALUE;
  return unset_pick_point;
}

const ON_PickPoint ON_PickPoint::Unset = ON_PickPoint_UnsetInit();

const ON_Color ON_Color::UnsetColor(ON_UNSET_COLOR);
const ON_Color ON_Color::Black(0, 0, 0);
const ON_Color ON_Color::White(255, 255, 255);
const ON_Color ON_Color::SaturatedRed(255, 0, 0);
const ON_Color ON_Color::SaturatedGreen(0, 255, 0);
const ON_Color ON_Color::SaturatedBlue(0, 0, 255);
const ON_Color ON_Color::SaturatedYellow(255, 255, 0);
const ON_Color ON_Color::SaturatedCyan(0, 255, 255);
const ON_Color ON_Color::SaturatedMagenta(255, 0, 255);
const ON_Color ON_Color::Gray126(126, 126, 126);
const ON_Color ON_Color::Gray160(160, 160, 160);
const ON_Color ON_Color::Gray230(230, 230, 230);

const ON_UuidIndex ON_UuidIndex::NilIndex = ON_UuidIndex();
const ON_UuidPtr ON_UuidPtr::NilPtr = ON_UuidPtr();

const ON_Line ON_Line::ZeroLine(ON_3dPoint::Origin, ON_3dPoint::Origin);
const ON_Line ON_Line::UnsetLine(ON_3dPoint::UnsetPoint, ON_3dPoint::UnsetPoint);
const ON_Line ON_Line::NanLine(ON_3dPoint::NanPoint, ON_3dPoint::NanPoint);

const ON_PlaneEquation ON_PlaneEquation::UnsetPlaneEquation(ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE);
const ON_PlaneEquation ON_PlaneEquation::ZeroPlaneEquation(0.0, 0.0, 0.0, 0.0);

const ON_Plane ON_xy_plane(ON_3dPoint::Origin, ON_3dVector::XAxis, ON_3dVector::YAxis);
const ON_Plane ON_yz_plane(ON_3dPoint::Origin, ON_3dVector::YAxis, ON_3dVector::ZAxis);
const ON_Plane ON_zx_plane(ON_3dPoint::Origin, ON_3dVector::ZAxis, ON_3dVector::XAxis);
const ON_Plane ON_Plane::World_xy = ON_xy_plane;

static ON_Plane ON_Plane_UnsetPlane()
{
  ON_Plane unset_plane;
  unset_plane.xaxis = ON_3dVector::UnsetVector;
  unset_plane.yaxis = ON_3dVector::UnsetVector;
  unset_plane.zaxis = ON_3dVector::UnsetVector;
  unset_plane.origin = ON_3dPoint::UnsetPoint;
  unset_plane.plane_equation = ON_PlaneEquation::UnsetPlaneEquation;
  return unset_plane;
}

const ON_Plane ON_Plane::UnsetPlane(ON_Plane_UnsetPlane());

// {F15F67AA-4AF9-4B25-A3B8-517CEDDAB134}
const ON_UUID ON_MeshParameters::RhinoLegacyMesherId = { 0xf15f67aa, 0x4af9, 0x4b25,{ 0xa3, 0xb8, 0x51, 0x7c, 0xed, 0xda, 0xb1, 0x34 } };

// {EB6F6F3F-F975-4546-9D1C-64E9423BEB7F}
const ON_UUID ON_MeshParameters::PangolinMesherId = { 0xeb6f6f3f, 0xf975, 0x4546,{ 0x9d, 0x1c, 0x64, 0xe9, 0x42, 0x3b, 0xeb, 0x7f } };

static ON_MeshParameters Internal_ON_MeshParameters_Constants(
  int selector
  )
{
  ON_MeshParameters mp;

  // If you change any parameter values, put your name, the date, and the reasons for
  // the change in a comment by the changed value. Include the previous value in
  // your comment. This is crtically important so we can keep track of what we are
  // trying to accomplish.

  switch (selector)
  {
  case 0: // ON_MeshParameters::DefaultMesh
    break;

  case 1: 
    // ON_MeshParameters::FastRenderMesh
    // Added 27 April 2006 for the Meshkateers
    //   Attempting to make jagged and faster render meshes a little
    //   more dense.
    //
    // Turn off everything ...
    mp.SetComputeCurvature(false);
    mp.SetTolerance(0.0);
    mp.SetJaggedSeams(false);
    mp.SetMaximumEdgeLength(0.0);
    mp.SetGridAspectRatio(0.0);
    mp.SetGridMaxCount(0);
    mp.SetGridAngleRadians(0.0);
    mp.SetGridAmplification(0.0);
    mp.SetRefineAngleRadians(0.0);

    // ... except ...
    // The m_relative_tolerance setting must be set so that
    // 0.0005 = ON_MeshParameters::Tolerance(m_relative_tolerance,1.0).
    mp.SetRelativeTolerance(0.65);
    //double x = Tolerance(m_relative_tolerance,1.0);

    mp.SetGridMinCount(16);
    mp.SetMinimumEdgeLength(0.0001);
    mp.SetRefine(true);
    mp.SetSimplePlanes(true);

    mp.SetTextureRange(2); // Don't change this without speaking to Dale Lear

                            //{
                            //  // 16 July, 2002 - copied V2 hard coded "jagged and faster" render mesh settings
                            //  //
                            //  // Settings used in V2, V3 and early V4 beta
                            //  mp.m_refine_angle       = 20.0*ON_PI/180.0;
                            //  mp.m_grid_angle         = 20.0*ON_PI/180.0;
                            //  mp.m_grid_aspect_ratio  = 0.0;
                            //  mp.m_min_edge_length    = 0.0001;
                            //  mp.m_max_edge_length    = 0.0;
                            //  mp.m_tolerance          = 0.0;
                            //  mp.m_grid_min_count     = 16;
                            //  mp.m_bRefine            = 1;
                            //  mp.m_bJaggedSeams       = 0;
                            //  mp.m_bSimplePlanes      = 0;
                            //}
    break;

  case 2: // ON_MeshParameters::QualityRenderMesh
          // Added 27 April 2006 for the Meshkateers
          //   Attempting to make smooth and slower render meshing a little
          //   faster.
          //
          // Turn off everything ...
    mp.SetComputeCurvature(false);
    mp.SetTolerance(0.0);
    mp.SetJaggedSeams(false);
    mp.SetMaximumEdgeLength(0.0);
    mp.SetGridAspectRatio(0.0);
    mp.SetGridMaxCount(0);
    mp.SetGridAngleRadians(0.0);
    mp.SetGridAmplification(0.0);

    // ... except ...
    // The m_relative_tolerance setting must be set so that
    // 0.00025 = ON_MeshParameters::Tolerance(m_relative_tolerance,1.0).
    mp.SetRelativeTolerance(0.8);
    //double x = Tolerance(m_relative_tolerance,1.0);

    mp.SetGridMinCount(16);
    mp.SetMinimumEdgeLength(0.0001);
    mp.SetRefine(true);
    mp.SetSimplePlanes(true);
    mp.SetRefineAngleDegrees(20.0);

    mp.SetTextureRange(2); // Don't change this without speaking to Dale Lear


                            //// 16 July, 2002 - copied V2 hard coded "smooth and slower" render mesh settings
                            ////
                            //// Settings used in V2, V3 and early V4 beta
                            //mp.Setrefine_angle       = 15.0*ON_PI/180.0;
                            //mp.Setgrid_angle         = 15.0*ON_PI/180.0;
                            //mp.Setgrid_aspect_ratio  = 6.0;
                            //mp.Setmin_edge_length    = 0.0001;
                            //mp.Setmax_edge_length    = 0.0;
                            //mp.Settolerance          = 0.0;
                            //mp.Setgrid_min_count     = 16;
                            //mp.SetbRefine            = 1;
                            //mp.SetbJaggedSeams       = 0;
                            //mp.SetbSimplePlanes      = 0;
    break;

  case 3: // ON_MeshParameters::DefaultAnalysisMesh
          // 7 July 2006 Dale Lear
          //    I switched from the default constructor to the density=0.5 constructor to fix RR 10330.
    mp = ON_MeshParameters(0.5, ON_MeshParameters::DefaultMesh.MinimumEdgeLength());
    mp.SetTextureRange(1); // m_texture_range must be 1.  Do not change this setting.

    break;
  }

  if (0 != selector)
  {
    if (1 == selector )
    {
      // ON_MeshParameters::DefaultMesh has been initialized.
      // ON_MeshParameters::DefaultMesh must be initialized before ContentHash() can be used.
      ON_MeshParameters::DefaultMesh.ContentHash();
    }
    // Compute cached sha1 content hash.
    mp.ContentHash();
  }

  return mp;
}

const ON_MeshParameters ON_MeshParameters::DefaultMesh = Internal_ON_MeshParameters_Constants(0);
const ON_MeshParameters ON_MeshParameters::FastRenderMesh = Internal_ON_MeshParameters_Constants(1);
const ON_MeshParameters ON_MeshParameters::QualityRenderMesh = Internal_ON_MeshParameters_Constants(2);
const ON_MeshParameters ON_MeshParameters::DefaultAnalysisMesh = Internal_ON_MeshParameters_Constants(3);

const ON_3dmUnitsAndTolerances ON_3dmUnitsAndTolerances::Millimeters ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmUnitsAndTolerances);

const ON_Circle ON_Circle::UnitCircle ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Circle);
const ON_Arc ON_Arc::UnitCircle ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Arc);
const ON_3dmRenderSettings ON_3dmRenderSettings::Default ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmRenderSettings);

const ON_ProgressStepCounter ON_ProgressStepCounter::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_ProgressStepCounter);

const ON_HistoryRecord ON_HistoryRecord::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_HistoryRecord);

const ON_RTreeMemPool ON_RTreeMemPool::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_RTreeMemPool);
const ON_RTree ON_RTree::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_RTree);

// {F5E3BAA9-A7A2-49FD-B8A1-66EB274A5F91}
const ON_UUID ON_MeshCache::RenderMeshId =
{ 0xf5e3baa9, 0xa7a2, 0x49fd,{ 0xb8, 0xa1, 0x66, 0xeb, 0x27, 0x4a, 0x5f, 0x91 } };

// {AC12F955-A29F-437B-A4C9-06C85B7FF57C}
const ON_UUID ON_MeshCache::AnalysisMeshId =
{ 0xac12f955, 0xa29f, 0x437b,{ 0xa4, 0xc9, 0x6, 0xc8, 0x5b, 0x7f, 0xf5, 0x7c } };

// {3AACA7F2-0444-4587-8083-A13C5BA0AE50}
const ON_UUID ON_MeshCache::PreviewMeshId =
{ 0x3aaca7f2, 0x444, 0x4587,{ 0x80, 0x83, 0xa1, 0x3c, 0x5b, 0xa0, 0xae, 0x50 } };

// {85D9E08D-386C-45C3-83DC-0C354685D504}
const ON_UUID ON_MeshCache::CoarseMeshId =
{ 0x85d9e08d, 0x386c, 0x45c3,{ 0x83, 0xdc, 0xc, 0x35, 0x46, 0x85, 0xd5, 0x4 } };

// {A2DAF594-F44B-44A6-A44B-E856CD7C05E8}
const ON_UUID ON_MeshCache::FineMeshId =
{ 0xa2daf594, 0xf44b, 0x44a6,{ 0xa4, 0x4b, 0xe8, 0x56, 0xcd, 0x7c, 0x5, 0xe8 } };

// {A82C0F1B-13E8-4DEC-AD35-D3DDD277203C}
const ON_UUID ON_MeshCache::AnyMeshId =
{ 0xa82c0f1b, 0x13e8, 0x4dec,{ 0xad, 0x35, 0xd3, 0xdd, 0xd2, 0x77, 0x20, 0x3c } };

const ON_MeshCache ON_MeshCache::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_MeshCache);

#if defined(OPENNURBS_ANNOTATION_TABLE_WIP)
const double ON_Table::MinColWidth    = 1.0;
const double ON_Table::MinRowHeight   = 0.1;
const double ON_Table::MinTableWidth  = 1.0;
const double ON_Table::MinTableHeight = 0.1;
#endif

const ON_2iPoint ON_2iPoint::Origin(0, 0);
const ON_2iPoint ON_2iPoint::Unset(ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX);

const ON_2iVector ON_2iVector::Zero(0, 0);
const ON_2iVector ON_2iVector::UnitX(1, 0);
const ON_2iVector ON_2iVector::UnitY(0, 1);
const ON_2iVector ON_2iVector::Unset(ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX);

const ON_2iBoundingBox ON_2iBoundingBox::Zero(ON_2iPoint::Origin,ON_2iPoint::Origin);
const ON_2iBoundingBox ON_2iBoundingBox::Unset(ON_2iPoint::Unset, ON_2iPoint::Unset);

const ON_2iSize ON_2iSize::Zero(0, 0);
const ON_2iSize ON_2iSize::Unset(ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX);

const ON_4iRect ON_4iRect::Zero(0, 0, 0, 0);
const ON_4iRect ON_4iRect::Unset(ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX, ON_UNSET_INT_INDEX);

const ON_2dSize ON_2dSize::Zero(0.0, 0.0);
const ON_2dSize ON_2dSize::Unset(ON_UNSET_VALUE, ON_UNSET_VALUE);

const ON_4dRect ON_4dRect::Zero(0.0, 0.0, 0.0, 0.0);
const ON_4dRect ON_4dRect::Unset(ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE, ON_UNSET_VALUE);

const ON_FontGlyphOutlinePoint ON_FontGlyphOutlinePoint::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_FontGlyphOutlinePoint);
const ON_FontGlyph ON_FontGlyph::Unset  ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_FontGlyph);

const double ON_FontMetrics::DefaultLineFeedRatio = 1.6;
const ON__UINT32 ON_FontMetrics::HeightOfCapitalCodePoint = 'I';
const ON_FontMetrics ON_FontMetrics::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_FontMetrics);

unsigned int ON_Font::__runtime_serial_number_generator = 0;

static ON_Font DefaultFontInitializer()
{
  const wchar_t* default_face_name =
#if defined(ON_RUNTIME_WIN)
    L"Arial"
    // L"Segoe UI" // better choice ?
#elif defined(ON_RUNTIME_APPLE)
    //L"Lucida Grande " // pre OS X Yosemite default
    L"Helvetica Neue" // OS X Yosemite and iOS default
#else
    L"Arial"
#endif
    ;
  ON_Font default_font;
  default_font.SetFontFaceName(default_face_name);
  return default_font;
}

static void Internal_SystemModelComponentInit(
  ON_UUID id,
  int index,
  const wchar_t* name,
  ON_ModelComponent& model_component
  )
{
  if (ON_nil_uuid != id)
    model_component.SetId(id);
  if (ON_UNSET_INT_INDEX != index)
    model_component.SetIndex(index);
  if (nullptr != name && 0 != name[0])
    model_component.SetName(name);

  // non-empty names are locked after application localization
  model_component.LockAllSettingsExceptName();
  if (model_component.NameIsEmpty())
    model_component.LockName();
}

// The order of the next three lines is critical

const ON_Font ON_Font::Default(1, DefaultFontInitializer());

const ON_Geometry ON_Geometry::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Geometry);

const ON_3dmObjectAttributes ON_3dmObjectAttributes::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmObjectAttributes);

const ON_3dmObjectAttributes ON_3dmObjectAttributes::DefaultAttributes  ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_3dmObjectAttributes);


static const ON_ModelComponentTypeIterator ON_ModelComponentIterator_Init(
  int list_selector
  )
{
  //const ON_ModelComponent::Type all_types[] =
  //{
  //  ON_ModelComponent::Type::Unset,
  //  ON_ModelComponent::Type::Image,
  //  ON_ModelComponent::Type::TextureMapping,
  //  ON_ModelComponent::Type::RenderMaterial,
  //  ON_ModelComponent::Type::LinePattern,
  //  ON_ModelComponent::Type::Layer,
  //  ON_ModelComponent::Type::Group,
  //  ON_ModelComponent::Type::TextStyle,
  //  ON_ModelComponent::Type::DimStyle,
  //  ON_ModelComponent::Type::RenderLight,
  //  ON_ModelComponent::Type::HatchPattern,
  //  ON_ModelComponent::Type::InstanceDefinition,
  //  ON_ModelComponent::Type::ModelGeometry,
  //  ON_ModelComponent::Type::HistoryRecord,
  //  ON_ModelComponent::Type::Mixed,
  //};

  const ON_ModelComponent::Type explicit_types[] =
  {
    //ALWAYS EXCLUDE// ON_ModelComponent::Type::Unset,
    ON_ModelComponent::Type::Image,
    ON_ModelComponent::Type::TextureMapping,
    ON_ModelComponent::Type::RenderMaterial,
    ON_ModelComponent::Type::LinePattern,
    ON_ModelComponent::Type::Layer,
    ON_ModelComponent::Type::Group,
    ON_ModelComponent::Type::TextStyle,
    ON_ModelComponent::Type::DimStyle,
    ON_ModelComponent::Type::RenderLight,
    ON_ModelComponent::Type::HatchPattern,
    ON_ModelComponent::Type::InstanceDefinition,
    ON_ModelComponent::Type::ModelGeometry,
    ON_ModelComponent::Type::HistoryRecord,
    //ALWAYS EXCLUDE// ON_ModelComponent::Type::Mixed,
  };

  const ON_ModelComponent::Type table_types[] =
  {
    //ALWAYS EXCLUDE// ON_ModelComponent::Type::Unset,
    //ON_ModelComponent::Type::Image,
    ON_ModelComponent::Type::TextureMapping,
    ON_ModelComponent::Type::RenderMaterial,
    ON_ModelComponent::Type::LinePattern,
    ON_ModelComponent::Type::Layer,
    ON_ModelComponent::Type::Group,
    ON_ModelComponent::Type::TextStyle,
    ON_ModelComponent::Type::DimStyle,
    //ON_ModelComponent::Type::RenderLight,
    ON_ModelComponent::Type::HatchPattern,
    ON_ModelComponent::Type::InstanceDefinition,
    //ON_ModelComponent::Type::ModelGeometry,
    //ON_ModelComponent::Type::HistoryRecord,
    //ALWAYS EXCLUDE// ON_ModelComponent::Type::Mixed,
  };

  switch (list_selector)
  {
    //case 0:
    //  return ON_ModelComponentTypeIterator(sizeof(all_types) / sizeof(all_types[0]), all_types);
  case 1:
    return ON_ModelComponentTypeIterator(sizeof(explicit_types) / sizeof(explicit_types[0]), explicit_types);
  case 2:
    return ON_ModelComponentTypeIterator(sizeof(table_types) / sizeof(table_types[0]), table_types);
  }

  return ON_ModelComponentTypeIterator(0, nullptr);
}

const ON_ModelComponentTypeIterator ON_ModelComponentTypeIterator::ExplicitComponentTypes(ON_ModelComponentIterator_Init(1));
const ON_ModelComponentTypeIterator ON_ModelComponentTypeIterator::TableComponentTypes(ON_ModelComponentIterator_Init(2));

const ON_ModelComponent ON_ModelComponent::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_ModelComponent);


const ON_ModelComponentContentMark ON_ModelComponentContentMark::Unset  ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_ModelComponentContentMark);


// explicit constructor added to work around Apple CLANG bug.
const ON_ComponentManifest ON_ComponentManifest::Empty;

const ON_ManifestMapItem ON_ManifestMapItem::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_ManifestMapItem);

// explicit constructor added to work around Apple CLANG bug.
const ON_ManifestMap ON_ManifestMap::Empty;

const ON_ComponentManifestItem ON_ComponentManifestItem::UnsetItem ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_ComponentManifestItem);

const ON_Bitmap ON_Bitmap::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Bitmap);

const ON_EmbeddedBitmap ON_EmbeddedBitmap::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_EmbeddedBitmap);

const ON_WindowsBitmap ON_WindowsBitmap::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_WindowsBitmap);

const ON_WindowsBitmapEx ON_WindowsBitmapEx::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_WindowsBitmapEx);

const ON_Light ON_Light::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Light);

const ON_InstanceDefinition ON_InstanceDefinition::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_InstanceDefinition);

const ON_ModelGeometryComponent ON_ModelGeometryComponent::Unset = ON_ModelGeometryComponent(ON_ModelComponent::Type::Unset);

const ON_Texture ON_Texture::Default ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Texture);

const double ON_Material::MaxShine = 255.0;

const ON_Material ON_Material::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Material);

static ON_Material DefaultMaterialInit()
{
  // {2FACDC3B-269B-4722-A68D-E7D5C6DE0C44}
  const ON_UUID id = { 0x2facdc3b, 0x269b, 0x4722,{ 0xa6, 0x8d, 0xe7, 0xd5, 0xc6, 0xde, 0xc, 0x44 } };
  ON_Material m;
  Internal_SystemModelComponentInit(id, -1, nullptr, m);
  return m;
}
const ON_Material ON_Material::Default(DefaultMaterialInit());

const ON_TextureMapping ON_TextureMapping::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_TextureMapping);

static ON_TextureMapping SurfaceParameterTextureMappingInitializer()
{
  //// {B988A6C2-61A6-45a7-AAEE-9AED7EF4E316}
  static const ON_UUID srfp_mapping_id = { 0xb988a6c2, 0x61a6, 0x45a7,{ 0xaa, 0xee, 0x9a, 0xed, 0x7e, 0xf4, 0xe3, 0x16 } };

  ON_TextureMapping tm;
  tm.SetId(srfp_mapping_id);
  tm.SetIndex(-1);
  // name = empty

  tm.m_type = ON_TextureMapping::TYPE::srfp_mapping;

  return tm;
}
const ON_TextureMapping ON_TextureMapping::SurfaceParameterTextureMapping(SurfaceParameterTextureMappingInitializer());

const ON_LinetypeSegment ON_LinetypeSegment::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_LinetypeSegment);
const ON_LinetypeSegment ON_LinetypeSegment::OneMillimeterLine(1.0, ON_LinetypeSegment::eSegType::stLine);

const ON_Linetype ON_Linetype::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Linetype);

const ON_Group ON_Group::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Group);

static void LinePatternInit(
  ON_UUID id,
  int index,
  const wchar_t* name,
  bool bIsContinuous,
  ON_Linetype& line_pattern
  )
{
  Internal_SystemModelComponentInit(id, index, name, line_pattern);
  if (bIsContinuous)
    line_pattern.AppendSegment(ON_LinetypeSegment::OneMillimeterLine);
  line_pattern.LockPattern();
}

static ON_Linetype ContinuousLinePattern()
{
  const ON_UUID continuous_line_pattern_id =
  { 0x3999bed5, 0x78ee, 0x4d73,{ 0xa0, 0x59, 0x3, 0x22, 0x24, 0xc6, 0xfd, 0x55 } };

  ON_Linetype continuous_line_pattern;
  LinePatternInit(
    continuous_line_pattern_id,
    -1,
    L"Continuous",
    true,
    continuous_line_pattern
    );

  return continuous_line_pattern;
}

const ON_Linetype ON_Linetype::Continuous(ContinuousLinePattern());


static ON_Linetype ByLayerLinetype()
{
  const ON_UUID by_layer_line_pattern_id =
  { 0x913882da, 0xbce9, 0x4a67,{ 0x8d, 0x86, 0xd4, 0x49, 0xfd, 0x58, 0x50, 0xb8 } };

  ON_Linetype by_layer_line_pattern;
  LinePatternInit(
    by_layer_line_pattern_id,
    -2,
    L"By Layer",
    false,
    by_layer_line_pattern
    );
  return by_layer_line_pattern;
}

const ON_Linetype ON_Linetype::ByLayer(ByLayerLinetype());

static ON_Linetype ByParentLinetype()
{
  const ON_UUID by_parent_line_pattern_id =
  { 0xef59d771, 0x5099, 0x4f60,{ 0x99, 0x14, 0xc1, 0x83, 0x6a, 0xeb, 0xe4, 0x84 } };

  ON_Linetype by_parent_line_pattern;
  LinePatternInit(
    by_parent_line_pattern_id,
    -3,
    L"By Parent",
    false,
    by_parent_line_pattern
    );

  return by_parent_line_pattern;
}

const ON_Linetype ON_Linetype::ByParent(ByParentLinetype());


static void TextStyleInit(
  const wchar_t* name,
  ON_UUID id,
  int index,
  const ON_Font* font,
  ON_TextStyle& text_style
  )
{
  ON_wString font_description;
  bool bLockName = false;
  if (nullptr != font)
  {
    font_description = font->FontDescription();
    text_style.SetFont(font);

    if ( (nullptr == name || 0 == name[0]) && font_description.IsNotEmpty() )
    {
      name = font_description;
      bLockName = true;
    }
  }

  Internal_SystemModelComponentInit(id, index, name, text_style);

  if (bLockName)
    text_style.LockName();
}

const ON_Layer ON_Layer::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Layer);

static ON_Layer ON_Layer_Default()
{
  // {061DF99E-2EF8-4A3F-8F2D-4B123A166089}
  const ON_UUID id = { 0x61df99e, 0x2ef8, 0x4a3f,{ 0x8f, 0x2d, 0x4b, 0x12, 0x3a, 0x16, 0x60, 0x89 } };
  ON_Layer layer;
  layer.SetParentId(ON_nil_uuid);
  Internal_SystemModelComponentInit(id, -1, L"Default", layer);
  return layer;
}
const ON_Layer ON_Layer::Default = ON_Layer_Default();

static ON_TextStyle UnsetTextStyle()
{
  ON_TextStyle text_style;
  TextStyleInit(nullptr, ON_nil_uuid, 0, nullptr, text_style);
  return text_style;
}

static ON_TextStyle DefaultTextStyle()
{
  // {8F3A5848-7741-4AA9-B6A0-FA4F76C9D918}
  const ON_UUID default_text_style_id =
  { 0x8f3a5848, 0x7741, 0x4aa9,{ 0xb6, 0xa0, 0xfa, 0x4f, 0x76, 0xc9, 0xd9, 0x18 } };

  ON_TextStyle text_style;
  TextStyleInit(L"Default", default_text_style_id, -1, &ON_Font::Default, text_style);
  return text_style;
}

static ON_TextStyle ByLayerTextStyle()
{
  // {DA800C9A-EB00-4251-8237-615017F3BB67}
  const ON_UUID ByLayer_text_style_id =
  { 0xda800c9a, 0xeb00, 0x4251,{ 0x82, 0x37, 0x61, 0x50, 0x17, 0xf3, 0xbb, 0x67 } };

  ON_TextStyle text_style;
  TextStyleInit(L"By Layer", ByLayer_text_style_id, -2, nullptr, text_style);
  return text_style;
}

static ON_TextStyle ByParentTextStyle()
{
  // {4D82AFFA-0433-4CE0-92C8-BD328E23C49F}
  const ON_UUID ByParent_text_style_id =
  { 0x4d82affa, 0x433, 0x4ce0,{ 0x92, 0xc8, 0xbd, 0x32, 0x8e, 0x23, 0xc4, 0x9f } };

  ON_TextStyle text_style;
  TextStyleInit(L"By Parent", ByParent_text_style_id, -3, nullptr, text_style);
  return text_style;
}

const ON_TextStyle ON_TextStyle::Unset(UnsetTextStyle());
const ON_TextStyle ON_TextStyle::Default(DefaultTextStyle());
const ON_TextStyle ON_TextStyle::ByLayer(ByLayerTextStyle());
const ON_TextStyle ON_TextStyle::ByParent(ByParentTextStyle());

const ON_TextMask ON_TextMask::None ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_TextMask);

//static bool ValidateCpyStuff(const ON_DimStyle& src)
//{
//  ON_DimStyle cpyctor(src);
//  ON_DimStyle opeq;
//  opeq = src;
//  const ON_ScaleValue v1 = src.ScaleValue();
//  const ON_ScaleValue v2 = cpyctor.ScaleValue();
//  const ON_ScaleValue v3 = opeq.ScaleValue();
//
//  bool rc = v1.RightToLeftScale() == v2.RightToLeftScale() && v1.RightToLeftScale() == v2.RightToLeftScale();
//
//  return rc;
//}

static void DimStyleInit(
  const wchar_t* name,
  int index,
  ON_UUID id,
  ON_DimStyle& dimension_style
  )
{
  Internal_SystemModelComponentInit(id, index, name, dimension_style);

  dimension_style.SetAlternatePrefix(L" [");
  dimension_style.SetAlternateSuffix(L"]");
  // identical values for most or all built-in dimension styles
  dimension_style.SetFont(ON_Font::Default);
  dimension_style.SetDimTextLocation(ON_DimStyle::TextLocation::AboveDimLine);
}

static void Internal_SystemDimStyleFinalize(
  ON_DimStyle& dimension_style
)
{
  dimension_style.ClearAllFieldOverrides();
  dimension_style.ContentHash();
}

static ON_DimStyle DimStyleDefault()
{
  // {25B90869-0022-4E04-B498-98B4175F65FD}
  const ON_UUID id =
  { 0x25b90869, 0x22, 0x4e04,{ 0xb4, 0x98, 0x98, 0xb4, 0x17, 0x5f, 0x65, 0xfd } };

  ON_DimStyle dimstyle;
  DimStyleInit(L"Default", -1, id, dimstyle);

  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static ON_DimStyle DimStyleInchDecimal()
{
  const ON_UUID id = { 0x2105610c, 0xcfc7, 0x4473,{ 0xa5, 0x80, 0xc3, 0xd9, 0xc, 0xe8, 0xc7, 0xa3 } };

  ON_DimStyle dimstyle;
  DimStyleInit(L"Inch Decimal", -2, id, dimstyle);

  //dimstyle.SetDimScale(10.0);
  dimstyle.SetDimScale(1.0, ON::LengthUnitSystem::Inches, 10.0, ON::LengthUnitSystem::Inches);
  dimstyle.SetUnitSystem(ON::LengthUnitSystem::Inches);

  dimstyle.SetExtExtension(0.125);
  dimstyle.SetExtOffset(0.0625);
  dimstyle.SetArrowSize(0.125);
  dimstyle.SetCenterMark(0.25);
  dimstyle.SetTextGap(0.0625);
  dimstyle.SetTextHeight(0.125);
  dimstyle.SetBaselineSpacing(0.375);


  dimstyle.SetDimTextLocation(ON_DimStyle::TextLocation::AboveDimLine);

  dimstyle.SetArrowType1(ON_Arrowhead::arrow_type::SolidTriangle);
  dimstyle.SetArrowType2(ON_Arrowhead::arrow_type::SolidTriangle);

  dimstyle.SetDimensionLengthDisplay(ON_DimStyle::LengthDisplay::InchesDecimal);
  dimstyle.SetLengthResolution(2);

  dimstyle.SetAngleFormat(ON_DimStyle::angle_format::DecimalDegrees);
  dimstyle.SetAngleResolution(0);

  dimstyle.SetDimensionLengthDisplay(ON_DimStyle::LengthDisplay::InchesDecimal);
  dimstyle.SetAlternateLengthResolution(2);
  dimstyle.SetLengthFactor(1.0);
  dimstyle.SetAlternate(false);
  dimstyle.SetAlternateDimensionLengthDisplay(ON_DimStyle::LengthDisplay::ModelUnits);
  dimstyle.SetAlternateLengthResolution(2);
  dimstyle.SetAlternateLengthFactor(1.0);

  dimstyle.SetDimExtension(0.0);

  dimstyle.SetLeaderArrowSize(0.125);
  dimstyle.SetLeaderArrowType(ON_Arrowhead::arrow_type::SolidTriangle);
  dimstyle.SetLeaderLandingLength(0.125);

  //ValidateCpyStuff(dimstyle);

  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}


static ON_DimStyle DimStyleInchFractional()
{
  const ON_UUID id = { 0x6bcb1506, 0x699f, 0x445d,{ 0xa1, 0x22, 0x4f, 0xc7, 0x78, 0x2b, 0xc4, 0x86 } };

  ON_DimStyle dimstyle;
  DimStyleInit(L"Inch Fractional", -3, id, dimstyle);

  //dimstyle.SetDimScale(12.0);
  dimstyle.SetDimScale(1.0, ON::LengthUnitSystem::Inches, 1.0, ON::LengthUnitSystem::Feet);
  dimstyle.SetUnitSystem(ON::LengthUnitSystem::Inches);

  dimstyle.SetExtExtension(0.125);
  dimstyle.SetExtOffset(0.0625);
  dimstyle.SetArrowSize(0.1);
  dimstyle.SetCenterMark(0.25);
  dimstyle.SetTextGap(0.0625);
  dimstyle.SetTextHeight(0.125);
  dimstyle.SetBaselineSpacing(0.375);

  dimstyle.SetArrowType1(ON_Arrowhead::arrow_type::Tick);
  dimstyle.SetArrowType2(ON_Arrowhead::arrow_type::Tick);


  dimstyle.SetAngleFormat(ON_DimStyle::angle_format::DecimalDegrees);
  dimstyle.SetAngleResolution(1);

  dimstyle.SetDimensionLengthDisplay(ON_DimStyle::LengthDisplay::InchesFractional);
  dimstyle.SetLengthResolution(4);
  dimstyle.SetLengthFactor(1.0);
  dimstyle.SetAlternate(false);
  dimstyle.SetAlternateDimensionLengthDisplay(ON_DimStyle::LengthDisplay::ModelUnits);
  dimstyle.SetAlternateLengthResolution(2);
  dimstyle.SetAlternateLengthFactor(1.0);

  dimstyle.SetDimExtension(0.1);

  dimstyle.SetLeaderArrowSize(0.1);
  dimstyle.SetLeaderArrowType(ON_Arrowhead::arrow_type::SolidTriangle);
  dimstyle.SetLeaderLandingLength(0.125);

  //ValidateCpyStuff(dimstyle);

  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}


static ON_DimStyle DimStyleFootInchArchitecture()
{
  const ON_UUID id = { 0x50d6ef1b, 0xd1d0, 0x408a,{ 0x86, 0xc0, 0xee, 0x8b, 0x36, 0x8, 0x88, 0x3e } };

  ON_DimStyle dimstyle;
  DimStyleInit(L"Foot-Inch Architectural", -4, id, dimstyle);

  //dimstyle.SetDimScale(96.0);
  dimstyle.SetDimScale(0.125, ON::LengthUnitSystem::Inches, 1.0, ON::LengthUnitSystem::Feet);
  dimstyle.SetUnitSystem(ON::LengthUnitSystem::Inches);

  dimstyle.SetExtExtension(0.125);
  dimstyle.SetExtOffset(0.0625);
  dimstyle.SetArrowSize(0.1);
  dimstyle.SetCenterMark(0.25);
  dimstyle.SetTextGap(0.0625);
  dimstyle.SetTextHeight(0.125);
  dimstyle.SetBaselineSpacing(0.375);

  dimstyle.SetArrowType1(ON_Arrowhead::arrow_type::Tick);
  dimstyle.SetArrowType2(ON_Arrowhead::arrow_type::Tick);


  dimstyle.SetAngleFormat(ON_DimStyle::angle_format::DecimalDegrees);
  dimstyle.SetAngleResolution(0);

  dimstyle.SetDimensionLengthDisplay(ON_DimStyle::LengthDisplay::FeetAndInches);
  dimstyle.SetLengthResolution(3);
  dimstyle.SetLengthFactor(1.0);
  dimstyle.SetAlternate(false);
  dimstyle.SetAlternateDimensionLengthDisplay(ON_DimStyle::LengthDisplay::ModelUnits);
  dimstyle.SetAlternateLengthResolution(2);
  dimstyle.SetAlternateLengthFactor(1.0);

  dimstyle.SetDimExtension(0.1);

  dimstyle.SetLeaderArrowSize(0.1);
  dimstyle.SetLeaderArrowType(ON_Arrowhead::arrow_type::SolidTriangle);
  dimstyle.SetLeaderLandingLength(0.125);

  //ValidateCpyStuff(dimstyle);

  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}


static ON_DimStyle DimStyleMillimeterSmall()
{
  const ON_UUID id = { 0xdbe22573, 0x8cad, 0x4ced,{ 0x89, 0x47, 0x3, 0xa0, 0x48, 0xed, 0xde, 0x56 } };

  ON_DimStyle dimstyle;
  DimStyleInit(L"Millimeter Small", -5, id, dimstyle);

  //dimstyle.SetDimScale(10.0);
  dimstyle.SetDimScale(1.0, ON::LengthUnitSystem::Millimeters, 10.0, ON::LengthUnitSystem::Millimeters);
  dimstyle.SetUnitSystem(ON::LengthUnitSystem::Millimeters);

  dimstyle.SetExtExtension(1.0);
  dimstyle.SetExtOffset(0.5);
  dimstyle.SetArrowSize(3.0);
  dimstyle.SetCenterMark(2.5);
  dimstyle.SetTextGap(0.8);
  dimstyle.SetTextHeight(2.5);
  dimstyle.SetBaselineSpacing(7.5);

  dimstyle.SetArrowType1(ON_Arrowhead::arrow_type::OpenArrow);
  dimstyle.SetArrowType2(ON_Arrowhead::arrow_type::OpenArrow);


  dimstyle.SetAngleFormat(ON_DimStyle::angle_format::DecimalDegrees);
  dimstyle.SetAngleResolution(1);

  dimstyle.SetDimensionLengthDisplay(ON_DimStyle::LengthDisplay::Millmeters);
  dimstyle.SetLengthResolution(2);
  dimstyle.SetLengthFactor(1.0);
  dimstyle.SetAlternate(false);
  dimstyle.SetAlternateDimensionLengthDisplay(ON_DimStyle::LengthDisplay::ModelUnits);
  dimstyle.SetAlternateLengthResolution(2);
  dimstyle.SetAlternateLengthFactor(1.0);

  dimstyle.SetDimExtension(0.0);

  dimstyle.SetLeaderArrowSize(3.0);
  dimstyle.SetLeaderArrowType(ON_Arrowhead::arrow_type::OpenArrow);
  dimstyle.SetLeaderLandingLength(3.0);

  //ValidateCpyStuff(dimstyle);

  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}


static ON_DimStyle DimStyleMillimeterLarge()
{
  const ON_UUID id = { 0xf7b30534, 0x773e, 0x45bc,{ 0x9d, 0x87, 0x9d, 0x14, 0x80, 0x9c, 0x96, 0x44 } };

  ON_DimStyle dimstyle;
  DimStyleInit(L"Millimeter Large", -6, id, dimstyle);

  //dimstyle.SetDimScale(100.0);
  dimstyle.SetDimScale(1.0, ON::LengthUnitSystem::Millimeters, 100.0, ON::LengthUnitSystem::Millimeters);
  dimstyle.SetUnitSystem(ON::LengthUnitSystem::Millimeters);

  dimstyle.SetExtExtension(1.0);
  dimstyle.SetExtOffset(0.5);
  dimstyle.SetArrowSize(3.5);
  dimstyle.SetCenterMark(3.5);
  dimstyle.SetTextGap(1.0);
  dimstyle.SetTextHeight(3.5);
  dimstyle.SetBaselineSpacing(10.5);

  dimstyle.SetArrowType1(ON_Arrowhead::arrow_type::OpenArrow);
  dimstyle.SetArrowType2(ON_Arrowhead::arrow_type::OpenArrow);


  dimstyle.SetAngleFormat(ON_DimStyle::angle_format::DecimalDegrees);
  dimstyle.SetAngleResolution(0);

  dimstyle.SetDimensionLengthDisplay(ON_DimStyle::LengthDisplay::Millmeters);
  dimstyle.SetLengthResolution(1);
  dimstyle.SetLengthFactor(1.0);
  dimstyle.SetAlternate(false);
  dimstyle.SetAlternateDimensionLengthDisplay(ON_DimStyle::LengthDisplay::ModelUnits);
  dimstyle.SetAlternateLengthResolution(2);
  dimstyle.SetAlternateLengthFactor(1.0);


  dimstyle.SetDimExtension(0.0);

  dimstyle.SetLeaderArrowSize(3.5);
  dimstyle.SetLeaderArrowType(ON_Arrowhead::arrow_type::OpenArrow);
  dimstyle.SetLeaderLandingLength(3.5);

  //ValidateCpyStuff(dimstyle);

  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}


static ON_DimStyle DimStyleMillimeterArchitecture()
{
  const ON_UUID id = { 0xe5a4c08f, 0x23b3, 0x4033,{ 0x90, 0xb2, 0xfb, 0x31, 0xec, 0x45, 0x92, 0x9b } };

  ON_DimStyle dimstyle;
  DimStyleInit(L"Millimeter Architectural", -7, id, dimstyle);
  dimstyle.SetUnitSystem(ON::LengthUnitSystem::Millimeters);

  //dimstyle.SetDimScale(100.0);
  dimstyle.SetDimScale(1.0, ON::LengthUnitSystem::Millimeters, 100.0, ON::LengthUnitSystem::Millimeters);

  dimstyle.SetExtExtension(1.0);
  dimstyle.SetExtOffset(0.5);
  dimstyle.SetArrowSize(3.0);
  dimstyle.SetCenterMark(3.0);
  dimstyle.SetTextGap(1.0);
  dimstyle.SetTextHeight(3.0);
  dimstyle.SetBaselineSpacing(9.0);

  dimstyle.SetArrowType1(ON_Arrowhead::arrow_type::Tick);
  dimstyle.SetArrowType2(ON_Arrowhead::arrow_type::Tick);

  dimstyle.SetAngleFormat(ON_DimStyle::angle_format::DecimalDegrees);
  dimstyle.SetAngleResolution(0);

  dimstyle.SetDimensionLengthDisplay(ON_DimStyle::LengthDisplay::Millmeters);
  dimstyle.SetLengthResolution(0);
  dimstyle.SetLengthFactor(1.0);
  dimstyle.SetAlternate(false);
  dimstyle.SetAlternateDimensionLengthDisplay(ON_DimStyle::LengthDisplay::ModelUnits);
  dimstyle.SetAlternateLengthResolution(2);
  dimstyle.SetAlternateLengthFactor(1.0);


  dimstyle.SetDimExtension(1.5);

  dimstyle.SetLeaderArrowSize(3.0);
  dimstyle.SetLeaderArrowType(ON_Arrowhead::arrow_type::OpenArrow);
  dimstyle.SetLeaderLandingLength(3.0);

  //ValidateCpyStuff(dimstyle);

  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}


const ON_DimStyle ON_DimStyle::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_DimStyle);
const ON_DimStyle ON_DimStyle::Default(DimStyleDefault());
const ON_DimStyle ON_DimStyle::DefaultInchDecimal(DimStyleInchDecimal());
const ON_DimStyle ON_DimStyle::DefaultInchFractional(DimStyleInchFractional());
const ON_DimStyle ON_DimStyle::DefaultFootInchArchitecture(DimStyleFootInchArchitecture());
const ON_DimStyle ON_DimStyle::DefaultMillimeterSmall(DimStyleMillimeterSmall());
const ON_DimStyle ON_DimStyle::DefaultMillimeterLarge(DimStyleMillimeterLarge());
const ON_DimStyle ON_DimStyle::DefaultMillimeterArchitecture(DimStyleMillimeterArchitecture());

const ON_StackedText ON_StackedText::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_StackedText);
const ON_TextRun ON_TextRun::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_TextRun);
const ON_TextRunArray ON_TextRunArray::EmptyArray ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_TextRunArray);
const ON_TextContent ON_TextContent::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_TextContent);

const ON_Text ON_Text::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Text);
const ON_Leader ON_Leader::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Leader);
const ON_DimLinear ON_DimLinear::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_DimLinear);
const ON_DimAngular ON_DimAngular::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_DimAngular);
const ON_DimRadial ON_DimRadial::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_DimRadial);
const ON_DimOrdinate ON_DimOrdinate::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_DimOrdinate);
const ON_Centermark ON_Centermark::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_Centermark);

const ON_HatchLine ON_HatchLine::Unset(ON_UNSET_VALUE);           // angle = unset
const ON_HatchLine ON_HatchLine::SolidHorizontal(0.0); // angle = 0
const ON_HatchLine ON_HatchLine::SolidVertical(ON_PI / 2);   // angle = pi/2

static ON_HatchPattern Internal_SolidHatchPatternInit()
{
  // {B319435D-86B6-4D89-972D-1F75FEF0D950}
  ON_UUID id = { 0xb319435d, 0x86b6, 0x4d89,{ 0x97, 0x2d, 0x1f, 0x75, 0xfe, 0xf0, 0xd9, 0x50 } };


  ON_HatchPattern hatch_pattern;
  hatch_pattern.SetFillType(ON_HatchPattern::HatchFillType::Solid);
  Internal_SystemModelComponentInit(id, -1, L"Solid", hatch_pattern);

  return hatch_pattern;
}

static ON_HatchPattern Internal_LineHatchPatternInit(
  const int hatch_index
  )
{
  ON_UUID id = ON_nil_uuid;
  ON_HatchLine lines[4];
  unsigned int line_count = 0;
  const wchar_t* name = nullptr;
  switch (hatch_index)
  {
  case -2:
  {
    // {3C25B9D3-0521-4120-8877-111FB5EA0016}
    const ON_UUID idctor = { 0x3c25b9d3, 0x521, 0x4120,{ 0x88, 0x77, 0x11, 0x1f, 0xb5, 0xea, 0x0, 0x16 } };
    id = idctor;
    name = L"Hatch1";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(ON_2dPoint::Origin);
    line0.SetOffset(ON_2dVector(0.0, 0.125));
  }
  break;

  case -3:
  {
    // {23287413-6AE6-4409-93DC-140796FA7864}
    const ON_UUID idctor = { 0x23287413, 0x6ae6, 0x4409,{ 0x93, 0xdc, 0x14, 0x7, 0x96, 0xfa, 0x78, 0x64 } };
    id = idctor;
    name = L"Hatch2";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(ON_2dPoint::Origin);
    line0.SetOffset(ON_2dVector(0.0, 0.375));

    ON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetBase(ON_2dPoint(0.0, 0.0625));
  }
  break;

  case -4:
  {
    // {282CBE0A-DB30-4241-BB5B-7290AF0BBD55}
    const ON_UUID idctor = { 0x282cbe0a, 0xdb30, 0x4241,{ 0xbb, 0x5b, 0x72, 0x90, 0xaf, 0xb, 0xbd, 0x55 } };
    id = idctor;
    name = L"Hatch3";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(ON_2dPoint::Origin);
    line0.SetOffset(ON_2dVector(0.0, 0.5));

    ON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetBase(ON_2dPoint(0.0, 0.0625));

    ON_HatchLine& line2 = lines[line_count++];
    line2 = line0;
    line2.SetBase(ON_2dPoint(0.0, 0.125));
  }
  break;

  case -5:
  {
    // {70710CA6-185D-42FE-AEB5-6437A8C15E7B}
    const ON_UUID idctor = { 0x70710ca6, 0x185d, 0x42fe,{ 0xae, 0xb5, 0x64, 0x37, 0xa8, 0xc1, 0x5e, 0x7b } };
    id = idctor;
    name = L"HatchDash";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(ON_2dPoint::Origin);
    line0.SetOffset(ON_2dVector(0.0, 0.25));

    ON_HatchLine& line1 = lines[line_count++];
    line1.SetAngleRadians(0.0);
    line1.SetBase(ON_2dPoint(0.0, 0.125));
    line1.SetOffset(ON_2dVector(0.0, 0.25));
    line1.AppendDash(0.125);
    line1.AppendDash(-0.125);
  }
  break;

  case -6:
  {
    // {86D5A4E7-AA8F-4FBA-BAA3-C844EF1704A1}
    const ON_UUID idctor = { 0x86d5a4e7, 0xaa8f, 0x4fba,{ 0xba, 0xa3, 0xc8, 0x44, 0xef, 0x17, 0x4, 0xa1 } };
    id = idctor;
    name = L"Grid";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(ON_2dPoint::Origin);
    line0.SetOffset(ON_2dVector(0.0, 0.25));

    ON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleRadians(ON_PI / 2.0);
  }
  break;

  case -7:
  {
    // {060226AB-2BA8-4550-BB0A-BC1A3CD8E2A1}
    const ON_UUID idctor = { 0x60226ab, 0x2ba8, 0x4550,{ 0xbb, 0xa, 0xbc, 0x1a, 0x3c, 0xd8, 0xe2, 0xa1 } };
    id = idctor;
    name = L"Grid60";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetOffset(ON_2dVector(0.0, 0.25));

    ON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleDegrees(60.0);

    ON_HatchLine& line2 = lines[line_count++];
    line2 = line0;
    line2.SetAngleDegrees(120.0);
  }
  break;

  case -8:
  {
    // {5FA0C0A0-B5F6-4799-ADED-AE202E260888}
    const ON_UUID idctor = { 0x5fa0c0a0, 0xb5f6, 0x4799,{ 0xad, 0xed, 0xae, 0x20, 0x2e, 0x26, 0x8, 0x88 } };
    id = idctor;
    name = L"Plus";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(ON_2dPoint(-0.125, 0.0));
    line0.SetOffset(ON_2dVector(0.0, 1.0));
    line0.AppendDash(0.25);
    line0.AppendDash(-0.75);

    ON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleRadians(ON_PI / 2.0);
    line1.SetBase(ON_2dPoint(0.0, -0.125));
  }
  break;

  case -9:
  {
    // {05B0404B-77AD-4206-8022-FD7D9D0ECECF}
    const ON_UUID idctor = { 0x5b0404b, 0x77ad, 0x4206,{ 0x80, 0x22, 0xfd, 0x7d, 0x9d, 0xe, 0xce, 0xcf } };
    id = idctor;
    name = L"Squares";
    ON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(ON_2dPoint::Origin);
    line0.SetOffset(ON_2dVector(0.0, 0.25));
    line0.AppendDash(0.25);
    line0.AppendDash(-0.25);

    ON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleRadians(ON_PI / 2.0);
  }
  break;
  }

  ON_HatchPattern hatch_pattern;
  hatch_pattern.SetHatchLines(line_count, lines);
  Internal_SystemModelComponentInit(id, hatch_index, name, hatch_pattern);
  return hatch_pattern;
}

const ON_HatchPattern ON_HatchPattern::Unset ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_HatchPattern);
const ON_HatchPattern ON_HatchPattern::Solid(Internal_SolidHatchPatternInit()); // index = -1, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::Hatch1(Internal_LineHatchPatternInit(-2));    // index = -2, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::Hatch2(Internal_LineHatchPatternInit(-3));    // index = -3, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::Hatch3(Internal_LineHatchPatternInit(-4));    // index = -4, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::HatchDash(Internal_LineHatchPatternInit(-5)); // index = -5, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::Grid(Internal_LineHatchPatternInit(-6));      // index = -6, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::Grid60(Internal_LineHatchPatternInit(-7));    // index = -7, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::Plus(Internal_LineHatchPatternInit(-8));      // index = -8, id set, unique and persistent
const ON_HatchPattern ON_HatchPattern::Squares(Internal_LineHatchPatternInit(-9));   // index = -9, id set, unique and persistent

#if defined(OPENNURBS_SUBD_WIP)

unsigned int ON_SubD::ErrorCount = 0;

static ON_SubDSectorLimitPoint ON_SubDSectorLimitPoint_Init(double x)
{
  ON_SubDSectorLimitPoint lp;
  memset(&lp, 0, sizeof(lp));

  lp.m_limitP[0] = x;
  lp.m_limitP[1] = x;
  lp.m_limitP[2] = x;

  lp.m_limitT1[0] = x;
  lp.m_limitT1[1] = x;
  lp.m_limitT1[2] = x;

  lp.m_limitT2[0] = x;
  lp.m_limitT2[1] = x;
  lp.m_limitT2[2] = x;

  lp.m_limitN[0] = x;
  lp.m_limitN[1] = x;
  lp.m_limitN[2] = x;

  return lp;
}

const ON_SubDSectorLimitPoint ON_SubDSectorLimitPoint::Unset = ON_SubDSectorLimitPoint_Init(ON_UNSET_VALUE);
const ON_SubDSectorLimitPoint ON_SubDSectorLimitPoint::Nan = ON_SubDSectorLimitPoint_Init(ON_DBL_QNAN);
const ON_SubDSectorLimitPoint ON_SubDSectorLimitPoint::Zero = ON_SubDSectorLimitPoint_Init(0.0);

const unsigned int ON_SubDLimitMeshFragment::MaximumSideSegmentCount = (1U << ON_SubDLimitMesh::MaximumDisplayDensity); // = 2^ON_SubDLimitMesh::MaximumDisplayDensity

const unsigned int ON_SubDSectorType::MaximumAngleIndex = 72;
const double ON_SubDSectorType::IgnoredCornerSectorAngle = 0.0;
const double ON_SubDSectorType::UnsetCornerSectorAngle = -8881.0;
const double ON_SubDSectorType::ErrorCornerSectorAngle = -9991.0;

const double ON_SubDSectorType::SmoothSectorTheta = 0.5*ON_PI;
const double ON_SubDSectorType::UnsetSectorTheta = -8882.0;
const double ON_SubDSectorType::ErrorSectorTheta = -9992.0;

const double ON_SubDSectorType::IgnoredSectorWeight = 0.0;
const double ON_SubDSectorType::UnsetSectorWeight = -8883.0;
const double ON_SubDSectorType::ErrorSectorWeight = -9993.0;

const unsigned int ON_SubDVertex::MaximumEdgeCount = 0xFFF0U;
const unsigned int ON_SubDVertex::MaximumFaceCount = 0xFFF0U;
const unsigned int ON_SubDEdge::MaximumFaceCount = 0xFFF0U;
const unsigned int ON_SubDFace::MaximumEdgeCount = 0xFFF0U;



const ON_ComponentStatus ON_ComponentStatus::NoneSet = ON_ComponentStatus(ON_ComponentState::Clear);
const ON_ComponentStatus ON_ComponentStatus::Selected = ON_ComponentStatus(ON_ComponentState::Selected);
const ON_ComponentStatus ON_ComponentStatus::SelectedPersistent = ON_ComponentStatus(ON_ComponentState::SelectedPersistent);
const ON_ComponentStatus ON_ComponentStatus::Highlighted = ON_ComponentStatus(ON_ComponentState::Highlighted);
const ON_ComponentStatus ON_ComponentStatus::Hidden = ON_ComponentStatus(ON_ComponentState::Hidden);
const ON_ComponentStatus ON_ComponentStatus::Locked = ON_ComponentStatus(ON_ComponentState::Locked);
const ON_ComponentStatus ON_ComponentStatus::Deleted = ON_ComponentStatus(ON_ComponentState::Deleted);
const ON_ComponentStatus ON_ComponentStatus::Damaged = ON_ComponentStatus(ON_ComponentState::Damaged);
static ON_ComponentStatus ON_ComponentStatus_AllSet()
{
  ON_ComponentStatus s;
  s.SetStates(ON_ComponentStatus::SelectedPersistent);
  s.SetStates(ON_ComponentStatus::Highlighted);
  s.SetStates(ON_ComponentStatus::Hidden);
  s.SetStates(ON_ComponentStatus::Locked);
  s.SetStates(ON_ComponentStatus::Damaged);
  return s;
}
const ON_ComponentStatus ON_ComponentStatus::AllSet = ON_ComponentStatus_AllSet();

static ON_AggregateComponentStatus ON_AggregateComponentStatus_Empty()
{
  ON_AggregateComponentStatus s;
  memset(&s, 0, sizeof(s));
  return s;
}
const ON_AggregateComponentStatus ON_AggregateComponentStatus::Empty = ON_AggregateComponentStatus_Empty();

static ON_AggregateComponentStatus ON_AggregateComponentStatus_NotCurrent()
{
  ON_AggregateComponentStatus s;
  memset(&s, 0, sizeof(s));
  s.MarkAsNotCurrent();
  return s;
}
const ON_AggregateComponentStatus ON_AggregateComponentStatus::NotCurrent = ON_AggregateComponentStatus_NotCurrent();

const ON_SubDComponentPtr ON_SubDComponentPtr::Null = { 0 };

const ON_SubDComponentPoint ON_SubDComponentPoint::Unset = ON_SubDComponentPoint();

static ON_SubDLimitMeshFragmentGrid EmptyLimitMeshFragmentGridInit()
{
  ON_SubDLimitMeshFragmentGrid empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

static ON_SubDLimitMeshFragment EmptyLimitMeshFragmentInit()
{
  ON_SubDLimitMeshFragment empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

const ON_SubDLimitMeshFragmentGrid ON_SubDLimitMeshFragmentGrid::Empty = EmptyLimitMeshFragmentGridInit();
const ON_SubDLimitMeshFragment ON_SubDLimitMeshFragment::Empty = EmptyLimitMeshFragmentInit();

ON_SubDLimitPatchFragment ON_SubDLimitPatchFragment_Init(double x)
{
  ON_SubDLimitPatchFragment pf;
  memset(&pf, 0, sizeof(pf));
  if (!(0.0 == x))
  {
    double* p = &pf.m_patch_cv[0][0][0];
    double* p1 = p + sizeof(pf.m_patch_cv) / sizeof(p[0]);
    while (p < p1)
      *p++ = x;
  }
  return pf;
}

const ON_SubDLimitPatchFragment ON_SubDLimitPatchFragment::Empty = ON_SubDLimitPatchFragment_Init(0.0);
const ON_SubDLimitPatchFragment ON_SubDLimitPatchFragment::Unset = ON_SubDLimitPatchFragment_Init(ON_UNSET_VALUE);
const ON_SubDLimitPatchFragment ON_SubDLimitPatchFragment::Nan = ON_SubDLimitPatchFragment_Init(ON_DBL_QNAN);

static ON_SubDComponentBase UnsetComponentBaseInit()
{
  // For efficiency, ON_SubDComponentBase() does not waste time
  // m_cache_subd_P[], m_displacementV[]
  // but the offical "Unset" ON_SubDComponentBase should have every byte initialized.
  ON_SubDComponentBase unset;
  memset(&unset, 0, sizeof(unset));
  return unset;
}

static ON_SubDVertex EmptyVertexInit()
{
  // For efficiency, ON_SubDVertex() does not waste time
  // initializing m_limitP[], ..., m_cache_subd_P[], m_displacementV[]
  // but the offical "Empty" vertex should have every byte initialized.
  ON_SubDVertex empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

static ON_SubDEdge EmptyEdgeInit()
{
  // For efficiency, ON_SubDEdge() does not waste time
  // initializing m_cache_subd_P[], m_displacementV[]
  // but the offical "Empty" edge should have every byte initialized.
  ON_SubDEdge empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

static ON_SubDFace EmptyFaceInit()
{
  // For efficiency, ON_SubDFace() does not waste time
  // initializing m_cache_subd_P[], m_displacementV[]
  // but the offical "Empty" face should have every byte initialized.
  ON_SubDFace empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

const ON_SubDComponentBase ON_SubDComponentBase::Unset = UnsetComponentBaseInit();
const ON_SubDVertex ON_SubDVertex::Empty = EmptyVertexInit();
const ON_SubDEdge ON_SubDEdge::Empty = EmptyEdgeInit();
const ON_SubDFace ON_SubDFace::Empty = EmptyFaceInit();

const ON_SubDDisplayParameters ON_SubDDisplayParameters::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDDisplayParameters);
const ON_SubDDisplayParameters ON_SubDDisplayParameters::DefaultDisplayMeshParameters = ON_SubDDisplayParameters::CreateFromDisplayDensity(ON_SubDLimitMesh::DefaultDisplayDensity);

const ON_SubD ON_SubD::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubD);
const ON_SubDRef ON_SubDRef::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDRef);
const ON_SubDLimitMesh ON_SubDLimitMesh::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDLimitMesh);
const ON_SubDSectorType ON_SubDSectorType::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDSectorType);
const ON_SubDMatrix ON_SubDMatrix::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDMatrix);
const ON_SubDComponentRef ON_SubDComponentRef::Empty ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDComponentRef);

static ON_SubDFromMeshOptions ON_SubDCreaseParameters_CreaseAt(
  ON_SubDFromMeshOptions::InteriorCreaseOption crease_type
  )
{
  ON_SubDFromMeshOptions cp;
  cp.SetInteriorCreaseOption(crease_type);
  return cp;
}

static ON_SubDFromMeshOptions ON_SubDCreaseParameters_ConvexCorners()
{
  ON_SubDFromMeshOptions cp;
  cp.SetConvexCornerOption(ON_SubDFromMeshOptions::ConvexCornerOption::AtMeshCorner);
  return cp;
}

const ON_SubDFromMeshOptions ON_SubDFromMeshOptions::Smooth ON_CLANG_CONSTRUCTOR_BUG_INIT(ON_SubDFromMeshOptions);
const ON_SubDFromMeshOptions ON_SubDFromMeshOptions::InteriorCreaseAtMeshCrease = ON_SubDCreaseParameters_CreaseAt(ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshCrease);
const ON_SubDFromMeshOptions ON_SubDFromMeshOptions::InteriorCreaseAtMeshEdge = ON_SubDCreaseParameters_CreaseAt(ON_SubDFromMeshOptions::InteriorCreaseOption::AtMeshEdge);
const ON_SubDFromMeshOptions ON_SubDFromMeshOptions::ConvexCornerAtMeshCorner = ON_SubDCreaseParameters_ConvexCorners();

#endif

unsigned int ON_ModelComponent::Internal_SystemComponentHelper()
{
  static unsigned int rc = 0;
  if (0 == rc)
  {
    rc = 1;
    const ON_ModelComponent* list[] =
    {
      &ON_ModelComponent::Unset,
      &ON_InstanceDefinition::Unset,
      &ON_Linetype::Unset,
      &ON_Layer::Unset,
      &ON_TextStyle::Unset,
      &ON_DimStyle::Unset,
      &ON_Material::Unset,
      &ON_Material::Default,
      &ON_TextureMapping::Unset,
      &ON_TextureMapping::SurfaceParameterTextureMapping,
      &ON_HatchPattern::Unset,
      &ON_Group::Unset,
      &ON_HistoryRecord::Empty
    };

    const ON_ModelComponent* list_localized_name[] =
    {
      &ON_Linetype::Continuous,
      &ON_Linetype::ByLayer,
      &ON_Linetype::ByParent,

      &ON_Layer::Default,

      &ON_TextStyle::Default,
      &ON_TextStyle::ByLayer,
      &ON_TextStyle::ByParent,

      &ON_DimStyle::Default,
      &ON_DimStyle::DefaultInchDecimal,
      &ON_DimStyle::DefaultInchFractional,
      &ON_DimStyle::DefaultFootInchArchitecture,
      &ON_DimStyle::DefaultMillimeterSmall,
      &ON_DimStyle::DefaultMillimeterLarge,
      &ON_DimStyle::DefaultMillimeterArchitecture,

      &ON_HatchPattern::Solid,
      &ON_HatchPattern::Hatch1,
      &ON_HatchPattern::Hatch2,
      &ON_HatchPattern::Hatch3,
      &ON_HatchPattern::HatchDash,
      &ON_HatchPattern::Grid,
      &ON_HatchPattern::Grid60,
      &ON_HatchPattern::Plus,
      &ON_HatchPattern::Squares
    };

    const size_t list_count = sizeof(list) / sizeof(list[0]);
    for (size_t i = 0; i < list_count; i++)
    {
      *(const_cast<unsigned short*>(&list[i]->m_locked_status)) = 0xFFFFU;
      *(const_cast<ON__UINT64*>(&list[i]->m_content_version_number)) = 0;
    }

    const unsigned int name_bit = ON_ModelComponent::Attributes::NameAttribute;
    const unsigned int name_mask = ~name_bit;
    const unsigned short bits
      = (unsigned short)((name_mask & ON_ModelComponent::Attributes::AllAttributes) | ON_ModelComponent::Attributes::SystemComponentAttribute);
    const size_t list_localized_name_count = sizeof(list_localized_name) / sizeof(list_localized_name[0]);
    for (size_t i = 0; i < list_localized_name_count; i++)
    {
      *(const_cast<unsigned short*>(&list_localized_name[i]->m_locked_status)) = bits;
      *(const_cast<ON__UINT64*>(&list_localized_name[i]->m_content_version_number)) = 0;
    }

    rc = (unsigned int)(list_count + list_localized_name_count);
  }
  return rc;
}

