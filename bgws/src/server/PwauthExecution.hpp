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

#ifndef BGWS_PWAUTH_EXECUTION_HPP_
#define BGWS_PWAUTH_EXECUTION_HPP_


#include "PwauthExecutor.hpp"

#include "utility/ChildProcesses.hpp"
#include "utility/EasyChildProcess.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>

#include <string>


namespace bgws {


class PwauthExecution : public boost::enable_shared_from_this<PwauthExecution>
{
public:

    PwauthExecution(
            const std::string& username,
            const std::string& password,
            const PwauthExecutor::CompleteCallback& complete_callback,
            const boost::filesystem::path& user_auth_exe_path,
            utility::ChildProcesses& child_processes
        );


    void start();


    ~PwauthExecution();


private:

    PwauthExecutor::CompleteCallback _complete_callback;

    std::string _write_msg;

    utility::EasyChildProcess::Ptr _process_ptr;


    void _handleProcessEnded(
            const bgq::utility::ExitStatus& exit_status
        );

};


} // namespace bgws

#endif
