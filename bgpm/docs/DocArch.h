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
 * \page bgpm_architecture BGPM Software Architecture
 *
 *
 * The BGPM architecture consists of the following software layers.
 *
 * \image html swlayers.gif "BlueGene/Q Hardware Performance Counting Software Layers"
 *
 * At the bottom of the image is the UPC hardware layer.
 * The counting function is distributed amoung multiple hardware units which
 * providing varying events, counters, controls and access methods.
 *
 * The UPC Low Level SPI provides the hardware register "bit twiddling" functions, along with
 * some initialization and query functionality.
 *
 * The UPCI (UPC Interface) SPI consolidates functions into the major units, and provides
 * the list of available events.  Mostly, it consolidates many complex operations
 * into the Punit counting type, which provides the threadable events and counters.
 * The UPCI SPI is limited in that it is not thread safe, is generally stateless, and does
 * no memory management.
 * That is, many UPC features would not operate correctly if multiple software
 * threads attempted to independently control them, and the SPI does not prevent clashing
 * operations.  However, it does provide opaque objects and methods to maintain state, and
 * when used properly can tracks states between threads and processes to prevent collisions
 * and inconsistencies.
 * But, in it's simplest form, a single thread can control many operations for multiple cores.
 *
 * It's not expected that the UPCI and UPC interfaces will be used by any but diagnostics and
 * the BGPM API, though the IBM HPC Toolkit has expressed the desire for a lower level interface
 * to the UPC and might be willing to code at that level (and deal with less stability)
 *
 * The CNK kernel also links in a level of UPC LLSPI and UPCI functionality in order to provide
 * syscalls unique to UPCI.  Some UPC operations require privileged access.  Also, some
 * UPC softwarwe hooks are required to maintain the hardware to software thread facade.
 * In addition, the future "automatic performance counting" functionality is mostly implemented
 * withing the CNK kernel.
 *
 * BGPM is the name of the native hardware peformance counting API.
 * BGPM provides a higher API with a mostly consistent user interface into the various
 * units. The level of the interface is intended to be semantically similar to the
 * PAPI-C interface, and will hopefully make the PAPI-C substrate port simple.
 *
 * It's our understanding that the Rice University HPC Toolkit will be implemented above PAPI-C.
 *
 * The classic PAPI interface will not be supported.  Only PAPI-C will be ported.
 *
 *
 *
 */
