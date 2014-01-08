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
  const std::string            int_list_0_DESCR = " [INT_LIST_0] L1P_int_list_0";
  const std::string            int_list_1_DESCR = " [INT_LIST_1] L1P_int_list_1";
  const std::string            int_list_2_DESCR = " [INT_LIST_2] L1P_int_list_2";
  const std::string            int_list_3_DESCR = " [INT_LIST_3] L1P_int_list_3";
  const std::string            int_list_4_DESCR = " [INT_LIST_4] L1P_int_list_4";
  const std::string     int_speculation_0_DESCR = " [INT_SPECULATION_0] L1P_int_speculation_0";
  const std::string     int_speculation_1_DESCR = " [INT_SPECULATION_1] L1P_int_speculation_1";
  const std::string     int_speculation_2_DESCR = " [INT_SPECULATION_2] L1P_int_speculation_2";
  const std::string     int_speculation_3_DESCR = " [INT_SPECULATION_3] L1P_int_speculation_3";
  const std::string          err_luq_ovfl_DESCR = " [ERR_LUQ_OVFL] fatal error overflow of lookup queue ";
  const std::string              err_sr_p_DESCR = " [ERR_SR_P] parity error on switch response interface";
  const std::string     err_sr_rd_valid_p_DESCR = " [ERR_SR_RD_VALID_P] parity error on rd_valid of switch response interface";
  const std::string              err_sw_p_DESCR = " [ERR_SW_P] parity error on switch write interface";
  const std::string            err_si_ecc_DESCR = " [ERR_SI_ECC] correctable ECC error on switch invalidate interface";
  const std::string         err_si_ecc_ue_DESCR = " [ERR_SI_ECC_UE] uncorrectable ECC error on switch invalidate interface";
  const std::string              err_si_p_DESCR = " [ERR_SI_P] parity error on switch invalidate interface";
  const std::string             err_sda_p_DESCR = " [ERR_SDA_P] benign parity error in store data array";
  const std::string          err_sda_p_ue_DESCR = " [ERR_SDA_P_UE] uncorrectable parity error in store data array";
  const std::string            err_rqra_p_DESCR = " [ERR_RQRA_P] parity error in request array";
  const std::string     err_reload_ecc_x2_DESCR = " [ERR_RELOAD_ECC_X2] correctable reload data ECC error";
  const std::string  err_reload_ecc_ue_x2_DESCR = " [ERR_RELOAD_ECC_UE_X2] uncorrectable reload data ECC error";
  const std::string            err_rira_p_DESCR = " [ERR_RIRA_P] parity error in request info array";
  const std::string            err_gctr_p_DESCR = " [ERR_GCTR_P] parity error on msync global counter";
  const std::string        err_lu_state_p_DESCR = " [ERR_LU_STATE_P] parity error in lookup state";
  const std::string          err_lu_ttype_DESCR = " [ERR_LU_TTYPE] bad ttype for memory region (cache op to IO space)";
  const std::string      err_lu_dcr_abort_DESCR = " [ERR_LU_DCR_ABORT] DCR abort on DCR request from this core";
  const std::string        err_mmio_async_DESCR = " [ERR_MMIO_ASYNC] asynchronous parity error on local MMIO bus (some reg got bit flipped)";
  const std::string      err_mmio_state_p_DESCR = " [ERR_MMIO_STATE_P] parity error on local MMIO master state";
  const std::string      err_mmio_timeout_DESCR = " [ERR_MMIO_TIMEOUT] timeout on local MMIO bus";
  const std::string         err_mmio_priv_DESCR = " [ERR_MMIO_PRIV] privilege access violation attempted on local MMIO bus";
  const std::string      err_mmio_rdata_p_DESCR = " [ERR_MMIO_RDATA_P] parity error on data read on local MMIO bus";
  const std::string      err_mmio_wdata_p_DESCR = " [ERR_MMIO_WDATA_P] parity error on data write on local MMIO bus";
  const std::string err_mmio_dcrs_timeout_DESCR = " [ERR_MMIO_DCRS_TIMEOUT] remote DCR - local MMIO bridge saw timeout";
  const std::string    err_mmio_dcrs_priv_DESCR = " [ERR_MMIO_DCRS_PRIV] remote DCR - local MMIO bridge saw privilege access violation attempt";
  const std::string     err_mmio_dcrs_par_DESCR = " [ERR_MMIO_DCRS_PAR] remote DCR - local MMIO bridge saw parity error";
  const std::string         err_dcrm_crit_DESCR = " [ERR_DCRM_CRIT] critical error from DCR master";
  const std::string      err_dcrm_noncrit_DESCR = " [ERR_DCRM_NONCRIT] non-critical error from DCR master";
  const std::string           err_dcrm_mc_DESCR = " [ERR_DCRM_MC] machine check from DCR master";
  const std::string       err_tag_timeout_DESCR = " [ERR_TAG_TIMEOUT] timeout waiting return of a core tag from memory system";
  const std::string     err_valid_timeout_DESCR = " [ERR_VALID_TIMEOUT] valid request live in l1p for too long ";
  const std::string      err_hold_timeout_DESCR = " [ERR_HOLD_TIMEOUT] lookup queue was held for too long";
  const std::string       err_ditc_req_x2_DESCR = " [ERR_DITC_REQ_X2] core attempted unsupported direct interthread communication";
  const std::string        err_pfd_addr_p_DESCR = " [ERR_PFD_ADDR_P] parity error on address tag in prefetch directory";
  const std::string      err_pfd_avalid_p_DESCR = " [ERR_PFD_AVALID_P] parity error on address valid in prefetch directory";
  const std::string    err_pfd_fill_pnd_p_DESCR = " [ERR_PFD_FILL_PND_P] parity error on fill pnd in prefetch directory";
  const std::string     err_pfd_hit_pnd_p_DESCR = " [ERR_PFD_HIT_PND_P] parity error on hit pnd in prefetch directory";
  const std::string      err_pfd_stream_p_DESCR = " [ERR_PFD_STREAM_P] parity error on stream id in prefetch directory";
  const std::string       err_pfd_depth_p_DESCR = " [ERR_PFD_DEPTH_P] parity error on depth in prefetch directory";
  const std::string       err_pfd_clone_p_DESCR = " [ERR_PFD_CLONE_P] parity error in prefetch directory cloned address";
  const std::string            err_hitq_p_DESCR = " [ERR_HITQ_P] parity error in hitq";
  const std::string              err_sd_p_DESCR = " [ERR_SD_P] parity error in stream detect";
  const std::string          err_pf2dfc_p_DESCR = " [ERR_PF2DFC_P] parity error in prefetch to demand fetch table";
  const std::string         err_wccm_p_x2_DESCR = " [ERR_WCCM_P_X2] parity error in write combine candidate match";
  const std::string     err_wccm_wcd_p_x2_DESCR = " [ERR_WCCM_WCD_P_X2] parity error in write combine decision maker1";
  const std::string          err_lu_wcd_p_DESCR = " [ERR_LU_WCD_P] parity error in write combine decision maker2";
  const std::string      err_lu_current_p_DESCR = " [ERR_LU_CURRENT_P] parity error in lookup request info";
  const std::string             err_l2cmd_DESCR = " [ERR_L2CMD] parity error in l2cmd signal";
  const std::string     err_lu_dcr_dbus_p_DESCR = " [ERR_LU_DCR_DBUS_P] parity error on DCR dbus";
  const std::string             err_luq_p_DESCR = " [ERR_LUQ_P] parity error on lookup queue";
  const std::string       err_sda_phase_p_DESCR = " [ERR_SDA_PHASE_P] parity error in SDA phase register";
  const std::string   slice_sel_ctrl_perr_DESCR = " [SLICE_SEL_CTRL_PERR] parity error on slice select control";
  const std::string       redun_ctrl_perr_DESCR = " [REDUN_CTRL_PERR] parity error on redundancy control";


