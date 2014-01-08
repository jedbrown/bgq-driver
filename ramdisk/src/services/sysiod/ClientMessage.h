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

//! \file  ClientMessage.h
//! \brief Declaration and inline methods for bgcios::sysio::ClientMessage class.

#ifndef SYSIO_CLIENTMESSAGE_H
#define SYSIO_CLIENTMESSAGE_H

// Includes
#include <ramdisk/include/services/MessageHeader.h>
#include <tr1/memory>

namespace bgcios
{

namespace sysio
{

//! \brief Manage an message received from the client connection.

class ClientMessage
{
public:

   //! \brief  Default constructor.
   //! \param  msg Pointer to message header for message.
   //! \param  processorId Processor id of thread that message was received from.

   ClientMessage(bgcios::MessageHeader *msg, uint32_t processorId)
   {
      _message = msg;
      _processorId = processorId;
      _storage = NULL;
      _inProgress =  false;
      _messageAck = NULL;
   }

   //! \brief  Default destructor.

   ~ClientMessage();

   //! \brief  Get the message type.
   //! \return Message type value.
inline
   uint16_t getType(void) const { return _message->type; }

   //! \brief  Get the job id value.
   //! \return Job id value.
inline
   uint64_t getJobId(void) const { return _message->jobId; }

   //! \brief  Get the rank value.
   //! \return Rank value.
inline
   uint32_t getRank(void) const { return _message->rank; }

   //! \brief  Get the pointer to the message.
   //! \return Pointer to message.
inline
   bgcios::MessageHeader *getAddress(void) { return _message; }

   //! \brief  Save the message to internal storage.
   //! \return Pointer to the saved message.

   void *save(void);

   
   //! \brief  Set the message to this message
   //! \param  msg  New value of message pointer
   //! \note   Used to switch from original incoming message to Ack message for tracking
   void setMessage(bgcios::MessageHeader *msg){_message=msg;}

   //! \brief  Return indicator if message is saved to internal storage.
   //! \return True if message is saved to internal storage, otherwise false.
inline
   bool isSaved(void) const { return _storage == NULL ? false : true; }

   //! \brief  Set indicator to mark message as in progress.
   //! \param  indicator New value of in progress indicator.
   //! \return Nothing.
inline
   void setInProgress(bool indicator) { _inProgress = indicator; }

   //! \brief  Return indicator if message is in progress.
   //! \return True if message is in progress, otherwise false.
inline
   bool isInProgress(void) const { return _inProgress; }

   //! \brief  Get the processor id of the thread that message was received from.
   //! \return Processor id value.
inline
   uint32_t getProcessorId(void) const { return _processorId; }

inline
void    saveAck(bgcios::MessageHeader *messageAck){_messageAck=messageAck;}
inline
bgcios::MessageHeader *   getAck(){return _messageAck;}

private:

   //! Pointer to current copy of the message.
   bgcios::MessageHeader *_message;

   //! Pointer to current copy of the Ack message.
   bgcios::MessageHeader *_messageAck;

   //! Pointer to storage for saved message.
   void *_storage;

   //! Processor id of thread that message was received from.
   uint32_t _processorId;

   //! Indicator if message is in progress.
   bool _inProgress;

};

//! Smart pointer for ClientMessage object.
typedef std::tr1::shared_ptr<ClientMessage> ClientMessagePtr;

} // namespace sysio

} // namespace bgcios

#endif // SYSIO_CLIENTMESSAGE_H

