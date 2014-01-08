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

#include <bgsched/DatabaseException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/RuntimeException.h>

#include "bgsched/BlockImpl.h"
#include "bgsched/ShapeImpl.h"
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
    const string No_Block_Name_Str("Compute block name is empty.");
    const string Block_Name_Too_Long_Str("Compute block name is too long.");
    const string No_Block_Description_Str("Compute block description is empty.");
    const string Block_Description_Too_Long_Str("Compute block description is too long.");
    const string No_Block_Options_Str("Compute block options is empty.");
    const string Block_Options_Too_Long_Str("Compute block options are too long.");
    const string No_Block_BootOptions_Str("Compute block boot options is empty.");
    const string Block_BootOptions_Too_Long_Str("Compute block boot options are too long.");
    const string No_Block_MicroLoaderImage_Str("Compute block micro-loader image file path is empty.");
    const string Block_MicroLoaderImage_Too_Long_Str("Compute block micro-loader image file path is too long.");
    const string No_Block_NodeConfiguration_Str("Compute block Node configuration is empty.");
    const string Block_NodeConfiguration_Too_Long_Str("Compute block Node configuration is too long.");
    const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
    const string DB_MidplaneBlockMapQueryFailed_Str("Database query of bpblockmap failed.");
    const string DB_SmallBlockQueryFailed_Str("Database query of small block failed.");
    const string Internal_No_Midplane_Found_Str("No midplane found to create small block.");
    const string Internal_No_Nodeboards_Found_Str("No node boards found to create small block.");
    const string Internal_Invalid_Block_Status_Str("Unexpected compute block status found.");
    const string Block_No_Midplanes_Found_Str("No midplanes found for large compute block.");
    const string No_Midplane_Location_Str("Midplane location string is empty.");
    const string No_Nodeboard_Location_Str("Node board location string is empty.");
    const string No_Passthrough_Midplane_Location_Str("Pass-through midplane location string is empty.");

    static const unsigned MidplaneLength = 6;
    static const unsigned SwitchLength = 8;
} // anonymous namespace

namespace bgsched {

/* Does a database query to get the midplanes and pass-through midplanes for the large compute block.
 * Looks up the midplane locations for the compute block in dbBlock that are in the bpblockmap table.
 *
 * If can't find the compute block in the bpblockmap table then Block::Midplanes will be empty.
 *
 * Throws following exception:
 * bgsched::DatabaseException with value:
 * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
 */

void
queryLargeBlockInfo(
        const string& block_name,
        cxxdb::Connection& conn,
        Block::Midplanes& midplanes,
        Block::PassthroughMidplanes& ptMidplanes
    )
{
    static const string SQL(

 "WITH sbm AS ("

" SELECT DISTINCT SUBSTR(switchId,3,6) AS mp"
   " FROM bgqSwitchBlockMap"
   " WHERE blockId=?"

" ),"
" bbm AS ("

" SELECT bpid, acoord, bcoord, ccoord, dcoord"
  " FROM bgqBpBlockMap"
  " WHERE blockId=?"

" )"
" SELECT sbm.mp, bbm.acoord, bbm.bcoord, bbm.ccoord, bbm.dcoord"
  " FROM sbm LEFT OUTER JOIN bbm ON sbm.mp = bbm.bpid"

        );

    try {
        cxxdb::QueryStatementPtr stmt_ptr(conn.prepareQuery(SQL, list_of("blockId")("blockId")));
        stmt_ptr->parameters()["blockId"].set(block_name);

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while (rs_ptr->fetch()) {
            string mp(rs_ptr->columns()["mp"].getString());
            // Check if midplane or pass-through midplane
            if (rs_ptr->columns()["acoord"]) {
                const cxxdb::Columns &cols(rs_ptr->columns());
                // Add midplane to the collection - the corner midplane (0,0,0,0) needs to go to the front
                if (cols["acoord"].as<int64_t>() == 0 && cols["bcoord"].as<int64_t>() == 0 && cols["ccoord"].as<int64_t>() == 0 && cols["dcoord"].as<int64_t>() == 0  ) {
                    midplanes.insert(midplanes.begin(), mp);
                } else {
                    midplanes.push_back(mp);
                }
            } else {
                // Add pass-through midplane
                ptMidplanes.push_back(mp);

            }
        }
    } catch (const std::exception& e) {
        LOG_DEBUG_MSG( "Exception in " << __FUNCTION__ << " is " << e.what() );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                DB_MidplaneBlockMapQueryFailed_Str
        );
    }
}

/*
 * Does a database query to get the node boards for the small compute block.
 *
 * Throws following exception:
 * bgsched::DatabaseException with value:
 * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
 */

