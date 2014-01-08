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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
 * \mainpage notitle
 * \author HLCS team
 *
 * \section toc Table of Contents
 *
 * - \ref introduction
 * - \ref terms
 * - \ref partitioning
 * - \ref differences
 *
 * \section introduction Introduction
 *
 * The High Level Control System (HLCS) consists of components running on the Service Node (SN)
 * the Subnet Service Nodes (SSN), and the Login or Front-End Nodes (LN or FEN). These components
 * provide services to partition and boot the hardware so jobs can be submitted to
 * them. They work in close coordination with the Low Level Control System (LLCS) to provide
 * these services.
 *
 * \sa \htmlonly <a href="../../../bgws/docs/bgws/html">Blue Gene Web Services</a>\endhtmlonly
 * \sa \htmlonly <a href="../realtime/html">Real-time server</a>\endhtmlonly
 * \sa \htmlonly <a href="../runjob">runjob</a>\endhtmlonly
 * \sa \htmlonly <a href="../master">bgmaster</a>\endhtmlonly
 * \sa Scheduler API \htmlonly <a href="../bgsched/external">external</a>\endhtmlonly and \htmlonly <a href="../bgsched/internal">internal</a>\endhtmlonly
 */

/*!
 * \page terms Terminology and Naming Conventions
 *
 * \section overview Overview
 *
 * Commonly used terms and naming conventions within the HLCS software stack are described here.
 *
 * \section deprecated Deprecated Terms
 *
 * To clear up confusion about some commonly used terms in previous BG/L and BG/P software
 * releases, the BG/Q HLCS stack has deprecated a few terms. They are called out in this list below.
 * It's important to note that this effort is focused on external interfaces, and not internal
 * cases where the old term is not exposed.
 *
 * \section terminology
 *
 * \subsection bgws BGWS
 * The Blue Gene Web Services, provides services for the Navigator.
 *
 * \subsection block
 * collection of nodes wired together such that they can communicate over a torus to their neighbors.
 * Replaces the term partition. Partition as a verb is still accurate
 *
 * \subsection cn compute node
 * a single node on a node board.
 *
 * \subsection job
 * logical representation of a single binary running on an arbitrary number of compute nodes
 *
 * \subsection io I/O node
 * node used for I/O capabilities to file systems and external networks
 *
 * \subsection midplane
 * 16 node boards populate a midplane.
 *
 * \subsection MMCS
 * midplane management control system, the heart of the HLCS software stack. Provides services
 * for block creation, allocation, and booting. Also provides RAS handling facilities and environmental
 * polling.
 *
 * \subsection Navigator
 * web based Graphical User Interface (GUI) to the HLCS.
 *
 * \subsection nb node board
 * 32 compute nodes populate a node board. Replaces the term node card.
 *
 * \subsection compute_rack Compute Rack
 * Two midplanes populate a compute rack.
 *
 * \subsection ras RAS
 * Reliability, Availability, and Serviceability
 *
 * \subsection Real-time
 * Real-time notification API for job, block, and hardware state and status transitions.
 *
 * \subsection runjob
 * job submission interface, replaces mpirun, submit, and mpiexec
 *
 * \subsection scheduler
 * A higher level component that creates and allocates blocks using APIs provided by the HLCS to
 * submit jobs in its queue.
 *
 * \subsection scheduler_api Scheduler API's
 * API for job schedulers to use, replaces Bridge and Allocator API's.
 *
 */

/*!
 * \page partitioning Partitioning and Block Sizes
 *
 * block sizes go here.
 *
 */

/*!
 * \page differences Differences from Previous Releases
 *
 * everything is different, nothing works.
 *
 */
