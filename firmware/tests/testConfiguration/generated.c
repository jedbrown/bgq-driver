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
#include <hwi/include/bqc/dcr_support.h>
#include <hwi/include/bqc/mu_dcr.h>
#include <stdio.h>

void test_master_port0_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR0_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err0",
      "(fatal error) sked_p_err - parity error on boundary latched value of sw_xm_sked_valid_2, sw_xm_sked_id_2 and sw_xm_sked_eager_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR1_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err1",
      "(fatal error) sked_id_err - MSB of switch sked_id is set and MU only supports 16 L2 slice destinations");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR2_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err2",
      "(fatal error) sked_eager_err - switch attempted eager schedule and MU doesn't support eager scheduling");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR3_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err3",
      "(fatal error) sked_dly_p_err - parity error on latched sked_valid and lut_wr_valid");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR4_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err4",
      "(fatal error) sr_xm_rd_valid_p_err - parity error on boundary latched value of sr_xm_rd_valid_2  and  sr_xm_rd_valid_p_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR5_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err5",
      "(fatal error) sr_xm_rd_info_p_err - parity error on boundary latched value of sr_xm_rd_info_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR6_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err6",
      "(fatal error) newreq_attr_p_err - parity error on internal ime_rtn_attr and newreq_info_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR7_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err7",
      "(fatal error) rdreturn_size_err - sr_xm_rd_info(0 to 1) specified an illegal '11' size");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR8_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err8",
      "(fatal error) avail_ime_idx_p_err - parity error covering the avail_ime_idx vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR9_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err9",
      "(fatal error) avail_idx_p_err - parity error covering the avail_idx vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR10_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err10",
      "(fatal error) alloc_ime_idx_p_err - parity error covering alloc_ime_idx_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR11_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err11",
      "(fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR12_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err12",
      "(fatal error) rdreturn_idx_notvalid_err - error if tag of read return is listed as unallocated");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR13_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err13",
      "(fatal error) ime_lut_p_err - parity error on LUT info associated with tag for read return");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR14_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err14",
      "(fatal error) beat_cnt_p_err - parity error on beat counter for read return data");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR15_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err15",
      "(fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR16_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err16",
      "(fatal error) fifo_dest_p_err - parity error in current request's destination field stored in request FIFO");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR17_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err17",
      "(fatal error) lut_valid_p_err - parity error on lut_valid vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR18_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err18",
      "(fatal error) lut_attr_p_err - parity error in look-up-table attributes for current request going out to switch");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR19_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err19",
      "(fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR20_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err20",
      "(fatal error) dealloc_idx_p_err - parity error on dealloc_idx");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR21_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err21",
      "(fatal error) genid_p_err - parity error on msgc_gen_ctr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR22_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR22_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err22",
      "(fatal error) req_cnt_p_err - parity error on the count of outstanding write requests from each rME");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR23_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_FATAL_ERR23_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_fatal_err23",
      "(fatal error) newreq_valid_p_err - parity error in the incoming request valid buffers");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_SOFTWARE_ERR24_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_SOFTWARE_ERR24_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_software_err24",
      "(software error) newreq_write_range_err - write request address range check violation, see master port error info register");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_SOFTWARE_ERR25_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_SOFTWARE_ERR25_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_software_err25",
      "(software error) newreq_read_range_err - read request address range check violation, see master port error info register");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_CORR_ERR26_set(-1) ) != MU_DCR__MASTER_PORT0_INTERRUPTS_CONTROL_HIGH__XM0_CORR_ERR26_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(MASTER_PORT0_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port0_interrupts",
      "xm0_corr_err26",
      "(correctable error) gen_cnt_p_err - gen_cnt latch HW correctable parity error");
  }
}

void test_master_port1_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR0_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err0",
      "(fatal error) sked_p_err - parity error on boundary latched value of sw_xm_sked_valid_2, sw_xm_sked_id_2 and sw_xm_sked_eager_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR1_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err1",
      "(fatal error) sked_id_err - MSB of switch sked_id is set and MU only supports 16 L2 slice destinations");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR2_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err2",
      "(fatal error) sked_eager_err - switch attempted eager schedule and MU doesn't support eager scheduling");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR3_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err3",
      "(fatal error) sked_dly_p_err - parity error on latched sked_valid and lut_wr_valid");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR4_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err4",
      "(fatal error) sr_xm_rd_valid_p_err - parity error on boundary latched value of sr_xm_rd_valid_2  and  sr_xm_rd_valid_p_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR5_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err5",
      "(fatal error) sr_xm_rd_info_p_err - parity error on boundary latched value of sr_xm_rd_info_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR6_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err6",
      "(fatal error) newreq_attr_p_err - parity error on internal ime_rtn_attr and newreq_info_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR7_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err7",
      "(fatal error) rdreturn_size_err - sr_xm_rd_info(0 to 1) specified an illegal '11' size");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR8_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err8",
      "(fatal error) avail_ime_idx_p_err - parity error covering the avail_ime_idx vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR9_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err9",
      "(fatal error) avail_idx_p_err - parity error covering the avail_idx vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR10_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err10",
      "(fatal error) alloc_ime_idx_p_err - parity error covering alloc_ime_idx_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR11_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err11",
      "(fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR12_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err12",
      "(fatal error) rdreturn_idx_notvalid_err - error if tag of read return is listed as unallocated");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR13_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err13",
      "(fatal error) ime_lut_p_err - parity error on LUT info associated with tag for read return");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR14_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err14",
      "(fatal error) beat_cnt_p_err - parity error on beat counter for read return data");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR15_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err15",
      "(fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR16_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err16",
      "(fatal error) fifo_dest_p_err - parity error in current request's destination field stored in request FIFO");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR17_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err17",
      "(fatal error) lut_valid_p_err - parity error on lut_valid vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR18_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err18",
      "(fatal error) lut_attr_p_err - parity error in look-up-table attributes for current request going out to switch");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR19_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err19",
      "(fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR20_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err20",
      "(fatal error) dealloc_idx_p_err - parity error on dealloc_idx");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR21_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err21",
      "(fatal error) genid_p_err - parity error on msgc_gen_ctr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR22_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR22_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err22",
      "(fatal error) req_cnt_p_err - parity error on the count of outstanding write requests from each rME");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR23_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_FATAL_ERR23_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_fatal_err23",
      "(fatal error) newreq_valid_p_err - parity error in the incoming request valid buffers");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_SOFTWARE_ERR24_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_SOFTWARE_ERR24_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_software_err24",
      "(software error) newreq_write_range_err - write request address range check violation, see master port error info register");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_SOFTWARE_ERR25_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_SOFTWARE_ERR25_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_software_err25",
      "(software error) newreq_read_range_err - read request address range check violation, see master port error info register");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_CORR_ERR26_set(-1) ) != MU_DCR__MASTER_PORT1_INTERRUPTS_CONTROL_HIGH__XM1_CORR_ERR26_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(MASTER_PORT1_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port1_interrupts",
      "xm1_corr_err26",
      "(correctable error) gen_cnt_p_err - gen_cnt latch HW correctable parity error");
  }
}

void test_arlog_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(ARLOG_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR0_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err0",
      "(fatal error) iME-to-imFIFO arbitration logic parity error in control flag latches");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR1_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err1",
      "(fatal error) iME-to-imFIFO arbitration stage 1 pipeline latch parity error (ready flag set request lost)");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR2_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err2",
      "(fatal error) iME-to-imFIFO arbitration logic, multiple requests accepted at the same time");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR3_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err3",
      "(non-fatal error) iME-to-imFIFO arbitration logic stage 1 pipeline latch parity error (recovered by retry)");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR4_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err4",
      "(non-fatal error) iME-to-imFIFO arbitration logic ime_available register bit error (recovered by retry)");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR5_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err5",
      "(non-fatal error) iME-to-imFIFO arbitration logic 136to1 arbiter random bit latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR6_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err6",
      "(non-fatal error) iME-to-imFIFO arbitration logic 64to1 arbiter random bit latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR7_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err7",
      "(fatal error) iME-to-imFIFO arbitration logic FIFO map latch scan and correct index counter parity error");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR8_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err8",
      "(fatal error) iME-to-imFIFO arbitration logic previous FIFO map read index latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR9_set(-1) ) != MU_DCR__ARLOG_INTERRUPTS_CONTROL_HIGH__ARLOG_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(ARLOG_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "arlog_interrupts",
      "arlog_err9",
      "(non-fatal error) iME-to-imFIFO arbitration logic 136to1 arbitration result latch parity error");
  }
}

