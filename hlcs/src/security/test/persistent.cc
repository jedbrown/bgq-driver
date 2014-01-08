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
#define BOOST_TEST_MODULE persistent_database_connection

#include "Fixture.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <hlcs/include/security/Enforcer.h>
#include <hlcs/include/security/privileges.h>

#include <utility/include/ScopeGuard.h>

#include <boost/assign/list_of.hpp>

#include <boost/test/unit_test.hpp>

#include <boost/bind.hpp>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

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

BOOST_AUTO_TEST_CASE( foo )
{
    const cxxdb::ConnectionPtr connection = BGQDB::DBConnectionPool::instance().getConnection();
    BOOST_CHECK( connection );

    using namespace hlcs::security;
    using namespace bgq::utility;
    const Enforcer enforcer(
            Properties::create( "security.properties" ),
            connection
            );

    // add a block
    const std::string block( "SECURITY_TEST_BLOCK" );
    const std::string owner( "fred" );
    connection->executeUpdate(
            "INSERT INTO TBGQBlock ( blockId, owner, numCnodes, nodeConfig, status ) VALUES ( '" + block + "', '" + owner + "', 512, 'CNKDefault', 'I' )"
            );
    ScopeGuard blockGuard( 
            boost::bind(
                &removeBlock,
                connection,
                block
                )
            );

    // set uid to owner
    UserId uid;
    uid.setUser( owner );
    uid.setGroups( bgq::utility::UserId::GroupList() );

    // ensure owner can do any action on the block
    Action::Type action = Action::Create;
    while ( action != Action::Invalid ) {
        if ( action == Action::Create ) {
            // create is special, it doesn't look in the database so skip it
        } else {
            BOOST_CHECK_EQUAL(
                    enforcer.validate(
                        Object( Object::Block, block ),
                        action,
                        uid
                        ),
                    true
                    );
        }

        // get next action
        action = static_cast<Action::Type>( static_cast<uint32_t>(action) + 1 );
    }

    // grant authority to a secondary group
    const std::string group( "grantedgroup" );
    BOOST_CHECK_NO_THROW(
            grant(
                Object( Object::Block, block ),
                Authority( group, Action::Read ),
                uid
                )
            );

    // ensure uid in secondary group 'owner' can perform the granted action
    uid.setUser( "notowner" );
    uid.setGroups( boost::assign::list_of(bgq::utility::UserId::Group(123,group)) );
    BOOST_CHECK_EQUAL(
            enforcer.validate(
                Object( Object::Block, block ),
                Action::Read,
                uid
                ),
            true
            );

    // revoke previously granted authority
    BOOST_CHECK_NO_THROW(
            revoke(
                Object( Object::Block, block ),
                Authority( group, Action::Read ),
                uid
                )
            );

    // ensure uid in secondary group 'group' can perform any action since
    // they have all authority from security.properties
    uid.setGroups( boost::assign::list_of(bgq::utility::UserId::Group(123,"group")) );
    action = Action::Create;
    while ( action != Action::Invalid ) {
        if ( action == Action::Create ) {
            // create is special, it doesn't look in the database so skip it
        } else {
            BOOST_CHECK_EQUAL(
                    enforcer.validate(
                        Object( Object::Block, block ),
                        action,
                        uid
                        ),
                    true
                    );
        }

        // get next action
        action = static_cast<Action::Type>( static_cast<uint32_t>(action) + 1 );
    }
}
