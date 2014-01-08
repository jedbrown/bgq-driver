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

#include <bgsched/Block.h>
#include <bgsched/DatabaseException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/IOLink.h>
#include <bgsched/RuntimeException.h>

#include <bgsched/core/core.h>

#include "bgsched/BlockImpl.h"
#include "bgsched/IOLinkImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/BlockInfo.h>
#include <db/include/api/Exception.h>
#include <db/include/api/genblock.h>
#include <db/include/api/GenBlockParams.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <hlcs/include/security/privileges.h>

#include <utility/include/Log.h>
#include <utility/include/Symlink.h>
#include <utility/include/UserId.h>
#include <utility/include/XMLEntity.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include <pthread.h>
#include <sstream>
#include <string>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace BGQDB;
using namespace std;

// Statically initialize "fast" mutex
pthread_mutex_t blockaction_lock = PTHREAD_MUTEX_INITIALIZER;

namespace {
    // Default compute block description
    const string Block_Description("Created by IBM Scheduler API");

    // Message strings
    const string No_Block_Name_Str("Compute block name is empty.");
    const string Block_Name_Too_Long_Str("Compute block name is too long.");
    const string DB_Access_Error_Str("Error occurred while accessing database.");
    const string DB_Access_Error_Str_NodeConfig("Error occurred while updating database. Possible cause is node configuration does not currently exist in the database.");
    const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
    const string Unexpected_Exception_Str("Received unexpected exception from internal method.");
    const string Unexpected_Return_Code_Str("Unexpected return code from internal database function.");
    const string Block_Description_Too_Long_Str("Compute block description is too long.");
    const string Block_Options_Too_Long_Str("Compute block options are too long.");
    const string Block_Info_Too_Long_Str("Options, boot options, Node configuration or micro-loader image is too long.");
    const string Midplanes_Empty_Str("Compute block not created, no midplanes found.");
    const string NodeBoard_Empty_Str("Compute Block not created, no node boards found.");
    const string NodeBoard_Start_Invalid_Arg_Str("Compute block not created, node board start location argument is not valid.");
    const string No_User_Name_Str("User name is empty.");
    const string XML_Parse_Error_Str("Error parsing XML data.");

    static const unsigned MidplaneLength = 6;
    static const unsigned NodeBoardLocationLength = 10;
    static const unsigned NodeBoardPositionLength = 3;
} // anonymous namespace

namespace bgsched {

Block::Block(
        Pimpl impl
        ) :
    _impl(impl)
{
    // Nothing to do
}

Block::Pimpl
Block::getPimpl() const
{
    return _impl;
}

Block::Ptr
Block::create(
        const Midplanes& midplanes,
        const PassthroughMidplanes& passthroughMidplanes,
        const DimensionConnectivity& dimensionConnectivity
)
{
    GenBlockParams genBlockParams;
    BlockDatabaseInfo::Ptr blockDBInfoPtr(new BlockDatabaseInfo());

    // Verify we got midplanes passed in
    if (midplanes.empty()) {
        // No midplanes
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidMidplanes,
                Midplanes_Empty_Str
                );
    }
    ostringstream os;
    BGQDB::DimensionSpecs dimensionSpecs;

    // Search dimensions A, B, C, and D for valid connectivity settings (Torus/Mesh)
    for (bgsched::Dimension dim = bgsched::Dimension::A; dim <= bgsched::Dimension::D; ++dim) {
        // Was dimension set?
        const DimensionConnectivity::const_iterator iter = dimensionConnectivity.find(dim);
        if (iter == dimensionConnectivity.end()) {
            os << "Compute block not created, connectivity (Torus/Mesh) was not set for dimension " << string(dim);
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidConnectivity,
                    os.str()
            );
        }
        // Was dimension set to Torus or Mesh?
        if ((iter->second == Block::Connectivity::Torus) || (iter->second == Block::Connectivity::Mesh)) {
            // Set the genblock params connectivity structure
            if (iter->second == Block::Connectivity::Torus) {
                // Set Torus connectivity for dimension
                dimensionSpecs[dim] = DimensionSpec(BGQDB::Connectivity::Torus);
            } else {
                // Set Mesh connectivity for dimension
                dimensionSpecs[dim] = DimensionSpec(BGQDB::Connectivity::Mesh);
            }
        } else {
            os << "Compute block not created, connectivity (Torus/Mesh) was not set correctly for dimension " << string(dim);
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidConnectivity,
                    os.str()
            );
        }
    }

    // Set the description, midplanes, pass-through midplanes and connectivity in the genblock params
    try {
        // Set default compute block description, can be overridden later
        genBlockParams.setDescription(Block_Description);
        // If the dimension connectivity is a mesh, the mesh will start at the first midplane location given
        genBlockParams.setMidplanes(midplanes, passthroughMidplanes);
        // Set the connectivity -- MUST be done after setMidplanes()
        genBlockParams.setDimensionSpecs(dimensionSpecs);
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotCreated,
                "Compute block not created, error is: " << e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotCreated,
                "Compute block not created, unexpected error calling GenBlockParams"
                );
    }

    // Convert from genblock format to block DB format
    try {
        genBlockParamsToBlockDatabaseInfoEx(genBlockParams, *blockDBInfoPtr);
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotCreated,
                "Compute block not created, error is: " << e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotCreated,
                "Compute block not created, unexpected error calling genBlockParamsToBlockDatabaseInfoEx"
        );
    }

    // Construct Block impl from the block database info
    Block::Pimpl blockPimpl(new Block::Impl(blockDBInfoPtr));
    return Ptr(new Block(blockPimpl));
}


