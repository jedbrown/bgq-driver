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

  // Detailed summaries of specific error bits:
  const std::string XM0_FATAL_ERR0_DESCR = "(xm0_fatal_err0) (fatal error) sked_p_err - parity error on boundary latched value of sw_xm_sked_valid_2, sw_xm_sked_id_2 and sw_xm_sked_eager_2. ";
  const std::string XM0_FATAL_ERR1_DESCR = "(xm0_fatal_err1) (fatal error) sked_id_err - MSB of switch sked_id is set and MU only supports 16 L2 slice destinations. ";
  const std::string XM0_FATAL_ERR2_DESCR = "(xm0_fatal_err2) (fatal error) sked_eager_err - switch attempted eager schedule and MU doesn't support eager scheduling. ";
  const std::string XM0_FATAL_ERR3_DESCR = "(xm0_fatal_err3) (fatal error) sked_dly_p_err - parity error on latched sked_valid and lut_wr_valid. ";
  const std::string XM0_FATAL_ERR4_DESCR = "(xm0_fatal_err4) (fatal error) sr_xm_rd_valid_p_err - parity error on boundary latched value of sr_xm_rd_valid_2  and  sr_xm_rd_valid_p_2. ";
  const std::string XM0_FATAL_ERR5_DESCR = "(xm0_fatal_err5) (fatal error) sr_xm_rd_info_p_err - parity error on boundary latched value of sr_xm_rd_info_2. ";
  const std::string XM0_FATAL_ERR6_DESCR = "(xm0_fatal_err6) (fatal error) newreq_attr_p_err - parity error on internal ime_rtn_attr and newreq_info_attr. ";
  const std::string XM0_FATAL_ERR7_DESCR = "(xm0_fatal_err7) (fatal error) rdreturn_size_err - sr_xm_rd_info(0 to 1) specified an illegal '11' size. ";
  const std::string XM0_FATAL_ERR8_DESCR = "(xm0_fatal_err8) (fatal error) avail_ime_idx_p_err - parity error covering the avail_ime_idx vector. ";
  const std::string XM0_FATAL_ERR9_DESCR = "(xm0_fatal_err9) (fatal error) avail_idx_p_err - parity error covering the avail_idx vector. ";
  const std::string XM0_FATAL_ERR10_DESCR = "(xm0_fatal_err10) (fatal error) alloc_ime_idx_p_err - parity error covering alloc_ime_idx_attr. ";
  const std::string XM0_FATAL_ERR11_DESCR = "(xm0_fatal_err11) (fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr. ";
  const std::string XM0_FATAL_ERR12_DESCR = "(xm0_fatal_err12) (fatal error) rdreturn_idx_notvalid_err - error if tag of read return is listed as unallocated. ";
  const std::string XM0_FATAL_ERR13_DESCR = "(xm0_fatal_err13) (fatal error) ime_lut_p_err - parity error on LUT info associated with tag for read return. ";
  const std::string XM0_FATAL_ERR14_DESCR = "(xm0_fatal_err14) (fatal error) beat_cnt_p_err - parity error on beat counter for read return data. ";
  const std::string XM0_FATAL_ERR15_DESCR = "(xm0_fatal_err15) (fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking. ";
  const std::string XM0_FATAL_ERR16_DESCR = "(xm0_fatal_err16) (fatal error) fifo_dest_p_err - parity error in current request's destination field stored in request FIFO. ";
  const std::string XM0_FATAL_ERR17_DESCR = "(xm0_fatal_err17) (fatal error) lut_valid_p_err - parity error on lut_valid vector. ";
  const std::string XM0_FATAL_ERR18_DESCR = "(xm0_fatal_err18) (fatal error) lut_attr_p_err - parity error in look-up-table attributes for current request going out to switch. ";
  const std::string XM0_FATAL_ERR19_DESCR = "(xm0_fatal_err19) (fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination. ";
  const std::string XM0_FATAL_ERR20_DESCR = "(xm0_fatal_err20) (fatal error) dealloc_idx_p_err - parity error on dealloc_idx. ";
  const std::string XM0_FATAL_ERR21_DESCR = "(xm0_fatal_err21) (fatal error) genid_p_err - parity error on msgc_gen_ctr. ";
  const std::string XM0_FATAL_ERR22_DESCR = "(xm0_fatal_err22) (fatal error) req_cnt_p_err - parity error on the count of outstanding write requests from each rME. ";
  const std::string XM0_FATAL_ERR23_DESCR = "(xm0_fatal_err23) (fatal error) newreq_valid_p_err - parity error in the incoming request valid buffers. ";
  const std::string XM0_SOFTWARE_ERR24_DESCR = "(xm0_software_err24) (software error) newreq_write_range_err - write request address range check violation, see master port error info register. ";
  const std::string XM0_SOFTWARE_ERR25_DESCR = "(xm0_software_err25) (software error) newreq_read_range_err - read request address range check violation, see master port error info register. ";
  const std::string XM0_CORR_ERR26_DESCR = "(xm0_corr_err26) (correctable error) gen_cnt_p_err - gen_cnt latch HW correctable parity error. ";

void decode_master_port0_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " MASTER_PORT0_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // xm0_fatal_err0
    str << XM0_FATAL_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // xm0_fatal_err1
    str << XM0_FATAL_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // xm0_fatal_err2
    str << XM0_FATAL_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // xm0_fatal_err3
    str << XM0_FATAL_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // xm0_fatal_err4
    str << XM0_FATAL_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // xm0_fatal_err5
    str << XM0_FATAL_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // xm0_fatal_err6
    str << XM0_FATAL_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // xm0_fatal_err7
    str << XM0_FATAL_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // xm0_fatal_err8
    str << XM0_FATAL_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // xm0_fatal_err9
    str << XM0_FATAL_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // xm0_fatal_err10
    str << XM0_FATAL_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // xm0_fatal_err11
    str << XM0_FATAL_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // xm0_fatal_err12
    str << XM0_FATAL_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // xm0_fatal_err13
    str << XM0_FATAL_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // xm0_fatal_err14
    str << XM0_FATAL_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // xm0_fatal_err15
    str << XM0_FATAL_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // xm0_fatal_err16
    str << XM0_FATAL_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // xm0_fatal_err17
    str << XM0_FATAL_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // xm0_fatal_err18
    str << XM0_FATAL_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // xm0_fatal_err19
    str << XM0_FATAL_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // xm0_fatal_err20
    str << XM0_FATAL_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // xm0_fatal_err21
    str << XM0_FATAL_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // xm0_fatal_err22
    str << XM0_FATAL_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // xm0_fatal_err23
    str << XM0_FATAL_ERR23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // xm0_software_err24
    str << XM0_SOFTWARE_ERR24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // xm0_software_err25
    str << XM0_SOFTWARE_ERR25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // xm0_corr_err26
    str << XM0_CORR_ERR26_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string XM1_FATAL_ERR0_DESCR = "(xm1_fatal_err0) (fatal error) sked_p_err - parity error on boundary latched value of sw_xm_sked_valid_2, sw_xm_sked_id_2 and sw_xm_sked_eager_2. ";
  const std::string XM1_FATAL_ERR1_DESCR = "(xm1_fatal_err1) (fatal error) sked_id_err - MSB of switch sked_id is set and MU only supports 16 L2 slice destinations. ";
  const std::string XM1_FATAL_ERR2_DESCR = "(xm1_fatal_err2) (fatal error) sked_eager_err - switch attempted eager schedule and MU doesn't support eager scheduling. ";
  const std::string XM1_FATAL_ERR3_DESCR = "(xm1_fatal_err3) (fatal error) sked_dly_p_err - parity error on latched sked_valid and lut_wr_valid. ";
  const std::string XM1_FATAL_ERR4_DESCR = "(xm1_fatal_err4) (fatal error) sr_xm_rd_valid_p_err - parity error on boundary latched value of sr_xm_rd_valid_2  and  sr_xm_rd_valid_p_2. ";
  const std::string XM1_FATAL_ERR5_DESCR = "(xm1_fatal_err5) (fatal error) sr_xm_rd_info_p_err - parity error on boundary latched value of sr_xm_rd_info_2. ";
  const std::string XM1_FATAL_ERR6_DESCR = "(xm1_fatal_err6) (fatal error) newreq_attr_p_err - parity error on internal ime_rtn_attr and newreq_info_attr. ";
  const std::string XM1_FATAL_ERR7_DESCR = "(xm1_fatal_err7) (fatal error) rdreturn_size_err - sr_xm_rd_info(0 to 1) specified an illegal '11' size. ";
  const std::string XM1_FATAL_ERR8_DESCR = "(xm1_fatal_err8) (fatal error) avail_ime_idx_p_err - parity error covering the avail_ime_idx vector. ";
  const std::string XM1_FATAL_ERR9_DESCR = "(xm1_fatal_err9) (fatal error) avail_idx_p_err - parity error covering the avail_idx vector. ";
  const std::string XM1_FATAL_ERR10_DESCR = "(xm1_fatal_err10) (fatal error) alloc_ime_idx_p_err - parity error covering alloc_ime_idx_attr. ";
  const std::string XM1_FATAL_ERR11_DESCR = "(xm1_fatal_err11) (fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr. ";
  const std::string XM1_FATAL_ERR12_DESCR = "(xm1_fatal_err12) (fatal error) rdreturn_idx_notvalid_err - error if tag of read return is listed as unallocated. ";
  const std::string XM1_FATAL_ERR13_DESCR = "(xm1_fatal_err13) (fatal error) ime_lut_p_err - parity error on LUT info associated with tag for read return. ";
  const std::string XM1_FATAL_ERR14_DESCR = "(xm1_fatal_err14) (fatal error) beat_cnt_p_err - parity error on beat counter for read return data. ";
  const std::string XM1_FATAL_ERR15_DESCR = "(xm1_fatal_err15) (fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking. ";
  const std::string XM1_FATAL_ERR16_DESCR = "(xm1_fatal_err16) (fatal error) fifo_dest_p_err - parity error in current request's destination field stored in request FIFO. ";
  const std::string XM1_FATAL_ERR17_DESCR = "(xm1_fatal_err17) (fatal error) lut_valid_p_err - parity error on lut_valid vector. ";
  const std::string XM1_FATAL_ERR18_DESCR = "(xm1_fatal_err18) (fatal error) lut_attr_p_err - parity error in look-up-table attributes for current request going out to switch. ";
  const std::string XM1_FATAL_ERR19_DESCR = "(xm1_fatal_err19) (fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination. ";
  const std::string XM1_FATAL_ERR20_DESCR = "(xm1_fatal_err20) (fatal error) dealloc_idx_p_err - parity error on dealloc_idx. ";
  const std::string XM1_FATAL_ERR21_DESCR = "(xm1_fatal_err21) (fatal error) genid_p_err - parity error on msgc_gen_ctr. ";
  const std::string XM1_FATAL_ERR22_DESCR = "(xm1_fatal_err22) (fatal error) req_cnt_p_err - parity error on the count of outstanding write requests from each rME. ";
  const std::string XM1_FATAL_ERR23_DESCR = "(xm1_fatal_err23) (fatal error) newreq_valid_p_err - parity error in the incoming request valid buffers. ";
  const std::string XM1_SOFTWARE_ERR24_DESCR = "(xm1_software_err24) (software error) newreq_write_range_err - write request address range check violation, see master port error info register. ";
  const std::string XM1_SOFTWARE_ERR25_DESCR = "(xm1_software_err25) (software error) newreq_read_range_err - read request address range check violation, see master port error info register. ";
  const std::string XM1_CORR_ERR26_DESCR = "(xm1_corr_err26) (correctable error) gen_cnt_p_err - gen_cnt latch HW correctable parity error. ";

