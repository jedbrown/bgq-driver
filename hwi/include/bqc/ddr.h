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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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

#ifndef _DDR_H_
#define _DDR_H_


#include <hwi/include/common/bgq_bitnumbers.h>

__BEGIN_DECLS

// MC satalite 0
#define   _DDR_MC_MCFIR(x)  (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x00))    // MC Fault Isolation Register
#define   _DDR_MC_MCFIRC(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x01))    // MC Fault Isolation Register Clear
#define   _DDR_MC_MCFIRS(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x02))    // MC Fault Isolation Register Set
#define   _DDR_MC_MCFIRACT0(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x03)) // MC Fault Isolation Register Action 0
#define   _DDR_MC_MCFIRACT1(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x04)) // MC Fault Isolation Register Action 1
#define   _DDR_MC_MCFIRM(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x06))    // MC Fault Isolation Register Mask
#define   _DDR_MC_MCFIRMC(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x07))    // MC Fault Isolation Register Mask Clear
#define   _DDR_MC_MCFIRMS(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x08))    // MC Fault Isolation Register Mask Set


#define   _DDR_MC_MCECCDIS(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x09))    // ECC Disable, Error Inject Register MCA
#define   _DDR_MC_MCZMRINT(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x0A))    // ZQ, MEMCAL, Refresh Interval MCA
#define   _DDR_MC_MCAMISC(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x0B)) // Miscellaneous Mode Register MCA
#define   _DDR_MC_MCADTAL(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x0C)) // MCA Data Latency Register MCA
#define   _DDR_MC_MCTCNTL(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x0D)) // MCA Timer Control Register MCA
#define   _DDR_MC_MCACFG0(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x0E)) // MCA Configuration Register 0 MCA
#define   _DDR_MC_MCACFG1(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x0F)) // MCA Configuration Register 1 MCA
#define   _DDR_MC_MCAPERFMON0(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x10)) // MCA Performance Counter Register 0 MCA
#define   _DDR_MC_MCAPERFMON1(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x11)) // MCA Performance Counter Register 1 MCA
#define   _DDR_MC_MCAPERFMON2(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x12)) // MCA Performance Counter Register 2 MCA
#define   _DDR_MC_MCAPERFMON3(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x13)) // MCA Performance Counter Register 3 MCA
#define   _DDR_MC_MCAPERFMON4(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x14)) // MCA Performance Counter Register 4 MCA
#define   _DDR_MC_MCAPERFMON5(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x15)) // MCA Performance Counter Register 5 MCA
#define   _DDR_MC_MCAPERFMON6(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x16)) // MCA Performance Counter Register 6 MCA
#define   _DDR_MC_MCAPERFMON7(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x17)) // MCA Performance Counter Register 7 MCA
#define   _DDR_MC_MCAPERFMONC0(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x18))    // MCA Performance Monitor Control Register 0 MCA

#define   _DDR_MC_MCAPERFMONC1(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x19))    // MCA Performance Monitor Control Register 1  MCA
#define   _DDR_MC_MCAPERFMONC2(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x1A))    // MCA Performance Monitor Control Register 2  MCA
#define   _DDR_MC_MCAISC(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x1B))    // Idle State Control Register   MCA
#define   _DDR_MC_MCARETRY(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x1C))    // Retry, CRC Error Counter Control Register   MCA
#define   _DDR_MC_MCAPOS(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x1D))  // Power On Sequence Register  MCA
#define   _DDR_MC_MCAODT0(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x1E)) // MCA ODT Channel 0 Register  MCA
#define   _DDR_MC_MCAODT1(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x1F)) // MCA ODT Channel 1 Register  MCA
#define   _DDR_MC_MCMCT(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x20))   // Maintenance Command Type Register   MCA
#define   _DDR_MC_MCMISMODE(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x23))   // Miscellaneous Mode Register MCA
#define   _DDR_MC_MCMCC(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x24))   // Maintenance Command Control Register    MCA
#define   _DDR_MC_MCMACA(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x25))  // Maintenance Address Control Register    MCA
#define   _DDR_MC_MCMEA(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x26))   // Maintenance End Address Register    MCA
#define   _DDR_MC_MCSSC(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x27))   // Memory Scrub Syndrome Count Register    MCA
#define   _DDR_MC_MCSSY(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x28))   // Memory Scrub Syndrome Register  MCA
#define   _DDR_MC_MCSCTL(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x2C))  // Memory Scrub Control Register   MCA
#define   _DDR_MC_MCRADR(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x2D))  // Memory Read Error Address Register  MCA
#define   _DDR_MC_MCRESY(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x2E))  // Memory Read Error Syndrome Register MCA
#define   _DDR_MC_MCECTL(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x2F))  // Error Lock Control Register MCA
#define   _DDR_MC_MCSPA(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x31))   // Special Attention Register  MCA
#define   _DDR_MC_MCSPAAND(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x32))    // Special Attention AND Register  MCA
#define   _DDR_MC_MCSPAOR(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x33)) // Special Attention OR Register   MCA
#define   _DDR_MC_MCSPAMSK(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x34))    // Special Attention Mask Register MCA
#define   _DDR_MC_MCDACR(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x35))  // Display/Alter Control Register  MCA
#define   _DDR_MC_MCDADR(x) (DR_ARB_DCR(x, SCOM_MC)+((0<<6)|0x36))  // Display/Alter Data Register MCA

