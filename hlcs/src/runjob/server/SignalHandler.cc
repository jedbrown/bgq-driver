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
#include "server/SignalHandler.h"

#include "server/Server.h"
#include "server/Shutdown.h"

#include "common/logging.h"
#include "common/LogSignalInfo.h"

#include <vector>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

SignalHandler::Ptr
SignalHandler::create(
        const Server::Ptr& server
        )
{
    const Ptr result(
            new SignalHandler( server )
            );

    LOG_DEBUG_MSG( "waiting for signals: TERM and INT" );

    result->_signals.async_wait(
            boost::bind(
                &SignalHandler::handle,
                result,
                _1,
                _2
                )
            );

    return result;
}

SignalHandler::SignalHandler(
        const Server::Ptr& server
        ) :
    _signals( server->getIoService() ),
    _server( server )
{

}

SignalHandler::~SignalHandler()
{
    LOG_DEBUG_MSG( "terminating" );
}

void
SignalHandler::handle(
        const boost::system::error_code& error,
        const siginfo_t& siginfo
        )
{
    if ( error == boost::asio::error::operation_aborted ) return;

    if ( error ) {
        LOG_ERROR_MSG( "signal handler: " << boost::system::system_error(error).what() );
        return;
    }

    // log where this signal came from
    LogSignalInfo info( siginfo );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    Shutdown::create( server );
}

} // server
} // runjob
