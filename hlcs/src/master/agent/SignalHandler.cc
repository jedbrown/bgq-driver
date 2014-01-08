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

#include "SignalHandler.h"

LOG_DECLARE_FILE( "master" );

SignalHandler::Ptr
SignalHandler::create(
        boost::asio::io_service& io_service
        )
{
    const Ptr result( 
            new SignalHandler(
                io_service
                )
            );

    result->async_wait(
            boost::bind(
                &SignalHandler::handler,
                result,
                _1,
                _2
                )
            );

    return result;
}

SignalHandler::SignalHandler(
        boost::asio::io_service& io_service
        ) : 
    Signals(io_service),
    _io_service( io_service ),
    _signal( 0 )
{

}

SignalHandler::~SignalHandler()
{
    LOG_DEBUG_MSG("Terminating")
}

void
SignalHandler::handler(
        const boost::system::error_code& error,
        const siginfo_t& siginfo
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    if ( !error ) {
        switch (siginfo.si_signo) {
            case SIGUSR1:
            case SIGPIPE:
            case SIGHUP:
                LOG_DEBUG_MSG( "ignored signal " << siginfo.si_signo);
                this->async_wait(
                        boost::bind(
                            &SignalHandler::handler,
                            shared_from_this(),
                            _1,
                            _2
                            )
                        );
                break;
            default:
                LOG_INFO_MSG( "received signal " << siginfo.si_signo);
                _signal = siginfo.si_signo;
                _io_service.stop();
                break;
        }
    } else {
        LOG_INFO_MSG( "signal handler: " << boost::system::system_error(error).what());
    }
}