// MC satalite 1
#define   _DDR_MC_MRKSTDTA0(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x00))   // Marking Store Data Register 0   MCA
#define   _DDR_MC_MRKSTDTA1(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x01))   // Marking Store Data Register 1   MCA
#define   _DDR_MC_MRKSTDTA2(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x02))   // Marking Store Data Register 2   MCA
#define   _DDR_MC_MRKSTDTA3(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x03))   // Marking Store Data Register 3   MCA
#define   _DDR_MC_MRKSTDTA4(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x04))   // Marking Store Data Register 4   MCA
#define   _DDR_MC_MRKSTDTA5(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x05))   // Marking Store Data Register 5   MCA
#define   _DDR_MC_MRKSTDTA6(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x06))   // Marking Store Data Register 6   MCA
#define   _DDR_MC_MRKSTDTA7(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x07))   // Marking Store Data Register 7   MCA

// NOTE: the mark/store regs 8-15 are Prism-only registers.  BQC uses these SCOM addresses for ECC
//       counters.

//#define   _DDR_MC_MRKSTDTA8(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x08))   // Marking Store Data Register 8   MCA
//#define   _DDR_MC_MRKSTDTA9(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x09))   // Marking Store Data Register 9   MCA
//#define   _DDR_MC_MRKSTDTA10(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0A))  // Marking Store Data Register 10  MCA
//#define   _DDR_MC_MRKSTDTA11(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0B))  // Marking Store Data Register 11  MCA
//#define   _DDR_MC_MRKSTDTA12(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0C))  // Marking Store Data Register 12  MCA
//#define   _DDR_MC_MRKSTDTA13(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0D))  // Marking Store Data Register 13  MCA
//#define   _DDR_MC_MRKSTDTA14(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0E))  // Marking Store Data Register 14  MCA
//#define   _DDR_MC_MRKSTDTA15(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0F))  // Marking Store Data Register 15  MCA

#define   _DDR_MC_SSECOUNT(x)  (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x08))  // Single Symbol Error Count
#define   _DDR_MC_DSECOUNT(x)  (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x09))  // Double Symbol Error Count
#define   _DDR_MC_CKCOUNT(x)   (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0A))  // Chip Kill Count
#define   _DDR_MC_SWEPOSAB(x)  (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0B))  // Single Wire Error Position A/B
#define   _DDR_MC_SWECOUNTA(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0C))  // Single Wire Error Count A
#define   _DDR_MC_SWECOUNTB(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0D))  // Single Wire Error Count B
#define   _DDR_MC_SWECOUNTO(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0E))  // Single Wire Error Count (Other)
#define   _DDR_MC_SWETHRESH(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x0F))  // Single Wire Error Threshold

#define   _DDR_MC_MCMTC(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x10))   // Memory Test Control MCA
#define   _DDR_MC_MCMTRS(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x11))  // Memory Test Rank Select MCA
#define   _DDR_MC_MCMTDMH(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x12)) // Memory Test Data Mask High  MCA
#define   _DDR_MC_MCMTDML(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x13)) // Memory Test Data Mask Low   MCA
#define   _DDR_MC_MCMTRDPH(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x14))    // Memory Test Rising Data Pattern High    MCA
#define   _DDR_MC_MCMTRDPL(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x15))    // Memory Test Rising Data Pattern Low MCA
#define   _DDR_MC_MCMTFDPH(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x16))    // Memory Test Falling Data Pattern High   MCA
#define   _DDR_MC_MCMTFDPL(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x17))    // Memory Test Falling Data Pattern Low    MCA
#define   _DDR_MC_MCMTSBEC(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x18))    // Memory Test Single-Bit Error Count  MCA
#define   _DDR_MC_MCMTMBEC(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x19))    // Memory Test Multi-Bit Error Count   MCA
#define   _DDR_MC_MCMTADH(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x1A)) // Memory Test Actual Data High    MCA
#define   _DDR_MC_MCMTADL(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x1B)) // Memory Test Actual Data Low MCA
#define   _DDR_MC_MCMTEDH(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x1C)) // Memory Test Expected Data High  MCA
#define   _DDR_MC_MCMTEDL(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x1D)) // Memory Test Expected Data Low   MCA
#define   _DDR_MC_MCMTEA(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x1E))  // Memory Test Error Address   MCA
#define   _DDR_MC_MCMLCNT(x) (DR_ARB_DCR(x, SCOM_MC)+((1<<6)|0x1F))  // Memory Test Loop Counter for Slam Test   MCA


