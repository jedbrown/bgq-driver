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

#include "CheckUserAdminExecution.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


CheckUserAdminExecution::CheckUserAdminExecution(
        const std::string& username,
        CheckUserAdminExecutor::CompleteCallback complete_callback,
        const boost::filesystem::path& exe_path,
        const std::string& properties_filename,
        utility::ChildProcesses& child_processes
    ) :
        _username(username),
        _complete_callback(complete_callback),
        _exe_path(exe_path),
        _properties_filename(properties_filename)
{
    utility::ChildProcess::Args args;

    args.push_back( "--properties" );
    args.push_back( properties_filename );
    args.push_back( username );

    _process_ptr = child_processes.createEasy( "checkUserAdmin", exe_path, args );
}


void CheckUserAdminExecution::start()
{
    _process_ptr->start(
            boost::bind( &CheckUserAdminExecution::_handleProcessEnded, shared_from_this(), _1 )
        );
}


void CheckUserAdminExecution::_handleProcessEnded(
        const bgq::utility::ExitStatus& exit_status
    )
{
    _complete_callback( exit_status ? UserInfo::UserType::Regular : UserInfo::UserType::Administrator );
}


} // namespace bgws
