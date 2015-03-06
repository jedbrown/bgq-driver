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
 * \file components/devices/shmem/qpx_8way_sum.h
 * \brief ???
 */
#ifndef __components_devices_shmem_qpx_8way_sum_h__
#define __components_devices_shmem_qpx_8way_sum_h__

#include "asmheader.h"

//atleast 128B(16 doubles) ..in multiples of 64B
//alignment expected is L1 cache line == 64B
void _pami_core_double_sum_8way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls);


inline unsigned _quad_double_sum_8way_align64B(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls)
{
  register double f0  FP_REGISTER(0) = 0.0;
  register double f1  FP_REGISTER(1) = 0.0;
  register double f2  FP_REGISTER(2) = 0.0;
  register double f3  FP_REGISTER(3) = 0.0;
  register double f4  FP_REGISTER(4) = 0.0;
  register double f5  FP_REGISTER(5) = 0.0;
  register double f6  FP_REGISTER(6) = 0.0;
  register double f7  FP_REGISTER(7) = 0.0;
  register double f8  FP_REGISTER(8) = 0.0;
  register double f9  FP_REGISTER(9) = 0.0;
  register double f10 FP_REGISTER(10) = 0.0;
  register double f11 FP_REGISTER(11) = 0.0;
  register double f12 FP_REGISTER(12) = 0.0;
  register double f13 FP_REGISTER(13) = 0.0;
  register double f14 FP_REGISTER(14) = 0.0;
  register double f15 FP_REGISTER(15) = 0.0;
  register double f29 FP_REGISTER(29) = 0.0;
  register double f31 FP_REGISTER(31) = 0.0;

  double *src0_1, *src1_1, *src2_1, *src3_1, *src4_1, *src5_1, *src6_1, *src7_1;
  double *src0_2, *src1_2, *src2_2, *src3_2, *src4_2, *src5_2, *src6_2, *src7_2;
  double *dst_1, *dst_2;
  register int inc ;
  register int	y;

  y = num_dbls>>3;

  src0_1 = src0 -8;
  src0_2 = src0 -4;

  src1_1 = src1 -8;
  src1_2 = src1 -4;

  src2_1 = src2 -8;
  src2_2 = src2 -4;

  src3_1 = src3 -8;
  src3_2 = src3 -4;

  src4_1 = src4 -8;
  src4_2 = src4 -4;

  src5_1 = src5 -8;
  src5_2 = src5 -4;

  src6_1 = src6 -8;
  src6_2 = src6 -4;

  src7_1 = src7 -8;
  src7_2 = src7 -4;

  dst_1 = dst -8;
  dst_2 = dst -4;

  inc=64;

  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src4_1,inc,f4);
  QPX_LOAD(src5_1,inc,f5);
  QPX_LOAD(src6_1,inc,f6);
  QPX_LOAD(src7_1,inc,f7);

  --y;

  ASM("qvfmr 28, 0" ::: "fr28"  );
  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src0_2,inc,f8);

  ASM("qvfmr 29, 1" ::: "fr29"  );
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src1_2,inc,f9);

  ASM("qvfadd 28, 28, 2" ::: "fr28"  );
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src2_2,inc,f10);
  ASM("qvfmr 30, 8" ::: "fr30"  );

  ASM("qvfadd 29, 29, 3" ::: "fr29"  );
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src3_2,inc,f11);
  ASM("qvfmr 31, 9" ::: "fr31"  );

  while (1){

  ASM("qvfadd 28, 28, 4" ::: "fr28"  );
  QPX_LOAD(src4_1,inc,f4);
  QPX_LOAD(src4_2,inc,f12);
  ASM("qvfadd 30, 30, 10" ::: "fr30"  );

  ASM("qvfadd 29, 29, 5" ::: "fr29"  );
  QPX_LOAD(src5_1,inc,f5);
  QPX_LOAD(src5_2,inc,f13);
  ASM("qvfadd 31, 31, 11" ::: "fr31"  );

  ASM("qvfadd 28, 28, 6" ::: "fr28"  );
  QPX_LOAD(src6_1,inc,f6);
  QPX_LOAD(src6_2,inc,f14);
  ASM("qvfadd 30, 30, 12" ::: "fr30"  );

  ASM("qvfadd 29, 29, 7" ::: "fr29"  );
  QPX_LOAD(src7_1,inc,f7);
  QPX_LOAD(src7_2,inc,f15);
  ASM("qvfadd 31, 31, 13" ::: "fr31"  );

  if (!--y) break;

  ASM("qvfadd 29, 29, 28" ::: "fr29"  );

  ASM("qvfmr 28, 0" ::: "fr28"  );
  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src0_2,inc,f8);
  ASM("qvfadd 30, 30, 14" ::: "fr30"  );

  QPX_STORE(dst_1,inc,f29);

  ASM("qvfmr 29, 1" ::: "fr29"  );
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src1_2,inc,f9);
  ASM("qvfadd 31, 31, 15" ::: "fr31"  );

  ASM("qvfadd 28, 28, 2" ::: "fr28"  );
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src2_2,inc,f10);
  ASM("qvfadd 31, 31, 30" ::: "fr31"  );
  ASM("qvfmr 30, 8" ::: "fr30"  );

  ASM("qvfadd 29, 29, 3" ::: "fr29"  );
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src3_2,inc,f11);
  QPX_STORE(dst_2,inc,f31);
  ASM("qvfmr 31, 9" ::: "fr31"  );

  }

  ASM("qvfadd 29, 29, 28" ::: "fr29"  );

  ASM("qvfmr 28, 0" ::: "fr28"  );
  QPX_LOAD(src0_2,inc,f8);
  ASM("qvfadd 30, 30, 14" ::: "fr30"  );

  QPX_STORE(dst_1,inc,f29);

  ASM("qvfmr 29, 1" ::: "fr29"  );
  QPX_LOAD(src1_2,inc,f9);
  ASM("qvfadd 31, 31, 15" ::: "fr31"  );

  ASM("qvfadd 28, 28, 2" ::: "fr28"  );
  QPX_LOAD(src2_2,inc,f10);
  ASM("qvfadd 31, 31, 30" ::: "fr31"  );
  ASM("qvfmr 30, 8" ::: "fr30"  );

  ASM("qvfadd 29, 29, 3" ::: "fr29"  );
  QPX_LOAD(src3_2,inc,f11);
  QPX_STORE(dst_2,inc,f31);
  ASM("qvfmr 31, 9" ::: "fr31"  );

  ASM("qvfadd 28, 28, 4" ::: "fr28"  );
  QPX_LOAD(src4_2,inc,f12);
  ASM("qvfadd 30, 30, 10" ::: "fr30"  );

  ASM("qvfadd 29, 29, 5" ::: "fr29"  );
  QPX_LOAD(src5_2,inc,f13);
  ASM("qvfadd 31, 31, 11" ::: "fr31"  );

  ASM("qvfadd 28, 28, 6" ::: "fr28"  );
  QPX_LOAD(src6_2,inc,f14);
  ASM("qvfadd 30, 30, 12" ::: "fr30"  );

  ASM("qvfadd 29, 29, 7" ::: "fr29"  );
  QPX_LOAD(src7_2,inc,f15);
  ASM("qvfadd 31, 31, 13" ::: "fr31"  );

  ASM("qvfadd 29, 29, 28" ::: "fr29"  );
  ASM("qvfadd 30, 30, 14" ::: "fr30"  );
  ASM("qvfadd 31, 31, 15" ::: "fr31"  );
  ASM("qvfadd 31, 31, 30" ::: "fr31"  );

  QPX_STORE(dst_1,inc,f29);
  QPX_STORE(dst_2,inc,f31);
  
  return ((num_dbls>>3) << 3);
}

