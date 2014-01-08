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
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/l2_util.h>
#include <hwi/include/bqc/l2_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/bqc/bedram_dcr.h>
#include <hwi/include/bqc/DDR_metrics.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <hwi/include/bqc/classroute.h>
#include <hwi/include/bqc/MU_Macros.h>
#include <hwi/include/bqc/MU_Pt2PtNetworkHeader.h>
#include <spi/include/mu/GIBarrier.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"
//#include "repro_mcreinit.h"
//#include "repro_iom.h"
#include "repro_ddr.h"

extern void exit(int);

// Define the address bit index matrix for the inverse scrambling 
// Indices are relative to the 0th bit of the physical address (bit 0 is MSB of address), not the 64-bit char* addr
static signed int scr_mat[4][8] = { {25, 21, 17, 13, 9, 5, 1, -1}, {26, 22, 18, 14, 10, 6, 2, -1}, 
                             {27, 23, 19, 15, 11, 7, 3, -1}, {28, 24, 20, 16, 12, 8, 4, 0} };

static signed int scramble_src[29]  = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                -1,-1,-1,-1,-1,-1,-1,-1,-1}; // The source bit indices which need to be moved
static signed int scramble_dest[29] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                -1,-1,-1,-1,-1,-1,-1,-1,-1}; // The destination bit indices which need to be moved
// E.g. if scramble_src[0] = 17 and scramble_dest[0] = 25, then bit 17 of the input address to scramble() will be moved 
// to bit 25 of the return address of scramble().
static unsigned int n_to_scramble  = 0; // The number of bits which need to be moved

static uint64_t keep_mask = 0x000000000000007f; // Masked with the address in scramble() to keep certain bits (unscrambled)


int fw_resetQuiesceNetwork()
{
   return 0;
}

int fw_resetSyncMultiNode()
{
    int rc;
    uint64_t tbstart;
    uint64_t tbend;
    uint64_t tbhalfway;
    MUSPI_GIBarrier_t GIBarrier;
    uint64_t value;
    uint64_t config = FW_Personality.Kernel_Config.NodeConfig;
    
    if(_PERS_ENABLED(PERS_ENABLE_PartitionRepro, config))
    {
        tlbwe(
            MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB | MAS1_IPROT(1),
            MAS2_EPN(   (PHYMAP_MINADDR_MMIO) >> 12) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
            MAS7_3_RPN( (PHYMAP_MINADDR_MMIO) >> 12) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX \
            (0),
            MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
            MMUCR3_X(0) | MMUCR3_R(1) |     MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
            );
        
        // set gsync_counter
        DCRWritePriv(TESTINT_DCR(GSYNC_CTR), -1ULL);
        do
        {
            value = DCRReadPriv(TESTINT_DCR(GSYNC_CTR));
        }
        while(value == -1ULL);
        
        DCRWritePriv (TESTINT_DCR(GSYNC_CTR), -1ULL);
        tbstart = GetTimeBase();
        do
        {
            value = DCRReadPriv(TESTINT_DCR(GSYNC_CTR));
        }
        while(value == -1ULL);
        tbend = GetTimeBase();
        
        tbhalfway = (tbend-tbstart)/2 + tbend;
        while(GetTimeBase() < tbhalfway) { } 
        
        if(PERS_ENABLED(PERS_ENABLE_MU))
        {
            MUSPI_GIBarrierInit (&GIBarrier, FW_GI_BARRIER_BOOT_CLASS_ROUTE);
            rc = MUSPI_GIBarrierEnterAndWaitWithTimeout(&GIBarrier, (tbend-tbstart)/2);
            if(rc)
            {
                FW_Error("Repro reset barrier timeout!  rc=0x%X.", rc);
                crash(-1);
            }
        }
        
        DCRWritePriv(TESTINT_DCR(GSYNC_CTR), -1ULL);
        do
        {
            value = DCRReadPriv(TESTINT_DCR(GSYNC_CTR));
        }
        while(value == -1ULL);

        tbhalfway = (tbend-tbstart)/2 + GetTimeBase();
        while(GetTimeBase() < tbhalfway) { } 
    }
    
    DCRWritePriv(TESTINT_DCR(GSYNC_CTR), 166); // .03 seconds * 166 = ~5 seconds
    
    return 0;
}

