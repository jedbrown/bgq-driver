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
#ifndef __BQL_TDR_H__
#define __BQL_TDR_H__

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/testint_tdr.h>

#define BQL_ONES                  (0xFFFFFFFFFFFFFFFFULL)
#define BQL_ZEROES                (0x0000000000000000ULL)

///////////////////////////////////////////////////////////////////
// JTAG definitions for BQL

// Top level JTAG controller.
// Following defined in https://w3.rchland.ibm.com/afs/rchland.ibm.com/rel/common/proj/bluelinkq/dd1.0/fep/bts/bql.speclist
// Note: while physically a 32-bit IR, for software commonality with BQC chip we pretend that it is a 40-bit IR

#define BQL_JTAG_BYPASS                 (0xFFFFFFFFFFLL)      // Bypass         (required IEEE1149.1 boundary scan instruction)
#define BQL_JTAG_EXTEST                 (0x0000000000LL)      // Extest         (IEEE1149.1 boundary scan instruction)
#define BQL_JTAG_SAMPLE                 (0x0900000100LL)      // Sample         (IEEE1149.1 boundary scan instruction)
#define BQL_JTAG_PRELOAD                (0x0A00000100LL)      // Preload        (IEEE1149.1 boundary scan instruction)
#define BQL_JTAG_HIGHZ                  (0x0700000100LL)      // High-Z         (IEEE1149.1 boundary scan instruction)
#define BQL_JTAG_CLAMP                  (0x0800000100LL)      // CLAMP          (IEEE1149.1 boundary scan instruction)
#define BQL_JTAG_EXTEST_PULSE           (0x0200000100LL)      // Extest-Pulse   (IEEE1149.6 boundary scan instruction)
#define BQL_JTAG_EXTEST_TRAIN           (0x0400000100LL)      // Extest-Train   (IEEE1149.6 boundary scan instruction)
#define BQL_JTAG_ID_CODE                (0x0F00010000LL)      // IDcode         (IEEE1149.6 boundary scan instruction)
#define BQL_JTAG_ECID                   (0x0100000100LL)      // Select ECID TDR

#define BQL_JTAG_SEL_ECID               (0x01000001LL << 8)   // redundant with above definition; pre shift left to compensate for the 40 bit IR
#define BQL_JTAG_SEL_IDCODE             (0x0F000100LL << 8)   // redundant with above definition; pre shift left to compensate for the 40 bit IR

// BQL Access OpCodes
#define BQL_JTAG_ACC_OP_SCAN            (0x0F00000000LL)           // scan in, no error checking
#define BQL_JTAG_ACC_OP_SCAN_IN         (0x1100000000LL)           // scan-in,  with input checking.
#define BQL_JTAG_ACC_OP_SCAN_OUT        (0x1200000000LL)           // scan-out, with checking; read-only
#define BQL_JTAG_ACC_OP_SCOM_READ_I     (0x1400000000LL)           // scan communications read immediate
#define BQL_JTAG_ACC_OP_SCOM_READ       (0x1700000000LL)           // scan communications read
#define BQL_JTAG_ACC_OP_SCOM_WRITE      (0x1800000000LL)           // scan communications write.
#define BQL_JTAG_ACC_OP_SCOM_RESET      (0x1B00000000LL)           // scan communications reset

    // Access Modifiers to select BQL Access-internal TDRs. Note: BQL modifiers need ODD parity in bit (1<<8)
    // Used along with:
    //     BQL_JTAG_ACC_OP_SCAN
    //     BQL_JTAG_ACC_OP_SCAN_IN
    //     BQL_JTAG_ACC_OP_SCAN_OUT
#define BQL_JTAG_SEL_ACCESS_STATUS      (0x0000000200LL)       // R/W Access Status Register          0:31
#define BQL_JTAG_SEL_ACCESS_OPTIONS     (0x0000000400LL)       // R/W Access Options Register         0:31
#define BQL_JTAG_SEL_ACCESS_BYP         (0x0000000800LL)       //     Access macro 1 bit bypass.      0:0
#define BQL_JTAG_SEL_ACCESS_CRCR        (0x0000002000LL)       // R   Access macro CRC register.      0:31
#define BQL_JTAG_SEL_ACCESS_SCSCAN      (0x0000004000LL)       // R/W Access macro SCOM Scan register 0:63
#define BQL_JTAG_SEL_ACCESS_SCSTAT      (0x0000008000LL)       // R   Access Macro SCOM status.       0:31
#define BQL_JTAG_SEL_ACCESS_ID          (0x0000010000LL)       // R   Access Macro internal ID code   0:31
#define BQL_JTAG_SEL_ACCESS_ALEV        (0x0000020000LL)       // R/W Access EC Level Constant        0:31
#define BQL_JTAG_SEL_ACCESS_LBISTC      (0x0000040000LL)       //     LBIST channel. Caution: destructive read. Only used for LBIST debug
#define BQL_JTAG_SEL_ACCESS_SIRP        (0x0000080000LL)       // R   Access Shadow instruction register. 0:31
#define BQL_JTAG_SEL_ACCESS_GP1         (0x0000100000LL)       // R/W Access GP1_REG         0:63
#define BQL_JTAG_SEL_ACCESS_GP2         (0x0000200000LL)       // R/W Access GP2_REG         0:63
#define BQL_JTAG_SEL_ACCESS_GP3         (0x0000300100LL)       // R/W Access GP3_REG         0:63
#define BQL_JTAG_SEL_ACCESS_GP4         (0x0000400000LL)       // R/W Access GP4_REG         0:63
#define BQL_JTAG_SEL_ACCESS_GP5         (0x0000500100LL)       // R/W Access GP5_REG         0:63
#define BQL_JTAG_SEL_ACCESS_GP_STAT     (0x0008000000LL)       // R   Access GP_STATUS_REG   0:63

// BQL JTAG_ID, ACCESS_ID and ACCESS_ALEV register hardcoded read-back values
#define BQL_JTAG_IDCODE_VALUE_DD1         (0x01066049LL)       // BQL chip DD1.0 IEEE1149.1 IDcode
#define BQL_JTAG_ACCESS_ID_VALUE          (0x01066049LL)       // Access_ID = copy of BQL chip DD1.0 IEEE1149.1 IDcode
#define BQL_JTAG_ACCESS_ALEV_VALUE        (0x6100FF00LL)       // BQL chip DD1.0 Access Level (though actually Access 6.2 !!)

#define BQL_JSEL_NONE    5
#define BQL_JSEL_ACCESS  6

// TDRs connected directly off the Master JTAG controller
#define BQL_TDR_MAST_JTAGID        BGQ_JTDRDEF(BQL_JSEL_NONE,   32,  BQL_JTAG_SEL_IDCODE)  // R/O Master JTAG IDcode
#define BQL_TDR_MAST_ECID          BGQ_JTDRDEF(BQL_JSEL_NONE,  129,  BQL_JTAG_SEL_ECID )   // R/O ECID code: 128 bits+fuse_done

// TDRs connected off the ACCESS child controller
#define BQL_TDR_ACCESS_STATUS      BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_STATUS  )  // R/W Access Status Register          0:31
#define BQL_TDR_ACCESS_OPTIONS     BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_OPTIONS )  // R/W Access Options Register         0:31
#define BQL_TDR_ACCESS_BYP         BGQ_JTDRDEF(BQL_JSEL_ACCESS,  1, BQL_JTAG_SEL_ACCESS_BYP     )  //     Access macro 1 bit bypass.      0:0
#define BQL_TDR_ACCESS_CRCR        BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_CRCR    )  // R   Access macro CRC register.      0:31
#define BQL_TDR_ACCESS_SCSCAN      BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, BQL_JTAG_SEL_ACCESS_SCSCAN  )  // R/W Access macro SCOM Scan register 0:63
#define BQL_TDR_ACCESS_SCSTAT      BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_SCSTAT  )  // R   Access Macro SCOM status.       0:31
#define BQL_TDR_ACCESS_ID          BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_ID      )  // R   Access Macro internal ID code   0:31
#define BQL_TDR_ACCESS_ALEV        BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_ALEV    )  // R/W Access EC Level Constant        0:31
#define BQL_TDR_ACCESS_LBISTC      BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_LBISTC  )  //     LBIST channel. Caution: destructive read. Only used for LBIST debug
#define BQL_TDR_ACCESS_SIRP        BGQ_JTDRDEF(BQL_JSEL_ACCESS, 32, BQL_JTAG_SEL_ACCESS_SIRP    )  // R   Access Shadow instruction register. 0:31
#define BQL_TDR_ACCESS_GP1         BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, BQL_JTAG_SEL_ACCESS_GP1     )  // R/W Access GP1_REG         0:63
#define BQL_TDR_ACCESS_GP2         BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, BQL_JTAG_SEL_ACCESS_GP2     )  // R/W Access GP2_REG         0:63
#define BQL_TDR_ACCESS_GP3         BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, BQL_JTAG_SEL_ACCESS_GP3     )  // R/W Access GP3_REG         0:63
#define BQL_TDR_ACCESS_GP4         BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, BQL_JTAG_SEL_ACCESS_GP4     )  // R/W Access GP4_REG         0:63
#define BQL_TDR_ACCESS_GP5         BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, BQL_JTAG_SEL_ACCESS_GP5     )  // R/W Access GP5_REG         0:63
#define BQL_TDR_ACCESS_GP_STAT     BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, BQL_JTAG_SEL_ACCESS_GP_STAT )  // R/W Access GP_STATUS_REG   0:63

// Access IR modifiers to select eTDRs  (TDRs external to access macro).
#define BGQ_ETDR_BQL(s,r)  BGQ_JTDRDEF(BQL_JSEL_ACCESS, 64, ((uint64_t)r<<20))

// Aliases
#define BQL_ETDR_PLL_Control       BQL_TDR_ACCESS_GP1    // R/W PLL Control     0:63
#define BQL_ETDR_Deskew_Control    BQL_TDR_ACCESS_GP3    // R/W Deskew Control  0:63
#define BQL_ETDR_Fuse_POR          BQL_TDR_ACCESS_GP4    // R/W Fuse POR        0:63
#define BQL_ETDR_HSS_Clock         BQL_TDR_ACCESS_GP5    // R/W HSS Clock       0:63

// Deprecated aliases
#define BQL_JTAG_ACCESS_EC               BQL_JTAG_SEL_ACCESS_ALEV
#define BGQ_JTAG_ACCESS_EC_VALUE_BQL_DD1 BQL_JTAG_ACCESS_ALEV_VALUE

///////////////////////////////////////////////////////////////
/////
//  BQL SCOM definitions

// encoder
// #define BGQ_SCOMSEL(x)     ((r(((uint64_t)(x))&0x0f)) << (64-4))
#define BQL_SCOMREG(x)     (((uint64_t)(x)) & 0xFFFFFF)

// decoder
// #define _BGQ_SCOMSEL(x)  (((x) >> (64-4)) & 0x0f)
#define _BQL_SCOMREG(x)  ((x) & 0xFFFFFF)

// SCOM address definition
// r -- register (JTAG IR modifier bits - the diag lib will << 1 these values to make room for the parity bit)
#define BQL_SCOMDEF(r)        (BGQ_SCOMSEL(BQL_JSEL_ACCESS) | BQL_SCOMREG(r))

