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
#define BOOST_TEST_MODULE max_user_processes
#include <boost/test/unit_test.hpp>

#include "common/defaults.h"
#include "common/properties.h"

#include "server/Options.h"

#include "test/make_argv.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/lexical_cast.hpp>

#include <sys/resource.h>

using namespace runjob::server;

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( garbage_properties )
{
    unsigned argc;
    char** argv = make_argv( "--properties garbage.properties --reconnect blocks --sim true", argc );
    Options options( argc, argv );

    struct rlimit limit;
    BOOST_CHECK_EQUAL(
            getrlimit( RLIMIT_NPROC, &limit),
            0
            );
    BOOST_CHECK_EQUAL(
            limit.rlim_cur,
            runjob::defaults::ServerMaxUserProcesses
            );
}

BOOST_AUTO_TEST_CASE( missing_properties )
{
    unsigned argc;
    char** argv = make_argv( "--properties missing.properties --reconnect blocks --sim true", argc );
    Options options( argc, argv );

    struct rlimit limit;
    BOOST_CHECK_EQUAL(
            getrlimit( RLIMIT_NPROC, &limit),
            0
            );
    BOOST_CHECK_EQUAL(
            limit.rlim_cur,
            runjob::defaults::ServerMaxUserProcesses
            );
}

BOOST_AUTO_TEST_CASE( good_properties )
{
    unsigned argc;
    char** argv = make_argv( "--properties good.properties --reconnect blocks --sim true", argc );
    Options options( argc, argv );

    struct rlimit limit;
    BOOST_CHECK_EQUAL(
            getrlimit( RLIMIT_NPROC, &limit),
            0
            );
    BOOST_CHECK_EQUAL(
            limit.rlim_cur,
            boost::lexical_cast< rlim_t >(
                options.getProperties()->getValue( PropertiesSection, "max_user_processes" )
                )
            );
}
