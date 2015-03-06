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
 * \file arch/i386/Arch.h
 * \brief ???
 */

#ifndef __arch_i386_Arch_h__
#define __arch_i386_Arch_h__

#include "arch/ArchInterface.h"

// Here go things specific to this processor architecture

#define NUM_CORES 1
#define NUM_SMT   1

// somewhat arbitrary...
#define PAMI_MAX_PROC_PER_NODE   32
#define PAMI_MAX_THREAD_PER_PROC 32

#endif // __pami_arch_i386_h__
