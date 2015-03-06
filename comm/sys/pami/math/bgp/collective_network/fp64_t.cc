/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

/**
 * \file math/bgp/collective_network/fp64_t.cc
 * \brief Default C math routines for 64 bit floating point operations.
 */

#include "pami_bg_math.h"
#include "CNUtil.h"
#include "util/common.h"
//#include "internal.h"
#include "math/FloatUtil.h"
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"

extern "C" size_t _g_num_active_nodes;

/*
 * u2 = firstword - u2
 * Need f since PPC unhappy with: *w1=(ADD_OP_T*)&firstword
 */
static void subfromfirstword(int ulen, uint64_t firstword, uint64_t *u2)
{
  int ratio=sizeof(uint64_t)/sizeof(ADD_OP_T);
  int i, wlen=ulen*ratio, carry=0;

  /**  \note In order for this code to work, we need these volatiles
   *  The gcc compiler seems to be generating bad code for this chunk
   *  of code without the volatiles
   *
   *  \todo track down root cause of the need for these volatiles
   */
  volatile uint64_t f=firstword;
  volatile uint64_t *df = &f;
  ADD_OP_T *w1=(ADD_OP_T*)df, *w2=(ADD_OP_T*)u2;
  ADD_OP2_T sub;
  for (i=wlen-1; i>=ratio; i--)
    {
      sub = (OP_10000 - w2[i]) - carry;
      w2[i] = sub & OP_FFFF;
      carry = (sub < OP_10000) ? 1 : 0;
    }
  for (i=ratio-1; i>=0; i--)
    {
      sub = ((OP_10000 + w1[i]) - w2[i]) - carry;
      w2[i] = sub & OP_FFFF;
      carry = (sub < OP_10000) ? 1 : 0;
    }
}

/*
 * u1 = u1 - firstword
 */
static void subfirstword(int ulen, uint64_t *u1, uint64_t firstword)
{
  int ratio=sizeof(uint64_t)/sizeof(ADD_OP_T);
  int i, wlen=ulen*ratio, carry=0;
  uint64_t f=firstword;
  uint64_t *df=&f;
  ADD_OP_T *w1=(ADD_OP_T*)u1, *w2=(ADD_OP_T*)df;
  ADD_OP2_T sub;
  for (i=wlen-1; i>=ratio; i--)
    {
      sub = (OP_10000 + w1[i]) - carry;
      w1[i] = sub & OP_FFFF;
      carry = (sub < OP_10000) ? 1 : 0;
    }
  for (i=ratio-1; i>=0; i--)
    {
      sub = ((OP_10000 + w1[i]) - w2[i]) - carry;
      w1[i] = sub & OP_FFFF;
      carry = (sub < OP_10000) ? 1 : 0;
    }
}

