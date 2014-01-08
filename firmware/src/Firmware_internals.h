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

#ifndef __ASSEMBLY__

#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/dcr_support.h>
#include <hwi/include/common/compiler_support.h>
#include <firmware/include/personality.h>
#include <firmware/include/Firmware.h>
#include <firmware/include/mailbox.h>

#endif


/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * @   This header contains things that should be considered internal implementation     @
 * @   detail for firmware.  Any inclusion of this header from code outside of           @
 * @   firmware proper (including firmware extensions!) is highly discouraged.  All      @
 * @   code included herein is subject to change at any time.                            @
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 */

#ifndef FW_DUAL_DOMAIN
#define FW_SINGLE_DOMAIN 1
#endif

#ifdef FW_SINGLE_DOMAIN
#define FW_MAX_DOMAINS 1
#define FW_MSG_BOX_SIZE 0
#else
#define FW_MAX_DOMAINS 2 // We currently support only two domains
#define FW_MSG_BOX_SIZE 128
#endif

#ifndef __ASSEMBLY__

typedef struct FW_MessageBox_t {
  volatile unsigned char   from;
  volatile unsigned char   length;
  volatile unsigned char   busy;
  unsigned char            data[FW_MSG_BOX_SIZE];
} FW_MessageBox_t;

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

  struct _NodeState_T {
    Firmware_DomainDescriptor_t   domain[FW_MAX_DOMAINS];
    FW_MessageBox_t               messageBox[FW_MAX_DOMAINS];
    uint32_t                      coreMask;
  } nodeState;
} FW_InternalState_t;

#define FW_CORE_STATE_THD_ACTIVE(n)  (1<<(n))
#define FW_CORE_STATE_THD_ACTIVE_ALL ( FW_CORE_STATE_THD_ACTIVE(0) | FW_CORE_STATE_THD_ACTIVE(1) | FW_CORE_STATE_THD_ACTIVE(2) | FW_CORE_STATE_THD_ACTIVE(3) )
#define FW_ARRAY_SIZE(a) ( sizeof(a) / sizeof( a[0] ) )

extern FW_InternalState_t FW_InternalState;
extern Firmware_Interface_t FW_Interface;
extern Personality_t FW_Personality;

#define FW_PERSONALITY_PTR() &FW_Personality

//#define MEASURE_FW_BOOT_TIMES

#ifdef MEASURE_FW_BOOT_TIMES

typedef struct _FW_StopWatch_t {
    int count;
    uint64_t timestamps[40];
    uint16_t line[40];
    char* name[40];
} FW_StopWatch_t;

#define LAPTIME(sw,func) { sw.timestamps[sw.count] = GetTimeBase(); sw.name[sw.count] = #func; sw.line[sw.count++] = __LINE__; }

