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

#ifndef MASTER_MONITOR_H
#define MASTER_MONITOR_H

#include "common/Thread.h"
#include "WaiterList.h"

#include "master/lib/BGMasterClient.h"

#include <control/include/mcServer/MCServerRef.h>

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <map>
#include <string>
#include <vector>

namespace mmcs {
namespace server {
namespace master {

class AliasWaiter;
class SubnetWaiter;

class Monitor : public common::Thread
{
    friend class AliasWaiter;
    friend class SubnetWaiter;
public:
    typedef std::map<std::string,BinaryId> AliasMap;

public:
    // This will monitor multiple binary waiter threads.
    void* threadStart();
    void setOptions(const std::vector<std::string>& opts) { _bringup_options = opts; }
    // map aliases to binary ids at START.  Only check it right after START.
    // We don't maintain this long term.
    static AliasMap _alias_binary_map;
    //! \brief Condition variable to notify mmcs_server.cc that the master monitor is started.
    static boost::condition_variable _startup_notifier;
    //! \brief Lock for the status condition variable.
    static boost::mutex _startup_lock;
    static bool _started;
private:
    void static start_terminate_waiter(const AliasWaiter* aw, const std::string& alias);
    bool static connect_mc_server();
    static WaiterList _waitlist;
    static MCServerRefPtr _server;
    static std::vector<std::string> _bringup_options;
};

} } } // namespace mmcs::server::master

#endif