int fw_resetInvalidateTLBs()
{
   int slot;
   uint64_t address;
   uint64_t output, w0, w1;
   
   uint64_t mask = ~(1<<(ProcessorThreadID()));
   mtspr(SPRN_TENC,((mask) & 0xf));

   // touch BeDRAM derat before erasing TLBs. ierat is already loaded by instruction stream
   address = PHYMAP_MINADDR_BEDRAM + PHYMAP_PRIVILEGEDOFFSET;
   asm volatile ("lwz %0, 0(%1);"
                 "isync;"
                 "msync;" : "=r" (output) : "b" (address) : "memory");
   
   for(slot=0; slot<32; slot++)
   {
      eratre(slot, &w0, &w1, MMUCR0_TLBSEL_DERAT);
      if((w1 & ERAT1_RPN(-1)) != ((PHYMAP_MINADDR_BEDRAM + PHYMAP_PRIVILEGEDOFFSET) & ~(1024*1024*1024-1)))
      {
         eratwe(slot, 0, 0, MMUCR0_TLBSEL_DERAT);
      }
      else
      {
         eratwe(slot, w0, w1, MMUCR0_TLBSEL_DERAT | MMUCR0_ECL(1));
      }
   }
   
   for(slot=0; slot<16; slot++)
   {
      eratre(slot, &w0, &w1, MMUCR0_TLBSEL_IERAT);
      if((w1 & ERAT1_RPN(-1)) != ((PHYMAP_MINADDR_BEDRAM + PHYMAP_PRIVILEGEDOFFSET) & ~(1024*1024*1024-1)))
      {
         eratwe(slot, 0, 0, MMUCR0_TLBSEL_IERAT);
      }
      else
      {
         eratwe(slot, w0, w1, MMUCR0_TLBSEL_IERAT | MMUCR0_ECL(1));
      }
   }
      
   if(PERS_ENABLED(PERS_ENABLE_MMU))
   {
      for(address=0; address<128; address++)
      {
         for(slot=0; slot<4; slot++)
         {
            if((address != 127)||(slot!=0))  // avoid MMIO tlb entry
            {
               tlbwe_slot(slot, MAS1_V(0) | MAS1_TSIZE_4KB, MAS2_EPN(address), 0, 0, 0);
            }
         }
      }
   }
   
   isync();   
   mtspr(SPRN_TENS,((mask) & 0xf));
   isync();
   return 0;
}

int fw_resetPrepareBeDRAM()
{
   uint64_t x = DCRReadPriv(BEDRAM_DCR(BEDRAM_CONTROL));
   BEDRAM_DCR__BEDRAM_CONTROL__REFRESH_MAX_insert(x, 0xd);
   DCRWritePriv(BEDRAM_DCR(BEDRAM_CONTROL), x);
   return 0;
}

int fw_resetPrepareL1p()
{
   // Disable L1P prefetching
   out64_sync((void *)L1P_CFG_SPEC,0);
   out64_sync((void *)L1P_CFG_PF_USR,0);
   out64_sync((void *)L1P_CFG_PF_SYS,0);
   out64_sync((void *)L1P_CFG_WC,0);
   out64_sync((void *)L1P_CFG_TO,0);
   out64_sync((void *)L1P_CFG_UPC,0);
   
   return 0;
}