Block::NodeBoards
querySmallBlockNodeBoards(
        const string& block_name,
        cxxdb::Connection& conn
)
{
    Block::NodeBoards nodeBoards;

    // Querying over small compute block table
    BGQDB::DBTSmallblock sb;

    // Indicate columns to return
    BGQDB::ColumnsBitmap cols;
    cols.set(sb.POSINMACHINE);
    cols.set(sb.NODECARDPOS);
    sb.setColumns( cols );

    // Build-up query to execute
    static const string where_clause("WHERE blockId=?");

    try {
        cxxdb::QueryStatementPtr stmt_ptr(sb.prepareSelect(conn, where_clause, list_of("blockId")));

        stmt_ptr->parameters()["blockId"].set(block_name);

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while (rs_ptr->fetch()) {
            // Set the midplane name part of string Rxx-Mx
            string nodeBoardLocation(rs_ptr->columns()[sb.POSINMACHINE_COL].getString());
            // Convert node board position to integer
            uint32_t node_board_pos(boost::lexical_cast<uint32_t> (rs_ptr->columns()[sb.NODECARDPOS_COL].getString().substr(1)));
            switch (node_board_pos) {
                case 0:
                    nodeBoardLocation.append("-N00");
                    break;
                case 1:
                    nodeBoardLocation.append("-N01");
                    break;
                case 2:
                    nodeBoardLocation.append("-N02");
                    break;
                case 3:
                    nodeBoardLocation.append("-N03");
                    break;
                case 4:
                    nodeBoardLocation.append("-N04");
                    break;
                case 5:
                    nodeBoardLocation.append("-N05");
                    break;
                case 6:
                    nodeBoardLocation.append("-N06");
                    break;
                case 7:
                    nodeBoardLocation.append("-N07");
                    break;
                case 8:
                    nodeBoardLocation.append("-N08");
                    break;
                case 9:
                    nodeBoardLocation.append("-N09");
                    break;
                case 10:
                    nodeBoardLocation.append("-N10");
                    break;
                case 11:
                    nodeBoardLocation.append("-N11");
                    break;
                case 12:
                    nodeBoardLocation.append("-N12");
                    break;
                case 13:
                    nodeBoardLocation.append("-N13");
                    break;
                case 14:
                    nodeBoardLocation.append("-N14");
                    break;
                case 15:
                    nodeBoardLocation.append("-N15");
                    break;
            }
            nodeBoards.push_back(nodeBoardLocation);
        }
        return nodeBoards;
    } catch(const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                DB_SmallBlockQueryFailed_Str
        );
    }
}

Block::Pimpl
Block::Impl::createFromDatabase(
        const cxxdb::Columns& block_cols,
        const bool isBlockExtendedInfo,
        cxxdb::Connection& conn
)
{
    Block::Pimpl ret(new Block::Impl(block_cols, isBlockExtendedInfo, conn));
    return ret;
}

Block::Status
Block::Impl::statusDbCharToValue(
        char db_char
)
{
    switch (db_char) {
        case 'A':
            return Block::Allocated;
        case 'B':
            return Block::Booting;
        case 'F':
            return Block::Free;
        case 'I':
            return Block::Initialized;
        case 'T':
            return Block::Terminating;
        default:
            // If get to this point the compute block status is unknown
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::InconsistentDataError,
                    Internal_Invalid_Block_Status_Str
            );
    }
}

char
Block::Impl::statusToDbChar(
        Block::Status status
)
{
    switch (status) {
        case Block::Allocated:
            return 'A';
        case Block::Booting:
            return 'B';
        case Block::Free:
            return 'F';
        case Block::Initialized:
            return 'I';
        case Block::Terminating:
            return 'T';
        default:
            // If get to this point the compute block status is unknown
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::InconsistentDataError,
                    Internal_Invalid_Block_Status_Str
            );
    }
}

Block::Status
Block::Impl::convertDBStatusToBlockStatus(
        const char *status
)
{
    if (strcmp(status,BGQDB::BLOCK_FREE) == 0) {
        return Block::Free;
    } else if (strcmp(status,BGQDB::BLOCK_INITIALIZED) == 0) {
        return Block::Initialized;
    } else if (strcmp(status,BGQDB::BLOCK_BOOTING) == 0) {
        return Block::Booting;
    } else if (strcmp(status,BGQDB::BLOCK_TERMINATING) == 0) {
        return Block::Terminating;
    } else if (strcmp(status,BGQDB::BLOCK_ALLOCATED) == 0) {
        return Block::Allocated;
    }
    // If get to this point the compute block status is unknown
    THROW_EXCEPTION(
            bgsched::InternalException,
            bgsched::InternalErrors::InconsistentDataError,
            Internal_Invalid_Block_Status_Str
    );
}

Block::Status
Block::Impl::convertDBBlockStateToBlockStatus(
        const int state
)
{
    switch(state)
    {
        case BGQDB::FREE:
            return Block::Free;
        case BGQDB::ALLOCATED:
            return Block::Allocated;
        case BGQDB::INITIALIZED:
            return Block::Initialized;
        case BGQDB::BOOTING:
            return Block::Booting;
        case BGQDB::TERMINATING:
            return Block::Terminating;
        case BGQDB::INVALID_STATE:
            break;
    }
    // If get to this point the compute block status is unknown
    THROW_EXCEPTION(
            bgsched::InternalException,
            bgsched::InternalErrors::InconsistentDataError,
            Internal_Invalid_Block_Status_Str
    );
}

