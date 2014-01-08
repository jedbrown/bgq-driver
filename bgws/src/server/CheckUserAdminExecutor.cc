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

#include "CheckUserAdminExecutor.hpp"

#include "CheckUserAdminExecution.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


void CheckUserAdminExecutor::start(
        const std::string& username,
        CompleteCallback cb
    )
{
    DynamicConfiguration::ConstPtr config_ptr(_dynamic_configuration_ptr);

    if ( config_ptr->getCheckUserAdminExecutable() == boost::filesystem::path() ) {
        LOG_DEBUG_MSG( "No check user admin executable set in properties, will just say everybody's a regular user." );

        _io_service.post( boost::bind( cb, UserInfo::UserType::Regular ) );

        return;
    }


    boost::shared_ptr<CheckUserAdminExecution> execution_ptr( new CheckUserAdminExecution(
            username,
            cb,
            config_ptr->getCheckUserAdminExecutable(),
            config_ptr->getPropertiesFilename(),
            _child_processes
        ) );

    execution_ptr->start();
}


} // namespace bgws
