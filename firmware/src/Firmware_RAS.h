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

#ifndef _FIRMWARE_RAS_H
#define _FIRMWARE_RAS_H


/*
  <rasevent 
    id="00080001"
    category="PCI"
    component="FIRMWARE"
    severity="FATAL"
    message="PCIe Error.  $(DETAILS)"
    description="A hardware eror has occurred in the the PCI Express subsystem of a BQC."
    service_action="$(Diagnostics)"
    control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
    decoder="fw_PCIe_machineCheckDecoder"
    relevant_diags="pcie"
    />
 */

#define FW_RAS_PCIE_MACHINE_CHECK 0x00080001

/*
  <rasevent
    category="BQC"
    component="FIRMWARE"
    id="00080002"
    severity="FATAL"
    message="CRC error detected. address=$(ADDDRESS) size=$(SIZE) expected-CRC=$(EXPECTED) actual-CRC=$(ACTUAL)"
    description="Code loaded through the JTAG mailbox failed the cyclic redundancy check (CRC).  This indicates a potential error in transmission of the code and thus the code image is not trustworthy."
    service_action="$(Diagnostics)"
    control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
    relevant_diags="checkup"
    />
*/

#define FW_RAS_CRC_ERROR 0x00080002


#define _FW_RAS_NO_LONGER_USED___CRC_WARNING 0x00080003

/* 
   <rasevent 
     id="00080004"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="BeDRAM Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the BeDRAM unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_BeDRAM_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_BEDRAM_MACHINE_CHECK 0x00080004


/* 
   <rasevent 
     id="00080005"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="ClockStop Unit Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the ClockStop unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_ClockStop_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_CLOCKSTOP_MACHINE_CHECK 0x00080005


/* 
   <rasevent 
     id="00080006"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="DCR Arbiter Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the DCR Arbiter unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_DcrArbiter_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_DC_ARBITER_MACHINE_CHECK 0x00080006


/* 
   <rasevent 
     id="00080007"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="DDR Arbiter Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the DDR Arbiter unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_DdrArbiter_machineCheckDecoder"
     relevant_diags="memory"
     />
 */

#define FW_RAS_DR_ARB_MACHINE_CHECK 0x00080007

/* 
   <rasevent 
     id="00080008"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="DevBus Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Device Bus unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_Devbus_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_DEVBUS_MACHINE_CHECK 0x00080008


/* 
   <rasevent 
     id="00080009"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="EnvMon Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Environmental Monitor unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_EnvMon_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_ENVMON_MACHINE_CHECK 0x00080009

/* 
   <rasevent 
     id="0008000A"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="GEA Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Global Event Aggregator unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_GEA_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_GEA_MACHINE_CHECK 0x0008000A


/* 
   <rasevent 
     id="0008000B"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="L1P Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Level 1 Prefetch unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_L1P_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_L1P_MACHINE_CHECK 0x0008000B



/* 
   <rasevent 
     id="0008000C"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="L2 Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Level 2 Cache unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_L2_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_L2_MACHINE_CHECK 0x0008000C



/* 
   <rasevent 
     id="0008000D"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="L2C Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the L2 Central unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_L2C_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_L2C_MACHINE_CHECK 0x0008000D


/* 
   <rasevent 
     id="0008000E"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="L2 Counter Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the L2 Counter unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_L2Ctr_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_L2CTR_MACHINE_CHECK 0x0008000E


/* 
   <rasevent 
     id="0008000F"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="MSGC Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the MSGC unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_MSGC_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_MSGC_MACHINE_CHECK 0x0008000F


/* 
   <rasevent 
     id="00080010"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="TestInt Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Test Interface unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_TestInt_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_TESTINT_MACHINE_CHECK 0x00080010

/* 
   <rasevent 
     id="00080011"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="UPC Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Universal Performace Counter unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_UPC_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_UPC_MACHINE_CHECK 0x00080011

/* 
   <rasevent 
     id="00080012"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="Wakeup Unit Machine Check : $(DETAILS)"
     description="A hardware eror has been detected by the Wakeup unit of a BQC."
     service_action="$(Diagnostics)"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     decoder="fw_WU_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_WU_MACHINE_CHECK 0x00080012


/* 
   <rasevent 
     id="00080013"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="RAS Storm Warning: Firmware has detected a burst of similar RAS events and has compressed them.  There were $(%d,COUNT) similar events detected for message code $(%08x,CODE).  The burst has subsided."
     description="A burst of similar RAS events was detected on a single node.  In order to minimize impact to the overall Blue Gene system, some of these events were suppressed.  The details of this event describe both the message code and the total number of messages that were suppressed.  The burst was not considered significant enough to be fatal. "
     service_action="Refer to the original message and proceed as directed."
     />
 */