Block::Impl::Impl(
        const string& name,
        const Block::Pimpl fromBlock
) :
    _name(name),
    _creationId(0),       // Do not copy creation id, this is not the same compute block
    _description(fromBlock->_description),
    _owner(fromBlock->_owner),
    _user(),              // Do not copy user, this is not the same compute block
    _bootOptions(fromBlock->_bootOptions),
    _mImage(fromBlock->_mImage),
    _nodeConfig(fromBlock->_nodeConfig),
    _sequenceId(0),       // Do not copy sequence id, this is not the same compute block
    _status(Block::Free), // Don't copy compute block status
    _numcnodes(fromBlock->_numcnodes),
    _dimensionSizes(0,0,0,0),
    _connectivity(),
    _options(fromBlock->_options),
    _jobIds(),             // Don't copy the compute block jobs
    _midplanes(),
    _passthroughMidplanes(),
    _nodeBoards(),
    _blockDBInfoPtr(),     // Don't copy
    _action(Block::Action::None)  // Don't copy compute block action
{

    for (Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension)
    {
        _dimensionSizes[dimension] = fromBlock->_dimensionSizes[dimension];
        _connectivity[dimension] = fromBlock->_connectivity[dimension];
    }

    // Copy midplane locations vector
    _midplanes = fromBlock->_midplanes;
    // Copy pass-through midplane vector
    _passthroughMidplanes = fromBlock->_passthroughMidplanes;
    // Copy node board vector
    _nodeBoards = fromBlock->_nodeBoards;
}

Block::Impl::Impl(
        const cxxdb::Columns& block_cols,
        bool isBlockExtendedInfo,
        cxxdb::Connection& conn
) :
    _name(block_cols[BGQDB::DBTBlock::BLOCKID_COL].getString()),
    _creationId(block_cols[BGQDB::DBTBlock::CREATIONID_COL].as<BlockCreationId>()),
    _description(block_cols[BGQDB::DBTBlock::DESCRIPTION_COL].isNull() ? "" : block_cols[BGQDB::DBTBlock::DESCRIPTION_COL].getString()),
    _owner(block_cols[BGQDB::DBTBlock::OWNER_COL].isNull() ? "" : block_cols[BGQDB::DBTBlock::OWNER_COL].getString()),
    _user(block_cols[BGQDB::DBTBlock::USERNAME_COL].isNull() ? "" : block_cols[BGQDB::DBTBlock::USERNAME_COL].getString()),
    _bootOptions(block_cols[BGQDB::DBTBlock::BOOTOPTIONS_COL].isNull() ? "" : block_cols[BGQDB::DBTBlock::BOOTOPTIONS_COL].getString()),
    _mImage(block_cols[BGQDB::DBTBlock::MLOADERIMG_COL].isNull() ? "" : block_cols[BGQDB::DBTBlock::MLOADERIMG_COL].getString()),
    _nodeConfig(block_cols[BGQDB::DBTBlock::NODECONFIG_COL].getString()),
    _sequenceId(block_cols[BGQDB::DBTBlock::SEQID_COL].as<SequenceId>()),
    _status(convertDBStatusToBlockStatus(block_cols[BGQDB::DBTBlock::STATUS_COL].getString().c_str())),
    _numcnodes(block_cols[BGQDB::DBTBlock::NUMCNODES_COL].as<uint32_t>()),
    _dimensionSizes(0,0,0,0),
    _connectivity(),
    _options(block_cols[BGQDB::DBTBlock::OPTIONS_COL].getString()),
    _jobIds(),         // Don't set job ids on constructor
    _midplanes(),
    _passthroughMidplanes(),
    _nodeBoards(),
    _blockDBInfoPtr(),  // Don't set when creating from compute block database
    _action(convertDBActionToBlockAction(block_cols[BGQDB::DBTBlock::ACTION_COL].getString().c_str()))
{
    // Convert torus char(5) in database to connectivity (Torus/Mesh) array (by dimension)
    string torus_dims_str(block_cols[BGQDB::DBTBlock::ISTORUS_COL].isNull() ? "     " : block_cols[BGQDB::DBTBlock::ISTORUS_COL].getString());
    // Only care about A-D dimensions
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        if (torus_dims_str[dim] == '1') {
            _connectivity[dim] = Block::Connectivity::Torus;
        } else {
            _connectivity[dim] = Block::Connectivity::Mesh;
        }
    }

    // Set the compute block shape for large compute blocks
    if (isLarge()) {
        _dimensionSizes[Dimension::A] = (block_cols[BGQDB::DBTBlock::SIZEA_COL].as<uint32_t>())/4;
        _dimensionSizes[Dimension::B] = (block_cols[BGQDB::DBTBlock::SIZEB_COL].as<uint32_t>())/4;
        _dimensionSizes[Dimension::C] = (block_cols[BGQDB::DBTBlock::SIZEC_COL].as<uint32_t>())/4;
        _dimensionSizes[Dimension::D] = (block_cols[BGQDB::DBTBlock::SIZED_COL].as<uint32_t>())/4;
        // Log compute block shape
        //LOG_TRACE_MSG("Compute block shape: " << _dimensionSizes[Dimension::A] << "," << _dimensionSizes[Dimension::B] << ","
        //                              << _dimensionSizes[Dimension::C] << "," << _dimensionSizes[Dimension::D]);
    }

    if (isBlockExtendedInfo) {
        if (isLarge()) {  // It's a large compute block.
            // Get the midplanes and pass-through midplanes for the compute block
            queryLargeBlockInfo(_name, conn, _midplanes, _passthroughMidplanes);

            // Verify midplanes were returned - compute block might have been deleted
            if (_midplanes.empty()) {
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::InconsistentDataError,
                        "No midplanes found in database for compute block " << _name
                );
            }

        } else {  // It's a small compute block, get the node boards for the compute block.
            _nodeBoards = querySmallBlockNodeBoards(_name, conn);
            if (_nodeBoards.empty()) {
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::InconsistentDataError,
                        "No node boards found in database for compute block " << _name
                );
            }
            // Get node board location (form of Rxx-Mx-Nxx)
            string nodeBoardLocation = _nodeBoards[0];
            // Extract the midplane from the node board location
            _midplanes.push_back(nodeBoardLocation.substr(0, MidplaneLength));
        }
    }
}