static void scramble_init(uint64_t slice_sel) {

    // TODO: current code assumes all slice_sel groups begin with '1'; change this.
    int m, n;
    int row_num[4] = {(slice_sel >> 60) & 0x0000000000000007, (slice_sel >> 56) & 0x0000000000000007,
                      (slice_sel >> 52) & 0x0000000000000007, (slice_sel >> 48) & 0x0000000000000007 };
    signed int scrambled[29];    // The bit order after scrambling
    int cur_scr_idx = 0;

    // The relevant slice select bits are packed into the left-most bits of slice_sel

    // First pack the columns
    for (n=0; n<4; n++) {
        int tmp_idx = scr_mat[n][row_num[n]];
        for (m=row_num[n]; m>0; m--) {
            scr_mat[n][m] = scr_mat[n][m-1];
        }
        scr_mat[n][0] = tmp_idx;
    }

    scrambled[cur_scr_idx] = scr_mat[3][7];
    cur_scr_idx++;
    for (m=6; m>=0; m--) {
        for (n=0; n<4; n++) {
            scrambled[cur_scr_idx] = scr_mat[n][m];
            cur_scr_idx++;
        }
    }

    cur_scr_idx = 0;

    // Generate the src->dest lists and keep_mask
    for (m=0; m<29; m++) {
        if (m != scrambled[m]) {
            scramble_src[cur_scr_idx]  = m;
            scramble_dest[cur_scr_idx] = scrambled[m];
            cur_scr_idx++;
            n_to_scramble++;
        } else {
            keep_mask = keep_mask | ((uint64_t)0x1 << (35 - m));
        }
    }
}

static inline char* scramble(uint64_t addr, uint64_t slice_sel) 
{
    uint64_t tmp_addr;
    uint64_t tmp_bit;
    int      m;

    // First mask the address to keep certain bits in their positions (this saves some time)
    tmp_addr = addr & keep_mask;

    // Next scramble the necessary bits as constructed in scramble_init()
    for (m=0; m<n_to_scramble; m++) {
        tmp_bit = ((uint64_t)0x1 & (addr >> (35 - scramble_src[m])));
        tmp_addr |= (tmp_bit << (35 - scramble_dest[m]));
    }

    return (char*)tmp_addr;

}

