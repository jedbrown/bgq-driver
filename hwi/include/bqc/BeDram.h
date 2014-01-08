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
#ifndef _BEDRAM_H_ // Prevent multiple inclusion.
#define _BEDRAM_H_

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/PhysicalMap.h>

__BEGIN_DECLS

// As of BeDRAM_HLD_1.5.doc, 3/31/2008
//
// BeDRAM access with bit 42 zeroed indicates atomic address range.
//
// BeDRAM Lock Addressing: Bits 43 to 57 of the physical address selects the 8Byte lock number.
//
// Understanding BeDRAM Lock addressing:
//    0x03ff_c000_0000: 1GB ERAT by Firmware (Guarded, Inhibited, Supervisor RWX)
//    0x03ff_fffc_0000: BeDRAM Starting Virtual Address within that ERAT for Normal Access
//    0x0000_0020_0000: For locks, addr bit 42 must be clear, ie, (0x03ff_fffc_0000 & ~0x0000_0020_0000)
//    0x03ff_ffdc_0000: Therefore, start of BeDRAM Locks Access which overlays the eDRAM memory
//
// Note: Mambo uses certain BeDRAM addresses for UARTS so be careful!
//

#if (BEDRAM_VERSION == 16)

#define BeDRAM_LockOffset(lknum)  _B15(57,(lknum))        // Lock Number: addr[43:57]
#define BeDRAM_HWTIndex()         (0)

#else

#define BeDRAM_LockOffset(lknum)  _B15(55,(lknum))            // Lock Number: addr[41:55]
#define BeDRAM_HWTIndex()         _B2(57, ProcessorThreadID()) // HWT index [56:57]

#endif
#define BeDRAM_OpCodes(x)         _B3(60,x)               // OpCode Field: addr[58:60]

//
// BeDRAM Atomic Operation codes.
//
#if defined(__KERNEL__)

// operations available only to the privileged memory image

#define  BeDRAM_WRITERAW           BeDRAM_OpCodes(6)   // Write without ECC correction  (priv area only)
#define  BeDRAM_READRAW            BeDRAM_OpCodes(6)   // Read without ECC correction   (priv area only)

#define  BeDRAM_WRITEECC           BeDRAM_OpCodes(7)   // Write into ECC Area of memory (priv area only)
#define  BeDRAM_READECC            BeDRAM_OpCodes(7)   // Read ECC correction code      (priv area only)

#endif // __KERNEL__

// operations available to the privileged or non-privileged memory image

#define  BeDRAM_WRITE              BeDRAM_OpCodes(0)   // Normal Write
#define  BeDRAM_READ               BeDRAM_OpCodes(0)   // Normal Read

#define  BeDRAM_WRITEADD           BeDRAM_OpCodes(1)   // Write with Add
#define  BeDRAM_READCLEAR          BeDRAM_OpCodes(1)   // Read then Clear

#define  BeDRAM_READINCSAT         BeDRAM_OpCodes(2)   // Read then Increment with Saturation
#define  BeDRAM_READINCWRAP        BeDRAM_OpCodes(3)   // Read then Increment with Wrap
#define  BeDRAM_READDECSAT         BeDRAM_OpCodes(4)   // Read then Decrement with Saturation
#define  BeDRAM_READDECWRAP        BeDRAM_OpCodes(5)   // Read then Decrement with Wrap


__END_DECLS

#endif // Add nothing below this line.
