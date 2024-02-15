#if 0

#include "opennurbs.h"


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN TestBase64 command
//

class CCommandTestBase64 : public CRhinoTestCommand
{
public:
	CCommandTestBase64() {}
	~CCommandTestBase64() {}
	UUID CommandUUID()
	{
		// {2BE047C-CC30-4178-AFA5-136E79A4335C}
		static const GUID TestBase64Command_UUID =
		{ 0x2BE047C, 0xCC30, 0x4178, { 0xAF, 0xA5, 0x13, 0x6E, 0x79, 0xA4, 0x33, 0x5C } };
		return TestBase64Command_UUID;
	}
	const wchar_t* EnglishCommandName() { return RHINOSTRING_COMMAND_NAME(L"TestBase64"); }
	CRhinoCommand::result RunCommand( const CRhinoCommandContext& );
};

// The one and only CCommandTestBase64 object
static class CCommandTestBase64 theTestBase64Command;

class MyDecode : public ON_DecodeBase64
{
public:
  const unsigned char* m_buf;
  unsigned char* m_buf1;
  int         m_buf_count;
  MyDecode(const unsigned char* buf, unsigned char* buf1);
  void Output();
};

MyDecode::MyDecode(const unsigned char* buf, unsigned char* buf1) : m_buf(buf), m_buf1(buf1), m_buf_count(0)
{
}

void MyDecode::Output()
{
  memcpy(m_buf1+m_buf_count,m_output,m_output_count);
  m_buf_count += m_output_count;

  //for( int i = 0; i < m_output_count; i++ )
  //{
  //  m_buf1[m_buf_count] = m_output[i];
  //  if ( m_buf[m_buf_count++] != m_output[i] )
  //  {
  //    m_buf_count--;
  //    SetError();
  //    break;
  //  }
  //}
}

class MyEncode : public ON_EncodeBase64
{
public:
  char* m_base64;
  int m_base64_count;
  MyEncode(char* p);
  void Output();
};

MyEncode::MyEncode(char* p) : m_base64(p), m_base64_count(0) {}

void MyEncode::Output()
{
  if ( m_base64_count + m_output_count <= 4000 )
  {
    memcpy(m_base64+m_base64_count,m_output,m_output_count);
    m_base64_count += m_output_count;
    m_base64[m_base64_count] = 0;
  }
}

bool Test64(const char* s, const char* e64)
{
	unsigned char buf[3000];
  char base64[4001];
  const int ec = ON_GetErrorCount();

  const int buf_count = (int)strlen(s);
  if ( buf_count > 3000 )
    return false;

  const int base64_count = 4*(buf_count/3) + ((buf_count%3)?4:0);

  memset(buf,0,sizeof(buf));
  memset(base64,0,sizeof(base64));
  MyEncode encode(base64);
  MyDecode decode((unsigned char*)s,buf);

  encode.Begin();
  encode.Encode(s,buf_count);
  encode.End();

  if ( ec != ON_GetErrorCount() )
    return false;

  if ( buf_count != encode.m_encode_count )
    return false;

  if ( base64_count != encode.m_base64_count )
    return false;

  if ( strcmp(base64,e64) )
    return false;

  decode.Begin();
  decode.Decode(e64);
  decode.End();

  if ( ec != ON_GetErrorCount() )
    return false;

  if ( base64_count != decode.m_decode_count )
    return false;

  if ( buf_count != decode.m_buf_count )
    return false;

  if ( strcmp((const char*)buf,s) )
    return false;

  return true;
}

CRhinoCommand::result CCommandTestBase64::RunCommand( const CRhinoCommandContext& context )
{
	unsigned char buf[3000];
	unsigned char buf1[3000];
  char base64[4001];
  int ec = ON_GetErrorCount();
  int jjj;

  memset(buf,0,sizeof(buf));
  memset(base64,0,sizeof(base64));
  MyEncode encode(base64);
  MyDecode decode(buf,buf1);

  {
    bool b;
    b = Test64("This is a test",  "VGhpcyBpcyBhIHRlc3Q=");
    if ( b)
    b = Test64("This is a test1", "VGhpcyBpcyBhIHRlc3Qx");
    if (b)
    b = Test64("This is a test12","VGhpcyBpcyBhIHRlc3QxMg==");
    if ( !b)
    {
      ON_ERROR("tests failed");
    }
  }

  for ( int buf_count = 1; buf_count <= 64; buf_count++) 
  {
    RhinoApp().Print("buf_count %d\n",buf_count);
    const int base64_count = 4*(buf_count/3) + ((buf_count%3)?4:0);

    for ( int buf_index = 0; buf_index < buf_count; buf_index++ )
    for ( int buf_val = 0; buf_val < 256; buf_val++ )
    for ( int val_count = 1; val_count <= 5; val_count++ )
    {
      int ii=-99;
      ec = ON_GetErrorCount();
      for ( int attempt = 0; attempt <= 2; attempt++ )
      {
        if ( attempt )
        {
          int tryagain = 123;
        }

        memset(buf,0,buf_count+6);
        memset(base64,0,base64_count);

        for ( jjj = 0; jjj < val_count; jjj++ )
          buf[buf_index+jjj] = (buf_val+jjj)%256;

        buf1[0]           = buf[0]           ? 0 : 0xFF;
        buf1[buf_count-1] = buf[buf_count-1] ? 0 : 0xFF;
        buf1[buf_count]   = buf[buf_count];

        encode.m_base64_count = 0;
        encode.Begin();
        encode.Encode(buf,buf_count);
        encode.End();


        if ( encode.m_encode_count != buf_count )
        {
          ON_ERROR("encode - encode.m_encode_count != buf_count");
          continue;
        }
        if ( encode.m_base64_count != base64_count )
        {
          ON_ERROR("encode - encode.m_base64_count != base64_count");
          continue;
        }

        for ( ii = 0; ii < base64_count; ii++ )
        {
          const char* ss;
          int sz;
          decode.m_buf_count = 0;
          decode.Begin();
          char c = base64[ii];

          if ( ii > 0 )
          {
            base64[ii] = 0;        
            ss = decode.Decode(base64);
            if ( !ss || decode.Error() )
            {
              ON_ERROR("decode - first block error");
              break;
            }
            sz = (int)(ss-base64);
            if ( sz != ii || decode.Error() )
            {
              ON_ERROR("decode - first block trauma");
              break;
            }
            base64[ii] = c;
          }

          ss = decode.Decode(base64+ii);
          if ( !ss || decode.Error() )
          {
            ON_ERROR("decode - 2nd block error");
            break;
          }
          sz = (int)(ss-base64);
          if ( sz != base64_count || decode.Error() )
          {
            ON_ERROR("decode - 2nd block trauma");
            break;
          }

          if ( !decode.End() )
          {
            ON_ERROR("decode - error");
            break;
          }

          if ( decode.m_decode_count != base64_count )
          {
            ON_ERROR("decode decode.m_decode_count != base64_count");
            break;
          }

          if ( memcmp(buf,buf1,buf_count+1) )
          {
            ON_ERROR("decode failure");
            break;
          }

          break;

          if ( ii > 8 && ii < base64_count-8 )
          {
            if ( ii < base64_count/2 )
              ii = base64_count/2;
            else
              ii = base64_count-8;
          }
        }
        if ( 0 == attempt && ec == ON_GetErrorCount() )
        {
          break; // no errors - no need to repeat
        }
      }
    }
  }
	return CRhinoCommand::success;
}

//
// END TestBase64 command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#endif
