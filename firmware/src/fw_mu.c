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


/* ---------------------------------------------------------
This a a library to release the network from reset correctly
and to do basic network termination checks at the end of a run
The initial version supports single node,(with all coordinates 0)
in internal loopback, which is sufficient for early full chip tests
Eventually, a somewhat modified version should evolve into the firmware.

-----------------------------------------------------*/

 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/mu_dcr.h>
#include <hwi/include/bqc/MU_Macros.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/classroute.h>
#include "Firmware_internals.h"
#include "Firmware_RAS.h"

// by default doesn't compile code just for xstate simulation
// #define FW_MU_XSTATE_SIM

//!< @todo fix DD2 bits by picking up new header

//#define MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_DD2_ERR0_set(x) _BGQ_SET(2,63,x)
//#define MU_DCR__MCSRAM_INTERRUPTS_CONTROL_LOW__MCSRAM_DD2_ERR1_set(x) _BGQ_SET(2,1,x)


#include "_fw_mu.h"

int FW_MU_VERBOSE=0;
int fw_mu_ismambo=0;


// Personality access function. Currently returns constant
// TODO : Change it when personality becomes available
#define FW_MU_GET_WAIT_FULL(P) (100)
#define FW_MU_GET_WAIT_TAIL(P) (10)
// For diagnostics, 1 CE would raise over threshold. Otherwise, 10 CEs to raise over threshold.
#define FW_MU_ECC_COUNT_THOLD(P) (((P)->Kernel_Config.NodeConfig & PERS_ENABLE_DiagnosticsMode) ? 0 : 9)


#define FW_MU_INIT_ONLY_GROUP0

// interrupt mode indicating that all the int bits are critical
#define FW_MU_IMODE_ALL_CRIT 0xaaaaaaaaaaaaaaaaUL

// interrupt mode indicating that all the int bits are non-critical
#define FW_MU_IMODE_ALL_NOCRIT 0xffffffffffffffffUL

// interrupt mode indicating that all the int bits are machine-check
#define FW_MU_IMODE_ALL_MCHECK 0x5555555555555555UL

// Makes a mask to cover interrupt mode fields from FROM_BIT to TO_BIT.
#define FW_MU_MK_IMASK(FROM_BIT, TO_BIT) ((FROM_BIT(2)<<1)-(TO_BIT(1)))

// Makes interrupt mode value setting from FROM_BIT to TO_BIT as critical
#define FW_MU_IMODE_CRIT(FROM_BIT, TO_BIT) (FW_MU_IMODE_ALL_CRIT & FW_MU_MK_IMASK(FROM_BIT, TO_BIT))

// Makes interrupt mode value setting from FROM_BIT to TO_BIT as non-critical
#define FW_MU_IMODE_NOCRIT(FROM_BIT, TO_BIT) (FW_MU_IMODE_ALL_NOCRIT & FW_MU_MK_IMASK(FROM_BIT, TO_BIT))

// Makes interrupt mode value setting from FROM_BIT to TO_BIT as machine-check
#define FW_MU_IMODE_MCHECK(FROM_BIT, TO_BIT) (FW_MU_IMODE_ALL_MCHECK & FW_MU_MK_IMASK(FROM_BIT, TO_BIT))


// address-data pair to write to DCR interrupt mode registers
// specifies whether each interrupt bit is critical or non-critical

// struct to enumerate interrupt enable DCR transactions
typedef struct {
  uint64_t addr;
  uint64_t intmode; 
} fw_mu_dcr_intmode_t;



const fw_mu_dcr_intmode_t fw_mu_dcr_intmode_data[] = {

  // DCR internal interrupt 
  { MU_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH),
    FW_MU_IMODE_NOCRIT(MU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set, // address/priv error => no crit
		       MU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set) |
    FW_MU_IMODE_MCHECK(MU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set, // addr/data par error => mcheck
		       MU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set)
  },
  
  { MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),   MU_MASTER_PORT0_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),   MU_MASTER_PORT1_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),   MU_MASTER_PORT2_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),     MU_SLAVE_PORT_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),         MU_MMREGS_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(FIFO_INTERRUPTS_CONTROL_LOW),            MU_FIFO_INTERRUPTS_CONTROL_LOW },
  { MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),        MU_IMU_ECC_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_LOW),         MU_IMU_ECC_INTERRUPTS_CONTROL_LOW  },
  { MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),         MU_MCSRAM_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(MCSRAM_INTERRUPTS_CONTROL_LOW),          MU_MCSRAM_INTERRUPTS_CONTROL_LOW },
  { MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),          MU_ARLOG_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),        MU_RMU_ECC_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),         MU_RMU_ECC_INTERRUPTS_CONTROL_LOW  },
  { MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),         MU_RCSRAM_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),       MU_RPUTSRAM_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),            MU_IME_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(IME_INTERRUPTS_CONTROL_LOW),             MU_IME_INTERRUPTS_CONTROL_LOW },
  { MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),           MU_RME_INTERRUPTS0_CONTROL_HIGH },
  { MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),            MU_RME_INTERRUPTS0_CONTROL_LOW },
  { MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),           MU_RME_INTERRUPTS1_CONTROL_HIGH },
  { MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),            MU_RME_INTERRUPTS1_CONTROL_LOW },
  { MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),         MU_ICSRAM_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),           MU_MISC_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),   MU_RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),    MU_RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW },
  { MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH),  MU_MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH },
  { MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),      MU_ECC_COUNT_INTERRUPTS_CONTROL_HIGH },
};


// Forware declarations of sub functions to initialize MU
extern int  fw_mu_reset_all_subunits(Personality_t *); // release reset of DCR unit
extern int  fw_mu_release_dcr_units(Personality_t *); // release reset of DCR unit
extern int  fw_mu_init_dcr_regs(Personality_t *);     // set up non-default DCR 
extern int  fw_mu_clear_srams(Personality_t *);       // clear all SRAM entries
extern int  fw_mu_clear_icsram(Personality_t *);      // clears ICSRAM
extern int  fw_mu_clear_rcsram(Personality_t *);      // clears RCSRAM
extern int  fw_mu_clear_rputsram(Personality_t *);    // clears RPUTSRAM
extern int  fw_mu_clear_dcr_interrupts(Personality_t *);  // clear DCR interrupts
extern int  fw_mu_enable_dcr_interrupts(Personality_t *); // setup DCR interrupt modes
extern int  fw_mu_check_ecc_count(Personality_t *);   // check that ECC count is 0
extern int  fw_mu_release_all_resets(Personality_t *); // release reset of all submodules
extern int  fw_mu_enable_me(Personality_t *);          // enables all iMEs/rMEs
extern int  fw_mu_clear_mmio_interrupts(Personality_t *);  // clear MMIO interrupts
extern int  fw_mu_setup_gea(Personality_t *p);        // setup GEA interrupt mapping for MU

  
//*******************************************************************************
// MMIO
//*******************************************************************************
// MMIO write with optional verbose print
extern inline void fw_mu_mmio_write(uint64_t address, uint64_t value)
{
  volatile uint64_t *a = (uint64_t*)address;
  *a = value;
  // if (FW_MU_VERBOSE && fw_mu_ismambo) printf ("WRITE: mmio_area[%lx] = %lx\n", address, *a);
}

