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

#include "PwauthExecutor.hpp"

#include "PwauthExecution.hpp"

#include <utility/include/Log.h>

#include <boost/shared_ptr.hpp>

#include <stdexcept>


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


PwauthExecutor::PwauthExecutor(
        utility::ChildProcesses& child_processes,
        DynamicConfiguration::ConstPtr dynamic_configuration_ptr
    ) :
        _child_processes(child_processes),
        _dynamic_configuration_ptr(dynamic_configuration_ptr)
{
    // Nothing to do.
}


void PwauthExecutor::start(
        const std::string& username,
        const std::string& password,
        const CompleteCallback& completed_callback
    )
{
    LOG_INFO_MSG( "Starting pwauth for user " << username );

    DynamicConfiguration::ConstPtr dynamic_configuration_ptr(_dynamic_configuration_ptr);

    boost::shared_ptr<PwauthExecution> pwauth_execution_ptr( new PwauthExecution(
            username,
            password,
            completed_callback,
            dynamic_configuration_ptr->getUserAuthExecutable(),
            _child_processes
        ) );

    pwauth_execution_ptr->start();
}


} // namespace bgws