void decode_master_port1_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " MASTER_PORT1_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // xm1_fatal_err0
    str << XM1_FATAL_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // xm1_fatal_err1
    str << XM1_FATAL_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // xm1_fatal_err2
    str << XM1_FATAL_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // xm1_fatal_err3
    str << XM1_FATAL_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // xm1_fatal_err4
    str << XM1_FATAL_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // xm1_fatal_err5
    str << XM1_FATAL_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // xm1_fatal_err6
    str << XM1_FATAL_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // xm1_fatal_err7
    str << XM1_FATAL_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // xm1_fatal_err8
    str << XM1_FATAL_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // xm1_fatal_err9
    str << XM1_FATAL_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // xm1_fatal_err10
    str << XM1_FATAL_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // xm1_fatal_err11
    str << XM1_FATAL_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // xm1_fatal_err12
    str << XM1_FATAL_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // xm1_fatal_err13
    str << XM1_FATAL_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // xm1_fatal_err14
    str << XM1_FATAL_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // xm1_fatal_err15
    str << XM1_FATAL_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // xm1_fatal_err16
    str << XM1_FATAL_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // xm1_fatal_err17
    str << XM1_FATAL_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // xm1_fatal_err18
    str << XM1_FATAL_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // xm1_fatal_err19
    str << XM1_FATAL_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // xm1_fatal_err20
    str << XM1_FATAL_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // xm1_fatal_err21
    str << XM1_FATAL_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // xm1_fatal_err22
    str << XM1_FATAL_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // xm1_fatal_err23
    str << XM1_FATAL_ERR23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // xm1_software_err24
    str << XM1_SOFTWARE_ERR24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // xm1_software_err25
    str << XM1_SOFTWARE_ERR25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // xm1_corr_err26
    str << XM1_CORR_ERR26_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string ARLOG_ERR0_DESCR = "(arlog_err0) (fatal error) iME-to-imFIFO arbitration logic parity error in control flag latches. ";
  const std::string ARLOG_ERR1_DESCR = "(arlog_err1) (fatal error) iME-to-imFIFO arbitration stage 1 pipeline latch parity error (ready flag set request lost). ";
  const std::string ARLOG_ERR2_DESCR = "(arlog_err2) (fatal error) iME-to-imFIFO arbitration logic, multiple requests accepted at the same time. ";
  const std::string ARLOG_ERR3_DESCR = "(arlog_err3) (non-fatal error) iME-to-imFIFO arbitration logic stage 1 pipeline latch parity error (recovered by retry). ";
  const std::string ARLOG_ERR4_DESCR = "(arlog_err4) (non-fatal error) iME-to-imFIFO arbitration logic ime_available register bit error (recovered by retry). ";
  const std::string ARLOG_ERR5_DESCR = "(arlog_err5) (non-fatal error) iME-to-imFIFO arbitration logic 136to1 arbiter random bit latch parity error. ";
  const std::string ARLOG_ERR6_DESCR = "(arlog_err6) (non-fatal error) iME-to-imFIFO arbitration logic 64to1 arbiter random bit latch parity error. ";
  const std::string ARLOG_ERR7_DESCR = "(arlog_err7) (fatal error) iME-to-imFIFO arbitration logic FIFO map latch scan and correct index counter parity error. ";
  const std::string ARLOG_ERR8_DESCR = "(arlog_err8) (fatal error) iME-to-imFIFO arbitration logic previous FIFO map read index latch parity error. ";
  const std::string ARLOG_ERR9_DESCR = "(arlog_err9) (non-fatal error) iME-to-imFIFO arbitration logic 136to1 arbitration result latch parity error. ";

void decode_arlog_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " ARLOG_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // arlog_err0
    str << ARLOG_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // arlog_err1
    str << ARLOG_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // arlog_err2
    str << ARLOG_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // arlog_err3
    str << ARLOG_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // arlog_err4
    str << ARLOG_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // arlog_err5
    str << ARLOG_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // arlog_err6
    str << ARLOG_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // arlog_err7
    str << ARLOG_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // arlog_err8
    str << ARLOG_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // arlog_err9
    str << ARLOG_ERR9_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string XM2_FATAL_ERR0_DESCR = "(xm2_fatal_err0) (fatal error) sked_p_err - parity error on boundary latched value of sw_xm_sked_valid_2, sw_xm_sked_id_2 and sw_xm_sked_eager_2. ";
  const std::string XM2_FATAL_ERR1_DESCR = "(xm2_fatal_err1) (fatal error) sked_id_err - MSB of switch sked_id is set and MU only supports 16 L2 slice destinations. ";
  const std::string XM2_FATAL_ERR2_DESCR = "(xm2_fatal_err2) (fatal error) sked_eager_err - switch attempted eager schedule and MU doesn't support eager scheduling. ";
  const std::string XM2_FATAL_ERR3_DESCR = "(xm2_fatal_err3) (fatal error) sked_dly_p_err - parity error on latched sked_valid and lut_wr_valid. ";
  const std::string XM2_FATAL_ERR4_DESCR = "(xm2_fatal_err4) (fatal error) sr_xm_rd_valid_p_err - parity error on boundary latched value of sr_xm_rd_valid_2  and  sr_xm_rd_valid_p_2. ";
  const std::string XM2_FATAL_ERR5_DESCR = "(xm2_fatal_err5) (fatal error) sr_xm_rd_info_p_err - parity error on boundary latched value of sr_xm_rd_info_2. ";
  const std::string XM2_FATAL_ERR6_DESCR = "(xm2_fatal_err6) (fatal error) newreq_attr_p_err - parity error on internal ime_rtn_attr and newreq_info_attr. ";
  const std::string XM2_FATAL_ERR7_DESCR = "(xm2_fatal_err7) (fatal error) rdreturn_size_err - sr_xm_rd_info(0 to 1) specified an illegal '11' size. ";
  const std::string XM2_FATAL_ERR8_DESCR = "(xm2_fatal_err8) (fatal error) avail_ime_idx_p_err - parity error covering the avail_ime_idx vector. ";
  const std::string XM2_FATAL_ERR9_DESCR = "(xm2_fatal_err9) (fatal error) avail_idx_p_err - parity error covering the avail_idx vector. ";
  const std::string XM2_FATAL_ERR10_DESCR = "(xm2_fatal_err10) (fatal error) alloc_ime_idx_p_err - parity error covering alloc_ime_idx_attr. ";
  const std::string XM2_FATAL_ERR11_DESCR = "(xm2_fatal_err11) (fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr. ";
  const std::string XM2_FATAL_ERR12_DESCR = "(xm2_fatal_err12) (fatal error) rdreturn_idx_notvalid_err - error if tag of read return is listed as unallocated. ";
  const std::string XM2_FATAL_ERR13_DESCR = "(xm2_fatal_err13) (fatal error) ime_lut_p_err - parity error on LUT info associated with tag for read return. ";
  const std::string XM2_FATAL_ERR14_DESCR = "(xm2_fatal_err14) (fatal error) beat_cnt_p_err - parity error on beat counter for read return data. ";
  const std::string XM2_FATAL_ERR15_DESCR = "(xm2_fatal_err15) (fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking. ";
  const std::string XM2_FATAL_ERR16_DESCR = "(xm2_fatal_err16) (fatal error) fifo_dest_p_err - parity error in current request's destination field stored in request FIFO. ";
  const std::string XM2_FATAL_ERR17_DESCR = "(xm2_fatal_err17) (fatal error) lut_valid_p_err - parity error on lut_valid vector. ";
  const std::string XM2_FATAL_ERR18_DESCR = "(xm2_fatal_err18) (fatal error) lut_attr_p_err - parity error in look-up-table attributes for current request going out to switch. ";
  const std::string XM2_FATAL_ERR19_DESCR = "(xm2_fatal_err19) (fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination. ";
  const std::string XM2_FATAL_ERR20_DESCR = "(xm2_fatal_err20) (fatal error) dealloc_idx_p_err - parity error on dealloc_idx. ";
  const std::string XM2_FATAL_ERR21_DESCR = "(xm2_fatal_err21) (fatal error) genid_p_err - parity error on msgc_gen_ctr. ";
  const std::string XM2_FATAL_ERR22_DESCR = "(xm2_fatal_err22) (fatal error) req_cnt_p_err - parity error on the count of outstanding write requests from each rME. ";
  const std::string XM2_FATAL_ERR23_DESCR = "(xm2_fatal_err23) (fatal error) newreq_valid_p_err - parity error in the incoming request valid buffers. ";
  const std::string XM2_SOFTWARE_ERR24_DESCR = "(xm2_software_err24) (software error) newreq_write_range_err - write request address range check violation, see master port error info register. ";
  const std::string XM2_SOFTWARE_ERR25_DESCR = "(xm2_software_err25) (software error) newreq_read_range_err - read request address range check violation, see master port error info register. ";
  const std::string XM2_CORR_ERR26_DESCR = "(xm2_corr_err26) (correctable error) gen_cnt_p_err - gen_cnt latch HW correctable parity error. ";

void decode_master_port2_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " MASTER_PORT2_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // xm2_fatal_err0
    str << XM2_FATAL_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // xm2_fatal_err1
    str << XM2_FATAL_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // xm2_fatal_err2
    str << XM2_FATAL_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // xm2_fatal_err3
    str << XM2_FATAL_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // xm2_fatal_err4
    str << XM2_FATAL_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // xm2_fatal_err5
    str << XM2_FATAL_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // xm2_fatal_err6
    str << XM2_FATAL_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // xm2_fatal_err7
    str << XM2_FATAL_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // xm2_fatal_err8
    str << XM2_FATAL_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // xm2_fatal_err9
    str << XM2_FATAL_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // xm2_fatal_err10
    str << XM2_FATAL_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // xm2_fatal_err11
    str << XM2_FATAL_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // xm2_fatal_err12
    str << XM2_FATAL_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // xm2_fatal_err13
    str << XM2_FATAL_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // xm2_fatal_err14
    str << XM2_FATAL_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // xm2_fatal_err15
    str << XM2_FATAL_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // xm2_fatal_err16
    str << XM2_FATAL_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // xm2_fatal_err17
    str << XM2_FATAL_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // xm2_fatal_err18
    str << XM2_FATAL_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // xm2_fatal_err19
    str << XM2_FATAL_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // xm2_fatal_err20
    str << XM2_FATAL_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // xm2_fatal_err21
    str << XM2_FATAL_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // xm2_fatal_err22
    str << XM2_FATAL_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // xm2_fatal_err23
    str << XM2_FATAL_ERR23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // xm2_software_err24
    str << XM2_SOFTWARE_ERR24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // xm2_software_err25
    str << XM2_SOFTWARE_ERR25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // xm2_corr_err26
    str << XM2_CORR_ERR26_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string ECC_COUNT_OVER0_DESCR = "(ecc_count_over0) (correctable error) iME-to-imFIFO arbitration logic correctable ECC error count has exceeded the threshold. ";
  const std::string ECC_COUNT_OVER1_DESCR = "(ecc_count_over1) (correctable error) ICSRAM correctable ECC error count has exceeded the threshold. ";
  const std::string ECC_COUNT_OVER2_DESCR = "(ecc_count_over2) (correctable error) MCSRAM correctable ECC error count has exceeded the threshold. ";
  const std::string ECC_COUNT_OVER3_DESCR = "(ecc_count_over3) (correctable error) RCSRAM correctable ECC error count has exceeded the threshold. ";
  const std::string ECC_COUNT_OVER4_DESCR = "(ecc_count_over4) (correctable error) RPUT correctable ECC error count has exceeded the threshold. ";
  const std::string ECC_COUNT_OVER5_DESCR = "(ecc_count_over5) (correctable error) MMREGS correctable ECC error count has exceeded the threshold. ";
  const std::string ECC_COUNT_OVER6_DESCR = "(ecc_count_over6) (correctable error) XS correctable ECC error count has exceeded the threshold. ";
  const std::string ECC_COUNT_OVER7_DESCR = "(ecc_count_over7) (correctable error) RME correctable ECC error count has exceeded the threshold. ";

