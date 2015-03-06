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
 * \file math/a2qpx/qpx_copy.h
 * \brief ???
 */

#ifndef __math_a2qpx_qpx_copy_h__
#define __math_a2qpx_qpx_copy_h__

#include "math/a2qpx/a2qpx_asm.h"

//Copy 128 bytes from 32b aligned pointers
static inline size_t quad_copy_128( char* dest, char* src ) {  
    register double *fpp1_1;
    register double *fpp2_1;
    
    register double f0  FP_REGISTER(0);
    register double f1  FP_REGISTER(1);

    int r0;
    int r1;
    int r2;
    int r3;

    r0 = 0;
    r1 = 64;
    r2 = 32;
    r3 = 96;    

    fpp1_1 = (double *) src;
    fpp2_1 = (double *) dest;

    // may speed up the 2nd to 7th load by using non-update versions of load, tbd later...
    QPX_LOAD_NU(fpp1_1,r0,f0);
    QPX_LOAD_NU(fpp1_1,r1,f1);        

    //Wait for the load offset 0 to complete    
    QPX_STORE_NU(fpp2_1,r0,f0);
    //load bottom half of cache line
    QPX_LOAD_NU(fpp1_1,r2,f0);        

    QPX_STORE_NU(fpp2_1,r1,f1);
    QPX_LOAD_NU(fpp1_1,r3,f1);        
    
    QPX_STORE_NU(fpp2_1,r2,f0);
    QPX_STORE_NU(fpp2_1,r3,f1);

    return 0;
}

//Copy 512 bytes from a 32b aligned pointers
static inline size_t quad_copy_512( char* dest, char* src ) {  
    register double *fpp1_1, *fpp1_2;
    register double *fpp2_1, *fpp2_2;

    register double f0  FP_REGISTER(0);
    register double f1  FP_REGISTER(1);
    register double f2  FP_REGISTER(2);
    register double f3  FP_REGISTER(3);
    register double f4  FP_REGISTER(4);
    register double f5  FP_REGISTER(5);
    register double f6  FP_REGISTER(6);
    register double f7  FP_REGISTER(7);

    int r0;
    int r1;
    int r2;
    int r3;
    int r4;
    int r5;
    int r6;
    int r7;
    r0 = 0;
    r1 = 64;
    r2 = 128;
    r3 = 192;
    r4 = 256;
    r5 = 320;
    r6 = 384;
    r7 = 448;

    fpp1_1 = (double *)src;
    fpp1_2 = (double *)src +4;

    fpp2_1 = (double *)dest;
    fpp2_2 = (double *)dest +4;

    // may speed up the 2nd to 7th load by using non-update versions of load, tbd later...
    QPX_LOAD_NU(fpp1_1,r0,f0);
    //asm volatile("qvlfdx 0,%0,%1": : "Ob" (fpp1_1), "r"(r0) :"memory");
    QPX_LOAD_NU(fpp1_1,r1,f1);
    QPX_LOAD_NU(fpp1_1,r2,f2);
    QPX_LOAD_NU(fpp1_1,r3,f3);
    QPX_LOAD_NU(fpp1_1,r4,f4);
    QPX_LOAD_NU(fpp1_1,r5,f5);
    QPX_LOAD_NU(fpp1_1,r6,f6);
    QPX_LOAD_NU(fpp1_1,r7,f7);

    QPX_STORE_NU(fpp2_1,r0,f0);
    QPX_LOAD_NU(fpp1_2,r0,f0);
    QPX_STORE_NU(fpp2_1,r1,f1);
    QPX_LOAD_NU(fpp1_2,r1,f1);
    QPX_STORE_NU(fpp2_1,r2,f2);
    QPX_LOAD_NU(fpp1_2,r2,f2);   
    QPX_STORE_NU(fpp2_1,r3,f3);
    QPX_LOAD_NU(fpp1_2,r3,f3);
    QPX_STORE_NU(fpp2_1,r4,f4);
    QPX_LOAD_NU(fpp1_2,r4,f4);
    QPX_STORE_NU(fpp2_1,r5,f5);
    QPX_LOAD_NU(fpp1_2,r5,f5);
    QPX_STORE_NU(fpp2_1,r6,f6);
    QPX_LOAD_NU(fpp1_2,r6,f6);
    QPX_STORE_NU(fpp2_1,r7,f7);
    QPX_LOAD_NU(fpp1_2,r7,f7);
    
    QPX_STORE_NU(fpp2_2,r0,f0);
    QPX_STORE_NU(fpp2_2,r1,f1);
    QPX_STORE_NU(fpp2_2,r2,f2);
    QPX_STORE_NU(fpp2_2,r3,f3);
    QPX_STORE_NU(fpp2_2,r4,f4);
    QPX_STORE_NU(fpp2_2,r5,f5);
    QPX_STORE_NU(fpp2_2,r6,f6);
    QPX_STORE_NU(fpp2_2,r7,f7);

    return 0;
}

//copying 'num' bytes. Assume 32b alignment
static inline int quad_copy_128n( char* dest, char* src, size_t num ) 
{
  size_t b128 = num >> 7;
  size_t nr = num & ~(0x7FL);
  
  while (b128 --) {
    quad_copy_128(dest, src);
    dest += 128;
    src  += 128;
  }
  
  return nr;
}

extern size_t quad_copy_1024n( char* dest, char* src, size_t num );

#endif
