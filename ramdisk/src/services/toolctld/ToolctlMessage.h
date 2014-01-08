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

//! \file  ToolctlMessage.h
//! \brief Declaration and inline methods for bgcios::toolctl::ToolctlMessage class.

#ifndef TOOLCTL_TOOLCTLMESSAGE_H
#define TOOLCTL_TOOLCTLMESSAGE_H

// Includes
#include "Tool.h"
#include <ramdisk/include/services/ToolctlMessages.h>
#include <stdlib.h>
#include <tr1/memory>

namespace bgcios
{

namespace toolctl
{

//! \brief Manage an message received from a tool.

class ToolctlMessage
{
public:

   //! \brief  Default constructor.
   //! \param  tool Tool that message was received from.
   //! \param  msg Pointer to message received from tool.

   inline ToolctlMessage(ToolPtr tool, ToolMessage *msg)
   {
      _tool = tool;
      _message = msg;
      _storage = NULL;
   }

   //! \brief  Default destructor.

   inline ~ToolctlMessage()
   {
      if (_storage != NULL) {
         free(_storage);
      }
   }

   //! \brief  Get the tool that the message was received from.
   //! \return Tool object pointer.

   inline ToolPtr getTool(void) const { return _tool; }

   //! \brief  Get the message type value.
   //! \return Message type value.

   inline uint16_t getType(void) const { return _message->header.type; }

   //! \brief  Get the job id value.
   //! \return Job id value.

   inline uint64_t getJobId(void) const { return _message->header.jobId; }

   //! \brief  Get the tool id value.
   //! \return Tool id value.

   inline uint32_t getToolId(void) const { return _message->toolId; }

   //! \brief  Get the rank value.
   //! \return Rank value.

   inline uint32_t getRank(void) const { return _message->header.rank; }

   //! \brief  Get the service value.
   //! \return Service value.

   inline uint8_t getService(void) const { return _message->header.service; }

   //! \brief  Get the version value.
   //! \return Version value.

   inline uint8_t getVersion(void) const { return _message->header.version; }

   //! \brief  Get the length value.
   //! \return Length value.

   inline uint32_t getLength(void) const { return _message->header.length; }

   //! \brief  Get the pointer to the message.
   //! \return Pointer to message.

   inline ToolMessage *getAddress(void) { return _message; }

   //! \brief  Get the pointer to the message header.
   //! \return Pointer to message header.

   inline bgcios::MessageHeader *getHeader(void) const { return &(_message->header); }

   //! \brief  Save the message to internal storage.
   //! \return Pointer to the saved message.

   void *save(void)
   {
      // Allocate storage for the message.
      int err = ::posix_memalign((void **)&_storage, 32, _message->header.length);
      if (err != 0) {
         return NULL;
      }

      // Save the message.
      ::memcpy(_storage, _message, _message->header.length);
      _message = (ToolMessage *)_storage;

      return _storage;
   }

   //! \brief  Return indicator if message is saved to internal storage.
   //! \return True if message is saved to internal storage, otherwise false.

   inline bool isSaved(void) const { return _storage == NULL ? false : true; }


private:

   //! Tool message was received from.
   ToolPtr _tool;

   //! Pointer to current copy of the message.
   ToolMessage *_message;

   //! Pointer to storage for message.
   void *_storage;

};

//! Smart pointer for ToolctlMessage object.
typedef std::tr1::shared_ptr<ToolctlMessage> ToolctlMessagePtr;

} // namespace toolctl

} // namespace bgcios

#endif // TOOLCTL_TOOLCTLMESSAGE_H


