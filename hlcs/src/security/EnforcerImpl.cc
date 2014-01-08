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
#include "EnforcerImpl.h"

#include "Ras.h"

#include <bgq_util/include/string_tokenizer.h>

#include <utility/include/Log.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace hlcs {
namespace security {

LOG_DECLARE_FILE( "security" );

Enforcer::Impl::Impl(
        bgq::utility::Properties::ConstPtr properties,
        const cxxdb::ConnectionPtr& database
        ) :
    _properties( properties ),
    _database()
{
    // ensure valid properties file
    if ( !_properties ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( "invalid properties arg" )
                );
    }

    // use persistent connection if one is provided
    if ( database ) {
        _database.reset( new db::Enforcer(database) );
    }

    LOG_DEBUG_MSG( "initialized with properties " << properties->getFilename() );
}

bool
Enforcer::Impl::validate(
        const Object& object,
        Action::Type action,
        const bgq::utility::UserId& user
        ) const
{
    // validate the action on the object
    object.validate( action );

    // check authority
    if ( this->hasAuthority(user, object, action) ) {
        return true;
    }
/*
<rasevent
  id="00063000" 
  category="Process" 
  component="MMCS"
  severity="INFO"
  message="user $(USER) denied $(ACTION) authority on $(OBJECT) $(ID)"
  description="a user attempted an action on an object they were not authorized to"
  service_action="$(NoService)"
/>

<rasevent
  id="00063001" 
  category="Process" 
  component="MMCS"
  severity="INFO"
  message="user $(USER) denied administrative authority for $(COMMAND)"
  description="a user attempted an administrative command they were not authorized to"
  service_action="$(NoService)"
/>
*/
    Ras::create( Ras::ValidateFailure ).
        object( object ).
        action( action ).
        user( user )
        ;

    return false;
}

std::string
Enforcer::Impl::getSectionName(
        Object::Type object
        ) const
{
    // create section name based on object type
    std::string section = "security.";
    switch ( object ) {
        case Object::Job:
            section += "jobs";
            break;
        case Object::Block:
            section += "blocks";
            break;
        case Object::Hardware:
            section += "hardware";
            break;
        default:
            BOOST_ASSERT( !"unhandled object type" );
    }

    return section;
}

bool
Enforcer::Impl::hasAllAuthority(
        const bgq::utility::UserId& user,
        Object::Type object
        ) const
{
    // get value from properties file
    const std::string section( this->getSectionName(object) );
    LOG_DEBUG_MSG( "looking for all in [" << section << "]" );

    std::string result;
    try {
        result = _properties->getValue( section, "all" );
        LOG_DEBUG_MSG( result );
    } catch ( const std::invalid_argument& e ) {
        // missing section isn't an error
        LOG_DEBUG_MSG( e.what() );
        return false;
    }

    StringTokenizer tokenizer;
    tokenizer.tokenize( result );

    // match user or one of their secondary groups in one of the tokens
    BOOST_FOREACH( const std::string& token, tokenizer ) {
        if ( user.getUser() == token ) {
            LOG_DEBUG_MSG( user.getUser() << " has all authority" );
            return true;
        }

        if ( user.isMember(token) ) {
            LOG_DEBUG_MSG( user.getUser() << " in secondary group " << token << " has all authority" );
            return true;
        }
    }

    return false;
}

bool
Enforcer::Impl::hasAuthority(
        const bgq::utility::UserId& user,
        const Object& object,
        Action::Type action
        ) const
{
    LOG_DEBUG_MSG( 
            "checking action " << action << " for user " << user.getUser() << " on object " << object.name() << " of type " << object.type()
            );

    // look for All authority first
    if ( this->hasAllAuthority(user, object.type()) ) {
        LOG_DEBUG_MSG( "user " << user.getUser() << " has all authority for " << object.type() << " objects" );
        return true;
    }

    // look for requested authority
    const std::string section( this->getSectionName(object.type()) );
    const std::string key(
            boost::to_lower_copy(
                boost::lexical_cast<std::string>(action)
                )
            );
    std::string result;

    try {
        LOG_DEBUG_MSG( "looking for " << key << " in [" << section << "]" );
        result = _properties->getValue(
                section,
                key
                );
        LOG_TRACE_MSG( result );
    } catch ( const std::invalid_argument& e ) {
        // this is ok, a missing section is not an error
        LOG_DEBUG_MSG( e.what() );

        // fall through
    }

    StringTokenizer tokenizer;
    tokenizer.tokenize( result );

    // find user in one of the tokens
    BOOST_FOREACH( const std::string& token, tokenizer ) {
        if ( user.getUser() == token ) {
            LOG_DEBUG_MSG( user.getUser() << " has " << action << " authority" );
            return true;
        }

        if ( user.isMember(token) ) {
            LOG_DEBUG_MSG( user.getUser() << " in secondary group " << token << " has " << action << " authority" );
            return true;
        }
    }

    if ( object.type() == Object::Hardware ) {
        // hardware objects don't exist in database
        return false;
    }

    // let database do the work
    boost::shared_ptr<const db::Enforcer> database( _database ); 
    if ( !database ) {
        database.reset( new db::Enforcer );
    }
    if ( database->validate( object, action, user ) ) {
        LOG_DEBUG_MSG( "matched user in database" );
        return true;
    }

    // getting here means the user was not present in the database
    LOG_INFO_MSG( user.getUser() << " denied " << action << " on " << object.type() << " " << object.name() );
    return false;
}

} // security
} // hlcs