#define FW_RAS_STORM_WARNING 0x00080013


/* 
   <rasevent 
     id="00080014"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="RAS Storm Error: Firmware has detected a significant burst of similar RAS events and has compressed them.  There were $(%d,COUNT) similar events detected for message code $(%08x,CODE)."
     description="A burst of similar RAS events was detected on a single node.  In order to minimize impact to the overall Blue Gene system, some of these events were suppressed.  The details of this event describe both the message code and the total number of messages that were suppressed.  The burst was considered significant enough to be considered a fatal error. "
     service_action="Refer to the original message and proceed as directed."
     control_action="END_JOB,FREE_COMPUTE_BLOCK"
     />
 */

#define FW_RAS_STORM_ERROR 0x00080014

 /* 
   <rasevent 
     id="00080015"
     category="Message_Unit"
     component="FIRMWARE"
     severity="WARN"
     message="Message Unit Recoverable Error: $(DETAILS)"
     description="A non-fatal error was detected by the Message Unit hardware."
     service_action="$(Diagnostics)"
     decoder="fw_MU_machineCheckDecoder"
     />
 */


#define FW_RAS_MU_WARNING 0x00080015


/* 
   <rasevent 
     id="00080016"
     category="Message_Unit"
     component="FIRMWARE"
     severity="FATAL"
     message="Message Unit Error: $(DETAILS)"
     description="A fatal error was detected by the Message Unit hardware."
     service_action="$(Diagnostics)"
     relevant_diags="torus"
     decoder="fw_MU_machineCheckDecoder"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     />
 */

#define FW_RAS_MU_ERROR  0x00080016


/* 
   <rasevent 
     id="00080017"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="SerDes Machine Check: $(DETAILS)"
     description="A hardware error has been detected in the SerDes unit of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_Serdes_machineCheckDecoder"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     relevant_diags="torus"
     />
 */

#define FW_RAS_SERDES_MACHINE_CHECK 0x00080017


 /* 
   <rasevent 
     id="00080018"
     category="Message_Unit"
     component="FIRMWARE"
     severity="WARN"
     message="ND Correctable Error: $(DETAILS)"
     description="A correctable error has been reported by the Network Device unit of a BQC."
     service_action="$(Diagnostics)"
     relevant_diags="torus"
     decoder="fw_ND_machineCheckDecoder"
     />
 */

#define FW_RAS_ND_WARNING 0x00080018


/* 
   <rasevent 
     id="00080019"
     category="Message_Unit"
     component="FIRMWARE"
     severity="FATAL"
     message="ND Fatal Error: $(DETAILS)"
     description="A fatal error has been reported by the Network Device unit of a BQC."
     service_action="$(Diagnostics)"
     relevant_diags="torus"
     decoder="fw_ND_machineCheckDecoder"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     />
 */

#define FW_RAS_ND_ERROR  0x00080019

/* 
   <rasevent 
     id="0008001A"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="A2 Processor Machine Check : $(DETAILS)"
     description="A machine check was reported by the A2.  This error is internal to the A2 core."
     service_action="$(Diagnostics)"
     relevant_diags="processor"
     decoder="fw_A2_machineCheckDecoder"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     />
 */

#define FW_RAS_A2_HARDWARE_MACHINE_CHECK  0x0008001A

/* 
   <rasevent 
     id="0008001B"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="DDR Arbiter Machine Check (Recoverable) : $(DETAILS)"
     description="A recoverable eror has been detected by the DDR Arbiter unit of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_DdrArbiter_machineCheckDecoder"
     relevant_diags="memory"
     />
 */

#define FW_RAS_DR_ARB_RECOVERABLE_MACHINE_CHECK 0x0008001B

/* 
   <rasevent 
     id="0008001C"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="L1P Correctable : $(DETAILS)"
     description="A correctable eror has been detected by the L1 Prefetch unit of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_L1P_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_L1P_CORRECTABLE_MACHINE_CHECK 0x0008001C


/* 
   <rasevent 
     id="0008001D"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="DDR Arbiter Machine Check (Recoverable) : $(DETAILS)"
     description="A correctable error has been detected by the DDR Arbiter unit of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_DdrArbiter_machineCheckDecoder"
     relevant_diags="memory"
     />
 */

