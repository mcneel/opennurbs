#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

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


bool ON_ComponentAttributes::IsSolid(
  unsigned int aggregate_edge_component_attributes
  )
{
  const unsigned int required_bits
    = ON_ComponentAttributes::EdgeFlags::Oriented
    | ON_ComponentAttributes::EdgeFlags::Damaged
    ;
  if ( required_bits != (required_bits & aggregate_edge_component_attributes) )
    return false;

  const unsigned int forbidden_bits
    = ON_ComponentAttributes::EdgeFlags::Wire
    | ON_ComponentAttributes::EdgeFlags::Boundary
    | ON_ComponentAttributes::EdgeFlags::Nonmanifold
    | ON_ComponentAttributes::EdgeFlags::NotOriented
    | ON_ComponentAttributes::EdgeFlags::Damaged
    ;
  if ( 0 != (forbidden_bits & aggregate_edge_component_attributes) )
    return false;

  return true;
}

bool ON_ComponentAttributes::HasBoundary(
  unsigned int aggregate_edge_component_attributes
  )
{
  const unsigned int required_bits
    = ON_ComponentAttributes::EdgeFlags::Boundary
    ;
  if ( required_bits != (required_bits & aggregate_edge_component_attributes) )
    return false;

  return true;}


bool ON_ComponentAttributes::IsOriented(
  unsigned int aggregate_edge_component_attributes
  )
{
  const unsigned int required_bits
    = ON_ComponentAttributes::EdgeFlags::Oriented
    ;
  if ( required_bits != (required_bits & aggregate_edge_component_attributes) )
    return false;

  const unsigned int forbidden_bits
    = ON_ComponentAttributes::EdgeFlags::NotOriented
    ;
  if ( 0 != (forbidden_bits & aggregate_edge_component_attributes) )
    return false;

  return true;
}

bool ON_ComponentAttributes::IsNotOriented(
  unsigned int aggregate_edge_component_attributes
  )
{
  const unsigned int required_bits
    = ON_ComponentAttributes::EdgeFlags::NotOriented
    ;
  if ( required_bits != (required_bits & aggregate_edge_component_attributes) )
    return false;

  return true;
}


bool ON_ComponentAttributes::IsManifold(
  unsigned int aggregate_edge_component_attributes
  )
{
  const unsigned int require_at_least_one_bit
    = ON_ComponentAttributes::EdgeFlags::Boundary
    | ON_ComponentAttributes::EdgeFlags::Interior
    | ON_ComponentAttributes::EdgeFlags::Seam
    ;
  if ( 0 == (require_at_least_one_bit & aggregate_edge_component_attributes) )
    return false;

  const unsigned int forbidden_bits
    = ON_ComponentAttributes::EdgeFlags::Wire
    | ON_ComponentAttributes::EdgeFlags::Nonmanifold
    ;
  if ( 0 != (forbidden_bits & aggregate_edge_component_attributes) )
    return false;

  return true;
}

bool ON_ComponentAttributes::IsNotManifold(
  unsigned int aggregate_edge_component_attributes
  )
{
  const unsigned int require_at_least_one_bit
    = ON_ComponentAttributes::EdgeFlags::Wire
    | ON_ComponentAttributes::EdgeFlags::Nonmanifold
    ;
  if ( 0 == (require_at_least_one_bit & aggregate_edge_component_attributes) )
    return false;

  return true;
}

