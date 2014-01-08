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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

/*!
 * \file DBBlockController.h
 */

#ifndef DB_BLOCK_CONTROLLER_H
#define DB_BLOCK_CONTROLLER_H

#include <vector>
#include <string>
#include <deque>
#include <memory>
#include <map>
#include <boost/thread.hpp>
#include "BlockHelper.h"
#include "BlockControllerBase.h"
#include "CheckBootCompleteThread.h"
#include "PerformanceCounters.h"
#include <db/include/api/job/types.h>
#include <db/include/api/tableapi/TxObject.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

class DBBlockController: public BlockHelper
{
public:
    DBBlockController(std::string userName, std::string blockName);
    // Already have a base to initialize
    DBBlockController(BlockPtr ptr);
    ~DBBlockController();
    void Die();
    void                          allocateBlock(std::deque<std::string> args, MMCSCommandReply& reply);
    void                          freeBlock(std::deque<std::string> args, MMCSCommandReply& reply);
    void                          waitBoot(std::deque<std::string> args, MMCSCommandReply& reply, bool interactive = false, bool rebooting = false);
    void                          initMachineConfig(MMCSCommandReply& reply);
    static std::string            strDBError(int result);
    PerformanceCounters&          counters() { return _counters; }

    // overridden from BlockController
    void                          create_block(std::deque<std::string> args, MMCSCommandReply& reply);
    void                          boot_block(std::deque<std::string> args, MMCSCommandReply& reply);
    void static                   sendPending();
    void static                   addPendingKill(BGQDB::job::Id jobid, int signal) { _pending_kills[jobid] = signal; }
    BGQDB::STATUS                 setBlockStatus(std::string blockname, 
                                                 BGQDB::BLOCK_STATUS status,
                                                 const std::deque<std::string>& option = std::deque<std::string>());
    int                           processRASMessage(RasEvent &rasEvent);
    void                          postProcessRASMessage(int recid);
    void                          processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage);
    void                          disconnect(deque<string>& args);
    std::string                   getBlockName() { return getBase()->_blockName; }
    void                          startBootCheck(BlockControllerTarget* target) { _cbc._target = target; _cbc.start(); }
    time_t                        getAllocateStartTime() { return _allocate_block_start; }
    void                          setAllocateStartTime(time_t& time) { _allocate_block_start = time; }
protected:
    // members
    time_t                              _allocate_block_start;
    BGQDB::DBTEventlog                  dbe;
    BGQDB::DBTJob_history               _dbj;
    SQLHANDLE                           _sqlstmt;
    SQLHANDLE                           _sqlquery;
    std::auto_ptr<BGQDB::TxObject>      _tx;
    CheckBootComplete                   _cbc;
    PerformanceCounters                 _counters;
    static std::map<BGQDB::job::Id,int> _pending_kills;
    boost::mutex                        _set_state_mutex;
    uint32_t                            _rasInsertionCount;
    boost::posix_time::time_duration    _rasInsertionTime;
    static const std::set<int>          _addNeighborInfo;
};

typedef boost::shared_ptr<DBBlockController> DBBlockPtr;

#endif
