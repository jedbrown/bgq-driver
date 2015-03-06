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
 * \file components/devices/shmem/qpx_4way_sum.h
 * \brief ???
 */
#ifndef __components_devices_shmem_qpx_3way_sum_h__
#define __components_devices_shmem_qpx_3way_sum_h__

#include "asmheader.h"

void _pami_core_double_sum_3way(double* dst, double* src0, double *src1, double* src2, unsigned num_dbls);

inline unsigned _quad_double_sum_3way(double* dst, double* src0, double *src1, double* src2, unsigned num_dbls)
{
  register double f0  FP_REGISTER(0) = 0.0;
  register double f1  FP_REGISTER(1) = 0.0;
  register double f2  FP_REGISTER(2) = 0.0;
  register double f4  FP_REGISTER(4) = 0.0;
  register double f5  FP_REGISTER(5) = 0.0;
  register double f6  FP_REGISTER(6) = 0.0;
  register double f8  FP_REGISTER(8) = 0.0;
  register double f9  FP_REGISTER(9) = 0.0;
  register double f10 FP_REGISTER(10) = 0.0;
  register double f12 FP_REGISTER(12) = 0.0;
  register double f13 FP_REGISTER(13) = 0.0;
  register double f14 FP_REGISTER(14) = 0.0;

  register double f3  FP_REGISTER(3) = 0.0;
  register double f7  FP_REGISTER(7) = 0.0;
  register double f11 FP_REGISTER(11) = 0.0;
  register double f15 FP_REGISTER(15) = 0.0;

  double *src0_1, *src0_2, *src1_1, *src1_2, *src2_1, *src2_2;
  double *dst_1, *dst_2;

  register int inc;
  register int	y;

  src0_1 = src0 -8;  //offset by stride=0 bytes
  src0_2 = src0 -4;

  src1_1 = src1 -8;  //offset by stride=0 bytes
  src1_2 = src1 -4;

  src2_1 = src2 -8;  //offset by stride=0 bytes
  src2_2 = src2 -4;

  dst_1 = dst -8;  //offset by stride=0 bytes
  dst_2 = dst -4;

  inc=64;

  y = (num_dbls >> 3) >> 1;
  //y=(num-128)/128;

  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src0_1,inc,f4);
  QPX_LOAD(src1_1,inc,f5);
  QPX_LOAD(src2_1,inc,f6);

  (--y);
  
  ASM("qvfadd 3, 0, 1" ::: "fr3"  );
  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src0_2,inc,f8);
  QPX_LOAD(src1_2,inc,f9);

  ASM("qvfadd 3, 3, 2" ::: "fr3"  );
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src2_2,inc,f10);
  ASM("qvfadd 11, 8, 9" ::: "fr11"  );

  do{
    ASM("qvfadd 7, 4, 5"  ::: "fr7"  );
    QPX_LOAD(src0_1,inc,f4);
    QPX_LOAD(src1_1,inc,f5);
    QPX_LOAD(src0_2,inc,f12);
    QPX_LOAD(src1_2,inc,f13);

    QPX_STORE(dst_1,inc,f3);

    ASM("qvfadd 7, 7, 6" ::: "fr7"  );
    QPX_LOAD(src2_1,inc,f6);
    QPX_LOAD(src2_2,inc,f14);

    ASM("qvfadd 15, 12, 13" ::: "fr15"  );
    ASM("qvfadd 11, 11, 10" ::: "fr11"  );

    if(!--y) break;

    ASM("qvfadd 3, 0, 1" ::: "fr3"  );
    QPX_LOAD(src0_1,inc,f0);
    QPX_LOAD(src1_1,inc,f1);
    QPX_LOAD(src0_2,inc,f8);
    QPX_LOAD(src1_2,inc,f9);

    QPX_STORE(dst_2,inc,f11);
    QPX_STORE(dst_1,inc,f7);

    ASM("qvfadd 15, 15, 14" ::: "fr15"  );
    ASM("qvfadd 3, 3, 2" ::: "fr3"  );
    QPX_LOAD(src2_1,inc,f2);
    QPX_LOAD(src2_2,inc,f10);
    ASM("qvfadd 11, 8, 9" ::: "fr11"  );
    QPX_STORE(dst_2,inc,f15);

  }while(1);

  QPX_STORE(dst_2,inc,f11);
  ASM("qvfadd 3, 0, 1" ::: "fr3"  );
  ASM("qvfadd 3, 3, 2" ::: "fr3"  );

  QPX_LOAD(src0_2,inc,f8);
  QPX_LOAD(src1_2,inc,f9);
  ASM("qvfadd 11, 8, 9" ::: "fr11"  );

  QPX_STORE(dst_1,inc,f7);
  ASM("qvfadd 15, 15, 14" ::: "fr15"  );

  QPX_LOAD(src2_2,inc,f10);

  ASM("qvfadd 11, 11, 10" ::: "fr11"  );
  QPX_STORE(dst_2,inc,f15);

  ASM("qvfadd 7, 4, 5" ::: "fr7"  );
  QPX_LOAD(src0_2,inc,f12);
  QPX_LOAD(src1_2,inc,f13);

  QPX_STORE(dst_1,inc,f3);
  QPX_STORE(dst_2,inc,f11);

  QPX_LOAD(src2_2,inc,f14);
  ASM("qvfadd 15, 12, 13" ::: "fr15"  );

  ASM("qvfadd 7, 7, 6" ::: "fr7"  );
  ASM("qvfadd 15, 15, 14" ::: "fr15"  );

  QPX_STORE(dst_1,inc,f7);
  QPX_STORE(dst_2,inc,f15);

  return ((num_dbls >> (3+1)) << (3+1));
}

