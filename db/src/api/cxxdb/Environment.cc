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

#include "cxxdb/Environment.h"

#include "BasicConnection.h"

#include "cxxdb/exceptions.h"

#include <utility/include/Log.h>

#include <sqlext.h>

#include <stdexcept>

using std::logic_error;

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

EnvironmentPtr *Environment::_application_environment_ptr_p = NULL;

EnvironmentPtr Environment::create()
{
    return EnvironmentPtr( new Environment() );
}

void Environment::initializeApplicationEnvironment()
{
    if ( _application_environment_ptr_p ) {
        // Already initialized.
        return;
    }

    _application_environment_ptr_p = new EnvironmentPtr( new Environment() );
}


Environment& Environment::getApplicationEnvironment()
{
    if ( ! _application_environment_ptr_p ) {
        CXXDB_THROW_EXCEPTION( logic_error(
                "called getApplicationEnvironment when the application environment hasn't been initialized. There is an error in the program."
            ) );
    }

    return **_application_environment_ptr_p;
}


void Environment::resetApplicationEnvironment()
{
    if ( _application_environment_ptr_p ) {
        delete _application_environment_ptr_p;
    }
    _application_environment_ptr_p = NULL;
}


Environment::Environment()
{
    _handle.setOdbcVersion( SQL_OV_ODBC3 );
}


ConnectionPtr Environment::newConnection(
        const std::string& db_name,
        const std::string* user_name_p,
        const std::string* user_pwd_p,
        const std::string& extra_connection_parameters
    )
{
    _LockGuard lg( _mtx );

    BasicConnectionPtr ret(new BasicConnection(
            shared_from_this(),
            _handle,
            db_name,
            user_name_p,
            user_pwd_p,
            extra_connection_parameters
        ));
    _conn_ptrs[ret.get()] = ret; // Store a weak pointer to the connection.
    return ret;
}


ConnectionPtr Environment::newConnection(
        const std::string& db_name,
        const std::string& user_name,
        const std::string& user_pwd,
        const std::string& extra_connection_parameters
    )
{
    return newConnection( db_name, &user_name, &user_pwd, extra_connection_parameters );
}


void Environment::notifyConnectionDestroyed( BasicConnection* conn_p )
{
    LOG_DEBUG_MSG( "Notified connection destroyed with conn_p=" << conn_p );

    _LockGuard lg( _mtx );
    if ( ! _conn_ptrs.erase( conn_p ) ) {
        LOG_WARN_MSG( "Notified connection destroyed but wasn't tracking it. This indicates a programming problem." );
    }
}


Environment::~Environment()
{
    if ( _conn_ptrs.empty() ) {
        LOG_DEBUG_MSG( "Destroying database environment" );
    } else {
        LOG_WARN_MSG( "Destroying database environment with " << _conn_ptrs.size() << " active connection(s)." );
    }

    // Invalidate all my connections.
    for ( _ConnPtrs::iterator i(_conn_ptrs.begin()) ; i != _conn_ptrs.end() ; ++i ) {
        BasicConnectionPtr conn_ptr(i->second.lock());
        if ( conn_ptr ) {
            conn_ptr->invalidate();
        }
    }
}


} // namespace cxxdb
