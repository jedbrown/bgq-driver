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
 * \page bgpm_changes Change/Issues notes
 *
 * This page will communicate development design changes which may effect usage,
 * and known issues.
 *
 * - \b 19/07/2012 V1R1M2 BGPM fixes.
 * - Documentation change: Bgpm- Documnetation change for AddEvent&AddEventList
 *   The return code for Bgpm_AddEvent  and Bgpm_AddEventList has been corrected in the documentation.
 * - Fixed Bgpm-Missing Event Attribute preservation while cloning
 *   Observed an issue with Event attribute preservation when adding a new event to the existing event set. 
 *   Two missing event attribute tracking for PEVT_INST_XU_GRP_MASK and PEVT_INST_QFPU_GRP_MASK has been added. 
 * - Fixed - Bgpm_GetEventIdFromLabel fais to create PEVT_INST_ALL eventid 
 *   Bgpm_GetEventIdFromLabel() converts an event name into an event ID. I  works fine for all native events, 
 *   except for PEVT_INST_ALL. Although  PEVT_INST_ALL is a valid event.This is because of the boundary issue of the loop varient used to extract event ids from the table. 
 *
 *
 * - \b 12/04/16 - Efix V1R1M0-bgpm-io-mux-overflows
 * - Fix problem with I/O Overflow interrupts not working.
 * - Fix problem with A2 overflows intermittently firing too early when multiplexing active.  That is, instead of always
 *   firing at expected period value, they would occasionally fire at a varying smaller period (about 1% of the time
 *   in a test example).
 * - Fix problem with too many overflows trigger when more than 6 counters per thread were active.
 *   This occurred when only for broadcasted overflows.
 *   The specific overflow instance would be recorded and the user's overflow handler called multiple times.
 * - Added A2 Pipeline information to Bgpm Event Tips Documentation page.
 *
 * - \b 12/03/08
 *   - The I/O Unit overflows are currently not working. Problem still being investigated.
 *   - When using automatic multiplexing (Bgpm_SetMultiplex() with a period value), there
 *     seems to be a small window where an Bgpm_Stop() may not work.  Problem still being investigated.
 *
 * - \b 06/11/2012
 *   - Limitation: 
     There is a limitation for setting period value for a counter which is configured with overflow.This depends mainly on the time it takes to execute the overflow handler. In BGPM, with few experimentation, the minimum acceptable period value to set for PEVT_CYCLES event is value > 2000. This value is denpendent on the event we are configuring for overflow. and with the basic overflow handler without any user profiling added to teh handler. If the user set a value less than the minimum acceptable threshold value of the event, it will create a hang inside the handler and the test case will eventually fail.
*
*/

