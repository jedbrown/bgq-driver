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

#include "Kernel.h"
#include "flih.h"
#include <hwi/include/bqc/BIC_inlines.h>
#include <hwi/include/bqc/wu_mmio.h>
#include <firmware/include/Firmware_Interrupts.h>
#include <spi/include/upci/upci_syscall.h>
#include <hwi/include/bqc/upc_c_dcr.h>
#include <spi/include/wu/wait.h>

#define USE_CNTLZ_IN_INTHANDLER 1

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
// THREAD: Which thread(s) are enabled to receive the interrupt. 0,1,2,3 or (-1)=all
// CONFIG_ENABLE: indicator used to control when/if a particular class of interrupts is enabled/disabled
// HANDLER: which interrupt handler function should be invoked with this condition occurs
// BIT: indicates the bit position of this interrupt within the PUEA Interrupt Summary register
//
// NOTE: Setting of a BIC_MACHINE_CHECK target should never be done by the kernel. The firmware will
//       map the PUEA lines that it wants treated as machine checks prior to launching the kernel.
//       The firmware will communicate which PUEA inputs are handled/mapped by firmware through the
//       FW_RESERVED_PUEA_STATUS mask, provided at compile-time.
//
PUEA_Table puea_table[ FLIH_PUEA_TABLE_SIZE ] =
{
// NOTE: COLUMNS 1, 2 and 3 INDICATE WHAT CORE/THREADS ARE TO RECEIVE THE EVENT AND 
//       WHAT LEVEL THAT EVENT SHOULD BE DELIVERED AT.   

//                                                                                                          SUMMARY
// COREMASK    THREAD     LEVEL                   GROUP_ENABLE   MAP                  1st LEVEL HANDLER       BIT  EVENT DESCRIPTION
// ----------  ---------  ---------------------   ------------  -------------------   -----------------      ----  ---------   
{  PUEA_RESERVED_HARDWARE                                                                                  },// 0:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 1:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 2:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 3:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 4:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 5:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 6:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 7:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 8:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 9:  Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 10: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 11: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 12: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 13: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 14: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 15: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 16: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 17: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 18: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 19: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 20: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 21: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 22: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 23: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 24: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 25: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 26: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 27: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 28: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 29: Undefined
//------------L2 Central ---------------------------------------------------------------------
{  CORES_ALL, THDMSK(0), BIC_EXTERNAL_INTERRUPT,  FLIH_GRP_BIC, BIC_INT30_MAPOFFSET,  IntHandler_L2Central },// 30: L2 Central 0 
{  CORES_ALL, THDMSK(1), BIC_EXTERNAL_INTERRUPT,  FLIH_GRP_BIC, BIC_INT31_MAPOFFSET,  IntHandler_L2Central },// 31: L2 Central 1 
{  CORES_ALL, THDMSK(2), BIC_EXTERNAL_INTERRUPT,  FLIH_GRP_BIC, BIC_INT32_MAPOFFSET,  IntHandler_L2Central },// 32: L2 Central 2
{  CORES_ALL, THDMSK(3), BIC_EXTERNAL_INTERRUPT,  FLIH_GRP_BIC, BIC_INT33_MAPOFFSET,  IntHandler_L2Central },// 33: L2 Central 3
//---------------------------------------------------------------------------------------------
{  PUEA_RESERVED_HARDWARE                                                                                  },// 34: Undefined
{  PUEA_RESERVED_HARDWARE                                                                                  },// 35: Undefined
//------------Wakeup Unit---------------------------------------------------------------------
{  CORES_ALL,  THDS_ALL,  BIC_EXTERNAL_INTERRUPT, FLIH_GRP_BIC, BIC_INT36_MAPOFFSET,  IntHandler_WAC       },// 36: Wakeup Unit Address compare
//------------Message Unit----------------------------------------
{  CORES_ALL,  THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_MSG,  BIC_INT37_MAPOFFSET, IntHandler_MU        },// 37: Message Unit 0 (modifiable target)
{  CORES_ALL,  THDMSK(1), BIC_NO_INTERRUPT,       FLIH_GRP_MSG,  BIC_INT38_MAPOFFSET, IntHandler_MU        },// 38: Message Unit 1 (modifiable target)
{  CORES_ALL,  THDMSK(2), BIC_NO_INTERRUPT,       FLIH_GRP_MSG,  BIC_INT39_MAPOFFSET, IntHandler_MU        },// 39: Message Unit 2 (modifiable target)
{  CORES_ALL,  THDMSK(3), BIC_NO_INTERRUPT,       FLIH_GRP_MSG,  BIC_INT40_MAPOFFSET, IntHandler_MU        },// 40: Message Unit 3 (modifiable target)
//------------Global Event Aggregator---------------------------------------------------------
{  COREMSK(0), THDMSK(0), BIC_MACHINE_CHECK,      FLIH_GRP_GEA,  BIC_INT41_MAPOFFSET, IntHandler_GEA_FLIH  },// 41: GEA 0 (used for firmware machine check handling)
{  COREMSK(0), THDMSK(0), BIC_CRITICAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT42_MAPOFFSET, IntHandler_GEA_FLIH  },// 42: GEA 1 This lane reserved for signals that are not expected 
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_MSG,  BIC_INT43_MAPOFFSET, IntHandler_MU        },// 43: GEA 2 This lane reserved for Messaging Unit Interrupts
{  COREMSK(0), THDMSK(0), BIC_EXTERNAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT44_MAPOFFSET, IntHandler_GEA_FLIH  },// 44: GEA 3 ND Software errors
{  COREMSK(16),THDMSK(1), BIC_EXTERNAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT45_MAPOFFSET, IntHandler_GEA_FLIH  },// 45: GEA 4 UPC Errors
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_GEA,  BIC_INT46_MAPOFFSET, IntHandler_GEA_FLIH  },// 46: GEA 5
{  COREMSK(16),THDMSK(1), BIC_EXTERNAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT47_MAPOFFSET, IntHandler_GEA_FLIH  },// 47: GEA 6  Scrubbing
{  COREMSK(0), THDMSK(0), BIC_EXTERNAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT48_MAPOFFSET, IntHandler_GEA_FLIH  },// 48: GEA 7  DCR Violations
{  COREMSK(16),THDMSK(1), BIC_CRITICAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT49_MAPOFFSET, IntHandler_GEA_FLIH  },// 49: GEA 8  Inbound Mailbox
{  COREMSK(16),THDMSK(1), BIC_EXTERNAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT50_MAPOFFSET, IntHandler_GEA_FLIH  },// 50: GEA 9  Power threshold
{  CORES_ALL,  THDS_ALL,  BIC_EXTERNAL_INTERRUPT, FLIH_GRP_GEA,  BIC_INT51_MAPOFFSET, IntHandler_GEA_FLIH  },// 51: GEA 10 GEA Timer Event
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_GEA,  BIC_INT52_MAPOFFSET, IntHandler_GEA_FLIH  },// 52: GEA 11 Nanosleep preempt timer
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_GEA,  BIC_INT53_MAPOFFSET, IntHandler_GEA_FLIH  },// 53: GEA 12 (also goes to wakeup unit)
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_GEA,  BIC_INT54_MAPOFFSET, IntHandler_GEA_FLIH  },// 54: GEA 13 (also goes to wakeup unit)
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_GEA,  BIC_INT55_MAPOFFSET, IntHandler_GEA_FLIH  },// 55: GEA 14 (also goes to wakeup unit)
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_GEA,  BIC_INT56_MAPOFFSET, IntHandler_GEA_FLIH  },// 56: GEA 15 (also goes to wakeup unit)
//------------L1P BIC -----------------------------------------------------------------------                                                                 
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_BIC,  BIC_INT57_MAPOFFSET, IntHandler_Default   },// 57: L1P BIC 0
{  CORES_ALL,  THDS_ALL,  BIC_EXTERNAL_INTERRUPT, FLIH_GRP_BIC,  BIC_INT58_MAPOFFSET, IntHandler_L1DCRV    },// 58: L1P BIC 1
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_BIC,  BIC_INT59_MAPOFFSET, IntHandler_Default   },// 59: L1P BIC 2
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_BIC,  BIC_INT60_MAPOFFSET, IntHandler_Default   },// 60: L1P BIC 3
{  COREMSK(0), THDMSK(0), BIC_NO_INTERRUPT,       FLIH_GRP_BIC,  BIC_INT61_MAPOFFSET, IntHandler_Default   },// 61: L1P BIC 4
// -----------Core 2 Core---------------------------------------------------------------------                                                                
{      0,         0,      BIC_EXTERNAL_INTERRUPT, FLIH_GRP_C2C,           -1,         IntHandler_IPI_FLIH  }, // 62: Core2Core Int 0 
{      0,         0,      BIC_EXTERNAL_INTERRUPT, FLIH_GRP_C2C,           -1,         IntHandler_IPI_FLIH17}// 63: Core2Core Int 1  
//----------------------------------------------------------------------------------------------
};


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
// Setting of a lane 0 target should never be done by the kernel. The firmware will
// map the signals that it wants treated as machine checks to lane 0 prior to reaching the kernel code.
// The firmware will communicate which GEA inputs are handled by firmware through the
// FW_RESERVED_GEA_STATUS_[0,1,2] masks, provided at compile-time. The kernel 
// will only map table entries that are marked as not being reserved by the firmware, therefore the 
// entries shown in the table below are only applied to the mappings if the firmware has not reserved that entry.
// Any entries that the kernel does not explicitly want to control will be directed down GEA lane 1 into the PUEA.
//
GEA_Table gea_stat_table[FLIH_GEA_NUM_STATUS_REGS][ FLIH_GEA_NUM_STATUS_BITS ] =
{
{
//                                                                  STAT STAT    
//  GEA_X  MAP REG,OFFSET          2nd LEVEL GEA HANDLER            REG  BIT  NAME            EVENT DESCRIPTION
// ------  ----------------------  -----------------------          ----  --- ------------    -------------------- 
{GEA_UPC,   GEA_INT0_00_MAPOFFSET, IntHandler_GEA_UPC           },// 0    0   upc_rt_int      UPC interrupt bit 0  
{GEA_NOCFG, GEA_INT0_01_MAPOFFSET, IntHandler_GEA_Default       },// 0    1   db_rt_int       Devbus interrupt bit 0  
{GEA_NOCFG, GEA_INT0_02_MAPOFFSET, IntHandler_GEA_Default       },// 0    2   l1p0_rt_int     L1P0 interrupt bit 0                                             
{GEA_NOCFG, GEA_INT0_03_MAPOFFSET, IntHandler_GEA_Default       },// 0    3   l1p1_rt_int     L1P1 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_04_MAPOFFSET, IntHandler_GEA_Default       },// 0    4   l1p2_rt_int     L1P2 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_05_MAPOFFSET, IntHandler_GEA_Default       },// 0    5   l1p3_rt_int     L1P3 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_06_MAPOFFSET, IntHandler_GEA_Default       },// 0    6   l1p4_rt_int     L1P4 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_07_MAPOFFSET, IntHandler_GEA_Default       },// 0    7   l1p5_rt_int     L1P5 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_08_MAPOFFSET, IntHandler_GEA_Default       },// 0    8   l1p6_rt_int     L1P6 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_09_MAPOFFSET, IntHandler_GEA_Default       },// 0    9   l1p7_rt_int     L1P7 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_10_MAPOFFSET, IntHandler_GEA_Default       },// 0    10  l1p8_rt_int     L1P8 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_11_MAPOFFSET, IntHandler_GEA_Default       },// 0    11  l1p9_rt_int     L1P9 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_12_MAPOFFSET, IntHandler_GEA_Default       },// 0    12  l1p10_rt_int    L1P10 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_13_MAPOFFSET, IntHandler_GEA_Default       },// 0    13  l1p11_rt_int    L1P11 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_14_MAPOFFSET, IntHandler_GEA_Default       },// 0    14  l1p12_rt_int    L1P12 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_15_MAPOFFSET, IntHandler_GEA_Default       },// 0    15  l1p13_rt_int    L1P13 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_16_MAPOFFSET, IntHandler_GEA_Default       },// 0    16  l1p14_rt_int    L1P14 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_17_MAPOFFSET, IntHandler_GEA_Default       },// 0    17  l1p15_rt_int    L1P15 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_18_MAPOFFSET, IntHandler_GEA_Default       },// 0    18  l1p16_rt_int    L1P16 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_19_MAPOFFSET, IntHandler_GEA_Default       },// 0    19  pcie_rt_int     PCIE interrupt bit 0  
{GEA_NOCFG, GEA_INT0_20_MAPOFFSET, IntHandler_GEA_Default       },// 0    20  sd0_rt_int      Serdes0 interrupt bit 0   
{GEA_NOCFG, GEA_INT0_21_MAPOFFSET, IntHandler_GEA_Default       },// 0    21  sd1_rt_int      Serdes1 interrupt bit 0 
{GEA_NOCFG, GEA_INT0_22_MAPOFFSET, IntHandler_GEA_Default       },// 0    22  db_pcie_rt_int  Devbus PCIE interrupt bit 0  
{GEA_NOCFG, GEA_INT0_23_MAPOFFSET, IntHandler_GEA_Default       },// 0    23  db_pcie_rt_int  Devbus PCIE interrupt bit 1  
{GEA_NOCFG, GEA_INT0_24_MAPOFFSET, IntHandler_GEA_Default       },// 0    24  db_pcie_rt_int  Devbus PCIE interrupt bit 2  
{GEA_NOCFG, GEA_INT0_25_MAPOFFSET, IntHandler_GEA_Default       },// 0    25  db_pcie_rt_int  Devbus PCIE interrupt bit 3  
{GEA_RESERVED_HARDWARE                                          },// 0    26  Undefined  
{GEA_RESERVED_HARDWARE                                          },// 0    27  envmon_int0_rt    
{GEA_RESERVED_HARDWARE                                          },// 0    28  Undefined
{GEA_RESERVED_HARDWARE                                          },// 0    29  Undefined    
{GEA_RESERVED_HARDWARE                                          },// 0    30  Undefined    
{GEA_DCRV,  GEA_INT0_31_MAPOFFSET, IntHandler_DCRUserViolation  },// 0    31  dcr_arb_rt_int  DCR Arbiter interrupt bit 0  
{GEA_NOCFG, GEA_INT0_32_MAPOFFSET, IntHandler_GEA_Default       },// 0    32  testint_rt_int  Testint interrupt bit 0  
{GEA_MU,    GEA_INT0_33_MAPOFFSET,NULL_INTHANDLER_GEA           },// 0    33  mu_rt_int       Message Unit interrupt bit 0 - Comm Handler intercepted at PUEA level 
{GEA_MU,    GEA_INT0_34_MAPOFFSET,NULL_INTHANDLER_GEA           },// 0    34  mu_rt_int       Message Unit interrupt bit 1 - Comm Handler intercepted at PUEA level
{GEA_MU,    GEA_INT0_35_MAPOFFSET,NULL_INTHANDLER_GEA           },// 0    35  mu_rt_int       Message Unit interrupt bit 2 - Comm Handler intercepted at PUEA level  
{GEA_MU,    GEA_INT0_36_MAPOFFSET,NULL_INTHANDLER_GEA           },// 0    36  mu_rt_int       Message Unit interrupt bit 3 - Comm Handler intercepted at PUEA level  
{GEA_MU,    GEA_INT0_37_MAPOFFSET,NULL_INTHANDLER_GEA           },// 0    37  mu_rt_int       Message Unit interrupt bit 4 - Comm Handler intercepted at PUEA level  
{GEA_MU,    GEA_INT0_38_MAPOFFSET,NULL_INTHANDLER_GEA           },// 0    38  mu_rt_int       Message Unit interrupt bit 5 - Comm Handler intercepted at PUEA level  
{GEA_NOCFG, GEA_INT0_39_MAPOFFSET, IntHandler_GEA_Default       },// 0    39  ddr0_rt_int     DDR Controller0 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_40_MAPOFFSET, IntHandler_GEA_Default       },// 0    40  ddr1_rt_int     DDR Controller1 interrupt bit 0  
{GEA_NOCFG, GEA_INT0_41_MAPOFFSET, IntHandler_GEA_Default       },// 0    41  sw_rt_int       Switch interrupt bit 0   
{GEA_NOCFG, GEA_INT0_42_MAPOFFSET, IntHandler_GEA_Default       },// 0    42  edram_rt_int    EDRAM interrupt bit 0  
{GEA_NOCFG, GEA_INT0_43_MAPOFFSET, IntHandler_GEA_Default       },// 0    43  l1p0_rt_int2    L1P0 interrupt bit 1
{GEA_NOCFG, GEA_INT0_44_MAPOFFSET, IntHandler_GEA_Default       },// 0    44  l1p1_rt_int2    L1P1 interrupt bit 1
{GEA_NOCFG, GEA_INT0_45_MAPOFFSET, IntHandler_GEA_Default       },// 0    45  l1p2_rt_int2    L1P2 interrupt bit 1
{GEA_NOCFG, GEA_INT0_46_MAPOFFSET, IntHandler_GEA_Default       },// 0    46  l1p3_rt_int2    L1P3 interrupt bit 1
{GEA_NOCFG, GEA_INT0_47_MAPOFFSET, IntHandler_GEA_Default       },// 0    47  l1p4_rt_int2    L1P4 interrupt bit 1
{GEA_NOCFG, GEA_INT0_48_MAPOFFSET, IntHandler_GEA_Default       },// 0    48  l1p5_rt_int2    L1P5 interrupt bit 1
{GEA_NOCFG, GEA_INT0_49_MAPOFFSET, IntHandler_GEA_Default       },// 0    49  l1p6_rt_int2    L1P6 interrupt bit 1   
{GEA_NOCFG, GEA_INT0_50_MAPOFFSET, IntHandler_GEA_Default       },// 0    50  l1p7_rt_int2    L1P7 interrupt bit 1
{GEA_NOCFG, GEA_INT0_51_MAPOFFSET, IntHandler_GEA_Default       },// 0    51  l1p8_rt_int2    L1P8 interrupt bit 1
{GEA_NOCFG, GEA_INT0_52_MAPOFFSET, IntHandler_GEA_Default       },// 0    52  l1p9_rt_int2    L1P9 interrupt bit 1
{GEA_NOCFG, GEA_INT0_53_MAPOFFSET, IntHandler_GEA_Default       },// 0    53  l1p10_rt_int2   L1P10 interrupt bit 1
{GEA_NOCFG, GEA_INT0_54_MAPOFFSET, IntHandler_GEA_Default       },// 0    54  l1p11_rt_int2   L1P11 interrupt bit 1
{GEA_NOCFG, GEA_INT0_55_MAPOFFSET, IntHandler_GEA_Default       },// 0    55  l1p12_rt_int2   L1P12 interrupt bit 1
{GEA_NOCFG, GEA_INT0_56_MAPOFFSET, IntHandler_GEA_Default       },// 0    56  l1p13_rt_int2   L1P13 interrupt bit 1
{GEA_NOCFG, GEA_INT0_57_MAPOFFSET, IntHandler_GEA_Default       },// 0    57  l1p14_rt_int2   L1P14 interrupt bit 1
{GEA_NOCFG, GEA_INT0_58_MAPOFFSET, IntHandler_GEA_Default       },// 0    58  l1p15_rt_int2   L1P15 interrupt bit 1
{GEA_NOCFG, GEA_INT0_59_MAPOFFSET, IntHandler_GEA_Default       },// 0    59  l1p16_rt_int2   L1P16 interrupt bit 1
{GEA_RESERVED_HARDWARE                                          },// 0    60  Undefined   
{GEA_SCRUB, GEA_INT0_61_MAPOFFSET, IntHandler_GEATimerEvent     },// 0    61  gea_rt_int      GEA interrupt bit 0  
{GEA_RESERVED_HARDWARE                                          },// 0    62  Undefined  
{GEA_RESERVED_HARDWARE                                          } // 0    63  Undefined  
},{
//-----------------------------------------------------------------------------------------------
//                     GEA INTERRUPT STATUS REGISTER 1 START       
//-----------------------------------------------------------------------------------------------
{GEA_UPC,   GEA_INT1_00_MAPOFFSET, IntHandler_GEA_UPC           },// 1    0   upc_int         UPC interrupt bit 1  
{GEA_NOCFG, GEA_INT1_01_MAPOFFSET, IntHandler_GEA_Default       },// 1    1   db_int          Devbus interrupt bit 1  
{GEA_NOCFG, GEA_INT1_02_MAPOFFSET, IntHandler_GEA_Default       },// 1    2   l1p0_int        L1P0 interrupt bit 2                                             
{GEA_NOCFG, GEA_INT1_03_MAPOFFSET, IntHandler_GEA_Default       },// 1    3   l1p1_int        L1P1 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_04_MAPOFFSET, IntHandler_GEA_Default       },// 1    4   l1p2_int        L1P2 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_05_MAPOFFSET, IntHandler_GEA_Default       },// 1    5   l1p3_int        L1P3 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_06_MAPOFFSET, IntHandler_GEA_Default       },// 1    6   l1p4_int        L1P4 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_07_MAPOFFSET, IntHandler_GEA_Default       },// 1    7   l1p5_int        L1P5 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_08_MAPOFFSET, IntHandler_GEA_Default       },// 1    8   l1p6_int        L1P6 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_09_MAPOFFSET, IntHandler_GEA_Default       },// 1    9   l1p7_int        L1P7 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_10_MAPOFFSET, IntHandler_GEA_Default       },// 1    10  l1p8_int        L1P8 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_11_MAPOFFSET, IntHandler_GEA_Default       },// 1    11  l1p9_int        L1P9 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_12_MAPOFFSET, IntHandler_GEA_Default       },// 1    12  l1p10_int       L1P10 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_13_MAPOFFSET, IntHandler_GEA_Default       },// 1    13  l1p11_int       L1P11 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_14_MAPOFFSET, IntHandler_GEA_Default       },// 1    14  l1p12_int       L1P12 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_15_MAPOFFSET, IntHandler_GEA_Default       },// 1    15  l1p13_int       L1P13 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_16_MAPOFFSET, IntHandler_GEA_Default       },// 1    16  l1p14_int       L1P14 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_17_MAPOFFSET, IntHandler_GEA_Default       },// 1    17  l1p15_int       L1P15 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_18_MAPOFFSET, IntHandler_GEA_Default       },// 1    18  l1p16_int       L1P16 interrupt bit 2  
{GEA_NOCFG, GEA_INT1_19_MAPOFFSET, IntHandler_GEA_Default       },// 1    19  pcie_int        PCIE interrupt bit 1  
{GEA_NOCFG, GEA_INT1_20_MAPOFFSET, IntHandler_GEA_Default       },// 1    20  sd0_int         Serdes0 interrupt bit 1   
{GEA_NOCFG, GEA_INT1_21_MAPOFFSET, IntHandler_GEA_Default       },// 1    21  sd0_int         Serdes1 interrupt bit 1  
{GEA_RESERVED_HARDWARE                                          },// 1    22  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    23  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    24  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    25  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    26  Undefined
{GEA_PWRTH, GEA_INT1_27_MAPOFFSET, IntHandler_PowerEvent        },// 1    27  envmon_int1
{GEA_RESERVED_HARDWARE                                          },// 1    28  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    29  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    30  Undefined
{GEA_NOCFG ,GEA_INT1_31_MAPOFFSET, IntHandler_GEA_Default       },// 1    31  dcr_arb_int     DCR Arbiter interrupt bit 1  
{GEA_MBOX,  GEA_INT1_32_MAPOFFSET, IntHandler_MailboxIn         },// 1    32  testint_int     Testint interrupt bit 1  
{GEA_ND,    GEA_INT1_33_MAPOFFSET, IntHandler_ND                },// 1    33  mu_int          Message Unit interrupt bit 6   
{GEA_RESERVED_HARDWARE                                          },// 1    34  Undefined
{GEA_NOCFG, GEA_INT1_35_MAPOFFSET, IntHandler_GEA_Default       },// 1    35  
{GEA_RESERVED_HARDWARE                                          },// 1    36  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    37  Undefined
{GEA_RESERVED_HARDWARE                                          },// 1    38  Undefined
{GEA_NOCFG, GEA_INT1_39_MAPOFFSET, IntHandler_GEA_Default       },// 1    39  ddr0_int        DDR Controller0 interrupt bit 1  
{GEA_NOCFG, GEA_INT1_40_MAPOFFSET, IntHandler_GEA_Default       },// 1    40  ddr1_int        DDR Controller1 interrupt bit 1  
{GEA_NOCFG, GEA_INT1_41_MAPOFFSET, IntHandler_GEA_Default       },// 1    41  sw_int          Switch interrupt bit 1  
{GEA_NOCFG, GEA_INT1_42_MAPOFFSET, IntHandler_GEA_Default       },// 1    42  edram_int       EDRAM interrupt bit 1  
{GEA_NOCFG, GEA_INT1_43_MAPOFFSET, IntHandler_GEA_Default       },// 1    43  l2s_int         L2 slice 0 interrupt bit  
{GEA_NOCFG, GEA_INT1_44_MAPOFFSET, IntHandler_GEA_Default       },// 1    44  l2s_int         L2 slice 1 interrupt bit  
{GEA_NOCFG, GEA_INT1_45_MAPOFFSET, IntHandler_GEA_Default       },// 1    45  l2s_int         L2 slice 2 interrupt bit  
{GEA_NOCFG, GEA_INT1_46_MAPOFFSET, IntHandler_GEA_Default       },// 1    46  l2s_int         L2 slice 3 interrupt bit  
{GEA_NOCFG, GEA_INT1_47_MAPOFFSET, IntHandler_GEA_Default       },// 1    47  l2s_int         L2 slice 4 interrupt bit  
{GEA_NOCFG, GEA_INT1_48_MAPOFFSET, IntHandler_GEA_Default       },// 1    48  l2s_int         L2 slice 5 interrupt bit  
{GEA_NOCFG, GEA_INT1_49_MAPOFFSET, IntHandler_GEA_Default       },// 1    49  l2s_int         L2 slice 6 interrupt bit  
{GEA_NOCFG, GEA_INT1_50_MAPOFFSET, IntHandler_GEA_Default       },// 1    50  l2s_int         L2 slice 7 interrupt bit  
{GEA_NOCFG, GEA_INT1_51_MAPOFFSET, IntHandler_GEA_Default       },// 1    51  l2s_int         L2 slice 8 interrupt bit  
{GEA_NOCFG, GEA_INT1_52_MAPOFFSET, IntHandler_GEA_Default       },// 1    52  l2s_int         L2 slice 9 interrupt bit  
{GEA_NOCFG, GEA_INT1_53_MAPOFFSET, IntHandler_GEA_Default       },// 1    53  l2s_int         L2 slice 10 interrupt bit  
{GEA_NOCFG, GEA_INT1_54_MAPOFFSET, IntHandler_GEA_Default       },// 1    54  l2s_int         L2 slice 11 interrupt bit  
{GEA_NOCFG, GEA_INT1_55_MAPOFFSET, IntHandler_GEA_Default       },// 1    55  l2s_int         L2 slice 12 interrupt bit  
{GEA_NOCFG, GEA_INT1_56_MAPOFFSET, IntHandler_GEA_Default       },// 1    56  l2s_int         L2 slice 13 interrupt bit  
{GEA_NOCFG, GEA_INT1_57_MAPOFFSET, IntHandler_GEA_Default       },// 1    57  l2s_int         L2 slice 14 interrupt bit  
{GEA_NOCFG, GEA_INT1_58_MAPOFFSET, IntHandler_GEA_Default       },// 1    58  l2s_int         L2 slice 15 interrupt bit  
{GEA_RESERVED_HARDWARE                                          },// 1    59  Undefined
{GEA_NOCFG, GEA_INT1_60_MAPOFFSET, IntHandler_GEA_Default       },// 1    60  l2c_int         L2 Central interrupt bit 0 asserted
{GEA_PWRMG, GEA_INT1_61_MAPOFFSET, IntHandler_GEATimerEvent     },// 1    61  gea_int         GEA interrupt bit 1       
{GEA_NOCFG, GEA_INT1_62_MAPOFFSET, IntHandler_GEA_Default       },// 1    62  nd_int          Network interrupt bit 0 is asserted
{GEA_RESERVED_HARDWARE                                          } // 1    63  Undefined 
},{
//-----------------------------------------------------------------------------------------------
//                     GEA INTERRUPT STATUS REGISTER  2 START       
//-----------------------------------------------------------------------------------------------
{GEA_UPC,   GEA_INT2_00_MAPOFFSET, IntHandler_GEA_UPC           },// 2    0   upc_int         UPC interrupt bit 2  
{GEA_NOCFG, GEA_INT2_01_MAPOFFSET, IntHandler_GEA_Default       },// 2    1   db_int          Devbus interrupt bit 2  
{GEA_NOCFG, GEA_INT2_02_MAPOFFSET, IntHandler_GEA_Default       },// 2    2   l1p0_int        L1P0 interrupt bit 3                                             
{GEA_NOCFG, GEA_INT2_03_MAPOFFSET, IntHandler_GEA_Default       },// 2    3   l1p1_int        L1P1 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_04_MAPOFFSET, IntHandler_GEA_Default       },// 2    4   l1p2_int        L1P2 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_05_MAPOFFSET, IntHandler_GEA_Default       },// 2    5   l1p3_int        L1P3 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_06_MAPOFFSET, IntHandler_GEA_Default       },// 2    6   l1p4_int        L1P4 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_07_MAPOFFSET, IntHandler_GEA_Default       },// 2    7   l1p5_int        L1P5 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_08_MAPOFFSET, IntHandler_GEA_Default       },// 2    8   l1p6_int        L1P6 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_09_MAPOFFSET, IntHandler_GEA_Default       },// 2    9   l1p7_int        L1P7 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_10_MAPOFFSET, IntHandler_GEA_Default       },// 2    10  l1p8_int        L1P8 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_11_MAPOFFSET, IntHandler_GEA_Default       },// 2    11  l1p9_int        L1P9 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_12_MAPOFFSET, IntHandler_GEA_Default       },// 2    12  l1p10_int       L1P10 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_13_MAPOFFSET, IntHandler_GEA_Default       },// 2    13  l1p11_int       L1P11 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_14_MAPOFFSET, IntHandler_GEA_Default       },// 2    14  l1p12_int       L1P12 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_15_MAPOFFSET, IntHandler_GEA_Default       },// 2    15  l1p13_int       L1P13 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_16_MAPOFFSET, IntHandler_GEA_Default       },// 2    16  l1p14_int       L1P14 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_17_MAPOFFSET, IntHandler_GEA_Default       },// 2    17  l1p15_int       L1P15 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_18_MAPOFFSET, IntHandler_GEA_Default       },// 2    18  l1p16_int       L1P16 interrupt bit 3  
{GEA_NOCFG, GEA_INT2_19_MAPOFFSET, IntHandler_GEA_Default       },// 2    19  pcie_int        PCIE interrupt bit 2  
{GEA_NOCFG, GEA_INT2_20_MAPOFFSET, IntHandler_GEA_Default       },// 2    20  sd0_int         Serdes0 interrupt bit 2   
{GEA_NOCFG, GEA_INT2_21_MAPOFFSET, IntHandler_GEA_Default       },// 2    21  sd1_int         Serdes1 interrupt bit 2  
{GEA_RESERVED_HARDWARE                                          },// 2    22  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    23  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    24  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    25  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    26  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    27  envmon_int2
{GEA_RESERVED_HARDWARE                                          },// 2    28  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    29  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    30  Undefined
{GEA_NOCFG, GEA_INT2_31_MAPOFFSET, IntHandler_GEA_Default       },// 2    31  dcr_arb_int     DCR Arbiter interrupt bit 2  
{GEA_NOCFG, GEA_INT2_32_MAPOFFSET, IntHandler_GEA_Default       },// 2    32  testint_int     Testint interrupt bit 2  
{GEA_NOCFG, GEA_INT2_33_MAPOFFSET, IntHandler_GEA_Default       },// 2    33  mu_int          Message Unit interrupt bit 7 - Machine check  
{GEA_RESERVED_HARDWARE                                          },// 2    34  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    35  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    36  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    37  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    38  Undefined
{GEA_NOCFG, GEA_INT2_39_MAPOFFSET, IntHandler_GEA_Default       },// 2    39  ddr0_int        DDR Controller0 interrupt bit 2  
{GEA_NOCFG, GEA_INT2_40_MAPOFFSET, IntHandler_GEA_Default       },// 2    40  ddr1_int        DDR Controller1 interrupt bit 2  
{GEA_NOCFG, GEA_INT2_41_MAPOFFSET, IntHandler_GEA_Default       },// 2    41  sw_int          Switch interrupt bit 2  
{GEA_NOCFG, GEA_INT2_42_MAPOFFSET, IntHandler_GEA_Default       },// 2    42  edram_int       EDRAM interrupt bit 2  
{GEA_NOCFG, GEA_INT2_43_MAPOFFSET, IntHandler_GEA_Default       },// 2    43  l2cntr0_int     L2 counter 0 interrupt     
{GEA_NOCFG, GEA_INT2_44_MAPOFFSET, IntHandler_GEA_Default       },// 2    44  l2cntr1_int     L2 counter 1 interrupt
{GEA_NOCFG, GEA_INT2_45_MAPOFFSET, IntHandler_GEA_Default       },// 2    45  l2cntr2_int     L2 counter 2 interrupt
{GEA_NOCFG, GEA_INT2_46_MAPOFFSET, IntHandler_GEA_Default       },// 2    46  l2cntr3_int     L2 counter 3 interrupt
{GEA_NOCFG, GEA_INT2_47_MAPOFFSET, IntHandler_GEA_Default       },// 2    47  msgc_int        Msync generation counter interrupt is asserted
{GEA_RESERVED_HARDWARE                                          },// 2    48  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    49  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    50  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    51  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    52  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    53  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    54  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    55  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    56  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    57  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    58  Undefined
{GEA_RESERVED_HARDWARE                                          },// 2    59  Undefined
{GEA_NOCFG, GEA_INT2_60_MAPOFFSET, IntHandler_GEA_Default       },// 2    60  l2c_int         L2 Central interrupt bit 1 asserted
{GEA_NOCFG, GEA_INT2_61_MAPOFFSET, IntHandler_GEA_Default       },// 2    61  gea_int         GEA interrupt bit 2       
{GEA_ND,    GEA_INT2_62_MAPOFFSET, IntHandler_ND                },// 2    62  nd_int          Network interrupt bit 1 is asserted
{GEA_RESERVED_HARDWARE                                          } // 2    63  Undefined
}
//----------------------------------------------------------------------------------------------
};

