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
 * \page upci_functions UPCI SPI Functions
 *
 * \if HTML
 *
\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref spi_overview
- \ref spi_user_functions
  - \ref spi_mode
  - \ref spi_punit_functions
    - \ref spi_punit_writereset
    - \ref spi_punit_thresreset
    - \ref spi_punit_startstop
    - \ref spi_punit_syncstartstop
    - \ref spi_config_mgt
    - \ref spi_lowlat
  - \ref spi_L2_functions
  - \ref spi_io_functions
  - \ref spi_mc_functions
  - \ref spi_nw_functions
  - \ref spi_env_functions
  - \ref spi_cnk_function
  - \ref spi_overflow_functions
  - \ref spi_upci_function
- \ref spi_fw_functions
  - \ref spi_fw_init_functions
- \ref spi_syscall_functions
\htmlonly
</div>
<div class="contents-right">
\endhtmlonly

 * \endif
 *
 * ***WARNING*** much of this information is old and stale. Some of the functions have changed
 * or moved.   The SPI is not intended to be used by users because the SPI functions provide
 * no protection against problems with multiple threads or a consistent interface between units.
 * Bgpm provides that protection.  But, the UPCI interfaces are needed for FWEXT diagnostic
 * tests.
 *
 * \section spi_overview Overview
 * \copydoc upci_spi
 *
 * The function list which follows is not exhaustive of UPCI and UPC related functions,
 * but gives a list of functions likely to used by higher level functions.
 * Some functions require privileged operations, and are only intended to be called from
 * the kernel or fwext tests or diagnostics.
 *
 *
 *
 * \section spi_user_functions UPCI User State Functions
 *
 *
 * \subsection spi_mode UPCI Mode object
 *
 * The mode object and functions maintain the overall operational mode for the UPC.
 * When used with the merge functions it can insure all threads operating in a consistent
 * fashion and there are no conflicts between processes or applications.
 *
 * In user state the mode object must be the 1st created. After a thread creates the
 * mode object, the software thread cannot move between hardware threads without
 * (possibly silently) breaking the counting functions.
 *
 * header: bgq/spi/include/upci/mode.h
 *
 * \li \ref Upci_Mode_t       - opaque mode object type.
 * \li Upci_Mode_Init()  - Set global mode across node and application type.
 * \li Upci_Mode_Merge() - merge mode settings between shared mode and current mode.
 *
 *
 * \subsection spi_punit_functions (Punit) Processing Unit Functions
 *
 * The Punit is the most complicated unit.
 * There is no selection mode which indicates which events or counters are available.
 * Rather, events must be individually selected (which causes assignment to supported counters).
 * Which events may be gathered simultaneously depends on the thread,
 * other events, the selection order, and potentially
 * even what events are selected for other units and threads.
 * Punit SPI functions only attempt to resolve these conflicts
 * for a single Punit object.
 * However, the Merge and Snapshot methods exist to check for conflicts between objects
 * (e.g. for checking conflicts between multiple Punit objects which might multiplex, or
 * or between a shared configurations from multiple threads).
 *
 *  header: bgq/spi/include/upci/punit.h
 *
 *  \li \ref Upci_Punit_t        - opaque punit object.
 *  \li Upci_Punit_Init()   - Initialize a punit prior to event and attribute configuations.
 *
 *  \li \ref Upci_Punit_EventH_t    - handle to a punit event record - owned by a Punit object.
 *  \li Upci_Punit_AddEvent()  - Add and reserve an event to a punit configuration.
 *  \li Upci_Punit_GetEventH() - retrieve a handle for event for a given index.
 *
 *  \li Upci_Punit_Apply()     - Apply the base punit configuration to hardware
 *  \li Upci_Punit_EnablePMInts() - Enable Perf Mon interrupts to create overflow signal
 *  \li Upci_Punit_SwitchMux() - Switch to next Mux set in punit config.
 *
 *  \li Upci_Punit_SetAttrib() - set punit shared attribute (thread combine, context, l1pmode, opcode match masks, shared overflow)
 *  \li Upci_Punit_GetAttrib() - return punit shared attribute (thread combine, context, l1pmode, opcode match masks, shared overflow)
 *
 *  \li Upci_Punit_Event_SetThreshold() - Set event threshold and enable overflow on event counter
 *  \li Upci_Punit_Event_GetThreshold() - Get event threshold and enable overflow on event counter
 *  \li Upci_Punit_Event_SetOpcodeMask() - Set opcode mask for generic opcode events.
 *  \li Upci_Punit_Event_GetOpcodeMask() - Return opcode mask used by opcode events.
 *
 *  \li Upci_Punit_MatchOvf()  - Return which Punit event is active in Punit Overflow mask (and new mask without that event)
 *
 *  \li Upci_Punit_Event_Read() - read the accumulated value for the event (Does not account for 800 cycle delay)
 *
 *  \subsubsection spi_punit_writereset Punit Normal Write/Reset operations
 *  In UPC distributed mode the Punit counter write operations are expensive.
 *  The target counter must be stopped and an 800 cycle time required to flush
 *  values in flight. Thus, use the "Start/Finish" methods below to overlap the writes or resets
 *  with other operations as appropriate.
 *  Note that for multiple events, the write starts may be grouped prior to the write finishes.
 *  (The "low latency" routines discussed later avoid this issue, but also
 *  have other restrictions)
 *
 *  In detailed mode, the writes are much less expensive, and the Write/Reset operations are faster.
 *
 *  When in synchronized counting mode the
 *  explicit counter start/stop controls performed by these routines is ignored by the hardware.
 *  Thus, counters must be explictely stopped via UPC_C_Stop_Sync_Counting() 1st to get a reliable write.
 *
 *  \li Upci_Punit_Event_Write() - write value for event.
 *  \li Upci_Punit_Event_Reset() - reset all punit configured counters.
 *  \li Upci_Punit_Event_Write_Start() - Begin a punit write operation and return before complete.
 *  \li Upci_Punit_Event_Write_Finish() - Complete an already started punit counter write.
 *  \li Upci_Punit_Event_Reset_Start() - Begin a reset of threaded punit counters.
 *  \li Upci_Punit_Event_Reset_Finish() - Complete an already reset of punit counters.
 *
 *  \subsubsection spi_punit_thresreset Punit Threshold Reset Operations
 *  There are 3 types of overflows which have slightly different requirements and handling.
 *  Without detailing the handling, the threshold reset routines are below:
 *
 *  \li Upci_Punit_Event_Reset_Start_ThdThres() - start reset of threaded ovf event to configured threshold.
 *  \li Upci_Punit_Event_Reset_Finish_ThdThres() - finish reset of threaded ovf event.
 *  \li Upci_Punit_Event_Reset_Start_ShrThres() - start reset of shared ovf event to configured threshold.
 *  \li Upci_Punit_Event_Reset_Finish_ShrThres() - finish reset of shared ovf event.
 *  \li Upci_Punit_Event_Reset_CThres() -  reset of UPC_C shared ovf event to configured threshold.
 *
 *
 *  \subsubsection spi_punit_startstop Punit Normal Start/Stop operations
 *
 *  \li Upci_Punit_Start()     - start configured counters. Optionally reset. Optionally filter by threaded, shared or overflow.
 *  \li Upci_Punit_Stop()      - stop configured counters. Optionally filter by threaded, shared or overflow.
 *
 *
 * \subsubsection spi_punit_syncstartstop Synchronous Punit/L2/IO Start/Stop
 *
 * When Upci_Mode_t is configured for synchronous start and stop,
 * the hardware synchronizes the start and stop of Punit/L2 and IO counters.
 * However, note that after calling the function, there is an 800 cycle delay before all counters
 * start counting, and a 1600 cycle delay after stop before the readable counts are stable.
 *
 * \li UPC_C_Start_Sync_Counting()
 * \li UPC_C_Start_Sync_Counting_Delay()
 * \li UPC_C_Stop_Sync_Counting()
 * \li UPC_C_Stop_Sync_Counting_Delay()
 *
 *
 *
 *
 *
 *
 *  \subsubsection spi_lowlat Low Latency Operations
 *
 *  Most operations have some checks or indirection required to support multiple modes or
 *  context variations.  The "low latency" operations provide a framework where most of the
 *  overhead on critical operations can by bypassed, but they can only operate in limited
 *  situations.  They are also "unsafe" - that is, if used incorrectly, they will silently
 *  fail.  You may or may not get the expected counts - so you need to know what you are doing.
 *
 *  The idea is to use the heavy-weight Punit methods to create, add events and apply event
 *  configurations, then use the low latency routines to get handles and masks
 *  to use with other inline methods to directly start/stop/read and write counters.
 *
 *  Restrictions: (if not followed failures are silent and may be undetectable)
 *  - \ref UPC_CM_SYNC counter mode is not supported. Control masks have not effect in this mode.
 *  - Multiplexing cannot be used.
 *  - If in \ref UPC_CM_DISCON counter mode, then upc_p counters are only 14 bits, and
 *    context switches will likely corrupt counts.
 *  - They only work when counting events for the current thread (hw thread doing the call).
 *  - Be careful with start and stop of shared (core wide) events.
 *
 *  \li Upci_Punit_GetLLCtlMask() - get punit counter control mask for start/stop of counters.
 *  \li Upci_Punit_LLResetStart() - reset and start counters (only works if ctr mode is \ref UPC_CM_DISCON)
 *  \li Upci_Punit_LLStart()      - start punit counters from mask.
 *  \li Upci_Punit_LLStop()       - stop punit counter from mask.
 *
 *  \li Upci_Punit_LLCCtr_Read()  - Read from UPC_C counter (context switches may corrupt count).
 *                                  LL Write to counter is not supported.
 *
 *  \li Upci_Punit_LLPCtr_Read()  - read upc_p ctr.
 *  \li Upci_Punit_LLPCtr_Write() - write upc_p ctr.
 *  \li Upci_Punit_LLPCtr_Ovf()   - check if counter value returned by Upci_Punit_LLPCtr_Read has overflowed.
 *
 *
 *
 *
 * \subsection spi_L2_functions L2 Unit Functions
 *
 *  header: bgq/spi/include/upci/upc_l2.h
 *
 *  \li UPC_L2_EnableUPC()  - Only use for some fw tests.  Otherwise called indirectly via Upci_Mode_Init().
 *  \li UPC_L2_SetCombine() - Set the L2 counter combining attribute (hardware combine event from all L2 slices)
 *  \li UPC_L2_ReadCtr()    - read
 *
 *  \li Upci_L2_MatchOvf    - return event which matches overflow mask (and new mask without that event)
 *
 * \subsection spi_io_functions I/O Unit Functions
 *
 *  The counters and events from the I/O units are static and non-configurable.
 *  Thus, the counters to read are mapped via an enum.  They are enabled with the UPC_C,
 *  and counters independent start or are sync'ed with the other UPC_C attached units.
 *
 *  \li UPC_IO_Read()    -- Read I/O counter
 *  \li UPC_IO_Write()   -- Write I/O counter
 *  \li UPC_IO_Threshold() -- Set I/O counter to interrupt on threshold.
 *
 *  \li Upci_IO_MatchOvf   - return event which matches overflow mask (and new mask without that event)
 *
 *
 *
 * \subsection spi_nw_functions Network Unit Functions
 *
 * There are 6 counters available for each of 11 Network links. Counters 0-3 give send counts.
 * Counters 4 & 5 combine to give receive metrics, and are controlled together.
 * The values to be counted are filtered by Virtual Channel masks.  The UPC routines allow for control by individual link and counter,
 * or selecting multiple links simultaneously via a link mask (e.g all torus links or just the
 * I/O link).
 *
 *  \li UPC_NW_EnableUPC() - enable the network unit UPC operation
 *
 *  \li UPC_NW_Vchannels_t      - Virtual Channels masks to use in counter configuration.
 *  \li UPC_NW_GetCtrVChannel() - Get virtual channel assigned to a particular link counter
 *  \li UPC_NW_SetCtrVChannel() - Set virtual channel for a particular link counter
 *  \li UPC_NW_SetLinkCtrVChannel() - Set counter virtual channel for multiple links (links specified via mask)
 *
 *  \li UPC_NW_CtrCtlMask() - general control mask from link and counter masks for use in clear,
 *                            start and stop routines.
 *  \li UPC_NW_Clear() - clear a set of nw counters.
 *  \li UPC_NW_Stop()  - stop a set of nw counters.
 *  \li UPC_NW_Start() - start a set of nw counters.
 *
 *  \li UPC_NW_ReadCtr() - read a particular counter
 *  \li UPC_NW_LinkCtrTotal() - return counter sum from all multiple links.
 *
 *  \note Writing to network counters is not supported.
 *
 *
 *
 *
 *
 * \section spi_fw_functions UPCI Firmware Functions
 *
 *
 *
 * \subsection spi_fw_init_functions Initialization
 *
 * header: bgq/spi/include/upci/upc_c.h
 *
 * \li UPC_C_Full_Clear()     -- (fw only) clear out the UPC_C hardware once required to prevent later parity errors.
 * \li UPC_C_Clear()          -- (fw only) clear UPC_C state without altering error interrupt settings.
 * \li UPC_C_Init_Mode()      -- (fw only) set UPC mode incl UPC_P/IO/L2 attached units to make consistent.
 * \li UPC_C_Disable()        -- (fw only) put UPC_C and attached unit to sleep to save power.
 * \li UPC_C_Init_Error_Interrupts()  -- (fw only) set interrupt bits. UPC errors are only
 *                                       considered critical errors and should not cause
 *                                       machine checks even if hardware is bad.
 *
 * \section spi_syscall_functions UPCI/CNK Syscalls
 *
 * \li Upci_Punit_Apply()     -- Apply a Punit configuration to the hardware.  This only configures
 *                               the UPC hardware and does not configure the kernel to account for
 *                               context switches or shared overflows.
 * \li Upci_Punit_Attach()    -- Apply and Attach a punit and shared overflow config to kernel.
 *                               CNK will snapshot counts and switch UPC configs as needed for the
 *                               attached software thread. Do not mix Apply and Attach in the
 *                               same process.
 *
 *
 *
 *
 */
