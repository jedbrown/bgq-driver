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

//! \file  SysioConfig.h
//! \brief Declaration and inline methods for bgcios::sysio::SysioConfig class.

#ifndef SYSIO_SYSIOCONFIG_H
#define SYSIO_SYSIOCONFIG_H

// Includes
#include <ramdisk/include/services/common/CiosConfig.h>
#include <string>
#include <tr1/memory>

namespace bgcios
{

namespace sysio
{

//! \brief Configuration for system I/O daemon.

class SysioConfig : public bgcios::CiosConfig
{
public:

   //! \brief  Default constructor.
   //! \param  argc Number of command line arguments.
   //! \param  argv Array of pointers to command line arguments.

   SysioConfig(int argc, char **argv);

   //! \brief  Get the value of the service_id configuration variable.
   //! \return Unique identifier for this instance of daemon.

   uint32_t getServiceId(void) const;

   //! \brief  Get the value of the log_job_statistics configuration variable.
   //! \return Log job statistics value.

   bool getLogJobStatistics(void) const;

   //! \brief  Get the value of the short_circuit_path configuration variable.
   //! \return Short circuit path.

   std::string getShortCircuitPath(void) const;

   //! \brief  Get the value of the posix_mode configuration variable.
   //! \return Posix mode value.

   bool getPosixMode(void) const;

   //! \brief  Get the value of the log_function_ship_errors configuraton variable.
   //! \return Log function ship errors value.

   bool getLogFunctionShipErrors(void) const;

   const std::string getDynamicLoadLibrary(void) const;
   int getFlags() const;

   //! \brief Get the value of the slow system call timeout.
   //! \return Number of seconds for a syscall to be deemed slow.
   uint64_t getSlowSyscallTimeout(void) const;

   //! \brief Get the value of the hung system call timeout.
   //! \return Number of seconds for a syscall to be deemed hung.
   uint64_t getHungSyscallTimeout(void) const;



};

//! Smart pointer for SysioConfig object.
typedef std::tr1::shared_ptr<SysioConfig> SysioConfigPtr;

} // namespace sysio

} // namespace bgcios

#endif // SYSIO_SYSIOCONFIG_H

