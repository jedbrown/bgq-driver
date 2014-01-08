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
#ifndef   _BGQ_FW_MU_H_ // Prevent multiple inclusion
#define   _BGQ_FW_MU_H_

#include <hwi/include/bqc/mu_dcr.h>
#include <hwi/include/bqc/MU_Macros.h>

#include "fwext_lib.h"


// Enables debug print
extern int FW_MU_VERBOSE;
// Is mambo?
extern int fw_mu_ismambo;


// MU initialization function 
extern int  fw_mu_reset_release(Personality_t *);
// termination check
extern int  fw_mu_term_check(Personality_t *);
// work around for init-X sims
extern int  fw_mu_wrap_rcp_fifos(void);

#define FW_MU_NELEMS(X)  (sizeof(X)/sizeof((X)[0]))

// write DCR with optional verbose print
extern inline void fw_mu_verbose_DCRWritePriv(uint64_t addr, uint64_t val)
{
  DCRWritePriv( (addr),(val));
  if (FW_MU_VERBOSE) printf(" dcr write address= 0x%016lx dcr val = 0x%016lx \n", addr, val);
}

// sets non-rget imFIFO freespace threshold in unit of 64B descriptors
extern inline void fw_mu_set_imu_threshold(uint64_t thresh)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(IMU_THOLD), MU_DCR__IMU_THOLD__VALUE_set(thresh));
}

// sets rget imFIFO freespace threshold in unit of 64B descriptors
extern inline void fw_mu_set_rgetfifo_threshold(uint64_t thresh)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(RGET_THOLD), MU_DCR__RGET_THOLD__VALUE_set(thresh));
}

// sets rmFIFO interrput threshold in unit of 32B chunks
extern inline void fw_mu_set_rmfifo_threshold(uint64_t thresh)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(RMU_THOLD), MU_DCR__RMU_THOLD__VALUE_set(thresh));
}

extern inline void fw_mu_set_dcr_bit(uint64_t addr, uint64_t bit_pos, uint64_t val)
{
  uint64_t rval = DCRReadPriv(addr);
  uint64_t set_bit  = _B1(bit_pos, val != 0);
  uint64_t set_mask = _BN(bit_pos);

  if ((rval & set_mask)^set_bit){ // current value is different from the required value
    fw_mu_verbose_DCRWritePriv(addr, rval ^ set_mask);
  }
}

// sets (range_id)-th usr range register
//  range_id : 0-19 (Not fixed yet. May will be changed)
// min_value : minimum address of the accessible range. Actual minimum byte address will be
//             (min_value & 0xffff00000). In other words, aligned to 1MB boundary.
// max_value : maximum address of the accessible range. Actual maximum byte address will be
//             (max_value | 0x0000fffff). In other words, rounded up to 1MB boundary.
extern inline void fw_mu_set_usr_range(unsigned range_id, uint64_t min_value, uint64_t max_value)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(MIN_USR_ADDR_RANGE)+range_id, min_value);
  fw_mu_verbose_DCRWritePriv(MU_DCR(MAX_USR_ADDR_RANGE)+range_id, max_value);
}

// specifies that (range_id)-th user range register is dedicated for injection side
extern inline void fw_mu_set_usr_range_isinj(unsigned range_id, int is_inj)
{
  fw_mu_set_dcr_bit(MU_DCR(USR_INJ_RANGE), range_id+44, is_inj);
}

// sets (range_id)-th sys range register
// range_id : 0-7 (Not fixed yet. May will be changed)
// min_value : minimum address of accessible range. Aligned in the same as user range
// max_value : minimum address of accessible range. Aligned in the same as user range
extern inline void fw_mu_set_sys_range(unsigned range_id, uint64_t min_value, uint64_t max_value)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(MIN_SYS_ADDR_RANGE)+range_id, min_value);
  fw_mu_verbose_DCRWritePriv(MU_DCR(MAX_SYS_ADDR_RANGE)+range_id, max_value);
}

// specifies that (range_id)-th system range register is dedicated for injection side
extern inline void fw_mu_set_sys_range_isinj(unsigned range_id, int is_inj)
{
  fw_mu_set_dcr_bit(MU_DCR(SYS_INJ_RANGE), range_id+56, is_inj);
}

//********************************************************************************
// Set system/user/rget permissions
//********************************************************************************

