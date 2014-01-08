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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  IosConfig.h
//! \brief Declaration and inline methods for bgcios::iosctl::IosConfig class.

#ifndef IOSCTL_IOSCONFIG_H
#define IOSCTL_IOSCONFIG_H

// Includes
#include <ramdisk/include/services/common/CiosConfig.h>
#include <string>

namespace bgcios
{

namespace iosctl
{

//! \brief Configuration for I/O services daemon.

class IosConfig : public bgcios::CiosConfig
{
public:

   //! \brief  Default constructor.
   //! \param  argc Number of command line arguments.
   //! \param  argv Array of pointers to command line arguments.

   IosConfig(int argc, char **argv);

   //! \brief  Get the value of the listen_port configuration variable.
   //! \return Port number for listening connection.

   uint16_t getListenPort(void) const;

   //! \brief  Get the value of the jobctl_daemon_path configuration variable.
   //! \return Path to job control daemon.

   std::string getJobctlDaemonPath(void) const;

   //! \brief  Get the value of the stdio_daemon_path configuration variable.
   //! \return Path to standard I/O daemon.

   std::string getStdioDaemonPath(void) const;

   //! \brief  Get the value of the sysio_daemon_path configuration variable.
   //! \return Path to system I/O daemon.

   std::string getSysioDaemonPath(void) const;

   //! \brief  Get the value of the toolctl_daemon_path configuration variable.
   //! \return Path to tool control daemon.

   std::string getToolctlDaemonPath(void) const;

   //! \brief  Get the value of the max_service_restarts configuration variable.
   //! \return Maximum number of times a service is restarted.

   uint16_t getMaxServiceRestarts(void) const;

};

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_IOSCONFIG_H

