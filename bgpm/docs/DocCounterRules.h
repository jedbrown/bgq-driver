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
 * \page bgpm_counter_rules Simultaneous Counter Rules

\htmlonly
<div class="toc"><!--[if IE]></div><div class="ie-toc"><![endif]-->
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref bgpm_scope_rules
- \ref bgpm_punit_rules
  - \ref A2
  - \ref bgpm_l1p_rules
  - \ref bgpm_opcode_rules
- \ref bgpm_rules_debugging
  - \ref bgpm_rule_feedback
- \ref bgpm_rule_attributes
\htmlonly
</div>
<div class="contents-right"><!--[if IE]></div><div class="ie-contents-right"><![endif]-->
\endhtmlonly

 * The various counting units provide a few types of counters with varying counting and control
 * nuances. This page describes these differences in some detail to help you understand
 * how to use the counters.
 *
 * \section bgpm_scope_rules Event Scopes
 *
 * Each event is assigned a event scope value (see \ref event_table_legend).
 * The scope value indicates both breadth of the event counts, as well as the level of control
 * available:
 *
 * - \b Thread - Threaded events have the simplest utility.
 *   These punit only events always use counters detected to a particular thread, and the events
 *   are directly attributable to that thread.
 *   In the distributed software modes (\ref BGPM_MODE_LLDISTRIB and \ref BGPM_MODE_SWDISTRIB)
 *   an application controls (start & stop) a threads counters independently of other threads.
 *   Threaded events in software modes are also paused when a software thread is swapped in
 *   a hardware thread.
 *
 * - \b Core - Core events are of course core wide, and so normally cannot be attributed only
 *   only a single thread.  However, each thread uses a detected hardware counter, so each
 *   thread has separate control of the state of the counter (reset, start/stop etc.).
 *   Even if the job is running with only one thread per core, there may be communications
 *   threads which may also run on the same core in some circumstances, and thus effect the
 *   results.  Core events are paused when a software thread is swapped.
 *
 * - \b Core-Shared - The core-shared events are also core-wide, however only a single
 *   hardware counter is able to use the event, and there may be only one software thread
 *   per core allowed to gather performance counters.  Thus, this one thread per core
 *   also has independent control of the counters.  Since there can be only one software thread
 *   in control of these events, they are paused when the software thread is swapped out of a
 *   core.
 *
 * - \b Node-Shared - The node shared events use shared counters across the node,
 *   and are thus not directly attributable to any particular thread.
 *   The L2 and I/O unit events are node-shared.  The shared nature means that any software
 *   thread may start or stop the counters on a 1st come/1st served basis, including reset or
 *   writing the values.  Thus, the application needs to be careful.  Node shared events
 *   do not pause when software threads are swapped.
 *
 * - \b Node - The node wide counters (CNK events) provide a software thread view of
 *   node wide CNK software events. The event counts are not attributable to any particular
 *   thread, but are only effectively accumulated while the event set is running.
 *   Also, resets, and writes operations effect only the current threads view of the counts.
 *   Node events are not paused when software threads are swapped.
 *   There is a Bgpm_CNK_SampleEvent() operation which can read the absolute value since boot time
 *   for an assigned CNK event.
 *
 * - \b Exclusive - the NW events have exclusive scope.
 *   These means the events are node wide,
 *   but only one software thread and event set may have configuration or start/stop/read control
 *   of a particular links counters (via a Bgpm_Attach() operation which returns an attached event set).
 *   Other event sets may attempt to configure/start/stop and
 *   read a links counters, but the operations return a warning error code (positive return code) and
 *   return a 0 in the value during a Bgpm_ReadEvent().
 *
 * - \b Sampled - Sampled event are node-wide, but return a sampled value since boot time,
 *   There is no start/stop or controls.  The Sampled scope is currently not implemented, but
 *   may have future use.
 *
 *
 * \section bgpm_punit_rules Punit Simultaneous Event Rules
 *
 * A Punit contains multiple subunits signals which shared the same set of counters.
 * Each subunit has varying constraints, which makes the simultaneous counting rules complicated.
 * However, in general, the Punit counts up to 24 events per core, split between the 4 hw threads
 * in the following fashion:
 *
 * The counters are assigned to hardware theads in a 1st come / 1st served fashion as events are
 *   added to an event set.  However, the assigned counters have an affinity to a
 *   particular hardware thread to support efficient overflow interrupts, and prevent collisions
 *   when the counters are not over-committed.
 *   - In general, you may allocate 6 events/counters per thread when all for hw threads on a core
 *     are using punit events.  12 events/counters when only 2 hw threads are counting.  All 24
 *     counters are available for simultaneous counting when only when thread per core is using the
 *     counters.
 *   - Each thread starts out with 6 counters associated with the hardware thread.
 *     Thus, when a punit event is first added, an event set for hw thread 0
 *     is assigned counters 0,4,8,12,16,20. Hw thread 1 = 1,5,9,13,17,21.  Likewise for other threads.
 *   - When a 7th event is assigned to a punit event set, the Bgpm software checks if an event
 *     exists for a "sibling" (mentioned later) hardware thread on a core.
 *     If that hw thread is free, then the "siblings" counters are reserved by use
 *     for event sets on the current hw thread.
 *     Likewise, when a 13th event is assigned, all 24 counters are reserved if possible
 *     for use the current hardware thread.
 *   - Hw threads 0&1 or 2&3 are considered "siblings" due to some nuances in the
 *     event signal programming in the A2 processor core.
 *     This relationship allows for the maximum simultaneous allocation of events counters to
 *     software thread.  It is also a reason why CNK schedules software threads to
 *     hw threads 0 and 2 before using threads 1 and 3.
 *
 * \subsection A2 Subunit Rules
 * The A2 subunits (XU, AXU, MMU, LSU, and IU) may each count up to 2 events when all 4 hw threads are
 *   busy counting (up to a total of 6 counters). Each subunit may count 4 events (up to 12 counters)
 *   when only 2 non-sibling threads are counting per core. Each A2 subunit is assigned 8 programmable
 *   event signals, which are distributed to the available counters when possible.
 *   Some further hardware constraints restrict the XU subunit events to counters 16-23.
 *
 *
 * \subsection bgpm_l1p_rules Punit L1p and Wakeup Counter Rules
 *
 * The L1p and Wakeup threaded events may only be counted by a single counter per thread, and the counters
 *   are over-committed.
 *   The result is there are L1p events where counters conflict (see list below).
 *   In addition, the L1p hardware event signals may only be configured for one counting mode per core
 *   (List, Switch or Stream) and is not multiplexable.  The "base" L1p events are are available
 *   with any L1p counting mode. In general, you will not be able to count more than 6 threaded L1p events
 *   per thread per core, no matter the number of hw thread doing counting on the core.
 *   The core-wide events do have a little more flexibility.
 *   \n\n
 *   The following table shows which events for each L1p mode
 *   share the same counter, and so cannot be counted simultaneously:
 *   \n
 *   \htmlinclude L1pConflicts.html
 *
 *
 * \subsection bgpm_opcode_rules Punit Opcode (instruction) counter rules
 *
 * The opcode events may be counted by any of the 24 counters by any thread.
 * So, counters beyond the initial 6 or 12 can only be used by the opcode events.
 * Opcode events count events in predefined groups, or allows you to define one event
 * which will match an opcode pattern.  See the table at \ref bgpm_opcodes to see which
 * events are in each group, and mask patterns to pass to select a particular opcode.
 *
 *
 * \section bgpm_rules_debugging Debugging Counter Collisions (Feedback)
 *
 * There is no need to assign events in an order which will make the best simultaneous use of the
 * available counters.  Whenever a new event is added to an event set, Bgpm will reassign the existing
 * events in an event set to assure the best use of the available counters.
 *
 * But, what do you do when Bgpm won't let you assign the events in your list?
 * With the complicated assignment rules, it can be difficult to understand what Bgpm doesn't
 * like about the events in the list.  To help provide a clue where a conflict exists,
 * Bgpm gives options to provide feedback by printing the list of events which have been successfully assigned to
 * counters in the optimal order, and which A2 signals and counters on the core are possible
 * are used.
 *
 * For example, after a failure to assign events to using 2 threads per core, Bgpm may print the
 * following:
 *
