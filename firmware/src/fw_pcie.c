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

#include <stdint.h>
#include <stdio.h>

#include <hwi/include/bqc/devbus_dcr.h>
#include <hwi/include/bqc/pe_dcr.h>
#include <hwi/include/bqc/pcie_cfg_space.h>
#include <hwi/include/bqc/PhysicalMap.h>
#include <hwi/include/bqc/pcie_utl.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>
#include "Firmware_internals.h"
#include "Firmware_RAS.h"


// Operations
#define PCIE_NOOP           0  // no operation
#define PCIE_DCR_READ       1
#define PCIE_DCR_WRITE      2  // write operation
#define PCIE_DCR_POLL       3  // iteratively read until a condition is met (see modifiers)
#define PCIE_CFGSPC_PREFIX  4  // Set the PCIE Config Space address prefix
#define PCIE_CFGSPC_WRITE   5  // Write to PCIE Config Space
#define PCIE_TERMINATE      6  // Terminate the initialization sequence (useful for debugging)
#define PCIE_DCR_ASSERT     7  // Assert a given value in a DCR (useful for debugging)
#define PCIE_SET_CFG_ROOT   8  // Locates a given capability structure and retains its location
#define PCIE_CLR_CFG_ROOT   9  // Clears the capability structure root (prefix)
#define PCIE_CUSTOM        10  // Perform some customizeable operation

#define PCIE_CUSTOM_EC0C    1  // Custom init of DCR EC0C

#define PCIE_VENDOR_ID_IBM    0x1014  /* IBM's PCI Vendor ID */
#define PCIE_DEVICE_ID_BGQ    0x03D7  /* Blue Gene Q's PCI Device ID */

// Modifiers
#define PCIE_POLL_VALUE     1  // read until value is equal to data
#define PCIE_POLL_BITS      2  // read until all data bits are 1

#define PCIE_ALL_ONES              0xFFFFFFFFFFFFFFFFul   // A mnemonic used to clear exception bits

#if 0
NOTE: These are no longer used since we walk the capabilities structure (via PCIE_SET_CFG_ROOT operations)
#define PCIE_CAPABILITIES_BASE     0x6C                   // Base address of the PCIE Capabilities Structure
#define PCIE_AER_CAPABILITIES_BASE 0x100                  // Base address of the Advanced Error Reporting Capabilities Structure
#endif

#define PCIE_STANDARD_CAPABILITIES 1
#define PCIE_EXTENDED_CAPABILITIES 2

typedef struct PCIE_InitializationOperation_t {
  uint8_t  operation;     //!< The operation being perfomed (see Operations above)
  uint8_t  modifier;      //!< An option modifier (see Modifiers above)
  uint32_t address;       //!< The address being operated upon.
  uint64_t data;          //!< The data being written/observed.
} __attribute__((__packed__)) PCIE_InitializationOperation_t;


// The following macro allows us to transcribe register addresses from the Upper Transaction Layer (UTL)
// specification into the corresponding Blue Gene DCR addresses:
#define PCIE_UTL_DCR(offset) (0x0D0100 + (offset))  

// The UTL registers are 32 bits wide, whereas the Blue Gene DCRs are 64 bits wide.  There has been some
// flux in where the bits resided within the DCR (upper or lower word).  The following macro protects
// this code from that flux:
#define PCIE_UTL_SHIFT(value) (value)

// The following mneomonic provides a base address for PCIE configuration space
#define PCIE_CONFIG_SPACE (PHYMAP_MINADDR_PCICFG + PHYMAP_PRIVILEGEDOFFSET)



#define TRACE(x)  if (TRACE_ENABLED(TRACE_PCIe)) printf x
#define TRACEV(x) if (TRACE_ENABLED(TRACE_PCIe | TRACE_Verbose )) printf x 
//#define TRACE(x) printf x;
//#define TRACE(X)
//#define TRACEV(x)

//#define ASSERT_DCR_STATE 1
//#define PCIE_FLIGHT_RECORDER
//#define PCIE_ECHO 

// 3.3.1 PHB Initialization Sequence