// Set the thread to be used by the firmware for machine check handling. The default core and thread in the table 
// above will be overridden by this function call.
void puea_set_mc_handler_thread(int hwthreadid)
{
    int table_offset = cntlz64(PUEA_INTERRUPT_STATUS_GEA(0));
    puea_table[table_offset].core_enable = COREMSK(hwthreadid>>2);
    puea_table[table_offset].thread_enable = THDMSK(hwthreadid & 0x3); 
}

// Set the PUEA mapping registers in the current hardware thread
void puea_init()
{
    // get our core
    uint64_t my_core_mask = COREMSK(ProcessorCoreID());
    uint8_t my_thread_mask = THDMSK(ProcessorThreadID());
    uint64_t local_map_reg = 0;
    int i; 
    // walk through the table
    for (i=0; i<FLIH_PUEA_TABLE_SIZE; i++)
    {
        // Is the entry in the table valid for this core and thread
        if  ((my_core_mask & puea_table[i].core_enable) && 
             (my_thread_mask & puea_table[i].thread_enable) &&
             (puea_table[i].mapreg_offset != -1)
            )
        {
            int shift_amount = 63 - puea_table[i].mapreg_offset;
            // initialize the bits for this mapping
            local_map_reg &= (~(((uint64_t)0x3) << shift_amount));
            // set the level information from the table
            local_map_reg |= (((uint64_t)puea_table[i].level) << shift_amount);
        }
    }
    // we have a fully contructed copy of the mapping register. Write it into the bic hardware
    BIC_WriteInterruptMap(ProcessorThreadID(), local_map_reg);
}