// Access IR Modifiers to select scom registers.  (All BQL scom registers are 64 bits)
// BQL LBIST register addresses.
// Note: these SCOM addresses are the 23 bit IR modifier bits. The spec lists these shifted 1 bit to the left
// with odd parity included. The values are constructed below to make it easier to reference the BQL LBIST spec.
#define BGQ_TI_SCOM_BQL_LBIST_OPTIONS          BQL_SCOMDEF(0x840002 >> 1)     // R/W LBIST options register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR0            BQL_SCOMDEF(0x840004 >> 1)     // R/W LBIST MISR0 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR1            BQL_SCOMDEF(0x840015 >> 1)     // R/W LBIST MISR1 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR2            BQL_SCOMDEF(0x840025 >> 1)     // R/W LBIST MISR2 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR3            BQL_SCOMDEF(0x840034 >> 1)     // R/W LBIST MISR3 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR4            BQL_SCOMDEF(0x840045 >> 1)     // R/W LBIST MISR4 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR5            BQL_SCOMDEF(0x840054 >> 1)     // R/W LBIST MISR5 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR6            BQL_SCOMDEF(0x840064 >> 1)     // R/W LBIST MISR6 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR7            BQL_SCOMDEF(0x840075 >> 1)     // R/W LBIST MISR7 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR8            BQL_SCOMDEF(0x840085 >> 1)     // R/W LBIST MISR8 register.
#define BGQ_TI_SCOM_BQL_LBIST_MISR9            BQL_SCOMDEF(0x840094 >> 1)     // R/W LBIST MISR9 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG0            BQL_SCOMDEF(0x840007 >> 1)     // R/W LBIST PRPG0 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG1            BQL_SCOMDEF(0x840016 >> 1)     // R/W LBIST PRPG1 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG2            BQL_SCOMDEF(0x840026 >> 1)     // R/W LBIST PRPG2 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG3            BQL_SCOMDEF(0x840037 >> 1)     // R/W LBIST PRPG3 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG4            BQL_SCOMDEF(0x840046 >> 1)     // R/W LBIST PRPG4 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG5            BQL_SCOMDEF(0x840057 >> 1)     // R/W LBIST PRPG5 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG6            BQL_SCOMDEF(0x840067 >> 1)     // R/W LBIST PRPG6 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG7            BQL_SCOMDEF(0x840076 >> 1)     // R/W LBIST PRPG7 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG8            BQL_SCOMDEF(0x840086 >> 1)     // R/W LBIST PRPG8 register.
#define BGQ_TI_SCOM_BQL_LBIST_PRPG9            BQL_SCOMDEF(0x840097 >> 1)     // R/W LBIST PRPG9 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_LENGTH   BQL_SCOMDEF(0x840008 >> 1)     // R/W LBIST channel length register.
#define BGQ_TI_SCOM_BQL_LBIST_TEST_LENGTH      BQL_SCOMDEF(0x84000B >> 1)     // R/W LBIST test length register.
#define BGQ_TI_SCOM_BQL_LBIST_STATUS           BQL_SCOMDEF(0x84000D >> 1)     // R/W LBIST status register.
#define BGQ_TI_SCOM_BQL_LBIST_START            BQL_SCOMDEF(0x84000E >> 1)     //   W Run LBIST register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK0    BQL_SCOMDEF(0x840105 >> 1)     // R/W Channel Mask 0 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK1    BQL_SCOMDEF(0x840114 >> 1)     // R/W Channel Mask 1 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK2    BQL_SCOMDEF(0x840124 >> 1)     // R/W Channel Mask 2 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK3    BQL_SCOMDEF(0x840135 >> 1)     // R/W Channel Mask 3 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK4    BQL_SCOMDEF(0x840144 >> 1)     // R/W Channel Mask 4 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK5    BQL_SCOMDEF(0x840155 >> 1)     // R/W Channel Mask 5 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK6    BQL_SCOMDEF(0x840165 >> 1)     // R/W Channel Mask 6 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK7    BQL_SCOMDEF(0x840174 >> 1)     // R/W Channel Mask 7 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK8    BQL_SCOMDEF(0x840184 >> 1)     // R/W Channel Mask 8 register.
#define BGQ_TI_SCOM_BQL_LBIST_CHANNEL_MASK9    BQL_SCOMDEF(0x840195 >> 1)     // R/W Channel Mask 9 register.

//
// Access IR Modifiers to select scom registers.  (All BQL scom registers are 64 bits)
// clock interface register  /afs/rchland.ibm.com/rel/common/proj/clocktree/Doc/workbook_cu65/ClkIntf.fm
// Note: these SCOM addresses are the 23 bit IR modifier bits. The spec lists these shifted 1 bit to the left with odd parity included.
#define BGQ_TI_SCOM_BQL_CLK_CMD                BQL_SCOMDEF(0x400000)     // R/W clock command Register
#define BGQ_TI_SCOM_BQL_CLK_STATUS             BQL_SCOMDEF(0x400001)     // R/W clock status Register
#define BGQ_TI_SCOM_BQL_CLK_OPTION1            BQL_SCOMDEF(0x400002)     // R/W clock Options Register #1
#define BGQ_TI_SCOM_BQL_CLK_OPTION2            BQL_SCOMDEF(0x400003)     // R/W clock Options Register #2
#define BGQ_TI_SCOM_BQL_CLK_OPTION3            BQL_SCOMDEF(0x400004)     // R/W clock Options Register #3
#define BGQ_TI_SCOM_BQL_CLK_STATUS_MASK        BQL_SCOMDEF(0x400006)     // R/W clock Status Reg Mask
#define BGQ_TI_SCOM_BQL_CLK_MISC1              BQL_SCOMDEF(0x400007)     // R/W clock Misc1 Register
#define BGQ_TI_SCOM_BQL_CLK_MISC2              BQL_SCOMDEF(0x400008)     // R/W clock Misc2 Register
#define BGQ_TI_SCOM_BQL_CLK_PLL0_LOCK          BQL_SCOMDEF(0x400009)     // R   clock read (only) PLL0 Observe bits & PLL0 Lock
#define BGQ_TI_SCOM_BQL_CLK_PLL1_LOCK          BQL_SCOMDEF(0x40000a)     // R   clock read (only) PLL1 Observe bits & PLL1 Lock
#define BGQ_TI_SCOM_BQL_CLK_PLL2_LOCK          BQL_SCOMDEF(0x40000b)     // R   clock read (only) PLL2 Observe bits & PLL2 Lock
#define BGQ_TI_SCOM_BQL_CLK_PLL3_LOCK          BQL_SCOMDEF(0x40000c)     // R   clock read (only) PLL3 Observe bits & PLL3 Lock
#define BGQ_TI_SCOM_BQL_CLK_PLL4_LOCK          BQL_SCOMDEF(0x40000d)     // R   clock read (only) PLL4 Observe bits & PLL4 Lock
#define BGQ_TI_SCOM_BQL_CLK_PLL5_LOCK          BQL_SCOMDEF(0x40000e)     // R   clock read (only) PLL5 Observe bits & PLL5 Lock
#define BGQ_TI_SCOM_BQL_CLK_PLL6_LOCK          BQL_SCOMDEF(0x40000f)     // R   clock read (only) PLL6 Observe bits & PLL6 Lock
#define BGQ_TI_SCOM_BQL_CLK_PLL7_LOCK          BQL_SCOMDEF(0x400010)     // R   clock read (only) PLL7 Observe bits & PLL7 Lock
#define BGQ_TI_SCOM_BQL_CLK_TEST_CTRL_SET      BQL_SCOMDEF(0x400011)     // R/W clock Test Control Register (Read/Set address)
#define BGQ_TI_SCOM_BQL_CLK_TEST_CTRL_CLEAR    BQL_SCOMDEF(0x400012)     // R/W clock Test Control Register (Clear address)
#define BGQ_TI_SCOM_BQL_CLK_TEST_CTRL_DIS      BQL_SCOMDEF(0x400013)     // R/W clock Test Control Disable Register
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK_LOAD        BQL_SCOMDEF(0x400018)     //   W clock TWGDSK Control Registers Load System Settings (Write only)
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK0_SET        BQL_SCOMDEF(0x400019)     // R/W clock TWGDSK Control Register 0 (Read/Set address)
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK0_CLEAR      BQL_SCOMDEF(0x40001a)     // R/W clock TWGDSK Control Register 0 (Clear address)
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK1_SET        BQL_SCOMDEF(0x40001b)     // R/W clock TWGDSK Control Register 1 (Read/Set address)
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK1_CLEAR      BQL_SCOMDEF(0x40001c)     // R/W clock TWGDSK Control Register 1 (Clear address)
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK2_SET        BQL_SCOMDEF(0x40001d)     // R/W clock TWGDSK Control Register 2 (Read/Set address)
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK2_CLEAR      BQL_SCOMDEF(0x40001e)     // R/W clock TWGDSK Control Register 2 (Clear address
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK3_SET        BQL_SCOMDEF(0x40001f)     // R/W clock TWGDSK Control Register 3 (Read/Set address)
#define BGQ_TI_SCOM_BQL_CLK_TWGDSK3_CLEAR      BQL_SCOMDEF(0x400020)     // R/W clock TWGDSK Control Register 3 (Clear address

// Switch Configuration Registers (unit 0x01)
#define BGQ_TI_SCOM_BQL_SWITCH_CNTL            BQL_SCOMDEF(0x001000)     // R/W Switch Routing Control
#define BQL_INIT_SWITCH_CNTL                   (0x0000000000000000ULL)
#define BQL_MASK_SWITCH_CNTL                   (0x8000800080008000ULL)

