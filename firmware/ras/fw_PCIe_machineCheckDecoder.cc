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

#include <sstream>
#include <iostream>
#include <vector>
#include <ras/include/RasEvent.h>
#include <hwi/include/bqc/pe_dcr.h>
#include <hwi/include/bqc/pcie_utl.h>

using namespace std;

#include "_pcie_decoder.cc"
#include "utils.h"

// The following macro allows us to transcribe register addresses from the Upper Transaction Layer (UTL)
// specification into the corresponding Blue Gene DCR addresses:
#define PCIE_UTL_DCR(offset) (0x0D0100 + (offset))  

extern "C" {

  Decoder PCIE_DECODERS[] = {
    { PE_DCR(PCIE_INTERRUPT_STATE__MACHINE_CHECK), decode_PCI_Express_Interrupt_State_Register },
    { PCIE_UTL_DCR(0x08),                          decode_UTL_System_Bus_Agent_Status_Register },
    { PCIE_UTL_DCR(0xA4),                          decode_UTL_Express_Port_Status_Register },
    { PCIE_UTL_DCR(0xB0),                          decode_Root_Complex_Status_Register },
  };

  void fw_PCIe_machineCheckDecoder(RasEvent& event, const std::vector<uint64_t>& mbox) {

    TRACE(("(>) %s\n", __func__));

    walkDetails( PCIE_DECODERS, sizeof(PCIE_DECODERS)/sizeof(PCIE_DECODERS[0]), mbox,  event, "DETAILS" );
  }


}