Block::Ptr
Block::create(
        const Block::NodeBoards& nodeBoardLocations
)
{
    GenBlockParams genBlockParams;
    BlockDatabaseInfo::Ptr blockDBInfoPtr(new BlockDatabaseInfo());
    vector<IOLink::ConstPtr> IOLinks;

    // Verify we got node boards passed in
    if (nodeBoardLocations.empty()) {
        // No node boards
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                NodeBoard_Empty_Str
        );
    }
/* Temporary workaround --- See Issue 7414

    // Extract midplane location (e.g. R00-M1) from node board location (e.g. R00-M1-N08)
    string midplaneLocation = nodeBoardLocations[0].substr(0,6);

    // Verify that I/O link exists for the compute block
    try {
        // Get the I/O links for the midplane
        IOLinks = bgsched::core::getIOLinks(midplaneLocation);
    } catch (...) { // Handle any exception retrieving I/O links
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockCreateError,
                "Error retrieving I/O links, unable to create small block."
        );
    }


    bool IOAvailable = false;
    // Search thru the node boards for I/O links
    for (Block::NodeBoards::const_iterator iter = nodeBoardLocations.begin(); iter != nodeBoardLocations.end(); ++iter) {
        string nodeBoardLocation = *iter;
        // Search thru the midplane I/O links looking for matching node board
        for (vector<IOLink::ConstPtr>::iterator it = IOLinks.begin(); it != IOLinks.end(); ++it) {
            string IOLinkLocation = (*it)->getLocation();
            // Check if IO link (e.g. R00-M1-N08-J06) is available on the node board
            if (nodeBoardLocation == IOLinkLocation.substr(0,10)) {
                IOAvailable = true;
                break;
            }
        }
        // If found I/O then stop searching
        if (IOAvailable) {
            break;
        }
    }

    // Throw exception if no I/O links were found on node boards
    if (!IOAvailable) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, no I/O links found on node boards."
        );
    }
*/

    // Set the compute block description and node board locations in the genblock params
    try {
        // Set default compute block description, can be overridden later
        genBlockParams.setDescription(Block_Description);
        // Following throws std::invalid_argument if the node board locations are not valid
        genBlockParams.setNodeBoardLocations(nodeBoardLocations);
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, error is: " << e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, unexpected error calling GenBlockParams"
        );
    }

    // Convert from genblock format to compute block DB format
    try {
        genBlockParamsToBlockDatabaseInfoEx(genBlockParams, *blockDBInfoPtr);
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, error is: " << e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, unexpected error calling genBlockParamsToBlockDatabaseInfoEx"
        );
    }

    // Construct Block impl from the block database info
    Block::Pimpl blockPimpl(new Block::Impl(blockDBInfoPtr));
    return Ptr(new Block(blockPimpl));
}

Block::Ptr
Block::create(
        const string& nodeBoardStartLocation,
        const uint8_t nodeBoardCount
)
{
    GenBlockParams genBlockParams;
    BlockDatabaseInfo::Ptr blockDBInfoPtr(new BlockDatabaseInfo());
    vector<IOLink::ConstPtr> IOLinks;

    // Verify we got valid starting node board passed in (form is Rxx-Mx-Nxx)
    if ((nodeBoardStartLocation.empty()) || (nodeBoardStartLocation.size() != NodeBoardLocationLength)) {
        // Bad starting node board specified
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                NodeBoard_Start_Invalid_Arg_Str
                );
    }

    // Verify we got valid node board count (range is 1 to 8)
    if ((nodeBoardCount == 0) || (nodeBoardCount > 8)) {
        // Node board count is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoardCount,
                "Node board count argument must be in range 1 to 8"
                );
    }