void decode_ecc_count_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " ECC_COUNT_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // ecc_count_over0
    str << ECC_COUNT_OVER0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // ecc_count_over1
    str << ECC_COUNT_OVER1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // ecc_count_over2
    str << ECC_COUNT_OVER2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // ecc_count_over3
    str << ECC_COUNT_OVER3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // ecc_count_over4
    str << ECC_COUNT_OVER4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // ecc_count_over5
    str << ECC_COUNT_OVER5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // ecc_count_over6
    str << ECC_COUNT_OVER6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // ecc_count_over7
    str << ECC_COUNT_OVER7_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string IMU_ECC_UE0_DESCR = "(imu_ecc_ue0) (fatal error) XM to MCSRAM write data word0 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE1_DESCR = "(imu_ecc_ue1) (fatal error) XM to MCSRAM write data word1 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE2_DESCR = "(imu_ecc_ue2) (fatal error) XM to MCSRAM write data word2 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE3_DESCR = "(imu_ecc_ue3) (fatal error) XM to MCSRAM write data word3 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE4_DESCR = "(imu_ecc_ue4) (fatal error) MCSRAM read data word0 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE5_DESCR = "(imu_ecc_ue5) (fatal error) MCSRAM read data word1 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE6_DESCR = "(imu_ecc_ue6) (fatal error) MCSRAM read data word2 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE7_DESCR = "(imu_ecc_ue7) (fatal error) MCSRAM read data word3 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE8_DESCR = "(imu_ecc_ue8) (fatal error) MCSRAM read data word4 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE9_DESCR = "(imu_ecc_ue9) (fatal error) MCSRAM read data word5 ECC uncorrectable error. ";
  const std::string IMU_ECC_UE10_DESCR = "(imu_ecc_ue10) (fatal error) iME-to-imFIFO arbitration logic FIFO map latch ECC uncorrectable error. ";
  const std::string IMU_ECC_CE0_DESCR = "(imu_ecc_ce0) (correctable error) XM to MCSRAM write data word0 ECC correctable error. ";
  const std::string IMU_ECC_CE1_DESCR = "(imu_ecc_ce1) (correctable error) XM to MCSRAM write data word1 ECC correctable error. ";
  const std::string IMU_ECC_CE2_DESCR = "(imu_ecc_ce2) (correctable error) XM to MCSRAM write data word2 ECC correctable error. ";
  const std::string IMU_ECC_CE3_DESCR = "(imu_ecc_ce3) (correctable error) XM to MCSRAM write data word3 ECC correctable error. ";
  const std::string IMU_ECC_CE4_DESCR = "(imu_ecc_ce4) (correctable error) MCSRAM read data word0 ECC correctable error. ";
  const std::string IMU_ECC_CE5_DESCR = "(imu_ecc_ce5) (correctable error) MCSRAM read data word1 ECC correctable error. ";
  const std::string IMU_ECC_CE6_DESCR = "(imu_ecc_ce6) (correctable error) MCSRAM read data word2 ECC correctable error. ";
  const std::string IMU_ECC_CE7_DESCR = "(imu_ecc_ce7) (correctable error) MCSRAM read data word3 ECC correctable error. ";
  const std::string IMU_ECC_CE8_DESCR = "(imu_ecc_ce8) (correctable error) MCSRAM read data word4 ECC correctable error. ";
  const std::string IMU_ECC_CE9_DESCR = "(imu_ecc_ce9) (correctable error) MCSRAM read data word5 ECC correctable error. ";
  const std::string IMU_ECC_CE10_DESCR = "(imu_ecc_ce10) (correctable error) iME-to-imFIFO arbitration logic FIFO map latch ECC correctable error. ";
  const std::string IMU_ECC_CE11_DESCR = "(imu_ecc_ce11) (correctable error) ICSRAM start_ecc_ce - correctable ECC error on start field, can be ignored until field is initialized. ";
  const std::string IMU_ECC_CE12_DESCR = "(imu_ecc_ce12) (correctable error) ICSRAM size_ecc_ce - correctable ECC error on size field, can be ignored until field is initialized. ";
  const std::string IMU_ECC_CE13_DESCR = "(imu_ecc_ce13) (correctable error) ICSRAM head_ecc_ce - correctable ECC error on head field, can be ignored until field is initialized. ";
  const std::string IMU_ECC_CE14_DESCR = "(imu_ecc_ce14) (correctable error) ICSRAM tail_ecc_ce - correctable ECC error on tail field, can be ignored until field is initialized. ";
  const std::string IMU_ECC_CE15_DESCR = "(imu_ecc_ce15) (correctable error) ICSRAM desc_ecc_ce - correctable ECC error on desc field, can be ignored until field is initialized. ";
  const std::string IMU_ECC_CE16_DESCR = "(imu_ecc_ce16) (correctable error) ICSRAM free_ecc_ce - correctable ECC error on free space field, cannot be ignored. ";
  const std::string IMU_ECC_CE17_DESCR = "(imu_ecc_ce17) (correctable error) ICSRAM xmint_head_ecc_ce - correctable ECC error on head address for descriptor fetch. ";
  const std::string IMU_ECC_CE18_DESCR = "(imu_ecc_ce18) (correctable error) ICSRAM rdstart_ecc_ce - correctable ECC error on start read for rGET packet. ";
  const std::string IMU_ECC_CE19_DESCR = "(imu_ecc_ce19) (correctable error) ICSRAM rdsize_ecc_ce - correctable ECC error on size read for rGET packet. ";
  const std::string IMU_ECC_CE20_DESCR = "(imu_ecc_ce20) (correctable error) ICSRAM rdtail_ecc_ce - correctable ECC error on tail read for rGET packet. ";
  const std::string IMU_ECC_CE21_DESCR = "(imu_ecc_ce21) (correctable error) ICSRAM rdfree_ecc_ce - correctable ECC error on free space read for rGET packet. ";

void decode_imu_ecc_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " IMU_ECC_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // imu_ecc_ue0
    str << IMU_ECC_UE0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // imu_ecc_ue1
    str << IMU_ECC_UE1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // imu_ecc_ue2
    str << IMU_ECC_UE2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // imu_ecc_ue3
    str << IMU_ECC_UE3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // imu_ecc_ue4
    str << IMU_ECC_UE4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // imu_ecc_ue5
    str << IMU_ECC_UE5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // imu_ecc_ue6
    str << IMU_ECC_UE6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // imu_ecc_ue7
    str << IMU_ECC_UE7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // imu_ecc_ue8
    str << IMU_ECC_UE8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // imu_ecc_ue9
    str << IMU_ECC_UE9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // imu_ecc_ue10
    str << IMU_ECC_UE10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // imu_ecc_ce0
    str << IMU_ECC_CE0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // imu_ecc_ce1
    str << IMU_ECC_CE1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // imu_ecc_ce2
    str << IMU_ECC_CE2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // imu_ecc_ce3
    str << IMU_ECC_CE3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // imu_ecc_ce4
    str << IMU_ECC_CE4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // imu_ecc_ce5
    str << IMU_ECC_CE5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // imu_ecc_ce6
    str << IMU_ECC_CE6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // imu_ecc_ce7
    str << IMU_ECC_CE7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // imu_ecc_ce8
    str << IMU_ECC_CE8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // imu_ecc_ce9
    str << IMU_ECC_CE9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // imu_ecc_ce10
    str << IMU_ECC_CE10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // imu_ecc_ce11
    str << IMU_ECC_CE11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // imu_ecc_ce12
    str << IMU_ECC_CE12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // imu_ecc_ce13
    str << IMU_ECC_CE13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // imu_ecc_ce14
    str << IMU_ECC_CE14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // imu_ecc_ce15
    str << IMU_ECC_CE15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // imu_ecc_ce16
    str << IMU_ECC_CE16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // imu_ecc_ce17
    str << IMU_ECC_CE17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // imu_ecc_ce18
    str << IMU_ECC_CE18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // imu_ecc_ce19
    str << IMU_ECC_CE19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // imu_ecc_ce20
    str << IMU_ECC_CE20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // imu_ecc_ce21
    str << IMU_ECC_CE21_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string MMREGS_ECC_CE0_DESCR = "(mmregs_ecc_ce0) (correctable error) wrdata_ecc_ce - Correctable ECC error on MMIO write data. ";
  const std::string MMREGS_ECC_CE1_DESCR = "(mmregs_ecc_ce1) (correctable error) barrier_mu_ecc_ce - Correctable ECC error in global interrupt control register. ";
  const std::string MMREGS_ECC_CE2_DESCR = "(mmregs_ecc_ce2) (correctable error) barrier_nd_ecc_ce - Correctable ECC error in global interrupt status register. ";
  const std::string XS_ECC_CE0_DESCR = "(xs_ecc_ce0) (correctable error) info_ecc_ce - Correctable ECC error on info field passed to MU slave by switch. ";
  const std::string XS_ECC_CE1_DESCR = "(xs_ecc_ce1) (correctable error) data_bvalids0_ecc_ce - Correctable ECC error on first 72b of write data  and  byte valids passed to MU slave by switch. ";
  const std::string XS_ECC_CE2_DESCR = "(xs_ecc_ce2) (correctable error) data_bvalids1_ecc_ce - Correctable ECC error on second 72b of write data  and  byte valids passed to MU slave by switch. ";

void decode_misc_ecc_corr_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " MISC_ECC_CORR_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // mmregs_ecc_ce0
    str << MMREGS_ECC_CE0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // mmregs_ecc_ce1
    str << MMREGS_ECC_CE1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // mmregs_ecc_ce2
    str << MMREGS_ECC_CE2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // xs_ecc_ce0
    str << XS_ECC_CE0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // xs_ecc_ce1
    str << XS_ECC_CE1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // xs_ecc_ce2
    str << XS_ECC_CE2_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RCSRAM_ERR0_DESCR = "(rcsram_err0) (fatal error) RCSRAM parity error in control flag latches. ";
  const std::string RCSRAM_ERR1_DESCR = "(rcsram_err1) (fatal error) RCSRAM parity error in slave index latch. ";
  const std::string RCSRAM_ERR2_DESCR = "(rcsram_err2) (fatal error) RCSRAM parity error in slave/get-tail request stage 1/4 latch. ";
  const std::string RCSRAM_ERR3_DESCR = "(rcsram_err3) (fatal error) RCSRAM parity error in slave/get-tail request stage 2 latch. ";
  const std::string RCSRAM_ERR4_DESCR = "(rcsram_err4) (fatal error) RCSRAM parity error in slave/get-tail request stage 3 latch. ";
  const std::string RCSRAM_ERR5_DESCR = "(rcsram_err5) (fatal error) RCSRAM parity error in interrupt index latch. ";
  const std::string RCSRAM_ERR6_DESCR = "(rcsram_err6) (fatal error) RCSRAM parity error in commit-tail request stage 1/3 latch. ";
  const std::string RCSRAM_ERR7_DESCR = "(rcsram_err7) (fatal error) RCSRAM parity error in commit-tail request stage 2 latch. ";
  const std::string RCSRAM_ERR8_DESCR = "(rcsram_err8) (fatal error) RCSRAM parity error in commit-tail request stage 4 latch. ";
  const std::string RCSRAM_ERR9_DESCR = "(rcsram_err9) (fatal error) RCSRAM get tail SRAM write data latch parity error. ";
  const std::string RCSRAM_ERR10_DESCR = "(rcsram_err10) (fatal error) RCSRAM get tail read data output latch parity error. ";
  const std::string RCSRAM_ERR11_DESCR = "(rcsram_err11) (fatal error) RCSRAM state encoding error. ";
  const std::string RCSRAM_ERR12_DESCR = "(rcsram_err12) (software error) RCSRAM user packet getting tail of system rmFIFO. ";
  const std::string RCSRAM_ERR13_DESCR = "(rcsram_err13) (software error) RCSRAM get-tail request index out of bound (>= 272). ";
  const std::string RCSRAM_ERR14_DESCR = "(rcsram_err14) (software error) RCSRAM requested rmFIFO not enabled by DCR register. ";
  const std::string RCSRAM_ERR15_DESCR = "(rcsram_err15) (software error) RCSRAM write data from slave not aligned to 64B or greater than 2**36-1. ";
  const std::string RCSRAM_ERR16_DESCR = "(rcsram_err16) (non-fatal error) RCSRAM get-tail request arbiter random bit latch parity error. ";
  const std::string RCSRAM_ERR17_DESCR = "(rcsram_err17) (non-fatal error) RCSRAM commit-tail request arbiter random bit latch parity error. ";
  const std::string RCSRAM_ERR18_DESCR = "(rcsram_err18) (non-fatal error) RCSRAM backdoor tail ID read data latch parity error. ";
  const std::string RCSRAM_ERR19_DESCR = "(rcsram_err19) (non-fatal error) RCSRAM backdoor ECC read data latch parity error. ";

