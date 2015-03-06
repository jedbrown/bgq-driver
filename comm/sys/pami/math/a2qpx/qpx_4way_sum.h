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
#ifndef __components_devices_shmem_qpx_4way_sum_h__
#define __components_devices_shmem_qpx_4way_sum_h__

#include "asmheader.h"

//atleast 128B(16 doubles) ..in multiples of 64B
//alignment expected is L1 cache line == 64B
void _pami_core_double_sum_4way(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls);


inline unsigned _quad_double_sum_4way_align64B(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls)
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
  register double f18 FP_REGISTER(18) = 0.0;
  register double f21 FP_REGISTER(21) = 0.0;
  register double f24 FP_REGISTER(24) = 0.0;
  register double f27 FP_REGISTER(27) = 0.0;

  double *src0_1, *src0_2, *src1_1, *src1_2, *src2_1, *src2_2, *src3_1, *src3_2;
  double *dst_1, *dst_2;

  register int inc;
  register int	y;

  src0_1 = src0 -8;  //offset by stride=0 bytes
  src0_2 = src0 -4;

  src1_1 = src1 -8;  //offset by stride=0 bytes
  src1_2 = src1 -4;

  src2_1 = src2 -8;  //offset by stride=0 bytes
  src2_2 = src2 -4;

  src3_1 = src3 -8;  //offset by stride=0 bytes
  src3_2 = src3 -4;

  dst_1 = dst -8;  //offset by stride=0 bytes
  dst_2 = dst -4;

  inc=64;

  y = (num_dbls >> 3) >> 1;
  //y=(num-128)/128;

  // may speed up the 2nd to 7th load by using non-update versions of load, tbd later...
  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src0_1,inc,f4);
  QPX_LOAD(src1_1,inc,f5);
  QPX_LOAD(src2_1,inc,f6);
  QPX_LOAD(src3_1,inc,f7);

  (--y);
  
  ASM("qvfadd 16, 0, 1" ::: "fr16"  );
  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src0_2,inc,f8);
  QPX_LOAD(src1_2,inc,f9);

  ASM("qvfadd 17, 2, 3" ::: "fr17"  );
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src2_2,inc,f10);
  QPX_LOAD(src3_2,inc,f11);
  ASM("qvfadd 22, 8, 9" ::: "fr22"  );

  do{
    ASM("qvfadd 18, 16, 17" ::: "fr18"  );


    ASM("qvfadd 19, 4, 5" ::: "fr19"  );
    QPX_LOAD(src0_1,inc,f4);
    QPX_LOAD(src1_1,inc,f5);
    QPX_LOAD(src0_2,inc,f12);
    QPX_LOAD(src1_2,inc,f13);

    ASM("qvfadd 23, 10, 11" ::: "fr23"  );
    QPX_STORE(dst_1,inc,f18);

    ASM("qvfadd 20, 6, 7" ::: "fr20"  );
    QPX_LOAD(src2_1,inc,f6);
    QPX_LOAD(src3_1,inc,f7);
    QPX_LOAD(src2_2,inc,f14);
    QPX_LOAD(src3_2,inc,f15);

    ASM("qvfadd 25, 12, 13" ::: "fr25"  );

    ASM("qvfadd 24, 22, 23" ::: "fr24"  );

    ASM("qvfadd 21, 19, 20" ::: "fr21"  );

    if(!--y) break;

    ASM("qvfadd 16, 0, 1" ::: "fr16"  );
    QPX_LOAD(src0_1,inc,f0);
    QPX_LOAD(src1_1,inc,f1);
    ASM("qvfadd 26, 14, 15" ::: "fr26"  );
    QPX_LOAD(src0_2,inc,f8);
    QPX_LOAD(src1_2,inc,f9);

    QPX_STORE(dst_2,inc,f24);

    QPX_STORE(dst_1,inc,f21);

    ASM("qvfadd 17, 2, 3" ::: "fr17"  );
    ASM("qvfadd 27, 25, 26" ::: "fr27"  );
    QPX_LOAD(src2_1,inc,f2);
    QPX_LOAD(src3_1,inc,f3);
    QPX_LOAD(src2_2,inc,f10);
    QPX_LOAD(src3_2,inc,f11);
    ASM("qvfadd 22, 8, 9" ::: "fr22"  );
    QPX_STORE(dst_2,inc,f27);

  }while(1);
  QPX_STORE(dst_2,inc,f24);

  ASM("qvfadd 26, 14, 15" ::: "fr26"  );

  ASM("qvfadd 16, 0, 1" ::: "fr16"  );
  ASM("qvfadd 17, 2, 3" ::: "fr17"  );
  QPX_LOAD(src0_2,inc,f8);
  QPX_LOAD(src1_2,inc,f9);
  ASM("qvfadd 22, 8, 9" ::: "fr22"  );

  QPX_STORE(dst_1,inc,f21);
  ASM("qvfadd 27, 25, 26" ::: "fr27"  );

  QPX_LOAD(src2_2,inc,f10);
  QPX_LOAD(src3_2,inc,f11);
  ASM("qvfadd 23, 10, 11" ::: "fr23"  );

  ASM("qvfadd 18, 16, 17" ::: "fr18"  );
  ASM("qvfadd 24, 22, 23" ::: "fr24"  );
  QPX_STORE(dst_2,inc,f27);

  ASM("qvfadd 19, 4, 5" ::: "fr19"  );
  QPX_LOAD(src0_2,inc,f12);
  QPX_LOAD(src1_2,inc,f13);

  QPX_STORE(dst_1,inc,f18);
  QPX_STORE(dst_2,inc,f24);

  ASM("qvfadd 20, 6, 7" ::: "fr20"  );
  QPX_LOAD(src2_2,inc,f14);
  QPX_LOAD(src3_2,inc,f15);
  ASM("qvfadd 25, 12, 13" ::: "fr25"  );
  ASM("qvfadd 26, 14, 15" ::: "fr26"  );


  ASM("qvfadd 21, 19, 20" ::: "fr21"  );
  ASM("qvfadd 27, 25, 26" ::: "fr27"  );

  QPX_STORE(dst_1,inc,f21);
  QPX_STORE(dst_2,inc,f27);

  return ((num_dbls >> (3+1)) << (3+1));
}

