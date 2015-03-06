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
 * \file components/devices/shmem/asmheader.h
 * \brief (qpx) quad-vector floating point asm macros for use in the shmem device
 */
#ifndef __components_devices_shmem_asmheader_h__
#define __components_devices_shmem_asmheader_h__

#include "math/a2qpx/a2qpx_asm.h"

#if 0
#define VECTOR_LOAD(si,sb,tgt) \
do { \
  asm volatile("qvlfdux %2,%0,%1": "+Ob" (si) : "r" (sb), "i" (tgt) :"memory"); \
} while(0)
#endif

#if 0
#define VECT_LOAD_NU(si,sb,tgt) \
do { \
  asm volatile("qvlfdx %2,%0,%1": : "Ob" (si), "r" (sb), "i" (tgt) :"memory"); \
} while(0)
#endif


#if 0
#define VECTOR_STORE(si,sb,tgt) \
do { \
  asm volatile("qvstfdux %2,%0,%1": "+Ob" (si) : "r" (sb), "i" (tgt) :"memory"); \
} while(0)
#endif 

#if 0
#define VECT_STORE_NU(si,sb,tgt) \
do { \
  asm volatile("qvstfdx %2,%0,%1": "+Ob" (si) : "r" (sb), "i" (tgt) :"memory"); \
} while(0)
#endif

#if 0
#define VECTOR_STORE_REG(si,sb,tgt) \
do { \
  asm volatile("qvstfdux %2,%0,%1": "+Ob" (si) : "r" (sb), "f" (tgt) :"memory"); \
} while(0)
#endif 

#if 0
#define VECTOR_SUM(si,sb,tgt) \
do { \
  asm volatile("qvfadd tgt,%0,%1":  : "i" (si), "i" (sb)); \
} while(0)
#endif 

#if 0
#define VECTOR_SUM_REG(si,sb,tgt) \
do { \
  asm volatile("qvfadd %0,%1,%2": "=f" (tgt) : "i" (si), "f" (sb)); \
} while(0)
#endif 

#define ASM asm volatile

#endif