// Sparing Registers (unit 0x02)
#define BGQ_TI_SCOM_BQL_SPARE_CNTL_C_SW01      BQL_SCOMDEF(0x002000)     // R/W Sparing Configuration C01
#define BGQ_TI_SCOM_BQL_SPARE_CNTL_D_SW01      BQL_SCOMDEF(0x002001)     // R/W Sparing Configuration D01
#define BGQ_TI_SCOM_BQL_SPARE_CNTL_C_SW23      BQL_SCOMDEF(0x002010)     // R/W Sparing Configuration C23
#define BGQ_TI_SCOM_BQL_SPARE_CNTL_D_SW23      BQL_SCOMDEF(0x002011)     // R/W Sparing Configuration D23
#define BGQ_TI_SCOM_BQL_SPARE_STATUS_SW01      BQL_SCOMDEF(0x00200A)     // R   Sparing Status SW01
#define BGQ_TI_SCOM_BQL_SPARE_STATUS_SW23      BQL_SCOMDEF(0x00201A)     // R   Sparing Status SW23
#define BGQ_TI_SCOM_BQL_SPARE_ERRCNT_SW01      BQL_SCOMDEF(0x00200B)     // R   Sparing Absolute Error Cnt SW01
#define BGQ_TI_SCOM_BQL_SPARE_ERRCNT_SW23      BQL_SCOMDEF(0x00201B)     // R   Sparing Absolute Error Cnt SW23
#define BGQ_TI_SCOM_BQL_SPARING_RATECNT_SW01   BQL_SCOMDEF(0x00200C)     // R   Sparing Error Rate Cntr SW01
#define BGQ_TI_SCOM_BQL_SPARING_RATECNT_SW23   BQL_SCOMDEF(0x00201C)     // R   Sparing Error Rate Cntr SW23
#define BGQ_TI_SCOM_BQL_SPARE_ERRTHRESH        BQL_SCOMDEF(0x002030)     // R/W Sparing Error Threshold
#define BQL_INIT_SPARE_CNTL_C                  (0x00A000B000000000ULL)
#define BQL_INIT_SPARE_CNTL_D                  (0x00A000B000000000ULL)
#define BQL_INIT_SPARE_STATUS                  (0x0000000000000000ULL)
#define BQL_INIT_SPARE_ERRCNT                  (0x0000000000000000ULL)
#define BQL_INIT_SPARING_RATECNT               (0x0000000000000000ULL)
#define BQL_INIT_SPARE_ERRTHRESH               (0xFFFFFFFF00000000ULL)
#define BQL_MASK_SPARE_CNTL_C                  (0x80FF60FF600FFFFFULL)
#define BQL_MASK_SPARE_CNTL_D                  (0x80FF20FF200FFFFFULL)
#define BQL_MASK_SPARE_CNTL_CONFIG             (0x8000000000000000ULL)
#define BQL_MASK_SPARE_CNTL_SP1                (0x00F0000000000000ULL)
#define BQL_MASK_SPARE_CNTL_SP1_SRC            (0x000F000000000000ULL)
#define BQL_MASK_SPARE_CNTL_SP1_IS_BAD         (0x0000200000000000ULL)
#define BQL_MASK_SPARE_CNTL_SP2                (0x000000F000000000ULL)
#define BQL_MASK_SPARE_CNTL_SP2_SRC            (0x0000000F00000000ULL)
#define BQL_MASK_SPARE_CNTL_SP2_IS_BAD         (0x0000000020000000ULL)
#define BQL_MASK_SPARE_CNTL_SHIFT_MASK         (0x00000000000FFFFFULL)
#define BQL_MASK_SPARE_STATUS                  (0xFFFFFFFF03FFFFFFULL)
#define BQL_MASK_SPARE_ERRCNT                  (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_SPARING_RATECNT               (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_SPARE_ERRTHRESH               (0xFFFFFFFF00000000ULL)

// Decode Registers (unit 0x03)
#define BGQ_TI_SCOM_BQL_ENCODE_DECODE_CTRL     BQL_SCOMDEF(0x003000)     // R/W Encode/Decode Configuration
#define BGQ_TI_SCOM_BQL_ERROR_THRES            BQL_SCOMDEF(0x003001)     // R/W Single Bit and Double Bit counters Error Threshold
#define BGQ_TI_SCOM_BQL_ERROR_COUNT_RESET      BQL_SCOMDEF(0x003002)     // R/W Single Bit and Double Bit Error Count Reset
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_0_0       BQL_SCOMDEF(0x003004)     // R/W Decode Error Injection for switch01 channel 0-2
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_0_1       BQL_SCOMDEF(0x003005)     // R/W Decode Error Injection for switch01 channel 3-5
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_0_2       BQL_SCOMDEF(0x003006)     // R/W Decode Error Injection for switch01 channel 6-8
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_0_3       BQL_SCOMDEF(0x003007)     // R/W Decode Error Injection for switch01 channel 9-11
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_1_0       BQL_SCOMDEF(0x003008)     // R/W Decode Error Injection for switch23 channel 0-2
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_1_1       BQL_SCOMDEF(0x003009)     // R/W Decode Error Injection for switch23 channel 3-5
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_1_2       BQL_SCOMDEF(0x00300A)     // R/W Decode Error Injection for switch23 channel 6-8
#define BGQ_TI_SCOM_BQL_CHANNEL_MASK_1_3       BQL_SCOMDEF(0x00300B)     // R/W Decode Error Injection for switch23 channel 9-11
#define BGQ_TI_SCOM_BQL_ERROR_ALERT            BQL_SCOMDEF(0x00300C)     // R   Single Bit (SBE) and Double Bit (DBE) Error Alert
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW0_LANE0      BQL_SCOMDEF(0x003010)     // R   Single bit error (SBE) counter for switch0 lane0
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW0_LANE1      BQL_SCOMDEF(0x003011)     // R   SBE counter for switch0 lane1
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW0_LANE2      BQL_SCOMDEF(0x003012)     // R   SBE counter for  switch0 lane2
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW0_LANE3      BQL_SCOMDEF(0x003013)     // R   SBE counter for switch0 lane3
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW0_LANE4      BQL_SCOMDEF(0x003014)     // R   SBE counter for switch0 lane4
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW1_LANE0      BQL_SCOMDEF(0x003018)     // R   SBE counter for switch1 lane0
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW1_LANE1      BQL_SCOMDEF(0x003019)     // R   SBE counter for switch1 lane1
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW1_LANE2      BQL_SCOMDEF(0x00301A)     // R   SBE counter for switch1 lane2
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW1_LANE3      BQL_SCOMDEF(0x00301B)     // R   SBE counter for switch1 lane3
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW1_LANE4      BQL_SCOMDEF(0x00301C)     // R   SBE counter for switch1 lane4
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW2_LANE0      BQL_SCOMDEF(0x003020)     // R   SBE counter for switch2 lane0
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW2_LANE1      BQL_SCOMDEF(0x003021)     // R   SBE counter for switch2 lane1
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW2_LANE2      BQL_SCOMDEF(0x003022)     // R   SBE counter for switch2 lane2
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW2_LANE3      BQL_SCOMDEF(0x003023)     // R   SBE counter for switch2 lane3
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW2_LANE4      BQL_SCOMDEF(0x003024)     // R   SBE counter for switch2 lane4
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW3_LANE0      BQL_SCOMDEF(0x003028)     // R   SBE counter for switch3 lane0
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW3_LANE1      BQL_SCOMDEF(0x003029)     // R   SBE counter for switch3 lane1
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW3_LANE2      BQL_SCOMDEF(0x00302A)     // R   SBE counter for switch3 lane2
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW3_LANE3      BQL_SCOMDEF(0x00302B)     // R   SBE counter for switch3 lane3
#define BGQ_TI_SCOM_BQL_SBE_CNT_SW3_LANE4      BQL_SCOMDEF(0x00302C)     // R   SBE counter for switch3 lane4
#define BGQ_TI_SCOM_BQL_DBE_CNT_EVEN_SW0       BQL_SCOMDEF(0x003030)     // R   Double Bit Error (DBE) even counter for switch0
#define BGQ_TI_SCOM_BQL_DBE_CNT_EVEN_SW1       BQL_SCOMDEF(0x003032)     // R   DBE even counter for switch1
#define BGQ_TI_SCOM_BQL_DBE_CNT_EVEN_SW2       BQL_SCOMDEF(0x003034)     // R   DBE even counter for switch2
#define BGQ_TI_SCOM_BQL_DBE_CNT_EVEN_SW3       BQL_SCOMDEF(0x003036)     // R   DBE even counter for switch3
#define BGQ_TI_SCOM_BQL_DBE_CNT_ODD_SW0        BQL_SCOMDEF(0x003031)     // R   DBE odd counter for switch0
#define BGQ_TI_SCOM_BQL_DBE_CNT_ODD_SW1        BQL_SCOMDEF(0x003033)     // R   DBE odd counter for switch1
#define BGQ_TI_SCOM_BQL_DBE_CNT_ODD_SW2        BQL_SCOMDEF(0x003035)     // R   DBE odd counter for switch2
#define BGQ_TI_SCOM_BQL_DBE_CNT_ODD_SW3        BQL_SCOMDEF(0x003037)     // R   DBE odd counter for switch3
#define BQL_INIT_ENCODE_DECODE_CTRL            (0x0000000000000000ULL)
#define BQL_INIT_ERROR_THRES                   (0xFFFFFFFFFFFFFFFFULL)
#define BQL_INIT_ERROR_COUNT_RESET             (0x0000000000000000ULL)
#define BQL_INIT_CHANNEL_MASK                  (0x0000000000000000ULL)
#define BQL_INIT_ERROR_ALERT                   (0x0000000000000000ULL)
#define BQL_INIT_SBE_CNT_SW_LANE               (0x0000000000000000ULL)
#define BQL_INIT_DBE_CNT                       (0x0000000000000000ULL)
#define BQL_MASK_ENCODE_DECODE_CTRL            (0xFFFFFC0000000000ULL)
#define BQL_MASK_ERROR_THRES                   (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_ERROR_COUNT_RESET             (0x8000800080008000ULL) // removed non-persistent bits: 1-7,17-23,33-39,49-55
#define BQL_MASK_CHANNEL_MASK                  (0xFFFFFFFFFFFFFFF8ULL)
#define BQL_MASK_ERROR_ALERT                   (0xFE01FC03F807F000ULL)
#define BQL_MASK_SBE_CNT_SW_LANE               (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_DBE_CNT                       (0xFFFFFFFFFFFFFFFFULL)

