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


#include "BgRealtimeServer.h"

#include "CommandClient.h"
#include "Configuration.h"
#include "DbChangesMonitor.h"
#include "TcpServer.h"

#include "db2/DbChangesMonitor.h"

#include <utility/include/Log.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include <stdexcept>
#include <vector>


using bgq::utility::Acceptor;

using boost::bind;
using boost::lexical_cast;
using boost::shared_ptr;

using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {


BgRealtimeServer::BgRealtimeServer(
        const std::string& application_name
    ) :
        _application_name(application_name),
        _status( _io_service )
{
    // Nothing to do.
}


void BgRealtimeServer::run(
        bgq::utility::ServerPortConfiguration& port_config,
        bgq::utility::ServerPortConfiguration& command_port_config
    )
{
    port_config.setProperties( _properties_ptr, Configuration::PROPERTIES_SECTION_NAME );
    port_config.notifyComplete();

    command_port_config.setProperties( _properties_ptr, Configuration::PROPERTIES_SECTION_NAME );
    command_port_config.notifyComplete();


    _config_ptr.reset( new Configuration( *_properties_ptr ) );

    boost::shared_ptr<DbChangesMonitor> db_changes_monitor_ptr( new db2::DbChangesMonitor (
            _io_service,
            *_config_ptr
        ));

    db_changes_monitor_ptr->setServerStatus( &_status );

    TcpServer tcp_server(
            _io_service,
            *db_changes_monitor_ptr,
            port_config,
            _status
        );

    Acceptor command_acceptor(
            _io_service,
            command_port_config
        );

    command_acceptor.start(
            bind(
                    &BgRealtimeServer::_handleAcceptCommandClient, this, _1
                )
        );

    LOG_INFO_MSG( "Starting " << _config_ptr->get_workers() << " workers" );

    boost::thread_group threads;

    for ( unsigned i(1) ; i < _config_ptr->get_workers() ; ++i ) {
        threads.create_thread( bind( &BgRealtimeServer::_runIoService, this ) );
    }

    _runIoService();

    threads.join_all();
}


void BgRealtimeServer::_runIoService()
{
    LOG_INFO_MSG( "Worker thread polling." );

    try {
        _io_service.run();
    } catch ( std::exception& e ) {
        LOG_ERROR_MSG( "Worker thread caught exception, " << e.what() );

        std::cerr << _application_name << ": error, " << e.what() << std::endl;
        exit(1);
    }

    LOG_INFO_MSG( "Worker thread exiting!" );
}


void BgRealtimeServer::_handleAcceptCommandClient(
        const bgq::utility::Acceptor::AcceptArguments& args
    )
{
    if ( args.status ) {
        if ( args.status == Acceptor::Status::NowAccepting ) {
            LOG_INFO_MSG( "Accepting command connections" );
            return;
        }

        LOG_ERROR_MSG( "Error on command acceptor, status=" << args.status );

        BOOST_THROW_EXCEPTION( std::runtime_error( "failed to resolve the real-time server command ports. Check the command_listen_ports setting in the [realtime.server] section in your Blue Gene configuration file (/bgsys/local/etc/bg.properties)." ) );
    }

    LOG_INFO_MSG( "Accepted command connection, user=" << args.user_id_ptr->getUser() << " type=" << args.user_type << " clientCn=" << (args.client_cn.empty() ? "None" : string() + "'" + args.client_cn + "'") );

    CommandClient::Ptr command_client_ptr( new CommandClient(
            args.socket_ptr,
            _status
        ) );

    command_client_ptr->start();
}


} } // namespace realtime::server