/* Temporary workaround --- See Issue 7414
    // Extract midplane location (e.g. R00-M1) from node board location (e.g. R00-M1-N08)
    string midplaneLocation = nodeBoardStartLocation.substr(0,6);

    // Verify that I/O link exists for the compute block
    try {
        // Get the I/O links for the midplane
        IOLinks = bgsched::core::getIOLinks(midplaneLocation);
    } catch (...) { // Handle any exception retrieving I/O links
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockCreateError,
                "Error retrieving I/O links, unable to create small block."
        );
    }

    // Search thru the node boards for I/O links
    bool IOAvailable = false;
    string nodeBoardLocation = nodeBoardStartLocation;
    // String to concatenate node board position to (e.g. R01-M1-N)
    string nodeBoardLocationSubset = nodeBoardStartLocation.substr(0,8);
    int nextBoard = 0;

    try {
        // Grab the starting node board position
        nextBoard = boost::lexical_cast<int>(nodeBoardLocation.substr(8,2));
    } catch(const boost::bad_lexical_cast&) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, starting node board location is not correct."
        );
    }

    for (uint32_t count = 1; count <= nodeBoardCount; ++count) {
        // Search thru the midplane I/O links looking for matching node board
        for (vector<IOLink::ConstPtr>::iterator it = IOLinks.begin(); it != IOLinks.end(); ++it) {
            string IOLinkLocation = (*it)->getLocation();
            // Check if IO link (e.g. R00-M1-N08-J06) is available on the node board
            if (nodeBoardLocation == IOLinkLocation.substr(0,10)) {
                IOAvailable = true;
                break;
            }
        }
        // If found I/O then stop searching
        if (IOAvailable) {
            break;
        }
        // No I/O link found on node board so bump to next node board
        nextBoard++;
        nodeBoardLocation = nodeBoardLocationSubset;
        try {
            nodeBoardLocation += boost::lexical_cast<string>(nextBoard);
        } catch(const boost::bad_lexical_cast&) {
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidNodeBoards,
                    "Compute block not created, the starting node board location is not correct."
            );
        }
    }

    // Throw exception if no I/O links were found on node boards
    if (!IOAvailable) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, no I/O links found on node boards."
        );
    }
*/

    // Set the compute block description, midplane, starting node board position and node board count in the genblock params
    try {
        // Set default compute block description, can be overridden later
        genBlockParams.setDescription(Block_Description);
        // Extract midplane location from node board location (Rxx-Mx)
        genBlockParams.setMidplane(nodeBoardStartLocation.substr(0, MidplaneLength));
        // Set the node board starting position and node board count
        genBlockParams.setNodeBoardAndCount(nodeBoardStartLocation.substr(MidplaneLength+1, NodeBoardPositionLength), nodeBoardCount);
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, error is: " << e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, unexpected error calling GenBlockParams"
                );
    }

    // Convert from genblock format to compute block DB format
    try {
        genBlockParamsToBlockDatabaseInfoEx(genBlockParams, *blockDBInfoPtr);
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoards,
                "Compute block not created, unexpected error calling genBlockParamsToBlockDatabaseInfoEx"
        );
    }

    // Construct Block impl from the compute block database info
    Block::Pimpl blockPimpl(new Block::Impl(blockDBInfoPtr));
    return Ptr(new Block(blockPimpl));
}

void
Block::checkIO(
        const string& blockName,
        vector<string>* unconnectedIONodes,
        vector<string>* midplanesFailingIORules
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    vector<string> unconnectedAvailableIONodes;

    LOG_TRACE_MSG("Calling BGQDB::checkBlockIO() for compute block " << blockName);
    BGQDB::STATUS result = BGQDB::checkBlockIO(blockName, unconnectedIONodes, midplanesFailingIORules, &unconnectedAvailableIONodes);

    switch (result) {
        case BGQDB::OK:
            return;
        case BGQDB::NOT_FOUND:
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::BlockNotFound,
                    "Compute block " << blockName << " was not found"
            );
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
        default :
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }
    return;
}

bool
Block::isIOConnected(
        const string& blockName,
        vector<string>* unconnectedIONodes
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    LOG_TRACE_MSG("Calling BGQDB::checkBlockConnection() for compute block " << blockName);
    BGQDB::STATUS result = BGQDB::checkBlockConnection(blockName, unconnectedIONodes);

    switch (result) {
        case BGQDB::OK:
            // Check if any I/O nodes are not booted
            if (unconnectedIONodes->size() > 0) {
                // Dependent I/O is not booted so return the unconnected I/O nodes
                return false;
            }
            break;
        case BGQDB::NOT_FOUND:
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::BlockNotFound,
                    "Compute block " << blockName << " was not found"
            );
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
        default :
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    // If we got here all of the blocks connected (and available) I/O nodes are booted.
    return true;
}

