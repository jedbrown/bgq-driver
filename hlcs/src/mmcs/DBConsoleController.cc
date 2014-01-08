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

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>
#include "utility/include/Log.h"
#include "bgq_util/include/pthreadmutex.h"
#include <db/include/api/BGQDBlib.h>
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSProperties.h"
#include "MMCSCommandProcessor.h"
#include "MMCSConsolePort.h"
#include "IOBlockController.h"
#include "CNBlockController.h"

LOG_DECLARE_FILE( "mmcs" );
#define LOGGING_DECLARE_USER_ID_MDC \
    log4cxx::MDC _userid_mdc_( "user", std::string("[") + _user.getUser() + "] " );

//******************************************************************************
// DBConsoleController
//******************************************************************************
// static member initialization
// Order of mutex locking to avoid deadlock.
// If you must hold both mutexes, always lock them in the order 1, then 2
// 1) DBConsoleController::_blockMapMutex
//    This controls adding to, removing from, or searching the
//    DBConsoleController::_blockMap.
// 2) BlockController:_mutex.
//    This controls access to fields within the a DBBlockController.
//    A thread should never hold this when adding or removing a DBBlockController
//    from the DBConsoleController::_blockMap
PthreadMutex    DBConsoleController::_blockMapMutex(PTHREAD_MUTEX_RECURSIVE_NP);
BlockMap        DBConsoleController::_blockMap;

PthreadMutex    DBConsoleController::_allocateMapMutex(PTHREAD_MUTEX_RECURSIVE_NP);
AllocateMap     DBConsoleController::_allocateMap;

PthreadMutex    DBConsoleController::_midplaneControllerListMutex(PTHREAD_MUTEX_ERRORCHECK_NP);
MidplaneControllerList DBConsoleController::_midplaneControllerList;


DBBlockPtr
DBConsoleController::findBlock(string blockId)
{
    PthreadMutexHolder mutex;
    // serialize this code
    int mutex_rc = mutex.Lock(&_blockMapMutex);
    assert(mutex_rc == 0);

    DBBlockPtr foundBlock;
    if (_blockMap.size() != 0) {
        BlockMap::iterator it = _blockMap.find(blockId);
        if (it != _blockMap.end())
            foundBlock = boost::dynamic_pointer_cast<DBBlockController>(it->second);
    }
    if(foundBlock != 0) {
      // increment the number of users of this block
        foundBlock->getBase()->incrBlockThreads();

    }
    return foundBlock;
}

DBBlockPtr
DBConsoleController::addBlock(string blockId, DBBlockPtr& block)
{
    PthreadMutexHolder mutex;
    // serialize this code
    int mutex_rc = mutex.Lock(&_blockMapMutex);
    assert(mutex_rc == 0);

    DBBlockPtr foundBlock;
    if (_blockMap.size() != 0) {
        BlockMap::iterator it = _blockMap.find(blockId);
        if (it != _blockMap.end())
            foundBlock = boost::dynamic_pointer_cast<DBBlockController>(it->second);
    }
    if (foundBlock == 0) {
        _blockMap[blockId] = block;
        foundBlock = block;
        LOG_INFO_MSG("DBConsoleController::addBlock(" << blockId << ")");
    }

    if(foundBlock != 0) {
        // increment the number of users of this block
        foundBlock->getBase()->incrBlockThreads();
    }

    return foundBlock;
}

DBBlockPtr
DBConsoleController::removeBlock(string blockId)
{
    DBBlockPtr zeroptr;
    PthreadMutexHolder mutex;
    // serialize this code
    int mutex_rc = mutex.Lock(&_blockMapMutex);
    assert(mutex_rc == 0);

    DBBlockPtr foundBlock;

    if (_blockMap.size() != 0) {
        BlockMap::iterator it = _blockMap.find(blockId);
        if (it != _blockMap.end())
            foundBlock = boost::dynamic_pointer_cast<DBBlockController>(it->second);
    }
    if (foundBlock != 0) {
        if(foundBlock->getBase()->getBlockThreads() != 0) {
            LOG_INFO_MSG("Block to remove, " << blockId
                         << ", found but still in use in another thread.");
            // Even though we found a block, we return 0 as if we didn't
            // because some other thread is using it now and we don't want
            // the block controller deleted.
            return zeroptr;
        }
        else {
            _blockMap.erase(blockId);
            LOG_INFO_MSG("DBConsoleController::removeBlock(" << blockId << ")");
        }
    }

    return foundBlock;
}

