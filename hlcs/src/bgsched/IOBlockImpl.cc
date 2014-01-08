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

#include <bgsched/DatabaseException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/RuntimeException.h>

#include "bgsched/IOBlockImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/Exception.h>
#include <db/include/api/genblock.h>
#include <db/include/api/GenBlockParams.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/assign.hpp>

#include <algorithm>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace BGQDB;
using namespace boost::assign;
using namespace std;

namespace {
    // Message strings
    const string No_Block_Name_Str("I/O block name is empty.");
    const string Block_Name_Too_Long_Str("I/O block name is too long.");
    const string No_Block_Description_Str("I/O block description is empty.");
    const string Block_Description_Too_Long_Str("I/O block description is too long.");
    const string No_Block_BootOptions_Str("I/O block boot options is empty.");
    const string Block_BootOptions_Too_Long_Str("I/O block boot options are too long.");
    const string No_Block_MicroLoaderImage_Str("I/O block micro-loader image file path is empty.");
    const string Block_MicroLoaderImage_Too_Long_Str("I/O block micro-loader image file path is too long.");
    const string No_Block_NodeConfiguration_Str("I/O block Node configuration is empty.");
    const string Block_NodeConfiguration_Too_Long_Str("I/O block Node configuration is too long.");
    const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
    const string Internal_Invalid_Block_Status_Str("Unexpected I/O block status found.");
    const string DB_IOBlockMapQueryFailed_Str("Database query of bgqIoBlockMap failed.");
} // anonymous namespace

namespace bgsched {

/*
 * Does a database query to get the I/O drawer or node locations for the I/O block.
 *
 * Throws the following exception:
 * bgsched::DatabaseException with value:
 * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
 */

IOBlock::IOLocations
queryIOLocations(
        const string& block_name,
        cxxdb::Connection& conn
)
{
    IOBlock::IOLocations locations;

    // Querying over bgqIoBlockMap table
    BGQDB::DBTIoblockmap IOBlockMap;

    // Indicate columns to return
    BGQDB::ColumnsBitmap cols;
    cols.set(IOBlockMap.LOCATION);
    IOBlockMap.setColumns(cols);

    // Build-up query to execute
    static const string where_clause("WHERE blockId=?");

    try {
        cxxdb::QueryStatementPtr stmt_ptr(IOBlockMap.prepareSelect(conn, where_clause, list_of("blockId")));

        stmt_ptr->parameters()["blockId"].set(block_name);

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while (rs_ptr->fetch()) {
            // Set the I/O location
            string location(rs_ptr->columns()[IOBlockMap.LOCATION_COL].getString());
            locations.push_back(location);
        }
        return locations;
    } catch(const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                DB_IOBlockMapQueryFailed_Str
        );
    }
}

IOBlock::Pimpl
IOBlock::Impl::createFromDatabase(
        const cxxdb::Columns& IOBlock_cols,
        const bool isIOBlockExtendedInfo,
        cxxdb::Connection& conn
)
{
    IOBlock::Pimpl ret(new IOBlock::Impl(IOBlock_cols, isIOBlockExtendedInfo, conn));
    return ret;
}

IOBlock::Status
IOBlock::Impl::statusDbCharToValue(
        char db_char
)
{
    switch (db_char) {
        case 'A':
            return IOBlock::Allocated;
        case 'B':
            return IOBlock::Booting;
        case 'F':
            return IOBlock::Free;
        case 'I':
            return IOBlock::Initialized;
        case 'T':
            return IOBlock::Terminating;
        default:
            // If get to this point the I/O block status is unknown
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::InconsistentDataError,
                    Internal_Invalid_Block_Status_Str
            );
    }
}

char
IOBlock::Impl::statusToDbChar(
        IOBlock::Status status
)
{
    switch (status) {
        case IOBlock::Allocated:
            return 'A';
        case IOBlock::Booting:
            return 'B';
        case IOBlock::Free:
            return 'F';
        case IOBlock::Initialized:
            return 'I';
        case IOBlock::Terminating:
            return 'T';
        default:
            // If get to this point the I/O block status is unknown
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::InconsistentDataError,
                    Internal_Invalid_Block_Status_Str
            );
    }
}