vector<IOLink::ConstPtr>
Block::checkIOLinks(
        const string& blockName
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    vector<IOLink::ConstPtr> IOLinksVector;
    stringstream IOLinksXML;
    stringstream blockXML;
    LOG_TRACE_MSG("Calling BGQDB::checkIOLinks for compute block " << blockName);
    BGQDB::STATUS result = BGQDB::checkIOLinks(IOLinksXML, blockName);

    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            XMLEntity::dumpXML(IOLinksXML, "BlockIOLinks", false);
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
        case BGQDB::NOT_FOUND: // Either compute block or I/O links not found
            // Check if compute block exists
            if (BGQDB::NOT_FOUND == BGQDB::getBlockXML(blockXML, blockName)) {
                THROW_EXCEPTION(
                        bgsched::InputException,
                        bgsched::InputErrors::BlockNotFound,
                        "Compute block " << blockName << " was not found"
                );
            } else {
                // No I/O links available - return empty vector
                return IOLinksVector;
            }
        default :
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    IOLink::Pimpl IOLinkImplPtr;
    vector<const XMLEntity *> XMLEntityPtrVector;
    vector<const XMLEntity *> subEntities;
    // Parse the compute block I/O links XML file
    try {
        boost::scoped_ptr<XMLEntity> XMLEntityPtr(XMLEntity::readXML(IOLinksXML));
        XMLEntityPtrVector = XMLEntityPtr->subentities();
        // Read all I/O links from XML file to vector
        subEntities = XMLEntityPtrVector[0]->subentities();
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQIOWire") {
                // Get location for I/O link constructor
                string location((*iter)->attrByName("source"));
                // Construct the I/O link object from XML stream
                IOLinkImplPtr.reset(new IOLink::Impl(location,*iter));
                IOLink::Ptr IOLinkPtr(new IOLink(IOLinkImplPtr));
                // Add I/O link to the vector
                IOLinksVector.push_back(IOLinkPtr);
            } else {
                // Force XML dump to log
                bgsched::logXML(IOLinksXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQIOWire tag."
                );
            }
        }
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(IOLinksXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }

    return IOLinksVector;
}

vector<IOLink::ConstPtr>
Block::checkAvailableIOLinks(
        const string& blockName
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    vector<IOLink::ConstPtr> IOLinksVector;
    stringstream IOLinksXML;
    stringstream blockXML;
    LOG_TRACE_MSG("Calling BGQDB::checkIOLinks for compute block " << blockName);
    BGQDB::STATUS result = BGQDB::checkIOLinks(IOLinksXML, blockName);

    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            XMLEntity::dumpXML(IOLinksXML, "BlockIOLinks", false);
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
        case BGQDB::NOT_FOUND: // Either compute block or I/O links not found
            // Check if compute block exists
            if (BGQDB::NOT_FOUND == BGQDB::getBlockXML(blockXML, blockName)) {
                THROW_EXCEPTION(
                        bgsched::InputException,
                        bgsched::InputErrors::BlockNotFound,
                        "Compute block " << blockName << " was not found"
                );
            } else {
                // No I/O links available - return empty vector
                return IOLinksVector;
            }
        default :
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    IOLink::Pimpl IOLinkImplPtr;
    vector<const XMLEntity *> XMLEntityPtrVector;
    vector<const XMLEntity *> subEntities;
    // Parse the compute block I/O links XML file
    try {
        boost::scoped_ptr<XMLEntity> XMLEntityPtr(XMLEntity::readXML(IOLinksXML));
        XMLEntityPtrVector = XMLEntityPtr->subentities();
        // Read all I/O links from XML file to vector
        subEntities = XMLEntityPtrVector[0]->subentities();
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQIOWire") {
                // Get location for I/O link constructor
                string location((*iter)->attrByName("source"));
                // Construct the I/O link object from XML stream
                IOLinkImplPtr.reset(new IOLink::Impl(location,*iter));
                // Add the I/O link if both the I/O link hardware state and destination I/O node are available
                if ((IOLinkImplPtr->getState() == Hardware::Available) && (IOLinkImplPtr->getIONodeState() == Hardware::Available)) {
                    IOLink::Ptr IOLinkPtr(new IOLink(IOLinkImplPtr));
                    // Add I/O link to the vector
                    IOLinksVector.push_back(IOLinkPtr);
                }
            } else {
                // Force XML dump to log
                bgsched::logXML(IOLinksXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQIOWire tag."
                );
            }
        }
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(IOLinksXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }

    return IOLinksVector;
}

