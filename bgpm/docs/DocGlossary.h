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
 *
 * \page bgpm_glossary BGPM Glossary
 *
 * - <b>UPC</b> - Universal Performance Counting (not to be confused with Unified Parallel C)
 *      refers to the performance monitoring
 *      hardware implementation on BG/Q (and prior BG systems).
 *      Where software interfaces with the hardware, or values refer to hardware
 *      settings, UPC is used in names and terminology.
 *
 * - <b>UPCI</b> - Universal Performance Counting Interface SPI layer cooperates with
 *      CNK syscalls to give user state visibility to needed privileged UPC operations.
 *
 * - <b>BGPM</b> - Hardware Performance Monitor is the primary term referring collectively
 *      to the hardware performance event counting instrumentation.  In software, it mostly refers
 *      to the BG/Q native user state API to control and access these counters.
 *
 * - <b>BGPM aware software thread</b> - Threads which have performed an Bgpm_Init(), which causes
 *      necessary memory structures to be created, and BGPM overflow interrupts to enabled
 *      for this software thread.
 *
 * - <b>Unit</b> - There are multiple event and counter units with varying capabilities
 *      and modes.  Each unit has a unique list of events to choose from.
 *      See \ref bgpm_units.
 *      A unit is similar to a PAPI-C Component, however there is no explicit assignment
 *      of event sets to units.  The unit associated with an event set is determined by
 *      the assigned events.
 *
 * - <b>BGPM Mode</b> - Selected during an initial Bgpm_Init(), the mode provides a gross
 *      direction of how the counting hardware and software is to operate, with varying
 *      capabilities and limitations.  See \ref bgpm_swfeatures_modes.
 *
 * - <b>Global Mode</b> - operating modes for all threads on node must agree.  The first thread to
 *      do Bgpm_Init() will determine the mode for the whole node.
 *
 * - \b Perspective - refers to a Punit hardware or software perspective.
 *      That is, whether events
 *      are to collected according to the hardware threads, or attributable to software
 *      threads.  The Perspective is a conceptual attribute of \ref Bgpm_Modes .
 *
 * - \b Context - The Punit A2 core collect events running in kernel
 *      context, user context, or both.  See \ref Bgpm_Context .
 *
 * - \b Granularity - Indicates how the Punit accumulates counts in a software perspective.
 *      That is, whether to accumulate the counts by software thread or process.
 *      Bgpm only accumulates counts by software thread.
 *
 * - \b Event - a condition signaled by the hardware or software which can be counted.
 *      Also refers to a particular event value selected from a components event table.
 *      See \ref bgpm_events .
 *
 * - <b>Event Set</b> - a list of events that a BGPM software thread is programmed to count.
 *      See \ref bgpm_swfeatures_eventsets
 *
 * - <b>Event Set Handle</b> - an integer handle identifying a unit's event set.
 *      Returned by Bgpm_CreateEventSet().
 *
 * - <b>Event Index</b> - after an event is added to an event set (i.e. Bgpm_AddEvent()) it
 *      is identified in other functions by an index into the event set.
 *
 * - <b>Reservations</b> - Most events don't map to a single hardware counter.
 *      Rather, events and event signals are added to an event set and mapped to counters.
 *      Available signal vary depending on other requested signals and operating modes.
 *      Thus, the the counting resources need to be "reserved" in such a way that
 *      no simultaneous run-time conflicts will occur.
 *      Internally, this is a "reservation" step which
 *      occurs as events are added to an event set.
 *
 * - <b>Overflow Threshold</b> - In order to profile code when a given number of events have
 *      occurred, we assign a threshold value to a counter which causes it to overflow on the
 *      desired number of events.  The overflow causes an interrupt to call a Bgpm registered
 *      signal handler to
 *      capture a hardware threads instruction address at the time of the interrupt.
 *      An application supplied overflow handler can be
 *      used to profile these instruction addresses.
 *
 * - <b>Fast Overflow</b> - In distributed BGPM modes, the Punit events can detect a
 *      counter overflow within few cycles and is software thread aware for hw thread
 *      specific events.  We term this a "fast overflow".
 *
 * - <b>Slow Overflow</b> - The L2 unit and I/O units produce node-wide events.
 *      These interrupts are termed "Slow Overflow" because it can take up to
 *      800 cycles for the overflow to be recognized, which makes the instruction address
 *      further removed from the event.  The Detailed BGPM Modes use Slow Overflow,
 *      and most of the Detailed mode events are hw thread specific.
 *      However, the L2 unit and I/O unit overflows
 *      are broadcast to the all cores and hardware threads, and it's a challenge
 *      to analyze the captured instruction address.
 *
 *
 *
 */
