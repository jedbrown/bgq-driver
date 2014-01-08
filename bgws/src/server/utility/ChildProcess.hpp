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

#ifndef BGWS_UTILITY_CHILD_PROCESS_HPP_
#define BGWS_UTILITY_CHILD_PROCESS_HPP_


#include "Pipe.hpp"

#include <utility/include/ExitStatus.h>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <string>
#include <vector>

#include <signal.h>

#include <sys/types.h>


namespace bgws {
namespace utility {


class ChildProcesses;


class ChildProcess : public boost::enable_shared_from_this<ChildProcess>
{
public:

    typedef boost::shared_ptr<ChildProcess> Ptr;
    typedef boost::shared_ptr<const ChildProcess> ConstPtr;

    typedef std::vector<std::string> Args;

    typedef boost::function<void ()> PreExecFn;

    typedef boost::function<void ( const bgq::utility::ExitStatus& exit_status )> EndedCallback;


    ChildProcess(
            const std::string& name,
            const boost::filesystem::path& exe,
            const Args& args,
            ChildProcesses& child_processes,
            boost::asio::io_service& io_service
        );

    const std::string& getName() const  { return _name; }


    void setPreExecFn( const PreExecFn& pre_exec_fn );


    // returns the SdPtr for the subprocess's stdin so can send data.
    Pipe::SdPtr start(
            EndedCallback ended_cb
        );

    pid_t getPid() const  { return _pid; }
    boost::asio::posix::stream_descriptor& out()  { return *_stdout_in_sd_ptr; }
    boost::asio::posix::stream_descriptor& err()  { return *_stderr_in_sd_ptr; }

    void kill( int sig = SIGTERM );


private:

    std::string _name;
    boost::filesystem::path _exe;
    Args _args;

    ChildProcesses &_child_processes;
    boost::asio::io_service &_io_service;

    PreExecFn _pre_exec_fn;

    EndedCallback _ended_cb;

    pid_t _pid;

    Pipe::SdPtr _stdout_in_sd_ptr;
    Pipe::SdPtr _stderr_in_sd_ptr;


    void _childProcess(
            Pipe& stdin_pipe,
            Pipe& stdout_pipe,
            Pipe& stderr_pipe
        );

    Pipe::SdPtr _mainProcess(
            Pipe& stdin_pipe,
            Pipe& stdout_pipe,
            Pipe& stderr_pipe
        );

    void _handleProcessEnded(
            const bgq::utility::ExitStatus& exit_status
        );

};


std::ostream& operator<<( std::ostream& os, const ChildProcess& cp );

std::ostream& operator<<( std::ostream& os, const ChildProcess::ConstPtr& cp_ptr );

std::ostream& operator<<( std::ostream& os, const ChildProcess::Ptr& cp_ptr );


} } // namespace bgws::utility


#endif
