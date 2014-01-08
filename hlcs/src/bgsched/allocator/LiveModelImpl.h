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

/*!
 * \file bgsched/allocator/LiveModelImpl.h
 * \brief LiveModel::Impl class definition.
 */

#ifndef BGSCHED_ALLOCATOR_LIVEMODEL_IMPL_H_
#define BGSCHED_ALLOCATOR_LIVEMODEL_IMPL_H_

#include <bgsched/allocator/LiveModel.h>
#include <bgsched/allocator/ResourceSpec.h>

#include "bgsched/allocator/ModelImpl.h"

#include "../realtime/ClientEventListenerImpl.h"

#include <bgsched/Block.h>
#include <bgsched/Job.h>
#include <bgsched/types.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <map>
#include <string>
#include <vector>

namespace bgsched {
namespace allocator {

/*!
 * \brief Represents Blue Gene compute blocks and snapshot state of the actual machine hardware.
 *
 * Note: A single LiveModel object is contained within the Allocator object and can not be constructed
 * independently.
 *
 * The live model state is **NEVER** updated automatically. State updates are triggered by calling
 * the syncState() method.
 */
class LiveModel::Impl : public Model::Impl
{
public:

    typedef std::multimap<std::string, bgsched::SequenceId> AllocatingBlocks;    //!< Collection of blocks being allocated
    typedef std::multimap<std::string, bgsched::SequenceId> DeallocatingBlocks;  //!< Collection of blocks being deallocated
    typedef std::vector<AllocatorEventListener*> Listeners;                 //!< Container of event listeners
    typedef std::map<boost::thread::id, boost::thread*> ThreadMap;          //!< Map of threads

    /*!
     * \brief
     *
     * \throws bgsched::allocator::RuntimeException with values:
     * - bgsched::allocator::RuntimeErrors::PipeError - if unexpected error creating pipe
     * - bgsched::allocator::RuntimeErrors::ThreadError - if unexpected error creating thread
     */
    Impl(
            Allocator::Impl* allocator,     //!< [in] Pointer to allocator implementation
            ComputeHardware::Pimpl hardware //!< [in] Pointer to compute hardware implementation
            );

    /*!
     * \brief
     */
    ~Impl();

    /*!
     * \brief Initiate the allocating (booting) of a compute block.
     *
     * The compute block must have a status of Free or an exception is thrown.
     * The boot process is done asynchronously so the status of the compute block
     * must be checked to determine if the compute block was successfully booted.
     *
     * Registered listeners will be notified when the compute block is available to run a job or if
     * boot of compute block failed.
     *
     * The compute block owner will be set to the user calling the method.
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::BlockBootError - if block status is not Free or block action already pending
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    void allocate(
            const std::string& blockName //!< [in] Compute block to allocate
            );

    /*!
     * \brief Initiate the deallocating (freeing) of a compute block.
     *
     * The freeing of the compute block is done asynchronously so the status of the compute block
     * must be checked to determine if the compute block was successfully freed.
     *
     * Registered listeners will be notified when the compute block resources are freed.
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::BlockFreeError - if block has block action already pending
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    void deallocate(
            const std::string& blockName //!< [in] Compute block to deallocate
            );

    /*!
     * \brief Add compute block to database.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::BlockNotAdded - if the block was not added to the database
     */
    void addBlock(
            const Block::Pimpl block,  //!< [in] Compute block to add to the database
            const std::string& owner   //!< [in] Compute block owner
            );

    /*!
     * \brief Remove compute block from database.
     *
     * A block can only be removed if its status is Free.
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::InvalidBlockState - if block status is not Free
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    void removeBlock(
            const std::string& blockName  //!< [in] Compute block name to remove
            );

    /*!
     * \brief Get compute block from database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched:DatabaseErrors::ConnectionError - if error occurs connecting to the database
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     * \return Compute block from database.
     */
    const Block::Pimpl getBlock(
            const std::string& blockName  //!< [in] Compute block to retrieve info on
            );