// MMIO read with optional verbose print
extern inline uint64_t fw_mu_mmio_read(uint64_t address)
{
  volatile uint64_t *a = (uint64_t*)address;
  // if (FW_MU_VERBOSE && fw_mu_ismambo) printf ("READ : mmio_area[%lx] = %lx\n", address, *a);
  return *a;
}

  
/********************************************************************************
 * fw_mu_reset_release : Main function to initialize MU (executed by core 0)
 ********************************************************************************/

int fw_mu_resetRelease( void )  
{

  TRACE_ENTRY(TRACE_MU);

  int rc = 0;
  Personality_t* p = FW_PERSONALITY_PTR();
  fw_mu_ismambo = ((p->Kernel_Config.NodeConfig & PERS_ENABLE_Mambo) != 0);

  if ( TRACE_ENABLED(TRACE_MU) ) FW_MU_VERBOSE = 1; // TEM - temporary hack

  if (rc == 0) rc = fw_mu_reset_all_subunits(p);   // reset every subunit, including DCR
  if (rc == 0) rc = fw_mu_release_dcr_units(p);  // release reset of DCR unit 
  if (rc == 0) rc = fw_mu_init_dcr_regs(p);      // set non-default DCR registers
  if (rc == 0) rc = fw_mu_clear_srams(p);        // clear ICSRAM, RCSRAM, and RPUT SRAM
  if (rc == 0) rc = fw_mu_clear_dcr_interrupts(p);        // clear DCR interrupts
  if (rc == 0) rc = fw_mu_enable_dcr_interrupts(p);       // enable DCR interrupts
  if (rc == 0) rc = fw_mu_release_all_resets(p);       // release submodule reset
  if (rc == 0) rc = fw_mu_clear_mmio_interrupts(p);         // clear all interrupts
  if (rc == 0) rc = fw_mu_setup_gea(p);                // setup GEA interrupt mapping
  // ECC count register is not supported by mambo yet -> don't check in mambo
  // check that ECC count is certainly 0
  if ((!fw_mu_ismambo) && (rc == 0)) rc = fw_mu_check_ecc_count(p); 

  if (rc == 0) rc = fw_mu_enable_me(p);                // enables iME/rME

  TRACE_EXIT(TRACE_MU);

  return rc;

}

/********************************************************************************
 * fw_mu_release_dcr_units : Releases reset of DCR unit
 ********************************************************************************/
int fw_mu_reset_all_subunits(Personality_t *p)
  
{
  uint64_t reset_dcr;

  // set every bits
  reset_dcr = ~0UL;
  fw_tracedDcrWrite( TRACE_MU, MU_DCR(RESET), reset_dcr );
  
  return 0;
}
/********************************************************************************
 * fw_mu_release_dcr_units : Releases reset of DCR unit
 ********************************************************************************/
int fw_mu_release_dcr_units(Personality_t *p)
  
{
  uint64_t reset_dcr;

  // lower the DCR register resets 
  reset_dcr = ~ MU_DCR__RESET__DCRS_OUT_set(1);
  fw_tracedDcrWrite( TRACE_MU, MU_DCR(RESET), reset_dcr );
  
  return 0;

}
 
/********************************************************************************
 * fw_mu_init_dcr_regs : initializes non-default DCR registers
 ********************************************************************************/
int fw_mu_init_dcr_regs(Personality_t *p)
{
  // Enable ARLOG fifo map scan & correct
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(ARLOG_SCAN_ENABLE), MU_DCR__ARLOG_SCAN_ENABLE__VALUE_set(1));

  // barrier class = ALL SYSTEM, because hardware default is 0 (user).
  //  will be removed when hardware default is changed to 1.
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(SYS_BARRIER), MU_DCR__SYS_BARRIER__VALUE_set(0xffff));

  // rME wait count for full rmFIFO
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(RME_WAIT_FULL), MU_DCR__RME_WAIT_TAIL__VALUE_set(FW_MU_GET_WAIT_FULL(p)));

  // rME wait count for commit tail
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(RME_WAIT_TAIL), MU_DCR__RME_WAIT_TAIL__VALUE_set(FW_MU_GET_WAIT_TAIL(p)));

  // ECC count interrupt threshold
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(ECC_INT_THRESH), MU_DCR__ECC_INT_THRESH__VALUE_set(FW_MU_ECC_COUNT_THOLD(p)));
			       
  return 0;
}


/********************************************************************************
 * fw_mu_clear_srams : Zero clear ICSRAM, RCSRAM, and RPUT SRAM
 ********************************************************************************/
 
int fw_mu_clear_srams(Personality_t *p)
{

  int rc = 0;
  // Release reset for slave, RCSRAM, ICSRAM, and RPUT SRAM
  {
    uint64_t reset = ~0UL;
    reset &= ~MU_DCR__RESET__DCRS_OUT_set(1);
    reset &= ~MU_DCR__RESET__XS_set(1);
    reset &= ~MU_DCR__RESET__ICSRAM_set(1);
    reset &= ~MU_DCR__RESET__RCSRAM_set(1);
    reset &= ~MU_DCR__RESET__RPUTSRAM_set(1);
    fw_tracedDcrWrite(TRACE_MU, MU_DCR(RESET), reset);
  }

  FW_TRACE( TRACE_MU, ( "(D) %s:%d\n", __func__, __LINE__ ) );

  // deassert halt enable for RCSRAM and ICSRAM to prevent them from halting by initial ECC error
  {
    uint64_t halt_val;
    halt_val = ~MU_DCR__MOD_HALT_EN__ICSRAM_set(1) & ~MU_DCR__MOD_HALT_EN__RCSRAM_set(1);
    fw_tracedDcrWrite(TRACE_MU, MU_DCR(MOD_HALT_EN), halt_val);
  }

  // zero clear ICSRAM
  if (rc == 0) rc = fw_mu_clear_icsram(p);

  // zero clear RCSRAM
  if (rc == 0) rc = fw_mu_clear_rcsram(p);

  // zero clear RPUT SRAM
  if (rc == 0) rc = fw_mu_clear_rputsram(p);

 
  // assert halt enable gain for RCSRAM and ICSRAM
  {
    uint64_t halt_val = ~0UL;
    fw_tracedDcrWrite(TRACE_MU, MU_DCR(MOD_HALT_EN), halt_val);
  }

  return rc;
}

