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
 * \file bgsched/realtime/ClientStateException.h
 * \brief ClientStateErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_REALTIME_CLIENT_STATE_EXCEPTION_H_
#define BGSCHED_REALTIME_CLIENT_STATE_EXCEPTION_H_

#include <bgsched/Exception.h>

namespace bgsched {
namespace realtime {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*! \brief %Client state errors.
 */
struct ClientStateErrors
{
    enum Value
    {
        MustBeConnected //!< The Client must be connected before the method is called.
    };

    /*!
     * \copydoc InputErrors::toString
     */
    static std::string toString( Value v, const std::string& what );
};


/*!
 *  \brief Exception class for client state errors.
 *
 *  %Client state errors are when the real-time API determines that the application has called a
 *  method out of sequence. An example of this is if the application attempts to get the poll
 *  descriptor before the connect() method has returned successfully. If this occurs, the calling
 *  application should not continue since it indicates the application is incorrect.
 *
 */
typedef LogicError<ClientStateErrors> ClientStateException;

//!< @}

} // namespace bgsched::realtime
} // namespace bgsched

#endif
