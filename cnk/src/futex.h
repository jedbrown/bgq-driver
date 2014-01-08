/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#ifndef _CNK_FUTEX_H_ // Prevent multiple inclusion
#define _CNK_FUTEX_H_


#include "Kernel.h"

// cnk/bgp_futex.c
uint64_t Futex_Wait( uint32_t futex_op_and_flags, KThread_t *pKThr, Futex_t* futex_vaddr, uint32_t futex_val, uint64_t timeout, uint64_t current_time);

uint64_t Futex_Wake( uint32_t futex_op_and_flags, Futex_t* futex_vaddr, int max_to_wake, Futex_t* secondary_futex_vaddr, int max_to_wake2, Futex_t secondary_value);

int Futex_CheckTimeout(uint64_t current_timebase, KThread_t *kthread, uint64_t *pending_expiration);

void Futex_Interrupt( KThread_t *pKThr );


#endif // Add nothing below this line
