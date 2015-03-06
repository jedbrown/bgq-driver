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

/// \file test/internals/atomics/bgp/bgpcompilation.cc
/// \brief BG/P comilation test to ensure objects do not get broken accidentally
///
/// This program should never be run!

#include "../compilation.h"

#include "components/atomic/bgp/BgpAtomicCounter.h"
#include "components/atomic/bgp/BgpAtomicMutex.h"
#include "components/atomic/bgp/LockBoxCounter.h"
#include "components/atomic/bgp/LockBoxMutex.h"
#include "components/atomic/bgp/LockBoxBarrier.h"
#include "components/atomic/bgp/LwarxStwcxMutex.h"
#include "components/atomic/indirect/IndirectBarrier.h"
#include "components/atomic/indirect/IndirectCounter.h"
#include "components/atomic/indirect/IndirectMutex.h"

int main(int argc, char **argv) {
	if (argc >= 0) {
		fprintf(stderr, "This program is not intended to be run!\n");
		exit(1);
	}
	PAMI::Memory::GenMemoryManager mm;
	mm.init(&mm, argc); // anything

        COUNTER_HELPER(PAMI::Counter::BGP::LockBoxCounter, counter1, &mm, argv[1]);

        COUNTER_HELPER2(PAMI::Counter::BGP::Atomic, counter3);

        COUNTER_HELPER(PAMI::Counter::Indirect<PAMI::Counter::BGP::Atomic>, counter4, &mm, argv[1]);




        MUTEX_HELPER2(PAMI::Mutex::BGP::Atomic, mutex1);

        MUTEX_HELPER(PAMI::Mutex::Indirect<PAMI::Mutex::BGP::Atomic>, mutex2, &mm, argv[1]);

        MUTEX_HELPER(PAMI::Mutex::BGP::LockBoxMutex, mutex3, &mm, argv[1]);

        MUTEX_HELPER(PAMI::Mutex::BGP::FairLockBoxMutex, mutex5, &mm, argv[1]);

        MUTEX_HELPER2(PAMI::Mutex::BGP::LwarxStwcx, mutex7);

        MUTEX_HELPER(PAMI::Mutex::Indirect<PAMI::Mutex::BGP::LwarxStwcx>, mutex8, &mm, argv[1]);




        BARRIER_HELPER(PAMI::Barrier::BGP::LockBoxNodeProcBarrier, barrier1, &mm, argv[1], argc, (argc == 0));

        BARRIER_HELPER(PAMI::Barrier::BGP::LockBoxNodeCoreBarrier, barrier2, &mm, argv[1], argc, (argc == 0));

        return 1;
}
