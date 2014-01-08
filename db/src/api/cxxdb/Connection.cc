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

#include "cxxdb/Connection.h"

#include "cxxdb/Environment.h"

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "database" );


namespace cxxdb {


const ParameterNames Connection::NoParameterNames;


ConnectionPtr Connection::create(
        const std::string& db_name,
        const std::string* user_name_p,
        const std::string* user_pwd_p,
        const std::string& extra_connection_parameters
    )
{
    return Environment::getApplicationEnvironment().newConnection(
            db_name,
            user_name_p,
            user_pwd_p,
            extra_connection_parameters
        );
}


ConnectionPtr Connection::create(
        const std::string& db_name,
        const std::string& user_name,
        const std::string& user_pwd,
        const std::string& extra_connection_parameters
    )
{
    return create( db_name, &user_name, &user_pwd, extra_connection_parameters );
}


Connection::~Connection()
{
    /* Nothing to do */
}


} // namespace cxxdb
