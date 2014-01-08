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
#include "grant.h"

#include "Log.h"
#include "utility.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTBlocksecurity.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>
#include <db/include/api/tableapi/gensrc/DBTJobsecurity.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/security/exception.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {
namespace db {

cxxdb::UpdateStatementPtr
grantJob(
        const cxxdb::ConnectionPtr& connection,
        const Object& object,
        const Authority& authority,
        const bgq::utility::UserId& user
        )
{
    cxxdb::Transaction tx( *connection );

    uint64_t id;
    try {
        id = boost::lexical_cast<uint64_t>( object.name() );
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "garbage job id: '" << object.name() << "'" );
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( "garbage job ID: '" + object.name() + "'" )
                );
    }

    const cxxdb::UpdateStatementPtr statement(
            connection->prepareUpdate(
                "INSERT into TBGQJobsecurity ( "
                + BGQDB::DBTJobsecurity::AUTHID_COL + ","
                + BGQDB::DBTJobsecurity::JOBID_COL + ","
                + BGQDB::DBTJobsecurity::AUTHORITY_COL +
                ") VALUES ( ?, ?, ? )",
                boost::assign::list_of
                ( BGQDB::DBTJobsecurity::AUTHID_COL )
                ( BGQDB::DBTJobsecurity::JOBID_COL )
                ( BGQDB::DBTJobsecurity::AUTHORITY_COL )
                )
            );

    statement->parameters()[ BGQDB::DBTJobsecurity::AUTHID_COL ].set( authority.user() );
    statement->parameters()[ BGQDB::DBTJobsecurity::JOBID_COL ].cast( id );
    statement->parameters()[ BGQDB::DBTJobsecurity::AUTHORITY_COL ].set( toString(authority.action()) );

    return statement;
}


cxxdb::UpdateStatementPtr
grantBlock(
        const cxxdb::ConnectionPtr& connection,
        const Object& object,
        const Authority& authority,
        const bgq::utility::UserId& user
        )
{
    cxxdb::Transaction tx( *connection );

    const cxxdb::UpdateStatementPtr statement(
            connection->prepareUpdate(
                "INSERT into TBGQBlocksecurity ( "
                + BGQDB::DBTBlocksecurity::AUTHID_COL + ","
                + BGQDB::DBTBlocksecurity::BLOCKID_COL + ","
                + BGQDB::DBTBlocksecurity::AUTHORITY_COL +
                ") VALUES ( ?, ?, ? )",
                boost::assign::list_of
                ( BGQDB::DBTBlocksecurity::AUTHID_COL )
                ( BGQDB::DBTBlocksecurity::BLOCKID_COL )
                ( BGQDB::DBTBlocksecurity::AUTHORITY_COL )
                )
            );

    statement->parameters()[ BGQDB::DBTBlocksecurity::AUTHID_COL ].set( authority.user() );
    statement->parameters()[ BGQDB::DBTBlocksecurity::BLOCKID_COL ].set( object.name() );
    statement->parameters()[ BGQDB::DBTBlocksecurity::AUTHORITY_COL ].set( toString(authority.action()) );

    return statement;
}

void
grant(
        const Object& object,
        const Authority& authority,
        const bgq::utility::UserId& user
        )
{
    // log arguments
    LOG_DEBUG_MSG( 
            user.getUser() << " granting " << authority.action() << " permission to " << 
            object.type() << " " << object.name() << " for user " << authority.user()
            );

    // validate action on the object
    object.validate( authority.action() );

    // get connection
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        BOOST_THROW_EXCEPTION(
                exception::DatabaseError( "could not get database connection" )
                );
    }

    cxxdb::UpdateStatementPtr statement;

    // ensure object type is valid
    switch ( object.type() ) {
        case Object::Block:
            statement = grantBlock( connection, object, authority, user );
            break;
        case Object::Job:
            statement = grantJob( connection, object, authority, user );
            break;
        default:
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument( 
                        boost::lexical_cast<std::string>(object.type()) + " object authority cannot be granted"
                        )
                    );
    }

    try {
        unsigned rows;
        statement->execute( &rows );
        LOG_TRACE_MSG( "inserted " << rows << " rows" );

        // create unnamed temporary
        Log( connection, object, Log::Action::Grant )(authority, user );
        connection->commit();
    } catch ( const cxxdb::DatabaseException& e ) {
        LOG_WARN_MSG( e.what() );
        if (
                !e.getDiagnosticRecords().empty() &&
                e.getDiagnosticRecords()[0].native_error == -530 // insert of a foreign key is invalid
           )
        {
            BOOST_THROW_EXCEPTION(
                    exception::ObjectNotFound( 
                        boost::lexical_cast<std::string>(object.type()) + " " + 
                        object.name() + " " +
                        " does not exist"
                        )
                    );
        } else {
            BOOST_THROW_EXCEPTION(
                    exception::DatabaseError( 
                        boost::lexical_cast<std::string>(authority.action()) + " authority for " + 
                        boost::lexical_cast<std::string>(object.type()) + " " +
                        object.name() + " for user " + authority.user() + " already exists"
                        )
                    );
        }
    } catch ( const cxxdb::WrongType& e ) {
        LOG_WARN_MSG( e.what() );
        BOOST_THROW_EXCEPTION(
                exception::DatabaseError( e.what() )
                );
    } catch ( const cxxdb::ValueTooBig& e ) {
        LOG_WARN_MSG( e.what() );
        BOOST_THROW_EXCEPTION(
                exception::DatabaseError( e.what() )
                );
    }
}

} // db
} // security
} // hlcs
