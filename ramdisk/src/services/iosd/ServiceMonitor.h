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

//! \file  ServiceMonitor.h
//! \brief Declaration and inline methods for bgcios::iosctl::ServiceMonitor class.

#ifndef IOSCTL_SERVICEMONITOR_H
#define IOSCTL_SERVICEMONITOR_H

// Includes
#include <ramdisk/include/services/common/Thread.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include "ServiceManager.h"
#include <tr1/memory>

namespace bgcios
{

namespace iosctl
{

//! \brief  Monitor all services managed by ServiceManager class.

class ServiceMonitor : public Thread
{
public:

   //! \brief  Default constructor.
   //! \param  manager Pointer to ServiceManager object.

   ServiceMonitor(ServiceManagerPtr manager) : Thread()
   {
      _manager = manager;
   }

   //! \brief  Default destructor.

   ~ServiceMonitor();

   //! \brief  Run the monitor function.
   //! \return Pointer to return code value (can be NULL).

   void *run(void);

private:

   //! Pointer to service manager object.
   ServiceManagerPtr _manager;

};

//! Smart pointer for ServiceMonitor class.
typedef std::tr1::shared_ptr<ServiceMonitor> ServiceMonitorPtr;

//! \brief Provide a handler for USR1 signals.

class Sigusr1Handler : public SignalHandler
{
public:

   //! \brief  Default constructor.

   Sigusr1Handler();

   //! \brief  Handler function that is run when signal is delivered.
   //! \param  signum Signal number.
   //! \param  siginfo Pointer to signal info structure with extended info about current state.
   //! \param  sigcontext Pointer to context at time signal was delivered.
   //! \return Nothing.

   static void myHandler(int signum, siginfo_t *siginfo, void *sigcontext);

};

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_SERVICEMONITOR_H