int double2uint(double *d, int ulen, uint64_t *u)
{
  uint64_t b = *((uint64_t*)d);  /* b is original*/
  uint64_t m = *((uint64_t*)d);  /* m is used to shift up mantissa.*/
  int blen = ulen*64;            /* length of u in bits.*/
  int i;

  int origfirstbit;     /* bit position in u, before exponent shift.*/
  int newfirstbit;      /* bit position in u, after  exponent shift.*/
  int firstword;        /* newfirstbit starts in u[firstword]
                         * The remaining bits are in u[firstword+1].*/
  int bitsinfirstword;  /*The number of bits in u[firstword].*/

  if (DOUBLE_EXP(b) == 2047)
    {
      u[0] = BGL_TR_BAD_OPERAND;  /* No need to set other u[i].*/
      return -1; /* NAN */
    }
  /*
   * Shift up just the mantissa, so turn off the sign.
   */
  m &= ~DOUBLE_BITS_SIGN;
  if (m & DOUBLE_BITS_EXP)
    {
      /*
       * Normalized number, i.e. non-0 exponent,
       * has implicit leading 1 on mantissa.
       * Use bit-layout feature that exponent LSB precedes mantissa MSB.
       */
      SET_DOUBLE_EXP(m,1);
    }
  /* Shift up the entire 64-bit mantissa m.
   * Bit 11 is the implicit 1.
   * For the maximum exponent supported, bit 10 is effective sign bit.
   * So first 10 bits 0:9 in m are always 0.
   * To avoid overflow on 2^16==65536 nodes, need topmost 16 bits of u to be 0.
   * So newfirstbit==0 would have only 10 free bits.
   * So require newfirstbit >= 6.
   * So bit 16 in BGL_TR_HUMUNGOUS gets added to every number,
   * leaving 16 bits 0:15 as 0.
   * Addendum:
   * Prepending the above, need the topmost 17 bits to be 0,
   * in order to propagate one or more nodes inserting NaN or Inf.
   * 17 bits need when all 2^16 nodes contribute NAn or Inf.
   * So require newfirstbit >=6+17.
   * So bit 16+17=33 in BGL_TR_HUMUNGOUS gets added to every number,
   * leaving 16+17=33 bits 0:32 as 0.

   * Optimization possibility:
   * Instead of assuming 2^16 participants,
   * could get actual number of participants as argument
   * and decrease the 16+17=33 bits appropriately.
   */
  origfirstbit = blen - 64;
  if (DOUBLE_EXP(b) <= 1)
    {
      /* Don't shift at all if e = 0 or if e = 1.*/
      newfirstbit  = origfirstbit;
    }
  else
    {
      /* Shift up by e-1. */
      newfirstbit  = origfirstbit - (DOUBLE_EXP(b)-1);
    }
  if (newfirstbit<6+17)
    {
      /* I.e. ulen is too small.*/
      u[0] = BGL_TR_BAD_OPERAND;  /* No need to set other u[i].*/
      return -1; /* exponent too large */
    }

  for (i=0; i<ulen; i++)
    u[i] = 0;

  firstword = newfirstbit / 64;
  bitsinfirstword = 64 - (newfirstbit % 64);
  u[firstword  ] = m >> (64 - bitsinfirstword);
  if (firstword < ulen-1)
    {
      u[firstword+1] = m << (     bitsinfirstword);
    }

  /* Tree only does unsigned add.
   * Tree doesn't like two's complement since alu_overflow_irq may fire.
   * e.g. ADD(-1,-1) == ADD(FFFF,FFFF) assuming 16-bit operands.
   * [If sacrifice initial 16-bit of 0, alu_overflow_irq won't fire.]
   * So instead treat negative numbers by moving numbers to positive range.
   */
  if (*d >= 0)
    {
      /* Positive numbers just set bit 16.*/
      u[0] |= BGL_TR_HUMUNGOUS;
    }
  else
    {
      uint64_t d = BGL_TR_HUMUNGOUS;
      /* u = d - u*/
      subfromfirstword(ulen, d, u);
    }
  return 0;
}


