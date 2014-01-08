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
#include "mux/CommandListener.h"

#include "mux/client/Container.h"

#include "common/ConnectionContainer.h"
#include "common/logging.h"

#include "mux/CommandConnection.h"
#include "mux/Multiplexer.h"
#include "mux/Options.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace runjob {
namespace mux {

LOG_DECLARE_FILE( runjob::mux::log );

CommandListener::CommandListener(
        boost::shared_ptr<Multiplexer> mux
        ) :
    _io_service( mux->getIoService() ),
    _options( mux->getOptions() ),
    _acceptor( mux->getIoService(), _options.getCommandPort() ),
    _mux( mux )
{

}

CommandListener::~CommandListener()
{
    LOG_TRACE_MSG( "terminating" );
}

void
CommandListener::start()
{
    _acceptor.start(
            boost::bind(
                &CommandListener::acceptHandler,
                shared_from_this(),
                _1
                )
            );
}

void
CommandListener::acceptHandler(
        const bgq::utility::Acceptor::AcceptArguments& args
        )
{
    LOG_TRACE_MSG( "accept handler" );

    if ( args.status == bgq::utility::Acceptor::Status::OK ) {
        // log remote user
        LOG_INFO_MSG( 
                "connection from " << args.user_id_ptr->getUser() <<
                " (" <<
                ( args.user_type == bgq::utility::portConfig::UserType::Administrator ? "administrator" : "normal" )
                << ")"
                );

        // create new connection
        const boost::shared_ptr<CommandConnection> connection(
                new CommandConnection(
                    args.socket_ptr,
                    args.user_id_ptr,
                    args.user_type,
                    _mux
                    )
                );

        // start handling requests
        connection->start();
    } else if ( args.status == bgq::utility::Acceptor::Status::NowAccepting ) {
        LOG_INFO_MSG( "accepting connections");
        BOOST_FOREACH( const auto& i, args.endpoints ) {
            LOG_INFO_MSG( "accepting on " << i );
        }
    } else if (
            args.status == bgq::utility::Acceptor::Status::ListenError ||
            args.status == bgq::utility::Acceptor::Status::ResolveError ||
            args.status == bgq::utility::Acceptor::Status::NoAcceptors
            )
    {
        _io_service.stop();
    } else if ( args.status == bgq::utility::Acceptor::Status::AcceptError ) {
        // still accepting on other ports
        LOG_WARN_MSG( "lost an acceptor" );
    }
}

} // mux
} // runjob