int fw_resetPrepareL2()
{
    int i;
    volatile char* block = (char*)0x4000000;
    uint64_t slice_sel_ctrl;

   if(PERS_ENABLED(PERS_ENABLE_DDR))  // If we don't have DDR, we cannot flush the L2.
   {
       slice_sel_ctrl = DCRReadPriv(TESTINT_DCR(SLICE_SEL_CTRL));
       scramble_init(0x8889000000000000);
       
      // adapted from hwverify/src/fullchip/l2cache/l2flush  -r19160
      // First set the threshold to 0 such that locked and speculative lines will be evicted
      // just as regular L2 lines. 
      for (i=0; i<16; i++) {
         uint64_t data64 = DCRReadPriv(L2_DCR(i, LRUCTRL1));
         data64 &= ~(L2_DCR__LRUCTRL1__SPEC_THRESH_set(0xf) | L2_DCR__LRUCTRL1__OVERLOCK_THRESH_set(0xf));
         DCRWritePriv(L2_DCR(i, LRUCTRL1), data64);
      }
      
      // Turn off scrubbing in case it is not already disabled
      for (i=0; i<16; i++) {
          uint64_t data64 = DCRReadPriv(L2_DCR(i, REFCTRL));
          data64 &= ~(L2_DCR__REFCTRL__SCB_RUN_set(0x1));
          DCRWritePriv(L2_DCR(i, REFCTRL), data64);
      }

      // Disable L2 prefetching
      for (i=0; i<16; i++) {
         uint64_t data64 = DCRReadPriv(L2_DCR(i, CTRL));
         data64 &= ~(L2_DCR__CTRL__ENABLE_PRFT_set(1));
         DCRWritePriv(L2_DCR(i, CTRL), data64);
      }
      
      // Invalidate L1D cache
      dci();
      ppc_msync();

      // Now load 64 of each 128 bytes in a contiguous 32MB range into a local variable, such that 
      // each line of the cache should be populated (i.e. the cache should be completely flushed).
      // For each set of the L2 in which we expect there to be valid lines at this point (e.g. set containing normal_line above),
      // we must write only 15 of the 16 ways in the set.  Then, we must write the LRUCTRL DCR so that the 
      // dd1 bug will demote way 0 to 0xf instead of promoting it to 0x0.  Then we can 
      volatile uint64_t tmp_line = 0;
      for (i=0; i<0x200000; i+=0x80) {
          tmp_line += *(scramble( ((uint64_t)block - 0x4000000) + i, slice_sel_ctrl ));
      }
      
      for (i=0x200000; i<0x2000000; i+=0x80) {
          tmp_line += *(scramble( (uint64_t)block + i, slice_sel_ctrl ));
      }
      
      // Set the demotion values to 15
      for (i=0; i<16; i++) {
          uint64_t data64 = DCRReadPriv(L2_DCR(i, LRUCTRL0));
          data64 &= ~(L2_DCR__LRUCTRL0__LRU1_set(0x3f) | L2_DCR__LRUCTRL0__LRU0_set(0x3f));
          data64 |= L2_DCR__LRUCTRL0__LRU1_set(0x1f) | L2_DCR__LRUCTRL0__LRU0_set(0x1f);
          DCRWritePriv(L2_DCR(i, LRUCTRL0), data64);
      }

      // demote way 0 to age 15 without actually loading a line into way 0
      for (i=0; i<0x200000; i+=0x80) {
          DCBLC_CT(scramble( (uint64_t)block + i, slice_sel_ctrl ), 2);
      }

      // Change the DCR values back
      for (i=0; i<16; i++) {
          uint64_t data64 = DCRReadPriv(L2_DCR(i, LRUCTRL0));
          data64 &= ~(L2_DCR__LRUCTRL0__LRU1_set(0x3f) | L2_DCR__LRUCTRL0__LRU0_set(0x3f));
          data64 |= L2_DCR__LRUCTRL0__LRU1_set(0x20) | L2_DCR__LRUCTRL0__LRU0_set(0x20);
          DCRWritePriv(L2_DCR(i, LRUCTRL0), data64);
      }

      // Read in the last addr for the set which contains normal_line (which should get evicted)
      // Read in the last addr for each set
      for (i=0; i<0x200000; i+=0x80) {
          tmp_line += *(scramble( (uint64_t)block + i, slice_sel_ctrl ));
      }
      
      ppc_msync();
   }
   return 0;
}

/*
void fw_iom_read()
{
   fw_IOM_REG_READ(0,_DDR_MC_IOM_PHYREGINDEX(0));// MC0(ARB0) Satellite id0
   fw_IOM_REG_READ(1,_DDR_MC_IOM_PHYREGINDEX(0) + 0x40);// MC0(ARB0) Satellite id1
   fw_IOM_REG_READ(2,_DDR_MC_IOM_PHYREGINDEX(0) + DELTA_ARB_B_A);// MC1(ARB1) Satellite id0
   fw_IOM_REG_READ(3,_DDR_MC_IOM_PHYREGINDEX(0) + DELTA_ARB_B_A + 0x40);// MC1(ARB1) Satellite id1
}

//--------------

void fw_iom_write()
{
   fw_IOM_REG_WRITE(0,_DDR_MC_IOM_PHYREGINDEX(0));// MC0(ARB0) Satellite id0
   fw_IOM_REG_WRITE(1,_DDR_MC_IOM_PHYREGINDEX(0) + 0x40);// MC0(ARB0) Satellite id1
   fw_IOM_REG_WRITE(2,_DDR_MC_IOM_PHYREGINDEX(0) + DELTA_ARB_B_A);// MC1(ARB1) Satellite id0
   fw_IOM_REG_WRITE(3,_DDR_MC_IOM_PHYREGINDEX(0) + DELTA_ARB_B_A + 0x40);// MC1(ARB1) Satellite id1
}
*/