void test_master_port2_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR0_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err0",
      "(fatal error) sked_p_err - parity error on boundary latched value of sw_xm_sked_valid_2, sw_xm_sked_id_2 and sw_xm_sked_eager_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR1_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err1",
      "(fatal error) sked_id_err - MSB of switch sked_id is set and MU only supports 16 L2 slice destinations");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR2_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err2",
      "(fatal error) sked_eager_err - switch attempted eager schedule and MU doesn't support eager scheduling");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR3_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err3",
      "(fatal error) sked_dly_p_err - parity error on latched sked_valid and lut_wr_valid");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR4_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err4",
      "(fatal error) sr_xm_rd_valid_p_err - parity error on boundary latched value of sr_xm_rd_valid_2  and  sr_xm_rd_valid_p_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR5_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err5",
      "(fatal error) sr_xm_rd_info_p_err - parity error on boundary latched value of sr_xm_rd_info_2");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR6_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err6",
      "(fatal error) newreq_attr_p_err - parity error on internal ime_rtn_attr and newreq_info_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR7_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err7",
      "(fatal error) rdreturn_size_err - sr_xm_rd_info(0 to 1) specified an illegal '11' size");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR8_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err8",
      "(fatal error) avail_ime_idx_p_err - parity error covering the avail_ime_idx vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR9_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err9",
      "(fatal error) avail_idx_p_err - parity error covering the avail_idx vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR10_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err10",
      "(fatal error) alloc_ime_idx_p_err - parity error covering alloc_ime_idx_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR11_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err11",
      "(fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR12_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err12",
      "(fatal error) rdreturn_idx_notvalid_err - error if tag of read return is listed as unallocated");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR13_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err13",
      "(fatal error) ime_lut_p_err - parity error on LUT info associated with tag for read return");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR14_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err14",
      "(fatal error) beat_cnt_p_err - parity error on beat counter for read return data");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR15_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err15",
      "(fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR16_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err16",
      "(fatal error) fifo_dest_p_err - parity error in current request's destination field stored in request FIFO");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR17_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err17",
      "(fatal error) lut_valid_p_err - parity error on lut_valid vector");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR18_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err18",
      "(fatal error) lut_attr_p_err - parity error in look-up-table attributes for current request going out to switch");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR19_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err19",
      "(fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR20_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err20",
      "(fatal error) dealloc_idx_p_err - parity error on dealloc_idx");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR21_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err21",
      "(fatal error) genid_p_err - parity error on msgc_gen_ctr");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR22_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR22_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err22",
      "(fatal error) req_cnt_p_err - parity error on the count of outstanding write requests from each rME");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR23_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_FATAL_ERR23_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_fatal_err23",
      "(fatal error) newreq_valid_p_err - parity error in the incoming request valid buffers");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_SOFTWARE_ERR24_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_SOFTWARE_ERR24_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_software_err24",
      "(software error) newreq_write_range_err - write request address range check violation, see master port error info register");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_SOFTWARE_ERR25_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_SOFTWARE_ERR25_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_software_err25",
      "(software error) newreq_read_range_err - read request address range check violation, see master port error info register");
  }
  if ( ( controlHIGH & MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_CORR_ERR26_set(-1) ) != MU_DCR__MASTER_PORT2_INTERRUPTS_CONTROL_HIGH__XM2_CORR_ERR26_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(MASTER_PORT2_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "master_port2_interrupts",
      "xm2_corr_err26",
      "(correctable error) gen_cnt_p_err - gen_cnt latch HW correctable parity error");
  }
}

void test_ecc_count_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER0_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over0",
      "(correctable error) iME-to-imFIFO arbitration logic correctable ECC error count has exceeded the threshold.");
  }
  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER1_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over1",
      "(correctable error) ICSRAM correctable ECC error count has exceeded the threshold.");
  }
  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER2_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over2",
      "(correctable error) MCSRAM correctable ECC error count has exceeded the threshold.");
  }
  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER3_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over3",
      "(correctable error) RCSRAM correctable ECC error count has exceeded the threshold.");
  }
  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER4_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over4",
      "(correctable error) RPUT correctable ECC error count has exceeded the threshold.");
  }
  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER5_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over5",
      "(correctable error) MMREGS correctable ECC error count has exceeded the threshold.");
  }
  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER6_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over6",
      "(correctable error) XS correctable ECC error count has exceeded the threshold.");
  }
  if ( ( controlHIGH & MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER7_set(-1) ) != MU_DCR__ECC_COUNT_INTERRUPTS_CONTROL_HIGH__ECC_COUNT_OVER7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(ECC_COUNT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ecc_count_interrupts",
      "ecc_count_over7",
      "(correctable error) RME correctable ECC error count has exceeded the threshold.");
  }
}

void test_imu_ecc_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH));
  uint64_t controlLOW  = DCRReadPriv(MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE0_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue0",
      "(fatal error) XM to MCSRAM write data word0 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE1_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue1",
      "(fatal error) XM to MCSRAM write data word1 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE2_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue2",
      "(fatal error) XM to MCSRAM write data word2 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE3_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue3",
      "(fatal error) XM to MCSRAM write data word3 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE4_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue4",
      "(fatal error) MCSRAM read data word0 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE5_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue5",
      "(fatal error) MCSRAM read data word1 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE6_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue6",
      "(fatal error) MCSRAM read data word2 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE7_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue7",
      "(fatal error) MCSRAM read data word3 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE8_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue8",
      "(fatal error) MCSRAM read data word4 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE9_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue9",
      "(fatal error) MCSRAM read data word5 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE10_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_UE10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ue10",
      "(fatal error) iME-to-imFIFO arbitration logic FIFO map latch ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE0_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce0",
      "(correctable error) XM to MCSRAM write data word0 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE1_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce1",
      "(correctable error) XM to MCSRAM write data word1 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE2_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce2",
      "(correctable error) XM to MCSRAM write data word2 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE3_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce3",
      "(correctable error) XM to MCSRAM write data word3 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE4_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce4",
      "(correctable error) MCSRAM read data word0 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE5_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce5",
      "(correctable error) MCSRAM read data word1 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE6_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce6",
      "(correctable error) MCSRAM read data word2 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE7_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce7",
      "(correctable error) MCSRAM read data word3 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE8_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce8",
      "(correctable error) MCSRAM read data word4 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE9_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce9",
      "(correctable error) MCSRAM read data word5 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE10_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce10",
      "(correctable error) iME-to-imFIFO arbitration logic FIFO map latch ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE11_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce11",
      "(correctable error) ICSRAM start_ecc_ce - correctable ECC error on start field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE12_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce12",
      "(correctable error) ICSRAM size_ecc_ce - correctable ECC error on size field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE13_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce13",
      "(correctable error) ICSRAM head_ecc_ce - correctable ECC error on head field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE14_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce14",
      "(correctable error) ICSRAM tail_ecc_ce - correctable ECC error on tail field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE15_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce15",
      "(correctable error) ICSRAM desc_ecc_ce - correctable ECC error on desc field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE16_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce16",
      "(correctable error) ICSRAM free_ecc_ce - correctable ECC error on free space field, cannot be ignored");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE17_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 28 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce17",
      "(correctable error) ICSRAM xmint_head_ecc_ce - correctable ECC error on head address for descriptor fetch");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE18_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 29 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce18",
      "(correctable error) ICSRAM rdstart_ecc_ce - correctable ECC error on start read for rGET packet");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE19_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 30 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce19",
      "(correctable error) ICSRAM rdsize_ecc_ce - correctable ECC error on size read for rGET packet");
  }
  if ( ( controlHIGH & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE20_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_HIGH__IMU_ECC_CE20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 31 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "imu_ecc_interrupts",
      "imu_ecc_ce20",
      "(correctable error) ICSRAM rdtail_ecc_ce - correctable ECC error on tail read for rGET packet");
  }
  if ( ( controlLOW & MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_LOW__IMU_ECC_CE21_set(-1) ) != MU_DCR__IMU_ECC_INTERRUPTS_CONTROL_LOW__IMU_ECC_CE21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 32 -> %s\n",
      MU_DCR(IMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "imu_ecc_interrupts",
      "imu_ecc_ce21",
      "(correctable error) ICSRAM rdfree_ecc_ce - correctable ECC error on free space read for rGET packet");
  }
}

