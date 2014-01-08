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

#include <bgsched/allocator/InputException.h>
#include <bgsched/allocator/RuntimeException.h>

#include "bgsched/allocator/AllocateEventHandler.h"
#include "bgsched/allocator/AllocatorEventListenerImpl.h"
#include "bgsched/allocator/AllocatorImpl.h"
#include "bgsched/allocator/DeallocateEventHandler.h"
#include "bgsched/allocator/LiveModelImpl.h"

#include <bgsched/DatabaseException.h>
#include <bgsched/InternalException.h>
#include <bgsched/InputException.h>
#include <bgsched/RuntimeException.h>

#include "bgsched/BlockImpl.h"
#include "bgsched/CableImpl.h"
#include "bgsched/utility.h"

#include <bgsched/core/core.h>

#include <bgsched/realtime/Client.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

#include <boost/bind.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <algorithm>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace bgsched::core;
using namespace bgsched::realtime;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace {
    // Message strings
    const string No_Block_Name_Str("Block name is empty.");
    const string Block_Name_Too_Long_Str("Block name is too long.");
    const string DB_Access_Error_Str("Error occurred while accessing database.");
    const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
    const string Unexpected_Exception_Str("Received unexpected exception from internal method.");
    const string Unexpected_Return_Code_Str("Unexpected return code from internal database function.");
    const string Invalid_Block_Action_Str("Block action invalid or already pending.");
    const string Midplane_Not_Found_Str("Midplane not found.");
    const string Block_Already_Exists_Str("Block already exists or error accessing database.");
} // anonymous namespace