PCIE_InitializationOperation_t PCIE_INIT[] = {

  // Set the Vendor and Device ID:
  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_CFG00),
      PE_DCR__PCIE_CFG00__VENDOR_ID_set(PCIE_VENDOR_ID_IBM) |
      PE_DCR__PCIE_CFG00__DEVICE_ID_set(PCIE_DEVICE_ID_BGQ)
  },

  // Set the Class Code and Revision ID:
  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_CFG08),
      PE_DCR__PCIE_CFG08__CLASS_CODE_set(0x060400) | /* PCI-to-PCI bridge */
      PE_DCR__PCIE_CFG08__REVISION_ID_set(0)
  },
  

  // -------------------------------------------------------------------------------------------------
  // From Mattias (fw01)
  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_DL_CONFIG),
    PE_DCR__PCIE_DL_CONFIG__RXCREDITTIMEOUT_VC0_set(0) |
    PE_DCR__PCIE_DL_CONFIG__TCTX_SCRAMBLEOFF_set(0) |
    PE_DCR__PCIE_DL_CONFIG__TCTX_LOOPBACK_set(0) |
    PE_DCR__PCIE_DL_CONFIG__TX_RPLBUFFERSIZE_set(0x100) |
    PE_DCR__PCIE_DL_CONFIG__LOCALNFTS_GEN1_set(0x80) |
    PE_DCR__PCIE_DL_CONFIG__LOCALNFTS_GEN2_set(0x80) |
    PE_DCR__PCIE_DL_CONFIG__LANESWAP_set(0) |
    PE_DCR__PCIE_DL_CONFIG__L0STIME_set(0x64) |
    PE_DCR__PCIE_DL_CONFIG__DLLPTLPRATIO_set(3)
  },

  // Workaround for issue 311
  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_CONFIG),
    PE_DCR__PCIE_CONFIG__SYS_OUT_PDAT_TOT_set(0) |
    PE_DCR__PCIE_CONFIG__SYS_PCIE_NPTRANS_TOT_set(1) |
    PE_DCR__PCIE_CONFIG__SYS_IN_PDAT_TOT_set(3) |
    PE_DCR__PCIE_CONFIG__SYS_GBIF_NPTRANS_TOT_set(1) |
    PE_DCR__PCIE_CONFIG__LINK_INIT_EN_set(0) |
    PE_DCR__PCIE_CONFIG__SYS_LINK_INIT_RXPH0_set(0x10) |
    PE_DCR__PCIE_CONFIG__SYS_LINK_INIT_RXPD0_set(0x20) |
    PE_DCR__PCIE_CONFIG__SYS_LINK_INIT_RXNPH0_set(0x10) |
    PE_DCR__PCIE_CONFIG__SYS_LINK_INIT_GBIFTAGS0_set(8) |
    PE_DCR__PCIE_CONFIG__SYS_LINK_TXPD0_set(2) |
    PE_DCR__PCIE_CONFIG__SYS_LINK_PCITAGS_set(8) |
    PE_DCR__PCIE_CONFIG__TXSWING_set(0) |
    PE_DCR__PCIE_CONFIG__CTCDISABLE_set(0)
  },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_2: PB MMIO Write (0x600, PCIE - System Configuration Register) 0x441000FC_00000000.
  //  - This is the default value out of reset. This register can be modified to change the following fields if needed:
  //      - bits 04:09 - SYS_EC0C_MAXLINKWIDTH[5:0] - The default link width for Torrent is x16. This can be
  //        reduced to x1, x4, or x8 if needed.
  //      - bits 10:12 - SYS_EC04_MAX_PAYLOAD[2:0] - The default max payload size for Torrent is 512B. This
  //        can be reduced to 128B or 256B as needed.
                        
  { PCIE_DCR_WRITE,  0, PE_DCR(PCIE_EC04), PE_DCR__PCIE_EC04__MAX_PAYLOAD_set(2)  }, // 512 bytes


  // Uncomment the following line to use Gen 2 Link Width 8 (full speed)
  //#define USE_GEN2_x8

  { PCIE_CUSTOM, PCIE_CUSTOM_EC0C, 0, 0 },


  // -------------------------------------------------------------------------------------------------
  // Reset the PCIE chip:
  // @todo: Do not set SIMSPEED when on real hardware!
  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_RESET), 
      PE_DCR__PCIE_RESET__UTL_RESET_set(1) | 
      PE_DCR__PCIE_RESET__SYS_RESET_set(1) |  
      PE_DCR__PCIE_RESET__PCI_OUT_P_set(1) |  
      PE_DCR__PCIE_RESET__TLDLP_ERROR_RESET_set(1) | 
      PE_DCR__PCIE_RESET__UTL_PCLK_ERROR_RESET_set(1) |
    /*PE_DCR__PCIE_RESET__SIMSPEED_set(1)*/ 0  },


  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_RESET), 
      PE_DCR__PCIE_RESET__UTL_RESET_set(0) | 
      PE_DCR__PCIE_RESET__SYS_RESET_set(0) |  
      PE_DCR__PCIE_RESET__PCI_OUT_P_set(0) |  
      PE_DCR__PCIE_RESET__TLDLP_ERROR_RESET_set(0) | 
      PE_DCR__PCIE_RESET__UTL_PCLK_ERROR_RESET_set(0) | 
    /*PE_DCR__PCIE_RESET__SIMSPEED_set(1)*/ 0  },

 
  // -------------------------------------------------------------------------------------------------
  // NOTE: Torrent steps PHB_INIT_4 - PHB_INIT_16 are skipped on BGQ.  There is no address translation
  //       mechanism in BQC.
  // -------------------------------------------------------------------------------------------------

  // -------------------------------------------------------------------------------------------------
  // NOTE: Torrent steps PHB_INIT_17 - PHB_INIT_25 are skipped on BGQ.  There is no IODA support
  //       in BQC.
  // -------------------------------------------------------------------------------------------------

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_26: PB MMIO Read (0x640, PCIE - DLP Training Control Register).
  //  - Firmware must poll this register to check bit 21 to see if the PCI-E link is up (DL_LINKUP).
  //  - Firmware cannot access the UTL core regs or PCI config space until the link is up. In reality,
  //    at least the PCI-E cores must be out of DL_PGRESET.
  //  - MMIO Stores to the link are silently dropped by the UTL core if the link is down.
  //  - MMIO Loads to the link will be dropped by the UTL core and will eventually time-out and will 
  //    return an all ones response if the link is down.
  { PCIE_DCR_POLL, 64, PE_DCR(PCIE_DL_STATUS),  PE_DCR__PCIE_DL_STATUS__DL_LINKUP_set(1) },

  
  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_27: PB MMIO Write (0x410, UTL - System Bus Agent Status Register) <all_ones>.
  //  - Write all ones to clear any spurious errors.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x08), PCIE_ALL_ONES },
  

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_28: PB MMIO Write (0x420, UTL - System Bus Agent Interrupt Enable Register) 0xAC800000_00000000.
  //  - This enables all UTL array parity errors to drive the UTL interrupt signal.

  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x0C), PCIE_UTL_SHIFT( UTL_SBAESR_RNPS | UTL_SBAESR_RPCS | UTL_SBAESR_RCIS | UTL_SBAESR_RCCS | UTL_SBAESR_RFTS ) },
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x10), PCIE_UTL_SHIFT( UTL_SBAIER_RNPI | UTL_SBAIER_RPCI | UTL_SBAIER_RCII | UTL_SBAIER_RCCI | UTL_SBAIER_RFTI ) },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_29: PB MMIO Write (0x4D0, UTL - Outbound Posted Data Buffer Allocation Register) 0x08000000_00000000.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x68), PCIE_UTL_SHIFT(0x08000000ull) },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_30: PB MMIO Write (0x4E0, UTL - Inbound Posted Header Buffer Allocation Register) 0x20000000_00000000.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x70), PCIE_UTL_SHIFT(0x20000000ull) },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_31: PB MMIO Write (0x4F0, UTL - Inbound Posted Data Buffer Allocation Register) 0x20000000_00000000.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x78), PCIE_UTL_SHIFT(0x20000000ull) },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_32: PB MMIO Write (0x520, UTL - PCI Express Tags Allocation Register) 0x08000000_00000000.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x90), PCIE_UTL_SHIFT(0x08000000ull) },

// The following causes problems with PCI adapters.  The hardware default value (0x8000000) seems to work well.
#if 0
  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_33: PB MMIO Write (0x530, UTL - GBIF Read Tags Allocation Register) 0x20000000_00000000.
  //  - These writes set up the inbound and outbound buffer sizes for header and data as well as the outbound
  //    number of PCI Express tags and inbound number GBIF read tags.
  //  - PCI Express outbound read tags = 8.
  //  - GBIF inbound read tags = 32.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0x98), PCIE_UTL_SHIFT(0x20000000ull) },