Block::Impl::Impl(
        BlockDatabaseInfo::Ptr blockDBInfoPtr
) :
    _name(),
    _creationId(0),
    _description(),
    _owner(),
    _user(),   // Don't set user on constructor
    _bootOptions(),
    _mImage(DEFAULT_MLOADERIMG),
    _nodeConfig(DEFAULT_COMPUTENODECONFIG),
    _sequenceId(0),
    _status(Block::Free), // New compute block is free
    _numcnodes(0),
    _dimensionSizes(0,0,0,0),
    _connectivity(),
    _options(),
    _jobIds(), // Don't set job ids on constructor
    _midplanes(),
    _passthroughMidplanes(),
    _nodeBoards(),
    _blockDBInfoPtr(blockDBInfoPtr),
    _action(Block::Action::None) // New compute block is None
{
    // Process generic fields: name, owner, description, micro-loader image and node config
    string blockName = blockDBInfoPtr->getId();
    string blockOwner = blockDBInfoPtr->getOwner();
    string blockDescription = blockDBInfoPtr->getDescription();
    string blockMloaderImg = blockDBInfoPtr->getMloaderImg();
    string blockNodeConfig = blockDBInfoPtr->getNodeConfig();

    // Process compute block name
    if (!blockName.empty()) {
        _name = blockName;
    }

    // Process compute block owner
    if (!blockOwner.empty()) {
        _owner = blockOwner;
    }

    // Process compute block description
    if (!blockDescription.empty()) {
        _description = blockDescription;
    }

    // Process micro-loader image
    if (!blockMloaderImg.empty()) {
        _mImage = blockMloaderImg;
    }

    // Process node config
    if (!blockNodeConfig.empty()) {
        _nodeConfig = blockNodeConfig;
    }

    // Set connectivity
    BlockDatabaseInfo::ConnectivitySpecs connectivity = blockDBInfoPtr->getConnectivitySpecs();
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        if (connectivity[dim] == BGQDB::Connectivity::Torus) {
            _connectivity[dim] = Block::Connectivity::Torus;
        } else {
            _connectivity[dim] = Block::Connectivity::Mesh;
        }
    }

    // Check if large or small compute block
    if (blockDBInfoPtr->isLarge()) {
        // Get dimension sizes
        BGQDB::DimensionSizes dimensionSizes = blockDBInfoPtr->getSizes();
        // Need to convert to midplanes
        _dimensionSizes[Dimension::A] = (dimensionSizes[BGQDB::Dimension::A])/4;
        _dimensionSizes[Dimension::B] = (dimensionSizes[BGQDB::Dimension::B])/4;
        _dimensionSizes[Dimension::C] = (dimensionSizes[BGQDB::Dimension::C])/4;
        _dimensionSizes[Dimension::D] = (dimensionSizes[BGQDB::Dimension::D])/4;

        // Get midplanes
        BGQDB::BlockDatabaseInfo::Midplanes midplanes = blockDBInfoPtr->getMidplanes();
        // Verify midplanes were returned
        if (midplanes.empty()) {
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Block_No_Midplanes_Found_Str
            );
        }
        // Set number of compute nodes
        _numcnodes = midplanes.size() * Midplane::NodeCount;
        // Iterate over midplanes and extract midplane locations
        for (BGQDB::BlockDatabaseInfo::Midplanes::iterator iter = midplanes.begin(); iter != midplanes.end(); ++iter) {
            // Set midplane location from structure
            _midplanes.push_back((*iter).location);
        }

        // Get switches
        BGQDB::BlockDatabaseInfo::Switches switches = blockDBInfoPtr->getSwitches();
        set<string> passthrough_mps; // Used to remove any duplicate pass-through midplane locations
        // Iterate over switches and extract midplane locations from pass-through switches
        for (BGQDB::BlockDatabaseInfo::Switches::iterator iter = switches.begin(); iter != switches.end(); ++iter) {
            // Only extract midplane location if it is a pass-through switch
            if ((*iter).used_for_passthrough == true) {
                // Get the switch location (format A_Rxx-Mx)
                string location((*iter).id);
                // Extract pass-through midplane location based on switch location (Rxx-Mx)
                // Use a set to eliminate any duplicates
                passthrough_mps.insert(location.substr(SwitchLength - MidplaneLength, MidplaneLength));
            }
        }
        // Copy pass-through midplanes from set to vector
        if (!passthrough_mps.empty()) {
            for (set<string>::iterator iter = passthrough_mps.begin(); iter != passthrough_mps.end(); ++iter) {
                _passthroughMidplanes.push_back((*iter));
            }
        }
    } else { // Small compute block
        // Get the midplane location string for the compute block
        string blockMidplaneLocation = blockDBInfoPtr->getMidplane();
        // Validate location string is not empty
        if (blockMidplaneLocation.empty()) {
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Internal_No_Midplane_Found_Str
                    );
        }
        // Set the midplane
        _midplanes.push_back(blockMidplaneLocation);
        // Build up the location, example R01-M0- (N00-N15 to follow)
        blockMidplaneLocation.append("-");
        // Get the list of node boards (e.g. N00)
        NodeBoardPositions nodeBoardPositions = blockDBInfoPtr->getNodeBoards();
        // Validate we got node boards
        if (nodeBoardPositions.size() == 0) {
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Internal_No_Nodeboards_Found_Str
                    );
        }
        // Iterate over node boards and construct node board location strings
        for (NodeBoardPositions::iterator iter = nodeBoardPositions.begin(); iter != nodeBoardPositions.end(); ++iter) {
            // Start with "Rxx-Mx-" format
            string location(blockMidplaneLocation);
            // Next append the node board position Nxx to form the complete location "Rxx-Mx-Nxx"
            location.append(*iter);
            // Set the node board location
            _nodeBoards.push_back(location);
            _numcnodes = _numcnodes + NodeBoard::NodeCount;
        }
    }
}

