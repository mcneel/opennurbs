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


// openNURBS Geometry Library Errors and Warnings
//
//   If an error condition occurs during a openNURBS Geometry Library
//   computation, the ON_Error() function is called, the computation is
//   stopped, and an error code (negative integer ) is returned.  If a
//   warning condition occurs during a Trout Lake Geometry Library 
//   computation, the ON_Warning() function is called and the computation
//   continues.
//
//   ON_GetErrorCount()
//   ON_GetWarningCount()
//   ON_Error()
//   ON_Warning()
//

#define ON_MAX_ERROR_MESSAGE_COUNT 50

static int ON_ERROR_COUNT = 0;
static int ON_WARNING_COUNT = 0;
static int ON_MATH_ERROR_COUNT = 0;

// 0 = no break
// 1 = break on errors, warnings, and asserts


static int ON_DEBUG_ERROR_MESSAGE_OPTION = 0;


int ON_GetErrorCount(void)
{
  return ON_ERROR_COUNT;	
}	

int ON_GetWarningCount(void)
{
  return ON_WARNING_COUNT;	
}	

int ON_GetMathErrorCount(void)
{
  return ON_MATH_ERROR_COUNT;	
}	


int ON_GetDebugErrorMessage(void)
{
  return ON_DEBUG_ERROR_MESSAGE_OPTION?true:false;
}


void ON_EnableDebugErrorMessage( int bEnableDebugErrorMessage )
{
  ON_DEBUG_ERROR_MESSAGE_OPTION = bEnableDebugErrorMessage ? 1 : 0;
}


void ON_MathError( 
        const char* sModuleName,
        const char* sErrorType,
        const char* sFunctionName
        )
{
  ON_MATH_ERROR_COUNT++; // <- Good location for a debugger breakpoint.

  if ( 0 == sModuleName)
    sModuleName = "";
  if ( 0 == sErrorType )
    sErrorType = "";
  if ( 0 == sFunctionName )
    sFunctionName = "";

  if ( 0 != sModuleName[0] || 0 != sErrorType[0] || 0 != sFunctionName[0] )
  {
    ON_ErrorEx(__FILE__,__LINE__,sFunctionName,
              "Math library or floating point ERROR # %d module=%s type=%s function=%s",
              ON_MATH_ERROR_COUNT, 
              sModuleName, // rhino.exe, opennurbs.dll, etc.
              sErrorType,   
              sFunctionName
              );
  }
  else
  {
    ON_ErrorEx(__FILE__,__LINE__,sFunctionName,
              "Math library or floating point ERROR # %d",
              ON_MATH_ERROR_COUNT
              );
  }
}	

static void ON_IncrementErrorCount()
{
  ON_ERROR_COUNT++;
}

static void ON_IncrementWarningCount()
{
  ON_WARNING_COUNT++;
}

bool ON_IsNotValid()
{
  return false;
}

static void ON_PrintErrorMessage(
    int type, // 0 = warning, 1 = error, 2 = assert
    const char* sFileName, 
    int line_number,
    const char* sFunctionName,
    const char* sFormat,
    va_list args
    )
{
  if ( 0 == ON_DEBUG_ERROR_MESSAGE_OPTION )
    return;

  if ( 0 == type )
  {
    // too many warnings - warning messages are suppressed
    if ( ON_WARNING_COUNT > ON_MAX_ERROR_MESSAGE_COUNT )
      return;
  }
  else if ( 1 == type || 2 == type )
  {
    // too many errors, asserts, etc. - error messages are suppressed
    if ( ON_ERROR_COUNT > ON_MAX_ERROR_MESSAGE_COUNT )
      return;
  }
  else
  {
    return;
  }

  
  char buffer[1024];
  const size_t buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
  buffer[0] = 0;
  buffer[buffer_capacity-1] = 0;

  if ( 0 == type )
  {
    if ( ON_WARNING_COUNT < ON_MAX_ERROR_MESSAGE_COUNT )
    {
      if (0 == sFileName )
        sFileName = "";
      if ( sFunctionName && sFunctionName[0] )
        ON_String::FormatIntoBuffer( buffer, buffer_capacity, "openNURBS WARNING # %d %s.%d %s()",ON_WARNING_COUNT,sFileName,line_number,sFunctionName);
      else
        ON_String::FormatIntoBuffer( buffer, buffer_capacity, "openNURBS WARNING # %d %s.%d",ON_WARNING_COUNT,sFileName,line_number);
    }
    else if ( ON_WARNING_COUNT == ON_MAX_ERROR_MESSAGE_COUNT )
    {
      ON_String::FormatIntoBuffer( buffer, buffer_capacity, "openNURBS WARNING # %d ... Suspending warning messages." ,ON_WARNING_COUNT);
      sFormat = 0;
    }
  }
  else
  {
    if ( ON_ERROR_COUNT < ON_MAX_ERROR_MESSAGE_COUNT )
    {
      if (0 == sFileName )
        sFileName = "";
      if ( sFunctionName && sFunctionName[0] )
        ON_String::FormatIntoBuffer( buffer, buffer_capacity, "openNURBS ERROR # %d %s.%d %s()",ON_ERROR_COUNT,sFileName,line_number,sFunctionName);
      else
        ON_String::FormatIntoBuffer( buffer, buffer_capacity, "openNURBS ERROR # %d %s.%d",ON_ERROR_COUNT,sFileName,line_number);
    }
    else if ( ON_ERROR_COUNT == ON_MAX_ERROR_MESSAGE_COUNT )
    {
      ON_String::FormatIntoBuffer( buffer, buffer_capacity, "openNURBS ERROR # %d ... Suspending error messages.", ON_ERROR_COUNT );
      sFormat = 0;
    }
  }

  if ( (0 != buffer[0] && 0 == buffer[buffer_capacity-1]) )
  {
    if ( 0 != sFormat && 0 != sFormat[0] )
    {
      for ( size_t i = 0; i < buffer_capacity; i++ )
      {
        if ( 0 == buffer[i])
        {
          if ( i + 32 < buffer_capacity )
          {
            buffer[i++] = ':';
            buffer[i++] = 32; // space
            buffer[i] = 0;
            ON_String::FormatVargsIntoBuffer(buffer + i, buffer_capacity-i, sFormat, args );
          }
          break;
        }
      }
    }
    ON_ErrorMessage(type,buffer);
  }
}

