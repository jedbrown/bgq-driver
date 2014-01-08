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

#include <bgsched/IOBlock.h>
#include <bgsched/DatabaseException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/RuntimeException.h>

#include <bgsched/core/core.h>

#include "bgsched/IOBlockImpl.h"
#include "bgsched/utility.h"

#include "libmmcs_client/CommandReply.h"
#include "libmmcs_client/ConsolePort.h"
#include "libmmcs_client/utility.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/dataconv.h>
#include <db/include/api/Exception.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>
#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include <pthread.h>
#include <sstream>
#include <string>
#include <vector>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

// Statically initialize "fast" mutex
pthread_mutex_t IOBlockaction_lock = PTHREAD_MUTEX_INITIALIZER;

namespace {
    // Default I/O block description
    const string Block_Description("Created by IBM Scheduler API");

    // Message strings
    const string No_Block_Name_Str("I/O block name is empty.");
    const string Block_Name_Too_Long_Str("I/O block name is too long.");
    const string DB_Access_Error_Str("Error occurred while accessing database.");
    const string DB_Access_Error_Str_NodeConfig("Error occurred while updating database. Possible cause is node configuration does not currently exist in the database.");
    const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
    const string Unexpected_Exception_Str("Received unexpected exception.");
    const string Unexpected_Return_Code_Str("Unexpected return code from internal database function.");
    const string Block_Description_Too_Long_Str("I/O block description is too long.");
    const string Block_Options_Too_Long_Str("I/O block options are too long.");
    const string Block_Info_Too_Long_Str("Boot options, Node configuration or micro-loader image is too long.");
    const string Start_Location_Invalid_Arg_Str("I/O block not created, I/O node start location argument is not valid.");
} // anonymous namespace

namespace bgsched {

IOBlock::IOBlock(
        Pimpl impl
        ) :
    _impl(impl)
{
    // Nothing to do
}

IOBlock::Pimpl
IOBlock::getPimpl() const
{
    return _impl;
}

//  The following accepts an I/O block name and returns the connected compute blocks that are booted ("B", "I' or "T").
const IOBlock::ConnectedComputeBlocks
IOBlock::getConnectedComputeBlocks(
        const string& IOBlockName
)
{
    IOBlock::ConnectedComputeBlocks computeBlocks;
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;

    // Verify I/O block name was specified
    if (IOBlockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the I/O block name length
    if (IOBlockName.length() >= sizeof(dbo._blockid)) {
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

    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    SQLLEN ind1;

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.STATUS);
    dbo.setColumns(colBitmap);

    // Validate the I/O block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", IOBlockName.c_str());

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
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // Validate that block is an I/O block
    if (dbo._numionodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // If I/O block is Free then return empty compute block container
    if (strcmp(dbo._status, BGQDB::BLOCK_FREE) == 0) {
        return computeBlocks;
    }

    // Done checking I/O block so close
    tx.close(&dbo);

    // Query for compute blocks using the I/O block
    string sqlstr;
    sqlstr = "select c.blockid  from tbgqioblockmap a, bgqcniolink b, bgqsmallblock c, bgqblock d    where  a.blockid = '" + IOBlockName +
        string("' and (ion = location  or substr(ion,1,6) = location) and ") +
        string("  substr(source,1,10) = posinmachine || '-' || nodecardpos  and c.blockid = d.blockid and d.status in ( 'B','I','T') and b.status = 'A' ") +
        string("  union select c.blockid  from tbgqioblockmap a, bgqcniolink b, bgqbpblockmap c, bgqblock d  where  a.blockid = '") + IOBlockName +
        string("' and (ion = location  or substr(ion,1,6) = location) and substr(source,1,6) = bpid  and c.blockid = d.blockid and d.status in ('B','I','T') and b.status = 'A' ");
    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    char cnBlock[33];

    // Bind compute block column column
    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, cnBlock, sizeof(cnBlock), &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    // Collect all the compute blocks using the I/O block
    sqlrc =  SQLFetch(hstmt);
    for(;sqlrc == SQL_SUCCESS;) {
        BGQDB::trim_right_spaces(cnBlock);
        computeBlocks.push_back(string(cnBlock));
        sqlrc = SQLFetch(hstmt);
    }

    SQLCloseCursor(hstmt);
    return computeBlocks;
}


IOBlock::Ptr
IOBlock::create(
        const string& IOBlockName,
        const string& startIOLocation,
        uint32_t IONodeCount,
        const string& description
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::DBTIoblockmap iob;
    BGQDB::DBVIodrawer iod;
    BGQDB::ColumnsBitmap colBitmap;

    // Verify I/O block name was specified
    if (IOBlockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }

    // Validate the I/O block name length
    if (IOBlockName.length() >= sizeof(dbo._blockid)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                Block_Name_Too_Long_Str
                );
    }

    // Validate rest of I/O block name
    if (!BGQDB::isBlockIdValid(IOBlockName, __FUNCTION__)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                "I/O block name " << IOBlockName << " is not valid"
        );
    }

    // Validate the description length
    if (description.length() >= sizeof(dbo._description)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockDescription,
                Block_Description_Too_Long_Str
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

    SQLRETURN sqlrc;
    sqlrc = tx.setAutoCommit(false);
    if (sqlrc != SQL_SUCCESS) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                "Database auto commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__
        );
    }

