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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef MMCS_RAS_H
#define MMCS_RAS_H

/**
<rasevent
  id="00060009"
  category="Block"
  component="MMCS"
  severity="WARN"
  message="Midplane has inconsistent node memory."
  description="Inconsistent memory."
  service_action="Validate hardware against database and block definition."
 />
<rasevent
  id="0006000A"
  category="Block"
  component="MMCS"
  severity="WARN"
  message="Block failed to boot."
  description="Boot failure."
  service_action="Make sure the block is valid, the hardware is available, and check for additional hardware RAS."
  details="BLOCKID"
 />

// 0006000B is missing, it was a duplicate security RAS event

<rasevent
  id="0006000C" 
  category="Process" 
  component="MMCS"
  severity="INFO"
  message="user $(JOB) cannot be killed.  The runjob_server cannot be reached."
  description="Cannot connect to runjob_server so mmcs_server cannot kill the RAS specified job."
  service_action="Make sure that runjob_server is running and correctly configured."
/>
<rasevent
  id="0006000D" 
  category="Process" 
  component="MMCS"
  severity="INFO"
  message="I/O node usage error.  Block $(BLOCK) cannot boot.  Limit exceeded for $(NODE).  Usage count is $(COUNT).  Limit is $(LIMIT)."
  description="Insufficient I/O for this block to boot."
  service_action="Check for failed I/O nodes or cables in error."
  details="BLOCK, NODE, COUNT, LIMIT"
/>
<rasevent
  id="0006000E"
  category="Process"
  component="MMCS"
  severity="INFO"
  message="inject_ras dummy RAS event for debug."
  description="inject_ras event"
  service_action="None"
 />
<rasevent
  id="0006000F"
  category="Process"
  component="MMCS"
  severity="WARN"
  message="mmcs_server could not connect to runjob_server"
  description="runjob_server connection failed"
  service_action="Make sure that runjob_server is running and that it is configured correctly in the properties file."
 />
*/


#define MMCSOps_0009 (0x00060009)  // MEMORY_MISMATCH: midplane contains inconsistent node memory
#define MMCSOps_000A (0x0006000A)  // BOOT_FAIL
#define MMCSOps_0301 (0x00063001)  // Authority fail
#define MMCSOps_000C (0x0006000C)  // runjob_server fail
#define MMCSOps_000D (0x0006000D)  // Mundy RAS
#define MMCSOps_000E (0x0006000E)  // INJECT_RAS: Dummy RAS event for testing RAS path
#define MMCSOps_000F (0x0006000F)  // NOTIFY_BLOCK: Attempt to notify runjob_server failed
#define barrier_ras  (0x00040096)
#endif