#if 0
/*===============================================================================
 * for debug 
 *==============================================================================*/
static void fw_mu_mmio_read_and_write(uint64_t addr, uint64_t wval)
{
  fw_mu_mmio_write(addr, wval);
  fw_mu_mmio_read(addr);
}

/*===============================================================================
 * for debug 
 *==============================================================================*/
static void fw_mu_mmio_debug(Personality_t *p)
{

  uint64_t wval = 0xcafebabedeadbeef;

  // ICSRAM 5 fields
  fw_mu_mmio_read_and_write(BGQ_MU_iDMA_START_OFFSET(0, 0, 0), wval);
  fw_mu_mmio_read_and_write(BGQ_MU_iDMA_SIZE_OFFSET(0, 0, 0), wval);
  fw_mu_mmio_read_and_write(BGQ_MU_iDMA_HEAD_OFFSET(0, 0, 0), wval);
  fw_mu_mmio_read_and_write(BGQ_MU_iDMA_TAIL_OFFSET(0, 0, 0), wval);
  fw_mu_mmio_read_and_write(BGQ_MU_iDMA_DESC_COUNTER_OFFSET(0, 0, 0), wval);

  // RCSRAM 4 fields
  fw_mu_mmio_read_and_write(BGQ_MU_rDMA_START_OFFSET(0, 0, 0), wval); // start field
  fw_mu_mmio_read_and_write(BGQ_MU_rDMA_SIZE_OFFSET(0, 0, 0), wval); // size  field
  fw_mu_mmio_read_and_write(BGQ_MU_rDMA_HEAD_OFFSET(0, 0, 0), wval); // head  field
  fw_mu_mmio_read_and_write(BGQ_MU_rDMA_TAIL_OFFSET(0, 0, 0), wval); // tail  field

  //RPUTSRAM
  fw_mu_mmio_read_and_write(BGQ_MU_DATA_COUNTER_BASE_ADDRESS_OFFSET(0, 0, 0), wval);
  
}
#endif

/********************************************************************************
 * fw_mu_check_mmio : Write and read MMIO register to check function
 ********************************************************************************/
int fw_mu_check_mmio(uint64_t test_addr, uint64_t magic, uint64_t restore)
{

  fw_mu_mmio_write(test_addr, magic);
  // if read value does not match the write value, return 1
  if (fw_mu_mmio_read(test_addr) != magic){
    return 5;
  }
  fw_mu_mmio_write(test_addr, restore);
  return 0;
}

/********************************************************************************
 * fw_mu_clear_icsram : Zero clear all ICSRAM fields
 ********************************************************************************/
int fw_mu_clear_icsram(Personality_t *p)
{
  uint64_t gid, sgid, i;


  //fw_mu_mmio_debug(p);
  for(gid = 0; gid < BGQ_MU_NUM_INJ_FIFO_GROUPS; gid ++){  // loop on group

    //FW_TRACE( TRACE_MU, ( "(D) %s  enabled %d -> %d\n", __func__, (int)gid, (int)MU_IS_GROUP_ENABLED(gid,p->Network_Config.MuFlags) ) );

    if ( MU_IS_GROUP_ENABLED(gid,p->Network_Config.MuFlags) == 0) // skip this group if not specified in bit map
      continue;

    for(sgid = 0; sgid < BGQ_MU_NUM_INJ_FIFO_SUBGROUPS; sgid ++){ // loop on subgroup

      for(i = 0; i < BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP; i ++){ // loop on FIFOs in each subgroup
	FW_TRACE( TRACE_MU, ( "initializing imFIFO(%ld, %ld, %ld)\n", gid, sgid, i ) );

	fw_mu_mmio_write(BGQ_MU_iDMA_START_OFFSET(gid, sgid, i), 0); // start field
	fw_mu_mmio_write(BGQ_MU_iDMA_SIZE_OFFSET(gid, sgid, i), 0x3fUL); // size  field
	fw_mu_mmio_write(BGQ_MU_iDMA_HEAD_OFFSET(gid, sgid, i), 0); // head  field
	fw_mu_mmio_write(BGQ_MU_iDMA_TAIL_OFFSET(gid, sgid, i), 0); // tail  field
	fw_mu_mmio_write(BGQ_MU_iDMA_DESC_COUNTER_OFFSET(gid, sgid, i),  0); // desc counter field


      }
    }
  }
  // To test MMIO function, write a magic number to a location, then read back.
  {
    int rc = 0;
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_iDMA_START_OFFSET(0, 0, 0), 0x03cafeba80ull, 0);
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_iDMA_SIZE_OFFSET(0, 0, 0), 0x03cafebabfull, 0x3FUL);
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_iDMA_HEAD_OFFSET(0, 0, 0), 0x03cafeba80ull, 0);
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_iDMA_TAIL_OFFSET(0, 0, 0), 0x03cafeba80ull, 0);
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_iDMA_DESC_COUNTER_OFFSET(0, 0, 0), 0xdeadbeefcafebabeull, 0);

    return rc;
  }

}

/********************************************************************************
 * fw_mu_clear_rcsram : Zero clear all RCSRAM fields
 ********************************************************************************/
