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

#include "EasyChildProcess.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>

#include <iostream>
#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace utility {


EasyChildProcess::EasyChildProcess(
        const std::string& name,
        const boost::filesystem::path& exe,
        const ChildProcess::Args& args,
        ChildProcesses& child_processes,
        boost::asio::io_service& io_service
    ) :
        _process_ptr( new ChildProcess (
                name,
                exe,
                args,
                child_processes,
                io_service
            ) ),
        _out_ended(false),
        _err_ended(false),
        _stdout_is( &_stdout_buf ),
        _stderr_is( &_stderr_buf )
{
    // Nothing to do.
}


void EasyChildProcess::setPreExecFn( const ChildProcess::PreExecFn& pre_exec_fn )
{
    _process_ptr->setPreExecFn( pre_exec_fn );
}


void EasyChildProcess::start(
        ChildProcess::EndedCallback end_cb,
        LineCallback line_cb,
        const std::string& input_text
    )
{
    _line_cb = line_cb;

    _stdin_sd_ptr = _process_ptr->start( end_cb );

    if ( input_text == string() ) {
        // Nothing to send.
        _stdin_sd_ptr.reset();
    } else {
        // I've got some input to send to the child process.
        _write_msg = input_text;

        boost::asio::async_write(
                *_stdin_sd_ptr,
                boost::asio::buffer( _write_msg ),
                boost::bind(
                        &EasyChildProcess::_handleWroteMsg,
                        shared_from_this(),
                        boost::asio::placeholders::error
                    )
            );
    }


    // read whatever it writes to stdout.

    boost::asio::async_read_until(
            _process_ptr->out(),
            _stdout_buf,
            '\n',
            boost::bind(
                    &EasyChildProcess::_handleStdoutLine,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
        );

    // read whatever it writes to stderr.

    boost::asio::async_read_until(
            _process_ptr->err(),
            _stderr_buf,
            '\n',
            boost::bind(
                    &EasyChildProcess::_handleStderrLine,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
        );

}


void EasyChildProcess::start(
        ChildProcess::EndedCallback end_cb,
        const std::string& input_text
    )
{
    start(
            end_cb,
            LineCallback(),
            input_text
        );
}


void EasyChildProcess::_handleStderrLine(
        const boost::system::error_code& ec,
        std::size_t // length
    )
{
    if ( ec ) {
        if ( ec == boost::asio::error::eof ) {
            LOG_DEBUG_MSG( _process_ptr << " closed stderr" );
            if ( _line_cb )  _line_cb( OutputType::Err, OutputIndicator::End, string() );
            _err_ended = true;
            if ( _out_ended && _err_ended )  _line_cb = LineCallback();
            return;
        }

        LOG_WARN_MSG( _process_ptr << " error reading stderr. ec=" << ec );

        if ( _line_cb )  _line_cb( OutputType::Err, OutputIndicator::Error, string() );
        _err_ended = true;
        if ( _out_ended && _err_ended )  _line_cb = LineCallback();

        return;
    }

    string line;

    std::getline( _stderr_is, line );

    LOG_WARN_MSG( _process_ptr << " error: " << line );

    if ( _line_cb )  _line_cb( OutputType::Err, OutputIndicator::Normal, line );

    // keep reading.
    boost::asio::async_read_until(
            _process_ptr->err(),
            _stderr_buf,
            '\n',
            boost::bind(
                    &EasyChildProcess::_handleStderrLine,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
        );
}


void EasyChildProcess::_handleStdoutLine(
        const boost::system::error_code& ec,
        std::size_t // length
    )
{
    if ( ec ) {
        if ( ec == boost::asio::error::eof ) {
            LOG_DEBUG_MSG( _process_ptr << " closed stdout" );
            if ( _line_cb )  _line_cb( OutputType::Out, OutputIndicator::End, string() );
            _out_ended = true;
            if ( _out_ended && _err_ended )  _line_cb = LineCallback();
            return;
        }

        LOG_WARN_MSG( _process_ptr << " error reading stdout. ec=" << ec );

        if ( _line_cb )  _line_cb( OutputType::Out, OutputIndicator::Error, string() );
        _out_ended = true;
        if ( _out_ended && _err_ended )  _line_cb = LineCallback();

        return;
    }

    string line;

    std::getline( _stdout_is, line );

    LOG_INFO_MSG( _process_ptr << " out: " << line );

    if ( _line_cb )  _line_cb( OutputType::Out, OutputIndicator::Normal, line );

    // keep reading.
    boost::asio::async_read_until(
            _process_ptr->out(),
            _stdout_buf,
            '\n',
            boost::bind(
                    &EasyChildProcess::_handleStdoutLine,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
        );
}


void EasyChildProcess::_handleWroteMsg(
        const boost::system::error_code& ec
    )
{
    if ( ec ) {
        LOG_WARN_MSG( _process_ptr << " Writing failed. error = " << ec );
    } else {
        LOG_TRACE_MSG( _process_ptr << " Writing complete." );
    }

    _stdin_sd_ptr.reset(); // close it.
}


std::ostream& operator<<( std::ostream& os, const EasyChildProcess& ecp )
{
    os << ecp._process_ptr;
    return os;
}


std::ostream& operator<<( std::ostream& os, const EasyChildProcess::Ptr& ecp_ptr )
{
    os << *ecp_ptr;
    return os;
}


} } // namespace bgws::utility
