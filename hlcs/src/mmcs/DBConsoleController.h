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

#ifndef DB_CONSOLE_CONTROLLER_H_
#define DB_CONSOLE_CONTROLLER_H_

#include "ConsoleController.h"
#include "MMCSThread.h"
#include "MMCSCommandReply.h"

#include <utility/include/UserId.h>

#include <deque>
#include <list>
#include <map>
#include <stdexcept>
#include <string>

class DBConsoleController;
class DBBlockController;
class MMCSConsolePortClient;

typedef std::map<std::string, BlockHelperPtr> BlockMap;
typedef std::map<std::string, pthread_t> AllocateMap;
typedef std::list<DBConsoleController*> MidplaneControllerList;

typedef boost::shared_ptr<DBBlockController> DBBlockPtr;

/*!
 * \brief control a midplane.
 */
class DBConsoleController: public ConsoleController
{
    friend class MMCSCommand_list_blocks;
    friend class MMCSCommand_list_users;
public:
    /*!
     * \brief ctor.
     */
    DBConsoleController(
            MMCSCommandProcessor* commandProcessor,     //!< [in]
            const bgq::utility::UserId& user,           //!< [in]
            CxxSockets::UserType utype = CxxSockets::Normal,
            MMCSConsolePortClient* connection=NULL      //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~DBConsoleController();

    void run();
private:
    void   serviceCommands();

    //
    // Block selection and deselection
    //
public:
    DBBlockPtr selectBlock(std::deque<std::string> args, MMCSCommandReply& reply, bool allocated, bool trustme = false);
    void deselectBlock();

    //
    // Target set selection and deselection
    //
public:
    const DBBlockPtr selectTargetset(std::deque<std::string> args, MMCSCommandReply& reply, bool exists);
    void deselectTargetset() { deselectBlock(); }

public:
    MMCSThread* getMMCSThread() { return _mmcsThread; }
    void setMMCSThread(MMCSThread* mmcsThread) { _mmcsThread = mmcsThread; }
    static bool setAllocating(const std::string& blockName);
    static void doneAllocating(const std::string& blockName);
    static void printAllocMap();

private:
    MMCSThread* _mmcsThread;	// for debugging mmcs_server - thread using this DBConsoleController

private:
    static PthreadMutex       _midplaneControllerListMutex;    // for serializing access to MidplaneControllerList
    static MidplaneControllerList  _midplaneControllerList;    // keeps track of all DBMidplaneControllers

public:
    static DBBlockPtr findBlock(std::string blockId);
    static BlockMap& getBlockMap() { return _blockMap; }
    static PthreadMutex& getBlockMapMutex() { return _blockMapMutex; }
private:
    DBBlockPtr addBlock(std::string blockId, DBBlockPtr& block);
    DBBlockPtr removeBlock(std::string blockId);
    static bool               allocateBlocked(std::string blockName);
    static PthreadMutex       _blockMapMutex;    // for serializing access to BlockMap
    static BlockMap           _blockMap;         // maps a blockId to a DBBlockController
    static PthreadMutex       _allocateMapMutex;
    static AllocateMap        _allocateMap;
};

#endif