#endif

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_34: PB MMIO Write (0x540, UTL - PCI Express Port Control Register) 0x80800060_00000000.
  //  - This write will enable the initialization of the PCI-E receive header and data credits for VC0.
  //  - This write will enable transmission of outbound TLPs.
  //  - This write will set the default timeout periods for outbound non-posted transactions.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0xA0), PCIE_UTL_SHIFT(0x80800060ull) },

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_35: PB MMIO Write (0x548, UTL - PCI Express Port Status Register) <all_ones>.
  //  - Write all ones to clear any spurious errors.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0xA4), PCIE_ALL_ONES },

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_36: PB MMIO Write (0x558, UTL - PCI Express Port Interrupt Enable Register) 0x7E600000_00000000.
  //  - This write will enable the UTL interrupt signal for array and internal parity errors.
  //  - This write will enable the UTL interrupt signal for a PCI Express LinkDown event.
  //  - This write will enable the UTL interrupt signal for an outbound request discard event.
  // NOTE: Also set the parity errors to be primary events


  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0xA8), 
    PCIE_UTL_SHIFT( UTL_PCIEPESR_TPCS | UTL_PCIEPESR_TNPS | UTL_PCIEPESR_TFTS | UTL_PCIEPESR_TCAS | UTL_PCIEPESR_TCIS | UTL_PCIEPESR_RCAS ) },

  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0xAC), 
    PCIE_UTL_SHIFT( UTL_PCIEPIER_TPCI | UTL_PCIEPIER_TNPI | UTL_PCIEPIER_TFTI | UTL_PCIEPIER_TCAI | UTL_PCIEPIER_TCII | UTL_PCIEPIER_RCAI | UTL_PCIEPIER_PLDI  | UTL_PCIEPIER_OTDI ) },

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_37: PB MMIO Write (0x560, UTL - Root Complex Status Register) <all_ones>.
  //  - Write all ones to clear any spurious errors.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0xB0), PCIE_ALL_ONES },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_38: PB MMIO Write (0x568, UTL - Root Complex Error Severity Register) 0xE0000000_00000000.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0xB4), PCIE_UTL_SHIFT(0xE0000000ull) },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_39: PB MMIO Write (0x570, UTL - Root Complex Interrupt Enable Register) 0x60000000_00000000.
  //  -  This write will enable the appropriate errors and UTL interrupt enables for the Root Complex errors.
  { PCIE_DCR_WRITE,  0, PCIE_UTL_DCR(0xB8),  PCIE_UTL_SHIFT(0x60000000ull) },




#ifdef ASSERT_DCR_STATE

  { PCIE_DCR_ASSERT, 0, PE_DCR(PCIE_DL_CONFIG),
    PE_DCR__PCIE_DL_CONFIG__RXCREDITTIMEOUT_VC0_set(0) |
    PE_DCR__PCIE_DL_CONFIG__TCTX_SCRAMBLEOFF_set(0) |
    PE_DCR__PCIE_DL_CONFIG__TCTX_LOOPBACK_set(0) |
    PE_DCR__PCIE_DL_CONFIG__TX_RPLBUFFERSIZE_set(0x100) |
    PE_DCR__PCIE_DL_CONFIG__LOCALNFTS_GEN1_set(0x80) |
    PE_DCR__PCIE_DL_CONFIG__LOCALNFTS_GEN2_set(0x80) |
    PE_DCR__PCIE_DL_CONFIG__LANESWAP_set(0) |
    PE_DCR__PCIE_DL_CONFIG__L0STIME_set(0x64) |
    PE_DCR__PCIE_DL_CONFIG__DLLPTLPRATIO_set(3)
  },
  { PCIE_DCR_ASSERT,  0, PE_DCR(PCIE_EC04), PE_DCR__PCIE_EC04__MAX_PAYLOAD_set(2)  }, // 512 bytes
  { PCIE_DCR_ASSERT,  0, PE_DCR(PCIE_EC0C), 
      PE_DCR__PCIE_EC0C__PM_SUPPORT_set(1) | 
      PE_DCR__PCIE_EC0C__MAXLINKWIDTH_set(1) | 
      PE_DCR__PCIE_EC0C__SYS_RC_EC0C_SUPPORTEDLINKSPEEDS_set(1)  
  },
  { PCIE_DCR_ASSERT, 0, PE_DCR(PCIE_RESET), 
      PE_DCR__PCIE_RESET__UTL_RESET_set(0) | 
      PE_DCR__PCIE_RESET__SYS_RESET_set(0) |  
      PE_DCR__PCIE_RESET__PCI_OUT_P_set(0) |  
      PE_DCR__PCIE_RESET__TLDLP_ERROR_RESET_set(0) | 
      PE_DCR__PCIE_RESET__UTL_PCLK_ERROR_RESET_set(0) | 
      PE_DCR__PCIE_RESET__SIMSPEED_set(1)  },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0x08), 0x0 },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0x10), PCIE_UTL_SHIFT(0xAC800000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0x68), PCIE_UTL_SHIFT(0x08000000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0x70), PCIE_UTL_SHIFT(0x20000000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0x78), PCIE_UTL_SHIFT(0x20000000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0x90), PCIE_UTL_SHIFT(0x08000000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0x98), PCIE_UTL_SHIFT(0x20000000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0xA0), PCIE_UTL_SHIFT(0x80800060ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0xA4), 0x0 },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0xAC), PCIE_UTL_SHIFT(0x7E600000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0xB0), 0x0 },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0xB4), PCIE_UTL_SHIFT(0xE0000000ull) },
  { PCIE_DCR_ASSERT, 0, PCIE_UTL_DCR(0xB8), PCIE_UTL_SHIFT(0x60000000ull) },