extern inline void fw_mu_set_imfifo_system(int imfifo_id, int is_sys)
{
  uint64_t reg_addr = MU_DCR(IMFIFO_SYSTEM) + (imfifo_id >> 5);
  fw_mu_set_dcr_bit(reg_addr, (imfifo_id & 0x1f)+32, is_sys);
}

extern inline void fw_mu_set_rmfifo_system(int rmfifo_id, int is_sys)
{
  uint64_t reg_addr = MU_DCR(RMFIFO_SYSTEM) + (rmfifo_id >> 4);
  fw_mu_set_dcr_bit(reg_addr, (rmfifo_id & 0xf)+48, is_sys);
}

extern inline void fw_mu_set_rputbase_system(int rput_group_id, int is_sys)
{
  uint64_t reg_addr = MU_DCR(RPUTBASE_SYSTEM0) + (rput_group_id >> 6);
  fw_mu_set_dcr_bit(reg_addr, (rput_group_id & 0x3f), is_sys);
}

extern inline void fw_mu_set_imfifo_rget(int imfifo_id, int is_rget)
{
  uint64_t reg_addr = MU_DCR(IMFIFO_RGET) +  (imfifo_id >> 5);
  fw_mu_set_dcr_bit(reg_addr, (imfifo_id & 0x1f)+32, is_rget);

}


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

//
// sets verbose flag
extern inline void fw_mu_set_verbose(int v)
{
  FW_MU_VERBOSE =v;
}

// TODO  should come from testint
extern inline int fw_mu_get_use_port6_for_io()
{
  return 0;
}

// TODO  should come from testint
extern inline int fw_mu_get_use_port7_for_io()
{
  return 0;
}


// TODO  in loopback, if one is set, so must the other
extern inline int fw_mu_get_use_port67_for_io()
{
  return ( fw_mu_get_use_port6_for_io() || fw_mu_get_use_port7_for_io() );
  
}
extern int fw_mu_is_rme_freezed(uint64_t state);

extern void fw_mu_freeze_rme(void);

extern inline void fw_mu_unfreeze_rme(void)
{
    fw_mu_verbose_DCRWritePriv(MU_DCR(RME_ENABLE), 0xffff000000000000UL); // disable rME
}

extern void fw_mu_freeze_ime(void);

extern inline void fw_mu_unfreeze_ime(void)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(IME_ENABLE), 0xffff000000000000UL); // disable iME
}

extern inline uint64_t fw_mu_bk_rcsram_rdata(void)
{
  return DCRReadPriv(MU_DCR(RCSRAM_BD_RDATA));
}

extern inline uint64_t fw_mu_bk_rcsram_gettail(uint64_t dcr_readval)
{
  return MU_DCR__RCSRAM_BD_RDATA__ID_get(dcr_readval);
}

extern inline uint64_t fw_mu_bk_rcsram_getecc(uint64_t dcr_readval)
{
  return MU_DCR__RCSRAM_BD_RDATA__ECC_get(dcr_readval);
}

extern inline void fw_mu_bk_rcsram_control(uint64_t id, uint64_t ecc, uint64_t tail_sel, uint64_t rd_en, uint64_t wid_en, uint64_t wecc_en)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(RCSRAM_BD_CTL),
			     MU_DCR__RCSRAM_BD_CTL__ID_set(id) |
			     MU_DCR__RCSRAM_BD_CTL__ECC_set(ecc) |
			     MU_DCR__RCSRAM_BD_CTL__TAIL_SEL_set(tail_sel) |
			     MU_DCR__RCSRAM_BD_CTL__RD_EN_set(rd_en) |
			     MU_DCR__RCSRAM_BD_CTL__WID_EN_set(wid_en) |
			     MU_DCR__RCSRAM_BD_CTL__WECC_EN_set(wecc_en));
}
extern inline void fw_mu_bk_rputsram_control(uint64_t en, uint64_t nowrecc, uint64_t wrecc)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(RPUT_BACKDOOR),
			     MU_DCR__RPUT_BACKDOOR__EN_set(en) |
			     MU_DCR__RPUT_BACKDOOR__NOWRECC_set(nowrecc) |
			     MU_DCR__RPUT_BACKDOOR__WRECC_set(wrecc));
}
extern inline uint64_t fw_mu_bk_rputsram_rdecc(void)
{
  return MU_DCR__RPUT_BACKDOOR_RDECC__VALUE_get(DCRReadPriv(MU_DCR(RPUT_BACKDOOR_RDECC)));
}

