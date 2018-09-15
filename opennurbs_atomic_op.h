/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2018 Robert McNeel & Associates. All rights reserved.
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

#if !defined(OPENNURBS_ATOMIC_OP_INC_)
#define OPENNURBS_ATOMIC_OP_INC_


/*
Description:
  Expert user tool to decrement the value of the parameter as an atomic operation.

Parameters:
  ptr_int32 - [in]
    A non-null pointer to a signed 32-bit integer.

Returns:
  Decremented value.

Example:
  int i = 3;
  // j will be 2
  int j = ON_AtomicDecrementInt32(&i);

Remarks:
  Caller is responsible for insuring ptr_int32 is not nullptr. 
*/
// ON_AtomicDecrementInt32(int* ptr_int32)


/*
Description:
  Expert user tool to increment the value of the parameter as an atomic operation.

Parameters:
  ptr_int32 - [in]
    A non-null pointer to a signed 32-bit integer.

Returns:
  Incremented value.

Example:
  int i = 3;
  // j will be 4
  int j = ON_AtomicIncrementInt32(&i);

Remarks:
  Caller is responsible for insuring ptr_int32 points to
  a signed 32-bit integer.
*/
// ON_AtomicIncrementInt32(int* ptr_int32)

#if defined(ON_RUNTIME_WIN)
#define ON_AtomicDecrementInt32(ptr_int32) InterlockedDecrement((long*)(ptr_int32))
#define ON_AtomicIncrementInt32(ptr_int32) InterlockedIncrement((long*)(ptr_int32))
#elif defined(ON_RUNTIME_APPLE_MACOS)
#include <libkern/OSAtomic.h>
#define ON_AtomicDecrementInt32(ptr_int32) OSAtomicDecrement32((int*)(ptr_int32))
#define ON_AtomicIncrementInt32(ptr_int32) OSAtomicIncrement32((int*)(ptr_int32))
#else
// NOT thread safe
#define ON_AtomicDecrementInt32(ptr_int32) (--(*ptr_int32))
#define ON_AtomicIncrementInt32(ptr_int32) (++(*ptr_int32))
#endif

#endif
