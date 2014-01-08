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


#include "common/Thread.h"

#include "runjob/commands/KillJob.h"

#include <db/include/api/job/types.h>

#include <utility/include/cxxsockets/types.h>
#include <utility/include/Singleton.h>

#include <boost/thread.hpp>

#include <map>


namespace mmcs {
namespace server {


/*!
 * \brief This object manages the single connection to runjob_server
 */
class RunJobConnection : public bgq::utility::Singleton<RunJobConnection>
{
public:
    RunJobConnection();

    bool Connect();

    bool isConnected() const { return _valid; }

    void Kill(
            const BGQDB::job::Id jobid,
            const int signal,
            const int recid=0
            );

    bool CheckViability();

private:
    int Send(
            const CxxSockets::Message& request,
            CxxSockets::Message& response,
            const uint32_t tag,
            const unsigned int message_length
            );

    void sendPending();

private:
    typedef std::map<BGQDB::job::Id,int> Container;

private:
    CxxSockets::SecureTCPSocketPtr _sock;
    bool _valid;
    Container _pending_kills;
    boost::mutex _lock;
};

//! \brief Thread to monitor and restart the connection.
class RunJobConnectionMonitor : public common::Thread
{
public:
    void* threadStart();
    RunJobConnectionMonitor();
};


} } // namespace mmcs::server

#endif