// Set the PUEA mapping register bits and status register bits of a particular group to there reset state 
void puea_ResetGroup(int bic_group)
{
    // get our core
    uint64_t my_core_mask = COREMSK(ProcessorCoreID());
    uint8_t my_thread_mask = THDMSK(ProcessorThreadID());
    int i; 
    // read the current value of the mapping register
    uint64_t local_map_reg = BIC_ReadInterruptMap(ProcessorThreadID());
    // walk through the table
    for (i=0; i<FLIH_PUEA_TABLE_SIZE; i++)
    {
        // Is the entry in the table valid for this core and thread
        if (!(_BN(i) & FW_RESERVED_PUEA_STATUS) && // Is this entry not under the control of the firmware?
            (bic_group == puea_table[i].group) &&
            (my_core_mask & puea_table[i].core_enable) && 
            (my_thread_mask & puea_table[i].thread_enable) &&
            (puea_table[i].mapreg_offset != -1))
        {
            int shift_amount = 63 - puea_table[i].mapreg_offset;
            // clear the existing mapping bits for this entry
            local_map_reg &= (~(((uint64_t)0x3) << shift_amount));
            // set the reset values 
            local_map_reg |= (((uint64_t)puea_table[i].level) << shift_amount);
        }
    }
    // We have a fully contructed copy of the mapping register. Write it into the bic hardware
    BIC_WriteInterruptMap(ProcessorThreadID(), local_map_reg);

}

