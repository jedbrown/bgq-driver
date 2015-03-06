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
#ifdef TYPE

//#warning "MATH:  need to turn these compile time asserts back on"
PAMI_assert_debug(nsrc <= MATH_MAX_NSRC);
PAMI_assert_debug(nsrc >= 2);

register int n = 0, m;
TYPE buf0[MATH_MAX_NSRC];
register TYPE rbuffer0;
#ifndef NOT_UNROLLED_LOOP
register TYPE rbuffer1, rbuffer2, rbuffer3;
TYPE buf1[MATH_MAX_NSRC];
TYPE buf2[MATH_MAX_NSRC];
TYPE buf3[MATH_MAX_NSRC];
for (; n < count - 3; n += 4) {
    for (m = 0; m < nsrc; ++m) {
        buf0[m] = srcs[m][n+0];
        buf1[m] = srcs[m][n+1];
        buf2[m] = srcs[m][n+2];
        buf3[m] = srcs[m][n+3];
    }

    rbuffer0 = OP(buf0[0],buf0[1]);
    rbuffer1 = OP(buf1[0],buf1[1]);
    rbuffer2 = OP(buf2[0],buf2[1]);
    rbuffer3 = OP(buf3[0],buf3[1]);
    for (m = 2; m < nsrc; ++m) {
        rbuffer0 = OP(rbuffer0,buf0[m]);
        rbuffer1 = OP(rbuffer1,buf1[m]);
        rbuffer2 = OP(rbuffer2,buf2[m]);
        rbuffer3 = OP(rbuffer3,buf3[m]);
    }

    dst[n+0] = rbuffer0;
    dst[n+1] = rbuffer1;
    dst[n+2] = rbuffer2;
    dst[n+3] = rbuffer3;
}
#endif
for (; n < count; n++) {
    for (m = 0; m < nsrc; ++m) {
        buf0[m] = srcs[m][n];
    }
    rbuffer0 = OP(buf0[0],buf0[1]);
    for (m = 2; m < nsrc; ++m) {
        rbuffer0 = OP(rbuffer0,buf0[m]);
    }
    dst[n] = rbuffer0;
}

#endif	/* TYPE */
#endif	/* OP */
