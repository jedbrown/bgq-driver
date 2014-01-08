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
#include "Log.h"

#include "utility.h"

#include <db/include/api/tableapi/gensrc/DBTBlocksecuritylog.h>
#include <db/include/api/tableapi/gensrc/DBTJobsecuritylog.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {
namespace db {

char
Log::Action::toChar(
        Log::Action::Type type
        )
{
    switch ( type ) {
        case Log::Action::Grant: return 'G';
        case Log::Action::Revoke: return 'R';
        default: BOOST_ASSERT( !"unhandled type" );
    }
}

std::ostream&
operator<<(
        std::ostream& os,
        Log::Action::Type type
        )
{
    switch ( type ) {
        case Log::Action::Grant: os << "Grant";
        case Log::Action::Revoke: os << "Revoke";
        default: BOOST_ASSERT( !"unhandled type" );
    }

    return os;
}

Log::Log(
        const cxxdb::ConnectionPtr& connection,
        const Object& object,
        Action::Type action
        ) :
    _connection( connection ),
    _statement(),
    _object( object )
{
    if ( _object.type() == Object::Block ) {
        _statement = _connection->prepareUpdate(
                "INSERT into TBGQBlockSecurityLog ( "
                + BGQDB::DBTBlocksecuritylog::ACTION_COL + ","
                + BGQDB::DBTBlocksecuritylog::AUTHID_COL + ","
                + BGQDB::DBTBlocksecuritylog::BLOCKID_COL + ","
                + BGQDB::DBTBlocksecuritylog::AUTHORITY_COL + ","
                + BGQDB::DBTBlocksecuritylog::CHANGEDBYID_COL +
                ") VALUES ( '" + Log::Action::toChar(action) + "', ?, ?, ?, ? )",
                boost::assign::list_of
                ( BGQDB::DBTBlocksecuritylog::AUTHID_COL )
                ( BGQDB::DBTBlocksecuritylog::BLOCKID_COL )
                ( BGQDB::DBTBlocksecuritylog::AUTHORITY_COL )
                ( BGQDB::DBTBlocksecuritylog::CHANGEDBYID_COL)
            );
    } else if ( _object.type() == Object::Job ) {
        _statement = _connection->prepareUpdate(
                "INSERT into TBGQJobSecurityLog ( "
                + BGQDB::DBTJobsecuritylog::ACTION_COL + ","
                + BGQDB::DBTJobsecuritylog::AUTHID_COL + ","
                + BGQDB::DBTJobsecuritylog::JOBID_COL + ","
                + BGQDB::DBTJobsecuritylog::AUTHORITY_COL + ","
                + BGQDB::DBTJobsecuritylog::CHANGEDBYID_COL +
                ") VALUES ( '" + Log::Action::toChar(action) + "', ?, ?, ?, ? )",
                boost::assign::list_of
                ( BGQDB::DBTJobsecuritylog::AUTHID_COL )
                ( BGQDB::DBTJobsecuritylog::JOBID_COL )
                ( BGQDB::DBTJobsecuritylog::AUTHORITY_COL )
                ( BGQDB::DBTJobsecuritylog::CHANGEDBYID_COL)
                );
    } else {
        BOOST_ASSERT( !"unhandled type" );
    }
}

void
Log::operator()(
        const Authority& authority,
        const bgq::utility::UserId& user
        )
{
    try {
        if ( _object.type() == Object::Block ) {
            _statement->parameters()[ BGQDB::DBTBlocksecuritylog::AUTHID_COL ].set( authority.user() );
            _statement->parameters()[ BGQDB::DBTBlocksecuritylog::BLOCKID_COL ].set( _object.name() );
            _statement->parameters()[ BGQDB::DBTBlocksecuritylog::AUTHORITY_COL ].set( toString(authority.action()) );
            _statement->parameters()[ BGQDB::DBTBlocksecuritylog::CHANGEDBYID_COL ].set( user.getUser() );
        } else if ( _object.type() == Object::Job ) {
            _statement->parameters()[ BGQDB::DBTJobsecuritylog::AUTHID_COL ].set( authority.user() );
            _statement->parameters()[ BGQDB::DBTJobsecuritylog::JOBID_COL ].cast( boost::lexical_cast<uint64_t>(_object.name()) );
            _statement->parameters()[ BGQDB::DBTJobsecuritylog::AUTHORITY_COL ].set( toString(authority.action()) );
            _statement->parameters()[ BGQDB::DBTJobsecuritylog::CHANGEDBYID_COL ].set( user.getUser() );
        } else {
            BOOST_ASSERT( !"unhandled type" );
        }

        unsigned rows;
        _statement->execute( &rows );
        LOG_TRACE_MSG( "inserted " << rows << " rows" );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        throw;
    }
}

} // db
} // security
} // hlcs