int fw_mu_clear_rcsram(Personality_t *p)
{
  uint64_t gid, sgid, i;

  for(gid = 0; gid < BGQ_MU_NUM_REC_FIFO_GROUPS; gid ++){  // loop on group

    if ( MU_IS_GROUP_ENABLED(gid,p->Network_Config.MuFlags) == 0) // skip this group if not specified in the bit map
      continue;

    for(sgid = 0; sgid < BGQ_MU_NUM_REC_FIFO_SUBGROUPS; sgid ++){ // loop on subgroup

      for(i = 0; i < BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP; i ++){ // loop on FIFOs in each subgroup
	FW_TRACE( TRACE_MU, ("initializing rmFIFO(%ld, %ld, %ld)\n", gid, sgid, i) );	
	fw_mu_mmio_write(BGQ_MU_rDMA_START_OFFSET(gid, sgid, i), 0); // start field
	fw_mu_mmio_write(BGQ_MU_rDMA_SIZE_OFFSET(gid, sgid, i), 0x1FUL); // size  field
	fw_mu_mmio_write(BGQ_MU_rDMA_HEAD_OFFSET(gid, sgid, i), 0); // head  field
	fw_mu_mmio_write(BGQ_MU_rDMA_TAIL_OFFSET(gid, sgid, i), 0); // tail  field


      }
    }
  }
  // To test MMIO function, write a magic number to a location, then read back.
  {
    int rc = 0;
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_rDMA_START_OFFSET(0, 0, 0), 0x03cafebaa0ull, 0);
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_rDMA_SIZE_OFFSET(0, 0, 0), 0x03cafebabfull, 0x1FUL);
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_rDMA_HEAD_OFFSET(0, 0, 0), 0x03cafebaa0ull, 0);
    if (rc == 0) rc = fw_mu_check_mmio(BGQ_MU_rDMA_TAIL_OFFSET(0, 0, 0), 0x03cafebaa0ull, 0);
    return rc;
  }
}


/********************************************************************************
 * fw_mu_clear_rputsram : Zero clear all RPUTSRAM fields
 ********************************************************************************/
int fw_mu_clear_rputsram(Personality_t *p)
{
  uint64_t gid, sgid, i;

  for(gid = 0; gid < BGQ_MU_NUM_FIFO_GROUPS; gid ++){  // loop on group

    if ( MU_IS_GROUP_ENABLED(gid,p->Network_Config.MuFlags) == 0) // skip this group if not specified in bit map
      continue;

    for(sgid = 0; sgid < BGQ_MU_NUM_FIFO_SUBGROUPS; sgid ++){ // loop on subgroup

      for(i = 0; i < BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP; i ++){ // loop on bases in each subgroup
	FW_TRACE( TRACE_MU, ("initializing RPUT_SRAM(%ld, %ld, %ld)\n", gid, sgid, i) );
	
	fw_mu_mmio_write(BGQ_MU_DATA_COUNTER_BASE_ADDRESS_OFFSET(gid, sgid, i), 0x1fffffffffUL); // base field


      }
    }
  }
  // To test MMIO function, write a magic number to a location, then read back.
  {
    return fw_mu_check_mmio(BGQ_MU_DATA_COUNTER_BASE_ADDRESS_OFFSET(BGQ_MU_NUM_FIFO_GROUPS-1,
							     BGQ_MU_NUM_FIFO_SUBGROUPS-1,
							     BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP-1),
			    0x13cafebabeull, 0x1fffffffffUL);

  }

}

// List of interrupt registers to clear after SRAM initialization
static const uint64_t int_to_clear[] = {
  MU_DCR(FIFO_INTERRUPTS__STATE),
  MU_DCR(IMU_ECC_INTERRUPTS__STATE),
  MU_DCR(RMU_ECC_INTERRUPTS__STATE),
  MU_DCR(ICSRAM_INTERRUPTS__STATE),
  MU_DCR(RMU_ECC_CORR_INTERRUPTS__STATE),
  MU_DCR(RCSRAM_INTERRUPTS__STATE),
};

/********************************************************************************
 * fw_mu_clear_dcr_interrupts : Clear DCR interrupts which may be set by initial SRAM ECC error
 ********************************************************************************/
int fw_mu_clear_dcr_interrupts(Personality_t *p)
{
  unsigned u;
  for(u = 0; u < sizeof(int_to_clear)/sizeof(int_to_clear[0]); u++){
    fw_tracedDcrWrite(TRACE_MU, int_to_clear[u], ~0UL);
  }
  return 0;
}

/********************************************************************************
 * fw_mu_enable_dcr_interrupts : Enable DCR interrupts
 ********************************************************************************/
int fw_mu_enable_dcr_interrupts(Personality_t *p)
{

  unsigned u;
  for(u = 0; u < sizeof(fw_mu_dcr_intmode_data)/sizeof(fw_mu_dcr_intmode_data[0]); u++){
    // refer to the constant array to get address-data pair.
    fw_tracedDcrWrite(TRACE_MU, fw_mu_dcr_intmode_data[u].addr, fw_mu_dcr_intmode_data[u].intmode);
  }
  return 0;
}

/********************************************************************************
 * fw_mu_release_all_resets : deasserts all submodule resets
 ********************************************************************************/
int fw_mu_release_all_resets(Personality_t *p)
{
  // clear all bits
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(RESET), 0);
  return 0;
}

/********************************************************************************
 * fw_mu_enable_me : enables all rMEs/iMEs
 ********************************************************************************/
int fw_mu_enable_me(Personality_t *p)
{
  // enable all iMEs
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(IME_ENABLE),  MU_DCR__IME_ENABLE__VALUE_set(0xffff));
  // enable all rMEs
  fw_tracedDcrWrite(TRACE_MU, MU_DCR(RME_ENABLE),  MU_DCR__RME_ENABLE__VALUE_set(0xffff));
  return 0;
}

/********************************************************************************
 * fw_mu_check_ecc_count : sample some ECC check counter and check that they are 0
 ********************************************************************************/
int  fw_mu_check_ecc_count(Personality_t *p)
{
  uint64_t dcr_val_out;

  // check that ICSRAM ecc count is 0
  dcr_val_out = DCRReadPriv(MU_DCR(ICSRAM_ECC_COUNT));
  if (dcr_val_out != 0) return 10;

  // check that RME ecc count is 0
  dcr_val_out = DCRReadPriv(MU_DCR(RME_ECC_COUNT));
  if (dcr_val_out != 0) return 11;

  return 0;
}

/********************************************************************************
 * fw_mu_clear_mmio_interrupts : clear SRAM-related interrupts that may be fired during initialization
 ********************************************************************************/
int  fw_mu_clear_mmio_interrupts(Personality_t *p)
{
  uint64_t gid;

 
#ifdef FW_MU_INIT_ONLY_GROUP0
  for(gid = 0; gid < 1; gid ++) /*loop doesn't iterate*/   // loop on group
#else
  for(gid = 0; gid < BGQ_MU_NUM_INJ_FIFO_GROUPS; gid ++)  // loop on group
#endif
    {
      fw_mu_mmio_write(BGQ_MU_CLEAR_INT_STATUS_REGISTER_OFFSET(gid, 0), ~0ULL); // clear interrupts
    }
  
  return 0;
}

