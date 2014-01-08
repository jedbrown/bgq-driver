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
// 0006000C is missing, it was used when a job could not be killed

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

// 0006000E is missing, it was used for inject_ras
// 0006000F is missing, it was used for a runjob connection mis-configuration

*/

#define MMCSOps_0009 (0x00060009)  // MEMORY_MISMATCH: midplane contains inconsistent node memory
#define MMCSOps_000A (0x0006000A)  // BOOT_FAIL
#define MMCSOps_0301 (0x00063001)  // Authority fail
#define MMCSOps_000D (0x0006000D)  // Mundy RAS
#endif