#define FW_RAS_DR_ARB_CORRECTABLE_MACHINE_CHECK 0x0008001D



/* 
   <rasevent 
     id="0008001E"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="L2 Machine Check (Recoverable) : $(DETAILS)"
     description="An correctable hardware eror has been detected by the Level 2 Cache unit of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_L2_machineCheckDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_L2_CORRECTABLE_MACHINE_CHECK 0x0008001E

/* 
   <rasevent 
     id="0008001F"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="Unrecoverable Machine Check."
     description="A machine check occurred before firmware was able to establish a recoverable context.  See previous RAS events at this location for more details."
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     service_action="$(Diagnostics)"
     relevant_diags="processor"
     />
 */

#define FW_RAS_UNRECOVERABLE_MACHINE_CHECK 0x0008001F


/* 
   <rasevent 
     id="00080020"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="Memory Controller Initialization Warning: $(DETAILS)"
     description="A non-fatal error occurred during memory controller initialization.  It is recommended that this  node be replaced at the next maintenance opportunity."
     decoder="fw_DdrInit_Decoder"
     service_action="$(Diagnostics)"
     relevant_diags="processor"
     threshold_count="10"
     />
 */

#define FW_RAS_DDR_INIT_WARNING 0x00080020
#define FW_RAS_DDR_INIT_ERROR   0x0008003D
#define FW_RAS_DDR_INIT_INFO    0x0008003E

#define FW_RAS_DDR_INIT_VTT_DD                   1
#define FW_RAS_DDR_INIT_VTT_SB                   2
#define FW_RAS_DDR_INIT_RANK_DETECTION_FAILED    3
#define FW_RAS_DDR_INIT_RANK_DETECTION_RETRIED   4
#define FW_RAS_DDR_INIT_IOM_CALIBRATION_FAILED   5
#define FW_RAS_DDR_INIT_IOM_CALIBRATION_RETRIED  6
#define FW_RAS_DDR_INIT_DENSITY_DETECTION_FAILED 7
#define FW_RAS_DDR_INIT_SMALL_BLOCK_TEST_FAILED  8
#define FW_RAS_DDR_INIT_NARROW_WRITE_DATA_WINDOW 9
#define FW_RAS_DDR_INIT_NARROW_ADDRESS_WINDOW    10

/* 
   <rasevent 
     id="00080021"
     category="PCI"
     component="FIRMWARE"
     severity="FATAL"
     message="PCIe Root Complex Initialization Failed at Step $(I)."
     description="The PCIe Root Complex in this BQC failed to initialize properly.  Possible causes are a) faulty PCIe network adapter  b) faulty BQC  c) failure in the power or clock domains for PCIe."
     service_action="$(Diagnostics)"
     relevant_diags="pcie"
     />
 */

#define FW_RAS_PCIE_INIT_FAILURE 0x00080021


/* 
   <rasevent 
     id="00080022"
     category="Software_Error"
     component="FIRMWARE"
     severity="WARN"
     message="(WARNING) $(MSG)"
     description="A serious condition was detected in firmware."
     service_action="$(CheckLevels)"
     />
 */

#define FW_RAS_WARNING 0x00080022


/* 
   <rasevent 
     id="00080023"
     category="Software_Error"
     component="FIRMWARE"
     severity="FATAL"
     message="(ERROR) $(MSG)"
     description="A fatal condition was detected in firmware."
     service_action="$(CheckLevels)"
     />
 */

#define FW_RAS_ERROR 0x00080023

/* 
   <rasevent 
     id="00080024"
     category="Software_Error"
     component="FIRMWARE"
     severity="FATAL"
     message="Unexpected Interrupt: $(DETAILS)."
     description="An interrupt was detected by firmware and this interrupt was not expected."
     service_action="$(CheckLevels)"
     control_action="END_JOB,FREE_COMPUTE_BLOCK,SOFTWARE_IN_ERROR"
     decoder="fw_UnexpectedInterrupt_Decoder"
     />
 */

#define FW_RAS_UNEXPECTED_INTERRUPT 0x00080024


