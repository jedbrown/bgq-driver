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

#ifndef _FIRMWARE_LINKAGE_H_ // Prevent multiple inclusion.
#define _FIRMWARE_LINKAGE_H_

//
// Define the linkage and segmentation for the Firmware
//

#if defined(__ASSEMBLY__)

// 4 bytes for a single instruction right at 0xFFFFFFFC
#define _PROLOG_RESET_VECTOR(fcn) \
         .section ".fw_resetvector","ax" ; \
         .type fcn,@function ; \
         .global fcn;

// Boot Stacks section
#define _DPROLOG_STACK(dat,a) \
         .section ".fw_stacks","aw" ; \
         .type dat,@object ; \
         .align a ; \
         .global dat

// 12 bytes abutted to .text.resetvector above
#define _DPROLOG_FW_STATE(dat) \
         .section ".fw_state","aw" ; \
         .type dat,@object ; \
         .global dat

// Reset Page: Text area of the 4KB page.
#define _PROLOG_RESET(fcn) \
         .section ".fw_text.reset","ax" ; \
         .align ALIGN_L1I_CACHE; \
         .type fcn,@function ; \
         .global fcn;

// Firmware Interrupt Vectors
#define _PROLOG_VECTORS(fcn) \
         .section ".fw_vectors","ax" ; \
         .align 12; \

// Reset Page: Data area of the 4KB page.
#define _DPROLOG_RESET(dat,a) \
         .section ".fw_data.reset","aw" ; \
         .type dat,@object ; \
         .align a ; \
         .global dat

#define _EPILOG(fcn) \
         .size fcn,.-fcn ; \
         .previous

#define _DEPILOG(dat) \
         .size dat,.-dat ;

#else  // not __ASSEMBLY__


#define FW_TEXTRESET       __attribute__ ((__section__ (".text.reset")))
#define FW_DATARESET       __attribute__ ((__section__ (".data.reset")))
#define FW_MAILBOX_TO_CORE __attribute__ ((__section__ (".MailboxToCore")))
#define FW_MAILBOX_TO_HOST __attribute__ ((__section__ (".MailboxToHost")))
#define FW_PERSONALITY     __attribute__ ((__section__ (".fw_personality")))
#define FW_LOCKS           __attribute__ ((__section__ (".fw_locks")))
#define FW_STACK0          __attribute__ ((__section__ (".fw_stack0")))
#define FW_STACKS          __attribute__ ((__section__ (".fw_stacks")))
#define FW_STATE           __attribute__ ((__section__ (".fw_state")))
#define FW_SUART           __attribute__ ((__section__ (".fw_suart")))
#define FW_TERMSTATUS      __attribute__ ((__section__ (".fw_termstatus")))
#define FW_TLB_TABLE       __attribute__ ((__section__ (".tlb_table")))



#endif // __ASSEMBLY__

#endif // Add nothing below this line.
