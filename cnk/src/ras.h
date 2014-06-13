/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
enum { RAS_CNKRASID = 0x00010000,
/*************************** */
       
       RAS_KERNELCRASH,
       /*
         <rasevent 
         id="00010001"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="Kernel unexpected operation.  IP=$(Address)  LR=$(LR)  ESR=$(ESR)  DEAR=$(DEAR)  MSR=$(MSR)  IntCode=$(CODE)"
         description="The kernel has performed an operation that was not permitted.  There are several triggers for these types of errors (bad memory reads or writes, branching to an invalid address).  These are typically software defects that should be understood and addressed.  On rare occasions, the trigger could be bad hardware but the initial debug assumption should always start with a software defect."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         relevant_diags="processor"
         />
       */
       
       RAS_INSUFFICIENTCORES,      
       /*
         <rasevent 
         id="00010002"
         category="BQC"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="Kernel invalid number of cores.  CoreMask=$(MASK)  Number=$(COUNT)"
         description="CNK requires that the number of cores starting CNK must be either 2, 3, 5, 9, or 17"
         service_action="In production environments, the BQC node likely has a bad core.  Run diagnostics to determine if core sparing is possible and follow diagnostics recommendations.  In development environments, suggest running coreprocessor to verify that there is not a software problem early in the boot sequence."
         relevant_diags="checkup"
         />
       */

       RAS_INVALIDPERSONALITY,      
       /*
         <rasevent 
         id="00010003"
         category="BQC"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="Kernel invalid personality options were specified.  NodeConfig=$(Config)"
         description="Settings in the personality are incompatible with CNK."
         service_action="The BQC node likely has a bad core, recommend running diagnostics and follow diagnostics recommendations."
         relevant_diags="checkup"
         />
       */
       RAS_CRC_EXCHANGE_ERROR,      
       /*
         <rasevent 
         id="00010004"
         category="BQC"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="Kernel Network CRC Exchange failed. Link=$(LINK) Expected=$(%x,EXPECTED) Actual=$(%x,ACTUAL)"
         description="The exchange of network packet CRCs failed."
         service_action="Check the torus network connections and review RAS correctable error thresholds."
         relevant_diags="checkup"
         />
       */
       RAS_POWERTHRESHOLD_A2,
       /*
         <rasevent 
         id="00010005"
         category="BQC"
         component="CNK"
         severity="WARN"
         message="Power threshold exceeded on processor domain.  Current=$(current) mA"
         description="The node board power consumption exceeds the preliminary threshold.  Processor Power management will be activated as a safeguard for the remainder of the job."
         service_action="Check the environmental monitor data.  Consider methods to reduce power consumption, such as proactive power management mode.  If there is a high rate of occurrence on this node board, this can be indicative of hardware problems that would be resolved by node board replacement. "
         relevant_diags="checkup"
         />
       */
       RAS_POWERTHRESHOLD_DDR,
       /*
         <rasevent 
         id="00010006"
         category="DDR"
         component="CNK"
         severity="WARN"
         message="Power threshold exceeded on memory domain.  Current=$(current) mA"
         description="The node board power consumption exceeds the preliminary threshold.  DDR Power management will be activated as a safeguard for the remainder of the job."
         service_action="Check the environmental monitor data.  Consider methods to reduce power consumption, such as proactive power management mode.  If there is a high rate of occurrence on this node board, this can be indicative of hardware problems that would be resolved by node board replacement. "
         relevant_diags="checkup"
         />
       */
       RAS_KERNELASSERTIONFAILURE,
       /*
         <rasevent 
         id="00010007"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="Kernel Internal assertion failure.  FileStringPtr=$(FILE1) on line $(LINE).  Function=$(FUNC)  Assert=$(ASSERT)"
         description="The kernel encountered an internal error.  This is likely due to a software problem."
         service_action="Please report this error message to IBM Support"
         relevant_diags="checkup"
         />
       */
       RAS_KERNELPRELOADFAIL,
       /*
         <rasevent 
         id="00010008"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="Kernel Preload Application failure.  JobID $(jobid).  LoadStateError=$(loaderr)  exitStatus=$(exitstatus)"
         description="The kernel encountered an internal error while starting a preloaded application.  This is likely due to a software problem."
         service_action="Please report this error message to IBM Support"
         relevant_diags="checkup"
         />
       */
       RAS_KERNELUNEXPECTEDEXIT,
       /*
         <rasevent 
         id="00010009"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="Kernel Unexpected Exit.  $(core) $(processor) $(timebase)"
         description="The kernel exit routine was unexpectedly invoked"
         service_action="Please report this error message to IBM Support"
         relevant_diags="checkup"
         />
       */
       RAS_KERNELUNEXPECTEDINTERRUPT,
       /*
         <rasevent 
         id="0001000A"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="END_JOB,FREE_COMPUTE_BLOCK"
         message="CNK Unexpected GEA Interrupt"
         description="The kernel interrupt was unexpectedly raised"
         service_action="Please report this error message to IBM Support"
         relevant_diags="checkup"
         />
       */
       RAS_KERNELVERSIONMISMATCH,
       /*
         <rasevent
         id="0001000B"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="For message service $(%d,service) CNK protocol version $(%d,myver) does not match CIOS protocol version $(%d,ciosver)."
         description="There is a mismatch between the CNK version and the CIOS version on the I/O node."
         service_action="$(CheckLevels)"
         />
       */
       RAS_KERNELCNVCONNECTFAIL,
       /*
         <rasevent
         id="0001000C"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="SOFTWARE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
         message="CNK:  Unable to connect CNV to address $(%lx,ADDR) port $(%ld,PORT) with return code $(%ld,RETCODE). Total failed nodes $(%ld,TOTALFAILS)"
         description="The compute node verbs (CNV) was unable to establish a connection with another service.  This could indicate a software level mismatch, bad address/port, or a service that isn't running."
         service_action="$(CheckLevels)"
         />
       */
       RAS_KERNELFATALMUND,
       /*
         <rasevent 
         id="0001000D"
         category="Software_Error"
         component="CNK"
         severity="FATAL"
         control_action="END_JOB,FREE_COMPUTE_BLOCK"
         message="CNK Unexpected MU or ND interrupt.  ND NFatal=$(nfe0) $(nfe1) Fatal=$(fe0) $(fe1) $(fe2) $(fe3) $(fe4) $(fe5) $(fe6) $(fe7) $(fe8) $(fe9) $(fe10) MU INTS=$(mu0) $(mu1) $(mu2) $(mu3) $(mu4) $(mu5) $(mu6) $(mu7) $(mu8) $(mu9) $(mu10) $(mu11) $(mu12)"
         description="An unexpected fatal interrupt condition occurred from the Messaging Unit or Network Device hardware. On rare occasions, this class of interrupt could be bad hardware but the initial debug assumption should always start with a software defect."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         relevant_diags="checkup"
      />
    */

