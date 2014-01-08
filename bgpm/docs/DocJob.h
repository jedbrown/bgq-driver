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
 * \page bgpm_job BGPM Job Environment
 *
 * \section bgpm_job_env BGPM Related "runjob" Environment Variables
 *
 * The BGPM library recognizes the following job environment variables to allow
 * job configuration of certain attributes.  Most of these environment variables
 * only apply when the BGPM function calls have been configured with a "...DEFAULT"
 * selection value.
 *
 *
 *
 * - BGPM_MODE \anchor BGPM_MODE -- Set BGPM default operational mode; see \ref Bgpm_Init().
 *
 * - BGPM_CONTEXT \anchor BGPM_CONTEXT -- Set BGPM default context; see \ref Bgpm_SetContext().
 *
 * - BGPM_NO_PRINT_ON_ERR \anchor BGPM_NO_PRINT_ON_ERR -- By default Bgpm will print error detail message on
 *   usage or configuration conflicts; Passing this environment variable will disable this default.
 *   You may also change within your program.
 *   See \ref Bgpm_PrintOnError() for more information.
 *
 * - BGPM_NO_EXIT_ON_ERR \anchor BGPM_NO_EXIT_ON_ERR -- By default Bgpm will exit the process on detected usage or configuration errors;
 *   Passing this environment varable will disable this default.
 *   You may also change within your program.  See \ref Bgpm_ExitOnError().
 *
 * - BGPM_EVT_FEEDBACK \anchor BGPM_EVT_FEEDBACK -- Select when Bgpm should print some feedback on
 *     event selection errors.  Bgpm will only allow you to add events which can be counted
 *     simultaneously.  When there are conflicts events or between thread configurations, Bgpm
 *     can print some feedback on punit signals and counters which have been previously reserved
 *     to provide a clue which events conflict and why.
 *     Feedback is also only provided when bgpm is set to print on error which is the
 *     default.
 *     Possible values
 *     - 0 - provide no feedback beyond a normal failure message.
 *     - 1 - provide feedback only on the 1st occurrence from any thread on node (default)
 *     - 2 - provide feedback only on 1st occurrence in process.
 *     - 3 - provide feedback all the time.
 *
 *  - BGPM_DUMP_ON_ERR \anchor BGPM_DUMP_ON_ERR -- if needed, you may set this environment to have
 *      Bgpm dump the process (create a core file instead of exiting), so you may examine the stack if desired.
 *
 *  - BGPM_EVTDESC_FILE \anchor BGPM_EVTDESC_FILE -- the long event descriptions are not built into the
 *      Bgpm libraries.  Rather, if you call Bgpm_GetLongDesc() it will read a detailed event description
 *      from a text file, which by default is found at /bgsys/drivers/ppcfloor/bgpm/lib/Bgpm_EventDetails.txt
 *      BGPM_EVTDESC_FILE will allow the path to be modified.
 *
 *  - BGPM_TRACESTART \anchor BGPM_TRACESTART -- libbgpm_debug.a contains trace routines to help with bgpm
 *      debug.  Trace into is printed to stderr by all threads.  Trace may be controlled
 *      within a program by calling Bgpm_EnableTracePrint() or Bgpm_DisabledTracePrint(), but
 *      usually you want to have everything traced from the beginning by setting environment variable
 *      BGPM_TRACESTART.
 *
 *  - BGPM_SIGNAL \anchor BGPM_SIGNAL -- The Bgpm enabled performance monitor signal handler is assigned
 *     by default to signal 34.  In case this may conflict with something in your application, you may
 *     assign a different signal using this environment variable.
 *
 *  - BGPM_SETS_PER_THREAD \anchor BGPM_SETS_PER_THREAD -- Bgpm uses a fixed array size for
 *      the list of event sets per process. The array size is not made vary large, and
 *      varies depending on the number of processes per node,
 *      but will be typically large enough to handle most uses.
 *      Just in an application needs to exceedingly overcommit event sets to threads,
 *      the BGPM_SETS_PER_THREAD will allow to set a fixed value as needed.
 *
 *
 */
