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

#include "DBConsoleController.h"

#include "BlockControllerBase.h"
#include "CNBlockController.h"
#include "DBBlockController.h"
#include "IOBlockController.h"
#include "MMCSCommandProcessor.h"

#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

#define LOGGING_DECLARE_USER_ID_MDC \
    log4cxx::MDC _userid_mdc_( "user", std::string("[") + _user.getUser() + "] " );

namespace mmcs {
namespace server {

PthreadMutex    DBConsoleController::_blockMapMutex(PTHREAD_MUTEX_RECURSIVE_NP);
BlockMap        DBConsoleController::_blockMap;

PthreadMutex    DBConsoleController::_allocateMapMutex(PTHREAD_MUTEX_RECURSIVE_NP);
AllocateMap     DBConsoleController::_allocateMap;

PthreadMutex    DBConsoleController::_consoleControllerListMutex(PTHREAD_MUTEX_ERRORCHECK_NP);
ConsoleControllerList DBConsoleController::_consoleControllerList;


DBBlockPtr
DBConsoleController::findBlock(
        const std::string& blockId
        )
{
    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&_blockMapMutex);
    assert(mutex_rc == 0);

    DBBlockPtr foundBlock;
    const BlockMap::const_iterator it = _blockMap.find(blockId);
    if (it != _blockMap.end()) {
        foundBlock = boost::dynamic_pointer_cast<DBBlockController>(it->second);
    }
    if (foundBlock != 0) {
        // Increment the number of users of this block
        foundBlock->getBase()->incrBlockThreads();
    }

    return foundBlock;
}

DBBlockPtr
DBConsoleController::addBlock(
        const std::string& blockId,
        const DBBlockPtr& block
        )
{
    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&_blockMapMutex);
    assert(mutex_rc == 0);

    DBBlockPtr foundBlock;
    const BlockMap::const_iterator it = _blockMap.find(blockId);
    if (it != _blockMap.end()) {
        foundBlock = boost::dynamic_pointer_cast<DBBlockController>(it->second);
    }
    if (foundBlock == 0) {
        _blockMap[blockId] = block;
        foundBlock = block;
        LOG_TRACE_MSG(__FUNCTION__ << "(" << blockId << ")");
    }

    if (foundBlock != 0) {
        // Increment the number of users of this block
        foundBlock->getBase()->incrBlockThreads();
    }

    return foundBlock;
}

DBBlockPtr
DBConsoleController::removeBlock(
        const std::string& blockId
        )
{
    DBBlockPtr zeroptr;
    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&_blockMapMutex);
    assert(mutex_rc == 0);

    DBBlockPtr foundBlock;

    if (!_blockMap.empty()) {
        const BlockMap::const_iterator it = _blockMap.find(blockId);
        if (it != _blockMap.end())
            foundBlock = boost::dynamic_pointer_cast<DBBlockController>(it->second);
    }
    if (foundBlock != 0) {
        if (foundBlock->getBase()->getBlockThreads() != 0) {
            LOG_WARN_MSG("Found block " << blockId << " to remove, but block is in use in another thread.");
            // Even though we found a block, we return 0 as if we didn't
            // because some other thread is using it now and we don't want
            // the block controller deleted.
            return zeroptr;
        } else {
            _blockMap.erase(blockId);
            LOG_TRACE_MSG(__FUNCTION__ << "(" << blockId << ")");
        }
    }

    return foundBlock;
}

bool
DBConsoleController::setAllocating(
        const std::string& blockName
        )
{
    // Lock the mutex, see if we're already allocating, then create an entry in the map.
    PthreadMutexHolder mutex;
    mutex.Lock(&_allocateMapMutex);
    if (!_allocateMap.empty()) {
        const AllocateMap::const_iterator it = _allocateMap.find(blockName);
        if (it != _allocateMap.end()) {
            if (_allocateMap[blockName] != 0) {
                return false;  // Can't allocate if we're already allocating
            }
        }
    }
    LOG_INFO_MSG(__FUNCTION__ << "() " << blockName);
    _allocateMap[blockName] = pthread_self();
    return true;
}