namespace bgsched {
namespace allocator {

void
LiveModel::Impl::allocateBlockThread()
{
    boost::thread* threadPtr;

    while (!_shutdown) {
        // Use real-time events to handle block allocate (boot) notifications
        realtimeAllocateBlocks();
        // Indicate thread is ready if real-time fails
        setAllocateThreadReady();

        if (!_shutdown) {
            // Switch to database polling when real-time server fails
            try {
                LOG_WARN_MSG("Switching over to database polling for block allocate monitor.");
                // Will poll in a loop until shutdown or polling timer expires
                pollAllocateBlocks();
            } catch (...) {
                // Request failed because database connection dropped or unexpected error occurred
                LOG_ERROR_MSG("Error occurred polling for status update on blocks in allocate thread, terminating thread.");
            }
        }
    }

    // Unblock any pending allocate requests
    setSwitchingAllocateHandler(false);

    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on thread map");
    // Lock mutex as single writer across multiple threads
    boost::unique_lock<boost::shared_mutex> thread_map_lock(_threadMapMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on thread map");
    // Shutdown or fatal error occurred so get reference to boost:thread so it can be deleted
    threadPtr = _threads.find(boost::this_thread::get_id())->second;
    // Remove thread from active list
    _threads.erase(boost::this_thread::get_id());
    // delete threadPtr;   // Free thread memory
    // Unlock thread map mutex
    thread_map_lock.unlock();
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked thread map");
}

void
LiveModel::Impl::realtimeAllocateBlocks()
{
    // Handler for real-time block status changes
    AllocateEventHandler allocateEventHandler(this);

    LOG_DEBUG_MSG("Creating real-time client to handle allocate requests for blocks.");
    Client rt_client;

    // Setup the real-time client to filter for block status changes to Free or Initialized
    try {
        Filter rt_filter(Filter::createNone());

        rt_filter.setBlocks(true);         // true=send blocks
        rt_filter.setBlockDeleted(false);  // false=don't send block deletes

        // Filter on Free and Initialized status changes
        Filter::BlockStatuses block_statuses;
        block_statuses.insert(Block::Free);
        block_statuses.insert(Block::Initialized);
        rt_filter.setBlockStatuses(&block_statuses);

        LOG_TRACE_MSG("Setting the filter on the real-time client in allocate monitor.");
        Filter::Id filter_id; // Assigned filter id
        // Set the event filter on the client to tell the server the types of events it wants to receive.
        //
        // The client assigns a filter ID which is returned in filter_id. When the
        // real-time server has received the filter and applied it to the client, it sends back an
        // acknowledgment with this filter ID.
        //
        // The client is blocking and needs to send the filter request. If this send is interrupted
        // then the client will repeatedly attempt to send the request until it's not interrupted.
        rt_client.setFilter(rt_filter, &filter_id, NULL);

        // Set the event handler that receives block status change messages
        rt_client.addListener(allocateEventHandler);

        LOG_DEBUG_MSG("Connecting real-time client to real-time server in allocate monitor.");
        rt_client.connect();

        // Get the file descriptor to poll on for socket activity
        int pollFd = rt_client.getPollDescriptor();

        // Request real-time updates from the server. The client attempts to send the request. If the
        // send is  interrupted then the client will repeatedly attempt to send the request until it's
        // not interrupted.
        LOG_TRACE_MSG("Requesting updates on the real-time client for allocate block status changes.");
        rt_client.requestUpdates(NULL);

        rt_client.setBlocking(false); // Change to non-blocking mode

        // Polling list
        struct pollfd pollList[2];

        pollList[0].fd = pollFd;      // Monitor real-time socket for data
        pollList[0].events = POLLIN;  // Notify on incoming data

        pollList[1].fd = _pipeFd[0];  // Monitor pipe for shutdown request
        pollList[1].events = POLLIN;  // Notify on incoming data (Shutdown)

        // Unblock any pending allocate requests
        setSwitchingAllocateHandler(false);

        bool isContinue = true;
        // The thread will poll for real-time messages or a shutdown request from the main thread
        while (isContinue) {
            // Wait for real-time message or shutdown
            int rc = poll(pollList, 2, -1);

            // Check for error in poll()
            if (rc == -1) {
                LOG_WARN_MSG("Allocate monitor received unexpected error with poll().");
                isContinue = false;  // Fall-back to database polling
            } else {
                // Check for shutdown
                if (_shutdown) {
                    LOG_DEBUG_MSG("Received shutdown request in allocate monitor.");
                    isContinue = false;
                } else {
                    // Check for problems with the real-time socket
                    if (((pollList[0].revents & POLLHUP)  == POLLHUP) ||
                        ((pollList[0].revents & POLLERR)  == POLLERR) ||
                        ((pollList[0].revents & POLLNVAL) == POLLNVAL)) {
                        LOG_WARN_MSG("Unexpected error with real-time socket in allocate monitor.");
                        isContinue = false;
                    } else {
                        // Received real-time message
                        if ((pollList[0].revents & POLLIN) == POLLIN) {
                            // If the real-time server closes the connection then the "end" indicator is set
                            // to true and the client is disconnected.
                            bool end = false;
                            // Get the real-time message (notifications are done via real-time allocate event handler)
                            //LOG_TRACE_MSG("Received real-time message in allocate monitor.");
                            rt_client.receiveMessages(NULL, NULL, &end);

                            // Check if server disconnected the client
                            if (end) {
                                LOG_WARN_MSG("Real-time server disconnected client in allocate monitor.");
                                isContinue = false;
                            }

                            // Check if server sent real-time ended event (too many transactions to handle)
                            if (allocateEventHandler.isRealtimeEndedEvent()) {
                                LOG_WARN_MSG("Real-time server temporarily stopped sending events in allocate monitor.");
                                isContinue = false;
                            }
                        }
                    }
                }
            }
        }
    } catch (const exception& rte) {
        LOG_WARN_MSG("Following error occurred in allocate monitor: " << rte.what());
        // Fall thru to disconnect real-time client
    } catch (...) {
        LOG_WARN_MSG("Unexpected error occurred in allocate monitor.");
        // Fall thru to disconnect real-time client
    }

    // Unblock any pending allocate requests
    setSwitchingAllocateHandler(false);

    // Disconnect from the real-time server (does nothing if already disconnected)
    rt_client.disconnect();
}

void
LiveModel::Impl::pollAllocateBlocks()
{
    // Block filter for polling on blocks
    BlockFilter block_filter;
    // Blocks returned from core::getBlocks()
    Block::Ptrs blockVector;

    string emptyErrorMessage;

    // Get properties set on bgsched::init()
    bgq::utility::Properties::Ptr properties(bgsched::getProperties());
    const string section("bgsched");
    const string keyword("polling_interval");

    unsigned poll_interval = 3000;
    // Get polling interval
    try {
        poll_interval = boost::lexical_cast<unsigned>(properties->getValue(section, keyword));
    } catch (const std::invalid_argument& e) {
        // Missing section isn't an error, just log and use a default
        LOG_DEBUG_MSG(e.what());
        poll_interval = 3000;
    }

    unsigned realtime_interval_check = 60000; // After 1 minute check if real-time server is back
    unsigned accumulated_time = 0; // Tracker for time spent polling

    // Block polling code. Used when we get an exception or the real-time server disconnects.
    // Keep polling for block status changes until shutdown, poll time expires, or unrecoverable error occurs
    while (!_shutdown) {
        // Time spent polling
        accumulated_time = accumulated_time + poll_interval;

        // Polling timer expired?
        if (accumulated_time > realtime_interval_check) {
            // Temporarily block any allocate requests
            setSwitchingAllocateHandler(true);
        }

        // Any blocks to check on?
        if (_bootBlocks.empty()) {
            // Polling timer expired?
            if (accumulated_time > realtime_interval_check) {
                LOG_WARN_MSG("Switching over to real-time events for block allocate monitor.");
                return;
            }
        } else {
            // Still have blocks to process so continue to allow new allocate requests
            if (accumulated_time > realtime_interval_check) {
                // Unblock allocate requests
                setSwitchingAllocateHandler(false);
            }

            LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on boot block container");
            // Lock mutex as single writer across multiple threads
            boost::unique_lock<boost::shared_mutex> lock(_bootMutex);
            LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on boot block container");

            // Iterate over booting blocks to check on the status - have to be careful when erasing the iterator
            // since the erase will invalidate iterators in other threads
            AllocatingBlocks::iterator iter = _bootBlocks.begin();
            while (iter != _bootBlocks.end()) {
                bool iteratorErased = false;
                string block = iter->first; // Get the block name in our monitor list
                SequenceId monitorSeqId = iter->second; // Get the sequence ID associated with block
                // Filter on specific block name
                block_filter.setName(block);
                // Clear out previous block entries
                blockVector.clear();
                // Get the block details from the database
                blockVector = bgsched::core::getBlocks(block_filter);
                bool blockFound = false;
                // Iterate thru the block vector and find match with block being allocated
                for (Block::Ptrs::iterator it = blockVector.begin(); it != blockVector.end(); it++) {
                    Block::Ptr blockPtr = *(it);
                    string name = blockPtr->getName();
                    SequenceId seqId = blockPtr->getSequenceId();
                    // Block names match?
                    if (name.compare(block) == 0) {
                        blockFound = true;
                        //LOG_TRACE_MSG("Block " << name << " with seqid " << seqId << " found in monitor list. Monitor list seqid is " << monitorSeqId);
                        // Check if block is Free or Initialized and status really changed
                        if ((blockPtr->getStatus() == Block::Free) && (seqId > monitorSeqId)) {
                            // Build dummy block event notification for Free
                            realtime::ClientEventListener::BlockStateChangedEventInfo::Impl dummyBlockEvent(
                                    block,
                                    Block::Free,
                                    seqId,
                                    Block::Booting,
                                    monitorSeqId
                            );
                            LOG_DEBUG_MSG("Block " << block << " changed to Free status");
                            // Post "Free" event to listeners
                            this->notifyAllocateListeners(dummyBlockEvent, emptyErrorMessage);
                            // Remove the block from the vector
                            _bootBlocks.erase(iter);
                            iteratorErased = true;
                        } else if ((blockPtr->getStatus() == Block::Initialized) && (seqId > monitorSeqId)) {
                            // Build dummy block event notification for Initialized
                            realtime::ClientEventListener::BlockStateChangedEventInfo::Impl dummyBlockEvent(
                                    block,
                                    Block::Initialized,
                                    seqId,
                                    Block::Booting,
                                    monitorSeqId
                            );
                            LOG_DEBUG_MSG("Block " << block << " changed to Initialized status");
                            // Post "Initialized" event to listeners
                            this->notifyAllocateListeners(dummyBlockEvent, emptyErrorMessage);
                            // Remove the block from the vector
                            _bootBlocks.erase(iter);
                            iteratorErased = true;
                        } else {
                            // Found a match on the block and it is not Free or Initialized so break out of loop
                            break;
                        }
                    }
                }

                // Did we find our block?
                if (!blockFound) {
                    // Request failed because no block was found
                    string errorMessage = "Allocate monitor: compute block " + block + " was not found.";
                    LOG_WARN_MSG(errorMessage);

                    // Build dummy block event notification for Free because block has been deleted
                    realtime::ClientEventListener::BlockStateChangedEventInfo::Impl dummyBlockFreeEvent(
                            block,
                            Block::Free,
                            0,
                            Block::Booting,
                            0
                    );

                    // Can't throw an exception since we are in a thread, post dummy "Free" event to listeners
                    this->notifyAllocateListeners(dummyBlockFreeEvent, errorMessage);
                    // Remove the block from the vector
                    _bootBlocks.erase(iter);
                    iteratorErased = true;
                }
                // Did we erase the current iterator?
                if (iteratorErased) {
                    iter =_bootBlocks.begin();
                } else {
                    iter++;
                }
            }
            // Unlock mutex
            lock.unlock();
            LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock on boot block container");
        }
        // Sleep before polling again
        boost::this_thread::sleep(boost::posix_time::milliseconds(poll_interval));
    }
}

void
LiveModel::Impl::deallocateBlockThread()
{
    boost::thread* threadPtr;

    while (!_shutdown) {
        // Use real-time events to handle block deallocate (free) notifications
        realtimeDeallocateBlocks();

        // Indicate thread is ready if real-time fails
        setDeallocateThreadReady();

        if (!_shutdown) {
            // Switch to database polling when real-time server fails
            try {
                LOG_WARN_MSG("Switching over to database polling for block deallocate monitor.");
                // Will poll in a loop until shutdown or polling timer expires
                pollDeallocateBlocks();
            } catch (...) {
                // Request failed because database connection dropped or unexpected error occurred
                LOG_ERROR_MSG("Error occurred polling for status update on blocks in deallocate thread, terminating thread.");
            }
        }
    }

    // Unblock any pending deallocate requests
    setSwitchingDeallocateHandler(false);

    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on thread map");
    // Lock mutex as single writer across multiple threads
    boost::unique_lock<boost::shared_mutex> thread_map_lock(_threadMapMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on thread map");
    // Shutdown or fatal error occurred so get reference to boost:thread so it can be deleted
    threadPtr = _threads.find(boost::this_thread::get_id())->second;
    // Remove thread from active list
    _threads.erase(boost::this_thread::get_id());
    // delete threadPtr;   // Free thread memory
    // Unlock thread map mutex
    thread_map_lock.unlock();
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked thread map");
}

void
LiveModel::Impl::realtimeDeallocateBlocks()
{
    // Handler for real-time block status changes
    DeallocateEventHandler deallocateEventHandler(this);

    LOG_DEBUG_MSG("Creating real-time client to handle deallocate requests for blocks.");
    Client rt_client;

    // Setup the real-time client to filter for block status changes to Free
    try {
        Filter rt_filter(Filter::createNone());

        rt_filter.setBlocks(true);         // true=send blocks
        rt_filter.setBlockDeleted(false);  // false=don't send block deletes

        // Filter on Free status changes
        Filter::BlockStatuses block_statuses;
        block_statuses.insert(Block::Free);
        rt_filter.setBlockStatuses(&block_statuses);

        LOG_TRACE_MSG("Setting the filter on the real-time client in deallocate monitor.");
        Filter::Id filter_id; // Assigned filter id
        // Set the event filter on the client to tell the server the types of events it wants to receive.
        //
        // The client assigns a filter ID which is returned in filter_id. When the
        // real-time server has received the filter and applied it to the client, it sends back an
        // acknowledgment with this filter ID.
        //
        // The client is blocking and needs to send the filter request. If this send is interrupted
        // then the client will repeatedly attempt to send the request until it's not interrupted.
        rt_client.setFilter(rt_filter, &filter_id, NULL);

        // Set the event handler that receives block status change messages
        rt_client.addListener(deallocateEventHandler);

        LOG_DEBUG_MSG("Connecting real-time client to real-time server in deallocate monitor." );
        rt_client.connect();

        // Get the file descriptor to poll on for socket activity
        int pollFd = rt_client.getPollDescriptor();

        // Request real-time updates from the server. The client attempts to send the request. If the
        // send is  interrupted then the client will repeatedly attempt to send the request until it's
        // not interrupted.
        LOG_TRACE_MSG("Requesting updates on the real-time client for deallocate block status changes.");
        rt_client.requestUpdates(NULL);

        rt_client.setBlocking(false); // Change to non-blocking mode

        // Polling list
        struct pollfd pollList[2];

        pollList[0].fd = pollFd;      // Monitor real-time socket for data
        pollList[0].events = POLLIN;  // Notify on incoming data

        pollList[1].fd = _pipeFd[0];  // Monitor pipe for shutdown request
        pollList[1].events = POLLIN;  // Notify on incoming data (Shutdown)

        // Unblock any pending deallocate requests
        setSwitchingDeallocateHandler(false);

        bool isContinue = true;
        // The thread will poll for real-time messages or a shutdown request from the main thread
        while (isContinue) {
            // Wait for real-time message or shutdown
            int rc = poll(pollList, 2, -1);

            // Check for error in poll()
            if (rc == -1) {
                LOG_WARN_MSG("Deallocate monitor received unexpected error with poll().");
                isContinue = false;  // Fall-back to database polling
            } else {
                // Check for shutdown
                if (_shutdown) {
                    LOG_DEBUG_MSG("Received shutdown request in deallocate monitor.");
                    isContinue = false;
                } else {
                    // Check for problems with the real-time socket
                    if (((pollList[0].revents & POLLHUP)  == POLLHUP) ||
                            ((pollList[0].revents & POLLERR)  == POLLERR) ||
                            ((pollList[0].revents & POLLNVAL) == POLLNVAL)) {
                        LOG_WARN_MSG("Unexpected error with real-time socket in deallocate monitor.");
                        isContinue = false;
                    } else {
                        // Received real-time message
                        if ((pollList[0].revents & POLLIN) == POLLIN) {
                            // If the real-time server closes the connection then the "end" indicator is set
                            // to true and the client is disconnected.
                            bool end = false;
                            //LOG_TRACE_MSG("Received real-time message in deallocate monitor.");
                            // Get the real-time message (notifications are done via real-time deallocate event handler)
                            rt_client.receiveMessages(NULL, NULL, &end);

                            // Check if server disconnected the client
                            if (end) {
                                LOG_WARN_MSG("Real-time server disconnected client in deallocate monitor.");
                                isContinue = false;
                            }

                            // Check if server sent real-time ended event (too many transactions to handle)
                            if (deallocateEventHandler.isRealtimeEndedEvent()) {
                                LOG_WARN_MSG("Real-time server temporarily stopped sending events in deallocate monitor.");
                                isContinue = false;
                            }
                        }
                    }
                }
            }
        }
    } catch (const exception& rte) {
        LOG_WARN_MSG("Following error occurred in deallocate monitor: " << rte.what());
        // Fall thru to disconnect real-time client
    } catch (...) {
        LOG_WARN_MSG("Unexpected error occurred in deallocate monitor.");
        // Fall thru to disconnect real-time client
    }

    // Unblock any pending deallocate requests
    setSwitchingDeallocateHandler(false);

    // Disconnect from the real-time server (does nothing if already disconnected)
    rt_client.disconnect();
}

void
LiveModel::Impl::pollDeallocateBlocks()
{
    // Block filter for polling on blocks
    BlockFilter block_filter;
    // Blocks returned from core::getBlocks()
    Block::Ptrs blockVector;

    string emptyErrorMessage;

    // Get properties set on bgsched::init()
    bgq::utility::Properties::Ptr properties(bgsched::getProperties());
    const string section("bgsched");
    const string keyword("polling_interval");

    unsigned poll_interval = 3000;
    // Get polling interval
    try {
        poll_interval = boost::lexical_cast<unsigned>(properties->getValue(section, keyword));
    } catch (const std::invalid_argument& e) {
        // Missing section isn't an error, just log and use a default
        LOG_DEBUG_MSG(e.what());
        poll_interval = 3000;
    }

    unsigned realtime_interval_check = 60000; // After 1 minute check if real-time server is back
    unsigned accumulated_time = 0; // Tracker for time spent polling

    // Block polling code. Used when we get an exception or the real-time server disconnects.
    // Keep polling for block status changes until shutdown, poll time expires, or unrecoverable error occurs
    while (!_shutdown) {
        // Time spent polling
        accumulated_time = accumulated_time + poll_interval;

        // Polling timer expired?
        if (accumulated_time > realtime_interval_check) {
            // Temporarily block any deallocate requests
            setSwitchingDeallocateHandler(true);
        }

        // Any blocks to check on?
        if (_freeBlocks.empty()) {
            // Have we exceeded our polling interval before retrying real-time events?
            if (accumulated_time > realtime_interval_check) {
                LOG_WARN_MSG("Switching over to real-time events for block deallocate monitor.");
                return;
            }
        } else {
            // Still have blocks to process so continue to allow new deallocate requests
            if (accumulated_time > realtime_interval_check) {
                // Unblock deallocate requests
                setSwitchingDeallocateHandler(false);
            }

            LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on free block container");
            // Lock mutex as single writer across multiple threads
            boost::unique_lock<boost::shared_mutex> lock(_freeMutex);
            LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on free block container");

            // Iterate over blocks being freed to check on the status - have to be careful when erasing the iterator
            // since the erase will invalidate iterators in other threads
            DeallocatingBlocks::iterator iter = _freeBlocks.begin();
            while (iter != _freeBlocks.end()) {
                bool iteratorErased = false;
                string block = iter->first; // Get the block name in our monitor list
                SequenceId monitorSeqId = iter->second; // Get the sequence ID associated with block
                // Filter on specific block name
                block_filter.setName(block);
                // Clear out previous block entries
                blockVector.clear();
                // Get the block details from the database
                blockVector = bgsched::core::getBlocks(block_filter);
                bool blockFound = false;
                // Iterate thru the block vector and find match with block being deallocated
                for (Block::Ptrs::iterator it = blockVector.begin(); it != blockVector.end(); it++) {
                    Block::Ptr blockPtr = *(it);
                    string name = blockPtr->getName();
                    SequenceId seqId = blockPtr->getSequenceId();
                    // Block names match?
                    if (name.compare(block) == 0) {
                        blockFound = true;
                        // Check if block is Free and status really changed
                        if ((blockPtr->getStatus() == Block::Free) && (seqId > monitorSeqId)) {
                            // Build dummy block event notification for Free
                            realtime::ClientEventListener::BlockStateChangedEventInfo::Impl dummyBlockEvent(
                                    block,
                                    Block::Free,
                                    blockPtr->getSequenceId(),
                                    Block::Booting,
                                    0
                            );
                            LOG_DEBUG_MSG("Block " << block << " changed to Free status");
                            // Post "Free" event to listeners
                            this->notifyDeallocateListeners(dummyBlockEvent, emptyErrorMessage);
                            // Remove the block from the vector
                            _freeBlocks.erase(iter);
                            iteratorErased = true;
                        } else {
                            // Found a match on the block and it is not Free so break out of loop
                            break;
                        }
                    }
                }

                // Did we find our block?
                if (!blockFound) {
                    // Request failed because no block was found
                    string errorMessage = "Deallocate thread: compute block " + block + " was not found.";
                    LOG_WARN_MSG(errorMessage);

                    // Build dummy block event notification for Free because block has been deleted
                    realtime::ClientEventListener::BlockStateChangedEventInfo::Impl dummyBlockFreeEvent(
                            block,
                            Block::Free,
                            0,
                            Block::Booting,
                            0
                    );

                    // Can't throw an exception since we are in a thread, post dummy "Free" event to listeners
                    this->notifyDeallocateListeners(dummyBlockFreeEvent, errorMessage);
                    // Remove the block from the vector
                    _freeBlocks.erase(iter);
                    iteratorErased = true;
                }

                // Did we erase the current iterator?
                if (iteratorErased) {
                    iter =_freeBlocks.begin();
                } else {
                    iter++;
                }
            }
            // Unlock mutex
            lock.unlock();
            LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive lock on free block container");
        }

        // Sleep before polling again
        boost::this_thread::sleep(boost::posix_time::milliseconds(poll_interval));
    }
}

LiveModel::Impl::Impl(
        Allocator::Impl* allocator,
        ComputeHardware::Pimpl hardware
        ) :
    Model::Impl(allocator, hardware),
    _bootBlocks(),
    _freeBlocks(),
    _threads(),
    _shutdown(false),
    _allocateThreadReady(false),
    _deallocateThreadReady(false),
    _switchingAllocate(false),
    _switchingDeallocate(false),
    _listeners(),
    _listenersMutex(),
    _bootMutex(),
    _freeMutex(),
    _threadMapMutex(),
    _pipeFd()

{
    // Create pipe for communicating shutdown to threads
    if (0 == pipe(_pipeFd)) {
        try {
            // Startup the allocate (boot) and deallocate (free) monitor threads
            boost::thread* allocateThreadPtr = new boost::thread(boost::bind(&LiveModel::Impl::allocateBlockThread, this));
            boost::thread* deallocateThreadPtr = new boost::thread(boost::bind(&LiveModel::Impl::deallocateBlockThread, this));
            LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on thread map");
            // Lock mutex as single writer across multiple threads
            boost::unique_lock<boost::shared_mutex> thread_map_lock(_threadMapMutex);
            LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on thread map");
            _threads[allocateThreadPtr->get_id()] = allocateThreadPtr;
            _threads[deallocateThreadPtr->get_id()] = deallocateThreadPtr;
            // Unlock thread map mutex
            thread_map_lock.unlock();
            LOG_TRACE_MSG(__FUNCTION__ << " Unlocked thread map");
            // Wait for the threads to become ready
            LOG_DEBUG_MSG("Waiting for threads to become ready.");
            while (_allocateThreadReady == false || _deallocateThreadReady == false ) {
                usleep(10000); // Sleep for 10,000 microseconds
            }
            LOG_DEBUG_MSG("Threads are ready.");
        } catch (...) {
            THROW_EXCEPTION(
                    bgsched::allocator::RuntimeException,
                    bgsched::allocator::RuntimeErrors::ThreadError,
                    "Unexpected error creating thread"
            );
        }
    } else {
        THROW_EXCEPTION(
                bgsched::allocator::RuntimeException,
                bgsched::allocator::RuntimeErrors::PipeError,
                "Unexpected error creating pipe"
        );
    }
}

LiveModel::Impl::~Impl()
{
    LOG_DEBUG_MSG("Number of active threads entering destructor " << _threads.size());

    // Set shutdown indicator and write to the pipe to wake-up the threads
    _shutdown = true;

    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on thread map");
    // Lock mutex as single writer across multiple threads
    boost::unique_lock<boost::shared_mutex> thread_map_lock(_threadMapMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on thread map");
    if (!_threads.empty()) {
        char ch[9];
        strcpy(ch, "SHUTDOWN");
        LOG_DEBUG_MSG("Sending shutdown request to threads");
        write(_pipeFd[1], ch, 8);
    }
    // Unlock thread map mutex
    thread_map_lock.unlock();
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked thread map");

    // Join with all of the threads
    map<boost::thread::id, boost::thread* >::iterator iter;
    while (!_threads.empty()) {
        iter = _threads.begin();
        LOG_DEBUG_MSG("Waiting to join thread");
        (*iter).second->join();
    }
    close(_pipeFd[1]);
    close(_pipeFd[0]);
    LOG_DEBUG_MSG("Completed joining threads");
}

void
LiveModel::Impl::syncState( )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    LOG_DEBUG_MSG("Synchronizing LiveModel with machine hardware state");

    try {
        // Get a snapshot of the the compute hardware state from persistent storage
        ComputeHardware::Pimpl hardware = ComputeHardware::Pimpl(new ComputeHardware::Impl());
        // First update the allocator hardware
        _allocator->updateHardware(hardware);
        // Now update the LiveModel hardware state
        _hardware = hardware;
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::allocator::RuntimeException,
                bgsched::allocator::RuntimeErrors::SyncError,
                "Error synchronizing LiveModel with machine hardware state"
        );
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock");
}

void
LiveModel::Impl::dump(
        ostream& os,
        EnumWrapper<Model::DumpVerbosity::Value> verbosity
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Block filter
    BlockFilter blockFilter;
    // Blocks returned from core::getBlocks()
    Block::Ptrs blocks;

    // Check verbosity level on what to dump
    switch (verbosity.toValue()) {
      case Model::DumpVerbosity::Hardware:
        // Dump compute hardware for LiveModel
        _hardware->dump(os);
        break;
      case Model::DumpVerbosity::Blocks:
        // Dump summary list of blocks in brief format
        blockFilter.setExtendedInfo(false);
        try {
            blocks = bgsched::core::getBlocks(blockFilter);
        } catch (...) {
            LOG_ERROR_MSG("Unexpected error retrieving live model blocks.");
        }
        if (blocks.empty()) {
            os << "No blocks found in live model." << endl;
        } else {
            for (Block::Ptrs::iterator iter = blocks.begin(); iter != blocks.end(); iter++) {
                Block::Ptr blockPtr = *iter;
                Block::Pimpl block = blockPtr->getPimpl();
                block->toString(os, false);
            }
        }
        break;
      case Model::DumpVerbosity::BlocksExtended:
        // Dump summary list of blocks in brief format
        blockFilter.setExtendedInfo(true);
        try {
            blocks = bgsched::core::getBlocks(blockFilter);
        } catch (...) {
            LOG_ERROR_MSG("Unexpected error retrieving live model blocks.");
        }
        if (blocks.empty()) {
            os << "No blocks found in live model." << endl;
        } else {
            for (Block::Ptrs::iterator iter = blocks.begin(); iter != blocks.end(); iter++) {
                Block::Ptr blockPtr = *iter;
                Block::Pimpl block = blockPtr->getPimpl();
                block->toString(os, true);
            }
        }
        break;
      case Model::DumpVerbosity::DrainList:
        // Dump midplane drain list for model
        if (_drainedMidplanes.empty()) {
            os << "No midplane locations found in live model drain list." << endl;
        } else {
            os << "List of midplane locations in live model drain list:" << endl;
            for (Model::DrainedMidplanes::const_iterator iter = _drainedMidplanes.begin(); iter != _drainedMidplanes.end(); iter++) {
                os << "Drained midplane location: " << *iter << endl;
            }
        }
        break;
      default:
        LOG_WARN_MSG("Unknown dump verbosity level");
        break;
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock");
}

void
LiveModel::Impl::allocate(
        const string& blockName
        )
{
    std::vector<std::string> unavailableResources;   // Unavailable block resources
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
        );
    }

    // Block filter
    BlockFilter block_filter;
    block_filter.setName(blockName);
    block_filter.setExtendedInfo(false);

    // Block returned from core::getBlocks()
    Block::Ptrs blocks;
    try {
        // Get the block detail including sequence ID from the database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        LOG_ERROR_MSG("Error occurred while attempting to allocate block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a block back?
    if (blocks.empty()) {
        LOG_ERROR_MSG("Error occurred while attempting to allocate block " << blockName);
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }

    Block::Ptr blockPtr = blocks[0];

    // Check if block has pending action
    if (blockPtr->getAction() != Block::Action::None) {
        LOG_ERROR_MSG("Error occurred while attempting to allocate block " << blockName);
        THROW_EXCEPTION( // Block already has action pending
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockBootError,
                "Block boot request failed because block " << blockName << " has pending action."
                );
    }

    // Only post the block to the allocate monitoring thread if there are listeners
    if (!_listeners.empty()) {
        // Check if block is already being monitored for allocate
        AllocatingBlocks allocatingBlocks = getAllocatingBlocks();
        AllocatingBlocks::iterator it = allocatingBlocks.find(blockName);
        if (it != allocatingBlocks.end()) {
            LOG_ERROR_MSG("Error occurred while attempting to allocate block " << blockName);
            THROW_EXCEPTION(
                    bgsched::RuntimeException,
                    bgsched::RuntimeErrors::BlockBootError,
                    "Block boot request failed because either block " << blockName << " is not Free or has pending action."
            );
        }

        // If polling the database we could get status before the boot request is processed.
        // To prevent this we want the seq id to be a higher value so we know the status changed.
        addAllocatingBlock(blockName, blockPtr->getSequenceId());

        // Check if block is already being monitored for deallocate
        DeallocatingBlocks deallocatingBlocks = getDeallocatingBlocks();
        DeallocatingBlocks::iterator it2 = deallocatingBlocks.find(blockName);
        // Only add to deallocate watch list if not already in the list
        if (it2 == deallocatingBlocks.end()) {
            // If polling the database we could get status before the free request is processed.
            // To prevent this we want the seq id to be a higher value so we know the status changed.
            // LoadLeveler requires that callbacks for deallocates be sent back on blocks they allocated since
            // deallocates can be done outside there control (e.g. RAS event).
            addDeallocatingBlock(blockName, blockPtr->getSequenceId());
        }
    }
    try {
        // Note: There are two versions of initiateBoot(). The version we are using does more extensive
        // checking to verify resources are available for the compute block to boot successfully.
        Block::initiateBoot(blockName, &unavailableResources);
    } catch (...) {
        if (unavailableResources.size() > 0) {
            // Print the unavailable compute resources
            LOG_ERROR_MSG("Compute block " << blockName << " has " << unavailableResources.size() << " unavailable resources:");
            for (unsigned int it = 0; it < unavailableResources.size(); it++) {
                LOG_ERROR_MSG(unavailableResources[it]);
            }
        } else {
            LOG_DEBUG_MSG("All compute resources are available for compute block " << blockName);
        }

        LOG_ERROR_MSG("Error occurred while attempting to allocate block " << blockName);
        removeAllocatingBlock(blockName);
        removeDeallocatingBlock(blockName);

        // Rethrow the exception
        throw;
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock");
}

void
LiveModel::Impl::deallocate(
        const string& blockName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
        );
    }

    // Block filter
    BlockFilter block_filter;
    block_filter.setName(blockName);
    block_filter.setExtendedInfo(false);

    // Block returned from core::getBlocks()
    Block::Ptrs blocks;
    try {
        // Get the block detail including sequence ID from the database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        LOG_ERROR_MSG("Error occurred while attempting to deallocate block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a block back?
    if (blocks.empty()) {
        LOG_ERROR_MSG("Error occurred while attempting to deallocate block " << blockName);
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }
    Block::Ptr blockPtr = blocks[0];

    // If the block is already Free just return
    if (blockPtr->getStatus() == Block::Free) {
        return;
    }

    // Check if block has pending action
    if (blockPtr->getAction() != Block::Action::None) {
        LOG_ERROR_MSG("Error occurred while attempting to deallocate block " << blockName);
        THROW_EXCEPTION( // Block already has action pending
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockFreeError,
                "Block free request failed because block " << blockName << " has pending action."
        );
    }

    // Only post the block to the deallocate monitoring thread if there are listeners
    if (!_listeners.empty()) {
        // Check if block is already being monitored for deallocate
        DeallocatingBlocks deallocatingBlocks = getDeallocatingBlocks();
        DeallocatingBlocks::iterator it = deallocatingBlocks.find(blockName);
        // Only add to deallocate watch list if not already in the list. It is normal for allocate() method to insert
        // the block on the deallocate watch list.
        if (it == deallocatingBlocks.end()) {
            // If polling the database we could get status before the free request is processed.
            // To prevent this we want the seq id to be a higher value so we know the status changed.
            addDeallocatingBlock(blockName, blockPtr->getSequenceId());
        }
    }

    // Block is not already Free and has no pending action so initiate a Free request on the block
    try {
        Block::initiateFree(blockName);
    } catch (...) {
        LOG_ERROR_MSG("Error occurred while attempting to deallocate block " << blockName);
        removeDeallocatingBlock(blockName);
        // Rethrow the exception
        throw;
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock");
}

const Block::Pimpl
LiveModel::Impl::getBlock(
        const string& blockName
        )
{
    BlockFilter filter;
    filter.setName(blockName);
    filter.setExtendedInfo(true);
    Block::Ptrs blocks;
    try {
        blocks = bgsched::core::getBlocks(filter);
    } catch (...) {
        // Rethrow the exception
        throw;
    }

    if (blocks.size() == 0)
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    return blocks[0]->getPimpl();
}

EnumWrapper<Block::Status>
LiveModel::Impl::getBlockStatus(
        const string& blockName
        )
{
    // Verify block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
        );
    }
    BGQDB::DBTBlock dbo;
    // Validate the block name size
    if (blockName.size() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Block filter
    BlockFilter block_filter;
    block_filter.setName(blockName);
    block_filter.setExtendedInfo(false);

    // Block returned from core::getBlocks()
    Block::Ptrs blocks;
    try {
        // Check if compute block exists in database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        LOG_ERROR_MSG("Error occurred while attempting to get status for compute block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a compute block back?
    if (blocks.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }

    BGQDB::BLOCK_STATUS state;
    BGQDB::STATUS result = BGQDB::getBlockStatus(blockName, state);
    switch (result) {
    case BGQDB::OK:
        break;
    case BGQDB::DB_ERROR:
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    case BGQDB::CONNECTION_ERROR:
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    case BGQDB::INVALID_ID: // Block name not correct
    case BGQDB::NOT_FOUND:  // Block not found
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
        );
    case BGQDB::FAILED:    // Block in unknown state
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::InvalidBlockState,
                "Invalid compute block status for " << blockName
        );
    default :
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::UnexpectedError,
                Unexpected_Return_Code_Str
        );
    }
    EnumWrapper<Block::Status> status (Block::Impl::convertDBBlockStateToBlockStatus( state ));

    return status;
}

void
LiveModel::Impl::addBlock(
        const Block::Pimpl block,
        const string& owner
        )
{
    // Verify we got a valid block pointer
    if (!block) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotAdded,
                "Block parameter is empty, block not added."
        );
    }

