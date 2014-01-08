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

#ifndef MMCS_SERVER_DB_BLOCK_CONTROLLER_H_
#define MMCS_SERVER_DB_BLOCK_CONTROLLER_H_

#include "BlockHelper.h"

#include "CheckBootComplete.h"
#include "PerformanceCounters.h"

#include "libmmcs_client/CommandReply.h"

#include <db/include/api/tableapi/TxObject.h>

#include <db/include/api/tableapi/gensrc/DBTEventlog.h>
#include <db/include/api/tableapi/gensrc/DBTJob_history.h>

#include <boost/date_time.hpp>
#include <boost/thread.hpp>

#include <auto_ptr.h>
#include <deque>
#include <string>

#include <stdint.h>
#include <time.h>

namespace mmcs {
namespace server {

class DBBlockController : public BlockHelper
{
public:
    DBBlockController(
            const std::string& user,
            const std::string& block
            );

    DBBlockController(
            BlockPtr ptr
            );

    ~DBBlockController();

    void                          Die();
    void                          allocateBlock(std::deque<std::string> args, mmcs_client::CommandReply& reply);
    void                          freeBlock(std::deque<std::string> args, mmcs_client::CommandReply& reply);
    void                          waitBoot(std::deque<std::string> args, mmcs_client::CommandReply& reply, bool interactive = false, bool rebooting = false);
    void                          waitFree(mmcs_client::CommandReply& reply);
    void                          initMachineConfig(mmcs_client::CommandReply& reply);
    static const std::string&     strDBError(BGQDB::STATUS result);
    PerformanceCounters&          counters() { return _counters; }

    void                          create_block(std::deque<std::string> args, mmcs_client::CommandReply& reply);
    void                          boot_block(std::deque<std::string> args, mmcs_client::CommandReply& reply);

    BGQDB::STATUS                 setBlockStatus(
            BGQDB::BLOCK_STATUS status,
            const std::deque<std::string>& option = std::deque<std::string>()
            );
    int                           processRASMessage(RasEvent &rasEvent);
    void                          postProcessRASMessage(int recid);
    void                          processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage);
    void                          disconnect(const std::deque<std::string>& args, mmcs_client::CommandReply& reply);
    const std::string&            getBlockName() const;
    void                          startBootCheck(BlockControllerTarget* target) { _cbc._target = target; _cbc.start(); }
    void                          setAllocateStartTime(time_t time) { _allocate_block_start = time; }

private:
    void abnormalComputeShutdown();

protected:
    time_t                              _allocate_block_start;
    BGQDB::DBTEventlog                  dbe;
    BGQDB::DBTJob_history               _dbj;
    SQLHANDLE                           _sqlstmt;
    SQLHANDLE                           _sqlquery;
    std::auto_ptr<BGQDB::TxObject>      _tx;
    CheckBootComplete                   _cbc;
    PerformanceCounters                 _counters;
    boost::mutex                        _status_mutex;
    boost::condition_variable           _status_notifier;
    uint32_t                            _rasInsertionCount;
    boost::posix_time::time_duration    _rasInsertionTime;
};

} } // namespace mmcs::server

#endif
