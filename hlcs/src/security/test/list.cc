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
#define BOOST_TEST_MODULE list_test
#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/ScopeGuard.h>
#include <utility/include/UserId.h>

#include <hlcs/include/security/Enforcer.h>
#include <hlcs/include/security/exception.h>
#include <hlcs/include/security/privileges.h>

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/job/Operations.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/genblock.h>

#include <boost/test/unit_test.hpp>
#include <boost/scoped_ptr.hpp>

#include <iostream>

#include "Fixture.h"

void
removeBlock( 
        const cxxdb::ConnectionPtr& connection,
        const std::string& block
        )
{
    connection->executeUpdate(
            "UPDATE TBGQBlock set status='F' where blockid='" + block + "'"
            );
    connection->executeUpdate(
            "DELETE FROM TBGQBlock WHERE blockid='" + block + "'"
            );
}

void
removeJob( 
        const cxxdb::ConnectionPtr& connection,
        uint64_t job
        )
{
    connection->executeUpdate(
            "DELETE FROM TBGQJob WHERE id=" + boost::lexical_cast<std::string>(job)
            );
}

namespace hlcs {
namespace security {

struct Fixture
{
    Fixture() :
        _jobOperations(),
        _user(),
        _properties( "security.properties" )
    {

    }

    uint64_t insertJob( 
            const std::string& block
            )
    {
        BGQDB::job::InsertInfo info;
        info.setBlock( block );
        info.setExe( "/usr/bin/true" );
        info.setMapping( "ABCDE" );
        info.setUserName( _user.getUser() );
        info.setHostname( "localhost" );
        info.setPid( getpid() );
        info.setProcessesPerNode( 1 );

        uint64_t result;
        _jobOperations.insert( info, &result );
        return result;
    }

    BGQDB::job::Operations _jobOperations;
    bgq::utility::UserId _user;
    const bgq::utility::Properties _properties;
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );
BOOST_FIXTURE_TEST_SUITE( list_test, Fixture )

BOOST_AUTO_TEST_CASE( list_hardware_authority )
{
    hlcs::security::list( _properties, Object(Object::Hardware, "bogus") );
}

BOOST_AUTO_TEST_CASE( list_block_authority )
{
    // add a block
    cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    BOOST_CHECK( connection );
    const std::string block( "SECURITY_TEST_BLOCK" );
    const std::string owner( "nobody" );
    connection->executeUpdate(
            "INSERT INTO TBGQBlock ( blockId, owner, numCnodes, nodeConfig, status ) VALUES ( '" + block + "', '" + owner + "', 512, 'CNKDefault', 'I' )"
            );

    const Object object( Object::Block, block );

    {
        bgq::utility::ScopeGuard blockGuard( 
                boost::bind(
                    &removeBlock,
                    connection,
                    block
                    )
                );

        // adding authority for the first time should succeed
        BOOST_CHECK_NO_THROW(
                grant(
                    object,
                    Authority( _user.getUser(), Action::Read ),
                    _user
                    )
                );

        // get container of authorities
        const Authorities authorities(
                hlcs::security::list( _properties, object )
                );


        BOOST_CHECK_EQUAL( owner, authorities.getOwner() );

        // 7 authorities from properties file, one from granted database
        BOOST_CHECK_EQUAL( authorities.get().size(), 7u );

        // root should have create authority
        Authority::Container::const_iterator i = authorities.get().begin();
        while ( i != authorities.get().end() ) {
            if ( i->user() == "root" ) {
                break;
            }
            ++i;
        }

        BOOST_CHECK(
                i != authorities.get().end()
                );
        BOOST_CHECK_EQUAL(
                i->action(),
                hlcs::security::Action::Create
                );
    }

    // object should not exist
    BOOST_CHECK_THROW(
            hlcs::security::list( _properties, object ),
            hlcs::security::exception::ObjectNotFound
            );
}

BOOST_AUTO_TEST_CASE( list_authority_does_not_exist )
{
    cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    BOOST_CHECK( connection );

    // remove job to ensure it does not exist
    const uint64_t job_id( 123 );
    removeJob( connection, job_id );
    BOOST_CHECK_THROW(
            hlcs::security::list( _properties, Object(Object::Job, boost::lexical_cast<std::string>(job_id)) ),
            hlcs::security::exception::ObjectNotFound
            );
   
    // remove block to ensure it does not exists
    const std::string block_id( "security_list_block_authority" );
    removeBlock( connection, block_id );
    BOOST_CHECK_THROW(
            hlcs::security::list( _properties, Object(Object::Block, block_id) ),
            hlcs::security::exception::ObjectNotFound
            );
}

BOOST_AUTO_TEST_CASE( list_authority_bad_job_id ) {
    // non-numeric job ID should throw
    BOOST_CHECK_THROW(
            hlcs::security::list( _properties, Object(Object::Job, "abcd") ),
            exception::ObjectNotFound
            );
}

BOOST_AUTO_TEST_CASE( list_job_authority )
{
    // add a block
    cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    BOOST_CHECK( connection );
    const std::string block( "SECURITY_TEST_BLOCK" );
    const std::string owner( "nobody" );
    connection->executeUpdate(
            "INSERT INTO TBGQBlock ( blockId, owner, numCnodes, nodeConfig, status ) VALUES ( '" + block + "', '" + owner + "', 512, 'CNKDefault', 'I' )"
            );
    bgq::utility::ScopeGuard blockGuard( 
            boost::bind(
                &removeBlock,
                connection,
                block
                )
            );

    // add a bogus job to the job table
    const uint64_t job_id = insertJob( block );
    {
        bgq::utility::ScopeGuard jobGuard(
                boost::bind(
                    &removeJob,
                    connection,
                    job_id
                    )
                );

        // adding authority for the first time should succeed
        BOOST_CHECK_NO_THROW(
                grant(
                    Object( Object::Job, boost::lexical_cast<std::string>(job_id) ),
                    Authority( _user.getUser(), Action::Read ),
                    _user
                    )
                );

        // get container of authorities
        const Authorities authorities(
                hlcs::security::list( _properties, Object(Object::Job, boost::lexical_cast<std::string>(job_id)) )
                );

        BOOST_CHECK_EQUAL(
                authorities.getOwner(),
                _user.getUser()
                );

        // should expect one authority from properties, and one from database
        BOOST_CHECK_EQUAL(
                authorities.get().size(),
                2u
                );
    }

    // object should not exist
    BOOST_CHECK_THROW(
            hlcs::security::list( _properties, Object(Object::Job, boost::lexical_cast<std::string>(job_id)) ),
            hlcs::security::exception::ObjectNotFound
            );
}

BOOST_AUTO_TEST_SUITE_END()

} // security
} // hlcs
