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

//! \file  ServiceProcess.h
//! \brief Declaration and inline methods for bgcios::iosctl::ServiceProcess class.

#ifndef IOSCTL_SERVICEPROCESS_H
#define IOSCTL_SERVICEPROCESS_H

// Includes
#include <ramdisk/include/services/common/Process.h>
#include <inttypes.h>
#include <string>
#include <tr1/memory>

namespace bgcios
{

namespace iosctl
{

//! \brief I/O service process.

class ServiceProcess : public Process
{
public:

   //! \brief  Default constructor.

   ServiceProcess() : Process() { _numStarts = 0; }

   //! \brief  Constructor.
   //! \param  program Path to executable program of the service.

   ServiceProcess(std::string program) : Process(program) { _numStarts = 0; }

   //! \brief  Default destructor.

   ~ServiceProcess();

   //! \brief  Create a new process and start the service.
   //! \return 0 when successful, errno when unsuccessful.

   bgcios::MessageResult start(void);

   //! \brief  Set the path to the command channel of the service.
   //! \param  path Path to command channel.
   //! \return Nothing.

   void setCmdChannelPath(std::string path) { _cmdChannelPath = path; }

   //! \brief  Set the number of times this service has started.
   //! \param  num The number of times this service has started.
   //! \return Nothing.

   void setNumStarts(uint16_t num) { _numStarts = num; }

   //! \brief  Get the path to the command channel of the service.
   //! \return String with path to command channel.

   std::string getCmdChannelPath(void) const { return _cmdChannelPath; }

   //! \brief  Get the number of times service has been started successfully.
   //! \return Number of times service has been started.

   uint16_t getNumStarts(void) const { return _numStarts; }

private:

   //! Path to command channel.
   std::string _cmdChannelPath;

   //! Number of times service has been started.
   uint16_t _numStarts;
};

//! Smart pointer for ServiceProcess object.
typedef std::tr1::shared_ptr<ServiceProcess> ServiceProcessPtr;

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_SERVICEPROCESS_H

