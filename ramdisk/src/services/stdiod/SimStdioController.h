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

//! \file  SimStdioController.h
//! \brief Declaration and inline methods for bgcios::stdio::SimStdioController class.

#ifndef STDIO_SIMSTDIOCONTROLLER_H
#define STDIO_SIMSTDIOCONTROLLER_H

// Includes
#include "StdioController.h"
#include "Job.h"
#include <ramdisk/include/services/common/PointerMap.h>
#include <tr1/memory>

namespace bgcios
{

namespace stdio
{

//! \brief Handle standard I/O service messages when running under control system simulator.

class SimStdioController : public StdioController
{
public:

   //! \brief  Default constructor.
   //! \param  config Configuration from command line and properties file.

   SimStdioController(StdioConfigPtr config);

   //! \brief  Default destructor.

   ~SimStdioController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

   //! \brief  Monitor for events from all connections.
   //! \return Nothing.

   void eventMonitor(void);

   //! \brief  Check if simulation mode is enabled.
   //! \return True if simulation mode is enabled, false if running on hardware.

   bool isSimulation(void) const { return true; }

   //! \brief  Check if running on hardware.
   //! \return True if running on hardware, false if simulation mode is enabled.

   bool isHardware(void) const { return false; }

private:

   //! \brief  Handle events from data channel.
   //! \return Nothing.
   //! \return 0 when successful, errno when unsuccessful.

   int dataChannelHandler(InetSocketPtr authOnly = InetSocketPtr());

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Process a StartJob message.
   //! \return 0 when successful, errno when unsuccessful.

   int startJob(void);

   //! \brief  Process a WriteStdout message.
   //! \return 0 when successful, errno when unsuccessful.

   int writeStdout(void);

   //! \brief  Process a WriteStderr message.
   //! \return 0 when successful, errno when unsuccessful.

   int writeStderr(void);

   //! \brief  Process a CloseStdio message.
   //! \return 0 when successful, errno when unsuccessful.

   int closeStdio(void);

   //! \brief  Process a Terminate message.
   //! \return 0 when successful, errno when unsuccessful.

   int terminate(void);

   //! \brief  Handle an ChangeConfig message.
   //! \return 0 when successful, errno when unsuccessful.

   int changeConfig(void);

   //! \brief  Handle an Authenticate message.
   //! \return 0 when successful, errno when unsuccessful.

   int authenticate(InetSocketPtr channel);

   //! \brief  Handle a Reconnect message.
   //! \return 0 when successful, errno when unsuccessful.

   int reconnect(void);

   //! Simulation id.
   uint16_t _simId;

   //! Map of currently running jobs indexed by job id.
   bgcios::PointerMap<uint64_t, JobPtr> _jobs;

   //! Typedef for job list iterator.
   typedef bgcios::PointerMap <uint64_t, JobPtr>::const_iterator job_list_iterator;
};

//! Smart pointer for SimStdioController object.
typedef std::tr1::shared_ptr<SimStdioController> SimStdioControllerPtr;

} // namespace stdio

} // namespace bgcios

#endif // STDIO_SIMSTDIOCONTROLLER_H