void test_misc_ecc_corr_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__MMREGS_ECC_CE0_set(-1) ) != MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__MMREGS_ECC_CE0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_ecc_corr_interrupts",
      "mmregs_ecc_ce0",
      "(correctable error) wrdata_ecc_ce - Correctable ECC error on MMIO write data.");
  }
  if ( ( controlHIGH & MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__MMREGS_ECC_CE1_set(-1) ) != MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__MMREGS_ECC_CE1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_ecc_corr_interrupts",
      "mmregs_ecc_ce1",
      "(correctable error) barrier_mu_ecc_ce - Correctable ECC error in global interrupt control register");
  }
  if ( ( controlHIGH & MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__MMREGS_ECC_CE2_set(-1) ) != MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__MMREGS_ECC_CE2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_ecc_corr_interrupts",
      "mmregs_ecc_ce2",
      "(correctable error) barrier_nd_ecc_ce - Correctable ECC error in global interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__XS_ECC_CE0_set(-1) ) != MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__XS_ECC_CE0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_ecc_corr_interrupts",
      "xs_ecc_ce0",
      "(correctable error) info_ecc_ce - Correctable ECC error on info field passed to MU slave by switch");
  }
  if ( ( controlHIGH & MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__XS_ECC_CE1_set(-1) ) != MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__XS_ECC_CE1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_ecc_corr_interrupts",
      "xs_ecc_ce1",
      "(correctable error) data_bvalids0_ecc_ce - Correctable ECC error on first 72b of write data  and  byte valids passed to MU slave by switch");
  }
  if ( ( controlHIGH & MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__XS_ECC_CE2_set(-1) ) != MU_DCR__MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH__XS_ECC_CE2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(MISC_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_ecc_corr_interrupts",
      "xs_ecc_ce2",
      "(correctable error) data_bvalids1_ecc_ce - Correctable ECC error on second 72b of write data  and  byte valids passed to MU slave by switch");
  }
}

void test_rcsram_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(RCSRAM_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR0_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err0",
      "(fatal error) RCSRAM parity error in control flag latches");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR1_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err1",
      "(fatal error) RCSRAM parity error in slave index latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR2_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err2",
      "(fatal error) RCSRAM parity error in slave/get-tail request stage 1/4 latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR3_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err3",
      "(fatal error) RCSRAM parity error in slave/get-tail request stage 2 latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR4_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err4",
      "(fatal error) RCSRAM parity error in slave/get-tail request stage 3 latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR5_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err5",
      "(fatal error) RCSRAM parity error in interrupt index latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR6_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err6",
      "(fatal error) RCSRAM parity error in commit-tail request stage 1/3 latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR7_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err7",
      "(fatal error) RCSRAM parity error in commit-tail request stage 2 latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR8_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err8",
      "(fatal error) RCSRAM parity error in commit-tail request stage 4 latch");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR9_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err9",
      "(fatal error) RCSRAM get tail SRAM write data latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR10_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err10",
      "(fatal error) RCSRAM get tail read data output latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR11_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err11",
      "(fatal error) RCSRAM state encoding error");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR12_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR12_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err12",
      "(software error) RCSRAM user packet getting tail of system rmFIFO");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR13_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR13_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err13",
      "(software error) RCSRAM get-tail request index out of bound (>= 272)");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR14_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR14_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err14",
      "(software error) RCSRAM requested rmFIFO not enabled by DCR register");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR15_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR15_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err15",
      "(software error) RCSRAM write data from slave not aligned to 64B or greater than 2**36-1");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR16_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err16",
      "(non-fatal error) RCSRAM get-tail request arbiter random bit latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR17_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err17",
      "(non-fatal error) RCSRAM commit-tail request arbiter random bit latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR18_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err18",
      "(non-fatal error) RCSRAM backdoor tail ID read data latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR19_set(-1) ) != MU_DCR__RCSRAM_INTERRUPTS_CONTROL_HIGH__RCSRAM_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(RCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rcsram_interrupts",
      "rcsram_err19",
      "(non-fatal error) RCSRAM backdoor ECC read data latch parity error");
  }
}

void test_mmregs_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(MMREGS_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR0_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err0",
      "(fatal error) wrdata_ecc_err - uncorrectable ECC error on incoming write data");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR1_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err1",
      "(fatal error) imfifo_enable_status_p_err - parity error on imFIFO enabled registers");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR2_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err2",
      "(fatal error) imfifo_thold_status_p_err - parity error on imFIFO threshold crossing interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR3_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err3",
      "(fatal error) rmfifo_thold_status_p_err - parity error on rmFIFO threshold crossing interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR4_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err4",
      "(fatal error) rmfifo_pktint_status_p_err - parity error on rmFIFO packet arrival interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR5_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err5",
      "(fatal error) info_p_err - parity error on info register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR6_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err6",
      "(fatal error) rd_barriers_latched_p_errn - parity error on latched barrier register read data (either barrier status or control)");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR7_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err7",
      "(fatal error) barrier_mu_ecc_err - uncorrectable ECC error on global interrupt control register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR8_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_FATAL_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_fatal_err8",
      "(fatal error) barrier_nd_ecc_err - uncorrectable ECC error on global interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR9_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR9_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err9",
      "(software error) rd_imfifo_enable_err - software attempted to read the write-only enable imFIFO register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR10_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR10_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err10",
      "(software error) rd_imfifo_disable_err - software attempted to read the write-only disable imFIFO register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR11_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR11_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err11",
      "(software error) rd_fifo_int_clr_err - software attempted to read the write-only clear interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR12_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR12_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err12",
      "(software error) wr_imfifo_enable_status_err - software attempted to write the read-only imFIFO enable status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR13_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR13_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err13",
      "(software error) wr_fifo_int_status_err - software attempted to write the read-only interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR14_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR14_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err14",
      "(software error) imfifo_enable_prot_err - user attempted to enable or disable a system imFIFO");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR15_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR15_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err15",
      "(software error) fifo_int_clr_prot_err - user attempted to clear an interrupt corresponding to a system imFIFO or rmFIFO");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR16_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR16_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err16",
      "(software error) wr_range_err - software attempted to write (set) unused MMIO bits, either bits 0-31 in imFIFO enable/disable/high-priority, or bits 0-60 in global interrupt control");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR17_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR17_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err17",
      "(software error) wr_barriers_nd_err - software attempted to write the read-only global interrupt status register");
  }
  if ( ( controlHIGH & MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR18_set(-1) ) != MU_DCR__MMREGS_INTERRUPTS_CONTROL_HIGH__MMREGS_SOFTWARE_ERR18_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(MMREGS_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mmregs_interrupts",
      "mmregs_software_err18",
      "(software error) barrier_mu_prot_err - user attempted to write a system global interrupt control bit;");
  }
}

