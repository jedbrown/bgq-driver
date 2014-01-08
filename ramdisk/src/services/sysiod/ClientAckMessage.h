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

//! \file  ClientAckMessage.h
//! \brief Declaration and inline methods for bgcios::sysio::ClientAckMessage class.

#ifndef SYSIO_CLIENTACKMESSAGE_H
#define SYSIO_CLIENTACKMESSAGE_H

// Includes
#include "ClientMessage.h"
#include <ramdisk/include/services/SysioMessages.h>
#include <ramdisk/include/services/common/RdmaMemoryRegion.h>
#include <tr1/memory>

namespace bgcios
{

namespace sysio
{

//! \brief Manage an acknowledge message to be sent to the client connection.

class ClientAckMessage
{
public:

   //! \brief  Default constructor.

  ClientAckMessage(bgcios::MessageHeader * inAddress)
  : _message(inAddress)
  {
  };

  //! \brief  Get the pointer to the message.
  //! \return Pointer to message.

  bgcios::MessageHeader *getAddress(void) const { return _message; }

  uint32_t getLength(){return _message->length;}
  
   //! Maximum size of a message.
  static const uint32_t MaxMessageSize = 512;

private:

   //! Pointer to current copy of the message.
   bgcios::MessageHeader *_message;

};

//! Smart pointer for ClientAckMessage object.
typedef std::tr1::shared_ptr<ClientAckMessage> ClientAckMessagePtr;

} // namespace sysio

} // namespace bgcios

#endif // SYSIO_CLIENTACKMESSAGE_H