    try {
        // Add the block to database
        block->add(owner, false);
    } catch (...) { // Handle any exception adding block
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotAdded,
                "Error prevented block " << block->getName() << " from being added"
        );
    }
}

void
LiveModel::Impl::removeBlock(
        const string& blockName
        )
{
    // Verify block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
        );
    }
    BGQDB::DBTBlock dbo;
    // Validate the block name size
    if (blockName.size() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Block filter
    BlockFilter block_filter;
    block_filter.setName(blockName);
    block_filter.setExtendedInfo(false);

    // Block returned from core::getBlocks()
    Block::Ptrs blocks;
    try {
        // Check if compute block exists in database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        LOG_ERROR_MSG("Error occurred while attempting to remove compute block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a compute block back?
    if (blocks.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }

    // Remove the block from the database
    try {
        Block::remove(blockName);
    } catch (...) {
        LOG_ERROR_MSG("Error occurred while attempting to remove block " << blockName);
        // Rethrow the exception
        throw;
    }
}

Block::Ptrs
LiveModel::Impl::getFilteredBlocks(
        const BlockFilter& filter
        )
{
    return bgsched::core::getBlocks(filter);
}

void
LiveModel::Impl::registerListener(
        AllocatorEventListener& listener
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on listener container");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_listenersMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on listener container");

    _listeners.push_back(&listener);
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock on listener container");
}

