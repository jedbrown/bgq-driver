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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
 * \file bgsched/IOBlock.h
 * \brief I/O block class definition.
 */

#ifndef BGSCHED_IOBLOCK_H_
#define BGSCHED_IOBLOCK_H_

#include <bgsched/EnumWrapper.h>
#include <bgsched/types.h>

#include <boost/exception_ptr.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <string>
#include <vector>

namespace bgsched {

/*!
 * \brief Represents an I/O block.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 */
class IOBlock : boost::noncopyable
{
public:

    typedef boost::shared_ptr<IOBlock> Ptr;                  //!< Pointer type.
    typedef boost::shared_ptr<const IOBlock> ConstPtr;       //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                           //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;                 //!< Collection of const pointers.

    typedef std::vector<std::string> IOLocations;            //!< Collection of I/O drawer or node location strings.
    typedef std::vector<std::string> ConnectedComputeBlocks; //!< Collection of connected compute block.

    /*!
     * \brief I/O block status.
     */
    enum Status {
        Allocated = 0,  //!< I/O block is allocated
        Booting,        //!< I/O block is booting
        Free,           //!< I/O block is free
        Initialized,    //!< I/O block is initialized
        Terminating     //!< I/O block is terminating
    };

    /*!
     * \brief I/O block action.
     */
    struct Action {
        enum Value {
            None = 0,       //!< No I/O block action
            Boot,           //!< Boot I/O block action
            Free            //!< Free I/O block action
        };
    };

    /*!
     * \brief Create an I/O block in the database using starting I/O node or I/O drawer location and number of I/O nodes
     * to include in the block. The I/O block owner will be the user who creates the I/O block.
     *
     * The owner of the I/O block will be the user id of the caller. If the "description" argument is an empty string
     * the default description with be "Created by IBM Scheduler API".
     *
     * The returned I/O block includes extended information (e.g. I/O locations for the block).
     *
     * If a bgsched::RuntimeException is thrown that indicates the I/O block was added to the database but the /O block object
     * could not be retrieved back from the database.
     *
     * Note: The number of I/O nodes must be a multiple of 8 when specifying an I/O drawer starting location. The number of
     * I/O nodes must be 1, 2, or 4 when specifying an I/O node starting location. For two I/O nodes, the I/O node location
     * must be J00, J02, J04, or J06. For four nodes, the I/O node location must be J00 or J04.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is invalid (empty, too long, invalid characters, etc.)
     * - bgsched::InputErrors::InvalidLocationString - if the starting I/O location string is invalid
     * - bgsched::InputErrors::InvalidIONodeCount - if I/O node count is invalid (valid range is 1 to 8 and must follow other rules)
     * - bgsched::InputErrors::InvalidBlockDescription - if the I/O block description is too long
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::BlockRetrievalError - in this situation I/O block was created but I/O block object was not returned
     *
     * \return Pointer to created I/O block object.
     */
    static IOBlock::Ptr create(
            const std::string& IOBlockName,     //!< [in] I/O block name
            const std::string& startIOLocation, //!< [in] Specify starting I/O location as either an I/O node (Qxx-Ix-Jxx or Rxx-Ix-Jxx) or I/O drawer (Qxx-Ix or Rxx-Ix)
            const uint32_t IONodeCount,         //!< [in] Number of I/O nodes to include from starting location
            const std::string& description      //!< [in] I/O block description to set
    );

    /*!
     * \brief Initiate the booting of an I/O block.
     *
     * The I/O block must exist in the database.
     *
     * The I/O block must have a status of Free or an exception is thrown. The boot process is done asynchronously
     * so the status of the I/O block must be checked to determine if the I/O block was successfully booted.
     *
     * The I/O block owner will be set to the user calling the method.
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::BlockBootError - if I/O block status is not Free or I/O block action already pending
     *                                            or required hardware is unavailable (in use or in error)
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if I/O block name was not found or not an I/O block
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     * \return A vector of unavailable I/O resources when "allowHoles" is false and resources are unavailable. Each vector string
     * will be like "IODRAWER: R11-ID" or "IONODE: R00-ID-J00". Also returns a vector of unavailable I/O node locations when
     * "allowHoles" is true. If all resources are available the vectors will be empty.
     */
    static void initiateBoot(
            const std::string& IOBlockName,                  //!< [in] I/O block name
            bool allowHoles,                                 //!< [in] Allow booting with some I/O drawers/nodes in error
            std::vector<std::string>* unavailableResources,  //!< [out] Unavailable I/O resources when allowHoles = False
            std::vector<std::string>* unavailableIONodes     //!< [out] Unavailable I/O node locations when allowHoles = true
    );

    /*!
     * \brief Initiate the freeing of an I/O block.
     *
     * The I/O block must exist in the database.
     *
     * The freeing of the I/O block is done asynchronously so the status of the I/O block
     * must be checked to determine if the I/O block was successfully freed.
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::BlockFreeError - if I/O block has block action already pending
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if I/O block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    static void initiateFree(
            const std::string& IOBlockName       //!< [in] I/O block name
    );

    /*!
     * \brief Remove I/O block from database.
     *
     * A I/O block can only be deleted if its status is Free.
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::InvalidBlockState - if I/O block status is not Free
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if I/O block name was not found
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    static void remove(
            const std::string& IOBlockName  //!< [in] I/O block name
    );

    /*!
     *  \brief Type for function called when rebootIONode is complete.
     */
    typedef boost::function<void ( boost::exception_ptr exc_ptr )> RebootIONodeCallbackFn;