#endif

  // -------------------------------------------------------------------------------------------------
  // Enable hard errors as machine checks:
  // -------------------------------------------------------------------------------------------------

  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_INTERRUPT_STATE_CONTROL_HIGH), 
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)       | /* Parity error on DCR ring. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_OW_PTY_set(1)       | /* Parity error on an outbound PCIe write request received from the device bus. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_OR_PTY_set(1)       | /* Parity error in an outbound PCIe read request received from the device bus. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_OR_TAG_STORE_set(1) | /* Parity error in some entry of the outbound PCIe read tag store. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_OR_POISONED_set(1)  | /* Outbound PCIe read data was poisoned */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_IW_PTY_set(1)       | /* Parity error in inbound PCIe write request register. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_IW_SM_STATE_set(1)  | /* Inbound PCIe write state machine reached an illegal state. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_IR_RESP_set(1)      | /* Parity error in inbound PCIe read response received from the device bus. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__PCI_IR_ECC_set(1)       | /* Uncorrectable ECC error in inbound PCIe read response data received from the device bus. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__CFG_PERR_set(1)         | /* Confguration core parity error. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__UTL_PCLK_ERR_set(1)     | /* PCIE error in the PCLK domain of the UTL. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__UTL_BCLK_ERR_set(1)     | /* PCIE error in the BCLK domain. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__TLDLP_PCLK_ERR_set(1)   | /* PCIE error in the TLDLP core. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__AL_PINT_set(1)          | /* UTL Primary Interrupt. */
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_HIGH__UTL_SM_PTY_set(1)       | /* UTL Internal Register State Machine Error. */
    0
  },

  { PCIE_DCR_WRITE, 0, PE_DCR(PCIE_INTERRUPT_STATE_CONTROL_LOW), 
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_LOW__PCI_MEM_RD_ERROR_set(1)       |
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_LOW__PCI_MEM_RD_ERROR_UR_set(1)    |
    PE_DCR__PCIE_INTERRUPT_STATE_CONTROL_LOW__PCI_MEM_RD_ERROR_RETRY_set(1) |
    0
  },


  // -------------------------------------------------------------------------------------------------
  // Note: we now set up the configuration space for the PCI bridge (root complex).  This is all
  //       done on Bus 0, Device 0, Function 0.  So we set the PCIE Config Space prefix to
  //       0x00000.
  // -------------------------------------------------------------------------------------------------

  { PCIE_CFGSPC_PREFIX, 0, 0,    0x00000    },

  // -------------------------------------------------------------------------------------------------
  // PCI - Primary/Secondary/Subordinate Bus Number Register.
  //  - Firmware must determine the correct information to write to this register.
  //  - Example typical simulation value is 0x00010400.
  { PCIE_CFGSPC_WRITE,  4, 0x18, 0x00010100 }, // @todo Is this the correct value?
  

  // -------------------------------------------------------------------------------------------------
  //  PCI - IO Base/Limit and Secondary Status Register.
  //  - The IO Base[7:4] is changed to a value of 0x1. This forces the Base > Limit so that all inbound transactions
  //    will be allowed to pass through regardless of address.
  { PCIE_CFGSPC_WRITE, 4, 0x1C, 0x10000000 },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_81: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000020_00000000.
  // PHB_INIT_82: PB MMIO Write (0x130, CONFIG_DATA) 0x10000000 (size=4B).
  //  - PCI - Memory Base/Limit Register.
  //  - The Mem Base[15:04] is changed to a value of 0x001. This forces the Base > Limit so that all inbound
  //    transactions will be allowed to pass through regardless of address.
  { PCIE_CFGSPC_WRITE, 4, 0x20, 0x00801080 },  


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_83: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000024_00000000.
  // PHB_INIT_84: PB MMIO Write (0x130, CONFIG_DATA) 0x10000000 (size=4B).
  //  - PCI - Prefetch Base/Limit Register.
  //  - The Prefetch Base[15:04] is changed to a value of 0x001. This forces the Base > Limit so that all
  //    inbound transactions will be allowed to pass through regardless of address.
  { PCIE_CFGSPC_WRITE, 4, 0x24, 0x01c0f1c1 },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_85: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x8000003C_00000000.
  // PHB_INIT_86: PB MMIO Write (0x130, CONFIG_DATA) 0x00000300 (size=4B).
  //  - PCI - Bridge Control Register.
  //  - Enable Parity Error response bit 16.
  //  - Enable SERR bit 17.
  { PCIE_CFGSPC_WRITE, 4, 0x3C, 
    _SWAP16( PCI_BRIDGE_CTL_PARITY | PCI_BRIDGE_CTL_SERR ) },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_87: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000088_00000000.
  // PHB_INIT_88: PB MMIO Write (0x130, CONFIG_DATA) 0x4F000F00 (size=4B).
  //  - EC - Device Control/Status Register.
  //  - Enable correctable, non-fatal, fatal, and unsupported request error reporting.
  //  - Disable relaxed-ordering.
  //  - Enable a max payload size of 512B.
  //  - Clear any detected errors.

  { PCIE_SET_CFG_ROOT, PCIE_STANDARD_CAPABILITIES, PCI_CAP_ID_EXP },

  { PCIE_CFGSPC_WRITE, 4, /* PCIE_CAPABILITIES_BASE + */ 0x08, 
    ( _SWAP16( 
	      PCI_EXP_DEVCTL_CERE |
	      PCI_EXP_DEVCTL_NFERE |
	      PCI_EXP_DEVCTL_FERE |
	      PCI_EXP_DEVCTL_URRE |
	      PCI_EXP_DEVCTL_PAYLOAD_128 ) << 16 ) |
    _SWAP16(  PCI_EXP_DEVSTA_CED |
	      PCI_EXP_DEVSTA_NFED |
	      PCI_EXP_DEVSTA_FED |
	      PCI_EXP_DEVSTA_URD )
  },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_89: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x8000009C_00000000.
  // PHB_INIT_90: PB MMIO Write (0x130, CONFIG_DATA) 0x07000000 (size=4B).
  //  - EC - Root Control Register.
  //  - Enable ERR_COR, ERR_NONFATAL, ERR_FATAL errors.

  { PCIE_CFGSPC_WRITE, 4, /*PCIE_CAPABILITIES_BASE +*/ 0x1C, 
    _SWAP16(  
	    PCI_EXP_RTCTL_SECEE |
	    PCI_EXP_RTCTL_SENFEE |
	    PCI_EXP_RTCTL_SEFEE 
	    ) << 16 
  },



  { PCIE_CLR_CFG_ROOT },
  { PCIE_SET_CFG_ROOT, PCIE_EXTENDED_CAPABILITIES, PCI_EXT_CAP_ID_ERR }, // Locate AER

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_91: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000104_00000000.
  // PHB_INIT_92: PB MMIO Write (0x130, CONFIG_DATA) 0xFFFFFFFF (size=4B).
  //  - AER - Uncorrectable Error Status Register.
  //  - Clear all error bits.
  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x04, 0xFFFFFFFF },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_93: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000108_00000000.
  // PHB_INIT_94: PB MMIO Write (0x130, CONFIG_DATA) 0x00000000 (size=4B).
  //  - AER - Uncorrectable Error Mask Register.
  //  - Clear all error mask bits.
  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x08, 0x00000000 },



  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_95: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x8000010C_00000000.
  // PHB_INIT_96: PB MMIO Write (0x130, CONFIG_DATA) 0x30200700 (size=4B).
  //  - AER - Uncorrectable Error Severity Register.
  //  - Set error bits 4,5,13,16-18 to be reported as Fatal errors.

  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x0C, 
    _SWAP32( 
	    PCI_ERR_UNC_DLP      |
	    PCI_ERR_UNC_SDES     |
	    PCI_ERR_UNC_FCP      |
	    PCI_ERR_UNC_UNX_COMP |
	    PCI_ERR_UNC_RX_OVER  |
	    PCI_ERR_UNC_MALF_TLP ) 
  },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_97: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000110_00000000.
  // PHB_INIT_98: PB MMIO Write (0x130, CONFIG_DATA) 0xFFFFFFFF (size=4B).
  //  - AER - Correctable Error Status Register.
  //  - Clear all error bits.
  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x10, 0xFFFFFFFF },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_99: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000114_00000000.
  // PHB_INIT_100: PB MMIO Write (0x130, CONFIG_DATA) 0x00200000 (size=4B).
  //  - AER - Correctable Error Mask Register.
  //  - Mask bit 13, Advisory Non-Fatal Error.
  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x14, _SWAP32( PCI_ERR_COR_ANFEM ) },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_101: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000118_00000000.
  // PHB_INIT_102: PB MMIO Write (0x130, CONFIG_DATA) 0x40010000 (size=4B).
  //  - AER - Capabilities and Control Register.
  //  - Enable ECRC generation bit 6.
  //  - Enable ECRC checking bit 8.
  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x18, 
    _SWAP32(
	    PCI_ERR_CAP_ECRC_GENE |
	    PCI_ERR_CAP_ECRC_CHKE
	    ) 
  },

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_103: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x8000012C_00000000.
  // PHB_INIT_104: PB MMIO Write (0x130, CONFIG_DATA) 0x07000000 (size=4B).
  //  - AER - Root Error Control Register.
  //  - Enable correctable, non-fatal, fatal error reporting bits 0,1, and 2.
  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x2C, 
    _SWAP32(
	    PCI_ERR_ROOT_CMD_COR_EN |
	    PCI_ERR_ROOT_CMD_NONFATAL_EN   |
	    PCI_ERR_ROOT_CMD_FATAL_EN
	    )
  },

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_105: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000130_00000000.
  // PHB_INIT_106: PB MMIO Write (0x130, CONFIG_DATA) 0xFFFFFFFF (size=4B).
  //  - AER - Root Error Status Register.
  //  - Clear all error bits.
  { PCIE_CFGSPC_WRITE, 4, /* PCIE_AER_CAPABILITIES_BASE + */ 0x30, 0xFFFFFFFF },

  { PCIE_CLR_CFG_ROOT }, // Done with AER


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_107: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000004_00000000.
  // PHB_INIT_108: PB MMIO Write (0x130, CONFIG_DATA) 0x470100F8 (size=4B).
  //  - PCI - Command/Status Register.
  //  - Enable IO space bit 0.
  //  - Enable Memory space bit 1.
  //  - Enable Bus Master bit 2. This will enable inbound requests from the endpoint.
  //  - Enable Parity Error Enable (PERR) bit 6.
  //  - Enable SERR bit 8.
  //  - Clear status bits 31:27.

  { PCIE_CFGSPC_WRITE, 4, 0x004, 
    ( _SWAP16( 
	      PCI_COMMAND_IO        |
	      PCI_COMMAND_MEMORY    |
	      PCI_COMMAND_MASTER      |
	      PCI_COMMAND_PARITY  |
	      PCI_COMMAND_SERR            ) << 16) |
      _SWAP16( 
	      PCI_STATUS_SIG_TARGET_ABORT   |
	      PCI_STATUS_REC_TARGET_ABORT   |
	      PCI_STATUS_REC_MASTER_ABORT   |
	      PCI_STATUS_SIG_SYSTEM_ERROR   |
	      PCI_STATUS_DETECTED_PARITY   )
  },

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_109: PB MMIO Read (0x548, UTL - PCI Express Port Status Register).
  //  - Firmware must poll this register to check bit 8 to see if the PCI-E link is up.
  //  - Torrent recommends a firmware polling frequency of once every 2ms (link configuration hardware loop
  //    time) with a timeout of about 200ms.
  //  - If firmware times-out polling for the linkup status it is recommended that the link be reset (PCI - Bridge
  //    Control Register bit 22) and another attempt be made. The total number of attempts is determined by
  //    PHYP, typically 2-3 times.
  //  - PCI - Bridge Control Register bit 22 is the PCI-E Hot Reset bit. Firmware must set and clear this bit, it
  //    does not automatically clear itself.
  //  - MMIO Stores to the link are silently dropped by the UTL core if the link is down.
  //  - MMIO Loads to the link will be dropped by the UTL core and will eventually time-out and will return an
  //    all ones response if the link is down.

