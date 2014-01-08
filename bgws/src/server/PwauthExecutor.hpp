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

#ifndef BGWS_PWAUTH_EXECUTOR_HPP_
#define BGWS_PWAUTH_EXECUTOR_HPP_


#include "DynamicConfiguration.hpp"
#include "types.hpp"

#include "utility/ChildProcesses.hpp"

#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <string>


namespace bgws {


class PwauthExecutor : boost::noncopyable
{
public:

    typedef boost::function<void ( bool authenticated )> CompleteCallback;


    PwauthExecutor(
            utility::ChildProcesses& child_processes,
            DynamicConfiguration::ConstPtr dynamic_configuration_ptr
        );

    void setNewDynamicConfiguration( DynamicConfiguration::ConstPtr dynamic_configuration_ptr )  { _dynamic_configuration_ptr = dynamic_configuration_ptr; }

    void start(
            const std::string& username,
            const std::string& password,
            const CompleteCallback& completed_callback
        );


private:

    utility::ChildProcesses &_child_processes;
    DynamicConfiguration::ConstPtr _dynamic_configuration_ptr;

};


} // namespace bgws


#endif
