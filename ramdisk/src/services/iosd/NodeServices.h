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

//! \file  NodeServices.h
//! \brief Declaration and inline methods for bgcios::iosctl::NodeServices class.

#ifndef IOSCTL_NODESERVICES_H
#define IOSCTL_NODESERVICES_H

// Includes
#include <ramdisk/include/services/common/MessageAccumulator.h>
#include <ramdisk/include/services/IosctlMessages.h>
#include <string>
#include <tr1/memory>

namespace bgcios
{

namespace iosctl
{

//! \brief Track a request to start services for a compute node.

class NodeServices
{
public:

   //! \brief  Default constructor.
   //! \param  inMsg Pointer to StartNodeServices message.
   //! \param  dest Path to destination command channel.
   //! \param  numServices Number of services started for compute node.

   NodeServices(StartNodeServicesMessage *inMsg, std::string dest, int numServices)
   {
      // Build a StartNodeServicesAck message that will be sent when all node services are ready.
      memcpy(&(_ackMsg.header), &(inMsg->header), sizeof(MessageHeader));
      _ackMsg.header.type = StartNodeServicesAck;
      _ackMsg.header.length = sizeof(StartNodeServicesAckMessage);
      _ackMsg.header.returnCode = bgcios::Success;
      _ackMsg.serviceId = inMsg->serviceId;

      // Save the destination command channel path.
      _destCmdChannel = dest;

      // Set limit for Ready message accumulator.
      _readyAccumulator.setLimit(numServices);
   }

   //! \brief  Add a Ready message from a service started for the compute node.
   //! \param  inMsg Pointer to Ready message from service.
   //! \return True if Ready messages have been received from all services, otherwise false.

   bool add(ReadyMessage *inMsg) { return _readyAccumulator.add(&(inMsg->header)); }

   //! \brief  Get pointer to StartNodeServicesAck message.
   //! \return Pointer to StartNodeServicesAck message.

   StartNodeServicesAckMessage *getAckMsg(void) { return &_ackMsg; }

   //! \brief  Get path to destination command channel.
   //! \return Path string.

   const std::string& getCmdChannel(void) const { return _destCmdChannel; }

   //! \brief  Get the service id for the compute node.
   //! \return Service id value.

   uint32_t getServiceId(void) const { return _ackMsg.serviceId; }

   //! \brief  Check if ready to send acknowledge message to destination command channel.
   //! \return True if ready to send message, otherwise false.

   bool isReady(void) { return _readyAccumulator.atLimit(); }

private:

   //! StartNodeServicesAck message to send when all services are ready.
   StartNodeServicesAckMessage _ackMsg;

   //! Path to destination command channel.
   std::string _destCmdChannel;

   //! Accumulator for Ready messages from services.
   MessageAccumulator _readyAccumulator;

};

//! Smart pointer for ComputeNode object.
typedef std::tr1::shared_ptr<NodeServices> NodeServicesPtr;

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_NODESERVICES_H



