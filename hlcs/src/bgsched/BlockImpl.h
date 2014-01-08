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
 * \brief Represents a block of any size.
 *
 */
class Block::Impl
{
public:

    /*!
     * \brief Convert database block status to Scheduler API block status.
     *
     * \return Scheduler API block status converted from single character database block status.
     */
    static Block::Status statusDbCharToValue(
            char db_char         //!< [in] Database block status
    );

    /*!
     * \brief Convert database block status to Scheduler API block status.
     *
     * \return Scheduler API block status converted from database block status.
     */
    static Block::Status convertDBStatusToBlockStatus(
            const char *status   //!< [in] Database block status
    );

    /*!
     * \brief Convert Scheduler API block status to database block status.
     *
     * \return Database block status in single character format converted from Scheduler API block status.
     */
    static char statusToDbChar(
            Block::Status status  //!< [in] Scheduler API block status
    );

    /*!
     * \brief Convert database block status in integer format to Scheduler API block status.
     *
     * \return Scheduler API block status converted from integer database block status.
     */
    static Block::Status convertDBBlockStateToBlockStatus(
            const int state  //!< [in] Database block status
    );

    /*!
     * \brief Convert database block action to Scheduler API block action.
     *
     * \return Scheduler API block action converted from database block action.
     */
    static Block::Action::Value convertDBActionToBlockAction(
            const char *action   //!< [in] Database block action
    );


    /*!
     * \brief Create a block from the database.
     */
    static Block::Pimpl createFromDatabase(
            const cxxdb::Columns& block_cols,  //!< [in] Database block columns
            const bool isBlockExtendedInfo,    //!< [in] Indicates if block extended info should be retrieved
            cxxdb::Connection& conn            //!< [in] Database connection
    );

    /*!
     * \brief Copy constructor.
     */
    Impl(
            const std::string& name,        //!< [in] New block name
            const Block::Pimpl fromBlock    //!< [in] Block to copy from
    );

    /*!
     * \brief Block object represented as a human readable string value.
     *
     * \return Compute block object represented as a human readable string value.
     */
    void toString(
            std::ostream& os,  //!< [in/out] Output stream to write block details
            bool verbose       //!< [in] true for extended block info, false for brief block info
    ) const;

    /*!
     * \brief Get the block name (Id).
     *
     * This is the block's unique name.
     *
     * \return Compute block name.
     */
    const std::string& getName() const;

    /*!
     * \brief Get the block creation identifier.
     *
     * A unique creation identifier is automatically generated when a block is added to
     * persistent storage (database). The id is stored in the block history table and also in the
     * job history table for any job run on the block. The creation identifier associates a job with
     * the exact block it was run on.
     *
     * \return Compute block creation Id.
     */
    BlockCreationId getCreationId() const;

    /*!
     * \brief Get the block description.
     *
     * \return Compute block description.
     */
    const std::string& getDescription() const;

    /*!
     * \brief Get the block owner.
     *
     * \return Compute block owner.
     */
    const std::string& getOwner() const;

    /*!
     * \brief Get the block user.
     *
     * \return Compute block user.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get the block boot options.
     *
     * \return Compute block boot options.
     */
    const std::string& getBootOptions() const;

    /*!
     * \brief Get the block microloader image.
     *
     * \return Compute block microloader image.
     */
    const std::string& getMicroLoaderImage() const;

    /*!
     * \brief Get the block Node configuration.
     *
     * \return Compute block Node configuration.
     */
    const std::string& getNodeConfiguration() const;

    /*!
     * \brief Get the block sequence ID.
     *
     * \return Compute block sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get the block status.
     *
     * This is the block status, not necessarily the status of the underlying hardware.
     *
     * \return Compute block status.
     */
    EnumWrapper<Block::Status> getStatus() const;

    /*!
     * \brief Get the block action.
     *
     * \return Compute block action.
     */
    EnumWrapper<Block::Action::Value> getAction() const;

    /*!
     * \brief Get the number of compute nodes in the block.
     *
     * \return Number of compute nodes in the compute block.
     */
    uint32_t getComputeNodeCount() const;

    /*!
     * \brief Get the block options.
     *
     * \return Compute block options.
     */
    const std::string& getOptions() const;

    /*!
     * \brief Get the job IDs associated with the block.
     *
     * \return Job IDs associated with the compute block.
     */
    const std::vector<Job::Id> getJobIds() const;

