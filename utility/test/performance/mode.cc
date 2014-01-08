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
#define BOOST_TEST_MODULE performance_mode
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/performance/Mode.h"

#include "utility/include/performance.h"

using namespace bgq::utility::performance;

class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        using namespace bgq::utility;
        Properties::Ptr properties = Properties::create();
        bgq::utility::initializeLogging( *properties );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( basic_properties_set )
{
    unsetenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    bgq::utility::Properties::Ptr props = bgq::utility::Properties::create( "basic.properties" );
    bgq::utility::performance::init( props );

    Mode::reset();
    Mode& mode = Mode::instance();
    BOOST_CHECK_EQUAL( mode.getMode(), Mode::Value::Basic );
}

BOOST_AUTO_TEST_CASE( extended_properties_set )
{
    unsetenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    bgq::utility::Properties::Ptr props = bgq::utility::Properties::create( "extended.properties" );
    bgq::utility::performance::init( props );

    Mode::reset();
    Mode& mode = Mode::instance();
    BOOST_CHECK_EQUAL( mode.getMode(), Mode::Value::Extended );
}

BOOST_AUTO_TEST_CASE( garbage_properties_set )
{
    unsetenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    bgq::utility::Properties::Ptr props = bgq::utility::Properties::create( "garbage.properties" );
    bgq::utility::performance::init( props );

    Mode::reset();
    Mode& mode = Mode::instance();
    BOOST_CHECK_EQUAL( mode.getMode(), Mode::DefaultMode );
}

BOOST_AUTO_TEST_CASE( none_properties_set )
{
    unsetenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    bgq::utility::Properties::Ptr props = bgq::utility::Properties::create( "none.properties" );
    bgq::utility::performance::init( props );

    Mode::reset();
    Mode& mode = Mode::instance();
    BOOST_CHECK_EQUAL( mode.getMode(), Mode::Value::None );
}

