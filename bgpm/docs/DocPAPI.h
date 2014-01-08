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
 * \page bgpm_papi PAPI Integration
 *
 *
\if HTML
\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref bgpm_papi_overview
- \ref bgpm_papi_diffs
- \ref bgpm_papi_events

\htmlonly
</div>
<div class="contents-right">
\endhtmlonly
 * \endif
 *
 *
 * \section bgpm_papi_overview PAPI for BG/Q Overview
 *
 * The PAPI-C library is an industry-standard API that is designed to provide a consistent
 * interface and methodology for using the performance counter hardware.
 * It can be used to interface with the BGPM API to control and access the performance counters.
 *
 * For more information about the PAPI-C interface, including the documentation and toolkit, see
 * the PAPI website: http://icl.cs.utk.edu/papi
 *
 * The PAPI-C library must be downloaded, built, and installed separately from the Blue Gene/Q
 * drivers. For information about installing the library, see the PAPI release installation notes in
 * the installation directory. The path is papi/INSTALL.txt.
 *
 * The PAPI-C features that can be used for the Blue Gene/Q system include:
 *
 * - A standard instrumentation API that can be used by other tools
 *
 * - A collection of standard Preset events, including some events that are derived from a
 *   collection of events. The BGPM API native events can also be used through the PAPI-C
 *   interfaces.
 *
 * - Support for both a C and a Fortran instrumentation interface (Bgpm only implements a C interface).
 *
 * - Supports for separate components for each of the BGPM API unit types. The Punit
 *   counter is the default PAPI-C component. The L2, I/O, Network, and CNK units require
 *   separate component instances in the PAPI-C interface.
 *
 * But note the following current limitations:
 *
 * - PAPI-C only uses the BGPM_MODE_SWDISTRIB mode of Bgpm operation.
 *   Thus, it only provides a "software" thread view of Punit counters.
 *   Accessing punit counters for the 17th system core is not possible thru PAPI-C.
 *
 * - Currently, some native Bgpm event attributes are not configurable via PAPI.
 *   That is, the attribute methods for unique Bgpm features are not available.
 *   For example, the generic instruction match
 *   events (like PEVT_INST_XU_MATCH) require other Bgpm calls (Bgpm_SetXuMatch()) to configure
 *   opocode match values.  These are not currently supported via PAPI.
 *   There are also other events and attributes with similar limitations.
 *
 * - For the NWUnit, a NW Component event set will attach to all torus links. There is no
 *   method currently in PAPI to indicate a subset of links to attach.  Nor is there the
 *   ability to query the values for a particular link.
 *   The Bgpm_ReadEvent() and Bgpm_ReadEventList() return an aggregate sum
 *   from all 10 links, including the NW Cycle count.

 *
 *
 *
 * \section bgpm_papi_diffs Known Semantic Differences
 *
 *  - The Bgpm Network counter sets must be explicitly attached to links to count.
 *    Only one thread can own counting a link at a time.
 *    Currently, PAPI only attaches to all 10 torus links.
 *
 *  - The L2 Events require a separate event set in Bgpm.
 *    In PAPI-C, typically the L2 counts are part, but with BG/Q you will need a
 *    separate event set and component to access the L2 counters.  The L2 counts are
 *    also only node-wide and cannot be directly attributable to a single thread.
 *    Note also that hw event activity on the 17th (system agents) core cannot be examined.
 *
 *  - There are a few PAPI predefined events which can potentially be counted by
 *    different Bgpm events.  Though the Bgpm events are not exactly the same.
 *    total instructions is a good example:
 *    - PEVT_INST_ALL is a opcode count, and counts all instructions, ucoded counted once,
 *      and any counter can be used up to the 24 available counters per core.
 *      But, it's not context sensitive.
 *    - PEVT_XU_PPC_COMMIT is a A2 count, and is context sensitive, but uses one of the
 *      two counters available to the thread for XU counts (4 counters if 2 threads per node)
 *    - Example: PAPI_BR_NTK
 *
 *  - Some Bgpm events are context sensitive, others are not.  By default Bgpm will not allow you
 *    to mix events from the two groups if the context is set to something other than ALL.
 *    The context must be set prior to adding any event.
 *
 *  - Bgpm has an Bgpm_Apply() or Bgpm_Attach() operation to configure the
 *    hardware for an event set.
 *    It can be a slow operation and is separate from the start operation.
 *    PAPI does both during trhe PAPI_start operation, so the 1st start may be slower than
 *    subsequent invocations.
 *
 *
 * \section bgpm_papi_events PAPI Predefined Events
 *
 * The following is a list of PAPI predefined events and their equivalent BG/Q native
 * event, including predefined events which must be software derived.
 *
 *   \htmlinclude Papi-Events.html
 *
 *
 *
 */