/* 
   <rasevent 
     id="00080025"
     category="Software_Error"
     component="FIRMWARE"
     severity="INFO"
     message="Firmware termination: status:$(STATUS) LR:$(LR) SRR0:$(SRR0) SRR1:$(SRR1) ESR:$(ESR) DEAR:$(DEAR)"
     description="Firmware has terminated."
     />
 */

#define FW_RAS_TERMINATION 0x00080025


/* 
   <rasevent 
     id="00080026"
     category="BQC"
     component="FIRMWARE"
     severity="INFO"
     message="DDR Drilldown : $(DETAILS)"
     description="Provides additional information to IBM regarding soft failures in the DDR subsystem."
     decoder="fw_DdrArbiter_drilldown"
     relevant_diags="memory"
     />
 */

#define FW_RAS_DDR_DRILLDOWN  0x00080026


/* 
   <rasevent 
     id="00080027"
     category="PCI"
     component="FIRMWARE"
     severity="INFO"
     message="PCIe Initialization took $(%d,MILLIS) millseconds."
     description="Provides additional information to IBM regarding soft failures in the DDR subsystem."
     />
 */

#define FW_RAS_PCIE_INIT_DURATION  0x00080027

/* 
   <rasevent 
     id="00080028"
     category="PCI"
     component="FIRMWARE"
     severity="WARN"
     message="PCIe PL_LINKUP status has not locked after $(%d,DURATION) milliseconds"
     description="The PCIe subsystem is taking longer than normal to initialize."
     relevant_diags="pcie"
     />
 */

#define FW_RAS_PCIE_LINKUP_IS_SLOW  0x00080028



/* 
   <rasevent 
     id="00080029"
     category="Software_Error"
     component="FIRMWARE"
     severity="FATAL"
     message="The actual DDR memory size of $(%d,ACTUAL)MB is less than the configured size of $(%d,CONFIGURED)MB."
     description="The control system database is configured with a different memory size than what actually exists on this BQC.  This likely indicates that an improper node was used as a replacement."
     control_action="SOFTWARE_IN_ERROR,FREE_COMPUTE_BLOCK"
     />
 */

#define FW_DDR_SIZE_MISMATCH_ERROR 0x00080029

/* 
   <rasevent 
     id="0008002A"
     category="Software_Error"
     component="FIRMWARE"
     severity="INFO"
     message="The actual DDR memory size of $(%d,ACTUAL)MB is larger then the configured size of $(%d,CONFIGURED)MB."
     description="The control system database is configured with a different memory size than what actually exists on this BQC.  Only the smaller amount of memory will be allocated for use by the kernel and applications."
     />
 */

#define FW_DDR_SIZE_MISMATCH_WARNING 0x0008002A

/* 
   <rasevent 
     id="0008002B"
     category="Software_Error"
     component="FIRMWARE"
     severity="INFO"
     message="DDR memory size has been automatically adjusted to $(%d,ACTUAL)MB from $(%d,CONFIGURED)MB to match the hardware."
     description="Firmware has detected a difference between the configured and actual DDR size of the BQC.  Furthermore, firmware has been configured to use the actual size."
     />
 */

#define FW_DDR_AUTO_DETECTION_SIZE_MISMATCH 0x0008002B

/* 
   <rasevent 
     id="0008002C"
     category="Software_Error"
     component="FIRMWARE"
     severity="WARN"
     message="A control system barrier has gone unacknowledged for $(%d,MICROS) microseconds."
     description="The node is waiting for an acknowledgement from the control system for a control system barrier."
     />
 */
#define FW_CS_BARRIER_WARNING 0x0008002C


/* 
   <rasevent 
     id="0008002D"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="Bad DRAM was detected - $(DETAILS)"
     description="A bad memory module was detected on the chip."
     decoder="fw_DdrArbiter_badDramDecoder"
     threshold_count="10"
     />
 */
#define FW_RAS_BAD_DRAM_WARNING 0x0008002D

/* 
   <rasevent 
     id="0008002E"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="Bad PHY was detected - MC $(%d,$MC) Byte $(%d,BYTE)"
     description="A bad memory module was detected on the chip."
     threshold_count="10"
     />
 */
#define FW_RAS_BAD_PHY_WARNING 0x0008002E