int uint2double(int participants, double *d, int ulen, uint64_t *u)
{
  uint64_t b=0, nan=0;
  uint64_t m, p;

  int firstword; /* The firstword of u containing non-0 bits.*/
  int firstbit;  /* The first bit within firstword;
                    This does not include the leading 0.*/
  int bitsinfirstword;  /* 64 - firstbit.*/

  if (u[0] & 0xFFFF800000000000ull)
    {
      SET_DOUBLE_EXP(nan,2047);
      SET_DOUBLE_MANTISSA(nan, 1);

      Core_memcpy(d,&nan,sizeof(double));
      return -1; /* NAN */
    }

  *d = 0.; /* In case of early exit.*/

  p = BGL_TR_HUMUNGOUS * participants;
  if (u[0]>=p)
    {
      b &= ~DOUBLE_BITS_SIGN;     /* Turn off the sign.*/
      /* u = u - p */
      subfirstword(ulen, u, p);
    }
  else
    {
      b |=  DOUBLE_BITS_SIGN;     /* Turn on  the sign.*/
      /* u = p - u */
      subfromfirstword(ulen, p, u);
    }

  for (firstword=0; firstword<ulen; firstword++)
    {
      if (u[firstword])
        {
          break;
        }
    }
  if (firstword == ulen)
    {
      return 0;
    }
  firstbit = cntlz64(u[firstword]);
  if (firstword == (ulen-1) && firstbit >= 12)
    {
      /* firstbit==12 and beyond is unnormalized number.*/
      SET_DOUBLE_EXP(b,0);
      firstbit = 12;
    }
  else
    {
      /* E.g. original firstbit 11 is normalized number, but there is no shift.*/
      int e = 64*(ulen-1-firstword) + 12 - firstbit;
      if (e >= 2047)
        {
          SET_DOUBLE_EXP(b,2047);
          SET_DOUBLE_MANTISSA(b, 1);
          Core_memcpy(d,&b,sizeof(double));
          return -1; /* NAN */
        }
      SET_DOUBLE_EXP(b, e);
      /* For normalized number, ++firstbit ignores the implicit 1,
       * which is not included in the mantissa.
       */
      ++firstbit;
    }

  /* firstbit ranges from 1 to 64 inclusive.*/
  bitsinfirstword = 64 - firstbit;

  /* Assemble m starting at bit 0,
   * since we don't know whether to shift up or down.
   */
  m = u[firstword] << firstbit;
  m |= u[firstword+1] >> bitsinfirstword;
  SET_DOUBLE_MANTISSA(b, m >> 12 );

  /* The above truncates the mantissa.
   * Could, but don't bother to round, since the tree ADD doesn't round.
   * So missing single last bit of precision is negligible
   * compared to the much larger differences that can arise
   * between summing many doubles and summing 2048-bit integers.
   */

  Core_memcpy(d,&b,sizeof(double));

  return 0;
}

void _pami_core_fp64_pre_all(double *dst, const double *src, int count) {
  PAMI_abort();
}

void _pami_core_fp64_post_all(double *dst, const double *src, int count) {
  PAMI_abort();
}

void _pami_core_fp64_pre_sum(double *dst, const double *src, int count) {
        int c = 0;
        double *s = (double *)src;
        struct b { char b[BGPCN_PKT_SIZE]; } *d = (struct b *)dst;
        for (c = 0; c < count; c++) {
                double2uint(s, PKTWORDS, (uint64_t *)d);
                ++s;
                ++d;
        }
}

void _pami_core_fp64_post_sum(double *dst, const double *src, int count) {
        int c = 0;
        struct b { char b[BGPCN_PKT_SIZE]; } *s = (struct b *)src;
        double *d = dst;
        for (c = 0; c < count; c++) {
                uint2double(_g_num_active_nodes, d, PKTWORDS, (uint64_t *)s);
                ++s;
                ++d;
        }
}

void _pami_core_fp64_pre_max(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MAX_DOUBLE_TOTREE(s0);
      d1 = MAX_DOUBLE_TOTREE(s1);
      d2 = MAX_DOUBLE_TOTREE(s2);
      d3 = MAX_DOUBLE_TOTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MAX_DOUBLE_TOTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_post_max(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MAX_DOUBLE_FRTREE(s0);
      d1 = MAX_DOUBLE_FRTREE(s1);
      d2 = MAX_DOUBLE_FRTREE(s2);
      d3 = MAX_DOUBLE_FRTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MAX_DOUBLE_FRTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_int32_pre_maxloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_TOTREE(s[n].a);
      d[n].b = LOC_INT_TOTREE(s[n].b);
      d[n].z = 0;
    }
}

void _pami_core_fp64_int32_post_maxloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_FRTREE(s[n].a);
      d[n].b = LOC_INT_FRTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_pre_maxloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_TOTREE(s[n].a);
      d[n].b = MIN_DOUBLE_TOTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_post_maxloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_FRTREE(s[n].a);
      d[n].b = MIN_DOUBLE_FRTREE(s[n].b);
    }
}