void
DBConsoleController::doneAllocating(
        const std::string& blockName
        )
{
    // We're done allocating the block. Clear our place in the map.
    PthreadMutexHolder mutex;
    mutex.Lock(&_allocateMapMutex);
    if (_allocateMap[blockName] != pthread_self()) {
        LOG_ERROR_MSG("Allocate map is in indeterminant state. Allocate thread " << _allocateMap[blockName]);
    }
    LOG_INFO_MSG(__FUNCTION__ << "() " << blockName);
    _allocateMap.erase(blockName);
    return;
}

bool
DBConsoleController::allocateBlocked(
        const std::string& blockName
        )
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_allocateMapMutex);
    // See if there is another thread that has allocation locked out
    if (!_allocateMap.empty()) {
        const AllocateMap::const_iterator it = _allocateMap.find(blockName);
        if (it != _allocateMap.end()) {
            if (_allocateMap[blockName] != pthread_self()) {
                LOG_WARN_MSG(__FUNCTION__ << "() " << blockName << " 0x" << std::hex << _allocateMap[blockName]);
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
        mmcs_client::ConsolePortClient* connection
        ) :
    ConsoleController(commandProcessor, user, utype),
    _mmcsThread(0),
    _blockMdc()
{
    setConsolePort(connection);

    // Add DBConsoleController object to list
    PthreadMutexHolder mutex;
    mutex.Lock(&_consoleControllerListMutex);
    _consoleControllerList.push_back(this);
    mutex.Unlock();
}

DBConsoleController::~DBConsoleController()
{
    mmcs_client::CommandReply reply;

    LOGGING_DECLARE_USER_ID_MDC;
    if ( _blockController ) {
        const log4cxx::MDC _blockid_mdc_( "blockId", std::string("{") + _blockController->getBase()->getBlockName() + "} " );

        if (_redirecting) {
            // Turn off redirection when this thread ends
            _blockController->getBase()->stopRedirection(reply);
            _redirecting = false;
        }

        // Let the block continue after we have been destroyed
        deselectBlock();
    }

    // Remove DBConsoleController object from list
    PthreadMutexHolder mutex;
    mutex.Lock(&_consoleControllerListMutex);
    _consoleControllerList.remove(this);
    mutex.Unlock();
}

DBBlockPtr
DBConsoleController::selectBlock(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        bool allocated
        )
{
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    const DBBlockPtr zeroptr;

    PthreadMutexHolder mutex;

    if (args.size() == 0) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return zeroptr;
    }

    // Do we already have the block selected?
    const std::string blockName = args[0];

    if (allocateBlocked(blockName)) {
        reply << mmcs_client::FAIL << "Cannot select block while being allocated or freed in another thread." << mmcs_client::DONE;
        return zeroptr;
    }

    if (_blockController != 0 && (_blockController->getBase()->getBlockName() == blockName)) {
        reply << mmcs_client::OK << mmcs_client::DONE;
        return boost::dynamic_pointer_cast<DBBlockController>(_blockController);
    }

    // Get the block information from the database
    if ((result = BGQDB::getBlockInfo(blockName, bInfo)) != BGQDB::OK) {
        LOG_ERROR_MSG("Block " << blockName << " not found.");
        reply << mmcs_client::FAIL << "Block " << blockName << " not found" << mmcs_client::DONE;
        return zeroptr;
    }
    std::string blockErrorText;
    BGQDB::getBlockErrorText(blockName, blockErrorText);

    // If block is not free, check if current owner has it allocated
    if (strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0) {
        // If block is free, check that the caller wants to select an unallocated block
        if (allocated == true) { // Caller only wants to select an allocated block
            LOG_ERROR_MSG("Requested allocated block, block is not allocated.");
            reply << mmcs_client::FAIL << "Block is not allocated" << mmcs_client::DONE;
            return zeroptr;
        }
    }

    // At this point, we know that the block is either free or allocated to the current user.

    // Search for the block in our map
    DBBlockPtr blockControllerPtr = DBConsoleController::findBlock(blockName);

    // We might find a DBBlockController for a free block in our table.
    // This can occur on a test server when a RAS event causes the block
    // to be freed by the DBMonitor on the production server.
    // Just eliminate it.
    if ((blockControllerPtr != 0)
        && strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0
        && (blockControllerPtr->getBase()->getBlockThreads() == 0)) {
        LOG_DEBUG_MSG(__FUNCTION__ << "() reaping block " << blockControllerPtr->getBase()->getBlockName());
        DBConsoleController::removeBlock(blockControllerPtr->getBase()->getBlockName());
    }

    // We didn't find the block in the map
    if (blockControllerPtr == 0) {
        // In the last phase of block termination, the block is removed from the BlockMap and destroyed.
        // The destructor waits for RAS messages and shutdown complete before dropping the mcServer connections,
        // so it may be in this state for some time.
        if (strcmp(bInfo.status, BGQDB::BLOCK_TERMINATING) == 0) {
            reply << mmcs_client::FAIL << "Block is being disconnected." << mmcs_client::DONE;
            return zeroptr;
        }
        LOG_DEBUG_MSG("Current block status is " << BGQDB::blockCodeToString(bInfo.status));
        // Check whether the block is allocated by another MMCS server instance
        if (strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0         // block is free
                || strcmp(bInfo.status, BGQDB::BLOCK_INITIALIZED) == 0     // block is initialized (--reconnect-blocks)
                || strcmp(bInfo.status, BGQDB::BLOCK_ALLOCATED) == 0)      // block is allocated from scheduler API
        {
            // Create a new DBBlockController and add it to the map
            if (bInfo.ionodes > 0) {
                const IOBlockPtr iop(new IOBlockController(0, _user.getUser(), blockName, true));
                const DBBlockPtr p(new DBBlockController(iop));
                blockControllerPtr = p;
            } else if (bInfo.cnodes > 0) {
                const CNBlockPtr cnp(new CNBlockController(0, _user.getUser(), blockName, true));
                const DBBlockPtr p(new DBBlockController(cnp));
                blockControllerPtr = p;
            } else {
                reply << mmcs_client::FAIL << "Bad block definition, no compute or I/O nodes." << mmcs_client::DONE;
                return zeroptr;
            }

            DBBlockPtr temp_blockControllerPtr = DBConsoleController::addBlock(blockName, blockControllerPtr);

            // In a race, we may get a different block controller back from addBlock
            if (temp_blockControllerPtr != blockControllerPtr) {
                blockControllerPtr = temp_blockControllerPtr;
            }
        } else {
            // Not a new allocate -- block may be in use by another MMCS server,
            // or the BGQBlock table entry may have an incorrect state.
            // An incorrect state may be set directly via sql, or by free block
            // after an interrupted runjob command
            LOG_ERROR_MSG("Active block list inconsistent. BlockController for " << blockName << " not found. Block status is " << BGQDB::blockCodeToString(bInfo.status));
            reply << mmcs_client::FAIL << "BlockController not found or block is being disconnected." << mmcs_client::DONE;
            return zeroptr;
        }
    } else { // We did find a block in the map
        // If we found a block controller in the map the block status may be 'free'
        // or 'configuring' in an allocation race.
        LOG_INFO_MSG(__FUNCTION__ << "(): "
                << blockControllerPtr->getBase()->getBlockName()
                << "\t" << blockControllerPtr->getBase()->getUserName()
                << "("  << blockControllerPtr->getBase()->peekBlockThreads() << ")"
                << (blockControllerPtr->getBase()->isConnected() ? "\tconnected" : "")
                << (blockControllerPtr->getBase()->peekDisconnecting() ? "\tdisconnecting" : "")
                << (blockControllerPtr->getBase()->getRedirectSock() != 0 ? "\tredirecting" : "")
                << " status: " << BGQDB::blockCodeToString(bInfo.status)
                << (blockErrorText[0] != '\0' ? std::string(" errorText:") + blockErrorText : std::string()));
    }

    // Now we either found a block in the pool or created a new one
    if (blockControllerPtr->getBase()->isDisconnecting()) {
        reply << mmcs_client::FAIL << "Disconnecting - " << blockControllerPtr->getBase()->disconnectReason() << mmcs_client::DONE;
        mutex.Lock(&_blockMapMutex);
        blockControllerPtr->getBase()->decrBlockThreads();
        mutex.Unlock();
        return zeroptr;
    }

    // Release any current block
    if (_blockController != 0)
        deselectBlock();

    _blockController = blockControllerPtr;
    _blockMdc.reset( new log4cxx::MDC( "blockId", std::string("{") + blockName + "} " ) );

    reply << mmcs_client::OK << mmcs_client::DONE;
    return blockControllerPtr;
}

void
DBConsoleController::deselectBlock()
{
    if (!_blockController) {
        return;
    }

    PthreadMutexHolder mutex;
    mutex.Lock(&_blockMapMutex);
    const int numthreads = _blockController->getBase()->decrBlockThreads();
    LOG_DEBUG_MSG( __FUNCTION__ << " " << numthreads );
    mutex.Unlock();

    DBBlockPtr blockControllerPtr;
    // If we were the last thread using the BlockController, delete it
    if (numthreads == 0 && _blockController->getBase()->readyToDie()) {
        BGQDB::BLOCK_STATUS blockState = BGQDB::INVALID_STATE;
        BGQDB::getBlockStatus(_blockController->getBase()->getBlockName(), blockState);
        if (
                blockState != BGQDB::ALLOCATED &&
                blockState != BGQDB::INITIALIZED &&
                blockState != BGQDB::BOOTING
           )
        {
            blockControllerPtr = DBConsoleController::removeBlock(_blockController->getBase()->getBlockName());
        }
    }

    _blockController.reset();

    if (blockControllerPtr) {
        boost::dynamic_pointer_cast<DBBlockController>(blockControllerPtr)->Die();
    }

    _blockMdc.reset();
}

void
DBConsoleController::run()
{
    LOGGING_DECLARE_USER_ID_MDC;
    if (_commandProcessor) {
        while (quit() == -1 && getConsolePort() != 0) {
            serviceCommands();

            // Check if another thread disconnected this block
            if (_blockController && _blockController->getBase()->isDisconnecting()) {
                deselectBlock();
            }
        }
    } else {
        LOG_FATAL_MSG(__FUNCTION__ << "() unexpected internal error: empty command list");
    }
}

void
DBConsoleController::serviceCommands()
{
    mmcs_client::ConsolePortClient* const connection = static_cast<mmcs_client::ConsolePortClient*>(getConsolePort());
    try {
        const unsigned timeout = 5;    // five seconds
        std::string szLine;
        if (connection->pollReceiveMessage(szLine, timeout) < 1) {
            return;
        }

        // Create the reply object
        mmcs_client::CommandReply reply(connection, 1, true);

        // Parse the command
        const std::deque<std::string> cmdStr = MMCSCommandProcessor::parseCommand(szLine);
        if (cmdStr.size() == 0) {
            reply << mmcs_client::FAIL << "command?" << mmcs_client::DONE;
            return;
        }

        // Log the command
        MMCSCommandProcessor::logCommand(cmdStr);

        // Execute the command, ignore the return code since the reply will contain it
        (void)getCommandProcessor()->execute(cmdStr, reply, this);

        // Write the output to the console
        reply.sync();  // Send the reply to the client
        return;
    } catch (const mmcs_client::ConsolePort::Error& e) {
        switch (e.errcode) {
            case ECONNRESET:
            case ESPIPE:
            case EPIPE:
            case EBADF:
                LOG_DEBUG_MSG("Client disconnected.");
                quit(EXIT_FAILURE);
                return;
            case EINTR:
                return;
            default:
                if (e.errcode == 0) { // This would be for a normal connection closed
                    // No msg is necessary
                    quit(EXIT_FAILURE);
                } else {
                    LOG_ERROR_MSG(__FUNCTION__ << "() " << e.what() << " (errno=" << e.errcode << ")");
                    quit(EXIT_FAILURE);
                }
                return;
        }
    }
}

} } // namespace mmcs::server
