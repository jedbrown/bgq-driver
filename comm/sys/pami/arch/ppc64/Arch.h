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
 * \file arch/ppc64/Arch.h
 * \brief ???
 */

#ifndef __arch_ppc64_Arch_h__
#define __arch_ppc64_Arch_h__

#include "arch/ArchInterface.h"

// Here go things specific to this processor architecture

#define NUM_CORES 1
#define NUM_SMT   1

// somewhat arbitrary...
#define PAMI_MAX_PROC_PER_NODE   128
#define PAMI_MAX_THREAD_PER_PROC 32
#define MATH_MAX_NSRC 4
#endif // __pami_arch_ppc64_h__
