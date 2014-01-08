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
  const std::string                            SBOX_ERROR_DESCR = " [SBOX_ERROR] A parity error or check sequence error was detected on either the Write Completion or Cancel Acknowledge asynchronous interfaces between MCA and MCS";
  const std::string                         MCS_MNT_ERROR_DESCR = " [MCS_MNT_ERROR] A parity error was detected on the buses between MCA and MCS which specify the address and operation type";
  const std::string            DISPSATHER_COLLISION_ERROR_DESCR = " [DISPSATHER_COLLISION_ERROR] An address collision occurred between two PBUS reflected commands (see 2";
  const std::string                      WRITE_HANG_ERROR_DESCR = " [WRITE_HANG_ERROR] A PBUS write hang has been detected";
  const std::string           MCFGP_REGISTER_PARITY_ERROR_DESCR = " [MCFGP_REGISTER_PARITY_ERROR] ";
  const std::string          MCFGC0_REGISTER_PARITY_ERROR_DESCR = " [MCFGC0_REGISTER_PARITY_ERROR] ";
  const std::string          MCFGC1_REGISTER_PARITY_ERROR_DESCR = " [MCFGC1_REGISTER_PARITY_ERROR] ";
  const std::string         MCNTCFG_REGISTER_PARITY_ERROR_DESCR = " [MCNTCFG_REGISTER_PARITY_ERROR] ";
  const std::string          MCMODE_REGISTER_PARITY_ERROR_DESCR = " [MCMODE_REGISTER_PARITY_ERROR] ";
  const std::string        MEPSILON_REGISTER_PARITY_ERROR_DESCR = " [MEPSILON_REGISTER_PARITY_ERROR] ";
  const std::string            MCTL_REGISTER_PARITY_ERROR_DESCR = " [MCTL_REGISTER_PARITY_ERROR] ";
  const std::string          MCBCFG_REGISTER_PARITY_ERROR_DESCR = " [MCBCFG_REGISTER_PARITY_ERROR] ";
  const std::string                         MDI_BIT_ERROR_DESCR = " [MDI_BIT_ERROR] ";
  const std::string                     READ_RETURN_ERROR_DESCR = " [READ_RETURN_ERROR] ";
  const std::string             PACKET_COUNTER_ADDRESS_UE_DESCR = " [PACKET_COUNTER_ADDRESS_UE] This bit is set when an Address UE has been detected during a Packet Counter operation";
  const std::string         MCAMISC_REGISTER_PARITY_ERROR_DESCR = " [MCAMISC_REGISTER_PARITY_ERROR] ";
  const std::string         MCTCNTL_REGISTER_PARITY_ERROR_DESCR = " [MCTCNTL_REGISTER_PARITY_ERROR] ";
  const std::string         MCADTAL_REGISTER_PARITY_ERROR_DESCR = " [MCADTAL_REGISTER_PARITY_ERROR] ";
  const std::string         MCACFG0_REGISTER_PARITY_ERROR_DESCR = " [MCACFG0_REGISTER_PARITY_ERROR] ";
  const std::string       MCAECCDIS_REGISTER_PARITY_ERROR_DESCR = " [MCAECCDIS_REGISTER_PARITY_ERROR] ";
  const std::string           MAINTENANCE_INTERFACE_ERROR_DESCR = " [MAINTENANCE_INTERFACE_ERROR] ";
  const std::string           ASYNC_SERIALIZER_RCMD_ERROR_DESCR = " [ASYNC_SERIALIZER_RCMD_ERROR] ";
  const std::string          ASYNC_SERIALIZER_CRESP_ERROR_DESCR = " [ASYNC_SERIALIZER_CRESP_ERROR] ";
  const std::string      WRITE_COMPLETION_READ_DONE_ERROR_DESCR = " [WRITE_COMPLETION_READ_DONE_ERROR] ";
  const std::string         WRITE_COMPLETION_CANCEL_ERROR_DESCR = " [WRITE_COMPLETION_CANCEL_ERROR] ";
  const std::string                  RSMFIFO_PARITY_ERROR_DESCR = " [RSMFIFO_PARITY_ERROR] ";
  const std::string                   READ_BUFFER_OVERRUN_DESCR = " [READ_BUFFER_OVERRUN] ";
  const std::string                  READ_BUFFER_UNDERRUN_DESCR = " [READ_BUFFER_UNDERRUN] ";
  const std::string                             MEMORY_CE_DESCR = " [MEMORY_CE] This bit is set when a memory CE is detected on a non-maintenance memory read op";
  const std::string                MEMORY_FAST_DECODER_UE_DESCR = " [MEMORY_FAST_DECODER_UE] This bit is set when a memory UE is detected by the Fast Decoder on a non-maintenance memory read op";
  const std::string                            MEMORY_SUE_DESCR = " [MEMORY_SUE] This bit is set when a memory SUE is detected on a non-maintenance memory read op";
  const std::string                        MAINTENANCE_CE_DESCR = " [MAINTENANCE_CE] This bit is set when a memory CE is detected on a maintenance memory read op";
  const std::string           MAINTENANCE_FAST_DECODER_UE_DESCR = " [MAINTENANCE_FAST_DECODER_UE] This bit is set when a memory UE is detected by the Fast Decoder on a maintenance memory read op";
  const std::string                       MAINTENANCE_SUE_DESCR = " [MAINTENANCE_SUE] This bit is set when a memory SUE is detected on a maintenance memory read op";
  const std::string              POWERBUS_WRITE_BUFFER_CE_DESCR = " [POWERBUS_WRITE_BUFFER_CE] This bit is set when a PBUS ECC CE is detected on a PBus write buffer read op";
  const std::string              POWERBUS_WRITE_BUFFER_UE_DESCR = " [POWERBUS_WRITE_BUFFER_UE] This bit is set when a PBUS ECC UE is detected on a PowerBus write buffer read op";
  const std::string             POWERBUS_WRITE_BUFFER_SUE_DESCR = " [POWERBUS_WRITE_BUFFER_SUE] This bit is set when a PBUS ECC SUE is detected on a PowerBus write buffer read op";
  const std::string               POWERBUS_READ_BUFFER_CE_DESCR = " [POWERBUS_READ_BUFFER_CE] This bit is set when a PBUS ECC CE is detected on a PowerBus read buffer read op";
  const std::string               POWERBUS_READ_BUFFER_UE_DESCR = " [POWERBUS_READ_BUFFER_UE] This bit is set when a PBUS ECC UE is detected on a PowerBus read buffer read op";
  const std::string              POWERBUS_READ_BUFFER_SUE_DESCR = " [POWERBUS_READ_BUFFER_SUE] This bit is set when a PBUS ECC SUE is detected on a PowerBus read buffer read op";
  const std::string               PACKET_COUNTER_DATA_SUE_DESCR = " [PACKET_COUNTER_DATA_SUE] This bit is set when an ECC data SUE is detected on a Packet Counter operation";
  const std::string      READ_CHANNEL_BUFFER_PARITY_ERROR_DESCR = " [READ_CHANNEL_BUFFER_PARITY_ERROR] Parity when data is read from channel burrer during ECC correction process";
  const std::string      MEMORY_ECC_MARKING_STORE_UPDATED_DESCR = " [MEMORY_ECC_MARKING_STORE_UPDATED] BGQ: This bit is set when the memory ECC check logic has updated the marking store";
  const std::string MEMORY_ECC_MARKING_STORE_PARITY_ERROR_DESCR = " [MEMORY_ECC_MARKING_STORE_PARITY_ERROR] This bit is set when a parity error is detected when the ECC mark store is read by the ECC check logic";
  const std::string            WRITE_ECC_GEN_PARITY_ERROR_DESCR = " [WRITE_ECC_GEN_PARITY_ERROR] A data parity error was detected in the write dataflow, by the Memory ECC generation logic";
  const std::string                  MEMORY_REFRESH_ERROR_DESCR = " [MEMORY_REFRESH_ERROR] ";
  const std::string     PACKET_COUNTER_CORRECTABLE_ERRORS_DESCR = " [PACKET_COUNTER_CORRECTABLE_ERRORS] This bit is set for the following errors: ECC address CE during a Packet Counter operation, a non-supported PBUS ttype to Packet Counter address space or a non-supported address alignment for a Packet Counter operation";
  const std::string                PACKET_COUNTER_DATA_CE_DESCR = " [PACKET_COUNTER_DATA_CE] This bit is set when an ECC data CE is detected during a Packet Counter operation";
  const std::string                        IOM0_PHY_ERROR_DESCR = " [IOM0_PHY_ERROR] ";
  const std::string   ECC_ERROR_COUNTER_THRESHOLD_REACHED_DESCR = " [ECC_ERROR_COUNTER_THRESHOLD_REACHED] ";
  const std::string MCA_REGISTER_PARITY_ERROR_RECOVERABLE_DESCR = " [MCA_REGISTER_PARITY_ERROR_RECOVERABLE] This bit is set for the following errors:MCARETRY parity error, MCZMRINT parity error or MCACFG1 parity error";
  const std::string MCS_REGISTER_PARITY_ERROR_RECOVERABLE_DESCR = " [MCS_REGISTER_PARITY_ERROR_RECOVERABLE] This bit is set for the following errors:MCCLMSK parity error or MCDBGSEL parity error";
  const std::string           INVALID_MAINTENANCE_COMMAND_DESCR = " [INVALID_MAINTENANCE_COMMAND] This bit is set when a maintenance command is started with an invalid maintenance command code entered in MCMCT";
  const std::string                  NO_CHANNELS_SELECTED_DESCR = " [NO_CHANNELS_SELECTED] A reflected command address matches the addresses accepted by this MC (matches MCFGP address/mask), but does not lie within the address range of either channel";
  const std::string            MULTIPLE_CHANNELS_SELECTED_DESCR = " [MULTIPLE_CHANNELS_SELECTED] A reflected command address matches the addresses accepted by this MC (matches MCFGP address/mask), and also matches more than one channel (matches multiple MCFGE addresses/masks)";
  const std::string                    ADDRESS_BAD_EXTENT_DESCR = " [ADDRESS_BAD_EXTENT] This bit is set when a PBUS reflected command arrives and the sum of the extents of Channels 0 and 1 do not match the total extent specified for the memory controller";
  const std::string                          INVALID_RANK_DESCR = " [INVALID_RANK] This bit is set when the logical rank address (given in the PBUS reflected command address) does not map onto a valid physical rank";
  const std::string                  SCOM_INTERFACE_ERROR_DESCR = " [SCOM_INTERFACE_ERROR] This bit is a general summary of Scom interface errors detected by the MCS scom satellite or the MCA scom satellite";
  const std::string               HTM_HPC_WRITE_COLLISION_DESCR = " [HTM_HPC_WRITE_COLLISION] ";
  const std::string                         MC_SCOM_ERROR_DESCR = " [MC_SCOM_ERROR] This bit is set when an internal error is detected by the scom fir logic";

