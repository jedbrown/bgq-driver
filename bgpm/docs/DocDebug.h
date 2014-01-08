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
/*!
 * \page bgpm_debug BGPM Debug Notes
 *
 * Notes about options to help debug BGPM code and use by applications.
 *
 * \section bgpm_comp_flags Compile Time Flags
 * - BGPM_PRT_ERR_LOC - when defined, include file and line # where a user error is printed by BGPM code.
 *
 *
 *
 * \section bgpm_sw_errors BGPM Return Code Error Details
 *
 * This section provides further details about some BGPM error messages.
 * It is not a comprehensive list.
 * Look at bgpm/include/err.h for a list of error defines and numbers and short descriptions.
 * The following list refers to the define names, though error messages will include the
 * error number and short description, so you will need to manually cross reference.
 *
 * - \ref BGPM_ETHD_COUNTS_RSV  In some cases, an attempt to Bgpm_Attach() a Punit
 *   event set to a hardware thread can fail because the configured event counters are
 *   inconsistent with other threads already using the counters.
 *   If only 6 counters are assigned to an event set, then this error will not occur.
 *   But, when 6 to 12 counters are assigned, then only one of hardware thread
 *   threads 0 and 1 on a core, or threads 2 & 3 on a core may be attached.
 *   When 12 to 24 counters are assigned, then only 1 thread on a core may be attached.
 *
 */
