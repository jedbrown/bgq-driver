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
#include "server/block/Io.h"

#include "common/defaults.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/properties.h"

#include "server/block/Container.h"
#include "server/block/IoLink.h"
#include "server/block/IoNode.h"

#include "server/database/IoNodeIpAddress.h"

#include "server/job/Container.h"

#include "server/sim/Inotify.h"
#include "server/sim/Iosd.h"

#include "server/cios/Connection.h"

#include "server/Options.h"
#include "server/Server.h"

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>
#include <control/include/bgqconfig/BGQIOBoardNodeConfig.h>

#include <boost/array.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

Io::Io(
        const std::string& name,
        const boost::shared_ptr<BGQBlockNodeConfig>& block,
        const Server::Ptr& server
        ) :
    Block( name, block, server ),
    _io_service( server->getIoService() ),
    _strand( _io_service ),
    _options( server->getOptions() ),
    _jobs( server->getJobs() ),
    _nodes(),
    _daemons(),
    _securityKey( block->securityKey(), name ),
    _iNotify(),
    _iosdName(),
    _iosdPath(),
    _initialized( false )
{

}

Io::Ptr
Io::create(
        const std::string& name,
        const boost::shared_ptr<BGQBlockNodeConfig>& block,
        const Server::Ptr& server
        )
{
    const Io::Ptr result(
            new Io(
                name,
                block,
                server
                )
            );

    result->init();

    return result;
}

void
Io::initialized()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( _initialized ) {
        LOG_TRACE_MSG( "already initialized" );
        return;
    }
    _initialized = true;

    const database::IoNodeIpAddress ipAddresses( *_config );
    
    const sim::Inotify::Ptr iNotify = this->setupSimulation();

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // iterate through I/O boards
    for (
            BGQBlockNodeConfig::ioboard_iterator board = _config->ioboardBegin();
            board != _config->ioboardEnd();
            ++board
        )
    {
        BOOST_FOREACH( const BGQIONodePos& io, *board->computes() ) {
            const std::string location(
                    board->posInMachine() +
                    "-" +
                    boost::lexical_cast<std::string>( io )
                    );
            LOGGING_DECLARE_LOCATION_MDC( location );

            const BGQNodeConfig* const config = board->nodeConfig( io );
            const bool ciosEnabled = config->_personality.Kernel_Config.NodeConfig & PERS_ENABLE_IOServices;
            if ( !ciosEnabled ) {
                LOG_INFO_MSG( "I/O services disabled" );
                continue;
            }

            const cios::Connection::Ptr control(
                    new cios::Connection(
                        bgcios::JobctlService,
                        location,
                        server,
                        shared_from_this()
                        )
                    );

            const cios::Connection::Ptr data(
                    new cios::Connection(
                        bgcios::StdioService,
                        location,
                        server,
                        shared_from_this()
                        )
                    );

            const Nodes::iterator node = std::find_if(
                    _nodes.begin(),
                    _nodes.end(),
                    boost::bind(
                        std::equal_to<Uci>(),
                        boost::bind(
                            &IoNode::getLocation,
                            _1
                            ),
                        location
                        )
                    );
            if ( node == _nodes.end() ) {
                LOG_WARN_MSG( "could not find node" );
                return;
            }
            (*node)->initialized( control, data );

            if ( _options.getSim() ) {
                const sim::Iosd::Ptr iosd(
                        new sim::Iosd(
                            iNotify,
                            _options,
                            location,
                            _iosdPath,
                            _iosdName
                            )
                        );

                iosd->start( control, data );
                _daemons.push_back( iosd );
            } else {
                this->connect(
                        ipAddresses,
                        location,
                        control,
                        data
                        );
            }
        }
    }

    if ( iNotify ) {
        iNotify->start();
    }
}

void
Io::terminating()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
    BOOST_FOREACH( const auto& i, _nodes ) {
        if ( cios::Connection::Ptr control = i->getControl() ) {
            control->stop();
        }
        if ( cios::Connection::Ptr data = i->getData() ) {
            data->stop();
        }
    }
}

void
Io::init()
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // iterate through I/O boards
    for (
            BGQBlockNodeConfig::ioboard_iterator board = _config->ioboardBegin();
            board != _config->ioboardEnd();
            ++board
        )
    {
        BOOST_FOREACH( const BGQIONodePos& io, *board->computes() ) {
            this->handleNode( *board, io );
        }
    }
}

Io::~Io()
{
    LOG_TRACE_MSG( "terminating" );

    if ( sim::Inotify::Ptr iNotify = _iNotify.lock() ){
        iNotify->stop();
    }

    // stop I/O connections
    BOOST_FOREACH( const IoNode::Ptr& i, _nodes ) {
        if ( i->getControl() ) {
            i->getControl()->stop();
        }
        if ( i->getData() ) {
            i->getData()->stop();
        }
    }
}

void
Io::handleNode(
        BGQIOBoardNodeConfig& board,
        const BGQIONodePos& io
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const std::string location(
            board.posInMachine() +
            "-" +
            boost::lexical_cast<std::string>( io )
            );

    LOGGING_DECLARE_LOCATION_MDC( location );

    const BGQNodeConfig* const config = board.nodeConfig( io );
    const bool ciosEnabled = config->_personality.Kernel_Config.NodeConfig & PERS_ENABLE_IOServices;
    if ( !ciosEnabled ) {
        LOG_DEBUG_MSG( "I/O services disabled" );
        return;
    }

    _nodes.push_back(
            IoNode::Ptr(
                new IoNode(
                    _name,
                    location
                    )
                )
            );
}

void
Io::connect(
        const database::IoNodeIpAddress& ip,
        const std::string& location,
        const cios::Connection::Ptr& control,
        const cios::Connection::Ptr& data
        )
{
    uint16_t jobctld_port = defaults::ServerJobctlPort;
    uint16_t stdiod_port = defaults::ServerStdioPort;

    try {
        jobctld_port = boost::lexical_cast<uint16_t>(
                _options.getProperties()->getValue( "cios.jobctld", "listen_port" )
                );
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    try {
        stdiod_port = boost::lexical_cast<uint16_t>(
                _options.getProperties()->getValue( "cios.stdiod", "listen_port" )
                );
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    try {
        const boost::asio::ip::address& address = ip.get( location );
        control->start(
                boost::asio::ip::tcp::endpoint(
                    address, 
                    jobctld_port
                    )
                );

        data->start(
                boost::asio::ip::tcp::endpoint(
                    address,
                    stdiod_port
                    )
                );
    } catch ( const runjob::Exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

sim::Inotify::Ptr
Io::setupSimulation()
{
    sim::Inotify::Ptr result;
    if ( !_options.getSim() ) return result;

    try {
        _iosdPath = _options.getProperties()->getValue(PropertiesSection, "iosd_sim_path");
        _iosdName = _options.getProperties()->getValue(PropertiesSection, "iosd_sim_name");
    } catch ( const std::invalid_argument& e ) {
        throw std::runtime_error( e.what() );
    }
    
    result.reset(
            new sim::Inotify(
                _io_service,
                _name
                )
            );
    _iNotify = result;

    return result;
}

} // block
} // server
} // runjob
