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
#include "Enforcer.h"

#include "utility.h"

#include <db/include/api/tableapi/gensrc/DBTBlocksecurity.h>
#include <db/include/api/tableapi/gensrc/DBTJobsecurity.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <hlcs/include/security/exception.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {
namespace db {

Enforcer::Enforcer(
        const cxxdb::ConnectionPtr& connection
        ) :
    _connection( connection ),
    _job(),
    _block(),
    _owner()
{
    if ( !_connection ) {
        // get connection from pool
        LOG_DEBUG_MSG( "getting connection from pool" );
        _connection = BGQDB::DBConnectionPool::instance().getConnection();
        if ( !_connection ) {
            BOOST_THROW_EXCEPTION(
                    exception::DatabaseError( "could not get database connection" )
                    );
        }
    }
    
    this->prepareStatement();
    _owner.reset( new Owner(_connection) );
}

void
Enforcer::prepareStatement()
{
    _job = _connection->prepareQuery(
            "SELECT " + BGQDB::DBTJobsecurity::AUTHID_COL + " FROM TBGQJobsecurity "
            "WHERE " +
            BGQDB::DBTJobsecurity::JOBID_COL + "=? " +
            "AND " +
            BGQDB::DBTJobsecurity::AUTHORITY_COL + "=?",
            boost::assign::list_of( BGQDB::DBTJobsecurity::JOBID_COL )( BGQDB::DBTJobsecurity::AUTHORITY_COL )
            );

    _block = _connection->prepareQuery(
            "SELECT " + BGQDB::DBTBlocksecurity::AUTHID_COL + " FROM TBGQBlocksecurity "
            "WHERE " +
            BGQDB::DBTBlocksecurity::BLOCKID_COL + "=? " +
            "AND " +
            BGQDB::DBTBlocksecurity::AUTHORITY_COL + "=?",
            boost::assign::list_of( BGQDB::DBTBlocksecurity::BLOCKID_COL )( BGQDB::DBTBlocksecurity::AUTHORITY_COL )
            );
}
 
bool
Enforcer::validate(
        const Object& object,
        Action::Type action,
        const bgq::utility::UserId& user
        ) const
{
    // create block is special, there will be no owner in the database if
    // we haven't created it yet!
    if ( object.type() == Object::Block && action == Action::Create ) {
        return false;
    }

    // check owner of this object, they have permission to do anything
    if ( _owner->execute(object, user) ) {
        LOG_DEBUG_MSG( user.getUser() << " owns " << object.type() << " " << object.name() );
        return true;
    }
   

    cxxdb::ResultSetPtr results;
    try {
        if ( object.type() == Object::Block ) {
            _block->parameters()[ BGQDB::DBTBlocksecurity::BLOCKID_COL ].set( object.name() );
            _block->parameters()[ BGQDB::DBTBlocksecurity::AUTHORITY_COL ].set( toString(action) );
            results = _block->execute();
        } else if ( object.type() == Object::Job ) {
            _job->parameters()[ BGQDB::DBTJobsecurity::JOBID_COL ].set( boost::lexical_cast<int64_t>(object.name()) );
            _job->parameters()[ BGQDB::DBTJobsecurity::AUTHORITY_COL ].set( toString(action) );
            results = _job->execute();
        } else {
            BOOST_ASSERT( !"unhandled type" );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        return false;
    }

    while ( results->fetch() ) {
        const std::string& authorizedUser = results->columns()[ BGQDB::DBTJobsecurity::AUTHID_COL ].getString();
        LOG_DEBUG_MSG( authorizedUser << " has " << action << " authority on " << object.type() << " " << object.name() );
        if ( user.getUser() == authorizedUser ) {
            LOG_DEBUG_MSG( user.getUser() << " matches" );
            return true;
        } else if ( user.isMember(authorizedUser) ) {
            LOG_DEBUG_MSG( user.getUser() << " matches a secondary group" );
            return true;
        }
    }

    LOG_DEBUG_MSG(
            "did not find " << action << " authority for " << object.type() << " '" << object.name() << "'"
            );
    return false;
}

} // db
} // security
} // hlcs
