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

#ifndef _FIRMWARE_H_ // Prevent multiple inclusion
#define _FIRMWARE_H_

#ifndef __KERNEL__
#define __KERNEL__
#endif

#define NUM_CORES          (17)
#define THREADS_PER_CORE   (4)
#define NUM_HW_THREADS     (THREADS_PER_CORE * NUM_CORES)

#define FW_STACK_SIZE          (1024)

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/asm_support.h>
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/common/bgq_alignment.h>

#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/VirtualMap.h>

#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/BeDram.h>

#include <firmware/include/Firmware_Linkage.h>
#include <firmware/include/Firmware_SimSupport.h>


#define FWEXT_DERAT_INDX_FIRST 4
#define FWEXT_DERAT_INDX_COUNT 4
#define FWEXT_DERAT_INDX_LAST  (FWEXT_DERAT_INDX_FIRST + FWEXT_DERAT_INDX_COUNT - 1)


#if !defined(__ASSEMBLY__)

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/BeDram_inlines.h>
#include <firmware/include/personality.h>

typedef struct FW_State_t
                {
                uint8_t  fw_Core_Ready;  // Flag to barrier non-init threads until the per-core work is done
                uint8_t  fw_BQC_Ready;   // Flag to barrier non-init threads until the per-node work is done by 0:0
                uint16_t fw_Pad0;        // Place Holder
                uint32_t fw_ResetCount;  // Increments on each pass through reset
                uint32_t fw_CRC;         // 32bit CRC of Firmware image
                }
                FW_State_t;


extern Personality_t FW_Personality;

extern int DDR_Initialize( void );
extern int Network_Initialize( void );
extern void SoftReset( void );

// see firmware/src/main.c or a firmware-extension testcase.
extern int main( int argc, char *argv[], char **envp );

extern int fw_writeRASEvent(fw_uint32_t message_id, fw_uint16_t number_of_details, fw_uint64_t details[]);

#endif // __ASSEMBLY__



#endif // Add nothing below this line.
