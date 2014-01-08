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

#ifndef BGWS_CHECK_USER_ADMIN_EXECUTOR_HPP_
#define BGWS_CHECK_USER_ADMIN_EXECUTOR_HPP_


#include "DynamicConfiguration.hpp"
#include "UserInfo.hpp"

#include "utility/ChildProcesses.hpp"

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <string>


namespace bgws {


class CheckUserAdminExecutor : boost::noncopyable
{
public:

    typedef boost::function<void ( UserInfo::UserType user_type )> CompleteCallback;


    CheckUserAdminExecutor(
            utility::ChildProcesses& child_processes,
            boost::asio::io_service& io_service,
            DynamicConfiguration::ConstPtr dynamic_configuration_ptr
        ) :
            _child_processes(child_processes),
            _io_service(io_service),
            _dynamic_configuration_ptr(dynamic_configuration_ptr)
    { /* Nothing to do */ }


    void start(
            const std::string& username,
            CompleteCallback cb
        );


    void setNewDynamicConfiguration( DynamicConfiguration::ConstPtr dynamic_configuration_ptr )  { _dynamic_configuration_ptr = dynamic_configuration_ptr; }


private:

    utility::ChildProcesses &_child_processes;
    boost::asio::io_service &_io_service;

    DynamicConfiguration::ConstPtr _dynamic_configuration_ptr;

};

} // namespace bgws

#endif
