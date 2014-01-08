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
#ifndef RUNJOB_COMMON_TERMINATE_HANDLER_H
#define RUNJOB_COMMON_TERMINATE_HANDLER_H

#include <utility/include/SignalHandler.h>

namespace runjob {

/*!
 * \brief Handle SIGINT and SIGTERM signals by cancelling the Boost.Asio I/O service.
 */
class TerminateHandler
{
public:
    /*!
     * \brief The signals we'll watch for.
     */
    typedef bgq::utility::SignalHandler<SIGINT,SIGTERM> Signals;

public:
    /*!
     * \brief ctor
     */
    explicit TerminateHandler(
            boost::asio::io_service& io_service //!< [in]
            ) :
        _signals( io_service),
        _io_service( io_service )
    {
        _signals.async_wait(
                boost::bind(
                    &TerminateHandler::handler,
                    this,
                    _1,
                    _2
                    )
                );
    }

private:
    /*!
     * \brief handle a signal.
     */
    void handler(
            const boost::system::error_code& error,
            const siginfo_t& 
            );

private:
    Signals _signals;
    boost::asio::io_service& _io_service;
};

} // runjob

#endif