void
Block::checkIOLinksSummary(
        const string& blockName,
        uint32_t *availableIOLinks,
        uint32_t *unavailableIOLinks
)
{
    *availableIOLinks = 0;
    *unavailableIOLinks = 0;

    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    stringstream IOLinksXML;
    stringstream blockXML;
    LOG_TRACE_MSG("Calling BGQDB::checkIOLinks() for compute block " << blockName);
    BGQDB::STATUS result = BGQDB::checkIOLinks(IOLinksXML, blockName);

    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            XMLEntity::dumpXML(IOLinksXML, "BlockIOLinks", false);
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
        case BGQDB::NOT_FOUND: // Either compute block or I/O links not found
            // Check if compute block exists
            if (BGQDB::NOT_FOUND == BGQDB::getBlockXML(blockXML, blockName)) {
                THROW_EXCEPTION(
                        bgsched::InputException,
                        bgsched::InputErrors::BlockNotFound,
                        "Compute block " << blockName << " was not found"
                );
            } else {
                // No I/O links available
                *availableIOLinks = 0;
                *unavailableIOLinks = 0;
                return;
            }
        default :
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    vector<const XMLEntity *> XMLEntityPtrVector;
    vector<const XMLEntity *> subEntities;
    // Parse the compute block I/O links XML file
    try {
        boost::scoped_ptr<XMLEntity> XMLEntityPtr(XMLEntity::readXML(IOLinksXML));
        XMLEntityPtrVector = XMLEntityPtr->subentities();
        // Read all I/O links from XML file to vector
        subEntities = XMLEntityPtrVector[0]->subentities();
        uint32_t available = 0;
        uint32_t unavailable = 0;
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQIOWire") {
                if ((strcmp((*iter)->attrByName("status"), "A") == 0) &&
                    (strcmp((*iter)->attrByName("IONstatus"), "A") == 0))
                {
                    ++available;
                } else {
                    ++unavailable;
                }
            } else {
                // Force XML dump to log
                bgsched::logXML(IOLinksXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQIOWire tag."
                );
            }
        }
        *availableIOLinks = available;
        *unavailableIOLinks = unavailable;
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(IOLinksXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }
}

void
Block::initiateBoot(
        const string& blockName
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMCNODES);
    dbo.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbo._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbo);

    // Get current uid for compute block user
    string user("root");
    try {
        bgq::utility::UserId uid;
        user = uid.getUser();
        LOG_DEBUG_MSG("Set compute block user to " << user);
    } catch (const runtime_error& e) {
        LOG_WARN_MSG(e.what());
        LOG_WARN_MSG("Using compute block user " << user);
    }

    // Set block action to Configure:
    // - Will fail if the compute block is not free or the hardware isn't free.
    deque<string> options;
    options.push_back(string("user=") + user);

    // Serialize compute block allocate requests
    pthread_mutex_lock(&blockaction_lock);
    BGQDB::STATUS result = BGQDB::setBlockAction(blockName, BGQDB::CONFIGURE_BLOCK, options);
    pthread_mutex_unlock(&blockaction_lock);

    switch (result) {
    case BGQDB::OK:
        LOG_DEBUG_MSG("Initiating boot for compute block " << blockName);
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
    case BGQDB::INVALID_ID: // Compute block name not correct
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                "Compute block " << blockName << " is not valid"
        );
    case BGQDB::NOT_FOUND: // Compute block not found
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    case BGQDB::DUPLICATE: // Compute block already has action pending
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockBootError,
                "Compute block boot request failed because compute block " << blockName << " has pending action"
        );
    case BGQDB::FAILED: // Compute block is not Free or hardware unavailable
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockBootError,
                "Compute block boot request failed because either compute block " << blockName << " is not Free or dependent hardware resources are in use"
        );
    default :
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Return_Code_Str
        );
    }
}

void
Block::initiateBoot(
        const string& blockName,
        vector<string>* unavailableResources
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMCNODES);
    dbo.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbo._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbo);

    // Check the required compute block resources in the database
    LOG_TRACE_MSG("Calling BGQDB::queryMissing() for compute block " << blockName);
    BGQDB::STATUS result = BGQDB::queryMissing(blockName, *unavailableResources, BGQDB::NO_DIAGS);
    switch (result) {
        case BGQDB::OK:
            // Stop boot if required resources are missing
            if (unavailableResources->size() > 0) {
                THROW_EXCEPTION(
                        bgsched::RuntimeException,
                        bgsched::RuntimeErrors::BlockBootError,
                        "Boot request failed because compute block " << blockName << " dependent hardware resources are unavailable."
                );
            }
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
        case BGQDB::NOT_FOUND:
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::BlockNotFound,
                    "Compute block " << blockName << " was not found."
            );
        default:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    // Hardware pre-check was successful or hardware is good. Let any exceptions bubble up the stack.
    Block::initiateBoot(blockName);
}

