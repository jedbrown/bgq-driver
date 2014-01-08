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

//! \file  SimJobController.h
//! \brief Declaration and inline methods for bgcios::jobctl::SimJobController class.

#ifndef JOBCTL_SIMJOBCONTROLLER_H
#define JOBCTL_SIMJOBCONTROLLER_H

// Includes
#include "JobController.h"
#include "SimulationManager.h"
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief Handle job control service messages when running under control system simulator.

class SimJobController : public JobController
{
public:

   //! \brief  Default constructor.
   //! \param  config Configuration from command line and properties file.

   SimJobController(JobctlConfigPtr config);

   //! \brief  Default destructor.

   ~SimJobController();

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

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Handle events from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int dataChannelHandler(InetSocketPtr dataChannel = InetSocketPtr());

   //! \brief  Handle a SetupJob message.
   //! \return 0 when successful, errno when unsuccessful.

   int setupJob(void);

   //! \brief  Handle a LoadJob message.
   //! \return 0 when successful, errno when unsuccessful.

   int loadJob(void);

   //! \brief  Handle a StartJob message.
   //! \return 0 when successful, errno when unsuccessful.

   int startJob(void);

   //! \brief  Handle a SignalJob message.
   //! \return 0 when successful, errno when unsuccessful.

   int signalJob(void);

   //! \brief  Handle a CleanupJob message.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanupJob(void);

   //! \brief  Handle a ExitJob message.
   //! \return 0 when successful, errno when unsuccessful.

   int exitJob(void);

   //! \brief  Handle a Terminate message.
   //! \return 0 when successful, errno when unsuccessful.

   int terminate(void);

   //! \brief  Handle a StartTool message.
   //! \return 0 when successful, errno when unsuccessful.

   int startTool(void);

   //! \brief  Handle an EndTool message.
   //! \return 0 when successful, errno when unsuccessful.

   int endTool(void);

   //! \brief  Handle a CheckToolStatus message received from data channel.
   //! \return True when successful, false when unsuccessful.

   int checkToolStatus(void);

   //! \brief  Handle an ChangeConfig message.
   //! \return 0 when successful, errno when unsuccessful.

   int changeConfig(void);

   //! \brief  Handle an Authenticate message.
   //! \return 0 when successful, errno when unsuccessful.

   int authenticate(InetSocketPtr channel);

   //! \brief  Handle a Reconnect message.
   //! \return 0 when successful, errno when unsuccessful.

   int reconnect(void);
   
   //! Simulation manager object.
   SimulationManagerPtr _sim;

};

//! Smart pointer for SimJobController object.
typedef std::tr1::shared_ptr<SimJobController> SimJobControllerPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_SIMJOBCONTROLLER_H