#if 0
  
this is broken ...

  { PCIE_DCR_POLL, PCIE_POLL_BITS, PCIE_UTL_DCR(0xA4), PCIE_UTL_SHIFT(0x00800000ull) }, //! @todo how to handle timeout
#endif

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_110: PB MMIO Write (0x140, CONFIG_ADDRESS) 0x80000090_00000000.
  // PHB_INIT_111: PB MMIO Read (0x130, CONFIG_DATA) (size=4B).
  //  - EC - Link Control / Status Register.
  //  - Firmware can read this register to obtain the negotiated link width and current link status if desired.
  //{ PCIE_MMIO_WRITE, 0, 0x140, 0x8000009000000000 },
  //{ PCIE_MMIO_READ, 0, 0x130, 0x00000000 }, //! @todo what to do with the results of this read?


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_112: PCI Config Space Initialization Concluded
  //  - Firmware can perform a Bus Walk or any other operation on the PCI-E link at this point.
  // -------------------------------------------------------------------------------------------------

  { PCIE_NOOP },

#if 0
  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_113: PB MMIO Write (0x138, CONFIG ADDRESS/DATA Lock Register) <all zeros>.
  //  - This step is only necessary in a multi-threaded environment. It is firmwares choice whether or not to
  //    perform this step.
  //  - Firmware should clear its lock once it is finished with its operations.
  { PCIE_MMIO_CLEAR, 0, 0x138 },
