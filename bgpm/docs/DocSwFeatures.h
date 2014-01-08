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
 * \page bgpm_swfeatures BGPM Software Features and Concepts
 *
 * \if HTML

\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref bgpm_swfeatures_modes
  - \ref bgpm_perspective
- \ref bgpm_swfeatures_eventsets
- \ref bgpm_swfeatures_events
- \ref bgpm_swfeatures_eventattrib
- \ref bgpm_swfeatures_evtscope
- \ref bgpm_swfeatures_access_methods
  - \ref bgpm_swfeatures_standard_access
  - \ref bgpm_swfeatures_fast_access
  - \ref bgpm_swfeatures_lowlat_access
- \ref bgpm_swfeatures_overflow
  - \ref bgpm_swfeatures_fast_overflow
  - \ref bgpm_swfeatures_slow_overflow
- \ref bgpm_swfeatures_multiplex
- \ref bgpm_concept_l2_combine
- \ref bgpm_swfeatures_rsv
- \ref bgpm_swfeatures_error
\htmlonly
</div>
<div class="contents-right">
\endhtmlonly

 * \endif
 *
 *
 *
 * \section bgpm_swfeatures_modes BGPM Operation Modes
 *
 * The underlying BGPM hardware (UPC) is comprised of
 * multiple hardware components, designed to work independently or together.
 * The Bgpm_Modes provide a gross indication of the hardware
 * interaction, or how the software should distribute or control resources
 * between threads. The Bgpm_Mode must be consistent for all sw threads
 * on a node.
 *
 * In conjunction with \ref bgpm_swfeatures_access_methods,
 * the modes offer varying capabilities and number of counters.
 * Only the Punit, L2, and I/O units are impacted by these modes.
 * Other units operate independently and have static capabilities.
 *
 * The following somewhat complex table summarizes the Punit features available in each mode.
 * Followed by a more verbose list of features and modes.
 * In essence, the number of available counters and features depends on the mode, source of the event,
 * and the hw threads being used per core.
 *
 * \htmlinclude ModeTable.html
 *
 *
 *
 *
 *
 * The active BGPM Operation Mode and options are determined by the 1st
 * user process to call Bgpm_Init() on the node,
 * or may be assigned at job start via the \ref bgpm_job_env.
 *
 * @copydoc bgpm::Bgpm_Modes
 *
 *
 * \subsection bgpm_perspective BGPM Hardware vs Software Perspectives
 *
 * By default, BGPM counts events from a software thread perspective.
 * it tracks the current software thread through context switches (sw thread swaps) to
 * attribute events with the software thread.
 * Note however, that Bgpm only supports one software thread per hardware thread,
 * so a software thread swap mostly amounts to stopping and restarting counting.
 * A software perspective applies to
 * \ref BGPM_MODE_LLDISTRIB and \ref BGPM_MODE_SWDISTRIB modes.
 *
 * You may also count events from a hardware perspective,
 * where generally a single process thread
 * is used to select and control the collection,
 * and chooses which hardware thread to collect.
 * Events are then counted irrespective of the software thread
 * running on the hardware. Also, the start/stop of Punit, I/O and L2 event sets is
 * synchronized by hardware (first call wins regardless of calling thread).
 * A hardware perspective applies to
 * \ref BGPM_MODE_HWDISTRIB mode.
 *
 * @note The hardware perspective requires an explicit attachment of Punit event sets to
 * hardware threads through the Bgpm_Attach() function.
 *
 *
 *
 *
 *
 * \section bgpm_swfeatures_eventsets Unit Event Sets
 *
 * Unit events are added to "Event Sets", which are lists of events to collect
 * in a session. Each event set, and the assigned events, belong to a single unit type
 * (\ref bgpm_units).
 * Thus, events for different units may not be mixed in the same event set.
 * The event set type is determined when the 1st event is assigned to an event set.
 * After events are added to event set (Bgpm_AddEvent()),
 * an event set is applied (Bgpm_Apply()) or attached (Bgpm_Attach()) to the hardware
 * resource to be counted.
 *
 * The events are distinguished via unique event ids (\ref bgpm_events).
 * Event sets, and events may be modified via optional attributes (\ref attrib_functions).
 * Most events are sufficiently defined by an event id,
 * but a few need further definition via a "filter mask" or other values
 * which further defines what to count.
 * For example, see \ref nwunit_attribute_functions or \ref bgpm_opcodes).
 *
 * If the calling code wishes to address a particular event later, it needs to
 * keep track of the order in which events are added, which is equivalent to the
 * 0 based index argument passed to most Bgpm functions.
 * That is, assigned events are addressed by event set
 * handle, and the event index within the event set
 * (for example, see \ref Bgpm_ReadEvent())
 *
 * There are others methods like Bgpm_GetEventIndex() or Bgpm_GetEventId()
 * to translate between the event id and event index.
 * However, because an event set may include multiple instances of the same event id,
 * the conversion may not be fully associative.
 *
 *
 *
 * \section bgpm_swfeatures_events Event Tags
 *
 * The events are mostly identified by EventId (see \ref bgpm_events).
 * The BGPM programs hardware registers to direct selected hardware event
 * signals through processing hardware to detect edges, levels, or inversions,
 * and directs these to selected counters. Mostly, these variations are
 * encoded into the available event ids.  For example, there is both a
 * \ref PEVT_IU_ICACHE_FETCH event, and a \ref PEVT_IU_ICACHE_FETCH_CYC event.
 * There is no need to select an icache_fetch event and choose to capture
 * edges or cycles to select whether to count cycles or events.
 * event ids encode this for you, and it's possible to count both events
 * simultaneously.
 *
 * However, it's not possible to get cycle counts for most events.
 * The event tables have a "tag" field which indicates the event/cycle
 * meaning of the fundamental hardware event.  Thus:
 * - b = Suitable for counting both cycles and number of events, but
 *       the fundamental event signal is split into be, or bc types.
 * - be = a "b" signal configured to count number of event by default
 * - bc = a "b" signal configured to count cycles by default
 * - c = Only suitable for counting cycles
 * - e = Only suitable for counting numbers of events
 * - s = Single cycle event, thus cycles and events are synonymous
 * - v = Single cycle event which can occur on consecutive cycles;
 *       thus cycles and events are synonymous
 *
 *
 *
 * \section bgpm_swfeatures_eventattrib Event Attribute Methods
 *
 * There are a great number of functions provided for configuring event set and
 * event attributes (see \ref attrib_functions).
 * Each attribute is generally supported by a different pair of
 * set and get attribute functions.  Any possible attributes must be set on
 * an event set prior to the first Bgpm_Apply() or Bgpm_Attach() operation.
 *
 * Separate set/get operations are used for attributes to avoid issues with
 * differing attribute structures for each Unit or subunit.
 * See \ref bgpm_units for an introduction to attributes for each unit,
 * unit events, and their effect.
 *
 *
 *
 *
 * \section bgpm_swfeatures_evtscope Event Scopes
 *
 * The are 6 "scope" characteristics defined with an event id which effect
 * the meaning and usage of an event counter.  Each event is labeled with the
 * scope characteristic in event tables.
 * See \ref bgpm_events and \ref bgpm_scope_rules for description.
 *
 *
 * \section bgpm_swfeatures_access_methods BGPM Access Methods
 *
 * Different methods of access to the counter control functions provide
 * varying overhead and consistency checks.
 *
 *
 * \subsection bgpm_swfeatures_standard_access Standard Access Methods
 *
 * The standard access methods (see \ref counter_functions)
 * use an event set handle
 * and an event index value into the event set to access
 * a particular assigned event.
 * In fact, most event operations work based off of the event set handle and index.
 * The standard access or control operations include extra checks to insure they
 * are used in the proper context, and that the events really point to the
 * correct counters, versus the other methods which avoid all checks.
 *
 *
 * \subsection bgpm_swfeatures_fast_access Fast Access Methods
 *
 * After configuring event sets,
 * the fast access methods (see \ref fast_punit_functions) allow
 * direct access to some hardware counters without any extra
 * indirection or consistency checks.
 * It requires that you obtain handles and control masks via a
 * Bgpm_Punit_GetHandles() (for example) function on the event set.
 * Then you pass the appropriate handles to start/stop/read counters.
 * Operations like reset and write are not available through these handles,
 * as they do have necessary overhead.
 *
 * Normally, stopping counters does insert some delays before returning to allow
 * the counter values to become stable before returning.
 * (800 cycles in sw perspective, 1600 cycles in hw perspective).
 * The fast methods do not account for these necessary delays, they are the
 * responsibility of the caller.
 *
 * Fast Access Methods are only reliable for events without overflow and when not multiplexing.
 *
 * See \ref bgpm/punit_fastaccess/main.c for an example
 *
 *
 *
 * \subsection bgpm_swfeatures_lowlat_access Punit Low Latency Access Methods
 *
 * In the The Low Latency access methods (see \ref counter_ll_handle_functions)
 * They provide direct and lowest latency access to
 * starting, stopping, reading and writing the Punit counters
 * via directly handles and inlined functions).
 *
 * The low latency functions only applies to the \ref BGPM_MODE_LLDISTRIB mode,
 * which provides for a 'fine-grained" operation of punit counters.
 * However, the counters are only 14 bits wide,
 * but can be accessed in O(40) hardware cycles versus the typical
 * O(100+) hardware cycles or more (plus extra software overhead).
 *
 * Also, 14 bit counters can easily overflow if not used carefully.  The hardware can
 * detect and report an overflow on a returned value via the BGPM_LLCOUNT_OVERFLOW() macro.
 *
 * See \ref bgpm/punit_lowlat/main.c for an example of use.
 *
 *
 *
 *
 *
 * \section bgpm_swfeatures_overflow Overflow Interrupt Handling
 *
 * Only the Punit, I/O and L2 UPC hardware support performance monitor interrupts when a
 * a programmed counter overflows.
 * The caller assigns a threshold period value to an event to cause an overflow
 * in the desired event period (see \ref Bgpm_SetOverflow()).
 * The Bpm_SetOverflowHandler() function is used to register a function to be called
 * within a Bgpm owned signal hander. There is handler per event set.
 * On overflow, the user's handler will be called with the event set handle,
 * instruction counter, and context structure.  Any profiling requirements
 * are then left to the user code.
 *
 * Some overflow and interrupt attributes:
 * - The UPC does not have a hardware register to capture the program or data
 *   addresses at overflow. Rather, the program counter is extracted from the interrupt context.
 *
 * - The A2 core and UPC hardware has dedicated performance monitor interrupt lines,
 *   and are considered external interrupts. Performance monitor interrupts are very low priority external
 *   interrupts, they can never occur while within the kernel.  Thus,
 *   any program addresses which are captured will only be from user state.
 *   So, depending on the circumstances, you may see a skewed number of addresses at entry
 *   points to kernel syscalls.
 *
 * - When an event is set for overflow, BGPM registers a private signal handler
 *   for handling the overflow. Punit counters can freeze on overflow
 *   until the overflow handling is complete.  However L2 and I/O units do not freeze on overflow.
 *   The L2 and I/O counts will be stopped when the interrupt is handled.  The signal handler restarts
 *   L2 and I/O counting when done.
 *
 * - All events assigned to a punit event set may be set for overflow. But, there are varying effects:
 *   \n\n
 *   - when a punit event set is assigned up to 6 counters, than the pm interrupts for each thread are
 *     independent.  When an overflow occurs, all 6 counters are stopped until the
 *     end of the signal handler.  There is a small window where some events can be
 *     lost until the interrupt and signal handler are delivered.
 *   \n\n
 *   - when a punit event set assigned over 6 counters, and all counters on the core are
 *     stopped on overflow, and the interrupt is broadcast to all Bgpm using threads on the core.
 *     The 1st signal handler will process all overflows and record the overflow in a queue
 *     for handling in all interested threads, and then restart counting at the
 *     end of the signal handler.
 *     So, in varying threads, a windows exists for counters to be active
 *     during the end handling of overflows. And, some thread counts
 *     may be polluted, some what, by the overflow processing, including the user's
 *     overflow handler.  Also, the window in which some events may be lost will increase.
 *
 * - If an overflow occurs while the interested software thread is swapped out, the overflow
 *   call will be missed, but recorded in a queue.
 *   When the next overflow occurs while the thread is swapped in, the
 *   overflow handler will also be called with queued entries -
 *   though the passed address and context will correspond
 *   with the current interrupt, rather than the original interrupt.
 *
  * \par
 * \subsection bgpm_swfeatures_fast_overflow Fast Overflow
 *
 * Punit counters, when run in distributed mode,
 * can detect a counter overflow and raise a thread interrupt within few cycles of the overflowing event (O(4)).
 * However, the counter value does take up to O(800) cycles before the readable counter value is updated.
 * This readable counter latency does not effect the overflow detection, so we refer to a distributed mode punit
 * overflow as a "fast overflow".
 *
 * \par
 * \subsection bgpm_swfeatures_slow_overflow Slow Overflow
 *
 * The I/O and L2 units take up to 800 processor cycles to accumulate an event and
 * detect an overflow.
 * Thus, so we refer to this as a "Slow Overflow", and the program counters may
 * skewed up to at least 800 cycles or more after the event.
 *
 * These overflows are broadcast to all bgpm active threads, and the Bgpm signal handler resolves
 * the counters and calls the registered overflow handlers for each applicable software thread
 * and event set.
 *
 *
 * \subsection Limitation: JUB
 *       There is a limitation for setting period value for a counter which is configured with overflow.This depends mainly on the time it takes to execute the overflow handler. In BGPM, with few experimentation, the minimum acceptable period value to set for PEVT_CYCLES event is value > 2000. This value is denpendent on the event we are configuring for overflow. and with the basic overflow handler without any user profiling added to the handler. If the user set a value less than the minimum acceptable threshold value of the event, it will create a hang inside the handler and the test case will eventually fail.
 *
 *
 * \section bgpm_swfeatures_multiplex Multiplexing Event Sets
 *
 * Bgpm supports multiplexing on Punit event sets only.
 *
 * A multiplexed set type will maintain groups of events which can
 * be counted simultaneously, while pushing conflicting events to other
 * internal groups.  A Muxed set also provides a method to switch between
 * sets, and captures the elapsed time a particular group of counters is active.
 *
 * There are some events which cannot be Mux'ed - like events between L1p modes,
 * The BGPM "event add" operations will detect and report these conditions.
 *
 * There are two methods to switch between internal groups of events.
 * You may call Bgpm_MuxSwitch() to force a switch in your mainline code, usually
 * between loops of the load to measure.
 * Or, you may specify a period in which Bgpm will implement a processor cycle
 * based overflow and perform the switch in the overflow handler.
 * (note that this cycle count cannot be made kernel or user context sensitive).
 *
 * Overflow on all muxed events is also supported.
 *
 * Multiplexing must be enabled via the Bgpm_SetMultiplex2() method prior to
 * adding events to an event set.
 *
 *
 *
 * \section bgpm_concept_l2_combine L2 Unit Event Combining
 *
 * L2 is composed of 16 units each managing a separate slice of the total memory space.
 * Each unit contains it's own version of the event counters. Since tracking the
 * events for a particular memory slice is unlikely to be meaningful or a particular
 * software thread, the BGPM hardware combines the separate slice counters into a single
 * set of event counters by default.
 * - The L2 combine counts are selected based on the selected events - there are different
 *   events for the combined counts, and the individual slices.
 * - It's not possible to mix event from individual slices and combined events.
 * - L2 event overflow is only supported against the "combined" events.
 * *
 *
 *
 * \section bgpm_swfeatures_rsv Reserving Counting Resources
 *
 * While events are added to an event set (\ref Bgpm_AddEvent()), event counting resources are
 * reserved.  However, some events are more restricted in their counters mapping.
 * And, the order in which events are added may effect the ability to add
 * events. Bgpm_AddEventList() and Bgpm_AddEvent will order or reorder events to assign them to counters
 * in the most efficient order (early versions of Bgpm did not support the reordering of events).
 *
 * Bgpm will assign punit events to counters in the following order:
 * - Wakeup events and/or L1p events
 * - a2_xu events
 * - other a2 events
 * - opcode events
 *
 * Also:
 * - Adding events from a list (Bgpm_AddEvent()) will automatically reserve in
 *   the best order, however it can be more difficult to determine why reservations fail.
 * - Enabling Multiplexing on an event set will cause conflicting Punit events to
 *   be reassigned into a multiplexed event set, if possible.
 *   If multiplexing is desired, it must set prior to adding
 *   events (Bgpm_SetMultiplex()).
 * - If job uses only 2 sw threads per core, then 12 punit counters may be
 *   reserved per thread.
 *
 * See \ref bgpm_counter_rules for more information about the reservation process.
 *
 *
 * \section bgpm_swfeatures_error Error Handling
 *
 * All BGPM error values are a negative integer.  Warning conditions are a positive integer.
 * Thus, most BGPM functions return a integer return code.
 * If the value is 0, then the
 * operation was successful. If the value is negative, it indicates the function has
 * failed for some reason. A few functions return a positive number indicating a non-fatal condition,
 * which you may wish to test.
 *
 * However, by default we expect that using the native BGPM interface, you might want to avoid
 * debugging side effects based on missing usage error checks that might arise.  So:
 * <b>By default, on errors BGPM prints a message and exits the program</b>.
 *
 * To avoid this exiting default behavior, there are functions which can disable the error handling
 * by thread: Bgpm_PrintOnError() and Bgpm_ExitOnError().
 * You may pass environment variable \ref BGPM_NO_EXIT_ON_ERR to disable this behavior.
 * There are also functions to return the last error value or string.  See \ref bgpm_error_functions
 * for more info.
 *
 *
 */
