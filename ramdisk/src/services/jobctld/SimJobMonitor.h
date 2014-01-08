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

//! \file  SimJobMonitor.h
//! \brief Declaration and inline methods for bgcios::jobctl::SimJobMonitor class.

#ifndef JOBCTL_SIMJOBMONITOR_H
#define JOBCTL_SIMJOBMONITOR_H

// Includes
#include "SimulationManager.h"
#include "SimProcess.h"
#include <ramdisk/include/services/common/Thread.h>
#include <ramdisk/include/services/common/LocalSocket.h>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief  Monitor a job running control system simulator.

class SimJobMonitor : public Thread
{
public:

   //! \brief  Default constructor.
   //! \param  sim Pointer to simulation manager object.
   //! \param  proc Pointer to simulated process object.

   SimJobMonitor(SimulationManagerPtr sim, SimProcessPtr proc) : Thread()
   {
      _sim = sim;
      _simProc = proc;
      pthread_attr_setdetachstate(&_attributes, PTHREAD_CREATE_JOINABLE);
   }

   //! \brief  Default destructor.

   ~SimJobMonitor();

   //! \brief  Run the monitor function.
   //! \return Pointer to return code value (can be NULL).

   void * run(void);

   //! \brief  Send a signal to the simulated job.
   //! \param  signo Signal number to send.
   //! \return 0 when successful, errno when unsuccessful.

   int signalJob(int signo) { return _simProc->signal(signo); }

private:

   //! \brief  Send a message to the specified destination command channel.
   //! \param  dest Path to destination command channel.
   //! \param  buffer Pointer to buffer containing message.
   //! \return 0 when successful, errno when unsuccessful.

   int sendMessage(std::string dest, void *buffer);

   //! \brief  Receive a message from the command channel.
   //! \param  buffer Pointer to buffer for storing message.
   //! \return 0 when successful, errno when unsuccessful.

   int recvMessage(void *buffer);

   //! \brief  Send multiple messages to the specified destination command channel.
   //! \param  dest Path to destination command channel.
   //! \param  buffer Pointer to buffer containing message.
   //! \return Nothing.

   void sendMultiple(std::string dest, void *buffer);

   //! Pointer to simulation manager object.
   SimulationManagerPtr _sim;

   //! Pointer to simulated process object.
   SimProcessPtr _simProc;

   //! Command channel for communicating with jobctld and stdiod.
   LocalDatagramSocketPtr _cmdChannel;
};

//! Smart pointer for SimJobMonitor class.
typedef std::tr1::shared_ptr<SimJobMonitor> SimJobMonitorPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_SIMJOBMONITOR_H