void fw_ddr_shutdown()
{
//   int i;
//   uint64_t temp, temp2;
   
// Disable any on-going maintenance operation (i.e. background scrubbing)

   fw_DDR_SCOM_2MC(_DDR_MC_MCMCC(0), 0, 0, "Stop Scrubbing", 0x0, 870);

// Wait for 1000 cycles for all memory requests to be completed

//   fw_DDR_SCOM(0, 5, 0, "WAIT FOR 1000 cycles", 1000, 880);
   fw_DDR_SCOM(0, 5, 0, "WAIT FOR 100000 cycles", 100000, 880);

/*
// Wait to get MCMODE[2]='1' 10 consecutive times

   for(i=0;i<10;) {
      temp=DCRReadPriv(_DDR_MC_MCMODE(0));
      temp2=DCRReadPriv(_DDR_MC_MCMODE(0) + DELTA_ARB_B_A);
//printf("[891-%d] MCMODE = 0x%016lX (MC0)0x%016lX (MC1)\n",i,temp,temp2);
      if(((temp >> 61 & 0x1) || temp2 >> 61) & 0x1)
         i++;
      else
         i=0;
   }
   //printf("[891-%d] MCMODE = 0x%016lX (MC0)0x%016lX (MC1)\n",i,temp,temp2);
*/


// Disable Refresh

   fw_DDR_SCOM_2MC(_DDR_MC_MCZMRINT(0), 0, 0, "Disable Refresh", 0x0, 892);

// Enter Self-Refresh

   fw_DDR_SCOM_2MC(_DDR_MC_MCAPOS(0), 2, _BN(37), "Enter Self-Refresh",
                _BN(19) |// RAS low
                _BN(20) |// CAS low
                _B8(30,0xFF) |// CS  low
                _B4(34,0x0) | // CKE low
                _BN(35) | // RESET high
                _BN(35) |
                _BN(36) |
                _BN(37),894);

// TI_DDR_SR_VALUE_OVERRIDE = '1'

   fw_DDR_SCOM(TESTINT_DCR(CONFIG1)+0x1, 0, 0, "SET JTAG VALUE = SR mode", _BN(32), 896);// bit 96

// Wait for 100 cycles for TI_DDR_SR_ENABLE_OVERRID to take effect

   fw_DDR_SCOM(0, 5, 0, "WAIT FOR 100 cycles", 100, 897);

// TI_DDR_SR_ENABLE_OVERRIDE = '1'

   fw_DDR_SCOM(TESTINT_DCR(CONFIG1)+0x1, 0, 0, "ENABLE JTAG", _BN(33), 898);// bit 97
}

/*
void fw_mc_reinit()
{
   int i;

// MC initialization sequence for cycle reproducible run

   for(i=0; i<sizeof(ddr_seq1)/sizeof(ddr_seq1[0]);++i)
      fw_DDR_SCOM_2MC(ddr_seq1[i].addr, ddr_seq1[i].rw, ddr_seq1[i].pattern, ddr_seq1[i].title, ddr_seq1[i].value, 1000+i);
}
*/

