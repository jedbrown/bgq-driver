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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

/*
 *  Client applications were getting an error on connect,
 *  "Cannot assign requested address".
 *
 *  This program recreated the error.
 *
 *  The problem was fixed by removing the call to
 *  bind and not setting SO_REUSEADDR on connect.
 */


#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddrList.h>

#include <utility/include/portConfiguration/Acceptor.h>
#include <utility/include/portConfiguration/PortConfiguration.h>

#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <vector>


using std::cout;


LOG_DECLARE_FILE( "test" );


static bgq::utility::Properties::Ptr s_properties_ptr;

std::vector<CxxSockets::SecureTCPSocketPtr> s_sockets;


void
Connect(
        const bgq::utility::PortConfiguration::Pairs& portpairs,
        CxxSockets::SecureTCPSocketPtr& result
        )
{
    result.reset();

    if ( portpairs.empty() ) return;

    using namespace bgq::utility;

    BOOST_FOREACH( const PortConfiguration::Pair& portpair, portpairs) {
        if ( result )  break;

        try {
            CxxSockets::SockAddrList remote_list(AF_UNSPEC, portpair.first, portpair.second);
            BOOST_FOREACH( const CxxSockets::SockAddr& remote, remote_list ) {
                if ( result ) break;

                bool done = false;
                while ( !done ) {
                    try {
                        CxxSockets::SecureTCPSocketPtr sock(
                                new CxxSockets::SecureTCPSocket(remote.family(), 0)
                                );

                        ClientPortConfiguration port_config(
                                0,
                                ClientPortConfiguration::ConnectionType::Administrative
                                );
                        port_config.setProperties(s_properties_ptr, "");
                        port_config.notifyComplete();
                        sock->Connect(remote, port_config);
                        result = sock;
                        LOG_DEBUG_MSG(
                                "connected to " << remote.getHostAddr() << ":" << remote.getServicePort() <<
                                " successfully"
                                );
                        done = true;
                    } catch ( const CxxSockets::SoftError& e ) {
                        LOG_INFO_MSG( e.what() << ", trying again" );
                        cout << "Have " << s_sockets.size() << " clients!\n";
                        sleep(1);
                    } catch ( const CxxSockets::Error& e ) {
                        LOG_WARN_MSG(
                                "connecting to " << remote.getHostAddr() << ":" << remote.getServicePort() <<
                                " failed: " << e.what()
                                );
                        done = true;
                    }
                }
            }
        } catch ( const CxxSockets::Error& e ) {
            LOG_INFO_MSG(
                "connect attempt failed to " << portpair.first << ":" << portpair.second  <<
                " " << e.what()
                );
        }
    }

    if ( !result ) {
        cout << "Have " << s_sockets.size() << " clients!\n";
        throw std::runtime_error("unable to connect");
    }
}


class Server
{
public:

    Server(
            boost::asio::io_service& io_service
        ) :
            _io_service(io_service),
            _ready(false)
    {
        bgq::utility::ServerPortConfiguration server_port_configuration( "1207" );

        server_port_configuration.setProperties( s_properties_ptr, "" );

        server_port_configuration.notifyComplete();

        _acceptor_ptr.reset( new bgq::utility::Acceptor(
                _io_service,
                server_port_configuration
            ) );

    }


    void start()
    {
        _acceptor_ptr->start(
                boost::bind( &Server::_acceptHandler, this, _1 )
            );
    }


    void waitReady()
    {
        boost::unique_lock<boost::mutex> lock( _mtx );
        while ( ! _ready ) {
            _cond.wait( lock );
        }
    }


private:

    boost::asio::io_service &_io_service;

    boost::shared_ptr<bgq::utility::Acceptor> _acceptor_ptr;

    bool _ready;
    boost::mutex _mtx;
    boost::condition_variable _cond;


    void _acceptHandler(
            const bgq::utility::Acceptor::AcceptArguments& args
        )
    {
        if ( args.status == bgq::utility::Acceptor::Status::NowAccepting ) {
            boost::unique_lock<boost::mutex> lock( _mtx );
            _ready = true;
            _cond.notify_all();
        }
    }
};


boost::shared_ptr<Server> s_server_ptr;


int main( int argc, char *argv[] )
{
    cout << "Working...\n";

    s_properties_ptr = bgq::utility::Properties::create();

    bgq::utility::initializeLogging(
            *s_properties_ptr,
            bgq::utility::LoggingProgramOptions( "ibm.test" ),
            "default"
        );


    boost::asio::io_service io_service;

    s_server_ptr.reset( new Server( io_service ) );

    s_server_ptr->start();

    boost::thread thd( boost::bind( &boost::asio::io_service::run, &io_service ) );

    s_server_ptr->waitReady();

    while ( true ) {
        bgq::utility::PortConfiguration::Pairs portpairs;
        bgq::utility::PortConfiguration::parsePortsStr( "localhost:1207", "1207", portpairs );

        CxxSockets::SecureTCPSocketPtr sock_ptr;
        Connect( portpairs, sock_ptr );

        s_sockets.push_back( sock_ptr );
    }

    cout << "Done!\n";
}