// HSS Registers (unit 0x04)
#define BGQ_TI_SCOM_BQL_HSS_EYECFG1_AB0        BQL_SCOMDEF(0x004000)     // R/W Port A0 and B0 - HSS EYE Config
#define BGQ_TI_SCOM_BQL_HSS_EYECFG1_AB1        BQL_SCOMDEF(0x004100)     // R/W Port A1 and B1 - HSS EYE Config
#define BGQ_TI_SCOM_BQL_HSS_EYECFG1_AB2        BQL_SCOMDEF(0x004200)     // R/W Port A2 and B2 - HSS EYE Config
#define BGQ_TI_SCOM_BQL_HSS_EYECFG1_AB3        BQL_SCOMDEF(0x004300)     // R/W Port A3 and B3 - HSS EYE Config
#define BGQ_TI_SCOM_BQL_HSS_EYECTL_AB0         BQL_SCOMDEF(0x004001)     // R/W Port A0 and B0 - HSS EYE Control
#define BGQ_TI_SCOM_BQL_HSS_EYECTL_AB1         BQL_SCOMDEF(0x004101)     // R/W Port A1 and B1 - HSS EYE Control
#define BGQ_TI_SCOM_BQL_HSS_EYECTL_AB2         BQL_SCOMDEF(0x004201)     // R/W Port A2 and B2 - HSS EYE Control
#define BGQ_TI_SCOM_BQL_HSS_EYECTL_AB3         BQL_SCOMDEF(0x004301)     // R/W Port A3 and B3 - HSS EYE Control
#define BGQ_TI_SCOM_BQL_HSS_EYESTAT_AB0        BQL_SCOMDEF(0x004002)     // R   Port A0 and B0 - HSS EYE Status
#define BGQ_TI_SCOM_BQL_HSS_EYESTAT_AB1        BQL_SCOMDEF(0x004102)     // R   Port A1 and B1 - HSS EYE Status
#define BGQ_TI_SCOM_BQL_HSS_EYESTAT_AB2        BQL_SCOMDEF(0x004202)     // R   Port A2 and B2 - HSS EYE Status
#define BGQ_TI_SCOM_BQL_HSS_EYESTAT_AB3        BQL_SCOMDEF(0x004302)     // R   Port A3 and B3 - HSS EYE Status
#define BGQ_TI_SCOM_BQL_DRV_CTRL1_B0           BQL_SCOMDEF(0x004003)     // R/W Port A0 and B0 - HSS Driver Control 1
#define BGQ_TI_SCOM_BQL_DRV_CTRL1_B1           BQL_SCOMDEF(0x004103)     // R/W Port A1 and B1 - HSS Driver Control 1
#define BGQ_TI_SCOM_BQL_DRV_CTRL1_B2           BQL_SCOMDEF(0x004203)     // R/W Port A2 and B2 - HSS Driver Control 1
#define BGQ_TI_SCOM_BQL_DRV_CTRL1_B3           BQL_SCOMDEF(0x004303)     // R/W Port A3 and B3 - HSS Driver Control 1
#define BGQ_TI_SCOM_BQL_DRV_CTRL2_B0           BQL_SCOMDEF(0x004004)     // R/W Port A0 and B0 - HSS Driver Control 2
#define BGQ_TI_SCOM_BQL_DRV_CTRL2_B1           BQL_SCOMDEF(0x004104)     // R/W Port A1 and B1 - HSS Driver Control 2
#define BGQ_TI_SCOM_BQL_DRV_CTRL2_B2           BQL_SCOMDEF(0x004204)     // R/W Port A2 and B2 - HSS Driver Control 2
#define BGQ_TI_SCOM_BQL_DRV_CTRL2_B3           BQL_SCOMDEF(0x004304)     // R/W Port A3 and B3 - HSS Driver Control 2
#define BGQ_TI_SCOM_BQL_RCV_CTRL1_A0           BQL_SCOMDEF(0x004005)     // R/W Port A0 and B0 - HSS Receiver Control 1
#define BGQ_TI_SCOM_BQL_RCV_CTRL1_A1           BQL_SCOMDEF(0x004105)     // R/W Port A1 and B1 - HSS Receiver Control 1
#define BGQ_TI_SCOM_BQL_RCV_CTRL1_A2           BQL_SCOMDEF(0x004205)     // R/W Port A2 and B2 - HSS Receiver Control 1
#define BGQ_TI_SCOM_BQL_RCV_CTRL1_A3           BQL_SCOMDEF(0x004305)     // R/W Port A3 and B3 - HSS Receiver Control 1
#define BGQ_TI_SCOM_BQL_RCV_CTRL2_A0           BQL_SCOMDEF(0x004006)     // R/W Port A0 and B0 - HSS Receiver Control 2
#define BGQ_TI_SCOM_BQL_RCV_CTRL2_A1           BQL_SCOMDEF(0x004106)     // R/W Port A1 and B1 - HSS Receiver Control 2
#define BGQ_TI_SCOM_BQL_RCV_CTRL2_A2           BQL_SCOMDEF(0x004206)     // R/W Port A2 and B2 - HSS Receiver Control 2
#define BGQ_TI_SCOM_BQL_RCV_CTRL2_A3           BQL_SCOMDEF(0x004306)     // R/W Port A3 and B3 - HSS Receiver Control 2
#define BGQ_TI_SCOM_BQL_RCV_STATUS_A0          BQL_SCOMDEF(0x004007)     // R   Port A0 and B0 - HSS Receiver Status
#define BGQ_TI_SCOM_BQL_RCV_STATUS_A1          BQL_SCOMDEF(0x004107)     // R   Port A1 and B1 - HSS Receiver Status
#define BGQ_TI_SCOM_BQL_RCV_STATUS_A2          BQL_SCOMDEF(0x004207)     // R   Port A2 and B2 - HSS Receiver Status
#define BGQ_TI_SCOM_BQL_RCV_STATUS_A3          BQL_SCOMDEF(0x004307)     // R   Port A3 and B3 - HSS Receiver Status
#define BGQ_TI_SCOM_BQL_TX_AMPL_CTRL_B0        BQL_SCOMDEF(0x004008)     // R/W Port A0 and B0 - HSS Transmit  Amplitude Control
#define BGQ_TI_SCOM_BQL_TX_AMPL_CTRL_B1        BQL_SCOMDEF(0x004108)     // R/W Port A1 and B1 - HSS Transmit  Amplitude Control
#define BGQ_TI_SCOM_BQL_TX_AMPL_CTRL_B2        BQL_SCOMDEF(0x004208)     // R/W Port A2 and B2 - HSS Transmit  Amplitude Control
#define BGQ_TI_SCOM_BQL_TX_AMPL_CTRL_B3        BQL_SCOMDEF(0x004308)     // R/W Port A3 and B3 - HSS Transmit  Amplitude Control
#define BGQ_TI_SCOM_BQL_TX_COEF_B0             BQL_SCOMDEF(0x004009)     // R/W Port A0 and B0 - HSS Transmit  Coefficient
#define BGQ_TI_SCOM_BQL_TX_COEF_B1             BQL_SCOMDEF(0x004109)     // R/W Port A1 and B1 - HSS Transmit  Coefficient
#define BGQ_TI_SCOM_BQL_TX_COEF_B2             BQL_SCOMDEF(0x004209)     // R/W Port A2 and B2 - HSS Transmit  Coefficient
#define BGQ_TI_SCOM_BQL_TX_COEF_B3             BQL_SCOMDEF(0x004309)     // R/W Port A3 and B3 - HSS Transmit  Coefficient
#define BGQ_TI_SCOM_BQL_HSS_CTRL_AB0           BQL_SCOMDEF(0x00400A)     // R/W Port A0 and B0 - HSS Control
#define BGQ_TI_SCOM_BQL_HSS_CTRL_AB1           BQL_SCOMDEF(0x00410A)     // R/W Port A1 and B1 - HSS Control
#define BGQ_TI_SCOM_BQL_HSS_CTRL_AB2           BQL_SCOMDEF(0x00420A)     // R/W Port A2 and B2 - HSS Control
#define BGQ_TI_SCOM_BQL_HSS_CTRL_AB3           BQL_SCOMDEF(0x00430A)     // R/W Port A3 and B3 - HSS Control
#define BGQ_TI_SCOM_BQL_HSS_STATUS_AB0         BQL_SCOMDEF(0x00400B)     // R   Port A0 and B0 - HSS Status
#define BGQ_TI_SCOM_BQL_HSS_STATUS_AB1         BQL_SCOMDEF(0x00410B)     // R   Port A1 and B1 - HSS Status
#define BGQ_TI_SCOM_BQL_HSS_STATUS_AB2         BQL_SCOMDEF(0x00420B)     // R   Port A2 and B2 - HSS Status
#define BGQ_TI_SCOM_BQL_HSS_STATUS_AB3         BQL_SCOMDEF(0x00430B)     // R   Port A3 and B3 - HSS Status
#define BGQ_TI_SCOM_BQL_RX_PRBS_CTRL_A0        BQL_SCOMDEF(0x00400C)     // R/W Port A0 and B0 - HSS Receive PRBS Control
#define BGQ_TI_SCOM_BQL_RX_PRBS_CTRL_A1        BQL_SCOMDEF(0x00410C)     // R/W Port A1 and B1 - HSS Receive PRBS Control
#define BGQ_TI_SCOM_BQL_RX_PRBS_CTRL_A2        BQL_SCOMDEF(0x00420C)     // R/W Port A2 and B2 - HSS Receive PRBS Control
#define BGQ_TI_SCOM_BQL_RX_PRBS_CTRL_A3        BQL_SCOMDEF(0x00430C)     // R/W Port A3 and B3 - HSS Receive PRBS Control
#define BGQ_TI_SCOM_BQL_RX_PRBS_STATUS_A0      BQL_SCOMDEF(0x00400D)     // R   Port A0 and B0 - HSS Receive PRBS Status
#define BGQ_TI_SCOM_BQL_RX_PRBS_STATUS_A1      BQL_SCOMDEF(0x00410D)     // R   Port A1 and B1 - HSS Receive PRBS Status
#define BGQ_TI_SCOM_BQL_RX_PRBS_STATUS_A2      BQL_SCOMDEF(0x00420D)     // R   Port A2 and B2 - HSS Receive PRBS Status
#define BGQ_TI_SCOM_BQL_RX_PRBS_STATUS_A3      BQL_SCOMDEF(0x00430D)     // R   Port A3 and B3 - HSS Receive PRBS Status
#define BGQ_TI_SCOM_BQL_TX_PRBS_CTRL_B0        BQL_SCOMDEF(0x00400E)     // R/W Port A0 and B0 - HSS Transmit PRBS Control
#define BGQ_TI_SCOM_BQL_TX_PRBS_CTRL_B1        BQL_SCOMDEF(0x00410E)     // R/W Port A1 and B1 - HSS Transmit PRBS Control
#define BGQ_TI_SCOM_BQL_TX_PRBS_CTRL_B2        BQL_SCOMDEF(0x00420E)     // R/W Port A2 and B2 - HSS Transmit PRBS Control
#define BGQ_TI_SCOM_BQL_TX_PRBS_CTRL_B3        BQL_SCOMDEF(0x00430E)     // R/W Port A3 and B3 - HSS Transmit PRBS Control
#define BGQ_TI_SCOM_BQL_TX_PRBS_STATUS_B0      BQL_SCOMDEF(0x00400F)     // R   Port A0 and B0 - HSS Transmit PRBS Status
#define BGQ_TI_SCOM_BQL_TX_PRBS_STATUS_B1      BQL_SCOMDEF(0x00410F)     // R   Port A1 and B1 - HSS Transmit PRBS Status
#define BGQ_TI_SCOM_BQL_TX_PRBS_STATUS_B2      BQL_SCOMDEF(0x00420F)     // R   Port A2 and B2 - HSS Transmit PRBS Status
#define BGQ_TI_SCOM_BQL_TX_PRBS_STATUS_B3      BQL_SCOMDEF(0x00430F)     // R   Port A3 and B3 - HSS Transmit PRBS Status
#define BGQ_TI_SCOM_BQL_HSS_RXEQU_AB0          BQL_SCOMDEF(0x004010)     // R/W Port A0 and B0 - HSS Receive for HSS4G0
#define BGQ_TI_SCOM_BQL_HSS_RXEQU_AB1          BQL_SCOMDEF(0x004110)     // R/W Port A0 and B0 - HSS Receive for HSS4G1
#define BGQ_TI_SCOM_BQL_HSS_RXEQU_AB2          BQL_SCOMDEF(0x004210)     // R/W Port A0 and B0 - HSS Receive for HSS4G2
#define BGQ_TI_SCOM_BQL_HSS_RXEQU_AB3          BQL_SCOMDEF(0x004310)     // R/W Port A0 and B0 - HSS Receive for HSS4G3
#define BGQ_TI_SCOM_BQL_HSS_SIGLEV_AB0         BQL_SCOMDEF(0x004011)     // R/W Port A0 and B0 - HSS Receive
#define BGQ_TI_SCOM_BQL_HSS_SIGLEV_AB1         BQL_SCOMDEF(0x004111)     // R/W Port A1 and B1 - HSS Receive
#define BGQ_TI_SCOM_BQL_HSS_SIGLEV_AB2         BQL_SCOMDEF(0x004211)     // R/W Port A2 and B2 - HSS Receive
#define BGQ_TI_SCOM_BQL_HSS_SIGLEV_AB3         BQL_SCOMDEF(0x004311)     // R/W Port A3 and B3 - HSS Receive
#define BGQ_TI_SCOM_BQL_SYNCAB_CTRL_AB0        BQL_SCOMDEF(0x004012)     // R/W Port A0 and B0 - SyncAB Control
#define BGQ_TI_SCOM_BQL_SYNCAB_CTRL_AB1        BQL_SCOMDEF(0x004112)     // R/W Port A1 and B1 -SyncAB Control
#define BGQ_TI_SCOM_BQL_SYNCAB_CTRL_AB2        BQL_SCOMDEF(0x004212)     // R/W Port A2 and B2 -SyncAB Control
#define BGQ_TI_SCOM_BQL_SYNCAB_CTRL_AB3        BQL_SCOMDEF(0x004312)     // R/W Port A3 and B3 -SyncAB Control
#define BGQ_TI_SCOM_BQL_SYNCAB_STAT_AB0        BQL_SCOMDEF(0x004013)     // R   Port A0 and B0 -SyncAB Status
#define BGQ_TI_SCOM_BQL_SYNCAB_STAT_AB1        BQL_SCOMDEF(0x004113)     // R   Port A1 and B1 -SyncAB Status
#define BGQ_TI_SCOM_BQL_SYNCAB_STAT_AB2        BQL_SCOMDEF(0x004213)     // R   Port A2 and B2 -SyncAB Status
#define BGQ_TI_SCOM_BQL_SYNCAB_STAT_AB3        BQL_SCOMDEF(0x004313)     // R   Port A3 and B3 -SyncAB Status
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG1_AB0     BQL_SCOMDEF(0x004014)     // R/W Port A0 and B0 -SyncAB Eye Config 1
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG1_AB1     BQL_SCOMDEF(0x004114)     // R/W Port A1 and B1 -SyncAB Eye Config 1
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG1_AB2     BQL_SCOMDEF(0x004214)     // R/W Port A2 and B2 -SyncAB Eye Config 1
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG1_AB3     BQL_SCOMDEF(0x004314)     // R/W Port A3 and B3 -SyncAB Eye Config 1
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG2_AB0     BQL_SCOMDEF(0x004015)     // R/W Port A0 and B0 -SyncAB Eye Config 2
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG2_AB1     BQL_SCOMDEF(0x004115)     // R/W Port A1 and B1 -SyncAB Eye Config 2
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG2_AB2     BQL_SCOMDEF(0x004215)     // R/W Port A2 and B2 -SyncAB Eye Config 2
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG2_AB3     BQL_SCOMDEF(0x004315)     // R/W Port A3 and B3 -SyncAB Eye Config 2
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG3_AB0     BQL_SCOMDEF(0x004016)     // R/W Port A0 and B0 -SyncAB Eye Config 3
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG3_AB1     BQL_SCOMDEF(0x004116)     // R/W Port A1 and B1 -SyncAB Eye Config 3
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG3_AB2     BQL_SCOMDEF(0x004216)     // R/W Port A2 and B2 -SyncAB Eye Config 3
#define BGQ_TI_SCOM_BQL_SYNCAB_EYECFG3_AB3     BQL_SCOMDEF(0x004316)     // R/W Port A3 and B3 -SyncAB Eye Config 3
#define BGQ_TI_SCOM_BQL_SYNCAB_EYESTAT_AB0     BQL_SCOMDEF(0x004017)     // R   Port A0 and B0 -SyncAB Eye Status
#define BGQ_TI_SCOM_BQL_SYNCAB_EYESTAT_AB1     BQL_SCOMDEF(0x004117)     // R   Port A1 and B1 -SyncAB Eye Status
#define BGQ_TI_SCOM_BQL_SYNCAB_EYESTAT_AB2     BQL_SCOMDEF(0x004217)     // R   Port A2 and B2 -SyncAB Eye Status
#define BGQ_TI_SCOM_BQL_SYNCAB_EYESTAT_AB3     BQL_SCOMDEF(0x004317)     // R   Port A3 and B3 -SyncAB Eye Status
#define BQL_INIT_HSS_EYECFG1                   (0x0000000000000000ULL)
#define BQL_INIT_HSS_EYECTL                    (0x0000000000000000ULL)
#define BQL_INIT_HSS_EYESTAT                   (0x0000000000000000ULL)
#define BQL_INIT_DRV_CTRL                      (0x0010004001000400ULL)
#define BQL_INIT_RCV_CTRL1                     (0x0204020402040204ULL)
#define BQL_INIT_RCV_CTRL2                     (0x0204020402040204ULL)
#define BQL_INIT_RCV_STATUS                    (0x0000000000000000ULL)
#define BQL_INIT_TX_AMPL_CTRL                  (0xFFFFFF0000000000ULL)
#define BQL_INIT_TX_COEF                       (0x8888888800000000ULL)
#define BQL_INIT_HSS_CTRL                      (0x8A80000000000000ULL)
#define BQL_INIT_HSS_STATUS                    (0x0000000000000000ULL)
#define BQL_INIT_RX_PRBS_CTRL                  (0x0000000000000000ULL)
#define BQL_INIT_RX_PRBS_STATUS                (0x0000000000000000ULL)
#define BQL_INIT_TX_PRBS_CTRL                  (0x0000000000000000ULL)
#define BQL_INIT_TX_PRBS_STATUS                (0x0000000000000000ULL)
#define BQL_INIT_HSS_RXEQU                     (0xFFFF000000000000ULL)
#define BQL_INIT_HSS_SIGLEV                    (0x4210842108000000ULL)
#define BQL_INIT_SYNCAB_CTRL                   (0x0000000000000000ULL)
#define BQL_INIT_SYNCAB_STAT                   (0x0000000000000000ULL)
#define BQL_INIT_SYNCAB_EYECFG1                (0x0000000000000000ULL)
#define BQL_INIT_SYNCAB_EYECFG2                (0x0000000000000000ULL)
#define BQL_INIT_SYNCAB_EYECFG3                (0x0000000000000000ULL)
#define BQL_INIT_SYNCAB_EYESTAT                (0x0000000000000000ULL)
#define BQL_MASK_HSS_EYECFG1                   (0xFF00000000000000ULL)
#define BQL_MASK_HSS_EYECTL                    (0xFC00000000000000ULL)
#define BQL_MASK_HSS_EYESTAT                   (0xFF00000000000000ULL)
#define BQL_MASK_DRV_CTRL                      (0xFFFFFFFFFFFFFF00ULL)
#define BQL_MASK_RCV_CTRL1                     (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_RCV_CTRL2                     (0xFFFFFFFFFFFFFFFFULL)  // (0xFFBFFFBFFFFBFFBFULL)
#define BQL_MASK_RCV_STATUS                    (0xFFFFFFFFFFFFFF00ULL)
#define BQL_MASK_TX_AMPL_CTRL                  (0xFFFFFF0000000000ULL)
#define BQL_MASK_TX_COEF                       (0xFFFFFFFF00000000ULL)
#define BQL_MASK_HSS_CTRL                      (0xFF80000000000000ULL)
#define BQL_MASK_HSS_STATUS                    (0xE000000000000000ULL)
#define BQL_MASK_RX_PRBS_CTRL                  (0xFFFFFFFFFFFFFF00ULL)
#define BQL_MASK_RX_PRBS_STATUS                (0xFFFF000000000000ULL)
#define BQL_MASK_TX_PRBS_CTRL                  (0xFFFFFFFFFFFF0000ULL)
#define BQL_MASK_TX_PRBS_STATUS                (0xFFFF000000000000ULL)
#define BQL_MASK_HSS_RXEQU                     (0xFFFF000000000000ULL)
#define BQL_MASK_HSS_SIGLEV                    (0xFFFFFFFFFF000000ULL)
#define BQL_MASK_SYNCAB_CTRL                   (0xFFFFFC0000000000ULL)
#define BQL_MASK_SYNCAB_STAT                   (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_SYNCAB_EYECFG1                (0xFFFFFFFFFFFFFF00ULL)
#define BQL_MASK_SYNCAB_EYECFG2                (0xFFFFFFFFFFFFFF00ULL)
#define BQL_MASK_SYNCAB_EYECFG3                (0xFFFFFFFFFFE00000ULL)
#define BQL_MASK_SYNCAB_EYESTAT                (0xFFFFFFFFFFFFFFFCULL)


