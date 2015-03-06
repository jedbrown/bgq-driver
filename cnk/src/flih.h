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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#ifndef _FLIH_H_ // Prevent multiple inclusion.
#define _FLIH_H_

#include <hwi/include/common/compiler_support.h>

// Core and thread interrupt enablement masks
#define COREMSK(b)   ((1<<(31-(b)))) 
#define CORES_ALL     0xFFFF8000
#define THDMSK(b)    ((1<<(7-(b))))
#define THDS_ALL      0xF0 

#define MAPREG(b,x)  (((x) & (uint64_t)(0x3) ) << (63-(b)))

#define FLIH_PUEA_TABLE_SIZE 64
#define FLIH_GEA_NUM_STATUS_REGS 3
#define FLIH_GEA_NUM_MAP_REGS 10
#define FLIH_GEA_NUM_STATUS_BITS 64
#define FLIH_PUEA_GEA_START 41

// Special GEA Lane Assignments
#define GEA_NOCFG  1     // Source unit not configured. Should not see interrupts on this lane
#define GEA_MU     2     // Messaging unit 
#define GEA_ND     3     // ND software errors 
#define GEA_UPC    4     // UPC errors
#define GEA_SCRUB  6     // Background scrub
#define GEA_DCRV   7     // DCR violation
#define GEA_MBOX   8     // Mailbox input
#define GEA_PWRTH  9     // Power threshold warning, start reactive throttling
#define GEA_PWRMG 10     // Power management timer
#define GEA_CS    11     // Clockstop

// Bit position in PUEA Interrupt Summary register corresponding to a C2C from the 17th core
#define FLIH_C2C_CORE16 63

// Interrupt Level specification defined in hwi/include/bqc/BIC.h

// Interrupt Groups to control enablement/disablement
#define FLIH_GRP_BIC 1       // generic group 
#define FLIH_GRP_GEA 2       // generic group for interrupts from GEA  
#define FLIH_GRP_MSG 3       // Messaging Unit interrupts
#define FLIH_GRP_C2C 4       // Core to Core interrupts

typedef void (*INTHANDLER_Fcn_t)( int );
typedef void (*INTHANDLER_GEA_Fcn_t)( int, int );

#define NULL_INTHANDLER (INTHANDLER_Fcn_t)0
#define NULL_INTHANDLER_GEA (INTHANDLER_GEA_Fcn_t)0 

// Indicator for an entry reserved by hardware in the PUEA and GEA tables i.e. not configurable.
#define PUEA_RESERVED_HARDWARE 0,0,BIC_NO_INTERRUPT,0,-1,NULL_INTHANDLER
#define GEA_RESERVED_HARDWARE -1,-1,-1,NULL_INTHANDLER_GEA

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the PUEA mapping register in the current thread for all entries in the PUEA table
void puea_init();
// Initialize the GEA mapping registers for the node
void gea_init();
// Initialize the wakeup unit
void wakeup_init();
void wakeup_cleanup_agent();
// Reset an interrupt group back to its original interrupt delivery level mappings
void puea_ResetGroup(int interrupt_group);
// enable mu interrupt delivery to the GEA
void nd_interrupt_init();
// disable mu and nd interrupts at the PUEA
void nd_disablemund();
// disable the delivery of the GEA timer interrupt to the current hardware thread
void IPI_disableMessageChecker();
// enable the delivery of the GEA timer interrupt to the current hardware thread
void IPI_enableMessageChecker();
// check for IPI messages stuck in the message buffers
void IntHandler_IPIchecker();                                 // IPI checker driven from a UDECR interrupt
// flush any pending tool commands
void IntHandler_IPI_FlushToolCommands();