#endif

#if 0

  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_114: PB MMIO Write (0xC18, LEM Error Mask Register) 0xE00817BF_7D162FAF.
  { PCIE_MMIO_WRITE, 0, 0xC18, 0xE00817BF7D162FAF },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_115: PB MMIO Write (0xCA0, PHB Error Interrupt Enable Register) 0xFFFD1830_FF800000.
  { PCIE_MMIO_WRITE, 0, 0xCA0, 0xFFFD1830FF800000 },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_116: PB MMIO Write (0xD20, MMIO Error Interrupt Enable Register) 0xD0FBE1F7_FC0000C0.
  { PCIE_MMIO_WRITE, 0, 0xD20, 0xD0FBE1F7FC0000C0 },


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_117: PB MMIO Write (0xDA0, DMA0 Error Interrupt Enable Register) 0xFFFE00FF_E103FFFC.
  //  - Mask all LEM bits except for errors that fence the PB bus. These errors must use the GEM to send an
  //    interrupt since the PB bus would be fenced. Fatal errors should set LEM mask bits to 0, all other
  //    errors should set mask bits to 1.
  //  - See GEM error bits in the Torrent spec for details on which error bits are asserted.
  //  - Enable error bits that are allowed to send a PHB error interrupt.
  { PCIE_MMIO_WRITE, 0, 0xDA0, 0xFFFE00FFE103FFFC },
#endif


  // -------------------------------------------------------------------------------------------------
  // PHB_INIT_118: End of PHB Initialization Sequence
  // -------------------------------------------------------------------------------------------------
};

#if 0
void fw_pcie_enable_paranoid_mode(void)
{
        fw_uint64_t tmp;

        tmp = DCRReadPriv(PE_DCR(PCIE_IFC_CFG));
        PE_DCR__PCIE_IFC_CFG__IN_WR_STRICT_SAFE_insert(tmp, 1);
        PE_DCR__PCIE_IFC_CFG__IN_RD_STRICT_SAFE_insert(tmp, 1);
        DCRWritePriv(PE_DCR(PCIE_IFC_CFG), tmp);
}
#endif

#define DEBUG 1

#ifdef DEBUG

const char* OPS[] = {
  "<NOOP       >",
  "<DCR-READ   >",
  "<DCR-WRITE  >",
  "<DCR POLL   >",
  "<CFG-SPC-PRE>",
  "<CFG-SPC-WRT>",
  "<TERMINATE  >",
  "<DCR-ASSERT >",
  "<SET-CFG-RT >",
  "<CLR-CFG-RT >",
  "<CUSTOM     >"
};

const char* MODS[] = {
  "            ",
  "<POLL_VALUE>",
  "<POLL_BITS >"
};

#endif

volatile int INIT_STEP = 0;
volatile uint64_t pcieConfigurationSpaceRoot = 0;

#ifdef PCIE_FLIGHT_RECORDER

typedef struct _PCIE_FlightRecorderData {
  uint8_t  step;
  uint8_t  extra;
  uint16_t address;
  uint64_t before;
  uint64_t after;
  uint64_t data;
} PCIE_FlightRecorderData ;

#define FR_SIZE 128

PCIE_FlightRecorderData FR_DATA[FR_SIZE] ALIGN_L1D_CACHE;

#endif

int fw_pcie_custom(int i) {

    switch ( PCIE_INIT[i].modifier ) {
	
    case PCIE_CUSTOM_EC0C : {

	uint64_t value = PE_DCR__PCIE_EC0C__PM_SUPPORT_set(1);

	// +-----------------------------------------------------------------------+
	// | NOTE: On DD1 we must run PCIe at gen 1 / link 1 speeds due to devbus  |
	// |       bugs.                                                           |
	// ------------------------------------------------------------------------+

	if ( FW_DD1_WORKAROUNDS_ENABLED() ) {
	  TRACE(("(I) PCIe Gen 1 x 1 mode.\n"));
	  value |= PE_DCR__PCIE_EC0C__MAXLINKWIDTH_set(1) | PE_DCR__PCIE_EC0C__SYS_RC_EC0C_SUPPORTEDLINKSPEEDS_set(1);
	}
	else {
	  TRACE(("(I) PCIe Gen 2 x 8 mode.\n"));
	  value |= PE_DCR__PCIE_EC0C__MAXLINKWIDTH_set(8) | PE_DCR__PCIE_EC0C__SYS_RC_EC0C_SUPPORTEDLINKSPEEDS_set(2);
	}

	DCRWritePriv( PE_DCR(PCIE_EC0C), value );
	
	break;
    }

    default : {
	FW_Error("Unsupported operation (step=%d modifer=%d) [%s:%d].", i, PCIE_INIT[i].modifier, __func__, __LINE__);
	return -1;
    }

    }

    return 0;
}

void fw_pcie_write_dcr(int i) {

#ifdef PCIE_FLIGHT_RECORDER
  FR_DATA[i].before = DCRReadPriv( PCIE_INIT[i].address);
#endif

  DCRWritePriv( PCIE_INIT[i].address, PCIE_INIT[i].data );

#ifdef PCIE_FLIGHT_RECORDER
  FR_DATA[i].after = DCRReadPriv( PCIE_INIT[i].address);
#endif
}

int fw_pcie_assert_dcr(int i) {

  uint64_t value = DCRReadPriv( PCIE_INIT[i].address);

  if ( value != PCIE_INIT[i].data ) {
    printf("[%d] DCR assertion failed  DCR=0x%05X (actual) %016lX-%016lX (expected)\n", INIT_STEP, PCIE_INIT[i].address, value, PCIE_INIT[i].data );
    return -1;
  }

  return 0;
}

