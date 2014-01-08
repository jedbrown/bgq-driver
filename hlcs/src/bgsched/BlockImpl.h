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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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
 * \file bgsched/BlockImpl.h
 * \brief Block::Impl class definition.
 */

#ifndef BGSCHED_BLOCK_IMPL_H_
#define BGSCHED_BLOCK_IMPL_H_

#include <bgsched/Dimension.h>
#include <bgsched/Block.h>
#include <bgsched/Job.h>

#include <bgq_util/include/Location.h>

#include <db/include/api/BlockDatabaseInfo.h>
#include <db/include/api/cxxdb/fwd.h>

#include <boost/array.hpp>

#include <iosfwd>
#include <string>
#include <vector>

class DBVComputeblock;

namespace bgsched {

/*!
 * \brief Represents a compute block of any size.
 *
 */
class Block::Impl
{
public:

    /*!
     * \brief Convert database compute block status to Scheduler API compute block status.
     *
     * \return Scheduler API compute block status converted from single character database compute block status.
     */
    static Block::Status statusDbCharToValue(
            char db_char         //!< [in] Database compute block status
    );

    /*!
     * \brief Convert database compute block status to Scheduler API compute block status.
     *
     * \return Scheduler API compute block status converted from database compute block status.
     */
    static Block::Status convertDBStatusToBlockStatus(
            const char *status   //!< [in] Database compute block status
    );

    /*!
     * \brief Convert Scheduler API compute block status to database compute block status.
     *
     * \return Database compute block status in single character format converted from Scheduler API compute block status.
     */
    static char statusToDbChar(
            Block::Status status  //!< [in] Scheduler API compute block status
    );

    /*!
     * \brief Convert database compute block status in integer format to Scheduler API compute block status.
     *
     * \return Scheduler API compute block status converted from integer database compute block status.
     */
    static Block::Status convertDBBlockStateToBlockStatus(
            const int state  //!< [in] Database compute block status
    );

    /*!
     * \brief Convert database compute block action to Scheduler API compute block action.
     *
     * \return Scheduler API compute block action converted from database compute block action.
     */
    static Block::Action::Value convertDBActionToBlockAction(
            const char *action   //!< [in] Database compute block action
    );


    /*!
     * \brief Create a compute block from the database.
     */
    static Block::Pimpl createFromDatabase(
            const cxxdb::Columns& block_cols,  //!< [in] Database compute block columns
            const bool isBlockExtendedInfo,    //!< [in] Indicates if compute block extended info should be retrieved
            cxxdb::Connection& conn            //!< [in] Database connection
    );

    /*!
     * \brief Copy constructor.
     */
    Impl(
            const std::string& name,        //!< [in] New compute block name
            const Block::Pimpl fromBlock    //!< [in] Compute block to copy from
    );

    /*!
     * \brief Compute block object represented as a human readable string value.
     *
     * \return Compute block object represented as a human readable string value.
     */
    void toString(
            std::ostream& os,  //!< [in/out] Output stream to write compute block details
            bool verbose       //!< [in] true for extended compute block info, false for brief compute block info
    ) const;

    /*!
     * \brief Get the compute block name (Id).
     *
     * This is the compute block's unique name.
     *
     * \return Compute block name.
     */
    const std::string& getName() const;

    /*!
     * \brief Get the compute block creation identifier.
     *
     * A unique creation identifier is automatically generated when a compute block is added to
     * persistent storage (database). The id is stored in the compute block history table and also in the
     * job history table for any job run on the compute block. The creation identifier associates a job with
     * the exact compute block it was run on.
     *
     * \return Compute block creation Id.
     */
    BlockCreationId getCreationId() const;

    /*!
     * \brief Get the compute block description.
     *
     * \return Compute block description.
     */
    const std::string& getDescription() const;

    /*!
     * \brief Get the compute block owner.
     *
     * \return Compute block owner.
     */
    const std::string& getOwner() const;

    /*!
     * \brief Get the compute block user.
     *
     * \return Compute block user.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get the compute block boot options.
     *
     * \return Compute block boot options.
     */
    const std::string& getBootOptions() const;

    /*!
     * \brief Get the compute block microloader image.
     *
     * \return Compute block microloader image.
     */
    const std::string& getMicroLoaderImage() const;

    /*!
     * \brief Get the compute block Node configuration.
     *
     * \return Compute block Node configuration.
     */
    const std::string& getNodeConfiguration() const;

    /*!
     * \brief Get the compute block sequence ID.
     *
     * \return Compute block sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get the compute block status.
     *
     * This is the compute block status, not the status of the underlying hardware.
     *
     * \return Compute block status.
     */
    EnumWrapper<Block::Status> getStatus() const;

    /*!
     * \brief Get the compute block action.
     *
     * \return Compute block action.
     */
    EnumWrapper<Block::Action::Value> getAction() const;