void
LiveModel::Impl::notifyAllocateListeners(
        const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& blockStatusEvent,
        const string& error_msg
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock on listener container");
    // Lock mutex as shared reader across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_listenersMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock on listeners container");

    AllocatorEventListener::AllocateCompleted::Pimpl info(new AllocatorEventListener::AllocateCompleted::Impl(blockStatusEvent, error_msg));
    LOG_DEBUG_MSG("Notifying allocate listeners that block " << blockStatusEvent.getBlockName() << " status changed.");

    // Notify each listener of block status change
    if (!_listeners.empty()) {
        for_each(
                _listeners.begin(),
                _listeners.end(),
                boost::bind(&AllocatorEventListener::handleAllocate, _1, info)
        );
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked shared read lock on listeners container");
}

void
LiveModel::Impl::notifyDeallocateListeners(
        const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& blockStatusEvent,
        const string& error_msg
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock on listeners container");
    // Lock mutex as shared reader across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_listenersMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock on listeners container");

    AllocatorEventListener::DeallocateCompleted::Pimpl info(new AllocatorEventListener::DeallocateCompleted::Impl(blockStatusEvent, error_msg));
    LOG_DEBUG_MSG("Notifying deallocate listeners that block " << blockStatusEvent.getBlockName() << " status changed.");

    // Notify each listener of block status change
    if (!_listeners.empty()) {
        for_each(
                _listeners.begin(),
                _listeners.end(),
                boost::bind(&AllocatorEventListener::handleDeallocate, _1, info)
        );
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked shared read lock on listeners container");
}

void
LiveModel::Impl::notifyAllocated(
        const string& blockName
        )
{
    realtime::ClientEventListener::BlockStateChangedEventInfo::Impl blockStatusEvent ( // Dummy block status change event
            blockName,
            Block::Initialized,
            0,
            Block::Booting,
            0
    );
    string error_msg;
    notifyAllocateListeners(blockStatusEvent, error_msg);
}

void
LiveModel::Impl::notifyDeallocated(
        const string& blockName
        )
{
    realtime::ClientEventListener::BlockStateChangedEventInfo::Impl blockStatusEvent ( // Dummy block status change event
            blockName,
            Block::Free,
            0,
            Block::Initialized,
            0
    );
    string error_msg;
    notifyDeallocateListeners(blockStatusEvent, error_msg);
}

LiveModel::Impl::AllocatingBlocks
LiveModel::Impl::getAllocatingBlocks()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock on boot block container");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_bootMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock on boot block container");
    AllocatingBlocks blocks = _bootBlocks;
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked shared read lock on boot block container");
    return blocks;
}

