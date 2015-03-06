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

/// \file test/internals/atomics/compilation.cc
/// \brief Basic comilation test to ensure objects do not get broken accidentally
///
/// This program should never be run!

#include "compilation.h"

#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/counter/CounterBarrier.h"
#include "components/atomic/pthread/Pthread.h"

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

        COUNTER_HELPER(PAMI::Counter::Indirect<PAMI::Counter::Native>, counter1, &mm, argv[1]);
        COUNTER_HELPER2(PAMI::Counter::Native, counter2);
        COUNTER_HELPER2(PAMI::Counter::Pthread, counter3);

        MUTEX_HELPER2(PAMI::Mutex::Counter<PAMI::Counter::Native>, mutex1);
        MUTEX_HELPER(PAMI::Mutex::IndirectCounter<PAMI::Counter::Indirect<PAMI::Counter::Native> >, mutex2, &mm, argv[1]);

        BARRIER_HELPER(PAMI::Barrier::IndirectCounter<PAMI::Counter::Indirect<PAMI::Counter::Native> >, barrier3, &mm, argv[1], argc, (argc == 0));

        return 1;
}
