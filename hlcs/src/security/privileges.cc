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
#include <hlcs/include/security/privileges.h>

#include "db/grant.h"
#include "db/list.h"
#include "db/revoke.h"

#include <bgq_util/include/string_tokenizer.h>

#include <utility/include/Log.h>

#include <boost/algorithm/string.hpp>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {

void
grant(
        const Object& object,
        const Authority& authority,
        const bgq::utility::UserId& user
     )
{
    db::grant( object, authority, user );
}

void
revoke(
        const Object& object,
        const Authority& authority,
        const bgq::utility::UserId& user
     )
{
    db::revoke( object, authority, user );
}

void
addAllAuthority(
        Authorities& authorities,
        const std::string& value
        )
{
    LOG_TRACE_MSG( "adding all authority for: '" << value << "'" );

    StringTokenizer tokenizer;
    tokenizer.tokenize( value, "," );

    BOOST_FOREACH( const std::string& user, tokenizer ) {
        // add all actions for these users
        Action::Type action = Action::Create;
        while ( action != Action::Invalid ) {
            Authority authority( user, action );
            authority.source( Authority::Source::Properties );

            authorities.add( authority );
            LOG_TRACE_MSG( "added authority " << authority << " from " << authority.source() );

            // get next action
            action = static_cast<Action::Type>( static_cast<uint32_t>(action) + 1 );
        }
    }
}

void
addAuthority(
        Authorities& authorities,
        const std::string& action,
        const std::string& value
        )
{
    try {
        // convert key into action
        const Action::Type a = boost::lexical_cast<Action::Type>( action );

        LOG_TRACE_MSG( "adding " << a << " authority for: '" << value << "'" );
        StringTokenizer tokenizer;
        tokenizer.tokenize( value, "," );

        BOOST_FOREACH( const std::string& user, tokenizer ) {
            Authority authority( user, a );
            authority.source( Authority::Source::Properties );

            authorities.add( authority );
            LOG_TRACE_MSG( "added authority " << authority << " from " << authority.source() );
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "ignoring action: " << action );
        return;
    }
}

void
addPropertiesAuthorities(
        Authorities& authorities,
        const Object& object,
        const bgq::utility::Properties& properties
        )
{
    // convert object type to section name
    std::string section;
    if ( object.type() == Object::Block ) {
        section = "blocks";
    } else if ( object.type() == Object::Job ) {
        section = "jobs";
    } else if ( object.type() == Object::Hardware ) {
        section = "hardware";
    } else {
        BOOST_ASSERT( !"unhandled type" );
    }

    // get section for the object
    bgq::utility::Properties::Section global;
    try {
        global = properties.getValues( "security." + section );
    } catch ( const std::invalid_argument& e ) {
        // empty properties file
        LOG_DEBUG_MSG( e.what() );
        return;
    }

    // iterate through each key value pair
    BOOST_FOREACH( const bgq::utility::Properties::Pair& i, global ) {
        const std::string& key = i.first;
        const std::string& value = i.second;

        // special case for all action
        if ( boost::iequals(key, "all") ) {
            addAllAuthority( authorities, value );
        } else {
            addAuthority( authorities, key, value );
        }
    }
}

Authorities
list(
        const bgq::utility::Properties& properties,
        const Object& object
    )
{
    const std::string owner; // empty owner
    Authorities result( owner );
    try {
        result = db::list( object );
    } catch ( const std::invalid_argument& e ) {
        // Hardware objects won't exist in database
        LOG_DEBUG_MSG( e.what() );
    }

    addPropertiesAuthorities( result, object, properties );

    return result;
}

Authorities
list(
        const Object& object
    )
{
    const std::string owner; // empty owner
    Authorities result( owner );
    try {
        result = db::list( object );
    } catch ( const std::invalid_argument& e ) {
        // Hardware objects won't exist in database
        LOG_DEBUG_MSG( e.what() );
    }

    return result;
}

} // security
} // hlcs
