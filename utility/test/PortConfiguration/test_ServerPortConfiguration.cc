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

#include "portConfiguration/ServerPortConfiguration.h"

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <boost/assign/std/vector.hpp> // for 'operator+=()'

#include <iostream>
#include <string>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE ServerPortConfiguration
#include <boost/test/unit_test.hpp>


using namespace boost::assign; // bring 'operator+=()' into scope

namespace po = boost::program_options;


using boost::lexical_cast;

using bgq::utility::PortConfiguration;
using bgq::utility::Properties;
using bgq::utility::ServerPortConfiguration;

using std::ostream;
using std::string;


BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );


BOOST_AUTO_TEST_CASE( test_constants )
{
    BOOST_CHECK_EQUAL( ServerPortConfiguration::DefaultHostname, "localhost" );
    BOOST_CHECK_EQUAL( ServerPortConfiguration::OptionName, "listen-port" );
    BOOST_CHECK_EQUAL( ServerPortConfiguration::HiddenOptionName, "listen-ports" );
    BOOST_CHECK_EQUAL( ServerPortConfiguration::PropertyName, "listen_ports" );
}


BOOST_AUTO_TEST_CASE( test_default )
{
    // can create a PortConfiguration w/default port and no hostnames and get pairs, will get localhost:port.

    ServerPortConfiguration host_ports( 32061 );

    host_ports.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( ServerPortConfiguration::DefaultHostname, "32061" );

    BOOST_CHECK_EQUAL( host_ports.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_no_port_type )
{
    // If don't set type name or description on the constructor then get back empty strings from getTypeName and getTypeDescription.

    ServerPortConfiguration port_config( "32062" );

    po::options_description desc_hidden( "Hidden" );
    po::options_description desc_visible( "Visible" );

    port_config.addTo( desc_visible, desc_hidden );

    BOOST_CHECK_EQUAL( lexical_cast<string> ( desc_visible ), string() +

"Visible:\n"
"  --" + ServerPortConfiguration::OptionName + " arg     Ports to accept connections on\n"

            );

    port_config.setProperties( Properties::create( "testPortTypeNameOptions.properties" ), "testserver" );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "test1", "55903" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_properties )
{
    // Can set the ports from a Properties, uses ServerPortConfiguration::PropertyName property in the section.

    ServerPortConfiguration host_ports( 32062 );

    host_ports.setProperties( bgq::utility::Properties::create( "testPortTypeNameOptions.properties" ), "realtime.server" /*section_name*/ );

    host_ports.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "localhost", "32061" );

    BOOST_CHECK_EQUAL( host_ports.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_port_type_name_options )
{
    // Can provide a port type name and description, which then is prefixed to the option name and description in the description.

    ServerPortConfiguration port_config(
            "32062",
            "console",
            "real-time console"
        );

    po::options_description desc_hidden( "Hidden" );
    po::options_description desc_visible( "Visible" );

    port_config.addTo( desc_visible, desc_hidden );

    BOOST_CHECK_EQUAL( lexical_cast<string> ( desc_visible ), string() +

"Visible:\n"
"  --console-" + ServerPortConfiguration::OptionName + " arg Ports to accept real-time console connections on\n"

            );

    port_config.setProperties( Properties::create( "testPortTypeNameOptions.properties" ), "testserver" );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "::1", "55902" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_command_line )
{
    // Can set the ports from the command line using --<ServerPortConfiguration::OptionName>

    ServerPortConfiguration host_ports( 32062 );

    po::options_description desc_hidden( "Hidden" );
    po::options_description desc_visible( "Visible" );

    host_ports.addTo( desc_visible, desc_hidden );

    desc_hidden.add( desc_visible );

    po::variables_map vm;

    PortConfiguration::Strings args;
    args.push_back( string() + "--" + ServerPortConfiguration::OptionName );
    args.push_back( "[::1]:32061" );

    po::store( po::command_line_parser( args ).options( desc_hidden ).run(), vm );
    po::notify( vm );

    host_ports.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "::1", "32061" );

    BOOST_CHECK_EQUAL( host_ports.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_command_line_port_type_name )
{
    // Can set the ports from the command line, when set port type name and description, using --<name>-<ServerPortConfiguration::OptionName>

    ServerPortConfiguration host_ports( "32062", "console", "real-time console" );

    po::options_description desc_hidden( "Hidden" );
    po::options_description desc_visible( "Visible" );

    host_ports.addTo( desc_visible, desc_hidden );

    desc_hidden.add( desc_visible );

    po::variables_map vm;

    PortConfiguration::Strings args;
    args.push_back( "--console-" + ServerPortConfiguration::OptionName );
    args.push_back( "[::1]:32061" );

    po::store( po::command_line_parser( args ).options( desc_hidden ).run(), vm );
    po::notify( vm );

    host_ports.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "::1", "32061" );

    BOOST_CHECK_EQUAL( host_ports.getPairs(), exp_pairs );
}


#if 0 // Port this to use Acceptor.

BOOST_AUTO_TEST_CASE( test_get_acceptors )
{
    // Can create a host_ports, create a vector of listening acceptor pointers based on the configuration.

    ServerPortConfiguration host_ports( 32061 );

    host_ports.setPorts( "127.0.0.1:12347,127.0.0.1:12348" );

    host_ports.notifyComplete();

    boost::asio::io_service io_service;

    ServerPortConfiguration::Acceptors acceptors(host_ports.createAcceptors( io_service ));

    BOOST_CHECK_EQUAL( lexical_cast<string>( acceptors.at( 0 )->local_endpoint() ), "127.0.0.1:12347" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( acceptors.at( 1 )->local_endpoint() ), "127.0.0.1:12348" );
    BOOST_CHECK_EQUAL( acceptors.size(), 2 );
}


BOOST_AUTO_TEST_CASE( test_acceptors_no_interface_ex )
{
    // If createAcceptors and the host name is not valid then fails.

    ServerPortConfiguration host_ports( 32061 );

    host_ports.setPorts( "notahost12345.rchland.ibm.com" );

    host_ports.notifyComplete();

    boost::asio::io_service io_service;

    BOOST_CHECK_THROW( host_ports.createAcceptors( io_service ), std::runtime_error );
}

#endif