LiveModel::Impl::DeallocatingBlocks
LiveModel::Impl::getDeallocatingBlocks()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock on free block container");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_freeMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock on free block container");
    DeallocatingBlocks blocks = _freeBlocks;
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked shared read lock on free block container");
    return blocks;
}

void
LiveModel::Impl::addAllocatingBlock(
        const string& blockName,
        bgsched::SequenceId seqId
        )
{
    // When switching from database polling to real-time events need to wait for handler to become ready so
    // no events are missed
    while (isSwitchingAllocateHandler()) {
        usleep(5000); // Sleep for 5,000 microseconds
    }

    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on boot block container");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_bootMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on boot block container");
    LOG_TRACE_MSG("Adding block " << blockName << " with sequence ID " << seqId << " to allocate monitor list");
    _bootBlocks.insert(AllocatingBlocks::value_type(blockName, seqId));
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock on boot block container");
}

void
LiveModel::Impl::monitorBlockAllocate(
        const string& blockName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Only post the block to the allocate monitoring thread if there are listeners
    if (_listeners.empty()) {
        LOG_WARN_MSG("Ignoring request to monitor booting of compute block " << blockName << ". No callbacks registered.");
        return;
    }

    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
        );
    }

    // Block filter
    BlockFilter block_filter;
    block_filter.setName(blockName);
    block_filter.setExtendedInfo(false);

    // Block returned from core::getBlocks()
    Block::Ptrs blocks;
    try {
        // Get the block detail including sequence ID from the database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a block back?
    if (blocks.empty()) {
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName);
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }

    Block::Ptr blockPtr = blocks[0];
    SequenceId seqId = blockPtr->getSequenceId();
    // Check if block already has Free or Initialized state which is what we will be monitoring for
    if (blockPtr->getStatus() == Block::Free || blockPtr->getStatus() == Block::Initialized) {
        if (blockPtr->getStatus() == Block::Free) {
            LOG_ERROR_MSG("Request to monitor booting of compute block " << blockName << " failed. Block has 'Free' status.");
            THROW_EXCEPTION( // Block has 'Free' status
                    bgsched::RuntimeException,
                    bgsched::RuntimeErrors::InvalidBlockState,
                    "Request to monitor booting of compute block " << blockName << " failed. Block has 'Free' status."
            );
        } else {
            LOG_ERROR_MSG("Request to monitor booting of compute block " << blockName << " failed. Block has 'Initialized' status.");
            THROW_EXCEPTION( // Block has 'Initialized' status
                    bgsched::RuntimeException,
                    bgsched::RuntimeErrors::InvalidBlockState,
                    "Request to monitor booting of compute block " << blockName << " failed. Block has 'Initialized' status."
            );
        }
    }

    // Check if compute block is already being monitored for allocate
    AllocatingBlocks allocatingBlocks = getAllocatingBlocks();
    AllocatingBlocks::iterator it = allocatingBlocks.find(blockName);
    // Only add the compute block if not currently being monitored
    if (it == allocatingBlocks.end()) {
        // The seqId is used to determine if status changed since it was posted on the monitor list
        addAllocatingBlock(blockName, seqId);
    } else {
        return;
    }

    // Close any timing windows on compute block status change by reverifying block status. Don't want to miss the status event.
    try {
        // Get the compute block details including sequence ID from the database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        removeAllocatingBlock(blockName);
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a compute block back?
    if (blocks.empty()) {
        removeAllocatingBlock(blockName);
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName << ". Block may have been deleted.");
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }

    blockPtr = blocks[0];
    SequenceId recentSeqId = blockPtr->getSequenceId();

    // Check if the compute block status changed since we posted to the monitor list
    if (recentSeqId > seqId) {
        // The sequence id indicates that the block status changed since we first looked and posted it to monitor list.
        // Check if compute block now has state of Free or Initialized which is what we were monitoring for.
        if (blockPtr->getStatus() == Block::Free || blockPtr->getStatus() == Block::Initialized) {
            removeAllocatingBlock(blockName);
            if (blockPtr->getStatus() == Block::Free) {
                LOG_ERROR_MSG("Request to monitor booting of compute block " << blockName << " failed. Block has 'Free' status.");
                THROW_EXCEPTION( // Compute block has 'Free' status
                        bgsched::RuntimeException,
                        bgsched::RuntimeErrors::InvalidBlockState,
                        "Request to monitor booting of compute block " << blockName << " failed. Block has 'Free' status."
                );
            } else {
                LOG_ERROR_MSG("Request to monitor booting of block " << blockName << " failed. Block has 'Initialized' status.");
                THROW_EXCEPTION( // Compute block has 'Initialized' status
                        bgsched::RuntimeException,
                        bgsched::RuntimeErrors::InvalidBlockState,
                        "Request to monitor booting of compute block " << blockName << " failed. Block has 'Initialized' status."
                );
            }
        }
    }

    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock");
}

