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
#define BOOST_TEST_MODULE actions

#include <hlcs/include/security/Enforcer.h>

#include <utility/include/UserId.h>
#include <utility/include/Properties.h>

#include "Fixture.h"

#include <boost/test/unit_test.hpp>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture )

using namespace hlcs::security;

BOOST_AUTO_TEST_CASE( job )
{
    // jobs support read and execute authority
    Action::Type action = Action::Create;
    while ( action != Action::Invalid ) {
        if ( action == Action::Read || action == Action::Execute ) {
            BOOST_CHECK_NO_THROW(
                    Object( Object::Job, "123" ).validate( action )
                    );
        } else {
            BOOST_CHECK_THROW(
                    Object( Object::Job, "123" ).validate( action ),
                    std::logic_error
                    );
        }

        // get next action
        action = static_cast<Action::Type>( static_cast<uint32_t>(action) + 1 );
    }
}

BOOST_AUTO_TEST_CASE( hardware )
{
    // hardware supports read and execute authority
    Action::Type action = Action::Create;
    while ( action != Action::Invalid ) {
        if ( action == Action::Read || action == Action::Execute ) {
            BOOST_CHECK_NO_THROW(
                    Object( Object::Hardware, "R00-M0-N00" ).validate( action )
                    );
        } else {
            BOOST_CHECK_THROW(
                    Object( Object::Hardware, "R00-M0-N00" ).validate( action ),
                    std::logic_error
                );
        }

        // get next action
        action = static_cast<Action::Type>( static_cast<uint32_t>(action) + 1 );
    }
}

BOOST_AUTO_TEST_CASE( block )
{
    // all actions on blocks are allowed
    Action::Type action = Action::Create;
    while ( action != Action::Invalid ) {
        BOOST_CHECK_NO_THROW(
                    Object( Object::Block, "myblock" ).validate( action )
                );

        // get next action
        action = static_cast<Action::Type>( static_cast<uint32_t>(action) + 1 );
    }
}

BOOST_AUTO_TEST_CASE( create_block )
{
    // creating a block should fail for a user that does not have permission
    const bgq::utility::UserId uid( "nobody" );
    const Action::Type action = Action::Create;
    const std::string id ("bogusid");
    const Enforcer enforcer( bgq::utility::Properties::create("security.properties") );

    BOOST_CHECK_EQUAL(
            enforcer.validate(
                Object( Object::Block, id ),
                action,
                uid
                ),
            false
            );
}

BOOST_AUTO_TEST_CASE( create_authority_properties )
{
    // security.properties has
    // 
    // [security.blocks]
    // create=root
    //
    // so we expect root to have create authority but
    // nobody should not have it

    const bgq::utility::Properties::ConstPtr properties(
            bgq::utility::Properties::create("security.properties" )
            );
    const hlcs::security::Enforcer enforcer( properties );

    const std::string id ("bogusid");

    BOOST_CHECK_EQUAL(
            enforcer.validate(
                Object( Object::Block, id ),
                Action::Create,
                bgq::utility::UserId( "root" )
                ),
            true
            );

    BOOST_CHECK_EQUAL(
            enforcer.validate(
                Object( Object::Block, id ),
                Action::Create,
                bgq::utility::UserId( "nobody" )
                ),
            false
            );
}

BOOST_AUTO_TEST_CASE( all_authority_properties )
{
    // security.properties has
    // 
    // [security.blocks]
    // all=group
    //
    // so we expect any uid in group to have all authority but
    // others should not

    const bgq::utility::Properties::ConstPtr properties(
            bgq::utility::Properties::create("security.properties" )
            );
    const hlcs::security::Enforcer enforcer( properties );

    bgq::utility::UserId uid;
    uid.setUser( "sally" );
    uid.setGroups( bgq::utility::UserId::GroupList() );

    BOOST_CHECK_EQUAL(
            enforcer.validate(
                Object( Object::Block, "bogusobject" ),
                Action::Create,
                uid
                ),
            false
            );

    // if sally is in group, she should have authority
    bgq::utility::UserId::GroupList groups;
    groups.push_back( std::make_pair(123, "group") );
    uid.setGroups( groups );

    BOOST_CHECK_EQUAL(
            enforcer.validate(
                Object( Object::Block, "bogusobject" ),
                Action::Create,
                uid
                ),
            true
            );
}

BOOST_AUTO_TEST_CASE( bad_job_id )
{
    const Enforcer enforcer( bgq::utility::Properties::create("security.properties") );
    BOOST_CHECK_THROW(
            enforcer.validate(
                Object( Object::Job, "notajobid" ),
                Action::Read,
                bgq::utility::UserId()
                ),
            std::invalid_argument
            );
}