// HSS Registers (unit 0x0A)
#define BGQ_TI_SCOM_BQL_HSTX_CTRL              BQL_SCOMDEF(0x00A000)     // R/W Transmit Control
#define BGQ_TI_SCOM_BQL_HSRX_STAT              BQL_SCOMDEF(0x00A001)     // R/W Receive Control
#define BGQ_TI_SCOM_BQL_HSPLL_CTRL             BQL_SCOMDEF(0x00A002)     // R/W HS PLL Control
#define BGQ_TI_SCOM_BQL_HSPLL_STAT             BQL_SCOMDEF(0x00A003)     // R/W HS PLL Status
#define BGQ_TI_SCOM_BQL_HS10G0_REG_ACC         BQL_SCOMDEF(0x00A004)     // R/W 10G0 Register Access
#define BGQ_TI_SCOM_BQL_HS10G0B_REG_ACC        BQL_SCOMDEF(0x00A006)     // R/W 10G0B Register Access
#define BGQ_TI_SCOM_BQL_HS10G2_REG_ACC         BQL_SCOMDEF(0x00A008)     // R/W 10G2 Register Access
#define BGQ_TI_SCOM_BQL_HS10G2B_REG_ACC        BQL_SCOMDEF(0x00A00A)     // R/W 10G2B Register Access
#define BGQ_TI_SCOM_BQL_HS10G0_REG_ACCRD       BQL_SCOMDEF(0x00A005)     // R/W 10G0 Register Access Read
#define BGQ_TI_SCOM_BQL_HS10G0B_REG_ACCRD      BQL_SCOMDEF(0x00A007)     // R/W 10G0B Register Access Read
#define BGQ_TI_SCOM_BQL_HS10G2_REG_ACCRD       BQL_SCOMDEF(0x00A009)     // R/W 10G2 Register Access Read
#define BGQ_TI_SCOM_BQL_HS10G2B_REG_ACCRD      BQL_SCOMDEF(0x00A00B)     // R/W 10G2B Register Access Read
#define BGQ_TI_SCOM_BQL_HSPWR_CTRL             BQL_SCOMDEF(0x00A00C)     // R/W Port 01 & 23 - AC coupling Mode Selector
#define BGQ_TI_SCOM_BQL_HSRXPHS_CTRL0          BQL_SCOMDEF(0x00A00E)     // R/W Port C2 & C3 - Receive Phase Rotator Cntl 0
#define BGQ_TI_SCOM_BQL_HSRXPHS_CTRL1          BQL_SCOMDEF(0x00A00F)     // R/W Port C2 & C3 - Receive Phase Rotator Cntl 1
#define BGQ_TI_SCOM_BQL_TXSYNC_CTRL            BQL_SCOMDEF(0x00A200)     // R/W Port Dx - Transmit Sync Buffer Control
#define BGQ_TI_SCOM_BQL_TXALIGN_CTRL           BQL_SCOMDEF(0x00A201)     // R/W Port Dx - Transmit Align Control
#define BGQ_TI_SCOM_BQL_RXSYNC_CTRL            BQL_SCOMDEF(0x00A202)     // R/W Port Cx - Receive Sync Buffer Control
#define BGQ_TI_SCOM_BQL_RXALIGN_CTRL           BQL_SCOMDEF(0x00A203)     // R/W Port Cx - Receive Align Control
#define BGQ_TI_SCOM_BQL_RXALIGN_STAT           BQL_SCOMDEF(0x00A204)     // R/W Port Cx - Receive Align Status
#define BQL_INIT_HSTX_CTRL                     (0xFFFFFF0000000000ULL)
#define BQL_INIT_HSRX_STAT                     (0x0000000000000000ULL)
#define BQL_INIT_HSPLL_CTRL                    (0x2121000000000000ULL)
#define BQL_INIT_HSPLL_STAT                    (0x2020000000000000ULL)
#define BQL_INIT_HS10G0_REG_ACC                (0x0000000000000000ULL)
#define BQL_INIT_HS10G0B_REG_ACC               (0x0000000000000000ULL)
#define BQL_INIT_HS10G2_REG_ACC                (0x0000000000000000ULL)
#define BQL_INIT_HS10G2B_REG_ACC               (0x0000000000000000ULL)
#define BQL_INIT_HS10G0_REG_ACCRD              (0x0000000000000000ULL)
#define BQL_INIT_HS10G0B_REG_ACCRD             (0x0000000000000000ULL)
#define BQL_INIT_HS10G2_REG_ACCRD              (0x0000000000000000ULL)
#define BQL_INIT_HS10G2B_REG_ACCRD             (0x0000000000000000ULL)
#define BQL_INIT_HSPWR_CTRL                    (0x0000000000000000ULL)
#define BQL_INIT_HSRXPHS_CTRL                  (0x0000000000000000ULL)
#define BQL_INIT_TXSYNC_CTRL                   (0x0101010100000000ULL)
#define BQL_INIT_TXALIGN_CTRL                  (0x0000000000000000ULL)
#define BQL_INIT_RXSYNC_CTRL                   (0x0101010100000000ULL)
#define BQL_INIT_RXALIGN_CTRL                  (0x0000000000000000ULL)
#define BQL_INIT_RXALIGN_STAT                  (0x0000000000000000ULL)
#define BQL_MASK_HSTX_CTRL                     (0xFFFFFF0000000000ULL)
#define BQL_MASK_HSRX_STAT                     (0xFFFFFF0000000000ULL)
#define BQL_MASK_HSPLL_CTRL                    (0xFFFF000000000000ULL)
#define BQL_MASK_HSPLL_STAT                    (0xDDDD000000000000ULL)
#define BQL_MASK_HS10G0_REG_ACC                (0xC3FFFFFF00000000ULL)
#define BQL_MASK_HS10G0B_REG_ACC               (0xC3FFFFFF00000000ULL)
#define BQL_MASK_HS10G2_REG_ACC                (0xC3FFFFFF00000000ULL)
#define BQL_MASK_HS10G2B_REG_ACC               (0xC3FFFFFF00000000ULL)
#define BQL_MASK_HS10G0_REG_ACCRD              (0xFFFFFFFF00000000ULL)
#define BQL_MASK_HS10G0B_REG_ACCRD             (0xFFFFFFFF00000000ULL)
#define BQL_MASK_HS10G2_REG_ACCRD              (0xFFFFFFFF00000000ULL)
#define BQL_MASK_HS10G2B_REG_ACCRD             (0xFFFFFFFF00000000ULL)
#define BQL_MASK_HSPWR_CTRL                    (0xF000000000000000ULL)
#define BQL_MASK_HSRXPHS_CTRL                  (0xFFFFFFFFFFFFFFF0ULL)
#define BQL_MASK_TXSYNC_CTRL                   (0xFFFFFFFF00000000ULL)
#define BQL_MASK_TXALIGN_CTRL                  (0xFFFFFFC000000000ULL)
#define BQL_MASK_RXSYNC_CTRL                   (0xFFFFFFFF00000000ULL)
#define BQL_MASK_RXALIGN_CTRL                  (0xFFFFFFFFFFFFC000ULL)
#define BQL_MASK_RXALIGN_STAT                  (0xFFFFFFFFFFFF0000ULL)
#define BQL_HS10G_RXPORTA                      (0x4041000000000000ULL)
#define BQL_HS10G_RXPORTB                      (0x4061000000000000ULL)
#define BQL_HS10G_RXPORTC                      (0x40C1000000000000ULL)
#define BQL_HS10G_RXPORTD                      (0x40E1000000000000ULL)
#define BQL_HS10G_RXPORTE                      (0x4141000000000000ULL)
#define BQL_HS10G_RXPORTF                      (0x4161000000000000ULL)
#define BQL_HS10G_RXPORTG                      (0x41C1000000000000ULL)
#define BQL_HS10G_RXPORTH                      (0x41E1000000000000ULL)
#define BQL_HS10G_TXPORTA                      (0x4001000000000000ULL)
#define BQL_HS10G_TXPORTB                      (0x4021000000000000ULL)
#define BQL_HS10G_TXPORTC                      (0x4081000000000000ULL)
#define BQL_HS10G_TXPORTD                      (0x40A1000000000000ULL)
#define BQL_HS10G_TXPORTE                      (0x4101000000000000ULL)
#define BQL_HS10G_TXPORTF                      (0x4121000000000000ULL)
#define BQL_HS10G_TXPORTG                      (0x4181000000000000ULL)
#define BQL_HS10G_TXPORTH                      (0x41A1000000000000ULL)

