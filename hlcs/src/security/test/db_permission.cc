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
#define BOOST_TEST_MODULE database_permission
#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/ScopeGuard.h>
#include <utility/include/UserId.h>

#include <hlcs/include/security/Enforcer.h>
#include <hlcs/include/security/exception.h>
#include <hlcs/include/security/privileges.h>

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <db/include/api/tableapi/gensrc/DBTBlocksecurity.h>
#include <db/include/api/tableapi/gensrc/DBTJobsecurity.h>

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
        _enforcer( new Enforcer( bgq::utility::Properties::create()) ),
        _object(hlcs::security::Object::Invalid),
        _action(hlcs::security::Action::Invalid),
        _name("123"),
        _user()
    {
        _enforcer.reset( new Enforcer( bgq::utility::Properties::create()) );
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
    boost::scoped_ptr<hlcs::security::Enforcer> _enforcer;
    hlcs::security::Object::Type _object;
    hlcs::security::Action::Type _action;
    std::string _name;
    bgq::utility::UserId _user;
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );
BOOST_FIXTURE_TEST_SUITE( database_permission, Fixture )

BOOST_AUTO_TEST_CASE( grant_revoke_hardware )
{
    // granting or revoking any action on Hardware objects should throw
    _action = Action::Create;
    while ( _action != Action::Invalid ) {
        if ( _action == Action::Read || _action == Action::Execute ) {
            BOOST_CHECK_THROW(
                    grant(
                        Object( Object::Hardware, "foobar" ),
                        Authority( _user.getUser(), _action ),
                        _user
                        ),
                    std::invalid_argument
                    );

            BOOST_CHECK_THROW(
                    revoke(
                        Object( Object::Hardware, "foobar" ),
                        Authority( _user.getUser(), _action ),
                        _user
                        ),
                    std::invalid_argument
                    );
        }

        // get next action
        _action = static_cast<Action::Type>( static_cast<uint32_t>(_action) + 1 );
    }
}

BOOST_AUTO_TEST_CASE( invalid_action )
{
    for (unsigned int i = 0; i < 2; ++i) {
        if (i == 0) {
            _object = Object::Job;
        } else {
            _object = Object::Block;
        }

        // invalid actions should fail

        BOOST_CHECK_THROW(
                grant(
                    Object( _object, _name ),
                    Authority( _user.getUser(), Action::Invalid ),
                    _user
                    ),
                std::logic_error
                );
        
        BOOST_CHECK_THROW(
                revoke(
                    Object( _object, _name ),
                    Authority( _user.getUser(), Action::Invalid ),
                    _user
                    ),
                std::logic_error
                );
    }
}

BOOST_AUTO_TEST_CASE( invalid_object )
{
    // iterate through actions
    _action = Action::Create;
    while (_action != Action::Invalid) {
        // invalid objects should throw

        BOOST_CHECK_THROW(
                revoke(
                    Object( Object::Invalid, _name ),
                    Authority( _user.getUser(), _action ),
                    _user
                    ),
                std::invalid_argument
                );

        BOOST_CHECK_THROW(
                grant(
                    Object( Object::Invalid, _name ),
                    Authority( _user.getUser(), _action ),
                    _user
                    ),
                std::invalid_argument
                );

        // get next action
        _action = static_cast<Action::Type>( static_cast<uint32_t>(_action) + 1 );
    }
}

