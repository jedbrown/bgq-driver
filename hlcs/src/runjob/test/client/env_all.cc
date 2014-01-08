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
#define BOOST_TEST_MODULE env_all

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/test/unit_test.hpp>

#include <cstdlib>

#include <unistd.h>

using namespace runjob::client::options;

extern char** environ;

BOOST_FIXTURE_TEST_SUITE( env_all, Fixture )

BOOST_AUTO_TEST_CASE( arg_none )
{
    // get properties file path
    char* properties = getenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    BOOST_CHECK( properties != NULL );

    // unset all environment variables    
    char* env = environ[0];
    environ[0] = NULL;

    const char* argv[] = {"foo", "--env-all", "--properties", properties};
    Parser options(4, const_cast<char**>(argv));
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            0u
            );

    // restore
    environ[0] = env;
}

BOOST_AUTO_TEST_CASE( arg_all )
{
    // get properties file path
    char* properties = getenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    BOOST_CHECK( properties != NULL );

    // unset all environment variables    
    // set two environment variables
    environ[0] = const_cast<char*>("foo=bar");
    environ[1] = const_cast<char*>("bar=foo");
    environ[2] = NULL;

    const char* argv[] = {"foo", "--env-all", "--properties", properties};
    Parser options(4, const_cast<char**>(argv));
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            2u
            );
}

BOOST_AUTO_TEST_SUITE_END()

