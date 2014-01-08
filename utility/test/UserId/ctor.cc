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
#define BOOST_TEST_MODULE ctor
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/UserId.h"

#include <boost/foreach.hpp>

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

BOOST_AUTO_TEST_CASE( ctor )
{
    {
        // no argument to the ctor means get our current uid
        UserId uid;
        BOOST_CHECK_EQUAL( uid.getUid(), getuid() );
    }

    {
        const uid_t root = 0;
        UserId uid( root );
        BOOST_CHECK_EQUAL( uid.getUid(), root );
    }

    {
        // Create remote user
        UserId uid("remoteuser", true);
        BOOST_CHECK_EQUAL( uid.getUser(), "remoteuser" );
    }
}

BOOST_AUTO_TEST_CASE( serialize )
{
    // serialize into a buffer
    UserId uid;
    std::string string(
            uid.serialize()
            );
    std::vector<char> buf;
    BOOST_FOREACH( char c, string ) {
        buf.push_back( c );
    }
    BOOST_CHECK( !buf.empty() );

    // deserialize
    UserId other( buf );

    BOOST_CHECK( uid == other );
}

BOOST_AUTO_TEST_CASE( missing_username )
{
    BOOST_CHECK_THROW(
            UserId uid("doesnotexist"),
            std::runtime_error
            );
}

BOOST_AUTO_TEST_CASE( is_member)
{
    UserId uid;

    // hopefully this group does not exist
    BOOST_CHECK(
            uid.isMember( "hopefullyidonotexist" ) == false
            );
}

BOOST_AUTO_TEST_CASE( setters )
{
    UserId uid;

    uid.setUser("foobar");
    BOOST_CHECK_EQUAL( uid.getUser(), "foobar" );

    uid.setUid(123);
    BOOST_CHECK_EQUAL( uid.getUid(), 123u );

    UserId::GroupList groups;
    groups.push_back(
            UserId::GroupList::value_type( 456, "admin")
            );
    uid.setGroups( groups );
    BOOST_CHECK_EQUAL(
            uid.getGroups().front().first,
            groups.front().first
            );
    BOOST_CHECK_EQUAL(
            uid.getGroups().front().second,
            groups.front().second
            );
}


BOOST_AUTO_TEST_CASE( primary_gid)
{
    UserId uid;

    // primary gid should be at the front of the list
    BOOST_CHECK_EQUAL(
            uid.getGroups().front().first,
            getgid()
            );
}

