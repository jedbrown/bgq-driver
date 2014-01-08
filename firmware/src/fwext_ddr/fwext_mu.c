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

your testcase code should then
#include <firmware/include/fwext/fwext_mu.h> 
see mu_reset_test1/test_main.c for sample use

-----------------------------------------------------*/

 
//#include <firmware/include/fwext/fwext.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>

#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/fwext_mu.h>

#include "Firmware_internals.h"

//int FW_MU_VERBOSE=0;
//int fw_mu_ismambo=0;



  
/********************************************************************************
 * fw_mu_reset_release : Main function to initialize MU (executed by core 0)
 ********************************************************************************/

int fw_mu_reset_release(Personality_t *p)  
{
  int rc = 0;
  fw_mu_ismambo = ((p->Kernel_Config.NodeConfig & PERS_ENABLE_Mambo) != 0);


#if 0
  //  +-------------------------------------------------------------------+
  //  |  NOTE: MU initialization is now in firmware but is gated by the   |
  //  |        enable bit in the personality.  So we test that bit here,  |
  //  |        which will catch any tests that have failed to enable the  |
  //  |        MU in their personality (svchost file).                    |
  //  +-------------------------------------------------------------------+

  if ( ( p->Kernel_Config.NodeConfig & PERS_ENABLE_MU ) == 0 ) {
    printf("(E) MU is not enabled.\n");
    Terminate(-1);
  }
#endif

  if ( ( p->Kernel_Config.NodeConfig & PERS_ENABLE_MU ) == 0 ) {

    extern int fw_mu_resetRelease( void );

    //printf("******************************** FORCING MU PERS BIT ***************************\n");

    p->Kernel_Config.NodeConfig |= PERS_ENABLE_MU; 
    rc = fw_mu_resetRelease();
    p->Kernel_Config.NodeConfig &= ~PERS_ENABLE_MU; 
    
  }

  return rc;

}


#if 0
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
#endif

#if 0 
/********************************************************************************
 * fw_mu_clear_icsram : Zero clear all ICSRAM fields
 ********************************************************************************/
