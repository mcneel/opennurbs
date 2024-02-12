//
// Copyright (c) 1993-2022 Robert McNeel & Associates. All rights reserved.
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

#if !defined(OPENNURBS_RANDOM_NUMBER_INC_)
#define OPENNURBS_RANDOM_NUMBER_INC_

ON_BEGIN_EXTERNC

struct ON_RANDOM_NUMBER_CONTEXT
{
  ON__UINT32 mti;     /* mti = 0xFFFFFFFF means mt[] is not initialized */
  ON__UINT32 mt[624]; /* the array for the state vector  */
};


/*
Description:
  Seed a context for on_random_number().
Parameters:
  s - [in]
  rand_context - [out] context to seed.

Remarks:
  on_random_number_seed() does not use any static memory.
Example:
          ON_RAND_CONTEXT rand_context;

          ON__UINT seed = 123;
          on_random_number_seed( seed, &rand_context );

          ON__UINT32 r1 = on_random_number( &rand_context );
          ON__UINT32 r2 = on_random_number( &rand_context );
          ON__UINT32 r3 = on_random_number( &rand_context );
*/
void on_random_number_seed(
        ON__UINT32 s,
        struct ON_RANDOM_NUMBER_CONTEXT* rand_context
        );

/*
Description:
  Get a random number.
Parameters:
  rand_context - [in/out]
    random number context.  The first time rand_context is
    used it must be either initialized by calling on_random_number_seed()
    or rand_context->mti must be set to 0xFFFFFFFF.  Otherwise do not 
    modify randcontext between calls to on_random_number.
Returns:
  A random number.
Remarks:
  on_random_number() does not use any static memory.
Example:
          ON_RAND_CONTEXT rand_context;

          ON__UINT seed = 123;
          on_random_number_seed( seed, &rand_context );

          ON__UINT32 r1 = on_random_number( &rand_context );
          ON__UINT32 r2 = on_random_number( &rand_context );
          ON__UINT32 r3 = on_random_number( &rand_context );
*/
ON__UINT32 on_random_number(
        struct ON_RANDOM_NUMBER_CONTEXT* rand_context
        );


/*
Description:
  Seed the random number generator used by on_rand().
Parameters:
  s - [in]
Remarks:
  on_srand() is not thread safe.  It used static global memory
  that is modified by on_srand() and on_rand().
*/
void on_srand(ON__UINT32 s);

/*
Description:
  Get a random number.
Returns:
  A random number.
Remarks:
  on_rand() is not thread safe.  It used static global memory
  that is modified by on_srand() and on_rand().
*/
ON__UINT32 on_rand(void);


ON_END_EXTERNC


#if defined(ON_CPLUSPLUS)

class ON_CLASS ON_RandomNumberGenerator
{
public:
  ON_RandomNumberGenerator();

  /*
  Returns:
    A upredictable seed value for a random number generator.
    This function is much slower than ON_RandomNumberGenerator::RandomNumber().
  */
  static ON__UINT32 RandomSeed();

  /*
  Description:
    Seed the random number generator.
  Parameters:
    s - [in]
  */
  void Seed( ON__UINT32 s );

  /*
  Description:
    Seed the random number generator in a way that cannot be reproduced.
  */
  void Seed();

  /*
  Returns:
    32 bit unsigned random number [0,0xFFFFFFFF] [0,4294967295]
  */
  ON__UINT32 RandomNumber();

  /*
  Returns:
    double in the interval [0.0 and 1.0]
  */
  double RandomDouble();

  /*
  Returns:
    double in the interval [t0,t1]
  */
  double RandomDouble(double t0, double t1);

  double RandomDouble(const class ON_Interval& range);

  /*
  Parameters:
    i0 - [in]
    i1 - [in]
  Returns:
    A signed integer in the interval [min(i0,i1), max(i0,i1)], inclusive
  Example
    RandomSignedInteger(-2,3) will return -2, -1, 0, 1, 2, or 3.
  */
  int RandomSignedInteger(int i0, int i1);

  /*
  Parameters:
    i0 - [in]
    i1 - [in]
  Returns:
    An unsigned integer in the interval [min(i0,i1), max(i0,i1)], inclusive
  Example
    RandomUnsignedInteger(3,7) will return 3, 4, 5, 6, or 7.
  */
  unsigned int RandomUnsignedInteger(unsigned int i0, unsigned int i1);

  /*
  Description:
    Perform a random permutation on an array.
  Parameters:
    base - [in/out]
      Array of element to permute
    nel - [in]
      number of elements in the array.
    sizeof_element
      size of an element in bytes.
  */
  void RandomPermutation(void* base, size_t nel, size_t sizeof_element );

private:
  struct ON_RANDOM_NUMBER_CONTEXT m_rand_context;
};

#endif


#endif
