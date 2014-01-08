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

#include "server/cios/Reconnect.h"

#include "server/cios/Connection.h"

#include "server/block/Container.h"
#include "server/block/Io.h"
#include "server/block/IoNode.h"

#include "server/Server.h"

#include "common/logging.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace cios {

Reconnect::Ptr
Reconnect::create(
        const Server::Ptr& server
        )
{
    const boost::shared_ptr<Reconnect> result(
            new Reconnect( server )
            );

    server->getBlocks()->get(
            boost::bind(
                &Reconnect::blocksHandler,
                result,
                _1
                )
            );

    return result;
}

Reconnect::Reconnect(
        const Server::Ptr& server
        ) :
    _server( server )
{

}

void
Reconnect::blocksHandler(
        const block::Container::Blocks& blocks
        )
{
    const Message::Ptr jobctl_request(
            Message::create( bgcios::jobctl::Reconnect, 0 /* job ID */ )
            );
    const Message::Ptr stdio_request(
            Message::create( bgcios::stdio::Reconnect, 0 /* job ID */ )
            );

    // iterate through block container, send reconnect messages to all I/O links
    // when we find an I/O block
    BOOST_FOREACH( const auto& i, blocks ) {
        const block::Io::Ptr io = boost::dynamic_pointer_cast<block::Io>(i);
        if ( !io ) continue;
        LOGGING_DECLARE_BLOCK_MDC( io->name() );

        BOOST_FOREACH( const auto& j, io->getNodes() ) {
            LOGGING_DECLARE_LOCATION_MDC( j->getLocation() );
            LOG_DEBUG_MSG( "reconnecting" );
            const cios::Connection::Ptr control( j->getControl() );
            const cios::Connection::Ptr data( j->getData() );
            if ( control ) control->write( jobctl_request );
            if ( data ) data->write( stdio_request );
        }
    }
}

Reconnect::~Reconnect()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

} // cios
} // server
} // runjob
