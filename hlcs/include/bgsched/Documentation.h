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
 * \author IBM
 * \version 1.2.0
 *
 * (C) Copyright IBM Corp.  2011, 2012
 *
 * The Blue Gene/Q (BG/Q) solution provides interfaces for external job
 * schedulers (resource managers) to access system hardware topology and to
 * manage blocks and jobs.  These overall set of interfaces are known as
 * the Scheduler APIs.
 *
 * To get an idea of the architecture of the allocator, look at the
 * bgsched::allocator::Allocator class.
 *
 * \image html bgsched.png
 *
 * A scheduler will, at a minimum, need to create an instance of the
 * bgsched::allocator::Allocator class and call its methods to allocate and deallocate
 * resources. Schedulers wishing to use a simple first fit may choose to use
 * the IBM-supplied IBMFirstFit::FirstFitPlugin provided in the firstfit library.
 *
 * A scheduler wishing to provide its own plug-in will implement the
 * bgsched::allocator::Plugin interface
 *
 */

/*!
 * \namespace bgsched
 * \brief General types and methods needed by allocator, core, realtime, and runjob components.
 */

/*!
 * \namespace bgsched::allocator
 * \brief A set of APIs used to allocate and deallocate blocks.
 *
 * The Allocator APIs can be used by external schedulers to find compute resources and to allocate those
 * resources to individual jobs. The allocator environment consists of a live model which reflects the
 * actual hardware allocations. The APIs are provided as C++ interfaces.
 *
 */

/*!
 * \namespace bgsched::core
 * \brief A %core set of APIs needed to query basic machine information.
 */

/*!
 * \namespace bgsched::realtime
 * \brief The real-time APIs allow an application to receive real-time updates of changes on the Blue Gene control system.
 *
 * To use the real-time APIs,
 * the application first creates an instance of a Client.
 * The application will typically create their own ClientEventListener,
 * overriding ClientEventListener methods with its own handlers.
 * Then the application registers its listener with its Client
 * by calling Client::addListener().
 *
 * Next, the application connects to the server by calling Client::connect().
 *
 * The application then calls Client::requestUpdates().
 * The Client will then be notified of events happening on the Blue Gene
 * by callback on its ClientEventListener.
 *
 */

/*!
 * \namespace bgsched::runjob
 * \brief The %runjob plug-in API.
 */

/*!
 * \namespace IBMFirstFit
 * \brief IBM provided implementation of a simple first fit plug-in
 */