// Set the GEA mapping registers
void gea_init()
{
    int i,j;
    uint64_t firmware_usage_mask[FLIH_GEA_NUM_STATUS_REGS];
    uint64_t localMappingRegister[FLIH_GEA_NUM_MAP_REGS]; // hold a local copy of the GEA mapping registers
    int init_clockstop = 0;
    for (i=0; i<FLIH_GEA_NUM_MAP_REGS; i++)
    {
        // Initialize the local mapping registers to what was set by firmware
        localMappingRegister[i] = BIC_ReadGeaInterruptMap(i);
    }
    // The following definitions from the firmware specify what GEA mappings we will not modify. 
    // These GEA input lines are reserved for use by the firmware.
    firmware_usage_mask[0] = FW_RESERVED_GEA_STATUS_0;
    firmware_usage_mask[1] = FW_RESERVED_GEA_STATUS_1;
    firmware_usage_mask[2] = FW_RESERVED_GEA_STATUS_2;
    // Walk through the STAT table, building a local copy of the mapping registers
    for (i=0; i<FLIH_GEA_NUM_STATUS_REGS; i++)
    {
        for (j=0; j<FLIH_GEA_NUM_STATUS_BITS; j++)
        {
            // do we have a valid entry to process
            if ((!(_BN(j) & firmware_usage_mask[i]) || (gea_stat_table[i][j].gea_bit_position == GEA_CS)) &&  // Is this entry not under the control of the firmware?
                (gea_stat_table[i][j].gea_bit_position >= 0)
               )
            {
                if(gea_stat_table[i][j].gea_bit_position == GEA_CS)
                {
                    init_clockstop = 1;
                }
                int shift_amount = 63 - gea_stat_table[i][j].map_offset;
                // Initialize the mapping register bits for this entry to zero
                localMappingRegister[gea_stat_table[i][j].map_reg] &=  (~(((uint64_t)0xF) << shift_amount));
                // Set the appropriate mapping register with the new value
                localMappingRegister[gea_stat_table[i][j].map_reg] |=  (((uint64_t)gea_stat_table[i][j].gea_bit_position) << shift_amount);
            }
        }
    }
    // Now write the local copy of the mapping registers into the GEA
    for (i=0; i<FLIH_GEA_NUM_MAP_REGS; i++)
    {
        BIC_WriteGeaInterruptMap(i,localMappingRegister[i]);
    }
    
    
    if(init_clockstop)
    {
        DCRWritePriv( CS_DCR(CONTROLREG_0), CS_DCR__CONTROLREG_0__ENABLE_BIC_CS_STOP_set(1) | CS_DCR__CONTROLREG_0__ENABLE_STOP_OUTPUT_set(1));
        ppc_msync();
    }
}