    /*!
     *
     * \brief Start reboot of an I/O node.
     *
     * Initiates the reboot of an I/O node.
     *
     * When the reboot operation is complete, the callback function is called. The call of the
     * callback function will be in another thread. If the reboot is successful, the exc_ptr
     * argument will be NULL. Otherwise, the exc_ptr argument will be set to the exception.
     *
     * Note that the callback function can have no target, in which case no function
     * will be called when the reboot I/O node operation is complete.
     *
     * This command connects to MMCS server. In order to connect to the MMCS server,
     * the caller must have access to the Blue Gene administrative certificate key.
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     * The following errors can be set in the exc_ptr argument for the callback:
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::BlockNotFound - if I/O block name was not found
     * - bgsched::InputErrors::InvalidLocationString - if the location is not valid or not in the block
     *
     * \throws bgsched::RuntimeException with values:
     * - bgsched::RuntimeErrors::InvalidBlockState - if I/O block status is not Allocated
     * - bgsched::RuntimeErrors::MmcsConnectionError - if connection to MMCS server failed
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     *
     */
    static void rebootIONode(
            const std::string& IOBlockName, //!< [in] I/O block name
            const std::string& location,    //!< [in] I/O node location
            RebootIONodeCallbackFn cbFn     //!< [in] Callback function
        );

    /*!
     * \brief Get compute blocks connected ("Booting", "Initialized" or "Terminating" status) to I/O block.
     *
     * The I/O block must exist in the database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if I/O block name was not found or not an I/O block
     *
     * \return A vector of compute block names connected to the I/O block. Returns an empty vector if no
     * compute blocks are connected to the I/O block.
     */
    static const ConnectedComputeBlocks getConnectedComputeBlocks(
            const std::string& IOBlockName                  //!< [in] I/O block name
    );

    /*!
     * \brief Update I/O block in database.
     *
     * This method will update select I/O block fields in the database based on current settings
     * in the Block object. The following fields are updated: description, boot options, micro-loader image,
     * and Node configuration.
     *
     * Note: The updated node configuration name must exist in the BG/Q database or a bgsched::DatabaseException
     * is thrown.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs updating the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is not valid
     * - bgsched::InputErrors::BlockNotFound - if I/O block name was not found or I/O block is not Free
     * - bgsched::InputErrors::InvalidBlockInfo - if boot options, micro-loader image or Node configuration is not valid
     * - bgsched::InputErrors::InvalidBlockDescription - if description is not valid
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    void update();

    /*!
     * \brief I/O block object represented as a human readable string value.
     *
     * \return I/O block object represented as a human readable string value.
     */
    std::string toString(
            bool verbose    //!< [in] true for extended I/O block info, false for brief I/O block info
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
     * A unique creation identifier is automatically generated when an I/O block is added to
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
     * Note: Customized I/O block Node configurations are created by System Administrators using the "create_node_config"
     * command from the bg_console. The default Node configuration is named "IODefault".
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
     * This is the I/O block status, not necessarily the status of the underlying hardware.
     *
     * \return I/O block status.
     */
    EnumWrapper<Status> getStatus() const;

    /*!
     * \brief Get the I/O block action.
     *
     * \return I/O block action.
     */
    EnumWrapper<Action::Value> getAction() const;

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
    const IOLocations getIOLocations() const;

    /*!
     * \brief Set the I/O block name (Id).
     *
     * This is a user-defined name limited to 32 characters. The name must be unique.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockName - if the I/O block name is not valid (empty or exceeds 32 characters)
     */
    void setName(
            const std::string& name       //!< [in] I/O block name
    );

    /*!
     * \brief Set the I/O block description.
     *
     * This is a user-defined description limited to 1024 characters, e.g. "Joe's test I/O block"
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockDescription - if the I/O block description is not valid (empty or exceeds 1024 characters)
     */
    void setDescription(
            const std::string& description      //!< [in] Description for I/O block
    );

    /*!
     * \brief Set the I/O block boot options.
     *
     * This is optional, if not set, the default boot options will be used.
     * Boot options are limited to 256 characters.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockBootOptions - if the I/O block boot options are not valid (empty or exceeds 256 characters)
     */
    void setBootOptions(
            const std::string& bootOptions      //!< [in] I/O block boot options
    );

    /*!
     * \brief Set the I/O block micro-loader image.
     *
     * This is optional, if not set, the default image will be used.
     * The micro-loader image file name is limited to 256 characters.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidBlockMicroLoaderImage - if the I/O block micro-loader image file name is not valid (empty or exceeds 256 characters)
     */
    void setMicroLoaderImage(
            const std::string& image            //!< [in] Path to the I/O block micro-loader image
    );

    /*!
     * \brief Set the I/O block Node configuration.
     *
     * This is optional, if not set, the default Node configuration "IODefault" will be used.
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
            const std::string& nodeConfig       //!< [in] Node configuration name for the I/O block
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
     * \brief Get pointer to implementation.
     *
     * \return Pointer to implementation.
     */
    Pimpl getPimpl() const;

    /*!
     * \brief
     */
    explicit IOBlock(
            Pimpl impl      //!< [in] Pointer to implementation
    );

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif

