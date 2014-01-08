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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE find_a_node
#include <boost/test/unit_test.hpp>

#include "Configuration.h"
#include "Plugin.h"

#include "hlcs/src/bgsched/runjob/TerminatedImpl.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/genblock.h>

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include "hlcs/src/bgsched/runjob/VerifyImpl.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <cstdlib>

struct Fixture
{
public:
    /*!
     * \brief
     */
    Fixture()
    {
        // initialize logging and db
        {
            const bgq::utility::Properties::Ptr properties = bgq::utility::Properties::create();
            bgq::utility::initializeLogging( *properties );
            BGQDB::init( properties, 1 );

            // use configuration in our local directory
            const int overwrite = 1;
            setenv(
                    find_a_node::Configuration::EnvironmentVariable.c_str(),
                    "find_a_node.properties",
                    overwrite
                  );
        }

        {
            // create block
            const bgq::utility::Properties::Ptr properties = bgq::utility::Properties::create( "find_a_node.properties" );
            const std::string midplane = "R00-M0";
            const unsigned nodes = 32;
            const bgq::utility::UserId uid;
            (void)BGQDB::genSmallBlock(
                    properties->getValue( "block", "name"),
                    midplane,
                nodes,
                properties->getValue( "block", "nodeboard"),
                uid.getUser()
                );

            // initialize block
            const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
            std::ostringstream sql;
            sql << "UPDATE " << BGQDB::DBTBlock().getTableName() << " " <<
                    "SET " << BGQDB::DBTBlock::STATUS_COL << "='" << BGQDB::BLOCK_INITIALIZED << "' " <<
                    "WHERE " << BGQDB::DBTBlock::BLOCKID_COL << " = '" << properties->getValue( "block", "name" ) << "'"
                    ;
            connection->executeUpdate( sql.str() );
        }
    }
};

BOOST_GLOBAL_FIXTURE( Fixture );

BOOST_AUTO_TEST_CASE( block )
{
    find_a_node::Plugin foo;
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_pid = 123;
    bgsched::runjob::Verify data(impl);
    foo.execute( data );

    // ensure block name in configuration file is what Plugin thinks
    BOOST_CHECK_EQUAL(
            data.block(),
            foo.configuration().getValue( "block", "name" )
            );
}

BOOST_AUTO_TEST_CASE( oversubscribed )
{
    find_a_node::Plugin foo;

    // start three jobs
    for ( pid_t i = 1; i < 4; ++i ) {
        bgsched::runjob::Verify::Pimpl impl(
                new bgsched::runjob::Verify::Impl
                );
        impl->_pid = i;
        bgsched::runjob::Verify data(impl);
        foo.execute( data );
        BOOST_CHECK_EQUAL(
                data.deny_job(),
                false
                );

        BOOST_CHECK_EQUAL(
                data.corner().location().empty(),
                false
                );
        BOOST_CHECK_EQUAL(
                data.shape().value().empty(),
                false
                );
    }

    // fourth should fail
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_pid = 4;
    bgsched::runjob::Verify data( impl );

    foo.execute( data );
    BOOST_CHECK_EQUAL(
            data.deny_job(),
            true
            );

    // complete a previous job
    bgsched::runjob::Terminated::Pimpl terminated_impl(
            new bgsched::runjob::Terminated::Impl
            );
    terminated_impl->_pid = 3;
    foo.execute( bgsched::runjob::Terminated(terminated_impl) );

    // fourth job should now run
    foo.execute( data );
    BOOST_CHECK_EQUAL(
            data.deny_job(),
            false
            );
}

BOOST_AUTO_TEST_CASE( specific_block )
{
    find_a_node::Plugin foo;
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_pid = 123;
    impl->_block = "bas";
    bgsched::runjob::Verify data( impl );

    foo.execute( data );
    BOOST_CHECK_EQUAL(
            data.block(),
            "baz"
            );
    BOOST_CHECK_EQUAL(
            data.corner().location().empty(),
            true
            );
    BOOST_CHECK_EQUAL(
            data.shape().value().empty(),
            true
            );
}