// Error Registers (unit 0x07)
#define BGQ_TI_SCOM_BQL_MCK_AND_MASK           BQL_SCOMDEF(0x007000)     // R/W Machine Check - Read, Writes w/"AND" mask
#define BGQ_TI_SCOM_BQL_MCK_OR_MASK            BQL_SCOMDEF(0x007001)     // R/W Machine Check - Read, Writes w/ "OR" mask
#define BGQ_TI_SCOM_BQL_MCKM                   BQL_SCOMDEF(0x007010)     // R/W Machine Check Mask
#define BGQ_TI_SCOM_BQL_MCMC                   BQL_SCOMDEF(0x007020)     // R/W Machine Check Capture Why is this _MCMC, why not MCKC?
#define BGQ_TI_SCOM_BQL_TSTAMP_CNTR_RESET      BQL_SCOMDEF(0x007030)     // R/W Time Stamp and Counter Master Reset
#define BGQ_TI_SCOM_BQL_TIMER                  BQL_SCOMDEF(0x007031)     // R/W Timer /  Runtime
#define BQL_INIT_MCK                           (0x0000000000000000ULL)
#define BQL_INIT_MCKM                          (0xFFFFFFFF00000000ULL)
#define BQL_INIT_MCMC                          (0x0000000000000000ULL)
#define BQL_INIT_TSTAMP_CNTR_RESET             (0x0000000000000000ULL)
#define BQL_INIT_TIMER                         (0x0000000000000000ULL)
#define BQL_MASK_MCK                           (0xFFFFFFFF00000000ULL)
#define BQL_MASK_MCKM                          (0xFFFFFFFF00000000ULL)
#define BQL_MASK_MCMC                          (0xFFFFFFFF00000000ULL)
#define BQL_MASK_TSTAMP_CNTR_RESET             (0x8000000000000000ULL) // removed non-persistent bits: 1-3
#define BQL_MASK_TIMER                         (0x000FFFFFFFFFFFFFULL)

// Lab / Debug Registers (unit 0x06)
#define BGQ_TI_SCOM_BQL_RATE_COUNT_CNTL        BQL_SCOMDEF(0x006010)     // R/W Rate Counter Controls
#define BGQ_TI_SCOM_BQL_RATE_TIMER             BQL_SCOMDEF(0x006011)     // R/W Specifies timer period of Rate Counter
#define BGQ_TI_SCOM_BQL_GPIO_SEL               BQL_SCOMDEF(0x006020)     // R/W GPIO Group Select
#define BQL_INIT_RATE_COUNT_CNTL               (0x0000000000000000ULL)
#define BQL_INIT_RATE_TIMER                    (0x0000000000000000ULL)
#define BQL_INIT_GPIO_SEL                      (0x0000000000000000ULL)
#define BQL_MASK_RATE_COUNT_CNTL               (0x4FFFFFFFF0000000ULL) // removed non-persistent bits: 0
#define BQL_MASK_RATE_TIMER                    (0xFFFFFFFFFFFF0000ULL)
#define BQL_MASK_GPIO_SEL                      (0xFFE0000000000000ULL)

// TVSENSE (unit 0x1B)
#define BGQ_TI_SCOM_BQL_TVSENSE_CNTL           BQL_SCOMDEF(0x01B000)     // R/W TVSense Control Register
#define BGQ_TI_SCOM_BQL_TVSENSE_MONITOR_CNTL   BQL_SCOMDEF(0x01B001)     // R/W TVSense Monitor Control Register
#define BGQ_TI_SCOM_BQL_TVSENSE_CAPTURE_RESULT BQL_SCOMDEF(0x01B002)     // R/W TVSense Capture Results Register
#define BGQ_TI_SCOM_BQL_TVSENSE_STATUS         BQL_SCOMDEF(0x01B003)     // R/W TVSense Status Register
#define BQL_INIT_TVSENSE_CNTL                  (0x80313200FFFFF000ULL)
#define BQL_INIT_TVSENSE_MONITOR_CNTL          (0x83EC20FBA17D7840ULL)
#define BQL_INIT_TVSENSE_CAPTURE_RESULT        (0x0000000000000000ULL)
#define BQL_INIT_TVSENSE_STATUS                (0x4000004008000000ULL)
#define BQL_DFLT_TVSENSE_STATUS                (0x00004941E83C0000ULL)
#define BQL_MASK_TVSENSE_CNTL                  (0xA0FFFFF6FFFFF000ULL) // removed non-persistent bits: 1, 31
#define BQL_MASK_TVSENSE_MONITOR_CNTL          (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_TVSENSE_CAPTURE_RESULT        (0xFFFFFFFFFFFFFFFFULL)
#define BQL_MASK_TVSENSE_STATUS                (0xFFFFEFFFFFFCFFFFULL)

