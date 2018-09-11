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

/////////////////////////////////////////////////////////////////////////////
// Empty strings point at empty_astring


struct ON_aStringHeader
{
	int   ref_count;       // reference count (>=0 or -1 for empty string)
	int   string_length;   // does not include nullptr terminator
	int   string_capacity; // does not include nullptr terminator
  char* string_array() {return (char*)(this+1);}
};

static struct {
  ON_aStringHeader header;
  char           s;  
} empty_astring = { {-1, 0, 0}, 0 }; // ref_count=-1, length=0, capacity=0, s=0 
static ON_aStringHeader* pEmptyStringHeader = &empty_astring.header;
static const char* pEmptyaString = &empty_astring.s;

//////////////////////////////////////////////////////////////////////////////
// protected helpers

void ON_String::Create()
{
  m_s = (char*)pEmptyaString;
}

void ON_String::Destroy()
{
  ON_aStringHeader* p = Header();
  if ( p != pEmptyStringHeader && p->ref_count > 0 ) {
    p->ref_count--;
		if ( p->ref_count == 0 )
			onfree(p);
  }
	Create();
}

void ON_String::Empty()
{
  ON_aStringHeader* p = Header();
  if ( p != pEmptyStringHeader ) {
    if ( p->ref_count > 1 ) {
      // string memory is shared
      p->ref_count--;
	    Create();
    }
    else if ( p->ref_count == 1 ) {
      // string memory is not shared - reuse it
      if (m_s && p->string_capacity>0)
        *m_s = 0;
      p->string_length = 0;
    }
    else {
      // should not happen
      ON_ERROR("ON_String::Empty() encountered invalid header - fixed.");
      Create();
    }
  }
  else {
    // initialized again
	  Create();
  }
}

void ON_String::EmergencyDestroy()
{
	Create();
}

void ON_String::EnableReferenceCounting( bool bEnable )
{
  // TODO fill this in;
}

bool ON_String::IsReferenceCounted() const
{
  return true;
}

ON_aStringHeader* ON_String::Header() const
{
  ON_aStringHeader* p = (ON_aStringHeader*)m_s;
  if (p)
    p--;
  else
    p = pEmptyStringHeader;
  return p;
}

char* ON_String::CreateArray( int capacity )
{
  Destroy();
  if ( capacity > 0 ) {
		ON_aStringHeader* p =
			(ON_aStringHeader*)onmalloc( sizeof(ON_aStringHeader) + (capacity+1)*sizeof(*m_s) );
		p->ref_count = 1;
		p->string_length = 0;
		p->string_capacity = capacity;
		m_s = p->string_array();
    memset( m_s, 0, (capacity+1)*sizeof(*m_s) );
    return m_s;
  }
  return nullptr;
}

void ON_String::CopyArray()
{
  // If 2 or more strings are using the array, it is duplicated.
  // Call CopyArray() before modifying array contents.
  ON_aStringHeader* p = Header();
  if ( p != pEmptyStringHeader && p && p->ref_count > 1 ) 
  {
    const char* s = m_s;
    // p and s remain valid after Destroy() because
    // will simply be decremented and no deallocation
    // will happen.
    Destroy();
    CopyToArray( p->string_capacity, s );
    if ( p->string_length < p->string_capacity )
    {
      Header()->string_length = p->string_length;
    }
  }
}

char* ON_String::ReserveArray( size_t array_capacity )
{
  ON_aStringHeader* p = Header();
  const int capacity = (int) array_capacity;
  if ( p == pEmptyStringHeader ) 
  {
		CreateArray(capacity);
  }
  else if ( p->ref_count > 1 ) 
  {
		CreateArray(capacity);
    ON_aStringHeader* p1 = Header();
    const int size = (capacity < p->string_length) ? capacity : p->string_length;
    if ( size > 0 ) 
    {
      memcpy( p1->string_array(), p->string_array(), size*sizeof(*m_s) );
      p1->string_length = size;
    }
  }
	else if ( capacity > p->string_capacity ) 
  {
		p = (ON_aStringHeader*)onrealloc( p, sizeof(ON_aStringHeader) + (capacity+1)*sizeof(*m_s) );
    m_s = p->string_array();
    memset( &m_s[p->string_capacity], 0, (1+capacity-p->string_capacity)*sizeof(*m_s) );
    p->string_capacity = capacity;
	}
  return Array();
}

void ON_String::ShrinkArray()
{
  ON_aStringHeader* p = Header();
  if ( p != pEmptyStringHeader ) {
    if ( p->string_length < 1 ) {
      Destroy();
    }
    else if ( p->ref_count > 1 ) {
      // shared string
      CreateArray(p->string_length);
		  ON_aStringHeader* p1 = Header();
      memcpy( m_s, p->string_array(), p->string_length*sizeof(*m_s));
      p1->string_length = p->string_length;
      m_s[p1->string_length] = 0;
    }
	  else if ( p->string_length < p->string_capacity ) {
      // onrealloc string
		  p = (ON_aStringHeader*)onrealloc( p, sizeof(ON_aStringHeader) + (p->string_length+1)*sizeof(*m_s) );
      p->string_capacity = p->string_length;
      m_s = p->string_array();
      m_s[p->string_length] = 0;
	  }
  }
}

void ON_String::CopyToArray( const ON_String& s )
{
  CopyToArray( s.Length(), s.Array() );
}

void ON_String::CopyToArray( int size, const char* s )
{
  if ( size > 0 && s && s[0] ) {
	  ReserveArray(size);
	  memcpy(m_s, s, size*sizeof(*m_s));
	  Header()->string_length = size;
    m_s[Header()->string_length] = 0;
  }
  else {
    if ( Header()->ref_count > 1 )
      Destroy();
    else {
      Header()->string_length = 0;
      m_s[0] = 0;
    }
  }
}

void ON_String::CopyToArray( int size, const unsigned char* s )
{
  CopyToArray( size, ((char*)s) );
}

