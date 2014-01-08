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

//! \file  MessageAccumulator.h
//! \brief Declaration and methods for bgcios::MessageAccumulator class.

#ifndef COMMON_MESSAGEACCUMULATOR_H
#define COMMON_MESSAGEACCUMULATOR_H

// Includes
#include <ramdisk/include/services/MessageHeader.h>
#include <tr1/memory>

namespace bgcios
{

//! \brief Accumulate results from multiple messages of the same type.

class MessageAccumulator
{
public:

   //! \brief  Default constructor.

   MessageAccumulator()
   {
      _limit = 0;
      resetCount();
   }

   //! \brief  Constructor.
   //! \param  limit Value for limit.

   MessageAccumulator(int limit)
   {
      _limit = limit;
      resetCount();
   }

   //! \brief  Set the limit to the specified value.
   //! \param  limit New value for limit.
   //! \return Nothing.

   void setLimit(int limit) { _limit = limit; }

   //! \brief  Increment the limit by one.
   //! \return Nothing.

   void incrementLimit(void) { _limit++; }

   //! \brief  Decrement the limit by one.
   //! \return Nothing.

   void decrementLimit(void) { _limit--; }

   //! \brief  Add a message to the accumulator.
   //! \param  msghdr Pointer to message header (only the message header is saved).
   //! \return True when limit has been reached, otherwise false.

   bool add(MessageHeader *msghdr)
   {
      // Save the first message.
      if (_counter == 0) {
         memcpy(&_msghdr, msghdr, sizeof(MessageHeader));
      }

      // Update the accumulated message with the last bad return code.
      if (msghdr->returnCode != bgcios::Success) {
         _msghdr.returnCode = msghdr->returnCode;
         _msghdr.errorCode = msghdr->errorCode;
      }

      // Bump the counter.
      _counter++;

      // Return true when the limit has been reached.
      if (_counter == _limit) {
         return true;
      }

      return false;
   }

   //! \brief  Check if limit has been reached.
   //! \return True when limit has been reached, otherwise false.

   bool atLimit(void) { return _counter == _limit ? true : false; }

   //! \brief  Get pointer to the accumulated message.
   //! \return Pointer to message header.

   MessageHeader *get(void) { return &_msghdr; }

   //! \brief  Get the current number of messages accumulated.
   //! \return Current number of messages.

   int getCount(void) const { return _counter; }

   //! \brief  Get the limit of the accumulator.
   //! \return Limit value.

   int getLimit(void) const { return _limit; }

   //! \brief  Reset the accumulator.
   //! \return Nothing.

   void resetCount(void)
   {
      _counter = 0;
      memset(&_msghdr, 0, sizeof(MessageHeader));
      return;
   }

protected:

   //! Current number of messages accumulated so far.
   volatile int _counter;

   //! Limit that when reached causes add() and atLimit() methods to return true.
   int _limit;

   //! Accumulated message header.
   MessageHeader _msghdr;

};

//! Smart pointer for MessageAccumulator object.
typedef std::tr1::shared_ptr<MessageAccumulator> MessageAccumulatorPtr;

} // namespace bgcios

#endif // COMMON_MESSAGEACCUMULATOR_H


