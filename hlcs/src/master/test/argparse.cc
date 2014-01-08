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
#define BOOST_TEST_MODULE argparse
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "../common/ArgParse.h"

using namespace bgq::utility;

class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        using namespace bgq::utility;
        bgq::utility::initializeLogging( *Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

void usage() { }
void help() { }

BOOST_AUTO_TEST_CASE( verbose )
{
    // multiple --verbose arguments should work
    const char* argv[] = { "foo", "--verbose", "T", "--verbose", "ibm.foo=T" };
    std::vector<std::string> unused;
    BOOST_CHECK_NO_THROW(
            Args( 5, argv, &usage, &help, unused, unused );
            );
}

BOOST_AUTO_TEST_CASE( single_properties )
{
    // a single --properties is supported
    const char* argv[] = { "foo", "--properties", "good.properties" };
    std::vector<std::string> unused;
    BOOST_CHECK_NO_THROW(
            Args( 3, argv, &usage, &help, unused, unused );
            );
}