void
LiveModel::Impl::addDeallocatingBlock(
        const string& blockName,
        bgsched::SequenceId seqId
        )
{
    // When switching from database polling to real-time events need to wait for handler to become ready so
    // no events are missed
    while (isSwitchingDeallocateHandler()) {
        usleep(5000); // Sleep for 5,000 microseconds
    }

    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on free block container");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_freeMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on free block container");
    LOG_TRACE_MSG("Adding block " << blockName << " with sequence ID " << seqId << " to deallocate monitor list");
    _freeBlocks.insert(DeallocatingBlocks::value_type(blockName, seqId));
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock on free block container");
}

void
LiveModel::Impl::monitorBlockDeallocate(
        const string& blockName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Only post the block to the deallocate monitoring thread if there are listeners
    if (_listeners.empty()) {
        LOG_WARN_MSG("Ignoring request to monitor freeing of compute block " << blockName << ". No callbacks registered.");
        return;
    }

    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
        );
    }

    // Block filter
    BlockFilter block_filter;
    block_filter.setName(blockName);
    block_filter.setExtendedInfo(false);

    // Block returned from core::getBlocks()
    Block::Ptrs blocks;
    try {
        // Get the block detail including sequence ID from the database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a block back?
    if (blocks.empty()) {
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName);
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }

    Block::Ptr blockPtr = blocks[0];
    SequenceId seqId = blockPtr->getSequenceId();
    // Check if block already has Free state which is what we will be monitoring for
    if (blockPtr->getStatus() == Block::Free) {
        LOG_ERROR_MSG("Request to monitor freeing of compute block " << blockName << " failed. Block has 'Free' status.");
        THROW_EXCEPTION( // Block has 'Free' status
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::InvalidBlockState,
                "Request to monitor freeing of compute block " << blockName << " failed. Block has 'Free' status."
        );
    }

    // Check if compute block is already being monitored for deallocate
    DeallocatingBlocks deallocatingBlocks = getDeallocatingBlocks();
    DeallocatingBlocks::iterator it = deallocatingBlocks.find(blockName);
    // Only add the compute block if not currently being monitored
    if (it == deallocatingBlocks.end()) {
        // The seqId is used to determine if status changed since it was posted on the monitor list
        addDeallocatingBlock(blockName, seqId);
    } else {
        return;
    }

    // Close any timing windows on compute block status change by reverifying block status. Don't want to miss the status event.
    try {
        // Get the compute block details including sequence ID from the database
        blocks = bgsched::core::getBlocks(block_filter);
    } catch (...) {
        removeDeallocatingBlock(blockName);
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName);
        // Rethrow the exception
        throw;
    }

    // Did we get a compute block back?
    if (blocks.empty()) {
        removeDeallocatingBlock(blockName);
        LOG_ERROR_MSG("Error while attempting to get details for compute block " << blockName << ". Block may have been deleted.");
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " not found."
                );
    }

    blockPtr = blocks[0];
    SequenceId recentSeqId = blockPtr->getSequenceId();

    // Check if the compute block status changed since we posted to the monitor list
    if (recentSeqId > seqId) {
        // The sequence id indicates that the block status changed since we first looked and posted it to monitor list.
        // Check if compute block now has state of Free which is what we were monitoring for.
        if (blockPtr->getStatus() == Block::Free) {
            removeDeallocatingBlock(blockName);
            LOG_ERROR_MSG("Request to monitor freeing of compute block " << blockName << " failed. Block has 'Free' status.");
            THROW_EXCEPTION( // Compute block has 'Free' status
                    bgsched::RuntimeException,
                    bgsched::RuntimeErrors::InvalidBlockState,
                    "Request to monitor freeing of compute block " << blockName << " failed. Block has 'Free' status."
            );
        }
    }

    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock");
}

