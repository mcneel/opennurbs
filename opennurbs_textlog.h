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

#if !defined(ON_TEXTLOG_INC_)
#define ON_TEXTLOG_INC_

class ON_CLASS ON_TextLog
{
public:
  /*
  Description:
    Create a text log that dumps to the virtual function
    void ON_TextLog::AppendText().
  */
  ON_TextLog();

  /*
  Description:
    Create a text log that dumps to an ASCII file.
  Parameters:
    fp - [in] Pointer to an open ASCII text file.  The file
              pointer must remain valid as long as the text
              log is in use.
  */
  ON_TextLog( FILE* fp); // dump to open ASCII text file

  /*
  Description:
    Create a text log that dumps to a string.
  Parameters:
    s - [in] String that must exist as long as
             the text log is in use.
  */
  ON_TextLog( ON_wString& s );

  /*
  Description:
    ON_TextLog::Null is a silent text log and can be used when no output
    is desired but an ON_TextLog parameter is required.
  */
  static ON_TextLog Null;

  bool IsTextHash() const;

public:
  virtual ~ON_TextLog();

  void SetDoubleFormat( const char* ); // default is %g
  void GetDoubleFormat( ON_String& ) const;

  void SetFloatFormat( const char* ); // default is %g
  void GetFloatFormat( ON_String& ) const;

  void PushIndent();
  void PopIndent();
  int IndentSize() const; //  0: one tab per indent
                          // >0: number of spaces per indent
  void SetIndentSize(int);

  /*
  Returns:
    Current indentation count
  */
  int IndentCount();
  /*
  Description:
    Set indentation count.
  */
  void SetIndentCount(
    int indent_count
  );
  
  void PrintWrappedText( const char*, int = 60 );    // last arg is maximum line length
  void PrintWrappedText( const wchar_t*, int = 60 ); // last arg is maximum line length

  /*
  Description:
    Print a formatted ASCII string of up to 2000 characters.
  Parameters:
    format - [in] nullptr terminated format control string 
  Remarks:
    To print strings longer than 2000 characters, you must
    use ON_TextLog::PrintString.
  See Also:
    ON_TextLog::PrintString
  */
  void ON_VARGS_FUNC_CDECL Print(const char* format, ...);

  /*
  Description:
    Print a formatted INICODE string of up to 2000 characters.
  Parameters:
    format - [in] nullptr terminated format control string 
  Remarks:
    To print strings longer than 2000 characters, you must
    use ON_TextLog::PrintString.
  See Also:
    ON_TextLog::PrintString
  */
  void ON_VARGS_FUNC_CDECL Print(const wchar_t* format, ...);

  void Print( float );
  void Print( double );
  void Print( const ON_2dPoint& );
  void Print( const ON_3dPoint& );
  void Print( const ON_4dPoint& );
  void Print( const ON_2dVector& );
  void Print( const ON_3dVector& );
  void Print( const ON_Xform& );
  void Print( const ON_UUID& );
  void Print( const ON_COMPONENT_INDEX& );

  /*
  Description:
    Print an unformatted wide char string of any length.
  Parameters:
    string - [in]
  */
  void Print( const ON_wString& string );

  /*
  Description:
    Print an unformatted UTF-8 string of any length.
  Parameters:
    string - [in]
  */
  void Print( const ON_String& string );

  void Print( const ON_3dPointArray&, const char* = nullptr );
  void Print( 
         const ON_Matrix&, 
         const char* = nullptr, // optional preamble
         int = 0             // optional number precision
    );

  // printing utilities
  /*
  Description:
    Same as calling Print("\n");
  */
  void PrintNewLine();

  /*
  Description:
    Print an unformatted ASCII string of any length.
  Parameters:
    s - [in] nullptr terminated ASCII string.
  */
  void PrintString( const char* s );

  /*
  Description:
    Print an unformatted UNICODE string of any length.
  Parameters:
    s - [in] nullptr terminated UNICODE string.
  */
  void PrintString( const wchar_t* s );

  void PrintRGB( const ON_Color& );

  void PrintTime( const struct tm& );

  void PrintPointList( 
    int,               // dim
    bool,              // true for rational points
    int,               // count
    int,               // stride
    const double*,     // point[] array
    const char* = nullptr // optional preabmle
    );

  void PrintPointGrid( 
    int,               // dim
    bool,              // true for rational points
    int, int,          // point_count0, point_count1
    int, int,          // point_stride0, point_stride1
    const double*,     // point[] array
    const char* = nullptr // optional preabmle
    );
    
  void PrintKnotVector( 
    int,             // order
    int,             // cv_count
    const double*    // knot[] array
    );

  ON_TextLog& operator<<( const char* );
  ON_TextLog& operator<<( char );
  ON_TextLog& operator<<( short );
  ON_TextLog& operator<<( int );
  ON_TextLog& operator<<( float );
  ON_TextLog& operator<<( double );
  ON_TextLog& operator<<( const ON_2dPoint& );
  ON_TextLog& operator<<( const ON_3dPoint& );
  ON_TextLog& operator<<( const ON_4dPoint& );
  ON_TextLog& operator<<( const ON_2dVector& );
  ON_TextLog& operator<<( const ON_3dVector& );
  ON_TextLog& operator<<( const ON_Xform& );

protected:
  friend class ON_TextHash;