// MC satalite 2
#define   _DDR_MC_TRACEHIDATA(x) (DR_ARB_DCR(x, SCOM_MC)+((2<<6)|0x00)) // Trace Array High Data Register  MCA
#define   _DDR_MC_TRACELODATA(x) (DR_ARB_DCR(x, SCOM_MC)+((2<<6)|0x01)) // Trace Array Low Data Register   MCA


// MCS satalite 0
#define   _DDR_MC_MCFGP(x) (DR_ARB_DCR(x, SCOM_MCS)+0x00)   // MC Primary Configuration Register   MCS
#define   _DDR_MC_MCFGC0(x) (DR_ARB_DCR(x, SCOM_MCS)+0x01)  // MC Channel 0 Configuration Register MCS
#define   _DDR_MC_MCFGC1(x) (DR_ARB_DCR(x, SCOM_MCS)+0x02)  // MC Channel 1 Configuration Register MCS
#define   _DDR_MC_MCNTCFG(x) (DR_ARB_DCR(x, SCOM_MCS)+0x03) // MC Packet Counters Configuration Register   MCS
#define   _DDR_MC_MEPSILON(x) (DR_ARB_DCR(x, SCOM_MCS)+0x04)    // MC Epsilon Counters Register    MCS
#define   _DDR_MC_MCTL(x) (DR_ARB_DCR(x, SCOM_MCS)+0x05)    // MC Control Register MCS
#define   _DDR_MC_MCMODE(x) (DR_ARB_DCR(x, SCOM_MCS)+0x09)  // MC Mode Register    MCS
#define   _DDR_MC_MCCLMSK(x) (DR_ARB_DCR(x, SCOM_MCS)+0x0A) // MCS Command List Mask Register  MCS
#define   _DDR_MC_MCBCFG(x) (DR_ARB_DCR(x, SCOM_MCS)+0x0B)  // MCB Configuration Register  MCS
#define   _DDR_MC_MCSPERFMON0(x) (DR_ARB_DCR(x, SCOM_MCS)+0x0C) // MCS Performance Counter Register 0  MCS
#define   _DDR_MC_MCSPERFMON1(x) (DR_ARB_DCR(x, SCOM_MCS)+0x0D) // MCS Performance Counter Register 1  MCS
#define   _DDR_MC_MCSPERFMON2(x) (DR_ARB_DCR(x, SCOM_MCS)+0x0E) // MCS Performance Counter Register 2  MCS
#define   _DDR_MC_MCSPERFMON3(x) (DR_ARB_DCR(x, SCOM_MCS)+0x0F) // MCS Performance Counter Register 3  MCS
#define   _DDR_MC_MCSPERFMON4(x) (DR_ARB_DCR(x, SCOM_MCS)+0x10) // MCS Performance Counter Register 4  MCS
#define   _DDR_MC_MCSPERFMON5(x) (DR_ARB_DCR(x, SCOM_MCS)+0x11) // MCS Performance Counter Register 5  MCS
#define   _DDR_MC_MCSPERFMON6(x) (DR_ARB_DCR(x, SCOM_MCS)+0x12) // MCS Performance Counter Register 6  MCS
#define   _DDR_MC_MCSPERFMON7(x) (DR_ARB_DCR(x, SCOM_MCS)+0x13) // MCS Performance Counter Register 7  MCS



#define DDR_MR0                         0x00000000    // Specifies mode register MR0.
#define DDR_MR1                         0x00010000    // Specifies mode register MR1.
#define DDR_MR2                         0x00020000    // Specifies mode register MR2.
#define DDR_MR3                         0x00030000    // Specifies mode register MR3.