void ON_String::AppendToArray( const ON_String& s )
{
  AppendToArray( s.Length(), s.Array() );
}

void ON_String::AppendToArray( int size, const char* s )
{
  if ( size > 0 && s && s[0] ) {
	  ReserveArray(size + Header()->string_length );
    // m_s = char array
	  memcpy(&m_s[Header()->string_length], s, size*sizeof(*m_s));
	  Header()->string_length += size;
    m_s[Header()->string_length] = 0;
  }
}

void ON_String::AppendToArray( int size, const unsigned char* s )
{
  AppendToArray( size, ((char*)s) );
}

int ON_String::Length( const char* s )
{
  size_t slen = s ? strlen(s) : 0;
  int n = ((0 < slen && slen <= 2147483645) ?((int)slen) : 0); // the (int) cast is for 64 bit size_t conversion
  return n;
}

unsigned int ON_String::UnsignedLength( const char* s )
{
  return (unsigned int)ON_String::Length( s );
}

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction


ON_String::ON_String() ON_NOEXCEPT
{
	Create();
}

ON_String::~ON_String()
{
  Destroy();
}

ON_String::ON_String(const ON_String& src)
{
	if ( src.Header()->ref_count > 0 )	
  {
		m_s = src.m_s;
    src.Header()->ref_count++;
	}
	else 
  {
		Create();
		*this = src.m_s; // use operator=(const char*) to copy
	}
}

#if defined(ON_HAS_RVALUEREF)

// Clone constructor
ON_String::ON_String( ON_String&& src ) ON_NOEXCEPT
{
  m_s = src.m_s;
  src.m_s = (char*)pEmptyaString;
}

// Clone Assignment operator
ON_String& ON_String::operator=( ON_String&& src ) ON_NOEXCEPT
{
  if ( this != &src )
  {
    this->Destroy();
    m_s = src.m_s;
    src.m_s = (char*)pEmptyaString;
  }
  return *this;
}

#endif


ON_String::ON_String( const char* s )
{
	Create();
  if ( s && s[0] ) {
    CopyToArray( (int)strlen(s), s ); // the (int) is for 64 bit size_t conversion
  }
}

ON_String::ON_String( const char* s, int length )
{
	Create();
  if ( s && length > 0 ) {
    CopyToArray(length,s);
	}
}

ON_String::ON_String( char c, int repeat_count )
{
  Create();
  if ( repeat_count > 0 ) {
    ReserveArray(repeat_count);
    memset( m_s, c, repeat_count*sizeof(*m_s) );
    m_s[repeat_count] = 0;
    Header()->string_length = repeat_count;
  }
}

ON_String::ON_String( const unsigned char* s )
{
	Create();
  if ( s && s[0] ) 
  {
    CopyToArray( (int)strlen((const char*)s), (const char*)s ); // the (int) is for 64 bit size_t conversion
  }
}

ON_String::ON_String( const unsigned char* s, int length )
{
	Create();
  if ( s && length > 0 ) {
    CopyToArray(length,s);
	}
}

ON_String::ON_String( unsigned char c, int repeat_count )
{
  Create();
  if ( repeat_count > 0 ) {
    ReserveArray(repeat_count);
    memset( m_s, c, repeat_count*sizeof(*m_s) );
    m_s[repeat_count] = 0;
    Header()->string_length = repeat_count;
  }
}


ON_String::ON_String( const wchar_t* w)
{
  Create();
  if ( w && w[0] ) {
    *this = w;
  }
}

ON_String::ON_String( const wchar_t* w, int w_length )
{
  // from substring
  Create();
  if ( w && w[0] ) 
  {
    CopyToArray( w_length, w );
  }
}


ON_String::ON_String( const ON_wString& w )
{
  Create();
  *this = w;
}



#if defined (ON_RUNTIME_WIN)
bool ON_String::LoadResourceString( HINSTANCE instance, UINT id )
{
  char s[2048]; // room for 2047 characters
  int length;

  Destroy();
  length = ::LoadStringA( instance, id, s, 2047 );
  if ( length > 0 && length < 2048 ) {
    CopyToArray( length, s );
  }
  return (length > 0 );
}
#endif

int ON_String::Length() const
{
  return Header()->string_length;
}

unsigned int ON_String::UnsignedLength() const
{
  return (unsigned int)Length();
}

// 25 October 2007 Dale Lear - remove bogus decl and defn
//void Destroy();
//void EmergencyDestroy()
//{
//}

ON_String::operator const char*() const
{
  return ( nullptr == m_s || m_s == pEmptyaString ) ? "" : m_s;
}

char& ON_String::operator[](int i)
{
  CopyArray();
  return m_s[i];
}

char ON_String::operator[](int i) const
{
  return m_s[i];
}

bool ON_String::IsEmpty() const
{
  return (Header()->string_length <= 0) ? true : false;
}

bool ON_String::IsNotEmpty() const
{
  return (Header()->string_length > 0) ? true : false;
}

ON_String& ON_String::operator=(const ON_String& src)
{
	if (m_s != src.m_s)	
  {
    if ( src.IsEmpty() ) 
    {
      Destroy();
      Create();
    }
    else if ( src.Header()->ref_count > 0 ) 
    {
      Destroy();
      src.Header()->ref_count++;
      m_s = src.m_s;
    }
    else 
    {
      ReserveArray(src.Length());
      memcpy( m_s, src.Array(), src.Length()*sizeof(*m_s));
      Header()->string_length = src.Length();
    }
  }
	return *this;
}

ON_String& ON_String::operator=( char c )
{
	CopyToArray( 1, &c );
	return *this;
}

ON_String& ON_String::operator=( const char* s )
{
  if ( (void*)s != (void*)m_s )
	  CopyToArray( ON_String::Length(s), s);
	return *this;
}

ON_String& ON_String::operator=( unsigned char c )
{
	CopyToArray( 1, &c );
	return *this;
}

