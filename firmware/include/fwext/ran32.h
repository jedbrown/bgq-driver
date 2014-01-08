/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2007, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#ifndef	_RAN32_H_ // Prevent multiple inclusion
#define	_RAN32_H_



///#include <common/namespace.h>

__BEGIN_DECLS

#include <stdint.h>
#include <hwi/include/common/bgq_bitnumbers.h>


/*
 * RAN32 provides some simple random number routines.
 *
 * These routines use a simple linear congruential random
 * number generator which has period 2^32, but which suffer
 * the usual linear congruential random number generator
 * problems ( low order bits are correlated, etc).
 *
 */



/*
 * Usage:
 *
 *    #include <ran32.h>
 *
 *    // define a variable in which to store a seed
 *
 *    unsigned int ran32_seed;
 *
 *    // set the seed with some value, this can be any 32bit pattern you like
 *    // read it back
 *
 *    unsigned int start_seed;
 *    unsigned int current_seed;
 *
 *    start_seed = 0x13572459;
 *    ran32_set_seed(&ran32_seed, start_seed); 
 *    current_seed = ran32_get_seed(&ran32_seed);
 *
 *    // generate a random 32 bit unsigned int
 *    // each call generates a different value
 *
 *    unsigned int random_uint;
 *    random_uint = ran32_uint(&ran32_seed);
 *
 *    // generate an n bit random pattern, with n < 32
 *    // this random pattern is picked of from the most signficant
 *    // bits of the seed, to help alleviate the low bit correlation
 *    // issues with linear congruential generators.
 *
 *    unsigned int random_nbit_pattern, n;
 *    n = 8;
 *    random_nbit_pattern = ran32_nbits(&ran32_seed, n);
 *
 *    // generate an n bit random pattern, with n < 32 and with a given
 *    // probability that only 1 bit in the pattern is 1.
 *    // The probability of only one '1' bit is given as unsigned int
 *    // with value 0-100.
 *    // If the function decides with that percentage to do either
 *    // 0x01 << ran32_uint(seed) % n or to call ran32_nbitx().
 *
 *    unsigned int random_nbit_pattern, n, prob_1_only;
 *    n = 8;
 *    prob_1_only = 50;
 *    random_nbit_pattern = ran32_nbits1prob(&ran32_seed, n, prob_1_only);
 *
 *    // generate 0 or 1 with a given probability for 1.
 *    // The probability for '1' is given as unsigned int with value 0-100.
 *
 *    unsigned int random, prob_1;
 *    prob_1 = 75;
 *    random = ran32_0or1(&ran32_seed, prob_1);
 *
 *    // generate a random double in range 0.0 to 1.0 inclusive
 *    double random_double;
 *    random_double = ran32_double(&ran32_seed);
 *
 *    // generate a 64bit random uint64_t 
 *    uint64_t r64;
 *    r64 = ran32_64bits(&ran32_seed);
 */



/*
 * The random number generation algorithm is:
 *
 *    seed = ( seed * A + C ) % M
 *
 * where, to get a sequence of maximum period M, one must select
 *
 *    (A-1) is multiple of p for each p which divides M
 *    (A-1) is a multiple of 4
 *    C,M must be relatively prime
 *
 *
 * Caution, this generator has poor low bit randomness, 
 * but does have maximum period (2^32).
 *
 * Current choices for LCG parameters:
 *
 *    M = 2^32           (eliminates need for modulus operation)
 *    A = 4*3*5*7*11*13*17*19*23 + 1
 *    C = 0x37459123
 *
 *    SCALE = (1/(2^32-1))    
 */

#define RAN32_A                       ((unsigned int) 446185741)
#define RAN32_C                       ((unsigned int) 0x37459123)
#define RAN32_SCALE                   ((double) 1.0)/((double) 4294967295.0)
#define RAN32_MASK                    ((unsigned int) 0xffffffff)

extern inline void ran32_set_seed(unsigned int *seed_p, unsigned int seed)
{
  *seed_p = (seed & RAN32_MASK);
}

extern inline unsigned int ran32_get_seed(unsigned int *seed_p)
{
  return ((*seed_p) & RAN32_MASK);
}

// WARNING: Rob observed that this returns even, odd, even, odd, etc. 
//          32b random numbers when called multiple times
extern inline unsigned int ran32_uint(unsigned int *seed_p)
{
  *seed_p = ((*seed_p)*RAN32_A + RAN32_C) & RAN32_MASK;

  return *seed_p;
}

extern inline unsigned int ran32_nbits(unsigned int *seed_p, unsigned int n)
{
  *seed_p = ((*seed_p)*RAN32_A + RAN32_C) & RAN32_MASK;

  return ( *seed_p >> (32 - n) );
}

extern inline unsigned int ran32_nbits1prob(unsigned int *seed_p,
                                            unsigned int n,
                                            unsigned int prob_only_1)
{
  return ( (ran32_uint(seed_p) < ((0xFFFFFFFF / 100) * prob_only_1)) ?
             0x1 << (ran32_uint(seed_p) % n) : ran32_nbits(seed_p, n) );
}

extern inline unsigned int ran32_0or1(unsigned int *seed_p,
                                      unsigned int prob_1)
{
  return ( (ran32_uint(seed_p) < ((0xFFFFFFFF / 100) * prob_1)) ?  1 : 0 );
}

extern inline double ran32_double(unsigned int *seed_p)
{
  *seed_p = ((*seed_p)*RAN32_A + RAN32_C) & RAN32_MASK;

  return ((double) *seed_p) * RAN32_SCALE;
}

extern inline uint64_t ran32_64bits(unsigned int *seed_p)
{
  // WARNING: Phil and Rob think this is not a very good 64b 
  //          random number generator so use at your own risk
  // make 2 calls to ran_32uint and shift the results 
  // to get 64 random bits
  // ran32_uint seems to produce even, then odd, then even numbers, etc
  // so try to randomize least significant bit
  uint64_t r64;
  uint32_t r32 = ran32_uint(seed_p);
  uint32_t lsb = (r32 >> (r32 & 0x01F)) & 0x01;
  r64 = _B32(31,((r32 & 0x0FFFFFFFE) | lsb));
  //printf("r32=%016x, lsb=%016x, r64=%016lx\n", r32, lsb, r64);
  r32 = ran32_uint(seed_p);
  lsb = (r32 >> (r32 & 0x01F)) & 0x01;
  r64 |= _B32(63,((r32 & 0x0FFFFFFFE) | lsb));
  //printf("r32=%016x, lsb=%016x, r64=%016lx\n", r32, lsb, r64);
  return r64;
  
}

/* extern inline uint64_t ran32_64bits(unsigned int *seed_p) */
/* { */
/*   // make 2 calls to ran_32uint and shift the results  */
/*   // to get 64 random bits */
/*   uint64_t r64; */
/*   uint32_t r32 = ran32_uint(seed_p); */
/*   r64 = _B32(31,r32); */
/*   r32 = ran32_uint(seed_p); */
/*   r64 |=  _B32(63,r32); */
/*   return r64; */
  
/* } */



__END_DECLS



#endif // Add nothing below this line