#define DDR_MR0_PPD_SLOW_EXIT           0x00000000    // Sets DLL control for precharge PD to slow exit.
#define DDR_MR0_PPD_FAST_EXIT           0x00001000    // Sets DLL control for precharge PD to fast exit.
#define DDR_MR0_DLL_RESET_OFF           0x00000000    // Sets DLL reset off.
#define DDR_MR0_DLL_RESET_ON            0x00000100    // Sets DLL reset on.
#define DDR_MR0_WR_25                   0x00000200    // Sets min write recovery for autoprecharge to 25 clock cycles.
#define DDR_MR0_WR_36                   0x00000400    // Sets min write recovery for autoprecharge to 36 clock cycles.
#define DDR_MR0_WR_49                   0x00000600    // Sets min write recovery for autoprecharge to 49 clock cycles.
#define DDR_MR0_WR_64                   0x00000800    // Sets min write recovery for autoprecharge to 64 clock cycles.
#define DDR_MR0_WR_100                  0x00000A00    // Sets min write recovery for autoprecharge to 100 clock cycles.
#define DDR_MR0_WR_144                  0x00000B00    // Sets min write recovery for autoprecharge to 144 clock cycles.
#define DDR_MR0_CAS_LATENCY_5           0x00000010    // Sets the CAS latency to 5ns.
#define DDR_MR0_CAS_LATENCY_6           0x00000020    // Sets the CAS latency to 6ns.
#define DDR_MR0_CAS_LATENCY_7           0x00000030    // Sets the CAS latency to 7ns.
#define DDR_MR0_CAS_LATENCY_8           0x00000040    // Sets the CAS latency to 8ns.
#define DDR_MR0_CAS_LATENCY_9           0x00000050    // Sets the CAS latency to 9ns.
#define DDR_MR0_CAS_LATENCY_10          0x00000060    // Sets the CAS latency to 10ns.
#define DDR_MR0_CAS_LATENCY_11          0x00000070    // Sets the CAS latency to 11ns (optional for DDR3-1600).
#define DDR_MR0_RBT_NIBBLE_SEQUENTIAL   0x00000000    // Sets the read burst type to nibble sequential.
#define DDR_MR0_RBT_INTERLEAVE          0x00000008    // Sets the read burst type to interleave.
#define DDR_MR0_BL_FIXED_8              0x00000000    // Sets the burst length to fixed BL8.
#define DDR_MR0_BL_ON_THE_FLY           0x00000001    // Sets the burst length to on-the-fly BC4 or BL8.
#define DDR_MR0_BL_FIXED_BC4            0x00000002    // Sets the burst length to fixed BC4.

#define DDR_MR1_TDQS_DISABLE            0x00000000    // Disables TDQS.
#define DDR_MR1_TDQS_ENABLE             0x00000800    // Enables TDQS.
#define DDR_MR1_WRITE_LEVELING_DISABLE  0x00000000    // Disables write leveling.
#define DDR_MR1_WRITE_LEVELING_ENABLE   0x00000080    // Enables write leveling.
#define DDR_MR1_OUTPUT_BUFFER_DISABLE   0x00001000    // Disables output buffer.
#define DDR_MR1_OUTPUT_BUFFER_ENABLE    0x00000000    // Enables output buffer.
#define DDR_MR1_AL_DISABLED             0x00000000    // Disables additive latency.
#define DDR_MR1_AL_CL1                  0x00000008    // Sets additive latency to CL-1.
#define DDR_MR1_AL_CL2                  0x00000010    // Sets additive latency to CL-2.
#define DDR_MR1_RTTNOM_DISABLED         0x00000000    // Disables Rtt_Nom.
#define DDR_MR1_RTTNOM_RZQ4             0x00000000    // Sets Rtt_Nom to 60 Ohms.
#define DDR_MR1_RTTNOM_RZQ2             0x00000000    // Sets Rtt_Nom to 120 Ohms.
#define DDR_MR1_RTTNOM_RZQ6             0x00000000    // Sets Rtt_Nom to 40 Ohms.
#define DDR_MR1_RTTNOM_RZQ12            0x00000000    // Sets Rtt_Nom to 20 Ohms.
#define DDR_MR1_RTTNOM_RZQ8             0x00000000    // Sets Rtt_Nom to 30 Ohms.
#define DDR_MR1_DLL_ENABLE              0x00000000    // Enables DLL (delay lock loop).
#define DDR_MR1_DLL_DISABLE             0x00000001    // Disables DLL (delay lock loop).
#define DDR_MR1_ODIC_RZQ6               0x00000000    // Sets output driver impedance control to 40 Ohms.
#define DDR_MR1_ODIC_RZQ7               0x00000002    // Sets output driver impedance control to 34.286 Ohms.