ON_String& ON_String::operator=( const unsigned char* s )
{
  if ( (void*)s != (void*)m_s )
	  CopyToArray( ON_String::Length((const char*)s), s);
	return *this;
}

ON_String& ON_String::operator=( const wchar_t* w )
{
  // converts wide string to byte string
  const int w_length = ON_wString::Length(w);
  CopyToArray( w_length, w);
	return *this;
}

ON_String& ON_String::operator=( const ON_wString& w )
{
  *this = w.Array();
  return *this;
}


ON_String ON_String::operator+(const ON_String& s2) const
{
	ON_String s(*this);
  s.AppendToArray( s2 );
	return s;
}

ON_String ON_String::operator+( char s2 ) const
{
	ON_String s(*this);
  s.AppendToArray( 1, &s2 );
	return s;
}

ON_String ON_String::operator+( unsigned char s2 ) const
{
	ON_String s(*this);
  s.AppendToArray( 1, &s2 );
	return s;
}

ON_String ON_String::operator+(const char* s2) const
{
	ON_String s(*this);
  s.AppendToArray( ON_String::Length(s2), s2 );
	return s;
}

ON_String ON_String::operator+( const unsigned char* s2) const
{
	ON_String s(*this);
  s.AppendToArray( ON_String::Length((const char*)s2), s2 );
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// operator+=()


void ON_String::Append( const char* s , int count )
{
  // append specified number of characters
  if ( s && count > 0 )
    AppendToArray(count,s);
}

void ON_String::Append( const unsigned char* s , int count )
{
  // append specified number of characters
  if ( s && count > 0 )
    AppendToArray(count,s);
}


const ON_String& ON_String::operator+=(const ON_String& s)
{
  AppendToArray(s);
	return *this;
}

const ON_String& ON_String::operator+=( char s )
{
  AppendToArray(1,&s);
	return *this;
}

const ON_String& ON_String::operator+=( unsigned char s )
{
  AppendToArray(1,&s);
	return *this;
}

const ON_String& ON_String::operator+=( const char* s )
{
  AppendToArray(Length(s),s);
	return *this;
}

const ON_String& ON_String::operator+=( const unsigned char* s )
{
  AppendToArray(ON_String::Length((const char*)s),s);
	return *this;
}

char* ON_String::SetLength(size_t string_length)
{
  int length = (int)string_length; // for 64 bit compilers
  if ( length >= Header()->string_capacity ) {
    ReserveArray(length);
  }
  if ( length >= 0 && length <= Header()->string_capacity ) {
    CopyArray();
    Header()->string_length = length;
    m_s[length] = 0;
    return m_s;
  }
  return nullptr;
}

char* ON_String::Array()
{
  CopyArray();
  return ( Header()->string_capacity > 0 ) ? m_s : 0;
}

const char* ON_String::Array() const
{
  return ( Header()->string_capacity > 0 ) ? m_s : 0;
}

/*
Returns:
  Total number of bytes of memory used by this class.
  (For use in ON_Object::SizeOf() overrides.
*/
unsigned int ON_String::SizeOf() const
{
  size_t sz = sizeof(*this);
  if ( ((const void*)m_s) != ((const void*)pEmptyaString) )
    sz += (sizeof(ON_aStringHeader) + (Header()->string_capacity+1));
  return (unsigned int)sz;
}

ON__UINT32 ON_String::DataCRC(ON__UINT32 current_remainder) const
{
  int string_length = Header()->string_length;
  if ( string_length > 0 )
  {
    current_remainder = ON_CRC32(current_remainder, string_length*sizeof(*m_s), m_s);
  }
  return current_remainder;
}

int ON_StringLengthUTF8(
  const char* string
  )
{
  const char* string1 = string;
  if (nullptr != string1)
  {
    while (0 != *string1)
      string1++;
  }
  return (int)(string1 - string);
}

int ON_StringLengthUTF16(
  const ON__UINT16* string
  )
{
  const ON__UINT16* string1 = string;
  if (nullptr != string1)
  {
    while (0 != *string1)
      string1++;
  }
  return (int)(string1 - string);
}

int ON_StringLengthUTF32(
  const ON__UINT32* string
  )
{
  const ON__UINT32* string1 = string;
  if (nullptr != string1)
  {
    while (0 != *string1)
      string1++;
  }
  return (int)(string1 - string);
}

int ON_StringLengthWideChar(
  const wchar_t* string
  )
{
#if (1 == ON_SIZEOF_WCHAR_T)
  return ON_StringLengthUTF8((const char*)string);
#elif (2 == ON_SIZEOF_WCHAR_T)
  return ON_StringLengthUTF16((const ON__UINT16*)string);
#elif (4 == ON_SIZEOF_WCHAR_T)
  return ON_StringLengthUTF32((const ON__UINT32*)string);
#else
#error ON_SIZEOF_WCHAR_T is not defined or has an unexpected value
#endif
}

int ON_StringLengthUTF8(
  const char* string,
  size_t string_capacity
  )
{
  const char* string1 = string;
  if (nullptr != string1 && string_capacity > 0 )
  {
    for (const char* end = string1 + string_capacity; string1 < end; string1++)
    {
      if ( 0 == *string1)
        break;
    }
  }
  return (int)(string1 - string);
}

int ON_StringLengthUTF16(
  const ON__UINT16* string,
  size_t string_capacity
  )
{
  const ON__UINT16* string1 = string;
  if (nullptr != string1 && string_capacity > 0 )
  {
    for (const ON__UINT16* end = string1 + string_capacity; string1 < end; string1++)
    {
      if ( 0 == *string1)
        break;
    }
  }
  return (int)(string1 - string);
}

int ON_StringLengthUTF32(
  const ON__UINT32* string,
  size_t string_capacity
  )
{
  const ON__UINT32* string1 = string;
  if (nullptr != string1 && string_capacity > 0 )
  {
    for (const ON__UINT32* end = string1 + string_capacity; string1 < end; string1++)
    {
      if ( 0 == *string1)
        break;
    }
  }
  return (int)(string1 - string);
}

int ON_StringLengthWideChar(
  const wchar_t* string,
  size_t string_capacity
  )
{
#if (1 == ON_SIZEOF_WCHAR_T)
  return ON_StringLengthUTF8((const char*)string,string_capacity);
#elif (2 == ON_SIZEOF_WCHAR_T)
  return ON_StringLengthUTF16((const ON__UINT16*)string,string_capacity);
#elif (4 == ON_SIZEOF_WCHAR_T)
  return ON_StringLengthUTF32((const ON__UINT32*)string,string_capacity);
#else
#error ON_SIZEOF_WCHAR_T is not defined or has an unexpected value
#endif
}


bool ON_WildCardMatch(const char* s, const char* pattern)
{
  if ( !pattern || !pattern[0] ) {
    return ( !s || !s[0] ) ? true : false;
  }

  if ( *pattern == '*' ) {
    pattern++;
    while ( *pattern == '*' )
      pattern++;
    
    if ( !pattern[0] )
      return true;

    while (*s) {
      if ( ON_WildCardMatch(s,pattern) )
        return true;
      s++;
    }

    return false;
  }

  while ( *pattern != '*' )
  {
    if ( *pattern == '?' ) {
      if ( *s) {
        pattern++;
        s++;
        continue;
      }
      return false;
    }
    
    if ( *pattern == '\\' ) {
      switch( pattern[1] )
      {
      case '*':
      case '?':
        pattern++;
        break;
      }
    }
    if ( *pattern != *s ) {
      return false;
    }

    if ( *s == 0 )
      return true;

    pattern++;
    s++;
  }
  
  return ON_WildCardMatch(s,pattern);
}


bool ON_WildCardMatchNoCase(const char* s, const char* pattern)
{
  if ( !pattern || !pattern[0] ) {
    return ( !s || !s[0] ) ? true : false;
  }

  if ( *pattern == '*' ) 
  {
    pattern++;
    while ( *pattern == '*' )
      pattern++;
    
    if ( !pattern[0] )
      return true;

    while (*s) {
      if ( ON_WildCardMatchNoCase(s,pattern) )
        return true;
      s++;
    }

    return false;
  }

  while ( *pattern != '*' )
  {
    if ( *pattern == '?' ) {
      if ( *s) {
        pattern++;
        s++;
        continue;
      }
      return false;
    }
    
    if ( *pattern == '\\' ) {
      switch( pattern[1] )
      {
      case '*':
      case '?':
        pattern++;
        break;
      }
    }
    if ( toupper(*pattern) != toupper(*s) ) {
      return false;
    }

    if ( *s == 0 )
      return true;

    pattern++;
    s++;
  }
  
  return ON_WildCardMatchNoCase(s,pattern);
}

bool ON_String::WildCardMatch( const char* pattern) const
{
  return ON_WildCardMatch(m_s,pattern);
}

bool ON_String::WildCardMatch( const unsigned char* pattern ) const
{
  return ON_WildCardMatch(m_s,(const char*)pattern);
}

bool ON_String::WildCardMatchNoCase( const char* pattern) const
{
  return ON_WildCardMatchNoCase(m_s,pattern);
}

bool ON_String::WildCardMatchNoCase( const unsigned char* pattern ) const
{
  return ON_WildCardMatchNoCase(m_s,(const char*)pattern);
}

int ON_String::Replace( const char* token1, const char* token2 )
{
  int count = 0;

  if ( 0 != token1 && 0 != token1[0] )
  {
    if ( 0 == token2 )
      token2 = "";
    const int len1 = (int)strlen(token1);
    if ( len1 > 0 )
    {
      const int len2 = (int)strlen(token2);
      int len = Length();
      if ( len >= len1 )
      {
        // in-place
        ON_SimpleArray<int> n(32);
        const char* s = m_s;
        int i;
        for ( i = 0; i <= len-len1; /*empty*/ )
        {
          if ( strncmp(s,token1,len1) )
          {
            s++;
            i++;
          }
          else
          {
            n.Append(i);
            i += len1;
            s += len1;
          }
        }

        count = n.Count();

        // reserve array space - must be done even when len2 <= len1
        // so that shared arrays are not corrupted.
        const int newlen = len + (count*(len2-len1));
        if ( 0 == newlen )
        {
          Destroy();
          return count;
        }

        CopyArray();

        // 24 August 2006 Dale Lear
        //    This used to say
        //       ReserveArray(newlen);
        //    but when newlen < len and the string had multiple
        //    references, the ReserveArray(newlen) call truncated
        //    the input array.  
        ReserveArray( ((newlen<len) ? len : newlen) );

        int i0, i1, ni, j;

        if ( len2 > len1 )
        {
          // copy from back to front
          i1 = newlen;
          i0 = len;
          for ( ni =0; ni < count; ni++ )
            n[ni] = n[ni] + len1;
          for ( ni = count-1; ni >= 0; ni-- )
          {
            j = n[ni];
            while ( i0 > j )
            {
              i0--;
              i1--;
              m_s[i1] = m_s[i0];
            }
            i1 -= len2;
            i0 -= len1;
            memcpy(&m_s[i1],token2,len2*sizeof(m_s[0]));
          }
        }
        else
        {
          // copy from front to back
          i0 = i1 = n[0];
          n.Append(len);
          for ( ni = 0; ni < count; ni++ )
          {
            if ( len2 > 0 )
            {
              memcpy(&m_s[i1],token2,len2*sizeof(m_s[0]));
              i1 += len2;
            }
            i0 += len1;
            j = n[ni+1];
            while ( i0 < j )
            {
              m_s[i1++] = m_s[i0++];
            }
          }
        }
        Header()->string_length = newlen;
        m_s[newlen] = 0;
      }
    }
  }

  return count;
}

int ON_String::Replace( const unsigned char* token1, const unsigned char* token2 )
{
  return Replace((const char*)token1, (const char*)token2);
}

int ON_String::Replace(char utf8_single_byte_c1, char utf8_single_byte_c2)
{
  int count = 0;
  if (ON_IsValidSingleByteUTF8CharValue(utf8_single_byte_c1) && ON_IsValidSingleByteUTF8CharValue(utf8_single_byte_c2))
  {
    int i = Length();
    while (i--)
    {
      if (utf8_single_byte_c1 == m_s[i])
      {
        if (0 == count)
          CopyArray();
        m_s[i] = utf8_single_byte_c2;
        count++;
      }
    }
  }
  return count;
}

int ON_String::Replace( unsigned char token1, unsigned char token2 )
{
  return Replace((const char)token1, (const char)token2);
}


///////////////////////////////////////////////////////////////////////////////

int ON_String::Find(char utf8_single_byte_c) const
{
	// find first single character
  if (ON_IsValidSingleByteUTF8CharValue(utf8_single_byte_c))
  {
    char s[2];
    s[0] = utf8_single_byte_c;
    s[1] = 0;
    return Find(s);
  }
  return -1;
}

int ON_String::Find(unsigned char utf8_single_byte_c) const
{
  return Find((char)utf8_single_byte_c);
}

int ON_String::ReverseFind(char utf8_single_byte_c) const
{
	// find first single character
  if (IsNotEmpty() && ON_IsValidSingleByteUTF8CharValue(utf8_single_byte_c))
  {
    const char* p0 = m_s;
    const char* p = p0 + Length();
    while ( p > p0)
    {
      p--;
      if (utf8_single_byte_c == *p)
        return ((int)(p - p0));
    }
  }
  return -1;
}

int ON_String::ReverseFind(unsigned char utf8_single_byte_c) const
{
  return ReverseFind((char)utf8_single_byte_c);
}

int ON_String::Find( const char* s ) const
{
  return Find(s, 0);
}

int ON_String::Find( const unsigned char* s ) const
{
  return Find((const char*)s, 0);
}

int ON_String::Find(const char* s, int start_index) const
{
  int rc = -1;
  const int length = Length();
  if (s && s[0] && length>0 && start_index>=0 && start_index<length) {
    const char* source = m_s + start_index;
    const char* p = strstr(source, s);
    if (p)
    {
      rc = ((int)(p - m_s)); // the (int) is for 64 bit size_t conversion
    }
  }
  return rc;
}

int ON_String::Find(const unsigned char* s, int start_index) const
{
  return Find((const char*)s, start_index);
}

int ON_String::ReverseFind(const char* s) const
{
  int rc = -1;
  if (s && s[0] && !IsEmpty()) 
  {
    int s_len = 0;
    while (0 != s[s_len])
      s_len++;
    if (Length() >= s_len)
    {
      const char* p0 = m_s;
      const char* p = p0 + (Length()-s_len);
      while (p >= p0)
      {
        if (0 == strncmp(p, s, s_len))
          return ((int)(p - p0));
        p--;
      }
    }
  }
  return rc;
}

int ON_String::ReverseFind(const unsigned char* s) const
{
  return ReverseFind((const char*)s);
}


void ON_String::MakeReverse()
{
  if ( IsNotEmpty() ) 
  {
  	CopyArray();
    ON_String::Reverse(m_s,Length());
  }
}

ON_String ON_String::Reverse() const
{
  ON_String reverse_string(*this);
  reverse_string.MakeReverse();
  return reverse_string;
}

static void ON_String_ReverseUTF8(
  char* string,
  int element_count
  )
{
  if ( element_count < 2 || nullptr == string )
    return;
  ON_String buffer(string,element_count);
  const char* b0 = static_cast<const char*>(buffer);
  const char* b1 = b0+element_count;
  char* s1 = string + (element_count-1);

  ON_UnicodeErrorParameters e;
  e.m_error_mask = 8; // mask overlong UTF-8 encoding errors.
  ON__UINT32 unicode_code_point;
  int count;
  memset(&e, 0, sizeof(e));
  while (b0 < b1)
  {
    const char* c0 = b0++;

    if (0xC0 == (*c0 & 0xC0))
    {
      // *c0 should be the first element in a UTF-8 multi-char encoding.

      while (b0 < b1 && 0x80 == (*b0 & 0xC0))
        b0++;

      unicode_code_point = 0;
      e.m_error_status = 0;
      count = (int)(b0 - c0);
      if (count != ON_DecodeUTF8(c0, count, &e, &unicode_code_point) && 0 != e.m_error_status)
      {
        // not a valid UTF-8 string
        b0 = c0+1;
      }
    }
    
    const char* c = b0;
    while (c > c0)
    {
      c--;
      *s1-- = *c;
    }
  }
}

char* ON_String::Reverse(
  char* string,
  int element_count
  )
{
  if (element_count < 0)
  {
    element_count = ON_String::Length(string);
    if (element_count < 0)
      return nullptr;
  }
  if ( 0 == element_count )
    return string;

  if (nullptr == string)
  {
    ON_ERROR("string is nullptr.");
    return nullptr;
  }

  int i, j;
  char a, b;

  for ( i = 0, j = element_count-1; i < j; i++, j-- ) 
  {
    a = string[i];
    b = string[j];
    if (0 == (0x80 & a) && 0 == (0x80 & b))
    {
      string[i] = b;
      string[j] = a;
      continue;
    }

    // code points with multi char element encodeings need to be handled
    ON_String_ReverseUTF8(string+i,j-i+1);
    break;
  }

  return string;
}


void ON_String::TrimLeft(const char* s)
{
  char c;
  const char* sc;
  char* dc;
  int i;
  if ( !IsEmpty() ) {
    if (nullptr == s)
    {
      for (i = 0; 0 != (c = m_s[i]); i++)
      {
        if ( c < 0 || c > ON_String::Space )
          break;
      }
    }
    else
    {
      for (i = 0; 0 != (c = m_s[i]); i++)
      {
        for (sc = s; *sc; sc++) {
          if (*sc == c)
            break;
        }
        if (!(*sc))
          break;
      }
    }
    if ( i > 0 ) {
      if ( m_s[i] ) {
        CopyArray();
        dc = m_s;
        sc = m_s+i;
        while( 0 != (*dc++ = *sc++) );
        Header()->string_length -= i;
      }
      else
        Destroy();
    }
  }
}

void ON_String::TrimRight(const char* s)
{
  char c;
  const char* sc;
  int i = Header()->string_length;
  if ( i > 0 ) {
    if (nullptr == s)
    {
      for (i--; i >= 0 && 0 != (c = m_s[i]); i--)
      {
        if ( c < 0 || c > ON_String::Space )
          break;
      }
    }
    else
    {
      for (i--; i >= 0 && 0 != (c = m_s[i]); i--)
      {
        for (sc = s; *sc; sc++) {
          if (*sc == c)
            break;
        }
        if (!(*sc))
          break;
      }
    }
    if ( i < 0 )
      Destroy();
    else if ( m_s[i+1] ) {
      CopyArray();
      m_s[i+1] = 0;
      Header()->string_length = i+1;
    }
  }
}

void ON_String::TrimLeftAndRight(const char* s)
{
  TrimRight(s);
  TrimLeft(s);
}

int ON_String::Remove(const char utf8_single_byte_c)
{
  if (false == ON_IsValidSingleByteUTF8CharValue(utf8_single_byte_c))
    return 0;

  CopyArray();

  char* pstrSource = m_s;
  char* pstrDest = m_s;
  char* pstrEnd = m_s + Length();

  while( pstrSource && pstrSource  < pstrEnd)
  {
    if (*pstrSource != utf8_single_byte_c)
    {
      *pstrDest = *pstrSource;
      pstrDest++;
    }
    pstrSource++;
  }

  *pstrDest = 0;
  int nCount = (int)(pstrSource - pstrDest); // the (int) is for 64 bit size_t conversion

  Header()->string_length -= nCount;

  return nCount;
}

char ON_String::GetAt( int i ) const
{
  // no error checking
  return m_s[i];
}

void ON_String::SetAt( int i, char c )
{
  if ( i >= 0 && i < Header()->string_length ) {
	  CopyArray();
	  m_s[i] = c;
  }
}

void ON_String::SetAt( int i, unsigned char c )
{
  SetAt( i, (char)c );
}

ON_String ON_String::Mid(int i, int count) const
{
  ON_String(s);
  if ( i >= 0 && i < Length() && count > 0 ) {
    if ( count > Length() - i )
      count = Length() - i;
    s.CopyToArray( count, &m_s[i] );
  }
  return s;
}

ON_String ON_String::Mid(int i) const
{
  return Mid( i, Length() - i );
}

ON_String ON_String::Left(int count) const
{
  ON_String s;
  if ( count > Length() )
    count = Length();
  if ( count > 0 ) {
    s.CopyToArray( count, m_s );
  }
  return s;
}

ON_String ON_String::Right(int count) const
{
  ON_String s;
  if ( count > Length() )
    count = Length();
  if ( count > 0 ) {
    s.CopyToArray( count, &m_s[Length()-count] );
  }
  return s;
}


ON_CheckSum::ON_CheckSum()
{
  Zero();
}

ON_CheckSum::~ON_CheckSum()
{
  Zero();
}

void ON_CheckSum::Zero()
{
  m_size = 0;
  m_time = 0;
  for ( int i = 0; i < 8; i++ ) 
    m_crc[i] = 0;
}

bool ON_CheckSum::IsSet() const
{
  return ( 0 != m_size 
           || 0 != m_time 
           || 0 != m_crc[0]
           || 0 != m_crc[1]
           || 0 != m_crc[2]
           || 0 != m_crc[3]
           || 0 != m_crc[4]
           || 0 != m_crc[5]
           || 0 != m_crc[6]
           || 0 != m_crc[7]
           );           
}

bool ON_CheckSum::SetBufferCheckSum( 
                size_t size, 
                const void* buffer,
                time_t time
               )
{
  bool rc = false;
  Zero();
  if ( size != 0 && buffer != 0 )
  {
    m_size = (unsigned int)size;

    ON__INT32 crc = 0;
    size_t sz, maxsize = 0x40000;
    const unsigned char* p = (const unsigned char*)buffer;
    for ( int i = 0; i < 7; i++ )
    {
      if ( size > 0 )
      {
        sz = (size > maxsize) ? maxsize : size;
        crc = ON_CRC32(crc,sz,p);
        p += sz;
        size -= sz;
        maxsize *= 2;
      }
      m_crc[i] = crc;
    }
    if ( size > 0 )
    {
      crc = ON_CRC32(crc,size,p);
    }
    m_crc[7] = crc;
    rc = true;
  }
  else if ( 0 == size )
  {
    rc = true;
  }
  m_time = time;
  return rc;
}

bool ON::GetFileStats( const wchar_t* filename,
                       size_t* filesize,
                       time_t* create_time,
                       time_t* lastmodify_time
                      )
{
  bool rc = false;

  if (filesize)
    *filesize = 0;
  if (create_time)
    *create_time = 0;
  if (lastmodify_time)
    *lastmodify_time = 0;

  if ( filename && filename[0] )
  {
    FILE* fp = ON::OpenFile(filename,L"rb");
    if ( fp )
    {
      rc = ON::GetFileStats(fp,filesize,create_time,lastmodify_time);
      ON::CloseFile(fp);
    }
  }

  return rc;
}

bool ON::GetFileStats( FILE* fp,
                       size_t* filesize,
                       time_t* create_time,
                       time_t* lastmodify_time
                      )
{
  bool rc = false;

  if (filesize)
    *filesize = 0;
  if (create_time)
    *create_time = 0;
  if (lastmodify_time)
    *lastmodify_time = 0;

  if ( fp )
  {

#if defined(ON_COMPILER_MSC)

    // Microsoft compilers
    int fd = _fileno(fp);    
#if (_MSC_VER >= 1400)
    // VC 8 (2005) 
    // works for file sizes > 4GB 
    // when size_t is a 64 bit integer
    struct _stat64 sb;
    memset(&sb,0,sizeof(sb));
    int fstat_rc = _fstat64(fd, &sb);
#else
    // VC6 compiler
    // works on most compilers
    struct _stat sb;
    memset(&sb,0,sizeof(sb));
    int fstat_rc = _fstat(fd, &sb);
#endif

#else
    // works on most compilers
    int fd = fileno(fp);
    struct stat sb;
    memset(&sb,0,sizeof(sb));
    int fstat_rc = fstat(fd, &sb);
#endif


    if (0 == fstat_rc)
    {
      if (filesize)
        *filesize = (size_t)sb.st_size;
      if (create_time)
        *create_time = (time_t)sb.st_ctime;
      if (lastmodify_time)
        *lastmodify_time = (time_t)sb.st_mtime;
      rc = true;
    }
  }

  return rc;
}

bool ON::IsDirectory( const wchar_t* pathname )
{
  bool rc = false;

  if ( 0 != pathname && 0 != pathname[0] )
  {
    ON_wString buffer;
    const wchar_t* stail = pathname;
    while ( 0 != *stail )
      stail++;
    stail--;
    if ( '\\' == *stail || '/' == *stail ) 
    {
      const wchar_t trim[2] = {*stail,0};
      buffer = pathname;
      buffer.TrimRight(trim);
      if ( buffer.Length() > 0 )
        pathname = static_cast< const wchar_t* >(buffer);
    }
#if defined(ON_COMPILER_MSC)
    // this works on Windows
    struct _stat64 buf;
    memset(&buf,0,sizeof(buf));
    int stat_errno = _wstat64( pathname, &buf );
    if ( 0 == stat_errno && 0 != (_S_IFDIR & buf.st_mode) )
    {
      rc = true;
    }
#else
    ON_String s = pathname;
    const char* utf8pathname = s;
    rc = ON::IsDirectory(utf8pathname);
#endif
  }

  return rc;
}

bool ON::IsDirectory( const char* utf8pathname )
{
  bool rc = false;

  if ( 0 != utf8pathname && 0 != utf8pathname[0] )
  {
    ON_String buffer;
    const char* stail = utf8pathname;
    while ( 0 != *stail )
      stail++;
    stail--;
    if ( '\\' == *stail || '/' == *stail ) 
    {
      const char trim[2] = {*stail,0};
      buffer = utf8pathname;
      buffer.TrimRight(trim);
      if ( buffer.Length() > 0 )
        utf8pathname = static_cast< const char* >(buffer);
    }
#if defined(ON_COMPILER_MSC)
    // this works on Windows
    struct _stat64 buf;
    memset(&buf,0,sizeof(buf));
    int stat_errno = _stat64( utf8pathname, &buf );
    if ( 0 == stat_errno && 0 != (_S_IFDIR & buf.st_mode) )
    {
      rc = true;
    }
#else
    // this works on Apple and gcc implentations.
    struct stat buf;
    memset(&buf,0,sizeof(buf));
    int stat_errno = stat( utf8pathname, &buf );
    if ( 0 == stat_errno && S_ISDIR(buf.st_mode) )
    {
      rc = true;
    }
#endif
  }

  return rc;
}


int ON::IsOpenNURBSFile( FILE* fp )
{
  ON_String sStartSectionComment;
  int version = 0;
  if ( 0 != fp )
  {
    ON_BinaryFile archive(ON::archive_mode::read3dm,fp);
    if ( !archive.Read3dmStartSection(&version,sStartSectionComment) )
      version = 0;
  }
  return version;
}

int ON::IsOpenNURBSFile( const wchar_t* pathname )
{
  int version = 0;
  if ( 0 != pathname && 0 != pathname[0] )
  {
    FILE* fp = ON::OpenFile(pathname,L"rb");
    if ( 0 != fp )
    {
      version = ON::IsOpenNURBSFile(fp);
      ON::CloseFile(fp);
    }
  }
  return version;
}

int ON::IsOpenNURBSFile( const char* utf8pathname )
{
  int version = 0;
  if ( 0 != utf8pathname && 0 != utf8pathname[0] )
  {
    FILE* fp = ON::OpenFile(utf8pathname,"rb");
    if ( 0 != fp )
    {
      version = ON::IsOpenNURBSFile(fp);
      ON::CloseFile(fp);
    }
  }
  return version;
}

bool ON_CheckSum::SetFileCheckSum( FILE* fp )
{
  bool rc = false;
  Zero();
  if ( fp )
  {
    size_t filesize = 0;
    time_t filetime = 0;
    if ( ON::GetFileStats(fp,&filesize,nullptr,&filetime) )
    {
      m_time = filetime;
    }

    unsigned char buffer[1024];
    int count=1024;
    ON__INT32 crc = 0;
    size_t sz0 = 0, maxsize = 0x40000;

    for( int i = 0; i < 7; i++ )
    {
      sz0 += maxsize;
      while(1024 == count && m_size < sz0)
      {
        count = (int)fread( buffer, 1, 1024, fp ); // the (int) is for 64 bit size_t conversion
        if ( count > 0 )
        {
          m_size += count;
          crc = ON_CRC32( crc, count, buffer );
        }
      }
      maxsize *= 2;
      m_crc[i] = crc;
    }

    while(1024 == count)
    {
      count = (int)fread( buffer, 1, 1024, fp ); // the (int) is for 64 bit size_t conversion
      if ( count > 0 )
      {
        m_size += count;
        crc = ON_CRC32( crc, count, buffer );
      }
    }
    m_crc[7] = crc;

    rc = (filesize == m_size);
  }
  return rc;
}

bool ON_CheckSum::Write(ON_BinaryArchive& archive) const
{
  bool rc = false;
  if ( archive.Archive3dmVersion() < 4 )
  {
    // V3 files had other information
    // 48 bytes of zeros will work ok
    unsigned char b[48];
    memset(b,0,sizeof(b));
    rc = archive.WriteByte(48,b);
  }
  else
  {
    rc = archive.WriteBigSize(m_size);
    if (rc)
      rc = archive.WriteBigTime(m_time);
    if (rc)
      rc = archive.WriteInt(8,&m_crc[0]);
  }
  return rc;
}

bool ON_CheckSum::Read(ON_BinaryArchive& archive)
{
  bool rc;

  Zero();

  rc  = archive.ReadBigSize(&m_size);
  if (rc)
    rc = archive.ReadBigTime(&m_time);
  if (rc)
    rc = archive.ReadInt(8,&m_crc[0]);

  if (    archive.ArchiveOpenNURBSVersion() < 200603100 
       || archive.Archive3dmVersion() < 4 
       )
  {
    // ON_CheckSums in V3 archives and V4 archives with
    // version < 200603100 have the same size but an 
    // incompatible format.  These were not used.
    Zero();
  }

  return rc;
}


bool ON_CheckSum::SetFileCheckSum( const wchar_t* filename )
{
  bool rc = false;
  Zero();
  if ( 0 == filename || 0 == filename[0] )
  {
    rc = true;
  }
  else
  {
    FILE* fp = ON::OpenFile(filename,L"rb");
    if ( fp )
    {
      rc = SetFileCheckSum(fp);
      ON::CloseFile(fp);
    }
  }
  return rc;
}

bool ON_CheckSum::CheckBuffer( 
  size_t size, 
  const void* buffer
  ) const
{
  if ( m_size != size )
    return false;
  if ( 0 == size )
    return true;
  if ( 0 == buffer )
    return false;

  ON__UINT32 crc = 0;
  size_t sz, maxsize = 0x40000;
  const unsigned char* p = (const unsigned char*)buffer;
  for ( int i = 0; i < 7; i++ )
  {
    if ( size > 0 )
    {
      sz = (size > maxsize) ? maxsize : size;
      crc = ON_CRC32(crc,sz,p);
      p += sz;
      size -= sz;
      maxsize *= 2;
    }
    if ( m_crc[i] != crc )
      return false;
  }
  if ( size > 0 )
  {
    crc = ON_CRC32(crc,size,p);
  }
  if ( m_crc[7] != crc )
    return false;

  return true;
}

bool ON_CheckSum::CheckFile( 
  FILE* fp,
  bool bSkipTimeCheck
  ) const
{
  if ( !fp )
    return false;

  size_t filesize=0;
  time_t filetime=0;
  if ( ON::GetFileStats( fp, &filesize, nullptr, &filetime ) )
  {
    if ( m_size != filesize )
    {
      return false;
    }

    if ( !bSkipTimeCheck && m_time != filetime)
    {
      return false;
    }
  }

  unsigned char buffer[1024];
  int count=1024;
  ON__UINT32 crc = 0;
  size_t sz0 = 0, maxsize = 0x40000;
  size_t sz = 0;

  for( int i = 0; i < 7; i++ )
  {
    sz0 += maxsize;
    while(1024 == count && sz < sz0)
    {
      count = (int)fread( buffer, 1, 1024, fp ); // the (int) is for 64 bit size_t conversion
      if ( count > 0 )
      {
        sz += count;
        crc = ON_CRC32( crc, count, buffer );
      }
    }
    maxsize *= 2;
    if ( m_crc[i] != crc )
      return false;
  }

  while(1024 == count)
  {
    count = (int)fread( buffer, 1, 1024, fp ); // the (int) is for 64 bit size_t conversion
    if ( count > 0 )
    {
      sz += count;
      crc = ON_CRC32( crc, count, buffer );
    }
  }
  if (m_crc[7] != crc)
    return false;

  if ( sz != m_size )
    return false;

  return true;
}

bool ON_CheckSum::CheckFile( 
  const wchar_t* filename,
  bool bSkipTimeCheck
  ) const
{
  bool rc = false;
  if ( filename && filename[0] )
  {
    FILE* fp = ON::OpenFile(filename,L"rb");
    if ( fp )
    {
      rc = CheckFile(fp,bSkipTimeCheck);
      ON::CloseFile(fp);
    }
  }
  return rc;
}

void ON_CheckSum::Dump(ON_TextLog& text_log) const
{
  // Using %llu so this code is portable for both 32 and 64 bit
  // builds on a wide range of compilers.

  unsigned long long u; // 8 bytes in windows and gcc - should be at least as big
                        // as a size_t or time_t.

  text_log.Print("Checksum:");
  if ( !IsSet() )
    text_log.Print("zero (not set)\n");
  else
  {
    text_log.PushIndent();
    text_log.Print("\n");
    u = (unsigned long long)m_size;
    text_log.Print("Size: %llu bytes\n",u);
    u = (unsigned long long)m_time;
    text_log.Print("Last Modified Time: %u (seconds since January 1, 1970, UCT)\n",u);
    text_log.Print("CRC List: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n",
                   m_crc[0],m_crc[1],m_crc[2],m_crc[3],m_crc[4],m_crc[5],m_crc[6],m_crc[7]
                   );
    text_log.PopIndent();
  }
}

/*
TODO for apple support
https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/wcscmp.3.html

 wcscasecmp_l(const wchar_t *s1, const wchar_t *s2, locale_t loc);
 wcsncasecmp_l(const wchar_t *s1, const wchar_t *s2, size_t n, locale_t loc);

 look in SetPOSIXLocale() to set up calls to opennurbs locale setter.

 ON_Locale will need to have posix locale_t available for apple functions.

*/