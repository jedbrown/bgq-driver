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

register double f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15;
register double res = 0.0;
register  unsigned n;

  f0 = src0[0];
  f1 = src1[0];
  f2 = src2[0];
  f3 = src3[0];
  f4 = src4[0];
  f5 = src5[0];
  f6 = src6[0];
  f7 = src7[0];
  f8 = src8[0];
  f9 = src9[0];
  f10 = src10[0];
  f11 = src11[0];
  f12 = src12[0];
  f13 = src13[0];
  f14 = src14[0];
  f15 = src15[0];

  for (n =1; n < num_dbls; n++)
  {
    res = OP(f0, f1);
    f0  = src0[n];

    f1  = src1[n];

    res = OP(f2, res);
    f2  = src2[n];

    res = OP(f3, res);
    f3  = src3[n];

    res = OP(f4, res);
    f4  = src4[n];

    res = OP(f5, res);
    f5  = src5[n];

    res = OP(f6, res);
    f6  = src6[n];

    res = OP(f7, res);
    f7  = src7[n];

    res = OP(f8, res);
    f8  = src8[n];

    res = OP(f9, res);
    f9  = src9[n];

    res = OP(f10, res);
    f10  = src10[n];

    res = OP(f11, res);
    f11  = src11[n];

    res = OP(f12, res);
    f12  = src12[n];

    res = OP(f13, res);
    f13  = src13[n];

    res = OP(f14, res);
    f14  = src14[n];

    res = OP(f15, res);
    f15  = src15[n];
  
    dst[n-1]  = res;
    res = 0.0;
  }

  res = OP(f0, f1);
  res = OP(f2, res);
  res = OP(f3, res);
  res = OP(f4, res);
  res = OP(f5, res);
  res = OP(f6, res);
  res = OP(f7, res);
  res = OP(f8, res);
  res = OP(f9, res);
  res = OP(f10, res);
  res = OP(f11, res);
  res = OP(f12, res);
  res = OP(f13, res);
  res = OP(f14, res);
  res = OP(f15, res);
  dst[n-1]  = res;
#endif	/* OP */
