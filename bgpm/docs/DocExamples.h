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
 * \page bgpm_examples BGPM Examples
 *
 *
 * The following is a growing list of examples highlighting BGPM features.
 *
 * - \ref bgpm/punit_simple/main.c     "punit_simple"      -- Single threaded Punit event set example using event list with default exit on error.
 *   \n\n
 * - \ref bgpm/punit_rtnerrs/main.c    "punit_rtnerrs"     -- Example adding individual event and returning on error rather than exiting.
 *   \n\n
 * - \ref bgpm/punit_fastaccess/main.c "punit_fastaccess"  -- Example using Punit event set with fast access functions
 *   \n\n
 * - \ref bgpm/punit_lowlat/main.c     "punit_lowlat"      -- Punit event set while using Low latency access methods.
 *   \n\n
 * - \ref bgpm/punit_hwmode/main.c     "punit_hwmode"      -- Punit event set in BGPM_HWDISTRIB mode to count all threads in process.  Uses OMP to dispatch threads.
 *   \n\n
 * - \ref bgpm/nw_omp/main.c           "nw_omp"            -- Example of configuring punit overflows to sample Network Unit events
 *                                                            Includes some overflow handling of Punit events and multithreading using OMP.
 *   \n\n
 * - \ref mpi/pingpong-nw.cc           "mpi-pingpong-nw"   -- 2 node MPI example counting punit, L2, Message Unit (I/O) and Network Unit.
 *   \n\n
 * - \ref bgpm/punit_overflow/main.c   "punit_overflow"   -- Example of Handling Overflows for Punit Events.
 *   \n\n
 * - \ref bgpm/l2_overflow/main.c      "l2_overflow"      -- Example of Handling Overflows for L2 Events.
 *   \n\n
 *
 *
 *
 */