void
Block::Impl::toString(
        ostream& os,
        bool verbose
) const
{
    // Common compute block info
    os << endl << "Compute block=" << _name << endl;         // Compute block name
    os << "Compute nodes=" << _numcnodes << endl;            // Block compute nodes

    // Compute block status
    switch (_status) {
      case Block::Allocated:
          os << "Status=Allocated" << endl;
        break;
      case Block::Booting:
          os << "Status=Booting" << endl;
        break;
      case Block::Free:
          os << "Status=Free" << endl;
        break;
      case Block::Initialized:
          os << "Status=Initialized" << endl;
        break;
      case Block::Terminating:
          os << "Status=Terminating" << endl;
        break;
      default:
          os << "Status=Unknown" << endl;
        break;
    }

    // Compute block action
    switch (_action) {
      case Block::Action::None:
          os << "Action=None" << endl;
        break;
      case Block::Action::Boot:
          os << "Action=Boot" << endl;
        break;
      case Block::Action::Free:
          os << "Action=Free" << endl;
        break;
      default:
          os << "Action=Unknown" << endl;
        break;
    }

    // Verbose compute block details
    if (verbose) {
        os << "Description=" << _description << endl;        // Compute block description
        os << "Owner=" << _owner << endl;                    // Compute block owner
        os << "Booted by=" << _user << endl;                 // Compute block user
        os << "Boot options=" << _bootOptions << endl;       // Compute block boot options
        os << "Options=" << _options << endl;                // Compute block options
        os << "Microloader image=" << _mImage << endl;       // Compute block microloader image
        os << "Node configuration=" << _nodeConfig << endl;  // Compute block node configuration
        os << "Sequence ID=" << _sequenceId << endl;         // Sequence ID

        if (isLarge()) {
            // Compute block shape
            os << "Shape=" << _dimensionSizes[Dimension::A] << "x" << _dimensionSizes[Dimension::B] << "x"
                << _dimensionSizes[Dimension::C] << "x" << _dimensionSizes[Dimension::D] << endl;
            // Compute block midplanes
            if (!_midplanes.empty()) {
                os << "Midplanes:" << endl;
                for (Midplanes::const_iterator i = _midplanes.begin(); i != _midplanes.end(); ++i) {
                    os << (*i) << endl;
                }
            }
            // Compute block pass-through midplanes
            if (!_passthroughMidplanes.empty()) {
                os << "Pass-through midplanes:" << endl;
                for (PassthroughMidplanes::const_iterator i = _passthroughMidplanes.begin(); i != _passthroughMidplanes.end(); ++i) {
                    os << (*i) << endl;
                }
            }
            // Compute block connectivity
            os << "Connectivity:" << endl;
            for (Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension) {
                os << string(dimension) << ":" << (isTorus(dimension)?"Torus":"Mesh") << ", ";
            }
            os << endl;
        } else { // Small compute block node boards
            if (!_nodeBoards.empty()) {
                os << "Node boards:" << endl;
                for (Block::NodeBoards::const_iterator nb = _nodeBoards.begin(); nb != _nodeBoards.end(); ++nb) {
                    os << (*nb) << endl;
                }
            }
        }
    }
}