// 
// access status register:
//
// 0 (master) Attention The source for this bit is ATTENT_DR (which feed the chip PO)
// 1 M_CHECK  input to ACCESS from chip logic.
// 2 SPEC_ATT  input to ACCESS from chip logic.
// 3 REC_ERR input to ACCESS from chip logic.
// 4 ANY_SCATTN The OR of all SCom attentions
// 5 CRC Miscompare. If set, a scan data check miscompare was found on a scan-in operation.
// 6 Invalid Opcode. The IR-opcode is not supported.
// 7 Invalid Modifier. Instruction does not support the received modifier.
// 8 Modifier Parity Error. Odd parity is required across bits 8-31 of the instruction register (the 
// modifier address), except as noted in Instruction register description.
// 9 SC_SCATTN attention from ACCESS SCom partition.
// 10 CI_SCATTN attention from Clock Tree SCom logic, in clock interface partition.
// 11 LB_SCATTN attention from LBIST SCom interface.
// 12 AB_SCATTN attention from ABIST SCom interface.
// 13 CHIP_SCATTN attention from chip SCom interface.
// 14 HB_SCATTN attention from HSSBIST SCom interface.
// 15 Undefined. (Not Writable).
// 16:31 Stuck OSC bits. 16 bits, as pairs of stuck 0 and stuck 1 bits for up to 8 buf ref clock inputs. Even 
// bits are stuck 0 checks and odd bits are stuck 1 checks. 0 values indicate falures. (Not Writable)

#define BQL_ACCESS_STAT_ATTN         (1<<(31-0))
#define BQL_ACCESS_STAT_MCHK         (1<<(31-1))
#define BQL_ACCESS_STAT_SPEC_ATTN    (1<<(31-2))
#define BQL_ACCESS_STAT_REC_ERR      (1<<(31-3))
#define BQL_ACCESS_STAT_ANY_SCATTN   (1<<(31-4))
#define BQL_ACCESS_STAT_CRC_MC       (1<<(31-5))
#define BQL_ACCESS_STAT_INVAL_OP     (1<<(31-6))
#define BQL_ACCESS_STAT_INVAL_MOD    (1<<(31-7))
#define BQL_ACCESS_STAT_INST_PERR    (1<<(31-8))
#define BQL_ACCESS_STAT_SC_SCATTN    (1<<(31-9))
#define BQL_ACCESS_STAT_CI_SCATTN    (1<<(31-10))
#define BQL_ACCESS_STAT_LBIST_SCERR  (1<<(31-11))
#define BQL_ACCESS_STAT_ABIST_SCERR  (1<<(31-12))
#define BQL_ACCESS_STAT_CHIP_SCATTN  (1<<(31-13))
#define BQL_ACCESS_STAT_HBIST_SCATTN (1<<(31-14))

#define BQL_LBIST_STATUS_IP            	 _BN(0) // Lbist in progress
#define BQL_LBIST_STATUS_DONE          	 _BN(1) // Lbist complete
#define BQL_LBIST_STATUS_INVAL_WRT     	 _BN(2) // Invalid Write Error: Set conditions detailed in bits 7:11. Feeds SCATTN.  
#define BQL_LBIST_STATUS_INVAL_RD      	 _BN(3) // Invalid Read Error: Set when an address within the base address was used for a read, but does not match a defined read operation. Drives SCATTN.
#define BQL_LBIST_STATUS_INVAL_WRT0    	 _BN(6) // Invalid Write Error 0: OPTR, PORSEQ or XTRASEQ loaded with an undefined clock domain. 
#define BQL_LBIST_STATUS_INVAL_WRT1    	 _BN(7) // Invalid Write Error 1: an address within the base address was used for a write, but did not match a defined write operation.
#define BQL_LBIST_STATUS_INVAL_WRT2    	 _BN(8) // Invalid Write Error 2: a write was attempted while LBIST was running.
#define BQL_LBIST_STATUS_INVAL_WRT3    	 _BN(9) // Invalid Write Error 3: a start LBIST was attempted while other BIST was in progress. 
#define BQL_LBIST_STATUS_INVAL_WRT4    	 _BN(10)// Invalid Write Error 4: a start LBIST was attempted while clocks were running.
#define BQL_LBIST_STATUS_INVAL_WRT5    	 _BN(11)// Invalid Write Error 5: a start LBIST was attempted while the chip was un-DIed.
#define BQL_LBIST_STATUS_CHILD         	 _BN(28) 
#define BQL_LBIST_STATUS_PTDONE        	 _BN(29)// (29)PTDONE: set when portesting complete, reset at the start of POR testing,
#define BQL_LBIST_STATUS_SYNCPORTESTI  	 _BN(30)// (30)syncPORTESTI: creates on synd source for PORTESTI, 
#define BQL_LBIST_STATUS_OPCGON        	 _BN(31)// (31)OPCGON: for mfg. only, reset when the test completes,
#define BQL_LBIST_STATUS_MISRFREEZE    	 _BN(32)// (32)MISRFREEZE status of MISRFREEZE function,
#define BQL_LBIST_STATUS_ANYCLKOPI     	 _BN(33)// (33)ANYCLKOPI: creates syncd source for ANYCLKOP (was CLKCMDACK)
#define BQL_LBIST_STATUS_SEQUENCER     	  0x00000000007FF000LL // 41:51 LBIST Sequencer State: Current state of the LBIST sequencer. (read only)
#define BQL_LBIST_STATUS_POLL_SEQUENCER	  0x00000000000003F0LL // 54:59 LBIST Poll Sequencer State: current state of the poll sequencer. (read only)
#define BQL_LBIST_STATUS_READ_ERROR    	 _BN(63)// 63 Read Error: Indicates an illegal read error. (read only)

#define BQL_LBIST_OPTION_ICC   (1ULL<<(63-18)) // Lbist option: Inhibit Channel Clearing


// MCK SCOM register is 64 bits, only first 32 are used
#define BQL_MCK_S0_HIT_SBE_THRESH      (1ULL<<(63-0))
#define BQL_MCK_S1_HIT_SBE_THRESH      (1ULL<<(63-1)) 
#define BQL_MCK_S2_HIT_SBE_THRESH      (1ULL<<(63-2)) 
#define BQL_MCK_S3_HIT_SBE_THRESH      (1ULL<<(63-3))
#define BQL_MCK_S0_HIT_DBE_THRESH      (1ULL<<(63-4))
#define BQL_MCK_S1_HIT_DBE_THRESH      (1ULL<<(63-5))
#define BQL_MCK_S2_HIT_DBE_THRESH      (1ULL<<(63-6))
#define BQL_MCK_S3_HIT_DBE_THRESH      (1ULL<<(63-7))
#define BQL_MCK_S0_DBE_OBSERVED        (1ULL<<(63-8))
#define BQL_MCK_S1_DBE_OBSERVED        (1ULL<<(63-9))
#define BQL_MCK_S2_DBE_OBSERVED        (1ULL<<(63-10))
#define BQL_MCK_S3_DBE_OBSERVED        (1ULL<<(63-11))
// there is a dedicated 4G HSS core to each of the 4 switches
#define BQL_MCK_S0_4G_READYLOST        (1ULL<<(63-12))
#define BQL_MCK_S1_4G_READYLOST        (1ULL<<(63-13))
#define BQL_MCK_S2_4G_READYLOST        (1ULL<<(63-14))
#define BQL_MCK_S3_4G_READYLOST        (1ULL<<(63-15))
#define BQL_MCK_S0_4G_PLLLOST_LOCK     (1ULL<<(63-16))
#define BQL_MCK_S1_4G_PLLLOST_LOCK     (1ULL<<(63-17))
#define BQL_MCK_S2_4G_PLLLOST_LOCK     (1ULL<<(63-18))
#define BQL_MCK_S3_4G_PLLLOST_LOCK     (1ULL<<(63-19))
// The 10G HSS are shared in pairs for a pair of switches 0/1 and 2/3
#define BQL_MCK_S01_10G0_PLLLOST_LOCK  (1ULL<<(63-20))
#define BQL_MCK_S01_10G0B_PLLLOST_LOCK (1ULL<<(63-21))
#define BQL_MCK_S23_10G2_PLLLOST_LOCK  (1ULL<<(63-22))
#define BQL_MCK_S23_10G2B_PLLLOST_LOCK (1ULL<<(63-23))
#define BQL_MCK_S01_10G0_DEGRADED_EYE  (1ULL<<(63-24))
#define BQL_MCK_S01_10G0B_DEGRADED_EYE (1ULL<<(63-25))
#define BQL_MCK_S23_10G2_DEGRADED_EYE  (1ULL<<(63-26))
#define BQL_MCK_S23_10G2B_DEGRADED_EYE (1ULL<<(63-27))
// The Sparing logic reports on a specfic Rx or Tx basis
#define BQL_MCK_C01_SPARING_MISCFG     (1ULL<<(63-28))
#define BQL_MCK_D01_SPARING_MISCFG     (1ULL<<(63-29))
#define BQL_MCK_C23_SPARING_MISCFG     (1ULL<<(63-30))
#define BQL_MCK_D23_SPARING_MISCFG     (1ULL<<(63-31))


// Sparing Status Register
// Rx bits are in the first 32 bits of status register
#define BQL_SPARE_RX_STATUS_CFG          (1ULL<<(63-0))   // bit 0  -- might be useful depending on granularity of drill down
#define BQL_SPARE_RX_SHFTMSK_ERR         (1ULL<<(63-1))   // bit 1  -- shifts outside of bounds in 5/6 cfg or shift by 3 found
#define BQL_SPARE_RX_SP1_SPEC_ERR        (1ULL<<(63-11))  // bit 11 -- SP1 > 5 in 5/6, or > 11 in 10/12
#define BQL_SPARE_RX_SP1_SRC_ERR         (1ULL<<(63-12))  // bit 12 -- SP1 Src > 4 in 5/6, or > 9 in 10/12
#define BQL_SPARE_RX_SP1_SHFTMSK_ERR     (1ULL<<(63-13))  // bit 13 -- Data is shifted on top of SP1, yields corrupted data
#define BQL_SPARE_RX_HIT_SP1_ERR_THRESH  (1ULL<<(63-14))  // bit 14 -- SP1 is encountering errors
#define BQL_SPARE_RX_SP2_SPEC_ERR        (1ULL<<(63-26))  // bit 26 -- SP2 < 6 in 5/6 or > 11 in 10/12
#define BQL_SPARE_RX_SP2_SRC_ERR         (1ULL<<(63-27))  // bit 27 -- SP2 Src < 5 in 5/6 or > 9 in 10/12
#define BQL_SPARE_RX_SP2_SHFTMSK_ERR     (1ULL<<(63-28))  // bit 28 -- Data is shifted on top of SP2, yields corrupted data
#define BQL_SPARE_RX_HIT_SP2_ERR_THRESH  (1ULL<<(63-29))  // bit 29 -- SP1 is encountering errors
// Tx bits are in the econd 32 bits of status register
#define BQL_SPARE_TX_STATUS_CFG          (1ULL<<(63-38))  // bit 38 -- might be useful depending on granularity of drill down
#define BQL_SPARE_TX_SHFTMSK_ERR         (1ULL<<(63-39))  // bit 39 -- shifts outside of bounds in 5/6 cfg or shift by 3 found
#define BQL_SPARE_TX_SP1_SPEC_ERR        (1ULL<<(63-48))  // bit 48 -- SP1 > 5 in 5/6, or > 11 in 10/12
#define BQL_SPARE_TX_SP1_SRC_ERR         (1ULL<<(63-49))  // bit 49 -- SP1 Src > 4 in 5/6, or > 9 in 10/12
#define BQL_SPARE_TX_SP1_SHFTMSK_ERR     (1ULL<<(63-50))  // bit 50 -- Data is shifted on top of SP1, yields corrupted data
#define BQL_SPARE_TX_SP2_SPEC_ERR        (1ULL<<(63-60))  // bit 60 -- SP1 > 5 in 5/6, or > 11 in 10/12
#define BQL_SPARE_TX_SP2_SRC_ERR         (1ULL<<(63-61))  // bit 61 -- SP1 Src > 4 in 5/6, or > 9 in 10/12
#define BQL_SPARE_TX_SP2_SHFTMSK_ERR     (1ULL<<(63-62))  // bit 62 -- Data is shifted on top of SP1, yields corrupted data


