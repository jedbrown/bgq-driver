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
 * \page bgpm_units BGPM Collection Unit Descriptions
 *
 * \if HTML

\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref punit_desc
  - \ref punit_set_attrib
    - \ref bgpm_punit_context
  - \ref punit_event_attrib
    - \ref punit_fp_event_attrib
    - \ref punit_opcode_match
- \ref L2unit_desc
  - \ref bgpm_l2_attributes
  - \ref bgpm_l2_event_attributes
- \ref iounit_desc
  - \ref bgpm_io_attributes
  - \ref bgpm_io_event_attributes
- \ref nwunit_desc
  - \ref bgpm_nw_attach
  - \ref bgpm_nw_gen_events
  - \ref bgpm_nw_time_event
  - \ref bgpm_nw_set_attributes
  - \ref bgpm_nw_attributes
- \ref cnkunit_desc
\htmlonly
</div>
<div class="contents-right">
\endhtmlonly

 * \endif
 *
 * BGPM groups hardware events by supported hardware and software \b Units.
 *
 * Each unit is configured and generally controlled separately.
 *
 *
 *
 * \section punit_desc PUnit (processor unit)
 *
 * The PUnit is probably the most important BGPM unit for counting
 * hardware performance events. A PUnit is an abstraction of hardware events
 * from 5 - A2 Core event subunits, an Opcode Instruction Counting unit, L1p, and Wakeup Units.
 * These sub-units share the same counter resources and are configured and controlled
 * in concert.
 *
 * Most of the BGPM complexities and modes reflect the varying features of the PUnit
 * and subunits, instead of the other Bgpm units.
 *
 * See \ref punit_events for a list of defined events.
 *
 * <b>The Punit</b>
 * - Provides 24 programmable hardware counters shared
 *   by 8 different event sources and core hw threads.
 *   An individual sw thread may configure, 6, 12, or all 24 depending on the
 *   number of active Bgpm aware threads being used on the a core.
 *   See \ref bgpm_counter_rules for details on simultaneous punit events.
 *
 * - Is the only unit which is hardware thread aware,
 *   and thus can track counts on a software thread basis.
 *
 * - Counters may be accessed in a "fine-grained" mode (\ref BGPM_MODE_LLDISTRIB)
 *   which gives 14 bit counter capacity and low access latency (O(40) cycles?),
 *
 * - Other modes provide 64 bit resolution counters.
 *
 * The event sources within the PUnit are:
 *
 * - <b>A2 core</b> - The A2 core provides 5 event sources, which provides events specific
 *   to the internal operation of the A2, like stall counts or branch mis-predictions.
 *   Some events are core-wide, others hardware thread specific.  The A2 Core events may also
 *   be made kernel or user context sensitive.
 *   \n\n
 *   - \b QFPU - the Quad floating point Unit. The QFPU is sometimes aliased by terms QPU,
 *            FU, QPX or AXU depending the context in on where used in hardware documentation.
 *            BGPM will generally refer to it as QFPU, and the UPC hardware implementation and related
 *            values use AXU.
 *   - \b MMU - The memory management unit.
 *   - \b IU  - instruction unit.
 *   - \b XU - execution unit
 *   - \b LSU - load store unit.
 *   \n\n
 *
 *
 * - \b OpCode - the operation code source provides counting of completed A2 instructions,
 *               along with a general A2 cycle count event
 *   \n\n
 *   - Instruction counts are selected by the major groups: QFPU instructions, and XU instructions.
 *   \n\n
 *   - Within the major groups, are a number of predefined instruction groups,
 *     and each instruction has been assigned to one of these groups (See \ref bgpm_opcodes).
 *     Generally, instructions are grouped by category, or by context synchronization effect.
 *     The QFPU instruction groups may also be used to count number of floating point operations
 *     (and hardware scales counts appropriately for the instruction).
 *     The are also two events to count specific QFPU or XU instructions
 *     (though they only match the major and minor instruction opcodes,
 *     and can only be configured for an entire core).
 *   \n\n
 *   - The OpCode events cannot be made context sensitive.
 *
 *
 * - \b L1p - the L1 prefetch source provides events from the L1 prefetch
 *            hardware associated with a core.
 *            The L1p event collection can only be configured to
 *            one of 4 L1p event signal modes (switch, list, stream or none).
 *            The event mode is determined by the mode of the assigned L1p events.
 *            (This mode is core-wide and cannot be multiplexed)
 *            \n\n
 *            Thus, events may be selected from 4 lists:
 *            - Base L1p events, which are available regardless of the L1p event mode
 *              (a core's l1p mode remains 'none' unless other l1p event types are selected)
 *            - Switch L1p mode events
 *            - List L1p mode events
 *            - Stream L1p mode events
 *            \n\n
 *            L1p events map to only one UPC_P hardware counter per event, and there are more
 *            events than available counters.
 *            Thus, some L1p events cannot be simultaneously counted in Bgpm distributed modes.
 *            \n\n
 *            See \ref bgpm_l1p_rules for a table of L1p events which collide.
 *
 *
 *
 * - \b Wakeup - the wakeup unit provides two events for wakeup hits associated with
 *      a cpu core.  These share over-committed UPC_P counters with the L1p Base L1p events.
 *
 *
 * \subsection punit_set_attrib Punit Event Set Attributes
 *
 * The following attributes may be get or set on a Punit event set:
 *
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
 </tr>
 <tr> <td>Context</td>      <td>BGPM_CTX_BOTH</td>     <td>Bgpm_SetContext()</td> </tr>
 <tr> <td>Allow Mixed Context</td> <td>no</td>         <td>Bgpm_AllowMixedContext()</td> </tr>
 <tr> <td>Multiplex</td>    <td>off</td>               <td>Bgpm_SetMultiplex2()</td> </tr>
 <tr> <td>L1p Mode</td>     <td>n/a</td>               <td>Bgpm_GetL1pMode() only</td> </tr>
 <tr> <td>User1</td>        <td>0</td>                 <td>Bgpm_SetEventUser1()</td> </tr>
 <tr> <td>User2</td>        <td>0</td>                 <td>Bgpm_SetEventUser2()</td> </tr>
 </table>
 * \endhtmlonly
 *
 *
 *
 * \subsubsection bgpm_punit_context  Punit Kernel/User Context
 *
 * @copydoc bgpm::Bgpm_Context
 *
 *
 *
 *
 *
 * \subsection punit_event_attrib  Punit Event Attributes
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
  <th style='border-bottom:1.0pt solid black;'>Note</th>
 </tr>
 <tr> <td>Qfpu Group Mask</td>  <td>none</td>   <td>Bgpm_SetQfpuGrpMask()</td> <td>PEVT_INST_QFPU_MASK only</td> </tr>
 <tr> <td>XU Group Mask</td>    <td>none</td>   <td>Bgpm_SetXuGrpMask()</td> <td>PEVT_INST_XU_MASK only</td> </tr>
 <tr> <td>Count FP ops</td>     <td>none</td>   <td>Bgpm_SetQfpuFp()</td> <td>Qfpu Instr Events only</td> </tr>
 <tr> <td>Qfpu Op Match</td>    <td>none</td>   <td>Bgpm_SetQfpuMatch()</td> <td>PEVT_INST_QFPU_MATCH only</td> </tr>
 <tr> <td>XU Op Match</td>      <td>none</td>   <td>Bgpm_SetXuMatch()</td> <td>PEVT_INST_XU_MATCH only</td> </tr>
 <tr> <td>FP Square Root Scale</td>  <td>8</td>   <td>Bgpm_SetFpSqrScale()</td> <td>Qfpu Instr Events only</td> </tr>
 <tr> <td>FP Division Scale</td>  <td>8</td>   <td>Bgpm_SetFpDivScale()</td> <td>Qfpu Instr Events only</td> </tr>
 <tr> <td>Overflow</td>         <td>none</td>   <td>Bgpm_SetOverflow()</td> <td>Enable Overflow and Handler</td> </tr>
 <tr> <td>User1</td>            <td>0</td>      <td>Bgpm_SetEventSetUser1()</td> <td></td></tr>
 <tr> <td>User2</td>            <td>0</td>      <td>Bgpm_SetEventSetUser2()</td> <td></td></tr>
 <tr> <td>Edge/Cycle</td>       <td>evt default</td> <td>Bgpm_SetEventEdgeCycle()</td> <td>Override only if necessary (not recommended)</td></tr>
 <tr> <td>Polarity</td>         <td>evt default</td> <td>Bgpm_SetEventPolarity()</td> <td>Override only if necessary (not recommended)</td></tr>
 </table>
 * \endhtmlonly
 *
 *
 *
 * \subsubsection punit_fp_event_attrib  Floating-Point Operations
 *
 * By default, most floating point (QFPU) instruction events count completed instructions.
 * If you wish to count floating point operations instead, then the Bgpm_SetQfpuFp() can be
 * set, and the hardware appropriately weights and scales the counts according to each Qfpu instruction.
 * However, all Qfpu instructions count for at least one FP operation, even those Qfpu
 * instructions like loads or stores which you might not consider real FP operations.
 *
 * Thus, there are three methods to choose the significant FP operations to count:
 * -# use the \b PEVT_INST_AXU_MASK mask event, Bgpm_SetQfpuGrpMask() to select which groups to count,
 *    then Bgpm_SetQfpuFp() to select to count floating point operations.
 *    \n\n
 * -# choose \b PEVT_INST_QFPU_FPGRP1 event, which selects to count floating point operations
 *    for a subset of available Qfpu instructions. In particular, Qfpu moves and store operations are excluded, but
 *    Square root and divisions are also scaled.
 *    You can see which instructions are included, and their scaling factors at
 *    \ref bgpm_opcodes.
 *    \n\n
 * -# choose \b PEVT_INST_QFPU_FPGRP2 event, which also counts floating point operations, however
 *    more test and conversion instructions are excluded with this event.
 *
 * The Square-Root and Division instructions are the only ones with configurable FP scaling.
 * By default, they are set to count 8 FP operations per instruction, which will be typical.
 * However, you may select to scale with 1, 2, 4, or 8 FP operations via the Bgpm_SetFpSqrScale()
 * and Bgpm_SetFpDivScale() functions.
 *
 *
 * \subsubsection punit_opcode_match Operation Matching
 *
 * There are also two events, PEVT_INST_QFPU_MATCH and PEVT_INST_XU_MATCH, which when
 * coupled with the Bgpm_SetQfpuMatch() and Bgpm_SetXuMatch() functions,
 * can match and count particular instructions.
 * The match only occurs on the instruction
 * major and minor operation codes, so their are a few variations which cannot be distinguished.
 * You may also set a floating point scaling factor for the event (1, 2, 4, or 8).
 * See the table at \ref bgpm_opcodes which gives the match value and selection mask to
 * set to match each instruction.
 *
 *
 *
 *
 *
 *
 * \section L2unit_desc L2 Unit
 *
 * The L2 unit provides events from 16 level 2 cache devices in the BG/Q chip.
 * The L2 caches slice the memory address space into 16 ranges of equal size,
 * and each L2 slice has 6 event counters.
 * By default, the 6 event counters aggregate into a single set of counters.
 * But, the slices may count events individually as well
 * (See Bgpm_GetL2Combine()).
 * L2 combined mode is selected based on the events assigned to an L2 event set.
 *
 * The 6 L2 Unit counters are node-wide, and cannot be isolated to a single core or thread.
 * If event overflow is desired, the overflow signal is "slow" (see \ref bgpm_swfeatures_slow_overflow).
 * *
 * See \ref l2unit_events for a list of defined L2 events.
 *
 *
 *
 * \subsection bgpm_l2_attributes L2 Unit Event Set attributes
 *
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
 </tr>
 <tr> <td>L2 Combine Mode</td>     <td>enabled</td>    <td>Bgpm_GetL2Combine()</td> </tr>
 <tr> <td>User1</td>               <td>0</td>          <td>Bgpm_SetEventSetUser1()</td> </tr>
 <tr> <td>User2</td>               <td>0</td>          <td>Bgpm_SetEventSetUser2()</td> </tr>
 </table>
 * \endhtmlonly
 *
 *
 * \subsection bgpm_l2_event_attributes L2 Unit Event attributes
 *
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
 </tr>
 <tr> <td>Overflow</td>         <td>none</td>   <td>Bgpm_SetOverflow()</td>  </tr>
 <tr> <td>User1</td>       <td>0</td>              <td>Bgpm_SetEventUser1()</td> </tr>
 <tr> <td>User2</td>       <td>0</td>              <td>Bgpm_SetEventUser2()</td> </tr>
 </table>
 * \endhtmlonly
 *
 *
 *
 *
 * \section iounit_desc I/O Unit
 *
 * The I/O unit provides static events and counters from the
 * MU (Message Unit - not be confused with BGPM Units), PCIe, and DevBus hardware devices.
 * These events are node-wide and cannot be isolated to any particular core or thread.
 *
 * The I/O event counters also support "slow" overflow signals and
 * are available in in only the \ref BGPM_MODE_SWDISTRIB mode.
 *
 * See \ref iounit_events for a list of defined I/O events.
 *
 *
 * \subsection bgpm_io_attributes I/O Unit Event Set attributes
 *
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
 </tr>
 <tr> <td>User1</td>               <td>0</td>          <td>Bgpm_SetEventSetUser1()</td> </tr>
 <tr> <td>User2</td>               <td>0</td>          <td>Bgpm_SetEventSetUser2()</td> </tr>
 </table>
 * \endhtmlonly


 *
 * \subsection bgpm_io_event_attributes I/O Unit Event attributes
 *
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
 </tr>
 <tr> <td>Overflow</td>         <td>none</td>   <td>Bgpm_SetOverflow()</td>  </tr>
 <tr> <td>User1</td>       <td>0</td>              <td>Bgpm_SetEventUser1()</td> </tr>
 <tr> <td>User2</td>       <td>0</td>              <td>Bgpm_SetEventUser2()</td> </tr>
 </table>
 * \endhtmlonly
 *
 *
 *
 * \section nwunit_desc Network Unit
 *
 * The BG/Q network topology is a 5-D Torus.
 * The Network unit provides counters for 10 torus links and 1 I/O link.
 * There are 6 hardware counters per link (64 bits each). Counters are node
 * wide, and only one sw thread at a time may control counters
 * for any particular link.
 * A link is reserved or released via the Bgpm_Attach(), Bgpm_Detach() operations.
 *
 * Of the 6 counters per link
 * - 4 count only 32 byte packet chunks sent.
 * - 1 counts packets received each network cycle.
 * - 1 counts # of packets in receive fifo each network cycle.
 *
 * BGPM also tracks the elapsed network cycles between reset, start and stop
 * of a link.  Along with the receive counts, this can be used
 * to derive the receive queue packet contention.  For example:
 * - from Little's law
 *   - avg queue length (L)
 *   - avg waiting times (W)
 *   - arrival rate (lamda)
 *   - L = lamda * W
 * - from counter with packets received each network cycle
 *   - count / elapsed nw cycles = arrival rate (lamda)
 * - from counter with accumulation of number of packets in receiver fifo each network cycle
 *   - count / elapsed nw cycles = avg queue length (L)
 *
 * The defined network events encode a filter of
 * 7 Virtual Channels as defined by network hardware.
 * The channels are:
 *   - user point to point dynamic
 *   - user point to point escape
 *   - user point to point priority
 *   - system
 *   - collective user commworld
 *   - collective user subcomm
 *   - collective system
 *
 * There are also events which combine channel sections to count
 * all user point to point events, or collective events.
 *
 *
 * See \ref nwunit_events for a list of defined network events.
 *
 *
 *
 * \subsection bgpm_nw_attach Attaching to Network Links
 *
 * One software thread has exclusive access to a link's network counters.
 * That is, only one thread can control the counting for a link at a time.
 * The event sets are configured independently of the link, and then a software
 * thread must Bgpm_Attach() with a link mask value (buildable via enumerated
 * mask values). Bgpm_Detach() is used to free the links for use by other sw threads.
 * The Bgpm_Attach() operation does not fail if all requested links are not free.
 * Rather, it gives a positive warning return code.  Event Read operations return
 * 0 if a link is not owned.
 *
 * The standard event read operation returns a combined value from attached links
 * for the event set (including the network cycle count - you will need to take into account
 * the number of attached targets.  See \ref Bgpm_NumTargets().
 * However, the Bgpm_NW_ReadLinkEvent() operation can be used
 * to get the value for an individual attached link.
 *
 *
 * \subsection bgpm_nw_gen_events General Channel Events
 *
 * The PEVT_NW_SENT, PEVT_NW_RECV, PEVT_NW_RECV_FIFO events in concert with
 * with Bgpm_SetEventVirtChannels() function gives the ability to flexibly
 * filter the channels to count.
 *
 *
 * \subsection bgpm_nw_time_event Network Elapsed Cycles Event
 *
 * The network events are counted in terms of "network cycles", which are
 * different than cpu cycles.  Bgpm maintains a PEVT_NW_CYCLES event
 * with each network link which is automatically generated from cpu cycle
 * time and network speed.
 * The PEVT_NW_CYCLES tracks the totals network cycles a link has been counting.
 * Thus, it is reset with when the event set is reset (Network counters may not
 * be written) and effectively stopped and started with event set start and stop.
 * A read returns the current elapsed network cycles.
 * Note that Bgpm_ReadEvent() returns aggregate counts for all attached network links.
 * As a result, the cycle count is also an aggregate, so the cycle count returned is the
 * number of cycles times the number of attached links.
 *
 *
 *
 * \subsection bgpm_nw_set_attributes Network Unit Event Set attributes
 *
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
 </tr>
 <tr> <td>User1</td>          <td>0</td>       <td>Bgpm_SetEventSetUser1()</td> </tr>
 <tr> <td>User2</td>          <td>0</td>       <td>Bgpm_SetEventSetUser2()</td> </tr>
 </table>
 * \endhtmlonly
 *
 *
 *
 * \subsection bgpm_nw_attributes Network Unit Event attributes
 *
 * \htmlonly
 <table class="bgpm" cellpadding=0 cellspacing=0 style='border:1.0pt solid black;'>
 <tr>
  <th style='border-bottom:1.0pt solid black;'>Name</th>
  <th style='border-bottom:1.0pt solid black;'>Def Value</th>
  <th style='border-bottom:1.0pt solid black;'>Function</th>
 </tr>
 <tr> <td>Virt Channels</td>  <td>0</td>       <td>Bgpm_SetEventVirtChannels()</td> </tr>
 <tr> <td>User1</td>          <td>0</td>       <td>Bgpm_SetEventSetUser1()</td> </tr>
 <tr> <td>User2</td>          <td>0</td>       <td>Bgpm_SetEventSetUser2()</td> </tr>
 </table>
 * \endhtmlonly
 *
 *
 *
 *
 *
 * \section cnkunit_desc CNK (Compute Node Kernel) Unit
 *
 * On linux kernels, the "proc" filesystem is the usual access method for kernel
 * counts.  Since BG/Q does not have a proc filesystem, the CNK counters will
 * be made access-able as a BGPM virtual unit.
 *
 * CNK provides node wide, process wide, and hardware thread based counters for kernel events.
 * By creating an event set containing CNK counters, BGPM is able to sample the counter
 * values.  Each threaded event set provide a somewhat software thread based view of the counter values.
 * That is, each software thread event set has a private delta of the counter values
 * The event set maintains the delta across start/stop/reset and write operations, though
 * no changes to the actual CNK counters occurs.
 * Note that these event sets are not able to account for changes which might occur while the
 * software thread is swapped out of a hardware thread.  There is also a special Bgpm_CNK_SampleEvent()
 * operation which can sample the current CNK counter value since boot.
 *
 * See \ref cnkunit_events for a list of possible events.
 *
 *
 *
 *
 *
 * <h2>Up Next...</h2>
 * \ref bgpm_swfeatures
 *
 *
 */