BGQDB::BlockDatabaseInfo::Ptr
Block::Impl::add(
        const string& owner,
        bool isValidateBlockOnly
)
{
    // For large compute blocks verify midplanes were specified
    if (isLarge()) { // Adding a large block
        // Verify we are working with a valid large compute block object
        if (_midplanes.empty()) {
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidMidplanes,
                    Block_No_Midplanes_Found_Str
            );
        }
    }

    bool isSlowPath = true;
    // If only validating the compute block take the slow path
    if (isValidateBlockOnly)  {
        isSlowPath = true;
    } else {
        // Take slow path for small compute blocks
        if (isSmall()) {
            isSlowPath = true;
        } else {
            // If already have a ptr to BlockDatabaseInfo object (from previous call to Block::create) take the fast path
            if (_blockDBInfoPtr) {
                isSlowPath = false;
            } else {
                isSlowPath = true;
            }
        }
    }

    // Check if using the slow path to add the compute block
    if (isSlowPath) {
        BGQDB::BlockDatabaseInfo::Ptr blockDBInfoPtr(new BlockDatabaseInfo());
        BGQDB::GenBlockParams genblock_params;

        try {
            // Set common compute block fields
            // Use caller user id if owner not set.
            if (owner.empty()) {
                try {
                    bgq::utility::UserId uid;
                    string user(uid.getUser());
                    LOG_DEBUG_MSG("Set compute block owner to " << user);
                    genblock_params.setOwner(user);            // Set block owner
                } catch (const runtime_error& e) {
                    LOG_ERROR_MSG(e.what());
                    THROW_EXCEPTION(
                            bgsched::RuntimeException,
                            bgsched::RuntimeErrors::BlockAddError,
                            "Unable to add compute block, error setting owner."
                    );
                }

            } else {
                LOG_DEBUG_MSG("Set compute block owner to " << owner);
                genblock_params.setOwner(owner);                // Set compute block owner
            }
            genblock_params.setBlockId(_name);                  // Set compute block name
            genblock_params.setDescription(_description);       // Set compute block description
            genblock_params.setOptions(_options);               // Set compute block options
            genblock_params.setBootOptions(_bootOptions);       // Set compute block boot options
            genblock_params.setMicroloaderImage(_mImage);       // Set compute block microloader image
            genblock_params.setNodeConfiguration(_nodeConfig);  // Set compute block node configuration

            if (isLarge()) { // Adding a large compute block
                // Set the pass-through and midplane locations - OK to have duplicate pass-through midplane locations
                genblock_params.setMidplanes(_midplanes, _passthroughMidplanes);

                // Set dimension spec (Torus/Mesh for each dimension)
                BGQDB::DimensionSpecs dimSpecs;
                for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
                {
                    // Set the connectivity (Torus/Mesh) for the dimension - MUST be done after setMidplanes
                    if (_connectivity[dim] == Block::Connectivity::Torus) {
                        dimSpecs[dim] = DimensionSpec(BGQDB::Connectivity::Torus);
                    } else {
                        dimSpecs[dim] = DimensionSpec(BGQDB::Connectivity::Mesh);
                    }
                }
                genblock_params.setDimensionSpecs(dimSpecs);
            } else { // Adding a small compute block
                // Set the node board locations
                genblock_params.setNodeBoardLocations(_nodeBoards);
            }

            // Check if adding compute block or only performing validation on compute block
            if (isValidateBlockOnly) {
                blockDBInfoPtr->setId(_name);
                BGQDB::genBlockParamsToBlockDatabaseInfoEx(genblock_params, *blockDBInfoPtr);
            } else {
                // Add compute block to database, skip additional checking as we have a valid compute block already
                BGQDB::genBlockEx(genblock_params, false);
                LOG_DEBUG_MSG("Compute block " << _name << " was successfully added.");
            }
        } catch (const BGQDB::Exception& e) {
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::BlockNotAdded,
                    "Compute block not added, error is " << e.what()
            );
        }

        return blockDBInfoPtr;
    } else {
        // Got a BlockDatabaseInfo object so refresh base block settings and use fast path to add the block
        return addFastPath(owner);
    }
}

