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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#ifndef _TEST_RAS_H
#define _TEST_RAS_H

// RAS Event definition for testing only
// The range of RAS IDs is FFFE0000 - FFFEFFFF


/*

<rasevent  id="0xFFFE0000"  component="TEST"
  category="Software_Error"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="SOFTWARE_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE0001"  component="TEST"
  category="Coolant_Monitor"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="RACK_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE0002"  component="TEST"
  category="BQL"
  severity="WARN"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="CABLE_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE0003"  component="TEST"
  category="BQL"
  severity="WARN"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="BQL_SPARE"
  service_action="None"
 />

<rasevent  id="0xFFFE0004"  component="TEST"
  category="Software_Error"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="FREE_COMPUTE_BLOCK,SOFTWARE_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE0007"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="END_JOB,FREE_COMPUTE_BLOCK"
  service_action="None"
 />

<rasevent  id="0xFFFE0008"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="END_JOB,FREE_COMPUTE_BLOCK,COMPUTE_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE0009"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="END_JOB,FREE_COMPUTE_BLOCK,BOARD_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE000A"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="END_JOB,FREE_COMPUTE_BLOCK,SOFTWARE_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE000B"  component="TEST"
  category="BQC"
  severity="WARN"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  threshold_count="10"
  service_action="None"
 />

<rasevent  id="0xFFFE000C"  component="TEST"
  category="BQC"
  severity="WARN"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  threshold_count="10"
  threshold_period="2 HOURS"
  service_action="None"
 />

<rasevent  id="0xFFFE000D"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="COMPUTE_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE000E"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="BOARD_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE000F"  component="TEST"
  category="Software_Error"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="END_JOB"
  service_action="None"
 />

<rasevent  id="0xFFFE0010"  component="TEST"
  category="BQL"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="COMPUTE_IN_ERROR"
  service_action="None"
 />

<rasevent  id="0xFFFE0011"  component="TEST"
  category="BQL"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="END_JOB"
  service_action="None"
 />

<rasevent  id="0xFFFE0013"  component="TEST"
  category="BQL"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="FREE_COMPUTE_BLOCK"
  service_action="None"
 />

<rasevent  id="0xFFFE0014"  component="TEST"
  category="Job"
  severity="WARN"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  threshold_count="1"
  service_action="None"
 />

<rasevent  id="0xFFFE0015"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="COMPUTE_IN_ERROR"
  threshold_count="1"
  threshold_period="1 HOUR"
  service_action="None"
 />

<rasevent  id="0xFFFE0016"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="BOARD_IN_ERROR"
  threshold_count="1"
  threshold_period="1 HOUR"
  service_action="None"
 />

<rasevent  id="0xFFFE0017"  component="TEST"
  category="BQC"
  severity="FATAL"
  message="This is a test ras message."
  description="This RAS is used for testing purpose only."
  control_action="END_JOB"
  threshold_count="1"
  threshold_period="1 HOUR"
  service_action="None"
 />

*/

#endif