void test_rmu_ecc_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH));
  uint64_t controlLOW  = DCRReadPriv(MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE0_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue0",
      "(fatal error) XS--RCSRAM write data ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE1_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue1",
      "(fatal error) RCSRAM read data word0 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE2_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue2",
      "(fatal error) RCSRAM read data word1 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE3_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue3",
      "(fatal error) RCSRAM read data word2 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE4_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue4",
      "(fatal error) RCSRAM read data word3 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE5_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue5",
      "(fatal error) RCSRAM read data word4 ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE6_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue6",
      "(fatal error) RPUT SRAM read data ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE7_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue7",
      "(fatal error) RPUT SRAM write data ECC uncorrectable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE8_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue8",
      "(fatal error) rME0 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE9_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue9",
      "(fatal error) rME0 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE10_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue10",
      "(fatal error) rME1 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE11_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue11",
      "(fatal error) rME1 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE12_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue12",
      "(fatal error) rME2 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE13_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue13",
      "(fatal error) rME2 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE14_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue14",
      "(fatal error) rME3 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE15_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue15",
      "(fatal error) rME3 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE16_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue16",
      "(fatal error) rME4 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE17_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue17",
      "(fatal error) rME4 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE18_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue18",
      "(fatal error) rME5 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE19_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue19",
      "(fatal error) rME5 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE20_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue20",
      "(fatal error) rME6 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE21_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue21",
      "(fatal error) rME6 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE22_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE22_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue22",
      "(fatal error) rME7 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE23_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE23_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue23",
      "(fatal error) rME7 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE24_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE24_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue24",
      "(fatal error) rME8 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE25_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE25_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue25",
      "(fatal error) rME8 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE26_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE26_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue26",
      "(fatal error) rME9 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE27_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE27_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue27",
      "(fatal error) rME9 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE28_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE28_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 28 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue28",
      "(fatal error) rME10 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE29_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE29_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 29 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue29",
      "(fatal error) rME10 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE30_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE30_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 30 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue30",
      "(fatal error) rME11 network data latch uncorrectable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE31_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_HIGH__RMU_ECC_UE31_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 31 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue31",
      "(fatal error) rME11 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE32_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE32_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 32 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue32",
      "(fatal error) rME12 network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE33_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE33_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 33 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue33",
      "(fatal error) rME12 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE34_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE34_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 34 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue34",
      "(fatal error) rME13 network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE35_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE35_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 35 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue35",
      "(fatal error) rME13 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE36_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE36_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 36 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue36",
      "(fatal error) rME14 network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE37_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE37_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 37 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue37",
      "(fatal error) rME14 previous 4 bytes network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE38_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE38_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 38 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue38",
      "(fatal error) rME15 network data latch uncorrectable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE39_set(-1) ) != MU_DCR__RMU_ECC_INTERRUPTS_CONTROL_LOW__RMU_ECC_UE39_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 39 -> %s\n",
      MU_DCR(RMU_ECC_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_interrupts",
      "rmu_ecc_ue39",
      "(fatal error) rME15 previous 4 bytes network data latch uncorrectable ECC error");
  }
}

void test_mcsram_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(MCSRAM_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR0_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err0",
      "(fatal error) Control flag latches parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR1_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err1",
      "(fatal error) XM--MCSRAM 1st cycle write data latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR2_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err2",
      "(fatal error) XM--MCSRAM 2nd cycle write data latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR3_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err3",
      "(fatal error) XM--MCSRAM write index latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR4_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err4",
      "(fatal error) SRAM write index latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR5_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err5",
      "(fatal error) Descriptor output latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR6_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err6",
      "(fatal error) Header output buffer iME ID latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR7_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err7",
      "(fatal error) iME request stage 1 latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR8_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err8",
      "(fatal error) iME request stage 2 latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR9_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err9",
      "(fatal error) iME request stage 3 latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR10_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err10",
      "(fatal error) MCSRAM to arbitration logic ready flag and FIFO map set request latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR11_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err11",
      "(fatal error) ICSRAM increment index latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR12_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err12",
      "(fatal error) Backdoor command latch parity error ");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR13_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err13",
      "(fatal error) 2nd XM data return missing");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR14_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err14",
      "(fatal error) Header buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR15_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR15_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err15",
      "(software error) Remote get packet payload address is atomic");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR16_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR16_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err16",
      "(software error) Payload size gt 512B");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR17_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR17_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err17",
      "(software error) Payload size = 0 for R-put or payload size lt 64B for R-get");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR18_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR18_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err18",
      "(software error) Payload size != 64N for R-get");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR19_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR19_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err19",
      "(software error) Payload size = 0 for non-zero message length");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR20_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR20_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err20",
      "(software error) Payload address starts in non-atomic area but ends in atomic area");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR21_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR21_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err21",
      "(software error) Message len != 64N for R-get");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR22_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR22_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err22",
      "(software error) Message len != 8 for atomic payload address");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR23_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR23_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err23",
      "(software error) Message len = 0 for R-put, or message len lt 64B for R-get");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR24_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR24_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err24",
      "(software error) User imFIFO sending system VC packet (VC = 3,6)");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR25_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR25_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err25",
      "(software error) Invalid MU packet type (type = '11')");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR26_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR26_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err26",
      "(software error) User imFIFO specifying system iME in FIFO map");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR27_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR27_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err27",
      "(software error) FIFO map is 0 (i.e. specifying no iME)");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR28_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR28_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 28 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err28",
      "(non-fatal error) Random bit latch parity error in MCSRAM descriptor read request arbitor");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR29_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR29_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 29 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err29",
      "(non-fatal error) Random bit latch parity error in MCSRAM packet end request arbitor");
  }
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR30_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_ERR30_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 30 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_err30",
      "(non-fatal error) Random bit latch parity error in MCSRAM message end request arbitor");
  }

#if 0
  if ( ( controlHIGH & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_DD2_ERR0_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_HIGH__MCSRAM_DD2_ERR0_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 31 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "mcsram_interrupts",
      "mcsram_dd2_err0",
      "(software error DD2 only) Invalid VC (VC=7 is undefined)");
  }
  if ( ( controlLOW & MU_DCR__MCSRAM_INTERRUPTS_CONTROL_LOW__MCSRAM_DD2_ERR1_set(-1) ) != MU_DCR__MCSRAM_INTERRUPTS_CONTROL_LOW__MCSRAM_DD2_ERR1_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 32 -> %s\n",
      MU_DCR(MCSRAM_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "mcsram_interrupts",
      "mcsram_dd2_err1",
      "(software error DD2 only) Put offset wrapped (put offset + msg len overflows 36bit limit)");
  }
#endif
}