void decode_rcsram_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " RCSRAM_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // rcsram_err0
    str << RCSRAM_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // rcsram_err1
    str << RCSRAM_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // rcsram_err2
    str << RCSRAM_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // rcsram_err3
    str << RCSRAM_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // rcsram_err4
    str << RCSRAM_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // rcsram_err5
    str << RCSRAM_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // rcsram_err6
    str << RCSRAM_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // rcsram_err7
    str << RCSRAM_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // rcsram_err8
    str << RCSRAM_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // rcsram_err9
    str << RCSRAM_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // rcsram_err10
    str << RCSRAM_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // rcsram_err11
    str << RCSRAM_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // rcsram_err12
    str << RCSRAM_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // rcsram_err13
    str << RCSRAM_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // rcsram_err14
    str << RCSRAM_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // rcsram_err15
    str << RCSRAM_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // rcsram_err16
    str << RCSRAM_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // rcsram_err17
    str << RCSRAM_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // rcsram_err18
    str << RCSRAM_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // rcsram_err19
    str << RCSRAM_ERR19_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string MMREGS_FATAL_ERR0_DESCR = "(mmregs_fatal_err0) (fatal error) wrdata_ecc_err - uncorrectable ECC error on incoming write data. ";
  const std::string MMREGS_FATAL_ERR1_DESCR = "(mmregs_fatal_err1) (fatal error) imfifo_enable_status_p_err - parity error on imFIFO enabled registers. ";
  const std::string MMREGS_FATAL_ERR2_DESCR = "(mmregs_fatal_err2) (fatal error) imfifo_thold_status_p_err - parity error on imFIFO threshold crossing interrupt status register. ";
  const std::string MMREGS_FATAL_ERR3_DESCR = "(mmregs_fatal_err3) (fatal error) rmfifo_thold_status_p_err - parity error on rmFIFO threshold crossing interrupt status register. ";
  const std::string MMREGS_FATAL_ERR4_DESCR = "(mmregs_fatal_err4) (fatal error) rmfifo_pktint_status_p_err - parity error on rmFIFO packet arrival interrupt status register. ";
  const std::string MMREGS_FATAL_ERR5_DESCR = "(mmregs_fatal_err5) (fatal error) info_p_err - parity error on info register. ";
  const std::string MMREGS_FATAL_ERR6_DESCR = "(mmregs_fatal_err6) (fatal error) rd_barriers_latched_p_errn - parity error on latched barrier register read data (either barrier status or control). ";
  const std::string MMREGS_FATAL_ERR7_DESCR = "(mmregs_fatal_err7) (fatal error) barrier_mu_ecc_err - uncorrectable ECC error on global interrupt control register. ";
  const std::string MMREGS_FATAL_ERR8_DESCR = "(mmregs_fatal_err8) (fatal error) barrier_nd_ecc_err - uncorrectable ECC error on global interrupt status register. ";
  const std::string MMREGS_SOFTWARE_ERR9_DESCR = "(mmregs_software_err9) (software error) rd_imfifo_enable_err - software attempted to read the write-only enable imFIFO register. ";
  const std::string MMREGS_SOFTWARE_ERR10_DESCR = "(mmregs_software_err10) (software error) rd_imfifo_disable_err - software attempted to read the write-only disable imFIFO register. ";
  const std::string MMREGS_SOFTWARE_ERR11_DESCR = "(mmregs_software_err11) (software error) rd_fifo_int_clr_err - software attempted to read the write-only clear interrupt status register. ";
  const std::string MMREGS_SOFTWARE_ERR12_DESCR = "(mmregs_software_err12) (software error) wr_imfifo_enable_status_err - software attempted to write the read-only imFIFO enable status register. ";
  const std::string MMREGS_SOFTWARE_ERR13_DESCR = "(mmregs_software_err13) (software error) wr_fifo_int_status_err - software attempted to write the read-only interrupt status register. ";
  const std::string MMREGS_SOFTWARE_ERR14_DESCR = "(mmregs_software_err14) (software error) imfifo_enable_prot_err - user attempted to enable or disable a system imFIFO. ";
  const std::string MMREGS_SOFTWARE_ERR15_DESCR = "(mmregs_software_err15) (software error) fifo_int_clr_prot_err - user attempted to clear an interrupt corresponding to a system imFIFO or rmFIFO. ";
  const std::string MMREGS_SOFTWARE_ERR16_DESCR = "(mmregs_software_err16) (software error) wr_range_err - software attempted to write (set) unused MMIO bits, either bits 0-31 in imFIFO enable/disable/high-priority, or bits 0-60 in global interrupt control. ";
  const std::string MMREGS_SOFTWARE_ERR17_DESCR = "(mmregs_software_err17) (software error) wr_barriers_nd_err - software attempted to write the read-only global interrupt status register. ";
  const std::string MMREGS_SOFTWARE_ERR18_DESCR = "(mmregs_software_err18) (software error) barrier_mu_prot_err - user attempted to write a system global interrupt control bit;. ";

void decode_mmregs_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " MMREGS_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // mmregs_fatal_err0
    str << MMREGS_FATAL_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // mmregs_fatal_err1
    str << MMREGS_FATAL_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // mmregs_fatal_err2
    str << MMREGS_FATAL_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // mmregs_fatal_err3
    str << MMREGS_FATAL_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // mmregs_fatal_err4
    str << MMREGS_FATAL_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // mmregs_fatal_err5
    str << MMREGS_FATAL_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // mmregs_fatal_err6
    str << MMREGS_FATAL_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // mmregs_fatal_err7
    str << MMREGS_FATAL_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // mmregs_fatal_err8
    str << MMREGS_FATAL_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // mmregs_software_err9
    str << MMREGS_SOFTWARE_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // mmregs_software_err10
    str << MMREGS_SOFTWARE_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // mmregs_software_err11
    str << MMREGS_SOFTWARE_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // mmregs_software_err12
    str << MMREGS_SOFTWARE_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // mmregs_software_err13
    str << MMREGS_SOFTWARE_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // mmregs_software_err14
    str << MMREGS_SOFTWARE_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // mmregs_software_err15
    str << MMREGS_SOFTWARE_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // mmregs_software_err16
    str << MMREGS_SOFTWARE_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // mmregs_software_err17
    str << MMREGS_SOFTWARE_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // mmregs_software_err18
    str << MMREGS_SOFTWARE_ERR18_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RMU_ECC_UE0_DESCR = "(rmu_ecc_ue0) (fatal error) XS--RCSRAM write data ECC uncorrectable error. ";
  const std::string RMU_ECC_UE1_DESCR = "(rmu_ecc_ue1) (fatal error) RCSRAM read data word0 ECC uncorrectable error. ";
  const std::string RMU_ECC_UE2_DESCR = "(rmu_ecc_ue2) (fatal error) RCSRAM read data word1 ECC uncorrectable error. ";
  const std::string RMU_ECC_UE3_DESCR = "(rmu_ecc_ue3) (fatal error) RCSRAM read data word2 ECC uncorrectable error. ";
  const std::string RMU_ECC_UE4_DESCR = "(rmu_ecc_ue4) (fatal error) RCSRAM read data word3 ECC uncorrectable error. ";
  const std::string RMU_ECC_UE5_DESCR = "(rmu_ecc_ue5) (fatal error) RCSRAM read data word4 ECC uncorrectable error. ";
  const std::string RMU_ECC_UE6_DESCR = "(rmu_ecc_ue6) (fatal error) RPUT SRAM read data ECC uncorrectable error. ";
  const std::string RMU_ECC_UE7_DESCR = "(rmu_ecc_ue7) (fatal error) RPUT SRAM write data ECC uncorrectable error. ";
  const std::string RMU_ECC_UE8_DESCR = "(rmu_ecc_ue8) (fatal error) rME0 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE9_DESCR = "(rmu_ecc_ue9) (fatal error) rME0 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE10_DESCR = "(rmu_ecc_ue10) (fatal error) rME1 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE11_DESCR = "(rmu_ecc_ue11) (fatal error) rME1 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE12_DESCR = "(rmu_ecc_ue12) (fatal error) rME2 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE13_DESCR = "(rmu_ecc_ue13) (fatal error) rME2 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE14_DESCR = "(rmu_ecc_ue14) (fatal error) rME3 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE15_DESCR = "(rmu_ecc_ue15) (fatal error) rME3 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE16_DESCR = "(rmu_ecc_ue16) (fatal error) rME4 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE17_DESCR = "(rmu_ecc_ue17) (fatal error) rME4 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE18_DESCR = "(rmu_ecc_ue18) (fatal error) rME5 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE19_DESCR = "(rmu_ecc_ue19) (fatal error) rME5 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE20_DESCR = "(rmu_ecc_ue20) (fatal error) rME6 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE21_DESCR = "(rmu_ecc_ue21) (fatal error) rME6 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE22_DESCR = "(rmu_ecc_ue22) (fatal error) rME7 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE23_DESCR = "(rmu_ecc_ue23) (fatal error) rME7 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE24_DESCR = "(rmu_ecc_ue24) (fatal error) rME8 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE25_DESCR = "(rmu_ecc_ue25) (fatal error) rME8 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE26_DESCR = "(rmu_ecc_ue26) (fatal error) rME9 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE27_DESCR = "(rmu_ecc_ue27) (fatal error) rME9 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE28_DESCR = "(rmu_ecc_ue28) (fatal error) rME10 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE29_DESCR = "(rmu_ecc_ue29) (fatal error) rME10 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE30_DESCR = "(rmu_ecc_ue30) (fatal error) rME11 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE31_DESCR = "(rmu_ecc_ue31) (fatal error) rME11 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE32_DESCR = "(rmu_ecc_ue32) (fatal error) rME12 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE33_DESCR = "(rmu_ecc_ue33) (fatal error) rME12 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE34_DESCR = "(rmu_ecc_ue34) (fatal error) rME13 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE35_DESCR = "(rmu_ecc_ue35) (fatal error) rME13 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE36_DESCR = "(rmu_ecc_ue36) (fatal error) rME14 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE37_DESCR = "(rmu_ecc_ue37) (fatal error) rME14 previous 4 bytes network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE38_DESCR = "(rmu_ecc_ue38) (fatal error) rME15 network data latch uncorrectable ECC error. ";
  const std::string RMU_ECC_UE39_DESCR = "(rmu_ecc_ue39) (fatal error) rME15 previous 4 bytes network data latch uncorrectable ECC error. ";

void decode_rmu_ecc_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " RMU_ECC_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // rmu_ecc_ue0
    str << RMU_ECC_UE0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // rmu_ecc_ue1
    str << RMU_ECC_UE1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // rmu_ecc_ue2
    str << RMU_ECC_UE2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // rmu_ecc_ue3
    str << RMU_ECC_UE3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // rmu_ecc_ue4
    str << RMU_ECC_UE4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // rmu_ecc_ue5
    str << RMU_ECC_UE5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // rmu_ecc_ue6
    str << RMU_ECC_UE6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // rmu_ecc_ue7
    str << RMU_ECC_UE7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // rmu_ecc_ue8
    str << RMU_ECC_UE8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // rmu_ecc_ue9
    str << RMU_ECC_UE9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // rmu_ecc_ue10
    str << RMU_ECC_UE10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // rmu_ecc_ue11
    str << RMU_ECC_UE11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // rmu_ecc_ue12
    str << RMU_ECC_UE12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // rmu_ecc_ue13
    str << RMU_ECC_UE13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // rmu_ecc_ue14
    str << RMU_ECC_UE14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // rmu_ecc_ue15
    str << RMU_ECC_UE15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // rmu_ecc_ue16
    str << RMU_ECC_UE16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // rmu_ecc_ue17
    str << RMU_ECC_UE17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // rmu_ecc_ue18
    str << RMU_ECC_UE18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // rmu_ecc_ue19
    str << RMU_ECC_UE19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // rmu_ecc_ue20
    str << RMU_ECC_UE20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // rmu_ecc_ue21
    str << RMU_ECC_UE21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // rmu_ecc_ue22
    str << RMU_ECC_UE22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // rmu_ecc_ue23
    str << RMU_ECC_UE23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // rmu_ecc_ue24
    str << RMU_ECC_UE24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // rmu_ecc_ue25
    str << RMU_ECC_UE25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // rmu_ecc_ue26
    str << RMU_ECC_UE26_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // rmu_ecc_ue27
    str << RMU_ECC_UE27_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // rmu_ecc_ue28
    str << RMU_ECC_UE28_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // rmu_ecc_ue29
    str << RMU_ECC_UE29_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // rmu_ecc_ue30
    str << RMU_ECC_UE30_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // rmu_ecc_ue31
    str << RMU_ECC_UE31_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // rmu_ecc_ue32
    str << RMU_ECC_UE32_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // rmu_ecc_ue33
    str << RMU_ECC_UE33_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // rmu_ecc_ue34
    str << RMU_ECC_UE34_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-35) ) ) != 0 ) // rmu_ecc_ue35
    str << RMU_ECC_UE35_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-36) ) ) != 0 ) // rmu_ecc_ue36
    str << RMU_ECC_UE36_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-37) ) ) != 0 ) // rmu_ecc_ue37
    str << RMU_ECC_UE37_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-38) ) ) != 0 ) // rmu_ecc_ue38
    str << RMU_ECC_UE38_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-39) ) ) != 0 ) // rmu_ecc_ue39
    str << RMU_ECC_UE39_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string MCSRAM_ERR0_DESCR = "(mcsram_err0) (fatal error) Control flag latches parity error. ";
  const std::string MCSRAM_ERR1_DESCR = "(mcsram_err1) (fatal error) XM--MCSRAM 1st cycle write data latch parity error. ";
  const std::string MCSRAM_ERR2_DESCR = "(mcsram_err2) (fatal error) XM--MCSRAM 2nd cycle write data latch parity error. ";
  const std::string MCSRAM_ERR3_DESCR = "(mcsram_err3) (fatal error) XM--MCSRAM write index latch parity error. ";
  const std::string MCSRAM_ERR4_DESCR = "(mcsram_err4) (fatal error) SRAM write index latch parity error. ";
  const std::string MCSRAM_ERR5_DESCR = "(mcsram_err5) (fatal error) Descriptor output latch parity error. ";
  const std::string MCSRAM_ERR6_DESCR = "(mcsram_err6) (fatal error) Header output buffer iME ID latch parity error. ";
  const std::string MCSRAM_ERR7_DESCR = "(mcsram_err7) (fatal error) iME request stage 1 latch parity error. ";
  const std::string MCSRAM_ERR8_DESCR = "(mcsram_err8) (fatal error) iME request stage 2 latch parity error. ";
  const std::string MCSRAM_ERR9_DESCR = "(mcsram_err9) (fatal error) iME request stage 3 latch parity error. ";
  const std::string MCSRAM_ERR10_DESCR = "(mcsram_err10) (fatal error) MCSRAM to arbitration logic ready flag and FIFO map set request latch parity error. ";
  const std::string MCSRAM_ERR11_DESCR = "(mcsram_err11) (fatal error) ICSRAM increment index latch parity error. ";
  const std::string MCSRAM_ERR12_DESCR = "(mcsram_err12) (fatal error) Backdoor command latch parity error . ";
  const std::string MCSRAM_ERR13_DESCR = "(mcsram_err13) (fatal error) 2nd XM data return missing. ";
  const std::string MCSRAM_ERR14_DESCR = "(mcsram_err14) (fatal error) Header buffer overflow. ";
  const std::string MCSRAM_ERR15_DESCR = "(mcsram_err15) (software error) Remote get packet payload address is atomic. ";
  const std::string MCSRAM_ERR16_DESCR = "(mcsram_err16) (software error) Payload size gt 512B. ";
  const std::string MCSRAM_ERR17_DESCR = "(mcsram_err17) (software error) Payload size = 0 for R-put or payload size lt 64B for R-get. ";
  const std::string MCSRAM_ERR18_DESCR = "(mcsram_err18) (software error) Payload size != 64N for R-get. ";
  const std::string MCSRAM_ERR19_DESCR = "(mcsram_err19) (software error) Payload size = 0 for non-zero message length. ";
  const std::string MCSRAM_ERR20_DESCR = "(mcsram_err20) (software error) Payload address starts in non-atomic area but ends in atomic area. ";
  const std::string MCSRAM_ERR21_DESCR = "(mcsram_err21) (software error) Message len != 64N for R-get. ";
  const std::string MCSRAM_ERR22_DESCR = "(mcsram_err22) (software error) Message len != 8 for atomic payload address. ";
  const std::string MCSRAM_ERR23_DESCR = "(mcsram_err23) (software error) Message len = 0 for R-put, or message len lt 64B for R-get. ";
  const std::string MCSRAM_ERR24_DESCR = "(mcsram_err24) (software error) User imFIFO sending system VC packet (VC = 3,6). ";
  const std::string MCSRAM_ERR25_DESCR = "(mcsram_err25) (software error) Invalid MU packet type (type = '11'). ";
  const std::string MCSRAM_ERR26_DESCR = "(mcsram_err26) (software error) User imFIFO specifying system iME in FIFO map. ";
  const std::string MCSRAM_ERR27_DESCR = "(mcsram_err27) (software error) FIFO map is 0 (i. ";
  const std::string MCSRAM_ERR28_DESCR = "(mcsram_err28) (non-fatal error) Random bit latch parity error in MCSRAM descriptor read request arbitor. ";
  const std::string MCSRAM_ERR29_DESCR = "(mcsram_err29) (non-fatal error) Random bit latch parity error in MCSRAM packet end request arbitor. ";
  const std::string MCSRAM_ERR30_DESCR = "(mcsram_err30) (non-fatal error) Random bit latch parity error in MCSRAM message end request arbitor. ";
  const std::string MCSRAM_DD2_ERR0_DESCR = "(mcsram_dd2_err0) (software error DD2 only) Invalid VC (VC=7 is undefined). ";
  const std::string MCSRAM_DD2_ERR1_DESCR = "(mcsram_dd2_err1) (software error DD2 only) Put offset wrapped (put offset + msg len overflows 36bit limit). ";

