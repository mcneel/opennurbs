/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2016 Robert McNeel & Associates. All rights reserved.
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

////////////////////////////////////////////////////////////////
//
//   Includes all openNURBS toolkit headers required to use the
//   openNURBS toolkit library.  See readme.txt for details.
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_PUBLIC_INC_)
#define OPENNURBS_PUBLIC_INC_

#define OPENNURBS_PUBLIC_INC_IN_PROGRESS

#if defined(ON_COMPILING_OPENNURBS)
#error Do not include opennurbs_public.h in opennurbs library source code.
#endif

#if defined(OPENNURBS_INC_)
//
// Read the following, think about what you are trying to accomplish,
// and then include exactly one of opennurbs.h or opennurbs_public.h.
//
// If you are building a Rhino plug-in or using the Rhino SDK, 
// then include RhinoSDK.h which will eventually include opennurbs.h.
//
// If you are building your own application and linking with an
// opennurbs_public* library, then include opennurbs_public.h.
// 
#error Include exactly one of opennurbs.h or opennurbs_public.h
#endif


#define OPENNURBS_PUBLIC
#include "opennurbs.h"

#undef OPENNURBS_PUBLIC_INC_IN_PROGRESS

#endif
