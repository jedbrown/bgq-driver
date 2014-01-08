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


#include "TcpServer.h"

#include "log_util.h"

#include <boost/bind.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>


using boost::bind;

using boost::asio::ip::tcp;

using std::string;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {


TcpServer::TcpServer(
        boost::asio::io_service& io_service,
        DbChangesMonitor& db_changes_monitor,
        const bgq::utility::ServerPortConfiguration& port_config,
        Status& status
    ) :
        _acceptor(
                io_service,
                port_config
            ),
        _db_changes_monitor(db_changes_monitor),
        _status(status)
{
    _acceptor.start(
            bind( &TcpServer::_handleAccept, this, _1 )
        );
}


void TcpServer::_handleAccept(
        const bgq::utility::Acceptor::AcceptArguments& args
    )
{
    if ( ! args.socket_ptr ) {
        if ( args.status == bgq::utility::Acceptor::Status::NowAccepting ) {

            LOG_INFO_MSG( "Accepting real-time client connections" );

        } else if ( args.status == bgq::utility::Acceptor::Status::ResolveError ) {

            BOOST_THROW_EXCEPTION( std::runtime_error( "failed to resolve the real-time server listen ports. Check the listen_ports setting in the [realtime.server] section in your Blue Gene configuration file (/bgsys/local/etc/bg.properties)." ) );

            return;
        } else {
            LOG_WARN_MSG( "Acceptor error, " << args.status );
        }
        return;
    }

    LOG_INFO_MSG( "Accepted real-time client connection, user=" << args.user_id_ptr->getUser() << " type=" << args.user_type << " clientCn=" << (args.client_cn.empty() ? "None" : string() + "'" + args.client_cn + "'") );

    Client::Ptr client_ptr( new Client(
            args.socket_ptr,
            _db_changes_monitor,
            _status
        ) );

    client_ptr->start();
}


} } // namespace realtime::server