//alteast 32B (4 Dbls) expected
inline unsigned _quad_double_sum_8way_align32B_short(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls)
{
  register double f0  FP_REGISTER(0) = 0.0;
  register double f1  FP_REGISTER(1) = 0.0;
  register double f2  FP_REGISTER(2) = 0.0;
  register double f3  FP_REGISTER(3) = 0.0;
  register double f4  FP_REGISTER(4) = 0.0;
  register double f5  FP_REGISTER(5) = 0.0;
  register double f6  FP_REGISTER(6) = 0.0;
  register double f7  FP_REGISTER(7) = 0.0;
  register double f29 FP_REGISTER(29) = 0.0;

  double *src0_1, *src1_1, *src2_1, *src3_1, *src4_1, *src5_1, *src6_1, *src7_1;
  double *dst_1;
  register int inc ;
  register int	y;

  src0_1 = src0 -4;
  src1_1 = src1 -4;
  src2_1 = src2 -4;
  src3_1 = src3 -4;
  src4_1 = src4 -4;
  src5_1 = src5 -4;
  src6_1 = src6 -4;
  src7_1 = src7 -4;

  dst_1 = dst -4;

  inc=32;

  y= num_dbls>>2;

  if (y == 0) return 0;

  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src4_1,inc,f4);
  QPX_LOAD(src5_1,inc,f5);
  QPX_LOAD(src6_1,inc,f6);
  QPX_LOAD(src7_1,inc,f7);

  if (y == 1)
  {
    ASM("qvfmr 28, 0" ::: "fr28"  );
    ASM("qvfmr 29, 1" ::: "fr29"  );
    ASM("qvfadd 28, 28, 2" ::: "fr28"  );
    ASM("qvfadd 29, 29, 3" ::: "fr29"  );
    ASM("qvfadd 28, 28, 4" ::: "fr28"  );
    ASM("qvfadd 29, 29, 5" ::: "fr29"  );
    ASM("qvfadd 28, 28, 6" ::: "fr28"  );
    ASM("qvfadd 29, 29, 7" ::: "fr29"  );
    ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    QPX_STORE(dst_1,inc,f29);
    return 4;
  }

  --y;

  ASM("qvfmr 28, 0" ::: "fr28"  );
  QPX_LOAD(src0_1,inc,f0);

  ASM("qvfmr 29, 1" ::: "fr29"  );
  QPX_LOAD(src1_1,inc,f1);

  while (1){
    ASM("qvfadd 28, 28, 2" ::: "fr28"  );
    QPX_LOAD(src2_1,inc,f2);

    ASM("qvfadd 29, 29, 3" ::: "fr29"  );
    QPX_LOAD(src3_1,inc,f3);

    ASM("qvfadd 28, 28, 4" ::: "fr28"  );
    QPX_LOAD(src4_1,inc,f4);

    ASM("qvfadd 29, 29, 5" ::: "fr29"  );
    QPX_LOAD(src5_1,inc,f5);

    ASM("qvfadd 28, 28, 6" ::: "fr28"  );
    QPX_LOAD(src6_1,inc,f6);

    ASM("qvfadd 29, 29, 7" ::: "fr29"  );
    QPX_LOAD(src7_1,inc,f7);

    if (!--y) break;

    ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    ASM("qvfmr 28, 0" ::: "fr28"  );
    QPX_LOAD(src0_1,inc,f0);

    QPX_STORE(dst_1,inc,f29);
    ASM("qvfmr 29, 1" ::: "fr29"  );
    QPX_LOAD(src1_1,inc,f1);
  } 

    ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    ASM("qvfmr 28, 0" ::: "fr28"  );
    QPX_STORE(dst_1,inc,f29);
    ASM("qvfmr 29, 1" ::: "fr29"  );
    ASM("qvfadd 28, 28, 2" ::: "fr28"  );
    ASM("qvfadd 29, 29, 3" ::: "fr29"  );
    ASM("qvfadd 28, 28, 4" ::: "fr28"  );
    ASM("qvfadd 29, 29, 5" ::: "fr29"  );
    ASM("qvfadd 28, 28, 6" ::: "fr28"  );
    ASM("qvfadd 29, 29, 7" ::: "fr29"  );
    ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    QPX_STORE(dst_1,inc,f29);

  return ((num_dbls >> 2) << 2);
}