#if 0
/*!
 *
 * - \b 11/15/11 - Added tables to the Opcode documentation table so users can now see what instructions are counted
 *                 in the various instruction groups, and what the floating point weights are per instruction.
 *                 The tables and page still need some formatting cleanup, but the basics are there.
 *
 * - \b 11/15/11 - Fixed problem with LongDescription file corruption breaking the Bgpm_GetLongDesc() operation
 *
 * - \b 11/15/11 - Fixed some problems with event attributes being lost on multiplexed sets
 *                 when doing separate Bgpm_AddEvent operations.
 *
 * - \b 11/05/11 - After some hold ups - finally integrated support for simultaneous
 *                 multiplexing and overflows.  In addition, all punit events now
 *                 support overflow - no longer a limit to number of overflows
 *                 per punit event set.
 *
 * - \b 10/17/11 - Write and reset operations will only be allowed while event set is not running, except for
 *                 low level access methods on the punit.
 *
 * - \b 10/17/11 - Fixed significant problems with multiplexing - counter control could get confused if using
 *                 the automatic swaps, results would be inaccurate.
 *
 * - \b 10/17/11 - Fixed significant bug where counters where hardware counts where not cleared between applies.
 *                 Subsequent apply/reads could include counts from prior collections without a new Bgpm_Init().
 *                 So, added that each apply/attach will reset applicable counters.
 *
 * - \b 09/19/11 - Converted Bgpm_SetEventOverflow() to accept a period value (# of events between overflows) instead of
 *                 a UPC threshold value (0 - period).  The BGPM_PERIOD2THRES() macros has been deprecated and no longer
 *                 does any conversion.
 *                 This will make the arguments consistent with PAPI and more consistent with other typical uses of overflow
 *                 interfaces.  However, internally Bgpm will continue to use the term "threshold" to indicate the 64bit value
 *                 (0ULL - period).
 *
 * - \b 09/16/11 - Modified PEVT_AXU_UCODE_INSTR_COMMIT to PEVT_AXU_UCODE_OPS_COMMIT to reflect that it's not the instructions
 *                 that are being counted, but the committed ucode sub-operations that are counted by this event.
 *
 *
 * - \b 09/15/11 - Added -fpic to both SPI and Bgpm object compiles to allow these to effectively be linked into shared libraries.
 * - \b 09/15/11 - Combined libSPI_upci_cnk.a into libbgpm.a so only libbgpm.a needs to be linked.
 *
 * - \b 09/12/11 - Increase number of overflowable counters from 4 to 6
 * - \b 09/12/11 - Fix significant startup hang associated with conflicting interprocess interrupts.
 *
 * - \b 09/07/11 - Mostly some bug fixes and adjustments
 * - \b 09/07/11 - A number of A2 events marked with both Edge and Cycle counting have been changed to only
 *                 cycle counting, based on some testing.  Thus, some edge events have disappeared and may break your
 *                 code if you were using them - but the edge counts were invalid anyway.
 * - \b 09/07/11 - Did a cursory change to the doygen headers to fix problems using the latest
 *                 1.7.4 version.
 *
 * - \b 07/22/11 - Added punit multiplexing support. While I was attempting to keep
 *                 the cycle times that mux group was active in software, I finally
 *                 did have to reserve a counter for counting cycles in each group.
 *                 So, with multiplexing, each group will only have 5 counters available.
 *                 Though, the cycles can be queried by using the Bgpm_GetMuxEventElapsedCycles()
 *                 as well.
 *                 \n
 *                 Simultaneous Overflows and Multiplexing has problems and has not been
 *                 debugged yet.  I might decide not to support them simultaneously in the
 *                 same event set because of difficult implementation issues.
 *
 * - \b 07/14/11 - The contract on the Bgpm_Attach() operation has changed.
 *                 It no longer returns a new event set handle.
 *                 Rather, you work with a single process-wide event set handle and
 *                 pass a thread mask of hardware thread ids
 *                 to attach the event set to.
 *                 Each attached core/thread will use the
 *                 same set of events.
 *                 A Bgpm_ReadEvent() operation will return the aggregate value from
 *                 all attached threads instead of a single thread value,
 *                 or you may use Bgpm_ReadThreadEvent to get
 *                 the value from a single thread or a sum of a subset of attached
 *                 threads in a single process.
 *                 This makes the operation consistent with that of the NW Unit as well.
 *                 \n
 *                 However, for the punit, only threads within the same process may read events from that
 *                 event set, it's not possible read events from another process.
 *                 \n
 *                 Network Unit event sets will eventually be able to do multiple attachments
 *                 to the same links within or between processes.
 *                 Though, only the 1st attachment will be able to control the links.
 *                 But, the Network unit changes is not yet implemented.
 *                 \n
 *                 See example: \ref bgpm/punit_5_omp_hwmode/punit_5_omp_hwmode.c for an
 *                 example of using the BGPM_HWDISTRIB mode.
 *
 * - \b 07/10/11 - The event information methods have been mostly completed.  They
 *                 are all implemented, though the Bgpm_GetLongDesc() method returns
 *                 a NULL string yet.
 *
 * - \b 06/30/11 - The event handles have been combined into a process wide thread safe list.
 *                 Thus, returned handles may be accessed process wide. However, only the creating
 *                 thread (designated the controlling thread) may perform operations which modify
 *                 an event set (create/delete/add/start/stop etc).  Other threads in the process
 *                 may performance event read and informational "get" operations.
 *                 Thread safely does imply that only one thread may be updating the event list at a
 *                 time, and any non-controlling thread using another threads handle will block
 *                 controlling threads from updating event set updates during the read period.
 *                 Note however, that reading events from other threads doesn't prevent a controlling
 *                 thread from using it's event sets (start/stop,reset,apply etc).  It just means that
 *                 attempts to change events sets will block.
 *
 * - \b 06/15/11 - Speculative Bucket Support has been officially removed.
 *                 And, there is no need for Bgpm to shadow speculative events from the
 *                 SE/TM libaries.
 *
 * - \b 06/15/11 - The granularity attribute feature has been canceled.  Bgpm will
 *                 not combine any event counts to explicitly produce speculative or process
 *                 counts.
 *
 * - \b 06/15/11 - 10 more L2 event types have been implemented.  See the event tables.
 *
 * - \b 04/18/11 - Setting Overflow Handler from the Bgpm_SetOverflow() function
 *                 moved to separate Bgpm_SetOverflowHandler() to be used per event set.
 *                 This better reflects the actual functionality, which requires one
 *                 handler per event set.   However, there is not a problem with
 *                 setting it multiple times and the last occurrence wins.
 *
 * - \b 04/18/11 - Design change to allow counting by only one software thread per hardware
 *                 thread.  Thus, CNK allows a hardware thread to be over committed with
 *                 software threads, but only one sw thread can be using Bgpm.
 *                 Bgpm will still pause punit counting when the sw thread is swapped out.
 *
 * - \b 04/13/11 - The BGPM_MODE_HWDISTRIB mode does not support multiplexing, or any overflow support.
 *                 Some necessary hardware function to do
 *                 cross core hardware reads does not work, and the necessary software threads to
 *                 receive signals does not exist in this case.
 *
 * - \b 04/06/11 - Added support for Speculation Buckets.
 *
 * - \b 04/06/11 - Added support for CNK Counters (what's available yet anyway).
 *     Switched CNK counters from being "sampled" type (which wouldn't support start/stop, etc),
 *     to having a new "node" scope, which means they are node-wide, but each thread has
 *     individual control and the counts only change while counting is active on that thread.
 *     However, any context switches on a hw thread are not taken into account.
 *     Also, a new Bgpm_CNK_SampleEvent() operation exists to just read the current value
 *     of the CNK counter since boot time.
 *
 * - \b 03/08/11 - Ability for a child thread to inherit the configuration from the main process thread is cancelled.
 *     Each sw thread will be required to do a Bgpm_Init() and configure it's own events.
 *     Process speculation granularity will be required to be consistent by all child threads, and
 *     event configuration will be checked at Bgpm_Apply() time.
 *
 * - \b 02/28/11 - Modified UPC initialization and operations to avoid cross-core DCR operations
 *     under CNK. This will avoid the DCR Timeout problems.
 *
 * - \b 02/14/11
 *   - BGPM_MODE_HWDETAIL mode deferred.
 *   - Environmental Component events. deferred.
 *   - Memory Controller Unit events deferred.
 *   - Removed Online Mode.
 *   - Both Process Granularities deferred.
 *
 * - \b 12/17/10 - User1 & User2 event attributes are implemented and tested.
 *
 * - \b 12/17/10 - \ref BGPM_HWDISTRIB mode has been implemented including the cross core
 *   control of counting.  Testing is still cursory however.
 *
 * - \b 12/17/10 - Bgpm_Attach() now returns a new event set handle, which must
 *   be used for subsequent operations.  The original handle will still exist and
 *   can be reused, but won't be usable for controlling the counters, only as a
 *   event template to be used in other Bgpm_Attach() operations.
 *   - currently, the code does not adequately check of event set conflicts, so attempts
 *     to apply or attach multiples will give undetermined results.
 *   - eventually an appropriate error message should occur.
 *
 * - \b 11/16/10 - Removed L2 Slice Mask attribute.  Instead, user must select
 *   L2 slice events and use the Bgpm_L2_ReadEvent() or Bgpm_L2_WriteEvent
 *   to read or write from individual L2 slices.
 *   The Bgpm_ReadEvent() function reads aggregates the values from
 *   all slices, and the Bgpm_WriteEvent() operation is disabled unless using the combined
 *   events.
 *
 * - \b 11/16/10 - UPCI L2 function contract has changed - the UPC_L2_EnableUPC function no
 *   longer starts L2 counters under independent control. A separate UPC_L2_Start() operation
 *   is required.
 *
 */
#endif
