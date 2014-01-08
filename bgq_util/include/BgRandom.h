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

#ifndef __BG_RANDOM_H__
#define __BG_RANDOM_H__

#include <stdint.h>
#include <hwi/include/common/compiler_support.h>

#include "mt64.h"

__BEGIN_DECLS

// A minimalist Random Library, based on the 64-bit
// Mersenne Twister Random Number Generator

#ifndef Min
#define Min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef Max
#define Max(a,b) (((a)>(b))?(a):(b))
#endif



void     BgRandomInit(MTRandomArg *R, uint32_t AppSeed, uint32_t Offset);
double   BgRandomUniform(MTRandomArg *R);
double   BgRandomDouble(double a, double b, MTRandomArg *R);
uint64_t BgRandom64(MTRandomArg *R);
uint32_t BgRandom32(MTRandomArg *R);
uint64_t BgRandom64NBits(int n, MTRandomArg *R);
uint32_t BgRandom32NBits(int n, MTRandomArg *R);
uint32_t BgRandom32(MTRandomArg *R);
int      BgRandomInteger(int a, int b, MTRandomArg *R);

int      BgRandomPermutation(int n, int *p, MTRandomArg *R);
int      BgRandomBernoulli(double p, MTRandomArg *R);
int      BgRandomPdf(int n, double *p, MTRandomArg *R);
int      BgRandomCdf(int n, double *p, MTRandomArg *R);

int      BgRandomIntegerMixture(int n, double *p, int *a, int *b,
				MTRandomArg *R);
double   BgRandomDoubleMixture(int n, double *p, double *a, double *b,
			       MTRandomArg *R);


__INLINE__ double BgRandomUniform(MTRandomArg *R)
{
    return genrand64_real2(R);	// generates a random number on the
    				// [0,1) real interval.
}

__INLINE__ void BgRandomInit(MTRandomArg *R, uint32_t AppSeed, uint32_t Offset)

{
    // Take a single 32-bit AppSeed and an Offset and initialize the
    // the 64-bit seed for the Mersenne Twister 64 bit RNG.
    // This is quite arbitrary and does not guarantee non-overlapping
    // sequences for different offsets.
    // To get such a guarantee is time consuming according to
    // http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/DC/dc.html.
    // Create 64 bits containing AppSeed in the MSBs and ((1 + Offset) times
    // some number) in the LSBs.
    // Max offset on a big BG machine is 16^4*2*68 = 8,912,896 threads;
    // 16^4*2 = 131072 nodes (~26 PetaFlops).
    // 2^32 / (2 + 8912896) = 481.88, so multiply by some prime less than
    // 481 so as not to drop any bits.
    // 149 is prime;  (2^32)/149 = 28,825,283;
    // (28825283 threads) / (68 threads/node) = 423901 nodes (~85 PetaFlops).

    uint64_t sd;

    sd = (((uint64_t) AppSeed) << 32) | (149 * (1 + Offset));
    if (sd == 0) sd = 0x123456789ABULL;
    init_genrand64(sd, R);

    BgRandomUniform(R);  // call it several times to mix things up a bit more
    BgRandomUniform(R);  // should reduce chance that first random numbers
    			 // are close to each other
}





__INLINE__ int BgRandomInteger(int a, int b, MTRandomArg *R)
{
    // return a random integer between a and b, inclusive

    int d;
    double u;
    int min_ab = Min(a,b);
    int max_ab = Max(a,b);
    int delta  = max_ab + 1 - min_ab;


    u =  BgRandomUniform(R);
    d =  min_ab + (int) (delta*u);
    if (d < min_ab ) d = min_ab;
    if (d > max_ab ) d = max_ab;
    return d;
}




__INLINE__ uint32_t BgRandom32(MTRandomArg *R)
{
    // return a random uint32_t
    uint64_t v;
    uint32_t v32;
    v = genrand64_int64(R);  // 64 bits
    v32 = (uint32_t) (v & 0xFFFFFFFFULL);
    return v32;
}


__INLINE__ uint32_t BgRandom32NBits(int n, MTRandomArg *R)
{
    // return n bits out of a 32-bit random number
    uint32_t v;
    if (n <= 0) return 0;
    v = BgRandom32(R);
    if (n >= 32) return v;
    v = v << (32 - n);
    v = v >> (32 - n);
    return v;
}