       RAS_DCRVIOLATION,
       /*
         <rasevent 
         id="0001000E"
         category="Software_Error"
         component="CNK"
         severity="WARN"
         control_action=""
         message="CNK detected DCR violation.  DCR_STATUS $(dcrnum)=$(status)"
         description="An unexpected fatal interrupt condition occurred from device control ring hardware. On rare occasions, this class of interrupt could be bad hardware but the initial debug assumption should always start with a software defect."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         relevant_diags="checkup"
      />
    */

       RAS_IPIINTERRUPT,
    /*
      <rasevent 
      id="0001000F"
      category="Software_Error"
      component="CNK"
      severity="WARN"
      control_action=""
      message="CNK detected un-delivered IPI Message. Sent from processor id $(%d,fromcpu) to processor id $(%d,tocpu)"
      description="An IPI message was found in a kernel buffer and no pending interrupt was detected in the destination processor. The message was then taken from the buffer and executed. This class of interrupt could be bad hardware."
      service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
      relevant_diags="checkup"
   />
 */

    RAS_IPINULLFUNCTION,
 /*
   <rasevent 
   id="00010010"
   category="Software_Error"
   component="CNK"
   severity="WARN"
   control_action=""
   message="CNK detected a NULL IPI target function pointer. Sent from processor id $(%d,fromcpu) to processor id $(%d,tocpu)"
   description="An IPI interrupt was delivered to a processor but no function pointer existed in the corresponding message data. This class of interrupt could be bad hardware."
   service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
   relevant_diags="checkup"
/>
*/

    RAS_UPCCPARITY,
/*
   <rasevent
  id="00010011"
  category="UPC"
  component="CNK"
  severity="WARN"
  message="UPC Hardware error detected. $(UPC_C_INT_STATE), $(INTERNAL_ERROR_STATE), $(UPC_C_INT_FIRST), $(INTERNAL_ERROR_FIRST), $(INTERNAL_SW_INFO), $(INTERNAL_HW_INFO), $(SRAM_PARITY_INFO), $(IOSRAM_PARITY_INFO) "
  description="An unexpected UPC interrupt condition occurred, most likely a temporary condition, but may effect the reliability of hardware performance counter results. On rare occasions, this interrupt could be bad hardware, but is likely to be a single temporary condition."
  control_action=""
  service_action="$(Diagnostics)"
  relevant_diags="processor"
/>
*/

    RAS_MAPFILEOPENFAIL,
/*
   <rasevent
  id="00010012"
  category="Software_Error"
  component="CNK"
  severity="WARN"
  message="CNK could not open the specified mapfile"
  description="The compute node kernel was unable to open the mapfile specified by the control system.  This is an ASCII RAS message containing the path that was specified to CNK.  It should be accessible from all ionodes used by the compute block.  To protect against a RAS storm, this RAS message will only originate from the compute node with torus coordinates 0,0,0,0,0."
  control_action=""
  service_action="$(Diagnostics)"
  relevant_diags="processor"
/>
*/


};