/********************************************************************************
 * fw_mu_setup_gea : setup GEA lane mapping
 ********************************************************************************/
// mapping : PUEA listens only lane 0 by default (i.e. catches machine check errors)
// 
// lane 0 : MU int 7 : machine check (ECC UE, parity error, logic error, too many ECC) 
// lane 1 : MU int 6 : non-critical interrupts (software errors, ECC CE > 0)
// lane 2 : MU int 5 : critical interrupts (im/rmFIFO full)
// lane 3-7 : MU int 0-4 : real time interrupts from MU (threshold crossed, pkt arrival, GI barrier)

int fw_mu_setup_gea(Personality_t *p)
{
  uint64_t u;
  int rc = 0;

#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { MU_GEA_MASK_0, MU_GEA_MASK_1, MU_GEA_MASK_2 };
  fw_installGeaHandler( fw_mu_machineCheckHandler, mask );
  
#endif

  // MAP1 reg (MU int 6) : non-critical
  u = DCRReadPriv(GEA_DCR(GEA_INTERRUPT_MAP1)); 
  GEA_DCR__GEA_INTERRUPT_MAP1__MU_MAP6_insert(u, 1); // MU interrupt #6 -> lane 1
  fw_tracedDcrWrite(TRACE_MU, GEA_DCR(GEA_INTERRUPT_MAP1), u);

  // MAP3 reg (MU int 7) : machine check
  u = DCRReadPriv(GEA_DCR(GEA_INTERRUPT_MAP3));
  GEA_DCR__GEA_INTERRUPT_MAP3__MU_MAP7_insert(u, 0); // MU interrupt #7 -> machine check -> lane 0
  fw_tracedDcrWrite(TRACE_MU, GEA_DCR(GEA_INTERRUPT_MAP3), u);

  // MAP8 reg (MU int 0,1,2,3,4) : real time interrupts
  u = DCRReadPriv(GEA_DCR(GEA_INTERRUPT_MAP8));
  GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP0_insert(u, 3);  // MU int #0 -> lane 3
  GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP1_insert(u, 4);  // MU int #1 -> lane 4
  GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP2_insert(u, 5);  // MU int #2 -> lane 5
  GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP3_insert(u, 6);  // MU int #3 -> lane 6
  GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP4_insert(u, 7);  // MU int #4 -> lane 7
  fw_tracedDcrWrite(TRACE_MU, GEA_DCR(GEA_INTERRUPT_MAP8), u);

  // MAP 9 reg (MU int 5) : critical interrupts
  u = DCRReadPriv(GEA_DCR(GEA_INTERRUPT_MAP9));
  GEA_DCR__GEA_INTERRUPT_MAP9__MU_MAP5_insert(u, 2); // MU int #5 -> lane 2
  fw_tracedDcrWrite(TRACE_MU, GEA_DCR(GEA_INTERRUPT_MAP9), u);

  return rc;

}






/********************************************************************************
 * fw_mu_wrap_rcp_fifos : work around for init X issues
 ********************************************************************************/
// Wraps ND HP,IO, COLL_0, and COLL_1 rcp FIFOs to avoid X-es from propagating
// to crossbar (BQC issue 863). 

// compile only when this macro is set
#ifdef FW_MU_XSTATE_SIM

#define FW_MU_WRAP_RCP_FIFOS_MKDESC(NDHDR, MLEN, FMAP,HINT,VC)		\
  0x0000000000000000ULL,						\
    0ULL, /*data does not matter */					\
    MLEN,								\
    FMAP,								\
    (NDHDR) | ((HINT)<<46) | ((VC) << 37),				\
    0x1000000000000000ULL,						\
    0,									\
    0 
#define FW_MU_WRAP_RCP_FIFOS_MKP2PDESC(MLEN, FMAP,HINT,VC) FW_MU_WRAP_RCP_FIFOS_MKDESC(0x5500000000000000UL, MLEN, FMAP,HINT,VC)
#define FW_MU_WRAP_RCP_FIFOS_MKCOLDESC(CLS, MLEN, FMAP,VC) FW_MU_WRAP_RCP_FIFOS_MKDESC(0x5A41001000000000UL | ((CLS)<<44UL), MLEN, FMAP,0UL,VC)

static __attribute((aligned(64))) uint64_t fw_mu_wrap_rcp_fifos_descs[] = {
  FW_MU_WRAP_RCP_FIFOS_MKP2PDESC(0x800UL, 0x0008UL, 0x200UL, 2UL), // A-, iME12, VC=high priority, 2KB out of 4KB
  FW_MU_WRAP_RCP_FIFOS_MKP2PDESC(0x800UL, 0x0004UL, 0x000UL, 3UL), // none, iME13, VC=system, 2KB out of 4KB
  FW_MU_WRAP_RCP_FIFOS_MKCOLDESC(0UL, 0x800UL, 0x0002UL, 4UL),        // , iME14, VC=commworld, 2KB out of 4KB
  FW_MU_WRAP_RCP_FIFOS_MKCOLDESC(1UL, 0x800UL, 0x0001UL, 6UL),        // , iME15, VC=sys coll,  2KB out of 4KB
};

uint64_t fw_mu_wrap_rcp_fifos_setup(void)
{
  uint64_t u;
  uint64_t old_croute;

  // open up ranges
  DCRWritePriv(MU_DCR(MIN_SYS_ADDR_RANGE), 0UL); // min = 0
  DCRWritePriv(MU_DCR(MAX_SYS_ADDR_RANGE), ~0UL); // max = 1111...
  DCRWritePriv(MU_DCR(MIN_USR_ADDR_RANGE), 0UL); // min = 0
  DCRWritePriv(MU_DCR(MAX_USR_ADDR_RANGE), ~0UL); // max = 1111...

  // save class route and setup
  old_croute = DCRReadPriv(ND_500_DCR(CTRL_COLL_CLASS_00_01));
  DCRWritePriv(ND_500_DCR(CTRL_COLL_CLASS_00_01),
	       ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS00_TYPE_set(0) | // usr commworld
	       ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS00_UP_PORT_I_set(BGQ_CLASS_INPUT_LINK_LOCAL) | // local link
	       ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS00_UP_PORT_O_set(0) | // no output
	       ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS01_TYPE_set(1) | // sys
	       ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS01_UP_PORT_I_set(BGQ_CLASS_INPUT_LINK_LOCAL) | // local link
	       ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS01_UP_PORT_O_set(0) // no output
	       );
  // set up imFIFOs
  for(u = 0; u < 4; u++) {
    uint64_t desc_addr = (uint64_t)(fw_mu_wrap_rcp_fifos_descs + u*8);
    uint64_t index = u & 0x7;
    uint64_t sgid = u >> 3;
    
    fw_mu_mmio_write(BGQ_MU_iDMA_START_OFFSET(0, sgid, index), desc_addr); // start
    fw_mu_mmio_write(BGQ_MU_iDMA_HEAD_OFFSET(0, sgid, index), desc_addr); //head
    fw_mu_mmio_write(BGQ_MU_iDMA_SIZE_OFFSET(0, sgid, index), 64*1024-1); //size
    fw_mu_mmio_write(BGQ_MU_iDMA_TAIL_OFFSET(0, sgid, index), desc_addr+64); //tail

  }
  return old_croute;
}

