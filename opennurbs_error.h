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

#if !defined(OPENNURBS_ERROR_INC_)
#define OPENNURBS_ERROR_INC_

/*
// Macros used to log errors and warnings.  The ON_Warning() and ON_Error()
// functions are defined in opennurbs_error.cpp.
*/
#define ON_ERROR(msg) ON_ErrorEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__,msg)
#define ON_WARNING(msg) ON_WarningEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__,msg)
#define ON_ASSERT_OR_RETURN(cond,returncode) do{if (!(cond)) {ON_ErrorEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__, #cond " is false");return(returncode);}}while(0)
#define ON_ASSERT_OR_RETURNVOID(cond) do{if (!(cond)) {ON_ErrorEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__, #cond " is false");return;}}while(0)

// Do not use ON_ASSERT. If a condition can be checked by ON_ASSERT, then the
// code must be written detect and respond to that condition. This define will
// be deleted ASAP. It is being used to detect situations where a crash will
// occur and then letting the crash occur. 
#define ON_ASSERT(cond) ON_REMOVE_ASAP_AssertEx(cond,__FILE__,__LINE__,OPENNURBS__FUNCTION__, #cond " is false")


ON_BEGIN_EXTERNC

/*
// All error/warning messages are sent to ON_ErrorMessage().  Replace the
// default handler (defined in opennurbs_error_message.cpp) with something
// that is appropriate for debugging your application.
*/
ON_DECL
void ON_ErrorMessage( 
       int,         /* 0 = warning message, 1 = serious error message, 2 = assert failure */
       const char*  
       ); 

/*
Returns:
  Number of opennurbs errors since program started.
*/
ON_DECL
int ON_GetErrorCount(void);

/*
Returns:
  Number of opennurbs warnings since program started.
*/
ON_DECL
int ON_GetWarningCount(void);

/*
Returns:
  Number of math library or floating point errors that have 
  been handled since program started.
*/
ON_DECL
int ON_GetMathErrorCount(void);


ON_DECL
int ON_GetDebugErrorMessage(void);

ON_DECL
void ON_EnableDebugErrorMessage( int bEnableDebugErrorMessage );

ON_DECL
void ON_VARGS_FUNC_CDECL ON_Error(
  const char* file_name, /* __FILE__ will do fine */
  int line_number,       /* __LINE__ will do fine */
  const char* format,    /* format string */
  ...                    /* format ags */
  );

ON_DECL
void ON_VARGS_FUNC_CDECL ON_ErrorEx(
  const char* file_name,      /* __FILE__ will do fine */
  int line_number,            /* __LINE__ will do fine */
  const char* function_name,  /* OPENNURBS__FUNCTION__ will do fine */
  const char* format,         /* format string */
  ...                         /* format ags */
  );

ON_DECL
void ON_VARGS_FUNC_CDECL ON_Warning(
  const char* file_name, /* __FILE__ will do fine */
  int line_number,       /* __LINE__ will do fine */
  const char* format,    /* format string */
  ...                    /* format ags */
  );

ON_DECL
void ON_VARGS_FUNC_CDECL ON_WarningEx(
  const char* file_name,      /* __FILE__ will do fine */
  int line_number,            /* __LINE__ will do fine */
  const char* function_name,  /*OPENNURBS__FUNCTION__ will do fine */
  const char* format,         /* format string */
  ...                         /* format ags */
  );

// Ideally - these "assert" functions will be deleted when the SDK can be changed.
ON_DECL
void ON_VARGS_FUNC_CDECL ON_REMOVE_ASAP_AssertEx(
  int,        // if false, error is flagged
  const char* file_name,      /* __FILE__ will do fine */
  int line_number,            /* __LINE__ will do fine */
  const char* function_name,  /* OPENNURBS__FUNCTION__ will do fine */
  const char* format,         /* format string */
  ...                         /* format ags */
  );

ON_DECL
void ON_MathError( 
  const char*, /* sModuleName */
  const char*, /* sErrorType */
  const char*  /* sFunctionName */
  );

ON_END_EXTERNC

#endif