extern inline void fw_mu_bk_mcsram_control(uint64_t index, uint64_t en, uint64_t nowrecc, uint64_t rd)
{
  fw_mu_verbose_DCRWritePriv(MU_DCR(MCSRAM_BD_CTL),
			     MU_DCR__MCSRAM_BD_CTL__INDEX_set(index) |
			     MU_DCR__MCSRAM_BD_CTL__EN_set(en) |
			     MU_DCR__MCSRAM_BD_CTL__NOWRECC_set(nowrecc) |
			     MU_DCR__MCSRAM_BD_CTL__RD_set(rd));
}

extern inline uint64_t fw_mu_bk_mcsram_rdata(uint64_t word_id)
{
  return DCRReadPriv(MU_DCR(MCSRAM_BD_RDATA0) + word_id);
}

extern inline uint64_t fw_mu_bk_mcsram_get_start(const uint64_t *mcsram_rdata)
{
  return _BGQ_GET(37, 36, mcsram_rdata[0]);
}

extern inline uint64_t fw_mu_bk_mcsram_get_mlen(const uint64_t *mcsram_rdata)
{
  return (_BGQ_GET(27, 63, mcsram_rdata[0]) << 10) | _BGQ_GET(10, 9, mcsram_rdata[1]);
}

extern inline uint64_t fw_mu_bk_mcsram_get_poff(const uint64_t *mcsram_rdata)
{
  return _BGQ_GET(37, 46, mcsram_rdata[1]);
}

extern inline uint64_t fw_mu_bk_mcsram_get_hd0(const uint64_t *mcsram_rdata)
{
  return mcsram_rdata[2];
}

extern inline uint64_t fw_mu_bk_mcsram_get_hd1(const uint64_t *mcsram_rdata)
{
  return mcsram_rdata[4] & 0xffffffffffe00000UL;
}

extern inline uint64_t fw_mu_bk_mcsram_get_hd2(const uint64_t *mcsram_rdata)
{
  return (_BGQ_GET(21, 63, mcsram_rdata[4]) << 27) | _BGQ_GET(27, 26, mcsram_rdata[5]);
}

extern inline uint64_t fw_mu_bk_mcsram_get_hd3(const uint64_t *mcsram_rdata)
{
  return mcsram_rdata[3];
}

extern inline uint64_t fw_mu_bk_mcsram_get_pfetch(const uint64_t *mcsram_rdata)
{
  return _BGQ_GET(1, 27, mcsram_rdata[5]);
}

extern inline uint64_t fw_mu_bk_mcsram_get_int(const uint64_t *mcsram_rdata)
{
  return _BGQ_GET(1, 28, mcsram_rdata[5]);
}

extern inline uint64_t fw_mu_bk_mcsram_get_ecc(uint64_t dcr_readval,  uint64_t word_id)
{
  return _BGQ_GET(8, 63-word_id*8, dcr_readval);
}

extern inline void fw_mu_bk_mcsram_set_drop(uint64_t *mcsram_rdata, int drop)
{
  _BGQ_INSERT(1, 8, mcsram_rdata[4], drop);
}

extern inline void fw_mu_bk_mcsram_write(uint64_t write_value, uint64_t word_id, uint64_t do_write, uint64_t latch_data) {
  fw_mu_verbose_DCRWritePriv(MU_DCR(MCSRAM_BD_WDATA), 
			     MU_DCR__MCSRAM_BD_WDATA__VALUE_set(write_value) |
			     MU_DCR__MCSRAM_BD_WDATA__ID_set(word_id) |
			     MU_DCR__MCSRAM_BD_WDATA__WR_set(do_write) |
			     MU_DCR__MCSRAM_BD_WDATA__LATCH_DATA_set(latch_data));
}
			     
extern void fw_mu_bk_mcsram_regen_ecc(uint64_t* mcsram_data);
extern void fw_mu_bk_mcsram_writeall(const uint64_t* mcsram_data);



extern void fw_mu_bk_mcsram_readall(uint64_t* dest);


#endif