bool
DBConsoleController::setAllocating(const std::string& blockName)
{
    // Lock the mutex, see if we're already allocating, then
    // create an entry in the map.
    PthreadMutexHolder mutex;
    mutex.Lock(&_allocateMapMutex);
    if (_allocateMap.size() != 0) {
        AllocateMap::iterator it = _allocateMap.find(blockName);
        if (it != _allocateMap.end()) {
            if(_allocateMap[blockName] != 0) {
                return false;  // Can't allocate if we're already allocating
            }
        }
    }
    LOG_INFO_MSG("setAllocating() "
                 << blockName);
    _allocateMap[blockName] = pthread_self();
    return true;
}

void
DBConsoleController::doneAllocating(const std::string& blockName)
{
    // We're done allocating the block.  Clear our place in the map.
    PthreadMutexHolder mutex;
    mutex.Lock(&_allocateMapMutex);
    if(_allocateMap[blockName] != pthread_self()) {
        LOG_ERROR_MSG("Allocate map is in indeterminant state.  Allocate thread " << _allocateMap[blockName]);
    }
    LOG_INFO_MSG("doneAllocating() " << blockName);
    _allocateMap.erase(blockName);
    return;
}

bool
DBConsoleController::allocateBlocked(string blockName)
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_allocateMapMutex);
    // See if there is another thread that has allocation locked out
    if (_allocateMap.size() != 0) {
        AllocateMap::iterator it = _allocateMap.find(blockName);
        if (it != _allocateMap.end()) {
            if(_allocateMap[blockName] != pthread_self()) {
                LOG_WARN_MSG("allocateBlocked() "
                             << blockName << " " << _allocateMap[blockName]);
                return true;
            }
        }
    }
    return false;
}

DBConsoleController::DBConsoleController(
        MMCSCommandProcessor* commandProcessor,
        const bgq::utility::UserId& user,
        CxxSockets::UserType utype,
        MMCSConsolePortClient* connection
        ) :
    ConsoleController(commandProcessor, user, utype),
    _mmcsThread(0)
{
    LOGGING_DECLARE_USER_ID_MDC;
    setConsolePort(connection);

    // add DBConsoleController object to list
    PthreadMutexHolder mutex;
    mutex.Lock(&_midplaneControllerListMutex);
    _midplaneControllerList.push_back(this);
    mutex.Unlock();
}

DBConsoleController::~DBConsoleController()
{
    MMCSCommandReply reply;

    LOGGING_DECLARE_USER_ID_MDC;
    // turn off redirection when this thread ends
    if (_blockController && _redirecting)
    {
        _blockController->getBase()->stopRedirection(reply);
        _redirecting = false;
    }

    // let the block continue after we have been destroyed
    deselectBlock();

    // remove DBConsoleController object from list
    PthreadMutexHolder mutex;
    mutex.Lock(&_midplaneControllerListMutex);
    _midplaneControllerList.remove(this);
    mutex.Unlock();
}