void decode_mcsram_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " MCSRAM_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // mcsram_err0
    str << MCSRAM_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // mcsram_err1
    str << MCSRAM_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // mcsram_err2
    str << MCSRAM_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // mcsram_err3
    str << MCSRAM_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // mcsram_err4
    str << MCSRAM_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // mcsram_err5
    str << MCSRAM_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // mcsram_err6
    str << MCSRAM_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // mcsram_err7
    str << MCSRAM_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // mcsram_err8
    str << MCSRAM_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // mcsram_err9
    str << MCSRAM_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // mcsram_err10
    str << MCSRAM_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // mcsram_err11
    str << MCSRAM_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // mcsram_err12
    str << MCSRAM_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // mcsram_err13
    str << MCSRAM_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // mcsram_err14
    str << MCSRAM_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // mcsram_err15
    str << MCSRAM_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // mcsram_err16
    str << MCSRAM_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // mcsram_err17
    str << MCSRAM_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // mcsram_err18
    str << MCSRAM_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // mcsram_err19
    str << MCSRAM_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // mcsram_err20
    str << MCSRAM_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // mcsram_err21
    str << MCSRAM_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // mcsram_err22
    str << MCSRAM_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // mcsram_err23
    str << MCSRAM_ERR23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // mcsram_err24
    str << MCSRAM_ERR24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // mcsram_err25
    str << MCSRAM_ERR25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // mcsram_err26
    str << MCSRAM_ERR26_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // mcsram_err27
    str << MCSRAM_ERR27_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // mcsram_err28
    str << MCSRAM_ERR28_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // mcsram_err29
    str << MCSRAM_ERR29_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // mcsram_err30
    str << MCSRAM_ERR30_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // mcsram_dd2_err0
    str << MCSRAM_DD2_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // mcsram_dd2_err1
    str << MCSRAM_DD2_ERR1_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string IME_ERR0_DESCR = "(ime_err0) (fatal error) iME--XM request valid flag latch parity error. ";
  const std::string IME_ERR1_DESCR = "(ime_err1) (fatal error) XM--iME return address latch parity error. ";
  const std::string IME_ERR2_DESCR = "(ime_err2) (fatal error) XM--iME return data valid flag latch parity error. ";
  const std::string IME_ERR3_DESCR = "(ime_err3) (fatal error) iME0 parity error. ";
  const std::string IME_ERR4_DESCR = "(ime_err4) (fatal error) iME0 invalid state entered. ";
  const std::string IME_ERR5_DESCR = "(ime_err5) (fatal error) iME1 parity error. ";
  const std::string IME_ERR6_DESCR = "(ime_err6) (fatal error) iME1 invalid state entered. ";
  const std::string IME_ERR7_DESCR = "(ime_err7) (fatal error) iME2 parity error. ";
  const std::string IME_ERR8_DESCR = "(ime_err8) (fatal error) iME2 invalid state entered. ";
  const std::string IME_ERR9_DESCR = "(ime_err9) (fatal error) iME3 parity error. ";
  const std::string IME_ERR10_DESCR = "(ime_err10) (fatal error) iME3 invalid state entered. ";
  const std::string IME_ERR11_DESCR = "(ime_err11) (fatal error) iME4 parity error. ";
  const std::string IME_ERR12_DESCR = "(ime_err12) (fatal error) iME4 invalid state entered. ";
  const std::string IME_ERR13_DESCR = "(ime_err13) (fatal error) iME5 parity error. ";
  const std::string IME_ERR14_DESCR = "(ime_err14) (fatal error) iME5 invalid state entered. ";
  const std::string IME_ERR15_DESCR = "(ime_err15) (fatal error) iME6 parity error. ";
  const std::string IME_ERR16_DESCR = "(ime_err16) (fatal error) iME6 invalid state entered. ";
  const std::string IME_ERR17_DESCR = "(ime_err17) (fatal error) iME7 parity error. ";
  const std::string IME_ERR18_DESCR = "(ime_err18) (fatal error) iME7 invalid state entered. ";
  const std::string IME_ERR19_DESCR = "(ime_err19) (fatal error) iME8 parity error. ";
  const std::string IME_ERR20_DESCR = "(ime_err20) (fatal error) iME8 invalid state entered. ";
  const std::string IME_ERR21_DESCR = "(ime_err21) (fatal error) iME9 parity error. ";
  const std::string IME_ERR22_DESCR = "(ime_err22) (fatal error) iME9 invalid state entered. ";
  const std::string IME_ERR23_DESCR = "(ime_err23) (fatal error) iME10 parity error. ";
  const std::string IME_ERR24_DESCR = "(ime_err24) (fatal error) iME10 invalid state entered. ";
  const std::string IME_ERR25_DESCR = "(ime_err25) (fatal error) iME11 parity error. ";
  const std::string IME_ERR26_DESCR = "(ime_err26) (fatal error) iME11 invalid state entered. ";
  const std::string IME_ERR27_DESCR = "(ime_err27) (fatal error) iME12 parity error. ";
  const std::string IME_ERR28_DESCR = "(ime_err28) (fatal error) iME12 invalid state entered. ";
  const std::string IME_ERR29_DESCR = "(ime_err29) (fatal error) iME13 parity error. ";
  const std::string IME_ERR30_DESCR = "(ime_err30) (fatal error) iME13 invalid state entered. ";
  const std::string IME_ERR31_DESCR = "(ime_err31) (fatal error) iME14 parity error. ";
  const std::string IME_ERR32_DESCR = "(ime_err32) (fatal error) iME14 invalid state entered. ";
  const std::string IME_ERR33_DESCR = "(ime_err33) (fatal error) iME15 parity error. ";
  const std::string IME_ERR34_DESCR = "(ime_err34) (fatal error) iME15 invalid state entered. ";

void decode_ime_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " IME_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // ime_err0
    str << IME_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // ime_err1
    str << IME_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // ime_err2
    str << IME_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // ime_err3
    str << IME_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // ime_err4
    str << IME_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // ime_err5
    str << IME_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // ime_err6
    str << IME_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // ime_err7
    str << IME_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // ime_err8
    str << IME_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // ime_err9
    str << IME_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // ime_err10
    str << IME_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // ime_err11
    str << IME_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // ime_err12
    str << IME_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // ime_err13
    str << IME_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // ime_err14
    str << IME_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // ime_err15
    str << IME_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // ime_err16
    str << IME_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // ime_err17
    str << IME_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // ime_err18
    str << IME_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // ime_err19
    str << IME_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // ime_err20
    str << IME_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // ime_err21
    str << IME_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // ime_err22
    str << IME_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // ime_err23
    str << IME_ERR23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // ime_err24
    str << IME_ERR24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // ime_err25
    str << IME_ERR25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // ime_err26
    str << IME_ERR26_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // ime_err27
    str << IME_ERR27_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // ime_err28
    str << IME_ERR28_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // ime_err29
    str << IME_ERR29_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // ime_err30
    str << IME_ERR30_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // ime_err31
    str << IME_ERR31_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // ime_err32
    str << IME_ERR32_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // ime_err33
    str << IME_ERR33_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // ime_err34
    str << IME_ERR34_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RME_ERR49_DESCR = "(rme_err49) (fatal error) rME8 parity error. ";
  const std::string RME_ERR50_DESCR = "(rme_err50) (fatal error) rME8 invalid state has been reached. ";
  const std::string RME_ERR51_DESCR = "(rme_err51) (fatal error) rME8 alignment buffer overflow. ";
  const std::string RME_ERR52_DESCR = "(rme_err52) (fatal error) rME8 network reception FIFO read timeout. ";
  const std::string RME_ERR53_DESCR = "(rme_err53) (software error) rME8 packet header field error. ";
  const std::string RME_ERR54_DESCR = "(rme_err54) (non-fatal error) rME8 parity error in FIFO read timeout count. ";
  const std::string RME_ERR55_DESCR = "(rme_err55) (fatal error) rME9 parity error. ";
  const std::string RME_ERR56_DESCR = "(rme_err56) (fatal error) rME9 invalid state has been reached. ";
  const std::string RME_ERR57_DESCR = "(rme_err57) (fatal error) rME9 alignment buffer overflow. ";
  const std::string RME_ERR58_DESCR = "(rme_err58) (fatal error) rME9 network reception FIFO read timeout. ";
  const std::string RME_ERR59_DESCR = "(rme_err59) (software error) rME9 packet header field error. ";
  const std::string RME_ERR60_DESCR = "(rme_err60) (non-fatal error) rME9 parity error in FIFO read timeout count. ";
  const std::string RME_ERR61_DESCR = "(rme_err61) (fatal error) rME10 parity error. ";
  const std::string RME_ERR62_DESCR = "(rme_err62) (fatal error) rME10 invalid state has been reached. ";
  const std::string RME_ERR63_DESCR = "(rme_err63) (fatal error) rME10 alignment buffer overflow. ";
  const std::string RME_ERR64_DESCR = "(rme_err64) (fatal error) rME10 network reception FIFO read timeout. ";
  const std::string RME_ERR65_DESCR = "(rme_err65) (software error) rME10 packet header field error. ";
  const std::string RME_ERR66_DESCR = "(rme_err66) (non-fatal error) rME10 parity error in FIFO read timeout count. ";
  const std::string RME_ERR67_DESCR = "(rme_err67) (fatal error) rME11 parity error. ";
  const std::string RME_ERR68_DESCR = "(rme_err68) (fatal error) rME11 invalid state has been reached. ";
  const std::string RME_ERR69_DESCR = "(rme_err69) (fatal error) rME11 alignment buffer overflow. ";
  const std::string RME_ERR70_DESCR = "(rme_err70) (fatal error) rME11 network reception FIFO read timeout. ";
  const std::string RME_ERR71_DESCR = "(rme_err71) (software error) rME11 packet header field error. ";
  const std::string RME_ERR72_DESCR = "(rme_err72) (non-fatal error) rME11 parity error in FIFO read timeout count. ";
  const std::string RME_ERR73_DESCR = "(rme_err73) (fatal error) rME12 parity error. ";
  const std::string RME_ERR74_DESCR = "(rme_err74) (fatal error) rME12 invalid state has been reached. ";
  const std::string RME_ERR75_DESCR = "(rme_err75) (fatal error) rME12 alignment buffer overflow. ";
  const std::string RME_ERR76_DESCR = "(rme_err76) (fatal error) rME12 network reception FIFO read timeout. ";
  const std::string RME_ERR77_DESCR = "(rme_err77) (software error) rME12 packet header field error. ";
  const std::string RME_ERR78_DESCR = "(rme_err78) (non-fatal error) rME12 parity error in FIFO read timeout count. ";
  const std::string RME_ERR79_DESCR = "(rme_err79) (fatal error) rME13 parity error. ";
  const std::string RME_ERR80_DESCR = "(rme_err80) (fatal error) rME13 invalid state has been reached. ";
  const std::string RME_ERR81_DESCR = "(rme_err81) (fatal error) rME13 alignment buffer overflow. ";
  const std::string RME_ERR82_DESCR = "(rme_err82) (fatal error) rME13 network reception FIFO read timeout. ";
  const std::string RME_ERR83_DESCR = "(rme_err83) (software error) rME13 packet header field error. ";
  const std::string RME_ERR84_DESCR = "(rme_err84) (non-fatal error) rME13 parity error in FIFO read timeout count. ";
  const std::string RME_ERR85_DESCR = "(rme_err85) (fatal error) rME14 parity error. ";
  const std::string RME_ERR86_DESCR = "(rme_err86) (fatal error) rME14 invalid state has been reached. ";
  const std::string RME_ERR87_DESCR = "(rme_err87) (fatal error) rME14 alignment buffer overflow. ";
  const std::string RME_ERR88_DESCR = "(rme_err88) (fatal error) rME14 network reception FIFO read timeout. ";
  const std::string RME_ERR89_DESCR = "(rme_err89) (software error) rME14 packet header field error. ";
  const std::string RME_ERR90_DESCR = "(rme_err90) (non-fatal error) rME14 parity error in FIFO read timeout count. ";
  const std::string RME_ERR91_DESCR = "(rme_err91) (fatal error) rME15 parity error. ";
  const std::string RME_ERR92_DESCR = "(rme_err92) (fatal error) rME15 invalid state has been reached. ";
  const std::string RME_ERR93_DESCR = "(rme_err93) (fatal error) rME15 alignment buffer overflow. ";
  const std::string RME_ERR94_DESCR = "(rme_err94) (fatal error) rME15 network reception FIFO read timeout. ";
  const std::string RME_ERR95_DESCR = "(rme_err95) (software error) rME15 packet header field error. ";
  const std::string RME_ERR96_DESCR = "(rme_err96) (non-fatal error) rME15 parity error in FIFO read timeout count. ";

