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
#include "list.h"

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

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {
namespace db {

void
validateObject(
        Object::Type object
        )
{
    switch ( object ) {
        case Object::Block:
        case Object::Job:
            break;
        default:
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument( boost::lexical_cast<std::string>(object) + " objects are not supported" )
                    );
    }
}

std::string
getOwner(
        const Object& object,
        const cxxdb::ConnectionPtr& connection
        )
{
    cxxdb::QueryStatementPtr statement;
    if ( object.type() == Object::Block ) {
        statement = connection->prepareQuery( 
                    "SELECT " + BGQDB::DBTBlock::BLOCKID_COL + "," + BGQDB::DBTBlock::OWNER_COL + " FROM TBGQBlock "
                    "WHERE " + BGQDB::DBTBlock::BLOCKID_COL + "=?",
                    boost::assign::list_of( BGQDB::DBTBlock::BLOCKID_COL )
                );
        statement->parameters()[ BGQDB::DBTBlock::BLOCKID_COL ].set( object.name() );
    } else if ( object.type() == Object::Job ) {
        statement = connection->prepareQuery( 
                    "SELECT " + BGQDB::DBTJob::ID_COL + "," + BGQDB::DBTJob::USERNAME_COL + " FROM TBGQJob "
                    "WHERE " + BGQDB::DBTJob::ID_COL + "=?",
                    boost::assign::list_of( BGQDB::DBTJob::ID_COL )
                );
        try {
            statement->parameters()[ BGQDB::DBTJob::ID_COL ].set( boost::lexical_cast<int64_t>(object.name()) );
        } catch ( const boost::bad_lexical_cast& e ) {
            BOOST_THROW_EXCEPTION(
                    exception::ObjectNotFound( " job " + object.name() + " was not found" )
                    );
        }
    } else {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( boost::lexical_cast<std::string>(object.type()) + " objects are not supported" )
                );
    }

    cxxdb::ResultSetPtr resultSet( statement->execute() );

    if ( !resultSet->fetch() ) {
        BOOST_THROW_EXCEPTION(
                exception::ObjectNotFound( boost::lexical_cast<std::string>(object.type()) + " " + object.name() + " not found" )
                );
    }

    std::string result;
    if ( object.type() == Object::Block ) {
        result = resultSet->columns()[ BGQDB::DBTBlock::OWNER_COL ].getString();
    } else if ( object.type() == Object::Job ) {
        result = resultSet->columns()[ BGQDB::DBTJob::USERNAME_COL ].getString();
    } else {
        BOOST_ASSERT( "unhandled object type" );
    }

    return result;
}

cxxdb::QueryStatementPtr
prepareQuery(
        const Object& object,
        const cxxdb::ConnectionPtr& connection
        )
{
    cxxdb::QueryStatementPtr statement;
    if ( object.type() == Object::Block ) {
        statement = connection->prepareQuery(
                "SELECT " +
                BGQDB::DBTBlocksecurity::AUTHID_COL + "," + BGQDB::DBTBlocksecurity::AUTHORITY_COL + " " +
                "FROM TBGQBlockSecurity WHERE " +
                BGQDB::DBTBlocksecurity::BLOCKID_COL + "=?",
                boost::assign::list_of( BGQDB::DBTBlocksecurity::BLOCKID_COL )
                );
        statement->parameters()[ BGQDB::DBTBlocksecurity::BLOCKID_COL ].set( object.name() );
    } else if ( object.type() == Object::Job ) {
        statement = connection->prepareQuery(
                "SELECT " +
                BGQDB::DBTJobsecurity::AUTHID_COL + "," + BGQDB::DBTJobsecurity::AUTHORITY_COL + " " +
                "FROM TBGQJobSecurity WHERE " +
                BGQDB::DBTJobsecurity::JOBID_COL + "=?",
                boost::assign::list_of( BGQDB::DBTJobsecurity::JOBID_COL )
                );
        statement->parameters()[ BGQDB::DBTJobsecurity::JOBID_COL ].set( boost::lexical_cast<int64_t>(object.name()) );
    } else {
        BOOST_THROW_EXCEPTION( 
                std::invalid_argument( boost::lexical_cast<std::string>(object.type()) + " objects are not supported" )
                );
    }

    return statement;
}

Authorities
list(
        const Object& object
        )
{
    validateObject( object.type() );

    const cxxdb::ConnectionPtr dbConnection = BGQDB::DBConnectionPool::instance().getConnection();
    if ( !dbConnection ) {
        BOOST_THROW_EXCEPTION( 
                exception::DatabaseError( "could not get database connection" )
                );
    }

    const std::string owner = getOwner( object, dbConnection );

    const cxxdb::QueryStatementPtr statement = prepareQuery( object, dbConnection );

    Authorities result( owner );

    try {
        const cxxdb::ResultSetPtr results = statement->execute();

        while ( results->fetch() ) {
            const std::string& authorizedIdColumn = object.type() == Object::Block ? BGQDB::DBTBlocksecurity::AUTHID_COL : BGQDB::DBTJobsecurity::AUTHID_COL;
            const std::string& authorityColumn = object.type() == Object::Block ? BGQDB::DBTBlocksecurity::AUTHORITY_COL : BGQDB::DBTJobsecurity::AUTHORITY_COL;
            Authority authority(
                    results->columns()[ authorizedIdColumn ].getString(),
                    fromChar( results->columns()[ authorityColumn ].getChar() )
                    );
            authority.source( Authority::Source::Granted );
            LOG_TRACE_MSG( "added authority: " << authority );
            result.add( authority );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return result;
}

} // db
} // security
} // hlcs
