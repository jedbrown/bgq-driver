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
 * \file math/_dual_src.x.h
 * \brief ???
 */
// X-Macro for dual source functions. i.e., c=op(a,b)

#ifdef OP
#ifdef TYPE

PAMI_assert_debug(nsrc == 2);
register int n=0;
TYPE *s0 = (TYPE *)srcs[0];
TYPE *s1 = (TYPE *)srcs[1];
#ifndef NOT_UNROLLED_LOOP
register TYPE rbuffer0, rbuffer1, rbuffer2, rbuffer3;
register TYPE buf00, buf01, buf02, buf03;
register TYPE buf10, buf11, buf12, buf13;
for(; n<count-3; n+=4)
  {
    buf00 = s0[n+0]; buf10 = s1[n+0];
    buf01 = s0[n+1]; buf11 = s1[n+1];
    buf02 = s0[n+2]; buf12 = s1[n+2];
    buf03 = s0[n+3]; buf13 = s1[n+3];

    rbuffer0 = OP(buf00,buf10);
    rbuffer1 = OP(buf01,buf11);
    rbuffer2 = OP(buf02,buf12);
    rbuffer3 = OP(buf03,buf13);

    dst[n+0] = rbuffer0;
    dst[n+1] = rbuffer1;
    dst[n+2] = rbuffer2;
    dst[n+3] = rbuffer3;
  }
#endif
for(; n<count; n++)
  {
    dst[n] = OP(s0[n],s1[n]);
  }

#endif
#endif