int fw_pcie_poll_dcr( int i ) {

  int rc = 0;
  int countdown = PERS_ENABLED(PERS_ENABLE_Simulation) ? 10 : 25000; // total quanta to wait before timeout
  int quanta_on = 0;
  int restarts = 0;


  // We expect PCIe to lock on in one second or less.  If

  fw_uint64_t warning_timeout = 10ull;
  fw_uint64_t warning_time    = GetTimeBase() + ( FW_Personality.Kernel_Config.FreqMHz * warning_timeout * 1000ull * 1000ull );


#ifdef PCIE_FLIGHT_RECORDER
    FR_DATA[i].before = DCRReadPriv( PCIE_INIT[i].address);
#endif

  // The modifier specifies how long the bits must remain active.  A zero modifier means that 
  // we immediately stop polling when the bits become active.
  do {

    if ( ( DCRReadPriv( PCIE_INIT[i].address) & PCIE_INIT[i].data ) == PCIE_INIT[i].data ) {
      quanta_on++; // poll bits are still on
    }
    else {

      if ( quanta_on > 0 ) {
	TRACE(( "Flicker #%d -> %d\n", restarts, quanta_on));
	restarts++;
      }

      quanta_on = 0;  // poll bits are off.  start over
    }

    fw_udelay(10000);  // wait about 10 milliseconds

    if ( GetTimeBase() > warning_time ) {
	fw_writeRASEvent( FW_RAS_PCIE_LINKUP_IS_SLOW, 1, &warning_timeout );
	warning_time = -1;
    }

  } while ( (countdown-- > 0) && (quanta_on < PCIE_INIT[i].modifier) );

#ifdef PCIE_FLIGHT_RECORDER
    FR_DATA[i].after = DCRReadPriv( PCIE_INIT[i].address);
    FR_DATA[i].extra = countdown;
#endif

  if (countdown < 0) {
      FW_Error("PCIe timeout.  Step:%d  Register:%X Bits:%lX", i, PCIE_INIT[i].address, PCIE_INIT[i].data);
      rc = FW_TIMEOUT;
  }

  if  ( restarts > 0 ) {
      TRACE(("(W) %d bit flickers\n",restarts));
  }

  return rc;
}

int fw_pcie_cfg_spc_write( int i ) {
 
  // The modifier holds the length of the write.  For now, we only support 4-byte accesses

  switch ( PCIE_INIT[i].modifier ) {

  case 4 : {

    volatile uint32_t* configRegister = (volatile uint32_t*)(PCIE_CONFIG_SPACE + pcieConfigurationSpaceRoot + PCIE_INIT[i].address);

#ifdef PCIE_FLIGHT_RECORDER
    FR_DATA[i].before = *configRegister;
#endif

    *configRegister = PCIE_INIT[i].data;

#ifdef PCIE_FLIGHT_RECORDER
    FR_DATA[i].after = *configRegister;
#endif

    break;
  }

  default : {
    return -1;
  }
  }

  // DevBus overrun workaround
  ppc_msync();
  fw_pdelay(1000);

  return 0;
}

int fw_pcie_set_cfg_spc_root( int i ) {

  if ( PCIE_INIT[i].modifier == PCIE_STANDARD_CAPABILITIES ) {

    volatile uint32_t*  configurationAddress = (volatile uint32_t*)(PCIE_CONFIG_SPACE + PCI_CAPABILITY_LIST);
    uint32_t            header               = *configurationAddress;
    uint32_t            nextHeader           = header >> 24; // adjust for little endian
    uint8_t             capabilityId;

    while (nextHeader != 0) {

      /*
       * Read the word in configuration space, which looks like this
       * (due to little endianness of the config space)
       *
       *   +--------+----------+---------+
       *   | CAP_ID | NEXT_CAP |   ///   |
       *   +--------+----------+---------+
       *    0      7 8       15 16     31
       */

      configurationAddress = (volatile uint32_t*)(PCIE_CONFIG_SPACE + nextHeader);
      header               = *configurationAddress;
      capabilityId         = header >> 24;

      // DevBus overrun workaround
      ppc_msync();
      fw_pdelay(1000);
	    
      if (capabilityId == PCIE_INIT[i].address) {
	pcieConfigurationSpaceRoot = (uint64_t)nextHeader;
	break;
      }
      nextHeader = (header >> 16) & 0xFF;
    }
  }
  else if ( PCIE_INIT[i].modifier == PCIE_EXTENDED_CAPABILITIES ) {

    volatile uint32_t*  configurationAddress;
    uint32_t            header;
    uint32_t            nextHeader = 0x100; 
    uint16_t            capabilityId;

    while (nextHeader != 0) {

      /*
       * Read the word in configuration space, which looks like this
       * (due to little endianness of the config space)
       *
       *   +--------+----------+---------+
       *   | CAP_ID | NEXT_CAP |   ///   |
       *   +--------+----------+---------+
       *    0      7 8       15 16     31
       */

      configurationAddress = (volatile uint32_t*)(PCIE_CONFIG_SPACE + nextHeader);
      header               = *configurationAddress;
      header               = _SWAP32(header);
      capabilityId         = PCI_EXT_CAP_ID(header);

      // DevBus overrun workaround
      ppc_msync();
      fw_pdelay(1000);

      if (capabilityId == PCIE_INIT[i].address) {
	pcieConfigurationSpaceRoot = (uint64_t)nextHeader;
	break;
      }
      nextHeader = PCI_EXT_CAP_NEXT(header);
    }
  }
  else {
      FW_Error("Unsupported Set Configuration Space Root option (step %d).", i);
      return -1;
  }

  return 0;
}




int fw_pcie_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t data[16];
  fw_uint16_t count = 0;

  data[count++] = PE_DCR(PCIE_INTERRUPT_STATE__MACHINE_CHECK);
  data[count++] = DCRReadPriv( PE_DCR(PCIE_INTERRUPT_STATE__MACHINE_CHECK) );

  data[count++] = PCIE_UTL_DCR(0x08); // System Bus Agent Status Register
  data[count++] = DCRReadPriv( PCIE_UTL_DCR(0x08) );

  data[count++] = PCIE_UTL_DCR(0xA4); // PCI Express Port Status Register
  data[count++] = DCRReadPriv( PCIE_UTL_DCR(0xA4) );

  data[count++] = PCIE_UTL_DCR(0xB0); // Root Complex Status Register
  data[count++] = DCRReadPriv( PCIE_UTL_DCR(0xB0) );

  fw_machineCheckRas( FW_RAS_PCIE_MACHINE_CHECK, data, count, __FILE__, __LINE__ );
  return -1;
}