void _pami_core_fp64_pre_min(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MIN_DOUBLE_TOTREE(s0);
      d1 = MIN_DOUBLE_TOTREE(s1);
      d2 = MIN_DOUBLE_TOTREE(s2);
      d3 = MIN_DOUBLE_TOTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MIN_DOUBLE_TOTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_post_min(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MIN_DOUBLE_FRTREE(s0);
      d1 = MIN_DOUBLE_FRTREE(s1);
      d2 = MIN_DOUBLE_FRTREE(s2);
      d3 = MIN_DOUBLE_FRTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MIN_DOUBLE_FRTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_int32_pre_minloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_TOTREE(s[n].a);
      d[n].b = LOC_INT_TOTREE(s[n].b);
      d[n].z = 0;
    }
}

void _pami_core_fp64_int32_post_minloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_FRTREE(s[n].a);
      d[n].b = LOC_INT_FRTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_pre_minloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_TOTREE(s[n].a);
      d[n].b = MIN_DOUBLE_TOTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_post_minloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_FRTREE(s[n].a);
      d[n].b = MIN_DOUBLE_FRTREE(s[n].b);
    }
}

/**
 * \brief Code template to split a double into exponent and mantissa
 *
 * \param[in] src	Pointer to doubles to be split
 * \param[out] exp	Where to save exponent
 * \param[out] mp	Where to save 96-bit mantissa
 */
static inline void split_dbl(const double *src, uint32_t *exp, uint32_t *mp) {
        uint32_t *sp = (uint32_t *)src;
        uint32_t mh, ml, x;
        uint32_t neg;

        mh = sp[0];
        ml = sp[1];
        x = (mh >> 20) & 0x000007ffUL;
        neg = ((mh & 0x80000000UL) != 0);
        mh &= 0x000fffff;
        if (x) mh |= 0x00100000UL;
        if (neg) {
                asm volatile (
                        "nor %0,%0,%0;"
                        "addic %0,%0,1;"
                        "nor %1,%1,%1;"
                        "addze %1,%1;"
                        : "+r"(ml),
                          "+r"(mh)
                        );
        }
        mp[0] = mh;
        mp[1] = ml;
        *exp = x;
}

/**
 * dst_e must be count * sizeof(uint16_t) in length.
 * dst_m must be count * 3 * sizeof(uint32_t) in length.
 *
 * Manitssas are not (yet) shifted. That must wait for the MAX(exp).
 *
 * This routine works on exponent-packet units.
 * This routine is not capable of spanning packet boundaries.
 * since that causes more overhead, both here and in caller's code,
 * it is probably OK.
 *
 * It does, however, handle mantissa packet boundaries.
 */
void _pami_core_fp64_pre1_2pass(uint16_t *dst_e, uint32_t *dst_m, const double *src, int count) {
        int n;
        uint32_t x, x2;
        uint32_t *ep = (uint32_t *)dst_e;
        uint32_t *mp = (uint32_t *)dst_m;
        uint32_t align = (unsigned)mp & 0x00ffUL;
        const double *sp = src;

        // PAMI_assert_debug(((unsigned)mp & 0x00ffUL) == 0x0000);
        // NOTE: first 4 bytes of mant pkt are unused,
        //       last 2 bytes of expo pkt are unused...
        n = count >> 1;
        while (n-- > 0) {
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                split_dbl(sp, &x, mp);
                ++sp;
                mp += 3;
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                split_dbl(sp, &x2, mp);
                ++sp;
                mp += 3;
                *ep++ = (x << 16) | x2;
        }
        if (count & 1) {
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                split_dbl(sp, &x, mp);
                //++sp;
                //mp += 3;
                *((uint16_t *)ep) = x;
        }
}