\verbatim
stderr[4]:                                                                         -------- Counters --------
stderr[4]:                                                        UnitSigs         00000000 00111111 11112222
stderr[4]: EvtLabel                       EvtIdx PuEvtIdx Unit    01234567 MaxCtrs 01234567 89012345 67890123
stderr[4]: hwThread[3]                         -        - -       --------      24 ******** ******** ********
stderr[4]: UnitSignals->Counters                                                   02461357 02461357 02461357
stderr[4]: PEVT_XU_THRD_RUNNING                4        0 XU      ----**X*       4 -------- -------- --*X--**
stderr[4]: PEVT_XU_TIMEBASE_TICK               5        1 XU      ----***X       4 -------- -------- --**--*X
stderr[4]: PEVT_XU_SPR_READ_COMMIT             6        2 XU      ----X***       4 -------- -------- --X*--**
stderr[4]: PEVT_XU_SPR_WRITE_COMMIT            7        3 XU      ----*X**       4 -------- -------- --**--X*
stderr[4]: PEVT_AXU_INSTR_COMMIT               0        4 AXU     ----**X*      12 --*X--** --**--** --**--**
stderr[4]: PEVT_AXU_CR_COMMIT                  1        5 AXU     ----***X      12 --**--*X --**--** --**--**
stderr[4]: PEVT_AXU_IDLE                       2        6 AXU     ----X***      12 --X*--** --**--** --**--**
stderr[4]: PEVT_AXU_FP_DS_ACTIVE               3        7 AXU     ----*X**      12 --**--X* --**--** --**--**
stderr[4]: PEVT_MMU_TLB_HIT_DIRECT_IERAT       8        8 MMU     ----**X*      12 --**--** --*X--** --**--**
stderr[4]: PEVT_MMU_TLB_MISS_DIRECT_IERAT      9        9 MMU     ----***X      12 --**--** --**--*X --**--**
stderr[4]: PEVT_MMU_TLB_MISS_INDIR_IERAT      10       10 MMU     ----X***      12 --**--** --X*--** --**--**
stderr[4]: PEVT_MMU_HTW_HIT_IERAT             11       11 MMU     ----*X**      12 --**--** --**--X* --**--**
stderr[4]: PEVT_MMU_HTW_MISS_IERAT             -        - MMU     ----****      12 --**--** --**--** --**--**
stderr[4]: Bgpm_AddEventList(R00-M0-N10-J00:35) Error at ../ES_SWPunit.cc:482: err=-1007; Counter Reserved (possibles for event are all used).
\endverbatim
 *
 * - EvtLabel shows the Bgpm event label in the optimal order to add to the punit (order shown by puEvtIdx).
 * - The first lines labeled "hwThread[?] indicate what counters have been reserved to the threads on the
 *   core.  It will only show threads which actually have punit event sets assigned to them.
 *   The current thread counters will be marked with the '*', and other thread counters will be marked with
 *   a 'X'.
 *   The rest of the lines indicate the event labels assigned or attempted to assign.
 * - EvtIdx gives the original order as added by the Bgpm_AddEvent or Bgpm_AddEventList operation.
 *   Not all events in event set may be shown, it only gives the list of events up to the failing event.
 *   In the above example, the XU subunit has to least flexibility to choose among the counters,
 *   so those events are assigned first.
 * - UnitSigs show the 8 available signals for each of the 5 A2 units.  There are actually 40 separate A2 event signals,
 *   but only the 8 for the particular subunit is shown in each row.
 *   An X indicates which signal was assigned.  A * indicates that this particular event is possible
 *   to assign to up to 4 subunit signals in this case.
 *   Thus, if you match up the X's in a column for the same subunit, you can
 *   see when all available signals have been used.  In some cases, only certain signals are possible
 *   but that is not shown in the scenario above.
 * - MaxCtrs indicate what are the maximum counters which can be used by this punit event type.
 *   For the XU, and with 2 non-sibling threads per core, only 4 counters are possible.
 *   The other A2 subunits are able to use up to 12 counters.
 * - Counters shows all 24 signals for the punit which are possible to use for this event, and which counter was actually chosen.
 *   Again, columns which have an X indicate which counters are filled.
 * - The row labeled "UnitSignals->Counters" is always printed and indicates which unit signals map to which counters.
 *   That is, each unit signal is only possible to map to specific counters.  Thus, there are cases where
 *   you might find counters are still available for an event, but there isn't a matching signal
 *   available for that counter. In this case you will see an "A2 Signals Reserved" error.
 * - The last row indicates the event assignment which failed.  In this case it failed because all the possible counters have
 *   already been assigned.  But, it would have also failed because MMU subunit is out of possible signals.
 * \n
 *
 * Here is an example of what happens when assignment of events from two different L1p modes occur:
 * \n
 *
