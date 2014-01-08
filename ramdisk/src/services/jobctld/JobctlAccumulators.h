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

//! \file  JobctlAccumulators.h
//! \brief Declaration and inline methods for bgcios::jobctl::ExitJobAccumulator class.

#ifndef JOBCTL_JOBCTLACCUMULATORS_H
#define JOBCTL_JOBCTLACCUMULATORS_H

// Includes
#include <ramdisk/include/services/common/MessageAccumulator.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <sys/wait.h>

namespace bgcios
{

namespace jobctl
{

//! \brief Accumulate ExitProcess messages.

class ExitProcessAccumulator : public bgcios::MessageAccumulator
{
public:

   //! \brief  Default constructor.

   ExitProcessAccumulator() : bgcios::MessageAccumulator() { }

   //! \brief  Add a message to the accumulator.
   //! \param  msg Pointer to CleanupJobAck message.
   //! \return True when limit has been reached, otherwise false.

   bool add(ExitProcessMessage *msg)
   {
      // Save the first message.
      if (_counter == 0) {
         memcpy(&_message, msg, sizeof(ExitProcessMessage));
      }

      // Update the accumulated message with the last bad return code.
      if (msg->header.returnCode != bgcios::Success) {
         _message.header.returnCode = msg->header.returnCode;
         _message.header.errorCode = msg->header.errorCode;
      }

      // Bump the counter.
      _counter++;

      // Return true when the limit has been reached.
      if (_counter == _limit) {
         return true;
      }

      return false;
   }

   //! \brief  Get pointer to the accumulated message.
   //! \return Pointer to message.

   ExitProcessMessage *get(void) { return &_message; }

private:

   //! Accumulated message.
   ExitProcessMessage _message;

};

//! \brief Accumulate ExitJob messages.

class ExitJobAccumulator : public bgcios::MessageAccumulator
{
public:

   //! \brief  Default constructor.

   ExitJobAccumulator() : bgcios::MessageAccumulator() { }

   //! \brief  Add a message to the accumulator.
   //! \param  msg Pointer to ExitJob message.
   //! \return True when limit has been reached, otherwise false.

   bool add(ExitJobMessage *msg)
   {
      // Save the first message.
      if (_counter == 0) {
         memcpy(&_message, msg, sizeof(ExitJobMessage));
      }

      // If one node has already ended by signal, do not change the status.
      if (!WIFSIGNALED(_message.status)) {
         if (WIFSIGNALED(msg->status)) {
            _message.status = msg->status;
         }
         else if (WIFEXITED(msg->status)) { // Save the highest exit status.
            if (WEXITSTATUS(msg->status) > WEXITSTATUS(_message.status)) {
               _message.status = msg->status;
            }
         }
      }

      // Update the accumulated message with the last bad return code.
      if (msg->header.returnCode != bgcios::Success) {
         _message.header.returnCode = msg->header.returnCode;
         _message.header.errorCode = msg->header.errorCode;
      }

      // Bump the counter.
      _counter++;

      // Return true when the limit has been reached.
      if (_counter == _limit) {
         return true;
      }

      return false;
   }

   //! \brief  Get pointer to the accumulated message.
   //! \return Pointer to message.

   const ExitJobMessage *get(void) const { return &_message; }

   //! \brief  Reset the accumulator.
   //! \return Nothing.

   void resetCount(void)
   {
      _counter = 0;
      memset(&_message, 0, sizeof(ExitJobMessage));
      return;
   }

private:

   //! Accumulated message.
   ExitJobMessage _message;

};

//! \brief Accumulate CleanupJobAck messages.

class CleanupJobAckAccumulator : public bgcios::MessageAccumulator
{
public:

   //! \brief  Default constructor.

   CleanupJobAckAccumulator() : bgcios::MessageAccumulator() { }

   //! \brief  Add a message to the accumulator.
   //! \param  msg Pointer to CleanupJobAck message.
   //! \return True when limit has been reached, otherwise false.

   bool add(CleanupJobAckMessage *msg)
   {
      // Save the first message.
      if (_counter == 0) {
         memcpy(&_message, msg, sizeof(CleanupJobAckMessage));
      }

      // Update the accumulated message with the last bad return code.
      if (msg->header.returnCode != bgcios::Success) {
         _message.header.returnCode = msg->header.returnCode;
         _message.header.errorCode = msg->header.errorCode;
      }

      // Bump the counter.
      _counter++;

      // Return true when the limit has been reached.
      if (_counter == _limit) {
         return true;
      }

      return false;
   }

   //! \brief  Get pointer to the accumulated message.
   //! \return Pointer to message.

   CleanupJobAckMessage *get(void) { return &_message; }

private:

   //! Accumulated message.
   CleanupJobAckMessage _message;

};

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_JOBCTLACCUMULATORS_H