//#define DDR_MR0                        (0x2E)         // DRAM Maintenance 0
//#define DDR_MR1                        (0x2F)         // DRAM Maintenance 1 (all reserved)
//#define DDR_MAINT_CHIPSEL(x)           _B4(3,x)       //  Chip Selects
//#define DDR_MAINT_ALL_CS               _BN(4)         //  All CS, overrides CHIPSEL
//#define DDR_MAINT_REFR_ACTIVE          _BN(5)         //  Self Timed Refresh Active
//#define DDR_MAINT_CLK_ENABLE           _BN(6)         //  Enable SDRAM Clocks
//#define DDR_MAINT_CLK_DISABLE          _BN(7)         //  Disable SDRAM Clocks
//#define DDR_MAINT_CKE_SET              _BN(8)         //  Set CKE
//#define DDR_MAINT_CKE_RESET            _BN(9)         //  Reset CKE
//#define DDR_MAINT_CMD(x)               _B3(12,x)      //  Command: controls RAS,CAS,WE bits
//#define DDR_MAINT_CMD_RAS              _BN(10)        //   Drive RAS
//#define DDR_MAINT_CMD_CAS              _BN(11)        //   Drive CAS
//#define DDR_MAINT_CMD_WE               _BN(12)        //   Drive WE
//#define DDR_MR_BA(x)                   _B3(15,x)      //  Internal Bank Addr (BA 0-2)
//#define DDR_MR_BA0                     DDR_MR_BA(0x4)
//#define DDR_MR_BA1                     DDR_MR_BA(0x2)
//#define DDR_MR_BA2                     DDR_MR_BA(0x1)
//#define DDR_MR_ADDR(x)                 _B16(31,x)     //  Control Addr Pins 0-15 (Bit Number Reversal compared to JEDEC spec)
//#define DDR_MR_ADDR_A0                 DDR_MR_ADDR(0x8000)
//#define DDR_MR_ADDR_A1                 DDR_MR_ADDR(0x4000)
//#define DDR_MR_ADDR_A2                 DDR_MR_ADDR(0x2000)
//#define DDR_MR_ADDR_A3                 DDR_MR_ADDR(0x1000)
//#define DDR_MR_ADDR_A4                 DDR_MR_ADDR(0x0800)
//#define DDR_MR_ADDR_A5                 DDR_MR_ADDR(0x0400)
//#define DDR_MR_ADDR_A6                 DDR_MR_ADDR(0x0200)
//#define DDR_MR_ADDR_A7                 DDR_MR_ADDR(0x0100)
//#define DDR_MR_ADDR_A8                 DDR_MR_ADDR(0x0080)
//#define DDR_MR_ADDR_A9                 DDR_MR_ADDR(0x0040)
//#define DDR_MR_ADDR_A10                DDR_MR_ADDR(0x0020)
//#define DDR_MR_ADDR_A11                DDR_MR_ADDR(0x0010)
//#define DDR_MR_ADDR_A12                DDR_MR_ADDR(0x0008)
//#define DDR_MR_ADDR_A13                DDR_MR_ADDR(0x0004)
//#define DDR_MR_ADDR_A14                DDR_MR_ADDR(0x0002)
//#define DDR_MR_ADDR_A15                DDR_MR_ADDR(0x0001)


#define DDR0_DCR                       0x078000       // DDR controller 0 base DCR address.
#define DDR1_DCR                       0x07C000       // DDR controller 1 base DCR address.

// Note: these values are ported from BG/P and need to be fixed for BG/Q once the hardware has been defined.
// Note: these DCR values need to move into a DCR map header file if we end up creating one.
#define DDR0_DCR_END       (DDR0_DCR + 0x0FF)        // 0xEFF: DDR controller 0 End
#define DDR1_DCR_END       (DDR1_DCR + 0x0FF)        // 0xFFF: DDR controller 1 End

#define DDR_MAINT0                      (0x2E)       // DRAM Maintenance 0 address
#define DDR_MAINT1                      (0x2F)       // DRAM Maintenance 1 address (all reserved)

#define DDR_MAINT0_CLK_ENABLE           _BN(6)       //  Enable SDRAM clocks.
#define DDR_MAINT0_CLK_DISABLE          _BN(7)       //  Disable SDRAM clocks.


