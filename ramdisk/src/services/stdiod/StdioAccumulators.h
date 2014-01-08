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

//! \file  StdioAccumulators.h
//! \brief Declaration and inline methods for bgcios::stdio::CloseStdioAccumulator class.

#ifndef STDIO_STDIOACCUMULATORS_H
#define STDIO_STDIOACCUMULATORS_H

// Includes
#include <ramdisk/include/services/common/MessageAccumulator.h>
#include <ramdisk/include/services/StdioMessages.h>

namespace bgcios
{

namespace stdio
{

//! \brief Accumulate CloseStdio messages.

class CloseStdioAccumulator : public bgcios::MessageAccumulator
{
public:

   //! \brief  Default constructor.

   CloseStdioAccumulator() : bgcios::MessageAccumulator() { }

   //! \brief  Add a message to the accumulator.
   //! \param  msg Pointer to CloseStdio message.
   //! \return True when limit has been reached, otherwise false.

   bool add(CloseStdioMessage *msg)
   {
      // Save the first message.
      if (_counter == 0) {
         memcpy(&_message, msg, sizeof(CloseStdioMessage));
      }

      // Update the accumulated message with the last bad return code.
      if (msg->header.returnCode != bgcios::Success) {
         _message.header.returnCode = msg->header.returnCode;
         _message.header.errorCode = msg->header.errorCode;
      }

      // Bump the counter.
      _counter += (int)msg->header.rank; // Rank field is the number of ranks on node

      // Return true when the limit has been reached.
      if (_counter == _limit) {
         return true;
      }

      return false;
   }

   //! \brief  Get pointer to the accumulated message.
   //! \return Pointer to message.

   const CloseStdioMessage *get(void) { return &_message; }

   //! \brief  Reset the accumulator.
   //! \return Nothing.

   void resetCount(void)
   {
      _counter = 0;
      memset(&_message, 0, sizeof(CloseStdioMessage));
      return;
   }

private:

   //! Accumulated message.
   CloseStdioMessage _message;
};

} // namespace stdio

} // namespace bgcios

#endif // STDIO_STDIOACCUMULATORS_H

