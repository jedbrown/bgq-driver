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
 * \file bgsched/IOBlockImpl.h
 * \brief IOBlock::Impl class definition.
 */

#ifndef BGSCHED_IOBLOCK_IMPL_H_
#define BGSCHED_IOBLOCK_IMPL_H_

#include <bgsched/IOBlock.h>

#include <db/include/api/BlockDatabaseInfo.h>
#include <db/include/api/cxxdb/fwd.h>

#include <iosfwd>
#include <string>
#include <vector>

class DBVIoblock;

namespace bgsched {

/*!
 * \brief Represents an I/O block.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 */
class IOBlock::Impl
{
public:

    /*!
     * \brief Convert database I/O block status to Scheduler API I/O block status.
     *
     * \return Scheduler API I/O block status converted from single character database I/O block status.
     */
    static IOBlock::Status statusDbCharToValue(
            char db_char         //!< [in] Database I/O block status
    );

    /*!
     * \brief Convert database I/O block status to Scheduler API I/O block status.
     *
     * \return Scheduler API I/O block status converted from database I/O block status.
     */
    static IOBlock::Status convertDBStatusToIOBlockStatus(
            const char *status   //!< [in] Database I/O block status
    );

    /*!
     * \brief Convert Scheduler API I/O block status to database I/O block status.
     *
     * \return Database I/O block status in single character format converted from Scheduler API I/O block status.
     */
    static char statusToDbChar(
            IOBlock::Status status  //!< [in] Scheduler API I/O block status
    );

    /*!
     * \brief Convert database I/O block status in integer format to Scheduler API I/O block status.
     *
     * \return Scheduler API I/O block status converted from integer database I/O block status.
     */
    static IOBlock::Status convertDBBlockStateToIOBlockStatus(
            const int state  //!< [in] Database I/O block status
    );

    /*!
     * \brief Convert database block action to Scheduler API block action.
     *
     * \return Scheduler API block action converted from database block action.
     */
    static IOBlock::Action::Value convertDBActionToIOBlockAction(
            const char *action   //!< [in] Database I/O block action
    );

    /*!
     * \brief Create an I/O block from the database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::InconsistentDataError - if I/O node locations not found in database
     *
     */
    static IOBlock::Pimpl createFromDatabase(
            const cxxdb::Columns& IOBlock_cols, //!< [in] Database I/O block columns
            const bool isIOBlockExtendedInfo,   //!< [in] Indicates if I/O block extended info should be retrieved
            cxxdb::Connection& conn             //!< [in] Database connection
    );

    /*!
     * \brief Copy constructor.
     */
    Impl(
            const std::string& name,            //!< [in] New I/O block name
            const IOBlock::Pimpl fromIOBlock    //!< [in] I/O block to copy from
    );

    /*!
     * \brief I/O block object represented as a human readable string value.
     *
     * \return I/O block object represented as a human readable string value.
     */
    void toString(
            std::ostream& os,  //!< [in/out] Output stream to write I/O block details
            bool verbose       //!< [in] true for extended I/O block info, false for brief I/O block info
    ) const;

    /*!
     * \brief Get the I/O block name (Id).
     *
     * This is the I/O block's unique name.
     *
     * \return I/O block name.
     */
    const std::string& getName() const;

    /*!
     * \brief Get the I/O block creation identifier.
     *
     * A unique creation identifier is automatically generated when an I/O  block is added to
     * persistent storage (database). The id is stored in the I/O block history table.
     *
     * \return I/O block creation Id.
     */
    BlockCreationId getCreationId() const;

    /*!
     * \brief Get the I/O block description.
     *
     * \return I/O block description.
     */
    const std::string& getDescription() const;

    /*!
     * \brief Get the I/O block owner.
     *
     * \return I/O block owner.
     */
    const std::string& getOwner() const;

    /*!
     * \brief Get the I/O block user. This is the user that booted the I/O block.
     *
     * \return I/O block user.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get the I/O block boot options.
     *
     * \return I/O block boot options.
     */
    const std::string& getBootOptions() const;

    /*!
     * \brief Get the I/O block microloader image.
     *
     * \return I/O block microloader image.
     */
    const std::string& getMicroLoaderImage() const;

    /*!
     * \brief Get the I/O block Node configuration.
     *
     * \return I/O block Node configuration.
     */
    const std::string& getNodeConfiguration() const;

    /*!
     * \brief Get the I/O block sequence ID.
     *
     * \return I/O block sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get the I/O block status.
     *
     * This is the I/O block status, not the status of the underlying hardware.
     *
     * \return I/O block status.
     */
    EnumWrapper<IOBlock::Status> getStatus() const;