void decode_MCFIR( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "MCFIR error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // SBOX_ERROR
    str << SBOX_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // MCS_MNT_ERROR
    str << MCS_MNT_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // DISPSATHER_COLLISION_ERROR
    str << DISPSATHER_COLLISION_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // WRITE_HANG_ERROR
    str << WRITE_HANG_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // MCFGP_REGISTER_PARITY_ERROR
    str << MCFGP_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // MCFGC0_REGISTER_PARITY_ERROR
    str << MCFGC0_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // MCFGC1_REGISTER_PARITY_ERROR
    str << MCFGC1_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // MCNTCFG_REGISTER_PARITY_ERROR
    str << MCNTCFG_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // MCMODE_REGISTER_PARITY_ERROR
    str << MCMODE_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // MEPSILON_REGISTER_PARITY_ERROR
    str << MEPSILON_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // MCTL_REGISTER_PARITY_ERROR
    str << MCTL_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // MCBCFG_REGISTER_PARITY_ERROR
    str << MCBCFG_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // MDI_BIT_ERROR
    str << MDI_BIT_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // READ_RETURN_ERROR
    str << READ_RETURN_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // PACKET_COUNTER_ADDRESS_UE
    str << PACKET_COUNTER_ADDRESS_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // MCAMISC_REGISTER_PARITY_ERROR
    str << MCAMISC_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // MCTCNTL_REGISTER_PARITY_ERROR
    str << MCTCNTL_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // MCADTAL_REGISTER_PARITY_ERROR
    str << MCADTAL_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // MCACFG0_REGISTER_PARITY_ERROR
    str << MCACFG0_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // MCAECCDIS_REGISTER_PARITY_ERROR
    str << MCAECCDIS_REGISTER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // MAINTENANCE_INTERFACE_ERROR
    str << MAINTENANCE_INTERFACE_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // ASYNC_SERIALIZER_RCMD_ERROR
    str << ASYNC_SERIALIZER_RCMD_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // ASYNC_SERIALIZER_CRESP_ERROR
    str << ASYNC_SERIALIZER_CRESP_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // WRITE_COMPLETION_READ_DONE_ERROR
    str << WRITE_COMPLETION_READ_DONE_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // WRITE_COMPLETION_CANCEL_ERROR
    str << WRITE_COMPLETION_CANCEL_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // RSMFIFO_PARITY_ERROR
    str << RSMFIFO_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // READ_BUFFER_OVERRUN
    str << READ_BUFFER_OVERRUN_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // READ_BUFFER_UNDERRUN
    str << READ_BUFFER_UNDERRUN_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // MEMORY_CE
    str << MEMORY_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // MEMORY_FAST_DECODER_UE
    str << MEMORY_FAST_DECODER_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // MEMORY_SUE
    str << MEMORY_SUE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-35) ) ) != 0 ) // MAINTENANCE_CE
    str << MAINTENANCE_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-36) ) ) != 0 ) // MAINTENANCE_FAST_DECODER_UE
    str << MAINTENANCE_FAST_DECODER_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-37) ) ) != 0 ) // MAINTENANCE_SUE
    str << MAINTENANCE_SUE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-38) ) ) != 0 ) // POWERBUS_WRITE_BUFFER_CE
    str << POWERBUS_WRITE_BUFFER_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-39) ) ) != 0 ) // POWERBUS_WRITE_BUFFER_UE
    str << POWERBUS_WRITE_BUFFER_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-40) ) ) != 0 ) // POWERBUS_WRITE_BUFFER_SUE
    str << POWERBUS_WRITE_BUFFER_SUE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-41) ) ) != 0 ) // POWERBUS_READ_BUFFER_CE
    str << POWERBUS_READ_BUFFER_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-42) ) ) != 0 ) // POWERBUS_READ_BUFFER_UE
    str << POWERBUS_READ_BUFFER_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-43) ) ) != 0 ) // POWERBUS_READ_BUFFER_SUE
    str << POWERBUS_READ_BUFFER_SUE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-44) ) ) != 0 ) // PACKET_COUNTER_DATA_SUE
    str << PACKET_COUNTER_DATA_SUE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-45) ) ) != 0 ) // READ_CHANNEL_BUFFER_PARITY_ERROR
    str << READ_CHANNEL_BUFFER_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-46) ) ) != 0 ) // MEMORY_ECC_MARKING_STORE_UPDATED
    str << MEMORY_ECC_MARKING_STORE_UPDATED_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-47) ) ) != 0 ) // MEMORY_ECC_MARKING_STORE_PARITY_ERROR
    str << MEMORY_ECC_MARKING_STORE_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-48) ) ) != 0 ) // WRITE_ECC_GEN_PARITY_ERROR
    str << WRITE_ECC_GEN_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-49) ) ) != 0 ) // MEMORY_REFRESH_ERROR
    str << MEMORY_REFRESH_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-50) ) ) != 0 ) // PACKET_COUNTER_CORRECTABLE_ERRORS
    str << PACKET_COUNTER_CORRECTABLE_ERRORS_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-51) ) ) != 0 ) // PACKET_COUNTER_DATA_CE
    str << PACKET_COUNTER_DATA_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-52) ) ) != 0 ) // IOM0_PHY_ERROR
    str << IOM0_PHY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-53) ) ) != 0 ) // ECC_ERROR_COUNTER_THRESHOLD_REACHED
    str << ECC_ERROR_COUNTER_THRESHOLD_REACHED_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-54) ) ) != 0 ) // MCA_REGISTER_PARITY_ERROR_RECOVERABLE
    str << MCA_REGISTER_PARITY_ERROR_RECOVERABLE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-55) ) ) != 0 ) // MCS_REGISTER_PARITY_ERROR_RECOVERABLE
    str << MCS_REGISTER_PARITY_ERROR_RECOVERABLE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-56) ) ) != 0 ) // INVALID_MAINTENANCE_COMMAND
    str << INVALID_MAINTENANCE_COMMAND_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-57) ) ) != 0 ) // NO_CHANNELS_SELECTED
    str << NO_CHANNELS_SELECTED_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-58) ) ) != 0 ) // MULTIPLE_CHANNELS_SELECTED
    str << MULTIPLE_CHANNELS_SELECTED_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-59) ) ) != 0 ) // ADDRESS_BAD_EXTENT
    str << ADDRESS_BAD_EXTENT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-60) ) ) != 0 ) // INVALID_RANK
    str << INVALID_RANK_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-61) ) ) != 0 ) // SCOM_INTERFACE_ERROR
    str << SCOM_INTERFACE_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-62) ) ) != 0 ) // HTM_HPC_WRITE_COLLISION
    str << HTM_HPC_WRITE_COLLISION_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-63) ) ) != 0 ) // MC_SCOM_ERROR
    str << MC_SCOM_ERROR_DESCR + ";";
}

