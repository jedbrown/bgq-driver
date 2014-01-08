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
#define BOOST_TEST_MODULE singleton
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/Singleton.h"

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>

#include <vector>

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

struct Foo : public bgq::utility::Singleton<Foo>
{
    Foo() : _bar(0) { }
    unsigned _bar;
};

BOOST_AUTO_TEST_CASE( dismiss )
{
    // reset singleton
    Foo::reset();

    // get instance and increment its member
    {
        Foo& foo = Foo::instance();
        BOOST_CHECK_EQUAL( foo._bar, 0u );
        ++foo._bar;
    }

    // get instance again, member should be the same
    {
        Foo& foo = Foo::instance();
        BOOST_CHECK_EQUAL( foo._bar, 1u );
    }
}

BOOST_AUTO_TEST_CASE( reset )
{
    // reset singleton
    Foo::reset();

    // get instance and increment member
    {
        Foo& foo = Foo::instance();
        BOOST_CHECK_EQUAL( foo._bar, 0u );
        ++foo._bar;
        BOOST_CHECK_EQUAL( foo._bar, 1u );
    }

    // reset instance
    Foo::reset();

    // get instance and ensure member is initialized
    {
        Foo& foo = Foo::instance();
        BOOST_CHECK_EQUAL( foo._bar, 0u );
    }
}