void fw_mu_wrap_rcp_fifos_start(void)
{
  fw_mu_mmio_write(BGQ_MU_iDMA_ENABLE_OFFSET(0, 0), 0xf0000000); // enable imFIFO 0-3
}

int fw_mu_wrap_rcp_fifos_wait(uint64_t old_croute)
{
  uint64_t u = 0;
  uint64_t timeout_count = 0;

  // wait for descriptor
  do {
    u = (DCRReadPriv(MU_DCR(MCSRAM_BD_FETCHING_DESC)) != 0) ? 0 : u + 1;
    if (++timeout_count > 30) return 12345;

  } while (u < 3); // wait until 3 successes

  // reset FIFO enable
  fw_mu_mmio_write(BGQ_MU_iDMA_DISABLE_OFFSET(0, 0), 0xffffffffUL); // disable imFIFOs

  // reset ranges
  DCRWritePriv(MU_DCR(MIN_SYS_ADDR_RANGE), ~0UL); // min = 1111...
  DCRWritePriv(MU_DCR(MAX_SYS_ADDR_RANGE), 0UL); // max = 0
  DCRWritePriv(MU_DCR(MIN_USR_ADDR_RANGE), ~0UL); // min = 1111...
  DCRWritePriv(MU_DCR(MAX_USR_ADDR_RANGE), 0UL); // max = 0
  
  // reset imFIFO ptrs
  for(u = 0; u < 4; u++){
    uint64_t index = u & 0x7;
    uint64_t sgid = u >> 3;
    
    fw_mu_mmio_write(BGQ_MU_iDMA_START_OFFSET(0, sgid, index), 0);
    fw_mu_mmio_write(BGQ_MU_iDMA_HEAD_OFFSET(0, sgid, index), 0); //head
    fw_mu_mmio_write(BGQ_MU_iDMA_SIZE_OFFSET(0, sgid, index), 64-1); //size
    fw_mu_mmio_write(BGQ_MU_iDMA_TAIL_OFFSET(0, sgid, index), 0); //tail
  }

  // wait for rME 12-15
  u = 0;
  timeout_count = 0;
  do {
    u = (DCRReadPriv(MU_DCR(RME_STATE0) + 1) != 0) ? 0 : u + 1;
    if (++timeout_count > 10) return 23456;
  } while (u < 3); // wait until 3 successes

  // restore class route
  DCRWritePriv(ND_500_DCR(CTRL_COLL_CLASS_00_01), old_croute);
  
  // clear rmFIFO not enabled error
  DCRWritePriv(MU_DCR(RCSRAM_INTERRUPTS__STATE), ~0UL);

  // clear imFIFO/rmFIFO interrupts
  fw_mu_mmio_write(BGQ_MU_CLEAR_INT_STATUS_REGISTER_OFFSET(0, 0), ~0UL);

  return 0;
}

int fw_mu_wrap_rcp_fifos(void)
{
  int rc = 0;
  uint64_t old_croute = fw_mu_wrap_rcp_fifos_setup();
  fw_mu_wrap_rcp_fifos_start();

  rc |= fw_mu_wrap_rcp_fifos_wait(old_croute);
  return rc;
}
#else // ifdef FW_MU_XSTATE_SIM
// otherwise, doesn't compile the code

int fw_mu_wrap_rcp_fifos(void) { return 0; }

#endif


//================================================================================

#define IS_BEDRAM(x)         ( ( (uint64_t)(x) & 0xFFFFFFFFFFFC0000ul ) == 0x000003FFFFFC0000ul )
#define IS_MU_DCR_ADDRESS(x) ( ( (x) >= MU_DCR_base ) && ( (x) < (MU_DCR_base + MU_DCR_size) ) )

typedef int (*MU_CustomHandler_t)(fw_uint64_t*, unsigned*, MU_MachineCheckData_t*, unsigned);

#undef SIMULATE_ERROR
//#define SIMULATE_ERROR 1

