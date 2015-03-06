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
 * \file components/devices/shmem/qpx_16way_min.h
 * \brief ???
 */
#ifndef __components_devices_shmem_qpx_16way_min_h__
#define __components_devices_shmem_qpx_16way_min_h__

#include "asmheader.h"

//atleast 128B(16 doubles) ..in multiples of 64B
//alignment expected is L1 cache line == 64B
//
void _pami_core_double_min_16way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls);

inline unsigned _quad_double_min_16way_align64B(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls)
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
  register double f16 FP_REGISTER(16) = 0.0;
  register double f17 FP_REGISTER(17) = 0.0;
  register double f18 FP_REGISTER(18) = 0.0;
  register double f19 FP_REGISTER(19) = 0.0;
  register double f28 FP_REGISTER(28) = 0.0;
  register double f29 FP_REGISTER(29) = 0.0;
  register double f30 FP_REGISTER(30) = 0.0;
  register double f31 FP_REGISTER(31) = 0.0;

  register double *src0_1, *src1_1, *src2_1, *src3_1, *src4_1, *src5_1, *src6_1, *src7_1;
  register double *src8_1, *src9_1, *src10_1, *src11_1, *src12_1, *src13_1, *src14_1, *src15_1;
 register  double *dst_1, *dst_2;
  //register int inc __MYASM__("r7");
  //register int inc __MYASM__("r7");
  register int inc ;
  register int inc1;
  register int	y;

  src0_1 = src0 -4;
  src1_1 = src1 -4;
  src2_1 = src2 -4;
  src3_1 = src3 -4;
  src4_1 = src4 -4;
  src5_1 = src5 -4;
  src6_1 = src6 -4;
  src7_1 = src7 -4;
  src8_1 = src8 -4;
  src9_1 = src9 -4;
  src10_1 = src10 -4;
  src11_1 = src11 -4;
  src12_1 = src12 -4;
  src13_1 = src13 -4;
  src14_1 = src14 -4;
  src15_1 = src15 -4;

  dst_1 = dst -8;
  dst_2 = dst -4;

  inc=32;
  inc1=64;

  y = (num_dbls>>3) >> 1;

  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src4_1,inc,f4);
  QPX_LOAD(src5_1,inc,f5);
  QPX_LOAD(src6_1,inc,f6);
  QPX_LOAD(src7_1,inc,f7);

  QPX_LOAD(src8_1,inc,f8);
  QPX_LOAD(src0_1,inc,f16);
  ASM("qvfadd 28, 0, 1" ::: "fr28"  );


  QPX_LOAD(src9_1,inc,f9);
  QPX_LOAD(src1_1,inc,f17);
  ASM("qvfadd 28, 28, 2" ::: "fr28"  );


  QPX_LOAD(src10_1,inc,f10);
  QPX_LOAD(src2_1,inc,f18);
  ASM("qvfadd 28, 28, 3" ::: "fr28"  );


  QPX_LOAD(src11_1,inc,f11);
  QPX_LOAD(src3_1,inc,f19);
  ASM("qvfadd 28, 28, 4" ::: "fr28"  );
  QPX_LOAD(src12_1,inc,f12);

  ASM("qvfadd 30, 16, 17" ::: "fr30"  );


  QPX_LOAD(src4_1,inc,f16);
  ASM("qvfadd 28, 28, 5" ::: "fr28"  );
  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 30, 30, 18" ::: "fr30"  );


  QPX_LOAD(src5_1,inc,f17);
  ASM("qvfadd 28, 28, 6" ::: "fr28"  );
  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 30, 30, 19" ::: "fr30"  );


  QPX_LOAD(src6_1,inc,f18);
  ASM("qvfadd 28, 28, 7" ::: "fr28"  );
  QPX_LOAD(src15_1,inc,f15);
  ASM("qvfadd 30, 30, 16" ::: "fr30"  );


  QPX_LOAD(src7_1,inc,f19);
  ASM("qvfadd 28, 28, 8" ::: "fr28"  );

  while (1)
  {

  QPX_LOAD(src0_1,inc,f0);
  ASM("qvfadd 30, 30, 17" ::: "fr30"  );
  QPX_LOAD(src8_1,inc,f16);
  ASM("qvfadd 28, 28, 9" ::: "fr28"  );



  QPX_LOAD(src1_1,inc,f1);
  ASM("qvfadd 30, 30, 18" ::: "fr30"  );
  QPX_LOAD(src9_1,inc,f17);
  ASM("qvfadd 28, 28, 10" ::: "fr28"  );

  QPX_LOAD(src2_1,inc,f2);
  ASM("qvfadd 30, 30, 19" ::: "fr30"  );
  QPX_LOAD(src10_1,inc,f18);
  ASM("qvfadd 28, 28, 11" ::: "fr28"  );



  QPX_LOAD(src3_1,inc,f3);
  ASM("qvfadd 30, 30, 16" ::: "fr30"  );
  QPX_LOAD(src11_1,inc,f19);
  ASM("qvfadd 28, 28, 12" ::: "fr28"  );


  QPX_LOAD(src4_1,inc,f4);
  ASM("qvfadd 30, 30, 17" ::: "fr30"  );
  QPX_LOAD(src12_1,inc,f16);
  ASM("qvfadd 28, 28, 13" ::: "fr28"  );


  QPX_LOAD(src5_1,inc,f5);
  ASM("qvfadd 30, 30, 18" ::: "fr30"  );
  QPX_LOAD(src13_1,inc,f17);
  ASM("qvfadd 28, 28, 14" ::: "fr28"  );


  QPX_LOAD(src6_1,inc,f6);
  ASM("qvfadd 30, 30, 19" ::: "fr30"  );
  QPX_LOAD(src14_1,inc,f18);
  ASM("qvfadd 28, 28, 15" ::: "fr28"  ); //first first-half min done


  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfadd 30, 30, 16" ::: "fr30"  );
  QPX_LOAD(src15_1,inc,f19);


  QPX_LOAD(src8_1,inc,f8);
  ASM("qvfadd 30, 30, 17" ::: "fr30"  );
  QPX_LOAD(src0_1,inc,f16);
  ASM("qvfadd 29, 0, 1" ::: "fr29"  ); 	//second first-half min start



  QPX_LOAD(src9_1,inc,f9);
  ASM("qvfadd 30, 30, 18" ::: "fr30"  );
  QPX_STORE(dst_1,inc1,f28);
  QPX_LOAD(src1_1,inc,f17);
  ASM("qvfadd 29, 29, 2" ::: "fr29"  );


  QPX_LOAD(src10_1,inc,f10);
  ASM("qvfadd 30, 30, 19" ::: "fr30"  ); //first second-half min done
  QPX_LOAD(src2_1,inc,f18);
  ASM("qvfadd 29, 29, 3" ::: "fr29"  );

  QPX_LOAD(src11_1,inc,f11);
  QPX_LOAD(src3_1,inc,f19);
  ASM("qvfadd 29, 29, 4" ::: "fr29"  );



  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 31, 16, 17" ::: "fr31"  ); //second second-half min start
  QPX_LOAD(src4_1,inc,f16);
  ASM("qvfadd 29, 29, 5" ::: "fr29"  );
  QPX_STORE(dst_2,inc1,f30);


  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 31, 31, 18" ::: "fr31"  );
  QPX_LOAD(src5_1,inc,f17);
  ASM("qvfadd 29, 29, 6" ::: "fr29"  );


  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 31, 31, 19" ::: "fr31"  );
  QPX_LOAD(src6_1,inc,f18);
  ASM("qvfadd 29, 29, 7" ::: "fr29"  );


  QPX_LOAD(src15_1,inc,f15);
  ASM("qvfadd 31, 31, 16" ::: "fr31"  );
  QPX_LOAD(src7_1,inc,f19);
  ASM("qvfadd 29, 29, 8" ::: "fr29"  );

	//y-=2;
	y-=1;
	if (!y) break;

  QPX_LOAD(src0_1,inc,f0);
  ASM("qvfadd 31, 31, 17" ::: "fr31"  );
  QPX_LOAD(src8_1,inc,f16);
  ASM("qvfadd 29, 29, 9" ::: "fr29"  );


  QPX_LOAD(src1_1,inc,f1);
  ASM("qvfadd 31, 31, 18" ::: "fr31"  );
  QPX_LOAD(src9_1,inc,f17);
  ASM("qvfadd 29, 29, 10" ::: "fr29"  );

  QPX_LOAD(src2_1,inc,f2);
  ASM("qvfadd 31, 31, 19" ::: "fr31"  );
  QPX_LOAD(src10_1,inc,f18);
  ASM("qvfadd 29, 29, 11" ::: "fr29"  );

  QPX_LOAD(src3_1,inc,f3);
  ASM("qvfadd 31, 31, 16" ::: "fr31"  );
  QPX_LOAD(src11_1,inc,f19);
  ASM("qvfadd 29, 29, 12" ::: "fr29"  );

  QPX_LOAD(src4_1,inc,f4);
  ASM("qvfadd 31, 31, 17" ::: "fr31"  );
  QPX_LOAD(src12_1,inc,f16);
  ASM("qvfadd 29, 29, 13" ::: "fr29"  );

  QPX_LOAD(src5_1,inc,f5);
  ASM("qvfadd 31, 31, 18" ::: "fr31"  );
  QPX_LOAD(src13_1,inc,f17);
  ASM("qvfadd 29, 29, 14" ::: "fr29"  );

  QPX_LOAD(src6_1,inc,f6);
  ASM("qvfadd 31, 31, 19" ::: "fr31"  );
  QPX_LOAD(src14_1,inc,f18);
  ASM("qvfadd 29, 29, 15" ::: "fr29"  );

  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfadd 31, 31, 16" ::: "fr31"  );
  QPX_LOAD(src15_1,inc,f19);

  QPX_LOAD(src8_1,inc,f8);
  ASM("qvfadd 31, 31, 17" ::: "fr31"  );
  QPX_LOAD(src0_1,inc,f16);
  ASM("qvfadd 28, 0, 1" ::: "fr28"  );

  QPX_LOAD(src9_1,inc,f9);
  ASM("qvfadd 31, 31, 18" ::: "fr31"  );
  QPX_STORE(dst_1,inc1,f29);
  QPX_LOAD(src1_1,inc,f17);
  ASM("qvfadd 28, 28, 2" ::: "fr28"  );

  QPX_LOAD(src10_1,inc,f10);
  ASM("qvfadd 31, 31, 19" ::: "fr31"  );
  QPX_LOAD(src2_1,inc,f18);
  ASM("qvfadd 28, 28, 3" ::: "fr28"  );

  QPX_LOAD(src11_1,inc,f11);
  QPX_LOAD(src3_1,inc,f19);
  ASM("qvfadd 28, 28, 4" ::: "fr28"  );

  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 30, 16, 17" ::: "fr30"  );
  QPX_LOAD(src4_1,inc,f16);
  QPX_STORE(dst_2,inc1,f31);
  ASM("qvfadd 28, 28, 5" ::: "fr28"  );

  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 30, 30, 18" ::: "fr30"  );
  QPX_LOAD(src5_1,inc,f17);
  ASM("qvfadd 28, 28, 6" ::: "fr28"  );

  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 30, 30, 19" ::: "fr30"  );
  QPX_LOAD(src6_1,inc,f18);
  ASM("qvfadd 28, 28, 7" ::: "fr28"  );

  QPX_LOAD(src15_1,inc,f15);
  ASM("qvfadd 30, 30, 16" ::: "fr30"  );
  QPX_LOAD(src7_1,inc,f19);
  ASM("qvfadd 28, 28, 8" ::: "fr28"  );
  }

  ASM("qvfadd 31, 31, 17" ::: "fr31"  );
  QPX_LOAD(src8_1,inc,f16);
  ASM("qvfadd 29, 29, 9" ::: "fr29"  );


  ASM("qvfadd 31, 31, 18" ::: "fr31"  );
  QPX_LOAD(src9_1,inc,f17);
  ASM("qvfadd 29, 29, 10" ::: "fr29"  );

  ASM("qvfadd 31, 31, 19" ::: "fr31"  );
  QPX_LOAD(src10_1,inc,f18);
  ASM("qvfadd 29, 29, 11" ::: "fr29"  );

  ASM("qvfadd 31, 31, 16" ::: "fr31"  );
  QPX_LOAD(src11_1,inc,f19);
  ASM("qvfadd 29, 29, 12" ::: "fr29"  );

  ASM("qvfadd 31, 31, 17" ::: "fr31"  );
  QPX_LOAD(src12_1,inc,f16);
  ASM("qvfadd 29, 29, 13" ::: "fr29"  );

  ASM("qvfadd 31, 31, 18" ::: "fr31"  );
  QPX_LOAD(src13_1,inc,f17);
  ASM("qvfadd 29, 29, 14" ::: "fr29"  );

  ASM("qvfadd 31, 31, 19" ::: "fr31"  );
  QPX_LOAD(src14_1,inc,f18);
  ASM("qvfadd 29, 29, 15" ::: "fr29"  );

  ASM("qvfadd 31, 31, 16" ::: "fr31"  );
  QPX_LOAD(src15_1,inc,f19);

  ASM("qvfadd 31, 31, 17" ::: "fr31"  );

  ASM("qvfadd 31, 31, 18" ::: "fr31"  );
  QPX_STORE(dst_1,inc1,f29);

  ASM("qvfadd 31, 31, 19" ::: "fr31"  );
  QPX_STORE(dst_2,inc1,f31);

  return ((num_dbls>>(3+1)) << (3+1));
}