  FILE* m_pFile;
  ON_wString* m_pString;

  
  /*
  Description:
    If the ON_TextLog(ON_wString& wstr) constructor was used, the
    default appends s to wstr.  If the ON_TextLog(FILE* fp) 
    constructor was used, the default calls fputs( fp, s).
    In all other cases, the default calls printf("%s",s).
  Parameters:
    s - [in];
  */
  virtual
  void AppendText(
        const char* s
        );

  /*
  Description:
    If the ON_TextLog(ON_wString& wstr) constructor was used, the
    default appends s to wstr.  In all other cases, the default 
    converts the string to an ON_String and calls the ASCII
    version AppendText(const char*).
  Parameters:
    s - [in];
  */
  virtual
  void AppendText(
        const wchar_t* s
        );
                  
private:
  ON_String m_indent;
  ON_String m_double_format;
  ON_String m_double2_format;
  ON_String m_double3_format;
  ON_String m_double4_format;
  ON_String m_float_format;
  ON_String m_float2_format;
  ON_String m_float3_format;
  ON_String m_float4_format;

  ON_String m_line;

  int m_beginning_of_line = 0; // 0

  // size of a single indentation
  int m_indent_size = 0;       // 0 use tabs, > 0 = number of spaces per indent level

  // Number of indentations at the start of a new line
  int m_indent_count = 0;

private:
  ON_TextLog( const ON_TextLog& ) = delete;
  ON_TextLog& operator=( const ON_TextLog& ) = delete;
};

/*
Description:
  ON_TextLogIndent is a class used with ON_TextLog to
  push and pop indentation.
*/
class ON_CLASS ON_TextLogIndent
{
public:
    // The constructor calls text_log.PushIndent()
    // and the destuctor calls text_log.PopIndent()
  ON_TextLogIndent(
    class ON_TextLog& text_log
    );

    // If bEnabled is true, the constructor calls text_log.PushIndent()
    // and the destuctor calls text_log.PopIndent()
    ON_TextLogIndent(
      class ON_TextLog& text_log,
      bool bEnabled
    );

  ~ON_TextLogIndent();

private:
  class ON_TextLog& m_text_log;
  bool m_bEnabled;

  // prevent use of copy construction and operator=
  // (no implementations)
  ON_TextLogIndent(const ON_TextLogIndent&);
  ON_TextLogIndent& operator=(const ON_TextLogIndent&);
};

class ON_CLASS ON_TextHash : public ON_TextLog
{
public:
  ON_TextHash() = default;
  ~ON_TextHash() = default;

private:
  ON_TextHash(const ON_TextHash&) = delete;
  ON_TextHash& operator= (const ON_TextHash&) = delete;

public:

  ON_StringMapType StringMapType() const;

  const class ON_Locale& StringMapLocale() const;

  void SetStringMap(
    const class ON_Locale& locale,
    ON_StringMapType map_type
  );

  void SetStringMap(
    ON_StringMapOrdinalType map_type
  );

  /*
  Parameters:
    bEnableIdRemap - [in]
    if bEnableIdRemap is true, the sequences of
    code points that match the format
    XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    where X is a hexadecimal digit (0-9, a-f, or A-F)
    and the hyphen is literal.
    will be replaced with an id created by 
    ON_NextNotUniqueId(). 
    This is used for comparing code that generates streams
    containg new uuids.
  */
  void SetIdRemap(
    bool bEnableIdRemap
  );

  /*
  Returns:
    True if id remap is available.
  */
  bool IdRemap() const;

  /*
  Description:
    In some testing situations, the output text log can be set 
    when it is necessary to see the text used to compute the 
    SHA-1 hash. The has can be caluculate which no output text
    log.

  Parameters:
    output_text_log - [in]
      Destination text log for the mtext used to calculate the hash.
  */
  void SetOutputTextLog(
    ON_TextLog* output_text_log
  );

  ON_TextLog* OutputTextLog() const;

  /*
  Returns:
    Total number of bytes that have passed through this text log.
  */
  ON__UINT64 ByteCount() const;

  /*
  Returns:
    SHA-1 hash value of the text sent to this text log.
  */
  ON_SHA1_Hash Hash() const;

private:
  void AppendText(const char* s) override;
  void AppendText(const wchar_t* s) override;

private:
  bool m_bApplyStringMap = false;
  bool m_bApplyIdRemap = false;
  
  ON_UUID m_remap_id = ON_nil_uuid;
  ON_UuidPairList m_remap_id_list;

  ON_StringMapType m_string_map_type = ON_StringMapType::Identity;
  ON_StringMapOrdinalType m_string_map_ordinal_type = ON_StringMapOrdinalType::Identity;
  ON_Locale m_string_map_local = ON_Locale::InvariantCulture;

  ON_TextLog* m_output_text_log = nullptr;

  static const char* Internal_ParseId(
    const char* s,
    ON_UUID* id
  );

  static bool Internal_IsHexDigit(char c);

  ON_SHA1 m_sha1;
};


#endif
