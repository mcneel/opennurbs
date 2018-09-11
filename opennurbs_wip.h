/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2014 Robert McNeel & Associates. All rights reserved.
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

#if !defined OPENNURBS_WIP_INC__
#define OPENNURBS_WIP_INC__

#if !defined(OPENNURBS_SUBD_WIP)
#if defined(ON_COMPILING_OPENNURBS) || defined(TL_INC_) || defined(RHINO_WIP_BUILD) || defined(RHINO_BETA_BUILD) || defined(RHINO_SUBD_WIP)
// SubD classes and .3dm support for reading it.
// SubD is alwasy available internally (ON_COMPILING_OPENNURBS is defined)
// so .3dm files can be read and written by all versions of opennurbs.
#define OPENNURBS_SUBD_WIP

#elif defined(RHINO_CORE_COMPONENT) && 0 != RHINO_CORE_COMPONENT
// SubD is available to core Rhino 6 and core Rhino WIP code
#define OPENNURBS_SUBD_WIP

#elif !defined(RHINO_COMMERCIAL_BUILD)
// SubD is available in the Rhino WIP C++ SDK.
// SubD is not avaialable in the Rhino 6 commercial C++ SDK.
#define OPENNURBS_SUBD_WIP

#endif
#endif


// Annotation table is being prototyped and on hold
// until V6 ships.
//#define OPENNURBS_ANNOTATION_TABLE_WIP

#endif