void test_ime_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(IME_INTERRUPTS_CONTROL_HIGH));
  uint64_t controlLOW  = DCRReadPriv(MU_DCR(IME_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR0_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err0",
      "(fatal error) iME--XM request valid flag latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR1_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err1",
      "(fatal error) XM--iME return address latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR2_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err2",
      "(fatal error) XM--iME return data valid flag latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR3_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err3",
      "(fatal error) iME0 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR4_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err4",
      "(fatal error) iME0 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR5_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err5",
      "(fatal error) iME1 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR6_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err6",
      "(fatal error) iME1 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR7_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err7",
      "(fatal error) iME2 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR8_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err8",
      "(fatal error) iME2 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR9_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err9",
      "(fatal error) iME3 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR10_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err10",
      "(fatal error) iME3 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR11_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err11",
      "(fatal error) iME4 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR12_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err12",
      "(fatal error) iME4 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR13_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err13",
      "(fatal error) iME5 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR14_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err14",
      "(fatal error) iME5 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR15_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err15",
      "(fatal error) iME6 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR16_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err16",
      "(fatal error) iME6 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR17_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err17",
      "(fatal error) iME7 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR18_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err18",
      "(fatal error) iME7 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR19_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err19",
      "(fatal error) iME8 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR20_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err20",
      "(fatal error) iME8 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR21_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err21",
      "(fatal error) iME9 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR22_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR22_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err22",
      "(fatal error) iME9 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR23_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR23_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err23",
      "(fatal error) iME10 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR24_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR24_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err24",
      "(fatal error) iME10 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR25_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR25_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err25",
      "(fatal error) iME11 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR26_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR26_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err26",
      "(fatal error) iME11 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR27_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR27_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err27",
      "(fatal error) iME12 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR28_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR28_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 28 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err28",
      "(fatal error) iME12 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR29_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR29_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 29 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err29",
      "(fatal error) iME13 parity error");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR30_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR30_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 30 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err30",
      "(fatal error) iME13 invalid state entered");
  }
  if ( ( controlHIGH & MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR31_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_HIGH__IME_ERR31_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 31 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "ime_interrupts",
      "ime_err31",
      "(fatal error) iME14 parity error");
  }
  if ( ( controlLOW & MU_DCR__IME_INTERRUPTS_CONTROL_LOW__IME_ERR32_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_LOW__IME_ERR32_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 32 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "ime_interrupts",
      "ime_err32",
      "(fatal error) iME14 invalid state entered");
  }
  if ( ( controlLOW & MU_DCR__IME_INTERRUPTS_CONTROL_LOW__IME_ERR33_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_LOW__IME_ERR33_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 33 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "ime_interrupts",
      "ime_err33",
      "(fatal error) iME15 parity error");
  }
  if ( ( controlLOW & MU_DCR__IME_INTERRUPTS_CONTROL_LOW__IME_ERR34_set(-1) ) != MU_DCR__IME_INTERRUPTS_CONTROL_LOW__IME_ERR34_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 34 -> %s\n",
      MU_DCR(IME_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "ime_interrupts",
      "ime_err34",
      "(fatal error) iME15 invalid state entered");
  }
}

void test_rme_interrupts1() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH));
  uint64_t controlLOW  = DCRReadPriv(MU_DCR(RME_INTERRUPTS1_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR49_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR49_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err49",
      "(fatal error) rME8 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR50_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR50_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err50",
      "(fatal error) rME8 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR51_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR51_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err51",
      "(fatal error) rME8 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR52_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR52_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err52",
      "(fatal error) rME8 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR53_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR53_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err53",
      "(software error) rME8 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR54_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR54_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err54",
      "(non-fatal error) rME8 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR55_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR55_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err55",
      "(fatal error) rME9 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR56_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR56_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err56",
      "(fatal error) rME9 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR57_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR57_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err57",
      "(fatal error) rME9 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR58_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR58_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err58",
      "(fatal error) rME9 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR59_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR59_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err59",
      "(software error) rME9 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR60_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR60_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err60",
      "(non-fatal error) rME9 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR61_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR61_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err61",
      "(fatal error) rME10 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR62_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR62_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err62",
      "(fatal error) rME10 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR63_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR63_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err63",
      "(fatal error) rME10 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR64_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR64_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err64",
      "(fatal error) rME10 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR65_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR65_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err65",
      "(software error) rME10 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR66_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR66_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err66",
      "(non-fatal error) rME10 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR67_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR67_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err67",
      "(fatal error) rME11 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR68_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR68_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err68",
      "(fatal error) rME11 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR69_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR69_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err69",
      "(fatal error) rME11 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR70_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR70_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err70",
      "(fatal error) rME11 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR71_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR71_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err71",
      "(software error) rME11 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR72_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR72_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err72",
      "(non-fatal error) rME11 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR73_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR73_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err73",
      "(fatal error) rME12 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR74_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR74_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err74",
      "(fatal error) rME12 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR75_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR75_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err75",
      "(fatal error) rME12 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR76_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR76_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err76",
      "(fatal error) rME12 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR77_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR77_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 28 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err77",
      "(software error) rME12 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR78_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR78_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 29 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err78",
      "(non-fatal error) rME12 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR79_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR79_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 30 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err79",
      "(fatal error) rME13 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR80_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_HIGH__RME_ERR80_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 31 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts1",
      "rme_err80",
      "(fatal error) rME13 invalid state has been reached");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR81_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR81_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 32 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err81",
      "(fatal error) rME13 alignment buffer overflow");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR82_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR82_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 33 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err82",
      "(fatal error) rME13 network reception FIFO read timeout");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR83_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR83_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 34 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err83",
      "(software error) rME13 packet header field error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR84_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR84_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 35 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err84",
      "(non-fatal error) rME13 parity error in FIFO read timeout count");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR85_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR85_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 36 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err85",
      "(fatal error) rME14 parity error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR86_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR86_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 37 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err86",
      "(fatal error) rME14 invalid state has been reached");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR87_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR87_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 38 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err87",
      "(fatal error) rME14 alignment buffer overflow");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR88_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR88_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 39 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err88",
      "(fatal error) rME14 network reception FIFO read timeout");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR89_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR89_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 40 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err89",
      "(software error) rME14 packet header field error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR90_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR90_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 41 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err90",
      "(non-fatal error) rME14 parity error in FIFO read timeout count");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR91_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR91_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 42 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err91",
      "(fatal error) rME15 parity error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR92_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR92_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 43 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err92",
      "(fatal error) rME15 invalid state has been reached");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR93_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR93_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 44 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err93",
      "(fatal error) rME15 alignment buffer overflow");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR94_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR94_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 45 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err94",
      "(fatal error) rME15 network reception FIFO read timeout");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR95_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR95_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 46 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err95",
      "(software error) rME15 packet header field error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR96_set(-1) ) != MU_DCR__RME_INTERRUPTS1_CONTROL_LOW__RME_ERR96_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 47 -> %s\n",
      MU_DCR(RME_INTERRUPTS1_CONTROL_LOW),
      controlLOW,
      "rme_interrupts1",
      "rme_err96",
      "(non-fatal error) rME15 parity error in FIFO read timeout count");
  }
}