void wakeup_cleanup_agent()
{
    int myThreadId = ProcessorThreadID();
    BIC_WriteClearWakeupRegister0(myThreadId, -1L);
    BIC_WriteClearWakeupRegister1(myThreadId, -1L);
    // Enable this thread to accept C2C wakeup events targetted to this thread (and targetted ONLY to this thread)
    // --bits 44,46,48,50 are wake_enable bits for wakeups targetted to threads 0,1,2,3 from cores 0-15
    // --bits 45,47,49,51 are wake_enable bits for wakeups targetted to threads 0,1,2,3 from core 16
    // --bits 32 - 43 are WAC enable bits for WAC0 - WAC11 
    uint64_t enableC2Cmask0_15 = _BN(myThreadId * 2) >> 44; // set enable bit for my thread to get wakeups from cores 0-15
    uint64_t enableC2Cmask16 = enableC2Cmask0_15 >> 1; // set enable bit for my thread to get wakeups from cores 16
    uint64_t enableWACmask   = _BN(myThreadId) >> (32+CNK_WAKEUP_SPI_FIRST_WAC); // enable WACs for SPI use base on a kernel configuration definition
    // Set enables for C2C wakeup events and WACs for SPI usage 
    WAKEUP_BASE[CLEAR_THREAD(myThreadId)] = -1; // Clear all enablements. This also clears the MU enablement bits, which we will not turn back on yet.
    WAKEUP_BASE[SET_THREAD(myThreadId)] = enableC2Cmask0_15 | enableC2Cmask16 | enableWACmask;
    ppc_msync ();
}