#if 0
inline unsigned _quad_double_min_16way_align64B(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls)
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
  register double f16 FP_REGISTER(16) = 0.0;
  register double f17 FP_REGISTER(17) = 0.0;
  register double f18 FP_REGISTER(18) = 0.0;
  register double f19 FP_REGISTER(19) = 0.0;
  register double f20 FP_REGISTER(20) = 0.0;
  register double f21 FP_REGISTER(21) = 0.0;
  register double f22 FP_REGISTER(22) = 0.0;
  register double f23 FP_REGISTER(23) = 0.0;
  register double f24 FP_REGISTER(24) = 0.0;
  register double f25 FP_REGISTER(25) = 0.0;
  register double f26 FP_REGISTER(26) = 0.0;
  register double f27 FP_REGISTER(27) = 0.0;
  register double f28 FP_REGISTER(28) = 0.0;
  register double f29 FP_REGISTER(29) = 0.0;
  register double f30 FP_REGISTER(30) = 0.0;
  register double f31 FP_REGISTER(31) = 0.0;

  double *src0_1, *src1_1, *src2_1, *src3_1, *src4_1, *src5_1, *src6_1, *src7_1;
  double *src8_1, *src9_1, *src10_1, *src11_1, *src12_1, *src13_1, *src14_1, *src15_1;
  double *dst_1, *dst_2;
  //register int inc __MYASM__("r7");
  //register int inc __MYASM__("r7");
  register int inc ;
  register int inc1;
  register int  y;
  
  src0_1 = src0 -4;  
  src1_1 = src1 -4; 
  src2_1 = src2 -4;
  src3_1 = src3 -4;
  src4_1 = src4 -4;
  src5_1 = src5 -4;
  src6_1 = src6 -4;
  src7_1 = src7 -4;
  src8_1 = src8 -4;
  src9_1 = src9 -4;
  src10_1 = src10 -4;
  src11_1 = src11 -4;
  src12_1 = src12 -4;
  src13_1 = src13 -4;
  src14_1 = src14 -4;
  src15_1 = src15 -4;

  dst_1 = dest -8;
  dst_2 = dest -4;  

  inc=32;
  inc1=64;

  y = (num_dbls>>3) >> 1;

  QPX_LOAD(src0_1,inc,f0); 
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src2_1,inc,f2);
  QPX_LOAD(src3_1,inc,f3);
  QPX_LOAD(src4_1,inc,f4);
  QPX_LOAD(src5_1,inc,f5);
  QPX_LOAD(src6_1,inc,f6);
  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfmr 28, 0" ::: "fr28"  );

  QPX_LOAD(src8_1,inc,f8);
  QPX_LOAD(src0_1,inc,f0);
  ASM("qvfadd 28, 28, 1" ::: "fr28"  );


  QPX_LOAD(src9_1,inc,f9);
  QPX_LOAD(src1_1,inc,f1);
  ASM("qvfadd 28, 28, 2" ::: "fr28"  );


  QPX_LOAD(src10_1,inc,f10);
  QPX_LOAD(src2_1,inc,f2);
  ASM("qvfadd 28, 28, 3" ::: "fr28"  );


  QPX_LOAD(src11_1,inc,f11);
  ASM("qvfmr 30, 0" ::: "fr30"  );
  QPX_LOAD(src3_1,inc,f3);
  ASM("qvfadd 28, 28, 4" ::: "fr28"  );       

  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 30, 30, 1" ::: "fr30"  );
  QPX_LOAD(src4_1,inc,f4);
  ASM("qvfadd 28, 28, 5" ::: "fr28"  );       

  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 30, 30, 2" ::: "fr30"  );
  QPX_LOAD(src5_1,inc,f5);
  ASM("qvfadd 28, 28, 6" ::: "fr28"  );       

  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 30, 30, 3" ::: "fr30"  );
  QPX_LOAD(src6_1,inc,f6);
  ASM("qvfadd 28, 28, 7" ::: "fr28"  );       

  QPX_LOAD(src15_1,inc,f15);
  ASM("qvfadd 30, 30, 4" ::: "fr30"  );
  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfadd 28, 28, 8" ::: "fr28"  );       

  while (1)
  {

  QPX_LOAD(src0_1,inc,f0); 
  ASM("qvfadd 30, 30, 5" ::: "fr30"  );
  QPX_LOAD(src8_1,inc,f8);
  ASM("qvfadd 28, 28, 9" ::: "fr28"  );       

  QPX_LOAD(src1_1,inc,f1);
  ASM("qvfadd 30, 30, 6" ::: "fr30"  );
  QPX_LOAD(src9_1,inc,f9);
  ASM("qvfadd 28, 28, 10" ::: "fr28"  );      

  QPX_LOAD(src2_1,inc,f2);
  ASM("qvfadd 30, 30, 7" ::: "fr30"  );
  QPX_LOAD(src10_1,inc,f10);
  ASM("qvfadd 28, 28, 11" ::: "fr28"  );      

  QPX_LOAD(src3_1,inc,f3);
  ASM("qvfadd 30, 30, 8" ::: "fr30"  );
  QPX_LOAD(src11_1,inc,f11);
  ASM("qvfadd 28, 28, 12" ::: "fr28"  );      

  QPX_LOAD(src4_1,inc,f4);
  ASM("qvfadd 30, 30, 9" ::: "fr30"  );
  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 28, 28, 13" ::: "fr28"  );      

  QPX_LOAD(src5_1,inc,f5);
  ASM("qvfadd 30, 30, 10" ::: "fr30"  );
  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 28, 28, 14" ::: "fr28"  );      

  QPX_LOAD(src6_1,inc,f6);
  ASM("qvfadd 30, 30, 11" ::: "fr30"  );
  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 28, 28, 15" ::: "fr28"  ); //first first-half min done      


  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfadd 30, 30, 12" ::: "fr30"  );
  QPX_LOAD(src15_1,inc,f15);


  QPX_LOAD(src8_1,inc,f8);
  ASM("qvfadd 30, 30, 13" ::: "fr30"  );
  QPX_LOAD(src0_1,inc,f0);
  ASM("qvfadd 29, 0, 1" ::: "fr29"  );  //second first-half min start


  QPX_LOAD(src9_1,inc,f9);
  ASM("qvfadd 30, 30, 14" ::: "fr30"  );
  QPX_STORE(dst_1,inc1,f28);
  QPX_LOAD(src1_1,inc,f1);
  ASM("qvfadd 29, 29, 2" ::: "fr29"  );


  QPX_LOAD(src10_1,inc,f10);
  ASM("qvfadd 30, 30, 15" ::: "fr30"  ); //first second-half min done
  QPX_LOAD(src2_1,inc,f2);
  ASM("qvfadd 29, 29, 3" ::: "fr29"  );
  
  QPX_LOAD(src11_1,inc,f11);
  QPX_LOAD(src3_1,inc,f3);
  ASM("qvfadd 29, 29, 4" ::: "fr29"  );


  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 31, 0, 1" ::: "fr31"  ); //second second-half min start
  QPX_LOAD(src4_1,inc,f4);
  ASM("qvfadd 29, 29, 5" ::: "fr29"  );
  QPX_STORE(dst_2,inc1,f30);


  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 31, 31, 2" ::: "fr31"  ); 
  QPX_LOAD(src5_1,inc,f5);
  ASM("qvfadd 29, 29, 6" ::: "fr29"  );


  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 31, 31, 3" ::: "fr31"  ); 
  QPX_LOAD(src6_1,inc,f6);
  ASM("qvfadd 29, 29, 7" ::: "fr29"  );


  QPX_LOAD(src15_1,inc,f15);
  ASM("qvfadd 31, 31, 4" ::: "fr31"  ); 
  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfadd 29, 29, 8" ::: "fr29"  );

  //y-=2;
  y-=1;
  if (!y) break; 

  QPX_LOAD(src0_1,inc,f0); 
  ASM("qvfadd 31, 31, 5" ::: "fr31"  ); 
  QPX_LOAD(src8_1,inc,f8);
  ASM("qvfadd 29, 29, 9" ::: "fr29"  );


  QPX_LOAD(src1_1,inc,f1);
  ASM("qvfadd 31, 31, 6" ::: "fr31"  ); 
  QPX_LOAD(src9_1,inc,f9);
  ASM("qvfadd 29, 29, 10" ::: "fr29"  );

  QPX_LOAD(src2_1,inc,f2);
  ASM("qvfadd 31, 31, 7" ::: "fr31"  ); 
  QPX_LOAD(src10_1,inc,f10);
  ASM("qvfadd 29, 29, 11" ::: "fr29"  );

  QPX_LOAD(src3_1,inc,f3);
  ASM("qvfadd 31, 31, 8" ::: "fr31"  ); 
  QPX_LOAD(src11_1,inc,f11);
  ASM("qvfadd 29, 29, 12" ::: "fr29"  );

  QPX_LOAD(src4_1,inc,f4);
  ASM("qvfadd 31, 31, 9" ::: "fr31"  ); 
  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 29, 29, 13" ::: "fr29"  );

  QPX_LOAD(src5_1,inc,f5);
  ASM("qvfadd 31, 31, 10" ::: "fr31"  ); 
  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 29, 29, 14" ::: "fr29"  );

  QPX_LOAD(src6_1,inc,f6);
  ASM("qvfadd 31, 31, 11" ::: "fr31"  ); 
  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 29, 29, 15" ::: "fr29"  );

  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfadd 31, 31, 12" ::: "fr31"  ); 
  QPX_LOAD(src15_1,inc,f15);
  ASM("qvfmr 28, 0" ::: "fr28"  );

  QPX_LOAD(src8_1,inc,f8);
  ASM("qvfadd 31, 31, 13" ::: "fr31"  ); 
  QPX_LOAD(src0_1,inc,f0);
  ASM("qvfadd 28, 28, 1" ::: "fr28"  );

  QPX_LOAD(src9_1,inc,f9);
  ASM("qvfadd 31, 31, 14" ::: "fr31"  ); 
  QPX_STORE(dst_1,inc1,f29);
  QPX_LOAD(src1_1,inc,f1);
  ASM("qvfadd 28, 28, 2" ::: "fr28"  );

  QPX_LOAD(src10_1,inc,f10);
  ASM("qvfadd 31, 31, 15" ::: "fr31"  ); 
  QPX_LOAD(src2_1,inc,f2);
  ASM("qvfadd 28, 28, 3" ::: "fr28"  );
  
  QPX_LOAD(src11_1,inc,f11);
  ASM("qvfmr 30, 0" ::: "fr30"  );
  QPX_LOAD(src3_1,inc,f3);
  ASM("qvfadd 28, 28, 4" ::: "fr28"  );

  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 30, 30, 1" ::: "fr30"  ); 
  QPX_LOAD(src4_1,inc,f4);
  QPX_STORE(dst_2,inc1,f31);
  ASM("qvfadd 28, 28, 5" ::: "fr28"  );

  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 30, 30, 2" ::: "fr30"  ); 
  QPX_LOAD(src5_1,inc,f5);
  ASM("qvfadd 28, 28, 6" ::: "fr28"  );

  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 30, 30, 3" ::: "fr30"  ); 
  QPX_LOAD(src6_1,inc,f6);
  ASM("qvfadd 28, 28, 7" ::: "fr28"  );

  QPX_LOAD(src15_1,inc,f15);
  ASM("qvfadd 30, 30, 4" ::: "fr30"  ); 
  QPX_LOAD(src7_1,inc,f7);
  ASM("qvfadd 28, 28, 8" ::: "fr28"  );
  }

  ASM("qvfadd 31, 31, 5" ::: "fr31"  ); 
  QPX_LOAD(src8_1,inc,f8);
  ASM("qvfadd 29, 29, 9" ::: "fr29"  );

  ASM("qvfadd 31, 31, 6" ::: "fr31"  ); 
  QPX_LOAD(src9_1,inc,f9);
  ASM("qvfadd 29, 29, 10" ::: "fr29"  );

  ASM("qvfadd 31, 31, 7" ::: "fr31"  ); 
  QPX_LOAD(src10_1,inc,f10);
  ASM("qvfadd 29, 29, 11" ::: "fr29"  );

  ASM("qvfadd 31, 31, 8" ::: "fr31"  ); 
  QPX_LOAD(src11_1,inc,f11);
  ASM("qvfadd 29, 29, 12" ::: "fr29"  );

  ASM("qvfadd 31, 31, 9" ::: "fr31"  ); 
  QPX_LOAD(src12_1,inc,f12);
  ASM("qvfadd 29, 29, 13" ::: "fr29"  );

  ASM("qvfadd 31, 31, 10" ::: "fr31"  ); 
  QPX_LOAD(src13_1,inc,f13);
  ASM("qvfadd 29, 29, 14" ::: "fr29"  );

  ASM("qvfadd 31, 31, 11" ::: "fr31"  ); 
  QPX_LOAD(src14_1,inc,f14);
  ASM("qvfadd 29, 29, 15" ::: "fr29"  );

  ASM("qvfadd 31, 31, 12" ::: "fr31"  ); 
  QPX_LOAD(src15_1,inc,f15);

  ASM("qvfadd 31, 31, 13" ::: "fr31"  ); 

  ASM("qvfadd 31, 31, 14" ::: "fr31"  ); 
  QPX_STORE(dst_1,inc1,f29);

  ASM("qvfadd 31, 31, 15" ::: "fr31"  ); 
  QPX_STORE(dst_2,inc1,f31);

  return ((num_dbls>>(3+1)) << 3+1);

}
#endif

