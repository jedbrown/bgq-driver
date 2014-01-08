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
 * \mainpage BG/Q Hardware Performance Monitoring API
 *
 *
 *
 * \if HTML
\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

\n<p><b><em>Overview</em></b>
- \ref bgpm_introduction
  - \ref bgpm_documentation
  - \ref bgpm_instr_example
  - \ref bgpm_build
- \ref bgpm_hwfeatures
- \ref bgpm_units
- \ref bgpm_swfeatures
- \ref bgpm_papi
- \ref bgpm_limitations
- \ref bgpm_glossary
- \ref bgpm_changes
 *
\n<p><b><em>Event Information</em></b></p>
- \ref bgpm_events
- \ref bgpm_opcodes
- \ref bgpm_event_tips
- \ref bgpm_papi_events
- \ref bgpm_counter_rules
 *
\n<p><b><em>Usage Reference</em></b></p>
- \ref bgpm_functions
- \ref bgpm_examples
- \ref bgpm_job
 *
\n<p><b><em>Internal Notes</em></b></p>
- \ref bgpm_architecture
- \ref upci_functions
- \ref bgpm_int_notes



\htmlonly
</div>
<div class="contents-right">
\endhtmlonly

 * \endif
 *
 * \section bgpm_introduction Introduction
 *
 * The Hardware Performance Monitoring API (BGPM) implements a
 * C programming interface for the BG/Q Universal Performance Counter
 * (UPC) hardware.
 * The UPC hardware in BG/Q programs and counts performance events
 * from multiple hardware units on a BG/Q node.
 * The API provide an interface to program which events to counts, and
 * also abstracts some hardware thread counters
 * to present a software thread context, with cooperation from the compute node kernel (CNK).
 *
 * BGPM provides multiple Hardware and Software Units which may be monitored,
 * like the Punit (CPU related events), L2 Unit (L2 cache related) or
 * Network Unit.
 * Each unit provides separate control of events and counters, but mostly use the same sequence of
 * API calls to set and control events.
 *
 * BGPM functions are fairly low level. While the BGPM API abstracts the hardware
 * collections into manageable events and units, it doesn't provide
 * constructs like formatted reports, or direct profiling.
 * However, it does provide functions on which these
 * can be built, like overflow detection and handler routines.
 *
 * In general, hardware performance event monitoring requires that user code be
 * instrumented with the native BGPM API, or a tool set like PAPI or HPC toolkit which
 * uses BGPM under the covers.
 *
 *
 *
 * \section bgpm_documentation About this documentation
 *
 * These pages provide the primary documentation for BGPM and monitoring features.
 * It is built using Doxygen, so will include much of the automated pages and cross referencing
 * normally provided by Doxygen.  However, this "Main Page" is a jumping off point to the big picture
 * documentation describing concepts and user references, and mostly links to other pages.
 * When viewed in a browser like "firefox",
 * these other pages have a table of contents on the left which refer to their contents (\ref intexpl_issue).
 * But, to jump to other pages, you should return to the main page.
 *
 * The pages are grouped into 3 sections:
 * - The \b Overview section provides conceptual directions, describing the
 *   hardware and software features along with limitations.
 *   \n\n
 * - The \b Event Information section provides references to the available units and events, and
 *   some tips and rules associated with their allocation.
 *   \n\n
 * - The \b Usage Reference section summarizes the API functions and settings and example.
 *   \n\n
 * - The \b Internal Notes section provides some operational notes for maintenance, but is not information
 *   a user will have any need to know.
 *
 *
 * \section bgpm_instr_example A Simple Example
 *
 * The following is a simple example of BGPM instrumentation.
 * You may find more complete examples at \ref bgpm_examples.
 *
 * @code
 #include "bgpm/include/bgpm.h"
 ...
    // Initialize Bgpm. The mode argument provides a gross indication
    // how counting resources should be allocated or shared between threads,
    // and must equal for all threads on the node.
    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    // Configured unit event sets.
    // The following generates a Punit event set for counting CPU related events.
    int hEvtSet = Bgpm_CreateEventSet();
    unsigned evtList[] =
        { PEVT_IU_IS1_STALL_CYC,
          PEVT_IU_IS2_STALL_CYC,
          PEVT_CYCLES,
          PEVT_INST_ALL,
        };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned) );

    // Apply or attach the event set to hardware.
    Bgpm_Apply(hEvtSet);

    // Start THIS event set for this sw thread while counting in mode BGPM_MODE_SWDISTRIB.
    // For hardware perspective modes, the Punit, L2 and I/O event sets are controlled together for all threads.
    Bgpm_Start(hEvtSet);

    Workload();

    Bgpm_Stop(hEvtSet);  // Stop THIS event set

    // Read elapsed counts
    // Each counter is identified by event set and event index into the event set.
    // There are also methods to query event characteristics or examine the list of events in the event set.
    printf("Counter Results:\n");
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (int i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        printf("    0x%016lx <= %s\n", cnt, Bgpm_GetEventLabel(hEvtSet, i));
    }

 @endcode
 *
 *
 * @note
 * Along with the native BGPM API,
 * the PAPI-C API will be externally available and include a wrapper
 * "substrate" to call the BG/Q native API.  See \ref bgpm_papi for more information.
 * PAPI instrumentation provides a software thread perspective of the
 * hardware events and counters.  Other HPC toolkits must use Bgpm under the covers to access
 * the hardware performance counters.
 * \n\n
 * Using BGPM API, PAPI, or HPC Tool-kit methods
 * are mutually exclusive and cannot simultaneously share a BGQ node.
 * Thus, for example, you should not instrument using both PAPI,
 * and BGPM calls on the same node.
 *
 *
 * \section bgpm_build Building and running with BGPM
 *
 * BGPM is a user state only interface designed for use with CNK.  The implementation library may be
 * found under <b>bgpm/lib/libbgpm.a</b>, but also has dependencies on shared memory and stdc++ libraries.
 * It also requires the spi/include/kernel/cnk path be explicitly included in the compile to resolve some CNK inline functions.
 * Thus, applications which statically link can do so as follows:
 *
 * @code
