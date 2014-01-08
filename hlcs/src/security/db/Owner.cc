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
#include "Owner.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <hlcs/include/security/exception.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {
namespace db {

Owner::Owner(
        const cxxdb::ConnectionPtr& connection
        ) :
    _job(),
    _block()
{
    if ( !connection ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( "Bad database connection parameter" )
                );
    }

    this->prepareJobStatement( connection );
    this->prepareBlockStatement( connection );
}

void
Owner::prepareJobStatement(
        const cxxdb::ConnectionPtr& connection
        )
{
    _job = connection->prepareQuery(
            "SELECT " + BGQDB::DBTJob::USERNAME_COL + " FROM TBGQJob "
            "WHERE " +
            BGQDB::DBTJob::ID_COL + "=?",
            boost::assign::list_of( BGQDB::DBTJob::ID_COL )
            );
    if ( !_job ) {
        BOOST_THROW_EXCEPTION(
                exception::DatabaseError( "Could not prepare job owner query" )
                );
    }
}

void
Owner::prepareBlockStatement(
        const cxxdb::ConnectionPtr& connection
        )
{
    _block = connection->prepareQuery(
            "SELECT " + BGQDB::DBTBlock::OWNER_COL + " FROM TBGQBlock "
            "WHERE " +
            BGQDB::DBTBlock::BLOCKID_COL + "=?",
            boost::assign::list_of( BGQDB::DBTBlock::BLOCKID_COL )
            );
    if ( !_block ) {
        BOOST_THROW_EXCEPTION(
                exception::DatabaseError( "Could not prepare block owner query" )
                );
    }
}

bool
Owner::execute(
        const Object& object,
        const bgq::utility::UserId& user
        ) const
{
    std::string owner;
    try {
        if ( object.type() == Object::Block ) {
            _block->parameters()[ BGQDB::DBTBlock::BLOCKID_COL ].set( object.name() );
            const cxxdb::ResultSetPtr results = _block->execute();
            if ( !results->fetch() ) {
                BOOST_THROW_EXCEPTION(
                        exception::ObjectNotFound( "Could not find block " + object.name() )
                        );
            }
            owner = results->columns()[ BGQDB::DBTBlock::OWNER_COL ].getString();

            // fall through
        } else if ( object.type() == Object::Job ) {
            _job->parameters()[ BGQDB::DBTJob::ID_COL ].set( boost::lexical_cast<int64_t>(object.name()) );
            const cxxdb::ResultSetPtr results = _job->execute();
            if ( !results->fetch() ) {
                BOOST_THROW_EXCEPTION(
                        exception::ObjectNotFound( "Could not find job " + object.name() )
                        );
            }
            owner = results->columns()[ BGQDB::DBTJob::USERNAME_COL ].getString();

            // fall through
        } else {
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument(
                        boost::lexical_cast<std::string>( object.type() ) + " objects are not supported"
                        )
                    );
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "Bad job ID: " << object.name() );
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( "Bad job ID: " + object.name() )
                );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        BOOST_THROW_EXCEPTION(
                exception::DatabaseError( e.what() )
                );
    }

    if ( user.getUser() == owner ) {
        LOG_DEBUG_MSG( user.getUser() << " owns " << object.type() << " " << object.name() );
        return true;
    }

    LOG_DEBUG_MSG( user.getUser() << " does not own " << object.type() << " " << object.name() );
    return false;
}

} // db
} // security
} // hlcs
