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
  const std::string    BEDRAM_DB_PARITY_DESCR = " [BEDRAM_DB_PARITY] Parity error in devbus";
  const std::string  BEDRAM_PIPE_PARITY_DESCR = " [BEDRAM_PIPE_PARITY] Parity error in pipeline";
  const std::string   BEDRAM_REF_PARITY_DESCR = " [BEDRAM_REF_PARITY] Parity error in refresh unit";
  const std::string   BEDRAM_RES_PARITY_DESCR = " [BEDRAM_RES_PARITY] Parity error in reset unit";
  const std::string   BEDRAM_SCB_PARITY_DESCR = " [BEDRAM_SCB_PARITY] Parity error in scrub unit";
  const std::string   BEDRAM_ERR_PARITY_DESCR = " [BEDRAM_ERR_PARITY] Parity error in error counter";
  const std::string       BEDRAM_DB_ECC_DESCR = " [BEDRAM_DB_ECC] Uncorrectable ECC error in devbus";
  const std::string    BEDRAM_EDRAM_ECC_DESCR = " [BEDRAM_EDRAM_ECC] Uncorrectable ECC error in eDRAM";
  const std::string     BEDRAM_PIPE_ECC_DESCR = " [BEDRAM_PIPE_ECC] Uncorrectable ECC error in pipeline";
  const std::string BEDRAM_ECC_CE_RAISE_DESCR = " [BEDRAM_ECC_CE_RAISE] Total amount of correctable ECC errors exceeded maximum";
  const std::string      BEDRAM_ALIGNED_DESCR = " [BEDRAM_ALIGNED] non aligned address";
  const std::string    BEDRAM_BYTE_SIZE_DESCR = " [BEDRAM_BYTE_SIZE] wrong byte size";
  const std::string     BEDRAM_NON_PRIV_DESCR = " [BEDRAM_NON_PRIV] non-privileged atomic operation";
  const std::string          LOCAL_RING_DESCR = " [LOCAL_RING] Parity error on DCR ring";

void decode_BEDRAM_INTERRUPT_STATUS( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "BEDRAM_INTERRUPT_STATUS error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // BEDRAM_DB_PARITY
    str << BEDRAM_DB_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // BEDRAM_PIPE_PARITY
    str << BEDRAM_PIPE_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // BEDRAM_REF_PARITY
    str << BEDRAM_REF_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // BEDRAM_RES_PARITY
    str << BEDRAM_RES_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // BEDRAM_SCB_PARITY
    str << BEDRAM_SCB_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // BEDRAM_ERR_PARITY
    str << BEDRAM_ERR_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // BEDRAM_DB_ECC
    str << BEDRAM_DB_ECC_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // BEDRAM_EDRAM_ECC
    str << BEDRAM_EDRAM_ECC_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // BEDRAM_PIPE_ECC
    str << BEDRAM_PIPE_ECC_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // BEDRAM_ECC_CE_RAISE
    str << BEDRAM_ECC_CE_RAISE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // BEDRAM_ALIGNED
    str << BEDRAM_ALIGNED_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // BEDRAM_BYTE_SIZE
    str << BEDRAM_BYTE_SIZE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // BEDRAM_NON_PRIV
    str << BEDRAM_NON_PRIV_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // LOCAL_RING
    str << LOCAL_RING_DESCR + ";";
}