void
Block::initiateFree(
        const string& blockName
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMCNODES);
    dbo.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbo._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbo);

    BGQDB::BLOCK_STATUS state;

    // create useful error message by gathering the program name from the first token of /proc/self/cmdline
    std::string errmsg( "errmsg=Deallocated by Scheduler API ");
    try {
        const std::string path( std::string("/proc/self/cmdline") );
        std::ifstream proc( path.c_str(), std::ifstream::in );
        if ( !proc ) {
            char buf[256];
            LOG_WARN_MSG( "Could not open " << path << ": " << strerror_r(errno, buf, sizeof(buf)) );
        } else {
            char c;
            std::string line;
            while ( proc.get(c) ) {
                if ( c == '\0' ) break;
                line += c;
            }
            if ( !proc ) {
                char buf[256];
                LOG_WARN_MSG( "Could not read a line from  " << path << ": " << strerror_r(errno, buf, sizeof(buf)) );
            } else {
                errmsg += line;
                errmsg += " ";
            }
        }
    } catch ( const std::exception& e ) {
        LOG_DEBUG_MSG( e.what() );
    }
    errmsg += "with pid " + boost::lexical_cast<std::string>( getpid() );

    // Serialize compute block deallocate requests
    pthread_mutex_lock(&blockaction_lock);
    BGQDB::STATUS result = BGQDB::setBlockAction(blockName, BGQDB::DEALLOCATE_BLOCK, std::deque<std::string>(1, errmsg));
    pthread_mutex_unlock(&blockaction_lock);
    switch (result) {
    case BGQDB::OK:
        LOG_DEBUG_MSG("Initiating free request for compute block " << blockName);
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
    case BGQDB::INVALID_ID: // Compute block name not correct
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                "Compute block " << blockName << " is not valid"
        );
    case BGQDB::NOT_FOUND: // Compute block not found
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    case BGQDB::DUPLICATE:
        if (BGQDB::OK == BGQDB::getBlockStatus(blockName, state)) {
            if (state == BGQDB::FREE) {
                LOG_WARN_MSG("Compute block free request ignored, " << blockName << " is already Free");
                return;
            }
        }
        THROW_EXCEPTION( // Compute block already has action pending
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockFreeError,
                "Compute block free request failed because compute block " << blockName << " has pending action"
        );
    case BGQDB::FAILED:
        if (BGQDB::OK == BGQDB::getBlockStatus(blockName, state)) {
            if (state == BGQDB::FREE) {
                LOG_WARN_MSG("Compute block free request ignored, " << blockName << " is already Free");
                return;
            }
        }
        THROW_EXCEPTION( // Compute block Free request is invalid or wrong status
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockFreeError,
                "Compute block free request failed because compute block " << blockName << " received invalid block action request"
        );
    default :
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Return_Code_Str
        );
    }
}

void
Block::remove(
        const string& blockName
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMCNODES);
    dbo.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbo._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbo);

    LOG_DEBUG_MSG("Attempting to remove compute block " << blockName);
    BGQDB::STATUS result = BGQDB::deleteBlock(blockName);
    switch (result) {
        case BGQDB::OK:
            LOG_DEBUG_MSG("Compute block " << blockName << " was removed");
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
                    );
        case BGQDB::FAILED:  // Compute block can only be deleted if (F)ree
            THROW_EXCEPTION(
                    bgsched::RuntimeException,
                    bgsched::RuntimeErrors::InvalidBlockState,
                    "Compute block " << blockName << " must be in Free state to be removed."
                    );
        case BGQDB::INVALID_ID:
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidBlockName,
                    Block_Name_Too_Long_Str
                    );
        case BGQDB::NOT_FOUND: // Compute block not found
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::BlockNotFound,
                    "Compute block " << blockName << " was not found"
                    );
        case BGQDB::CONNECTION_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::ConnectionError,
                    DB_Connection_Error_Str
                    );
        default :
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
                    );
    }
}

void
Block::addUser(
        const string& blockName,
        const string& user
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    DBTBlocksecurity dbo;
    // Verify user name was specified
    if (user.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidUser,
                No_User_Name_Str
                );
    }

    // Validate the user name size
    if (user.size() >= sizeof(dbo._authid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidUser,
                "User name " << user << " is too long."
                );
    }

    // If already authorized to compute block just return
    try {
        if (Block::isAuthorized(blockName, user)) {
            LOG_WARN_MSG("No action taken on add user request, user " << user << " is already authorized to compute block " << blockName);
            return;
        }
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                "Error adding user " << user << " to compute block " << blockName
        );
    }

    // Get runtime user and grant read and execute authority to the compute block for the user.
    // Read authority is not needed to run jobs but allows job schedulers to return compute block info
    // back to authorized users when using getBlocks() with user argument.
    try {
        bgq::utility::UserId uid;
        hlcs::security::Object blockObject(hlcs::security::Object::Block, blockName);
        hlcs::security::Authority readAuthority(user, hlcs::security::Action::Read);
        hlcs::security::Authority executeAuthority(user, hlcs::security::Action::Execute);
        hlcs::security::grant(blockObject, readAuthority, uid);
        hlcs::security::grant(blockObject, executeAuthority, uid);
        LOG_DEBUG_MSG("Successfully added user " << user << " to compute block " << blockName);
    } catch(const runtime_error& e) {
        LOG_ERROR_MSG("Error adding user " << user << " to compute block " << blockName);
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                "Error adding user " << user << " to compute block " << blockName
        );
    }
}

