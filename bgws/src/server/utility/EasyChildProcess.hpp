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

#ifndef BGWS_UTILITY_EASY_CHILD_PROCESS_HPP_
#define BGWS_UTILITY_EASY_CHILD_PROCESS_HPP_


#include "ChildProcess.hpp"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <string>


namespace bgws {
namespace utility {


class ChildProcesses;


class EasyChildProcess : public boost::enable_shared_from_this<EasyChildProcess>
{
public:

    typedef boost::shared_ptr<EasyChildProcess> Ptr;


    enum class OutputType {
        Out,
        Err
    };

    enum class OutputIndicator {
        Normal,
        End,
        Error
    };

    typedef boost::function<void ( OutputType out_type, OutputIndicator ind, const std::string& line )> LineCallback;


    EasyChildProcess(
            const std::string& name,
            const boost::filesystem::path& exe,
            const ChildProcess::Args& args,
            ChildProcesses& child_processes,
            boost::asio::io_service& io_service
        );

    void setPreExecFn( const ChildProcess::PreExecFn& pre_exec_fn );

    void start(
            ChildProcess::EndedCallback end_cb,
            LineCallback line_cb = LineCallback(),
            const std::string& input_text = std::string()
        );

    void start(
            ChildProcess::EndedCallback end_cb,
            const std::string& input_text
        );

    void kill( int sig = SIGTERM )  { _process_ptr->kill( sig ); }


private:

    ChildProcess::Ptr _process_ptr;

    utility::Pipe::SdPtr _stdin_sd_ptr;
    std::string _write_msg;

    LineCallback _line_cb;

    bool _out_ended;
    bool _err_ended;

    boost::asio::streambuf _stdout_buf;
    std::istream _stdout_is;
    boost::asio::streambuf _stderr_buf;
    std::istream _stderr_is;


    void _handleStderrLine(
            const boost::system::error_code& ec,
            std::size_t length
        );

    void _handleStdoutLine(
            const boost::system::error_code& ec,
            std::size_t length
        );

    void _handleWroteMsg(
            const boost::system::error_code& ec
        );


    friend std::ostream& operator<<( std::ostream& os, const EasyChildProcess& ecp );
};


std::ostream& operator<<( std::ostream& os, const EasyChildProcess& ecp );

std::ostream& operator<<( std::ostream& os, const EasyChildProcess::Ptr& ecp_ptr );


} } // namespace bgws::utility


#endif