// Initialize the Wakeup Unit
void wakeup_init()
{
    int myThreadId = ProcessorThreadID();
    // Set the core scoped values
    if (myThreadId == 0) // only need to do this on one of the threads since it affects the entire core
    {
        // Initialize WAC registers to safe values that will not match 
        // IMPORTANT NOTE: the default zero values for WAC_ENABLE will generate a constant match for all addresses  
        int i;
        for (i=0; i<12; i++)
        {
            WAKEUP_BASE[WAC_BASE(i) ] = -1L;      
            WAKEUP_BASE[WAC_ENABLE(i) ] = -1L;  
        }
        ppc_msync();

        // Match on store, stdcx, and stwcx ttypes.  Exclude all else  
        // Set 1's in all the physical address bit positions. We are using this max physical address value 
        // as the "no match" value in the initialiation of the WACs above. Note that if the WAC_ENABLE() mask
        // is set to all zeros, this is considered a "match" for all addresses
        // Also set bit 33 which enables a match with a snoop invalidate condition
        // Guard Page support needs to tolerate Spurious wakeups due to bit33=1 (cross core snoop invalidates) 
        WAKEUP_BASE[WAC_TTYPES] = 0x6036213F;  

        // Initialize all bits in the guard register to zero
        WAKEUP_BASE[CLEAR_GUARD] = -1L;

        // Open up user access to certain wakeup facilties 
        WAKEUP_BASE[WU_USER_ACCESS] = 0xF00F000000000000; // enable user access to WAC0,1,2,3 and SET_THREAD CLEAR_THREAD
    }

    // Wait until the core-scoped initialization is complete before continuing with the thread-scoped inits
    Kernel_Barrier(Barrier_HwthreadsInCore);

    ppc_msync ();

    BIC_WriteClearWakeupRegister0(myThreadId, -1L);
    BIC_WriteClearWakeupRegister1(myThreadId, -1L);

    // Enable a WAC for guarding use. This will cause a match to send an interrupt to the BIC
    // -- bits 32-43 correspond to WAC0 thru WAC11. The CNK_STACK_GUARD_FIRST_WAC defines
    //    which group of 4 registers will be assigned to threads 0,1,2,3 respectively
    WAKEUP_BASE[SET_GUARD]   = _BN(myThreadId) >> (32+CNK_STACK_GUARD_FIRST_WAC);

    // Enable this thread to accept C2C wakeup events targetted to this thread (and targetted ONLY to this thread)
    // --bits 44,46,48,50 are wake_enable bits for wakeups targetted to threads 0,1,2,3 from cores 0-15
    // --bits 45,47,49,51 are wake_enable bits for wakeups targetted to threads 0,1,2,3 from core 16
    // --bits 32 - 43 are WAC enable bits for WAC0 - WAC11 
    uint64_t enableC2Cmask0_15 = _BN(myThreadId * 2) >> 44; // set enable bit for my thread to get wakeups from cores 0-15
    uint64_t enableC2Cmask16 = enableC2Cmask0_15 >> 1; // set enable bit for my thread to get wakeups from cores 16
    uint64_t enableWACmask   = _BN(myThreadId) >> (32+CNK_WAKEUP_SPI_FIRST_WAC); // enable WACs for SPI use base on a kernel configuration definition

    // Set enables for C2C wakeup events, WACs for guarding and WACs for SPI usage 
    WAKEUP_BASE[CLEAR_THREAD(myThreadId)] = -1; // Clear all enablements. This also clears the MU enablement bits, which we will not turn back on yet.
    WAKEUP_BASE[SET_THREAD(myThreadId)] = enableC2Cmask0_15 | enableC2Cmask16 | enableWACmask;

    ppc_msync ();
}


// Default PUEA interrupt handler
void IntHandler_Default(int intrp_sum_bitnum)
{
    printf("In IntHandler_Default. PUEA interrupt bit num %d\n", intrp_sum_bitnum);
}

void IntHandler_GEA_Default(int status_reg, int bitnum)
{
    printf("2nd Level GEA handler: stat reg %d bitnum %d\n", status_reg, bitnum);
    
    RASBEGIN(2);
    RASPUSH(status_reg);
    RASPUSH(bitnum);
    RASFINAL(RAS_KERNELUNEXPECTEDINTERRUPT);
    Kernel_Crash(RAS_KERNELUNEXPECTEDINTERRUPT);
}


// First level GEA interrupt handler. Will decode gea status register and call second level GEA handlers
void IntHandler_GEA_FLIH(int intrp_sum_bitnum)
{
    int i;
    uint64_t bitnum;
    // Determine which mapped interrupt status register to read based on the PUEA
    // interrupt condition that got us here.
    uint64_t gea_bit = intrp_sum_bitnum - FLIH_PUEA_GEA_START;
    // Read the threee status registers associated with this interrupt line
    for (i=0; i<3; i++)
    {
        uint64_t gea_status_reg = BIC_ReadGeaMappedInterruptStatus(gea_bit, i);
        // printf("1st level GEA handler: PUEA bit: %d GEA status reg %d: %016lx\n",intrp_sum_bitnum, i,gea_status_reg);
        // Walk through the bits in the status register, calling the appropriate handler
#if USE_CNTLZ_IN_INTHANDLER
        while(gea_status_reg) // Are there any bits set in this status register?
        {
            bitnum = cntlz64(gea_status_reg); // gea_status_reg is non-zero, so cntlz result will be less than 64.
            gea_stat_table[i][bitnum].fcn(i,bitnum);
            gea_status_reg &= ~(1ULL<<(63-bitnum));
        }
#else
        if(gea_status_reg)
        {
            for (bitnum=0; bitnum<64; bitnum++)
            {
                // is this bit set in the status register?
                if (_BN(bitnum) & gea_status_reg)
                {
                    // Call the handler associated with this status bit
                    gea_stat_table[i][bitnum].fcn(i,bitnum);
                }
            }
        }
#endif
    }
}

// 1st Level IPI interrupt handler
// This interrupt handler assumes that it is being called from an External level interrupt flow and NOT a critial
// interrupt flow. Support of a critical interrupt IPI handler would require additional support on the sending side
// and in this handler
void IntHandler_IPI_FLIH(int intrp_sum_bitnum)
{
    int i;
    uint64_t mask = 0x8000000000000000;
    
    uint64_t c2c_status = BIC_ReadStatusExternalRegister0(ProcessorThreadID());
    for (i=0; i<64; i++)
    {
        if (mask & c2c_status)
        {
            // Make a local copy of the message data so we can release the IPI message data structure prior to calling the handler
            IPI_Message_t* pIPImsg = (IPI_Message_t*)&(NodeState.CoreState[i/4].HWThreads[i%4].ipi_message[ProcessorID()]);
            IPI_Message_t IPImsg_local = *pIPImsg;
            BIC_WriteClearExternalRegister0(ProcessorThreadID(), mask); 
            // Reset fnc field in the IPI message data to enable subsequent IPIs
            pIPImsg->fcn = NULL;
            ppc_msync();
#if CONFIG_IPI_FLIGHTLOG
            Kernel_WriteFlightLog(FLIGHTLOG, FL_RECEIVIPI, i, (uint64_t)IPImsg_local.fcn, IPImsg_local.param1, mfspr(SPRN_SRR0));
#endif        
            // Call the handler
            if (IPImsg_local.fcn)
            {
                IPImsg_local.fcn(IPImsg_local.param1, IPImsg_local.param2);
                // The following refresh of the c2c_status is necessary when the previously called
                // function attempt to send an IPI to another hwt and the IPI Deadlock Avoidance
                // code processes a pending IPI that is still indicated in our now stale c2c_status. Note
                // that a new interrupt could arrive from a hwt we have already processed, We are not
                // concerned about that now. That interrupt will still be pending so we will process it 
                // when we re-enter this interrupt handler. 
                c2c_status = BIC_ReadStatusExternalRegister0(ProcessorThreadID());
            }
            else
            {
                // RAS this occurrence
                RASBEGIN(2);
                RASPUSH(i);
                RASPUSH(ProcessorID());
                RASFINAL(RAS_IPINULLFUNCTION);
            }

            // reset the  bit we processed 
            c2c_status = c2c_status & ~mask; 
            // Exit early if there are no additional bits on in the status dword
            if (!c2c_status) break; 
        }
        mask = mask>>1;
    }
}

// 1st Level IPI interrupt handler
// This interrupt handler assumes that it is being called from an External level interrupt flow and NOT a critial
// interrupt flow. Support of a critical interrupt IPI handler would require additional support on the sending side
// and in this handler
void IntHandler_IPI_FLIH17(int intrp_sum_bitnum)
{
   // Interrupt originated from Core 17
    int i;
    uint64_t mask = 0x0000000000000008;

    uint64_t c2c_status = BIC_ReadStatusExternalRegister1(ProcessorThreadID());
    for (i=0; i<4; i++)
    {
        if (mask & c2c_status)
        {
            // Make a local copy of the message data so we can release the IPI message data structure prior to calling the handler
            IPI_Message_t* pIPImsg = (IPI_Message_t*)&(NodeState.CoreState[16].HWThreads[i].ipi_message[ProcessorID()]);
            IPI_Message_t IPImsg_local = *pIPImsg;
            // Clear the interrupt in the PUEA
            BIC_WriteClearExternalRegister1(ProcessorThreadID(), mask);                    
            // Reset fnc field in the IPI message data to enable subsequent IPIs
            pIPImsg->fcn = NULL;
            ppc_msync();
#if CONFIG_IPI_FLIGHTLOG
            Kernel_WriteFlightLog(FLIGHTLOG, FL_RECEIVIPI, i+64, (uint64_t)IPImsg_local.fcn, IPImsg_local.param1, mfspr(SPRN_SRR0));
#endif
            
            // Call the handler                
            if (IPImsg_local.fcn)
            {
                IPImsg_local.fcn(IPImsg_local.param1, IPImsg_local.param2);
            }
            else
            {
                // RAS this occurrence
                RASBEGIN(2);
                RASPUSH(i);
                RASPUSH(ProcessorID());
                RASFINAL(RAS_IPINULLFUNCTION);
            }

            // reset the  bit we processed 
            c2c_status = c2c_status & ~mask; 
            // Exit early if there are no additional bits on in the status dword
            if (!c2c_status) break; 
        }
        mask = mask>>1;
    }
}

