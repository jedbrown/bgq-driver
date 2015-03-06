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
 * \file math/a2qpx/qpx_copy.cc
 * \brief ???
 */

#include <stdint.h>
#include "util/common.h"
#include "math/a2qpx/qpx_copy.h"

//copying num bytes num >= 1024
size_t quad_copy_1024n( char* dest, char* src, size_t num )
{
  size_t nb = 0; 
  if (num < 1024)
    goto short_msg;

  register double f0  FP_REGISTER(0);
  register double f1  FP_REGISTER(1);
  register double f2  FP_REGISTER(2);
  register double f3  FP_REGISTER(3);
  register double f4  FP_REGISTER(4);
  register double f5  FP_REGISTER(5);
  register double f6  FP_REGISTER(6);
  register double f7  FP_REGISTER(7);
  register double f8  FP_REGISTER(8);
  register double f9  FP_REGISTER(9);
  register double f10 FP_REGISTER(10);
  register double f11 FP_REGISTER(11);

  nb = num & ~(1023L);

  double *fpp1_1, *fpp1_2;
  double *fpp2_1, *fpp2_2;
  size_t y;

  register int inc asm("r7");
  
  fpp1_1 = (double *)src -8;  //offset by stride=0 bytes
  fpp1_2 = (double *)src -4;

  fpp2_1 = (double *)dest -8;  //offset by stride=0 bytes
  fpp2_2 = (double *)dest -4;

  inc=64;

  y=((nb >> 3) - 64)/64;

  QPX_LOAD(fpp1_1,inc,f0);
  QPX_LOAD(fpp1_1,inc,f1);
  QPX_LOAD(fpp1_1,inc,f2);
  QPX_LOAD(fpp1_1,inc,f3);
  QPX_LOAD(fpp1_1,inc,f4);
  QPX_LOAD(fpp1_1,inc,f5);
  QPX_LOAD(fpp1_1,inc,f6);
  QPX_LOAD(fpp1_1,inc,f7);

  QPX_STORE(fpp2_1,inc,f0);
  QPX_LOAD(fpp1_1,inc,f0);
  QPX_LOAD(fpp1_2,inc,f8);

  QPX_STORE(fpp2_1,inc,f1);
  QPX_LOAD(fpp1_1,inc,f1);
  QPX_LOAD(fpp1_2,inc,f9);

  do{
    QPX_STORE(fpp2_1,inc,f2);
    QPX_LOAD(fpp1_1,inc,f2);
    QPX_LOAD(fpp1_2,inc,f10);
    QPX_STORE(fpp2_2,inc,f8);

    QPX_STORE(fpp2_1,inc,f3);
    QPX_LOAD(fpp1_1,inc,f3);
    QPX_LOAD(fpp1_2,inc,f11);
    QPX_STORE(fpp2_2,inc,f9);

    QPX_STORE(fpp2_1,inc,f4);
    QPX_LOAD(fpp1_1,inc,f4);
    QPX_LOAD(fpp1_2,inc,f8);
    QPX_STORE(fpp2_2,inc,f10);

    QPX_STORE(fpp2_1,inc,f5);
    QPX_LOAD(fpp1_1,inc,f5);
    QPX_LOAD(fpp1_2,inc,f9);
    QPX_STORE(fpp2_2,inc,f11);

    QPX_STORE(fpp2_1,inc,f6);
    QPX_LOAD(fpp1_1,inc,f6);
    QPX_LOAD(fpp1_2,inc,f10);
    QPX_STORE(fpp2_2,inc,f8);

    QPX_STORE(fpp2_1,inc,f7);
    QPX_LOAD(fpp1_1,inc,f7);
    QPX_LOAD(fpp1_2,inc,f11);
    QPX_STORE(fpp2_2,inc,f9);

    if(!--y) break;

    QPX_STORE(fpp2_1,inc,f0);
    QPX_LOAD(fpp1_1,inc,f0);
    QPX_LOAD(fpp1_2,inc,f8);
    QPX_STORE(fpp2_2,inc,f10);

    QPX_STORE(fpp2_1,inc,f1);
    QPX_LOAD(fpp1_1,inc,f1);
    QPX_LOAD(fpp1_2,inc,f9);
    QPX_STORE(fpp2_2,inc,f11);

  }while(1);

  QPX_STORE(fpp2_1,inc,f0);
  QPX_LOAD(fpp1_2,inc,f8);
  QPX_STORE(fpp2_2,inc,f10);

  QPX_STORE(fpp2_1,inc,f1);
  QPX_LOAD(fpp1_2,inc,f9);
  QPX_STORE(fpp2_2,inc,f11);

  QPX_STORE(fpp2_1,inc,f2);
  QPX_LOAD(fpp1_2,inc,f10);
  QPX_STORE(fpp2_2,inc,f8);

  QPX_STORE(fpp2_1,inc,f3);
  QPX_LOAD(fpp1_2,inc,f11);
  QPX_STORE(fpp2_2,inc,f9);

  QPX_STORE(fpp2_1,inc,f4);
  QPX_LOAD(fpp1_2,inc,f8);
  QPX_STORE(fpp2_2,inc,f10);

  QPX_STORE(fpp2_1,inc,f5);
  QPX_LOAD(fpp1_2,inc,f9);
  QPX_STORE(fpp2_2,inc,f11);

  QPX_STORE(fpp2_1,inc,f6);
  QPX_LOAD(fpp1_2,inc,f10);
  QPX_STORE(fpp2_2,inc,f8);

  QPX_STORE(fpp2_1,inc,f7);
  QPX_LOAD(fpp1_2,inc,f11);
  QPX_STORE(fpp2_2,inc,f9);

  QPX_STORE(fpp2_2,inc,f10);
  QPX_STORE(fpp2_2,inc,f11);

  src += nb;
  dest += nb;
  num -= nb;
  
 short_msg:
  if (num >= 512) {
    quad_copy_512(dest, src);
    src  += 512;
    dest += 512;
    num  -= 512;
  }

  nb += quad_copy_128n (dest, src, num);
  return nb;
}