void test_rme_interrupts0() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH));
  uint64_t controlLOW  = DCRReadPriv(MU_DCR(RME_INTERRUPTS0_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR0_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err0",
      "(fatal error) rME--xm request valid flag latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR1_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err1",
      "(fatal error) rME0 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR2_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err2",
      "(fatal error) rME0 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR3_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err3",
      "(fatal error) rME0 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR4_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err4",
      "(fatal error) rME0 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR5_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR5_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err5",
      "(software error) rME0 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR6_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err6",
      "(non-fatal error) rME0 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR7_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err7",
      "(fatal error) rME1 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR8_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err8",
      "(fatal error) rME1 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR9_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err9",
      "(fatal error) rME1 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR10_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err10",
      "(fatal error) rME1 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR11_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR11_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err11",
      "(software error) rME1 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR12_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err12",
      "(non-fatal error) rME1 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR13_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err13",
      "(fatal error) rME2 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR14_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err14",
      "(fatal error) rME2 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR15_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err15",
      "(fatal error) rME2 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR16_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err16",
      "(fatal error) rME2 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR17_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR17_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err17",
      "(software error) rME2 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR18_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err18",
      "(non-fatal error) rME2 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR19_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err19",
      "(fatal error) rME3 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR20_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err20",
      "(fatal error) rME3 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR21_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err21",
      "(fatal error) rME3 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR22_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR22_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err22",
      "(fatal error) rME3 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR23_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR23_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err23",
      "(software error) rME3 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR24_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR24_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err24",
      "(non-fatal error) rME3 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR25_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR25_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err25",
      "(fatal error) rME4 parity error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR26_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR26_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err26",
      "(fatal error) rME4 invalid state has been reached");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR27_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR27_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err27",
      "(fatal error) rME4 alignment buffer overflow");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR28_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR28_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 28 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err28",
      "(fatal error) rME4 network reception FIFO read timeout");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR29_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR29_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 29 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err29",
      "(software error) rME4 packet header field error");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR30_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR30_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 30 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err30",
      "(non-fatal error) rME4 parity error in FIFO read timeout count");
  }
  if ( ( controlHIGH & MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR31_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_HIGH__RME_ERR31_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 31 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_HIGH),
      controlHIGH,
      "rme_interrupts0",
      "rme_err31",
      "(fatal error) rME5 parity error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR32_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR32_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 32 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err32",
      "(fatal error) rME5 invalid state has been reached");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR33_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR33_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 33 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err33",
      "(fatal error) rME5 alignment buffer overflow");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR34_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR34_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 34 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err34",
      "(fatal error) rME5 network reception FIFO read timeout");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR35_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR35_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 35 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err35",
      "(software error) rME5 packet header field error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR36_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR36_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 36 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err36",
      "(non-fatal error) rME5 parity error in FIFO read timeout count");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR37_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR37_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 37 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err37",
      "(fatal error) rME6 parity error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR38_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR38_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 38 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err38",
      "(fatal error) rME6 invalid state has been reached");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR39_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR39_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 39 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err39",
      "(fatal error) rME6 alignment buffer overflow");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR40_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR40_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 40 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err40",
      "(fatal error) rME6 network reception FIFO read timeout");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR41_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR41_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 41 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err41",
      "(software error) rME6 packet header field error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR42_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR42_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 42 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err42",
      "(non-fatal error) rME6 parity error in FIFO read timeout count");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR43_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR43_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 43 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err43",
      "(fatal error) rME7 parity error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR44_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR44_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 44 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err44",
      "(fatal error) rME7 invalid state has been reached");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR45_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR45_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 45 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err45",
      "(fatal error) rME7 alignment buffer overflow");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR46_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR46_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 46 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err46",
      "(fatal error) rME7 network reception FIFO read timeout");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR47_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR47_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 47 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err47",
      "(software error) rME7 packet header field error");
  }
  if ( ( controlLOW & MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR48_set(-1) ) != MU_DCR__RME_INTERRUPTS0_CONTROL_LOW__RME_ERR48_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 48 -> %s\n",
      MU_DCR(RME_INTERRUPTS0_CONTROL_LOW),
      controlLOW,
      "rme_interrupts0",
      "rme_err48",
      "(non-fatal error) rME7 parity error in FIFO read timeout count");
  }
}

void test_icsram_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(ICSRAM_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR0_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err0",
      "(fatal error) start_ecc_err - uncorrectable ECC error on start field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR1_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err1",
      "(fatal error) size_ecc_err - uncorrectable ECC error on size field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR2_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err2",
      "(fatal error) head_ecc_err - uncorrectable ECC error on head field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR3_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err3",
      "(fatal error) tail_ecc_err - uncorrectable ECC error on tail field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR4_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err4",
      "(fatal error) desc_ecc_err - uncorrectable ECC error on desc field, can be ignored until field is initialized");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR5_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err5",
      "(fatal error) free_ecc_err - uncorrectable ECC error on free space field, cannot be ignored");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR6_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err6",
      "(fatal error) fsm_p_err - parity error on FSM state registers");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR7_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err7",
      "(fatal error) imfifo_not_empty_p_err - parity error on register that tracks if imFIFO has descriptor(s) to fetch");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR8_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err8",
      "(fatal error) imfifo_cand_p_err - parity error on imFIFO candidate ID chosen for descriptor fetch");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR9_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err9",
      "(fatal error) fetching_desc_p_err - parity error on register that tracks if imFIFO is currently fetching descriptor");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR10_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err10",
      "(fatal error) xmreq_fifo_ptr_p_err - parity error on req fifo pointers that issue descriptor requests to master port");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR11_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err11",
      "(fatal error) xmint_head_ecc_err - uncorrectable ECC error on head address for descriptor fetch");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR12_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err12",
      "(fatal error) icsram_me_valid_p_err - parity error on valid bit telling rME that read of head/tail/size is done");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR13_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err13",
      "(fatal error) rmerd_attr_p_err - parity error on head/tail/size read by rME");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR14_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err14",
      "(fatal error) icsram_din_l2_p_err - parity error on icsram DIN latch containing start, size, etc");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR15_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err15",
      "(fatal error) rdstart_ecc_err - uncorrectable ECC error on start read for rGET packet");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR16_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err16",
      "(fatal error) rdsize_ecc_err - uncorrectable ECC error on size read for rGET packet");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR17_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err17",
      "(fatal error) rdtail_ecc_err - uncorrectable ECC error on tail read for rGET packet");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR18_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err18",
      "(fatal error) rdfree_ecc_err - uncorrectable ECC error on free space read for rGET packet");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR19_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err19",
      "(correctable error) imfifo_cand_p_err_1 - correctable parity error on intermediate register storing imFIFO candidate for descriptor fetch. Corrected by hardware re-arbitration of imFIFO candidate.");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR20_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR20_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err20",
      "(software error) xs_wr_free_err - software attempted to write read-only free space");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR21_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR21_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err21",
      "(software error) startsizeheadtail_align_atomic_err - start, head, or tail address are not 64B aligned, or size is not 64B-1 aligned, or atomic address was specified");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR22_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR22_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err22",
      "(software error) startsizeheadtail_range_err - start, head, tail or size are out of range (bits(0 to 26) are non-zero)");
  }
  if ( ( controlHIGH & MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR23_set(-1) ) != MU_DCR__ICSRAM_INTERRUPTS_CONTROL_HIGH__ICSRAM_ERR23_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(ICSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "icsram_interrupts",
      "icsram_err23",
      "(software notification) rget_fifo_full_err - rget FIFO is full, rME is waiting for software to free up space in fifo. See associated info register for rget FIFO ID.");
  }
}

void test_rputsram_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR0_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err0",
      "(fatal error) RPUT SRAM control flag latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR1_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err1",
      "(fatal error) RPUT SRAM state encoding error");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR2_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err2",
      "(fatal error) RPUT SRAM stage 1 request latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR3_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err3",
      "(non-fatal error) RPUT SRAM stage 2 request latch parity error (recovered by retry)");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR4_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err4",
      "(fatal error) RPUT SRAM stage 3 request latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR5_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err5",
      "(fatal error) RPUT SRAM stage 4 request latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR6_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err6",
      "(non-fatal error) RPUT SRAM backdoor ECC read data latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR7_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err7",
      "(non-fatal error) RPUT SRAM rME request arbitor random bit latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR8_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR8_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err8",
      "(software error) RPUT SRAM user packet accessing put base but it is system entry");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR9_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR9_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err9",
      "(software error) RPUT SRAM user packet accessing counter base but it is system entry");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR10_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR10_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err10",
      "(software error) RPUT SRAM rput/counter address addition overflow");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR11_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR11_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err11",
      "(software error) RPUT SRAM rput destination area crossing atomic/nonatomic boundary");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR12_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR12_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err12",
      "(software error) RPUT SRAM data size != 8 for atomic rput address");
  }
  if ( ( controlHIGH & MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR13_set(-1) ) != MU_DCR__RPUTSRAM_INTERRUPTS_CONTROL_HIGH__RPUTSRAM_ERR13_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(RPUTSRAM_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rputsram_interrupts",
      "rputsram_err13",
      "(software error) RPUT SRAM rput counter address in non-atomic");
  }
}

