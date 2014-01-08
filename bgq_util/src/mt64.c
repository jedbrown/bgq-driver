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


#include "mt64.h"

/* initializes mt[NN_MERSENNE] with a seed */
void init_genrand64(unsigned long long seed,  
		      MTRandomArg *R)
{
    R->mti = NN_MERSENNE+1;
    R->mt[0] = seed;
    for (R->mti=1; R->mti<NN_MERSENNE; R->mti++) 
      R->mt[R->mti] =  (6364136223846793005ULL * (R->mt[R->mti-1] ^ (R->mt[R->mti-1] >> 62)) + R->mti);
}


/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */

void init_by_array64(unsigned long long init_key[],
		     unsigned long long key_length,
		     MTRandomArg *R)
{
    unsigned long long i, j, k;
    init_genrand64(19650218ULL,R);
    i=1; j=0;
    k = (NN_MERSENNE>key_length ? NN_MERSENNE : key_length);
    for (; k; k--) {
        R->mt[i] = (R->mt[i] ^ ((R->mt[i-1] ^ (R->mt[i-1] >> 62)) * 3935559000370003845ULL))
          + init_key[j] + j; /* non linear */
        i++; j++;
        if (i>=NN_MERSENNE) { R->mt[0] = R->mt[NN_MERSENNE-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=NN_MERSENNE-1; k; k--) {
        R->mt[i] = (R->mt[i] ^ ((R->mt[i-1] ^ (R->mt[i-1] >> 62)) * 2862933555777941757ULL))
          - i; /* non linear */
        i++;
        if (i>=NN_MERSENNE) { R->mt[0] = R->mt[NN_MERSENNE-1]; i=1; }
    }

    R->mt[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */ 
}



/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(MTRandomArg *R)
{
    int i;
    unsigned long long x;
    static unsigned long long mag01[2]={0ULL, MATRIX_A_MERSENNE};

    if (R->mti >= NN_MERSENNE) { /* generate NN_MERSENNE words at one time */

        /* if init_genrand64() has not been called, */
        /* a default initial seed is used     */
        if (R->mti == NN_MERSENNE+1) 
	  init_genrand64(5489ULL,R); 

        for (i=0;i<NN_MERSENNE-MM_MERSENNE;i++) {
            x = (R->mt[i]&UM_MERSENNE)|(R->mt[i+1]&LM_MERSENNE);
            R->mt[i] = R->mt[i+MM_MERSENNE] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        for (;i<NN_MERSENNE-1;i++) {
            x = (R->mt[i]&UM_MERSENNE)|(R->mt[i+1]&LM_MERSENNE);
            R->mt[i] = R->mt[i+(MM_MERSENNE-NN_MERSENNE)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        x = (R->mt[NN_MERSENNE-1]&UM_MERSENNE)|(R->mt[0]&LM_MERSENNE);
        R->mt[NN_MERSENNE -1] = R->mt[MM_MERSENNE-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];

        R->mti = 0;
    }
  
    x = R->mt[R->mti++];

    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);

    return x;
}