DBBlockPtr
DBConsoleController::selectBlock(deque<string> args, MMCSCommandReply& reply, bool allocated)
{
    LOGGING_DECLARE_USER_ID_MDC;
    string blockName;
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    DBBlockPtr blockControllerPtr;
    DBBlockPtr zeroptr;

    PthreadMutexHolder mutex;

    if (args.size() == 0) {
        reply << FAIL << "args?" << DONE;
        return zeroptr;
    }

    // do we already have the block selected?
    blockName = args[0];

    if(allocateBlocked(blockName)) {
        LOG_ERROR_MSG("Cannot select block while being allocated in another thread.");
        reply << FAIL << "Cannot select block while being allocated in another thread." << DONE;
        return zeroptr;
    }

    if (_blockController != 0 && (_blockController->getBase()->getBlockName() == blockName)) {
        reply << OK << DONE;
        return boost::dynamic_pointer_cast<DBBlockController>(_blockController);
    }

    // get the block information from the database
    if ((result = BGQDB::getBlockInfo(blockName, bInfo)) != BGQDB::OK) {
        LOG_ERROR_MSG("block " << blockName << " not found.");
        reply << FAIL << "block " << blockName << " not found" << DONE;
        return zeroptr;
    }
    string blockErrorText;
    BGQDB::getBlockErrorText(blockName, blockErrorText);

    // get block owner from database
    string blockOwner;
    if ((result = BGQDB::getBlockOwner(blockName, blockOwner)) != BGQDB::OK) {
        LOG_ERROR_MSG("block " << blockName << " not found.");
        reply << FAIL << "block " << blockName << " not found" << DONE;
        return zeroptr;
    }

    // if block is not free, check if current owner has it allocated
    if (strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0) {
        // if block is free, check that the caller wants to select an unallocated block
        if (allocated == true) { // caller only wants to select an allocated block
            LOG_ERROR_MSG("Requested allocated block, block is not allocated.");
            reply << FAIL << "block is not allocated" << DONE;
            return zeroptr;
        }
    }

    // At this point, we know that the block is either free or
    // allocated to the current user.

    // search for the block in our map
    blockControllerPtr = DBConsoleController::findBlock(blockName);

    // We might find a DBBlockController for a free block in our table
    // This can occur on a test server when a RAS event causes the block
    // to be freed by the DBMonitor on the production server.
    // Just eliminate it.
    if ((blockControllerPtr != 0)
        && strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0
        && (blockControllerPtr->getBase()->getBlockThreads() == 0)) {
        LOG_INFO_MSG("selectBlock() reaping block " << blockControllerPtr->getBase()->getBlockName());
        DBConsoleController::removeBlock(blockControllerPtr->getBase()->getBlockName());
        //    blockControllerPtr = 0;
    }

    // We didn't find the block in the map
    if (blockControllerPtr == 0) {
        // In the last phase of block termination, the block is removed from the BlockMap and destroyed.
        // The destructor waits for RAS messages and shutdown complete before dropping the mcServer connections,
        // so it may be in this state for some time.
        if (strcmp(bInfo.status, BGQDB::BLOCK_TERMINATING) == 0) {
            reply << FAIL << "block is being disconnected" << DONE;
            return zeroptr;
        }
        LOG_INFO_MSG("block state " << bInfo.status);
        // Check whether the block is allocated by another MMCS server instance
        if (strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0         // block is free
                || strcmp(bInfo.status, BGQDB::BLOCK_INITIALIZED) == 0     // block is initialized (--reconnect-blocks)
                || strcmp(bInfo.status, BGQDB::BLOCK_ALLOCATED) == 0)      // block is allocated from scheduler API
        {

            // create a new DBBlockController and add it to the map
            if (bInfo.ionodes > 0) {
                // create an io block pointer
                IOBlockPtr iop(new IOBlockController(0, _user.getUser(), blockName, true));
                // now create the db block pointer and initialize it with the io block ptr
                DBBlockPtr p(new DBBlockController(iop));
                blockControllerPtr = p;
            } else if(bInfo.cnodes > 0) {
                // create an io block pointer
                CNBlockPtr cnp(new CNBlockController(0, _user.getUser(), blockName, true));
                // now create the db block pointer and initialize it with the io block ptr
                DBBlockPtr p(new DBBlockController(cnp));
                blockControllerPtr = p;
            } else {
                reply << FAIL << "Bogus block definition.  No compute or ionodes." << DONE;
                return zeroptr;
            }

            DBBlockPtr temp_blockControllerPtr =
                DBConsoleController::addBlock(blockName, blockControllerPtr);

            // in a race, we may get a different block controller back from addBlock
            if (temp_blockControllerPtr != blockControllerPtr) {
                blockControllerPtr = temp_blockControllerPtr;
            }
        } else {
            // Not a new allocate -- block may be in use by another MMCS server, or the BGQBlock table entry may have an incorrect state
            // An incorrect state may be set directly via sql, or by free block after an interrupted runjob command
            LOG_ERROR_MSG("Active Block List inconsistent. BlockController for " << blockName << " not found. Block state=" << bInfo.status);
            reply << FAIL << "BlockController not found or block is being disconnected" << DONE;
            return zeroptr;
        }

    } else { // we did find a block in the map
        // if we found a block controller in the map
        // The block state may be 'free' or 'configuring' in an allocation race
        LOG_INFO_MSG("selectBlock(): "
                << blockControllerPtr->getBase()->getBlockName()
                << "\t" << blockControllerPtr->getBase()->getUserName()
                << "("  << blockControllerPtr->getBase()->peekBlockThreads() << ")"
                << (blockControllerPtr->getBase()->isConnected() ? "\tconnected" : "")
                << (blockControllerPtr->getBase()->peekDisconnecting() ? "\tdisconnecting" : "")
                << (blockControllerPtr->getBase()->getRedirectSock() != 0 ? "\tredirecting" : "")
                << " state: " << bInfo.status << " owner: " << blockOwner
                << (blockErrorText[0] != '\0' ? string(" errorText:") + blockErrorText : string()));
    }

    // Now we either found a block in the pool or created a new one

    if (blockControllerPtr->getBase()->isDisconnecting()) {
        reply << FAIL << "disconnecting - " << blockControllerPtr->getBase()->disconnectReason() << DONE;
        mutex.Lock(&_blockMapMutex);
        blockControllerPtr->getBase()->decrBlockThreads();
        mutex.Unlock();
        return zeroptr;
    }

    // release any current block
    if (_blockController != 0)
        deselectBlock();

    _blockController = blockControllerPtr;

    reply << OK << DONE;
    return blockControllerPtr;
}

