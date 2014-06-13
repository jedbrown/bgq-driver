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

#ifndef MMCS_SERVER_DB_CONSOLE_CONTROLLER_H_
#define MMCS_SERVER_DB_CONSOLE_CONTROLLER_H_


#include "common/ConsoleController.h"

#include "types.h"

#include "libmmcs_client/CommandReply.h"

#include <bgq_util/include/pthreadmutex.h>
#include <extlib/include/log4cxx/mdc.h>
#include <utility/include/UserId.h>

#include <boost/scoped_ptr.hpp>

#include <deque>
#include <list>
#include <map>
#include <string>

#include <pthread.h>

namespace mmcs {
namespace common {

class Thread;

} // common

namespace server {

namespace command {
    class ListBlocks;
    class ListUsers;
    class Status;
}

typedef std::map<std::string, BlockHelperPtr> BlockMap;
typedef std::map<std::string, pthread_t> AllocateMap;
typedef std::list<DBConsoleController*> ConsoleControllerList;

/*!
 * \brief Control a console.
 */
class DBConsoleController : public common::ConsoleController
{
    friend class command::ListBlocks;
    friend class command::ListUsers;
    friend class command::Status;
public:
    /*!
     * \brief ctor.
     */
    DBConsoleController(
            MMCSCommandProcessor* commandProcessor,     //!< [in]
            const bgq::utility::UserId& user,           //!< [in]
            CxxSockets::UserType utype = CxxSockets::Normal,
            mmcs_client::ConsolePortClient* connection=NULL      //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~DBConsoleController();

    void run();
private:
    void serviceCommands();

public:
    DBBlockPtr selectBlock(std::deque<std::string> args, mmcs_client::CommandReply& reply, bool allocated);
    void deselectBlock();

public:
    common::Thread* getThread() const { return _mmcsThread; }
    const std::string& getPeerName() const { return _peerName; }
    void setMMCSThread(common::Thread* mmcsThread) { _mmcsThread = mmcsThread; }
    void setPeerName(const std::string& name) { _peerName = name; }
    static bool setAllocating(const std::string& blockName);
    static void doneAllocating(const std::string& blockName);

private:
    common::Thread* _mmcsThread; // for debugging mmcs_server - thread using this DBConsoleController
    std::string _peerName;
    boost::scoped_ptr<log4cxx::MDC> _blockMdc;

private:
    static PthreadMutex       _consoleControllerListMutex;    // for serializing access to ConsoleControllerList
    static ConsoleControllerList  _consoleControllerList;    // keeps track of all DBConsoleControllers

public:
    static DBBlockPtr findBlock(const std::string& blockId);
    static BlockMap& getBlockMap() { return _blockMap; }
    static PthreadMutex& getBlockMapMutex() { return _blockMapMutex; }
private:
    DBBlockPtr addBlock(const std::string& blockId, const DBBlockPtr& block);
    DBBlockPtr removeBlock(const std::string& blockId);
    static bool               allocateBlocked(const std::string& blockName);
    static PthreadMutex       _blockMapMutex;    // for serializing access to BlockMap
    static BlockMap           _blockMap;         // maps a blockId to a DBBlockController
    static PthreadMutex       _allocateMapMutex;
    static AllocateMap        _allocateMap;
};

} } // namespace mmcs::server

#endif
