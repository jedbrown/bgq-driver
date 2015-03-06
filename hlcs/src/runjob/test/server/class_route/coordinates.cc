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
#define BOOST_TEST_MODULE coordinates
#include <boost/test/unit_test.hpp>

#include "common/JobInfo.h"

#include "server/job/class_route/Coordinates.h"

#include "test/server/args/Fixture.h"
#include "test/make_argv.h"

#include <spi/include/mu/Classroute.h>

namespace runjob {
namespace server {

BOOST_FIXTURE_TEST_SUITE( coordinates, Fixture )

BOOST_AUTO_TEST_CASE( equality )
{
    const job::class_route::Coordinates foo( 0, 0, 0, 0, 0 );
    BOOST_CHECK_EQUAL( foo, foo );
}

BOOST_AUTO_TEST_CASE( inequality )
{
    const job::class_route::Coordinates foo( 0, 0, 0, 0, 0 );
    const job::class_route::Coordinates bar( 1, 0, 0, 0, 0 );
    BOOST_CHECK( !(foo == bar) );
}

BOOST_AUTO_TEST_CASE( less_than )
{
    const job::class_route::Coordinates foo( 0, 0, 0, 0, 0 );
    const job::class_route::Coordinates bar( 1, 0, 0, 0, 0 );
    BOOST_CHECK( bar < foo );
    BOOST_CHECK( !(foo < bar) );
}


BOOST_AUTO_TEST_SUITE_END()

} // server
} // runjob