    /*!
     * \brief Get the I/O block action.
     *
     * \return I/O block action.
     */
    EnumWrapper<IOBlock::Action::Value> getAction() const;

    /*!
     * \brief Get the number of I/O nodes in the I/O block.
     *
     * \return Number of I/O nodes in the I/O block.
     */
    uint32_t getIONodeCount() const;

    /*!
     * \brief Get the I/O locations (drawers or nodes) this I/O block uses.
     *
     * There is a situation where this will be an empty vector:
     * - The I/O block was created using core::getIOBlocks and extended information wasn't requested.
     *
     * \return I/O locations (drawers or nodes) the I/O block uses.
     */
    const IOBlock::IOLocations getIOLocations() const;

    /*!
     * \brief Set the I/O block name (Id).
     *
     * This is a user-defined name limited to 32 characters. The name must be unique.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is not valid (empty or exceeds 32 characters)
     */
    void setName(
            const std::string& name  //!< [in] I/O block name
    );

    /*!
     * \brief Set the I/O block description.
     *
     * This is a user-defined description limited to 1024 characters, e.g. "Joe's test block"
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockDescription - if the I/O block description is not valid (empty or exceeds 1024 characters)
     */
    void setDescription(
            const std::string& description  //!< [in] Description for I/O block
    );

    /*!
     * \brief Set the I/O block boot options.
     *
     * This is optional, if not set, the default boot options will be used.
     * Boot options are limited to 256 characters.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidBlockBootOptions - if the I/O block boot options are not valid (empty or exceeds 256 characters)
     */
    void setBootOptions(
            const std::string& bootOptions  //!< [in] I/O block boot options
    );

    /*!
     * \brief Set the I/O block micro-loader image.
     *
     * This is optional, if not set, the default image will be used.
     * The micro-loader image file name is limited to 256 characters.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidBlockMicroLoaderImage - if the I/O block micro-loader image file name is not valid (empty or exceeds 256 characters)
     */
    void setMicroLoaderImage(
            const std::string& image  //!< [in] Path to the I/O block micro-loader image
    );

    /*!
     * \brief Set the I/O block Node configuration.
     *
     * This is optional, if not set, the default Node configuration "CNKDefault" will be used.
     *
     * The Node configuration name is limited to 32 characters.
     *
     * Note: Customized I/O block Node configurations are created by System Administrators using the "create_node_config" command
     * from the bg_console. The Node configuration name specified must exist in the database.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockNodeConfiguration - if the I/O block Node configuration is not valid (empty or exceeds 32 characters)
     */
    void setNodeConfiguration(
            const std::string& nodeConfig  //!< [in] Node configuration name for the I/O block
    );

    /*!
     * \brief Set the I/O block status.
     *
     * No validation is done to ensure I/O block status transitions are valid.
     */
    void setStatus(
            IOBlock::Status status  //!< [in] I/O block status to set
    );

    /*!
     * \brief Set the I/O block action.
     *
     * No validation is done to ensure I/O block action transitions are valid.
     */
    void setAction(
            IOBlock::Action::Value action  //!< [in] I/O block action to set
    );

protected:

    /*!
     * \brief Construct an I/O block from the database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::InconsistentDataError - if I/O node locations not found in database
     *
     */
    Impl(
            const cxxdb::Columns& IOBlock_cols,  //!< [in] Database I/O block columns
            const bool isIOBlockExtendedInfo,    //!< [in] Indicates if I/O block extended info should be retrieved
            cxxdb::Connection& conn              //!< [in] Database connection
    );

protected:

    std::string                    _name;                  //!< I/O block name
    uint32_t                       _numionodes;            //!< I/O block I/O node count
    std::string                    _owner;                 //!< I/O block owner
    std::string                    _user;                  //!< I/O block user (booter of I/O block)
    std::string                    _description;           //!< I/O block description
    IOBlock::Status                _status;                //!< I/O block status
    std::string                    _mImage;                //!< I/O block micro-loader image
    std::string                    _nodeConfig;            //!< I/O block Node configuration
    std::string                    _bootOptions;           //!< I/O block boot options
    SequenceId                     _sequenceId;            //!< I/O block sequence Id
    BlockCreationId                _creationId;            //!< I/O block creation Id
    IOBlock::Action::Value         _action;                //!< I/O block action
    IOBlock::IOLocations           _IOLocations;           //!< I/O block I/O locations -- may be empty if the info wasn't gathered.
};

} // namespace bgsched

#endif