// First level Interrupt Handlers
void IntHandler_Default(int intrpt_sum_bitnum);               // Default PUEA interrupt handler
void IntHandler_WAC(int intrpt_sum_bitnum);                   // Wakeup Unit Address compare condition
void IntHandler_IPI_FLIH(int intrpt_sum_bitnum);              // IPI interrupt handler
void IntHandler_IPI_FLIH17(int intrpt_sum_bitnum);            // IPI interrupt handler for interrupts from 17th core
void IntHandler_GEA_FLIH(int intrpt_sum_bitnum);              // First level handler for Interrupts from the GEA 
void IntHandler_GEA_Default(int status_reg, int bitnum);      // Second level default handler for GEA
void IntHandler_RollbackSnapshot(int status_reg, int bitnum); // Rollback snapshot interrupt handler
void IntHandler_MU(int intrpt_sum_bitnum);                    // Handler for Messaging Unit interrupts
void IntHandler_PowerMgmt(int status_reg, int bitnum);        // Periodic Power management timer
void IntHandler_PowerEvent(int status_reg, int bitnum);       // Reactive Power management threshold exceeded
void IntHandler_ND(int status_reg, int bitnum);               // Handle soft errors in the ND
void IntHandler_MailboxIn(int status_reg, int bitnum);        // Handle Mailbox input received
void puea_set_mc_handler_thread(int hwthreadid);              // Set the thread that will handle machine checks
void IntHandler_DCRUserViolation(int status_Reg, int bitnum); // Handler for DCR access errors
void IntHandler_MemoryScrub(int status_Reg, int bitnum);
void IntHandler_GEATimerEvent(int status_Reg, int bitnum);
void IntHandler_GEA_UPC(int status_reg, int bitnum);          // Second level default handler for UPC parity error
void IntHandler_L1DCRV(int bitnum);


#ifdef __cplusplus
}
#endif

//
// Table of PUEA Interrupt characteristics 
// 
// Each entry in this table corresponds to the bit positions within the 
// PUEA Interrupt Summary Registers.
// 
// Columns in the table:
// LEVEL:  how should this interrupt be delivered, i.e. STD, CRIT, or MACH
// CORE:   Core mask indicating Which cores are enabled to receive this interrupt. This will
//         determine the settings for a particular interrupt in each PUEA
// THREAD: Which thread(s) are enabled to receive the interrupt. 0x80, 0x40, 0x20, 0x10 or (0xFF)=all
// CONFIG_ENABLE: indicator used to control when/if a particular class of interrupts is enabled/disabled
// HANDLER: which interrupt handler function should be invoked with this condition occurs
// BIT: indicates the bit position of this interrupt within the PUEA Interrupt Summary register
//
typedef struct PUEA_Table
                {
                uint32_t  core_enable;   // bit mask defining which cores are enabled to accept this interrupt
                uint8_t   thread_enable; // bit mask defining which threads are enabled. Bit 0=thdid 0
                uint8_t   level;         // disabled, standard, critical, machine check
                uint8_t   group;         // Use _BGP_PERS_GROUP_BIC  for anything always on
                int8_t    mapreg_offset; // Bit offset into the mapping register
                INTHANDLER_Fcn_t fcn;                  // Interrupt handler function                    
                }
                PUEA_Table;

                //
// Table of GEA Interrupt characteristics 
// 
// Each entry in this table corresponds to the bit positions within one of the three GEA Mapped Interrupt Status register.
// 
// Columns in the table:
// GEA_X: a number between 0 and 15 indicating which of the 16 possible GEA bit lines in the PUEA unit (bits 41:56) that will receive this event
// MAP REG,OFFSET: The mapping register and the bit offset within that mapping register that corresponds to this event
// 2nd LEVEL GEA HANDLER: the function to be called when the corresponding status bit is set
// NAME: name of the latch
// DESCRIPTION: text description of the event
//
typedef struct GEA_Table
                {
                int8_t  gea_bit_position; // which of the 16 possible GEA bit lines in the PUEA units that will receive this event
                int8_t  map_reg;  // The mapping register that corresponds to this event
                int8_t  map_offset; // The bit offset within the mapping register that corresponds to this event
                INTHANDLER_GEA_Fcn_t fcn; // Second Level Interrupt handler function for GEA interrupt sources                    

                }
                GEA_Table;



#endif