\verbatim
stderr[0]:                                                                       -------- Counters --------
stderr[0]:                                                      UnitSigs         00000000 00111111 11112222
stderr[0]: EvtLabel                     EvtIdx PuEvtIdx Unit    01234567 MaxCtrs 01234567 89012345 67890123
stderr[0]: hwThread[0]                       -        - -       --------       6 X---X--- X---X--- X---X---
stderr[0]: hwThread[1]                       -        - -       --------       6 -X---X-- -X---X-- -X---X--
stderr[0]: hwThread[2]                       -        - -       --------       6 --*---*- --*---*- --*---*-
stderr[0]: hwThread[3]                       -        - -       --------       6 ---X---X ---X---X ---X---X
stderr[0]: UnitSignals->Counters                                                 02461357 02461357 02461357
stderr[0]: PEVT_L1P_SW_MAS_SW_DATA_GATE      0        0 L1PSW   --------       1 -------- -------- ------X-
stderr[0]: PEVT_L1P_SW_SR_MAS_RD_VAL_2       1        1 L1PSW   --------       1 --X----- -------- --------
stderr[0]: PEVT_L1P_LIST_SKIP_1              -        - L1PLIST --------       4 ****---- -------- --------
stderr[0]: Bgpm_AddEventList(R00-M0-N10-J00:06) Error at ../ES_SWPunit.cc:482: err=-1008; Current L1p mode for this core conflicts with requested event L1p mode.
\endverbatim
 *
 * Again, there are two reasons the L1p assign may have failed (the necessary counter was already used), but the L1p mode
 * conflict was detected first, as indicated by the failure message.
 *
 * \subsection bgpm_rule_feedback  Feedback options
 *
 * By default, Bgpm will print feedback the 1st time counter assignment fails on a node by any process.
 * (unless \ref BGPM_NO_PRINT_ON_ERR environment variable is passed).
 * You may modify how many processes or threads will print via the \ref BGPM_EVT_FEEDBACK environment variable.
 *
 *
 * \section bgpm_rule_attributes  Punit Event Attribute Conflicts
 *
 * A Punit event set has the most possible attributes, and most of these are core-wide settings.
 * Thus, when multiple threads on a core attempt to set these attributes, they must be consistent.
 * The following lists these attributes and how they may interact:
 *
 * - Bgpm_SetQfpuMatch() sets the opcode match and mask values along with a FP scaling value
 *   relevant to the \ref PEVT_INST_QFPU_MATCH event.
 *   Applied to an event set, multiple threads on the core require the same value.
 *   See \ref bgpm_opcodes for more information.
 *
 * - Bgpm_SetQfpuMatch() sets the opcode match and mask values
 *   relevant to the \ref PEVT_INST_XU_MATCH event.
 *   Applied to an event set, multiple threads on the core require the same value.
 *   See \ref bgpm_opcodes for more information.
 *
 * - Bgpm_SetFpSqrScale() and Bgpm_SetFpDivScale() respectively set the the square root
 *   and division operation scaling factors.  These are used when counting floating point
 *   operations rather than instructions. The FP scaling factors default to 8 per instruction.
 *   Relevant to events PEVT_INST_QFPU_FPGRP1 and PEVT_INST_QFPU_FPGRP2, as well as
 *   PEVT_INST_QFPU_GRP_MASK with Bgpm_SetQfpuFp() set on the event.
 *
 * - Bgpm_SetContext() indicates whether to count events when in user or kernel state or both.
 *   However, this context is only relevant to Punit A2 events.  L1p, Wakeup and Opcode events cannot
 *   be filtered by the context. Thus, when an event set has other than A2 events, the context
 *   cannot be set to anything other than "both".
 *
 *
 *
 *
 *
 *
 *
 */