$(BGQ_CROSS_CC) -m64 -g -Wall  -I$(BGQ_INSTALL_DIR) -c <file.c>
$(BGQ_CROSS_CC) -o <file.elf> <objects> -static -L$(BGQ_INSTALL_DIR)/bgpm/lib -lbgpm -lrt -lstdc++
 * @endcode
 *
 * BGPM also needs a little less than 16K of shared memory to operate when there is more than 1 process per node.
 * This is a small requirement and usually fits within the CNK defaults and other uses of shared memory,
 * If needed, a size may be set via the BG_SHAREDMEMSIZE environment variable (in 1 MByte values).
 * For example, the following will allocate 64 MByte of shared memory, usually adequate for most multiprocess
 * MPI applications.
 *
 * @code
export BG_SHAREDMEMSIZE=64
 * @endcode
 *
 *
 *
 * <h2>Up Next...</h2> \ref bgpm_hwfeatures
 *
 * \section intexpl_issue Internet Explorer Issue
 *
 * Doxygen currently produces a DOCTYPE that uses transitional html. Internet Explorer apparently does
 * not support some convenient (needed?) CSS features except when "strict" xhtml is used, but other browsers
 * do.  Thus, for Internet Explorer the table of contents remains at the top of the page.
 *
</div>
 *
 *
 *
 * \addtogroup bgpm_user_api Bgpm User API
 * These files comprise the main user C application interfaces to the BGPM functions.
 *
 * \addtogroup bgpm_api_impl Bgpm API Implementation
 * These files comprise the functional implementation of the user API.
 * They are used to build libbgpm.a which a user links into their application.
 * libbgpm.a also contains a copy of objects from libSPI_upci_cnk.a .
 *
 * \addtogroup upci_spi UPCI SPI Implementation
 * UPCI stands for the Universal Performance Counting Interface.
 * UPCI works at a higher level than just the hardware bit twiddling UPC funtions,
 * providing the following features and limitations:
 * - The UPCI is intended for internal use or for prototyping.
 *   How to use the UPC SPI is not well documented.
 *   The UPCI may change without notice.
 * - The UPC Spi functions are also considered part of the UPCI SPI, though the files
 *   and methods are distinctly labeled (i.e. if UPCI and UPC are visually distinquishable :)
 * - Contains initialization functions to config all of the UPC hardware to a
 *   consistent state depending on the UPC mode.
 * - Interface functions for each of the hardware counting units (punit, l2, network, etc).
 *   There is no attempt to make the interfaces consistent.  Rather, they usually reflect the
 *   low level functionality of the unit. The creation of a consistent interface is relegated
 *   to higher level interfaces.
 * - The functions effectively assume they own the hardware.  That is, there is little
 *   attempt to coordinate hardware usage between cores or threads - so they can easily
 *   stomp on one another without higher level coordination.  There are functions to
 *   help with that coordination, but the higher level functions must use it.
 * - The UPCI does no memory management.  The required object are owned and managed by
 *   the caller.
 * - The UPCI is stateless. The state is maintained by opaque objects the caller must maintain.
 *   In general, the interfaces do not query the hardware to discover the state.
 *   Much of the state is distributed and queries would be slow.  Thus, the usage model is
 *   mostly a push of object settings to the hardware.
 * - The UPCI include files are found under bgq/spi/include/upci
 * - The UPCI function libraries are in bgq/spi/lib
 *   - libSPI_ucpi_fw.a    - link with firmware ddr tests and diagnostics
 *   - libSPI_upci_fwcnk.a - link with cnk for kernel syscalls and functions.
 *   - libSPI_upci_cnk.a   - link with cnk applications.
 *
 *
 * \addtogroup upc_spi UPC SPI Implementation
 * The UPC Spi generally refers mostly implementation which controls the hardware or refer
 * directly to UPC hardware entities (upc_atomic.h is an exception).
 * These files comprise the C functional implementation of the SPI and are built into libSPI_upci_fw.a
 * and libSPI_upci_fwcnk.a
 *
 *
 *
 *
  *
 */