__INLINE__ uint64_t BgRandom64(MTRandomArg *R)
{
    // return a random uint64_t
    return genrand64_int64(R);
}


__INLINE__ uint64_t BgRandom64NBits(int n,MTRandomArg *R)
{
    // return n bits out of a 64-bit random number
    uint64_t v;
    if (n <= 0) return 0;
    v = BgRandom64(R);
    if (n >= 64) return v;
    v = v << (64 - n);
    v = v >> (64 - n);
    return v;
}


__INLINE__ double BgRandomDouble(double a, double b, MTRandomArg *R)
{
    // return a random double between a and b
    double u;
    double min_ab = Min(a,b);
    double max_ab = Max(a,b);
    u = BgRandomUniform(R);
    return min_ab + u*(max_ab - min_ab);
}


__INLINE__ int BgRandomPermutation(int n, int* p, MTRandomArg *R)

{
    // return a random permutation of the numbers 1, 2, ..., n-1
    // in p[0], p[1], ..., p[n-1]
    // no number is repeated,
    // eg, if n=5  p[0]=4, p[1]=2, p[2]=0, p[3]=3, p[4]=1
    // rc = 0 means success, else failure (n < 0)

    if (n < 0) return 1;
    int i;
    int r, tmp;
    int b;
    for (i = 0; i < n; i++) p[i] = i;
    b = n-1;
    for (i = 0; i < n; i++)
    {
	r =  BgRandomInteger(i, b, R) ;  // sample r between i and n-1,
	tmp  = p[i];                     // swap p[i] with p[r]
	p[i] = p[r];
	p[r] = tmp;
    }

    return(0);
}


__INLINE__ int BgRandomBernoulli(double p, MTRandomArg *R)

{
    // returns 1 with probability p, else 0
    double u = BgRandomUniform(R);
    if (u <= p) return 1;
    return 0;
}


__INLINE__ int BgRandomPdf(int n, double* p, MTRandomArg *R)
{
    // p[0], ..., p[n-1] is a probability density function, P(X=i) = p[i]
    // returns i with probability p[i]
    // returns a negative number if n <= 0 or if a p[i] is found that is
    // less than 0.0

    if (n <= 0) return -1;
    int i;
    double u = BgRandomUniform(R);
    double cdf = 0.0;
    for (i = 0; i < n; i++)
    {
	if (p[i] < 0.0) return -(i+1);
	cdf += p[i];
	if (u <= cdf) return i;
    }
    // if we get here either the p[i]'s don't sum to less than 1.0
    // or there might be some roundoff error, return n-1, as if
    // the p[i]'s did sum to 1.0
    return (n-1);
}

__INLINE__ int BgRandomCdf(int n, double* p, MTRandomArg *R)
{
    // p[0], ..., p[n-1] is a cumulative probability distribution function
    // i.e., P(X <= i) = p[i]
    // returns a negative number if n <= 0 or a p[i] is found that is
    // less than 0.0

    if (n <= 0) return -1;
    int i;
    double u = BgRandomUniform(R);
    for (i = 0; i < n; i++)
    {
	if (p[i] < 0.0) return -(i+1);
	if (u <= p[i]) return i;
    }
    // if we get here either the last p[i] may be less than 1.0
    // return n-1, as if p[n-1] = 1.0
    return (n-1);
}


__INLINE__ int BgRandomIntegerMixture(int n, double *p, int *a, int *b,
				      MTRandomArg *R)
{
    // There are n intervals, sample interval i with probability p[i]
    // given i, sample uniformly between a[i] and b[i]
    // WARNING no error checks
    int i;
    i = BgRandomPdf(n, p, R);
    return BgRandomInteger(a[i], b[i], R);
}


__INLINE__ double BgRandomDoubleMixture(int n, double *p, double *a, double *b,
					MTRandomArg *R)
{
    // There are n intervals, sample interval i with probability p[i]
    // given i, sample uniformly between a[i] and b[i]
    // WARNING no error checks
    int i;
    i = BgRandomPdf(n, p, R);
    return BgRandomDouble(a[i], b[i], R);
}

__END_DECLS

#endif // __BG_RANDOM_H__