BOOST_AUTO_TEST_CASE( specific_block_matching )
{
    find_a_node::Plugin foo;
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_pid = 123;
    impl->_block = "R00-M0-N00";
    bgsched::runjob::Verify data( impl );
    
    foo.execute( data );
    BOOST_CHECK_EQUAL(
            data.corner().location().empty(),
            false
            );
    BOOST_CHECK_EQUAL(
            data.shape().value().empty(),
            false
            );
}

BOOST_AUTO_TEST_CASE( kill_timeout )
{
    find_a_node::Plugin foo;

    // start three jobs
    for ( unsigned i = 1; i < 4; ++i ) {
        bgsched::runjob::Verify::Pimpl impl(
                new bgsched::runjob::Verify::Impl
                );
        impl->_pid = i;
        bgsched::runjob::Verify data( impl );
        foo.execute( data );
    }

    // one times out
    bgsched::runjob::Terminated::Pimpl terminated_impl(
            new bgsched::runjob::Terminated::Impl
            );

    terminated_impl->_pid = 1;
    terminated_impl->_killTimeout = true;
    foo.execute( bgsched::runjob::Terminated(terminated_impl) );

    // fourth job should not start
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_pid = 4;
    bgsched::runjob::Verify data( impl );
    foo.execute( data );
    BOOST_CHECK_EQUAL(
            data.deny_job(),
            true
            );
}

BOOST_AUTO_TEST_CASE( kill_timeout_with_reset )
{
    find_a_node::Plugin foo;

    // start three jobs
    for ( unsigned i = 1; i < 4; ++i ) {
        bgsched::runjob::Verify::Pimpl impl(
                new bgsched::runjob::Verify::Impl
                );
        impl->_pid = i;
        bgsched::runjob::Verify data( impl );
        foo.execute( data );
    }

    // one times out
    bgsched::runjob::Terminated::Pimpl terminated_impl(
            new bgsched::runjob::Terminated::Impl
            );
    terminated_impl->_pid = 1;
    terminated_impl->_killTimeout = true;
    foo.execute( bgsched::runjob::Terminated(terminated_impl) );

    // fourth job should not start
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_pid = 4;
    bgsched::runjob::Verify data( impl );
    foo.execute( data );
    BOOST_CHECK_EQUAL(
            data.deny_job(),
            true
            );

    // update block time stamp
    const bgq::utility::Properties::Ptr properties = bgq::utility::Properties::create( "find_a_node.properties" );
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    std::ostringstream sql;
    sql << "UPDATE " << BGQDB::DBTBlock().getTableName() << " " <<
        "SET " << BGQDB::DBTBlock::STATUS_COL << "='" << BGQDB::BLOCK_FREE << "' " <<
        "WHERE " << BGQDB::DBTBlock::BLOCKID_COL << " = '" << properties->getValue( "block", "name" ) << "'"
        ;
    uint64_t rows;
    connection->executeUpdate( sql.str(), &rows );
    BOOST_CHECK_EQUAL(
            rows,
            1u
            );

    sql.str("");
    sql << "UPDATE " << BGQDB::DBTBlock().getTableName() << " " <<
        "SET " << BGQDB::DBTBlock::STATUS_COL << "='" << BGQDB::BLOCK_INITIALIZED << "' " <<
        "WHERE " << BGQDB::DBTBlock::BLOCKID_COL << " = '" << properties->getValue( "block", "name" ) << "'"
        ;
    connection->executeUpdate( sql.str(), &rows );
    BOOST_CHECK_EQUAL(
            rows,
            1u
            );

    // fourt job should now run
    foo.execute( data );
    BOOST_CHECK_EQUAL(
            data.deny_job(),
            false
            );
}