int fw_pcie_init( void ) {

  int i;
  int rc = 0;

  uint64_t start_time = GetTimeBase();

  TRACE(("(>) %s\n", __func__));

  extern void fw_serdes_pcie_init(void);
  fw_serdes_pcie_init();

  if ( PERS_ENABLED(PERS_ENABLE_Simulation) ) {
      printf("(W) Fast Sim has been disabled!\n");
  }

  for (i = 0; (i < sizeof(PCIE_INIT) / sizeof(PCIE_INIT[0]) ) && (rc == 0); i++ ) {
    
    INIT_STEP = i;

#ifdef PCIE_FLIGHT_RECORDER
    FR_DATA[i].step = i;
    FR_DATA[i].address = PCIE_INIT[i].address;
    FR_DATA[i].data = PCIE_INIT[i].data;
#endif

    TRACEV((
	   "step %3d) %s %08X %016lX  %d:%s\n",
	   i,
	   OPS[PCIE_INIT[i].operation], 
	   PCIE_INIT[i].address, 
	   PCIE_INIT[i].data,
	   PCIE_INIT[i].modifier, 
	   (PCIE_INIT[i].modifier < sizeof(MODS)/sizeof(MODS[0])) ? MODS[PCIE_INIT[i].modifier] : MODS[0]
	   ));

    switch ( PCIE_INIT[i].operation ) {

    case PCIE_DCR_READ : {
      break;
    }

    case PCIE_DCR_WRITE : {
      fw_pcie_write_dcr(i);
      break;
    }

    case PCIE_DCR_ASSERT : {
      rc = fw_pcie_assert_dcr(i);
      break;
    }

    case PCIE_CUSTOM : {
	rc = fw_pcie_custom(i);
	break;
    }

    case PCIE_DCR_POLL : {
      rc = fw_pcie_poll_dcr(i);
      break;
    }

    case PCIE_NOOP : {
      break;
    }

    case PCIE_CFGSPC_PREFIX : {
	volatile uint32_t* prefixRegister = (volatile uint32_t*)(PCIE_CONFIG_SPACE + 4096);
	*prefixRegister = PCIE_INIT[i].data;
      break;
    }

    case PCIE_CFGSPC_WRITE : {
	rc = fw_pcie_cfg_spc_write(i);
      break;
    }

    case PCIE_SET_CFG_ROOT : {
	rc = fw_pcie_set_cfg_spc_root(i);
      break;
    }

    case PCIE_CLR_CFG_ROOT :  {
	pcieConfigurationSpaceRoot = 0;
	break;
      }

    case PCIE_TERMINATE : {
      //Terminate(-1);
      return 0;
      break;
    }

    default : {
	FW_Error("Unsupported operation %d step=%d [%s:%d]", PCIE_INIT[i].operation, i, __func__, __LINE__ );
      rc = 0 - __LINE__;
    }

    }

    if ( rc != 0 ) {
      fw_uint64_t details[1];
      details[0] = i;
      fw_writeRASEvent( FW_RAS_PCIE_INIT_FAILURE, 1, details );
      return rc;
    }


    // DevBus overrun workaround
    ppc_msync();
    fw_pdelay(1000);



#ifdef PCIE_ECHO
    printf("[%3d] %s %08X %016lX->%016lX->%016lX (%d)\n", FR_DATA[i].step, OPS[PCIE_INIT[i].operation], FR_DATA[i].address, FR_DATA[i].before, FR_DATA[i].data, FR_DATA[i].after, FR_DATA[i].extra);
#endif

  }

  /*
   * These steps cannot be (easily) done in the table driven manner since they require
   * run-time configuration data (e.g. DDR size).
   *
   *   1) set the inbound read and write windows based on the actual
   *      DDR size. (@todo: what does this mean to domains?)
   */

  uint64_t tmp;
  unsigned ddrSize = FW_Personality.DDR_Config.DDRSizeMB / 256 ; // convert to 1/4 GB units

  if (ddrSize >= 0x100) {
    ddrSize = 0xFF;
  }


  tmp = DCRReadPriv( PE_DCR(PCIE_IFC_CFG) );
  
  PE_DCR__PCIE_IFC_CFG__IN_WR_PAGE_OFFSET_insert(tmp, 0);
  PE_DCR__PCIE_IFC_CFG__IN_WR_WINDOW_TOP_insert(tmp, ddrSize);
  PE_DCR__PCIE_IFC_CFG__IN_WR_WINDOW_BOTTOM_insert(tmp, 0x00);

  DCRWritePriv( PE_DCR(PCIE_IFC_CFG), tmp );


  tmp = DCRReadPriv( DEVBUS_DCR( PCIE_CFG ) );

  DEVBUS_DCR__PCIE_CFG__IN_RD_PAGE_OFFSET_insert( tmp, 0 );
  DEVBUS_DCR__PCIE_CFG__IN_RD_WINDOW_TOP_insert( tmp, ddrSize );
  DEVBUS_DCR__PCIE_CFG__IN_RD_WINDOW_BOTTOM_insert( tmp, 0x00 );

  DCRWritePriv( DEVBUS_DCR( PCIE_CFG ), tmp );

#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = {
    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__PCIE_RT_INT_set(1), //@todo is this right?
    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_1__PCIE_INT_set(1),
    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_2__PCIE_INT_set(1)
  };

  fw_installGeaHandler( fw_pcie_machineCheckHandler, mask );

#endif

  uint64_t end_time = GetTimeBase();
  fw_uint64_t duration = (end_time - start_time) / FW_Personality.Kernel_Config.FreqMHz / 1000ull;

  fw_writeRASEvent( FW_RAS_PCIE_INIT_DURATION, 1, &duration );

  TRACE(("(<) %s rc=%d\n", __func__, rc));
  return rc;
}

void fw_pcie_shutdown() {

    if ( PERS_ENABLED(PERS_ENABLE_PCIe) ) {

	TRACE(("(I) PCIe shutdown.\n"));

	// Mask all interrupts:
	DCRWritePriv( PE_DCR(PCIE_INTERRUPT_STATE_CONTROL_HIGH), 0 );
	DCRWritePriv( PE_DCR(PCIE_INTERRUPT_STATE_CONTROL_LOW), 0 );
	ppc_msync();

	// Reset the root complex
	DCRWritePriv(PE_DCR(PCIE_RESET), PE_DCR__PCIE_RESET__SYS_RESET_set(1));
    }

}