/*
void fw_ddr_reinit()
{
   int i=100;

// DDR initialization sequence for cycle reproducible run
// Enter Self-Refresh

   fw_DDR_SCOM_2MC(_DDR_MC_MCAPOS(0), 0, 0, "Enter Self-Refresh",
                _BN(19) |// RAS low
                _BN(20) |// CAS low
                _B8(30,0xFF) |// CS  low
                _B4(34,0x0) | // CKE low
                _BN(35) | // RESET high
                _BN(35) |
                _BN(36) |
                _BN(37),1000+i++);

// TI_DDR_SR_ENABLE_OVERRIDE = '0'

   fw_DDR_SCOM(TESTINT_DCR(CONFIG1)+0x2, 0, 0, "DISABLE JTAG", _BN(33), 1000+i++);

// Wait for 100 cycles for TI_DDR_SR_ENABLE_OVERRID to take effect

   fw_DDR_SCOM(0, 5, 0, "WAIT FOR 100 cycles", 100, 1000+i++);

// TI_DDR_SR_VALUE_OVERRIDE = '0'

   fw_DDR_SCOM(TESTINT_DCR(CONFIG1)+0x2, 0, 0, "SET JTAG VALUE = Normal mode", _BN(32), 1000+i++);

// Exit Self-Refresh

   fw_DDR_SCOM_2MC(_DDR_MC_MCAPOS(0), 0, 0, "Exit Self-Refresh",
                _B4(34,0xF) | // CKE low
                _BN(35) | // RESET high
                _BN(35) |
                _BN(36) |
                _BN(37),1000+i++);

// Wait for 1000 cycles for tRFC

   fw_DDR_SCOM(0, 5, 0, "WAIT FOR 100 cycles", 100, 1000+i++);

// Issue a Sigle Refresh

   fw_DDR_SCOM_2MC(_DDR_MC_MCAPOS(0), 0, 0, "Issue a Single Refresh",
                _BN(19) |       // RAS low
                _BN(20) |       // CAS low
                _B8(30,0xFF) |  // CS  low
                _B4(34,0xF) |
                _BN(35) |
                _BN(36) |
                _BN(37),1000+i++);

// Enable Refresh

   fw_DDR_SCOM_2MC(_DDR_MC_MCAPOS(0), 0, 0, "Enable Refresh",
                _BN(32) | 
                _B10(43,20) | 
                _B7(50,26),1000+i++);

// Wait for 1000 cycles for tRFC

   fw_DDR_SCOM(0, 5, 0, "WAIT FOR 100 cycles", 100, 1000+i++);

// Issue ZQCL

   fw_DDR_SCOM_2MC(_DDR_MC_MCAPOS(0), 0, 0, "Issue ZQCL",
                _BN(10) | // A10 set
                _BN(21) | // WE active
                _B8(30,0xFF) |
                _B4(34,0xF) |
                _BN(35) |
                _BN(36) |
                _BN(37),1000+i++);

}
*/

int fw_resetPrepareMC()
{
   uint64_t mask = ~(1<<(ProcessorThreadID()));
   mtspr(SPRN_TENC,((mask) & 0xf));
   
//   fw_iom_read();
   fw_ddr_shutdown();
   
   mtspr(SPRN_TENS,((mask) & 0xf));
   isync();
   
   return 0;
}

int fw_resetResumeMC()
{
   uint64_t mask = ~(1<<(ProcessorThreadID()));
   mtspr(SPRN_TENC,((mask) & 0xf));
   
//   fw_mc_reinit();
//   fw_iom_write();

   struct DDRINIT_metrics ddr = { 0, };
   int DDR_ResumeSequence();
   DDR_ResumeSequence(ddr);

//   fw_ddr_reinit();

   mtspr(SPRN_TENS,((mask) & 0xf));
   isync();
   
   return 0;
}

extern void _fw_resetConfig();
extern void _fw_resetConfig_end();

int fw_resetSetupICache()
{
   int slot;
   uint64_t w0, w1;
   uint64_t offset = 0;
   for(slot=0; slot<16; slot++)
   {
      eratre(slot, &w0, &w1, MMUCR0_TLBSEL_IERAT);
      if((w1 & ERAT1_RPN(-1)) != ((PHYMAP_MINADDR_BEDRAM + PHYMAP_PRIVILEGEDOFFSET) & ~(1024*1024*1024-1)))
      {
         eratwe(slot, ERAT0_EPN(PHYMAP_MINADDR_BEDRAM | VA_PRIVILEGEDOFFSET | 0x4000000000000000ULL | offset) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_64K | ERAT0_ThdID(0xF),
                ERAT1_RPN(PHYMAP_MINADDR_BEDRAM | PHYMAP_PRIVILEGEDOFFSET | offset) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_G | ERAT1_U1 | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR,
                MMUCR0_TLBSEL_IERAT | MMUCR0_ECL(1));
         offset += 64*1024;
         if(offset >= 256*1024)
            break;
      }
   }
   
   uint64_t address = (uint64_t)_fw_resetConfig;
   address &= ~(L1I_CACHE_LINE_SIZE-1);
   for(; address < (uint64_t)_fw_resetConfig_end + L1I_CACHE_LINE_SIZE; address += L1I_CACHE_LINE_SIZE)
   {
      icache_block_touch((void*) (address | 0x4000000000000000ULL));
   }
   
   return 0;
}