// IOMx sataliate 0
#define   _DDR_MC_IOM_PHYREGINDEX(x) (DR_ARB_DCR(x, SCOM_IOM)+0x00) // Indirect Addressing - Index register    IOM_MC
#define   _DDR_MC_IOM_PHYREGDATA(x) (DR_ARB_DCR(x, SCOM_IOM)+0x01)  // Indirect Addressing - Data register IOM_MC
#define   _DDR_MC_IOM_PHYWRITE(x) (DR_ARB_DCR(x, SCOM_IOM)+0x00) // Indirect Addressing - PHY write IOM_MC
#define   _DDR_MC_IOM_PHYREAD(x) (DR_ARB_DCR(x, SCOM_IOM)+0x01)  // Indirect Addressing - PHY read IOM_MC
#define   _DDR_MC_IOM_VREF_CNTL(x) (DR_ARB_DCR(x, SCOM_IOM)+0x02)   // DIMM Vref Control Register  IOM_MC
#define   _DDR_MC_IOM_INT_VREF(x) (DR_ARB_DCR(x, SCOM_IOM)+0x03)    // DDR3 Internal Vref Control Register  IOM_MC
#define   _DDR_MC_IOM_IO_IMPEDANCE(x) (DR_ARB_DCR(x, SCOM_IOM)+0x04)    // DDR3 IO Impedance Control Register  IOM_MC
#define   _DDR_MC_IOM_ADDR_IMP(x) (DR_ARB_DCR(x, SCOM_IOM)+0x05)    //
#define   _DDR_MC_IOM_DATA_IMP(x) (DR_ARB_DCR(x, SCOM_IOM)+0x06)    //
#define   _DDR_MC_IOM_TERM_IMP(x) (DR_ARB_DCR(x, SCOM_IOM)+0x07)    //
#define   _DDR_MC_IOM_IOM_HW_ERR(x) (DR_ARB_DCR(x, SCOM_IOM)+0x08)    //
#define   _DDR_MC_IOM_IOM_HW_ERRM(x) (DR_ARB_DCR(x, SCOM_IOM)+0x09)    //
#define   _DDR_MC_IOM_FIFO_CNTL(x) (DR_ARB_DCR(x, SCOM_IOM)+0x0A)    //
#define   _DDR_MC_IOM_CAL_CNTL(x) (DR_ARB_DCR(x, SCOM_IOM)+0x0B)    //
#define   _DDR_MC_IOM_CAL_CNTL2(x) (DR_ARB_DCR(x, SCOM_IOM)+0x0C)    //
#define   _DDR_MC_IOM_SCOPE_SEL(x) (DR_ARB_DCR(x, SCOM_IOM)+0x0D)    //


// Register: MCFIR
// DDR Controller Fault Isolation Register

#define MCFIR_SBOX_ERROR _BN(0) // A parity error or check sequence error was detected on either the
          // Write Completion or Cancel Acknowledge asynchronous interfaces between MCA and MCS.
#define MCFIR_MCS_MNT_ERROR _BN(1) // A parity error was detected on the buses between MCA and MCS which
          // specify the address and operation type.
#define MCFIR_DISPSATHER_COLLISION_ERROR _BN(2) // An address collision occurred between two PBUS reflected commands
          // (see 2.2.5 Collision Rules on page 41 for more details).
#define MCFIR_WRITE_HANG_ERROR _BN(3) // A PBUS write hang has been detected. A write remains outstanding
          // after 3 hang pulses have arrived on the PBUS.
#define MCFIR_MCFGP_REGISTER_PARITY_ERROR _BN(5) // 
#define MCFIR_MCFGC0_REGISTER_PARITY_ERROR _BN(6) // 
#define MCFIR_MCFGC1_REGISTER_PARITY_ERROR _BN(7) // 
#define MCFIR_MCNTCFG_REGISTER_PARITY_ERROR _BN(8) // 
#define MCFIR_MCMODE_REGISTER_PARITY_ERROR _BN(9) // 
#define MCFIR_MEPSILON_REGISTER_PARITY_ERROR _BN(10) // 
#define MCFIR_MCTL_REGISTER_PARITY_ERROR _BN(11) // 
#define MCFIR_MCBCFG_REGISTER_PARITY_ERROR _BN(12) // 
#define MCFIR_MDI_BIT_ERROR _BN(13) // 
#define MCFIR_READ_RETURN_ERROR _BN(14) // 
#define MCFIR_PACKET_COUNTER_ADDRESS_UE _BN(15) // This bit is set when an Address UE has been detected during a Packet
          // Counter operation.
