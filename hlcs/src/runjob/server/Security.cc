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
#include "server/Security.h"

#include "common/logging.h"

#include "server/Options.h"
#include "server/Server.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <hlcs/include/security/privileges.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

Security::Ptr
Security::create(
        const Server::Ptr& server
        )
{
    const Security::Ptr result(
            new Security( server )
            );

    return result;
}

Security::Security(
        const Server::Ptr& server
        ) :
    _database( BGQDB::DBConnectionPool::instance().getConnection() ),
    _blockEnforcer( server->getOptions().getProperties(), _database ),
    _commandEnforcer( server->getOptions().getProperties() ),
    _strand( server->getIoService() )
{

}

void
Security::grant(
        const std::string& user,
        const hlcs::security::Action::Type action,
        const BGQDB::job::Id job,
        const bgq::utility::UserId::ConstPtr& granter
        )
{
    hlcs::security::grant(
            hlcs::security::Object( hlcs::security::Object::Job, boost::lexical_cast<std::string>(job) ),
            hlcs::security::Authority(user, action),
            *granter
            );
}

void
Security::revoke(
        const std::string& user,
        const hlcs::security::Action::Type action,
        const BGQDB::job::Id job,
        const bgq::utility::UserId::ConstPtr& revoker
        )
{
    hlcs::security::revoke(
            hlcs::security::Object( hlcs::security::Object::Job, boost::lexical_cast<std::string>(job) ),
            hlcs::security::Authority(user, action),
            *revoker
            );
}

void
Security::executeBlockImpl(
        const bgq::utility::UserId::ConstPtr& user,
        const std::string& block,
        const ExecuteBlockCallback& callback
        )
{
    try {
        const bool result = _blockEnforcer.validate( 
                hlcs::security::Object( hlcs::security::Object::Block, block ),
                hlcs::security::Action::Execute,
                *user
                );

        callback( result );
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
        callback( false );
    }
}

bool
Security::validate(
        const bgq::utility::UserId::ConstPtr& user,
        const hlcs::security::Action::Type action,
        const BGQDB::job::Id job
        )
{
    try {
        return _commandEnforcer.validate(
                hlcs::security::Object( hlcs::security::Object::Job, boost::lexical_cast<std::string>(job) ),
                action,
                *user
                );
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
        return false;
    }
}


} // server
} // runjob
