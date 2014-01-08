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

//! \file  MessageResult.h
//! \brief Declaration and inline methods for bgcios::MessageResult class.

#ifndef COMMON_MESSAGERESULT_H
#define COMMON_MESSAGERESULT_H

// Includes
#include <ramdisk/include/services/MessageHeader.h>

namespace bgcios
{

//! \brief  Result of handling a message.

class MessageResult
{

public:

   //! \brief  Default constructor.

   MessageResult() : _returnCode(bgcios::Success), _errorCode(0) { }

   //! \brief  Constructor.
   //! \param  rc Return code value.
   //! \param  ec Error code value.

   MessageResult(bgcios::ReturnCode rc, int ec) : _returnCode(rc), _errorCode(ec) { }

   //! \brief  Set the result.
   //! \param  rc Return code value.
   //! \param  ec Error code value.
   //! \return Nothing.

   void set(bgcios::ReturnCode rc, int ec)
   {
      _returnCode = rc;
      _errorCode = ec;
      return;
   }

   //! \brief  Check for successsful result.
   //! \return True if handling message was successful, otherwise false.

   bool isSuccess(void) const { return _returnCode == bgcios::Success ? true : false; }

   //! \brief  Check for error result.
   //! \return True if there was an error handling message, otherwise false.

   bool isError(void) const { return _returnCode != bgcios::Success ? true : false; }

   //! \brief  Get the return code from handling message.
   //! \return Return code value.

   bgcios::ReturnCode returnCode(void) const { return _returnCode; }

   //! \brief  Get the error code from handling message.
   //! \return Error code value.

   int errorCode(void) const { return _errorCode; }

   //! \brief  Set a message header with the result.
   //! \param  header Message header to set.
   //! \return Nothing.

   void setHeader(bgcios::MessageHeader& header)
   {
      header.returnCode = _returnCode;
      header.errorCode = (uint32_t)_errorCode;
      return;
   }

private:

   //! Return code.
   bgcios::ReturnCode _returnCode;

   //! Error code.
   int _errorCode;
};

} // namespace bgcios

#endif // COMMON_MESSAGERESULT_H

