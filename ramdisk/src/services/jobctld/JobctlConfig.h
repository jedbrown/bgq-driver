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

//! \file  JobctlConfig.h
//! \brief Declaration and inline methods for bgcios::jobctl::JobctlConfig class.

#ifndef JOBCTL_JOBCTLCONFIG_H
#define JOBCTL_JOBCTLCONFIG_H

// Includes
#include <ramdisk/include/services/common/CiosConfig.h>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief  Configuration for job control daemon.

class JobctlConfig : public bgcios::CiosConfig
{
public:

   //! \brief  Default constructor.
   //! \param  argc Number of command line arguments.
   //! \param  argv Array of pointers to command line arguments.

   JobctlConfig(int argc, char **argv);

   //! \brief  Get the value of the listen_port configuration variable.
   //! \return Port number for listening connection.

   uint16_t getListenPort(void) const;

   //! \brief  Get the value of the job_prolog_program_path configuration variable.
   //! \return Path to job prolog program.

   const std::string getJobPrologProgramPath(void) const;

   //! \brief  Get the value of the job_epilog_program_path configuration variable.
   //! \return Path to job epilog program.

   const std::string getJobEpilogProgramPath(void) const;

   //! \brief  Get the value of the job_prolog_program_timeout configuration variable.
   //! \return Number of seconds to wait for job prolog program to complete.

   int32_t getJobPrologProgramTimeout(void) const;

   //! \brief  Get the value of the job_epilog_program_timeout configuration variable.
   //! \return Number of seconds to wait for job epilog program to complete.

   int32_t getJobEpilogProgramTimeout(void) const;

   //! \brief  Get the value of the jobctl_heartbeat configuration variable.
   //! \return Number of seconds between heartbeats

   int32_t getHeartbeatTimeout(void) const;
   
   //! \brief  Get the value of the start_time_threshold configuration variable.
   //! \return Number of seconds between heartbeats

   uint32_t getStartTimeThreshold(void) const;
};

//! Smart pointer for JobctlConfig object.
typedef std::tr1::shared_ptr<JobctlConfig> JobctlConfigPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_JOBCTLCONFIG_H