/**
 * \brief Code template to shift mantissas based on MAX(exponent)
 *
 * \param[in] max_e		Pointer to MAX(exponent)
 * \param[in] expo		Pointer to local exponent
 * \param[in,out] src_dst	Where to load 52-bit mantissa and store 96-bit result
 */
static inline void adj_mant(uint16_t max_e, uint16_t expo, uint32_t *src_dst) {
        uint16_t dx;
        uint32_t *mp = src_dst;
        int32_t mh, ml, mt;

        // NOTE: since we add 6 here, dx will never be zero.
        dx = (max_e + EXPO_PRE_OFFSET) - expo;
        if (dx >= 96) {
                mp[0] = 0;
                mp[1] = 0;
                mp[2] = 0;
        } else if (dx >= 64) {
                dx -= 64;
                mt = mp[0];
                mh = (mt >> 31); // sign only
                mp[0] = mh;
                mp[1] = mh;
                mp[2] = (mt >> dx);
        } else if (dx >= 32) {
                dx -= 32;
                uint32_t umt = mp[1];
                ml = mp[0];
                mp[0] = (ml >> 31); // sign only
                int64_t m = ((int64_t)ml << 32) | umt;
                m = m >> dx;
                mp[1] = (m >> 32) & 0x00000000ffffffffULL;
                mp[2] = m & 0x00000000ffffffffULL;
        } else {
                uint32_t uml = mp[1];
                mt = uml << (32 - dx);
                mh = mp[0];
                uint64_t m = ((((uint64_t)mh << 32) | uml) >> dx);
                ml = m & 0x00000000ffffffffULL;
                mp[0] = (mh >> dx);	// keep sign
                mp[1] = ml;
                mp[2] = mt;
        }
}

/**
 * Shift mantissa into a normalized position, based on MAX(exponent)
 *
 * This routine works on exponent-packet units.
 * This routine is not capable of spanning packet boundaries.
 * since that causes more overhead, both here and in caller's code,
 * it is probably OK.
 *
 * It does, however, handle mantissa packet boundaries.
 *
 * We always are starting at a Mantissa Packet boundary, but the
 * exponents might be mid-packet. They must however be synced.
 */
void _pami_core_fp64_pre2_2pass(uint32_t *dst_src_m, uint16_t *src_e,
                                uint16_t *src_ee, int count) {
        uint32_t *oe = (uint32_t *)src_e;
        uint32_t *xe = (uint32_t *)src_ee;
        uint32_t *mp = dst_src_m;
        uint32_t mxe, exp;
        uint32_t align = (unsigned)mp & 0x00ffUL;
        int n;
        // PAMI_assert_debug(((unsigned)mp & 0x00ffUL) == 0x0000);
        // PAMI_assert_debug((((unsigned)oe ^ (unsigned)xe) & 0x00ffUL) == 0x0000);
        n = count >> 1;
        while (n-- > 0) {
                mxe = *xe++;
                exp = *oe++;
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                adj_mant(mxe >> 16, exp >> 16, mp);
                mp += 3;
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                adj_mant(mxe & 0x0ffffUL, exp & 0x0ffffUL, mp);
                mp += 3;
        }
        if (count & 1) {
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                adj_mant(*((uint16_t *)xe), *((uint16_t *)oe), mp);
                // mp += 3;
        }
}

/**
 * \brief Code template to regenerate double from mantissa (SUM) and exponent (MAX)
 *
 * \param[in] mh	Mantissa, hi 32 bits
 * \param[in] ml	Mantissa, mid 32 bits
 * \param[in] mt	Mantissa, lo 32 bits
 * \param[in] ex	Exponent
 * \param[out] dst	Where to store result (double, as two 32-bit ints)
 */