inline unsigned _quad_double_sum_8way_single(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls)
{

  unsigned i;
  for (i = 0; i < num_dbls; i++)
    dst[i] = src0[i] + src1[i] + src2[i] + src3[i] + src4[i] + src5[i] + src6[i] + src7[i];

  return num_dbls;

}

//atleast 20 Dbls exptected..(20-4) == 16 for 64B aligned sum
inline unsigned _quad_double_sum_8way_align32B(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls)
{

  unsigned  dbls = 0;
  uint64_t  mask;
  mask = (uint64_t)dst|(uint64_t)src0|(uint64_t)src1|(uint64_t)src2|(uint64_t)src3|  
         (uint64_t)src4|(uint64_t)src5|(uint64_t)src6|(uint64_t)src7;  
  bool is_64B_aligned;
  is_64B_aligned = ((mask & ((uint64_t)63)) == 0);
  
  if (is_64B_aligned)
  {
    dbls = _quad_double_sum_8way_align64B(dst, src0, src1, src2, src3, src4, src5, src6, src7, num_dbls);
    if (dbls == num_dbls) return dbls;
    _quad_double_sum_8way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, src3+dbls, src4+dbls, 
                                  src5+dbls, src6+dbls, src7+dbls, num_dbls - dbls);
    return num_dbls;
  }

  
  /*_quad_double_sum_8way_single(dst, src0, src1, src2, src3, src4, src5, src6, src7, 4);

  dbls = _quad_double_sum_8way_align64B(dst+4, src0+4, src1+4, src2+4, src3+4, src4+4, src5+4, src6+4, src7+4, num_dbls - 4);
  dbls+=4;*/
   dbls = _quad_double_sum_8way_align32B_short(dst, src0, src1, src2, src3, src4, src5, src6, src7, num_dbls);

  return dbls;

}

//buffers aligned to 32B 
inline unsigned quad_double_sum_8way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls)
{
 
  const uint64_t      alignment = 32;
  uint64_t    mask    = 0;
  mask = (alignment - 1);

  if (mask & ((uint64_t)src0|(uint64_t)src1|(uint64_t)src2|(uint64_t)src3|(uint64_t)src4|(uint64_t)src5|(uint64_t)src6|(uint64_t)src7|(uint64_t)dst))
  {
    _pami_core_double_sum_8way(dst, src0, src1, src2, src3, src4, src5, src6, src7, num_dbls);
    return 0;
  }

  unsigned  dbls = 0;

  if (num_dbls < 20)
   dbls = _quad_double_sum_8way_align32B_short(dst, src0, src1, src2, src3, src4, src5, src6, src7, num_dbls);
  else
   dbls = _quad_double_sum_8way_align32B(dst, src0, src1, src2, src3, src4, src5, src6, src7, num_dbls);

  if (num_dbls - dbls)
   _quad_double_sum_8way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, src3+dbls, src4+dbls, 
                                  src5+dbls, src6+dbls, src7+dbls, num_dbls - dbls);

  return num_dbls;
}



#endif
