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
#define BOOST_TEST_MODULE scope_guard
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/ScopeGuard.h"

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

BOOST_AUTO_TEST_CASE( dismiss )
{
    typedef std::vector<unsigned> Vector;
    const unsigned value = 5;
    Vector foo = boost::assign::list_of( value );

    // create a scope guard inside of a scope block so it can trigger
    {
        ScopeGuard guard(
                boost::bind(
                    &Vector::clear,
                    boost::ref(foo)
                    )
                );

        // dismissing the guard should not cause it to fire
        guard.dismiss();
    }

    BOOST_CHECK_EQUAL( foo.size(), 1u );
    BOOST_CHECK_EQUAL( foo[0], value );
}

BOOST_AUTO_TEST_CASE( fire )
{
    typedef std::vector<unsigned> Vector;
    const unsigned value = 5;
    Vector foo = boost::assign::list_of( value );

    // create a scope guard inside of a scope block so it can trigger
    {
        ScopeGuard guard(
                boost::bind(
                    &Vector::clear,
                    boost::ref(foo)
                    )
                );

        // not dismissed, guard should be fired
    }

    BOOST_CHECK_EQUAL( foo.size(), 0u );
}

struct Foo
{
    Foo() { }
    ~Foo() { }
    void throws() const { throw std::logic_error( "hello world" ); }
};

BOOST_AUTO_TEST_CASE( trigger_throws )
{
    const Foo foo;
    BOOST_CHECK_THROW(
            foo.throws(),
            std::logic_error
            );

    // create a scope guard inside of a scope block so it can trigger
    {
        // a trigger that throws should be caught by the ScopeGuard,
        // there's nothing to check here other than std::terminate
        // isn't called from throwing an exception from within a destructor
        ScopeGuard guard(
                boost::bind(
                    &Foo::throws,
                    boost::cref( foo )
                    )
                );

        // not dismissed, guard should be fired
    }
}
