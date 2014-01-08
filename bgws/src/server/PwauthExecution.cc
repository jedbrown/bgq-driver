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

#include "PwauthExecution.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


PwauthExecution::PwauthExecution(
        const std::string& username,
        const std::string& password,
        const PwauthExecutor::CompleteCallback& complete_callback,
        const boost::filesystem::path& user_auth_exe_path,
        utility::ChildProcesses& child_processes
    ) :
        _complete_callback(complete_callback),
        _write_msg(username + "\n" + password + "\n"),
        _process_ptr(child_processes.createEasy( "pwauth", user_auth_exe_path ))
{
    // Nothing to do.
}


void PwauthExecution::start()
{
    _process_ptr->start(
            boost::bind( &PwauthExecution::_handleProcessEnded, shared_from_this(), _1 ),
            _write_msg
        );
}


PwauthExecution::~PwauthExecution()
{
    LOG_DEBUG_MSG( _process_ptr << " Destroyed." );
}


void PwauthExecution::_handleProcessEnded(
        const bgq::utility::ExitStatus& exit_status
    )
{
    _complete_callback( exit_status ? false : true );

    _complete_callback = PwauthExecutor::CompleteCallback();
}


} // namespace bgws
