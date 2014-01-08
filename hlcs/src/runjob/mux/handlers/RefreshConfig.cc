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
#include "mux/handlers/RefreshConfig.h"

#include "common/logging.h"

#include "common/ConnectionContainer.h"

#include "mux/CommandConnection.h"
#include "mux/Multiplexer.h"
#include "mux/Options.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace handlers {

RefreshConfig::RefreshConfig(
        const boost::weak_ptr<Multiplexer>& mux
        ) :
    CommandHandler(),
    _mux( mux ),
    _connection(),
    _response( boost::make_shared<runjob::commands::response::RefreshConfig>() )
{

}

RefreshConfig::~RefreshConfig()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( _connection && _response ) {
        _connection->write( _response );
    }
}

void
RefreshConfig::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    _connection = connection;
    const runjob::commands::request::RefreshConfig::Ptr refreshConfigMessage( 
            boost::static_pointer_cast<runjob::commands::request::RefreshConfig>(request)
            );

    const Multiplexer::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    try {
        // refresh config
        (void)mux->getOptions().getProperties()->reload(
                refreshConfigMessage->_filename
                );

        // reload plugin, there's no need to test if this worked since it is asynchronous
        mux->getPlugin()->start();

        LOG_INFO_MSG( "reloaded config file " << mux->getOptions().getProperties()->getFilename() );
    } catch ( const std::exception& e ) {
        _response->setMessage( e.what() );
        _response->setError( runjob::commands::error::config_file_invalid );
    }
}

} // handlers
} // mux
} // runjob
