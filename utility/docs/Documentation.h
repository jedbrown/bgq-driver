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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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
 * \mainpage BG/Q Utility
 * \author Brant Knudson, Sam Miller, Will Stockdell
 * \version 1.0.0
 *
 * The utility directory provides implementations of basic functions useful across
 * multiple components. 
 *
 * - Common logging: Log.h
 * - Configuration %file: Properties.h
 * - CxxSockets
 * - Control System \link bgq::utility::performance performance counter\endlink classes and macros.
 * - Port Configuration \link bgq::utility::PortConfiguration classes\endlink for \link bgq::utility::ClientPortConfiguration clients\endlink and \link bgq::utility::ServerPortConfiguration servers\endlink
 * - Executing arbitrary code at scope exit with \link bgq::utility::ScopeGuard ScopeGuard\endlink
 * - Generic \link bgq::utility::PluginHandle plugin interface\endlink  for loading shared libraries.
 * - Implementation of the \link bgq::utility::Singleton Singleton Design Pattern\endlink.
 * - \link bgq::utility::SignalHandler SignalHandler\endlink using <a href="http://think-async.com">Boost.Asio</a> design patterns.
 * - \link bgq::utility::Inotify Inotify\endlink wrapper using <a href="http://think-async.com">Boost.Asio</a> design patterns.
 * - \link bgq::utility::Symlink Symlink\endlink wrapper around readlink(2).
 * - \link bgq::utility::BoolAlpha BoolAlpha\endlink wrapper around std::boolalpha.
 *
 * The library is libbgutility.so
 */

/*!
 * \namespace CxxSockets
 * \brief Classes implementing a generic, sockets-based protocol.
 */

/*!
 *  \namespace bgq::utility
 *
 *  \brief Blue Gene utilities.
 */

/*!
 * \directory cxxsockets
 * \directory performance
 * \directory portConfiguration
 */