// Clock Interface Status:  BQL_ClkIntf.pdf section 2.9.3
#define BQL_ACCESS_CLK_STAT_CMD_COL_ERR       (1ULL<<(63-0))   // bit 0 Command Collision -- new cmd while still processing prior cmd
#define BQL_ACCESS_CLK_STAT_CMD_SRC_ERR       (1ULL<<(63-1))   // bit 1 Invalid Command Source Error. Not from JTAG of [AHL]BIST
#define BQL_ACCESS_CLK_STAT_INVAL_RW_ADDR_ERR (1ULL<<(63-2))   // bit 2 Invalid Read/Write Address Error. 
#define BQL_ACCESS_CLK_STAT_JTAG_POR_COL_ERR  (1ULL<<(63-3))   // bit 3 JTAG/POR Test Collision Error. JTAG attempted command with POR in progress
#define BQL_ACCESS_CLK_STAT_JTAG_BIST_COL_ERR (1ULL<<(63-4))   // bit 4 JTAG/BIST Collision Error. JTAG attempted command with BIST in progress                                                     
// bit 5:  ABIST error -- unused, no arrays on BQL -- should never be unmasked
#define BQL_ACCESS_CLK_STAT_LBIST_ERROR       (1ULL<<(63-6))   // bit 6 LBIST Error - Command src says LBIST, but LBIST not in progress
// bit 7:  HBIST error -- unused, no HBIST -- should never be unmasked
#define BQL_ACCESS_CLK_STAT_SP_CMD_SRC_ERR(x) _B4(11,x)        // bits 8:11 Spare Command Source Error(0:3). Command 
#define BQL_ACCESS_CLK_STAT_POR_TEST_COMP     (1ULL<<(63-12))  // bit 12 POR Test Complete. 
#define BQL_ACCESS_CLK_STAT_REG_WR_ERR        (1ULL<<(63-13))  // bit 13 Register Write Error. 
#define BQL_ACCESS_CLK_STAT_CMD_SEQ_ERR       (1ULL<<(63-14))  // bit 14 Command Sequence Error. multiple domains not properly sequenced statrt/stop
#define BQL_ACCESS_CLK_STAT_CLK_CMD_ERR       (1ULL<<(63-15))  // bit 15 Clock Error. A reserved encode was used in Clk Cmd Reg. Treated as No-OP
#define BQL_ACCESS_CLK_STAT_PLL_UNLOCK(x)     _B8(23,x)        // bits 16:23 PLL_UNLOCK(0:7) State. If 1, PLL unlock detected. 
// The following do not raise CI_SCATTN
#define BQL_ACCESS_CLK_STAT_IPLMODE_STCLKS    (1ULL<<(63-24))  // bit 24 IPL Mode. Shows value of PORRUNCLKS 1 means start clocks
#define BQL_ACCESS_CLK_STAT_IPLMODE_RUN_LBIST (1ULL<<(63-25))  // bit 25 IPL Mode. shows value of PORRUNBIST(0) , 0 means run Lbist
#define BQL_ACCESS_CLK_STAT_IPLMODE_RUN_ABIST (1ULL<<(63-26))  // bit 26 IPL Mode. shows value of PORRUNBIST(1) , 0 means run Abist
#define BQL_ACCESS_CLK_STAT_IPL_MODE_RUN_MODE (1ULL<<(63-27))  // bit 27 IPL Mode. shows value of PORRUNBIST(2) , 0 means run Mode
#define BQL_ACCESS_CLK_STAT_IPLMODE_RUN_HBIST (1ULL<<(63-28))  // bit 28 IPL Mode. shows value of PORRUNBIST(3) , 0 means run Hbist
#define BQL_ACCESS_CLK_STAT_IPLMODE_SPARES(x) _BN3(31)         // bit 29:31 IPL Mode Spares. shows valueof PORRUNBIST(4:6) , 0 means run Mode
#define BQL_ACCESS_CLK_STAT_CLK_STATE(x)      _BN32(63)        // bit 32:63 Clock State bits , CLK_STATE(0:31) Indicates if a clock has Started (On)

#define BQL_CLK_TST_CTRL_CLOCK_OP_IP           (1ULL<<(63-0))//Clock Op In Progress. Set to 1b before issuing a clock operation command. Causes clock logic to take control of the user test ports.
#define BQL_CLK_TST_CTRL_USER_CE1_ASST_CG      (1ULL<<(63-1))
#define BQL_CLK_TST_CTRL_USER_CE1_DI1          (1ULL<<(63-2))
#define BQL_CLK_TST_CTRL_USER_CE1_DI2          (1ULL<<(63-3))
#define BQL_CLK_TST_CTRL_USER_CE1_RI           (1ULL<<(63-4))
#define BQL_CLK_TST_CTRL_USER_CE0_RRSE         (1ULL<<(63-5))
#define BQL_CLK_TST_CTRL_USER_CE0_TESTM1       (1ULL<<(63-6))
#define BQL_CLK_TST_CTRL_UNUSED_7              (1ULL<<(63-7))
#define BQL_CLK_TST_CTRL_USER_JTAG_HIGHZ       (1ULL<<(63-8))
#define BQL_CLK_TST_CTRL_USER_JTAG_MODEB       (1ULL<<(63-9))
#define BQL_CLK_TST_CTRL_USER_JTAG_MODEC       (1ULL<<(63-10))
#define BQL_CLK_TST_CTRL_UNUSED_11             (1ULL<<(63-11))
#define BQL_CLK_TST_CTRL_UNUSED_12             (1ULL<<(63-12))
#define BQL_CLK_TST_CTRL_USER_CE0_ASST_DLH     (1ULL<<(63-13))
#define BQL_CLK_TST_CTRL_USER_CE0_ASST_PLH     (1ULL<<(63-14))
#define BQL_CLK_TST_CTRL_USER_CE0_ASYNC_RXN    (1ULL<<(63-15))
#define BQL_CLK_TST_CTRL_USER_CE0_CLKEN        (1ULL<<(63-16))
#define BQL_CLK_TST_CTRL_USER_CE0_HSLT         (1ULL<<(63-17))
#define BQL_CLK_TST_CTRL_USER_CE0_LBIST_ACA    (1ULL<<(63-18))
#define BQL_CLK_TST_CTRL_USER_CE0_SEFCG        (1ULL<<(63-19))
#define BQL_CLK_TST_CTRL_USER_CE0_TEOVERRIDE   (1ULL<<(63-20))
#define BQL_CLK_TST_CTRL_USER_CE0_TESTM3       (1ULL<<(63-21))
#define BQL_CLK_TST_CTRL_USER_CE1_ASYNC_SX     (1ULL<<(63-22))
#define BQL_CLK_TST_CTRL_USER_CE0_TBON         (1ULL<<(63-23))
#define BQL_CLK_TST_CTRL_USER_CE0_NA_TBON      (1ULL<<(63-24))
#define BQL_CLK_TST_CTRL_USER_CE0_MEMCLKSEL    (1ULL<<(63-25))
#define BQL_CLK_TST_CTRL_USER_CE0_NA_MEMCLKSEL (1ULL<<(63-26))
#define BQL_CLK_TST_CTRL_USER_CE0_MPGSE        (1ULL<<(63-27))
#define BQL_CLK_TST_CTRL_USER_CE0_SCAN         (1ULL<<(63-28))
#define BQL_CLK_TST_CTRL_UNUSED_29             (1ULL<<(63-29))
#define BQL_CLK_TST_CTRL_UNUSED_30             (1ULL<<(63-30))
#define BQL_CLK_TST_CTRL_UNUSED_31             (1ULL<<(63-31))
#define BQL_CLK_TST_CTRL_USER_CE1_LPGC1        (1ULL<<(63-32))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_LPGC1     (1ULL<<(63-33))
#define BQL_CLK_TST_CTRL_USER_CE1_MPGC1        (1ULL<<(63-34))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_MPGC1     (1ULL<<(63-35))
#define BQL_CLK_TST_CTRL_USER_CE1_LPGC2        (1ULL<<(63-36))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_LPGC2     (1ULL<<(63-37))
#define BQL_CLK_TST_CTRL_USER_CE1_MNGC2        (1ULL<<(63-38))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_MNGC2     (1ULL<<(63-39))
#define BQL_CLK_TST_CTRL_USER_CE1_MPGC2        (1ULL<<(63-40))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_MPGC2     (1ULL<<(63-41))
#define BQL_CLK_TST_CTRL_USER_CE1_BCLK         (1ULL<<(63-42))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_BCLK      (1ULL<<(63-43))
#define BQL_CLK_TST_CTRL_USER_CE1_RA_C1        (1ULL<<(63-44))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_RA_C1     (1ULL<<(63-45))
#define BQL_CLK_TST_CTRL_USER_CE1_RA_C2        (1ULL<<(63-46))
#define BQL_CLK_TST_CTRL_USER_CE1_NA_RA_C2     (1ULL<<(63-47))
#define BQL_CLK_TST_CTRL_USER_CE1_LTLC         (1ULL<<(63-48))
#define BQL_CLK_TST_CTRL_USER_CE1_MTLC         (1ULL<<(63-49))
#define BQL_CLK_TST_CTRL_USER_CE0_ACLK         (1ULL<<(63-50))
#define BQL_CLK_TST_CTRL_USER_CE0_MNGB         (1ULL<<(63-51))
#define BQL_CLK_TST_CTRL_USER_CE0_RAB          (1ULL<<(63-52))
#define BQL_CLK_TST_CTRL_USER_CE0_ASST_LOS2R   (1ULL<<(63-53))
#define BQL_CLK_TST_CTRL_USER_CE0_ASST_LOS2F   (1ULL<<(63-54))
#define BQL_CLK_TST_CTRL_USER_CE1_ASST_GO      (1ULL<<(63-55))
#define BQL_CLK_TST_CTRL_UNUSED_56             (1ULL<<(63-56))
#define BQL_CLK_TST_CTRL_UNUSED_57             (1ULL<<(63-57))
#define BQL_CLK_TST_CTRL_UNUSED_58             (1ULL<<(63-58))
#define BQL_CLK_TST_CTRL_UNUSED_59             (1ULL<<(63-59))
#define BQL_CLK_TST_CTRL_UNUSED_60             (1ULL<<(63-60))
#define BQL_CLK_TST_CTRL_UNUSED_61             (1ULL<<(63-61))
#define BQL_CLK_TST_CTRL_UNUSED_62             (1ULL<<(63-62))
#define BQL_CLK_TST_CTRL_UNUSED_63             (1ULL<<(63-63))

#endif
