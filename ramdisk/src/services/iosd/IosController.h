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

//! \file  IosController.h
//! \brief Declaration and inline methods for bgcios::iosctl::IosController class.

#ifndef IOSCTL_IOSCONTROLLER_H
#define IOSCTL_IOSCONTROLLER_H

// Includes
#include <ramdisk/include/services/common/ServiceController.h>
#include <ramdisk/include/services/common/InetSocket.h>
#include <ramdisk/include/services/common/MessageAccumulator.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include <tr1/memory>

namespace bgcios
{

namespace iosctl
{

//! \brief Handle job control service messages.

class IosController : public bgcios::ServiceController
{
public:

   //! \brief  Default constructor.

   IosController() : bgcios::ServiceController() { }

   //! \brief  Default destructor.

   ~IosController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int cleanup(void) { return 0; }

   //! \brief  Monitor for events from all connections.
   //! \return Nothing.

   void eventMonitor(bgcios::SigtermHandler& sigtermHandler);

   //! \brief  Wait for service daemons to send Ready messages.
   //! \param  numServices Number of service daemons.
   //! \return 0 when successful, errno when unsuccessful.

   int waitForReadyMessages(int numServices);

   //! \brief  Wait for service daemons to send TerminateAck messages.
   //! \param  numServices Number of service daemons.
   //! \return 0 when successful, errno when unsuccessful.

   int waitForTerminateAckMessages(int numServices);

protected:

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Handle events from data channel.
   //! \return Nothing.

   void dataChannelHandler(void);

   //! \brief  Process a Ready message.
   //! \param  dest Path to destination command channel.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int ready(std::string dest);

   //! \brief  Process a StartNodeServices message.
   //! \param  dest Path to destination command channel.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int startNodeServices(std::string dest);

   //! \brief  Process a AllocateRegion message.
   //! \param  dest Path to destination command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int allocateRegion(std::string dest);

   //! \brief  Process a ReleaseRegion message.
   //! \param  dest Path to destination command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int releaseRegion(std::string dest);

   //! Accumulate Ready messages.
   MessageAccumulator _readyAccumulator;

   //! Accumulate TerminateAck messages. 
   MessageAccumulator _terminateAckAccumulator;

};

//! Smart pointer for JobController object.
typedef std::tr1::shared_ptr<IosController> IosControllerPtr;

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_IOSCONTROLLER_H


