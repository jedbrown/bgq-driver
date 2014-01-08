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
#include <firmware/include/fwext/fwext.h>
//#include "core_dcr.h"
//#include "core_parms.h"
inline uint64_t  get_ibm_bit64 (uint64_t val) ;
inline uint16_t  get_std_bit16 (uint16_t val) ;
inline uint64_t ibm_move64_upto (uint64_t val, int bit) ;
inline uint64_t std_move_upto (uint64_t val, int bit) ;
inline uint64_t get_mask64 (int size) ;
inline uint64_t get_ibm_mask64 (int size, int bitnum) ;
inline uint64_t get_ibm_range64 (uint64_t val, int from, int to) ;
inline uint64_t quickmask (int *array) ;
inline uint64_t quickmask1 (int *array) ;
inline uint64_t quickval (uint64_t val, int *array) ;
inline uint64_t quickget (uint64_t val, int *array) ;
void pma_wait_lock_pll ();
void pma_wait_lanes ();
void dcr_wr (int addr, uint64_t val) ;
uint64_t dcr_rd (int addr) ;
void dcr_set (int addr, uint64_t setmask) ;
void dcr_set_acc (int addr, uint64_t setmask) ;
void dcr_clr_acc (int addr, uint64_t clrmask) ;
void dcr_acc_flush (int addr) ;
void dcr_clr (int addr, uint64_t clrmask) ;
uint64_t get_goal_mask ();
int use_left ();
int use_right ();
void pma_wake_lanes ();
void pma_wake_lanes_eonly ();
void pma_wake_lanes_uni ();
void  lanes_release_reset ();
void pma_wake_pll () ;
void pma_wake_pll_eonly () ;
void pma_wake_pll_uni () ;
void pma_enable_pclk ();
void pma_reset_pcs_rcb () ;
void sd_init_0 (int usem, int usep, int addr) ;
void sd_init_1 (int usem, int usep, int addr) ;
void sd_top_init () ;
void sd_bottom_init () ;
void rx_reset_assert (int usem, int usep, int addr) ;
void rx_reset_deassert (int usem, int usep, int addr) ;
void pcs_reset_assert (int usem, int usep, int addr) ;
void pcs_reset_deassert (int usem, int usep, int addr) ;
void pipe0_reset_deassert (int usem, int usep, int addr) ;
void assert_resets () ;
void pcs_reset () ;
void pma_t2 ();
void pma_t4 ();
void training_disable (int usem, int usep, int addr);
void pma_t5 ();
void train_set (int usem, int usep, int addr) ;
void pma_train () ;
void sd_forcestop_elastic_buffers () ;
void sd_release_elastic_buffers () ;
void lanes_rxtxrst_deassert () ;