BGQDB::BlockDatabaseInfo::Ptr
Block::Impl::addFastPath(
        const string& owner
)
{
    try {
        // Refresh compute block fields which may be out of sync
        // Use caller user id if owner not set.
        if (owner.empty()) {
            try {
                bgq::utility::UserId uid;
                string user(uid.getUser());
                LOG_DEBUG_MSG("Set compute block owner to " << user);
                _blockDBInfoPtr->setOwner(user);             // Set compute block owner
            } catch (const runtime_error& e) {
                LOG_ERROR_MSG(e.what());
                THROW_EXCEPTION(
                        bgsched::RuntimeException,
                        bgsched::RuntimeErrors::BlockAddError,
                        "Unable to add compute block, error setting owner."
                );
            }

        } else {
            LOG_DEBUG_MSG("Set compute block owner to " << owner);
            _blockDBInfoPtr->setOwner(owner);                // Set compute block owner
        }

        _blockDBInfoPtr->setId(_name);                       // Set compute block name
        _blockDBInfoPtr->setDescription(_description);       // Set compute block description
        _blockDBInfoPtr->setOptions(_options);               // Set compute block options
        _blockDBInfoPtr->setBootOptions(_bootOptions);       // Set compute block boot options
        _blockDBInfoPtr->setMloaderImg(_mImage);             // Set compute block microloader image
        _blockDBInfoPtr->setNodeConfig(_nodeConfig);         // Set compute block node configuration

        // Add compute block to database
        BGQDB::insertBlock(*_blockDBInfoPtr);
        LOG_DEBUG_MSG("Compute block " << _name << " was successfully added.");
    } catch (const BGQDB::Exception& dbe) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotAdded,
                "Compute block not added, error is " << dbe.what()
        );
    } catch (const cxxdb::DatabaseException& cxxdbe) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                "Compute block not added, error is " << cxxdbe.what()
        );
    }  catch (const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotAdded,
                "Compute block not added, error is " << e.what()
        );
    }
    return _blockDBInfoPtr;
}

void
Block::Impl::setName(
        const string& blockName
)
{
    BGQDB::DBTBlock dbo;
    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name size
    if (blockName.size() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }
    _name = blockName;
}

void
Block::Impl::setDescription(
        const string& description
)
{
    BGQDB::DBTBlock dbo;
    // Verify compute block description was specified
    if (description.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockDescription,
                No_Block_Description_Str
                );
    }
    // Validate the compute block description string size
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
Block::Impl::setOptions(
        const string& options
)
{
    BGQDB::DBTBlock dbo;
    // Verify compute block options were specified
    if (options.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockOptions,
                No_Block_Options_Str
                );
    }
    // Validate the compute block options string size
    if (options.size() >= sizeof(dbo._options)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockOptions,
                Block_Options_Too_Long_Str
                );
    }
    _options = options;
}

void
Block::Impl::setBootOptions(
        const string& bootOptions
)
{
    BGQDB::DBTBlock dbo;
    // Verify compute block boot options were specified
    if (bootOptions.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockBootOptions,
                No_Block_BootOptions_Str
                );
    }
    // Validate the compute block boot options string size
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
Block::Impl::setMicroLoaderImage(
        const string& image
)
{
    BGQDB::DBTBlock dbo;
    // Verify compute block micro-loader image was specified
    if (image.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockMicroLoaderImage,
                No_Block_MicroLoaderImage_Str
                );
    }
    // Validate the compute block micro-loader image string size
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
Block::Impl::setNodeConfiguration(
        const string& nodeConfig
)
{
    BGQDB::DBTBlock dbo;
    // Verify compute block Node configuration was specified
    if (nodeConfig.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockNodeConfiguration,
                No_Block_NodeConfiguration_Str
                );
    }
    // Validate the compute block Node configuration string size
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
Block::Impl::setStatus(
        Block::Status status
)
{
    _status = status;
}

void
Block::Impl::addJobId(
        Job::Id jobId
)
{
    _jobIds.push_back(jobId);
}

void
Block::Impl::addMidplane(
        const string& midplane
)
{
    if (midplane.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidMidplanes,
                No_Midplane_Location_Str
        );
    } else {
        _midplanes.push_back(midplane);
    }
}

void
Block::Impl::addNodeBoard(
        const string& nodeBoard
)
{
    if (nodeBoard.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                No_Nodeboard_Location_Str
        );
    } else {
        _nodeBoards.push_back(nodeBoard);
    }
}

void
Block::Impl::addPassthroughMidplane(
        const string& passthroughMidplane
)
{
    if (passthroughMidplane.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidPassthroughMidplanes,
                No_Passthrough_Midplane_Location_Str
        );
    } else {
        _passthroughMidplanes.push_back(passthroughMidplane);
    }
}


