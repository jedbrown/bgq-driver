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

#ifndef BGWS_UTILITY_CHILD_PROCESSES_HPP_
#define BGWS_UTILITY_CHILD_PROCESSES_HPP_


#include "ChildProcess.hpp"
#include "EasyChildProcess.hpp"

#include <utility/include/ExitStatus.h>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <string>
#include <map>

#include <sys/types.h>


namespace bgws {
namespace utility {


class ChildProcesses : boost::noncopyable
{
public:


    typedef boost::function<void ( const bgq::utility::ExitStatus& exit_status )> ProcessEndCallback;


    ChildProcesses(
            boost::asio::io_service& io_service
        );

    ChildProcess::Ptr create(
            const std::string& name,
            const boost::filesystem::path& executable,
            const ChildProcess::Args& args = ChildProcess::Args()
        );

    EasyChildProcess::Ptr createEasy(
            const std::string& name,
            const boost::filesystem::path& executable,
            const ChildProcess::Args& args = ChildProcess::Args()
        );

    void notifySigChld();

    void waitForProcessToEnd(
            pid_t child_pid,
            const ProcessEndCallback& process_end_callback
        );


private:

    typedef std::map<pid_t,bgq::utility::ExitStatus> _EndedProcesses;
    typedef std::map<pid_t,ProcessEndCallback> _PidToCallback;


    boost::asio::io_service &_io_service;
    boost::asio::strand _strand;

    _EndedProcesses _ended_processes;
    _PidToCallback _pid_to_callback;


    void _childEnded(
            pid_t child_pid,
            const bgq::utility::ExitStatus& exit_status
        );

    void _notifySigChldImpl();

    void _waitForProcessToEndImpl(
            pid_t child_pid,
            const ProcessEndCallback& process_end_callback
        );

};


} // namespace bgws::utility
} // namespace bgws

#endif