void
LiveModel::Impl::removeAllocatingBlock(
        const string& blockName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on boot block container");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_bootMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on boot block container");

    AllocatingBlocks::iterator it = _bootBlocks.find(blockName);
    if (it != _bootBlocks.end()) {
        LOG_TRACE_MSG("Removing block " << blockName << " from allocate monitor list");
        _bootBlocks.erase(it);
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock on boot block container");
}

void
LiveModel::Impl::removeDeallocatingBlock(
        const string& blockName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock on free block container");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_freeMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock on free block container");

    DeallocatingBlocks::iterator it = _freeBlocks.find(blockName);
    if (it != _freeBlocks.end()) {
        LOG_TRACE_MSG("Removing block " << blockName << " from deallocate monitor list");
        _freeBlocks.erase(it);
    }
    LOG_TRACE_MSG(__FUNCTION__ << " Unlocked exclusive write lock on free block container");
}

void
LiveModel::Impl::setAllocateThreadReady()
{
    _allocateThreadReady = true;
}

void
LiveModel::Impl::setDeallocateThreadReady()
{
    _deallocateThreadReady = true;
}

void
LiveModel::Impl::setSwitchingAllocateHandler(
        bool switchingHandler
        )
{
    _switchingAllocate = switchingHandler;
}

void
LiveModel::Impl::setSwitchingDeallocateHandler(
        bool switchingHandler
        )
{
    _switchingDeallocate = switchingHandler;
}

bool
LiveModel::Impl::isSwitchingAllocateHandler()
{
    return _switchingAllocate;
}

bool
LiveModel::Impl::isSwitchingDeallocateHandler()
{
    return _switchingDeallocate;
}

} // namespace bgsched::allocator
} // namespace bgsched