    /*!
     * \brief Get compute block status from database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     * - bgsched::DatabaseErrors::UnexpectedError - if unexpected database error
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::InvalidBlockState - if unknown block status
     *
     * \return Compute block status from database.
     */
    EnumWrapper<Block::Status> getBlockStatus(
            const std::string& blockName  //!< [in] Compute block to retrieve status on
            );

    /*!
     * \brief Get compute block(s) from database based on filter criteria.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched:DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     * \return Compute block(s) from database matching filter criteria.
     */
    Block::Ptrs getFilteredBlocks(
            const BlockFilter& spec  //!< [in] Compute block filter criteria
            );

    /*!
     * \brief Register a listener for allocator events.
     *
     * The appropriate methods of the listener are called when an allocation or deallocation completes.
     */
    void registerListener(
            AllocatorEventListener& listener //!< [in] Implementation of event listener
            );

    /*!
     * \brief Notify listeners that a compute block allocate has completed. In normal circumstances
     * the allocate() interface is used and it will notify listeners if the allocate is
     * successful or unsuccessful. This interface can be used for other situations such as
     * reusing already booted blocks and listeners need to know the block is ready for running
     * jobs.
     *
     * This interface will notify listeners that the allocation of resources for the block is complete.
     *
     * No verification is done that the compute block is actually allocated.
     */
    void notifyAllocated(
            const std::string& blockName  //!< [in] Compute block allocated
            );

    /*!
     * \brief Notify listeners that a compute block deallocate has completed. In normal circumstances
     * the deallocate() interface is used and it will notify listeners if the deallocate is
     * successful or unsuccessful.
     *
     * This interface will notify listeners that the deallocation of resources for the block is complete.
     *
     * No verification is done that the compute block is actually deallocated.
     */
    void notifyDeallocated(
            const std::string& blockName  //!< [in] Compute block deallocated
            );

    /*!
     * \brief Notify listeners the allocate request results.
     */
    void notifyAllocateListeners(
            const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& blockStatusEvent, //!< [in] Compute block status event
            const std::string& error_msg   //!< [in] Error message (if any)
            );

    /*!
     * \brief Notify listeners the deallocate request results.
     */
    void notifyDeallocateListeners(
            const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& blockStatusEvent, //!< [in] Compute block status event
            const std::string& error_msg   //!< [in] Error message (if any)
            );

    /*!
     * \brief Synchronize the state in this model with hardware state.
     *
     * This gets a snapshot of the compute hardware state from persistent storage.
     * The model hardware snapshot is never updated automatically. It is the responsibility
     * of a job scheduler to call syncState() to synchronize the state of the model
     * with the Blue Gene machine state tracked in persistent storage. Typically a
     * job scheduler would refresh the model state prior to making a call to findBlockResources()
     * from the allocator.
     *
     * \throws bgsched::allocator::RuntimeException with value:
     * - bgsched::allocator::RuntimeErrors::SyncError - if error synchronizing LiveModel with machine hardware state
     */
    void syncState();

    /*!
     * \brief Dump the live model information to a stream (typically for debugging usage).
     */
    void dump(
           std::ostream& os,                                  //!< [in/out] Stream to write to
           EnumWrapper<Model::DumpVerbosity::Value> verbosity //!< [in] Verbosity level of output
           );

    /*!
     * \brief Thread that handles notifications for compute block allocate (boot).
     */
    void allocateBlockThread();

    /*!
     * \brief Thread that handles notifications for compute block deallocate (free).
     */
    void deallocateBlockThread();

    /*!
     * \brief Handles notifications for compute block allocates (boot) using real-time events.
     */
    void realtimeAllocateBlocks();

    /*!
     * \brief Handles notifications for compute block deallocates (free) using real-time events.
     */
    void realtimeDeallocateBlocks();

    /*!
     * \brief Handles notifications for compute block allocates (boot) using database polling.
     */
    void pollAllocateBlocks();

    /*!
     * \brief Handles notifications for compute block deallocates (free) using database polling.
     */
    void pollDeallocateBlocks();

    /*!
     * \brief Get container of blocks being allocated.
     */
    AllocatingBlocks getAllocatingBlocks();

