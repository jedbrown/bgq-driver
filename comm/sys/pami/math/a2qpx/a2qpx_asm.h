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
 * \file math/a2qpx/a2qpx_asm.h
 * \brief ???
 */

#ifndef __math_a2qpx_a2qpx_asm_h__
#define __math_a2qpx_a2qpx_asm_h__

#define QPX_LOAD(si,sb,fp)			\
  do {									\
  asm volatile("qvlfdux %0,%1,%2": "=f"(fp),"+b" (si): "r" (sb));	\
  } while(0);

#define QPX_LOAD_NU(si,sb,fp) \
  do {									\
  asm volatile("qvlfdx %0,%1,%2": "=f"(fp) : "b" (si), "r" (sb));	\
  } while(0);

#define QPX_STORE(si,sb,fp) \
  do {									\
  asm volatile("qvstfdux %2,%0,%1": "+b" (si) : "r" (sb), "f"(fp) :"memory");	\
  } while(0);

#define QPX_STORE_NU(si,sb,fp)						\
  do {									\
  asm volatile("qvstfdx %2,%0,%1": : "b" (si), "r" (sb), "f"(fp) :"memory"); \
  } while(0);

#endif
