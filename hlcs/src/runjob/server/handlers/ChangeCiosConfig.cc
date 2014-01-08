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
#include "server/handlers/ChangeCiosConfig.h"

#include "common/logging.h"

#include "server/block/Compute.h"
#include "server/block/Io.h"
#include "server/block/IoNode.h"

#include "server/cios/Connection.h"
#include "server/cios/Message.h"

#include "server/Block.h"
#include "server/CommandConnection.h"
#include "server/Server.h"

#include <hlcs/include/runjob/commands/error.h>

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

ChangeCiosConfig::ChangeCiosConfig(
        const Server::Ptr& server
        ) :
    CommandHandler( server ),
    _response(),
    _connection()
{

}

void
ChangeCiosConfig::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    _request = boost::static_pointer_cast<runjob::commands::request::ChangeCiosConfig>( request );
    _response.reset(
            new runjob::commands::response::ChangeCiosConfig
            );
    _connection = connection;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getBlocks()->get(
            boost::bind(
                &ChangeCiosConfig::getBlocksHandler,
                shared_from_this(),
                _1
                )
            );
}

ChangeCiosConfig::~ChangeCiosConfig()
{
    if ( !_connection ) return;
    if ( !_response ) return;

    try {
        if ( _response->getError() ) {
            LOG_INFO_MSG( _response->getMessage() );
        }

        _connection->write( _response );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( ... ) {
        LOG_FATAL_MSG( "caught some other exception" );
    }
}

void
ChangeCiosConfig::getBlocksHandler(
        const block::Container::Blocks& blocks
        )
{
    if ( !_request->_block.empty() ) {
        const block::Container::Blocks::const_iterator result = std::find_if(
                blocks.begin(),
                blocks.end(),
                boost::bind(
                    std::equal_to<std::string>(),
                    _request->_block,
                    boost::bind(
                        &Block::name,
                        _1
                        )
                    )
                );
        if ( result == blocks.end() ) {
            _response->setMessage( "could not find block '" + _request->_block + "'" );
            _response->setError( runjob::commands::error::block_not_found );
            return;
        }

        this->impl( *result, blocks );

        return;
    }

    const Uci uci( _request->_location );

    block::IoNode::Ptr location;
    LOG_TRACE_MSG( blocks.size() << " blocks" );
    BOOST_FOREACH( const Block::Ptr& i, blocks ) {
        const block::Io::Ptr io = boost::dynamic_pointer_cast<block::Io>(i);
        if ( !io ) continue;

        BOOST_FOREACH( const block::IoNode::Ptr j, io->getNodes() ) {
            if ( j->getLocation() == uci ) {
                LOG_TRACE_MSG( "matched location " << j->getLocation() );
                location = j;
                break;
            }
        }

        if ( location ) break;
    }

    if ( !location ) {
        _response->setMessage( "could not find I/O node location '" + _request->_location + "'" );
        _response->setError( runjob::commands::error::block_not_found );
        return;
    }

    this->impl( location );
}

void
ChangeCiosConfig::impl(
        const Block::Ptr& block,
        const block::Container::Blocks& blocks
        )
{
    const block::Io::Ptr io = boost::dynamic_pointer_cast<block::Io>( block );
    const block::Compute::Ptr compute = boost::dynamic_pointer_cast<block::Compute>( block );

    // for I/O blocks it's easy, just use all the nodes
    if ( io ) {
        BOOST_FOREACH( const block::IoNode::Ptr i, io->getNodes() ) {
            this->impl( i );
        }

        return;
    } 
    
    if ( compute ) {
        // gather I/O nodes for this compute block
        block::Compute::Nodes io_nodes;
        BOOST_FOREACH( const auto& i, compute->io() ) {
            io_nodes.push_back( i.first );
        }

        // iterate through all I/O blocks
        BOOST_FOREACH( const Block::Ptr& i, blocks ) {
            const block::Io::Ptr io = boost::dynamic_pointer_cast<block::Io>(i);
            if ( !io ) continue;

            BOOST_FOREACH( const block::IoNode::Ptr j, io->getNodes() ) {
                const auto node = std::find(
                        io_nodes.begin(),
                        io_nodes.end(),
                        j->getLocation()
                        );
                if ( node == io_nodes.end() ) continue;

                LOG_TRACE_MSG( j->getLocation() );
                this->impl( j );
            }
        }

        return;
    }

    BOOST_ASSERT( !"shouldn't get here" );
}

void
ChangeCiosConfig::impl(
        const block::IoNode::Ptr& node
        )
{
    if ( !_request->_jobctl.empty() || !_request->_common.empty() ) {
        this->control( node );
    }
    if ( !_request->_stdio.empty() || !_request->_common.empty() ) {
        this->data( node );
    }
}

void
ChangeCiosConfig::control(
        const block::IoNode::Ptr& node
        )
{
    const BGQDB::job::Id job = 0;
    const cios::Message::Ptr msg = cios::Message::create(
            bgcios::jobctl::ChangeConfig,
            job
            );
    if ( !_request->_common.empty() ) {
        msg->as<bgcios::jobctl::ChangeConfigMessage>()->commonTraceLevel = _request->_common[0];
    }
    if ( !_request->_jobctl.empty() ) {
        msg->as<bgcios::jobctl::ChangeConfigMessage>()->jobctldTraceLevel = _request->_jobctl[0];
    }

    const cios::Connection::Ptr& control = node->getControl();
    if ( !control ) {
        LOG_WARN_MSG( "lost jobctld connection to " << node->getLocation() );
        return;
    }

    control->write( msg );
}

void
ChangeCiosConfig::data(
        const block::IoNode::Ptr& node
        )
{
    const BGQDB::job::Id job = 0;
    const cios::Message::Ptr msg = cios::Message::create(
            bgcios::stdio::ChangeConfig,
            job
            );
    if ( !_request->_common.empty() ) {
        msg->as<bgcios::stdio::ChangeConfigMessage>()->commonTraceLevel = _request->_common[0];
    }
    if ( !_request->_stdio.empty() ) {
        msg->as<bgcios::stdio::ChangeConfigMessage>()->stdiodTraceLevel = _request->_stdio[0];
    }

    const cios::Connection::Ptr& data = node->getData();
    if ( !data ) {
        LOG_WARN_MSG( "lost stdiod connection to " << node->getLocation() );
        return;
    }
    data->write( msg );
}

} // handlers
} // server
} // runjob