int fw_mu_clear_icsram(Personality_t *p)
{
  uint64_t gid, sgid, i;

  //fw_mu_mmio_debug(p);

  for(gid = 0; gid < BGQ_MU_NUM_INJ_FIFO_GROUPS; gid ++){
    if (_G1(fw_mu_init_gmap, gid) == 0)  // skip this group if not specified in bit map
      continue;


    for(sgid = 0; sgid < BGQ_MU_NUM_INJ_FIFO_SUBGROUPS; sgid ++){ // loop on subgroup

      for(i = 0; i < BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP; i ++){ // loop on FIFOs in each subgroup
	if (FW_MU_VERBOSE) printf("initializing imFIFO(%ld, %ld, %ld)\n", gid, sgid, i);

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

  for(gid = 0; gid < BGQ_MU_NUM_INJ_FIFO_GROUPS; gid ++){
    if (_G1(fw_mu_init_gmap, gid) == 0)  // skip this group if not specified in bit map
      continue;


    for(sgid = 0; sgid < BGQ_MU_NUM_REC_FIFO_SUBGROUPS; sgid ++){ // loop on subgroup

      for(i = 0; i < BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP; i ++){ // loop on FIFOs in each subgroup
	if (FW_MU_VERBOSE) printf("initializing rmFIFO(%ld, %ld, %ld)\n", gid, sgid, i);	
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

  for(gid = 0; gid < BGQ_MU_NUM_INJ_FIFO_GROUPS; gid ++){
    if (_G1(fw_mu_init_gmap, gid) == 0)  // skip this group if not specified in bit map
      continue;

    for(sgid = 0; sgid < BGQ_MU_NUM_FIFO_SUBGROUPS; sgid ++){ // loop on subgroup

      for(i = 0; i < BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP; i ++){ // loop on bases in each subgroup
	if (FW_MU_VERBOSE) printf("initializing RPUT_SRAM(%ld, %ld, %ld)\n", gid, sgid, i);
	
	fw_mu_mmio_write(BGQ_MU_DATA_COUNTER_BASE_ADDRESS_OFFSET(gid, sgid, i), 0); // base field


      }
    }
  }
  // To test MMIO function, write a magic number to a location, then read back.
  {
    return fw_mu_check_mmio(BGQ_MU_DATA_COUNTER_BASE_ADDRESS_OFFSET(BGQ_MU_NUM_FIFO_GROUPS-1,
							     BGQ_MU_NUM_FIFO_SUBGROUPS-1,
							     BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP-1),
			    0x13cafebabeull, 0);

  }

}

// List of interrupt registers to clear after SRAM initialization
static const uint64_t int_to_clear[] = {
  MU_DCR(FIFO_INTERRUPTS__STATE),
  MU_DCR(IMU_ECC_INTERRUPTS__STATE),
  MU_DCR(RMU_ECC_INTERRUPTS__STATE),
  MU_DCR(ICSRAM_INTERRUPTS__STATE),
  MU_DCR(RMU_ECC_CORR_INTERRUPTS__STATE)
};

/********************************************************************************
 * fw_mu_clear_dcr_interrupts : Clear DCR interrupts which may be set by initial SRAM ECC error
 ********************************************************************************/
int fw_mu_clear_dcr_interrupts(Personality_t *p)
{
  unsigned u;
  for(u = 0; u < FW_MU_NELEMS(int_to_clear); u++){
    fw_mu_verbose_DCRWritePriv(int_to_clear[u], ~0UL);
  }
  return 0;
}

/********************************************************************************
 * fw_mu_enable_dcr_interrupts : Enable DCR interrupts
 ********************************************************************************/
int fw_mu_enable_dcr_interrupts(Personality_t *p)
{

  unsigned u;
  for(u = 0; u < FW_MU_NELEMS(fw_mu_dcr_intmode_data); u++){
    // refer to the constant array to get address-data pair.
    fw_mu_verbose_DCRWritePriv(fw_mu_dcr_intmode_data[u].addr,
				 fw_mu_dcr_intmode_data[u].intmode);
  }
  return 0;
}

/********************************************************************************
 * fw_mu_release_all_resets : deasserts all submodule resets
 ********************************************************************************/
int fw_mu_release_all_resets(Personality_t *p)
{
  // clear all bits
  fw_mu_verbose_DCRWritePriv(MU_DCR(RESET), 0);
  return 0;
}

/********************************************************************************
 * fw_mu_enable_me : enables all rMEs/iMEs
 ********************************************************************************/
int fw_mu_enable_me(Personality_t *p)
{
  // enable all iMEs
  fw_mu_verbose_DCRWritePriv(MU_DCR(IME_ENABLE),  MU_DCR__IME_ENABLE__VALUE_set(0xffff));
  // enable all rMEs
  fw_mu_verbose_DCRWritePriv(MU_DCR(RME_ENABLE),  MU_DCR__RME_ENABLE__VALUE_set(0xffff));
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
  for(gid = 0; gid < 1; gid ++)   // loop on group
#else
  for(gid = 0; gid < BGQ_MU_NUM_INJ_FIFO_GROUPS; gid ++)  // loop on group
#endif
    {
      fw_mu_mmio_write(BGQ_MU_CLEAR_INT_STATUS_REGISTER_OFFSET(gid, 0), ~0ULL); // clear interrupts
    }
  
  return 0;
}

#endif

/********************************************************************************
 * fw_mu_check_dcr_errints : check that no DCR error is reported
 ********************************************************************************/

const uint16_t fw_mu_dcr_intregs[] = {
  MU_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset,
  MU_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset,
  MU_DCR__MASTER_PORT0_INTERRUPTS__STATE_offset,   
  MU_DCR__MASTER_PORT1_INTERRUPTS__STATE_offset,   
  MU_DCR__MASTER_PORT2_INTERRUPTS__STATE_offset,   
  MU_DCR__SLAVE_PORT_INTERRUPTS__STATE_offset,     
  MU_DCR__MMREGS_INTERRUPTS__STATE_offset,         
  MU_DCR__FIFO_INTERRUPTS__STATE_offset,           
  MU_DCR__IMU_ECC_INTERRUPTS__STATE_offset,       
  MU_DCR__MCSRAM_INTERRUPTS__STATE_offset,         
  MU_DCR__ARLOG_INTERRUPTS__STATE_offset,          
  MU_DCR__RMU_ECC_INTERRUPTS__STATE_offset,        
  MU_DCR__RCSRAM_INTERRUPTS__STATE_offset,         
  MU_DCR__RPUTSRAM_INTERRUPTS__STATE_offset,       
  MU_DCR__IME_INTERRUPTS__STATE_offset,            
  MU_DCR__RME_INTERRUPTS0__STATE_offset,           
  MU_DCR__RME_INTERRUPTS1__STATE_offset,           
  MU_DCR__ICSRAM_INTERRUPTS__STATE_offset,         
  MU_DCR__MISC_INTERRUPTS__STATE_offset,           
  MU_DCR__RMU_ECC_CORR_INTERRUPTS__STATE_offset,   
  MU_DCR__MISC_ECC_CORR_INTERRUPTS__STATE_offset,  
  MU_DCR__ECC_COUNT_INTERRUPTS__STATE_offset,
};


int fw_mu_check_dcr_errints(Personality_t *p)
{
  uint64_t u, addr, regval;

  // try to clear FIFO interrupts
  DCRWritePriv(MU_DCR(FIFO_INTERRUPTS__STATE), MU_DCR__FIFO_INTERRUPTS__RMFIFO_INSUFFICIENT_SPACE_set(1));
  DCRWritePriv(MU_DCR(ICSRAM_INTERRUPTS__STATE), MU_DCR__ICSRAM_INTERRUPTS__ICSRAM_ERR23_set(1));

  for(u = 0; u < sizeof(fw_mu_dcr_intregs)/sizeof(fw_mu_dcr_intregs[0]); u++){
    addr = MU_DCR_base + fw_mu_dcr_intregs[u];
    regval = DCRReadPriv(addr);
    // work around for wrong parity on redun_ctrl (issue 513) => 6/27 resolved
    // if (__UNLIKELY(addr == MU_DCR(MISC_INTERRUPTS__STATE))){
    //   regval &= ~0x0100000000000000UL; // clear mu_err0
    // }
	  
    if (regval != 0) { // some error interrupt asserted
      printf("MU termination check error : DCR interrupt register %lx is nonzero (%lx)\n", 
	     addr, regval);
      return 111;
    }
  }
  return 0;
}


/********************************************************************************
 * fw_mu_term_check_ecc_count : check that all ECC counters are 0
 ********************************************************************************/

const uint16_t fw_mu_dcr_ecccregs[] = {
  MU_DCR__ARLOG_ECC_COUNT_offset,
  MU_DCR__ICSRAM_ECC_COUNT_offset,
  MU_DCR__MCSRAM_ECC_COUNT_offset,
  MU_DCR__RCSRAM_ECC_COUNT_offset,
  MU_DCR__RPUT_ECC_COUNT_offset,
  MU_DCR__MMREGS_ECC_COUNT_offset,
  MU_DCR__XS_ECC_COUNT_offset,
  MU_DCR__RME_ECC_COUNT_offset,
};

int fw_mu_term_check_ecc_count(Personality_t *p)
{


  uint64_t u, addr, regval;

  for(u = 0; u < sizeof(fw_mu_dcr_ecccregs)/sizeof(fw_mu_dcr_ecccregs[0]); u++){
    addr = MU_DCR_base + fw_mu_dcr_ecccregs[u];
    regval = DCRReadPriv(addr);
    if (regval != 0) { // ECC count not zero
      printf("MU termination check error : ECC count register %lx is nonzero (%lu)\n", 
	     addr, regval);
      return 112;
    }
  }
  return 0;
}
/********************************************************************************
 * fw_mu_check_logic_state : check that ICSRAM, rMEs, and iMEs are in stable state
 ********************************************************************************/

int fw_mu_check_logic_state(Personality_t *p)
{
  uint64_t u, regval;

  // check ICSRAM state
  regval = DCRReadPriv(MU_DCR(MCSRAM_BD_FETCHING_DESC));
  if (regval != 0) { // still fetching desc
    printf("MU termination check error : ICSRAM is still fetching descriptors\n");
    return 113;
  }

  // check rME state
  for(u = 0; u < 2; u ++){
    regval = DCRReadPriv(MU_DCR(RME_STATE0) + u);
    if (regval != 0) {
      printf("MU termination check error : at least one rME is not in idle state: state reg(%lu) = %lx\n", 
	     u, regval);
      return 114;
    }
  }

  // check iME state
  regval = DCRReadPriv(MU_DCR(IME_STATE));
  if (regval != 0x249249249249UL) { // iME is not in idle or arbitration wait state
    printf("MU termination check error : at least one iME is in active state: state reg = %lx\n", regval);
    return 115;
  }
  
  return 0;
}

/********************************************************************************
 * fw_mu_term_check : termination check
 ********************************************************************************/

int fw_mu_termCheck( void )
{
  int rc = 0;

  Personality_t* p = FW_PERSONALITY_PTR();

  if ( ! PERS_ENABLED( PERS_ENABLE_MU ) ) {
    return 0;
  }

  // check that no DCR error is reported
  if (rc == 0) rc = fw_mu_check_dcr_errints(p);

  // check that ICSRAM, iMEs and rMEs are in idle/arbitration wait state
  if (rc == 0) rc = fw_mu_check_logic_state(p);

  // check that ECC count is all 0
  if (rc == 0) rc = fw_mu_term_check_ecc_count(p);
  
  return rc;

}



/********************************************************************************
 * fw_mu_term_check : termination check
 ********************************************************************************/

int fw_mu_term_check(Personality_t *p)
{


  int rc = 0;

  if ( ( p->Kernel_Config.NodeConfig & PERS_ENABLE_MU ) == 0 ) {

    extern int fw_mu_termCheck( void );

    p->Kernel_Config.NodeConfig |= PERS_ENABLE_MU;
    rc = fw_mu_termCheck();
    p->Kernel_Config.NodeConfig &= ~PERS_ENABLE_MU;
  }
  else {
    rc = fwext_termCheck();
  }
  return rc;

}


//********************************************************************************
// checks whether all rMEs are in safe state
//********************************************************************************
int fw_mu_is_rme_freezed(uint64_t state)
{
  // returns 1 when all rMEs are in state 00000(idle), 10000(RCSRAM wait), or 10001(ICSRAM wait)

  uint64_t mask = 0x8c6318c631UL; // 10001 10001 10001...
  if (state & ~mask) { // at least 1 rME is in a state other than 00000, 00001, 10000, 10001
    return 0; 
  }
  // state &= mask;
  if ((~(state >> 4) & state) & 0x0842108421UL) { // at least 1 rME is in state 00001
    return 0;
  }
  return 1;
}

//********************************************************************************
// freeze rME
//********************************************************************************
void fw_mu_freeze_rme(void)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(RME_ENABLE), 0); // disable rME
  ppc_msync(); 
  unsigned state;
  unsigned u;
  for(u = 0; u < 2; u++){ // wait until all rMEs are in safe states
    do {
      state = DCRReadPriv(MU_DCR(RME_STATE0) + u);
    } while(fw_mu_is_rme_freezed(state) == 0);
  }
}


//********************************************************************************
// freeze iME
//********************************************************************************
void fw_mu_freeze_ime(void)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(IME_ENABLE), 0); // disable iME
  ppc_msync(); 
  unsigned state;
  do {
    state = DCRReadPriv(MU_DCR(IME_STATE));
  } while(state & ~0x249249249249UL); // at least 1 iME is not in safe state
}




//********************************************************************************
// reads all MCSRAM data word + ECC
//********************************************************************************
void fw_mu_bk_mcsram_readall(uint64_t* dest)
{
  uint64_t u;
  for(u = 0; u < 7; u++){
    dest[u] = fw_mu_bk_mcsram_rdata(u);
  }
}
//****************************************************************************************************
// ECC utility
//****************************************************************************************************

//------------------------------------------------------------
// ECC64 table
//------------------------------------------------------------
static const uint64_t fw_mu_gen_mat64[]= {
        0xff0000e8423c0f99ll,
        0x99ff0000e8423c0fll,
        0x0f99ff0000e8423cll,
        0x3c0f99ff0000e842ll,
        0x423c0f99ff0000e8ll,
        0xe8423c0f99ff0000ll,
        0x00e8423c0f99ff00ll,
        0x0000e8423c0f99ffll
};

//================================================================================
// calculate 64bit ECC
//================================================================================
uint64_t fw_mu_ecc64(uint64_t v) 
{

    unsigned ecc=0;
    const uint64_t* m=fw_mu_gen_mat64;
    int i;

    for (i=8;i;--i,++m)
    {
        uint64_t tl= v & *m;
        uint32_t t=((uint32_t)tl)^((uint32_t)(tl>>32));
        t^=t>>16;
        t^=t>>8;
        t^=t>>4;
        t^=t>>2;
        t^=t>>1;
        ecc<<=1;
        ecc|=(~t)&1;
    }
    return ecc;
}

//------------------------------------------------------------
// ECC32 table
//------------------------------------------------------------
static const uint32_t fw_mu_gen_mat32[] =  {
        0xf009523e,
        0xff808911,
        0x8f7804a8,
        0x48ffc045,
        0x2447bc02,
        0x12a47fe0,
        0x011223df
};

//================================================================================
// calculate 32bit ECC
//================================================================================


uint64_t fw_mu_ecc32(uint32_t v) 
{
  unsigned ecc=0;
  const uint32_t* m=fw_mu_gen_mat32;
  int i;

  for (i=7;i;--i,++m)
    {
      uint32_t t= v & *m;
      t^=t>>16;
      t^=t>>8;
      t^=t>>4;
      t^=t>>2;
      t^=t>>1;
      ecc<<=1;
      ecc|=(~t)&1;
    }
  return ecc;
}

//********************************************************************************
// regenerate MCSRAM ECC
//********************************************************************************
void fw_mu_bk_mcsram_regen_ecc(uint64_t* mcsram_data)
{
  uint64_t ecc = 0;
  uint64_t u;
  for(u = 0; u < 5; u++){
    ecc |= fw_mu_ecc64(mcsram_data[u]) << (u<<3);
  }
  ecc |= fw_mu_ecc32(mcsram_data[5]) << 40;

  mcsram_data[6] = ecc;
}

//********************************************************************************
// write whole MCSRAM line
//********************************************************************************
void fw_mu_bk_mcsram_writeall(const uint64_t* mcsram_data)
{
  uint64_t u;
  
  u = _BGQ_GET(56, 55, mcsram_data[0]);
  fw_mu_bk_mcsram_write(u, 0, 0, 1);
  u = (_BGQ_GET(8, 63, mcsram_data[0]) << 48) | (_BGQ_GET(47, 46, mcsram_data[1]) << 1);
  fw_mu_bk_mcsram_write(u, 1, 0, 0);
  u = _BGQ_GET(56, 55, mcsram_data[2]);
  fw_mu_bk_mcsram_write(u, 2, 0, 1);
  u = (_BGQ_GET(8, 63, mcsram_data[2]) << 48) | _BGQ_GET(48, 47, mcsram_data[3]);
  fw_mu_bk_mcsram_write(u, 3, 0, 0);  
  u = (_BGQ_GET(16, 63, mcsram_data[3]) << 40) | _BGQ_GET(40, 39, mcsram_data[4]);
  fw_mu_bk_mcsram_write(u, 4, 0, 1);  
  u = (_BGQ_GET(24, 63, mcsram_data[4]) << 32) | (_BGQ_GET(29, 28, mcsram_data[5]) << 3);
  fw_mu_bk_mcsram_write(u, 5, 0, 0);  
  u = 
    (_BGQ_GET(8, 63, mcsram_data[6]) << 39) |
    (_BGQ_GET(8, 55, mcsram_data[6]) << 31) |
    (_BGQ_GET(8, 47, mcsram_data[6]) << 23) |
    (_BGQ_GET(8, 39, mcsram_data[6]) << 15) |
    (_BGQ_GET(8, 31, mcsram_data[6]) << 7)  |
    (_BGQ_GET(7, 23, mcsram_data[6]) << 0);
  u <<= 9;
  fw_mu_bk_mcsram_write(u, 6, 1, 1);
  fw_mu_bk_mcsram_write(0, 0, 0, 0);
}

void fw_machineCheckRas( uint32_t rasMessageCode, uint64_t details[], uint16_t numDetails, const char* file, int lineNumber ) {
}

int fw_writeRASEvent( fw_uint32_t msgId, fw_uint16_t numDetails, fw_uint64_t details[] ) {
    return fwext_getFwInterface()->writeRASEvent(msgId, numDetails, details );
}