    /*!
     * \brief Get the block dimension size in midplanes. This only returns dimensions for large blocks.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small block
     *
     * \return Size in midplanes for the given dimension.
     */
    uint32_t getDimensionSize(
            const Dimension& dimension   //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Get the block dimension size in midplanes. This only returns dimensions for large blocks.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small block
     *
     * \return Size in midplanes for the given dimension.
     */
    uint32_t getDimensionSize(
            const bgq::util::Location::Dimension::Value dimension   //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Returns true if block is Torus in specified dimension.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small block
     *
     * \return True if compute block is Torus in specified dimension.
     */
    bool isTorus(
            const Dimension& dimension     //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Return true if block is Mesh in specified dimension.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::InputErrors::InvalidBlockSize - if small block
     *
     * \return True if compute block is Mesh in specified dimension.
     */
    bool isMesh(
            const Dimension& dimension     //!< [in] Dimension, A-D
    ) const;

    /*!
     * \brief Returns true if block is large (midplane or larger).
     *
     * \return True if compute block is large (midplane or larger).
     */
    bool isLarge() const;

    /*!
     * \brief Returns true if block is small (smaller than a midplane).
     *
     * \return True if compute block is small (smaller than a midplane).
     */
    bool isSmall() const;

    /*!
     * \brief Get the midplane locations this block uses.
     *
     * The returned midplanes don't include the pass-through midplanes for a large block. Use getPassthroughMidplanes()
     * to obtain any pass-through midplanes.
     *
     * The returned vector may be empty if this block was queried from core::getBlocks and extended information wasn't requested.
     *
     * \return Midplane locations the compute block uses.
     */
    Block::Midplanes getMidplanes() const;

    /*!
     * \brief Get the node board locations this block uses.
     *
     * There are two cases where this will be an empty vector:
     * - The block is a midplane or larger.
     * - The block was created using core::getBlocks and extended information wasn't requested.
     *
     * \return Node board locations the compute block uses.
     */
    Block::NodeBoards getNodeBoards() const;

    /*!
     * \brief Get the midplane locations that this block uses for pass-through.
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
     * - bgsched::InputErrors::InvalidBlockName - if the block name is not valid (empty or exceeds 32 characters)
     */
    void setName(
            const std::string& name  //!< [in] Compute block name
    );

    /*!
     * \brief Set the block description.
     *
     * This is a user-defined description limited to 1024 characters, e.g. "Joe's test block"
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockDescription - if the block description is not valid (empty or exceeds 1024 characters)
     */
    void setDescription(
            const std::string& description  //!< [in] Description for compute block
    );

    /*!
     * \brief Set the block options.
     *
     * This is optional, if not set, the default options will be used. Options are limited to 16 characters.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockOptions - if the block options are not valid (empty or exceeds 16 characters)
     */
    void setOptions(
            const std::string& options   //!< [in] Compute block options
    );

    /*!
     * \brief Set the block boot options.
     *
     * This is optional, if not set, the default boot options will be used.
     * Boot options are limited to 256 characters.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidBlockBootOptions - if the block boot options are not valid (empty or exceeds 256 characters)
     */
    void setBootOptions(
            const std::string& bootOptions  //!< [in] Compute block boot options
    );

    /*!
     * \brief Set the block micro-loader image.
     *
     * This is optional, if not set, the default image will be used.
     * The micro-loader image file name is limited to 256 characters.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidBlockMicroLoaderImage - if the block micro-loader image file name is not valid (empty or exceeds 256 characters)
     */
    void setMicroLoaderImage(
            const std::string& image  //!< [in] Path to the compute block micro-loader image
    );

    /*!
     * \brief Set the block Node configuration.
     *
     * This is optional, if not set, the default Node configuration "CNKDefault" will be used.
     *
     * The Node configuration name is limited to 32 characters.
     *
     * Note: Customized compute block Node configurations are created by System Administrators using the "create_node_config" command
     * from the bg_console. The Node configuration name specified must exist in the database.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockNodeConfiguration - if the block Node configuration is not valid (empty or exceeds 32 characters)
     */
    void setNodeConfiguration(
            const std::string& nodeConfig  //!< [in] Node configuration name for the compute block
    );

    /*!
     * \brief Set the block status.
     *
     * No validation is done to ensure block status transitions are valid.
     */
    void setStatus(
            Block::Status status  //!< [in] Block status to set
    );

    /*!
     * \brief Set the block action.
     *
     * No validation is done to ensure block action transitions are valid.
     */
    void setAction(
            Block::Action::Value action  //!< [in] Block action to set
    );

    /*!
     * \brief Add job id using the block.
     */
    void addJobId(
            Job::Id jobId  //!< [in] Job ID
    );

    /*!
     * \brief Add block to database.
     *
     * This method will add the block to the database. If the owner string is empty the user id
     * of the caller will be used as the block owner.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::BlockNotAdded - if the block was not added to the database
     * - bgsched::InputErrors::InvalidMidplanes - if large block and no midplanes found
     *
     * \throws bgsched::RuntimeException with values:
     * - bgsched::RuntimeErrors::BlockAddError - if error setting block owner
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed
     *
     * \return BlockDataBaseInfo when validating block.
     */
    BGQDB::BlockDatabaseInfo::Ptr add(
            const std::string& owner,  //!< [in] Compute block owner
            bool isValidateBlockOnly   //!< [in] Indicator on whether to add block or only validate block
    );

    /*!
     * \brief Add a midplane location to this block.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidMidplanes - if midplane location is empty
     */
    void addMidplane(
            const std::string& midplaneLocation    //!< [in] Midplane location string
    );

    /*!
     * \brief Add a node board location to this block
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidNodeBoards - if node board location is empty
     */
    void addNodeBoard(
            const std::string& nodeBoardLocation   //!< [in] Node board location string
    );

    /*!
     * \brief Add a pass-through midplane location to this block.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidPassthroughMidplanes - if pass-through midplane location is empty
     */
    void addPassthroughMidplane(
            const std::string& passthroughMidplane //!< [in] Pass-through midplane location string
    );

    /*!
     * \brief Construct a block from a block database info object pointer.
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    Impl(
            BGQDB::BlockDatabaseInfo::Ptr blockDBInfoPtr  //!< [in] Block database info object pointer
    );

protected:

    /*!
     * \brief Construct a block from the database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    Impl(
            const cxxdb::Columns& block_cols,  //!< [in] Database block columns
            const bool isBlockExtendedInfo,    //!< [in] Indicates if block extended info should be retrieved
            cxxdb::Connection& conn            //!< [in] Database connection
    );

    /*!
     * \brief Fast path to add block to database.
     *
     * This method will add the block to the database. If the owner string is empty the user id
     * of the caller will be used as the block owner.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::BlockNotAdded - if the block was not added to the database
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::BlockAddError - if error setting block owner
     *
     * \throws bgsched::DatabaseException with value:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed
     *
     * \return BlockDataBaseInfo when validating block.
     */
    BGQDB::BlockDatabaseInfo::Ptr addFastPath(
            const std::string& owner  //!< [in] Compute block owner
    );

protected:

    std::string                    _name;                  //!< Block name
    BlockCreationId                _creationId;            //!< Block creation Id
    std::string                    _description;           //!< Block description
    std::string                    _owner;                 //!< Block owner
    std::string                    _user;                  //!< Block user (booter of block)
    std::string                    _bootOptions;           //!< Block boot options
    std::string                    _mImage;                //!< Block micro-loader image
    std::string                    _nodeConfig;            //!< Block Node configuration
    SequenceId                     _sequenceId;            //!< Block sequence Id
    Block::Status                  _status;                //!< Block status
    uint32_t                       _numcnodes;             //!< Block compute node count
    Coordinates                    _dimensionSizes;        //!< Block dimension size in midplanes
    boost::array<Block::Connectivity::Value, Dimension::MidplaneDims> _connectivity; //!< Block connectivity by dimension
    std::string                    _options;               //!< Block options
    std::vector<Job::Id>           _jobIds;                //!< Block job Ids
    Block::Midplanes               _midplanes;             //!< Block midplane locations -- may be empty if the info wasn't gathered.
    Block::PassthroughMidplanes    _passthroughMidplanes;  //!< Block midplane locations used for pass-through -- may be empty if the info wasn't gathered.
    Block::NodeBoards              _nodeBoards;            //!< Block node board locations, will we empty if the hardware info wasn't gathered or the block is Large.
    BGQDB::BlockDatabaseInfo::Ptr  _blockDBInfoPtr;        //!< Pointer to BlockDataBaseInfo object - may be NULL
    Block::Action::Value           _action;                //!< Block action
};

} // namespace bgsched

#endif