#define MCFIR_MCAMISC_REGISTER_PARITY_ERROR _BN(16) // 
#define MCFIR_MCTCNTL_REGISTER_PARITY_ERROR _BN(17) // 
#define MCFIR_MCADTAL_REGISTER_PARITY_ERROR _BN(18) // 
#define MCFIR_MCACFG0_REGISTER_PARITY_ERROR _BN(19) // 
#define MCFIR_MCAECCDIS_REGISTER_PARITY_ERROR _BN(20) // 
#define MCFIR_MAINTENANCE_INTERFACE_ERROR _BN(21) // 
#define MCFIR_ASYNC_SERIALIZER_RCMD_ERROR _BN(22) // 
#define MCFIR_ASYNC_SERIALIZER_CRESP_ERROR _BN(23) // 
#define MCFIR_WRITE_COMPLETION_READ_DONE_ERROR _BN(24) // 
#define MCFIR_WRITE_COMPLETION_CANCEL_ERROR _BN(25) // 
#define MCFIR_RSMFIFO_PARITY_ERROR _BN(26) // 
#define MCFIR_READ_BUFFER_OVERRUN _BN(27) // 
#define MCFIR_READ_BUFFER_UNDERRUN _BN(28) // 
#define MCFIR_MEMORY_CE _BN(32) // This bit is set when a memory CE is detected on a non-maintenance
          // memory read op.
#define MCFIR_MEMORY_FAST_DECODER_UE _BN(33) // This bit is set when a memory UE is detected by the Fast Decoder
          // on a non-maintenance memory read op.
#define MCFIR_MEMORY_SUE _BN(34) // This bit is set when a memory SUE is detected on a non-maintenance
          // memory read op.
#define MCFIR_MAINTENANCE_CE _BN(35) // This bit is set when a memory CE is detected on a maintenance memory
          // read op.
#define MCFIR_MAINTENANCE_FAST_DECODER_UE _BN(36) // This bit is set when a memory UE is detected by the Fast Decoder
          // on a maintenance memory read op.
#define MCFIR_MAINTENANCE_SUE _BN(37) // This bit is set when a memory SUE is detected on a maintenance memory
          // read op.
#define MCFIR_POWERBUS_WRITE_BUFFER_CE _BN(38) // This bit is set when a PBUS ECC CE is detected on a PBus write buffer
          // read op.
#define MCFIR_POWERBUS_WRITE_BUFFER_UE _BN(39) // This bit is set when a PBUS ECC UE is detected on a PowerBus write
          // buffer read op.
#define MCFIR_POWERBUS_WRITE_BUFFER_SUE _BN(40) // This bit is set when a PBUS ECC SUE is detected on a PowerBus write
          // buffer read op.
#define MCFIR_POWERBUS_READ_BUFFER_CE _BN(41) // This bit is set when a PBUS ECC CE is detected on a PowerBus read
          // buffer read op.
#define MCFIR_POWERBUS_READ_BUFFER_UE _BN(42) // This bit is set when a PBUS ECC UE is detected on a PowerBus read
          // buffer read op.
#define MCFIR_POWERBUS_READ_BUFFER_SUE _BN(43) // This bit is set when a PBUS ECC SUE is detected on a PowerBus read
          // buffer read op.
#define MCFIR_PACKET_COUNTER_DATA_SUE _BN(44) // This bit is set when an ECC data SUE is detected on a Packet Counter
          // operation.
#define MCFIR_READ_CHANNEL_BUFFER_PARITY_ERROR _BN(45) // Parity when data is read from channel burrer during ECC correction
          // process.
#define MCFIR_MEMORY_ECC_MARKING_STORE_UPDATED _BN(46) // BGQ: This bit is set when the memory ECC check logic has updated
          // the marking store. Firmware must read the marking store to determine which entry (or entires) have
          // been updated.
#define MCFIR_MEMORY_ECC_MARKING_STORE_PARITY_ERROR _BN(47) // This bit is set when a parity error is detected when the ECC mark
          // store is read by the ECC check logic. If a parity error is detected, the ECC check logic uses zero
          // mark data for the correction (i.e. nothing marked).
#define MCFIR_WRITE_ECC_GEN_PARITY_ERROR _BN(48) // A data parity error was detected in the write dataflow, by the Memory
          // ECC generation logic.