//alteast 32B (4 Dbls) expected
inline unsigned _quad_double_min_16way_align32B_short(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls)
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
  register double f31 FP_REGISTER(31) = 0.0;

  register double *src0_1, *src1_1, *src2_1, *src3_1, *src4_1, *src5_1, *src6_1, *src7_1;
  register double *src8_1, *src9_1, *src10_1, *src11_1, *src12_1, *src13_1, *src14_1, *src15_1;
  register double *dst_1;
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
  src8_1 = src8 -4;
  src9_1 = src9 -4;
  src10_1 = src10 -4;
  src11_1 = src11 -4;
  src12_1 = src12 -4;
  src13_1 = src13 -4;
  src14_1 = src14 -4;
  src15_1 = src15 -4;

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
  QPX_LOAD(src8_1,inc,f8);
  QPX_LOAD(src9_1,inc,f9);
  QPX_LOAD(src10_1,inc,f10);
  QPX_LOAD(src11_1,inc,f11);
  QPX_LOAD(src12_1,inc,f12);
  QPX_LOAD(src13_1,inc,f13);
  QPX_LOAD(src14_1,inc,f14);
  QPX_LOAD(src15_1,inc,f15);

  if (y == 1)
  {
    ASM("qvfmr 28, 0" ::: "fr28"  );
    ASM("qvfmr 29, 1" ::: "fr29"  );
    ASM("qvfmr 30, 2" ::: "fr30"  );
    ASM("qvfmr 31, 3" ::: "fr31"  );
    //ASM("qvfadd 28, 28, 4" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 4" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 4" ::: "fr28" );
    //ASM("qvfadd 29, 29, 5" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 5" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 5" ::: "fr29" );
    //ASM("qvfadd 30, 30, 6" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 6" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 6" ::: "fr30" );
    //ASM("qvfadd 31, 31, 7" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 7" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 7" ::: "fr31" );
    //ASM("qvfadd 28, 28, 8" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 8" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 8" ::: "fr28" );
    //ASM("qvfadd 29, 29, 9" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 9" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 9" ::: "fr29" );
    //ASM("qvfadd 30, 30, 10" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 10" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 10" ::: "fr30" );
    //ASM("qvfadd 31, 31, 11" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 11" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 11" ::: "fr31" );
    //ASM("qvfadd 28, 28, 12" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 12" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 12" ::: "fr28" );
    //ASM("qvfadd 29, 29, 13" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 13" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 13" ::: "fr29" );
    //ASM("qvfadd 30, 30, 14" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 14" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 14" ::: "fr30" );
    //ASM("qvfadd 31, 31, 15" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 15" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 15" ::: "fr31" );
    //ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 28" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 28" ::: "fr29" );
    //ASM("qvfadd 31, 31, 30" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 30" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 30" ::: "fr31" );
    //ASM("qvfadd 31, 31, 29" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 29" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 29" ::: "fr31" );
    QPX_STORE(dst_1,inc,f31);
    return 4;
  }

  --y;

  ASM("qvfmr 28, 0" ::: "fr28"  );
  QPX_LOAD(src0_1,inc,f0);

  ASM("qvfmr 29, 1" ::: "fr29"  );
  QPX_LOAD(src1_1,inc,f1);

  ASM("qvfmr 30, 2" ::: "fr30"  );
  QPX_LOAD(src2_1,inc,f2);

  ASM("qvfmr 31, 3" ::: "fr31"  );
  QPX_LOAD(src3_1,inc,f3);

  //ASM("qvfadd 28, 28, 4" ::: "fr28"  );
  ASM("qvfcmplt 16, 28, 4" ::: "fr16" );
  ASM("qvfsel 28, 16, 28, 4" ::: "fr28" );
  QPX_LOAD(src4_1,inc,f4);

  //ASM("qvfadd 29, 29, 5" ::: "fr29"  );
  ASM("qvfcmplt 16, 29, 5" ::: "fr16" );
  ASM("qvfsel 29, 16, 29, 5" ::: "fr29" );
  QPX_LOAD(src5_1,inc,f5);

  //ASM("qvfadd 30, 30, 6" ::: "fr30"  );
  ASM("qvfcmplt 16, 30, 6" ::: "fr16" );
  ASM("qvfsel 30, 16, 30, 6" ::: "fr30" );
  QPX_LOAD(src6_1,inc,f6);

  //ASM("qvfadd 31, 31, 7" ::: "fr31"  );
  ASM("qvfcmplt 16, 31, 7" ::: "fr16" );
  ASM("qvfsel 31, 16, 31, 7" ::: "fr31" );
  QPX_LOAD(src7_1,inc,f7);

  while (1){

    //ASM("qvfadd 28, 28, 8" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 8" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 8" ::: "fr28" );
    QPX_LOAD(src8_1,inc,f8);

    //ASM("qvfadd 29, 29, 9" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 9" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 9" ::: "fr29" );
    QPX_LOAD(src9_1,inc,f9);

    //ASM("qvfadd 30, 30, 10" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 10" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 10" ::: "fr30" );
    QPX_LOAD(src10_1,inc,f10);

    //ASM("qvfadd 31, 31, 11" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 11" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 11" ::: "fr31" );
    QPX_LOAD(src11_1,inc,f11);

    //ASM("qvfadd 28, 28, 12" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 12" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 12" ::: "fr28" );
    QPX_LOAD(src12_1,inc,f12);

    //ASM("qvfadd 29, 29, 13" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 13" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 13" ::: "fr29" );
    QPX_LOAD(src13_1,inc,f13);

    //ASM("qvfadd 30, 30, 14" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 14" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 14" ::: "fr30" );
    QPX_LOAD(src14_1,inc,f14);

    //ASM("qvfadd 31, 31, 15" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 15" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 15" ::: "fr31" );
    QPX_LOAD(src15_1,inc,f15);

    if (!--y) break;

    //ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 28" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 28" ::: "fr29" );
    ASM("qvfmr 28, 0" ::: "fr28"  );
    QPX_LOAD(src0_1,inc,f0);

    //ASM("qvfadd 31, 31, 30" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 30" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 30" ::: "fr31" );
    //ASM("qvfadd 31, 31, 29" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 29" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 29" ::: "fr31" );

    ASM("qvfmr 29, 1" ::: "fr29"  );
    QPX_LOAD(src1_1,inc,f1);

    ASM("qvfmr 30, 2" ::: "fr30"  );
    QPX_LOAD(src2_1,inc,f2);

    QPX_STORE(dst_1,inc,f31);
    ASM("qvfmr 31, 3" ::: "fr31"  );
    QPX_LOAD(src3_1,inc,f3);

    //ASM("qvfadd 28, 28, 4" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 4" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 4" ::: "fr28" );
    QPX_LOAD(src4_1,inc,f4);

    //ASM("qvfadd 29, 29, 5" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 5" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 5" ::: "fr29" );
    QPX_LOAD(src5_1,inc,f5);

    //ASM("qvfadd 30, 30, 6" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 6" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 6" ::: "fr30" );
    QPX_LOAD(src6_1,inc,f6);

    //ASM("qvfadd 31, 31, 7" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 7" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 7" ::: "fr31" );
    QPX_LOAD(src7_1,inc,f7);
  } 

    //ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 28" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 28" ::: "fr29" );
    //ASM("qvfadd 31, 31, 30" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 30" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 30" ::: "fr31" );
    //ASM("qvfadd 31, 31, 29" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 29" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 29" ::: "fr31" );
    QPX_STORE(dst_1,inc,f31);

    ASM("qvfmr 28, 0" ::: "fr28"  );
    ASM("qvfmr 29, 1" ::: "fr29"  );
    ASM("qvfmr 30, 2" ::: "fr30"  );
    ASM("qvfmr 31, 3" ::: "fr31"  );
    //ASM("qvfadd 28, 28, 4" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 4" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 4" ::: "fr28" );
    //ASM("qvfadd 29, 29, 5" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 5" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 5" ::: "fr29" );
    //ASM("qvfadd 30, 30, 6" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 6" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 6" ::: "fr30" );
    //ASM("qvfadd 31, 31, 7" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 7" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 7" ::: "fr31" );
    //ASM("qvfadd 28, 28, 8" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 8" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 8" ::: "fr28" );
    //ASM("qvfadd 29, 29, 9" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 9" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 9" ::: "fr29" );
    //ASM("qvfadd 30, 30, 10" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 10" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 10" ::: "fr30" );
    //ASM("qvfadd 31, 31, 11" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 11" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 11" ::: "fr31" );
    //ASM("qvfadd 28, 28, 12" ::: "fr28"  );
    ASM("qvfcmplt 16, 28, 12" ::: "fr16" );
    ASM("qvfsel 28, 16, 28, 12" ::: "fr28" );
    //ASM("qvfadd 29, 29, 13" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 13" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 13" ::: "fr29" );
    //ASM("qvfadd 30, 30, 14" ::: "fr30"  );
    ASM("qvfcmplt 16, 30, 14" ::: "fr16" );
    ASM("qvfsel 30, 16, 30, 14" ::: "fr30" );
    //ASM("qvfadd 31, 31, 15" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 15" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 15" ::: "fr31" );
    //ASM("qvfadd 29, 29, 28" ::: "fr29"  );
    ASM("qvfcmplt 16, 29, 28" ::: "fr16" );
    ASM("qvfsel 29, 16, 29, 28" ::: "fr29" );
    //ASM("qvfadd 31, 31, 30" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 30" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 30" ::: "fr31" );
    //ASM("qvfadd 31, 31, 29" ::: "fr31"  );
    ASM("qvfcmplt 16, 31, 29" ::: "fr16" );
    ASM("qvfsel 31, 16, 31, 29" ::: "fr31" );
    QPX_STORE(dst_1,inc,f31);

  return ((num_dbls >> 2) << 2);

}

