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

#ifndef MMCS_SERVER_RUNJOB_CONNECTION_H_
#define MMCS_SERVER_RUNJOB_CONNECTION_H_

#include "runjob/commands/KillJob.h"

#include <db/include/api/job/types.h>

#include <utility/include/Singleton.h>
#include <utility/include/portConfiguration/Connector.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <deque>

namespace mmcs {
namespace server {

/*!
 * \brief This object manages the single connection to runjob_server.
 *
 * \note This class assumes a single thread invokes io_service::run so there
 * is no strand to serialize access to the data structures accessed by the
 * completion handlers
 */
class RunJobConnection : public bgq::utility::Singleton<RunJobConnection>
{
public:
    RunJobConnection();

    void kill(
            const BGQDB::job::Id jobid,
            const int signal,
            const int recid
            );
    
    void kill(
            const BGQDB::job::Id jobid,
            const int signal,
            const std::string& details
            );

    void stop();

private:
    void connect();

    void connectHandler(
            const boost::shared_ptr<bgq::utility::Connector>& connector,
            const bgq::utility::Connector::ConnectResult& result
            );

    void waitHandler(
            const boost::shared_ptr<boost::asio::deadline_timer>& timer,
            const boost::system::error_code& error
            );

    void killRecidImpl(
            const BGQDB::job::Id job,
            const int signal,
            const int recid
            );
    
    void killDetailsImpl(
            const BGQDB::job::Id job,
            const int signal,
            const std::string& details
            );

    void readHeaderHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

    void readBodyHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

    void write();

    void writeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

private:
    struct PendingSignal {
        BGQDB::job::Id _job;
        int _signal;
        int _recid;
        std::string _details;
    };

    typedef std::deque<PendingSignal> Container;

private:
    bgq::utility::portConfig::SocketPtr _socket;
    boost::asio::io_service _io_service;
    Container _pendingSignals;
    runjob::commands::Header _incomingHeader;
    boost::asio::streambuf _incomingMessage;
    runjob::commands::Header _outgoingHeader;
    boost::asio::streambuf _outgoingMessage;
    boost::thread _thread;
};

} } // namespace mmcs::server

#endif