#define MCFIR_MEMORY_REFRESH_ERROR _BN(49) // 
#define MCFIR_PACKET_COUNTER_CORRECTABLE_ERRORS _BN(50) // This bit is set for the following errors: ECC address CE during a
          // Packet Counter operation, a non-supported PBUS ttype to Packet Counter address space or a non-supported
          // address alignment for a Packet Counter operation.
#define MCFIR_PACKET_COUNTER_DATA_CE _BN(51) // This bit is set when an ECC data CE is detected during a Packet Counter
          // operation.
#define MCFIR_IOM0_PHY_ERROR _BN(52) // 
#define MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED _BN(53) // BGQ: ECC ERROR COUNTER THRESHOLD REACHED
#define MCFIR_MCA_REGISTER_PARITY_ERROR_RECOVERABLE _BN(54) // This bit is set for the following errors:MCARETRY parity error, MCZMRINT
          // parity error or MCACFG1 parity error. Firmware can read these registers to determine which one is
          // in error, and rewrite the correct value.
#define MCFIR_MCS_REGISTER_PARITY_ERROR_RECOVERABLE _BN(55) // This bit is set for the following errors:MCCLMSK parity error or
          // MCDBGSEL parity error. Firmware can read these registers to determine which one is in error, and
          // rewrite the correct value.
#define MCFIR_INVALID_MAINTENANCE_COMMAND _BN(56) // This bit is set when a maintenance command is started with an invalid
          // maintenance command code entered in MCMCT
#define MCFIR_NO_CHANNELS_SELECTED _BN(57) // A reflected command address matches the addresses accepted by this
          // MC (matches MCFGP address/mask), but does not lie within the address range of either channel. If
          // this condition is detected, MCFIR(23) is set and the operation is not enqueued in a MCS command list
          // entry. This bit is set due to a software error (read/write address does not match memory range behind
          // this MC), or a hardware error (read/write address does match memory range behind this MC, but MC
          // does not respond correctly).
#define MCFIR_MULTIPLE_CHANNELS_SELECTED _BN(58) // A reflected command address matches the addresses accepted by this
          // MC (matches MCFGP address/mask), and also matches more than one channel (matches multiple MCFGE addresses/masks).
          // If this condition is detected, MCFIR(24) is set and the operation is not enqueued in a MCS command
          // list entry. This bit is set due to a software error (read/write address matches multiple memory ranges
          // behind this MC), or a hardware error (read/write address does not match multiple memory ranges behind
          // this MC, but MC extent hardware detects multiple matches).
#define MCFIR_ADDRESS_BAD_EXTENT _BN(59) // This bit is set when a PBUS reflected command arrives and the sum
          // of the extents of Channels 0 and 1 do not match the total extent specified for the memory controller.
          // These extents are specified in registers MCFGC0 bits 0:8, MCFG1 bits 0:8 and MCFGP bits 0:9 respectively.
          // This bit is set when MCFGC0(0:8) + MCFGC1(0:8) does not equal MCFGP(0:9) when a PBUS reflected command
          // arrives.
#define MCFIR_INVALID_RANK _BN(60) // This bit is set when the logical rank address (given in the PBUS
          // reflected command address) does not map onto a valid physical rank.
#define MCFIR_SCOM_INTERFACE_ERROR _BN(61) // This bit is a general summary of Scom interface errors detected by
          // the MCS scom satellite or the MCA scom satellite.
#define MCFIR_HTM_HPC_WRITE_COLLISION _BN(62) // 
#define MCFIR_MC_SCOM_ERROR _BN(63) // This bit is set when an internal error is detected by the scom fir
          // logic.


#define MCAMISC_NUM_IDLES_get(x)       _BGQ_GET(7,43,x)
#define MCAMISC_NUM_IDLES_set(v)       _BGQ_SET(7,43,v)
#define MCAMISC_NUM_IDLES_insert(x,v)  _BGQ_INSERT(7,43,x,v)


#define MCMCC_MAINTENANCE_COMMAND_VALID                     _BN(0)
#define MCMCC_MAINTENANCE_COMMAND_IN_PROGRESS               _BN(1)
#define MCMCC_MAINTENANCE_COMMAND_COMPLETE                  _BN(2)
#define MCMCC_ENABLE_SPATTN_ON_MAINTENANCE_COMMAND_COMPLETE _BN(3)
#define MCMCC_RESERVE_READ_BUFFER_REQUEST                   _BN(4)
#define MCMCC_READ_BUFFERED_RESERVED                        _BN(5)
#define MCMCC_STOP_ON_MAINT_TYPE1_END_ADDRESS               _BN(6)

__END_DECLS

#endif
