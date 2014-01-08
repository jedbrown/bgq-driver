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

//! \mainpage
//! Common I/O Services (CIOS) provides the I/O subsystem for a Blue Gene/Q system.  CNK uses CIOS for job control,
//! function shipped I/O operations (including standard I/O), and running tools.
//!
//! CIOS includes the following services:
//! <ul>
//! <li> I/O Services Daemon (IOSD) is the service that manages and monitors the other services.
//!      It is started when the I/O node is booted.  There is one IOSD per I/O node.
//!      IOSD is configurable to start daemons for other services.
//!
//! <li> Job Control Daemon (JOBCTLD) is the service used by both CNK and runjob_server for job control.
//!      It is started when the I/O node is booted.  There is one JOBCTLD per I/O node.  JOBCTLD is mostly a bridge between
//!      CNK and runjob_server for loading, starting, signaling, and ending jobs.
//!
//! <li> Standard I/O Daemon (STDIOD) is the service used by both CNK and runjob_server for standard I/O.
//!      It is started when the I/O node is booted.  There is one STDIOD per I/O node.  STDIOD is mostly a bridge between
//!      CNK and runjob_server to receive standard input data and send standard output and standard error data.
//!
//! <li> System I/O Daemon (SYSIOD) is the service used by CNK to provide default function shipped I/O.
//!      It is started when the I/O node is booted.  There is one SYSIOD for each physical compute node and it is
//!      shared by all of the processes running on a compute node.  SYSIOD is a very thin layer that simply invokes 
//!      system calls on behalf of the compute node process.
//!
//! <li> Tool Control Daemon (TOOLCTLD) is the service used by CNK to coordinate access to compute node processes among tools.
//!      It is started when the I/O node is booted.  There is one TOOLCTLD for each physical compute node and it is
//!      shared by all of the processes running on a compute node.  Tools provide additional services to CNK.
//!      A debugger is the primary example of a tool.  Other tools include a programmable core file generation
//!      daemon and a snapshot daemon.  
//! </ul>
//!
//! Additional services can be added as needed to enable new functions or extend the support provided with the system.
//! For example, a different daemon could be provided for different file systems that are using the pluggable file
//! system support in CNK.
//!
//! The I/O link connecting compute node and I/O node partitions is a link on the BG/Q network interface.  On the I/O node,
//! the BG/Q network interface hardware is managed by bgvrnic which enables standard OFED user-mode interfaces for
//! communication on the network interface.
//!
//! There is a common message format that I/O services use for communicating with CNK.  The message format is extensible
//! to allow new services or new functions to be added.  
//!
//! I/O services are configured using the /bgsys/local/etc/bg.properties file.  Each daemon has its own section in the
//! bg.properties file.  A different properties file can be specified using a command line option.
//!
//! I/O services are instrumented with logging using log4cxx to provide common log files.  The supported logging levels are
//! TRACE, DEBUG, INFO, WARN, ERROR, and FATAL.  The default logging level for all services is WARN.  All services allow the logging
//! level to be configured when they are started.  The log files are stored in /bgsys/logs.  I/O services use the logging levels
//! as follows:
//!
//! <ul>
//! <li> FATAL log messages are used for unrecoverable errors that have caused a CIOS daemon to become inoperable.
//!
//! <li> ERROR log messages are used for error conditions that should be investigated.  While the CIOS daemon is still functional,
//!      there might be unexpected problems.
//!
//! <li> WARN log messages are used for recoverable errors such as invalid property values.
//!
//! <li> INFO log messages are used for showing the flow of jobs through the CIOS daemons.
//!
//! <li> DEBUG log messages are used for showing the flow of messages through the CIOS daemons.
//!
//! <li> TRACE log messages are used for showing low-level traces of components used by CIOS daemons.
//! </ul>