//alteast 32B (4 Dbls) expected
inline unsigned _quad_double_sum_4way_align32B_short(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls)
{
  register double f0  FP_REGISTER(0) = 0.0;
  register double f1  FP_REGISTER(1) = 0.0;
  register double f2  FP_REGISTER(2) = 0.0;
  register double f3  FP_REGISTER(3) = 0.0;
  register double f29 FP_REGISTER(29) = 0.0;

  double *src0_1, *src1_1, *src2_1, *src3_1;
  double *dst_1;
  register int inc ;
  register int	y;

  src0_1 = src0 -4;
  src1_1 = src1 -4;
  src2_1 = src2 -4;
  src3_1 = src3 -4;

  dst_1 = dst -4;

  inc=32;

  y= num_dbls>>2;

  if (y == 0) return 0;

  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src3_1,inc,f3);

  if (y == 1)
  {
    ASM("qvfmr 28, 0" ::: "fr28"  );
    ASM("qvfmr 29, 1" ::: "fr29"  );
    ASM("qvfadd 28, 28, 2" ::: "fr28"  );
    ASM("qvfadd 29, 29, 3" ::: "fr29"  );
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
    ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    QPX_STORE(dst_1,inc,f29);

  return ((num_dbls >> 2) << 2);
}

inline unsigned _quad_double_sum_4way_single(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls)
{

  unsigned i;
  for (i = 0; i < num_dbls; i++)  
    dst[i] = src0[i] + src1[i] + src2[i] + src3[i];

  return num_dbls;

}

//atleast 20 Dbls exptected..(20-4) == 16 for 64B aligned sum
inline unsigned _quad_double_sum_4way_align32B(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls)
{

  unsigned  dbls = 0;
  uint64_t  mask;
  mask = (uint64_t)dst|(uint64_t)src0|(uint64_t)src1|(uint64_t)src2|(uint64_t)src3 ; 

  bool is_64B_aligned;
  is_64B_aligned = ((mask & ((uint64_t)63)) == 0);
  
  if (is_64B_aligned)
  {
    dbls = _quad_double_sum_4way_align64B(dst, src0, src1, src2, src3, num_dbls);
    if (dbls == num_dbls) return dbls;
    _quad_double_sum_4way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, src3+dbls, num_dbls - dbls);
    return num_dbls;
  }

  /*_quad_double_sum_4way_single(dst, src0, src1, src2, src3, 4);

  dbls = _quad_double_sum_4way_align64B(dst+4, src0+4, src1+4, src2+4, src3+4, num_dbls - 4);
  dbls+=4;*/
   dbls = _quad_double_sum_4way_align32B_short(dst, src0, src1, src2, src3, num_dbls);

  return dbls;

}

//buffers aligned to 32B 
inline unsigned quad_double_sum_4way(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls)
{
  const uint64_t      alignment = 32;
  uint64_t    mask    = 0;
  mask = (alignment - 1);


  if (mask & ((uint64_t)src0|(uint64_t)src1|(uint64_t)src2|(uint64_t)src3|(uint64_t)dst))
  {
    _pami_core_double_sum_4way(dst, src0, src1, src2, src3, num_dbls);
    return 0;
  }
  

  unsigned  dbls = 0;

  if (num_dbls < 36)
   dbls = _quad_double_sum_4way_align32B_short(dst, src0, src1, src2, src3, num_dbls);
  else
   dbls = _quad_double_sum_4way_align32B(dst, src0, src1, src2, src3,  num_dbls);

  if (num_dbls - dbls)
   _quad_double_sum_4way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, src3+dbls, num_dbls - dbls);

  return num_dbls;
}



#endif