int fw_mu_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[32];
  unsigned i, j, n = 0;
  int rc = 0;

  // +------------------------------------------------------------------------+
  // | IMPLEMENTATION NOTE: We execute a two-pass walk of the machine check   |
  // |     data table.  The first pass handles situations that are not fatal; |
  // |     the second pass handles fatal situations.  This allows non-fatal   |
  // |     RAS events to be written to the mailbox before a fatal event is    |
  // |     written and potentially brings down the node.                      |
  // +------------------------------------------------------------------------+

  for ( i = 0 ; i < 2; i++ ) {

    n = 0; // Clear out the RAS details array for this pass.

    for ( j = 0; j < sizeof(MU_MachineCheck_Data)/sizeof(MU_MachineCheck_Data[0]); j++ ) {
      
      MU_MachineCheckData_t* mcd = &(MU_MachineCheck_Data[j]);

      // Skip over this entry if it doesn't have any interesting interrupt
      // bits for this pass:

      if ( mcd->level[i].mask != 0 ) {

	// Read the specified interrupt status DCR:
	uint64_t data = DCRReadPriv( mcd->dcrAddress );

#ifdef SIMULATE_ERROR
	// This code simulates errors and should not be enabled in production code
	
	if ( ( mcd->dcrAddress == MU_DCR(ECC_COUNT_INTERRUPTS__MACHINE_CHECK) ) && ( i == 0 ) ) {
	    data = MU_DCR__ECC_COUNT_INTERRUPTS__ECC_COUNT_OVER0_set(1);
	}
#endif

	// -------------------------------------------------------------------------
	// If one or more status bits are 'on' for this level, then walk the 
	// remainder of the table data for this entry.  The possible paths 
	// are:
	//
	//    - additionalInfo is NULL : This is a basic entry and we only
	//      need to capture the machine check status bits (i.e. the DCR
	//      contents.
	//    - additionalInfo is a DCR address : The address refers to an
	//      auxiliary DCR whose contents should also be packaged with
	//      the RAS payload.
	//    - additionalInfo is a BeDRAM address : The address is interpreted
	//      as a function pointer, i.e. a custom handler for this sitution.
	//      The handler is invoked.
	// -------------------------------------------------------------------------

	if ( ( data & mcd->level[i].mask ) != 0 ) {

	  if ( TRACE_ENABLED(TRACE_MChk) ) { //!< @todo - fix me

	    printf(
		   "i=%d j=%d DCR=%X data=%lX mask=%lX info=%lX isBeDRAM=%d isDCR=%d\n", 
		 i, 
		 j, 
		 mcd->dcrAddress, 
		 data, 
		 mcd->level[i].mask, 
		 mcd->level[i].additionalInfo, 
		 IS_BEDRAM(mcd->level[i].additionalInfo),
		 IS_MU_DCR_ADDRESS(mcd->level[i].additionalInfo)
		 );
	  }

	  if ( IS_BEDRAM(mcd->level[i].additionalInfo) ) {
	    MU_CustomHandler_t func = (MU_CustomHandler_t)mcd->level[i].additionalInfo;
	    rc |= func( details, &n, mcd, i );
	  }
	  else {

	    details[n++] = mcd->dcrAddress;
	    details[n++] = data;

	    if ( IS_MU_DCR_ADDRESS(mcd->level[i].additionalInfo) ) {
	      details[n++] = mcd->level[i].additionalInfo;
	      details[n++] = DCRReadPriv(mcd->level[i].additionalInfo);
	    }
	  }

	  // Clear the status.  Note that the register in the table is the Machine Check status
	  // register.  The status is cleared by writing to the State register, which is always
	  // five DCRs away.

	  DCRWritePriv( mcd->dcrAddress - 5, mcd->level[i].mask );
	  
	}
      }
    }

    // If there are RAS details on this pass, issue the event:
    if ( n > 0 ) {
      fw_uint32_t rasEvent = (i == 0) ? FW_RAS_MU_WARNING : FW_RAS_MU_ERROR;
      rc |=  ( (i == 0) ? 0 : -1 );
      fw_machineCheckRas( rasEvent, details, n, __FILE__, __LINE__ );
    }
  }

  return rc;
}

int fw_mu_captureRMEInterrupts0(uint64_t* details, unsigned* numDetails, MU_MachineCheckData_t* mcd, unsigned level) {
  
  // +-----------------------------------------------------------------------------+
  // | If rme_interrupts_0.bit[N] = 1 (N = 1, 7, 13, ..., 43 ), then also collect  |
  // | DCR rme_p_err[M]               (M = 0, 1,  2, ...,  7)                      |
  // |                                                                             |
  // | Note that M = (N - 1) / 6                                                   |
  // +-----------------------------------------------------------------------------+

  uint64_t rme_interrupts_0 = DCRReadPriv( MU_DCR(RME_INTERRUPTS0__MACHINE_CHECK) );
  unsigned n;

  // Add RME_INTERRUPTS0 regardless .... there may be additional info
  // to collect.

  details[ (*numDetails)++ ] = MU_DCR(RME_INTERRUPTS0__MACHINE_CHECK);
  details[ (*numDetails)++ ] = rme_interrupts_0;

  for ( n = 1; n <= 43; n += 6 ) {

    uint64_t mask = 1ull << (63-n);

    if ( rme_interrupts_0 & mask ) {

      unsigned m = (n - 1) / 6;

      details[ (*numDetails)++ ] = MU_DCR( RME_P_ERR0 ) + (m*2);
      details[ (*numDetails)++ ] = DCRReadPriv( MU_DCR( RME_P_ERR0 ) + (m*2) );
    }

  }
  return 0;
}

int fw_mu_captureRMEInterrupts1(uint64_t* details, unsigned* numDetails, MU_MachineCheckData_t* mcd, unsigned level) {
  
  // +-----------------------------------------------------------------------------+
  // | If rme_interrupts_1.bit[N] = 1 (N = 0, 6, 12, ..., 42 ), then also collect  |
  // | DCR rme_p_err[M]               (M = 8, 9, 10, ..., 15)                      |
  // |                                                                             |
  // | Note that M = N/6 + 8                                                       |
  // +-----------------------------------------------------------------------------+

  uint64_t rme_interrupts_1 = DCRReadPriv( MU_DCR(RME_INTERRUPTS1__MACHINE_CHECK) );
  unsigned n;

  // Add RME_INTERRUPTS1 regardless .... there may be additional info
  // to collect.

  details[ (*numDetails)++ ] = MU_DCR(RME_INTERRUPTS1__MACHINE_CHECK);
  details[ (*numDetails)++ ] = rme_interrupts_1;

  for ( n = 0; n <= 42; n += 6 ) {

    uint64_t mask = 1ull << (63-n);

    if ( rme_interrupts_1 & mask ) {

      unsigned m = (n / 6) + 8;

      details[ (*numDetails)++ ] = MU_DCR( RME_P_ERR0 ) + (m*2);
      details[ (*numDetails)++ ] = DCRReadPriv( MU_DCR( RME_P_ERR0 ) + (m*2) );
    }

  }
  return 0;
}

int fw_mu_captureIMEInterrupts(uint64_t* details, unsigned* numDetails, MU_MachineCheckData_t* mcd, unsigned level) {
  
  // +-----------------------------------------------------------------------------+
  // | If ime_interrupts.bit[N] = 1 (N = 1, 3, 5, ..., 31),  then also collect     |
  // | DCR ime_p_err[M]             (M = 0, 1, 2, ..., 15)                         |
  // |                                                                             |
  // | Note that M = N/2                                                           |
  // +-----------------------------------------------------------------------------+

  uint64_t ime_interrupts = DCRReadPriv( MU_DCR(IME_INTERRUPTS__MACHINE_CHECK) );
  unsigned n;

  // Add IME_INTERRUPTS regardless .... there may be additional info
  // to collect.

  details[ (*numDetails)++ ] = MU_DCR(IME_INTERRUPTS__MACHINE_CHECK);
  details[ (*numDetails)++ ] = ime_interrupts;

  for ( n = 1; n <= 31; n += 2 ) {

    uint64_t mask = 1ull << (63-n);

    if ( ime_interrupts & mask ) {

      unsigned m = (n / 2);

      details[ (*numDetails)++ ] = MU_DCR( IME_P_ERR0 ) + m;
      details[ (*numDetails)++ ] = DCRReadPriv( MU_DCR( IME_P_ERR0 ) + m );
    }

  }
  return 0;
}