static inline void regen(int32_t mh, int32_t ml, int32_t mt, uint16_t ex, double *dst) {
        uint32_t *dp = (uint32_t *)dst;
        uint32_t neg, dx = 0;
        int32_t nx;

        if (mh | ml | mt) {
                // only normalize if non-zero
                neg = (mh & 0x80000000UL);
                if (neg) {
                        asm volatile (
                                "nor %0,%0,%0;"
                                "addic %0,%0,1;"
                                "nor %1,%1,%1;"
                                "addze %1,%1;"
                                "nor %2,%2,%2;"
                                "addze %2,%2;"
                                : "+r"(mt),
                                  "+r"(ml),
                                  "+r"(mh)
                                );
                }
                if (ex >= 0x7ff) {
                        dp[0] = 0x07ff7000UL;	// NAN
                        dp[1] = 0;
                        return;
                }
                nx = ex + EXPO_POST_OFFSET;
                if (mh) {
                } else if (ml) {
                        mh = ml;
                        ml = mt;
                        mt = 0;
                        nx -= 32;
                } else {
                        mh = mt;
                        ml = 0;
                        mt = 0;
                        nx -= 64;
                }
                asm volatile (
                        "cntlzw %1,%0"
                        : "+r"(mh),
                          "+r"(dx)
                        );
                nx -= dx;
                if (nx >= 0x7ff) {
                        dp[0] = 0x07ff0000UL;	// INF
                        dp[1] = 0;
                        return;
                }
                if (dx < 11) {
                        // shift right...
                        dx = 11 - dx;
                        uint32_t uml = ml;
                        ml = ((((uint64_t)mh << 32) | uml) >> dx) & 0x00000000ffffffffULL;
                        mh = mh >> dx;
                } else {
                        dx -= 11;
                        if (nx < 1) {
                                nx = 0;
                                dx = ex + EXPO_PRE_OFFSET - (ex != 0);
                        }
                        if (dx) {
                                dx = 32 - dx;
                                uint32_t uml = ml;
                                uint32_t umt = mt;
                                mh = ((((uint64_t)mh << 32) |
                                        uml) >> dx) & 0x00000000ffffffffULL;
                                ml = ((((uint64_t)uml << 32) |
                                        umt) >> dx) & 0x00000000ffffffffULL;
                        }
                }
                dp[0] = neg | ((nx & 0x07ff) << 20) | (mh & 0x000fffffUL);
                dp[1] = ml;
        } else {
                dp[0] = 0;
                dp[1] = 0;
        }
}

/**
 * This routine is not capable of spanning exponent packet boundaries.
 * since that causes more overhead, both here and in caller's code,
 * it is probably OK.
 *
 * It does, however, handle mantissa packet boundaries.
 *
 * Note, exponents are not signed quantities. 0x000 represents the
 * smallest exponent (~ e-309 if mantissa bits are one).
 * therefore we don't have to worry about exponent signs.
 */
void _pami_core_fp64_post_2pass(double *dst, uint16_t *src_e, uint32_t *src_m, int count) {
        uint32_t *ep = (uint32_t *)src_e;
        uint32_t *mp = src_m;
        double *dp = dst;
        uint32_t ux;
        int32_t mh, ml, mt;
        uint32_t align = (unsigned)mp & 0x00ffUL;

        // PAMI_assert_debug(((unsigned)mp & 0x00ffUL) == 0x0000);
        // PAMI_assert_debug(n <= EXPO_PER_PKT);
        int n = count >> 1;
        while (n-- > 0) {
                ux = *ep++;
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                mh = *mp++;
                ml = *mp++;
                mt = *mp++;
                regen(mh, ml, mt, (ux >> 16), dp);
                ++dp;

                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                mh = *mp++;
                ml = *mp++;
                mt = *mp++;
                regen(mh, ml, mt, (ux & 0x0000ffffUL), dp);
                ++dp;
        }
        if (count & 1) {
                if (((unsigned)mp & 0x00ffUL) == align) {
                        ++mp;
                }
                mh = *mp++;
                ml = *mp++;
                mt = *mp++;
                regen(mh, ml, mt, *((uint16_t *)ep), dp);
                //++dp;
        }
}
