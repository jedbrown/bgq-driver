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

#include <hlcs/include/master/BGMasterClientApi.h>
#include <hlcs/include/master/BGMasterExceptions.h>
#include <utility/include/cxxsockets/SocketTypes.h>
#include "MMCSThread.h"
#include "MMCSProperties.h"

#ifndef MASTER_MONITOR_H
#define MASTER_MONITOR_H

class WaiterList {
public:
    enum Status { STOPPED, WAIT_FOR_START, RUNNING, ERROR };
    void init() {
        std::string dont_care = "not_init";
        _used_binids_cap = MMCSProperties::getSubnets(dont_care) + 10;
    }

    Status getStat(const std::string& alias) {
        PthreadMutexHolder _holder(&_list_lock);
        return _list[alias];
    }
    void setStat(const std::string& alias, Status stat);

    // Returns first stopped item we find.
    std::string getStopped() {
        PthreadMutexHolder _holder(&_list_lock);
        for(std::map<std::string, Status>::iterator li = _list.begin();
            li != _list.end(); ++li) {
            if(li->second == STOPPED) {
                return li->first;
            }
        }
        return "";
    }

    std::string getErrored() {
        PthreadMutexHolder _holder(&_list_lock);
        for(std::map<std::string, Status>::iterator li = _list.begin();
            li != _list.end(); ++li) {
            if(li->second == ERROR) {
                return li->first;
            }
        }
        return "";
    }

    void addBinId(std::string binid) { 
        _used_binids.push_back(binid); 
        if(_used_binids.size() > _used_binids_cap) {
            _used_binids.pop_back();
        }
    }
    void removeBinId(std::string& binid) { 
        _used_binids.erase(std::remove(_used_binids.begin(), _used_binids.end(), binid)); 
    }
    bool findBinId(std::string binid) { 
        if(std::find(_used_binids.begin(), _used_binids.end(), binid) != _used_binids.end())
            return true;
        else return false;
    }
private:
    PthreadMutex _list_lock;
    std::map<std::string, Status> _list;
    unsigned _used_binids_cap;
    std::vector<std::string> _used_binids;
};

// Waits for a specific subnet_mc to end
class MasterWaiter : public MMCSThread {
public:
    MasterWaiter() : MMCSThread() {}
    void* threadStart();
    void setBin(BinaryControllerPtr bin) { _bin_to_wait = bin; }
    void setAlias(std::string alias) { _alias = alias; }
private:
    BGMasterClient _client;
    BinaryControllerPtr _bin_to_wait;
    std::string _alias;
};

// Waits for an alias to start
class AliasWaiter : public MMCSThread {
public:
    AliasWaiter(std::string& alias, bool update_mc = false) : MMCSThread(), _alias(alias), _update_mc(update_mc) {}
    void* threadStart();

    static bool sendInitialize(MCServerRefPtr ref, std::vector<std::string>& hw_to_unmark, MCServerMessageSpec::FailoverReply& failrep);

    // This guy gets everything in the fail reply and removes it from the booted hardware 
    // and puts it in the idle hardware.
    static bool actuallySendMessages(MCServerRefPtr ref, MCServerMessageSpec::FailoverReply& failrep, std::vector<std::string>& markAvail);

    static bool buildFailover(MCServerMessageSpec::FailoverRequest& failreq,
                              MCServerMessageSpec::FailoverReply& failrep,
                              std::string subnet_mc,
                              BinaryId& binid);

    static void* updateMcServer(std::string& subnet_mc, std::vector<std::string>& blocks_to_free, std::vector<std::string>& error_list, BinaryId& binid);
    BinaryId getBinId() { return _binid; }
private:
    BGMasterClient _client;
    std::string _alias;
    bool _update_mc;
    BinaryId _binid;
};

class MasterMonitor : public MMCSThread {
    friend class AliasWaiter;
    friend class MasterWaiter;
public:
    // This will monitor multiple binary waiter threads.
    void* threadStart();
    void setOptions(std::vector<std::string>& opts) { _bringup_options = opts; }
    // map aliases to binary ids at START.  Only check it right after START.
    // We don't maintain this long term.
    static std::map<std::string,BinaryId> _alias_binary_map;
    //! \brief Condition variable to notify mmcs_server.cc that the master monitor is started.
    static boost::condition_variable _startup_notifier;
    //! \brief Lock for the status condition variable.
    static boost::mutex _startup_lock;
    static bool _started;
private:
    void static start_terminate_waiter(AliasWaiter* aw, std::string& alias);
    bool static connect_mc_server();
    static WaiterList _waitlist;
    static MCServerRefPtr _server;
    static std::vector<std::string> _bringup_options;
};

#endif