#if defined(__ANDROID__)
static void ON_PrintErrorMessage(
                                 int type, // 0 = warning, 1 = error, 2 = assert
                                 const char* sFileName,
                                 int line_number,
                                 const char* sFunctionName,
                                 const char* sFormat,
                                 int empty_args
                                 )
{
  va_list empty_va;
  ON_PrintErrorMessage(type, sFileName, line_number, sFunctionName, sFormat, empty_va);
}
#endif

void ON_VARGS_FUNC_CDECL ON_Error(
  const char* sFileName, 
  int line_number,
  const char* sFormat,
  ...)
{
  ON_IncrementErrorCount();

  if ( 0 != ON_DEBUG_ERROR_MESSAGE_OPTION && ON_ERROR_COUNT <= ON_MAX_ERROR_MESSAGE_COUNT )
  {
    if (sFormat && sFormat[0]) 
    {
      va_list args;
      va_start(args, sFormat);
      ON_PrintErrorMessage(1,sFileName,line_number,0,sFormat,args);
      va_end(args);
    }
    else
    {
      ON_PrintErrorMessage(1,sFileName,line_number,0,0,0);
    }
  }

}

void ON_VARGS_FUNC_CDECL ON_ErrorEx(const char* sFileName, int line_number, const char* sFunctionName,
              const char* sFormat, ...)
{
  ON_IncrementErrorCount();

  if ( 0 != ON_DEBUG_ERROR_MESSAGE_OPTION && ON_ERROR_COUNT <= ON_MAX_ERROR_MESSAGE_COUNT )
  {
    if (sFormat && sFormat[0]) 
    {
      va_list args;
      va_start(args, sFormat);
      ON_PrintErrorMessage(1,sFileName,line_number,sFunctionName,sFormat,args);
      va_end(args);
    }
    else
    {
      ON_PrintErrorMessage(1,sFileName,line_number,sFunctionName,0,0);
    }
  }

}

void ON_VARGS_FUNC_CDECL ON_Warning(const char* sFileName, int line_number,
                const char* sFormat, ...)
{
  ON_IncrementWarningCount();

  if ( 0 != ON_DEBUG_ERROR_MESSAGE_OPTION && ON_WARNING_COUNT <= ON_MAX_ERROR_MESSAGE_COUNT )
  {
    if (sFormat && sFormat[0]) 
    {
      va_list args;
      va_start(args, sFormat);
      ON_PrintErrorMessage(0,sFileName,line_number,0,sFormat,args);
      va_end(args);
    }
    else
    {
      ON_PrintErrorMessage(0,sFileName,line_number,0,0,0);
    }
  }

}


void ON_VARGS_FUNC_CDECL ON_WarningEx(const char* sFileName, int line_number, const char* sFunctionName,
                const char* sFormat, ...)
{
  ON_IncrementWarningCount();

  if ( 0 != ON_DEBUG_ERROR_MESSAGE_OPTION && ON_WARNING_COUNT <= ON_MAX_ERROR_MESSAGE_COUNT )
  {
    if (sFormat && sFormat[0]) 
    {
      va_list args;
      va_start(args, sFormat);
      ON_PrintErrorMessage(0,sFileName,line_number,sFunctionName,sFormat,args);
      va_end(args);
    }
    else
    {
      ON_PrintErrorMessage(0,sFileName,line_number,sFunctionName,0,0);
    }
  }

}

void ON_VARGS_FUNC_CDECL ON_REMOVE_ASAP_AssertEx(int bCondition,
               const char* sFileName, int line_number, const char* sFunctionName,
               const char* sFormat, ...)
{
  if ( false == bCondition)
  {
    if ( 0 != ON_DEBUG_ERROR_MESSAGE_OPTION && ON_ERROR_COUNT <= ON_MAX_ERROR_MESSAGE_COUNT )
    {
      if (sFormat && sFormat[0]) 
      {
        va_list args;
        va_start(args, sFormat);
        ON_PrintErrorMessage(1,sFileName,line_number,sFunctionName,sFormat,args);
        va_end(args);
      }
      else
      {
        ON_PrintErrorMessage(1,sFileName,line_number,sFunctionName,0,0);
      }
    }
  }
}