typedef struct _ECC_COUNTERS_t {
    uint32_t counter;
    uint32_t status;
} ECC_COUNTERS_t;

static ECC_COUNTERS_t ECC_COUNTERS[] = {
    { MU_DCR(ARLOG_ECC_COUNT),  MU_DCR(ARLOG_INTERRUPTS__STATE),  },
    { MU_DCR(ICSRAM_ECC_COUNT), MU_DCR(ICSRAM_INTERRUPTS__STATE) }, 
    { MU_DCR(MCSRAM_ECC_COUNT), MU_DCR(MCSRAM_INTERRUPTS__STATE) },
    { MU_DCR(RCSRAM_ECC_COUNT), MU_DCR(RCSRAM_INTERRUPTS__STATE) },
    { MU_DCR(RPUT_ECC_COUNT),   MU_DCR(RPUTSRAM_INTERRUPTS__STATE) },
    { MU_DCR(MMREGS_ECC_COUNT), MU_DCR(MISC_ECC_CORR_INTERRUPTS__STATE) },
    { MU_DCR(XS_ECC_COUNT),     MU_DCR(MISC_ECC_CORR_INTERRUPTS__STATE) },
    { MU_DCR(RME_ECC_COUNT),    MU_DCR(RME_INTERRUPTS0__STATE) },
};



int fw_mu_eccCountInterrupts(uint64_t* unused_details, unsigned* unused_numDetails, MU_MachineCheckData_t* mcd, unsigned level) {


    // NOTE:  This routine issues its own correctable event and therefore allocates its own RAS
    //        message details array.

  fw_uint64_t details[32];
  int N = 0;

  uint64_t eccCountInterrupts = DCRReadPriv( MU_DCR(ECC_COUNT_INTERRUPTS__MACHINE_CHECK) );
  unsigned i;

#ifdef SIMULATE_ERROR
  eccCountInterrupts = MU_DCR__ECC_COUNT_INTERRUPTS__ECC_COUNT_OVER0_set(1) | MU_DCR__ECC_COUNT_INTERRUPTS__ECC_COUNT_OVER6_set(1);
#endif

  // Add the interrupt status:

  details[ N++ ] = MU_DCR(ECC_COUNT_INTERRUPTS__MACHINE_CHECK);
  details[ N++ ] = eccCountInterrupts;

  // Add the threshold:

  details[ N++ ] = MU_DCR(ECC_INT_THRESH);
  details[ N++ ] = DCRReadPriv( MU_DCR(ECC_INT_THRESH) );

  // NOTE: The ECC counters are reset via bits 24:31 of the MU_DCR__RESET register.  The bits are
  //       in the same order as the ECC_COUNTERS array.

  uint64_t resetMask = 0;

  for ( i = 0; i < sizeof(ECC_COUNTERS) / sizeof(ECC_COUNTERS[0]) ; i++ ) {

    if ( eccCountInterrupts & _BN(i) ) {

      // Collect both the counter and the status:
      details[ N++ ] = ECC_COUNTERS[i].counter;
      details[ N++ ] = DCRReadPriv( ECC_COUNTERS[i].counter );

      details[ N++ ] = ECC_COUNTERS[i].status;
      details[ N++ ] = DCRReadPriv( ECC_COUNTERS[i].status );

      resetMask |= ( ( MU_DCR__RESET__ECC_VEC_set(1) << (MU_DCR__RESET__ECC_VEC_width-1) ) >> i );
    }
  }

  // Clear the counters.  This is done by writing to the RESET register:
  DCRWritePriv( MU_DCR(RESET), resetMask );

  fw_machineCheckRas( FW_RAS_MU_ECC_SUMMARY, details, N, __FILE__, __LINE__ );

  return 0;
}

int fw_mu_handleIMUCorrectables(uint64_t* details, unsigned* numDetails, MU_MachineCheckData_t* mcd, unsigned level) {
  

  unsigned n;

  uint64_t imu_ecc = DCRReadPriv( MU_DCR(IMU_ECC_INTERRUPTS__MACHINE_CHECK) );

  details[ (*numDetails)++ ] = MU_DCR(IMU_ECC_INTERRUPTS__MACHINE_CHECK);
  details[ (*numDetails)++ ] = imu_ecc;

  uint64_t controlHigh = DCRReadPriv( MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH) );

  for ( n = 0; n <= 4; n ++ ) {

      uint64_t ce_mask = MU_DCR__IMU_ECC_INTERRUPTS__IMU_ECC_CE0_set(1) >> n;

      if ( ( imu_ecc & ce_mask ) != 0 ) {
	  controlHigh &= ~(MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE0_set(-1) >> (2*n));
      }
  }

  DCRWritePriv( MU_DCR(IMU_ECC_INTERRUPTS__STATE), imu_ecc );
  DCRWritePriv( MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH), controlHigh );

  return 0;
}

void fw_mu_flushCorrectables() {


  fw_uint64_t details[32];
  int i, N = 0;

  // NOTE: The ECC counters are reset via bits 24:31 of the MU_DCR__RESET register.  The bits are
  //       in the same order as the ECC_COUNTERS array.

  uint64_t resetMask = 0;

  for ( i = 0; i < sizeof(ECC_COUNTERS) / sizeof(ECC_COUNTERS[0]) ; i++ ) {

      uint64_t counter = DCRReadPriv( ECC_COUNTERS[i].counter );

      if ( counter > 0 ) {

	  // Collect both the counter and the status:
	  details[ N++ ] = ECC_COUNTERS[i].counter;
	  details[ N++ ] = counter;

	  details[ N++ ] = ECC_COUNTERS[i].status;
	  details[ N++ ] = DCRReadPriv( ECC_COUNTERS[i].status );

	  resetMask |= ( ( MU_DCR__RESET__ECC_VEC_set(1) << (MU_DCR__RESET__ECC_VEC_width-1) ) >> i );
      }
  }

  if ( resetMask != 0 ) {
      // Clear the counters.  This is done by writing to the RESET register:
      DCRWritePriv( MU_DCR(RESET), resetMask );
  }


  if ( N > 0 ) {
      fw_machineCheckRas( FW_RAS_MU_ECC_SUMMARY, details, N, __FILE__, __LINE__ );
  }
}

