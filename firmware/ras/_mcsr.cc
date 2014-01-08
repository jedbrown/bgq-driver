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
const std::string A2_MCSR_DPOVR_DESCR = "(DPOVR) DITC Data Port Overrun Condition ";
const std::string A2_MCSR_DDMH_DESCR = "(DDMH) Data Cache Directory Multihit Error ";
const std::string A2_MCSR_TLBIVAXSR_DESCR = "(TLBIVAXSR) tlbivax Snoop Reject ";
const std::string A2_MCSR_TLBLRUPE_DESCR = "(TLBLRUPE) TLB LRU Parity Error ";
const std::string A2_MCSR_IL2ECC_DESCR = "(IL2ECC) Instruction Cache L2 ECC Error ";
const std::string A2_MCSR_DL2ECC_DESCR = "(DL2ECC) Data Cache L2 ECC Error ";
const std::string A2_MCSR_DDPE_DESCR = "(DDPE) Data Cache Directory Parity Error ";
const std::string A2_MCSR_EXT_DESCR = "(EXT) External Machine Check ";
const std::string A2_MCSR_DCPE_DESCR = "(DCPE) Data Cache Parity Error ";
const std::string A2_MCSR_IEMH_DESCR = "(IEMH) I-ERAT Multi-Hit Error ";
const std::string A2_MCSR_DEMH_DESCR = "(DEMH) D-ERAT Multi-Hit Error ";
const std::string A2_MCSR_TLBMH_DESCR = "(TLBMH) TLB Multi-Hit Error ";
const std::string A2_MCSR_IEPE_DESCR = "(IEPE) I-ERAT Parity Error ";
const std::string A2_MCSR_DEPE_DESCR = "(DEPE) D-ERAT Parity Error ";
const std::string A2_MCSR_TLBPE_DESCR = "(TLBPE) TLB Parity Error ";


void decode_A2_MCSR( ostringstream& str, uint64_t mcsr, uint64_t core ) {

  if ( mcsr == 0 ) // If mcsr is clear, there is nothing to do.
    return;

  if ( core != (uint64_t)-1 ) {
      str << "MCSR(" << core << ") : ";
  }


  if ( ( mcsr & ((uint64_t)1 << (63-0-48) ) ) != 0 ) // DPOVR
    str << A2_MCSR_DPOVR_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-49) ) ) != 0 ) // DDMH
    str << A2_MCSR_DDMH_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-50) ) ) != 0 ) // TLBIVAXSR
    str << A2_MCSR_TLBIVAXSR_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-51) ) ) != 0 ) // TLBLRUPE
    str << A2_MCSR_TLBLRUPE_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-52) ) ) != 0 ) // IL2ECC
    str << A2_MCSR_IL2ECC_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-53) ) ) != 0 ) // DL2ECC
    str << A2_MCSR_DL2ECC_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-54) ) ) != 0 ) // DDPE
    str << A2_MCSR_DDPE_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-55) ) ) != 0 ) // EXT
    str << A2_MCSR_EXT_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-56) ) ) != 0 ) // DCPE
    str << A2_MCSR_DCPE_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-57) ) ) != 0 ) // IEMH
    str << A2_MCSR_IEMH_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-58) ) ) != 0 ) // DEMH
    str << A2_MCSR_DEMH_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-59) ) ) != 0 ) // TLBMH
    str << A2_MCSR_TLBMH_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-60) ) ) != 0 ) // IEPE
    str << A2_MCSR_IEPE_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-61) ) ) != 0 ) // DEPE
    str << A2_MCSR_DEPE_DESCR;

  if ( ( mcsr & ((uint64_t)1 << (63-0-62) ) ) != 0 ) // TLBPE
    str << A2_MCSR_TLBPE_DESCR;
}

