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
 * \page bgpm_functions BGPM API Functions
 *
 * \if HTML
\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref init_functions
- \ref event_set_functions
- \ref bgpm_event_query_functions
- \ref attrib_functions
  - \ref user_attribute_functions
  - \ref punit_attribute_functions
  - \ref punit_instr_attr_functions
  - \ref punit_debug_attr_functions
  - \ref l2unit_attribute_functions
  - \ref nwunit_attribute_functions
  - \ref overflow_handling_functions
  - \ref multiplex_handling_functions
- \ref counter_functions
- \ref fast_punit_functions
- \ref counter_ll_handle_functions
- \ref bgpm_error_functions
\htmlonly
</div>
<div class="contents-right">
\endhtmlonly

 * \endif
 *
 *
 *
 *
 * \section init_functions Initialization
 *
 * This is the main initialization function for BGPM which decides the mode of operation
 * and initializes the performance monitor hardware for operation.
 *
 * \li Bgpm_Init()            -- BGPM initialization for current software thread including hardware and software modes.
 * \li Bgpm_Disable()         -- Disable BGPM functions in current thread
 *
 *
 *
 * \section event_set_functions Event Set Operations
 *
 * Event sets are created empty and unassigned to a unit.
 * Once events or attributes are assigned, the event set is tied to a
 * the appropriate performance monitor unit.
 *
 * \li Bgpm_CreateEventSet()  -- Create an empty native event set.
 * \li Bgpm_DeleteEventSet()  -- Delete event set and deallocate reference events and related hardware resources.
 * \li Bgpm_AddEvent()        -- Append an event to an event set.
 * \li Bgpm_AddEventList()    -- Append a list of events to an event list.
 *
 *
 * \section bgpm_event_query_functions Event Information Query Functions
 *
 * \li Bgpm_NumEvents()       -- Return number of events in hEvtSet
 * \li Bgpm_NumTargets()      -- Return number of attached target threads or links
 * \li Bgpm_GetEventLabel()   -- Return event label for event in hEvtSet
 * \li Bgpm_GetEventId()      -- Return event id for event at index in hEvtSet
 * \li Bgpm_GetEventIndex()   -- Find event set index for event with given id.
 * \li Bgpm_GetUnitType()     -- Return unit type assigned to event set.
 * \li Bgpm_GetEventIdInfo()  -- Return default details about an event id.
 * \li Bgpm_GetEventIdLabel() -- Return event label given an event id
 * \li Bgpm_GetEventIdFromLabel() -- Return event id for the given label string
 * \li Bgpm_GetLongDesc()     -- Get the long description for a general event number.
 *                                (Requires File I/O operations to retrieve descriptions)
 *
 *
 *
 * \section attrib_functions Unit and Event Attribute Functions
 *
 * These functions set variable attributes to a single event, or possibly all
 * events for an event set.
 * The functions exist in set/get attribute pairs, with separate functions
 * representing each attribute. so there are quite a few functions.
 *
 * Some attributes effect event sets node wide, others individual events.
 * See the functions for distinctions.
 *
 *
 * \subsection user_attribute_functions User defined attributes
 *
 * Each event set and assigned event has two 64 bit user attributes
 * (User1 and User2) for use by the caller.
 * These allow you to track private information with each event set
 * or assigned event as you see fit.
 *
 * \li Bgpm_SetEventSetUser1() -- associate a uint64_t value with event set's user1 attribute.
 * \li Bgpm_GetEventSetUser1() -- return event set's user1 attribute.
 * \li Bgpm_SetEventSetUser2() -- associate a uint64_t value with event set's user2 attribute.
 * \li Bgpm_GetEventSetUser2() -- return event set's user2 attribute.
 *
 * \li Bgpm_SetEventUser1()    -- associate a uint64_t value with event's user1 attribute.
 * \li Bgpm_GetEventUser1()    -- return event's user1 attribute.
 * \li Bgpm_SetEventUser2()    -- associate a uint64_t value with event's user2 attribute.
 * \li Bgpm_GetEventUser2()    -- return event's user2 attribute.
 *
 *
 * \subsection punit_attribute_functions PUnit Event Set Attributes
 *
 * \li Bgpm_SetContext()      -- Set punit context to count kernel or user state events (limited to cpu events)
 * \li Bgpm_GetContext()      -- Return punit context
 * \li Bgpm_AllowMixedContext() -- Allow events with mixed context sensitivity while the Context setting is restricted.
 * \li Bgpm_GetL1pMode()      -- Return current core wide L1p Mode (set by assigned events)
 *
 *
 * \subsection punit_instr_attr_functions Punit Instruction Event Attributes
 * The following functions are used to set counting masks for the "generic" instruction counting events.
 *
 * \li Bgpm_SetQfpuGrpMask()   -- Set punit Quad Processing Unit event group mask (group of instructions to count together)
 * \li Bgpm_GetQfpuGrpMask()   -- Return active group mask
 * \li Bgpm_SetXuGrpMask()     -- Set punit Execution Unit event group mask (group of instructions to count together)
 * \li Bgpm_GetXuGrpMask()     -- Return active group mask
 * \li Bgpm_SetQfpuFp()        -- Set QFPU to count Floating Point operations instead of instructions.
 * \li Bgpm_GetQfpuFp()        -- Return QFPU FP count setting.
 *
 * \li Bgpm_SetQfpuMatch()     -- Set major/minor opcode match value, mask, and FP scaling values for the QFPU Instruction Match event (common to all threads on core).
 * \li Bgpm_GetQfpuMatch()     -- Return QFPU instruction match arguments.
 * \li Bgpm_SetXuMatch()       -- Set major/minor opcode match value, and mask values for the XU Instruction Match event (common to all threads on core).
 * \li Bgpm_GetXuMatch()       -- Return XU instruction match arguments.
 *
 * \li Bgpm_SetFpSqrScale()    -- Set QFPU floating point Square Root scaling factor (how many FP operations counted per sqr-root instruction) (common to all FP events and threads on core)
 * \li Bgpm_GetFpSqrScale()    -- Return QFPU floating point Square root scale.
 * \li Bgpm_SetFpDivScale()    -- Set QFPU floating point division scaling factor (common to all FP events and threads on core)
 * \li Bgpm_GetFpDivScale()    -- Return QFPU floating point division scale.
 *
 *
 * \subsection punit_debug_attr_functions  Punit Misc Overrides
 * The following functions should only be used if necessary.
 * The defined events have the event edge or cycle, and polarity inversion for an event.
 * Any change of these settings will change the meaning of the event.
 * Note: Only applies to A2 and L1p events (AXU, XU, IU, MMU, LSU and L1p types).
 *
 * \li Bgpm_SetEventEdge() -- Set punit event to count edges or cycles (warning - only use if necessary - may change meaning of event)
 * \li Bgpm_GetEventEdge() -- Return event edge or cycle setting
 * \li Bgpm_SetEventInvert() -- Set punit event polarity to be inverted (warning - only use if necessary - may change meaning of event)
 * \li Bgpm_GetEventInvert() -- Return event polarity inversion setting
 *
 *
 *
 * \subsection l2unit_attribute_functions L2 unit and Event attributes
 *
 * \li Bgpm_GetL2Combine()    -- Return L2 combine setting.
 *
 *
 * \subsection nwunit_attribute_functions Network Unit attributes
 *
 * Virtual Channels attributes for the PEVT_NW_SENT, PEVT_NW_RECV, and PEVT_NW_RECV_FIFO events.
 *
 * \li Bgpm_SetEventVirtChannels()   -- Set Virtual Channels for a general network event
 * \li Bgpm_GetEventVirtChannels()   -- Return Virtual Channels for a general network event
 *
 *
 * \subsection overflow_handling_functions  Overflow Handling
 *
 * \li Bgpm_SetOverflow()             -- set overflow on an event, along with threshold.
 * \li Bgpm_GetOverflow()             -- get overflow settings for an event: i.e. threshold
 * \li \ref Bgpm_OverflowHandler_t    -- overflow callback handler type.
 * \li Bgpm_SetOverflowHandler()      -- set handler function for this event set. Called when event overflow occurs.
 * \li Bgpm_GetOverflowEventIndices()  -- convert internal event handles passed to overflow handler into eventset index list.
 *
 * \subsection multiplex_handling_functions  Multiplex Handling
 *
 * If multiplexing is set on a Punit event set, this operation will switch to the next
 * "Mux" group.
 *
 * \li Bgpm_SetMultiplex()          -- Set multiplexing active on this event set.
 * \li Bgpm_SetMultiplex2()         -- Set multiplexing active on this event set. 
 * \li Bgpm_GetMultiplex()          -- Return current multiplex settings and statistics
 * \li Bgpm_SwitchMux()             -- switch to next mux group of events.
 * \li Bgpm_GetMuxGroups()          -- get number of mux groups.
 * \li Bgpm_GetMuxElapsedCycles()   -- get the elapsed run cycles for a mux group.
 * \li Bgpm_GetMuxEventElapsedCycles() -- get the elapsed run time for a particular event
 *
 *
 *
 * \section counter_functions Standard Counter Control Functions
 *
 * Control counting and access counter values by event set and index.
 * The standard control functions are common to all unit types.
 * Normally, all units event sets must be applied to the hardware by calling
 * Bgpm_Apply(), however Network Unit event sets must use Bgpm_Attach(),
 * and Punit types will only use Bgpm_Attach() in \ref BGPM_MODE_HWDISTRIB mode.
 *
 * \li Bgpm_Apply()           -- Apply event set config to current thread hardware
 * \li Bgpm_Attach()          -- Apply event set config to a particular hardware thread or network link
 *                               and return a new event set for control and access to counts.
 *                               (Only valid in hardware perspective modes for Punits.
 *                               Valid in all modes for NW unit.
 * \li Bgpm_Detach()          -- Detach from resource.
 * \li Bgpm_IsController()    -- Test if current thread is controlling thread (creator) of event set.
 * \li Bgpm_Start()           -- Start counting all configured events for this sw thread
 * \li Bgpm_ResetStart()      -- Reset counters before starting.
 * \li Bgpm_Stop()            -- Stop counting all configured events for this sw thread.
 * \li Bgpm_Reset()           -- Reset the counters
 * \li Bgpm_ReadEvent()       -- Read counter for event
 * \li Bgpm_ReadEventList()   -- Read multiple counter for event set.
 * \li Bgpm_WriteEvent()      -- Write counter for event
 *
 *
 * In BGPM_HWDISTRIB mode an event set is attached to multiple threads via Bgpm_Attach() operation.
 * The Bgpm_ReadEvent() and Bgpm_ReadEventList() operation returns the aggregate values from
 * the attached threads. To read an individual thread value you need to use:
 *
 * \li Bgpm_ReadThreadEvent()  -- Return aggregate value of event from subset of attached threads.
 *
 *
 * Each Network event set counts events for a unique set of Network links.
 * Which links to count is determined via the Bgpm_Attach() operation mask.
 * The Bgpm_ReadEvent() method returns the sum of the attached link values.
 * But, the Bgpm_NW_ReadLinkEvent() can be used to get the value from a particular link
 * attached with the event set.
 * There are 10 torus links, and 1 I/O link.
 *
 * \li Bgpm_NW_ReadLinkEvent()  -- Return aggregate value of event from subset of attached links
 *
 *
 * If the selected L2 events are for individual slice counts, then these method
 * Can read or write particular L2 slice.
 * \li Bgpm_L2_ReadEvent()  -- Read event from a particular L2 Slice
 * \li Bgpm_L2_WriteEvent()  -- Write event to a particular L2 Slice
 *
 *
 * To sample a current CNK counter value since boot regardless of current run state of the cnk eventset:
 * \li Bgpm_CNK_SampleEvent() -- Read current CNK event counter value (since boot)
 *
 *
 *
 * \section fast_punit_functions Fast Punit Control Functions
 *
 * The following functions provide direct access to punit event counters via
 * handles which must be extracted from a configured unit event set.
 * There are some restrictions:
 * \li The handles are only valid as long as there are no changes to the event set
 *     after performing a Bgpm_<unit>_GetHandles operation.
 * \li The results are invalid if Multiplexing is active.
 * \li These methods must be used with care - there is no validity or consistency
 *     checks.  Improper use may result in program or machine checks or just invalid
 *     results.
 * \li The event set must have been applied (Bgpm_Apply()) prior to start.
 * \li Do not mix the start and stop functions of the Standard, Fast or Low Latency
 *     functions - the results are unpredictable.
 * \li These function only work properly in modes mentioned below, otherwise they may not work as expected.
 *
 * (Works only in BGPM_MODE_SWDISTRIB)
 *
 * \li \ref Bgpm_Punit_Handles_t  -- structure which receives the control and event handles
 * \li Bgpm_Punit_GetHandles()   -- populate structure with control and event handles
 * \li Bgpm_Punit_Start()        -- start counters given the passed handle (\ref BGPM_MODE_SWDISTRIB only)
 * \li Bgpm_Punit_Stop()         -- stop counters given the passed handle (\ref BGPM_MODE_SWDISTRIB only)
 * \li Bgpm_Punit_Read()         -- read event
 *
 *
 * \section counter_ll_handle_functions Low Latency Punit Counter Access Functions
 *
 * Low Latency access function operate only when Bgpm_Init mode is \ref BGPM_MODE_LLDISTRIB
 * to provide a "fine-grained" operation of the Punit counters.
 * In this mode, the "punit" counters MUST be accessed using these low latency functions.
 * The fine-grained counting only provides 14 bits of counter data, so must also be
 * used with care.
 *
 * The functions use returned handles to directly access counters though a fast mmio operation.
 * See \ref bgpm_swfeatures_lowlat_access for more information.
 *
 * \li \ref Bgpm_Punit_LLHandles_t -- structure to receive handles for Low Latency functions
 * \li Bgpm_Punit_GetLLHandles()    --  Get a low latency punit handles
 * \li Bgpm_Punit_LLStart()  -- Start punit counters
 * \li Bgpm_Punit_LLResetStart() -- Reset then start punit counters
 * \li Bgpm_Punit_LLStop()  -- stop punit counters
 * \li Bgpm_Punit_LLRead()  -- read ll counter
 * \li Bgpm_Punit_LLWrite() -- write ll counter
 * \li BGPM_LLCOUNT_OVERFLOW()   -- #define macro to test if LL counter has overflowed 14 bits
 *
 * \note Overflow and multiplex counting is not supported in \ref BGPM_MODE_LLDISTRIB mode.
 *
 *
 *
 *
 * \section bgpm_error_functions Error Handling Functions
 *
 * \li Bgpm_LastErr()       -- Return the last Bgpm function error during this software thread.
 * \li Bgpm_LastErrStrg()   -- Return the last Bgpm function error string during this software thread.
 *                           May include object name replacement values in the string.
 * \li Bgpm_ErrStrg()       -- Return a general Bgpm error string for an error number.
 *                           Object names not included, but replaced with general "type" names.
 * \li Bgpm_PrintOnError()  -- Thread specific indication whether Bgpm should automatically print error messages to stderr
 *                             when usage or configuration conflict errors arise in the use of Bgpm
 *                             (Default is yes)
 * \li Bgpm_ExitOnError()   -- Thread specific indication whether Bgpm should exit the process with the error code when
 *                             usage or config errors arise (Default is yes).
 *
 *
 */