    // Rules: The number of I/O nodes must be a multiple of 8 when specifying an I/O drawer starting location. The number of
    // I/O nodes must be 1, 2, or 4 when specifying an I/O node starting location. For two I/O nodes, the I/O node location
    // must be J00, J02, J04, or J06. For four nodes, the I/O node location must be J00 or J04.
    if (IONodeCount == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidIONodeCount,
                "Number of I/O nodes must be 1 or larger"
        );
    }

    if (IONodeCount == 3 || IONodeCount == 5 || IONodeCount == 6 || IONodeCount == 7) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidIONodeCount,
                "Value of " << IONodeCount << " is not valid for number of I/O nodes"
        );
    }

    if (startIOLocation.length() == 6 && IONodeCount%8 != 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidIONodeCount,
                "Number of I/O nodes must be a multiple of 8 when specifying an I/O drawer"
        );
    }

    if (startIOLocation.length() != 10 && startIOLocation.length() != 6) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Starting I/O location " << startIOLocation << " is not an I/O node or I/O drawer location"
        );
    }

    if (startIOLocation.length() == 10 && IONodeCount > 7) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Starting I/O location must be an I/O drawer when specifying 8 or more I/O nodes"
        );
    }

    if ((IONodeCount == 2 && startIOLocation.substr(7,3) != "J00" && startIOLocation.substr(7,3) != "J02" &&
         startIOLocation.substr(7,3) != "J04" && startIOLocation.substr(7,3) != "J06") || (IONodeCount == 4 &&
         startIOLocation.substr(7,3) != "J00" && startIOLocation.substr(7,3) != "J04")) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Invalid starting I/O location for number of I/O nodes"
        );
    }

    // Validate the starting I/O location, the previous checks only validated the length
    string whereClause(" where location='");
    whereClause += startIOLocation +string("'");
    int nrows;
    if (startIOLocation.length() == 6) {
        nrows = tx.count("BGQIODrawer", whereClause.c_str());
    } else {
        nrows = tx.count("BGQIONode", whereClause.c_str());
    }
    if (nrows != 1) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Starting I/O location " << startIOLocation << " is not a valid I/O drawer or I/O node"
        );
    }

    colBitmap.reset();
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.OWNER);
    colBitmap.set(dbo.NUMCNODES);
    colBitmap.set(dbo.DESCRIPTION);
    colBitmap.set(dbo.MLOADERIMG);
    colBitmap.set(dbo.NODECONFIG);
    dbo.setColumns(colBitmap);

    // Set block name
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", IOBlockName.c_str());

    // Set current uid for I/O block owner
    string owner("root");
    try {
        bgq::utility::UserId uid;
        owner = uid.getUser();
        LOG_DEBUG_MSG("Setting I/O block owner to " << owner);
    } catch (const runtime_error& e) {
        LOG_WARN_MSG(e.what());
        LOG_WARN_MSG("Could not get user id of caller, setting I/O block owner to " << owner);
    }
    snprintf(dbo._owner, sizeof(dbo._owner), "%s", owner.c_str());

    // Set I/O block description, use default value if empty
    if (description.empty()) {
        snprintf(dbo._description, sizeof(dbo._description), "%s", "Created by IBM Scheduler API");
    } else {
        snprintf(dbo._description, sizeof(dbo._description), "%s", description.c_str());
    }

    // Set the default microloader and node configuration values, these can be changed later
    snprintf(dbo._mloaderimg, sizeof(dbo._mloaderimg), "%s", BGQDB::DEFAULT_MLOADERIMG.c_str());
    snprintf(dbo._nodeconfig, sizeof(dbo._nodeconfig), "%s", BGQDB::DEFAULT_IONODECONFIG.c_str());

    // This is an I/O block so number of compute nodes is zero and I/O nodes is set based on argument value
    dbo._numcnodes = 0;
    dbo._numionodes = IONodeCount;

    // Insert the I/O block record, could fail if it is a duplicate or for other database errors
    sqlrc = tx.insert(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                "Database I/O block insert failed with SQL error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__
        );
    }

    // Block table was updated so now modify the I/O block mapping table
    colBitmap.reset();
    colBitmap.set(iob.BLOCKID);
    colBitmap.set(iob.LOCATION);
    iob.setColumns(colBitmap);

    // Set the I/O block name and starting I/O location
    snprintf(iob._blockid, sizeof(iob._blockid), "%s", IOBlockName.c_str());
    snprintf(iob._location, sizeof(iob._location), "%s", startIOLocation.c_str());

    // Handle the spanning of I/O racks and tophats
    colBitmap.reset();
    colBitmap.set(iod.LOCATION);
    iod.setColumns(colBitmap);

    int position = 0;
    for (uint32_t IONodesProcessed = 0; IONodesProcessed < IONodeCount;) {
        sqlrc = tx.insert(&iob);
        if (sqlrc != SQL_SUCCESS) {
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    "Database I/O block map insert failed with SQL error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__
            );
        }

        // Check if starting location is an I/O drawer
        if (startIOLocation.length() == 6) {
            // Each I/O drawer contains 8 I/O nodes
            IONodesProcessed += 8;

            // Move to the next I/O drawer
            if (IONodesProcessed < IONodeCount) {
                sqlrc =  tx.query(&iod,(string(" where location >'") + string(iob._location) + "' order by location").c_str());
                // Check if we have more I/O drawers on the system or whether we exceeded I/O capacity
                if ((sqlrc = tx.fetch(&iod)) != SQL_SUCCESS) {
                    // Exceeded system I/O capacity
                    THROW_EXCEPTION(
                            bgsched::InputException,
                            bgsched::InputErrors::InvalidIONodeCount,
                            "Number of I/O nodes specified spans beyond system capacity when starting from I/O drawer " << startIOLocation
                    );
                } else {
                    // Set the next I/O location that the I/O block uses
                    snprintf(iob._location, sizeof(iob._location), "%s", iod._location);
                    tx.close(&iod);
                }
            }
        } else {
            // Starting location is an I/O node so increment processed I/O nodes by one
            IONodesProcessed += 1;
            try {
                position = boost::lexical_cast<int>(&iob._location[8]);
            } catch (const boost::bad_lexical_cast& e) {
                LOG_ERROR_MSG(e.what());
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::UnexpectedError,
                        "Unexpected error with I/O node location string in database"
                );
            }
            sprintf(&iob._location[8], "%02d", ++position);
        }
    }

    // Now commit the transaction
    if ((sqlrc = tx.commit()) != SQL_SUCCESS) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                "Unable to commit database transaction, SQL error is: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__
        );
    }

    // The database transaction has been completed, now get the I/O block back
    IOBlock::Ptr IOBlockPtr;
    IOBlock::Ptrs IOBlockVector; // Vector of I/O blocks returned by core::getIOBlocks()
    IOBlockFilter IOblockFilter;
    core::IOBlockSort IOblockSort;
    try {
        // Filter by name and return extended info (e.g. I/O locations)
        IOblockFilter.setName(IOBlockName);
        IOblockFilter.setExtendedInfo(true);
        IOBlockVector = core::getIOBlocks(IOblockFilter, IOblockSort);
    } catch (...) { // Handle all exceptions
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockRetrievalError,
                "I/O block " << IOBlockName << " successfully created but unable to retrieve from database"
        );
    }

    // Make sure we got the I/O block back
    if (IOBlockVector.empty()) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockRetrievalError,
                "I/O block " << IOBlockName << " successfully created but unable to retrieve from database"
        );
    }

    // Now set the I/O block pointer that will be returned to caller
    IOBlock::Ptrs::iterator iter = IOBlockVector.begin();
    IOBlockPtr = *iter;

    return IOBlockPtr;
}