void test_slave_port_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR0_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err0",
      "(fatal error) info_ecc_err - uncorrectable ECC error on switch info field OR single bit error on ttype(0) bit");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR1_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err1",
      "(fatal error) data_bvalids_ecc_err - uncorrectable ECC error on switch data  and  byte valid fields");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR2_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err2",
      "(fatal error) req_valid_src_p_err - parity error on src field");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR3_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err3",
      "(fatal error) src_id_err - src field is >= 18, which means a non-L1p master port is trying to access MU slave");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR4_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err4",
      "(fatal error) req_info_chop_p_err - parity error on a subset of the request info bits");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR5_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err5",
      "(fatal error) dgate_valid_err - req_valid asserted by switch, but dgate not asserted so info/data fields not latched");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR6_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err6",
      "(fatal error) req_ack_err - more than 1 MU subunit ack asserted simultaneously");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR7_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err7",
      "(fatal error) req_fifo_ptr_p_err - parity error on request FIFO pointers");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR8_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err8",
      "(fatal error) req_fifo_full_err - request FIFO is full, should not happen b/c switch tracks MU slave tokens");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR9_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err9",
      "(fatal error) req_fifo_attr_p_err - parity error in request FIFO attributes");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR10_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err10",
      "(fatal error) req_info_p_err - parity error in request info from request buffer");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR11_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err11",
      "(fatal error) sked_eager_err - switch indicated eager schedule to MU slave for read return, but MU slave does not support this");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR12_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err12",
      "(fatal error) sked_p_err - parity error in sked_id on slave read data return side");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR13_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err13",
      "(fatal error) sked_id_err - sked_id is >= 18, which indicates a read return to a non-L1p master port");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR14_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err14",
      "(fatal error) avail_idx_p_err - parity error covering the avail_idx vector");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR15_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err15",
      "(fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR16_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err16",
      "(fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR17_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err17",
      "(fatal error) fifo_dest_p_err - parity error in current request's destination field stored in read return request FIFO");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR18_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err18",
      "(fatal error) lut_valid_p_err - parity error on lut_valid vector");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR19_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err19",
      "(fatal error) lut_attr_p_err - parity error in look-up-table attributes for current read return request going out to switch");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR20_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_FATAL_ERR20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_fatal_err20",
      "(fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR21_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR21_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_software_err21",
      "(software error) bad_op_err - unsupported opcode (ttype) sent to MU slave, see slave error info register");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR22_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR22_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_software_err22",
      "(software error) rd_size_err - read request size is not 8B, see slave error info register");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR23_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR23_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_software_err23",
      "(software error) byte_valid_err - more than 8B of byte valids asserted OR byte valids inconsistent with odd/even address");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR24_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR24_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_software_err24",
      "(software error) addr_range_err - request's address lies outside MU MMIO address space, see slave error info register");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR25_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR25_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_software_err25",
      "(software error) icsram_perm_err - user is attempting to access a system imFIFO");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR26_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR26_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_software_err26",
      "(software error) rcsram_perm_err - user is attempting to access a system rmFIFO");
  }
  if ( ( controlHIGH & MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR27_set(-1) ) != MU_DCR__SLAVE_PORT_INTERRUPTS_CONTROL_HIGH__XS_SOFTWARE_ERR27_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(SLAVE_PORT_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "slave_port_interrupts",
      "xs_software_err27",
      "(software error) rputsram_perm_err - user is attempting to access a system rput base address");
  }
}

void test_rmu_ecc_corr_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH));
  uint64_t controlLOW  = DCRReadPriv(MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE0_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce0",
      "(correctable error) XS--RCSRAM write data ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE1_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce1",
      "(correctable error) RCSRAM read data word0 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE2_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce2",
      "(correctable error) RCSRAM read data word1 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE3_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE3_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce3",
      "(correctable error) RCSRAM read data word2 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE4_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE4_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce4",
      "(correctable error) RCSRAM read data word3 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE5_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE5_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce5",
      "(correctable error) RCSRAM read data word4 ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE6_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE6_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce6",
      "(correctable error) RPUT SRAM read data ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE7_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE7_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce7",
      "(correctable error) RPUT SRAM write data ECC correctable error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE8_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE8_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce8",
      "(correctable error) rME0 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE9_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE9_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce9",
      "(correctable error) rME0 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE10_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE10_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 10 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce10",
      "(correctable error) rME1 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE11_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE11_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 11 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce11",
      "(correctable error) rME1 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE12_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE12_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 12 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce12",
      "(correctable error) rME2 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE13_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE13_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 13 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce13",
      "(correctable error) rME2 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE14_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE14_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 14 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce14",
      "(correctable error) rME3 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE15_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE15_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 15 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce15",
      "(correctable error) rME3 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE16_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE16_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 16 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce16",
      "(correctable error) rME4 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE17_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE17_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 17 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce17",
      "(correctable error) rME4 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE18_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE18_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 18 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce18",
      "(correctable error) rME5 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE19_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE19_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 19 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce19",
      "(correctable error) rME5 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE20_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE20_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 20 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce20",
      "(correctable error) rME6 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE21_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE21_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 21 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce21",
      "(correctable error) rME6 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE22_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE22_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 22 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce22",
      "(correctable error) rME7 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE23_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE23_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 23 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce23",
      "(correctable error) rME7 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE24_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE24_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 24 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce24",
      "(correctable error) rME8 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE25_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE25_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 25 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce25",
      "(correctable error) rME8 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE26_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE26_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 26 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce26",
      "(correctable error) rME9 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE27_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE27_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 27 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce27",
      "(correctable error) rME9 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE28_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE28_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 28 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce28",
      "(correctable error) rME10 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE29_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE29_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 29 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce29",
      "(correctable error) rME10 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE30_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE30_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 30 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce30",
      "(correctable error) rME11 network data latch correctable ECC error");
  }
  if ( ( controlHIGH & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE31_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH__RMU_ECC_CE31_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 31 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce31",
      "(correctable error) rME11 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE32_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE32_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 32 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce32",
      "(correctable error) rME12 network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE33_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE33_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 33 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce33",
      "(correctable error) rME12 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE34_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE34_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 34 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce34",
      "(correctable error) rME13 network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE35_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE35_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 35 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce35",
      "(correctable error) rME13 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE36_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE36_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 36 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce36",
      "(correctable error) rME14 network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE37_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE37_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 37 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce37",
      "(correctable error) rME14 previous 4 bytes network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE38_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE38_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 38 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce38",
      "(correctable error) rME15 network data latch correctable ECC error");
  }
  if ( ( controlLOW & MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE39_set(-1) ) != MU_DCR__RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW__RMU_ECC_CE39_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 39 -> %s\n",
      MU_DCR(RMU_ECC_CORR_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "rmu_ecc_corr_interrupts",
      "rmu_ecc_ce39",
      "(correctable error) rME15 previous 4 bytes network data latch correctable ECC error");
  }
}

void test_misc_interrupts() {

  uint64_t controlHIGH = DCRReadPriv(MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH));
  //uint64_t controlLOW  = DCRReadPriv(MU_DCR(MISC_INTERRUPTS_CONTROL_LOW));

  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR0_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 0 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "icsram_select_err0",
      "(fatal error) rME-to-ICSRAM arbiter request valid flag latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR1_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 1 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "icsram_select_err1",
      "(fatal error) rME-to-ICSRAM arbiter request info latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR2_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR2_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 2 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "icsram_select_err2",
      "(non-fatal error) rME-to-ICSRAM arbiter random bit latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR3_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR3_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 3 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "icsram_select_err3",
      "(software error) rME requested system imFIFO for user R-get packet");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR4_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR4_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 4 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "icsram_select_err4",
      "(software error) rME requested imFIFO whose rget flag is not set");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR5_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__ICSRAM_SELECT_ERR5_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 5 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "icsram_select_err5",
      "(software error) rME requested imFIFO not enabled by DCR");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__RAN_ERR0_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__RAN_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 6 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "ran_err0",
      "(non-fatal error) Random bit generator bit error");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__MU_ERR0_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__MU_ERR0_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 7 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "mu_err0",
      "(fatal error) redun_ctrl signal latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__MU_ERR1_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__MU_ERR1_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 8 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "mu_err1",
      "(fatal error) slice_sel_ctrl signal latch parity error");
  }
  if ( ( controlHIGH & MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__UPC_COUNTER_OVF_set(-1) ) != MU_DCR__MISC_INTERRUPTS_CONTROL_HIGH__UPC_COUNTER_OVF_set(1) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 9 -> %s\n",
      MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH),
      controlHIGH,
      "misc_interrupts",
      "upc_counter_ovf",
      "(non-fatal error) UPC counter overflowed, indicating a soft error in counter or a problem with UPC ring");
  }
}

