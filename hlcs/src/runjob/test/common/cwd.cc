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
#define BOOST_TEST_MODULE cwd

#include <boost/test/unit_test.hpp>

#include "common/WorkingDir.h"

#include "test/common.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/lexical_cast.hpp>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

using namespace runjob;

BOOST_AUTO_TEST_CASE( path_too_long )
{
    std::string value( "/" );
    for (int32_t i = 0; i < bgcios::jobctl::MaxPathSize - 1; ++i) {
        value.append( boost::lexical_cast<std::string>(i % 10) );
    }

    WorkingDir cwd;
    std::istringstream is( value );
    is >> cwd;
    BOOST_CHECK_EQUAL( is.good(), false );
}

BOOST_AUTO_TEST_CASE( path_not_fully_qualified )
{
    const std::string value( "hello/world/my/path" );
    std::istringstream is( value );

    WorkingDir cwd;
    is >> cwd;

    BOOST_CHECK_EQUAL( is.good(), false );
}

BOOST_AUTO_TEST_CASE( fully_qualified )
{
    const std::string value( "/foo/bar/baz" );
    std::istringstream is( value );

    WorkingDir cwd;
    is >> cwd;

    BOOST_CHECK_EQUAL( is.good(), false );
}