    /*!
     * \brief Get the number of compute nodes in the compute block.
     *
     * \return Number of compute nodes in the compute block.
     */
    uint32_t getComputeNodeCount() const;

    /*!
     * \brief Get the compute block options.
     *
     * \return Compute block options.
     */
    const std::string& getOptions() const;

    /*!
     * \brief Get the job IDs associated with the compute block.
     *
     * \return Job IDs associated with the compute block.
     */
    const std::vector<Job::Id> getJobIds() const;

    /*!
     * \brief Get the compute block dimension size in midplanes. This only returns dimensions for large compute blocks.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small compute block
     *
     * \return Size in midplanes for the given dimension.
     */
    uint32_t getDimensionSize(
            const Dimension& dimension   //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Get the compute block dimension size in midplanes. This only returns dimensions for large compute blocks.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small compute block
     *
     * \return Size in midplanes for the given dimension.
     */
    uint32_t getDimensionSize(
            const bgq::util::Location::Dimension::Value dimension   //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Returns true if compute block is Torus in specified dimension.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small compute block
     *
     * \return True if compute block is Torus in specified dimension.
     */
    bool isTorus(
            const Dimension& dimension     //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Return true if compute block is Mesh in specified dimension.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small compute block
     *
     * \return True if compute block is Mesh in specified dimension.
     */
    bool isMesh(
            const Dimension& dimension     //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Returns true if compute block is large (midplane or larger).
     *
     * \return True if compute block is large (midplane or larger).
     */
    bool isLarge() const;

    /*!
     * \brief Returns true if compute block is small (smaller than a midplane).
     *
     * \return True if compute block is small (smaller than a midplane).
     */
    bool isSmall() const;

    /*!
     * \brief Get the midplane locations this compute block uses.
     *
     * The returned midplanes don't include the pass-through midplanes for a large compute block. Use getPassthroughMidplanes()
     * to obtain any pass-through midplanes.
     *
     * The returned vector may be empty if this compute block was queried from core::getBlocks and extended information wasn't requested.
     *
     * \return Midplane locations the compute block uses.
     */
    Block::Midplanes getMidplanes() const;

    /*!
     * \brief Get the node board locations this compute block uses.
     *
     * There are two cases where this will be an empty vector:
     * - The compute block is a midplane or larger.
     * - The compute block was created using core::getBlocks and extended information wasn't requested.
     *
     * \return Node board locations the compute block uses.
     */
    Block::NodeBoards getNodeBoards() const;

    /*!
     * \brief Get the midplane locations that this compute block uses for pass-through.
     *
     * \return Midplane locations the compute block uses for pass-through.
     */
    Block::PassthroughMidplanes getPassthroughMidplanes() const;

    /*!
     * \brief Set the compute block name (Id).
     *
     * This is a user-defined name limited to 32 characters. The name must be unique.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the compute block name is not valid (empty or exceeds 32 characters)
     */
    void setName(
            const std::string& name  //!< [in] Compute block name
    );

    /*!
     * \brief Set the compute block description.
     *
     * This is a user-defined description limited to 1024 characters, e.g. "Joe's test block"
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockDescription - if the compute block description is not valid (empty or exceeds 1024 characters)
     */
    void setDescription(
            const std::string& description  //!< [in] Description for compute block
    );

    /*!
     * \brief Set the compute block options.
     *
     * This is optional, if not set, the default options will be used. Options are limited to 16 characters.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockOptions - if the compute block options are not valid (empty or exceeds 16 characters)
     */
    void setOptions(
            const std::string& options   //!< [in] Compute block options
    );

    /*!
     * \brief Set the compute block boot options.
     *
     * This is optional, if not set, the default boot options will be used.
     * Boot options are limited to 256 characters.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidBlockBootOptions - if the compute block boot options are not valid (empty or exceeds 256 characters)
     */
    void setBootOptions(
            const std::string& bootOptions  //!< [in] Compute block boot options
    );

    /*!
     * \brief Set the compute block micro-loader image.
     *
     * This is optional, if not set, the default image will be used.
     * The micro-loader image file name is limited to 256 characters.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidBlockMicroLoaderImage - if the compute block micro-loader image file name is not valid (empty or exceeds 256 characters)
     */
    void setMicroLoaderImage(
            const std::string& image  //!< [in] Path to the compute block micro-loader image
    );

    /*!
     * \brief Set the compute block Node configuration.
     *
     * This is optional, if not set, the default Node configuration "CNKDefault" will be used.
     *
     * The Node configuration name is limited to 32 characters.
     *
     * Note: Customized compute block Node configurations are created by System Administrators using the "create_node_config" command
     * from the bg_console. The Node configuration name specified must exist in the database.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockNodeConfiguration - if the compute block Node configuration is not valid (empty or exceeds 32 characters)
     */
    void setNodeConfiguration(
            const std::string& nodeConfig  //!< [in] Node configuration name for the compute block
    );

    /*!
     * \brief Set the compute block status.
     *
     * No validation is done to ensure compute block status transitions are valid.
     */
    void setStatus(
            Block::Status status  //!< [in] Compute block status to set
    );

    /*!
     * \brief Set the compute block action.
     *
     * No validation is done to ensure compute block action transitions are valid.
     */
    void setAction(
            Block::Action::Value action  //!< [in] Compute block action to set
    );

    /*!
     * \brief Add job id using the compute block.
     */
    void addJobId(
            Job::Id jobId  //!< [in] Job ID
    );

    /*!
     * \brief Add compute block to database.
     *
     * This method will add the compute block to the database. If the owner string is empty the user id
     * of the caller will be used as the compute block owner.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::BlockNotAdded - if the compute block was not added to the database
     * - bgsched::InputErrors::InvalidMidplanes - if large compute block and no midplanes found
     *
     * \throws bgsched::RuntimeException with values:
     * - bgsched::RuntimeErrors::BlockAddError - if error setting compute block owner
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed
     *
     * \return BlockDataBaseInfo when validating compute block.
     */
    BGQDB::BlockDatabaseInfo::Ptr add(
            const std::string& owner,  //!< [in] Compute block owner
            bool isValidateBlockOnly   //!< [in] Indicator on whether to add compute block or only validate compute block
    );

    /*!
     * \brief Add a midplane location to this compute block.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidMidplanes - if midplane location is empty
     */
    void addMidplane(
            const std::string& midplaneLocation    //!< [in] Midplane location string
    );

    /*!
     * \brief Add a node board location to this compute block
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidNodeBoards - if node board location is empty
     */
    void addNodeBoard(
            const std::string& nodeBoardLocation   //!< [in] Node board location string
    );

    /*!
     * \brief Add a pass-through midplane location to this compute block.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidPassthroughMidplanes - if pass-through midplane location is empty
     */
    void addPassthroughMidplane(
            const std::string& passthroughMidplane //!< [in] Pass-through midplane location string
    );

    /*!
     * \brief Construct a compute block from a compute block database info object pointer.
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    Impl(
            BGQDB::BlockDatabaseInfo::Ptr blockDBInfoPtr  //!< [in] Compute block database info object pointer
    );

protected:

    /*!
     * \brief Construct a compute block from the database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    Impl(
            const cxxdb::Columns& block_cols,  //!< [in] Database compute block columns
            const bool isBlockExtendedInfo,    //!< [in] Indicates if compute block extended info should be retrieved
            cxxdb::Connection& conn            //!< [in] Database connection
    );

    /*!
     * \brief Fast path to add compute block to database.
     *
     * This method will add the compute block to the database. If the owner string is empty the user id
     * of the caller will be used as the block owner.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::BlockNotAdded - if the compute block was not added to the database
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::BlockAddError - if error setting compute block owner
     *
     * \throws bgsched::DatabaseException with value:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed
     *
     * \return BlockDataBaseInfo when validating compute block.
     */
    BGQDB::BlockDatabaseInfo::Ptr addFastPath(
            const std::string& owner  //!< [in] Compute block owner
    );

protected:

    std::string                    _name;                  //!< Compute block name
    BlockCreationId                _creationId;            //!< Compute block creation Id
    std::string                    _description;           //!< Compute block description
    std::string                    _owner;                 //!< Compute block owner
    std::string                    _user;                  //!< Compute block user (booter of compute block)
    std::string                    _bootOptions;           //!< Compute block boot options
    std::string                    _mImage;                //!< Compute block micro-loader image
    std::string                    _nodeConfig;            //!< Compute block Node configuration
    SequenceId                     _sequenceId;            //!< Compute block sequence Id
    Block::Status                  _status;                //!< Compute block status
    uint32_t                       _numcnodes;             //!< Compute block compute node count
    Coordinates                    _dimensionSizes;        //!< Compute block dimension size in midplanes
    boost::array<Block::Connectivity::Value, Dimension::MidplaneDims> _connectivity; //!< Compute block connectivity by dimension
    std::string                    _options;               //!< Compute block options
    std::vector<Job::Id>           _jobIds;                //!< Compute block job Ids
    Block::Midplanes               _midplanes;             //!< Compute block midplane locations -- may be empty if the info wasn't gathered.
    Block::PassthroughMidplanes    _passthroughMidplanes;  //!< Compute block midplane locations used for pass-through -- may be empty if the info wasn't gathered.
    Block::NodeBoards              _nodeBoards;            //!< Compute block node board locations, will we empty if the hardware info wasn't gathered or the Compute block is Large.
    BGQDB::BlockDatabaseInfo::Ptr  _blockDBInfoPtr;        //!< Pointer to BlockDataBaseInfo object - may be NULL
    Block::Action::Value           _action;                //!< Compute block action
};

} // namespace bgsched

#endif