    /*!
     * \brief Get container of blocks being deallocated.
     */
    DeallocatingBlocks getDeallocatingBlocks();

    /*!
     * \brief Add block to list of blocks being allocated.
     */
    void addAllocatingBlock(
            const std::string& blockName,  //!< [in] Compute block to add to list
            bgsched::SequenceId seqId      //!< [in] Sequence ID at time compute block is added to list
            );

    /*!
     * \brief Add block to list of blocks being deallocated.
     */
    void addDeallocatingBlock(
            const std::string& blockName,  //!< [in] Compute block to add to list
            bgsched::SequenceId seqId      //!< [in] Sequence ID at time compute block is added to list
            );

    /*!
     * \brief Remove block from list of blocks being allocated.
     */
    void removeAllocatingBlock(
            const std::string& blockName   //!< [in] Compute block to remove from list
            );

    /*!
     * \brief Remove block from list of blocks being deallocated.
     */
    void removeDeallocatingBlock(
            const std::string& blockName   //!< [in] Compute block to remove from list
            );

    /*!
     * \brief Set allocate thread indicator to ready state.
     */
    void setAllocateThreadReady();

    /*!
     * \brief Set deallocate thread indicator to ready state.
     */
    void setDeallocateThreadReady();

    /*!
     * \brief Set allocate indicator that switching from polling to real-time events.
     */
    void setSwitchingAllocateHandler(
            bool switchingHandler  //!< [in] Indicator that software is switching handlers for allocates
            );

    /*!
     * \brief Set deallocate indicator that switching from polling to real-time events.
     */
    void setSwitchingDeallocateHandler(
            bool switchingHandler  //!< [in] Indicator that software is switching handlers for deallocates
            );

    /*!
     * \brief Indicator if in process of switching from polling to real-time events for allocates.
     *
     * \return True switching from polling to real-time events for allocates.
     */
    bool isSwitchingAllocateHandler();

    /*!
     * \brief Indicator if in process of switching from polling to real-time events for deallocates.
     *
     \return True switching from polling to real-time events for deallocates.
     */
    bool isSwitchingDeallocateHandler();

    /*!
     * \brief Request that the LiveModel start monitoring the specified compute block for allocate status.
     *
     * Registered listeners will be notified when the compute block is available to run a job or if
     * the boot of the compute block failed.
     *
     * \note This method was added in V1R2M1.
     * \ingroup V1R2M1
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::InvalidBlockState - if compute block status is Free or Initialized
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    void monitorBlockAllocate(
            const std::string& blockName  //!< [in] Compute block to monitor for allocate
    );

    /*!
     * \brief Request that the LiveModel start monitoring the specified compute block for deallocate status.
     *
     * Registered listeners will be notified when the compute block resources are freed.
     *
     * \note This method was added in V1R2M1.
     * \ingroup V1R2M1
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::InvalidBlockState - if compute block status is Free
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    void monitorBlockDeallocate(
            const std::string& blockName  //!< [in] Compute block to watch for deallocate
    );

protected:

    AllocatingBlocks       _bootBlocks;            //!< Container of blocks being booted
    DeallocatingBlocks     _freeBlocks;            //!< Container of blocks being freed
    ThreadMap              _threads;               //!< Container of allocate/deallocate threads
    bool                   _shutdown;              //!< Thread shutdown indicator
    bool                   _allocateThreadReady;   //!< Allocate thread ready indicator
    bool                   _deallocateThreadReady; //!< Allocate thread ready indicator
    bool                   _switchingAllocate;     //!< Allocate thread switching to real-time events
    bool                   _switchingDeallocate;   //!< Deallocate thread switching to real-time events
    Listeners              _listeners;             //!< Listeners for callbacks
    boost::shared_mutex    _listenersMutex;        //!< Mutex to protect listeners container
    boost::shared_mutex    _bootMutex;             //!< Mutex to protect boot block containers
    boost::shared_mutex    _freeMutex;             //!< Mutex to protect free block containers
    boost::shared_mutex    _threadMapMutex;        //!< Mutex to protect access to thread map
    int                    _pipeFd[2];             //!< Pipe file descriptors for communicating shutdown with threads

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