IOBlock::Status
IOBlock::Impl::convertDBStatusToIOBlockStatus(
        const char *status
)
{
    if (strcmp(status,BGQDB::BLOCK_FREE) == 0) {
        return IOBlock::Free;
    } else if (strcmp(status,BGQDB::BLOCK_INITIALIZED) == 0) {
        return IOBlock::Initialized;
    } else if (strcmp(status,BGQDB::BLOCK_BOOTING) == 0) {
        return IOBlock::Booting;
    } else if (strcmp(status,BGQDB::BLOCK_TERMINATING) == 0) {
        return IOBlock::Terminating;
    } else if (strcmp(status,BGQDB::BLOCK_ALLOCATED) == 0) {
        return IOBlock::Allocated;
    }
    // If get to this point the I/O block status is unknown
    THROW_EXCEPTION(
            bgsched::InternalException,
            bgsched::InternalErrors::InconsistentDataError,
            Internal_Invalid_Block_Status_Str
    );
}

IOBlock::Status
IOBlock::Impl::convertDBBlockStateToIOBlockStatus(
        const int state
)
{
    switch(state)
    {
        case BGQDB::FREE:
            return IOBlock::Free;
        case BGQDB::ALLOCATED:
            return IOBlock::Allocated;
        case BGQDB::INITIALIZED:
            return IOBlock::Initialized;
        case BGQDB::BOOTING:
            return IOBlock::Booting;
        case BGQDB::TERMINATING:
            return IOBlock::Terminating;
        case BGQDB::INVALID_STATE:
            break;
    }
    // If get to this point the I/O block status is unknown
    THROW_EXCEPTION(
            bgsched::InternalException,
            bgsched::InternalErrors::InconsistentDataError,
            Internal_Invalid_Block_Status_Str
    );
}

IOBlock::Impl::Impl(
        const string& name,
        const IOBlock::Pimpl fromIOBlock
) :
    _name(name),
    _numionodes(fromIOBlock->_numionodes),
    _owner(fromIOBlock->_owner),
    _user(),              // Do not copy user, this is not the same I/O block
    _description(fromIOBlock->_description),
    _status(IOBlock::Free), // Don't copy I/O block status
    _mImage(fromIOBlock->_mImage),
    _nodeConfig(fromIOBlock->_nodeConfig),
    _bootOptions(fromIOBlock->_bootOptions),
    _sequenceId(0),       // Do not copy sequence id, this is not the same I/O block
    _creationId(0),       // Do not copy creation id, this is not the same I/O block
    _action(IOBlock::Action::None),  // Don't copy block action
    _IOLocations()
{
    // Copy I/O locations vector
    _IOLocations = fromIOBlock->_IOLocations;
}