#define IPI_TIMEOUT 1000
// Look for an orphaned request in the IPI message buffers that target this hardware thread
void IntHandler_IPIchecker()
{
    int timeout;
    int i;
    for (i=0; i<68; i++)
    {
        // Make a local copy of the message data so we can release the IPI message data structure prior to calling the handler
        IPI_Message_t* pIPImsg = (IPI_Message_t*)&(NodeState.CoreState[i/4].HWThreads[i%4].ipi_message[ProcessorID()]);
        IPI_Message_t IPImsg_local = *pIPImsg;
        if (pIPImsg->fcn)
        {
            // We have a pending IPI operation to investigate. 
            bool callHandler = 0;
            if (i<64)
            {
                timeout = IPI_TIMEOUT;
                while (timeout--)
                {
                    uint64_t c2c_status = BIC_ReadStatusExternalRegister0(ProcessorThreadID());
                    // If there is an interrupt pending, just let the normal IPI handler deal with it
                    callHandler = (c2c_status & _BN(i)) ? 0 : 1;
                    if (!callHandler) break;
                }
            }
            else
            {
                timeout = IPI_TIMEOUT;
                while (timeout--)
                {
                    // last nibble in the 64 bit word are the interrupt status bits for processor ids 64-67
                    uint64_t c2c_status = BIC_ReadStatusExternalRegister1(ProcessorThreadID());
                    // If there is an interrupt pending, just let the normal IPI handler deal with it
                    callHandler = (c2c_status & _BN(i-4)) ? 0 : 1;
                    if (!callHandler) break;
                }
            }
            if (callHandler)
            {
                // We have found an orphan!


                // free up the buffer
                pIPImsg->fcn = NULL;
                ppc_msync();

                // Call the handler                
                IPImsg_local.fcn(IPImsg_local.param1, IPImsg_local.param2);

                // RAS this occurrence
                RASBEGIN(2);
                RASPUSH(i);
                RASPUSH(ProcessorID());
                RASFINAL(RAS_IPIINTERRUPT);

                printf("(E) Detected an un-delivered IPI from hwt %d to hwt %d\n", i, ProcessorID());
                Kernel_WriteFlightLog(FLIGHTLOG, FL_IPIORPHAN, i, (uint64_t)IPImsg_local.fcn, IPImsg_local.param1, mfspr(SPRN_SRR0));
            }
        }
    }
}


// Critical Interrupts
__C_LINKAGE void IntHandler_Critical( Regs_t *context )
{
    uint64_t bitnum;
    uint64_t int_summary;

    // Find the first bit on, index into the interrupt table to find the handler
    while ((int_summary = BIC_ReadCriticalIntSummary(ProcessorThreadID()))) 
    {
        bitnum = cntlz64(int_summary);  // int_summary is non-zero, so via cntlz, bitnum must be less than 64.
        puea_table[bitnum].fcn(bitnum);
    }
    CountException(CNK_HWTPERFCOUNT_CRITICAL);
}

// Standard Interrupts
__C_LINKAGE void IntHandler_Standard( Regs_t *context )
{
    uint64_t bitnum;
    uint64_t int_summary;
    
    // Find the first bit on, index into the interrupt table to find the handler
    while ((int_summary = BIC_ReadExternalIntSummary(ProcessorThreadID()))) 
    {
        bitnum = cntlz64(int_summary);  // int_summary is non-zero, so via cntlz, bitnum must be less than 64.
        puea_table[bitnum].fcn(bitnum);
    }
    CountException(CNK_HWTPERFCOUNT_STANDARD);
}

void IntHandler_WAC(int intrpt_sum_bitnum)  // Wakeup Unit Address compare condition
{
#if 0
    uint64_t csrr0 = mfspr(SPRN_CSRR0_IP);
    uint64_t lr = mfspr(SPRN_LR);
    printf("IntHandler_WAC IP %016lx\n lr %016lx\n", csrr0, lr);
#endif
    int myProcessorThreadID = ProcessorThreadID();
    // This path is used exclusively by the Stack Guard facility. The 
    // WAC registers will be set by each thread in the core. All threads will be delivered
    // interrupts and this code will need to ensure the interrupt was caused by 
    // a thread in this process. This is necessary since we may have multiple processes 
    // running on the core and we cant keep the WAC conditions scoped to the threads 
    // since any WAC compare condition will fire the one existing interrupt in the PUEA

    // Read the wakeup unit status register for this core and determine which WAC register(s)
    // encountered the match condition
    uint64_t status = WAKEUP_BASE[SET_GUARD];

    // CNK allocates 4 WAC registers to threads 0,1,2,3 for use by the guarding facility. The first 
    // of the group of 4 is defined by a configuration setting CNK_STACK_GUARD_FIRST_WAC.
    // Test to see if the WAC register that corresponds to this thread has encountered a match
    // If so, we are responsible for resetting the interrupt condition
    uint64_t myThreadStatus = status & ((_BN(myProcessorThreadID))>>CNK_STACK_GUARD_FIRST_WAC);
    if (myThreadStatus)
    {
#if 0
        int i;
        uint64_t wac_addr[4];
//        uint64_t wac_mask[4];
        for (i=0; i<4; i++)
        {
            wac_addr[i] = WAKEUP_BASE[WAC_BASE(i+CNK_STACK_GUARD_FIRST_WAC) ];
//            uint64_t wac_mask[i] = WAKEUP_BASE[WAC_ENABLE(i+CNK_STACK_GUARD_FIRST_WAC) ];
        }
        printf("IntHandler_WACs status %016lx\n wac%d %016lx\n   wac%d %016lx\n   wac%d %016lx\n   wac%d %016lx",status, CNK_STACK_GUARD_FIRST_WAC, wac_addr[0], CNK_STACK_GUARD_FIRST_WAC+1, wac_addr[1], CNK_STACK_GUARD_FIRST_WAC+2, wac_addr[2], CNK_STACK_GUARD_FIRST_WAC+3, wac_addr[3]);
#endif

         // All threads in the core will get an interrupt for any WAC interrupt, even if that WAC interrupt is due 
        // to a violation in a different process. Since the WAC register values are REAL addresses, 
        // not Virtual, there will not be any "false" violations reported from a different process, however, 
        // the violations must be handled to only the process that caused the violation. We will have the hwthread that is the owner
        // of the WAC register handle the condition, even though it may have been some other hardware thread in the core that caused the violation.
        // We cannot determine which of the 4 threads actually caused the violation since the interrupt is not reported synchronously.

        // It is possible that this was a spurious wakeup. If it was, then we want to ignore this occurrence.
        // Read the data in the guarded area and if there has been no modification to the area, assume that
        // all is well and just return.
        uint64_t wac_base_orig = WAKEUP_BASE[WAC_BASE(myProcessorThreadID+CNK_STACK_GUARD_FIRST_WAC) ];
        // the uint32_t casting in the following line is necessary to deal with the zero values returned in bits 0-21 of the enable register
        uint64_t wac_size =  (uint64_t)(~((uint32_t)WAKEUP_BASE[WAC_ENABLE(myProcessorThreadID+CNK_STACK_GUARD_FIRST_WAC)]) + 1);
        WAKEUP_BASE[WAC_BASE(myProcessorThreadID+CNK_STACK_GUARD_FIRST_WAC)] = -1; // Temporarily store a physical address that will never match
        ppc_msync();

        AppProcess_t *process = GetMyProcess();
        // Convert the wac base to virtual address 
        uint64_t vsegbase = process->Heap_VStart;
        uint64_t psegbase = process->Heap_PStart;

        uint64_t *ptr1 = (uint64_t *)(((uint64_t)wac_base_orig - psegbase) + vsegbase);
        uint64_t *ptr1_end = (uint64_t *)((uint8_t*)ptr1 + wac_size);
        while (ptr1 < ptr1_end)
        {
            if (*ptr1 != 0xDEADBEEFDEADBEEF)
            {
                // If we are in the kernel thread, then the kthread violator is on on of the other hardware threads.
                // The other hardware threads in this core are now spinning in an interrupt loop, waiting for the 
                // hardware thread in charge (this thread) to clear the interrupt status. We do not know which kthread
                // caused the interrupt, but we know its not us. Look for the first hardware thread in this core that has
                // a kthread that is active in the application. This is our best guess as to the violator.
                int i;
                int mycore = ProcessorCoreID();
                process->coredump_kthread = NULL;
                for (i=0; i<CONFIG_HWTHREADS_PER_CORE; i++)
                {
                    KThread_t *kthread = GetHWThreadState(mycore, i)->pCurrentThread;
                    if (kthread && 
                        (kthread->SlotIndex < CONFIG_SCHED_KERNEL_SLOT_INDEX) &&
                        (kthread->State == SCHED_STATE_RUN))
                    {
                        kthread->Reg_State.dear = (uint64_t)ptr1; // Store the location of the data mismatch for the corefile.
                        Signal_Deliver(kthread->pAppProc, GetTID(kthread), SIGSTKFLT);
                        break; 
                    }
                }

                // Reset just the status bit that we own. This will release the other hardware threads from their hot interrupt spin
                // We do that after we find the kthread, otherwise the other hardware threads may progress to a different kthread
                // during our search.
                WAKEUP_BASE[CLEAR_GUARD] = myThreadStatus;
            }
            ptr1++;
            // If our pointer is on a cache line boundary, invalidate the page before this one.
            // This test is purposely done after the ptr1 is incremented so that we are invalidating
            // the cache page that we just completed accessing
            if (!(((uint64_t)ptr1)%L1D_CACHE_LINE_SIZE))
            {
                dcache_block_inval((void*)(((uint8_t*)ptr1)-L1D_CACHE_LINE_SIZE));
            }
        }
        // Reset just the status bit that we own, freeing the other hwthreads in the core from there interrupt spin
        WAKEUP_BASE[CLEAR_GUARD] = myThreadStatus;

#if 0
        printf("(W) Spurious GUARD Wakeup due to L2 cross slice snoop! base: %016lx\n",wac_base_orig);
        // Consider maintaining a count of these occurrences
#endif
    }
}


