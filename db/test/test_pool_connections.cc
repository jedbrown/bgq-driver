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

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE pool_connections_properties_good
#include <boost/test/unit_test.hpp>

#include "common.h"

#include "api/tableapi/DBConnectionPool.h"

#include <boost/bind.hpp>

struct InitializeLogging
{
    InitializeLogging()
    {
        bgq::utility::Properties::ConstPtr properties;
        properties = bgq::utility::Properties::create();
        bgq::utility::initializeLogging( bgq::utility::Properties() );
    }
};

bool is_invalid_minimum( const std::invalid_argument& e ) { return e.what() == std::string("size"); }
bool is_invalid_maximum( const std::invalid_argument& e ) { return e.what() == std::string("maximum"); }
bool is_invalid_properties( const std::invalid_argument& e ) { return e.what() == std::string("properties"); }

BOOST_GLOBAL_FIXTURE( InitializeLogging );

BOOST_AUTO_TEST_CASE( good_properties_custom_section )
{
    // test using a custom section for initializing the connection pool
    const std::string my_section( "my_section" );
    bgq::utility::Properties::ConstPtr properties;
    properties = bgq::utility::Properties::create( "good.properties" );
    BGQDB::DBConnectionPool::reset();
    BGQDB::DBConnectionPool::init( properties, my_section );
    BGQDB::DBConnectionPool& pool = BGQDB::DBConnectionPool::instance();
    BOOST_CHECK_EQUAL(
            boost::lexical_cast<unsigned>( properties->getValue(my_section, "connection_pool_size") ),
            pool.size()
            );
}

BOOST_AUTO_TEST_CASE( good_properties_explicit_size )
{
    // test using the default database section for initializing the connection pool
    bgq::utility::Properties::ConstPtr properties;
    properties = bgq::utility::Properties::create( "good.properties" );
    BGQDB::DBConnectionPool::reset();
    BGQDB::DBConnectionPool::init( properties, 1);
    BGQDB::DBConnectionPool& pool = BGQDB::DBConnectionPool::instance();
    BOOST_CHECK_EQUAL(
            1u,
            pool.size()
            );
}

BOOST_AUTO_TEST_CASE( garbage_properties_custom_section )
{
    // test using a custom section for initializing the connection pool with garbage (non-numerical) values
    // it should use the default values
    const std::string my_section( "my_section" );
    bgq::utility::Properties::ConstPtr properties;
    properties = bgq::utility::Properties::create( "garbage.properties" );
    BGQDB::DBConnectionPool::reset();
    BGQDB::DBConnectionPool::init( properties, my_section );
    BGQDB::DBConnectionPool& pool = BGQDB::DBConnectionPool::instance();
    BOOST_CHECK_EQUAL(
            BGQDB::DBConnectionPool::DefaultSize,
            pool.size()
            );
}

BOOST_AUTO_TEST_CASE( multiple_init )
{
    // ensure an application initializing the API twice only gets the values set the first time
    bgq::utility::Properties::ConstPtr properties;
    properties = bgq::utility::Properties::create( "good.properties" );
    BGQDB::DBConnectionPool::reset();
    BGQDB::DBConnectionPool::init( properties, 1 );
    BOOST_CHECK_EQUAL(
            1u,
            BGQDB::DBConnectionPool::instance().size()
            );

    // init again with different values
    BOOST_CHECK_THROW(
            BGQDB::DBConnectionPool::init( properties, 2 ),
            std::logic_error
            );
}

BOOST_AUTO_TEST_CASE( invalid_properties )
{
    // ensure if we init the API with a NULL properties pointer, it throws
    bgq::utility::Properties::ConstPtr properties;
    BGQDB::DBConnectionPool::reset();
    const std::string my_section( "my_section" );

    // use section name init
    BOOST_CHECK_EXCEPTION(
            BGQDB::DBConnectionPool::init( properties, my_section ),
            std::invalid_argument,
            is_invalid_properties
            );

    // use size init
    const unsigned size = 1;
    BOOST_CHECK_EXCEPTION(
            BGQDB::DBConnectionPool::init( properties, size ),
            std::invalid_argument,
            is_invalid_properties
            );
}

BOOST_AUTO_TEST_CASE( auto_commit )
{
    BGQDB::DBConnectionPool::reset();
    const bgq::utility::Properties::ConstPtr properties(
            bgq::utility::Properties::create( "good.properties" )
            );
    BGQDB::DBConnectionPool::init( properties, 1 );
    BOOST_CHECK_EQUAL( BGQDB::DBConnectionPool::instance().size(), 1u );

    // get a connection
    {
        const BGQDB::DBConnection::Ptr connection(
                BGQDB::DBConnectionPool::instance().checkout()
                );
        BOOST_CHECK( connection != NULL );

        // auto-commit should be enabled
        BOOST_CHECK_EQUAL( connection->isAutoCommit(), true );

        // disable it
        connection->setAutoCommit( false );
        BOOST_CHECK_EQUAL( connection->isAutoCommit(), false );
    }

    // get another connection
    {
        const BGQDB::DBConnection::Ptr connection(
                BGQDB::DBConnectionPool::instance().checkout()
                );
        BOOST_CHECK( connection != NULL );

        // auto-commit should be enabled
        BOOST_CHECK_EQUAL( connection->isAutoCommit(), true );
    }
}
