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


#ifndef _FIRMWARE_INTERNALS_H
#define _FIRMWARE_INTERNALS_H

#include <firmware/include/fwext/fwext.h>

#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/dcr_support.h>
#include <hwi/include/common/compiler_support.h>
#include <firmware/include/personality.h>
//#include <firmware/include/Firmware.h>


#if 0

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * @   This header contains things that should be considered internal implementation     @
 * @   detail for firmware.  Any inclusion of this header from code outside of           @
 * @   firmware proper (including firmware extensions!) is highly discouraged.  All      @
 * @   code included herein is subject to change at any time.                            @
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 */


typedef struct FW_InternalState_t {

  struct _CoreState_T {

  /*
   * The flags field has the following format:
   *
   *   +------+----+----+----+----+
   *   | ///  | T3 | T2 | T1 | T0 |
   *   +------+----+----+----+----+
   *    0    3 4    5    6    7
   *
   * where
   *
   *   Tn - thread "n" is taken (i.e. has been handed off to
   *        the kernel).
   *
   */


    uint8_t flags;
    void*   entryPoint;
    void*   arg;
  } coreState[NUM_CORES];
} FW_InternalState_t;

#define FW_CORE_STATE_THD_ACTIVE(n)  (1<<(n))
#define FW_CORE_STATE_THD_ACTIVE_ALL ( FW_CORE_STATE_THD_ACTIVE(0) | FW_CORE_STATE_THD_ACTIVE(1) | FW_CORE_STATE_THD_ACTIVE(2) | FW_CORE_STATE_THD_ACTIVE(3) )



extern Personality_t FW_Personality;

#endif


//-#define FW_INIT(condition,func) if ( (condition) ) rc = func(); if(rc) { printf("FW: Initialization failed in %s.  rc=0x%x\n", #func, rc); crash(-1); }


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#if 0

// Forward declarations of (internal) firmware calls.  These are to be considered as implementation
// detail and should not be called by any code that links the firmware image (e.g. firmware extension
// test cases).

extern void crash(int status);

extern int fw_ddr_init( void );

extern int fw_mailbox_init( void );
extern int fw_mailbox_load_via_jtag( void );
extern int fw_mailbox_put(const char*);
extern int fw_mailbox_puts(const char*);
extern int fw_mailbox_putn(const char*, unsigned);
extern int fw_mailbox_terminate(int status);
extern int fw_mailbox_writeRASEvent(uint32_t, uint16_t, uint64_t*);
extern int fw_putn(const char* s, size_t len );

extern int fw_mu_reset_release( void );
extern int fw_mu_term_check( void );

extern int fw_nd_reset_release( void );
extern int fw_nd_term_check( void );

extern int fw_pcie_init( void );

extern int fw_sim_put(const char*);
extern int fw_sim_puts(const char*);
extern int fw_sim_putn(const char*, unsigned);
extern int fw_sim_writeRASEvent(uint32_t, uint16_t, uint64_t*);

extern void fw_semaphore_down( size_t lock_number );
extern int  fw_semaphore_down_w_timeout( size_t lock_number, uint64_t microseconds );
extern void fw_semaphore_up( size_t lock_number );
extern void fw_semaphore_init( size_t lock_number, uint32_t number_of_available_resources);

extern void fw_ticket_wait( size_t lock_ticketnum, size_t lock_nowserving );
extern int  fw_ticket_wait_w_timeout( size_t lock_ticketnum, size_t lock_nowserving, uint64_t microseconds );
extern void fw_ticket_post( size_t lock_nowserving );

extern int fw_mmu_init( void );
extern int fw_fpu_init( void );

extern int fw_getPersonality( Personality_t* personality, unsigned size );
extern int fw_isIoNode( void );

__INLINE__  __NORETURN  void fw_sleep_forever( void ) {
  uint64_t mask = (1<<(ProcessorThreadID()));
  mtspr(SPRN_TENC,((mask) & 0xf));
  while(1);
}

#endif


extern FwExt_State_t  FWExtState;
#define FW_PERSONALITY_PTR() FWExtState.personality

#define PERS_ENABLED(mask)  ( ( FWExtState.personality->Kernel_Config.NodeConfig & (mask) ) != 0 )
#define TRACE_ENABLED(mask) ( ( FWExtState.personality->Kernel_Config.TraceConfig & (mask) ) == (mask) )

#define FW_TRACE(mask,message) if ( TRACE_ENABLED(mask) ) printf message ;

__INLINE__ void fw_tracedDcrWrite( unsigned traceFlags, unsigned dcr, uint64_t data ) {
  
  if ( TRACE_ENABLED(traceFlags) ) {
    printf( "DCR 0x%X write -> %016lX\n", dcr, data );
  }
  
  
  DCRWritePriv( dcr, data );
}

extern  int  fw_nd_print_network_personality(Personality_t *p);

void FW_Warning( const char* fmt, ... );


#define TRACE_ENTRY(x)
#define TRACE_EXIT(x)

#define FW_PREINSTALLED_GEA_HANDLERS 1

#define MU_GEA_MASK_0      0
#define MU_GEA_MASK_1      0
#define MU_GEA_MASK_2      GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__MU_INT_set(1)
extern int fw_mu_machineCheckHandler( uint64_t status[] );

#endif
