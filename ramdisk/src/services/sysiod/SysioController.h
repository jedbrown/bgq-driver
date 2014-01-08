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

//! \file  SysioController.h
//! \brief Declaration and inline methods for bgcios::sysio::SysioController class.

#ifndef SYSIO_SYSIOCONTROLLER_H
#define SYSIO_SYSIOCONTROLLER_H

// Includes
#include "ClientMonitor.h"
#include "SysioConfig.h"
#include <ramdisk/include/services/common/ServiceController.h>
#include <tr1/memory>

namespace bgcios
{

namespace sysio
{

//! \brief Handle system I/O service messages.

class SysioController : public bgcios::ServiceController
{

public:

   //! \brief  Default constructor.

   SysioController();

   //! \brief  Default destructor.

   ~SysioController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  config Configuration from command line and properties.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(SysioConfigPtr config);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

   //! \brief  Monitor for events from all connections.
   //! \return Nothing.

   void eventMonitor(void);

   ClientMonitorPtr getClientMonitor(){return _clientMonitor;}

private:

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Handle an Interrupt message from Job Control for job end
   //! \param  source Path to source command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int interrupt(std::string source);

   //! \brief  Continue on any Interrupt message from Job Control for job end
   //! \return 0 when successful, errno when unsuccessful.

   int interruptContinue();

   //! \brief  Handle an Interrupt message from Tool Control to interrupt blocking function call
   //! \param  source Path to source command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int interruptForToolCtl(std::string source);

   //! Unique identifier for this instance of the daemon.
   uint32_t _serviceId;

   //! Thread to monitor the client connection to the compute node.
   ClientMonitorPtr _clientMonitor;

   //! List of Interrupt messages queued waiting for final job cleanup
   std::list <bgcios::iosctl::InterruptMessage *> _queuedInterruptMessages;
};

//! Smart pointer for SysioController object.
typedef std::tr1::shared_ptr<SysioController> SysioControllerPtr;

} // namespace sysio

} // namespace bgcios

#endif // SYSIO_SYSIOCONTROLLER_H

