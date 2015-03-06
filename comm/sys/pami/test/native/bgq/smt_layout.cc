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

/// \file test/native/bgq/smt_layout.cc
/// \brief Simple, native compiled, test for commthread layout pattern
///
/// Dumps the layout of commthreads given the number of processes per node.
/// Does not do much error checking.
///

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NUM_SMT		4
#define NUM_CORES	16

#define PAMI_assert_debugf(a...)

uint64_t ppn;		// processes per node
uint64_t tcoord;	// t-coord being processed

#define Kernel_ProcessorCount()	((NUM_CORES * NUM_SMT) / ppn)
#define Kernel_ProcessorID()	(tcoord * Kernel_ProcessorCount())
#define Kernel_ProcessCount()	(ppn)
#define Kernel_ThreadMask(t)	(ppn == 1 ? -1UL : \
	(((1UL << Kernel_ProcessorCount()) - 1) << \
	((NUM_CORES * NUM_SMT) - Kernel_ProcessorID() - Kernel_ProcessorCount())) )

namespace PAMI {
namespace Memory {
class MemoryManager {
	MemoryManager() { }
}; // class MemoryManager
}; // namespace Memory
}; // namespace PAMI

#define COMMTHREAD_LAYOUT_TESTING
#include "components/devices/bgq/commthread/CommThreadFactory.h"
#include "components/devices/bgq/commthread/CommThreadWakeup.h"

PAMI::Device::CommThread::Factory *fa;

int main(int argc, char **argv) {
	int x;
	fa = (PAMI::Device::CommThread::Factory *)malloc(sizeof(*fa));

	for (x = 1; x < argc; ++x) {
		ppn = strtoul(argv[x], NULL, 0);
		uint32_t cpp = NUM_CORES / ppn;

		uint32_t c0;
		for (tcoord = 0; tcoord < ppn; ++tcoord) {
			c0 = Kernel_ProcessorID() / NUM_SMT;
			new (fa) PAMI::Device::CommThread::Factory(NULL, NULL);
			uint32_t core, thread;
			int t;
			for (core = c0; core < c0 + cpp; ++core) {
				fprintf(stderr, "%3d %3d", c0, core);
				for (t = 0; t < NUM_SMT; ++t) {
					fa->getNextThread(core, thread);
					fprintf(stderr, " %3d", thread);
				}
				fprintf(stderr, "\n");
			}
		}
	}
}
