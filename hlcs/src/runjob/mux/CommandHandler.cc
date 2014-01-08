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
#include "mux/CommandHandler.h"

#include "mux/client/Container.h"

#include "mux/server/Connection.h"

#include "mux/handlers/LogLevel.h"
#include "mux/handlers/RefreshConfig.h"
#include "mux/handlers/Status.h"

#include "mux/Multiplexer.h"
#include "mux/Options.h"

#include "common/message/InsertRas.h"

#include "common/commands/create.h"

#include "common/ConnectionContainer.h"
#include "common/logging.h"

#include "mux/CommandConnection.h"

#include <hlcs/include/runjob/commands/Message.h>

#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {

void
CommandHandler::create(
        const Multiplexer::Ptr& mux,
        const runjob::commands::Request::Ptr& request,
        const boost::shared_ptr<CommandConnection>& connection
        )
{
    LOG_DEBUG_MSG( "handling " << runjob::commands::Message::toString( request->getTag() ) << " message" );

    Ptr handler;
    switch( request->getTag() ) {
        case runjob::commands::Message::Tag::LogLevel:
            handler.reset( new handlers::LogLevel() );
            break;
        case runjob::commands::Message::Tag::RefreshConfig:
            handler.reset( new handlers::RefreshConfig(mux) );
            break;
        case runjob::commands::Message::Tag::MuxStatus:
            handler.reset( new handlers::Status(mux) );
            break;
        default:
            LOG_INFO_MSG( "unsupported message: " << runjob::commands::Message::toString( request->getTag() ) );
            return;
    }

    // validate and handle the request
    if  (
            connection->getUserType() == bgq::utility::portConfig::UserType::Normal &&
            handler->getUserType() == bgq::utility::portConfig::UserType::Administrator
        )
    {
        // kick out non-administrator users
        LOG_WARN_MSG( runjob::commands::Message::toString(request->getTag()) << " permission denied" );
        runjob::commands::Response::Ptr response = runjob::commands::create(
                request->getTag()
                );
        response->setError( runjob::commands::error::permission_denied );
        connection->write(response);
       
        // tell runjob_server to insert a RAS event for this
        const message::InsertRas::Ptr ras(
                boost::make_shared<message::InsertRas>()
                );
        (void)ras->_details.insert(
                std::make_pair( "USER", connection->getUser()->getUser() )
                );
        (void)ras->_details.insert(
                std::make_pair( "COMMAND", runjob::commands::Message::toString(request->getTag()) )
                );
        LOG_DEBUG_MSG( ras->_details["USER"] );
        LOG_DEBUG_MSG( ras->_details["COMMAND"] );

        const server::Connection::Ptr& server( mux->getServer() );
        server->write( ras );
        
        return;
    }

    try {
        handler->handle( request, connection );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

CommandHandler::CommandHandler()
{

}

CommandHandler::~CommandHandler()
{

}

} // mux
} // runjob
