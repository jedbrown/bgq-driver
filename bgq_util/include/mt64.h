/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef  __MT64_H__
#define  __MT64_H__

#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

/* 
   A C-program for MT19937-64 (2004/9/29 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)  
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and 
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and 
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/


/* Used with permission from Dr Makoto Matsumoto  dd 06/06/2004 */
/* Used with permission from ACM dd 10/27/2004 */

// Modified to include a structure  MTRandomArg that contains
// the state of the random number generator, rather than having
// the state be in global variables
// Calls contain a pointer to the structure
// Also Modified to change names, eg, NN -> NN_MERSENNE


#define NN_MERSENNE 312
#define MM_MERSENNE 156
#define MATRIX_A_MERSENNE 0xB5026F5AA96619E9ULL
#define UM_MERSENNE 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM_MERSENNE 0x7FFFFFFFULL /* Least significant 31 bits */



#include <stdint.h>

typedef struct {
    unsigned long long mt[NN_MERSENNE];
    int mti; 
} MTRandomArg;




/* initializes mt[NN_MERSENNE] with a seed */
void init_genrand64(unsigned long long seed, MTRandomArg *R);

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
// void init_by_array64(unsigned long long init_key[], 
//		     unsigned long long key_length,MTRandomArg *R);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(MTRandomArg *R);


/* generates a random number on [0, 2^63-1]-interval */
// long long genrand64_int63(MTRandomArg *R);

/* generates a random number on [0,1]-real-interval */
// double genrand64_real1(MTRandomArg *R);

/* generates a random number on [0,1)-real-interval */
// double genrand64_real2(MTRandomArg *R);

/* generates a random number on (0,1)-real-interval */
// double genrand64_real3(MTRandomArg *R);





/* generates a random number on [0, 2^63-1]-interval */
__INLINE__ long long genrand64_int63(MTRandomArg *R)
{
    return (long long)(genrand64_int64(R) >> 1);
}

/* generates a random number on [0,1]-real-interval */
__INLINE__ double genrand64_real1(MTRandomArg *R)
{
    return (genrand64_int64(R) >> 11) * (1.0/9007199254740991.0);
}

/* generates a random number on [0,1)-real-interval */
__INLINE__ double genrand64_real2(MTRandomArg *R)
{
    return (genrand64_int64(R) >> 11) * (1.0/9007199254740992.0);
}

/* generates a random number on (0,1)-real-interval */
__INLINE__ double genrand64_real3(MTRandomArg *R)
{
    return ((genrand64_int64(R) >> 12) + 0.5) * (1.0/4503599627370496.0);
}

__END_DECLS

#endif // __MT64_H__