IOBlock::Impl::Impl(
        const cxxdb::Columns& IOBlock_cols,
        bool isIOBlockExtendedInfo,
        cxxdb::Connection& conn
) :
    _name(IOBlock_cols[BGQDB::DBTBlock::BLOCKID_COL].getString()),
    _numionodes(IOBlock_cols[BGQDB::DBTBlock::NUMIONODES_COL].as<uint32_t>()),
    _owner(IOBlock_cols[BGQDB::DBTBlock::OWNER_COL].isNull() ? "" : IOBlock_cols[BGQDB::DBTBlock::OWNER_COL].getString()),
    _user(IOBlock_cols[BGQDB::DBTBlock::USERNAME_COL].isNull() ? "" : IOBlock_cols[BGQDB::DBTBlock::USERNAME_COL].getString()),
    _description(IOBlock_cols[BGQDB::DBTBlock::DESCRIPTION_COL].isNull() ? "" : IOBlock_cols[BGQDB::DBTBlock::DESCRIPTION_COL].getString()),
    _status(convertDBStatusToIOBlockStatus(IOBlock_cols[BGQDB::DBTBlock::STATUS_COL].getString().c_str())),
    _mImage(IOBlock_cols[BGQDB::DBTBlock::MLOADERIMG_COL].isNull() ? "" : IOBlock_cols[BGQDB::DBTBlock::MLOADERIMG_COL].getString()),
    _nodeConfig(IOBlock_cols[BGQDB::DBTBlock::NODECONFIG_COL].getString()),
    _bootOptions(IOBlock_cols[BGQDB::DBTBlock::BOOTOPTIONS_COL].isNull() ? "" : IOBlock_cols[BGQDB::DBTBlock::BOOTOPTIONS_COL].getString()),
    _sequenceId(IOBlock_cols[BGQDB::DBTBlock::SEQID_COL].as<SequenceId>()),
    _creationId(IOBlock_cols[BGQDB::DBTBlock::CREATIONID_COL].as<BlockCreationId>()),
    _action(convertDBActionToIOBlockAction(IOBlock_cols[BGQDB::DBTBlock::ACTION_COL].getString().c_str())),
    _IOLocations()
{
    if (isIOBlockExtendedInfo) {
        // Get the I/O locations for the I/O block.
        _IOLocations = queryIOLocations(_name, conn);
        if (_IOLocations.empty()) {
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::InconsistentDataError,
                    "No I/O locations found in database for I/O block " << _name
            );
        }
    }
}

void
IOBlock::Impl::toString(
        ostream& os,
        bool verbose
) const
{
    // Common I/O block info
    os << endl << "I/O block=" << _name << endl;    // I/O block name
    os << "I/O nodes=" << _numionodes << endl;      // I/O node count

    // I/O block status
    switch (_status) {
      case IOBlock::Allocated:
          os << "Status=Allocated" << endl;
        break;
      case IOBlock::Booting:
          os << "Status=Booting" << endl;
        break;
      case IOBlock::Free:
          os << "Status=Free" << endl;
        break;
      case IOBlock::Initialized:
          os << "Status=Initialized" << endl;
        break;
      case IOBlock::Terminating:
          os << "Status=Terminating" << endl;
        break;
      default:
          os << "Status=Unknown" << endl;
        break;
    }

    // I/O block action
    switch (_action) {
      case IOBlock::Action::None:
          os << "Action=None" << endl;
        break;
      case IOBlock::Action::Boot:
          os << "Action=Boot" << endl;
        break;
      case IOBlock::Action::Free:
          os << "Action=Free" << endl;
        break;
      default:
          os << "Action=Unknown" << endl;
        break;
    }

    // Verbose block details
    if (verbose) {
        os << "Description=" << _description << endl;        // I/O block description
        os << "Owner=" << _owner << endl;                    // I/O block owner
        os << "Booted by=" << _user << endl;                 // I/O block user
        os << "Boot options=" << _bootOptions << endl;       // I/O block boot options
        os << "Microloader image=" << _mImage << endl;       // I/O block microloader image
        os << "Node configuration=" << _nodeConfig << endl;  // I/O block node configuration
        os << "Sequence ID=" << _sequenceId << endl;         // Sequence ID
        // I/O locations
        if (!_IOLocations.empty()) {
            os << "I/O locations:" << endl;
            for (IOLocations::const_iterator i = _IOLocations.begin(); i != _IOLocations.end(); ++i) {
                os << (*i) << endl;
            }
        }
    }
}

void
IOBlock::Impl::setName(
        const string& IOBlockName
)
{
    BGQDB::DBTBlock dbo;
    // Verify I/O block name was specified
    if (IOBlockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the I/O block name size
    if (IOBlockName.size() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }
    _name = IOBlockName;
}

void
IOBlock::Impl::setDescription(
        const string& description
)
{
    BGQDB::DBTBlock dbo;
    // Verify I/O block description was specified
    if (description.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockDescription,
                No_Block_Description_Str
                );
    }
    // Validate the I/O block description string size
    if (description.size() >= sizeof(dbo._description)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockDescription,
                Block_Description_Too_Long_Str
                );
    }
    _description = description;
}

