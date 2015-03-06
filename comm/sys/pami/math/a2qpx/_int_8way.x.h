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
// 8way operation on ints

#ifdef OP
#ifdef DTYPE
register DTYPE r0, r1, r2, r3, r4, r5, r6, r7;
register DTYPE res = 0;
register  unsigned  n;

  r0 = src0[0];
  r1 = src1[0];
  r2 = src2[0];
  r3 = src3[0];
  r4 = src4[0];
  r5 = src5[0];
  r6 = src6[0];
  r7 = src7[0];

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
  dst[n-1]  = res;
#endif	/* DTYPE */
#endif	/* OP */

