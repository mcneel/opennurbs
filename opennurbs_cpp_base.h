/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2015 Robert McNeel & Associates. All rights reserved.
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

#if !defined(OPENNURBS_CPP_BASE_INC_)
#define OPENNURBS_CPP_BASE_INC_

// basic C++ declarations


#if !defined(UUID_DEFINED) && !defined(GUID_DEFINED)
// basic C++ declarations
bool operator==(const struct ON_UUID_struct& a, const struct ON_UUID_struct& b);
bool operator!=(const struct ON_UUID_struct& a, const struct ON_UUID_struct& b);
#endif

#endif