void
IOBlock::initiateBoot(
        const std::string& IOBlockName,
        bool allowHoles,
        vector<string>* unavailableResources,
        vector<string>* unavailableIONodes
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify I/O block name was specified
    if (IOBlockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the I/O block name length
    if (IOBlockName.length() >= sizeof(dbo._blockid)) {
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
    colBitmap.set(dbo.NUMIONODES);
    dbo.setColumns(colBitmap);

    // Validate the I/O block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", IOBlockName.c_str());

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
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // Validate that block is an I/O block
    if (dbo._numionodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // Done checking I/O block so close
    tx.close(&dbo);

    BGQDB::STATUS result;
    // Check if allowing booting of I/O block when some I/O resources (drawers/nodes) are unavailable
    if (allowHoles) {
        // Don't care if there are some I/O resources unavailable
        LOG_TRACE_MSG("Calling BGQDB::queryError() for I/O block " << IOBlockName);
        result = BGQDB::queryError(IOBlockName, *unavailableIONodes);
        switch (result) {
            case BGQDB::OK:
                 // Check if any I/O resources are unavailable
                 if (unavailableIONodes->size() > 0) {
                     // Log warning message but continue on with boot
                     LOG_WARN_MSG("Found " << unavailableIONodes->size() << " unavailable I/O resources for I/O block " << IOBlockName);
                 }
                break;
            case BGQDB::DB_ERROR:
                THROW_EXCEPTION(
                        bgsched::DatabaseException,
                        bgsched::DatabaseErrors::DatabaseError,
                        DB_Access_Error_Str
                );
            case BGQDB::INVALID_ARG:
                THROW_EXCEPTION(
                        bgsched::InputException,
                        bgsched::InputErrors::BlockNotFound,
                        "I/O block " << IOBlockName << " was not found or is not an I/O block."
                );
            case BGQDB::CONNECTION_ERROR:
                THROW_EXCEPTION(
                        bgsched::DatabaseException,
                        bgsched::DatabaseErrors::ConnectionError,
                        DB_Connection_Error_Str
                );
            default:
                THROW_EXCEPTION(
                        bgsched::DatabaseException,
                        bgsched::DatabaseErrors::UnexpectedError,
                        Unexpected_Return_Code_Str
                );
        }
    } else {
        // Check the required I/O block resources in the database
        LOG_TRACE_MSG("Calling BGQDB::queryMissing() for I/O block " << IOBlockName);
        result = BGQDB::queryMissing(IOBlockName, *unavailableResources, BGQDB::NO_DIAGS);
        switch (result) {
            case BGQDB::OK:
                // Stop boot if required resources are missing
                if (unavailableResources->size() > 0) {
                    THROW_EXCEPTION(
                            bgsched::RuntimeException,
                            bgsched::RuntimeErrors::BlockBootError,
                            "I/O block boot request failed because I/O block " << IOBlockName << " dependent hardware resources are unavailable."
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
                        "I/O block " << IOBlockName << " was not found."
                );
            default:
                THROW_EXCEPTION(
                        bgsched::DatabaseException,
                        bgsched::DatabaseErrors::UnexpectedError,
                        Unexpected_Return_Code_Str
                );
        }
    }

    // Get current uid for I/O block user
    string user("root");
    try {
        bgq::utility::UserId uid;
        user = uid.getUser();
        LOG_DEBUG_MSG("Set I/O block user to " << user);
    } catch (const runtime_error& e) {
        LOG_WARN_MSG(e.what());
        LOG_WARN_MSG("Using I/O block user " << user);
    }

    // Set block action to Configure:
    // - Will fail if the I/O block is not free or the hardware isn't free.
    deque<string> options;
    options.push_back(string("user=") + user);

    // Serialize I/O block allocate requests
    pthread_mutex_lock(&IOBlockaction_lock);
    // Set block action based on whether allowing I/O blocks with holes or not
    if (allowHoles) {
        result = BGQDB::setBlockAction(IOBlockName, BGQDB::CONFIGURE_BLOCK_NO_CHECK, options);
    } else {
        result = BGQDB::setBlockAction(IOBlockName, BGQDB::CONFIGURE_BLOCK, options);
    }
    pthread_mutex_unlock(&IOBlockaction_lock);

    switch (result) {
    case BGQDB::OK:
        LOG_DEBUG_MSG("Initiating boot for I/O block " << IOBlockName);
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
    case BGQDB::INVALID_ID: // I/O block name not correct
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                "I/O block " << IOBlockName << " is not valid."
        );
    case BGQDB::NOT_FOUND: // I/O block not found
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "I/O block " << IOBlockName << " was not found."
        );
    case BGQDB::DUPLICATE: // I/O block already has action pending
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockBootError,
                "I/O block boot request failed because I/O block " << IOBlockName << " has pending action."
        );
    case BGQDB::FAILED: // I/O block is not Free or hardware unavailable
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockBootError,
                "I/O block boot request failed because either I/O block " << IOBlockName << " is not Free or dependent hardware resources are unavailable."
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
IOBlock::initiateFree(
        const string& IOBlockName
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify I/O block name was specified
    if (IOBlockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the I/O block name length
    if (IOBlockName.length() >= sizeof(dbo._blockid)) {
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
    colBitmap.set(dbo.NUMIONODES);
    dbo.setColumns(colBitmap);

    // Validate the I/O block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", IOBlockName.c_str());

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
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // Validate that block is an I/O block
    if (dbo._numionodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // Done checking I/O block so close
    tx.close(&dbo);

    BGQDB::BLOCK_STATUS state;

    // Serialize I/O block deallocate requests
    pthread_mutex_lock(&IOBlockaction_lock);
    BGQDB::STATUS result = BGQDB::setBlockAction(IOBlockName, BGQDB::DEALLOCATE_BLOCK);
    pthread_mutex_unlock(&IOBlockaction_lock);
    switch (result) {
    case BGQDB::OK:
        LOG_DEBUG_MSG("Initiating free request for I/O block " << IOBlockName);
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
    case BGQDB::INVALID_ID: // I/O block name not correct
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                "I/O block " << IOBlockName << " is not valid"
        );
    case BGQDB::NOT_FOUND: // I/O block not found
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "I/O block " << IOBlockName << " was not found"
        );
    case BGQDB::DUPLICATE:
        if (BGQDB::OK == BGQDB::getBlockStatus(IOBlockName, state)) {
            if (state == BGQDB::FREE) {
                LOG_WARN_MSG("I/O block free request ignored, " << IOBlockName << " is already Free");
                return;
            }
        }
        THROW_EXCEPTION( // I/O block already has action pending
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockFreeError,
                "I/O block free request failed because I/O block " << IOBlockName << " has pending action"
        );
    case BGQDB::FAILED:
        if (BGQDB::OK == BGQDB::getBlockStatus(IOBlockName, state)) {
            if (state == BGQDB::FREE) {
                LOG_WARN_MSG("I/O block free request ignored, " << IOBlockName << " is already Free");
                return;
            }
        }
        THROW_EXCEPTION( // I/O block Free request is invalid or wrong status
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::BlockFreeError,
                "I/O block free request failed because I/O block " << IOBlockName << " received invalid block action request"
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
IOBlock::remove(
        const string& IOBlockName
)
{
    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    SQLRETURN sqlrc;

    // Verify I/O block name was specified
    if (IOBlockName.empty()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidBlockName,
                No_Block_Name_Str
                );
    }
    // Validate the I/O block name length
    if (IOBlockName.length() >= sizeof(dbo._blockid)) {
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
    colBitmap.set(dbo.NUMIONODES);
    dbo.setColumns(colBitmap);

    // Validate the I/O block exists in the database
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", IOBlockName.c_str());

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
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // Validate that block is an I/O block
    if (dbo._numionodes == 0) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "I/O block " << IOBlockName << " was not found"
        );
    }

    // Done checking I/O block so close
    tx.close(&dbo);

    LOG_DEBUG_MSG("Attempting to remove I/O block " << IOBlockName);
    BGQDB::STATUS result = BGQDB::deleteBlock(IOBlockName);
    switch (result) {
        case BGQDB::OK:
            LOG_DEBUG_MSG("I/O block " << IOBlockName << " was removed");
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
                    );
        case BGQDB::FAILED:  // I/O block can only be deleted if (F)ree
            THROW_EXCEPTION(
                    bgsched::RuntimeException,
                    bgsched::RuntimeErrors::InvalidBlockState,
                    "I/O block " << IOBlockName << " must be in Free state to be removed."
                    );
        case BGQDB::INVALID_ID:
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidBlockName,
                    Block_Name_Too_Long_Str
                    );
        case BGQDB::NOT_FOUND: // I/O block not found
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::BlockNotFound,
                    "I/O block " << IOBlockName << " was not found"
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
IOBlock::update()
{
    LOG_DEBUG_MSG("Attempting to update I/O block " << getName());

    // Modify I/O block boot options, micro-loader image, and Node configuration.
    // Requires I/O block to be in Free, Terminating or Allocated state.
    BGQDB::BlockInfo info;

    string options;
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
        LOG_DEBUG_MSG("I/O block " << getName() << " boot options, micro-loader image and Node configuration updated");
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
                "I/O block " << getName() << " was either not found or has a status that does not allow updates."
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

    // Modify the description (doesn't require I/O block to be Free)
    result = BGQDB::setBlockDesc(getName(), getDescription());
    switch (result) {
    case BGQDB::OK:
        LOG_DEBUG_MSG("I/O block " << getName() << " description updated");
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
    case BGQDB::NOT_FOUND: // I/O block not found
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotFound,
                "I/O block " << getName() << " was not found"
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

const string&
IOBlock::getName() const
{
    return _impl->getName();
}

BlockCreationId
IOBlock::getCreationId() const
{
    return _impl->getCreationId();
}

const string&
IOBlock::getDescription() const
{
    return _impl->getDescription();
}

const string&
IOBlock::getOwner() const
{
    return _impl->getOwner();
}

const string&
IOBlock::getUser() const
{
    return _impl->getUser();
}

const string&
IOBlock::getBootOptions() const
{
    return _impl->getBootOptions();
}

const string&
IOBlock::getMicroLoaderImage() const
{
    return _impl->getMicroLoaderImage();
}

const string&
IOBlock::getNodeConfiguration() const
{
    return _impl->getNodeConfiguration();
}

SequenceId
IOBlock::getSequenceId() const
{
    return _impl->getSequenceId();
}

EnumWrapper<IOBlock::Status>
IOBlock::getStatus() const
{
    return _impl->getStatus();
}

uint32_t
IOBlock::getIONodeCount() const
{
    return _impl->getIONodeCount();
}

const IOBlock::IOLocations
IOBlock::getIOLocations() const
{
    IOBlock::IOLocations ret;

    const IOLocations &locations(_impl->getIOLocations());

    for (IOLocations::const_iterator i(locations.begin()); i != locations.end(); ++i) {
        ret.push_back(*i);
    }
    return ret;
}

void
IOBlock::setName(
        const string& name
)
{
    return _impl->setName(name);
}

void
IOBlock::setDescription(
        const string& description
)
{
    return _impl->setDescription(description);
}

void
IOBlock::setBootOptions(
        const string& bootOptions
)
{
    return _impl->setBootOptions(bootOptions);
}

void
IOBlock::setMicroLoaderImage(
        const string& image
)
{
    return _impl->setMicroLoaderImage(image);
}

void
IOBlock::setNodeConfiguration(
        const string& nodeConfig
)
{
    return _impl->setNodeConfiguration(nodeConfig);
}

string
IOBlock::toString(
        bool verbose
) const
{
    ostringstream os;
    _impl->toString(os, verbose);
    return os.str();
}

EnumWrapper<IOBlock::Action::Value>
IOBlock::getAction() const
{
    return _impl->getAction();
}


static void rebootIONodeThread(
        const std::string& IOBlockName,
        const std::string& location,
        IOBlock::RebootIONodeCallbackFn cbFn
    )
{
    try {

        static const string default_service_name( "32031" );

        bgq::utility::ClientPortConfiguration client_port_config(
                default_service_name,
                bgq::utility::ClientPortConfiguration::ConnectionType::Administrative
            );

        static const string section_name( "bg_console" );

        client_port_config.setProperties( getProperties(), section_name );

        client_port_config.notifyComplete();

        mmcs_client::ConsolePortClient client(
                client_port_config
            );

        {
            string select_block_command_str( "select_block " + IOBlockName );

            mmcs_client::CommandReply select_block_reply;

            mmcs_client::sendCommandReceiveReply(
                    client,
                    select_block_command_str,
                    select_block_reply
                );

            if ( select_block_reply.getStatus() == mmcs_client::CommandReply::STATUS_ABORT ) {

                THROW_EXCEPTION(
                        RuntimeException,
                        RuntimeErrors::MmcsConnectionError,
                        "Connection to MMCS server failed"
                    );

            }

            if ( select_block_reply.getStatus() != mmcs_client::CommandReply::STATUS_OK ) {

                string response_str(select_block_reply.str());

                if ( response_str.find( "does not exist" ) != string::npos ) {
                    THROW_EXCEPTION(
                            InputException,
                            InputErrors::BlockNotFound,
                            "Cannot reboot I/O nodes in I/O block " << IOBlockName << " because the block was not found"
                        );
                }

                if ( response_str == "block is not allocated" ) {
                    THROW_EXCEPTION(
                            RuntimeException,
                            RuntimeErrors::InvalidBlockState,
                            "Cannot reboot I/O nodes in I/O block " << IOBlockName << " because the block is not allocated"
                        );
                }

                THROW_EXCEPTION(
                        InternalException,
                        InternalErrors::UnexpectedError,
                        "Failed to reboot I/O nodes because failed to select the block. The response from MMCS is '" << response_str << "'"
                    );

            }
        }

        {
            string reboot_nodes_command_str( "{" + location + "} reboot_nodes" );

            mmcs_client::CommandReply reboot_nodes_reply;

            mmcs_client::sendCommandReceiveReply(
                    client,
                    reboot_nodes_command_str,
                    reboot_nodes_reply
                );

            if ( reboot_nodes_reply.getStatus() == mmcs_client::CommandReply::STATUS_ABORT ) {

                THROW_EXCEPTION(
                        RuntimeException,
                        RuntimeErrors::MmcsConnectionError,
                        "Connection to MMCS server failed"
                    );

            }

            if ( reboot_nodes_reply.getStatus() != mmcs_client::CommandReply::STATUS_OK ) {

                string response_str(reboot_nodes_reply.str());

                if ( response_str == "list of targets for reboot_nodes is empty." ) {

                    THROW_EXCEPTION(
                            InputException,
                            InputErrors::InvalidLocationString,
                            "Cannot reboot " << location << " in " << IOBlockName << " because the I/O node is not in the block or the I/O node location is not valid"
                        );

                }

                THROW_EXCEPTION(
                        InternalException,
                        InternalErrors::UnexpectedError,
                        "Failed to reboot I/O nodes because the reboot_nodes command failed. The response from MMCS is '" << response_str << "'"
                    );
            }
        }

        if ( cbFn ) {
            cbFn( boost::exception_ptr() );
        }

    } catch ( mmcs_client::ConsolePort::Error& e ) {

        LOG_DEBUG_MSG( "During rebootIONode, failed to connect to MMCS server. Error is " << e.what() );

        if ( cbFn ) {
            try {

                THROW_EXCEPTION(
                        RuntimeException,
                        RuntimeErrors::MmcsConnectionError,
                        "Connection to MMCS server failed"
                    );

            } catch ( std::exception& e ) {
                cbFn( boost::current_exception() );
            }
        }

    } catch ( InputException& e ) {

        LOG_DEBUG_MSG( "Exception occurred while rebooting I/O nodes. exception=" << e.what() );

        if ( cbFn ) {
            cbFn( boost::current_exception() );
        }

    } catch ( InternalException& e ) {

        LOG_DEBUG_MSG( "Exception occurred while rebooting I/O nodes. exception=" << e.what() );

        if ( cbFn ) {
            cbFn( boost::current_exception() );
        }

    } catch ( RuntimeException& e ) {

        LOG_DEBUG_MSG( "Exception occurred while rebooting I/O nodes. exception=" << e.what() );

        if ( cbFn ) {
            cbFn( boost::current_exception() );
        }

    } catch ( std::exception& e ) {

        LOG_DEBUG_MSG( "Unexpected exception occurred while rebooting I/O nodes. exception=" << e.what() );

        if ( cbFn ) {
            try {

                THROW_EXCEPTION(
                        InternalException,
                        InternalErrors::UnexpectedError,
                        "Failed to reboot I/O nodes because an unexpected error occurred, the error message is '" << e.what() << "'"
                    );

            } catch ( std::exception& e  ) {
                cbFn( boost::current_exception() );
            }
        }

    }
}

void IOBlock::rebootIONode(
        const std::string& IOBlockName,
        const std::string& location,
        RebootIONodeCallbackFn cbFn
    )
{

    try {

        boost::thread( &rebootIONodeThread, IOBlockName, location, cbFn );

    } catch ( std::exception& e ) {

        LOG_DEBUG_MSG( "Unexpected error occurred while starting reboot I/O node thread. exception=" << e.what() );

        THROW_EXCEPTION(
                InternalException,
                InternalErrors::UnexpectedError,
                "Failed to reboot I/O nodes because an unexpected error occurred, the error message is '" << e.what() << "'"
            );

    }
}


} // namespace bgsched