void decode_rme_interrupts1( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " RME_INTERRUPTS1 : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // rme_err49
    str << RME_ERR49_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // rme_err50
    str << RME_ERR50_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // rme_err51
    str << RME_ERR51_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // rme_err52
    str << RME_ERR52_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // rme_err53
    str << RME_ERR53_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // rme_err54
    str << RME_ERR54_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // rme_err55
    str << RME_ERR55_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // rme_err56
    str << RME_ERR56_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // rme_err57
    str << RME_ERR57_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // rme_err58
    str << RME_ERR58_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // rme_err59
    str << RME_ERR59_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // rme_err60
    str << RME_ERR60_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // rme_err61
    str << RME_ERR61_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // rme_err62
    str << RME_ERR62_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // rme_err63
    str << RME_ERR63_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // rme_err64
    str << RME_ERR64_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // rme_err65
    str << RME_ERR65_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // rme_err66
    str << RME_ERR66_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // rme_err67
    str << RME_ERR67_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // rme_err68
    str << RME_ERR68_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // rme_err69
    str << RME_ERR69_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // rme_err70
    str << RME_ERR70_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // rme_err71
    str << RME_ERR71_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // rme_err72
    str << RME_ERR72_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // rme_err73
    str << RME_ERR73_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // rme_err74
    str << RME_ERR74_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // rme_err75
    str << RME_ERR75_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // rme_err76
    str << RME_ERR76_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // rme_err77
    str << RME_ERR77_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // rme_err78
    str << RME_ERR78_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // rme_err79
    str << RME_ERR79_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // rme_err80
    str << RME_ERR80_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // rme_err81
    str << RME_ERR81_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // rme_err82
    str << RME_ERR82_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // rme_err83
    str << RME_ERR83_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-35) ) ) != 0 ) // rme_err84
    str << RME_ERR84_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-36) ) ) != 0 ) // rme_err85
    str << RME_ERR85_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-37) ) ) != 0 ) // rme_err86
    str << RME_ERR86_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-38) ) ) != 0 ) // rme_err87
    str << RME_ERR87_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-39) ) ) != 0 ) // rme_err88
    str << RME_ERR88_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-40) ) ) != 0 ) // rme_err89
    str << RME_ERR89_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-41) ) ) != 0 ) // rme_err90
    str << RME_ERR90_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-42) ) ) != 0 ) // rme_err91
    str << RME_ERR91_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-43) ) ) != 0 ) // rme_err92
    str << RME_ERR92_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-44) ) ) != 0 ) // rme_err93
    str << RME_ERR93_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-45) ) ) != 0 ) // rme_err94
    str << RME_ERR94_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-46) ) ) != 0 ) // rme_err95
    str << RME_ERR95_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-47) ) ) != 0 ) // rme_err96
    str << RME_ERR96_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RME_ERR0_DESCR = "(rme_err0) (fatal error) rME--xm request valid flag latch parity error. ";
  const std::string RME_ERR1_DESCR = "(rme_err1) (fatal error) rME0 parity error. ";
  const std::string RME_ERR2_DESCR = "(rme_err2) (fatal error) rME0 invalid state has been reached. ";
  const std::string RME_ERR3_DESCR = "(rme_err3) (fatal error) rME0 alignment buffer overflow. ";
  const std::string RME_ERR4_DESCR = "(rme_err4) (fatal error) rME0 network reception FIFO read timeout. ";
  const std::string RME_ERR5_DESCR = "(rme_err5) (software error) rME0 packet header field error. ";
  const std::string RME_ERR6_DESCR = "(rme_err6) (non-fatal error) rME0 parity error in FIFO read timeout count. ";
  const std::string RME_ERR7_DESCR = "(rme_err7) (fatal error) rME1 parity error. ";
  const std::string RME_ERR8_DESCR = "(rme_err8) (fatal error) rME1 invalid state has been reached. ";
  const std::string RME_ERR9_DESCR = "(rme_err9) (fatal error) rME1 alignment buffer overflow. ";
  const std::string RME_ERR10_DESCR = "(rme_err10) (fatal error) rME1 network reception FIFO read timeout. ";
  const std::string RME_ERR11_DESCR = "(rme_err11) (software error) rME1 packet header field error. ";
  const std::string RME_ERR12_DESCR = "(rme_err12) (non-fatal error) rME1 parity error in FIFO read timeout count. ";
  const std::string RME_ERR13_DESCR = "(rme_err13) (fatal error) rME2 parity error. ";
  const std::string RME_ERR14_DESCR = "(rme_err14) (fatal error) rME2 invalid state has been reached. ";
  const std::string RME_ERR15_DESCR = "(rme_err15) (fatal error) rME2 alignment buffer overflow. ";
  const std::string RME_ERR16_DESCR = "(rme_err16) (fatal error) rME2 network reception FIFO read timeout. ";
  const std::string RME_ERR17_DESCR = "(rme_err17) (software error) rME2 packet header field error. ";
  const std::string RME_ERR18_DESCR = "(rme_err18) (non-fatal error) rME2 parity error in FIFO read timeout count. ";
  const std::string RME_ERR19_DESCR = "(rme_err19) (fatal error) rME3 parity error. ";
  const std::string RME_ERR20_DESCR = "(rme_err20) (fatal error) rME3 invalid state has been reached. ";
  const std::string RME_ERR21_DESCR = "(rme_err21) (fatal error) rME3 alignment buffer overflow. ";
  const std::string RME_ERR22_DESCR = "(rme_err22) (fatal error) rME3 network reception FIFO read timeout. ";
  const std::string RME_ERR23_DESCR = "(rme_err23) (software error) rME3 packet header field error. ";
  const std::string RME_ERR24_DESCR = "(rme_err24) (non-fatal error) rME3 parity error in FIFO read timeout count. ";
  const std::string RME_ERR25_DESCR = "(rme_err25) (fatal error) rME4 parity error. ";
  const std::string RME_ERR26_DESCR = "(rme_err26) (fatal error) rME4 invalid state has been reached. ";
  const std::string RME_ERR27_DESCR = "(rme_err27) (fatal error) rME4 alignment buffer overflow. ";
  const std::string RME_ERR28_DESCR = "(rme_err28) (fatal error) rME4 network reception FIFO read timeout. ";
  const std::string RME_ERR29_DESCR = "(rme_err29) (software error) rME4 packet header field error. ";
  const std::string RME_ERR30_DESCR = "(rme_err30) (non-fatal error) rME4 parity error in FIFO read timeout count. ";
  const std::string RME_ERR31_DESCR = "(rme_err31) (fatal error) rME5 parity error. ";
  const std::string RME_ERR32_DESCR = "(rme_err32) (fatal error) rME5 invalid state has been reached. ";
  const std::string RME_ERR33_DESCR = "(rme_err33) (fatal error) rME5 alignment buffer overflow. ";
  const std::string RME_ERR34_DESCR = "(rme_err34) (fatal error) rME5 network reception FIFO read timeout. ";
  const std::string RME_ERR35_DESCR = "(rme_err35) (software error) rME5 packet header field error. ";
  const std::string RME_ERR36_DESCR = "(rme_err36) (non-fatal error) rME5 parity error in FIFO read timeout count. ";
  const std::string RME_ERR37_DESCR = "(rme_err37) (fatal error) rME6 parity error. ";
  const std::string RME_ERR38_DESCR = "(rme_err38) (fatal error) rME6 invalid state has been reached. ";
  const std::string RME_ERR39_DESCR = "(rme_err39) (fatal error) rME6 alignment buffer overflow. ";
  const std::string RME_ERR40_DESCR = "(rme_err40) (fatal error) rME6 network reception FIFO read timeout. ";
  const std::string RME_ERR41_DESCR = "(rme_err41) (software error) rME6 packet header field error. ";
  const std::string RME_ERR42_DESCR = "(rme_err42) (non-fatal error) rME6 parity error in FIFO read timeout count. ";
  const std::string RME_ERR43_DESCR = "(rme_err43) (fatal error) rME7 parity error. ";
  const std::string RME_ERR44_DESCR = "(rme_err44) (fatal error) rME7 invalid state has been reached. ";
  const std::string RME_ERR45_DESCR = "(rme_err45) (fatal error) rME7 alignment buffer overflow. ";
  const std::string RME_ERR46_DESCR = "(rme_err46) (fatal error) rME7 network reception FIFO read timeout. ";
  const std::string RME_ERR47_DESCR = "(rme_err47) (software error) rME7 packet header field error. ";
  const std::string RME_ERR48_DESCR = "(rme_err48) (non-fatal error) rME7 parity error in FIFO read timeout count. ";