uint32_t
Block::Impl::getDimensionSize(
        const Dimension& dimension
) const
{
     if (isSmall()) {
         // Must be a large compute block
         THROW_EXCEPTION(
                 bgsched::InputException,
                 bgsched::InputErrors::InvalidBlockSize,
                 "Must be a large compute block to get midplane dimension size."
         );
     }
     if (dimension <= Dimension::D) {
         if (_dimensionSizes[dimension] > 0) {
             return _dimensionSizes[dimension];
         } else {
            return 0;
         }
     } else {
         // Dimension is outside valid range
         THROW_EXCEPTION(
                 bgsched::InputException,
                 bgsched::InputErrors::InvalidDimension,
                 "Dimension must be A, B, C or D"
         );
     }
}

uint32_t
Block::Impl::getDimensionSize(
        const bgq::util::Location::Dimension::Value dimension
) const
{
     if (isSmall()) {
         // Must be a large compute block
         THROW_EXCEPTION(
                 bgsched::InputException,
                 bgsched::InputErrors::InvalidBlockSize,
                 "Must be a large compute block to get midplane dimension size."
         );
     }
     if (dimension <= bgq::util::Location::Dimension::D) {
         if (_dimensionSizes[dimension] > 0) {
             return _dimensionSizes[dimension];
         } else {
            return 0;
         }
     } else {
         // Dimension is outside valid range
         THROW_EXCEPTION(
                 bgsched::InputException,
                 bgsched::InputErrors::InvalidDimension,
                 "Dimension must be A, B, C or D"
         );
     }
}

bool
Block::Impl::isTorus(
        const Dimension& dimension
) const
{
    if (isSmall()) {
        // Must be a large block
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockSize,
                "Must be a large compute block to get dimension connectivity."
        );
    }
    if (dimension <= Dimension::D) {
        if (_connectivity.at(dimension) == Connectivity::Torus) {
            return true;
        } else {
            return false;
        }
    } else {
        // Dimension is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension must be A, B, C or D"
        );
    }
}

bool
Block::Impl::isMesh(
        const Dimension& dimension
) const
{
    if (isSmall()) {
        // Must be a large compute block
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockSize,
                "Must be a large compute block to get dimension connectivity."
        );
    }
    if (dimension <= Dimension::D) {
        if (_connectivity.at(dimension) == Connectivity::Mesh) {
            return true;
        } else {
            return false;
        }
    } else {
        // Dimension is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension must be A, B, C or D"
        );
    }
}

const string&
Block::Impl::getName() const
{
    return _name;
}

BlockCreationId
Block::Impl::getCreationId() const
{
    return _creationId;
}

const string&
Block::Impl::getDescription() const
{
    return _description;
}

const string&
Block::Impl::getOwner() const
{
    return _owner;
}

const string&
Block::Impl::getUser() const
{
    return _user;
}

const string&
Block::Impl::getBootOptions() const
{
    return _bootOptions;
}

const string&
Block::Impl::getMicroLoaderImage() const
{
    return _mImage;
}

const string&
Block::Impl::getNodeConfiguration() const
{
    return _nodeConfig;
}

SequenceId
Block::Impl::getSequenceId() const
{
    return _sequenceId;
}

EnumWrapper<Block::Status>
Block::Impl::getStatus() const
{
    return _status;
}

uint32_t
Block::Impl::getComputeNodeCount() const
{
    return _numcnodes;
}

const string&
Block::Impl::getOptions() const
{
    return _options;
}

const vector<Job::Id>
Block::Impl::getJobIds() const
{
    return _jobIds;
}

bool
Block::Impl::isLarge() const
{
    return (_numcnodes >= Midplane::NodeCount);
}

bool
Block::Impl::isSmall() const
{
    return (_numcnodes < Midplane::NodeCount);
}

Block::Midplanes
Block::Impl::getMidplanes() const
{
    return _midplanes;
}

Block::NodeBoards
Block::Impl::getNodeBoards() const
{
    return _nodeBoards;
}

Block::PassthroughMidplanes
Block::Impl::getPassthroughMidplanes() const
{
    return _passthroughMidplanes;
}

void
Block::Impl::setAction(
        Block::Action::Value action
)
{
    _action = action;
}

EnumWrapper<Block::Action::Value>
Block::Impl::getAction() const
{
    return _action;
}

Block::Action::Value
Block::Impl::convertDBActionToBlockAction(
        const char *action
)
{
    if (strcmp(action,BGQDB::BLOCK_BOOTING) == 0) {
        return Block::Action::Boot;
    } else if (strcmp(action,BGQDB::BLOCK_DEALLOCATING) == 0) {
        return Block::Action::Free;
    } else  {
        return Block::Action::None;
    }
}

} // namespace bgsched
