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

#include <boost/lexical_cast.hpp>

#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include <boost/bind.hpp>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE ClientPortConfiguration
#include <boost/test/unit_test.hpp>


using namespace boost::assign; // bring 'operator+=()' into scope

namespace po = boost::program_options;

using bgq::utility::ClientPortConfiguration;
using bgq::utility::PortConfiguration;
using bgq::utility::Properties;

using boost::lexical_cast;

using std::string;


BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );


BOOST_AUTO_TEST_CASE( test_const )
{
    BOOST_CHECK_EQUAL( ClientPortConfiguration::DefaultHostname, "" );
    BOOST_CHECK_EQUAL( ClientPortConfiguration::OptionName, "host" );
    BOOST_CHECK_EQUAL( ClientPortConfiguration::PropertyName, "host" );
}


BOOST_AUTO_TEST_CASE( test_default )
{
    // can create a PortConfiguration w/default port and no hostnames and get pairs, will get :port.

    ClientPortConfiguration port_config( 32061 );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( ClientPortConfiguration::DefaultHostname, "32061" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_properties )
{
    // Can set the ports from a Properties. When configured for client, uses ClientPortConfiguration::PropertyName property in the section.

    ClientPortConfiguration port_config( 32062 );

    port_config.setProperties( Properties::create( "testPortTypeNameOptions.properties" ), "bgsched.realtime" /*section_name*/ );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "localhost", "32061" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}


BOOST_AUTO_TEST_CASE( test_command_line )
{
    // Can set the ports from the command line. When is client, allows --host

    ClientPortConfiguration port_config( 32062 );

    po::options_description desc( "Options" );

    port_config.addTo( desc );

    po::variables_map vm;

    PortConfiguration::Strings args;
    args.push_back( "--" + ClientPortConfiguration::OptionName );
    args.push_back( "[::1]" );

    po::store( po::command_line_parser( args ).options( desc ).run(), vm );
    po::notify( vm );

    port_config.notifyComplete();

    PortConfiguration::Pairs exp_pairs; exp_pairs +=  PortConfiguration::Pair( "::1", "32062" );

    BOOST_CHECK_EQUAL( port_config.getPairs(), exp_pairs );
}