// \todo resetBarrier should ideally be a full-fledge barrier.  The following assumes hwthread0 will be last.
#define resetBarrier(n, step, maxstep) { if(ProcessorID() == 0) { BeDRAM_ReadIncSat(n); } else { while(BeDRAM_Read(n)%maxstep == step) { fw_pdelay(1600); } } }

int fw_doCycleReproReset( void )  
{   
   uint64_t config = FW_Personality.Kernel_Config.NodeConfig;

   ThreadPriority_Low();   
   resetBarrier(BeDRAM_LOCKNUM_REPROBARRIER, 0, 6);
   
   if(ProcessorThreadID() == 0)
      ThreadPriority_Medium();
   
   if(ProcessorID() == 0)
   {
       fw_resetQuiesceNetwork();
       fw_resetSyncMultiNode();
   }
   
   resetBarrier(BeDRAM_LOCKNUM_REPROBARRIER, 1, 6);
   if(ProcessorThreadID() == 0)
   {
      fw_resetPrepareBeDRAM();
      fw_resetPrepareL1p();
   }
   
   resetBarrier(BeDRAM_LOCKNUM_REPROBARRIER, 2, 6);
   if(ProcessorID() == 0)
   {
      fw_resetPrepareL2();
   }
   
   resetBarrier(BeDRAM_LOCKNUM_REPROBARRIER, 3, 6);

   if(ProcessorThreadID() == 0)
   {
      fw_resetInvalidateTLBs();
      ppc_msync();
   }
   
   resetBarrier(BeDRAM_LOCKNUM_REPROBARRIER, 4, 6);
   if(ProcessorID() == 0)
   {
      if(_PERS_ENABLED(PERS_ENABLE_DDR, config))
      {
         fw_resetPrepareMC();
      }
   }
   
   mtmsr( mfmsr() & ~(MSR_EE | MSR_CE | MSR_ME) );
   isync();
   resetBarrier(BeDRAM_LOCKNUM_REPROBARRIER, 5, 6);
   
   if(ProcessorID() == 0)
   {
      while(mfspr(SPRN_TENSR) != TEN_TEN0) // wait for other threads on this core to disable themselves.
      {
      }
      fw_mailbox_waitForQuiesced();
      fw_resetSetupICache();
      uint64_t config1 = _BN(113-64);
      if(_PERS_ENABLED(PERS_ENABLE_DDR, config) == 0)
      {
          config1 |= _BN(114-64) | _BN(96-64);
          DCRWritePriv(TESTINT_DCR(CONFIG1), DCRReadPriv(TESTINT_DCR(CONFIG1)) | config1);  // Avoid BeDRAM clear upon reset.
          config1 |= _BN(97-64);
      }
      DCRWritePriv(TESTINT_DCR(CONFIG1), DCRReadPriv(TESTINT_DCR(CONFIG1)) | config1);  // Avoid BeDRAM clear upon reset.
      config1 |= _BN(79-64);
      
      asm volatile ("lis 3, 0x4000;"
                    "rldicr 3,3,32,31;"
                    "or %0, %0, 3;"
                    "mtctr %0;" 
                    "mr 3, %1;"
                    "bctrl;"
                    : : "r" (_fw_resetConfig), "r" (config1) : "r3", "memory");
      
      // we should not get here!!!!!
      while(1)
      {
      }
   }
   
   // Place all hardware threads via A2 sleep.  Main thread will shutoff threads via config register
   mtspr(SPRN_TENC, (1 << ProcessorThreadID()));
   isync();
   while(1)
   {
   }
   return( 0 );
}
