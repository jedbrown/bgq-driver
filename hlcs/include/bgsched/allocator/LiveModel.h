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
 * \file bgsched/allocator/LiveModel.h
 * \brief LiveModel class definition.
 */

#ifndef BGSCHED_ALLOCATOR_LIVE_MODEL_H_
#define BGSCHED_ALLOCATOR_LIVE_MODEL_H_

#include <bgsched/allocator/AllocatorEventListener.h>
#include <bgsched/allocator/Model.h>

#include <bgsched/Coordinates.h>

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
class LiveModel : public Model, boost::noncopyable
{
public:

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
            const std::string& blockName  //!< [in] Compute block to allocate
    );

    /*!
     * \brief Initiate the deallocating (freeing) of a compute block.
     *
     * The freeing of the compute block is done asynchronously so the status of the compute block
     * must be checked to determine if the compute block was successfully freed.
     *
     * Registered listeners will be notified when the compute block resources are freed.
     *
     * Note: If the compute block is already in a Free state this method will just return without
     * throwing any exception.
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
            const std::string& blockName  //!< [in] Compute block to deallocate
    );

    /*!
     * \brief Add compute block to database.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::BlockNotAdded - if the block was not added to the database
     */
    void addBlock(
            const Block::Ptr block,   //!< [in] Compute block to add to the database
            const std::string& owner  //!< [in] Compute block owner
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
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     * \return Compute block from database.
     */
    const Block::Ptr getBlock(
            const std::string& blockName  //!< [in] Compute block to retrieve info on
    ) const;

    /*!
     * \brief Get compute block status from database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     * - bgsched::DatabaseErrors::UnexpectedError - if unexpected database error
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidBlockName - if the block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if block name was not found
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::InvalidBlockState - if unknown block status
     *
     * \return Compute block status from database.
     */
    EnumWrapper<Block::Status> getBlockStatus(
            const std::string& blockName  //!< [in] Compute block to retrieve status on
    ) const;

    /*!
     * \brief Get compute block(s) from database based on filter criteria.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched:DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InternalException with value:     *
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     * \return Compute block(s) from database matching filter criteria.
     */
    Block::Ptrs getFilteredBlocks(
            const BlockFilter& spec  //!< [in] Compute block filter criteria
    ) const;

    /*!
     * \brief Get all midplanes.
     *
     * \return All midplanes.
     */
    Midplanes::ConstPtr getMidplanes() const;

    /*!
     * \brief Get midplane at specific coordinate.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::InvalidMidplaneCoordinates - if coordinates are invalid
     *
     * \return Midplane at specific coordinate.
     */
    Midplane::Ptr getMidplane(
            const bgsched::Coordinates& coordinates //!< [in] Coordinates to retrieve midplane for
    ) const;

    /*!
     * \brief Get midplane at specific location.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::InvalidLocationString - if invalid midplane location
     *
     * \return Midplane at specific location.
     */
    Midplane::Ptr getMidplane(
            const std::string& location //!< [in] Midplane location like R00-M0
    ) const;

    /*!
     * \brief Get list of drained midplanes for the model. Midplanes in the drain list are listed by location.
     *
     * \return List of drained midplanes for the model.
     */
    Model::DrainedMidplanes getDrainedMidplanes() const;

    /*!
     * \brief Add drained midplane for the model. The drain list consists of midplane locations to exclude
     * when searching for block resources.
     */
    void addDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to add to drain list
    );

    /*!
     * \brief Remove drained midplane for the model. The drain list consists of midplane locations to exclude
     * when searching for block resources.
     */
    void removeDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to remove from drain list
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
    ) const;

    /*!
     * \brief Register a listener for allocator events.
     *
     * The appropriate methods of the listener are called when an allocation or deallocation completes.
     */
    void registerListener(
            AllocatorEventListener& listener  //!< [in] Implementation of event listener
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
            const std::string& blockName //!< [in] Compute block allocated
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
            const std::string& blockName //!< [in] Compute block deallocated
    );

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
            const std::string& blockName  //!< [in] Compute block to monitor for deallocate
    );

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief
     */
    explicit LiveModel(
            Pimpl impl    //!< [in] Pointer to implementation
    );
};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
