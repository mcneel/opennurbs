/* $Header: /src4/opennurbs/Tests/TestZLib.cpp 3     2/24/06 9:46p Dalelear $ */
/* $NoKeywords: $ */

#include "Tests.h"
#include <sys/types.h>
#include <sys/stat.h>


static 
bool TestZLibCompressionHelper2( 
          const wchar_t* filename, 
          __int64 filesize, 
          ON_TextLog& text_log 
          )
{
  ON_Workspace ws;

  if ( 0 == filename )
    return false;

  if ( filesize > 0xFFFFFFF0 )
  {
    text_log.Print("Huge file - cannot compress this\n");
    return false;
  }

  ////////////////////////////////////////////////////////////////
  //
  // Allocate inbuffer[] and read filename into inbuffer[]
  //
  const size_t insz = (size_t)filesize;
  void* inbuffer = ws.GetMemory( insz );

  if ( 0 == inbuffer )
  {
    text_log.Print("onmalloc( %u ) returned NULL\n",insz);
    return false;
  }

  FILE* fp = _wfopen(filename,L"rb");
  if ( !fp )
  {
    ON_ERROR("Unable to read file\n");
    text_log.Print(L"ERROR: wfopen(%s,L\"rb\") failed\n",filename);
    return false;
  }
  size_t freadsz = fread(inbuffer,1,insz,fp);
  fclose(fp);
  fp = 0;
  if ( freadsz != insz )
  {
    text_log.Print(L"ERROR: fread(inbuffer,1,%u,%s) = %u\n",insz,filename,freadsz);
    return false;
  }

  ////////////////////////////////////////////////////////////////
  //
  // Compress inbuffer[] and save compressed informtion in zlibtest.tmp
  //
  const wchar_t* tempfile = L"C:\\Temp\\zlibtest.tmp";

  fp = ON::OpenFile(tempfile,L"wb");
  if ( !fp )
  {
    ON_ERROR("Unable to write temp file\n");
    text_log.Print(L"ERROR: ON::OpenFile(%s,L\"wb\") failed\n",tempfile);
    return false;
  }

  bool bWriteOk = false;
  {
    ON_BinaryFile file( ON::write, fp );
    bWriteOk = file.WriteCompressedBuffer( insz, inbuffer );
  }
  fclose(fp);
  fp = 0;
  if ( !bWriteOk )
  {
    text_log.Print(L"ERROR: WriteCompressedBuffer(%u,...) failed\n",insz);
    return false;
  }

  ////////////////////////////////////////////////////////////////
  //
  // Read zlibtest.tmp uncompress its contents into outbuffer[]
  //
  size_t outsz    = 0;
  void* outbuffer = 0;

  fp = ON::OpenFile(tempfile,L"rb");
  if ( !fp )
  {
    ON_ERROR("Unable to read temp file\n");
    text_log.Print(L"ERROR: ON::OpenFile(%s,L\"rb\") failed\n",tempfile);
    return false;
  }

  int bFailedCRC = true;
  bool bReadOk = false;
  {
    ON_BinaryFile file( ON::read, fp );
    bReadOk = file.ReadCompressedBufferSize( &outsz );
    if (bReadOk && outsz == insz)
    {
      outbuffer = ws.GetMemory(outsz);
      if ( 0 == outbuffer )
      {
        text_log.Print("onmalloc( %u ) returned NULL\n",outsz);
        fclose(fp);
        return false;
      }
      else
      {
        bReadOk = file.ReadCompressedBuffer( outsz, outbuffer, &bFailedCRC );
      }
    }
  }
  fclose(fp);
  fp = 0;
  if ( !bReadOk )
  {
    text_log.Print(L"ERROR: ReadCompressedBuffer(%u,...) failed\n",outsz);
    return false;
  }

  ////////////////////////////////////////////////////////////////
  //
  // Compare inbuffer[] and outbuffer[]
  //
  if ( insz != outsz || memcmp(inbuffer,outbuffer,insz) || bFailedCRC )
  {
    ON_ERROR("TestZLibCompressionHelper2 compression failure");
    text_log.Print("ERROR: %s compression failure\n");
    return false;
  }

  return true;
}


static 
void TestZLibCompressionHelper1( 
          const wchar_t* filespec, 
          ON_TextLog& text_log, 
          int& file_counter,
          int recursion_counter
          )
{
  const int max_file_count = 0x7FFFFFF0;
  if ( file_counter > max_file_count )
    return;

  if ( recursion_counter > 32 )
  {
    ON_ERROR("TestZLibCompressionHelper1 recursion_counter > 32");
    return;
  }

  if ( 0 == recursion_counter && (0 == filespec || 0 == filespec[0]) )
  {
    filespec = L"C:\\Documents and Settings\\dalelear\\My Documents";
  }

  bool bRecurse = false;
  struct __stat64 fs;

  ON_wString wildcard = filespec;
  ON_wString path;
  if ( 0 == recursion_counter )
  {
    wildcard.TrimRight(L"/\\");
  }
  if ( !wildcard.IsEmpty() )
  {
    memset(&fs,0,sizeof(fs));
    if ( 0 == _wstat64( wildcard, &fs ) )
    {
      if ( 0 != (_S_IFDIR & fs.st_mode) )
      {
        bRecurse = true;
        path = wildcard;
        path += '\\';
        wildcard = path;
        wildcard += '*';
        filespec = wildcard;
      }
    }
  }

  struct _wfinddata64_t fi;
  memset(&fi,0,sizeof(fi));
  INT_PTR h = _wfindfirst64( filespec, &fi );
  if ( -1 == h )
  {
    text_log.Print(L"ERROR: Compression test found nothing in %s\n",filespec);
    return;
  }

  int rc = 0;
  while ( 0 == rc && file_counter < max_file_count )
  {
    if ( 0 != fi.name[0] 
        && (fi.name[0] != '.' || (0 != fi.name[1] && wcscmp(fi.name,L"..")))
       )
    {
      // fi.name is not empty and is not "." or ".."
      ON_wString fullpath = path;
      fullpath += fi.name;
      memset(&fs,0,sizeof(fs));
      if ( 0 == _wstat64( fullpath, &fs ) )
      {
        bool bIsDir  = (0 != (_S_IFDIR & fs.st_mode));
        bool bIsFile = (false == bIsDir && (0 != (_S_IFREG & fs.st_mode)));
        if ( bIsDir && bRecurse )
        {
          // recurse through subdirectory
          TestZLibCompressionHelper1(fullpath,text_log,file_counter,recursion_counter+1);
        }
        else if ( bIsFile && fs.st_size > 0)
        {
          // use file for a compression test
          if ( !TestZLibCompressionHelper2(fullpath,fs.st_size,text_log) )
          {
            text_log.Print(L"ERROR: %s compression test failed\n",fullpath);
          }
          else
          {
            file_counter++;
            if ( 0 == (file_counter % 1000) )
            {
              text_log.Print(L"%6d files tested\n",file_counter);
            }
          }
        }
      }
    }
    rc = _wfindnext64( h, &fi );
  }

  _findclose(h);

  return;
}

void TestZLibCompression( 
          const wchar_t* filespec, 
          ON_TextLog& text_log 
          )
{
  int file_counter = 0;
  TestZLibCompressionHelper1(filespec,text_log,file_counter,0);
}