void
IOBlock::Impl::setBootOptions(
        const string& bootOptions
)
{
    BGQDB::DBTBlock dbo;
    // Verify I/O block boot options were specified
    if (bootOptions.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockBootOptions,
                No_Block_BootOptions_Str
                );
    }
    // Validate the I/O block boot options string size
    if (bootOptions.size() >= sizeof(dbo._bootoptions)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockBootOptions,
                Block_BootOptions_Too_Long_Str
                );
    }
    _bootOptions = bootOptions;
}

void
IOBlock::Impl::setMicroLoaderImage(
        const string& image
)
{
    BGQDB::DBTBlock dbo;
    // Verify I/O block micro-loader image was specified
    if (image.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockMicroLoaderImage,
                No_Block_MicroLoaderImage_Str
                );
    }
    // Validate the I/O block micro-loader image string size
    if (image.size() >= sizeof(dbo._mloaderimg)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockMicroLoaderImage,
                Block_MicroLoaderImage_Too_Long_Str
                );
    }
    _mImage = image;
}

void
IOBlock::Impl::setNodeConfiguration(
        const string& nodeConfig
)
{
    BGQDB::DBTBlock dbo;
    // Verify I/O block Node configuration was specified
    if (nodeConfig.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockNodeConfiguration,
                No_Block_NodeConfiguration_Str
                );
    }
    // Validate the I/O block Node configuration string size
    if (nodeConfig.size() >= sizeof(dbo._nodeconfig)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockNodeConfiguration,
                Block_NodeConfiguration_Too_Long_Str
                );
    }
    _nodeConfig = nodeConfig;
}

void
IOBlock::Impl::setStatus(
        IOBlock::Status status
)
{
    _status = status;
}

const string&
IOBlock::Impl::getName() const
{
    return _name;
}

BlockCreationId
IOBlock::Impl::getCreationId() const
{
    return _creationId;
}

const string&
IOBlock::Impl::getDescription() const
{
    return _description;
}

const string&
IOBlock::Impl::getOwner() const
{
    return _owner;
}

const string&
IOBlock::Impl::getUser() const
{
    return _user;
}

const string&
IOBlock::Impl::getBootOptions() const
{
    return _bootOptions;
}

const string&
IOBlock::Impl::getMicroLoaderImage() const
{
    return _mImage;
}

const string&
IOBlock::Impl::getNodeConfiguration() const
{
    return _nodeConfig;
}

SequenceId
IOBlock::Impl::getSequenceId() const
{
    return _sequenceId;
}

EnumWrapper<IOBlock::Status>
IOBlock::Impl::getStatus() const
{
    return _status;
}

uint32_t
IOBlock::Impl::getIONodeCount() const
{
    return _numionodes;
}

const IOBlock::IOLocations
IOBlock::Impl::getIOLocations() const
{
    return _IOLocations;
}

void
IOBlock::Impl::setAction(
        IOBlock::Action::Value action
)
{
    _action = action;
}

EnumWrapper<IOBlock::Action::Value>
IOBlock::Impl::getAction() const
{
    return _action;
}

IOBlock::Action::Value
IOBlock::Impl::convertDBActionToIOBlockAction(
        const char *action
)
{
    if (strcmp(action,BGQDB::BLOCK_BOOTING) == 0) {
        return IOBlock::Action::Boot;
    } else if (strcmp(action,BGQDB::BLOCK_BOOTING_NO_CHECK) == 0) {
        return IOBlock::Action::Boot;
    } else if (strcmp(action,BGQDB::BLOCK_DEALLOCATING) == 0) {
        return IOBlock::Action::Free;
    } else  {
        return IOBlock::Action::None;
    }
}

} // namespace bgsched