BOOST_AUTO_TEST_CASE( garbage_job_id )
{
    // job IDs should be convertible to integers
    _name = "123abcd";
    _object = Object::Job;
    _action = Action::Read;
    BOOST_CHECK_THROW(
            grant(
                Object( _object, _name ),
                Authority( _user.getUser(), _action ),
                _user
                ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( job_not_found )
{
    _name = "12345";
    _object = Object::Job;
    _action = Action::Read;
    BOOST_CHECK_THROW(
            grant(
                Object( _object, _name ),
                Authority( _user.getUser(), _action ),
                _user
                ),
            exception::ObjectNotFound
            );
}

BOOST_AUTO_TEST_CASE( block_id_too_long )
{
    BGQDB::DBTBlock dbo;
    _name = std::string(sizeof(dbo._blockid), 'a');
    _object = Object::Block;
    _action = Action::Read;
    BOOST_CHECK_THROW(
            grant(
                Object( _object, _name ),
                Authority( _user.getUser(), _action ),
                _user
                ),
            std::range_error
            );
}

BOOST_AUTO_TEST_CASE( user_id_too_long )
{
    BGQDB::DBTBlocksecurity dbo;
    std::string user(sizeof(dbo._authid), 'a');
    _object = Object::Block;
    _action = Action::Read;
    BOOST_CHECK_THROW(
            grant(
                Object( _object, _name ),
                Authority( user, _action ),
                _user
                ),
            std::range_error
            );
}

BOOST_AUTO_TEST_CASE( add_block_user )
{
    // add a block
    cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    BOOST_CHECK( connection );
    const std::string block( "SECURITY_TEST_BLOCK" );
    connection->executeUpdate(
            "INSERT INTO TBGQBlock ( blockId, owner, numCnodes, nodeConfig, status ) VALUES ( '" + block + "', 'owner', 512, 'CNKDefault', 'I' )"
            );
    bgq::utility::ScopeGuard blockGuard( 
            boost::bind(
                &removeBlock,
                connection,
                block
                )
            );

    _object = Object::Block;
    _action = Action::Read;
    BOOST_CHECK_NO_THROW(
            grant(
                Object( _object, block ),
                Authority( _user.getUser(), _action ),
                _user
                )
            );
}

BOOST_AUTO_TEST_CASE( add_job_user )
{
    cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    BOOST_CHECK( connection );

    // add a block
    const std::string block( "SECURITY_TEST_BLOCK" );
    connection->executeUpdate(
            "INSERT INTO TBGQBlock ( blockId, owner, numCnodes, nodeConfig, status ) VALUES ( '" + block + "', 'owner', 512, 'CNKDefault', 'I' )"
            );
    bgq::utility::ScopeGuard blockGuard( 
            boost::bind(
                &removeBlock,
                connection,
                block
                )
            );

    _object = Object::Block;
    _action = Action::Read;

    const uint64_t job_id = insertJob( block );
    bgq::utility::ScopeGuard jobGuard(
            boost::bind(
                &removeJob,
                connection,
                job_id
                )
            );
    
    {
        // ensure job owner can do any action
        bgq::utility::UserId uid;
        uid.setUser( _user.getUser() );
        uid.setGroups( bgq::utility::UserId::GroupList() );
        BOOST_CHECK_EQUAL(
                _enforcer->validate(
                    Object( Object::Job, boost::lexical_cast<std::string>(job_id) ),
                    Action::Read,
                    uid
                    ),
                true
                );
    }


    {
        // ensure sally cannot read
        bgq::utility::UserId uid;
        uid.setUser( "sally" );
        uid.setGroups( bgq::utility::UserId::GroupList() );
        
        BOOST_CHECK_EQUAL(
                _enforcer->validate(
                    Object( Object::Job, boost::lexical_cast<std::string>(job_id) ),
                    Action::Read,
                    uid
                    ),
                false
                );

        // grant sally read authority
        BOOST_CHECK_NO_THROW(
                grant(
                    Object( Object::Job, boost::lexical_cast<std::string>(job_id) ),
                    Authority( uid.getUser(), Action::Read ),
                    _user
                    )
                );

        // ensure sally can read
        BOOST_CHECK_EQUAL(
                _enforcer->validate(
                    Object( Object::Job, boost::lexical_cast<std::string>(job_id) ),
                    Action::Read,
                    uid
                    ),
                true
                );

        // revoke sally's read authority
        BOOST_CHECK_NO_THROW(
                revoke(
                    Object( Object::Job, boost::lexical_cast<std::string>(job_id) ),
                    Authority( uid.getUser(), Action::Read ),
                    _user
                    )
                );
    }
}

BOOST_AUTO_TEST_CASE( add_duplicate_block_user )
{
    // add a block
    cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    BOOST_CHECK( connection );
    const std::string block( "SECURITY_TEST_BLOCK" );
    connection->executeUpdate(
            "INSERT INTO TBGQBlock ( blockId, owner, numCnodes, nodeConfig, status ) VALUES ( '" + block + "', 'owner', 512, 'CNKDefault', 'I' )"
            );
    bgq::utility::ScopeGuard blockGuard( 
            boost::bind(
                &removeBlock,
                connection,
                block
                )
            );

    // adding authority for the first time should succeed
    _object = Object::Block;
    _action = Action::Read;
    BOOST_CHECK_NO_THROW(
            grant(
                Object( _object, block ),
                Authority( _user.getUser(), _action ),
                _user
                )
            );

    // adding authority for the second time should fail
    _object = Object::Block;
    _action = Action::Read;
    BOOST_CHECK_THROW(
            grant(
                Object( _object, block ),
                Authority( _user.getUser(), _action ),
                _user
                ),
            exception::DatabaseError
            );
}

BOOST_AUTO_TEST_CASE( grant_bad_job_id )
{
    BOOST_CHECK_THROW(
            grant(
                Object( Object::Job, "notajobid" ),
                Authority( _user.getUser(), Action::Read ),
                _user
                ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( revoke_bad_job_id )
{
    BOOST_CHECK_THROW(
            revoke(
                Object( Object::Job, "notajobid" ),
                Authority( _user.getUser(), Action::Read ),
                _user
                ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( hardware_validate_not_in_database )
{
    BOOST_CHECK_NO_THROW(
            _enforcer->validate(
                Object( Object::Hardware, "foobar" ),
                Action::Read,
                _user
                )
            );
}

BOOST_AUTO_TEST_CASE( object_does_not_exist_in_database )
{
    bgq::utility::UserId uid;
    uid.setUser( "sally" );
    uid.setGroups( bgq::utility::UserId::GroupList() );

    BOOST_CHECK_THROW(
            _enforcer->validate(
                Object( Object::Block, "notablock" ),
                Action::Read,
                uid
                ),
            exception::ObjectNotFound
            );

    BOOST_CHECK_THROW(
            _enforcer->validate(
                Object( Object::Job, "123" ),
                Action::Read,
                uid
                ),
            exception::ObjectNotFound
            );
}

BOOST_AUTO_TEST_SUITE_END()

} // security
} // hlcs
