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

#include "CommandClient.h"

#include "Status.h"

#include "common/common.h"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using bgq::utility::LoggingProgramOptions;

using boost::bind;

using std::exception;
using std::istream;
using std::ostringstream;
using std::getline;
using std::string;
using std::vector;

LOG_DECLARE_FILE( "realtime.server" );

namespace realtime {
namespace server {

CommandClient::CommandClient(
        bgq::utility::portConfig::SocketPtr socket_ptr,
        Status& status
    ) :
        _socket_ptr(socket_ptr),
        _status(status),
        _strand(_socket_ptr->get_io_service()),
        _error(false),
        _writing(false)
{
    // Nothing to do.
}

void CommandClient::start()
{
    _writing = false;

    _startRead();
}

void CommandClient::_startRead()
{
    boost::asio::async_read_until(
            *_socket_ptr,
            _in_sb,
            '\n',
            _strand.wrap( bind(
                    &CommandClient::_handleReadLine,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                ) )
        );
}

void CommandClient::_handleMsgStr(
        const std::string& msg_str
    )
{
    // split the message string on spaces.

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    boost::char_separator<char> sep( " " );
    tokenizer tokens( msg_str, sep );

    if ( tokens.begin() == tokens.end() ) {
        // empty line.
        return;
    }

    string msg_name(*tokens.begin());
    _Strings args( ++tokens.begin(), tokens.end() );

    if ( msg_name == common::StatusCommandName ) {
        _status.requestStatus( _strand.wrap( bind( &CommandClient::_gotStatus, shared_from_this(), _1 ) ) );
        return;
    }

    if ( msg_name == common::LoggingCommandName ) {
        _setLogging( args );
        return;
    }

    LOG_WARN_MSG( "Invalid command '" << msg_name << "'" );
    _newOutMsg( "invalid_command" );
}

void CommandClient::_handleReadLine(
        const boost::system::error_code& err,
        size_t //bytes_transferred
    )
{
    if ( _error ) {
        return;
    }

    if ( err ) {
        LOG_DEBUG_MSG( "read error: " << err.message() );
        _error = true;
        return;
    }

    istream is( &_in_sb );

    string msg_str;

    getline( is, msg_str );

    LOG_INFO_MSG( "Command: " << msg_str );

    _handleMsgStr( msg_str );

    _startRead();
}

void CommandClient::_newOutMsg(
        const std::string& msg_str
    )
{
    _out_msgs.push_back( msg_str + "\n" );

    _startWriting();
}

void CommandClient::_startWriting()
{
    if ( _writing ) {
        return;
    }

    if ( _out_msgs.empty() ) {
        return;
    }

    _writing = true;

    boost::asio::async_write(
            *_socket_ptr,
            boost::asio::buffer( _out_msgs.front() ),
            _strand.wrap( bind(
                    &CommandClient::_wroteMessage,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                ) )
        );
}

void CommandClient::_wroteMessage(
        const boost::system::error_code& err,
        size_t //bytes_transferred
    )
{
    if ( _error ) {
        return;
    }

    if ( err ) {
        LOG_DEBUG_MSG( "write error: " << err.message() );
        _error = true;
        return;
    }

    _writing = false;

    _out_msgs.pop_front();

    _startWriting();
}

void CommandClient::_gotStatus(
        Status::Details status_details
    )
{
    ostringstream oss;
    oss << "running " << status_details.connected_clients <<
            (status_details.db_changes_monitor_state == DbChangesMonitor::State::Idle ? "idle" :
             status_details.db_changes_monitor_state == DbChangesMonitor::State::Monitoring ? "monitoring" :
             status_details.db_changes_monitor_state == DbChangesMonitor::State::MaxXact ? "maxXact" : "unknown") <<
            " " << status_details.db2_version;

    _newOutMsg( oss.str() );
}

void CommandClient::_setLogging(
        const _Strings& logging_strs
    )
{
    string fail_msg;

    try {
        LOG_INFO_MSG_FORCED( "Setting logging level." );

        LoggingProgramOptions logging_program_options(
                "ibm.realtime.server"
            );

        logging_program_options.notifier(
                logging_strs
            );

        logging_program_options.apply();

        _newOutMsg( "success" );
        return;
    } catch ( exception& e ) {
        LOG_WARN_MSG( "Failed to set logging level, error=" << e.what() );
        fail_msg = e.what();
    }

    _newOutMsg( string() + "failed \"" + fail_msg + "\"" );
}

} // namespace realtime::server
} // namespace realtime