// Performance Monitor Interrupts
__C_LINKAGE void IntHandler_PERFMON( Regs_t *context )
{
    CountException(CNK_HWTPERFCOUNT_PERFMON);

    Upci_PerfMon_Exception();
}


void nd_disablemund() 
{
    uint64_t local_map_reg = BIC_ReadInterruptMap(ProcessorThreadID());
    int pueaTableIndex = cntlz64(PUEA_INTERRUPT_STATUS_GEA(GEA_ND));  // Find the mapping information for the ND interrupt handler
    int shift_amount = 63 - puea_table[pueaTableIndex].mapreg_offset;
    local_map_reg &= (~(((uint64_t)0x3) << shift_amount)); // initialize the bits for this mapping
    local_map_reg |= (((uint64_t)BIC_NO_INTERRUPT) << shift_amount); // Set the mapping so that no interrupt is sent
    BIC_WriteInterruptMap(ProcessorThreadID(), local_map_reg); // Write the modified mapping register
    ppc_msync();
}

void dcrArb_init()
{
    uint64_t ctrl_low;
    ctrl_low = DCRReadPriv(DC_ARBITER_DCR( INT_REG_CONTROL_LOW));
    DC_ARBITER_DCR__INT_REG_CONTROL_LOW__NO_ACK_AFTER_REQ_insert(ctrl_low, 2);
    DCRWritePriv(DC_ARBITER_DCR( INT_REG_CONTROL_LOW), ctrl_low);
}

void IntHandler_DCRUserViolation(int status_reg, int bitnum)
{
    AppProcess_t *proc = GetMyProcess();
    assert(proc != NULL);

    RASBEGIN(2);
    RASPUSH(DC_ARBITER_DCR(STATUS_REG));
    RASPUSH(DCRReadPriv( DC_ARBITER_DCR(STATUS_REG)));
    RASFINAL(RAS_DCRVIOLATION);
    
    Signal_Deliver(proc, 0, SIGDCRVIOLATION); 
    DCRWritePriv(DC_ARBITER_DCR(INT_REG__STATE), DC_ARBITER_DCR__INT_REG__NO_ACK_AFTER_REQ_set(1));
}

void IPI_disableMessageChecker() 
{

    GetMyHWThreadState()->ipiMessageCheckerExpiration = 0;
}

void IPI_enableMessageChecker() 
{
    HWThreadState_t *hwt = GetMyHWThreadState();
    // Enable and start the UDECR timer
    uint64_t curtime = GetTimeBase(); 
    // Set a non-zero interval to start setting a repeating wakeup
    hwt->ipiMessageCheckerExpiration = curtime + (CONFIG_IPIMESSAGECHECKER_INTERVAL * GetPersonality()->Kernel_Config.FreqMHz);
    Timer_enableUDECRwakeup(curtime, hwt->ipiMessageCheckerExpiration);
}

// If there are any pending ipi cmd interrupts from the ToolController, process them now.
void IntHandler_IPI_FlushToolCommands()
{
   // Test for interrrupt from the node controller on hwthread 66
    int i = 2;
    uint64_t mask = 0x0000000000000002;
    uint64_t c2c_status = BIC_ReadStatusExternalRegister1(ProcessorThreadID());
    if (mask & c2c_status)
    {
        // Make a local copy of the message data so we can release the IPI message data structure prior to calling the handler
        IPI_Message_t* pIPImsg = (IPI_Message_t*)&(NodeState.CoreState[16].HWThreads[i].ipi_message[ProcessorID()]);
        if (pIPImsg->fcn == IPI_handler_tool_cmd)
        {
            IPI_Message_t IPImsg_local = *pIPImsg;
            // Clear the interrupt in the PUEA
            BIC_WriteClearExternalRegister1(ProcessorThreadID(), mask);                    
            // Reset fcn field in the IPI message data to enable subsequent IPIs
            pIPImsg->fcn = NULL;
            ppc_msync();
            Kernel_WriteFlightLog(FLIGHTLOG, FL_FLUSHTCMD, i+64, (uint64_t)IPImsg_local.fcn, IPImsg_local.param1, mfspr(SPRN_SRR0));

            //printf("(I) Deadlock avoidance. Flushing pending IPI to process tool command\n");

            IPImsg_local.fcn(IPImsg_local.param1, IPImsg_local.param2);
        }
    }
}

void IntHandler_GEA_UPC(int status_reg, int bitnum)
{
    //printf("I am in UPC GEA handler: stat reg %d bitnum %d\n", status_reg, bitnum);

    // Shut off this interrupt until the compute node is re-booted. The RAS message will indicate the occurrence of this condition
    uint64_t interruptMap = BIC_ReadInterruptMap(ProcessorThreadID());
    int table_offset = cntlz64(PUEA_INTERRUPT_STATUS_GEA(GEA_UPC)); 
    int shift_amount = 63 - puea_table[table_offset].mapreg_offset;
    interruptMap &= (~(((uint64_t)0x3) << shift_amount));
    BIC_WriteInterruptMap(ProcessorThreadID(), interruptMap);
    isync();

    // Generate RAS for this condition
    uint64_t upc_c_int_state = DCRReadPriv(UPC_C_DCR(UPC_C_INTERRUPT_STATE__STATE));
    uint64_t internal_error_state = DCRReadPriv(UPC_C_DCR(INTERRUPT_INTERNAL_ERROR__STATE));
    uint64_t upc_c_int_first = DCRReadPriv(UPC_C_DCR(UPC_C_INTERRUPT_STATE__FIRST));
    uint64_t internal_sw_info = DCRReadPriv(UPC_C_DCR(INTERRUPT_INTERNAL_ERROR_SW_INFO));
    uint64_t internal_hw_info = DCRReadPriv(UPC_C_DCR(INTERRUPT_INTERNAL_ERROR_HW_INFO));
    uint64_t sram_parity_info = DCRReadPriv(UPC_C_DCR(UPC_SRAM_PARITY_INFO));
    uint64_t iosram_parity_info = DCRReadPriv(UPC_C_DCR(UPC_IO_SRAM_PARITY_INFO));
    RASBEGIN(7);
    RASPUSH(upc_c_int_state);
    RASPUSH(internal_error_state);
    RASPUSH(upc_c_int_first);
    RASPUSH(internal_sw_info);
    RASPUSH(internal_hw_info);
    RASPUSH(sram_parity_info);
    RASPUSH(iosram_parity_info);
    RASFINAL(RAS_UPCCPARITY);

}

void IntHandler_L1DCRV(int bitnum)
{
    AppProcess_t *proc = GetMyProcess();
    assert(proc != NULL);
    
    // Acknowledge the interrupt
    out64_sync((void*)L1P_ESR, L1P_ESR_err_mmio_priv);
    
    // Deliver signal to the interrupted thread.  
    Signal_Deliver(proc, GetTID(GetMyKThread()), SIGDCRVIOLATION);
}
