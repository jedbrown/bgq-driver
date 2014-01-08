/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#ifndef _FWEXT_H_ // Prevent multiple inclusion.
#define _FWEXT_H_

#include <hwi/include/common/compiler_support.h>

#ifndef __KERNEL__
#define __KERNEL__
#endif

#define FWEXT_STACK_SIZE (1ULL * 1024ULL * 1024ULL)

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>

#include "fwext_Linkage.h"

#include <hwi/include/common/bgq_alignment.h>
#include <hwi/include/common/bgq_bitnumbers.h>
#include <firmware/include/personality.h>
#include <firmware/include/Firmware_Interface.h>

#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/BeDram.h>
#include <firmware/include/BeDram_inlines.h>
#include <firmware/include/Firmware.h>
#include "fwext_regs.h"

/*
 * Naming conventions for the Firmware DDR Extensions:
 *
 *  - Types are of the format FwExt_*_t
 *  - Functions are of the form fwext_*()
 *
 */

typedef void (*FwExt_InterruptHandler_t)(FwExt_Regs_t* context, uint64_t code);
void fwext_DefaultInterruptHandler(FwExt_Regs_t* context, uint64_t code);

void fwext_setInterruptHandler( FwExt_InterruptHandler_t handler );
void fwext_setPUEAInterruptHandler( unsigned lane, FwExt_InterruptHandler_t handler );
int  fwext_mapPUEAInterruptLane( unsigned lane, unsigned threadMask, unsigned interruptType );
void fwext_takeMachineCheckHandlerFromFirmware( void );

typedef struct {
  volatile __attribute__((aligned(32))) uint64_t start;
  volatile __attribute__((aligned(32))) uint64_t count;
} FwExt_Barrier_t;

typedef struct FwExt_State_t {
  Firmware_Interface_t*       firmware;
  FwExt_Barrier_t             coreBarriers[NUM_CORES]; // Barriers used to synchronize the threads on a single core
  //Personality_t             PersonalityCopy;
  Personality_t*              personality;
  Firmware_DomainDescriptor_t domain;
  uint64_t                    vaddrLimit; // end of address space (virtual)
} FwExt_State_t;

extern FwExt_State_t FWExtState;

extern void fwext_barrier( FwExt_Barrier_t* barrier, unsigned numberOfThreads );
extern void fwext_coreBarrier( void );

static inline Firmware_Interface_t* fwext_getFwInterface( void ) { return FWExtState.firmware; }

static inline Personality_t* fwext_getPersonality() {
  if ( FWExtState.personality->Version != PERSONALITY_VERSION ) {
    printf("(E) Personality Version mismatch (actual %d vs %d expected).  Update, reconfigure and recompile.\n", FWExtState.personality->Version, PERSONALITY_VERSION );
    Terminate(__LINE__);
  }
  return FWExtState.personality; 
}

//extern int test_main( void );


#endif // Add nothing below this line.

