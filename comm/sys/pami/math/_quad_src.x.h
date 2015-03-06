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
 * \file math/_quad_src.x.h
 * \brief 4-way math generic unoptimized implementation
 */
// X-Macro for quad (4-way) source functions. i.e., e=op(a,b,c,d)

#ifdef OP
#ifdef TYPE

PAMI_assert_debug(nsrc == 4);
register int n=0;
#ifndef NOT_UNROLLED_LOOP
register TYPE rbuffer0, rbuffer1, rbuffer2, rbuffer3;
register TYPE buf00, buf01, buf02, buf03;
register TYPE buf10, buf11, buf12, buf13;
register TYPE buf20, buf21, buf22, buf23;
register TYPE buf30, buf31, buf32, buf33;
for(; n<count-3; n+=4)
  {
    buf00 = src0[n+0]; buf10 = src1[n+0];
    buf01 = src0[n+1]; buf11 = src1[n+1];
    buf02 = src0[n+2]; buf12 = src1[n+2];
    buf03 = src0[n+3]; buf13 = src1[n+3];

    buf20 = src2[n+0]; buf30 = src3[n+0];
    buf21 = src2[n+1]; buf31 = src3[n+1];
    buf22 = src2[n+2]; buf32 = src3[n+2];
    buf23 = src2[n+3]; buf33 = src3[n+3];

    rbuffer0 = OP(buf00,buf10,buf20,buf30);
    rbuffer1 = OP(buf01,buf11,buf21,buf31);
    rbuffer2 = OP(buf02,buf12,buf22,buf32);
    rbuffer3 = OP(buf03,buf13,buf23,buf33);

    dst[n+0] = rbuffer0;
    dst[n+1] = rbuffer1;
    dst[n+2] = rbuffer2;
    dst[n+3] = rbuffer3;
  }
#endif
for(; n<count; n++)
  {
    dst[n] = OP(src0[n],src1[n],src2[n],src3[n]);
  }

#endif	/* TYPE */
#endif	/* OP */