void decode_rme_interrupts0( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " RME_INTERRUPTS0 : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // rme_err0
    str << RME_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // rme_err1
    str << RME_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // rme_err2
    str << RME_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // rme_err3
    str << RME_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // rme_err4
    str << RME_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // rme_err5
    str << RME_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // rme_err6
    str << RME_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // rme_err7
    str << RME_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // rme_err8
    str << RME_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // rme_err9
    str << RME_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // rme_err10
    str << RME_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // rme_err11
    str << RME_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // rme_err12
    str << RME_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // rme_err13
    str << RME_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // rme_err14
    str << RME_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // rme_err15
    str << RME_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // rme_err16
    str << RME_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // rme_err17
    str << RME_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // rme_err18
    str << RME_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // rme_err19
    str << RME_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // rme_err20
    str << RME_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // rme_err21
    str << RME_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // rme_err22
    str << RME_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // rme_err23
    str << RME_ERR23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // rme_err24
    str << RME_ERR24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // rme_err25
    str << RME_ERR25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // rme_err26
    str << RME_ERR26_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // rme_err27
    str << RME_ERR27_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // rme_err28
    str << RME_ERR28_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // rme_err29
    str << RME_ERR29_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // rme_err30
    str << RME_ERR30_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // rme_err31
    str << RME_ERR31_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // rme_err32
    str << RME_ERR32_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // rme_err33
    str << RME_ERR33_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // rme_err34
    str << RME_ERR34_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-35) ) ) != 0 ) // rme_err35
    str << RME_ERR35_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-36) ) ) != 0 ) // rme_err36
    str << RME_ERR36_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-37) ) ) != 0 ) // rme_err37
    str << RME_ERR37_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-38) ) ) != 0 ) // rme_err38
    str << RME_ERR38_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-39) ) ) != 0 ) // rme_err39
    str << RME_ERR39_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-40) ) ) != 0 ) // rme_err40
    str << RME_ERR40_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-41) ) ) != 0 ) // rme_err41
    str << RME_ERR41_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-42) ) ) != 0 ) // rme_err42
    str << RME_ERR42_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-43) ) ) != 0 ) // rme_err43
    str << RME_ERR43_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-44) ) ) != 0 ) // rme_err44
    str << RME_ERR44_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-45) ) ) != 0 ) // rme_err45
    str << RME_ERR45_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-46) ) ) != 0 ) // rme_err46
    str << RME_ERR46_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-47) ) ) != 0 ) // rme_err47
    str << RME_ERR47_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-48) ) ) != 0 ) // rme_err48
    str << RME_ERR48_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string ICSRAM_ERR0_DESCR = "(icsram_err0) (fatal error) start_ecc_err - uncorrectable ECC error on start field, can be ignored until field is initialized. ";
  const std::string ICSRAM_ERR1_DESCR = "(icsram_err1) (fatal error) size_ecc_err - uncorrectable ECC error on size field, can be ignored until field is initialized. ";
  const std::string ICSRAM_ERR2_DESCR = "(icsram_err2) (fatal error) head_ecc_err - uncorrectable ECC error on head field, can be ignored until field is initialized. ";
  const std::string ICSRAM_ERR3_DESCR = "(icsram_err3) (fatal error) tail_ecc_err - uncorrectable ECC error on tail field, can be ignored until field is initialized. ";
  const std::string ICSRAM_ERR4_DESCR = "(icsram_err4) (fatal error) desc_ecc_err - uncorrectable ECC error on desc field, can be ignored until field is initialized. ";
  const std::string ICSRAM_ERR5_DESCR = "(icsram_err5) (fatal error) free_ecc_err - uncorrectable ECC error on free space field, cannot be ignored. ";
  const std::string ICSRAM_ERR6_DESCR = "(icsram_err6) (fatal error) fsm_p_err - parity error on FSM state registers. ";
  const std::string ICSRAM_ERR7_DESCR = "(icsram_err7) (fatal error) imfifo_not_empty_p_err - parity error on register that tracks if imFIFO has descriptor(s) to fetch. ";
  const std::string ICSRAM_ERR8_DESCR = "(icsram_err8) (fatal error) imfifo_cand_p_err - parity error on imFIFO candidate ID chosen for descriptor fetch. ";
  const std::string ICSRAM_ERR9_DESCR = "(icsram_err9) (fatal error) fetching_desc_p_err - parity error on register that tracks if imFIFO is currently fetching descriptor. ";
  const std::string ICSRAM_ERR10_DESCR = "(icsram_err10) (fatal error) xmreq_fifo_ptr_p_err - parity error on request FIFO pointers that issue descriptor requests to master port. ";
  const std::string ICSRAM_ERR11_DESCR = "(icsram_err11) (fatal error) xmint_head_ecc_err - uncorrectable ECC error on head address for descriptor fetch. ";
  const std::string ICSRAM_ERR12_DESCR = "(icsram_err12) (fatal error) icsram_me_valid_p_err - parity error on valid bit telling rME that read of head/tail/size is done. ";
  const std::string ICSRAM_ERR13_DESCR = "(icsram_err13) (fatal error) rmerd_attr_p_err - parity error on head/tail/size read by rME. ";
  const std::string ICSRAM_ERR14_DESCR = "(icsram_err14) (fatal error) icsram_din_l2_p_err - parity error on icsram DIN latch containing start, size, etc. ";
  const std::string ICSRAM_ERR15_DESCR = "(icsram_err15) (fatal error) rdstart_ecc_err - uncorrectable ECC error on start read for rGET packet. ";
  const std::string ICSRAM_ERR16_DESCR = "(icsram_err16) (fatal error) rdsize_ecc_err - uncorrectable ECC error on size read for rGET packet. ";
  const std::string ICSRAM_ERR17_DESCR = "(icsram_err17) (fatal error) rdtail_ecc_err - uncorrectable ECC error on tail read for rGET packet. ";
  const std::string ICSRAM_ERR18_DESCR = "(icsram_err18) (fatal error) rdfree_ecc_err - uncorrectable ECC error on free space read for rGET packet. ";
  const std::string ICSRAM_ERR19_DESCR = "(icsram_err19) (correctable error) imfifo_cand_p_err_1 - correctable parity error on intermediate register storing imFIFO candidate for descriptor fetch. ";
  const std::string ICSRAM_ERR20_DESCR = "(icsram_err20) (software error) xs_wr_free_err - software attempted to write read-only free space. ";
  const std::string ICSRAM_ERR21_DESCR = "(icsram_err21) (software error) startsizeheadtail_align_atomic_err - start, head, or tail address are not 64B aligned, or size is not 64B-1 aligned, or atomic address was specified. ";
  const std::string ICSRAM_ERR22_DESCR = "(icsram_err22) (software error) startsizeheadtail_range_err - start, head, tail or size are out of range (bits(0 to 26) are non-zero). ";
  const std::string ICSRAM_ERR23_DESCR = "(icsram_err23) (software notification) rget_fifo_full_err - rget FIFO is full, rME is waiting for software to free up space in FIFO. ";

void decode_icsram_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " ICSRAM_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // icsram_err0
    str << ICSRAM_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // icsram_err1
    str << ICSRAM_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // icsram_err2
    str << ICSRAM_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // icsram_err3
    str << ICSRAM_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // icsram_err4
    str << ICSRAM_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // icsram_err5
    str << ICSRAM_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // icsram_err6
    str << ICSRAM_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // icsram_err7
    str << ICSRAM_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // icsram_err8
    str << ICSRAM_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // icsram_err9
    str << ICSRAM_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // icsram_err10
    str << ICSRAM_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // icsram_err11
    str << ICSRAM_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // icsram_err12
    str << ICSRAM_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // icsram_err13
    str << ICSRAM_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // icsram_err14
    str << ICSRAM_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // icsram_err15
    str << ICSRAM_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // icsram_err16
    str << ICSRAM_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // icsram_err17
    str << ICSRAM_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // icsram_err18
    str << ICSRAM_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // icsram_err19
    str << ICSRAM_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // icsram_err20
    str << ICSRAM_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // icsram_err21
    str << ICSRAM_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // icsram_err22
    str << ICSRAM_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // icsram_err23
    str << ICSRAM_ERR23_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RPUTSRAM_ERR0_DESCR = "(rputsram_err0) (fatal error) RPUT SRAM control flag latch parity error. ";
  const std::string RPUTSRAM_ERR1_DESCR = "(rputsram_err1) (fatal error) RPUT SRAM state encoding error. ";
  const std::string RPUTSRAM_ERR2_DESCR = "(rputsram_err2) (fatal error) RPUT SRAM stage 1 request latch parity error. ";
  const std::string RPUTSRAM_ERR3_DESCR = "(rputsram_err3) (non-fatal error) RPUT SRAM stage 2 request latch parity error (recovered by retry). ";
  const std::string RPUTSRAM_ERR4_DESCR = "(rputsram_err4) (fatal error) RPUT SRAM stage 3 request latch parity error. ";
  const std::string RPUTSRAM_ERR5_DESCR = "(rputsram_err5) (fatal error) RPUT SRAM stage 4 request latch parity error. ";
  const std::string RPUTSRAM_ERR6_DESCR = "(rputsram_err6) (non-fatal error) RPUT SRAM backdoor ECC read data latch parity error. ";
  const std::string RPUTSRAM_ERR7_DESCR = "(rputsram_err7) (non-fatal error) RPUT SRAM rME request arbitor random bit latch parity error. ";
  const std::string RPUTSRAM_ERR8_DESCR = "(rputsram_err8) (software error) RPUT SRAM user packet accessing put base but it is system entry. ";
  const std::string RPUTSRAM_ERR9_DESCR = "(rputsram_err9) (software error) RPUT SRAM user packet accessing counter base but it is system entry. ";
  const std::string RPUTSRAM_ERR10_DESCR = "(rputsram_err10) (software error) RPUT SRAM rput/counter address addition overflow. ";
  const std::string RPUTSRAM_ERR11_DESCR = "(rputsram_err11) (software error) RPUT SRAM rput destination area crossing atomic/nonatomic boundary. ";
  const std::string RPUTSRAM_ERR12_DESCR = "(rputsram_err12) (software error) RPUT SRAM data size != 8 for atomic rput address. ";
  const std::string RPUTSRAM_ERR13_DESCR = "(rputsram_err13) (software error) RPUT SRAM rput counter address in non-atomic. ";

void decode_rputsram_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " RPUTSRAM_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // rputsram_err0
    str << RPUTSRAM_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // rputsram_err1
    str << RPUTSRAM_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // rputsram_err2
    str << RPUTSRAM_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // rputsram_err3
    str << RPUTSRAM_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // rputsram_err4
    str << RPUTSRAM_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // rputsram_err5
    str << RPUTSRAM_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // rputsram_err6
    str << RPUTSRAM_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // rputsram_err7
    str << RPUTSRAM_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // rputsram_err8
    str << RPUTSRAM_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // rputsram_err9
    str << RPUTSRAM_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // rputsram_err10
    str << RPUTSRAM_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // rputsram_err11
    str << RPUTSRAM_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // rputsram_err12
    str << RPUTSRAM_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // rputsram_err13
    str << RPUTSRAM_ERR13_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string XS_FATAL_ERR0_DESCR = "(xs_fatal_err0) (fatal error) info_ecc_err - uncorrectable ECC error on switch info field OR single bit error on ttype(0) bit. ";
  const std::string XS_FATAL_ERR1_DESCR = "(xs_fatal_err1) (fatal error) data_bvalids_ecc_err - uncorrectable ECC error on switch data  and  byte valid fields. ";
  const std::string XS_FATAL_ERR2_DESCR = "(xs_fatal_err2) (fatal error) req_valid_src_p_err - parity error on src field. ";
  const std::string XS_FATAL_ERR3_DESCR = "(xs_fatal_err3) (fatal error) src_id_err - src field is >= 18, which means a non-L1p master port is trying to access MU slave. ";
  const std::string XS_FATAL_ERR4_DESCR = "(xs_fatal_err4) (fatal error) req_info_chop_p_err - parity error on a subset of the request info bits. ";
  const std::string XS_FATAL_ERR5_DESCR = "(xs_fatal_err5) (fatal error) dgate_valid_err - req_valid asserted by switch, but dgate not asserted so info/data fields not latched. ";
  const std::string XS_FATAL_ERR6_DESCR = "(xs_fatal_err6) (fatal error) req_ack_err - more than 1 MU subunit ack asserted simultaneously. ";
  const std::string XS_FATAL_ERR7_DESCR = "(xs_fatal_err7) (fatal error) req_fifo_ptr_p_err - parity error on request FIFO pointers. ";
  const std::string XS_FATAL_ERR8_DESCR = "(xs_fatal_err8) (fatal error) req_fifo_full_err - request FIFO is full, should not happen b/c switch tracks MU slave tokens. ";
  const std::string XS_FATAL_ERR9_DESCR = "(xs_fatal_err9) (fatal error) req_fifo_attr_p_err - parity error in request FIFO attributes. ";
  const std::string XS_FATAL_ERR10_DESCR = "(xs_fatal_err10) (fatal error) req_info_p_err - parity error in request info from request buffer. ";
  const std::string XS_FATAL_ERR11_DESCR = "(xs_fatal_err11) (fatal error) sked_eager_err - switch indicated eager schedule to MU slave for read return, but MU slave does not support this. ";
  const std::string XS_FATAL_ERR12_DESCR = "(xs_fatal_err12) (fatal error) sked_p_err - parity error in sked_id on slave read data return side. ";
  const std::string XS_FATAL_ERR13_DESCR = "(xs_fatal_err13) (fatal error) sked_id_err - sked_id is >= 18, which indicates a read return to a non-L1p master port. ";
  const std::string XS_FATAL_ERR14_DESCR = "(xs_fatal_err14) (fatal error) avail_idx_p_err - parity error covering the avail_idx vector. ";
  const std::string XS_FATAL_ERR15_DESCR = "(xs_fatal_err15) (fatal error) alloc_idx_p_err - parity error covering alloc_idx_attr. ";
  const std::string XS_FATAL_ERR16_DESCR = "(xs_fatal_err16) (fatal error) wr_pos_hot_err - wr_pos vector is not one hot, indicating error in write position tracking. ";
  const std::string XS_FATAL_ERR17_DESCR = "(xs_fatal_err17) (fatal error) fifo_dest_p_err - parity error in current request's destination field stored in read return request FIFO. ";
  const std::string XS_FATAL_ERR18_DESCR = "(xs_fatal_err18) (fatal error) lut_valid_p_err - parity error on lut_valid vector. ";
  const std::string XS_FATAL_ERR19_DESCR = "(xs_fatal_err19) (fatal error) lut_attr_p_err - parity error in look-up-table attributes for current read return request going out to switch. ";
  const std::string XS_FATAL_ERR20_DESCR = "(xs_fatal_err20) (fatal error) lut_notvalid_err - look-up-table does not contain a valid entry for switch's requested destination. ";
  const std::string XS_SOFTWARE_ERR21_DESCR = "(xs_software_err21) (software error) bad_op_err - unsupported opcode (ttype) sent to MU slave, see slave error info register. ";
  const std::string XS_SOFTWARE_ERR22_DESCR = "(xs_software_err22) (software error) rd_size_err - read request size is not 8B, see slave error info register. ";
  const std::string XS_SOFTWARE_ERR23_DESCR = "(xs_software_err23) (software error) byte_valid_err - more than 8B of byte valids asserted OR byte valids inconsistent with odd/even address. ";
  const std::string XS_SOFTWARE_ERR24_DESCR = "(xs_software_err24) (software error) addr_range_err - request's address lies outside MU MMIO address space, see slave error info register. ";
  const std::string XS_SOFTWARE_ERR25_DESCR = "(xs_software_err25) (software error) icsram_perm_err - user is attempting to access a system imFIFO. ";
  const std::string XS_SOFTWARE_ERR26_DESCR = "(xs_software_err26) (software error) rcsram_perm_err - user is attempting to access a system rmFIFO. ";
  const std::string XS_SOFTWARE_ERR27_DESCR = "(xs_software_err27) (software error) rputsram_perm_err - user is attempting to access a system rput base address. ";