inline unsigned _quad_double_min_16way_single(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls)
{

  unsigned i;

  for (i = 0; i < num_dbls; i++)
  {
    dst[i] = (src0[i] < src1[i])? src0[i]:src1[i];
    dst[i] = (dst[i] < src2[i])? dst[i]:src2[i];
    dst[i] = (dst[i] < src3[i])? dst[i]:src3[i];
    dst[i] = (dst[i] < src4[i])? dst[i]:src4[i];
    dst[i] = (dst[i] < src5[i])? dst[i]:src5[i];
    dst[i] = (dst[i] < src6[i])? dst[i]:src6[i];
    dst[i] = (dst[i] < src7[i])? dst[i]:src7[i];
    dst[i] = (dst[i] < src8[i])? dst[i]:src8[i];
    dst[i] = (dst[i] < src9[i])? dst[i]:src9[i];
    dst[i] = (dst[i] < src10[i])? dst[i]:src10[i];
    dst[i] = (dst[i] < src11[i])? dst[i]:src11[i];
    dst[i] = (dst[i] < src12[i])? dst[i]:src12[i];
    dst[i] = (dst[i] < src13[i])? dst[i]:src13[i];
    dst[i] = (dst[i] < src14[i])? dst[i]:src14[i];
    dst[i] = (dst[i] < src15[i])? dst[i]:src15[i];
  }
  return num_dbls;

}