void _decode_L1P_ESR( ostringstream& str, uint64_t esr ) {

  if ( ( esr & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // int_list_0
    str << int_list_0_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // int_list_1
    str << int_list_1_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // int_list_2
    str << int_list_2_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // int_list_3
    str << int_list_3_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // int_list_4
    str << int_list_4_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // int_speculation_0
    str << int_speculation_0_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // int_speculation_1
    str << int_speculation_1_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // int_speculation_2
    str << int_speculation_2_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // int_speculation_3
    str << int_speculation_3_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // err_luq_ovfl
    str << err_luq_ovfl_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // err_sr_p
    str << err_sr_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // err_sr_rd_valid_p
    str << err_sr_rd_valid_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // err_sw_p
    str << err_sw_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // err_si_ecc
    str << err_si_ecc_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // err_si_ecc_ue
    str << err_si_ecc_ue_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // err_si_p
    str << err_si_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // err_sda_p
    str << err_sda_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // err_sda_p_ue
    str << err_sda_p_ue_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // err_rqra_p
    str << err_rqra_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // err_reload_ecc_x2
    str << err_reload_ecc_x2_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // err_reload_ecc_ue_x2
    str << err_reload_ecc_ue_x2_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // err_rira_p
    str << err_rira_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // err_gctr_p
    str << err_gctr_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // err_lu_state_p
    str << err_lu_state_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // err_lu_ttype
    str << err_lu_ttype_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // err_lu_dcr_abort
    str << err_lu_dcr_abort_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // err_mmio_async
    str << err_mmio_async_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // err_mmio_state_p
    str << err_mmio_state_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // err_mmio_timeout
    str << err_mmio_timeout_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // err_mmio_priv
    str << err_mmio_priv_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // err_mmio_rdata_p
    str << err_mmio_rdata_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // err_mmio_wdata_p
    str << err_mmio_wdata_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // err_mmio_dcrs_timeout
    str << err_mmio_dcrs_timeout_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-35) ) ) != 0 ) // err_mmio_dcrs_priv
    str << err_mmio_dcrs_priv_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-36) ) ) != 0 ) // err_mmio_dcrs_par
    str << err_mmio_dcrs_par_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-37) ) ) != 0 ) // err_dcrm_crit
    str << err_dcrm_crit_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-38) ) ) != 0 ) // err_dcrm_noncrit
    str << err_dcrm_noncrit_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-39) ) ) != 0 ) // err_dcrm_mc
    str << err_dcrm_mc_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-40) ) ) != 0 ) // err_tag_timeout
    str << err_tag_timeout_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-41) ) ) != 0 ) // err_valid_timeout
    str << err_valid_timeout_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-42) ) ) != 0 ) // err_hold_timeout
    str << err_hold_timeout_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-43) ) ) != 0 ) // err_ditc_req_x2
    str << err_ditc_req_x2_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-44) ) ) != 0 ) // err_pfd_addr_p
    str << err_pfd_addr_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-45) ) ) != 0 ) // err_pfd_avalid_p
    str << err_pfd_avalid_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-46) ) ) != 0 ) // err_pfd_fill_pnd_p
    str << err_pfd_fill_pnd_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-47) ) ) != 0 ) // err_pfd_hit_pnd_p
    str << err_pfd_hit_pnd_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-48) ) ) != 0 ) // err_pfd_stream_p
    str << err_pfd_stream_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-49) ) ) != 0 ) // err_pfd_depth_p
    str << err_pfd_depth_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-50) ) ) != 0 ) // err_pfd_clone_p
    str << err_pfd_clone_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-51) ) ) != 0 ) // err_hitq_p
    str << err_hitq_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-52) ) ) != 0 ) // err_sd_p
    str << err_sd_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-53) ) ) != 0 ) // err_pf2dfc_p
    str << err_pf2dfc_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-54) ) ) != 0 ) // err_wccm_p_x2
    str << err_wccm_p_x2_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-55) ) ) != 0 ) // err_wccm_wcd_p_x2
    str << err_wccm_wcd_p_x2_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-56) ) ) != 0 ) // err_lu_wcd_p
    str << err_lu_wcd_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-57) ) ) != 0 ) // err_lu_current_p
    str << err_lu_current_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-58) ) ) != 0 ) // err_l2cmd
    str << err_l2cmd_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-59) ) ) != 0 ) // err_lu_dcr_dbus_p
    str << err_lu_dcr_dbus_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-60) ) ) != 0 ) // err_luq_p
    str << err_luq_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-61) ) ) != 0 ) // err_sda_phase_p
    str << err_sda_phase_p_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-62) ) ) != 0 ) // slice_sel_ctrl_perr
    str << slice_sel_ctrl_perr_DESCR + ";";

  if ( ( esr & ((uint64_t)1 << (63-0-63) ) ) != 0 ) // redun_ctrl_perr
    str << redun_ctrl_perr_DESCR + ";";
}


void decode_L1P_ESR( ostringstream& str, uint64_t esr ) {

  if ( esr == 0 ) // If esr is clear, there is nothing to do.
    return;

  // MANUAL ADD: Also ignore the case where only the MSB is set:

  if ( esr == 0x8000000000000000ull ) 
      return;

  str << "L1P_ESR :";

  _decode_L1P_ESR( str, esr );

}

void decode_L1P_ESR_STATE( ostringstream& str, uint64_t esr, uint32_t core ) {

  if ( esr == 0 ) // If esr is clear, there is nothing to do.
    return;

  // MANUAL ADD: Also ignore the case where only the MSB is set:

  if ( esr == 0x8000000000000000ull ) 
      return;

  str << "L1P_ESR(" << core << ") :";

  _decode_L1P_ESR( str, esr );
}
