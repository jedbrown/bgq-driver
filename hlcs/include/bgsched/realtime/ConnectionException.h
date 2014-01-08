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
 * \file bgsched/realtime/ConnectionException.h
 * \brief ConnectionErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_REALTIME_CONNECTION_EXCEPTION_H_
#define BGSCHED_REALTIME_CONNECTION_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <iosfwd>

namespace bgsched {
namespace realtime {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*! \brief Connection errors
 */
struct ConnectionErrors
{
    enum Value
    {
        CannotResolve, //!< Cannot resolve the real-time server host or port.
        CannotConnect, //!< Cannot connect to the real-time server.
        LostConnection //!< Unexpectedly lost the connection to the real-time server.
    };

    /*!
     * \copydoc InputErrors::toString
     */
    static std::string toString( Value v, const std::string& what );
};


/*!
 *  \brief Exception class for connection problems.
 *
 *  Connection problems are when the real-time API either can't connect or loses the connection to
 *  the real-time server unexpectedly. This is an indication that the server is down or there's no
 *  network connectivity. The calling application will probably want to retry the connection at a
 *  later time to recover.
 *
 */
typedef RuntimeError<ConnectionErrors> ConnectionException;

//!< @}

} // namespace bgsched::realtime
} // namespace bgsched

#endif
