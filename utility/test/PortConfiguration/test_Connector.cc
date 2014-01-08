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


#include "common.hpp"

#include "portConfiguration/ClientPortConfiguration.h"
#include "portConfiguration/Connector.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Connector
#include <boost/test/unit_test.hpp>


using bgq::utility::ClientPortConfiguration;
using bgq::utility::Connector;
using bgq::utility::Properties;

using boost::lexical_cast;

using std::string;


BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

#if 0
BOOST_AUTO_TEST_CASE( test_connect )
{
    ClientPortConfiguration port_config( 22 );

    port_config.notifyComplete();

    boost::asio::io_service io_service;

    Connector connector(
            io_service,
            port_config,
            ClientSslConfiguration( Properties::create() )
        );

    Connector::SocketPtr socket_ptr(connector.connect());

    BOOST_REQUIRE( socket_ptr );
    BOOST_CHECK_EQUAL( lexical_cast<string>( socket_ptr->lowest_layer().remote_endpoint() ), "127.0.0.1:22" );
}


BOOST_AUTO_TEST_CASE( test_connect_no_throw )
{
    // Should be able to connect to the configured ports.

    ClientPortConfiguration port_config( 22 );

    port_config.notifyComplete();

    boost::asio::io_service io_service;

    Connector connector(
            io_service,
            port_config,
            ClientSslConfiguration( Properties::create() )
        );

    Connector::Error::Type error;

    Connector::SocketPtr socket_ptr(connector.connect( error ));

    BOOST_REQUIRE( socket_ptr );
    BOOST_CHECK_EQUAL( lexical_cast<string>( socket_ptr->lowest_layer().remote_endpoint() ), "127.0.0.1:22" );
    BOOST_CHECK_EQUAL( error, Connector::Error::Success );
}


void connect_callback(
        Connector::SocketPtr& socket_ptr,
        Connector::Error::Type& error,
        const string& error_str,
        Connector::SocketPtr& socket_ptr_out,
        Connector::Error::Type& error_out,
        string& error_str_out
    )
{
    std::cerr << "connect_callback!\n";

    socket_ptr_out = socket_ptr;
    error_out = error;
    error_str_out = error_str;
}


BOOST_AUTO_TEST_CASE( test_async_connect )
{
    // Should be able to connect to the configured ports.

    ClientPortConfiguration port_config( 22 );

    port_config.notifyComplete();

    boost::asio::io_service io_service;

    Connector connector(
            io_service,
            port_config,
            ClientSslConfiguration( Properties::create() )
        );

    Connector::SocketPtr socket_ptr;
    Connector::Error::Type error;
    string error_str;

    connector.async_connect(
            boost::bind(
                    &connect_callback,
                    _1, _2, _3,
                    boost::ref(socket_ptr),
                    boost::ref(error),
                    boost::ref(error_str)
                )
        );

    // run I/O service
    io_service.run();

    BOOST_CHECK_EQUAL( error, Connector::Error::Success );
    BOOST_REQUIRE( socket_ptr );
    BOOST_CHECK_EQUAL( lexical_cast<string>( socket_ptr->lowest_layer().remote_endpoint() ), "127.0.0.1:22" );
}

#else

BOOST_AUTO_TEST_CASE( test1 )
{
    BOOST_CHECK( true );
}

#endif