//atleast 20 Dbls exptected..(20-4) == 16 for 64B aligned min
inline unsigned _quad_double_min_16way_align32B(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls)
{

  unsigned  dbls = 0;
  uint64_t  mask;
  mask = (uint64_t)dst|(uint64_t)src0|(uint64_t)src1|(uint64_t)src2|(uint64_t)src3|(uint64_t)src4|(uint64_t)src5|
           (uint64_t)src6|(uint64_t)src7|(uint64_t)src8|(uint64_t)src9|(uint64_t)src10|(uint64_t)src11| 
           (uint64_t)src12|(uint64_t)src13|(uint64_t)src14|(uint64_t)src15;
  bool is_64B_aligned;
  is_64B_aligned = !((mask & ((uint64_t)63)) == 0);
  
  if (is_64B_aligned)
  {
    dbls = _quad_double_min_16way_align64B(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8, 
                                          src9, src10, src11, src12, src13, src14, src15, num_dbls);
    if (dbls == num_dbls) return dbls;
    _quad_double_min_16way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, src3+dbls, src4+dbls, 
        src5+dbls, src6+dbls, src7+dbls, src8+dbls, src9+dbls, src10+dbls, src11+dbls, 
        src12+dbls, src13+dbls, src14+dbls, src15+dbls, num_dbls - dbls);
    return num_dbls;
  }

  /*_quad_double_min_16way_single(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8, 
                                src9, src10, src11, src12, src13, src14, src15, 4);

  dbls = _quad_double_min_16way_align64B(dst+4, src0+4, src1+4, src2+4, src3+4, src4+4, src5+4, src6+4, src7+4, src8+4, 
                                        src9+4, src10+4, src11+4, src12+4, src13+4, src14+4, src15+4, num_dbls - 4);
  dbls+=4;*/
   dbls = _quad_double_min_16way_align32B_short(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8, 
                                          src9, src10, src11, src12, src13, src14, src15, num_dbls);

  return dbls;

}
#if 0
//buffers aligned to 32B 
inline unsigned quad_double_min_16way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls)
{
  
  unsigned  dbls = 0;

  if (num_dbls < 20)
   dbls = _quad_double_min_16way_align32B_short(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8, 
                                          src9, src10, src11, src12, src13, src14, src15, num_dbls);
  else
   dbls = _quad_double_min_16way_align32B(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8,
                                          src9, src10, src11, src12, src13, src14, src15, num_dbls);

  if (num_dbls - dbls)
   return _quad_double_min_16way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, src3+dbls, src4+dbls, 
                                  src5+dbls, src6+dbls, src7+dbls, src8+dbls, src9+dbls, src10+dbls, src11+dbls, 
                                  src12+dbls, src13+dbls, src14+dbls, src15+dbls, num_dbls - dbls);

  return num_dbls;
}
#endif