void test_fifo_interrupts() {

  //uint64_t controlHIGH = DCRReadPriv(MU_DCR(FIFO_INTERRUPTS_CONTROL_HIGH));
  uint64_t controlLOW  = DCRReadPriv(MU_DCR(FIFO_INTERRUPTS_CONTROL_LOW));

  if ( ( controlLOW & MU_DCR__FIFO_INTERRUPTS_CONTROL_LOW__RMFIFO_INSUFFICIENT_SPACE_set(-1) ) != MU_DCR__FIFO_INTERRUPTS_CONTROL_LOW__RMFIFO_INSUFFICIENT_SPACE_set(3) ) {
    printf(
      "(E) (%X:%016lX) %s:%s bit 63 -> %s\n",
      MU_DCR(FIFO_INTERRUPTS_CONTROL_LOW),
      controlLOW,
      "fifo_interrupts",
      "rmfifo_insufficient_space",
      "(software notification) There is insufficient space in an rmFIFO to receive the next packet.");
  }
}

/*
void test_master_port0_interrupts(void);
void test_master_port1_interrupts(void);
void test_arlog_interrupts(void);
void test_master_port2_interrupts(void);
void test_ecc_count_interrupts(void);
void test_imu_ecc_interrupts(void);
void test_misc_ecc_corr_interrupts(void);
void test_rcsram_interrupts(void);
void test_mmregs_interrupts(void);
void test_rmu_ecc_interrupts(void);
void test_mcsram_interrupts(void);
void test_ime_interrupts(void);
void test_rme_interrupts1(void);
void test_rme_interrupts0(void);
void test_icsram_interrupts(void);
void test_rputsram_interrupts(void);
void test_slave_port_interrupts(void);
void test_rmu_ecc_corr_interrupts(void);
void test_misc_interrupts(void);
void test_fifo_interrupts(void);
*/



void test_DCR_address_assumptions() {
  if (MU_DCR(MASTER_PORT0_INTERRUPTS__STATE) != (MU_DCR(MASTER_PORT0_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(MASTER_PORT0_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(MASTER_PORT1_INTERRUPTS__STATE) != (MU_DCR(MASTER_PORT1_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(MASTER_PORT1_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(ARLOG_INTERRUPTS__STATE) != (MU_DCR(ARLOG_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(ARLOG_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(MASTER_PORT2_INTERRUPTS__STATE) != (MU_DCR(MASTER_PORT2_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(MASTER_PORT2_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(ECC_COUNT_INTERRUPTS__STATE) != (MU_DCR(ECC_COUNT_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(ECC_COUNT_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(IMU_ECC_INTERRUPTS__STATE) != (MU_DCR(IMU_ECC_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(IMU_ECC_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(MISC_ECC_CORR_INTERRUPTS__STATE) != (MU_DCR(MISC_ECC_CORR_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(MISC_ECC_CORR_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(RCSRAM_INTERRUPTS__STATE) != (MU_DCR(RCSRAM_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(RCSRAM_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(MMREGS_INTERRUPTS__STATE) != (MU_DCR(MMREGS_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(MMREGS_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(RMU_ECC_INTERRUPTS__STATE) != (MU_DCR(RMU_ECC_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(RMU_ECC_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(MCSRAM_INTERRUPTS__STATE) != (MU_DCR(MCSRAM_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(MCSRAM_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(IME_INTERRUPTS__STATE) != (MU_DCR(IME_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(IME_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(RME_INTERRUPTS1__STATE) != (MU_DCR(RME_INTERRUPTS1__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(RME_INTERRUPTS1__STATE)\n\n");
   }
  if (MU_DCR(RME_INTERRUPTS0__STATE) != (MU_DCR(RME_INTERRUPTS0__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(RME_INTERRUPTS0__STATE)\n\n");
   }
  if (MU_DCR(ICSRAM_INTERRUPTS__STATE) != (MU_DCR(ICSRAM_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(ICSRAM_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(RPUTSRAM_INTERRUPTS__STATE) != (MU_DCR(RPUTSRAM_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(RPUTSRAM_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(SLAVE_PORT_INTERRUPTS__STATE) != (MU_DCR(SLAVE_PORT_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(SLAVE_PORT_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(RMU_ECC_CORR_INTERRUPTS__STATE) != (MU_DCR(RMU_ECC_CORR_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(RMU_ECC_CORR_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(MISC_INTERRUPTS__STATE) != (MU_DCR(MISC_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(MISC_INTERRUPTS__STATE)\n\n");
   }
  if (MU_DCR(FIFO_INTERRUPTS__STATE) != (MU_DCR(FIFO_INTERRUPTS__MACHINE_CHECK) - 5 ) ) {
    printf("(E) DCR offset assertion failure for MU_DCR(FIFO_INTERRUPTS__STATE)\n\n");
   }
  {
    unsigned a = MU_DCR(RME_P_ERR0);
    unsigned b = MU_DCR(RME_P_ERR1);
    if ( a != (b-2) ) printf("(E) RME_P_ERR0 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR1);
    unsigned b = MU_DCR(RME_P_ERR2);
    if ( a != (b-2) ) printf("(E) RME_P_ERR1 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR2);
    unsigned b = MU_DCR(RME_P_ERR3);
    if ( a != (b-2) ) printf("(E) RME_P_ERR2 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR3);
    unsigned b = MU_DCR(RME_P_ERR4);
    if ( a != (b-2) ) printf("(E) RME_P_ERR3 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR4);
    unsigned b = MU_DCR(RME_P_ERR5);
    if ( a != (b-2) ) printf("(E) RME_P_ERR4 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR5);
    unsigned b = MU_DCR(RME_P_ERR6);
    if ( a != (b-2) ) printf("(E) RME_P_ERR5 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR6);
    unsigned b = MU_DCR(RME_P_ERR7);
    if ( a != (b-2) ) printf("(E) RME_P_ERR6 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR8);
    unsigned b = MU_DCR(RME_P_ERR9);
    if ( a != (b-2) ) printf("(E) RME_P_ERR8 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR9);
    unsigned b = MU_DCR(RME_P_ERR10);
    if ( a != (b-2) ) printf("(E) RME_P_ERR9 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR10);
    unsigned b = MU_DCR(RME_P_ERR11);
    if ( a != (b-2) ) printf("(E) RME_P_ERR10 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR11);
    unsigned b = MU_DCR(RME_P_ERR12);
    if ( a != (b-2) ) printf("(E) RME_P_ERR11 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR12);
    unsigned b = MU_DCR(RME_P_ERR13);
    if ( a != (b-2) ) printf("(E) RME_P_ERR12 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR13);
    unsigned b = MU_DCR(RME_P_ERR14);
    if ( a != (b-2) ) printf("(E) RME_P_ERR13 offset error. %d %d\n\n", a, b);
  }
  {
    unsigned a = MU_DCR(RME_P_ERR14);
    unsigned b = MU_DCR(RME_P_ERR15);
    if ( a != (b-2) ) printf("(E) RME_P_ERR14 offset error. %d %d\n\n", a, b);
  }
}
