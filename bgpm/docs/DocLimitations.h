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
 * \page bgpm_limitations BGPM Limitations
 *
 * This page high-lights some of the limitations or bumps in the design or implementation of BGPM and UPC.
 * These items are mostly mentioned elsewhere, but are deemed important enough to mention as a group.
 *
 * \section general_limits Job and Thread Limitations
 *
 * - CNK Sub-block jobs are incompatible with BGPM support and Bgpm operations will fail.
 *   Sub-block jobs allow multiple jobs to exist simultaneously on a node.
 *   BGPM operations are not available within sub-block jobs.
 *   Is possible for UPCI and UPC to operate, but is not supported.
 *
 * - Only one software thread performing BGPM counting may exist for a hardware thread
 *   at a time.  That is, it's possible to over-commit a hardware thread with software
 *   threads, but only one may be using BGPM.  In software modes, the punit counters are
 *   paused when a thread is swapped out. Other node-wide counts do not pause.
 *
 *
 * - BGPM only operates with "bound" threads.
 *   That is, software threads cannot move to new hardware thread after Bgpm_Init() has occurred.
 *   BGPM operations run and talk to hardware in user state as much as possible.
 *   Consequently, software threads cannot move between hardware threads without breaking
 *   a software thread BGPM operation.  BGPM doesn't prevent the sw threads from moving, but
 *   with CNK it requires an explicit sched_setaffinity operation.
 *   If a thread moves, the operations may fail, or results be silently corrupted.
 *
 *
 * \section counting_limits Counter limitations
 *
 * - Operations to write or reset a counter are time expensive, unless using the \ref BGPM_MODE_LLDISTRIB mode
 *   with the low latency access method.
 *
 * - Counters have an 800 Cycle counter update latency in \ref BGPM_MODE_SWDISTRIB mode.
 *   \n\n
 *   It normally takes up to 800 cycles before events are recorded in the
 *   readable counter (UPC_C SRAM). This 800 cycles latency exists for
 *   the Punit, L2unit, and I/O Units.
 *   In order to allow events to
 *   reach counters before accessing the value, the Bgpm_Stop() operation
 *   waits 800 cycles before returning.
 *   If using the fast access operations, the caller is responsible to delay.
 *   before reading if accuracy is important.
 *   Internally, context switch and multiplex operations will also delay
 *   as needed.  This delay does not effect punit overflows, which detect overflow
 *   in a low latency counter armed within the 800 cycles delay before overflow.
 *
 * - 1600 Cycle counter update latency with \ref BGPM_MODE_HWDISTRIB  mode.
 *   \n\n
 *   When using a HW Perspective mode, the counter control is synchronized for
 *   all UPC_P, I/O and L2 counters.
 *   It takes up to 800 cycles to achieve simultaneous start or stop, and
 *   another 800 cycles for counters to be updated.
 *   Thus, in HW Perspective, the standard access start operation
 *   will delay 800 cycles after start before
 *   returning.
 *   The stop operation will wait 1600 cycles before returning.
 *
 *
 *
 * \section network_limits Network Counting Limitations
 *
 * - Only one software thread can control counters for a link at a time.
 *   \n\n
 *   A software thread must Bgpm_Attach() and Bgpm_Detach() a network event set to a set of links for collection.
 *
 * - It's not considered a failure for processes attach to links already owned.
 *   However, the event set configuration must be identical.
 *   Attempts to read the counters for unattached links will return a zero value.
 *   Only the first thread to attach to a link may control the configuration and counting
 *   for the link.
 *
 *
 * \section L2_limits L2 and I/O Counter limitations
 *
 * - The L2 and I/O counters are shared node wide, and there is no attempt to restrict the control to a
 *   single thread.  Thus, in \ref BGPM_MODE_SWDISTRIB mode, the 1st thread to start will start counting, and the 1st thread to stop will
 *   stop counting.  In the hardware modes the Punit, L2 and I/O start/stop is synchronized.
 *
 *
 */