/* 
   <rasevent 
     id="0008002F"
     category="BQC"
     component="FIRMWARE"
     severity="INFO"
     message="L1P Correctable Error Summary : $(DETAILS)"
     description="Correctable errors have been detected by the L1 Prefetch unit of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_L1P_correctableSummaryDecoder"
     relevant_diags="processor"
     threshold_count="100000"
     threshold_period="1 day"
     />
 */

#define FW_RAS_L1P_CORRECTABLE_SUMMARY 0x0008002F

/* 
   <rasevent 
     id="00080030"
     category="BQC"
     component="FIRMWARE"
     severity="INFO"
     message="L2 Array Correctable Error Summary : $(DETAILS)"
     description="Correctable errors have been detected in the L2 cache arrays of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_L2_correctableSummaryDecoder"
     relevant_diags="processor"
     threshold_count="2400"
     threshold_period="1 day"
     />
 */

#define FW_RAS_L2_CORRECTABLE_ARRAY_SUMMARY 0x00080030


/* 
   <rasevent 
     id="00080031"
     category="BQC"
     component="FIRMWARE"
     severity="INFO"
     message="L2 Directory Correctable Error Summary : $(DETAILS)"
     description="Correctable errors have been detected in the L2 cache directory of a BQC."
     service_action="$(Diagnostics)"
     decoder="fw_L2_correctableSummaryDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_L2_CORRECTABLE_DIRECTORY_SUMMARY 0x00080031

/* 
   <rasevent 
     id="00080032"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="Illegal DCR Access : $(DETAILS)"
     description="An illegal DCR request has been detected.  This is most likely a software error and a result of either attempting to read or write a DCR that does not exist or attempting to read or write a DCR that is privileged."
     service_action="$(CheckLevels)"
     decoder="fw_DcrArbiter_badDcrAccessDecoder"
     control_action="END_JOB"
     />
 */

#define FW_RAS_BAD_DCR_ACCESS 0x00080032

/* 
   <rasevent 
     id="00080033"
     category="DDR"
     component="FIRMWARE"
     severity="INFO"
     message="DDR Correctable Error Summary : $(DETAILS)"
     description="Correctable errors have been detected in the DDR subsystem of a BQC."
     decoder="fw_DdrArbiter_correctableSummaryDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_DDR_CORRECTABLE_SUMMARY 0x00080033

/* 
   <rasevent 
     id="00080034"
     category="DDR"
     component="FIRMWARE"
     severity="INFO"
     message="DDR Maintenance Correctable Error Summary : $(DETAILS)"
     description="Correctable errors have been detected in the maintenance of the DDR subsystem of a BQC."
     decoder="fw_DdrArbiter_correctableSummaryDecoder"
     relevant_diags="processor"
     />
 */

#define FW_RAS_DDR_MAINTENANCE_CORRECTABLE_SUMMARY 0x00080034




#define FW_RAS_NO_LONGER_IN_USE_00080035


 /* 
   <rasevent 
     id="00080036"
     category="Message_Unit"
     component="FIRMWARE"
     severity="INFO"
     message="Message Unit ECC Summary : $(DETAILS)"
     description="One or more ECC errors was detected by the Message Unit hardware.  The hardware as able to correct these errors."
     service_action="$(Diagnostics)"
     decoder="fw_MU_eccSummaryDecoder"
     threshold_count="10"
     threshold_period="1 hour"

     />
 */


#define FW_RAS_MU_ECC_SUMMARY 0x00080036



 /* 
   <rasevent 
     id="00080037"
     category="Message_Unit"
     component="FIRMWARE"
     severity="INFO"
     message="ND Receiver Link Error : $(LINK) count=$(%d,2)  $(DETAILS)"
     description="A correctable error has been reported by the Network Device unit of a BQC."
     decoder="fw_ND_correctableDecoder"
     />
 */

#define FW_RAS_ND_RE_LINK_ERROR 0x00080037

 /* 
   <rasevent 
     id="00080038"
     category="Message_Unit"
     component="FIRMWARE"
     severity="INFO"
     message="ND Sender Retransmission Correctable Error : $(LINK) count=$(%d,2) $(DETAILS)"
     description="A correctable error has been reported by the Network Device unit of a BQC."
     decoder="fw_ND_correctableDecoder"
     />
 */

