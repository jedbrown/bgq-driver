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

#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <utility/include/cxxsockets/SocketTypes.h>
#include <hlcs/include/runjob/commands/KillJob.h>
#include <db/include/api/job/types.h>
#include "MMCSThread.h"

//! \brief This static object manages the single connection to runjob_server
class RunJobConnection {
    static CxxSockets::SecureTCPSocketPtr _sock;
    static bool _valid;
    static boost::mutex _lock;
public:
    RunJobConnection() {}
    bool static Connect();
    bool static isConnected() { return _valid; }
    int  static Kill(const BGQDB::job::Id jobid, const int signal, const int recid=0);
    bool static CheckViability();
private:
    int static Send(CxxSockets::Message& request, CxxSockets::Message& response, const uint32_t tag, const unsigned int message_length);
};

//! \brief Thread to monitor and restart the connection.
class RunJobConnectionMonitor : public MMCSThread {
public:
    void* threadStart();
    RunJobConnectionMonitor() : _runjob_start_barrier(2) {}
    boost::barrier _runjob_start_barrier;
};
