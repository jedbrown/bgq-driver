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

#ifndef BGWS_CHECK_USER_ADMIN_EXECUTION_HPP_
#define BGWS_CHECK_USER_ADMIN_EXECUTION_HPP_


#include "CheckUserAdminExecutor.hpp"

#include "utility/ChildProcesses.hpp"
#include "utility/EasyChildProcess.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>

#include <string>

#include <sys/types.h>


namespace bgws {


class CheckUserAdminExecution : public boost::enable_shared_from_this<CheckUserAdminExecution>
{
public:

    CheckUserAdminExecution(
            const std::string& username,
            CheckUserAdminExecutor::CompleteCallback complete_callback,
            const boost::filesystem::path& exe_path,
            const std::string& properties_filename,
            utility::ChildProcesses& child_processes
        );

    void start();


private:

    std::string _username;
    CheckUserAdminExecutor::CompleteCallback _complete_callback;
    boost::filesystem::path _exe_path;
    std::string _properties_filename;

    utility::EasyChildProcess::Ptr _process_ptr;


    void _handleProcessEnded(
            const bgq::utility::ExitStatus& exit_status
        );

};


}

#endif
