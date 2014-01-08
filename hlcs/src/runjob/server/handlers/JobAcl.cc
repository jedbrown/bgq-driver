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
#include "server/handlers/JobAcl.h"

#include "hlcs/src/security/db/Owner.h"

#include "common/commands/JobAcl.h"
#include "common/logging.h"

#include "server/job/Container.h"

#include "server/CommandConnection.h"
#include "server/Options.h"
#include "server/Ras.h"
#include "server/Security.h"
#include "server/Server.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <hlcs/include/security/exception.h>
#include <hlcs/include/security/privileges.h>
#include <hlcs/include/security/Types.h>

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

JobAcl::JobAcl(
        const Server::Ptr& server
        ) :
    CommandHandler( server )
{

}

void
JobAcl::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const runjob::commands::request::JobAcl::Ptr jobAclRequest(
            boost::static_pointer_cast<runjob::commands::request::JobAcl>( request )
            );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( connection->getUserType() == bgq::utility::portConfig::UserType::Administrator ) {
        // user presented administrative certificate, let them do anything
        //
        // fall through
        LOG_DEBUG_MSG( "skipping security check for administrator: " << connection->getUser()->getUser() );
    } else if ( connection->getUserType() == bgq::utility::portConfig::UserType::Normal ) {
        if ( this->checkOwner(jobAclRequest, connection) ) return;
    } else {
        BOOST_ASSERT( !"unhandled user type" );
    }

    const runjob::commands::request::JobAcl::Mode mode = jobAclRequest->_mode;
    LOG_TRACE_MSG( runjob::commands::request::JobAcl::toString( mode ) );

    const runjob::commands::response::JobAcl::Ptr response( new runjob::commands::response::JobAcl );
    try {
        switch ( mode ) {
            case runjob::commands::request::JobAcl::List:
                this->list( jobAclRequest, response );
                break;
            case runjob::commands::request::JobAcl::Grant:
                this->grant( jobAclRequest, connection );
                break;
            case runjob::commands::request::JobAcl::Revoke:
                this->revoke( jobAclRequest, connection );
                break;
            default:
                LOG_WARN_MSG( mode );
                BOOST_ASSERT( !"unhandled mode" );
        }
    } catch ( const hlcs::security::exception::ObjectNotFound& e ) {
        LOG_WARN_MSG( e.what() );
        response->setError( runjob::commands::error::job_not_found );
        response->setMessage( e.what() );
    } catch ( const hlcs::security::exception::DatabaseError& e ) {
        LOG_WARN_MSG( e.what() );
        response->setError( runjob::commands::error::database_error );
        response->setMessage( e.what() );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        response->setError( runjob::commands::error::unknown_failure );
        response->setMessage( e.what() );
    }

    connection->write( response );
}

bool
JobAcl::checkOwner(
        const runjob::commands::request::JobAcl::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    runjob::commands::response::JobAcl::Ptr response( new runjob::commands::response::JobAcl );

    try {
        const cxxdb::ConnectionPtr databaseConnection = BGQDB::DBConnectionPool::instance().getConnection();
        if ( !databaseConnection ) {
            response->setError( runjob::commands::error::database_error );
            response->setMessage( "could not get database connection to determine job owner" );
            LOG_WARN_MSG( response->getMessage() );
            connection->write( response );
            return false;
        }

        // use hlcs::security::db::Owner object to establish ownership
        const hlcs::security::db::Owner owner( databaseConnection );
        const hlcs::security::Object object( 
                hlcs::security::Object::Job,
                boost::lexical_cast<std::string>(request->_id)
                );
        const bgq::utility::UserId::ConstPtr uid( connection->getUser() );
        if ( !owner.execute( object, *uid )) {
            response->setError( runjob::commands::error::permission_denied );
            response->setMessage( uid->getUser() + " does not own job " + boost::lexical_cast<std::string>(request->_id) );

            std::string command;
            switch ( request->_mode ) {
                case runjob::commands::request::JobAcl::List:
                    command = "list";
                    break;
                case runjob::commands::request::JobAcl::Grant:
                    command = "grant";
                    break;
                case runjob::commands::request::JobAcl::Revoke:
                    command = "revoke";
                break;
            default:
                break;
            }

            Ras::create( Ras::AdministrativeAuthorityDenied ).
                detail( "USER", uid->getUser() ).
                detail( "COMMAND", std::string("runjob_server command: ") + command + "_job_authority" )
                ;
        }
    } catch ( const hlcs::security::exception::ObjectNotFound& e ) {
        LOG_WARN_MSG( e.what() );
        response->setError( runjob::commands::error::job_not_found );
        response->setMessage( e.what() );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        response->setError( runjob::commands::error::database_error );
        response->setMessage( e.what() );
    }

    if ( response->getError() ) {
        LOG_WARN_MSG( response->getMessage() );
        connection->write( response );
    }

    return response->getError();
}

void
JobAcl::list(
        const runjob::commands::request::JobAcl::Ptr& request,
        const runjob::commands::response::JobAcl::Ptr& response
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const Options& options = server->getOptions();
    const bgq::utility::Properties::ConstPtr properties = options.getProperties();

    const hlcs::security::Authorities authorities(
            hlcs::security::list(
                *properties,
                hlcs::security::Object(
                    hlcs::security::Object::Job,
                    boost::lexical_cast<std::string>(request->_id)
                    )
                )
            );

    response->_owner = authorities.getOwner();

    // iterate through container, converting to type used in response message
    BOOST_FOREACH( const hlcs::security::Authority& i, authorities.get() ) {
        runjob::commands::response::JobAcl::Authority authority;
        authority._user = i.user();
        authority._action = i.action();
        authority._source = i.source();
        response->_authorities.push_back( authority );
    }
}

void
JobAcl::grant(
        const runjob::commands::request::JobAcl::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getSecurity()->grant(
            request->_user,
            request->_action,
            request->_id,
            connection->getUser()
            );
}

void
JobAcl::revoke(
        const runjob::commands::request::JobAcl::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getSecurity()->revoke(
            request->_user,
            request->_action,
            request->_id,
            connection->getUser()
            );
}

} // handlers
} // server
} // runjob