#define FW_RAS_ND_SE_RETRANS_ERROR 0x00080038

 /* 
   <rasevent 
     id="00080039"
     category="Message_Unit"
     component="FIRMWARE"
     severity="INFO"
     message="ND Receiver Correctable Error : $(LINK)  count=$(%d,2)  $(DETAILS)"
     description="A correctable error has been reported by the Network Device unit of a BQC."
     decoder="fw_ND_correctableDecoder"
     threshold_count="5"
     threshold_period="1 hour"
     />
 */

#define FW_RAS_ND_RE_CE_ERROR 0x00080039

/* 
   <rasevent 
     id="0008003A"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="A2 Processor Machine Check : $(DETAILS)"
     description="A machine check was reported by the A2.  This error is internal to the A2 core."
     service_action="$(Diagnostics)"
     relevant_diags="processor"
     decoder="fw_A2_machineCheckDecoder"
     control_action="END_JOB,SOFTWARE_IN_ERROR,FREE_COMPUTE_BLOCK"
     />
 */

#define FW_RAS_A2_SOFTWARE_MACHINE_CHECK  0x0008003A

/*
    <rasevent 
     id="0008003B"
     category="BQC" 
     component="FIRMWARE"
     severity="WARN"
     message="A2 TLB Parity Error : MMUCR1=$(MMUCR1) MCSR=$(MCSR) : $(MCSR_DETAILS)"
     description="A TLB parity error machine check was reported by the A2.  Recovery will be attempted."
     service_action="$(Diagnostics)"
     relevant_diags="processor"
     threshold_count="10"
     decoder="fw_A2_tlbParityErrorDecoder"
     />
*/


#define FW_RAS_A2_TLBPE_MACHINE_CHECK  0x0008003B

/* 
   <rasevent 
     id="0008003C"
     category="Software_Error"
     component="FIRMWARE"
     severity="INFO"
     message="$(MSG)"
     description="This message provides additional information that may be useful to IBM in a support capacity."
     />
 */

#define FW_RAS_INFO 0x0008003C


/* 
   <rasevent 
     id="0008003D"
     category="BQC"
     component="FIRMWARE"
     severity="FATAL"
     message="Memory Controller Initialization Error: $(DETAILS)"
     description="An error occurred during memory controller initialization."
     decoder="fw_DdrInit_Decoder"
     service_action="$(Diagnostics)"
     relevant_diags="memory"
     control_action="END_JOB,COMPUTE_IN_ERROR,FREE_COMPUTE_BLOCK"
     />
 */

// #defined above (FW_RAS_DDR_INIT_ERROR)

/* 
   <rasevent 
     id="0008003E"
     category="BQC"
     component="FIRMWARE"
     severity="INFO"
     message="Memory Controller Initialization Information : $(DETAILS)"
     description="Memory controller initialization encountered an unexpected but non-serious event."
     decoder="fw_DdrInit_Decoder"
     />
 */

// #defined above (FW_RAS_DDR_INIT_INFO)

/*
    <rasevent 
     id="0008003F"
     category="BQC" 
     component="FIRMWARE"
     severity="FATAL"
     message="Barrier Initialization Error : $(DETAILS)"
     description="An error occurred when initializing the primordial MU barrier.  This could be a defective BQC.  But it could also be due to a defective cable or a misplugged cable.  VerifyCables should be run prior to or in addition to any diagnostics."
     service_action="$(Diagnostics)"
     relevant_diags="torus"
     />
*/


#define FW_RAS_BARRIER_INIT_ERROR  0x0008003F




#define FW_RAS_EVENT_FILTER_THRESHOLD   10ul
#define FW_RAS_EVENT_ERROR_THRESHOLD    100ul
#define FW_RAS_STORM_THRESHOLD_MILLIS   500ul  //! Average acceptable time between events


#define FW_RAS_TEST_MESSAGE 0x00080099


/* 
   <rasevent 
     id="00080099"
     category="BQC"
     component="FIRMWARE"
     severity="WARN"
     message="This is a test."
     description="This is a tst."
     service_action="$(Diagnostics)"
     relevant_diags="@todo"
     decoder="decoder_does_not_exist"
     />
 */


extern void fw_machineCheckRas( fw_uint32_t rasMessageCode, fw_uint64_t details[], fw_uint16_t numDetails, const char* file, int lineNumber );

extern void FW_RAS_printf( const uint32_t msg_id, const char* fmt, ... );
extern void FW_Error( const char* fmt, ... );
extern void FW_Warning( const char *fmt, ... );

#endif

