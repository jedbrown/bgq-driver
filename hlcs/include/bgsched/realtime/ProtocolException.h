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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

/*!
 * \file bgsched/realtime/ProtocolException.h
 * \brief ProtocolErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_REALTIME_PROTOCOL_EXCEPTION_H_
#define BGSCHED_REALTIME_PROTOCOL_EXCEPTION_H_

#include <bgsched/Exception.h>

namespace bgsched {
namespace realtime {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*! \brief Protocol errors.
 *
 */
struct ProtocolErrors
{
    enum Value
    {
        MessageTooLong, //!< A message received from the real-time server is too long.
        UnexpectedMessageType, //!< The type of message received from the real-time server is not expected.
        ErrorReadingMessage, //!< An error occurred parsing a message received from the real-time server.
        UnexpectedDbChangeType, //!< The type of DB change message received from the real-time server is not expected.
        MessageNotValid //!< A message received from the real-time server is not valid.
    };

    /*!
     * \copydoc InputErrors::toString
     */
    static std::string toString( Value v, const std::string& what );
};


/*! \brief Exception class for protocol errors.
 *
 * Protocol errors are when the server sends the client something that it wasn't expecting.
 * The typical cause of this is that the client and server are not at the same level or
 * the stream has been corrupted. The application should not use real-time until the
 * problem is corrected.
 *
 */
typedef RuntimeError<ProtocolErrors> ProtocolException;

//!< @}

} // namespace bgsched::realtime
} // namespace bgsched

#endif