void
DBConsoleController::deselectBlock()
{
    LOGGING_DECLARE_USER_ID_MDC;
    DBBlockPtr blockControllerPtr;
    BGQDB::BLOCK_STATUS blockState = BGQDB::INVALID_STATE; // For target sets (which aren't in the DB), getBlockStatus will leave this unchanged
    PthreadMutexHolder mutex;
    if (_blockController != 0)
    {
        mutex.Lock(&_blockMapMutex);
        int numthreads = _blockController->getBase()->decrBlockThreads();
        mutex.Unlock();
        // if we were the last thread using the BlockController, delete it
        if (numthreads == 0 && _blockController->getBase()->readyToDie())
        {
            BGQDB::getBlockStatus(_blockController->getBase()->getBlockName(), blockState);
            if (blockState != BGQDB::ALLOCATED && blockState != BGQDB::INITIALIZED
                && blockState != BGQDB::BOOTING) {
                blockControllerPtr =
                    DBConsoleController::removeBlock(_blockController->getBase()->getBlockName());
            }
        }
        _blockController.reset();
    }

    if(blockControllerPtr)
        boost::dynamic_pointer_cast<DBBlockController>(blockControllerPtr)->Die();
}

void
DBConsoleController::run()
{
    LOGGING_DECLARE_USER_ID_MDC;
    if (_commandProcessor)
    {
        while (quit() == -1 && getConsolePort() != 0)
        {
            serviceCommands();

            // Check if another thread disconnected this block
            if (_blockController && _blockController->getBase()->isDisconnecting())
            {
                deselectBlock();
            }
        }
    }
    else
    {
        LOG_FATAL_MSG("DBConsoleController::run() internal error: empty command list");
    }
}

void
DBConsoleController::serviceCommands()
{
    LOGGING_DECLARE_USER_ID_MDC;
    std::string szLine;
    unsigned timeout = 5;    // five seconds
    MMCSConsolePortClient* connection = (MMCSConsolePortClient*) getConsolePort();
    try
    {
        if(connection->pollReceiveMessage(szLine,timeout) < 1)
            return;

        // Create the reply object
        MMCSCommandReply reply(connection, _replyFormat, true);

        // parse the command
        deque<string> cmdStr = MMCSCommandProcessor::parseCommand(szLine);
        if (cmdStr.size() == 0)
        {
            reply << FAIL << "command?" << DONE;
            return;
        }

        // log the command
        MMCSCommandProcessor::logCommand(cmdStr);

        //
        // Execute command
        //
        MMCSCommandProcessorStatus::procstat cmdExecStatus = MMCSCommandProcessorStatus::CMD_NOT_FOUND;
        cmdExecStatus = getCommandProcessor()->execute(cmdStr, reply, this);

        //
        // Write the output to the console
        //
        reply.sync();  // send the reply to the client
        return;
    }
    catch (MMCSConsolePort::Error e)
    {
        switch (e.errcode)
        {
            case ECONNRESET:
            case ESPIPE:
            case EPIPE:
            case EBADF:
                LOG_INFO_MSG("client disconnected");
                quit(EXIT_FAILURE);
                return;
            case EINTR:
                return;
            default:
                std::string err = e.what();
                if(err.find("Success") == std::string::npos) {
                    LOG_ERROR_MSG("DBConsoleController::serviceCommands() " << e.what() << ", errno=" << e.errcode);
                    quit(EXIT_FAILURE);
                } else {
                    LOG_INFO_MSG("Connection closed");
                    quit(EXIT_FAILURE);
                }
                return;
        }
    }
}
