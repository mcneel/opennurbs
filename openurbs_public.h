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

#if defined(ON_COMPILING_OPENNURBS)
#error Never include opennurbs_public.h when building opennurbs libraries.
#endif

#if defined(OPENNURBS_INC_)

// If you are building a Rhino plug-in or using opennurbs as 
// part of the Rhino SDK, then include opennurbs.h.
//
// If you are building your own application and linking with one of the
// opennurbs_public libraries for .3dm file IO, then include opennurbs_public.h.
// 
#error Your project should include exactly one of opennurbs_public.h or opennurbs.h. See comment above.
#endif

#if !defined(OPENNURBS_PUBLIC)
#define OPENNURBS_PUBLIC
#endif

#include "opennurbs.h"

#endif