#define FW_INIT(condition,func)						\
    if ( (condition) ) {						\
	threadStatus[ProcessorID()] = __LINE__;				\
	rc = func();							\
    }									\
    if (rc) {								\
	FW_Error("FW: Initialization failed in %s.  rc=0x%x.", #func, rc); \
	crash(-__LINE__);						\
    } \
    if (ProcessorID()==0) LAPTIME(_fw_stopwatch,#func); \

#else

#define FW_INIT(condition,func)						\
    if ( (condition) ) {						\
	threadStatus[ProcessorID()] = __LINE__;				\
	rc = func();							\
    }									\
    if (rc) {								\
	FW_Error("FW: Initialization failed in %s.  rc=0x%x.", #func, rc); \
	crash(-__LINE__);						\
    }

#endif

/*
  if ( TESTINT_DCR_PRIV_PTR->ti_interrupt_state__state != 0 ) {	      \
    Terminate(__LINE__);					      \
  }								      \
*/

#define PERS_ENABLED(mask)  ( ( FW_Personality.Kernel_Config.NodeConfig & (mask) ) != 0 )
#define TRACE_ENABLED(mask) ( ( FW_Personality.Kernel_Config.TraceConfig & (mask) ) == (mask) )

#define TRACE_ENTRY(mask) if ( TRACE_ENABLED(mask) ) printf("(>) %s :%d\n", __func__, __LINE__);
#define TRACE_EXIT(mask)  if ( TRACE_ENABLED(mask) ) printf("(<) %s :%d\n", __func__, __LINE__);

#define _PERS_ENABLED(mask,flag)  ( ( (flag) & (mask) ) != 0 )
#define FW_TRACE(mask,message) if ( TRACE_ENABLED(mask) ) printf message ;


// A macro to enable DD1 workarounds. Workarounds are enabled if a) the JTAG_ID register indicates that
// this is DD1 hardware  or b) if the DD1Workarounds personality bit is set

#define FW_DD1_WORKAROUNDS_ENABLED() ( TI_isDD1() || PERS_ENABLED( PERS_ENABLE_DD1_Workarounds ) )

#define FW_IS_NODE_0 ( (FW_Personality.Network_Config.Acoord==0) && (FW_Personality.Network_Config.Bcoord==0) && (FW_Personality.Network_Config.Ccoord==0) && (FW_Personality.Network_Config.Dcoord==0) && (FW_Personality.Network_Config.Ecoord==0) )


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define FW_1GB (1024ull*1024ull*1024ull)

// Forward declarations of (internal) firmware calls.  These are to be considered as implementation
// detail and should not be called by any code that links the firmware image (e.g. firmware extension
// test cases).

extern void crash(int status);

extern uint64_t fw_getThisDomainID( void );

extern int fw_ddr_init( void );
extern int fw_ddr_flush_fifos( void );
extern void fw_ddr_unmaskCorrectableErrors( void );
extern void fw_ddr_flushCorrectables( int endOfJob );

extern int fw_bedram_init( void );
extern int fw_devbus_init( void );
extern int fw_envmon_init( void );
extern int fw_l1p_init( void );
extern void fw_l1p_unmaskCorrectableErrors( void );
extern void fw_l1p_flushCorrectables(int endOfJob );

extern int fw_l2_init( void );
extern int fw_l2_central_init( void );
extern int fw_l2_counter_init( void );
extern int fw_l2_preload( void );
extern void fw_l2_unmaskCorrectableErrors( void );
extern void fw_l2_flushCorrectables( int endOfJob );

extern int fw_ms_genct_init( void );
extern int fw_testint_init( void );
extern int fw_upc_init( void );
extern int fw_wu_init( void );
extern int fw_serdes_init( void );
extern int fw_sync_timebase( void );
extern int fw_A2_setTLBTable(Firmware_TLBEntries_t* mmu);

extern int fw_mailbox_init( unsigned core );
extern int fw_mailbox_init_on_core_0( void );
extern int fw_mailbox_load_via_jtag( void );
extern int fw_mailbox_put(const char*);
extern int fw_mailbox_puts(const char*);
extern int fw_mailbox_putn(const char*, unsigned);
extern int fw_mailbox_terminate(int status);
extern int fw_mailbox_writeRASEvent(fw_uint32_t, fw_uint16_t, fw_uint64_t*);
extern int fw_mailbox_writeRASString(fw_uint32_t, const char* );
extern void fw_mailbox_sendReadyMessage( void );
extern int fw_mailbox_perform_section_crc( MailBoxPayload_crcEntry_t* crc );
extern int fw_putn(const char* s, unsigned len );
extern int fw_mailbox_barrier( fw_uint64_t timeoutInMicroseconds, fw_uint64_t warningThresholdInMicroseconds ); 
extern int fw_mailbox_get(const char* s, unsigned);
extern int fw_mailbox_sendBlockStatus( fw_uint16_t, fw_uint16_t, fw_uint64_t [] );
extern void fw_mailbox_waitForQuiesced();
extern int fw_mailbox_pollInbox( void* message, fw_uint32_t* messageType, unsigned bufferLength );
extern int fw_mailbox_pollOutbox( void );

extern int fw_mu_resetRelease( void );
extern int fw_mu_termCheck( void );
extern void fw_mu_flushCorrectables( void );

extern int fw_nd_resetRelease( void );
extern int fw_nd_init_global_barrier( void );
extern int fw_nd_termCheck( void );
extern int fw_nd_machineCheckHandler( uint64_t status[] );
extern void fw_nd_flushCorrectables( void );

extern int  fw_pcie_init( void );
extern void fw_pcie_shutdown( void );

extern void fw_pdelay( uint64_t pclocks );
extern void fw_udelay( uint64_t microseconds );

extern void fw_semaphore_down( size_t lock_number );
extern int  fw_semaphore_down_w_timeout( size_t lock_number, uint64_t microseconds );
extern void fw_semaphore_up( size_t lock_number );
extern void fw_semaphore_init( size_t lock_number, uint32_t number_of_available_resources);

extern void fw_ticket_wait( size_t lock_ticketnum, size_t lock_nowserving );
extern int  fw_ticket_wait_w_timeout( size_t lock_ticketnum, size_t lock_nowserving, uint64_t microseconds );
extern void fw_ticket_post( size_t lock_nowserving );
extern void fw_core_barrier( void );

extern int fw_mmu_init( void );
extern int fw_fpu_init( void );

extern int fw_getPersonality( Personality_t* personality, unsigned size );
extern int fw_isIoNode( void );

extern int fw_writeRASEvent(fw_uint32_t message_id, fw_uint16_t number_of_details, fw_uint64_t details[]);
extern int fw_writeRASString(uint32_t fw_message_id, const char* message );

extern int  fw_interrupts_init( void );
extern int  fw_installInterruptVector( void* vec, unsigned size );
extern void fw_machineCheckHandler( void );
extern int  fw_installGeaHandler( int (*handler)(uint64_t status[]), uint64_t mask[] );

extern int fw_clockstop_init( void );
extern int fw_clockstop_increment( void );

extern int fw_dcr_arbiter_init( void );

extern int fw_doCycleReproReset( void );

unsigned short Crc16n( unsigned short usInitialCrc, unsigned char *pData, unsigned long ulLen );
unsigned long Crc32n( unsigned long ulInitialCrc, unsigned char *pData, unsigned long ulLen );

void fw_flushAllRasHistory( void );

extern uint8_t fw_stacks[];


__INLINE__  __NORETURN  void fw_sleep_forever( void ) {
  uint64_t mask = (1<<(ProcessorThreadID()));
  mtspr(SPRN_TENC,((mask) & 0xf));
  while(1);
}


#if 0
__INLINE__ void fw_tracedDcrWrite( unsigned traceFlags, unsigned dcr, uint64_t data ) {
  
  if ( TRACE_ENABLED(traceFlags) ) {
    printf( "DCR 0x%X write -> %016lX\n", dcr, data );
  }
  
  
  DCRWritePriv( dcr, data );
}
#else
#define fw_tracedDcrWrite( traceFlags,  dcr, data )   DCRWritePriv( dcr, data )
#endif

#define  fw_nd_print_network_personality(pers)  // stubbed out in firmware

#define FW_RAS(id) ( 0x00080000 | (id) )


//#define PCI_0001    RAS_CODE( RAS_CATEGORY_PCI, 0x0001 )   // PCI Express Fatal Error
//#define KERN_0001   RAS_CODE( RAS_CATEGORY_KERN, 0x0001 )  // Elf Section CRC error
//#define KERN_0002   RAS_CODE( RAS_CATEGORY_KERN, 0x0002 )  // Elf Section CRC warning




/* GEA Masks and Handlers */


#define FW_PREINSTALLED_GEA_HANDLERS 1

#define BEDRAM_GEA_MASK_0      0
#define BEDRAM_GEA_MASK_1      0
#define BEDRAM_GEA_MASK_2      GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__EDRAM_INT_set(1)

extern int fw_bedram_machineCheckHandler( uint64_t status[] );

#define CLOCKSTOP_GEA_MASK_0      0
#define CLOCKSTOP_GEA_MASK_1      0
#define CLOCKSTOP_GEA_MASK_2      GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__CS_INT_set(1) 

extern int fw_clockstop_machineCheckHandler( uint64_t status[] );


#define DCR_ARB_GEA_MASK_0      0
#define DCR_ARB_GEA_MASK_1      0
#define DCR_ARB_GEA_MASK_2      GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__DCR_ARB_INT_set(1)

extern int fw_dcr_arbiter_machineCheckHandler( uint64_t status[] );


#define DDR_GEA_MASK_0      GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__DDR0_RT_INT_set(1) | GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__DDR1_RT_INT_set(1)
#define DDR_GEA_MASK_1      0
#define DDR_GEA_MASK_2      0

extern int fw_ddr_machineCheckHandler( uint64_t status[] );


#define DEVBUS_GEA_MASK_0    0
#define DEVBUS_GEA_MASK_1    0
#define DEVBUS_GEA_MASK_2    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__DB_INT_set(1) 

extern int fw_devbus_machineCheckHandler( uint64_t status[] );


#define ENVMON_GEA_MASK_0 0
#define ENVMON_GEA_MASK_1 0
#define ENVMON_GEA_MASK_2 GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__EN_INT_set(1)

extern int fw_envmon_machineCheckHandler( uint64_t status[] );

#define GEA_GEA_MASK_0      0
#define GEA_GEA_MASK_1      0
#define GEA_GEA_MASK_2      GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__GEA_INT_set(1) 

extern int fw_gea_machineCheckHandler( uint64_t status[] );


#define L1P_GEA_MASK_0 \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P0_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P1_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P2_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P3_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P4_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P5_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P6_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P7_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P8_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P9_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P10_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P11_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P12_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P13_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P14_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P15_RT_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P16_RT_INT_set(1) | \
  0

#define L1P_GEA_MASK_1    0
#define L1P_GEA_MASK_2    0


extern int fw_l1p_machineCheckHandler( uint64_t status[] );




#define L2_GEA_MASK_0    0
#define L2_GEA_MASK_1    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_1__L2S_INT_set(0xFFFF)
#define L2_GEA_MASK_2    0

extern int fw_l2_machineCheckHandler( uint64_t status[] );


#define L2C_GEA_MASK_0    0
#define L2C_GEA_MASK_1    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_1__L2C_INT_set(1)
#define L2C_GEA_MASK_2    0

extern int fw_l2_central_machineCheckHandler( uint64_t status[] );


#define L2CTR_GEA_MASK_0    0
#define L2CTR_GEA_MASK_1    0
#define L2CTR_GEA_MASK_2 \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__L2CNTR0_INT_set(1) |    \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__L2CNTR1_INT_set(1) |    \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__L2CNTR2_INT_set(1) |    \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__L2CNTR3_INT_set(1) |    \
  0

extern int fw_l2_counter_machineCheckHandler( uint64_t status[] );

#define MSGC_GEA_MASK_0    0
#define MSGC_GEA_MASK_1    0
#define MSGC_GEA_MASK_2    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__MSGC_INT_set(1)

extern int fw_ms_genct_machineCheckHandler( uint64_t status[] );

#define MU_GEA_MASK_0      0
#define MU_GEA_MASK_1      0
#define MU_GEA_MASK_2      GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__MU_INT_set(1)

#define ND_GEA_MASK_0      0
#define ND_GEA_MASK_1      GEA_DCR__GEA_INTERRUPT_STATUS1__ND_INT_set(1)
#define ND_GEA_MASK_2      0 /*GEA_DCR__GEA_INTERRUPT_STATUS2__ND_INT_set(1)*/

extern int fw_mu_machineCheckHandler( uint64_t status[] );

#define PCIE_GEA_MASK_0    0
#define PCIE_GEA_MASK_1    0
#define PCIE_GEA_MASK_2    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__PCIE_INT_set(1)

extern int fw_pcie_machineCheckHandler( uint64_t status[] );

#define SERDES_GEA_MASK_0    0
#define SERDES_GEA_MASK_1    0
#define SERDES_GEA_MASK_2    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__SD0_INT_set(1) | \
  GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__SD1_INT_set(1) | \
  0

extern int fw_serdes_machineCheckHandler( uint64_t status[] );


#define TESTINT_GEA_MASK_0    0
#define TESTINT_GEA_MASK_1    0
#define TESTINT_GEA_MASK_2    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__TESTINT_INT_set(1)

extern int fw_testint_machineCheckHandler( uint64_t status[] );

#define UPC_GEA_MASK_0   0
#define UPC_GEA_MASK_1   0
#define UPC_GEA_MASK_2   GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__UPC_INT_set(1) 

extern int fw_upc_machineCheckHandler( uint64_t status[] );

#define WU_GEA_MASK_0  0
#define WU_GEA_MASK_1  0
#define WU_GEA_MASK_2  0

extern int fw_wu_machineCheckHandler( uint64_t status[] );


#endif

#endif