void decode_slave_port_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " SLAVE_PORT_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // xs_fatal_err0
    str << XS_FATAL_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // xs_fatal_err1
    str << XS_FATAL_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // xs_fatal_err2
    str << XS_FATAL_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // xs_fatal_err3
    str << XS_FATAL_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // xs_fatal_err4
    str << XS_FATAL_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // xs_fatal_err5
    str << XS_FATAL_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // xs_fatal_err6
    str << XS_FATAL_ERR6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // xs_fatal_err7
    str << XS_FATAL_ERR7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // xs_fatal_err8
    str << XS_FATAL_ERR8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // xs_fatal_err9
    str << XS_FATAL_ERR9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // xs_fatal_err10
    str << XS_FATAL_ERR10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // xs_fatal_err11
    str << XS_FATAL_ERR11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // xs_fatal_err12
    str << XS_FATAL_ERR12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // xs_fatal_err13
    str << XS_FATAL_ERR13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // xs_fatal_err14
    str << XS_FATAL_ERR14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // xs_fatal_err15
    str << XS_FATAL_ERR15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // xs_fatal_err16
    str << XS_FATAL_ERR16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // xs_fatal_err17
    str << XS_FATAL_ERR17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // xs_fatal_err18
    str << XS_FATAL_ERR18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // xs_fatal_err19
    str << XS_FATAL_ERR19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // xs_fatal_err20
    str << XS_FATAL_ERR20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // xs_software_err21
    str << XS_SOFTWARE_ERR21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // xs_software_err22
    str << XS_SOFTWARE_ERR22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // xs_software_err23
    str << XS_SOFTWARE_ERR23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // xs_software_err24
    str << XS_SOFTWARE_ERR24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // xs_software_err25
    str << XS_SOFTWARE_ERR25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // xs_software_err26
    str << XS_SOFTWARE_ERR26_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // xs_software_err27
    str << XS_SOFTWARE_ERR27_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RMU_ECC_CE0_DESCR = "(rmu_ecc_ce0) (correctable error) XS--RCSRAM write data ECC correctable error. ";
  const std::string RMU_ECC_CE1_DESCR = "(rmu_ecc_ce1) (correctable error) RCSRAM read data word0 ECC correctable error. ";
  const std::string RMU_ECC_CE2_DESCR = "(rmu_ecc_ce2) (correctable error) RCSRAM read data word1 ECC correctable error. ";
  const std::string RMU_ECC_CE3_DESCR = "(rmu_ecc_ce3) (correctable error) RCSRAM read data word2 ECC correctable error. ";
  const std::string RMU_ECC_CE4_DESCR = "(rmu_ecc_ce4) (correctable error) RCSRAM read data word3 ECC correctable error. ";
  const std::string RMU_ECC_CE5_DESCR = "(rmu_ecc_ce5) (correctable error) RCSRAM read data word4 ECC correctable error. ";
  const std::string RMU_ECC_CE6_DESCR = "(rmu_ecc_ce6) (correctable error) RPUT SRAM read data ECC correctable error. ";
  const std::string RMU_ECC_CE7_DESCR = "(rmu_ecc_ce7) (correctable error) RPUT SRAM write data ECC correctable error. ";
  const std::string RMU_ECC_CE8_DESCR = "(rmu_ecc_ce8) (correctable error) rME0 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE9_DESCR = "(rmu_ecc_ce9) (correctable error) rME0 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE10_DESCR = "(rmu_ecc_ce10) (correctable error) rME1 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE11_DESCR = "(rmu_ecc_ce11) (correctable error) rME1 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE12_DESCR = "(rmu_ecc_ce12) (correctable error) rME2 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE13_DESCR = "(rmu_ecc_ce13) (correctable error) rME2 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE14_DESCR = "(rmu_ecc_ce14) (correctable error) rME3 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE15_DESCR = "(rmu_ecc_ce15) (correctable error) rME3 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE16_DESCR = "(rmu_ecc_ce16) (correctable error) rME4 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE17_DESCR = "(rmu_ecc_ce17) (correctable error) rME4 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE18_DESCR = "(rmu_ecc_ce18) (correctable error) rME5 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE19_DESCR = "(rmu_ecc_ce19) (correctable error) rME5 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE20_DESCR = "(rmu_ecc_ce20) (correctable error) rME6 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE21_DESCR = "(rmu_ecc_ce21) (correctable error) rME6 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE22_DESCR = "(rmu_ecc_ce22) (correctable error) rME7 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE23_DESCR = "(rmu_ecc_ce23) (correctable error) rME7 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE24_DESCR = "(rmu_ecc_ce24) (correctable error) rME8 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE25_DESCR = "(rmu_ecc_ce25) (correctable error) rME8 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE26_DESCR = "(rmu_ecc_ce26) (correctable error) rME9 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE27_DESCR = "(rmu_ecc_ce27) (correctable error) rME9 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE28_DESCR = "(rmu_ecc_ce28) (correctable error) rME10 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE29_DESCR = "(rmu_ecc_ce29) (correctable error) rME10 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE30_DESCR = "(rmu_ecc_ce30) (correctable error) rME11 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE31_DESCR = "(rmu_ecc_ce31) (correctable error) rME11 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE32_DESCR = "(rmu_ecc_ce32) (correctable error) rME12 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE33_DESCR = "(rmu_ecc_ce33) (correctable error) rME12 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE34_DESCR = "(rmu_ecc_ce34) (correctable error) rME13 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE35_DESCR = "(rmu_ecc_ce35) (correctable error) rME13 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE36_DESCR = "(rmu_ecc_ce36) (correctable error) rME14 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE37_DESCR = "(rmu_ecc_ce37) (correctable error) rME14 previous 4 bytes network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE38_DESCR = "(rmu_ecc_ce38) (correctable error) rME15 network data latch correctable ECC error. ";
  const std::string RMU_ECC_CE39_DESCR = "(rmu_ecc_ce39) (correctable error) rME15 previous 4 bytes network data latch correctable ECC error. ";

void decode_rmu_ecc_corr_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "R MU_ECC_CORR_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // rmu_ecc_ce0
    str << RMU_ECC_CE0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // rmu_ecc_ce1
    str << RMU_ECC_CE1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // rmu_ecc_ce2
    str << RMU_ECC_CE2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // rmu_ecc_ce3
    str << RMU_ECC_CE3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // rmu_ecc_ce4
    str << RMU_ECC_CE4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // rmu_ecc_ce5
    str << RMU_ECC_CE5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // rmu_ecc_ce6
    str << RMU_ECC_CE6_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // rmu_ecc_ce7
    str << RMU_ECC_CE7_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // rmu_ecc_ce8
    str << RMU_ECC_CE8_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // rmu_ecc_ce9
    str << RMU_ECC_CE9_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // rmu_ecc_ce10
    str << RMU_ECC_CE10_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // rmu_ecc_ce11
    str << RMU_ECC_CE11_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // rmu_ecc_ce12
    str << RMU_ECC_CE12_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // rmu_ecc_ce13
    str << RMU_ECC_CE13_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // rmu_ecc_ce14
    str << RMU_ECC_CE14_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // rmu_ecc_ce15
    str << RMU_ECC_CE15_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // rmu_ecc_ce16
    str << RMU_ECC_CE16_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // rmu_ecc_ce17
    str << RMU_ECC_CE17_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // rmu_ecc_ce18
    str << RMU_ECC_CE18_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // rmu_ecc_ce19
    str << RMU_ECC_CE19_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // rmu_ecc_ce20
    str << RMU_ECC_CE20_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // rmu_ecc_ce21
    str << RMU_ECC_CE21_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // rmu_ecc_ce22
    str << RMU_ECC_CE22_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // rmu_ecc_ce23
    str << RMU_ECC_CE23_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // rmu_ecc_ce24
    str << RMU_ECC_CE24_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // rmu_ecc_ce25
    str << RMU_ECC_CE25_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // rmu_ecc_ce26
    str << RMU_ECC_CE26_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // rmu_ecc_ce27
    str << RMU_ECC_CE27_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // rmu_ecc_ce28
    str << RMU_ECC_CE28_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // rmu_ecc_ce29
    str << RMU_ECC_CE29_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // rmu_ecc_ce30
    str << RMU_ECC_CE30_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // rmu_ecc_ce31
    str << RMU_ECC_CE31_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // rmu_ecc_ce32
    str << RMU_ECC_CE32_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // rmu_ecc_ce33
    str << RMU_ECC_CE33_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // rmu_ecc_ce34
    str << RMU_ECC_CE34_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-35) ) ) != 0 ) // rmu_ecc_ce35
    str << RMU_ECC_CE35_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-36) ) ) != 0 ) // rmu_ecc_ce36
    str << RMU_ECC_CE36_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-37) ) ) != 0 ) // rmu_ecc_ce37
    str << RMU_ECC_CE37_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-38) ) ) != 0 ) // rmu_ecc_ce38
    str << RMU_ECC_CE38_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-39) ) ) != 0 ) // rmu_ecc_ce39
    str << RMU_ECC_CE39_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string ICSRAM_SELECT_ERR0_DESCR = "(icsram_select_err0) (fatal error) rME-to-ICSRAM arbiter request valid flag latch parity error. ";
  const std::string ICSRAM_SELECT_ERR1_DESCR = "(icsram_select_err1) (fatal error) rME-to-ICSRAM arbiter request info latch parity error. ";
  const std::string ICSRAM_SELECT_ERR2_DESCR = "(icsram_select_err2) (non-fatal error) rME-to-ICSRAM arbiter random bit latch parity error. ";
  const std::string ICSRAM_SELECT_ERR3_DESCR = "(icsram_select_err3) (software error) rME requested system imFIFO for user R-get packet. ";
  const std::string ICSRAM_SELECT_ERR4_DESCR = "(icsram_select_err4) (software error) rME requested imFIFO whose rget flag is not set. ";
  const std::string ICSRAM_SELECT_ERR5_DESCR = "(icsram_select_err5) (software error) rME requested imFIFO not enabled by DCR. ";
  const std::string RAN_ERR0_DESCR = "(ran_err0) (non-fatal error) Random bit generator bit error. ";
  const std::string MU_ERR0_DESCR = "(mu_err0) (fatal error) redun_ctrl signal latch parity error. ";
  const std::string MU_ERR1_DESCR = "(mu_err1) (fatal error) slice_sel_ctrl signal latch parity error. ";
  const std::string UPC_COUNTER_OVF_DESCR = "(upc_counter_ovf) (non-fatal error) UPC counter overflowed, indicating a soft error in counter or a problem with UPC ring. ";

void decode_misc_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " MISC_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // icsram_select_err0
    str << ICSRAM_SELECT_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // icsram_select_err1
    str << ICSRAM_SELECT_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // icsram_select_err2
    str << ICSRAM_SELECT_ERR2_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // icsram_select_err3
    str << ICSRAM_SELECT_ERR3_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // icsram_select_err4
    str << ICSRAM_SELECT_ERR4_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // icsram_select_err5
    str << ICSRAM_SELECT_ERR5_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // ran_err0
    str << RAN_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // mu_err0
    str << MU_ERR0_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // mu_err1
    str << MU_ERR1_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // upc_counter_ovf
    str << UPC_COUNTER_OVF_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RMFIFO_INSUFFICIENT_SPACE_DESCR = "(rmfifo_insufficient_space) (software notification) There is insufficient space in an rmFIFO to receive the next packet. ";

void decode_fifo_interrupts( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << " FIFO_INTERRUPTS : ";


  if ( ( status & ((uint64_t)1 << (63-0-63) ) ) != 0 ) // rmfifo_insufficient_space
    str << RMFIFO_INSUFFICIENT_SPACE_DESCR;
}

