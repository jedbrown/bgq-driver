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
#include "server/handlers/RefreshConfig.h"

#include "common/logging.h"

#include "common/ConnectionContainer.h"

#include "server/block/Container.h"

#include "server/CommandConnection.h"
#include "server/Options.h"
#include "server/Server.h"

#include <utility/include/ScopeGuard.h>

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

RefreshConfig::RefreshConfig(
        const Server::Ptr& server
        ) :
    CommandHandler( server )
{

}

void
RefreshConfig::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const runjob::commands::request::RefreshConfig::Ptr refreshConfigMessage(
            boost::static_pointer_cast<runjob::commands::request::RefreshConfig>( request )
            );

    const runjob::commands::response::RefreshConfig::Ptr response(
            new runjob::commands::response::RefreshConfig
            );

    try {
        (void)_options.getProperties()->reload(
                refreshConfigMessage->_filename
                );
        LOG_INFO_MSG( "reloaded config file " << _options.getProperties()->getFilename() );
    }  catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
        response->setError( runjob::commands::error::config_file_invalid );
        response->setMessage( e.what() );
    }

    connection->write( response );
}


} // handlers
} // server
} // runjob
