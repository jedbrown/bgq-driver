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
 * \file math/_N_src.x.h
 * \brief N-way math generic unoptimized implementation
 */

// X-Macro for generic (N-way) source functions using binary
// operands, i.e. e=op(a,b)

#ifdef OP
#ifdef DTYPE

register DTYPE r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
register DTYPE res = 0;
register  unsigned n;

  r0 = src0[0];
  r1 = src1[0];
  r2 = src2[0];
  r3 = src3[0];
  r4 = src4[0];
  r5 = src5[0];
  r6 = src6[0];
  r7 = src7[0];
  r8 = src8[0];
  r9 = src9[0];
  r10 = src10[0];
  r11 = src11[0];
  r12 = src12[0];
  r13 = src13[0];
  r14 = src14[0];
  r15 = src15[0];

  for (n =1; n < num_ints; n++)
  {
    res = OP(r0, r1);
    r0  = src0[n];
    r1  = src1[n];

    res = OP(r2, res);
    r2  = src2[n];

    res = OP(r3, res);
    r3  = src3[n];

    res = OP(r4, res);
    r4  = src4[n];

    res = OP(r5, res);
    r5  = src5[n];

    res = OP(r6, res);
    r6  = src6[n];

    res = OP(r7, res);
    r7  = src7[n];

    res = OP(r8, res);
    r8  = src8[n];

    res = OP(r9, res);
    r9  = src9[n];

    res = OP(r10, res);
    r10  = src10[n];

    res = OP(r11, res);
    r11  = src11[n];

    res = OP(r12, res);
    r12  = src12[n];

    res = OP(r13, res);
    r13  = src13[n];

    res = OP(r14, res);
    r14  = src14[n];

    res = OP(r15, res);
    r15  = src15[n];
  
    dst[n-1]  = res;
    res = 0;
  }

  res = OP(r0, r1);
  res = OP(r2, res);
  res = OP(r3, res);
  res = OP(r4, res);
  res = OP(r5, res);
  res = OP(r6, res);
  res = OP(r7, res);
  res = OP(r8, res);
  res = OP(r9, res);
  res = OP(r10, res);
  res = OP(r11, res);
  res = OP(r12, res);
  res = OP(r13, res);
  res = OP(r14, res);
  res = OP(r15, res);
  dst[n-1]  = res;
#endif	/* DTYPE */
#endif	/* OP */