inline unsigned quad_double_min_16way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls)
{

  unsigned  dbls = 0;
  const uint64_t      alignment = 32;
  uint64_t    mask    = 0;
  mask = (alignment - 1);

  if (mask & ((uint64_t)dst | (uint64_t)src0|(uint64_t)src1|(uint64_t)src2|(uint64_t)src3|(uint64_t)src4|(uint64_t)src5|(uint64_t)src6|
      (uint64_t)src7| (uint64_t)src8|(uint64_t)src9|(uint64_t)src10|(uint64_t)src11|(uint64_t)src12|(uint64_t)src13|
      (uint64_t)src14|(uint64_t)src15))
  {
    _pami_core_double_min_16way(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8,
        src9, src10, src11, src12, src13, src14, src15, num_dbls);
    return 0;
  }


  if (num_dbls <= 128)
   dbls = _quad_double_min_16way_align32B_short(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8, 
                                          src9, src10, src11, src12, src13, src14, src15, num_dbls);
  else
  {
    dbls = quad_double_min_8way(dst, src0, src1, src2, src3, src4, src5, src6, src7, num_dbls);
    /*for (unsigned i=0; i < num_dbls; i++)
    { printf("[%d]:%f\t",i, dst[i]); printf("\n");}*/
    dbls = quad_double_min_8way(dst, dst, src8, src9, src10, src11, src12, src13, src14, num_dbls);
    /*for (unsigned i=0; i < num_dbls; i++)
    { printf("[%d]:%f\t",i, dst[i]); printf("\n");}*/
    dbls = quad_double_min_2way(dst, dst, src15, num_dbls);
    /*for (unsigned i=0; i < num_dbls; i++)
    { printf("[%d]:%f\t",i, dst[i]); printf("\n");}*/
  }

  if (num_dbls - dbls)
    _quad_double_min_16way_single(dst+dbls, src0+dbls, src1+dbls, src2+dbls, src3+dbls, src4+dbls, 
        src5+dbls, src6+dbls, src7+dbls, src8+dbls, src9+dbls, src10+dbls, src11+dbls, 
        src12+dbls, src13+dbls, src14+dbls, src15+dbls, num_dbls - dbls);

  return num_dbls;
}


#endif
