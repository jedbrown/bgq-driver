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
#include "portConfiguration/ServerPortConfiguration.h"

#include <boost/assign/std/vector.hpp> // for 'operator+=()'


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE PortConfiguration
#include <boost/test/unit_test.hpp>


using namespace boost::assign; // bring 'operator+=()' into scope

using bgq::utility::ClientPortConfiguration;
using bgq::utility::PortConfiguration;
using bgq::utility::Properties;
using bgq::utility::ServerPortConfiguration;


BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );


BOOST_AUTO_TEST_CASE( test_single_pair )
{
    // can create PortConfiguration w/default port, set pairs w/hostname and port, and get pack those pairs.
    ServerPortConfiguration port_config( 32061 );

    PortConfiguration::Pairs pairs_in;
    pairs_in.push_back( PortConfiguration::Pair( "bmw323", "55901" ) );

    port_config.setPairs( pairs_in );

    port_config.notifyComplete();

    BOOST_CHECK_EQUAL( port_config.getPairs(), pairs_in );
}


BOOST_AUTO_TEST_CASE( test_default_service_name )
{
    // Can create a ServerPortConfiguration with either a port number or service name.
    BOOST_CHECK_EQUAL( ServerPortConfiguration( 32061 ).getDefaultServiceName(), "32061" );
    BOOST_CHECK_EQUAL( ServerPortConfiguration( "32061" ).getDefaultServiceName(), "32061" );
}


BOOST_AUTO_TEST_CASE( test_single_string )
{
    // can configure with a string like "localhost:55901" and get that back as a pair.

    ServerPortConfiguration port_config( 32061 );

    port_config.setPorts( "localhost:55901" );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "localhost", "55901" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_default_port )
{
    // if configure with string like "localhost", get back ( "localhost", default_port ).

    ServerPortConfiguration port_config( 32061 );

    port_config.setPorts( "localhost" );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "localhost", "32061" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_string_ports_multi )
{
    // can configure with a string like "localhost:55901,[::1]:55902" and get back those pairs.

    ServerPortConfiguration port_config( 32061 );

    port_config.setPorts( "localhost:55901,[::1]:55902" );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "localhost", "55901" ), PortConfiguration::Pair( "::1", "55902" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_strings )
{
    // Can set ports with a vector of strings with single or multi, get back pairs with all those strings.

    ServerPortConfiguration port_config( 32061 );

    PortConfiguration::Strings ports_strs;
    ports_strs.push_back( "localhost:55901" );
    ports_strs.push_back( "bmw323.rchland.ibm.com:55902,[::1]" );

    port_config.setPorts( ports_strs );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs;
    exp_pairs +=
            PortConfiguration::Pair( "localhost", "55901" ),
            PortConfiguration::Pair( "bmw323.rchland.ibm.com", "55902" ),
            PortConfiguration::Pair( "::1", "32061" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_missing_section )
{
    // If the section is missing from the config file then should use the default, prints a warning (not testing printing a warning)

    ServerPortConfiguration port_config( 32061 );

    port_config.setProperties( Properties::create( "testPortTypeNameOptions.properties" ), "notasection" );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( ServerPortConfiguration::DefaultHostname, "32061" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_missing_property )
{
    // If the value is missing from the config file then should use the default, prints a warning (not testing printing a warning)

    ServerPortConfiguration port_config( 32061 );

    port_config.setProperties( Properties::create( "testPortTypeNameOptions.properties" ), "section2" );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( ServerPortConfiguration::DefaultHostname, "32061" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


#if 0 // I don't know how to generate this error.
BOOST_AUTO_TEST_CASE( test_invalid_port_str_single )
{
    // If pass an invalid string like "", get a PortConfiguration::InvalidPortStr exception.
    ServerPortConfiguration port_config( 32061 );

    port_config.setPorts( "[]:" );
    std::cerr << port_config.getPairs() << "\n";

    BOOST_CHECK_THROW( port_config.setPorts( "]" ), PortConfiguration::InvalidPortStr );
}
#endif