void
Block::removeUser(
        const string& blockName,
        const string& user
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    DBTBlocksecurity dbo;
    // Verify user name was specified
    if (user.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidUser,
                No_User_Name_Str
                );
    }

    // Validate the user name size
    if (user.size() >= sizeof(dbo._authid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidUser,
                "User name " << user << " is too long."
                );
    }

    // If not already authorized to compute block just return
    try {
        if (Block::isAuthorized(blockName, user) == false) {
            LOG_WARN_MSG("No action taken on remove user request, user " << user << " was not previously authorized to compute block " << blockName);
            return;
        }
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                "Error removing user " << user << " from compute block " << blockName
        );
    }

    // Get runtime user and revoke read and execute authority on the compute block for the user
    try {
        bgq::utility::UserId uid;
        hlcs::security::Object blockObject(hlcs::security::Object::Block, blockName);
        hlcs::security::Authority readAuthority(user, hlcs::security::Action::Read);
        hlcs::security::Authority executeAuthority(user, hlcs::security::Action::Execute);
        hlcs::security::revoke(blockObject, readAuthority, uid);
        hlcs::security::revoke(blockObject, executeAuthority, uid);
        LOG_DEBUG_MSG("Successfully removed user " << user << " from compute block " << blockName);
    } catch(const runtime_error& e) {
        LOG_ERROR_MSG("Error removing user " << user << " from compute block " << blockName);
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                "Error removing user " << user << " from compute block " << blockName
        );
    }
}

bool
Block::isAuthorized(
        const string& blockName,
        const string& user
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    DBTBlocksecurity dbo;
    // Verify user name was specified
    if (user.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidUser,
                No_User_Name_Str
                );
    }

    // Validate the user name size
    if (user.size() >= sizeof(dbo._authid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidUser,
                "User name " << user << " is too long."
                );
    }

    // Get list of users with "execute" permissions to block
    try {
        vector<string> users(Block::getUsers(blockName));
        if (users.size() == 0) {
            return false;
        } else {
            for (unsigned int i = 0; i < users.size(); i++) {
                // Users match?
                if (user.compare(users[i]) == 0) {
                    return true;
                }
            }
        }
        return false;
    } catch(const bgsched::RuntimeException& e) {
        LOG_ERROR_MSG("Error getting user authorities for compute block " << blockName);
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                e.what()
        );
    }
}

vector<string>
Block::getUsers(
        const string& blockName
)
{
    DBTBlock dbblock;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify compute block name was specified
    if (blockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the compute block name length
    if (blockName.length() >= sizeof(dbblock._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Get database connection
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }
    colBitmap.set(dbblock.BLOCKID);
    colBitmap.set(dbblock.NUMCNODES);
    dbblock.setColumns(colBitmap);

    // Validate the compute block exists in the database
    snprintf(dbblock._blockid, sizeof(dbblock._blockid), "%s", blockName.c_str());

    sqlrc = tx.queryByKey(&dbblock);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    sqlrc = tx.fetch(&dbblock);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Validate that block is a compute block
    if (dbblock._numcnodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << blockName << " was not found"
        );
    }

    // Done checking compute block so close
    tx.close(&dbblock);

    vector<string> users;

    // Get the user permissions for the block
    try {
        hlcs::security::Object blockObject(hlcs::security::Object::Block, blockName);
        // Get container of authorities for the compute block (excluding bg.properties settings)
        hlcs::security::Authorities authorities(hlcs::security::list(blockObject));
        hlcs::security::Authority::Container authorityContainer = authorities.get();

        // Get the list of users and authorities
        for (hlcs::security::Authority::Container::iterator it = authorityContainer.begin(); it != authorityContainer.end(); ++it) {
            // Only interested in users who have been granted "Execute" permissions
            if ((it->source() == hlcs::security::Authority::Source::Granted) && (it->action() == hlcs::security::Action::Execute)){
                // Push the user to the vector
                users.push_back(it->user());
            }
        }
        // Return the user vector
        return users;
    } catch(const runtime_error& e) {
        LOG_ERROR_MSG("Error getting user authorities for compute block " << blockName << ". Error is: " << e.what());
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                e.what()
        );
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                "Error getting user authorities for compute block " << blockName
        );
    }
}