//alteast 32B (4 Dbls) expected
inline unsigned _quad_double_sum_3way_short(double* dst, double* src0, double *src1, double *src2, unsigned num_dbls)
{
  register double f0  FP_REGISTER(0) = 0.0;
  register double f1  FP_REGISTER(1) = 0.0;
  register double f2  FP_REGISTER(2) = 0.0;
  register double f3  FP_REGISTER(3) = 0.0;

  double *src0_1, *src1_1, *src2_1;
  double *dst_1;
  register int inc ;
  register int	y;

  src0_1 = src0 -4;
  src1_1 = src1 -4;
  src2_1 = src2 -4;
  dst_1 = dst -4;

  inc=32;
  y= num_dbls>>2;

  if (y == 0) return 0;

  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);

  if (y == 1)
  {
    ASM("qvfadd 3, 0, 1" ::: "fr3"  );    
    ASM("qvfadd 3, 3, 2" ::: "fr3"  );    
    QPX_STORE(dst_1,inc,f3);
    return 4;
  }

  while (--y) {
    ASM("qvfadd 3, 0, 1" ::: "fr3"  );    
    ASM("qvfadd 3, 3, 2" ::: "fr3"  );    
    QPX_LOAD(src0_1,inc,f0);
    QPX_LOAD(src1_1,inc,f1);
    QPX_LOAD(src2_1,inc,f2);
    QPX_STORE(dst_1,inc,f3);
  } 

  ASM("qvfadd 3, 0, 1" ::: "fr3"  );    
  ASM("qvfadd 3, 3, 2" ::: "fr3"  );    
  QPX_STORE(dst_1,inc,f3);
  
  return ((num_dbls >> 2) << 2);
}




inline unsigned _quad_double_sum_3way_single(double* dst, double* src0, double *src1, double* src2, unsigned num_dbls)
{

  unsigned i;
  for (i = 0; i < num_dbls; i++)  
    dst[i] = src0[i] + src1[i] + src2[i];

  return num_dbls;
}

//buffers aligned to 32B 
inline unsigned quad_double_sum_3way(double* dst, double* src0, double *src1, double* src2, unsigned num_dbls)
{
  const uint64_t      alignment = 32;
  uint64_t    mask    = 0;
  mask = (alignment - 1);

  if (mask & ((uint64_t)src0|(uint64_t)src1|(uint64_t)src2|(uint64_t)dst))
  {
    _pami_core_double_sum_3way(dst, src0, src1, src2, num_dbls);
    return num_dbls;
  }
  
  unsigned  dbls = 0;
  if (num_dbls >= 36)
    dbls = _quad_double_sum_3way(dst, src0, src1, src2, num_dbls);
  
  if (num_dbls - dbls)
    dbls +=_quad_double_sum_3way_short(dst+dbls, src0+dbls, src1+dbls, src2+dbls, num_dbls - dbls);
  
  if (num_dbls - dbls)
    _quad_double_sum_3way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, num_dbls - dbls);

  return num_dbls;
}

#endif
