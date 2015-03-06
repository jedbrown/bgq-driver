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

#ifndef REALTIME_SERVER_COMMAND_CLIENT_H_
#define REALTIME_SERVER_COMMAND_CLIENT_H_

#include "Status.h"

#include <utility/include/portConfiguration/types.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <deque>
#include <string>
#include <vector>

namespace realtime {
namespace server {

class CommandClient : public boost::enable_shared_from_this<CommandClient>
{
public:

    typedef boost::shared_ptr<CommandClient> Ptr;

    CommandClient(
            bgq::utility::portConfig::SocketPtr socket_ptr,
            Status& status
        );

    void start();

private:

    typedef std::deque<std::string> _OutMsgs;

    typedef std::vector<std::string> _Strings;

    bgq::utility::portConfig::SocketPtr _socket_ptr;
    Status &_status;

    boost::asio::strand _strand;

    bool _error;
    bool _writing;

    boost::asio::streambuf _in_sb;

    _OutMsgs _out_msgs;


    void _handleMsgStr(
            const std::string& msg_str
        );

    void _startRead();

    void _handleReadLine(
            const boost::system::error_code& err,
            size_t bytes_transferred
        );

    void _newOutMsg(
            const std::string& msg_str
        );

    void _startWriting();

    void _wroteMessage(
            const boost::system::error_code& err,
            size_t bytes_transferred
        );

    void _gotStatus(
            Status::Details status_details
        );

    void _setLogging(
            const _Strings& logging_strs
        );
};


/*! \page commandClient Command clients

Command clients send commands to the RTS.

The RTS supports the status and logging commands.
The \ref statusUtility and \ref loggingUtility generate these requests.

 */

} // namespace realtime::server
} // namespace realtime

#endif