void
Block::update()
{
    LOG_DEBUG_MSG("Attempting to update compute block " << getName());

    // Modify block options, boot options, micro-loader image, and Node configuration.
    // Requires compute block to be in Free, Terminating or Allocated state.
    BGQDB::BlockInfo info;

    string options = getOptions();
    string bootOptions = getBootOptions();
    string microLoaderImage = getMicroLoaderImage();
    string nodeConfig = getNodeConfiguration();

    strcpy(info.options, options.c_str());
    strcpy(info.bootOptions, bootOptions.c_str());
    strcpy(info.uloaderImg, microLoaderImage.c_str());
    strcpy(info.nodeConfig, nodeConfig.c_str());

    BGQDB::STATUS result = BGQDB::setBlockInfo(getName(), info);
    switch (result) {
    case BGQDB::OK:
        LOG_DEBUG_MSG("Compute block " << getName() << " options, boot options, micro-loader image and Node configuration updated");
        break;
    case BGQDB::DB_ERROR:
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str_NodeConfig
        );
    case BGQDB::NOT_FOUND:
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << getName() << " was either not found or has a status that does not allow updates."
        );
    case BGQDB::INVALID_ID:
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
        );
    case BGQDB::INVALID_ARG:
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockInfo,
                Block_Info_Too_Long_Str
        );
    case BGQDB::CONNECTION_ERROR:
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    default :
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Return_Code_Str
        );
    }

    // Modify the description (doesn't require compute block to be Free)
    result = BGQDB::setBlockDesc(getName(), getDescription());
    switch (result) {
    case BGQDB::OK:
        LOG_DEBUG_MSG("Compute block " << getName() << " description updated");
        break;
    case BGQDB::DB_ERROR:
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    case BGQDB::INVALID_ID:
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
        );
    case BGQDB::INVALID_ARG:
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockDescription,
                Block_Description_Too_Long_Str
        );
    case BGQDB::NOT_FOUND: // Compute block not found
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "Compute block " << getName() << " was not found"
        );
    case BGQDB::CONNECTION_ERROR:
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    default :
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Return_Code_Str
        );
    }
}

void
Block::add(
        const string& owner
)
{
    _impl->add(owner, false);
}

uint32_t
Block::getDimensionSize(
        const Dimension dimension
) const
{
    return _impl->getDimensionSize(dimension);
}

bool
Block::isTorus(
        const Dimension dimension
) const
{
    return _impl->isTorus(dimension);
}

bool
Block::isMesh(
        const Dimension dimension
) const
{
    return _impl->isMesh(dimension);
}

bool
Block::isLarge() const
{
    return _impl->isLarge();
}

bool
Block::isSmall() const
{
    return _impl->isSmall();
}

const string&
Block::getName() const
{
    return _impl->getName();
}

BlockCreationId
Block::getCreationId() const
{
    return _impl->getCreationId();
}

const string&
Block::getDescription() const
{
    return _impl->getDescription();
}

const string&
Block::getOwner() const
{
    return _impl->getOwner();
}

const string&
Block::getUser() const
{
    return _impl->getUser();
}

const string&
Block::getBootOptions() const
{
    return _impl->getBootOptions();
}

const string&
Block::getMicroLoaderImage() const
{
    return _impl->getMicroLoaderImage();
}

const string&
Block::getNodeConfiguration() const
{
    return _impl->getNodeConfiguration();
}

SequenceId
Block::getSequenceId() const
{
    return _impl->getSequenceId();
}

EnumWrapper<Block::Status>
Block::getStatus() const
{
    return _impl->getStatus();
}

uint32_t
Block::getComputeNodeCount() const
{
    return _impl->getComputeNodeCount();
}

const string&
Block::getOptions() const
{
    return _impl->getOptions();
}

vector<Job::Id>
Block::getJobIds() const
{
    return _impl->getJobIds();
}

Block::Midplanes
Block::getMidplanes() const
{
    Midplanes ret;

    const Midplanes &midplanes(_impl->getMidplanes());

    for (Midplanes::const_iterator i(midplanes.begin()); i != midplanes.end(); ++i) {
        ret.push_back(*i);
    }
    return ret;
}

Block::NodeBoards
Block::getNodeBoards() const
{
    NodeBoards ret;

    const NodeBoards &nodeboards(_impl->getNodeBoards());

    for (NodeBoards::const_iterator i(nodeboards.begin()); i != nodeboards.end(); ++i) {
        ret.push_back(*i);
    }
    return ret;
}

Block::PassthroughMidplanes
Block::getPassthroughMidplanes() const
{
    PassthroughMidplanes ret;

    const PassthroughMidplanes &passthroughMidplanes(_impl->getPassthroughMidplanes());

    for (PassthroughMidplanes::const_iterator i(passthroughMidplanes.begin()); i != passthroughMidplanes.end(); ++i) {
        ret.push_back(*i);
    }
    return ret;
}

void
Block::setName(
        const string& name
)
{
    return _impl->setName(name);
}

void
Block::setDescription(
        const string& description
)
{
    return _impl->setDescription(description);
}

void
Block::setBootOptions(
        const string& bootOptions
)
{
    return _impl->setBootOptions(bootOptions);
}

void
Block::setMicroLoaderImage(
        const string& image
)
{
    return _impl->setMicroLoaderImage(image);
}

void
Block::setNodeConfiguration(
        const string& nodeConfig
)
{
    return _impl->setNodeConfiguration(nodeConfig);
}

void
Block::setOptions(
        const string& options
)
{
    return _impl->setOptions(options);
}

string
Block::toString(
        bool verbose
) const
{
    ostringstream os;
    _impl->toString(os, verbose);
    return os.str();
}

EnumWrapper<Block::Action::Value>
Block::getAction() const
{
    return _impl->getAction();
}

} // namespace bgsched
